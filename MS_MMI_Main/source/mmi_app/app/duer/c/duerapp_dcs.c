// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_dcs.c
 * Auth: Li Chang (changli@baidu.com)
 * Desc: DCS handler.
 */

#include "duerapp_dcs.h"
#include "duerapp_recorder.h"
#include "duerapp_audio_play_utils.h"
#include "duerapp_system.h"

#include "os_api.h"
#include "gui_ucs2b_converter.h"

#include "lightduer_audio_player.h"
#include "lightduer_audio_codec_ops.h"
#include "lightduer_lib.h"
#include "lightduer_sleep.h"
#include "lightduer_timestamp.h"
#include "lightduer_mutex.h"
#include "lightduer_queue_cache.h"
#include "lightduer_dcs.h"
#include "lightduer_dcs_router.h"
#include "lightduer_dcs_local.h"
#include "lightduer_profile.h"
#include "baidu_json.h"

#ifdef DUER_CES_ENABLE
#include "lightduer_ces.h"
#endif

#include "duerapp_main.h"
#include "duerapp_login.h"
#include "duerapp_nv.h"
#include "duerapp_playing_timer.h"
#include "duerapp_homepage.h"
#include "duerapp_common.h"
#include "duerapp_assists_activity.h"
#include "duerapp_res_guard.h"
#define USE_DUERINPUT 0

#if (USE_DUERINPUT == 1)
#include "duerinput_common.h"
#endif

static duer_mutex_t s_mutex = NULL;
static int is_playing_speech = 0;

typedef struct {
    char *buf;
    duer_size_t size;
} duer_tts_data_t;

static duer_qcache_handler s_tts_queue;
static bool s_tts_write_directly = false;
static duer_u32_t s_tts_last_play_time = 0;

static void duer_free_tts_data(duer_tts_data_t *data)
{
    if (data) {
        if (data->buf) {
            DUER_FREE(data->buf);
            data->buf = NULL;
        }
        DUER_FREE(data);
    }
}

static duer_tts_data_t *duer_create_tts_data(const void *data, duer_size_t size)
{
    duer_tts_data_t *tts_data = NULL;

    do {
        tts_data = DUER_MALLOC(sizeof(duer_tts_data_t));
        if (!tts_data) {
            DUER_LOGE("memory overflow");
            break;
        }
        DUER_MEMSET(tts_data, 0, sizeof(duer_tts_data_t));

        if (size > 0 && data) {
            tts_data->buf = DUER_MALLOC(size);
            if (!tts_data->buf) {
                DUER_LOGE("memory overflow");
                break;
            }
            DUER_MEMCPY(tts_data->buf, data, size);
        }

        tts_data->size = size;
        return tts_data;
    } while (0);

    duer_free_tts_data(tts_data);

    return NULL;
}

static void duer_clear_tts_queue()
{
    duer_tts_data_t *data = NULL;

    if (s_tts_queue) {
        while ((data = duer_qcache_pop(s_tts_queue)) != NULL) {
            duer_free_tts_data(data);
        }
    }
}
static bool duer_check_tts_play_time()
{
#ifdef DUER_CES_ENABLE
    duer_u32_t curr_time = duer_timestamp();
    const int TTS_TIMEOUT_MS = 10 * 1000;
    duer_u32_t elapsed_time_ms = 0;
    char err_msg[32];
    
    if (s_tts_last_play_time != 0 && curr_time > s_tts_last_play_time + TTS_TIMEOUT_MS) {
        elapsed_time_ms = curr_time - s_tts_last_play_time;
        DUER_SNPRINTF(err_msg, sizeof(err_msg), "tts timeout %u", elapsed_time_ms);

        duer_ces_event_tts_play_failed(CES_TTS_ERR_TYPE_NO_DATA, NULL, s_speech_cid, err_msg);
        s_tts_last_play_time = 0;
        return false;
    }
    return true;
#else
    return true;
#endif
}

int duer_auth_hook(int status)
{
    if (status) {
        DUER_LOGI("auth success!");
        MMIDUERAPP_SyncAccessToken();
        duerapp_device_report();
        duer_engine_enable();
        duer_dcs_init();
        duerapp_get_user_mapinfo();
        // for Test
        duerapp_get_sensitive_words_config();
        DUER_LOGI("Duer dcs inited!");
    } else {
        DUER_LOGI("auth failed!");
    }
    // baidu add
    MMIDUERAPP_InitState(status);
    return 0;
}

static int asr_result_parse(char *res, char **words)
{
    int ret = DUER_ERR_FAILED;
    baidu_json *root = NULL;
    baidu_json *err = NULL;
    char *reaason_str = NULL;
    baidu_json *result = NULL;
    baidu_json *word = NULL;
    baidu_json *item = NULL;

    if (!res) {
        DUER_LOGE("Invalid param");
        return DUER_ERR_FAILED;
    }

    do {
        root = baidu_json_Parse((const char *)res);
        if (!root) {
            DUER_LOGE("\"root\" is NULL");
            break;
        }

        err = baidu_json_GetObjectItem(root, "err_msg");
        if (err) {
            /*
                sdk speech is too long[info:-3]  ??????
                asr server not find effective speech[info:-4]  ???????
            */
            if(strstr(err->valuestring, "[info:-3]")) {
                ret = DUER_ERR_ASR_TIMEOUT;
                reaason_str = "TIMEOUT";
            } else if (strstr(err->valuestring, "[info:-4]")) {
                ret = DUER_ERR_ASR_NO_TEXT;
                reaason_str = "NOTEXT";
            } else {
                ret = DUER_ERR_ASR_UNKOWN;
                reaason_str = "UNKOWN";
            }
            DUER_LOGE("(%s)[duer_watch]:error msg(%s), reason(%S)", __func__, err->valuestring, reaason_str);
            break; 
        }

        result = baidu_json_GetObjectItem(root, "result");
        if (!result) {
            DUER_LOGE("\"result\" is NULL");
            break;
        }

        word = baidu_json_GetObjectItem(result, "word");
        if (!word) {
            DUER_LOGE("\"word\" is NULL");
            break;
        }

        item = baidu_json_GetArrayItem(word, 0);
        if (!item) {
            DUER_LOGE("\"item\" is NULL");
            break;
        }

        *words = DUER_MALLOC(strlen(item->valuestring) + 1);
        if (*words) {
            strcpy(*words, item->valuestring);
            DUER_LOGI("words:%s", *words);
        } else {
            DUER_LOGE("malloc failed");
            break;
        }

        ret = DUER_OK;
    } while (0);

    if (root) {
        baidu_json_Delete(root);
    }

    return ret;
}

void duer_event_hook(duer_event_t *event)
{
#if (USE_DUERINPUT == 1)
    static char s_asr_input[2048] = {0};
#endif
    char *p = NULL;
    int ret = -1;
#if 0
    // static char s_asr_input[2048];
    // static int s_asr_input_p = 0;
    // char *p = NULL;
#endif

    if (event == NULL) {
        DUER_LOGE("NULL event!!!");
        return;
    }
    DUER_LOGI("event: %d", event->_event);

    switch (event->_event) {
    case DUER_EVENT_STARTED:
        break;
    case DUER_EVENT_STOPPED:
        break;
    case DUER_EVENT_ASR_CANCELLED:
        DUER_LOGI("ASR Cancelled, code:%d", event->_code);
        duer_dcs_on_listen_stopped();
        break;
    case DUER_EVENT_ASR_PROCESSING:
        DUER_LOGI("ASR processing, code:%d,%s", event->_code, (char *)(event->_object));

    #if (USE_DUERINPUT == 1)
        if (DuerInput_IsWorkingNow())
        {
            static char s_asr_input[2048] = {0};
            char *p = NULL;
            asr_result_parse((char *)(event->_object), &p);
            DUER_LOGI("DUER_EVENT_ASR_PROCESSING:%s", p);
            sprintf(s_asr_input, "%s", (const char *)p);
            DUER_FREE(p);

            MMIDUERINPUT_ScreenInputMidResultEvent(s_asr_input);
        }
    #endif /* (USE_DUERINPUT == 1) */
#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)
        //parent assists
        if (PUB_DUERAPP_MessageSelectWinIsOpen())
        {
            ret = asr_result_parse((char *)(event->_object), &p);
            sprintf(s_asr_input, "%s", (const char *)p);
            DUER_FREE(p);
            PUB_DUERAPP_ScreenInputMidResultEvent(s_asr_input, ret);
        }
#endif
        break;
    case DUER_EVENT_ASR_FINISHED:
        DUER_LOGI("ASR Finished, code:%d", event->_code);
        duer_dcs_on_listen_stopped();

    #if (USE_DUERINPUT == 1)
        if (DuerInput_IsWorkingNow())
        {
            // static char s_asr_input[2048] = {0};
            // char *p = NULL;
            ret = asr_result_parse((char *)(event->_object), &p);
            
            DUER_LOGI("(%s)[duer_watch]:finish, input_txt(%s)", __func__, p);
            sprintf(s_asr_input, "%s", (const char *)p);
            DUER_FREE(p);
            MMIDUERINPUT_ScreenInputFinalResultEvent(s_asr_input, ret);
        }
    #endif /* (USE_DUERINPUT == 1) */

#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)
        //parent assists
        if (PUB_DUERAPP_MessageSelectWinIsOpen()) {
            // static char s_asr_input[2048] = {0};
            // char *p = NULL;
            // int ret = -1;
            
            ret = asr_result_parse((char *)(event->_object), &p);
            sprintf(s_asr_input, "%s", (const char *)p);
            DUER_FREE(p);
            PUB_DUERAPP_ScreenInputFinalResultEvent(s_asr_input, ret);
        }
#endif
        break;
    default:
        break;
    }
    //baidu add
    MMIDUERAPP_DuerAsrState(event);
}

void duer_dcs_init(void)
{
    DUER_LOGI("duer_dcs_init\n");
    duer_dcs_framework_init();
    duer_dcs_voice_input_init();
    duer_dcs_voice_output_init();
    duer_dcs_speaker_control_init();
    duer_dcs_audio_player_init();
    duer_dcs_screen_init();
    duer_dcs_device_control_init();
    //duer_sys_info_init();
    duer_dcs_push_service_init();
    duer_dcs_power_init();
    duer_dcs_screen_ext_card_init();
    duer_dcs_location_init();
    duer_dcs_screen_ext_card_watch_pay_init();
    duer_dcs_authentication_init();
    duer_dcs_thirdparty_init();

    if (s_mutex == NULL) {
        s_mutex = duer_mutex_create();

        if (s_mutex == NULL) {
            DUER_LOGE("Failed to create mutex");
        } else {
            DUER_LOGD("Create mutex success");
        }
    }
    duer_dcs_sync_state();
}

void duer_dcs_listen_handler(void)
{
    DUER_LOGI("enable_listen_handler, open mic\n");
    duer_record_start(true);
}

void duer_dcs_stop_listen_handler(void)
{
    DUER_LOGI("stop_listen, close mic\n");
    duer_asr_stop();
}

void duer_dcs_volume_set_handler(int volume, duer_dcs_volume_target_t target)
{
    DUER_LOGI("set_volume to %d\n", volume);
    duer_audio_dac_set_volume(volume);
}

void duer_dcs_volume_adjust_handler(int volume, duer_dcs_volume_target_t target)
{
    DUER_LOGI("adj_volume by %d\n", volume);
    duer_audio_dac_adj_volume(volume);
}

void duer_dcs_mute_handler(duer_bool is_mute, duer_dcs_volume_target_t target)
{
    DUER_LOGI("set_mute to  %d\n", (int)is_mute);
    duer_audio_dac_set_mute((int)is_mute);
}

duer_status_t duer_dcs_input_text_handler(const char *text, const char *type)
{
    DUER_LOGI("(%s)[duer_watch]:input text", __FUNCTION__);
    MMIDUERAPP_ScreenInputEvent((char *)text);
    return DUER_OK;
}

static void duer_render_card_passed_callback(void *ctx, RES_CATEGORY_E category, baidu_json *payload_json) {
    DUER_LOGI("PASSED");
    MMIDUERAPP_ScreenRenderCardEvent(payload_json); 
}

static void duer_render_card_filtered_callback(void *ctx, RES_CATEGORY_E category) {
    const char *text = L"小度还不太理解呢";
    DUER_LOGI("FILTERED");
    duer_dcs_user_interact(text);
}

duer_status_t duer_dcs_render_card_handler(baidu_json *payload)
{
    DuerPermissionLog("access network..");
    duerapp_res_is_filter_async(payload, INFORMATION, duer_render_card_passed_callback, duer_render_card_filtered_callback, NULL);
    return DUER_OK;
}

static void duer_render_player_info_passed_callback(void *ctx, RES_CATEGORY_E category, baidu_json *payload_json) {
    DUER_LOGI("PASSED payload_json = %x", payload_json);
    MMIDUERAPP_ScreenRenderPlayInfoEvent(payload_json);
    DuerPermissionLog("access network..");
}

static void duer_render_player_info_filtered_callback(void *ctx, RES_CATEGORY_E category) {
    const char *text = L"暂无该播放资源";
    DUER_LOGI("FILTERED");
    duer_dcs_user_interact(text);
}

duer_status_t duer_dcs_render_player_info_hdl(baidu_json *payload)
{
    char *msg = baidu_json_PrintBuffered(payload, 1024, 0);
    /*int len = strlen(msg);
    int i = 0;
    int cnt = 0;
    char str[100 + 1] = {0};
    if (len <= 100) {
        SCI_TRACE_LOW("DUER: %s", msg);
    } else {
        cnt = len / 100 + 1;
        while (cnt--) {
            snprintf(str, 100 + 1, "%s", msg + i * 100);
            SCI_TRACE_LOW("DUER: %s\n", str);
            i++;
        }   
    }*/
    DUER_LOGI("render playerinfo result: %s", msg);
    baidu_json_release(msg);
    duerapp_res_is_filter_async(payload, UNICAST, duer_render_player_info_passed_callback, duer_render_player_info_filtered_callback, NULL);
    return DUER_OK;
}

duer_status_t duer_dcs_render_audio_list_hdl(baidu_json *payload)
{
    char *msg = baidu_json_PrintBuffered(payload, 1024, 0);
    if (msg) {
        // SCI_TRACE_HIGH("[DUER]%s.\n", msg);
        DUER_LOGE("[DUER]%s.\n", msg);
        baidu_json_release(msg);
    }
    return DUER_OK;
}

duer_status_t duer_dcs_render_weather_hdl(baidu_json *payload)
{
    DuerPermissionLog("access network..");
    #if 0 //????????????UI???????????
    MMIDUERAPP_ScreenRenderWeatherEvent(payload);
    #endif
    // char *msg = baidu_json_PrintBuffered(payload, 1024, 0);
    // if (msg) {
    //     SCI_TRACE_HIGH("[DUER]%s.\n", msg);
    //     int utf8_len = strlen(msg);
    //     wchar *wch_txt = NULL;
    //     wch_txt = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
    //     GUI_UTF8ToWstr(wch_txt, utf8_len, msg, utf8_len);
    //     MMIDUERAPP_ScreenRenderCardEvent(wch_txt);
    //     DUER_FREE(wch_txt);
    //     baidu_json_release(msg);
    // }
    return DUER_OK;
}

baidu_json *duer_dcs_get_speaker_state(void)
{
    int ret = 0;
    int vol = 0;
    int mute = 0;
    baidu_json *state = NULL;

    DUER_LOGI("duer_dcs_get_speaker_state\n");

    do {
        state = baidu_json_CreateObject();
        if (state == NULL) {
            DUER_LOGE("create state failed");
            break;
        }

        duer_audio_dac_get_volume(&vol);
        baidu_json_AddNumberToObjectCS(state, "volume", vol);
        // alarmVolume is not reported in SP16.
        // TODO(chenchi): this dcs event format changed in SP17.
        // vol = duer_get_volume(VOLUME_TYPE_ALERT);
        // baidu_json_AddNumberToObjectCS(state, "alarmVolume", vol);

        duer_audio_dac_get_mute(&mute);
        baidu_json_AddBoolToObject(state, "muted", mute);
    } while (0);

    return state;
}

void duer_dcs_tts_flow_handler(const char *buf, size_t len)
{
    duer_u32_t start_time = duer_timestamp();
    int timespent = 0;

    duer_mutex_lock(s_mutex);
    s_tts_last_play_time = (buf == NULL ? 0 : start_time);

    if (s_tts_write_directly) {
        duer_audio_player_stream_write_data((char *)buf, len);
        if (buf == NULL || len == 0) {
            s_tts_write_directly = false;
        }
    } else {
        duer_tts_data_t *tts_data = duer_create_tts_data(buf, len);
        if (tts_data) {
            duer_qcache_push(s_tts_queue, tts_data);
        }
    }
    duer_mutex_unlock(s_mutex);
    timespent = duer_timestamp() - start_time;
    if (timespent > 100) {
        DUER_LOGW("write a2dp data timespent:%d", timespent);
    }
}

void duer_dcs_speak_handler(const char *url)
{
    DUER_LOGI("Playing speak: %s\n", url);
    if (DUER_STRNCMP(url, "http", 4) == 0) {
        duer_audio_player_play(url, AUDIO_TYPE_DCS_SPEECH, 0);
    } else {
        duer_mutex_lock(s_mutex);
        s_tts_write_directly = false;
        if (s_tts_queue == NULL) {
            s_tts_queue = duer_qcache_create();
            if (s_tts_queue == NULL) {
                DUER_LOGE("Failed to create tts queue");
            }
        } else {
            duer_clear_tts_queue();
        }
        duer_mutex_unlock(s_mutex);

#ifdef DUER_CES_ENABLE
        if (url) {
            sscanf(url, "cid:%49s", s_speech_cid);
            DUER_LOGI("cid:%s", s_speech_cid);
        }
#endif
        duer_audio_player_play("stream://tts.mp3", AUDIO_TYPE_DCS_SPEECH, 0);
    }
    duer_mutex_lock(s_mutex);
    is_playing_speech = 1;
    duer_mutex_unlock(s_mutex);
}

void duer_dcs_audio_download_handler(const duer_dcs_audio_info_t *audio_info)
{
    if (audio_info) {
        DUER_LOGI("pre download url: %s", audio_info->url);
        duer_audio_player_pre_download(audio_info->url, audio_info->offset);
    }
    duer_mutex_lock(s_mutex);
    is_playing_speech = 1;
    duer_mutex_unlock(s_mutex);
}

int duer_dcs_audio_play_handler(const duer_dcs_audio_info_t *audio_info)
{
    int ret = 0;

    if (audio_info) {
#ifdef DUER_CES_ENABLE
        duer_ces_dpvs_clock_directive_time();
        duer_ces_dpvs_set_resource_url(audio_info->url);
        duer_ces_dpvs_clock_player_start_time();

        duer_ces_dcvs_clock_duer_result_throw_time();
        duer_ces_dcvs_complete();
#endif
        /* Do not play when xiaodu APP is exit. */
        if (MMIDUERAPP_GetInitStatus()) {
            DUER_LOGI("Playing audio: %s\n", audio_info->url);
            duer_audio_player_play(audio_info->url, AUDIO_TYPE_DCS_AUDIO, audio_info->offset);

            //  baidu add
            MMIDUERAPP_MediaPlayStateEvent(MEDIA_PLAY_STATE_PLAYING);
        } else {
            DUER_LOGI("Outof APP.");
            duer_audio_player_stop();
            ret = -1;
        }
    }
    duer_audio_on_dcs_paused(DUER_FALSE);
    return ret;
}

void duer_dcs_audio_stop_handler()
{
    int status = is_playing_speech;
    DUER_LOGI("Stop audio play");

    //if audio player is playing speech, no need to stop audio player
    duer_mutex_lock(s_mutex);
    duer_mutex_unlock(s_mutex);

    if (status == 1) {
        DUER_LOGI("Is playing speech, no need to stop");
    } else {
        duer_audio_player_stop();
    }
    duer_audio_on_dcs_paused(DUER_FALSE);
}

void duer_dcs_audio_pause_handler()
{
    uint32_t pos = 0;
#if 0

    if (xTimerStop(s_recog_led_tmr, 120) != pdTRUE) {
        DUER_LOGW("xTimerStop failed!!");
    }

#endif
    DUER_LOGI("DCS pause audio play");

    duer_audio_player_dcs_pause(&pos);
    duer_audio_on_dcs_paused(DUER_TRUE);
}

void duer_dcs_audio_resume_handler(const duer_dcs_audio_info_t *audio_info)
{
    DUER_LOGI("Resume audio play");

    if (audio_info) {
        duer_audio_player_play(audio_info->url, AUDIO_TYPE_DCS_AUDIO, audio_info->offset);

        //  baidu add
        MMIDUERAPP_MediaPlayStateEvent(MEDIA_PLAY_STATE_PLAYING);
    } else {
        duer_audio_player_dcs_resume();
    }
    duer_audio_on_dcs_paused(DUER_FALSE);
}

int duer_dcs_audio_get_play_progress()
{
    int ret = 0;
    uint32_t play_time = 0;

    ret = duer_audio_player_get_play_progress(&play_time, true);

    if (ret == 0) {
        DUER_LOGD("Play time: %d", play_time);
        return play_time;
    } else {
        DUER_LOGE("Failed to get play progress.");
        return -1;
    }
}

static char *duer_get_stuttered_msg(const char *ip_infos)
{
    /*const int MSG_OTHER_LEN = 60;
    char *msg = NULL;
    int len = 0;

    if (ip_infos == NULL) {
        return NULL;
    }

    len = strlen(ip_infos) + MSG_OTHER_LEN;
    msg = (char *)DUER_MALLOC(len);
    if (msg == NULL) {
        DUER_LOGE("memory overflow");
        return NULL;
    }

    if (duer_wifi_is_connected()) {
        wifi_ap_record_t ap_info;
        char mac_str[18] = {0};
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            DUER_SNPRINTF(mac_str, sizeof(mac_str), MACSTR, MAC2STR(ap_info.bssid));
        }
        DUER_SNPRINTF(msg, len, "rssi:%d|bssid:%s|ip:%s", duer_wifi_rssi_get(), mac_str, ip_infos);
    } else {
        DUER_SNPRINTF(msg, len, "discon|ip:%s", ip_infos);
    }

    DUER_LOGD("msg:%s", msg);
    return msg;*/
    return NULL;
}

static void duer_dcs_on_stuttered(duer_audio_play_type_t type, bool is_stuttered,
        const char *ip_infos, const char *host, const char *ip)
{
    char *msg = NULL;
    DUER_LOGI("duer_dcs_on_stuttered, %d", is_stuttered);
    if (type == AUDIO_TYPE_DCS_AUDIO) {
        msg = duer_get_stuttered_msg(ip_infos);
        duer_dcs_audio_on_stuttered(is_stuttered, msg, host, ip);
        if (msg) {
            DUER_FREE(msg);
        }
    } else {
        // do nothing
    }
}


int duer_dcs_audio_on_started_cb(duer_audio_play_type_t type)
{
    duer_tts_data_t *data = NULL;
    if (type == AUDIO_TYPE_DCS_AUDIO) {
        MMIDUERAPP_MediaPlayStateEvent(MEDIA_PLAY_STATE_PLAYING);

        duer_try_playing_timer_start();
        //duer_audio_register_on_stuttered_cb(duer_dcs_on_stuttered);
    } else if (type == AUDIO_TYPE_DCS_SPEECH) {
        duer_mutex_lock(s_mutex);
        while ((data = duer_qcache_pop(s_tts_queue)) != NULL) {
            duer_audio_player_stream_write_data(data->buf, data->size);
            if (data->buf == NULL || data->size == 0) {
                duer_free_tts_data(data);
                break;
            }
            duer_free_tts_data(data);
        }

        if (duer_qcache_length(s_tts_queue) == 0) {
            s_tts_write_directly = true;
        }
        is_playing_speech = 1;
        duer_mutex_unlock(s_mutex);
    } else {
        // do nothing
    }

    duer_audio_on_play_started(type);

    return 0;
}

int duer_dcs_audio_on_stopped_cb(duer_audio_play_type_t type)
{
    if (type == AUDIO_TYPE_DCS_SPEECH) {
        duer_dcs_speech_on_stopped();
        duer_mutex_lock(s_mutex);
        duer_check_tts_play_time();
        is_playing_speech = 0;
        duer_mutex_unlock(s_mutex);
    } else if (type == AUDIO_TYPE_DCS_AUDIO) {
        duer_try_playing_timer_stop();
        MMIDUERAPP_MediaPlayStateEvent(MEDIA_PLAY_STATE_STOP);   
    }

    duer_audio_on_play_stopped(type);
    return 0;
}

void duer_dcs_stop_speak_handler()
{
    int ret = 0;
    duer_mutex_lock(s_mutex);

    if (is_playing_speech) {
        DUER_LOGI("Stop speak");
        duer_check_tts_play_time();
        ret = duer_audio_player_stop();

        if (ret != AUDIO_PLAYER_RETCODE_SUCCESS) {
            DUER_LOGE("Failed to stop speak, errcode: %d", ret);
        }

        is_playing_speech = 0;

        duer_clear_tts_queue();
    }

    duer_mutex_unlock(s_mutex);
}

int duer_dcs_audio_on_finished_cb(duer_audio_play_type_t type)
{
    duer_audio_player_retcode_t ret = AUDIO_PLAYER_RETCODE_SUCCESS;

    if (type == AUDIO_TYPE_DCS_SPEECH) {
        DUER_LOGI("Send speech on finish event");
        duer_mutex_lock(s_mutex);
        if (duer_qcache_length(s_tts_queue) > 0) {
            duer_audio_player_play("stream://tts.mp3", AUDIO_TYPE_DCS_SPEECH, 0);
        } else {
            is_playing_speech = 0;
        }
        duer_mutex_unlock(s_mutex);

        if (is_playing_speech == 0) {
            duer_dcs_speech_on_finished();
        }
    } else if (type == AUDIO_TYPE_DCS_AUDIO) {
        duer_try_playing_timer_stop();
        MMIDUERAPP_MediaPlayStateEvent(MEDIA_PLAY_STATE_FINISH);
        DUER_LOGI("Send audio on finish event");
        duer_dcs_audio_on_finished();
    }

    duer_audio_on_play_finished(type);
    
    return 0;
}

int duer_dcs_audio_on_error_cb(duer_audio_play_type_t type)
{
    if (type == AUDIO_TYPE_DCS_SPEECH) {
        duer_mutex_lock(s_mutex);
        is_playing_speech = 0;
        duer_mutex_unlock(s_mutex);

#ifdef DUER_CES_ENABLE
        duer_ces_event_tts_play_failed(CES_TTS_ERR_TYPE_PLAYER, NULL, s_speech_cid, NULL);
#endif
    } else if (AUDIO_TYPE_DCS_OTHER == type) {
    } else if (type == AUDIO_TYPE_DCS_AUDIO) {
        duer_try_playing_timer_stop();
        MMIDUERAPP_MediaPlayStateEvent(MEDIA_PLAY_STATE_ERROR);
    }

    duer_audio_on_play_error(type);
    return 0;
}

void duer_ext_stop_speak()
{
    int ret = 0;
    duer_mutex_lock(s_mutex);

    if (is_playing_speech) {
        DUER_LOGI("Stop speak");
        duer_check_tts_play_time();
        ret = duer_audio_player_stop();
        if (ret != AUDIO_PLAYER_RETCODE_SUCCESS) {
            DUER_LOGE("Failed to stop speak, errcode: %d", ret);
        }

        is_playing_speech = 0;
    }

    duer_mutex_unlock(s_mutex);
}

baidu_json *duer_dcs_get_gps_state()
{
    baidu_json *state = NULL;
    gps_info_t info;
    
    DUER_LOGI("duer_dcs_get_gps_state\n");

    do {
        state = baidu_json_CreateObject();
        if (state == NULL) {
            DUER_LOGE("create state failed");
            break;
        }

        duer_get_gps_info(&info);
        baidu_json_AddNumberToObject(state, "longitude", info.longitude);
        baidu_json_AddNumberToObject(state, "latitude", info.latitude);
        baidu_json_AddStringToObject(state, "geoCoordinateSystem", info.coordinate_sys);
    } while (0);

    return state;
}

duer_status_t duer_dcs_render_watch_pay_hdl(baidu_json *payload)
{
    char *msg = baidu_json_PrintBuffered(payload, 1024, 0);
    if (msg) {
        DUER_LOGI("ms=%s.\n", msg);
        duer_print_long_str(msg);
        baidu_json_release(msg);
    }
    // baidu add
    duerapp_WatchPayLogin(payload);
    return DUER_OK;
}

duer_status_t duer_dcs_render_watch_auth_hdl(baidu_json *payload)
{
    char *msg = baidu_json_PrintBuffered(payload, 1024, 0);
    if (msg) {
        DUER_LOGI("ms=%s.\n", msg);
        baidu_json_release(msg);
    }
    // baidu add
    duerapp_WatchAuthLogin(payload);
    return DUER_OK;
}

duer_status_t duer_dcs_require_login_hdl(baidu_json *payload)
{
    char *msg = baidu_json_PrintBuffered(payload, 1024, 0);
    if (msg) {
        DUER_LOGI("ms=%s.\n", msg);
        baidu_json_release(msg);
    }
    // baidu add
    MMIDUERAPP_WatchRequireLogin(payload);
    return DUER_OK;
}

duer_status_t duer_dcs_access_token_invalid_hdl()
{
    DUER_LOGI("Into invalid_access_token handler..");

    // baidu add
    duerapp_user_login_out();
    return DUER_OK;
}

duer_status_t duer_dcs_activity_notify_hdl(baidu_json *payload)
{
    DUER_LOGI("duer_dcs_activity_notify_hdl");
    // baidu add
    duerapp_notify_message(payload);
    return DUER_OK;
}

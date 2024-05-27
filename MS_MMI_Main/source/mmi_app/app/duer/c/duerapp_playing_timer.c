// Copyright (2021) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_playing_timer.c
 * Auth: Li Chang (changli@baidu.com)
 * Desc: Try playing timer APIs.
 */

#include <stdbool.h>
#include "os_api.h"
#include "lightduer_log.h"
#include "lightduer_thread.h"
#include "lightduer_timers.h"
#include "lightduer_audio_player.h"

static duer_timer_handler s_try_playing_timer = NULL;
int s_duration = 60000; // micro seconds
static int s_try_playing_cnt = 0;
static int s_try_playing_max_cnt = 10;
static bool s_prompt_flag = false;
static volatile bool s_is_try_playing = false;
static volatile bool s_is_started = false;

static const char *prompt_1 = "\xe6\x82\xa8\xe5\xb7\xb2\xe8\xaf\x95\xe5\x90\xac\xe4\xba\x86"; //您已试听了
static const char *prompt_3 = "\xe9\xa6\x96\xe4\xbc\x9a\xe5\x91\x98\xe6\xad\x8c\xe6\x9b\xb2\xef\xbc\x8c\xe5\xbc\x80\xe9\x80\x9a\xe4\xbc\x9a\xe5\x91\x98\xe5\x8f\xaf\xe7\x95\x85\xe5\x90\xac\xe5\x85\xa8\xe9\x83\xa8\xe6\xad\x8c\xe6\x9b\xb2\xe5\x93\xa6\xef\xbc\x81"; //首会员歌曲，开通会员可畅听全部歌曲哦！

static BLOCK_ID playing_callback_thread = 0;
static volatile signed char playing_callback_thread_flag = 0;

static unsigned int timer_get_remain_time(SCI_TIMER_PTR timer)
{
    char name[16] = {0};
    BOOLEAN is_active = FALSE;
    unsigned int remaining_time = 0;
    unsigned int reschedule_time = 0;
    uint32 ret = SCI_ERROR;

    if (timer == NULL) {
        return 0;
    }

    ret = SCI_GetTimerInfo(timer, name, &is_active, &remaining_time, &reschedule_time);
    DUER_LOGI("timer_info ret=%d, name=%s, is_active=%d, remaining_time=%u, reschedule_time=%u.", ret, name,
            is_active, remaining_time, reschedule_time);

    return remaining_time;
}

// static void exec_by_CA(int what, void *object)
// {
//     duer_audio_play_next();
// }

// static void duer_timer_callback_entry(unsigned int argc, duer_thread_entry_args_t *params)
// {
//     // duer_emitter_emit(exec_by_CA, 0, NULL);
//     duer_audio_play_next();
//     duer_thread_exit(params, 1);
// }
LOCAL void duer_timer_callback_entry(
    uint32 argc,
    void * argv
)
{
    while (1) {
        if (0 != playing_callback_thread_flag) {
            playing_callback_thread_flag = 0;
            duer_audio_play_next();
        }
        // SCI_Sleep(10);
        SCI_SuspendThread(playing_callback_thread);
    }
}

static void duer_try_playing_tm_cb(void *param)
{
    DUER_LOGI("time up for try playing.");
    if (0 == playing_callback_thread_flag) {
        playing_callback_thread_flag = 1;
        SCI_ResumeThread(playing_callback_thread);
    }
}

int duer_get_try_playing_max_cnt()
{
    return s_try_playing_max_cnt;
}

void duer_set_try_playing_max_cnt(int max_cnt)
{
    s_try_playing_max_cnt = max_cnt;
}

int duer_try_playing_timer_start()
{
    char tts[80] = {0};
    unsigned int remaining_time = 0;

    if (!s_is_try_playing) {
        return 0;
    }

    if (!playing_callback_thread) {
        // playing_callback_thread = duer_thread_create("duer_try_play_tm", "duer_try_play_tm_queue", 8192, 1, 76);
        // if (NULL == playing_callback_thread) {
        //     DUER_LOGE("duer timer playing_callback_thread create failed");
        //     return (-1);
        // }
        // if (duer_thread_start(playing_callback_thread, duer_timer_callback_entry, NULL) != 0) {
        //     DUER_LOGE("duer timer start create failed");
        //     duer_thread_destroy(playing_callback_thread);
        //     playing_callback_thread = NULL;
        //     return (-2);
        // }
        playing_callback_thread = SCI_CreateAppThread(
                                    "duer_try_play_tm",
                                    "duer_try_play_tm_queue",
                                    duer_timer_callback_entry,
                                    0,
                                    0,
                                    8192,
                                    8,
                                    76,
                                    SCI_PREEMPT,
                                    SCI_AUTO_START);
    }

    if (s_try_playing_cnt % (s_try_playing_max_cnt + 1) == 0 && !s_prompt_flag) {
        DUER_LOGI("try playing upto %d times.", s_try_playing_cnt);
        //tts to remind "您已试听了N首会员歌曲，开通会员可畅听全部歌曲哦！"
        snprintf(tts, sizeof(tts), "%s%d%s", prompt_1, s_try_playing_max_cnt, prompt_3);
        duer_dcs_user_interact(tts);
        s_prompt_flag = true;
        // 咪咕弹框
        duerapp_show_miguwin();
        return 0;
    }

    if (!s_is_started) {
        DUER_LOGI("start timer duration=%d, s_try_playing_cnt=%d.", s_duration, s_try_playing_cnt);
        duer_timer_start(s_try_playing_timer, s_duration);
        s_is_started = true;
    } else {
        remaining_time = timer_get_remain_time(duer_timer_get_inner_handle(s_try_playing_timer));
        if (remaining_time > 0) {
            DUER_LOGI("resume timer.");
            duer_timer_resume(s_try_playing_timer);
        }
    }

    return 0;
}

int duer_try_playing_timer_stop()
{
    if (!s_is_try_playing) {
        return 0;
    }

    DUER_LOGI("stop timer.");
    duer_timer_stop(s_try_playing_timer);

    return 0;
}

int duer_try_playing_timer_create(int duration, int *playing_cnt)
{
    duer_audio_playinfo_t playinfo;

    if (s_try_playing_timer == NULL) {
        s_try_playing_timer = duer_timer_acquire(duer_try_playing_tm_cb, NULL, DUER_TIMER_ONCE);
        if (s_try_playing_timer == NULL) {
            DUER_LOGE("Failed to create s_try_playing_timer!");
        }
    }

    s_duration = duration;
    s_is_try_playing = true;
    s_is_started = false;
    s_try_playing_cnt = ++(*playing_cnt);
    s_prompt_flag = false;
    DUER_LOGI("create timer, duration=%d, playing_cnt=%d.", duration, *playing_cnt);

    duer_audio_player_get_current_playinfo(&playinfo);
    if (playinfo.type == AUDIO_TYPE_DCS_AUDIO && playinfo.status == AUDIO_PLAY_STATUS_PLAY) {
        duer_try_playing_timer_start();
    }
}

int duer_try_playing_timer_delete()
{
    DUER_LOGI("delete timer.");
    s_is_try_playing = false;
}

void duer_try_playing_timer_thread_delete(void)
{
    if (playing_callback_thread) {
        SCI_TerminateThread(playing_callback_thread);
        playing_callback_thread = 0;
    }
}

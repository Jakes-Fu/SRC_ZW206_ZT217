// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_main.c
 * Auth: Liuwenshuai (liuwenshuai@baidu.com)
 * Desc: duerapp main.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 4/2009         liuwenshuai       Create                                   *
******************************************************************************/

/**--------------------------------------------------------------------------*/
/**                         Include Files                                    */
/**--------------------------------------------------------------------------*/
#include "std_header.h"
#include "window_parse.h"
#include "guilcd.h"
#include "mmidisplay_data.h"
#include "guifont.h"
#include "mmi_textfun.h"
#include "mmipub.h"
#include "guilistbox.h"
#include "guibutton.h"
#include "guitext.h"
#include "guilabel.h"
#include "duerapp_id.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "duerapp_text.h"
#include "duerapp_image.h"
#include "duerapp_anim.h"
#include "duerapp_nv.h"
#include "duerapp_main.h"
#include "watch_common_btn.h"
#include "watch_common_list.h"
#include "watch_commonwin_export.h"
#include "mmi_event_api.h"
#include "mmk_app.h"
#include "mmi_applet_table.h"
#include "guirichtext.h"
// #include "mmiwifi_export.h"
// #include "mmiphone_export.h"
#include "ctrlsetlist_export.h"
#include "ctrlanim_export.h"
#include "guisetlist.h"
#include "guictrl_api.h"
#include "gui_ucs2b_converter.h"
#include "dal_time.h"
#include "mmicom_time.h"
#include "img_dec_interface.h"

#include "baidu_json.h"
#include "lightduer_log.h"
#include "lightduer_random.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_timestamp.h"
#include "lightduer_dcs.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"
#include "lightduer_lib.h"
#include "lightduer_audio_codec_ops.h"
#include "lightduer_audio_adapter.h"
#include "lightduer_audio_player.h"
#include "lightduer_audio_codec_adapter.h"
#include "lightduer_thread.h"
#include "lightduer_sleep.h"

#include "duerapp.h"
#include "duerapp_slide_indicator.h"
#include "duerapp_writing.h"
#include "duerapp_common.h"
#include "duerapp_login.h"
#include "duerapp_qrwin.h"
#include "duerapp_http.h"
#include "duerapp_operate.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_audio_play_utils.h"
#include "duerapp_pcm_file.h"
#include "duerapp_center.h"
#include "duerapp_mplayer.h"
#include "duerapp_res_down.h"
#include "duerapp_dict_activity.h"
#include "duerapp_statistics.h"
#include "duerapp_assists_activity.h"
#include "duerapp_openapi.h"

/**--------------------------------------------------------------------------*/
/**                         MACRO DEFINITION                                 */
/**--------------------------------------------------------------------------*/
#define USE_DUERINPUT 0

#define DUERAPP_DUERBTN_RECT            {71,133,169,211}
#define DUERAPP_INPUT_TXT_RECT          {5,5,235,133}

#define DUERAPP_QUERY_TITLE_RECT        {10,16,230,42}
#define DUERAPP_QUERY_CONTENT_RECT_1    {1,60,239,85}
#define DUERAPP_QUERY_CONTENT_RECT_2    {1,100,239,125}
#define DUERAPP_QUERY_REMIND_RECT       {50,210,190,240}

#define DUERAPP_MUSIC_DANCE_RECT        {208,7,239,28}

#define DUERAPP_CENTER_BUTTON_RECT      {0,185,55,240}

#define DUERAPP_AGREE_RECT              {25,175,60,215}
#define DUERAPP_PROTOCOL_RECT           {70,180,230,210}
#define DUERAPP_START_RECT              {50,120,190,160}

#define DUERAPP_TIMEOUT_TIME            5000
#define DUERAPP_VOLUME_LENGTH           10
#define DUERAPP_CENTER_WIN_BG           RGB8882RGB565(0x000000)

#define DUERAPP_STANDARDCARD_PIC_SIZE   66000 //标准卡片显示阈值

#define INTERPRETER_TRY_USE_CNT         1

#define CONFIG_SHOW_CARD_PIC            0   //query页面是否支持图片显示

/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          CONSTANT ARRAY                                   */
/*---------------------------------------------------------------------------*/
#define DUERAPP_QUERY_ITEM_MAX   3

duerapp_interact_mode_t s_interact_mode = DUERAPP_INTERACT_MODE_NORMAL;
LOCAL MMI_STRING_T s_query_str_array[DUERAPP_QUERY_ITEM_MAX] = {0};
LOCAL const wchar *s_voice_interact_queries[20] = { // voice
    L"\"今天会下雨吗\"",
    L"\"现在多少度\"",
    L"\"深圳天气怎么样\"",
    L"\"下星期二是几号\"",
    L"\"天宫二号是什么\"",
    L"\"世界上最高的山\"",
    L"\"最大猫科动物是什么\"",
    L"\"黄河有多长\"",
    L"\"45加28等于多少\"",
    L"\"给我讲个故事\"",
    L"\"刻舟求剑的意思\"",
    L"\"苹果英文怎么说\"",
    L"\"春风吹又生的上一句\"",
    L"\"心情不好怎么办\"",
    L"\"我想玩成语接龙\"",
    L"\"下周二是几号\"",
    L"\"周杰伦的妈妈是谁\"",
    L"\"我想听米小圈上学记\"",
    L"\"放首适合写作业的歌\"",
    L"\"我想听欢快的音乐\""
};
LOCAL const s_voice_interact_query_num = sizeof(s_voice_interact_queries) / sizeof(s_voice_interact_queries[0]);

LOCAL const wchar *s_composition_queries[] = { // composition
    L"\"一件有意义的事\"", 
    L"\"关于春天的作文\"", 
    L"\"我的祖国\"", 
    L"\"我的同桌\"", 
    L"\"二十年后的我\"", 
    L"\"课间十分钟\"", 
    L"\"最可爱的人\""
};
LOCAL const s_composition_query_num = sizeof(s_composition_queries) / sizeof(s_composition_queries[0]);

LOCAL const wchar *s_dictionary_queries[] = {//dictionary
    L"\"鸟瞰\"", 
    L"\"掩耳盗铃\"", 
    L"\"买椟还珠\"", 
    L"\"艰辛\"", 
    L"\"中国\"", 
    L"\"积累\""
};
LOCAL const s_dictionary_query_num = sizeof(s_dictionary_queries) / sizeof(s_dictionary_queries[0]);

LOCAL const wchar *s_interpreter_queries[] = {//interpreter
    L"\"我来自上海\"", 
    L"\"我喜欢玩游戏\"", 
    L"\"今天天气不错\"", 
    L"\"你叫什么名字\"", 
    L"\"西红柿怎么说\""
};
LOCAL const s_interpreter_query_num = sizeof(s_interpreter_queries) / sizeof(s_interpreter_queries[0]);

/**--------------------------------------------------------------------------*/
/**                         EXTERNAL DECLARE                                 */
/**--------------------------------------------------------------------------*/
PUBLIC duerapp_render_card_t *s_render_card = NULL;

/**--------------------------------------------------------------------------*/
/**                         STATIC DEFINITION                                */
/**--------------------------------------------------------------------------*/
LOCAL uint8 s_timeout_timer = 0; // used for delay shoot
LOCAL const wchar **s_query_hints = NULL;

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*


/**--------------------------------------------------------------------------*
**                         LOCAL VARIABLES                                   *
**--------------------------------------------------------------------------*/

/*****************************************************************************/
//  Description : 主窗口的回调函数
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleDuerMainWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          );

/*****************************************************************************/
//  Description : RenderCard???????????
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleRenderCardWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           );

/*****************************************************************************/
//  Description : waiting
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleWaitingWinMsg(
                                       MMI_WIN_ID_T       win_id,     // ?????ID
                                       MMI_MESSAGE_ID_E   msg_id,     // ???????????ID
                                       DPARAM             param       // ???????????
                                       );
/*****************************************************************************/

LOCAL MMI_HANDLE_T RenderCard_TextBoxCtrlCreate(
                                    MMI_HANDLE_T                   win_handle,   //win id
                                    MMI_CTRL_ID_T                  ctrl_id       //control id
                                    );
LOCAL void RenderCard_UpdateTextCard(wchar *string_ptr);
/**--------------------------------------------------------------------------*/
/**                         GLOBAL DEFINITION                                */
/**--------------------------------------------------------------------------*/
LOCAL void MainWinFormParam(MMI_WIN_ID_T win_id);
LOCAL void UserCenterButtonCallBack(void);
LOCAL void NetworkErrorTips(void);
LOCAL void HandleWinTimer(uint8 timer_id);
LOCAL void UpdateDuerAsr(int code);
LOCAL void UpdateWinLabel(void);
LOCAL void UpdateInputScreenText(char *text);
LOCAL void UpdateDirectiveCard(duerapp_render_card_t *p_render_card);
LOCAL void UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_E state);
LOCAL void SetSysDatetime(void);
LOCAL void RenderCard_StandardCardFree(duer_standardcard_type_t type);
LOCAL void RenderCard_ListCardFree(duer_listcard_type_t type);
LOCAL void RenderCard_TextCardDestoryCntrl(bool is_create);
LOCAL void RenderCard_StandardCardDestoryCntrl(bool is_create);
LOCAL void RenderCard_ListCardDestoryCntrl(bool is_create);
LOCAL BOOLEAN duerapp_interpreter_is_allowed_use(void);

// ??????????
WINDOW_TABLE( MMI_DUERAPP_RENDERCARD_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_RENDER_CARD_WIN_ID ),
    WIN_FUNC((uint32) HandleRenderCardWinMsg ),    
    CREATE_TEXT_CTRL(MMI_DUERAPP_TEXTBOX_CTRL_ID),
    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
}; 

// ??????
WINDOW_TABLE( MMI_DUERAPP_MAIN_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_MAIN_WIN_ID ),
    WIN_FUNC((uint32) HandleDuerMainWinMsg ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE,MMI_DUERAPP_QUERY_TITLE_LABEL_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE,MMI_DUERAPP_QUERY_LABEL_CTRL_ID_1),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE,MMI_DUERAPP_QUERY_LABEL_CTRL_ID_2),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE,MMI_DUERAPP_REMIND_LABEL_CTRL_ID),
    CREATE_ANIM_CTRL(MMI_DUERAPP_ANIM_CTRL_ID,MMI_DUERAPP_MAIN_WIN_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_BUTTON_CTRL_ID),
    END_WIN
};

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
LOCAL int s_record_sate = 0;
LOCAL int s_init_status = 0;
LOCAL int s_media_playing = 0;
RENDER_PLAYER_INFO *s_renderPlayerInfo = PNULL;
volatile BOOLEAN s_user_novip = FALSE;
LOCAL BOOLEAN s_auto_listen = FALSE;
LOCAL BOOLEAN s_standcard_img_is_check = FALSE;

/*****************************************************************************/
PUBLIC MMI_HANDLE_T MMIDUERAPP_CreateDuerMainWin(BOOLEAN auto_listen)
{
    DUER_LOGI("%s auto_listen(%d)", __func__, auto_listen);
    s_auto_listen = auto_listen;
    return MMK_CreateWin((uint32*)MMI_DUERAPP_MAIN_WIN_TAB, PNULL);
}

/*****************************************************************************/
//  Description : left keystoke callback
//  Global resource dependence :
//  Author:liuwenshuai
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E DuerButtonCallBack(void)
{
    if (s_record_sate == 0) {
        if (!MMIDUERAPP_GetInitStatus()) {
            DUER_LOGW("dueros has not finished initialize yet, ignore record button callback...");
            return MMI_RESULT_TRUE;
        }
        if (DUERAPP_INTERACT_MODE_INTERPRETER == s_interact_mode
                && duerapp_interpreter_is_allowed_use() == FALSE) {

            wchar *wstr_ptr = L"跳转中...";
            duerapp_show_toast(wstr_ptr);
            //弹支付二维码，需要设置回正常的DIDP
            duer_set_didp_version(DIDP_VERSION_NORMAL);     //DIDP version for normal voice interactive mode
            duer_update_user_agent();
            duer_send_link_click_url(LINKCLICK_BUY_VIP);

            return MMI_RESULT_TRUE;
        }
        DUER_LOGI("start record...");
        DuerPermissionLog("access network..");
        MMIDEFAULT_TurnOnLCDBackLight();
        MMIDEFAULT_AllowTurnOffBackLight(FALSE);
        duer_asr_cancel();
        duer_record_start(FALSE);
        s_record_sate = 1;
        UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_LISTENING);
    } else {
        DUER_LOGI("stop record...");
        MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        duer_asr_stop();
        s_record_sate = 0;
        UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
    }
   return MMI_RESULT_TRUE;
}

LOCAL void StartTimeoutTimer(void)
{
    if(0 == s_timeout_timer)
    {
        s_timeout_timer = MMK_CreateTimer(DUERAPP_TIMEOUT_TIME, TRUE);
    }
    else
    {
        MMK_StopTimer(s_timeout_timer);
        s_timeout_timer = MMK_CreateTimer(DUERAPP_TIMEOUT_TIME, TRUE);
    }
}

/*****************************************************************************/
//  Description : close delay shoot timer
//  Global resource dependence : none
//  Author: liuwenshuai
//  Note:
/*****************************************************************************/
LOCAL void StopTimeoutTimer(void)
{
    if(0 != s_timeout_timer)
    {
        MMK_StopTimer(s_timeout_timer);
        s_timeout_timer = 0;
    }
}

LOCAL void UserCenterButtonCallBack(void)
{
    DUER_LOGI("UserCenterButtonCallBack...");
    MMIDUERAPP_CreateUserCenterWin();
}

/*****************************************************************************/
LOCAL void WatchRec_MainwinDrawBG(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
}

void duerapp_set_interact_mode(duerapp_interact_mode_t mode)
{
    DUER_LOGI("Interact mode:%d", mode);
    s_interact_mode = mode;
    switch (mode) {
    case DUERAPP_INTERACT_MODE_NORMAL:
        duer_set_didp_version(DIDP_VERSION_NORMAL);     //DIDP version for normal voice interactive mode
        duer_update_user_agent();
        duer_audio_dac_tts_mode(DUERAPP_INTERACT_MODE_NORMAL);
        break;
    case DUERAPP_INTERACT_MODE_WRITING:
        duer_set_didp_version(DIDP_VERSION_WRITING);    //DIDP version for querying compositions
        duer_update_user_agent();
        duer_audio_dac_tts_mode(DUERAPP_INTERACT_MODE_WRITING);
        break;
    case DUERAPP_INTERACT_MODE_DICTIONARY:
        duer_set_didp_version(DIDP_VERSION_DICTIONARY);    //DIDP version for querying dictionary
        duer_update_user_agent();
        duer_audio_dac_tts_mode(DUERAPP_INTERACT_MODE_DICTIONARY);
        break;
    case DUERAPP_INTERACT_MODE_INTERPRETER:
        duer_set_didp_version(DIDP_VERSION_INTERPRETER);    //DIDP version for querying interpreter
        duer_update_user_agent();
        duer_audio_dac_tts_mode(DUERAPP_INTERACT_MODE_INTERPRETER);
        break;
    default:
        DUER_LOGE("Invalid interact mode");
    }
}

PUBLIC wchar* DUERAPP_GetRandomHint(void)
{
    static int last_index = 0;    
    static int last_timestamp = 0;
    BOOLEAN use_last = FALSE;
    int now_timestamp = duer_real_timestamp();
    const int diff = 1;
    int32 num_hints = s_voice_interact_query_num;
    int32 rand_index1 = 0;

    if (((last_timestamp - now_timestamp) >= diff) || (now_timestamp - last_timestamp) >= diff) {
        DUER_LOGE("%s out time,now=%d,last=%d", __FUNCTION__, now_timestamp, last_timestamp);
    } else {
        DUER_LOGE("%s inprogress,now=%d,last=%d", __FUNCTION__, now_timestamp, last_timestamp);
        use_last = TRUE;
    }
    last_timestamp = now_timestamp;
    
    if (use_last) {
        rand_index1 = last_index;
    } else {
        rand_index1 = duer_random() % num_hints;
    }
    last_index = rand_index1;
    DUER_LOGI("DUERAPP_GetRandomHint index %d / %d", rand_index1, num_hints);
    return s_voice_interact_queries[rand_index1];
}

LOCAL void MainWinFormParam(MMI_WIN_ID_T win_id) 
{
    GUI_RECT_T remindRect = DUERAPP_QUERY_REMIND_RECT;
    GUI_RECT_T queryRect = DUERAPP_QUERY_TITLE_RECT;
    MMI_STRING_T querySting = {0};
    int num_hints = 0;
    int rand_index1 = 0, rand_index2 = 0;
    GUI_RECT_T queryRect1 = DUERAPP_QUERY_CONTENT_RECT_1;
    GUI_RECT_T queryRect2 = DUERAPP_QUERY_CONTENT_RECT_2;
    GUI_RECT_T btnRect = DUERAPP_DUERBTN_RECT;

    WatchRec_MainwinDrawBG();
    GUILABEL_SetRect(MMI_DUERAPP_REMIND_LABEL_CTRL_ID,&remindRect,FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_REMIND_LABEL_CTRL_ID, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_QUERY_FONT_COLOR);
    GUILABEL_SetRect(MMI_DUERAPP_QUERY_TITLE_LABEL_CTRL_ID,&queryRect,FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_QUERY_TITLE_LABEL_CTRL_ID, WATCH_DEFAULT_BIG_FONT, MMI_WHITE_COLOR);
    querySting.wstr_ptr = L"你可以这样问我";
    querySting.wstr_len = MMIAPICOM_Wstrlen(querySting.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_QUERY_TITLE_LABEL_CTRL_ID ,&querySting, FALSE);

    switch (s_interact_mode) {
    case DUERAPP_INTERACT_MODE_NORMAL:
        s_query_hints = s_voice_interact_queries;
        num_hints = s_voice_interact_query_num;
        break;
    case DUERAPP_INTERACT_MODE_WRITING:
        s_query_hints = s_composition_queries;
        num_hints = s_composition_query_num;
        break;
    case DUERAPP_INTERACT_MODE_DICTIONARY:
        s_query_hints = s_dictionary_queries;
        num_hints = s_dictionary_query_num;
        break;
    case DUERAPP_INTERACT_MODE_INTERPRETER:
        s_query_hints = s_interpreter_queries;
        num_hints = s_interpreter_query_num;
        break;
    default:
        s_query_hints = s_voice_interact_queries;
        num_hints = s_voice_interact_query_num;
    }

    rand_index1 = duer_random() % num_hints;
    DUER_LOGI("random index %d / %d",rand_index1, num_hints);
    GUILABEL_SetRect(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_1,&queryRect1,FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_1, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_QUERY_FONT_COLOR);
    s_query_str_array[0].wstr_ptr = s_query_hints[rand_index1];
    s_query_str_array[0].wstr_len = MMIAPICOM_Wstrlen(s_query_str_array[0].wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_1 ,&s_query_str_array[0], FALSE);

    rand_index2 = (rand_index1 + 1) % num_hints;
    GUILABEL_SetRect(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_2,&queryRect2,FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_2, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_QUERY_FONT_COLOR);
    s_query_str_array[1].wstr_ptr = s_query_hints[rand_index2];
    s_query_str_array[1].wstr_len = MMIAPICOM_Wstrlen(s_query_str_array[1].wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_2 ,&s_query_str_array[1], FALSE);

    GUIBUTTON_SetRect(MMI_DUERAPP_BUTTON_CTRL_ID, &btnRect);
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_BUTTON_CTRL_ID, DuerButtonCallBack);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_BUTTON_CTRL_ID,FALSE);
    GUIBUTTON_SetHandleLong(MMI_DUERAPP_BUTTON_CTRL_ID, TRUE);
    GUIAPICTRL_SetState(MMI_DUERAPP_BUTTON_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);
    GUIAPICTRL_SetBothRect(MMI_DUERAPP_ANIM_CTRL_ID, &btnRect);
    GUIANIM_SetVisible(MMI_DUERAPP_ANIM_CTRL_ID, FALSE, TRUE);
    GUIBUTTON_SetVisible(MMI_DUERAPP_BUTTON_CTRL_ID, FALSE, FALSE);
}


/*****************************************************************************/

MMIDUERAPP_ANIMATE_STATE_E s_duer_state = MMIDUERAPP_ANIMATE_STATE_LISTENING;

LOCAL void UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_E state)
{
    MMI_STRING_T tipSting = {0};
    GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    GUIANIM_DATA_INFO_T     data_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    MMI_HANDLE_T            anim_handle = NULL;

    if (!MMK_IsOpenWin(MMI_DUERAPP_MAIN_WIN_ID) ||
      !MMK_IsFocusWin(MMI_DUERAPP_MAIN_WIN_ID)) {
        return;
    }
    if (s_duer_state == state) {
        return;
    }

    anim_handle = MMK_GetCtrlHandleByWin(MMI_DUERAPP_MAIN_WIN_ID, MMI_DUERAPP_ANIM_CTRL_ID);
    ctrl_info.is_ctrl_id        = TRUE;
    ctrl_info.ctrl_id           = anim_handle;
    display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
    display_info.bg.bg_type = GUI_BG_COLOR;
    display_info.is_update      = TRUE;
    switch(state) {
        case MMIDUERAPP_ANIMATE_STATE_PREPARE:
        {
            tipSting.wstr_ptr = L"点击说话";
            data_info.img_id        = IMAGE_DUERAPP_PREPARE_ANIM;
            display_info.is_play_once = TRUE;
            MMIDEFAULT_AllowTurnOffBackLight(TRUE);
            //StopTimeoutTimer();
        }
        break;

        case MMIDUERAPP_ANIMATE_STATE_LISTENING:
        {
            data_info.img_id        = IMAGE_DUERAPP_LISTENING_ANIM;
            tipSting.wstr_ptr = L"聆听中";
        }
        break;

        case MMIDUERAPP_ANIMATE_STATE_THINKING:
        {
            tipSting.wstr_ptr = L"思考中";
            data_info.img_id        = IMAGE_DUERAPP_THINKING_ANIM;
            // ????????????????sdk??????ж?
            //StartTimeoutTimer();
        }
        break;
        default:
        break;
    }
    DUER_LOGI("UpdateDuerAnimation...");
    s_duer_state = state;
    GUIANIM_SetParam(&ctrl_info,&data_info,PNULL,&display_info);
    GUIANIM_SetVisible(MMI_DUERAPP_ANIM_CTRL_ID, TRUE, TRUE);
    GUIANIM_PlayAnim(anim_handle);
    //GUIANIM_UpdateDisplay(MMI_DUERAPP_ANIM_CTRL_ID);

    tipSting.wstr_len = MMIAPICOM_Wstrlen(tipSting.wstr_ptr);
    GUILABEL_SetText(MMI_DUERAPP_REMIND_LABEL_CTRL_ID ,&tipSting, TRUE);
    GUILABEL_SetVisible(MMI_DUERAPP_REMIND_LABEL_CTRL_ID,TRUE,TRUE);

    //MMK_UpdateScreen();
}


LOCAL void UpdateWinLabel(void)
{
    MMI_STRING_T querySting = {0};
    querySting.wstr_ptr = L"Ba";
    GUILABEL_SetText(MMI_DUERAPP_QUERY_TITLE_LABEL_CTRL_ID,&querySting,TRUE);
    GUILABEL_SetText(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_2,&querySting,TRUE);
    GUILABEL_SetText(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_1,&querySting,TRUE);
    GUILABEL_SetVisible(MMI_DUERAPP_QUERY_TITLE_LABEL_CTRL_ID,FALSE,TRUE);
    GUILABEL_SetVisible(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_2,FALSE,TRUE);
    GUILABEL_SetVisible(MMI_DUERAPP_QUERY_LABEL_CTRL_ID_1,FALSE,TRUE);
}

PUBLIC void duerapp_miguwin_close(void)
{
    if (s_renderPlayerInfo && s_renderPlayerInfo->url) {
        DUER_FREE(s_renderPlayerInfo->url);
        s_renderPlayerInfo->url = NULL;
    }
}

PUBLIC void duerapp_show_miguwin(void)
{
#if 0 //????????????UI???????????
    MMIDUERAPP_CreateMiguWin(LINKCLICK_BUY_VIP);
#else
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID,MSG_DUERAPP_SHOW_MIGU, PNULL,0);
#endif
}

PUBLIC BOOLEAN duerapp_Image_FromUrl_GetWh(const uint8* UrlData, uint32 *out_img_w, uint32 *out_img_h)
{
    char *pTmp = NULL, *pIdx = NULL;
    int len = 0;
    char tmp[4] = {0};
    uint32 img_w = 0, img_h = 0;
    
    if (UrlData == NULL
        || out_img_w == NULL
        || out_img_h == NULL) {
        return FALSE;
    }
    //?????
    pTmp = strstr((const char *)UrlData, "w=");
    if (pTmp == NULL) {
        return FALSE;
    }
    pTmp+=2;
    pIdx = pTmp;
    while (pTmp) {
        if (*pTmp >= '0'
            && *pTmp <= '9') {
            len++;
            pTmp++;
        } else {
            break;
        }
    }
    strncpy(tmp, pIdx, len);
    img_w = atoi(tmp);
    //?????
    pTmp = strstr((const char *)UrlData, "h=");
    if (pTmp == NULL) {
        return FALSE;
    }
    pTmp+=2;
    pIdx = pTmp;
    while (pTmp) {
        if (*pTmp >= '0'
            && *pTmp <= '9') {
            len++;
            pTmp++;
        } else {
            break;
        }
    }
    strncpy(tmp, pIdx, len);
    img_h = atoi(tmp);
    
    *out_img_w = img_w;
    *out_img_h = img_h;
    
    return TRUE;
}

PUBLIC BOOLEAN duerapp_Image_FromData_GetWh(const uint8* lpszData, uint32 unDataLen, uint32 *out_img_w, uint32 *out_img_h)
{
    uint8*   pszRGBData=NULL;
    uint32  img_handle = 0;
    IMG_DEC_SRC_T   dec_src = {0};
    IMG_DEC_INFO_T      dec_info = {0};
    IMG_DEC_FRAME_IN_PARAM_T    dec_in = {0};
    IMG_DEC_FRAME_OUT_PARAM_T   dec_out = {0};
    int w = 0, h = 0;

    //????
    if (0 != unDataLen)
    {
        dec_src.src_ptr       = (uint8*)lpszData;
        dec_src.src_file_size = unDataLen;
    }
    else
    {
        dec_src.file_name_ptr = (uint16 *)lpszData;
    }

    //?????????
    if (IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info))
    {
        DUER_LOGE("(%d)[duer_watch]:error", __LINE__);
        return FALSE;
    }

    switch (dec_info.img_type)
    {
    case IMG_DEC_TYPE_BMP:
        w = dec_info.img_detail_info.bmp_info.img_width;
        h = dec_info.img_detail_info.bmp_info.img_height;
        break;
    case IMG_DEC_TYPE_PNG:
        w = dec_info.img_detail_info.png_info.img_width;
        h = dec_info.img_detail_info.png_info.img_height;
        break;
    case IMG_DEC_TYPE_JPEG:
        w = dec_info.img_detail_info.jpeg_info.img_width;
        h = dec_info.img_detail_info.jpeg_info.img_heigth;
        break;
    case IMG_DEC_TYPE_GIF:
        w = dec_info.img_detail_info.gif_info.img_width;
        h = dec_info.img_detail_info.gif_info.img_height;
        break;
    case IMG_DEC_TYPE_WBMP:
        w = dec_info.img_detail_info.wbmp_info.img_width;
        h = dec_info.img_detail_info.wbmp_info.img_height;
        break;
    default:
        return FALSE;
    }

    pszRGBData = (uint8*)DUER_CALLOC(1, w*h*2);
    if (NULL == pszRGBData) 
    {
        DUER_LOGE("(%d)[duer_watch]:error", __LINE__);
        return FALSE;
    }

    if (IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle)) 
    {
        DUER_FREE(pszRGBData);
        DUER_LOGE("(%d)[duer_watch]:error", __LINE__);
        return FALSE;
    }

    dec_in.handle = img_handle;
    //set decode target
    dec_in.is_dec_thumbnail = 0;
    dec_in.target_ptr       = pszRGBData;
    dec_in.target_buf_size  = w*h*2;
    dec_in.target_width     = w;
    dec_in.target_height    = h;
    dec_in.img_rect.left    = 0;
    dec_in.img_rect.top     = 0;
    dec_in.img_rect.right   = w-1;
    dec_in.img_rect.bottom  = h-1;
    dec_in.data_format = IMG_DEC_RGB565;

    //??ν???
    if (IMG_DEC_RET_SUCCESS != IMG_DEC_GetFrame(&dec_in,&dec_out))
    {
        IMG_DEC_Destroy(img_handle);
        DUER_FREE(pszRGBData);
        return FALSE;
    }

    *out_img_w = dec_out.real_width;
    *out_img_h = dec_out.real_height;

    IMG_DEC_Destroy(img_handle);

    DUER_FREE(pszRGBData);
    return TRUE;
}

/*=============================================================
 ???????? : duerapp_display_image
 ???? : ????????????????????
 ????:
        const unsigned short* lpszData[IN]  --- δ????????BUFFER
        unsigned long unDataLen[IN]         --- 0/δ??????????????
        const GUI_RECT_T* lpRC[IN]          --- ???????
        const GUI_LCD_DEV_INFO *pdevInfo[IN]--- ?豸???
 ???? : TRUE: ???
        FALSE: ???
=============================================================*/
PUBLIC BOOLEAN duerapp_Image_Display(const uint8* lpszData, uint32 unDataLen, GUI_RECT_T* lpRC, const GUI_LCD_DEV_INFO *pdevInfo)
{
    uint8*   pszRGBData=NULL;
    uint32  img_handle = 0;
    IMG_DEC_SRC_T   dec_src = {0};
    IMG_DEC_INFO_T      dec_info = {0};
    IMG_DEC_FRAME_IN_PARAM_T    dec_in = {0};
    IMG_DEC_FRAME_OUT_PARAM_T   dec_out = {0};
    GUIIMG_BITMAP_T  bitmap_handle = {0};
    GUI_POINT_T                 dis_point = {0};
    int w = 0, h = 0;
    
    //????
    if (0 != unDataLen)
    {
        dec_src.src_ptr       = (unsigned char*)lpszData;
        dec_src.src_file_size = unDataLen;
    }
    else
    {
        dec_src.file_name_ptr = (uint16 *)lpszData;
    }

    //?????????
    if (IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info))
    {
        DUER_LOGE("(%d)[duer_watch]:error", __LINE__);
        return FALSE;
    }

    switch (dec_info.img_type)
    {
    case IMG_DEC_TYPE_BMP:
        w = dec_info.img_detail_info.bmp_info.img_width;
        h = dec_info.img_detail_info.bmp_info.img_height;
        break;
    case IMG_DEC_TYPE_PNG:
        w = dec_info.img_detail_info.png_info.img_width;
        h = dec_info.img_detail_info.png_info.img_height;
        break;
    case IMG_DEC_TYPE_JPEG:
        w = dec_info.img_detail_info.jpeg_info.img_width;
        h = dec_info.img_detail_info.jpeg_info.img_heigth;
        break;
    case IMG_DEC_TYPE_GIF:
        w = dec_info.img_detail_info.gif_info.img_width;
        h = dec_info.img_detail_info.gif_info.img_height;
        break;
    case IMG_DEC_TYPE_WBMP:
        w = dec_info.img_detail_info.wbmp_info.img_width;
        h = dec_info.img_detail_info.wbmp_info.img_height;
        break;
    default:
        return FALSE;
    }

    pszRGBData = (uint8*)DUER_CALLOC(1, w*h*2);
    if (NULL == pszRGBData)
    {
        return FALSE;
    }

    if (IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle))
    {
        DUER_FREE(pszRGBData);
        return FALSE;
    }

    dec_in.handle = img_handle;
    //set decode target
    dec_in.is_dec_thumbnail = 0;
    dec_in.target_ptr       = pszRGBData;
    dec_in.target_buf_size  = w*h*2;
    dec_in.target_width     = w;
    dec_in.target_height    = h;
    dec_in.img_rect.left    = 0;
    dec_in.img_rect.top     = 0;
    dec_in.img_rect.right   = w-1;
    dec_in.img_rect.bottom  = h-1;
    dec_in.data_format = IMG_DEC_RGB565;

    //??ν???
    if (IMG_DEC_RET_SUCCESS != IMG_DEC_GetFrame(&dec_in,&dec_out))
    {
        IMG_DEC_Destroy(img_handle);
        DUER_FREE(pszRGBData);
        return FALSE;
    }

    IMG_DEC_Destroy(img_handle);

    bitmap_handle.bit_ptr = pszRGBData;
    bitmap_handle.img_width = dec_out.real_width;
    bitmap_handle.img_height = dec_out.real_height;
    
    dis_point.x=lpRC->left;
    dis_point.y=lpRC->top;

    GUIIMG_DisplayBmp(FALSE, lpRC, &dis_point, &bitmap_handle, pdevInfo);
    
    DUER_FREE(pszRGBData);
    return TRUE;
}

PUBLIC void MMIDUERAPP_ScreenInputEvent(char *text)
{
    int utf8_len = strlen(text) + 1;
    if (utf8_len > 1) {
        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_MAIN_WIN_ID,MSG_DUERAPP_INPUT_SCREEN,text,utf8_len);
    }
}

LOCAL void UpdateInputScreenText(char *text) 
{   
    int utf8_len = 0;
    wchar *wch_txt = NULL;
    UpdateWinLabel();
    utf8_len = strlen(text);
    
    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    wch_txt = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
    if (wch_txt) {
        GUI_UTF8ToWstr(wch_txt, utf8_len, (const uint8 *)text, utf8_len);
        DUER_LOGI("(%s)[duer_watch]:wtxt(%s)", __FUNCTION__, wch_txt);
        RenderCard_UpdateTextCard(wch_txt);
        DUER_FREE(wch_txt);
    }
}

LOCAL void RenderCard_StandardCardTimerCb(uint32 ctx)
{
    DUER_LOGI("(%s)[duer_watch]:cb", __func__);
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
}

LOCAL void RenderCard_StandardCardTimerStop(void)
{
    BOOLEAN ret = SCI_ERROR;
    if(s_render_card->timer_id != NULL)
    {
        SCI_DeactiveTimer(s_render_card->timer_id);
        ret = SCI_DeleteTimer(s_render_card->timer_id);

        if(ret == SCI_SUCCESS) {
            DUER_LOGI("(%s)[duer_watch]:delete success", __func__);
        } else {
            DUER_LOGI("(%s)[duer_watch]:delete failed", __func__);
        }
        s_render_card->timer_id = NULL;
    }
}

LOCAL void RenderCard_StandardCardTimerStart(void)
{
    BOOLEAN ret = SCI_ERROR;
    
    RenderCard_StandardCardTimerStop();
    
    if(s_render_card->timer_id == NULL)
    {
        s_render_card->timer_id = SCI_CreateTimer("STANDARDCARD_PIC_DOWNLOAD_TIMEROUT", RenderCard_StandardCardTimerCb, NULL, 1000, SCI_NO_ACTIVATE);
        if(s_render_card->timer_id == NULL) {
            DUER_LOGI("(%s)[duer_watch]:create failed", __func__);
            return;
        }
    }
    ret = SCI_ActiveTimer(s_render_card->timer_id);
 
    if(ret == SCI_SUCCESS) {
        DUER_LOGI("(%s)[duer_watch]:active success", __func__);
    } else {
        DUER_LOGI("(%s)[duer_watch]:active failed", __func__);
    }
}

LOCAL BOOLEAN RenderCard_ParamInit(void)
{
    if (s_render_card == NULL) {
        s_render_card = (duerapp_render_card_t *)DUER_CALLOC(1, sizeof(duerapp_render_card_t));
        return TRUE;
    }
    return FALSE;
}

LOCAL MMI_HANDLE_T RenderCard_ParamDestory(void)
{
    RenderCard_StandardCardFree(DUER_STANDARDCARD_TYPE_ALL);
    RenderCard_ListCardFree(DUER_LISTCARD_TYPE_ALL);
    if (s_render_card) {
        if (s_render_card->standardcard) {
            DUER_FREE(s_render_card->standardcard);
            s_render_card->standardcard = NULL;
        }
        
        if (s_render_card->listcard) {
            DUER_FREE(s_render_card->listcard);
            s_render_card->listcard = NULL;
        }
        DUER_FREE(s_render_card);
        s_render_card = NULL;
        return TRUE;
    }
    RenderCard_TextCardDestoryCntrl(FALSE);
    RenderCard_StandardCardDestoryCntrl(FALSE);
    RenderCard_ListCardDestoryCntrl(FALSE);
    return FALSE;
}

LOCAL void RenderCard_TextCardDestoryCntrl(bool is_create)
{
    static BOOLEAN is_have_create = FALSE;

    if (is_create) {
        if (is_have_create) {
            MMK_DestroyControl(MMI_DUERAPP_LABEL_CTRL_ID);
        }
        is_have_create = is_create;
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            MMK_DestroyControl(MMI_DUERAPP_LABEL_CTRL_ID);
        }
    }
}

LOCAL void RenderCard_StandardCardDestoryCntrl(bool is_create)
{
    static BOOLEAN is_have_create = FALSE;
    bool state = FALSE;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    MMK_DestroyControl(MMI_DUERAPP_RENDER_CARD_ANIM_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_RENDER_CARD_TEXT_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_RENDER_CARD_FORM_CTRL_ID);
}

LOCAL void RenderCard_ListCardDestoryCntrl(bool is_create)
{
    static BOOLEAN is_have_create = FALSE;
    bool state = FALSE;
    int i = 0, number = 0;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    number = (MMI_DUERAPP_RENDER_CARD_LIST_CONTENT20_CTRL_ID-1) - MMI_DUERAPP_RENDER_CARD_LIST_FORM_CTRL_ID;
    for (i = number; i >= 0; i--) {
        MMK_DestroyControl((MMI_DUERAPP_RENDER_CARD_LIST_CONTENT20_CTRL_ID-1) - (number - i));
    }
}

LOCAL MMI_HANDLE_T RenderCard_TextBoxCtrlCreate(
    MMI_HANDLE_T win_handle, //win id
    MMI_CTRL_ID_T ctrl_id    //control id
)
{   
    MMI_HANDLE_T ctrl_handle = 0;
    GUITEXT_INIT_DATA_T init_data = {0};
    // MMI_CONTROL_CREATE_T create = {.ctrl_id = ctrl_id, .guid = SPRD_GUI_TEXTBOX_ID, .parent_win_handle = win_handle, .init_data_ptr = &init_data};
    MMI_CONTROL_CREATE_T create;
     create.ctrl_id = ctrl_id;
    create.guid = SPRD_GUI_TEXTBOX_ID;
    create.parent_win_handle = win_handle;
    create.init_data_ptr = &init_data;

    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);   
    //destory control
    RenderCard_ListCardDestoryCntrl(FALSE);
    RenderCard_StandardCardDestoryCntrl(FALSE);
    RenderCard_TextCardDestoryCntrl(TRUE);
    
    MMK_CreateControl(&create);
    
    ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
    return ctrl_handle;
}

LOCAL void RenderCard_StandardCardFree(duer_standardcard_type_t type)
{
    if(s_render_card == NULL
        || s_render_card->standardcard == NULL) {
        DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
        return;
    }

    if (type == DUER_STANDARDCARD_TYPE_ALL) {
        goto STANDARD_SRC;
    } else if (type == DUER_STANDARDCARD_TYPE_SRC) {
        STANDARD_SRC:
        if (s_render_card->standardcard->image_url) {
            DUER_FREE(s_render_card->standardcard->image_url);
            s_render_card->standardcard->image_url = NULL;
        }
        if (type == DUER_STANDARDCARD_TYPE_ALL) {
            goto STANDARD_LINK;
        }
    } else if (type == DUER_STANDARDCARD_TYPE_LINK) {
        STANDARD_LINK:
        if (s_render_card->standardcard->image_click_url) {
            DUER_FREE(s_render_card->standardcard->image_click_url);
            s_render_card->standardcard->image_click_url = NULL;
        }
        if (type == DUER_STANDARDCARD_TYPE_ALL) {//?????
            if (s_render_card->standardcard->content) {
                DUER_FREE(s_render_card->standardcard->content);
                s_render_card->standardcard->content = NULL;
            }
            if (s_render_card->standardcard) {
                DUER_FREE(s_render_card->standardcard);
                s_render_card->standardcard = NULL;
            }
        }
    }
}

LOCAL void RenderCard_StandardCardResCfg(duerapp_render_card_t *p_render_card)
{
    if (p_render_card == NULL
        || s_render_card == NULL) {
        return;
    }
    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    RenderCard_StandardCardFree(DUER_STANDARDCARD_TYPE_ALL);

    *s_render_card = *p_render_card;
    s_render_card->type = p_render_card->type;
    s_render_card->standardcard = p_render_card->standardcard;

    if(s_render_card->standardcard == NULL) {
        DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
        return;
    }
    s_render_card->standardcard->content = p_render_card->standardcard->content;
    s_render_card->standardcard->image_url = p_render_card->standardcard->image_url;
    s_render_card->standardcard->image_click_url = p_render_card->standardcard->image_click_url;
}

LOCAL void RenderCard_ListCardFree(duer_listcard_type_t type)
{
    if(s_render_card == NULL
        || s_render_card->listcard == NULL) {
        DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
        return;
    }
    DUER_LOGI("(%s)[duer_watch]:type(%d)", __FUNCTION__, type);

    if (type == DUER_LISTCARD_TYPE_ALL) {
        DUER_FREE(s_render_card->listcard);
        s_render_card->listcard = NULL;
    } 
}

LOCAL void RenderCard_ListCardResCfg(duerapp_render_card_t *p_render_card)
{
    int i = 0;
    if (p_render_card == NULL
        || s_render_card == NULL) {
        return;
    }
    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    RenderCard_ListCardFree(DUER_LISTCARD_TYPE_ALL);
    
    *s_render_card = *p_render_card;
    s_render_card->type = p_render_card->type;
    s_render_card->listcard = p_render_card->listcard;
    
    if (s_render_card->listcard == NULL) {
        DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
        return;
    }
    
    s_render_card->listcard->index = p_render_card->listcard->index;
    s_render_card->listcard->cnt = p_render_card->listcard->cnt;
    
    for (i = 0; i < s_render_card->listcard->cnt; i++) {
        strcpy(s_render_card->listcard->image_src[i], p_render_card->listcard->image_src[i]);
        strcpy(s_render_card->listcard->title[i], p_render_card->listcard->title[i]);
        strcpy(s_render_card->listcard->content[i], p_render_card->listcard->content[i]);
        strcpy(s_render_card->listcard->url[i], p_render_card->listcard->url[i]);
    }
    DUER_LOGI("(%s)[duer_watch]:cnt(%d)", __FUNCTION__, s_render_card->listcard->cnt);
}

LOCAL MMI_RESULT_E RenderCard_StandardCardImgCallback(void *usr_data)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    int ret = -1;
    char *url = (char *)usr_data;
    
    if(url) {//?????????????
//        ret = duer_send_link_click_url(s_render_card.standardcard.image_click_url);
//        if(ret == -1) {
//            result = MMI_RESULT_FALSE;
//        }
    }
    DUER_LOGI("(%s)[duer_watch]:query(%s)", __FUNCTION__, result ? "success" : "failed");
    return result;
}

LOCAL MMI_RESULT_E RenderCard_ListCardImgCallback(void *usr_data)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    int ret = -1;
    char *url = (char *)usr_data;
    
    if(url) {
        ret = duer_send_link_click_url(url);
        if(ret == -1) {
            result = MMI_RESULT_FALSE;
        }
        DUER_LOGI("(%s)[duer_watch]:url(%s)", __FUNCTION__, url);
    }
    DUER_LOGI("(%s)[duer_watch]:query(%s)", __FUNCTION__, result ? "success" : "failed");
    return result;
}

LOCAL MMI_HANDLE_T RenderCard_FormCtrlCreate(MMI_HANDLE_T win_handle, duerapp_render_card_t *p_render_card)
{
    MMI_HANDLE_T ctrl_handle = 0;
    int i = 0, j = 0, k = 0;
    int rows = 0;
    
    if (p_render_card == NULL) {
        DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
        return 0;
    }
    DUER_LOGI("(%s)[duer_watch]:type(%d)", __FUNCTION__, p_render_card->type);
    
    if (p_render_card->type == DUER_CARD_TYPE_STANDARD) {//standard card
        MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_RENDER_CARD_FORM_CTRL_ID;
        
        //0???
        {
            RenderCard_TextCardDestoryCntrl(FALSE);
            RenderCard_ListCardDestoryCntrl(FALSE);
            RenderCard_StandardCardDestoryCntrl(TRUE);
        }

        //1动态创建form
        {
            GUIFORM_CreatDynaCtrl(win_handle, form_ctrl_id, GUIFORM_LAYOUT_ORDER);
        }

        //2动态创建form子控件ANIM
        {
            GUIANIM_INIT_DATA_T anim_init_data = {0};
            GUIFORM_DYNA_CHILD_T anim_form_child_ctrl = {0};
            anim_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_ANIM_CTRL_ID;
            anim_form_child_ctrl.init_data_ptr = &anim_init_data;
            anim_form_child_ctrl.guid = SPRD_GUI_ANIM_ID;
            GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &anim_form_child_ctrl);
        }

        //3动态创建form子控件TEXT
        {
            GUITEXT_INIT_DATA_T text_init_data = {0};
            GUIFORM_DYNA_CHILD_T text_form_child_ctrl = {0};
            text_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_TEXT_CTRL_ID;
            text_form_child_ctrl.init_data_ptr = &text_init_data;
            text_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
            GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &text_form_child_ctrl);
        }
        ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, form_ctrl_id);
    } else if (p_render_card->type == DUER_CARD_TYPE_LIST) {//list card
        MMI_HANDLE_T form_ctrl_id = MMI_DUERAPP_RENDER_CARD_FORM_CTRL_ID;

        if (p_render_card->listcard == NULL) {
            DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
            return 0;
        }
        rows = (p_render_card->listcard->cnt - 1)/2 + 1;
        
        //0???
        RenderCard_TextCardDestoryCntrl(FALSE);
        RenderCard_StandardCardDestoryCntrl(FALSE);
        RenderCard_ListCardDestoryCntrl(TRUE);

        //1动态创建form
        {
            GUIFORM_CreatDynaCtrl(win_handle, form_ctrl_id, GUIFORM_LAYOUT_ORDER);
        }

        //2动态创建子form
        {            
            GUIFORM_INIT_DATA_T childform_init_data = {0};
            GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};

            childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
            
            for (i = 0; i < rows; i++) {
                MMI_HANDLE_T form_child_ctrl_id = 0;

                childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
                childform_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_FORM1_CTRL_ID + i;
                childform_form_child_ctrl.init_data_ptr = &childform_init_data;
                form_child_ctrl_id = childform_form_child_ctrl.child_handle;
                GUIFORM_CreatDynaChildCtrl(win_handle, form_ctrl_id, &childform_form_child_ctrl);
                GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
                for (j = 0; j < 2; j++) {
                    childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
                    childform_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_FORMX_FORM1_CTRL_ID + k;
                    childform_form_child_ctrl.init_data_ptr = &childform_init_data;
                    GUIFORM_CreatDynaChildCtrl(win_handle, form_child_ctrl_id, &childform_form_child_ctrl);
                    GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
                    k++;
                }
            }
        }

        {
            MMI_HANDLE_T child_form_ctrl_id = 0;
            GUIANIM_INIT_DATA_T anim_init_data = {0};
            GUIFORM_DYNA_CHILD_T anim_form_child_ctrl = {0};
            GUILABEL_INIT_DATA_T title_init_data = {0};
            GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
            GUILABEL_INIT_DATA_T content_init_data = {0};
            GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};

            anim_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_ANIM1_CTRL_ID;
            anim_form_child_ctrl.init_data_ptr = &anim_init_data;
            anim_form_child_ctrl.guid = SPRD_GUI_ANIM_ID;

            title_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_TITLE1_CTRL_ID;
            title_form_child_ctrl.init_data_ptr = &title_init_data;
            title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;

            content_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_CONTENT1_CTRL_ID;
            content_form_child_ctrl.init_data_ptr = &content_init_data;
            content_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;

            for (j = 0; j < p_render_card->listcard->cnt; j++) {
                child_form_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_FORMX_FORM1_CTRL_ID + j;
                // 3动态创建form子控件ANIM
                anim_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_ANIM1_CTRL_ID + j;
                GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &anim_form_child_ctrl);
                // 4动态创建form子控件TITLE
                title_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_TITLE1_CTRL_ID + j;
                GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
                // 5动态创建form子控件CONTENT
                content_form_child_ctrl.child_handle = MMI_DUERAPP_RENDER_CARD_LIST_CONTENT1_CTRL_ID + j;
                GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
            }
        }
        ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, form_ctrl_id);
    }
    return ctrl_handle;
}

LOCAL void RenderCard_UpdateStandardCardAnim(void)
{
    MMI_HANDLE_T form_ctrl_handle = MMK_GetCtrlHandleByWin(MMI_DUERAPP_MAIN_WIN_ID, MMI_DUERAPP_RENDER_CARD_FORM_CTRL_ID);
    MMI_HANDLE_T anim_ctrl_id = MMI_DUERAPP_RENDER_CARD_ANIM_CTRL_ID;

    GUIFORM_CHILD_WIDTH_T width = {160, GUIFORM_CHILD_WIDTH_FIXED};
    GUIFORM_CHILD_HEIGHT_T height = {80, GUIFORM_CHILD_HEIGHT_FIXED};
    uint32 img_w = 0, img_h = 0;
    BOOLEAN ret = FALSE;
    
    GUIANIM_DATA_INFO_T img_info = {0};
    img_dsc_t *resource = NULL;
    
    // GUIANIM_CTRL_INFO_T ctrl_info = {.is_ctrl_id = TRUE, .ctrl_id = anim_ctrl_id};
    GUIANIM_CTRL_INFO_T ctrl_info;
    // GUIANIM_DISPLAY_INFO_T  display_info = {
    //     .align_style = GUIANIM_ALIGN_HVMIDDLE,
    //     .is_auto_zoom_in = TRUE,
    //     .is_update = TRUE,
    //     .is_disp_one_frame = TRUE,
    //     .is_zoom = TRUE,
    //     .is_handle_transparent = TRUE,
    //     .is_syn_decode = TRUE,
    // };
    GUIANIM_DISPLAY_INFO_T  display_info;

    DUER_LOGI("(%s)[duer_watch]:update standardcard anim", __FUNCTION__);

    memset(&ctrl_info, 0x00, sizeof(ctrl_info));
    ctrl_info.is_ctrl_id = TRUE;
    ctrl_info.ctrl_id = anim_ctrl_id;

    memset(&display_info, 0x00, sizeof(display_info));
    display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
    display_info.is_auto_zoom_in = TRUE;
    display_info.is_update = TRUE;
    display_info.is_disp_one_frame = TRUE;
    display_info.is_zoom = TRUE;
    display_info.is_handle_transparent = TRUE;
    display_info.is_syn_decode = TRUE;
    
    resource = imageGetResource(IMG_DOWN_RENDER_STANDARDCARD, 0);
    if (resource
        && resource->data) {
        if (s_standcard_img_is_check) {//URL???????????
            s_standcard_img_is_check = FALSE;
            DUER_LOGI("(%s)[duer_watch]:img nomral", __FUNCTION__);
        } else {
            ret = duerapp_Image_FromData_GetWh(resource->data, resource->data_size, &img_w, &img_h);
            if (ret == FALSE //???????????1024*1024
                || img_w > 1024
                || img_h > 1024) {
                return;
            }
            DUER_LOGI("(%s)[duer_watch]:img_w(%d), img_h(%d)", __FUNCTION__, img_w, img_h);
        }
        img_info.data_ptr = (resource)->data;
        img_info.data_size = (resource)->data_size;
    } else {
        DUER_LOGE("(%s)[duer_watch]:resource get err", __FUNCTION__);
        return;
    }
    
    CTRLFORM_SetChildWidth(form_ctrl_handle, anim_ctrl_id, &width);
    CTRLFORM_SetChildHeight(form_ctrl_handle, anim_ctrl_id, &height);
    CTRLFORM_SetChildAlign(form_ctrl_handle, anim_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
    
    CTRLANIM_SetBgColor(anim_ctrl_id, MMI_BLACK_COLOR);
    CTRLANIM_SetCallBackFunc(anim_ctrl_id, RenderCard_StandardCardImgCallback, NULL);
    CTRLANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
    CTRLANIM_SetVisible(anim_ctrl_id, TRUE, TRUE);
    MMK_SetAtvCtrl(MMI_DUERAPP_MAIN_WIN_ID, anim_ctrl_id);
}

LOCAL void RenderCard_UpdateListCardAnim(int res_id)
{
    MMI_HANDLE_T anim_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_ANIM1_CTRL_ID + res_id;
    GUIANIM_DATA_INFO_T img_info = {0};
    img_dsc_t *resource = NULL;
    // GUIANIM_CTRL_INFO_T ctrl_info = {.is_ctrl_id = TRUE, .ctrl_id = anim_ctrl_id};
    GUIANIM_CTRL_INFO_T ctrl_info;
    // GUIANIM_DISPLAY_INFO_T  display_info = {
    //     .align_style = GUIANIM_ALIGN_HVMIDDLE,
    //     .is_auto_zoom_in = TRUE,
    //     .is_update = TRUE,
    //     .is_disp_one_frame = TRUE,
    //     .is_zoom = TRUE,
    //     .is_handle_transparent = TRUE,
    //     .is_syn_decode = TRUE,
    // };
    GUIANIM_DISPLAY_INFO_T  display_info;

    memset(&ctrl_info, 0x00, sizeof(ctrl_info));
    ctrl_info.is_ctrl_id = TRUE;
    ctrl_info.ctrl_id = anim_ctrl_id;

    memset(&display_info, 0x00, sizeof(display_info));
    display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
    display_info.is_auto_zoom_in = TRUE;
    display_info.is_update = TRUE;
    display_info.is_disp_one_frame = TRUE;
    display_info.is_zoom = TRUE;
    display_info.is_handle_transparent = TRUE;
    display_info.is_syn_decode = TRUE;

    DUER_LOGI("(%s)[duer_watch]:update listcard anim", __FUNCTION__);
    
    resource = imageGetResource(IMG_DOWN_RENDER_LISTCARD, res_id);
    if (resource) {
        if (resource->data && resource->data_size > 0) {
            img_info.data_ptr = (resource)->data;
            img_info.data_size = (resource)->data_size;
        } else {
            DUER_LOGE("(%s)[duer_watch]:resource get err", __FUNCTION__);
            img_info.img_id = IMAGE_DUERAPP_TITLE;
        }
    } else {
        DUER_LOGE("(%s)[duer_watch]:resource get err", __FUNCTION__);
        img_info.img_id = IMAGE_DUERAPP_TITLE;
    }
    
    GUIANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
    GUIANIM_SetVisible(anim_ctrl_id, TRUE, TRUE);
    MMK_SetAtvCtrl(MMI_DUERAPP_MAIN_WIN_ID, anim_ctrl_id);
}

LOCAL void RenderCard_UpdateTextCard(wchar *string_ptr)
{
    MMI_HANDLE_T ctrl_handle = RenderCard_TextBoxCtrlCreate(MMI_DUERAPP_MAIN_WIN_ID, MMI_DUERAPP_LABEL_CTRL_ID);
    
    GUI_RECT_T rectTime = DUERAPP_INPUT_TXT_RECT;
    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    // GUI_BG_T bg = {.bg_type = GUI_BG_COLOR, .color = MMI_BLACK_COLOR};
    GUI_BG_T bg;
    bg.bg_type = GUI_BG_COLOR;
    bg.color = MMI_BLACK_COLOR;

    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    
    GUITEXT_IsDisplayPrg(TRUE, ctrl_handle);
    GUITEXT_SetAlign(ctrl_handle, ALIGN_HMIDDLE);
    GUITEXT_SetRect(ctrl_handle, &rectTime);
    GUITEXT_SetClipboardEnabled(ctrl_handle, FALSE);
    GUITEXT_SetFont(ctrl_handle, &font_size, &font_color);
    GUITEXT_SetBg(ctrl_handle, &bg);
    GUITEXT_SetString(ctrl_handle, string_ptr, MMIAPICOM_Wstrlen(string_ptr), TRUE);
    
    MMK_SetAtvCtrl(MMI_DUERAPP_MAIN_WIN_ID, MMI_DUERAPP_LABEL_CTRL_ID);
}

LOCAL void RenderCard_UpdateStandardCard(duerapp_render_card_t *p_render_card, BOOLEAN is_update_anim)
{
    MMI_HANDLE_T form_ctrl_handle = 0;
    MMI_HANDLE_T anim_ctrl_id = 0;
    char *content = NULL;
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;

    if (p_render_card == NULL
        || p_render_card->standardcard == NULL
        || p_render_card->standardcard->content == NULL) {
        DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
        return;
    }
    form_ctrl_handle = RenderCard_FormCtrlCreate(MMI_DUERAPP_MAIN_WIN_ID, p_render_card);
    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    
    // 设置form参数
    {
        GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, RGB8882RGB565(0x0), FALSE};
        GUI_RECT_T form_rect = DUERAPP_INPUT_TXT_RECT;
        uint16 ver_space = 2;

        // 设置form背景
        GUIFORM_SetBg(form_ctrl_handle, &form_bg);
        GUIFORM_SetRect(form_ctrl_handle, &form_rect);
        GUIFORM_PermitChildFont(form_ctrl_handle, FALSE);
        GUIFORM_PermitChildBorder(form_ctrl_handle, FALSE);
        GUIFORM_SetSpace(form_ctrl_handle, PNULL, &ver_space);
    }

    // 设置ANIM控件参数
    {
        GUIFORM_CHILD_WIDTH_T width = {160, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T height = {1, GUIFORM_CHILD_HEIGHT_FIXED}; // 默认无图片高度
        GUIANIM_DATA_INFO_T img_info = {0};

        GUIANIM_CTRL_INFO_T ctrl_info = {0};
        GUIANIM_DISPLAY_INFO_T display_info = {0};
        BOOLEAN is_visible = FALSE;
        BOOLEAN is_update = FALSE;

        anim_ctrl_id = MMI_DUERAPP_RENDER_CARD_ANIM_CTRL_ID;
        img_info.img_id = IMAGE_DUERAPP_TITLE;
        ctrl_info.is_ctrl_id = TRUE;
        ctrl_info.ctrl_id = anim_ctrl_id;
        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_auto_zoom_in = TRUE;
        display_info.is_update = TRUE;
        display_info.is_disp_one_frame = TRUE;
       
        if (is_update_anim) {
            RenderCard_StandardCardTimerStop();
        }
        
        CTRLFORM_SetChildAlign(form_ctrl_handle, anim_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
        CTRLFORM_SetChildWidth(form_ctrl_handle, anim_ctrl_id, &width);
        CTRLFORM_SetChildHeight(form_ctrl_handle, anim_ctrl_id, &height);
        
        CTRLANIM_SetBgColor(anim_ctrl_id, MMI_BLACK_COLOR);
        CTRLANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
        CTRLANIM_SetVisible(anim_ctrl_id, is_visible, is_update);
    }

    // 设置text控件参数
    {
        GUIFORM_CHILD_WIDTH_T width = {240, GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_RENDER_CARD_TEXT_CTRL_ID;
        int txt_len = 0;
        MMI_STRING_T wch_dst_ptr = {0}, wch_src1_ptr = {0}, wch_src2_ptr = {0};
        int utf8_len = 0;
        wchar *wch_txt = NULL;
        char *content = NULL;

        content = p_render_card->standardcard->content;
        
        utf8_len = strlen(content);
        wch_txt = DUER_CALLOC(1, (utf8_len + 2) * sizeof(wchar));
        GUI_UTF8ToWstr(wch_txt, utf8_len, (const uint8 *)content, utf8_len);
        wch_src1_ptr.wstr_ptr = wch_txt;
        wch_src1_ptr.wstr_len = MMIAPICOM_Wstrlen(wch_src1_ptr.wstr_ptr);
        wch_src2_ptr.wstr_ptr = L"\n\n";// 针对汉字只能此处宽字符增加换行显示
        wch_src2_ptr.wstr_len = MMIAPICOM_Wstrlen(wch_src2_ptr.wstr_ptr);
        MMIAPICOM_CatTwoString(&wch_dst_ptr, &wch_src1_ptr, &wch_src2_ptr);
        
        CTRLFORM_SetChildWidth(form_ctrl_handle, text_ctrl_id, &width);
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_HMIDDLE);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        txt_len = MMIAPICOM_Wstrlen(wch_dst_ptr.wstr_ptr);
        SCI_TraceLow("(%s)(%d)[duer_watch]:txt_len(%d)", __func__, __LINE__, txt_len);
        GUITEXT_SetString(text_ctrl_id, wch_dst_ptr.wstr_ptr, MMIAPICOM_Wstrlen(wch_dst_ptr.wstr_ptr), TRUE);
        GUITEXT_SetMarginEx(text_ctrl_id, 0, 0, 0, (uint16)txt_len/8.65);// 目前无智能行高度算法
        
        DUER_FREE(wch_txt);
    }

    MMK_SetAtvCtrl(MMI_DUERAPP_MAIN_WIN_ID, MMI_DUERAPP_RENDER_CARD_FORM_CTRL_ID);
    // 动态刷新图片
    if (is_update_anim) {
        RenderCard_UpdateStandardCardAnim();
    }
}

LOCAL void RenderCard_UpdateListCard(duerapp_render_card_t *p_render_card)
{
    MMI_HANDLE_T form_ctrl_handle = RenderCard_FormCtrlCreate(MMI_DUERAPP_MAIN_WIN_ID, p_render_card);
    int i = 0, j = 0, k = 0;
    int rows = 0;
    int array_cnt = 0;

    if (p_render_card == NULL
        || p_render_card->listcard == NULL) {
        DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
        return;
    }
    array_cnt = p_render_card->listcard->cnt;
    
    // 设置form参数
    {
        GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, RGB8882RGB565(0x0), FALSE};
        GUI_RECT_T form_rect = DUERAPP_INPUT_TXT_RECT;
        uint16 hor_space = 0; // 水平间距
        uint16 ver_space = 3; // 垂直间距

        CTRLFORM_SetRect(form_ctrl_handle, &form_rect);
        CTRLFORM_SetBg(form_ctrl_handle, &form_bg);
        CTRLFORM_PermitChildBg(form_ctrl_handle, FALSE);
        CTRLFORM_PermitChildFont(form_ctrl_handle, FALSE);
        CTRLFORM_PermitChildBorder(form_ctrl_handle, FALSE);
        CTRLFORM_SetSpace(form_ctrl_handle, &hor_space, &ver_space);
        
        MMK_SetAtvCtrl(MMI_DUERAPP_MAIN_WIN_ID, form_ctrl_handle);
    }

    // 设置子form参数
    {
        GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, RGB8882RGB565(0x0), FALSE};
        MMI_HANDLE_T child_form_ctrl_id = 0;
        MMI_HANDLE_T child_form_form_ctrl_id = 0;
        uint16 form_height = 0;
        GUIFORM_CHILD_HEIGHT_T child_form_height = {0, GUIFORM_CHILD_HEIGHT_FIXED};

        form_height = CTRLFORM_GetHeight(form_ctrl_handle);
        rows = (p_render_card->listcard->cnt - 1)/2 + 1;
        
        for(i = 0; i < rows; i++) {
            child_form_height.add_data = form_height;
            child_form_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, FALSE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, FALSE);
            CTRLFORM_SetChildHeight(form_ctrl_handle, child_form_ctrl_id, &child_form_height);
            for (j = 0; j < 2; j++) {
                child_form_height.add_data = 144;
                child_form_form_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_FORMX_FORM1_CTRL_ID + k;
                CTRLFORM_SetBg(child_form_form_ctrl_id, &form_bg);
                CTRLFORM_PermitChildBg(child_form_form_ctrl_id, FALSE);
                CTRLFORM_PermitChildFont(child_form_form_ctrl_id, FALSE);
                CTRLFORM_PermitChildBorder(child_form_form_ctrl_id, FALSE);
                CTRLFORM_SetChildHeight(child_form_ctrl_id, child_form_form_ctrl_id, &child_form_height);
                k++;
            }
        }
    }

    // 设置LISTCARD参数
    {
        // 初始化ANIM控件参数
        MMI_HANDLE_T anim_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_ANIM1_CTRL_ID;
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_FORM1_CTRL_ID;
        GUIFORM_CHILD_WIDTH_T anim_width = {100, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T anim_height = {80, GUIFORM_CHILD_HEIGHT_FIXED};
        GUIANIM_DATA_INFO_T img_info = {0};
        GUIANIM_CTRL_INFO_T ctrl_info = {0};
        GUIANIM_DISPLAY_INFO_T display_info = {0};
        // 设初始化text控件的title参数
        MMI_HANDLE_T title_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_TITLE1_CTRL_ID;
        GUIFORM_CHILD_WIDTH_T title_width = {100, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T title_height = {32, GUIFORM_CHILD_HEIGHT_FIXED};
        GUI_COLOR_T title_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T title_font_size = WATCH_DEFAULT_NORMAL_FONT;
        int title_utf8_len = 0;
        MMI_STRING_T title_wshowtxt = {0};
        wchar *title_wstr = NULL;
        // 初始化text控件的content参数
        MMI_HANDLE_T content_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_CONTENT1_CTRL_ID;
        GUIFORM_CHILD_WIDTH_T content_width = {100, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T content_height = {32, GUIFORM_CHILD_HEIGHT_FIXED};
        GUI_COLOR_T content_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T content_font_size = WATCH_DEFAULT_NORMAL_FONT;
        int content_utf8_len = 0;
        MMI_STRING_T content_wshowtxt = {0};
        wchar *content_wstr = NULL;

        ctrl_info.is_ctrl_id = TRUE;
        ctrl_info.ctrl_id = anim_ctrl_id;

        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_auto_zoom_in = TRUE;
        display_info.is_update = TRUE;
        display_info.is_disp_one_frame = TRUE;
        display_info.is_zoom = TRUE;
        display_info.is_handle_transparent = TRUE;
        display_info.is_syn_decode = TRUE;

        for (j = 0; j < p_render_card->listcard->cnt; j++) {
            child_form_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_FORMX_FORM1_CTRL_ID + j;
            // 设置ANIM控件参数
            anim_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_ANIM1_CTRL_ID + j;
            img_info.img_id = IMAGE_DUERAPP_TITLE;
            CTRLFORM_SetChildWidth(child_form_ctrl_id, anim_ctrl_id, &anim_width);
            CTRLFORM_SetChildHeight(child_form_ctrl_id, anim_ctrl_id, &anim_height);
            
            CTRLANIM_SetBgColor(anim_ctrl_id, MMI_BLACK_COLOR);
            CTRLANIM_SetCallBackFunc(anim_ctrl_id, RenderCard_ListCardImgCallback, p_render_card->listcard->url[j]);
            CTRLANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
            CTRLANIM_SetVisible(anim_ctrl_id, TRUE, TRUE);
            
            // 设置text控件的title参数
            title_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_TITLE1_CTRL_ID + j;
            CTRLFORM_SetChildWidth(child_form_ctrl_id, title_ctrl_id, &title_width);
            CTRLFORM_SetChildHeight(child_form_ctrl_id, title_ctrl_id, &title_height);
            
            CTRLLABEL_SetAlign(title_ctrl_id, GUILABEL_ALIGN_MIDDLE);
            CTRLLABEL_SetFont(title_ctrl_id, title_font_size, title_font_color);
            CTRLLABEL_SetVisible(title_ctrl_id, TRUE, TRUE);
            
            title_utf8_len = strlen(p_render_card->listcard->title[j]);
            title_wstr = DUER_CALLOC(1, (title_utf8_len + 1)*sizeof(wchar));
            if (title_wstr)
            {
                GUI_UTF8ToWstr(title_wstr, title_utf8_len, (const uint8 *)p_render_card->listcard->title[j], title_utf8_len);
                title_wshowtxt.wstr_ptr = title_wstr, title_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(title_wstr);
                CTRLLABEL_SetText(title_ctrl_id, &title_wshowtxt, TRUE);
                DUER_FREE(title_wstr);
            }

            // 设置text控件的content参数
            content_ctrl_id = MMI_DUERAPP_RENDER_CARD_LIST_CONTENT1_CTRL_ID + j;
            CTRLFORM_SetChildWidth(child_form_ctrl_id, content_ctrl_id, &content_width);
            CTRLFORM_SetChildHeight(child_form_ctrl_id, content_ctrl_id, &content_height);
            
            CTRLLABEL_SetAlign(content_ctrl_id, GUILABEL_ALIGN_MIDDLE);
            CTRLLABEL_SetFont(content_ctrl_id, content_font_size, content_font_color);
            CTRLLABEL_SetVisible(content_ctrl_id, TRUE, TRUE);
            
            content_utf8_len = strlen(p_render_card->listcard->content[j]);
            content_wstr = DUER_CALLOC(1, (content_utf8_len + 1)*sizeof(wchar));
            if (content_wstr)
            {
                GUI_UTF8ToWstr(content_wstr, content_utf8_len, (const uint8 *)p_render_card->listcard->content[j], content_utf8_len);
                content_wshowtxt.wstr_ptr = content_wstr, content_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(content_wstr);
                CTRLLABEL_SetText(content_ctrl_id, &content_wshowtxt, TRUE);
                DUER_FREE(content_wstr);
            }
        }
    }
}

LOCAL void UpdateDirectiveCard(duerapp_render_card_t *p_render_card)
{
    char *content = NULL;
    int utf8_len = 0;
    wchar *wch_txt = NULL;
    MMI_STRING_T wch_dst_ptr = {0}, wch_src1_ptr = {0}, wch_src2_ptr = {0};
    uint32 img_w = 0, img_h = 0;

    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    if (!p_render_card)
    {
        DUER_LOGI("(%s)[duer_watch]:params error", __FUNCTION__);
        return;
    }
    
    switch(p_render_card->type) {
        case DUER_CARD_TYPE_TEXT:
        {
            if (p_render_card->textcard == NULL) {
                DUER_LOGI("(%s)[duer_watch]:params error", __FUNCTION__);
                break;
            }
            content = p_render_card->textcard->content;
            
            if(content == NULL) {
                DUER_LOGI("(%s)[duer_watch]:params error", __FUNCTION__);
                break;
            }
            
            {
                utf8_len = strlen(content);
                wch_txt = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
                duerapp_close_waiting();
                UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
                
                if (wch_txt) {                    
                    GUI_UTF8ToWstr(wch_txt, utf8_len, (const uint8 *)content, utf8_len);
                    wch_src1_ptr.wstr_ptr = wch_txt;
                    wch_src1_ptr.wstr_len = MMIAPICOM_Wstrlen(wch_src1_ptr.wstr_ptr);
                    
                    wch_src2_ptr.wstr_ptr = L"\n\n";//??????????????????????????
                    wch_src2_ptr.wstr_len = MMIAPICOM_Wstrlen(wch_src2_ptr.wstr_ptr);
                    
                    MMIAPICOM_CatTwoString(&wch_dst_ptr, &wch_src1_ptr, &wch_src2_ptr);
                    RenderCard_UpdateTextCard(wch_dst_ptr.wstr_ptr);
                    DUER_FREE(wch_txt);
                }
            }

            DUER_FREE(p_render_card->textcard->content);
            p_render_card->textcard->content = NULL;
            DUER_FREE(p_render_card->textcard);
            p_render_card->textcard = NULL;
            break;
        }

        case DUER_CARD_TYPE_STANDARD:
        {
            if (p_render_card->standardcard == NULL) {
                DUER_LOGI("(%s)[duer_watch]:params error", __FUNCTION__);
                break;
            }
            
            if (p_render_card->standardcard->content == NULL) {
                DUER_LOGI("(%s)[duer_watch]:params error", __FUNCTION__);
                break;
            }
            
            duerapp_close_waiting();
            UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
            RenderCard_UpdateStandardCard(p_render_card, FALSE);
            
            if (p_render_card->standardcard->image_url) {                
                s_standcard_img_is_check = duerapp_Image_FromUrl_GetWh((const uint8 *)p_render_card->standardcard->image_url, &img_w, &img_h);
                DUER_LOGI("(%s)[duer_watch]:img check %s, w(%d), h(%d)", __FUNCTION__, s_standcard_img_is_check?"success":"failed", img_w, img_h);
                if (img_w > 1024
                    || img_h > 1024) {//??????1024*1024
                    MMK_SendMsg(MMI_DUERAPP_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
                    break;
                }
                RenderCard_StandardCardResCfg(p_render_card);
            
            #if CONFIG_SHOW_CARD_PIC
                imageStartDownResource(IMG_DOWN_RENDER_STANDARDCARD);
                RenderCard_StandardCardTimerStart();
            #else
                MMK_SendMsg(MMI_DUERAPP_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
            #endif
            } else {
                DUER_LOGI("(%s)[duer_watch]:no image in render card", __FUNCTION__);
                MMK_SendMsg(MMI_DUERAPP_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
            }
            break;
        }

        case DUER_CARD_TYPE_LIST:
        {
            duerapp_close_waiting();
            WatchRec_MainwinDrawBG();//clear
            UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
            RenderCard_UpdateListCard(p_render_card);
            RenderCard_ListCardResCfg(p_render_card);

            #if CONFIG_SHOW_CARD_PIC
                imageStartDownResource(IMG_DOWN_RENDER_LISTCARD);
            #endif

            break;
        }

        default:
        {
            break;
        }
    }
    
}

PUBLIC void MMIDUERAPP_WatchRequireLogin(baidu_json *payload) {
    duerapp_user_login();
}

PUBLIC void MMIDUERAPP_ScreenRenderCardEvent(baidu_json *payload) 
{
    char *image_src_url = NULL;
    char *image_click_link_url = NULL;
    // MMI_STRING_T toast_str = {.wstr_ptr = L"δ????????",};
    MMI_STRING_T toast_str;
    char *str = NULL;
    baidu_json *type = NULL;
    char *type_ptr = NULL;
    char *list_content = NULL;
    duerapp_render_card_t render_card = {0};
    baidu_json *list_item = NULL;
    baidu_json *content_item = NULL;
    baidu_json *title_item = NULL;
    baidu_json *url_item = NULL;
    char *title = NULL;
    char *content = NULL;
    int temp_len = 0;
    baidu_json *list_array = NULL;
    int len = 0;
    char *temp_ptr = NULL;
    baidu_json *image = NULL;
    baidu_json *src = NULL;
    char *tmp_str = NULL;
    baidu_json *content_json = NULL;
    // char *content = NULL;
    // baidu_json *image = NULL;
    baidu_json *image_click_link = NULL;
    // baidu_json *src = NULL;
    char *src_url = NULL;
    baidu_json *url = NULL;
    char *link_url = NULL;
    int i = 0;
    BOOLEAN is_information = FALSE;

    toast_str.wstr_ptr = L"未获取到信息";

    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);

    str = baidu_json_PrintBuffered(payload, JSON_PREBUFF_SIZE_128, 0);
    if (str) 
    {
        duer_print_long_str(str);
        baidu_json_release(str);
    }

    { //?ж???????????listcard????????
        baidu_json *pre_link = baidu_json_GetObjectItem(payload, "link");
        if (pre_link) {
            is_information = TRUE;
        }
    }

    type = baidu_json_GetObjectItem(payload,"type");
    type_ptr = type->valuestring;
    DUER_LOGI("(%s)[duer_watch]:type(%s)", __FUNCTION__, type_ptr);
    DUER_LOGI("is_information=%d", is_information);
    if (strcmp(RENCARD_TYPE_LIST_CARD, type_ptr) == 0 && is_information) {
        baidu_json *list_array = baidu_json_GetObjectItem(payload,"list");
        if (list_array) {
            baidu_json *list_item = baidu_json_GetArrayItem(list_array, 0);
            if (list_item) {
                content_json = NULL;
                content = NULL;
                
                if ((content_json = baidu_json_GetObjectItem(list_item, "content")) != NULL) {
                    content = content_json->valuestring;
                } else {
                    content_json = baidu_json_GetObjectItem(list_item, "title");
                    if (content_json) {
                        content = content_json->valuestring;
                    } else {
                        content = "没有找到对应内容呢";
                        DUER_LOGE("(%s)[duer_watch]:content error", __func__);
                    }
                }
                list_content = DUER_CALLOC(1, strlen(content) + 1 + 2);
                strcpy(list_content, content);
                
                render_card.textcard = (duerapp_text_card_t *)DUER_CALLOC(1, sizeof(duerapp_text_card_t));
                
                if (render_card.textcard == NULL) {
                    DUER_LOGE("(%s)[duer_watch]:error", __func__);
                }
                render_card.type = DUER_CARD_TYPE_TEXT;
                render_card.textcard->content = list_content;
            }
        }
    } else if (strcmp(RENCARD_TYPE_LIST_CARD, type_ptr) == 0 && !is_information) {//listcard
        list_array = baidu_json_GetObjectItem(payload,"list");
        len = baidu_json_GetArraySize(list_array);
        
        if (len == 0
            || len >= DUER_LIST_CARD_NUMBER_MAX) {
            toast_str.wstr_ptr = L"卡片数量超出";
            DUER_LOGE("(%s)[duer_watch]:error, list'array max, current'len(%d), max'len(%d)", __func__, len, DUER_LIST_CARD_NUMBER_MAX);
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_MAIN_WIN_ID, MSG_DUERAPP_TOAST, &toast_str, sizeof(MMI_STRING_T));
            return;
        }
        
        DUER_LOGE("(%s)[duer_watch]:list array size(%d), max'len(%d)", __func__, len, DUER_LIST_CARD_NUMBER_MAX);
        render_card.listcard = (duerapp_list_card_t *)DUER_CALLOC(1, sizeof(duerapp_list_card_t));
        
        if (render_card.listcard == NULL) {
            DUER_LOGE("(%s)[duer_watch]:error", __func__);
        }
        
        render_card.type = DUER_CARD_TYPE_LIST;
        render_card.listcard->cnt = len;
        
        for (i = 0;i < len;i++) {           
            list_item = baidu_json_GetArrayItem(list_array,i);
            
            if (list_item) {
                image = baidu_json_GetObjectItem(list_item, "image");
                if (image) {
                    src = baidu_json_GetObjectItem(image, "src");
                    if (src) {
                        tmp_str = src->valuestring;
                        if (tmp_str) {
                            strcpy(render_card.listcard->image_src[i], tmp_str);
                            DUER_LOGI("(%s)[duer_watch]:src_url(%s)", __FUNCTION__, render_card.listcard->image_src[i]);
                        }
                    }
                }
                
                if ((title_item = baidu_json_GetObjectItem(list_item, "title")) != NULL) {
                    tmp_str = title_item->valuestring;
                    if(tmp_str) {
                        strcpy(render_card.listcard->title[i], tmp_str);
                        DUER_LOGI("(%s)[duer_watch]:title(%s)", __FUNCTION__, render_card.listcard->title[i]);
                    }
                }
                
                if ((content_item = baidu_json_GetObjectItem(list_item, "content")) != NULL) {
                    tmp_str = content_item->valuestring;
                    if (tmp_str) {
                        strcpy(render_card.listcard->content[i], tmp_str);
                        DUER_LOGI("(%s)[duer_watch]:content(%s)", __FUNCTION__, render_card.listcard->content[i]);
                    }
                }
                
                if ((url_item = baidu_json_GetObjectItem(list_item, "url")) != NULL) {
                    tmp_str = url_item->valuestring;
                    if (tmp_str) {
                        strcpy(render_card.listcard->url[i], tmp_str);
                        DUER_LOGI("(%s)[duer_watch]:url(%s)", __FUNCTION__, render_card.listcard->url[i]);
                    }
                }
            }
        }
    } else if(strcmp(RENCARD_TYPE_STAND_CARD, type_ptr) == 0) {//standard card
        
        
        
        image = baidu_json_GetObjectItem(payload, "image");
        if (image)
        {
            src = baidu_json_GetObjectItem(image, "src");
            if (src)
            {
                src_url = src->valuestring;
                if (src_url)
                {
                    image_src_url = DUER_CALLOC(1, strlen(src_url) + 1);
                    strcpy(image_src_url, src_url);
                    DUER_LOGI("(%s)[duer_watch]:image_src_url(%s)", __FUNCTION__, image_src_url);
                }
            }
        }
        
        image_click_link = baidu_json_GetObjectItem(payload, "link");
        if (image_click_link)
        {
            url = baidu_json_GetObjectItem(image_click_link, "url");
            if (url)
            {
                link_url = url->valuestring;
                if (link_url)
                {
                    image_click_link_url = DUER_CALLOC(1, strlen(link_url) + 1);
                    strcpy(image_click_link_url, link_url);
                    DUER_LOGI("(%s)[duer_watch]:image_click_link_url(%s)", __FUNCTION__, image_click_link_url);
                }
            }
        }
        
        if ((content_json = baidu_json_GetObjectItem(payload, "content")) != NULL) {
            content = content_json->valuestring;
        } else {
            content = "";
            DUER_LOGE("(%s)[duer_watch]:content error", __func__);
        }
        list_content = DUER_CALLOC(1, strlen(content) + 1 + 2);
        strcpy(list_content, content);
        
        render_card.standardcard = (duerapp_standard_card_t *)DUER_CALLOC(1, sizeof(duerapp_standard_card_t));
        
        if (render_card.standardcard == NULL) {
            DUER_LOGE("(%s)[duer_watch]:error", __func__);
        }
        
        render_card.type = DUER_CARD_TYPE_STANDARD;
        render_card.standardcard->content = list_content;
        render_card.standardcard->image_url = image_src_url;
        render_card.standardcard->image_click_url = image_click_link_url;
        
    } else {//text card
        
        content_json = NULL;
        content = NULL;
        
        if ((content_json = baidu_json_GetObjectItem(payload, "content")) != NULL) {
            content = content_json->valuestring;
        } else {
            content = "";
            DUER_LOGE("(%s)[duer_watch]:content error", __func__);
        }
        list_content = DUER_CALLOC(1, strlen(content) + 1 + 2);
        strcpy(list_content, content);
        
        render_card.textcard = (duerapp_text_card_t *)DUER_CALLOC(1, sizeof(duerapp_text_card_t));
        
        if (render_card.textcard == NULL) {
            DUER_LOGE("(%s)[duer_watch]:error", __func__);
        }
        render_card.type = DUER_CARD_TYPE_TEXT;
        render_card.textcard->content = list_content;
    }
    
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_MAIN_WIN_ID, MSG_DUERAPP_DIRECTIVE_CARD, &render_card, sizeof(duerapp_render_card_t));
}

PUBLIC void MMIDUERAPP_ScreenRenderWeatherEvent(baidu_json *payload) {
    char *msg = NULL;
    wchar *wch_txt = NULL;
    UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
    msg = baidu_json_PrintBuffered(payload, 1024, 0);
    if (msg) {
        int utf8_len = strlen(msg);
        wch_txt = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
        GUI_UTF8ToWstr(wch_txt, utf8_len, (const uint8 *)msg, utf8_len);
        baidu_json_release(msg);
    }
}

PUBLIC void MMIDUERAPP_ScreenRenderPlayInfoEvent(baidu_json *payload) {

    char *miguStr = "咪咕音乐";
    baidu_json *content = NULL;
    baidu_json *titleJson = NULL;
    baidu_json *subTitleJson = NULL;
    baidu_json *popupJson = NULL;
    baidu_json *urlJson = NULL;
    baidu_json *durationJson = NULL;
    baidu_json *artJson = NULL;
    baidu_json *artSrcJson = NULL;
    baidu_json *paymentJson = NULL;
    baidu_json *membershipJson = NULL;
    baidu_json *audioTypeJson = NULL;
    baidu_json *providerJson = NULL;
    baidu_json *miguJson = NULL;

    char *title = NULL;
    char *subTitle = NULL;
    char *url = NULL;
    int32 str_title_len = 0;
    int32 str_subtitle_len = 0;
    int32 str_url_len = 0;
    BOOLEAN need_timer = FALSE;
    int duration = 60000;   //60000 ms by default
    static int playing_cnt = 0;
    BOOLEAN is_migu_music = FALSE;
    RENDER_PLAYER_INFO *render_player_info = DUER_CALLOC(1, sizeof(RENDER_PLAYER_INFO));
    int32 src_url_len = 0;

    content = baidu_json_GetObjectItem(payload, "content");
    if (!content) {
        DUER_LOGW("missing content from RenderPlayInfo json");
      return;
    }
    audioTypeJson = baidu_json_GetObjectItem(content, "audioItemType");
    titleJson = baidu_json_GetObjectItem(content, "title");
    subTitleJson = baidu_json_GetObjectItem(content, "titleSubtext1");
    popupJson = baidu_json_GetObjectItem(content, "popupInfo");
    artJson = baidu_json_GetObjectItem(content, "art");
    title = (titleJson && titleJson->valuestring) ? titleJson->valuestring : "";
    subTitle = (subTitleJson && subTitleJson->valuestring) ? subTitleJson->valuestring : "";
    if (audioTypeJson && audioTypeJson->valuestring) {
        DUER_LOGI("RenderPlayInfo audio-type->%s", audioTypeJson->valuestring);
    }
    providerJson = baidu_json_GetObjectItem(content, "provider");
    if (providerJson) {
        miguJson = baidu_json_GetObjectItem(providerJson, "name");
        if (miguJson && miguJson->valuestring) {
            is_migu_music = (strcasecmp(miguStr, miguJson->valuestring) == 0);
        }
    }

    if (popupJson) {
        urlJson = baidu_json_GetObjectItem(popupJson, "url");
        url = (urlJson && urlJson->valuestring) ? urlJson->valuestring : "";
        DUER_LOGI("RENDER_PLAYER_INFO URL->%s",url);
    }

    if (!render_player_info) {
        DUER_LOGW("alloc memory for RENDER_PLAYER_INFO failed");
       return;
    }

    str_title_len = strlen((char*)title);
        if (is_migu_music) {
        render_player_info->title = DUER_CALLOC(1, str_title_len + strlen(miguStr) + 1);
        if (!render_player_info->title) {
            DUER_LOGW("alloc memory for RENDER_PLAYER_INFO.title failed");
            free_render_player(&render_player_info);
            return;
        }
        sprintf(render_player_info->title, "%s-%s", title, miguStr);
        } else {
        render_player_info->title = DUER_CALLOC(1, str_title_len + 1);
        if (!render_player_info->title) {
            DUER_LOGW("alloc memory for RENDER_PLAYER_INFO.title failed");
            free_render_player(&render_player_info);
            return;
        }
        strcpy(render_player_info->title, title);
        }

    str_subtitle_len = strlen((char*)subTitle);
    render_player_info->subtitle = DUER_CALLOC(1, str_subtitle_len + 1);
    if (!render_player_info->subtitle) {
        DUER_LOGW("alloc memory for RENDER_PLAYER_INFO.subtitle failed");
        free_render_player(&render_player_info);
        return;
    }
    strcpy(render_player_info->subtitle,subTitle);

    //Get "popupInfo"->"url":"dueros:\/\/cc33b657-eacb-2b9e-4f7c-e2fe002b1327\/buyXiaoduWatchVip?source=audio_music"
    //
    //Get "payment"->"membershipType":"NO_VIP"
    //Get "mediaLengthInMilliseconds":"60"

    if (url) {
        str_url_len = strlen((char*)url);
        if (str_url_len > 0) {
            render_player_info->url = DUER_CALLOC(1, str_url_len + 1);
            if (!render_player_info->url) {
                DUER_LOGW("alloc memory for RENDER_PLAYER_INFO.url failed");
                free_render_player(&render_player_info);
                return;
        }
            strcpy(render_player_info->url,url);
        }
    }

    if (artJson) {
        artSrcJson = baidu_json_GetObjectItem(artJson, "src");
        if (artSrcJson && artSrcJson->valuestring) {
            char *src_url = artSrcJson->valuestring;
            DUER_LOGI("RENDER_PLAYER_INFO ART SRC URL->%s", src_url);

            src_url_len = strlen(src_url);
            if (src_url_len > 0) {
                render_player_info->art_src_url = DUER_CALLOC(1, src_url_len + 1);
                if (!render_player_info->art_src_url) {
                    DUER_LOGW("alloc memory for RENDER_PLAYER_INFO.art_src_url failed");
                    free_render_player(&render_player_info);
                    return;
                }
                strcpy(render_player_info->art_src_url, src_url);
            }
        }
    }

    free_render_player(NULL);
    s_renderPlayerInfo = render_player_info;
    s_user_novip = FALSE;
    paymentJson = baidu_json_GetObjectItem(content, "payment");
    if (paymentJson) {
        membershipJson = baidu_json_GetObjectItem(paymentJson, "membershipType");
        if (membershipJson && membershipJson->valuestring && strcasecmp(membershipJson->valuestring, "NO_VIP") == 0) {
            need_timer = TRUE;
            s_user_novip = TRUE;
        }
    }

    if (need_timer) {
        durationJson = baidu_json_GetObjectItem(content, "mediaLengthInMilliseconds");
        if (durationJson) {
            if (durationJson->valueint > 0) {
                duration = durationJson->valueint;
            }
        }
        duer_try_playing_timer_create(duration, &playing_cnt);
    } else {
        duer_try_playing_timer_delete();
    }


    DUER_LOGI("%s", __FUNCTION__);

    MMK_duer_other_task_to_MMI(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_DUERAPP_MUSIC_UPDATE, PNULL, 0);
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID,MSG_DUERAPP_PLAYER_INFO_CARD, PNULL, 0);
}

PUBLIC void free_render_player(RENDER_PLAYER_INFO **p_render_player_info)
{
    RENDER_PLAYER_INFO *render_player_info = NULL;
    // if not specified, free static one
    if (!p_render_player_info) {
        p_render_player_info = &s_renderPlayerInfo;
    }
    render_player_info = *p_render_player_info;

    if (render_player_info) {
        if (render_player_info->title != NULL) {
            DUER_FREE(render_player_info->title);
            render_player_info->title = NULL;
        }
        if (render_player_info->subtitle != NULL) {
            DUER_FREE(render_player_info->subtitle);
            render_player_info->subtitle = NULL;
        }
        if (render_player_info->url != NULL) {
            DUER_FREE(render_player_info->url);
            render_player_info->url = NULL;
        }
        if (render_player_info->art_src_url != NULL) {
            DUER_FREE(render_player_info->art_src_url);
            render_player_info->art_src_url = NULL;
        }

        DUER_FREE(render_player_info);
        DUER_LOGI("DUER_FREE RENDER_PLAYER_INFO...");
    }
    *p_render_player_info = NULL;
}

/*****************************************************************************/

LOCAL void HandleWinTimer(uint8 timer_id)
{
    if(s_timeout_timer == timer_id)
    {
        UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
    }
}

LOCAL MMI_RESULT_E HandleDuerMainWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          )
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    char *input_text = NULL;
//    SCI_TraceLow("(%s)(%d)[duer_watch]:win_id(0x%0lx), msg_id(0x%0x)", __func__, __LINE__, win_id, msg_id);
    duerapp_render_card_t *p_render_card = NULL;
    img_down_param_t *pImgDownData = NULL;
    MMI_STRING_T *toast_str = NULL;
    dictionary_mmk_msg_info_t *mmk_ptr = NULL;

    DUER_LOGI("(%s)(%d)[duer_watch]:win_id(0x%0lx), msg_id(0x%0x)", __func__, __LINE__, win_id, msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        DUER_LOGI("MSG_OPEN_WINDOW open");
        WatchRec_MainwinDrawBG();
        MainWinFormParam(win_id);
        GUIBUTTON_SetVisible(MMI_DUERAPP_BUTTON_CTRL_ID, TRUE, FALSE);
        UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
        RenderCard_ParamInit();
        if (s_interact_mode == DUERAPP_INTERACT_MODE_DICTIONARY) {
            duer_statistics_time_count_start(STATISTICS_TIME_TYPE_DICT);
        }
        break;

    case MSG_LCD_SWITCH:
        break;
    case MSG_TIMER:
    {
        if(MMK_IsFocusWin(win_id))
        {
            HandleWinTimer(*((uint8*)param));
        }
        break;
    }
    case MSG_FULL_PAINT:
        WatchRec_MainwinDrawBG();
        DUER_LOGI("MSG_FULL_PAINT:s_init_status(%d), s_auto_listen(%d)", s_init_status, s_auto_listen);
        if (s_init_status && s_auto_listen) {
            s_auto_listen = FALSE;
            DuerButtonCallBack();
        } else {
            UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
        }
        break;
    case MSG_DUERAPP_PLAYER_INFO_CARD:
        UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
        break;
    case MSG_DUERAPP_INPUT_SCREEN:
    {
        input_text  = (char*)param;
        if (input_text != PNULL) 
        {
            UpdateInputScreenText(input_text);
        }
        break;
    }
    case MSG_DUERAPP_AUTH_STATUS:
        break;
    case MSG_DUERAPP_DIRECTIVE_CARD:
    {
        p_render_card = (duerapp_render_card_t *)param;
        if (p_render_card != PNULL) 
        {
            UpdateDirectiveCard(p_render_card);
        }
        break;
    }
    case MSG_DUERAPP_ANIM_PIC_UPDATE:
    {
        pImgDownData = (img_down_param_t *)param; 

        if (pImgDownData == NULL) {
            DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
            break;
        }
        DUER_LOGI("(%s)[duer_watch]:anim picture update, type(%d), index(%d)", __FUNCTION__, pImgDownData->type, pImgDownData->index);
        
        if (pImgDownData->type == IMG_DOWN_RENDER_STANDARDCARD) {
            RenderCard_StandardCardFree(DUER_STANDARDCARD_TYPE_SRC);
            RenderCard_UpdateStandardCard(s_render_card, TRUE);
        } else if (pImgDownData->type == IMG_DOWN_RENDER_LISTCARD) {
            if (s_render_card && s_render_card->listcard) {
                s_render_card->listcard->index++;
                RenderCard_UpdateListCardAnim(pImgDownData->index);
                DebugimageMMIDealDownCbMsg(win_id, param);
            } else {
                DUER_LOGI("(%s)[duer_watch]:error list card", __FUNCTION__);
            }
        }
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        break;
    }
    case MSG_DUERAPP_DUER_UPDATE:
        DUER_LOGI("MSG_DUERAPP_DUER_UPDATE");
        if (param != NULL) {
            UpdateDuerAsr(*((int*)param));
        }
        break;
    case MSG_APP_OK:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        DUER_LOGI("MSG_CLOSE_WINDOW");
        if (s_interact_mode == DUERAPP_INTERACT_MODE_DICTIONARY) {
            duer_statistics_time_count_stop(STATISTICS_TIME_TYPE_DICT);
        }

        RenderCard_ParamDestory();
        s_record_sate = 0;
        s_duer_state = MMIDUERAPP_ANIMATE_STATE_THINKING;
        MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        if (MMIDUERAPP_GetInitStatus()) {
            duer_asr_stop();
            duer_ext_stop_speak();
            duer_dcs_close_multi_dialog();
            duer_dcs_dialog_cancel();
        }

        MMIDUERAPP_StartNormalAsrSet();
        break;
    case MSG_GET_FOCUS: 
    {
        DUER_LOGI("MSG_GET_FOCUS");
        duerapp_set_interact_mode(s_interact_mode); //GET FOCUS?????????????DIDP
    } break;
    case MSG_LOSE_FOCUS: {
        DUER_LOGD("MSG_LOSE_FOCUS");
        //LOSE FOCUS?????????mode???????normal DIDP
        if (DUERAPP_INTERACT_MODE_NORMAL != s_interact_mode) {
            duer_set_didp_version(DIDP_VERSION_NORMAL); //DIDP version for normal voice interactive mode
            duer_update_user_agent();
        }

        if (s_record_sate == 1) {
            duer_asr_stop();
            DUER_LOGD("ASR stop!!!");
            s_record_sate = 0;
        }
    } break;
    case MSG_DUERAPP_TOAST:
    {
        toast_str = (MMI_STRING_T *)param;
        if(toast_str) {
            duerapp_show_toast(toast_str->wstr_ptr);
        }
        break;
    }
    case MSG_DUERAPP_DICT_RENDER_DICTSEL:
    {
        mmk_ptr = (dictionary_mmk_msg_info_t *)param;
        if (mmk_ptr) {
            PUB_DUERAPP_DictionaryCardWinOpen(DUERAPP_DICT_RENDER_DICTSEL, mmk_ptr);
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __func__, __LINE__);
        }
        break;
    }
    case MSG_DUERAPP_DICT_RENDER_NORMAL:
    {
        mmk_ptr = (dictionary_mmk_msg_info_t *)param;
        if (mmk_ptr) {
            PUB_DUERAPP_DictionaryCardWinOpen(DUERAPP_DICT_RENDER_NORMAL, mmk_ptr);
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __func__, __LINE__);
        }
        break;
    }
    case MSG_DUERAPP_DICT_RENDER_EMPTY:
    {
        duerapp_show_toast(L"去问小度助手吧");
        break;
    }
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

PUBLIC void MMIDUERAPP_MainSetInitStatus(int status)
{
    DUER_LOGI("%s status(%d)", __func__, status);
    s_init_status = status;
    return;
}

LOCAL void NetworkErrorTips(void) {
     wchar *wstr_ptr = L"网络出错了";
     duerapp_show_toast(wstr_ptr);
}

/*****************************************************************************/
LOCAL MMI_RESULT_E HandleRenderCardWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    MMI_CTRL_ID_T       ctrl_id = MMI_DUERAPP_TEXTBOX_CTRL_ID;
    wchar *content = PNULL;

    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    GUI_BG_T bg = {0};

    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;
    
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        content = (wchar*) MMK_GetWinAddDataPtr(win_id);
        if (content == PNULL) {
            MMK_CloseWin(win_id);
            return MMI_RESULT_FALSE;
        }  
        MMK_SetAtvCtrl(win_id, ctrl_id);
        font_size = WATCH_DEFAULT_NORMAL_FONT;
        font_color = MMI_WHITE_COLOR;
        // bg = {0};
        bg.bg_type = GUI_BG_COLOR;
        bg.color = MMI_BLACK_COLOR;
        GUITEXT_SetBg(ctrl_id, &bg);
        GUITEXT_SetFont(ctrl_id, &font_size, &font_color);
        GUITEXT_SetString(ctrl_id , content, MMIAPICOM_Wstrlen(content), TRUE);
        break;

    case MSG_GET_FOCUS:
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        DUER_FREE(content);
        break;

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
}

LOCAL MMI_RESULT_E HandleWaitingWinMsg(
                                       MMI_WIN_ID_T       win_id,     // ?????ID
                                       MMI_MESSAGE_ID_E   msg_id,     // ???????????ID
                                       DPARAM             param       // ???????????
                                       )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    DUER_LOGI("HandleWaitingWinMsg-->msg_id=%s",msg_id);
    switch (msg_id)
    {
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_APP_CANCEL:
    case MSG_LOSE_FOCUS:
        MMK_CloseWin(win_id);
        break;
    default:
        result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, param);
        break;
    }

    return (result);
}

PUBLIC void MMIDUERAPP_DuerAsrState(duer_event_t *event)
{
    int code = 0;
    switch (event->_event) {
    case DUER_EVENT_STARTED:
        break;
    case DUER_EVENT_STOPPED:
        break;
    case DUER_EVENT_ASR_FINISHED:
    {
        code = event->_code;
    #if (USE_DUERINPUT == 1)
        if (DuerInput_IsWorkingNow()) 
        {
            DUER_LOGI("voice'win, asr code->%d",code);
            MMIAPICOM_OtherTaskToMMI(MMI_DUERINPUT_VOICE_WIN_ID,MSG_DUERAPP_DUER_UPDATE,&code,sizeof(int));
        }
        else 
    #endif
#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)
        if (PUB_DUERAPP_MessageSelectWinIsOpen())//parents assists
        {
            DUER_LOGI("assists'win, asr code->%d",code);
            MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_VIP_MESSAGE_SELECT_WIN_ID,MSG_DUERAPP_DUER_UPDATE,&code,sizeof(int));
        }
        else
#endif
        {
            DUER_LOGI("main'win, asr code->%d",code);
            MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_MAIN_WIN_ID,MSG_DUERAPP_DUER_UPDATE,&code,sizeof(int));
        }
        break;
    }
    default:
        break;
    }
}

LOCAL void UpdateDuerAsr(int code)
{
    DUER_LOGI("DUER_ASR code: %d\r\n", code);
    UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
    s_record_sate = 0;
    if (code < 0) {
        NetworkErrorTips();
    }
}

LOCAL BOOLEAN duerapp_interpreter_is_allowed_use(void)
{
    static unsigned int gs_interpreter_try_cnt = 0;
    BOOLEAN is_login = duerapp_is_login();
    DUER_USER_INFO *info = duerapp_get_user_info();
    BOOLEAN is_vip = (info && info->is_vip);

    if (is_login && is_vip) {
        gs_interpreter_try_cnt = 0;
        DUER_LOGI("%s, is vip with login", __FUNCTION__);
    } else {
        if (gs_interpreter_try_cnt < INTERPRETER_TRY_USE_CNT) {
            gs_interpreter_try_cnt++;
            DUER_LOGI("%s, is try", __FUNCTION__);
        } else {
            DUER_LOGI("%s, can not try", __FUNCTION__);
            return FALSE;
        }
    }
    return TRUE;
}

/*****************************************************************************/

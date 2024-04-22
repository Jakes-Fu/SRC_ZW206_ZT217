// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_homepage.c
 * Auth: Gu Yahui (guyahui@baidu.com)
 * Desc: duerapp home page.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2021        Gu Yahui          Create                                   *
******************************************************************************/

/**--------------------------------------------------------------------------*/
/**                         Include Files                                    */
/**--------------------------------------------------------------------------*/
#include "duerapp_homepage.h"
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
#include "guisetlist.h"
#include "duerapp_slide_indicator.h"

#include "dal_time.h"
#include "duerapp.h"
#include "mmicom_time.h"
#include "duerapp_common.h"
#include "duerapp_login.h"
#include "duerapp_qrwin.h"
#include "duerapp_http.h"
#include "duerapp_statistics.h"
#include "duerapp_pay.h"
#include "duerapp_local_param.h"
#include "duerapp_operate.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_audio_play_utils.h"
#include "duerapp_center.h"
#include "duerapp_writing.h"
#include "duerapp_res_down.h"
#include "duerapp_openapi.h"
#include "duerapp_dict_activity.h"
#include "duerapp_interpreter_activity.h"
#include "duerapp_operate_comqr.h"
#include "duerapp_oauth_request.h"
#include "duerapp_mplayer.h"

#include "baidu_json.h"
#include "lightduer_log.h"
#include "lightduer_debug.h"
#include "lightduer_random.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_dcs.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"
#include "lightduer_lib.h"
#include "lightduer_audio_codec_ops.h"
#include "lightduer_audio_adapter.h"
#include "lightduer_audio_player.h"
#include "lightduer_audio_codec_adapter.h"
#include "lightduer_profile.h"

/**--------------------------------------------------------------------------*/
/**                         MACRO DEFINITION                                 */
/**--------------------------------------------------------------------------*/

#define DUERAPP_AGREE_RECT              {25,175,60,215}
#define DUERAPP_PROTOCOL_RECT           {70,180,230,210}
#define DUERAPP_START_RECT              {50,120,190,160}

#define DUERAPP_TIMEOUT_TIME            5000
#define DUERAPP_VOLUME_LENGTH           10
#define DUERAPP_CENTER_WIN_BG           RGB8882RGB565(0x000000)
#define DUERAPP_NETWORK_SUBTITLE_COLOR  RGB8882RGB565(0x666666)

#define DUERAPP_DEFAULT_LOAD_TXT        L"加载中"

#define DUERAPP_WIN_ID_FORM_CHILD_BANNER(_form, _n) \
                                    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_HOME_BANNER_FORM##_n##_CTRL_ID, _form), \
                                    CHILD_ANIM_CTRL(FALSE, TRUE, MMI_DUERAPP_HOME_BANNER_ANIM##_n##_CTRL_ID, MMI_DUERAPP_HOME_BANNER_FORM##_n##_CTRL_ID), \
                                    CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_HOME_BANNER_LABEL##_n##_CTRL_ID, _form)
                               
                               
#define DUERAPP_WIN_ID_FORM_CHILD_ITEM(_form, _n) \
                                    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_HOME_SKILL_ICON_FORM##_n##_CTRL_ID, _form), \
                                    CHILD_ANIM_CTRL(FALSE, TRUE, MMI_DUERAPP_HOME_SKILL_ANIM##_n##_1_CTRL_ID, MMI_DUERAPP_HOME_SKILL_ICON_FORM##_n##_CTRL_ID), \
                                    CHILD_ANIM_CTRL(FALSE, TRUE, MMI_DUERAPP_HOME_SKILL_ANIM##_n##_2_CTRL_ID, MMI_DUERAPP_HOME_SKILL_ICON_FORM##_n##_CTRL_ID), \
                                    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_HOME_SKILL_LABEL_FORM##_n##_CTRL_ID, _form), \
                                    CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_HOME_SKILL_LABEL##_n##_1_CTRL_ID, MMI_DUERAPP_HOME_SKILL_LABEL_FORM##_n##_CTRL_ID), \
                                    CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, FALSE, MMI_DUERAPP_HOME_SKILL_LABEL##_n##_2_CTRL_ID, MMI_DUERAPP_HOME_SKILL_LABEL_FORM##_n##_CTRL_ID)

#define MMI_DUERAPP_SKILL_ITEM1(img_id, hide, txt, cb, _n) \
                                { \
                                    img_id, hide, \
                                    MMI_DUERAPP_HOME_SKILL_ICON_FORM##_n##_CTRL_ID, \
                                    MMI_DUERAPP_HOME_SKILL_ANIM##_n##_1_CTRL_ID, \
                                    MMI_DUERAPP_HOME_SKILL_LABEL_FORM##_n##_CTRL_ID, \
                                    MMI_DUERAPP_HOME_SKILL_LABEL##_n##_1_CTRL_ID, \
                                    txt, cb, MMI_DUERAPP_HOME_SKILL_ANIM##_n##_1_CTRL_ID \
                                }
#define MMI_DUERAPP_SKILL_ITEM2(img_id, hide, txt, cb, _n) \
                                { \
                                    img_id, hide, \
                                    MMI_DUERAPP_HOME_SKILL_ICON_FORM##_n##_CTRL_ID, \
                                    MMI_DUERAPP_HOME_SKILL_ANIM##_n##_2_CTRL_ID, \
                                    MMI_DUERAPP_HOME_SKILL_LABEL_FORM##_n##_CTRL_ID, \
                                    MMI_DUERAPP_HOME_SKILL_LABEL##_n##_2_CTRL_ID, \
                                    txt, cb, MMI_DUERAPP_HOME_SKILL_ANIM##_n##_2_CTRL_ID \
                                }

#define MMI_DUERAPP_SKILL_BANNER(hide, img_id, _w, _h, txt, cb, _n) \
                                { \
                                    hide,\
                                    MMI_DUERAPP_HOME_BANNER_FORM##_n##_CTRL_ID, MMI_DUERAPP_HOME_BANNER_ANIM##_n##_CTRL_ID, \
                                    MMI_DUERAPP_HOME_BANNER_LABEL##_n##_CTRL_ID, _w, _h, \
                                    img_id, txt, \
                                    cb, (void*)MMI_DUERAPP_HOME_BANNER_ANIM##_n##_CTRL_ID \
                                }

/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          CONSTANT ARRAY                                   */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*/
/**                         EXTERNAL DECLARE                                 */
/**--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*/
/**                         STATIC DEFINITION                                */
/**--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
LOCAL MMI_RESULT_E HandleDuerHomeWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          );

LOCAL MMI_RESULT_E HandleDuerTestWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          );


LOCAL MMI_RESULT_E HandleWelcomeWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           );

LOCAL MMI_RESULT_E HandleProtocolWinMsg(
                                            MMI_WIN_ID_T     win_id,
                                            MMI_MESSAGE_ID_E msg_id,
                                            DPARAM           param
                                          );
LOCAL MMI_RESULT_E HandleDuerNetworkWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          );
/*****************************************************************************/

LOCAL MMI_HANDLE_T MMIDUERAPP_CreateWelcomeWin(void);
LOCAL MMI_HANDLE_T MMIDUERAPP_CreateNoNetworkWin(void);

LOCAL void AddItemProtocolImgRes(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id);
LOCAL MMI_HANDLE_T MMIDUERAPP_CreateProtocolWin(void);
LOCAL void MMIDUERAPP_DuerEnter(void);
LOCAL void MMIDUERAPP_CreateDuerHomeWin(void);
LOCAL void MMIDUERAPP_CreateDuerTestWin(void);
LOCAL void WelcomeWinOpenParam(void);
LOCAL void HomeWinFormParam(MMI_WIN_ID_T win_id);
LOCAL void TestWinFormParam(MMI_WIN_ID_T win_id);
LOCAL void NetworkWinFormParam(MMI_WIN_ID_T win_id);

LOCAL void StartButtonCallBack(void);
LOCAL void ProtocolButtonCallBack(void);
LOCAL void MusicDanceButtonCallBack(void);
LOCAL void WatchRec_MainwinDrawBG(MMI_WIN_ID_T win_id);
LOCAL void DUERAPP_FormBG(MMI_WIN_ID_T win_id, MMI_HANDLE_T ctrl_id);
LOCAL void setAnimImgBg(MMI_CTRL_ID_T anim_ctrl_id, GUIANIM_DATA_INFO_T *img_data);
LOCAL duer_draw_skill_item_t* DUERAPP_GetSkillItemViaCtrlId(duer_draw_skill_form_t* form, MMI_CTRL_ID_T id);
LOCAL int32 DUERAPP_GetSkillItemIndexViaCtrlId(duer_draw_skill_form_t* form, MMI_CTRL_ID_T id);
LOCAL int32 DUERAPP_GetBannerIndexViaCtrlId(MMI_CTRL_ID_T id);
LOCAL duer_draw_banner_t* DUERAPP_GetBannerViaCtrlId(MMI_CTRL_ID_T id);
LOCAL duer_draw_skill_form_t* DUERAPP_GetSkillFormViaCtrlId(MMI_CTRL_ID_T id);
LOCAL MMI_RESULT_E DuerSkillAnimCallBack(void *usr_data);
LOCAL MMI_RESULT_E DuerBannerCallBack(void *usr_data);
LOCAL void CalculateHomeFormCnt(void);
LOCAL void NoNetworkFormParam(MMI_WIN_ID_T win_id);
LOCAL void HomeSyncRes(void);
LOCAL void HomeUpdateSkillFormRes(MMI_HANDLE_T form_ctrl_id);
LOCAL void HomeFreeSkillFormRes(void);
LOCAL void StartDuerMainCallBack(void);
LOCAL MMI_RESULT_E StartWithQueryCallBack(void);
LOCAL void NetworkErrorTips(void);
extern PUBLIC void duerapp_res_list_enter(int index);
LOCAL void DrawHomepageListItem( MMI_CTRL_ID_T ctrl_id );
// extern void bdsc_session_asr_set_fun_value(int fun);

// 主窗口
WINDOW_TABLE( MMI_DUERAPP_HOME_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_HOME_WIN_ID ),
    WIN_FUNC((uint32) HandleDuerHomeWinMsg ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_MAIN_FORM_CTRL_ID),
#if DUERAPP_USE_SLIDEPAGE
    CHILD_BUTTON_CTRL(TRUE, IMAGE_DUER_IC_POINT_1_OF_3, MMI_DUERAPP_HOME_SLIDE_POINT_BUTTON_CTRL_ID, MMI_DUERAPP_MAIN_FORM_CTRL_ID),
#endif
    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID, MMI_DUERAPP_MAIN_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(TRUE, IMAGE_DUERAPP_TITLE, MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID, MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(TRUE, IMAGE_NULL, MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID, MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID), // click me to try
    CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, MMI_DUERAPP_MAIN_FORM_CTRL_ID), // tips
    //player
    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, MMI_DUERAPP_MAIN_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(TRUE, IMAGE_DUERAPP_MUSIC_ENTER, MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID, MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID),

    CHILD_FORM_CTRL(TRUE, GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_HOME_FORM_CTRL_ID, MMI_DUERAPP_MAIN_FORM_CTRL_ID),
#if DUERAPP_USE_WATCH_CONFIG
    //banner
    DUERAPP_WIN_ID_FORM_CHILD_BANNER(MMI_DUERAPP_HOME_FORM_CTRL_ID, 1),
    //skill
    CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE1_CTRL_ID, MMI_DUERAPP_HOME_FORM_CTRL_ID),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 1),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 2),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 3),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 4),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 5),

    //banner
    DUERAPP_WIN_ID_FORM_CHILD_BANNER(MMI_DUERAPP_HOME_FORM_CTRL_ID, 2),
    //skill
    CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE2_CTRL_ID, MMI_DUERAPP_HOME_FORM_CTRL_ID),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 6),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 7),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 8),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 9),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 10),

    //banner
    DUERAPP_WIN_ID_FORM_CHILD_BANNER(MMI_DUERAPP_HOME_FORM_CTRL_ID, 3),
    //skill
    CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE3_CTRL_ID, MMI_DUERAPP_HOME_FORM_CTRL_ID),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 11),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 12),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 13),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 14),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 15),

    //banner
    DUERAPP_WIN_ID_FORM_CHILD_BANNER(MMI_DUERAPP_HOME_FORM_CTRL_ID, 4),
    //skill
    CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE4_CTRL_ID, MMI_DUERAPP_HOME_FORM_CTRL_ID),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 16),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 17),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 18),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 19),
    DUERAPP_WIN_ID_FORM_CHILD_ITEM(MMI_DUERAPP_HOME_FORM_CTRL_ID, 20),
#else
    CHILD_LIST_CTRL(FALSE, GUILIST_TYPE_NONE, MMI_DUERAPP_HOME_LIST_CTRL_ID, MMI_DUERAPP_MAIN_FORM_CTRL_ID),
#endif
    END_WIN
};

// ??????????????
WINDOW_TABLE( MMI_DUERAPP_NETWORK_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_NETWORK_WIN_ID ),
    WIN_FUNC((uint32) HandleDuerNetworkWinMsg),
    WIN_STYLE(WS_DISABLE_COMMON_BG),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_NETWORK_TITLE_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_NETWORK_SUBTITLE_CTRL_ID),
    END_WIN
};

// ????????
WINDOW_TABLE( MMI_DUERAPP_TEST_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_HOME_TEST_WIN_ID ),
    WIN_FUNC((uint32) HandleDuerTestWinMsg),
    WIN_STYLE(WS_DISABLE_COMMON_BG),
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_HOME_TEST_FORM_CTRL_ID),
    //{
        CHILD_BUTTON_CTRL(TRUE, IMAGE_DUER_IC_POINT_3_OF_3, MMI_DUERAPP_TEST_SLIDE_POINT_BUTTON_CTRL_ID, MMI_DUERAPP_HOME_TEST_FORM_CTRL_ID),
        CHILD_TEXT_CTRL(FALSE, MMI_DUERAPP_HOME_TEST_TXT_CTRL_ID, MMI_DUERAPP_HOME_TEST_FORM_CTRL_ID),
        CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID, MMI_DUERAPP_HOME_TEST_FORM_CTRL_ID),
        //{
            CHILD_BUTTON_CTRL(TRUE, IMAGE_NULL, MMI_DUERAPP_TEST_ENGI_1_BUTTON_CTRL_ID, MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID),
            CHILD_BUTTON_CTRL(TRUE, IMAGE_NULL, MMI_DUERAPP_TEST_ENGI_2_BUTTON_CTRL_ID, MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID),
        //}//MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID
    //}MMI_DUERAPP_HOME_TEST_FORM_CTRL_ID
    END_WIN
};

// ??ν?????
WINDOW_TABLE( MMI_DUERAPP_WELCOME_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_WELCOME_WIN_ID ),
    WIN_FUNC((uint32) HandleWelcomeWinMsg ), 
    WIN_STYLE(WS_DISABLE_COMMON_BG),
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_START,MMI_DUERAPP_START_BUTTON_CTRL_ID),
    CREATE_CHECKBOX_CTRL(MMI_DUERAPP_PROTOCOL_CHECKBOX_CTRL_ID),
    // CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMI_DUERAPP_PROTOCOL_CHECKBOX_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID),
    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

// ???Э?鴰??
WINDOW_TABLE( MMI_DUERAPP_PROTOCOL_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_PROTOCOL_WIN_ID ),
    WIN_FUNC((uint32) HandleProtocolWinMsg ),    
    CREATE_RICHTEXT_CTRL(MMI_DUERAPP_PROTOCOL_RICHTEXT_CTRL_ID),
    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

#if DUERAPP_USE_WATCH_CONFIG
/*
 *    MMI_DUERAPP_SKILL_ITEM1     MMI_DUERAPP_SKILL_ITEM2
 *--------------------------------------------------------
 *              item1_1                 item1_2
 *              item2_1                 item2_2
 *               ...                      ...
*/
LOCAL duer_draw_skill_item_t home_skill1_items[] = {
    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 1),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 1),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 2),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 2),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 3),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 3),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 4),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 4),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 5),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 5),

};

LOCAL duer_draw_skill_item_t home_skill2_items[] = {
    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 6),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 6),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 7),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 7),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 8),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 8),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 9),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 9),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 10),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 10),

};

LOCAL duer_draw_skill_item_t home_skill3_items[] = {
    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 11),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 11),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 12),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 12),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 13),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 13),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 14),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 14),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 15),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 15),

};

LOCAL duer_draw_skill_item_t home_skill4_items[] = {
    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 16),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 16),
    #if 0
    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 17),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 17),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 18),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 18),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 19),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 19),

    MMI_DUERAPP_SKILL_ITEM1(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 20),
    MMI_DUERAPP_SKILL_ITEM2(IMAGE_DUERAPP_ROUND_ICON_LOAD, FALSE, DUERAPP_DEFAULT_LOAD_TXT, DuerSkillAnimCallBack, 20),
    #endif
};


LOCAL duer_draw_banner_t home_banner1 = MMI_DUERAPP_SKILL_BANNER(FALSE, IMAGE_DUERAPP_BANNER1,
                                                 210, 98, DUERAPP_DEFAULT_LOAD_TXT, DuerBannerCallBack, 1);
LOCAL duer_draw_banner_t home_banner2 = MMI_DUERAPP_SKILL_BANNER(FALSE, IMAGE_DUERAPP_BANNER1,
                                                 210, 98, DUERAPP_DEFAULT_LOAD_TXT, DuerBannerCallBack, 2);
LOCAL duer_draw_banner_t home_banner3 = MMI_DUERAPP_SKILL_BANNER(FALSE, IMAGE_DUERAPP_BANNER1,
                                                 210, 98, DUERAPP_DEFAULT_LOAD_TXT, DuerBannerCallBack, 3);
LOCAL duer_draw_banner_t home_banner4 = MMI_DUERAPP_SKILL_BANNER(FALSE, IMAGE_DUERAPP_BANNER1,
                                                 210, 98, DUERAPP_DEFAULT_LOAD_TXT, DuerBannerCallBack, 4);


LOCAL duer_draw_skill_form_t home_skill1_form = {DUERAPP_DEFAULT_LOAD_TXT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE1_CTRL_ID, 
        DUERAPP_ARRAY_CNT(home_skill1_items), home_skill1_items};
LOCAL duer_draw_skill_form_t home_skill2_form = {DUERAPP_DEFAULT_LOAD_TXT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE2_CTRL_ID, 
        DUERAPP_ARRAY_CNT(home_skill2_items), home_skill2_items};
LOCAL duer_draw_skill_form_t home_skill3_form = {DUERAPP_DEFAULT_LOAD_TXT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE3_CTRL_ID, 
        DUERAPP_ARRAY_CNT(home_skill3_items), home_skill3_items};
LOCAL duer_draw_skill_form_t home_skill4_form = {DUERAPP_DEFAULT_LOAD_TXT, FALSE, MMI_DUERAPP_HOME_SKILL_TITLE4_CTRL_ID, 
        DUERAPP_ARRAY_CNT(home_skill4_items), home_skill4_items};




//TODO: add a lock for thread safety
LOCAL duerapp_draw_t home_draws[8] = {0};
LOCAL unsigned char home_draws_inited = 0U;
#endif

LOCAL int s_run_once = 0;
LOCAL int s_init_status = 0;

LOCAL BOOLEAN duerapp_first_in = FALSE;
LOCAL int s_home_skill_form_cnt = 0;
LOCAL int s_home_banner_cnt = 0;
//LOCAL BOOLEAN s_duerapp_running = FALSE;
LOCAL BOOLEAN s_music_enter_display = FALSE;
LOCAL wchar *s_last_query_text;
LOCAL unsigned int gsa_engi_cnt[2] = {0};

#ifdef DUERAPP_USE_WATCH_CONFIG
extern DUEROS_DISCOVERY_LIST_ITEM_ST    *s_discovery_list;
extern DUEROS_BLOCK_LIST_ITEM_ST        *s_home_block_list;
extern int                              s_home_block_list_size;
extern int                              s_discovery_list_size;
#endif
extern DUEROS_RESOURCE_TYPE_E           s_dueros_audio_type;
extern RENDER_PLAYER_INFO               *s_renderPlayerInfo;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

#if DUERAPP_USE_WATCH_CONFIG
LOCAL void MMIDUERAPP_CreateDraws()
{
    home_draws[0].type = DUER_DRAW_TYPE_BANNER;
    home_draws[0].func = DUERAPP_HomeBannerCreate;
    home_draws[0].draw.banner = &home_banner1;

    home_draws[1].type = DUER_DRAW_TYPE_SKILL_FORM;
    home_draws[1].func = DUERAPP_HomeSkillFormCreate;
    home_draws[1].draw.skill_form = &home_skill1_form;

    home_draws[2].type = DUER_DRAW_TYPE_BANNER;
    home_draws[2].func = DUERAPP_HomeBannerCreate;
    home_draws[2].draw.banner = &home_banner2;

    home_draws[3].type = DUER_DRAW_TYPE_SKILL_FORM;
    home_draws[3].func = DUERAPP_HomeSkillFormCreate;
    home_draws[3].draw.skill_form = &home_skill2_form;

    home_draws[4].type = DUER_DRAW_TYPE_BANNER;
    home_draws[4].func = DUERAPP_HomeBannerCreate;
    home_draws[4].draw.banner = &home_banner3;

    home_draws[5].type = DUER_DRAW_TYPE_SKILL_FORM;
    home_draws[5].func = DUERAPP_HomeSkillFormCreate;
    home_draws[5].draw.skill_form = &home_skill3_form;

    home_draws[6].type = DUER_DRAW_TYPE_BANNER;
    home_draws[6].func = DUERAPP_HomeBannerCreate;
    home_draws[6].draw.banner = &home_banner4;

    home_draws[7].type = DUER_DRAW_TYPE_SKILL_FORM;
    home_draws[7].func = DUERAPP_HomeSkillFormCreate;
    home_draws[7].draw.skill_form = &home_skill4_form;
}
#endif

/*****************************************************************************/
PUBLIC void MMIDUERAPP_CreateWinEnter(void)
{
    duer_debug_hook_level_set(DUER_LOG_VERBOSE);
    if(MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_1))
    {
        if (MMIDUERAPP_GetIsUserAgree())
        {
            MMIDUERAPP_DuerEnter();
        } else {
            MMIDUERAPP_CreateWelcomeWin();
        }
    } else {
        MMIDUERAPP_CreateNoNetworkWin();
    }
}

PUBLIC void duerapp_operate_notify_callback(void)
{
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_OPERATE_CALL_BACK, NULL, 0);
}

/* 调试发现，在login.c里使用MMK_duer_other_task_to_MMI接口发送的MMI_DUERAPP_HOME_WIN_ID，
* 会与homepage.c里的MMI_DUERAPP_HOME_WIN_ID不同，原因待分析 
*/
PUBLIC void duerapp_other_task_to_MMI_home(MMI_MESSAGE_ID_E msg_id,
                                     DPARAM           param_ptr,
                                     uint32           size_of_param
                                     )
{
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID, msg_id, param_ptr, size_of_param);
}
/*****************************************************************************/

PUBLIC int32 DUERAPP_HomeSkillFormCreate(MMI_HANDLE_T ctrl_id, void *arg)
{
    MMI_STRING_T tmp_string = {0};
    GUIFORM_CHILD_WIDTH_T   width = {0};
    GUIFORM_CHILD_HEIGHT_T height = {0};
    uint16 hor_space = 0;
    uint16 ver_space = 0;

    uint16 item_cnt = 0;
    uint16 tmp_cnt = 0;
    GUIFORM_CHILD_DISPLAY_E form_dis = GUIFORM_CHILD_DISP_NORMAL;
    duer_draw_skill_item_t *items = PNULL;
    duer_draw_skill_form_t *form = (duer_draw_skill_form_t*)arg;
    uint16 i = 0;

    if (PNULL == form) {
        return -1;
    }

    {//skill title
    ver_space = form->is_hide ? 0 : 10;
    CTRLFORM_SetChildSpace(ctrl_id, form->tile_ctrl_id, PNULL, &ver_space);
    width.type = GUIFORM_CHILD_WIDTH_FIXED;
    width.add_data = 200;
    GUIFORM_SetChildWidth(ctrl_id,
                form->tile_ctrl_id,
                &width);
    height.type = GUIFORM_CHILD_HEIGHT_FIXED;
    height.add_data = 33;
    CTRLFORM_SetChildHeight(ctrl_id,
                form->tile_ctrl_id,
                &height);
    tmp_string.wstr_ptr = (form->title != PNULL) ? form->title : L"";
    tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
    GUILABEL_SetFont(form->tile_ctrl_id, WATCH_DEFAULT_BIG_FONT, MMI_WHITE_COLOR);
    GUILABEL_SetText(form->tile_ctrl_id, &tmp_string, FALSE);
    form_dis = form->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
    CTRLFORM_SetChildDisplay(ctrl_id, form->tile_ctrl_id, form_dis);
    }

    item_cnt = form->item_cnt;
    tmp_cnt = item_cnt / 2;
    for (i = 0; i < tmp_cnt; i++) {
        GUIANIM_DATA_INFO_T img_info = {0};
        GUIFORM_CHILD_DISPLAY_E dis = GUIFORM_CHILD_DISP_NORMAL;
        items = &form->items[2*i];
        //icon
        if (!items->img_loaded) {
            dis = items->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
            ver_space = items->is_hide ? 0 : 10;
            CTRLFORM_SetChildSpace(ctrl_id, items->img_form_ctrl_id, PNULL, &ver_space);
            width.type = GUIFORM_CHILD_WIDTH_FIXED;
            width.add_data = 90;
            GUIFORM_SetChildWidth(items->img_form_ctrl_id,
                        items->img_ctrl_id,
                        &width);
            height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            height.add_data = 90;
            CTRLFORM_SetChildHeight(items->img_form_ctrl_id,
                    items->img_ctrl_id,
                    &height);
            img_info.img_id = items->img_id;
            CTRLANIM_SetCallBackFunc(items->img_ctrl_id, items->item_click_cb, items->usr_data);
            setAnimImgBg(items->img_ctrl_id, &img_info);
            CTRLFORM_SetChildDisplay(items->img_form_ctrl_id, items->img_ctrl_id, dis);
            CTRLFORM_SetChildDisplay(ctrl_id, items->img_form_ctrl_id, form_dis);
            //text
            ver_space = items->is_hide ? 0 : 5;
            CTRLFORM_SetChildSpace(ctrl_id, items->txt_form_ctrl_id, PNULL, &ver_space);
            width.type = GUIFORM_CHILD_WIDTH_FIXED;
            width.add_data = 90;
            GUIFORM_SetChildWidth(items->txt_form_ctrl_id,
                        items->txt_ctrl_id,
                        &width);
            height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            height.add_data = 28;
            CTRLFORM_SetChildHeight(items->txt_form_ctrl_id,
                    items->txt_ctrl_id,
                    &height);
            tmp_string.wstr_ptr = (items->txt != PNULL) ? items->txt : L"";
            tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
            GUILABEL_SetFont(items->txt_ctrl_id, WATCH_DEFAULT_NORMAL_FONT, MMI_WHITE_COLOR);
            GUILABEL_SetText(items->txt_ctrl_id ,&tmp_string, FALSE);
            CTRLFORM_SetChildDisplay(items->txt_form_ctrl_id, items->txt_ctrl_id, dis);
            CTRLFORM_SetChildDisplay(ctrl_id, items->txt_form_ctrl_id, form_dis);
        }

        //icon
        items = &form->items[2*i + 1];
        if (!items->img_loaded) {
            dis = items->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
            hor_space = 30;
            CTRLFORM_SetChildSpace(items->img_form_ctrl_id, items->img_ctrl_id, &hor_space, PNULL);
            width.type = GUIFORM_CHILD_WIDTH_FIXED;
            width.add_data = 90;
            GUIFORM_SetChildWidth(items->img_form_ctrl_id,
                        items->img_ctrl_id,
                        &width);
            height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            height.add_data = 90;
            CTRLFORM_SetChildHeight(items->img_form_ctrl_id,
                    items->img_ctrl_id,
                    &height);
            img_info.img_id = items->img_id;
            CTRLANIM_SetCallBackFunc(items->img_ctrl_id, items->item_click_cb, items->usr_data);
            setAnimImgBg(items->img_ctrl_id, &img_info);
            CTRLFORM_SetChildDisplay(items->img_form_ctrl_id, items->img_ctrl_id, dis);
            CTRLFORM_SetChildDisplay(ctrl_id, items->img_form_ctrl_id, form_dis);
            //text
            hor_space = 30;
            CTRLFORM_SetChildSpace(items->txt_form_ctrl_id, items->txt_ctrl_id, &hor_space, PNULL);
            width.type = GUIFORM_CHILD_WIDTH_FIXED;
            width.add_data = 90;
            GUIFORM_SetChildWidth(items->txt_form_ctrl_id,
                        items->txt_ctrl_id,
                        &width);
            height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            height.add_data = 28;
            CTRLFORM_SetChildHeight(items->txt_form_ctrl_id,
                    items->txt_ctrl_id,
                    &height);
            tmp_string.wstr_ptr = (items->txt != PNULL) ? items->txt : L"";
            tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
            GUILABEL_SetFont(items->txt_ctrl_id, WATCH_DEFAULT_NORMAL_FONT, MMI_WHITE_COLOR);
            GUILABEL_SetText(items->txt_ctrl_id ,&tmp_string, FALSE);
            CTRLFORM_SetChildDisplay(items->txt_form_ctrl_id, items->txt_ctrl_id, dis);
            CTRLFORM_SetChildDisplay(ctrl_id, items->txt_form_ctrl_id, form_dis);
            }
    }
    tmp_cnt = item_cnt % 2;
    if (tmp_cnt != 0) {
        GUIANIM_DATA_INFO_T img_info = {0};
        GUIFORM_CHILD_DISPLAY_E dis = GUIFORM_CHILD_DISP_NORMAL;
        items = &form->items[item_cnt - 1];
        dis = items->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
        //icon
        if (!items->img_loaded) {
            ver_space = items->is_hide ? 0 : 10;
            CTRLFORM_SetChildSpace(ctrl_id, items->img_form_ctrl_id, PNULL, &ver_space);
            width.type = GUIFORM_CHILD_WIDTH_FIXED;
            width.add_data = 90;
            GUIFORM_SetChildWidth(items->img_form_ctrl_id,
                        items->img_ctrl_id,
                        &width);
            height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            height.add_data = 90;
            CTRLFORM_SetChildHeight(items->img_form_ctrl_id,
                    items->img_ctrl_id,
                    &height);
            img_info.img_id = items->img_id;
            CTRLANIM_SetCallBackFunc(items->img_ctrl_id, items->item_click_cb, items->usr_data);
            setAnimImgBg(items->img_ctrl_id, &img_info);
            CTRLFORM_SetChildDisplay(items->img_form_ctrl_id, items->img_ctrl_id, dis);
            CTRLFORM_SetChildDisplay(ctrl_id, items->img_form_ctrl_id, form_dis);
            //text
            ver_space = items->is_hide ? 0 : 5;
            CTRLFORM_SetChildSpace(ctrl_id, items->txt_form_ctrl_id, PNULL, &ver_space);
            width.type = GUIFORM_CHILD_WIDTH_FIXED;
            width.add_data = 90;
            GUIFORM_SetChildWidth(items->txt_form_ctrl_id,
                        items->txt_ctrl_id,
                        &width);
            height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            height.add_data = 28;
            CTRLFORM_SetChildHeight(items->txt_form_ctrl_id,
                    items->txt_ctrl_id,
                    &height);
            tmp_string.wstr_ptr = (items->txt != PNULL) ? items->txt : L"";
            tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
            GUILABEL_SetFont(items->txt_ctrl_id, WATCH_DEFAULT_NORMAL_FONT, MMI_WHITE_COLOR);
            GUILABEL_SetText(items->txt_ctrl_id ,&tmp_string, FALSE);
            CTRLFORM_SetChildDisplay(items->txt_form_ctrl_id, items->txt_ctrl_id, dis);
            CTRLFORM_SetChildDisplay(ctrl_id, items->txt_form_ctrl_id, form_dis);
        }

    }
    return 0;
}

PUBLIC int32 DUERAPP_HomeBannerCreate(MMI_HANDLE_T ctrl_id, void *arg)
{
    GUIFORM_CHILD_DISPLAY_E dis = GUIFORM_CHILD_DISP_NORMAL;
    MMI_STRING_T tmp_string = {0};
    GUIFORM_CHILD_WIDTH_T   width = {0};
    GUIFORM_CHILD_HEIGHT_T height = {0};
    GUIANIM_DATA_INFO_T img_info = {0};
    uint16 hor_space = 0;
    uint16 ver_space = 0;
    char *show_type = NULL;

    duer_draw_banner_t *banner = (duer_draw_banner_t*)arg;
    if (PNULL == banner) {
        return -1;
    }
    dis = banner->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
    //form
    ver_space = banner->is_hide ? 0 : 10;
    CTRLFORM_SetChildSpace(ctrl_id, banner->img_form_ctrl_id, PNULL, &ver_space);
    //banner image
    if (!banner->img_loaded) {
        width.type = GUIFORM_CHILD_WIDTH_FIXED;
        width.add_data = banner->width;
        GUIFORM_SetChildWidth(banner->img_form_ctrl_id,
                    banner->img_ctrl_id,
                    &width);
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;
        height.add_data = banner->height;
        CTRLFORM_SetChildHeight(banner->img_form_ctrl_id,
                banner->img_ctrl_id,
                &height);
        img_info.img_id = banner->img_id;
        CTRLANIM_SetCallBackFunc(banner->img_ctrl_id, banner->banner_click_cb, banner->usr_data);
        setAnimImgBg(banner->img_ctrl_id, &img_info);
        CTRLFORM_SetChildDisplay(banner->img_form_ctrl_id, banner->img_ctrl_id, dis);
        CTRLFORM_SetChildDisplay(ctrl_id, banner->img_form_ctrl_id, dis);
    }
    // ?????text???????????????
    if (MMIAPICOM_Wstrlen(banner->txt) > 0)
    {
        ver_space = banner->is_hide ? 0 : 5;
        CTRLFORM_SetChildSpace(ctrl_id, banner->txt_ctrl_id, PNULL, &ver_space);
        width.type = GUIFORM_CHILD_WIDTH_FIXED;
        width.add_data = 210;
        GUIFORM_SetChildWidth(ctrl_id,
                    banner->txt_ctrl_id,
                    &width);
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;
        height.add_data = 28;
        CTRLFORM_SetChildHeight(ctrl_id,
                banner->txt_ctrl_id,
                &height);
        tmp_string.wstr_ptr = (banner->txt != PNULL) ? banner->txt : L"";
        tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
        GUILABEL_SetFont(banner->txt_ctrl_id, WATCH_DEFAULT_NORMAL_FONT, MMI_WHITE_COLOR);
        GUILABEL_SetText(banner->txt_ctrl_id ,&tmp_string, FALSE);
        CTRLFORM_SetChildDisplay(ctrl_id, banner->txt_ctrl_id, dis);
    } else {
        CTRLFORM_SetChildDisplay(ctrl_id, banner->txt_ctrl_id, GUIFORM_CHILD_DISP_HIDE);
    }

    // ??????????????
    show_type = banner->show_type;
    if (strlen(show_type) > 0) {
        BOOLEAN is_login = duerapp_is_login();
        DUER_USER_INFO *info = duerapp_get_user_info();
        BOOLEAN is_vip = (info && info->is_vip);
        if (strcmp(show_type, "vip") == 0)
        {
            if (is_vip) {
                GUIFORM_SetChildDisplay(ctrl_id, banner->img_form_ctrl_id, GUIFORM_CHILD_DISP_HIDE);
            }
        } else if (strcmp(show_type, "login") == 0)
        {
            if (is_login) {
                GUIFORM_SetChildDisplay(ctrl_id, banner->img_form_ctrl_id, GUIFORM_CHILD_DISP_HIDE);
            }
        }
    }
    return 0;
}

#ifdef DUERAPP_USE_WATCH_CONFIG
PUBLIC void DUERAPP_FormItemCreate(MMI_HANDLE_T form_ctrl_id, duerapp_draw_t* draws, uint16 draw_cnt)
{
    int i = 0;
    if (PNULL == draws) {
        return ;
    }
    for (i = 0; i < draw_cnt; i++) {
        if (draws[i].func != PNULL) {
            switch (draws[i].type) {
                case DUER_DRAW_TYPE_SKILL_FORM:
                {
                    draws[i].func(form_ctrl_id, (void*)draws[i].draw.skill_form);
                } break;
                case DUER_DRAW_TYPE_BANNER:
                {
                    draws[i].func(form_ctrl_id, (void*)draws[i].draw.banner);
                } break;
                default :
                    break;
            }
            
        }
    }

}


PUBLIC MMI_CTRL_ID_T GetCtrlIdFromDiscoveryIndex(int index)
{
    int32 res_cnt = 0;
    int32 skill_cnt = 0;
    duer_draw_skill_form_t *skill_form = PNULL;
    DUEROS_BLOCK_LIST_ITEM_ST *block_list = PNULL;
    MMI_CTRL_ID_T ctrl_id = 0;
    int32 j = 0, i = 0;

    DUER_LOGI("GetCtrlIdFromDiscoveryIndex->index:%d", index);

    if (index >= s_discovery_list_size) {
        DUER_LOGI("GetCtrlIdFromDiscoveryIndex invalid->index:%d", index);
        return 0;
    }

    for (j = 0; j < DUERAPP_ARRAY_CNT(home_draws); j++) {
        if (DUER_DRAW_TYPE_SKILL_FORM == home_draws[j].type) {
            skill_form = home_draws[j].draw.skill_form;
            if(!skill_form->is_hide) {
                res_cnt = skill_form->item_cnt;
                for (i = 0; i < res_cnt; i++) {
                    if (!skill_form->items[i].is_hide) {
                        if (skill_cnt == index) {
                            ctrl_id = skill_form->items[i].img_ctrl_id;
                            skill_form->items[i].img_loaded = TRUE;
                            DUER_LOGI("GetCtrlIdFromDiscoveryIndex find ctrlid[%d %d]:%d", j, i, ctrl_id);
                            return ctrl_id;
                        }
                        skill_cnt++;
                    }
                }
            }
        }
    }

    return ctrl_id;
}

PUBLIC MMI_CTRL_ID_T GetCtrlIdFromBlockIndex(int index)
{
    int32 res_cnt = 0;
    int32 banner_cnt = 0;
    duer_draw_banner_t *banner = PNULL;
    DUEROS_BLOCK_LIST_ITEM_ST *block_list = PNULL;
    MMI_CTRL_ID_T ctrl_id = 0;
    int32 j = 0;

    DUER_LOGI("GetCtrlIdFromDiscoveryIndex->index:%d", index);

    if (index >= s_home_block_list_size) {
        DUER_LOGE("GetCtrlIdFromBlockIndex invalid->index:%d", index);
        return 0;
    }

    for (j = 0; j < DUERAPP_ARRAY_CNT(home_draws); j++) {
        if (DUER_DRAW_TYPE_BANNER == home_draws[j].type) {
            banner = home_draws[j].draw.banner;
            //if (!banner->is_hide) 
            {
                if (banner_cnt == index) {
                    ctrl_id = banner->img_ctrl_id;
                    banner->img_loaded = TRUE;
                    break;
                }
                banner_cnt++;
            }
        }
    }

    return ctrl_id;
}
#endif

PUBLIC void duerapp_create_qr_callback(DUERAPP_ASSISTS_RESPONSE_TYPE_E type) 
{
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_QR_LOGIN_SHOW, &type, sizeof(DUERAPP_ASSISTS_RESPONSE_TYPE_E));
}

PUBLIC int MMIDUERAPP_GetInitStatus()
{
    return s_init_status;
}

PUBLIC void MMIDUERAPP_InitState(int status)
{
    DUER_LOGI("%s status(%d)", __func__, status);
    s_init_status = status;
    MMIDUERAPP_MainSetInitStatus(MMIDUERAPP_GetInitStatus());
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_AUTH_STATUS, PNULL, 0);
}

PUBLIC void MMIDUERAPP_MediaPlayStateEvent(MMIDUERAPP_MEDIA_STATE_E state)
{
    if (MEDIA_PLAY_STATE_ERROR == state)
    {
        s_music_enter_display = FALSE;
        #if 0 //????????????UI???????????
        if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
            MMK_CloseWin(MMI_DUERAPP_PLAY_INFO_WIN_ID);
        }
        //GUIBUTTON_SetVisible(MMI_DUERAPP_MUSIC_ENTER_BUTTON_CTRL_ID,FALSE,FALSE);
        CTRLFORM_SetChildDisplay(MMI_DUERAPP_HOME_FORM_CTRL_ID, MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, GUIFORM_CHILD_DISP_HIDE);
        wchar *wstr_ptr = L"播放出错了";
        duerapp_show_toast(wstr_ptr);
        #else
        MMK_duer_other_task_to_MMI(MMI_DUERAPP_PLAY_INFO_WIN_ID,MSG_DUERAPP_MUSIC_PLAY_ERR,PNULL,0);
        #endif
        return;
    }
    #if 0 //????????????UI???????????
    if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID) && MMK_IsFocusWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_PLAY_INFO_WIN_ID,MSG_DUERAPP_MEDIA_BUTTON,PNULL,0);
    }
    #else
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_PLAY_INFO_WIN_ID,MSG_DUERAPP_MEDIA_BUTTON,PNULL,0);
    #endif
}

PUBLIC void MMIDUERAPP_InitModule(void) {
    //MMIDUERAPP_RegAppletInfo();
    MMIDUERAPP_RegNv();
    MMIDUERAPP_RegWinIdNameArr();
}

/*****************************************************************************/

LOCAL MMI_HANDLE_T MMIDUERAPP_CreateNoNetworkWin(void)
{
  return MMK_CreateWin((uint32*)MMI_DUERAPP_NETWORK_WIN_TAB, PNULL);
}

LOCAL MMI_HANDLE_T MMIDUERAPP_CreateWelcomeWin(void)
{
    return MMK_CreateWin((uint32*)MMI_DUERAPP_WELCOME_WIN_TAB, PNULL);
}

LOCAL void MMIDUERAPP_CreateDuerHomeWin(void)
{
    MMI_HANDLE_T win_handle = 0;
    MMI_WIN_ID_T win_id = MMI_DUERAPP_HOME_WIN_ID;
    //GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    if (MMK_IsOpenWin(win_id)) {
        MMK_CloseWin(win_id);
    }
    win_handle = MMK_CreateWin((uint32*)MMI_DUERAPP_HOME_WIN_TAB, PNULL);
   //MMK_SetWinRect(win_handle, &rect);
}

LOCAL void MMIDUERAPP_CreateDuerTestWin(void)
{
    MMI_HANDLE_T win_handle = 0;
    MMI_WIN_ID_T win_id = MMI_DUERAPP_HOME_TEST_WIN_ID;
    if (MMK_IsOpenWin(win_id)) {
        MMK_CloseWin(win_id);
    }
    win_handle = MMK_CreateWin((uint32*)MMI_DUERAPP_TEST_WIN_TAB, PNULL);
}

static duerapp_silde_indicator_setting_t setting;

LOCAL void MMIDUERAPP_DuerEnter(void)
{
#ifdef DUERAPP_USE_SLIDEPAGE
#define DUERAPP_MAIN_PAGE_CNT   (sizeof(pages)/sizeof(pages[0]))
    tWatchSlidePageItem pages[3];

    duer_debug_hook_level_set(DUER_LOG_VERBOSE);
    if (1 != home_draws_inited) {
        MMIDUERAPP_CreateDraws();

        
        home_draws_inited = 1;
    }
    // tWatchSlidePageItem pages[] = {
    //     [0] = {
    //         .win_id = MMI_DUERAPP_HOME_WIN_ID,
    //         .fun_enter_win = MMIDUERAPP_CreateDuerHomeWin,
    //     },
    //     [1] = {
    //         .win_id = MMI_DUERAPP_CENTER_INFO_WIN_ID,
    //         .fun_enter_win = MMIDUERAPP_CreateUserCenterWin,
    //     },
    //     [2] = {
    //         .win_id = MMI_DUERAPP_HOME_TEST_WIN_ID,
    //         .fun_enter_win = MMIDUERAPP_CreateDuerTestWin,
    //     },
    // };

    // duerapp_silde_indicator_setting_t setting = {
    //     .ind_pos = DUERAPP_SLIDE_IND_POS_TYPE_TOP,
    //     .page_cnt = DUERAPP_MAIN_PAGE_CNT,
    // };
    

    DUER_LOGI("%S: %d\r\n", __func__, __LINE__);

    memset(pages, 0x00, sizeof(pages));
    memset(&setting, 0x00, sizeof(setting));
    setting.ind_pos = DUERAPP_SLIDE_IND_POS_TYPE_TOP;
    setting.page_cnt = DUERAPP_MAIN_PAGE_CNT;

    setting.slide_handle = WatchSLIDEPAGE_CreateHandle();
    pages[0].win_id = MMI_DUERAPP_HOME_WIN_ID;
    pages[0].fun_enter_win = MMIDUERAPP_CreateDuerHomeWin;
    
    pages[1].win_id = MMI_DUERAPP_CENTER_INFO_WIN_ID;
    pages[1].fun_enter_win = MMIDUERAPP_CreateUserCenterWin;

    pages[2].win_id = MMI_DUERAPP_HOME_TEST_WIN_ID;
    pages[2].fun_enter_win = MMIDUERAPP_CreateDuerTestWin;
    // setting.slide_handle = WatchSLIDEPAGE_CreateHandle();
    DUERAPP_SlideSettingInit(&setting);
    WatchSLIDEPAGE_Open(setting.slide_handle, pages, DUERAPP_MAIN_PAGE_CNT, 0, FALSE, DUERAPP_SlideHandleCb);
#else
    MMIDUERAPP_CreateDuerHomeWin();
#endif
}


LOCAL MMI_HANDLE_T MMIDUERAPP_CreateProtocolWin(void)
{
   return MMK_CreateWin((uint32*)MMI_DUERAPP_PROTOCOL_WIN_TAB, PNULL);
}

LOCAL int MMIDUERAPP_AsrCfg(duerapp_interact_mode_t mode, duer_profile_param_key_t key, int profile_val, int fun_val)
{
    DUER_LOGI("(%s)[duer_watch]:mode(%d), key(%d), profile_val(%d), fun_val(%d)", __func__, mode, key, profile_val, fun_val);
    MMIDUERAPP_MainSetInitStatus(MMIDUERAPP_GetInitStatus());
    duerapp_set_interact_mode(mode);
    duer_profile_set_int_param(key, profile_val);
    duer_refresh_bds_asr_config();
    // bdsc_session_asr_set_fun_value(fun_val);
}

PUBLIC void MMIDUERAPP_StartInputAsrSet(void)
{
    DUER_LOGI("(%s)[duer_watch]:", __func__);
    MMIDUERAPP_AsrCfg(DUERAPP_INTERACT_MODE_NORMAL, PROFILE_KEY_BDSPEECH_PRIMARY_PID, DUER_BDSPEECH_DUERINPUT_PID, 0);
}

PUBLIC void MMIDUERAPP_StartNormalAsrSet(void)
{
    DUER_LOGI("(%s)[duer_watch]:", __func__);
    MMIDUERAPP_AsrCfg(DUERAPP_INTERACT_MODE_NORMAL, PROFILE_KEY_BDSPEECH_PRIMARY_PID, DUER_BDSPEECH_PRIMARY_PID, 1);
}

PUBLIC void MMIDUERAPP_StartDictionaryAsrSet(void)
{
    DUER_LOGI("(%s)[duer_watch]:", __func__);
    MMIDUERAPP_AsrCfg(DUERAPP_INTERACT_MODE_DICTIONARY, PROFILE_KEY_BDSPEECH_PRIMARY_PID, DUER_BDSPEECH_PRIMARY_PID, 1);
}

PUBLIC void MMIDUERAPP_StartInterpreterAsrSet(void)
{
    DUER_LOGI("(%s)[duer_watch]:", __func__);
    MMIDUERAPP_AsrCfg(DUERAPP_INTERACT_MODE_INTERPRETER, PROFILE_KEY_BDSPEECH_PRIMARY_PID, DUER_BDSPEECH_PRIMARY_PID, 1);
}

LOCAL void StartButtonCallBack(void)
{
    if (GUIBUTTON_GetSelect(MMI_DUERAPP_PROTOCOL_CHECKBOX_CTRL_ID)) {
        MMIDUERAPP_SetUserAgree(TRUE);
        MMK_CloseWin(MMI_DUERAPP_WELCOME_WIN_ID);
        MMIDUERAPP_DuerEnter();
    } else {
        wchar *pro_ptr = L"请先同意用户协议";
        duerapp_show_toast(pro_ptr);
    }
}

LOCAL void StartDuerMainCallBack(void)
{
    DUER_LOGI("(%s)[duer_watch]:", __func__);
    duer_add_click_statistics(STATISTICS_CLICK_DUER_HEAD, 0, NULL);
    MMIDUERAPP_StartNormalAsrSet();
    MMIDUERAPP_CreateDuerMainWin(FALSE); //0429V2.1 asr 
}

LOCAL MMI_RESULT_E StartWithQueryCallBack(void)
{
    uint8 url_ptr[256] = {0};
    uint16 str_len = 0;
    #if 0
    duer_send_link_click_url(LINKCLICK_NOTIFY);
    #else
    DUER_LOGI("(%s)[duer_watch]:", __func__);
    if (s_last_query_text && MMIAPICOM_Wstrlen(s_last_query_text) > 0)
    {
        DUER_LOGI("(%s)[duer_watch]:query text", __func__);
        duer_add_click_statistics(STATISTICS_CLICK_DUER_HEAD, 0, NULL);
        MMIDUERAPP_StartNormalAsrSet();
        MMIDUERAPP_CreateDuerMainWin(FALSE);
        str_len = (uint16)(MMIAPICOM_Wstrlen(s_last_query_text) * 3 + 1);
        GUI_WstrToUTF8(url_ptr, str_len, s_last_query_text, MMIAPICOM_Wstrlen(s_last_query_text));
        duerapp_send_query((char*)url_ptr);
    }
    #endif
    return MMI_RESULT_TRUE;
}

LOCAL void MusicDanceButtonCallBack(void)
{
    DUER_LOGI("MusicDanceButtonCallBack...");
    if (s_renderPlayerInfo != PNULL) {
        MMIDUERAPP_CreatePlayInfoWin(PNULL);
    }
}

LOCAL void ProtocolButtonCallBack(void)
{
    MMIDUERAPP_CreateProtocolWin();
}

LOCAL MMI_RESULT_E EngiButton1CallBack(void)
{
    DUER_LOGI("Engi Button 1,cnt=%d,%d...", gsa_engi_cnt[0], gsa_engi_cnt[1]);

    if (gsa_engi_cnt[1] > 0) {
        gsa_engi_cnt[0] = 0;
    } else {
        gsa_engi_cnt[0]++;
    }
    gsa_engi_cnt[1] = 0;

#if 0 //fortestV2.1
    extern void duerapp_get_oauth_refresh(void);
    duerapp_get_oauth_refresh();
#endif

    return MMI_RESULT_TRUE;
}

LOCAL MMI_RESULT_E EngiButton2CallBack(void)
{
    DUER_LOGI("Engi Button 2,cnt=%d,%d...", gsa_engi_cnt[0], gsa_engi_cnt[1]);

    gsa_engi_cnt[1]++;
    if (gsa_engi_cnt[0] >= 7) {
        gsa_engi_cnt[1]++;
    } else {
        gsa_engi_cnt[0] = 0;
        gsa_engi_cnt[1] = 0;
    }

    if (gsa_engi_cnt[1] >= 7) {
        DUER_LOGI("Engi Button 2 action");
        //todo engi mode
        // SCI_ASSERT(0);
        DUER_LOGI("************************************");
        #ifndef RELEASE_INFO //debug?汾
            MMIDUERAPP_SetSandOnce(TRUE); //fortestV2.1
        #endif
        DUER_LOGI("******ready to sand once******");
        DUER_LOGI("************************************");
    }

    return MMI_RESULT_TRUE;
}


/*****************************************************************************/
LOCAL void WatchRec_MainwinDrawBG(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUI_RECT_T win_rect = {0};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, rect);
    GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);
}

/*****************************************************************************/
LOCAL void WelcomeWinOpenParam(void)
{

    // checkout rec
    GUI_RECT_T agreeRect = DUERAPP_AGREE_RECT;
    GUI_RECT_T protocolRect = DUERAPP_PROTOCOL_RECT;
    GUI_FONT_ALL_T  font_info = {0};
    MMI_STRING_T querySting = {0};
    GUI_RECT_T startRect = DUERAPP_START_RECT;

    GUIBUTTON_SetRect(MMI_DUERAPP_PROTOCOL_CHECKBOX_CTRL_ID, &agreeRect);
    GUIBUTTON_SetSelect(MMI_DUERAPP_PROTOCOL_CHECKBOX_CTRL_ID,TRUE);

    GUIBUTTON_SetRect(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID, &protocolRect);
    GUIBUTTON_SetHandleLong(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID, TRUE);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID,FALSE);
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID,ProtocolButtonCallBack);
    font_info.font  = MMI_DEFAULT_NORMAL_FONT;
    font_info.color = RGB8882RGB565(0xA8A8A8);
    GUIBUTTON_SetTextAlign(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID,  ALIGN_VMIDDLE);
    GUIBUTTON_SetFont(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID,&font_info);
    querySting.wstr_ptr = L"同意用户隐私协议";
    querySting.wstr_len = MMIAPICOM_Wstrlen(querySting.wstr_ptr);
    //GUIBUTTON_SetTextAlign(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID,  ALIGN_LEFT);
    GUIBUTTON_SetText(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID, querySting.wstr_ptr,querySting.wstr_len);
    //GUIBUTTON_SetGrayed(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID, FALSE, PNULL, FALSE);
    GUIBUTTON_Update(MMI_DUERAPP_PROTOCOL_BUTTON_CTRL_ID);
    //DUER_FREE(agree_txt_title);

    GUIBUTTON_SetRect(MMI_DUERAPP_START_BUTTON_CTRL_ID, &startRect);
    GUIBUTTON_SetHandleLong(MMI_DUERAPP_START_BUTTON_CTRL_ID, TRUE);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_START_BUTTON_CTRL_ID,FALSE);
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_START_BUTTON_CTRL_ID,StartButtonCallBack);
}

LOCAL void DUERAPP_FormBG(MMI_WIN_ID_T win_id, MMI_HANDLE_T ctrl_id)
{
    GUI_RECT_T win_rect = {0};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    GUI_BG_T    bg_info = {0};
    GUI_PADDING_T   padding = {0};
    IGUICTRL_T *ictrl = PNULL;
    uint16 hor_space = 0;

    bg_info.bg_type = GUI_BG_COLOR;
    bg_info.color = MMI_BLACK_COLOR;
    CTRLFORM_SetBg(ctrl_id, &bg_info);
#ifdef DUERAPP_USE_SLIDEPAGE
    rect.top += DUERAPP_SLIDE_INDICATOR_HEIGHT;
#endif
    win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, rect);
    GUIFORM_SetRect(ctrl_id, &win_rect);
    hor_space = 15;
    padding.left   = 0;
#if DUERAPP_USE_SLIDEPAGE
    padding.top    = DUERAPP_SLIDE_POINT_PADING_TOP;
#else
    padding.top    = 10;
#endif
    padding.right  = 0;
    padding.bottom = 0;
    //set padding
    CTRLFORM_SetPadding(ctrl_id, &padding);
    // // //GUIFORM_SetMargin(ctrl_id, hor_space);
    ictrl = MMK_GetCtrlPtr(MMK_GetCtrlHandleByWin(win_id, ctrl_id));
    // GUICTRL_SetProgress(ictrl, FALSE);
    GUIFORM_PermitChildBg(ctrl_id, FALSE);     // ???????????
    GUIFORM_PermitChildFont(ctrl_id, FALSE);   // ???????????
    GUIFORM_PermitChildBorder(ctrl_id, FALSE);
}

LOCAL void setAnimImgBg(MMI_CTRL_ID_T anim_ctrl_id, GUIANIM_DATA_INFO_T *img_data)
{
    GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};

    ctrl_info.is_ctrl_id        = TRUE;
    ctrl_info.ctrl_id           = anim_ctrl_id;
    display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
    display_info.is_auto_zoom_in = TRUE;
    display_info.is_update      = TRUE;
    display_info.is_disp_one_frame = TRUE;
    GUIANIM_SetParam(&ctrl_info, img_data, PNULL, &display_info);
    GUIANIM_SetVisible(anim_ctrl_id, TRUE, TRUE);
}

#ifdef DUERAPP_USE_WATCH_CONFIG
LOCAL duer_draw_skill_item_t* DUERAPP_GetSkillItemViaCtrlId(duer_draw_skill_form_t* form, MMI_CTRL_ID_T id)
{
    int32 cnt = 0;
    duer_draw_skill_item_t *item = PNULL;
    duer_draw_skill_item_t *item_tmp = PNULL;
    int32 i = 0;
    if (PNULL == form) {
        return PNULL;
    }
    cnt = form->item_cnt;
    for (i = 0; i < cnt; i++) {
        item_tmp = &form->items[i];
        if (id == item_tmp->img_ctrl_id) {
            item = item_tmp;
            break;
        }
    }
    return item;
}

LOCAL int32 DUERAPP_GetSkillItemIndexViaCtrlId(duer_draw_skill_form_t* form, MMI_CTRL_ID_T id)
{
    int32 cnt = 0;
    int32 index = -1;
    duer_draw_skill_item_t *item_tmp = PNULL;
    int32 i = 0;
    if (PNULL == form) {
        return PNULL;
    }
    cnt = form->item_cnt;
    for (i = 0; i < cnt; i++) {
        item_tmp = &form->items[i];
        if (id == item_tmp->img_ctrl_id) {
            index = i;
            break;
        }
    }
    return index;
}

LOCAL int32 DUERAPP_GetBannerIndexViaCtrlId(MMI_CTRL_ID_T id)
{
    int32 cnt = 0;
    int32 index = -1;
    duer_draw_banner_t *banner = PNULL;
    int32 i = 0;
    for (i = 0; i < DUERAPP_ARRAY_CNT(home_draws); i++) {
        if (DUER_DRAW_TYPE_BANNER == home_draws[i].type) {
            banner = home_draws[i].draw.banner;
            if (banner->img_ctrl_id == id) {
                index = i;
                break;
            }
        }
    }
    return index;
}

LOCAL duer_draw_banner_t* DUERAPP_GetBannerViaCtrlId(MMI_CTRL_ID_T id)
{
    int32 cnt = 0;
    duer_draw_banner_t *banner = PNULL;
    duer_draw_banner_t *tmp_banner = PNULL;
    int32 i = 0;
    for (i = 0; i < DUERAPP_ARRAY_CNT(home_draws); i++) {
        if (DUER_DRAW_TYPE_BANNER == home_draws[i].type) {
            tmp_banner = home_draws[i].draw.banner;
            if (tmp_banner->img_ctrl_id == id) {
                banner = tmp_banner;
                break;
            }
        }
    }

    return banner;
}


LOCAL duer_draw_skill_form_t* DUERAPP_GetSkillFormViaCtrlId(MMI_CTRL_ID_T id)
{
    duer_draw_skill_form_t *form = PNULL;
    duer_draw_skill_form_t *tmp_form = PNULL;
    int32 cnt = 0;
    MMI_CTRL_ID_T max_id = 0;
    MMI_CTRL_ID_T min_id = 0;
    int32 i = 0;
    for (i = 0; i < DUERAPP_ARRAY_CNT(home_draws); i++) {
        if (DUER_DRAW_TYPE_SKILL_FORM == home_draws[i].type) {
            tmp_form = home_draws[i].draw.skill_form;
            cnt = tmp_form->item_cnt;
            min_id = tmp_form->items[0].img_ctrl_id;
            max_id = tmp_form->items[cnt-1].img_ctrl_id;
            if ((id >= min_id) && (id <= max_id)) {
                form = tmp_form;
                break;
            }
        }
    }
    return form;
}

LOCAL MMI_RESULT_E DuerSkillAnimCallBack(void *usr_data)
{
    int32 index = -1;
    MMI_CTRL_ID_T item_ctrl_id = (MMI_CTRL_ID_T)usr_data;
    duer_draw_skill_item_t *item = PNULL;
    duer_draw_skill_form_t *form = PNULL;

    form = DUERAPP_GetSkillFormViaCtrlId(item_ctrl_id);
    if (PNULL == form) {
        DUER_LOGE("DuerSkillAnimCallBack not find form via ctrl id:%d", item_ctrl_id);
        return MMI_RESULT_FALSE;
    }
    DUER_LOGI("item clicked:%d", item_ctrl_id);
    index = DUERAPP_GetSkillItemIndexViaCtrlId(form, item_ctrl_id);
    //item = DUERAPP_GetSkillItemViaCtrlId(&home_skill1_form, item_ctrl_id);
    if (index < 0) {
        DUER_LOGE("DuerSkillAnimCallBack invalid ctrl id:%d", item_ctrl_id);
        return MMI_RESULT_FALSE;
    }
    if (PNULL == s_discovery_list) {
        wchar *wstr_ptr = L"资源加载中...";
        duerapp_show_toast(wstr_ptr);
        return MMI_RESULT_FALSE;
    }
    item = &form->items[index];
    if (item->res_index >= s_discovery_list_size) {
        DUER_LOGE("DuerSkillAnimCallBack invalid discovery index:%d", item->res_index);
        return MMI_RESULT_FALSE;
    }
    if (0 == DUER_STRCMP(s_discovery_list[item->res_index].res_tag, "xiaoduzuowen")) {
        duerapp_writing_init();
        duerapp_writing_start();
    } else if (0 == DUER_STRCMP(s_discovery_list[item->res_index].res_tag, "xiaoduzidian")) {
        PUB_DUERAPP_DictionaryMainWinOpen();
    } else if (0 == DUER_STRCMP(s_discovery_list[item->res_index].res_tag, "xiaodufanyi")) {
        PUB_DUERAPP_InterpreterMainWinOpen();
    } else if (s_discovery_list[item->res_index].skill_query_url != PNULL) {
        MMIDUERAPP_SkillListItem_Click(s_discovery_list[item->res_index].skill_query_url);
    } else {
        s_dueros_audio_type = DUER_AUDIO_UNICAST_STORY;
        duerapp_res_list_enter(item->res_index);
    }

    duer_add_click_statistics(STATISTICS_CLICK_APP_ICON, item->res_index, 
                                        s_discovery_list[item->res_index].res_tag);

    //NetworkErrorTips();//for test
    return MMI_RESULT_TRUE;
}

LOCAL MMI_RESULT_E DuerBannerCallBack(void *usr_data)
{
    MMI_CTRL_ID_T banner_ctrl_id = (MMI_CTRL_ID_T)usr_data;
    duer_draw_banner_t *banner = PNULL;
    int home_banner_index = 0;
    char *url = NULL;
    wchar *wstr_ptr = L"跳转中...";

    DUER_LOGI("banner clicked:%d", banner_ctrl_id);
    banner = DUERAPP_GetBannerViaCtrlId(banner_ctrl_id);
    if (PNULL == banner) {
        DUER_LOGE("DuerBannerCallBack-> not find the banner via id");
        return MMI_RESULT_FALSE;
    }
    home_banner_index = banner->res_index;

    if (home_banner_index < s_home_block_list_size) {
        duer_audio_play_pause();
        duer_add_click_statistics(STATISTICS_CLICK_BANNER, home_banner_index, NULL);

        url = s_home_block_list[home_banner_index].banner_url;
        duerapp_show_toast(wstr_ptr);
        MMIDUERAPP_OperateActionUrl(url);
    }

    return MMI_RESULT_TRUE;
}
#endif

LOCAL void HomeWinFormParam(MMI_WIN_ID_T win_id) 
{
    MMI_STRING_T tmp_string = {0};
    GUI_PADDING_T   padding = {0};
    GUIFORM_CHILD_WIDTH_T   width = {0};
    GUIFORM_CHILD_HEIGHT_T height = {0};
    MMI_HANDLE_T ctrl_id = MMI_DUERAPP_MAIN_FORM_CTRL_ID;
    uint16 hor_space = 0;
    uint16 ver_space = 0;
    GUI_BG_T bg = {0};
    GUI_FONT_ALL_T  font_info = {0};
    GUI_BG_T du_btn_bg = {0};
    GUI_BG_T btn_bg_rel = {0};

    WatchRec_MainwinDrawBG(win_id);
    DUERAPP_FormBG(win_id, ctrl_id);
    {//duer header
    hor_space = 8;

    width.type = GUIFORM_CHILD_WIDTH_FIXED;
    width.add_data = 62;
    GUIFORM_SetChildWidth(MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID,
                MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID,
                &width);
    height.type = GUIFORM_CHILD_HEIGHT_FIXED;
    height.add_data = 46;
    CTRLFORM_SetChildHeight(MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID,
                MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID,
                &height);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID, FALSE);

    du_btn_bg.bg_type = GUI_BG_IMG;
    du_btn_bg.img_id = IMAGE_DUERAPP_TITLE;
    GUIBUTTON_SetFg(MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID, &du_btn_bg);
    GUIBUTTON_SetPressedFg(MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID, &du_btn_bg);
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID, StartDuerMainCallBack);
    }
    {//tips
    height.type = GUIFORM_CHILD_HEIGHT_FIXED;
    height.add_data = 46;
    width.type = GUIFORM_CHILD_WIDTH_FIXED;
    width.add_data = 140;
    GUIFORM_SetChildWidth(MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID,
                MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID,
                &width);
    CTRLFORM_SetChildHeight(MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID,
                MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID,
                &height);
    tmp_string.wstr_ptr = L"点我试试说";
    tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);

    font_info.font = WATCH_DEFAULT_NORMAL_FONT;
    font_info.color = MMI_WHITE_COLOR;

    GUIBUTTON_SetFont(MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID, &font_info);
    GUIBUTTON_SetText(MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID, tmp_string.wstr_ptr, tmp_string.wstr_len);
    GUIBUTTON_SetTextAlign(MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID,  ALIGN_HVMIDDLE);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID, FALSE);
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_HOME_TITLE_LABEL_CTRL_ID, StartDuerMainCallBack);

    GUIFORM_SetAlign(MMI_DUERAPP_HOME_TITLE_FORM_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);

    }
    {//hint
    #define DUERAPP_HOME_HINT_LABEL_COLOR RGB8882RGB565(0x5486E6)
    width.type = GUIFORM_CHILD_WIDTH_FIXED;
    width.add_data = 210;
    GUIFORM_SetChildWidth(ctrl_id,
                MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID,
                &width);
    height.type = GUIFORM_CHILD_HEIGHT_FIXED;
    height.add_data = 40;
    CTRLFORM_SetChildHeight(ctrl_id,
                MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID,
                &height);

    bg.bg_type = GUI_BG_COLOR;
    bg.color = DUERAPP_HOME_HINT_LABEL_COLOR;
    bg.shape = GUI_SHAPE_ROUNDED_RECT;
    GUIBUTTON_SetBg(MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, &bg);
    GUIBUTTON_SetPressedBg(MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, &bg);

    s_last_query_text = DUERAPP_GetRandomHint();
    tmp_string.wstr_ptr = s_last_query_text;
    tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);

    GUIFORM_SetChildSpace(ctrl_id, MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, &hor_space, PNULL);
    GUIBUTTON_SetFont(MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, &font_info);
    GUIBUTTON_SetText(MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, tmp_string.wstr_ptr, tmp_string.wstr_len);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, FALSE);
    //0429V2.1 asr  //V2已修复 // 已知问题dcs连续query，TTS被打断会下次接收不到，修改较大暂时去掉点击query
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, StartDuerMainCallBack);
    GUIFORM_SetChildAlign(ctrl_id, MMI_DUERAPP_HOME_HINT_LABEL_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);

    }
    {//player

    padding.left   = 0;
    padding.top    = 8;
    padding.right  = 0;
    padding.bottom = 2;
    GUIFORM_SetAlign(MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
    CTRLFORM_SetPadding(MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, &padding);

    width.type = GUIFORM_CHILD_WIDTH_FIXED;
    width.add_data = 180;
    GUIFORM_SetChildWidth(MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID,
                MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID,
                &width);
    height.type = GUIFORM_CHILD_HEIGHT_FIXED;
    height.add_data = 45;
    CTRLFORM_SetChildHeight(MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID,
                MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID,
                &height);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID, FALSE);
    btn_bg_rel.bg_type = GUI_BG_IMG;
    btn_bg_rel.img_id = IMAGE_DUERAPP_MUSIC_ENTER;
    GUIBUTTON_SetBg(MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID, &btn_bg_rel);
    GUIBUTTON_SetFg(MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID, &btn_bg_rel);
    GUIBUTTON_SetPressedFg(MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID, &btn_bg_rel);
    GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID, MusicDanceButtonCallBack);
    CTRLFORM_SetChildDisplay(ctrl_id, MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, s_music_enter_display ? GUIFORM_CHILD_DISP_NORMAL : GUIFORM_CHILD_DISP_HIDE);
    }

#ifdef DUERAPP_USE_WATCH_CONFIG
    DUERAPP_FormItemCreate(MMI_DUERAPP_HOME_FORM_CTRL_ID, &home_draws, DUERAPP_ARRAY_CNT(home_draws));
    padding.left   = 15;
    padding.top    = 8;
    padding.right  = 0;
    padding.bottom = 10;
    CTRLFORM_SetPadding(MMI_DUERAPP_HOME_FORM_CTRL_ID, &padding);
#else
    DrawHomepageListItem(MMI_DUERAPP_HOME_LIST_CTRL_ID);
    padding.left   = 15;
    padding.top    = 8;
    padding.right  = 0;
    padding.bottom = 10;
    CTRLFORM_SetPadding(MMI_DUERAPP_HOME_FORM_CTRL_ID, &padding);
#endif
}

LOCAL void TestWinFormParam(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T form_ctrl_id = MMI_DUERAPP_HOME_TEST_FORM_CTRL_ID;
    MMI_CTRL_ID_T text_ctrl_id = MMI_DUERAPP_HOME_TEST_TXT_CTRL_ID;
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    GUI_FONT_T font_size = WATCH_DEFAULT_BIG_FONT;
    MMI_STRING_T tmp_string = {0};
    GUIFORM_CHILD_WIDTH_T   width = {0};
    GUIFORM_CHILD_HEIGHT_T  height = {0};

    DUERAPP_FormBG(win_id, form_ctrl_id);
    WatchRec_MainwinDrawBG(win_id);

    tmp_string.wstr_ptr = L"\n更多功能\n\n敬请期待...";
    tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);

    GUIFORM_SetAlign(form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
    
    GUITEXT_SetAlign(text_ctrl_id, ALIGN_HMIDDLE);
    GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
    GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
    GUITEXT_SetString(text_ctrl_id, tmp_string.wstr_ptr, tmp_string.wstr_len, FALSE);
    
    //ENGI BUTTON
    {
        width.type = GUIFORM_CHILD_WIDTH_FIXED;
        width.add_data = 80;
        GUIFORM_SetChildWidth(MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID,
                    MMI_DUERAPP_TEST_ENGI_1_BUTTON_CTRL_ID,
                    &width);
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;
        height.add_data = 60;
        CTRLFORM_SetChildHeight(MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID,
                    MMI_DUERAPP_TEST_ENGI_1_BUTTON_CTRL_ID,
                    &height);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_TEST_ENGI_1_BUTTON_CTRL_ID, FALSE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_TEST_ENGI_1_BUTTON_CTRL_ID, EngiButton1CallBack);
        GUIBUTTON_SetVisible(MMI_DUERAPP_TEST_ENGI_1_BUTTON_CTRL_ID, TRUE, FALSE);

        GUIFORM_SetChildWidth(MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID,
                    MMI_DUERAPP_TEST_ENGI_2_BUTTON_CTRL_ID,
                    &width);
        CTRLFORM_SetChildHeight(MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID,
                    MMI_DUERAPP_TEST_ENGI_2_BUTTON_CTRL_ID,
                    &height);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_TEST_ENGI_2_BUTTON_CTRL_ID, FALSE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_TEST_ENGI_2_BUTTON_CTRL_ID, EngiButton2CallBack);
        GUIBUTTON_SetVisible(MMI_DUERAPP_TEST_ENGI_2_BUTTON_CTRL_ID, TRUE, FALSE);
        
        GUIFORM_SetAlign(MMI_DUERAPP_TEST_ENGI_FORM_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
    }
}


LOCAL void NetworkWinFormParam(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T txt_ctrl_id = MMI_DUERAPP_NETWORK_TITLE_CTRL_ID;
    MMI_CTRL_ID_T txt_ctrl_id2 = MMI_DUERAPP_NETWORK_SUBTITLE_CTRL_ID;
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};

    GUI_RECT_T titleRect = {0,20,240,50};
    GUI_RECT_T sub_titleRect = {0,60,240,85};
    GUI_POINT_T start_point = {0};

    MMI_STRING_T title_text = {0};
    MMI_STRING_T sub_title_text = {0};

    title_text.wstr_ptr = L"网络未连接";
    title_text.wstr_len = MMIAPICOM_Wstrlen(title_text.wstr_ptr);

    sub_title_text.wstr_ptr = L"请联网后重试";
    sub_title_text.wstr_len = MMIAPICOM_Wstrlen(sub_title_text.wstr_ptr);

    GUILABEL_SetRect(txt_ctrl_id,&titleRect,FALSE);
    GUILABEL_SetFont(txt_ctrl_id, WATCH_DEFAULT_BIG_FONT, MMI_WHITE_COLOR);
    GUILABEL_SetAlign(txt_ctrl_id, GUILABEL_ALIGN_MIDDLE);
    GUILABEL_SetText(txt_ctrl_id ,&title_text, TRUE);

    GUILABEL_SetRect(txt_ctrl_id2,&sub_titleRect,FALSE);
    GUILABEL_SetFont(txt_ctrl_id2, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_NETWORK_SUBTITLE_COLOR);
    GUILABEL_SetAlign(txt_ctrl_id2, GUILABEL_ALIGN_MIDDLE);
    GUILABEL_SetText(txt_ctrl_id2 ,&sub_title_text, TRUE);

    start_point.x = 62;
    start_point.y = 90;
    GUIRES_DisplayImg(&start_point,PNULL,PNULL
            ,win_id
            ,IMAGE_DUERAPP_NO_NETWORK
            ,&lcd_dev_info);

    MMK_SetAtvCtrl(win_id,txt_ctrl_id);
}

#ifdef DUERAPP_USE_WATCH_CONFIG
LOCAL void CalculateHomeFormCnt(void)
{
    int32 i = 0;
    s_home_skill_form_cnt = 0;
    s_home_banner_cnt = 0;
    for (i = 0; i < DUERAPP_ARRAY_CNT(home_draws); i++) {
        if (DUER_DRAW_TYPE_SKILL_FORM == home_draws[i].type) {
            s_home_skill_form_cnt++;
        } else if (DUER_DRAW_TYPE_BANNER == home_draws[i].type) {
            s_home_banner_cnt++;
        }
    }
}
#endif

LOCAL void NoNetworkFormParam(MMI_WIN_ID_T win_id) 
{
    wchar *net_error = L"网络连接失败了";
    CTRLFORM_SetChildDisplay(MMI_DUERAPP_HOME_FORM_CTRL_ID, MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, GUIFORM_CHILD_DISP_HIDE);
}

PUBLIC void MMIDUERAPP_HomePageOpenWinInit(void)
{
    // DUER_LOGI may not available before duer_initialize()
    SCI_TRACE_LOW("%s", __FUNCTION__);
    if (!s_run_once) {
        if (MMIDUERAPP_GetIsSandOnce()) {
            duer_sand_domain_change(TRUE);
        }
        MMIDUERAPP_SetSandOnce(FALSE);

        duer_client_id_init();
#ifdef DUERAPP_USE_WATCH_CONFIG
        CalculateHomeFormCnt();
#endif
        duerapp_init();
        duer_sfs_cfg_init();
        duerapp_promoted_pay_init();
        DUER_LOGI("%s", "run_once finished");
        s_run_once = 1;
    } else {
        DUER_LOGI("call duer_start.");
        duer_start(NULL, 0);
    }
    duer_auth_anonymous();
}

PUBLIC void MMIDUERAPP_HomePageCloseWinDeinit(void)
{
    if (MMIDUERAPP_GetInitStatus()) {
        duer_asr_stop();
        duer_ext_stop_speak();
        duer_audio_player_stop();
        duer_dcs_audio_on_stopped();
        duer_dcs_close_multi_dialog();
        duer_dcs_dialog_cancel();
    }
#ifdef DUERAPP_USE_SLIDEPAGE
    // Hemingway
    WatchSLIDEPAGE_DestoryHandle(setting.slide_handle);
#endif    
    s_init_status = 0;
}
/*****************************************************************************/

/*****************************************************************************/
LOCAL MMI_RESULT_E HandleWelcomeWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{

    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    GUI_RECT_T          win_rect = {0};
    //lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    //lcd_dev_info.block_id   = GUI_BLOCK_MAIN;
    GUI_POINT_T start_point = {0};

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:

        //WatchRec_MainwinDrawBG();
        WelcomeWinOpenParam();
        break;

    case MSG_GET_FOCUS:
        //WatchRec_MainwinDrawBG();
        //WelcomeWinOpenParam();
        break;

    case MSG_LOSE_FOCUS:
        break;

    case MSG_FULL_PAINT:
        MMK_GetWinRect(win_id, &win_rect);
        MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
        GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);
        WelcomeWinOpenParam();
        start_point.x = 80;
        start_point.y = 30;
        GUIRES_DisplayImg(&start_point,PNULL,PNULL
            ,win_id
            ,IMAGE_DUERAPP_PREPARE
            ,&lcd_dev_info);
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
}

LOCAL MMI_RESULT_E HandleProtocolWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{

    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_PROTOCOL_RICHTEXT_CTRL_ID;
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        GUIRICHTEXT_SetBgColor(ctrl_id,MMI_BLACK_COLOR);
        AddItemProtocolImgRes(win_id, ctrl_id);
        GUIRICHTEXT_SetBorder(ctrl_id, GUIRICHTEXT_FRAME_NONE);
        //GUITEXT_IsDisplayPrg(FALSE,ctrl_id);
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
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
}

LOCAL MMI_RESULT_E HandleDuerHomeWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          )
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    char *input_text  = PNULL;
    char *render_card = PNULL;
    static BOOLEAN s_last_focus_sta = FALSE;
    wchar *login_success = L"支付成功";
    duer_operate_qr_t *p_operate_qr = NULL;
    wchar *net_tips = L"小度会想你的哦";
    int idx = -1;
    uint32 src_id = 0;

    DUER_LOGI("%s win_id=%d,msg=%x", __FUNCTION__, win_id, msg_id);


    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
    {
        DuerPermissionLog("welcome to xiaodu app..");
        MMIDUERAPP_HomePageOpenWinInit();
        duerapp_first_in = TRUE;
        s_music_enter_display = FALSE;
        duerapp_mplayer_netchecked_refresh();
        WatchRec_MainwinDrawBG(win_id);
        //MMK_SetAtvCtrl(MMI_DUERAPP_HOME_FORM_CTRL_ID, MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID);
        duer_statistics_time_count_start(STATISTICS_TIME_TYPE_XIAODU);
    } break;
    case MSG_LCD_SWITCH:
    {
    } break;
    case MSG_TIMER:
    {
    } break;
    case MSG_FULL_PAINT:
    {
        //WatchRec_MainwinDrawBG(win_id);
        //HomeSyncRes();
        HomeWinFormParam(win_id);
        if (duerapp_first_in) {
            duerapp_first_in = FALSE;
            MMK_SetAtvCtrl(MMI_DUERAPP_HOME_FORM_CTRL_ID, MMI_DUERAPP_HOME_TITLE_BUTTON_CTRL_ID);
        }
    } break;
    case MSG_GET_FOCUS:
    {
        DUER_LOGI("MSG_GET_FOCUS");
        if (!s_last_focus_sta) {
            duer_add_click_statistics(STATISTICS_CLICK_HOME_PAGE, STATISTICS_PAGE_FIRST, NULL);
        }
        s_last_focus_sta = TRUE;
    } break;
    case MSG_LOSE_FOCUS:
    {
        s_last_focus_sta = FALSE;
    } break;
    case MSG_DUERAPP_AUTH_STATUS:
    {
        DUER_LOGI("DUER_CONFIG init status: %d\r\n", s_init_status);
        if (s_init_status) {
            DUER_LOGI("duer_send_link_click_url...");    
            duer_send_link_click_url(LINKCLICK_NOTIFY);
            duer_send_version_statistics_quick();
            duer_send_click_statistics_quick();
#ifdef DUERAPP_USE_WATCH_CONFIG
            if (PNULL == s_discovery_list) {
                DUER_LOGI("DUER_CONFIG get config\r\n");
                duerapp_get_watch_config();
            } else {
                DUER_LOGI("DUER_CONFIG config list not null\r\n");
                HomeSyncRes();
            }
#endif
        } else {
            NoNetworkFormParam(win_id);
            NetworkErrorTips();
        }
    } break;
    case MSG_DUERAPP_DISCOVERY_CALL_BACK: {
        DUER_LOGI("%s", __FUNCTION__);
#ifdef DUERAPP_USE_WATCH_CONFIG
        HomeSyncRes();
        HomeUpdateSkillFormRes(MMI_DUERAPP_HOME_FORM_CTRL_ID);
        if (MMK_IsFocusWin(MMI_DUERAPP_HOME_WIN_ID)) {
            DUER_LOGI("%s send full paint to homepage", __FUNCTION__);
            MMK_SendMsg(MMI_DUERAPP_HOME_WIN_ID, MSG_FULL_PAINT, PNULL); //???????????????
        }
        imageStartDownResource(IMG_DOWN_HOME_BANNER);
        imageStartDownResource(IMG_DOWN_CENTER_BANNER);
#endif
        if (duerapp_is_need_refresh_token()) {
            duerapp_get_oauth_refresh();
        }
    } break;
    case MSG_DUERAPP_ANIM_PIC_UPDATE:
    {
        DebugimageMMIDealDownCbMsg(MMI_DUERAPP_HOME_WIN_ID, param);
        // MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL); //FULL PAINT???????????????anim?????????????????
    } break;
    case MSG_DUERAPP_PLAYER_INFO_CARD:
    {
        //GUIBUTTON_SetVisible(MMI_DUERAPP_HOME_MUSIC_ENTER_CTRL_ID, TRUE, FALSE);
        duerapp_close_waiting();
        if (!MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
            MMIDUERAPP_CreatePlayInfoWin(s_renderPlayerInfo);
        }
        DUER_LOGI("%s", __FUNCTION__);
        s_music_enter_display = TRUE;
        if (MMK_IsFocusWin(MMI_DUERAPP_HOME_WIN_ID)) {
            DUER_LOGI("%s", __FUNCTION__);
            CTRLFORM_SetChildDisplay(MMI_DUERAPP_HOME_FORM_CTRL_ID, MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, GUIFORM_CHILD_DISP_NORMAL);
        }
        //UpdateDuerAnimation(MMIDUERAPP_ANIMATE_STATE_PREPARE);
    } break;
    case MSG_DUERAPP_QR_LOGIN_SHOW:
    {
        MMIDUERAPP_CreateQrWin((ADD_DATA)param);
    } break;
    case MSG_DUERAPP_OPERATE_CALL_BACK:
    {
        MMIDUERAPP_CreateOperateWin();
    } break;
    case MSG_DUERAPP_ACTIVITY_CALL_BACK:
    {
        MMIDUERAPP_OpLongpicDownload();
    } break;
    case MSG_DUERAPP_ACTIVITY_SHOW:
    {
        MMIDUERAPP_CreateOpLongpicWin();
    } break;
    case MSG_APP_OK:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
    
    case MSG_DUERAPP_QRPAY_SUCCESS:
    {
        duerapp_show_toast(login_success);
        if (MMK_IsOpenWin(MMI_DUERAPP_QR_WIN_ID)) {
            MMK_CloseWin(MMI_DUERAPP_QR_WIN_ID);
        }
    }
    break;
    case MSG_DUERAPP_CLOSE_PLAY_INFO:
    {
        if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
            MMK_CloseWin(MMI_DUERAPP_PLAY_INFO_WIN_ID);
        }
    }
    break;
    case MSG_DUERAPP_CLOSE_CENTER_INFO:
    {
        if (MMK_IsOpenWin(MMI_DUERAPP_CENTER_INFO_WIN_ID)) {
            MMK_CloseWin(MMI_DUERAPP_CENTER_INFO_WIN_ID);
        }
    }
    break;
    case MSG_DUERAPP_QRCODE_TEMPLATE:
    {
        DUER_LOGI("QRCODE_TEMPLATE show");
        p_operate_qr = (duer_operate_qr_t *)param;
        if (p_operate_qr) {
            MMIDUERAPP_CreateOperateCommonQrWin(p_operate_qr->url, p_operate_qr->utf8_str);

            if (p_operate_qr->url) {
                DUER_FREE(p_operate_qr->url);
                p_operate_qr->url = NULL;
            }
            if (p_operate_qr->utf8_str) {
                DUER_FREE(p_operate_qr->utf8_str);
                p_operate_qr->utf8_str = NULL;
            }
        } else {
            DUER_LOGE("QRCODE_TEMPLATE msg error");
        }
    } 
    break;
    case MSG_DUERAPP_SHOW_MIGU:
    {
        MMIDUERAPP_CreateMiguWin(LINKCLICK_BUY_VIP);
    }
    break;
#ifndef DUERAPP_USE_WATCH_CONFIG
    case MSG_CTL_PENOK:
    {
        if (PNULL == param)
        {
            break;
        }
        src_id = ((MMI_NOTIFY_T*)param)->src_id;
        DUER_LOGI("src_id = %d", src_id);
        if (src_id == MMI_DUERAPP_HOME_LIST_CTRL_ID)
        {
            idx = GUILIST_GetCurItemIndex(src_id);
            DUER_LOGI("list index = %d", idx);
            if (0 == idx)
            {
                MMIDUERAPP_CreateUserCenterWin();
            }
            else if (1 == idx)
            {
                PUB_DUERAPP_DictionaryMainWinOpen();
            }
            else if (2 == idx)
            {
                PUB_DUERAPP_InterpreterMainWinOpen();
            }
            else if (3 == idx)
            {
                duerapp_writing_init();
                duerapp_writing_start();
            }
        }
        break;
    }
#endif
    case MSG_CLOSE_WINDOW:
    {
        DUER_LOGI("DUER_EXIT: msg close window\r\n");
        {//work around for a crash when close the window in third-page
            duerapp_show_toast(net_tips);
        }

        s_music_enter_display = FALSE;
        duerapp_mplayer_netchecked_refresh();
#ifdef DUERAPP_USE_WATCH_CONFIG
        HomeFreeSkillFormRes();
#endif
        free_render_player(NULL);

        duerapp_free_res_list();
        duerapp_free_user_info();
        duerapp_free_qr_info();
        imageArtFreeAll();
        MMIDUERAPP_HomePageCloseWinDeinit();
        //s_duerapp_running = FALSE;
        duer_statistics_time_count_stop(STATISTICS_TIME_TYPE_XIAODU);
    } break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

LOCAL MMI_RESULT_E HandleDuerNetworkWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          )
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    char *input_text  = PNULL;
    char *render_card = PNULL;
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        WatchRec_MainwinDrawBG(win_id);
        break;
    case MSG_FULL_PAINT:
        WatchRec_MainwinDrawBG(win_id);
        NetworkWinFormParam(win_id);
        break;
    case MSG_APP_OK:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}




LOCAL MMI_RESULT_E HandleDuerTestWinMsg(
                                          MMI_WIN_ID_T     win_id, 
                                          MMI_MESSAGE_ID_E msg_id, 
                                          DPARAM           param
                                          )
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    char *input_text  = PNULL;
    char *render_card = PNULL;
    static BOOLEAN s_last_focus_sta = FALSE;

    DUER_LOGI("%s msg=%x", __FUNCTION__, msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        WatchRec_MainwinDrawBG(win_id);
        break;
    case MSG_GET_FOCUS: //?????????
        //duerapp_check_user_info(win_id);
        if (!s_last_focus_sta) {
            duer_add_click_statistics(STATISTICS_CLICK_HOME_PAGE, STATISTICS_PAGE_THIRD, NULL);
        }
        s_last_focus_sta = TRUE;
        break;
    case MSG_LOSE_FOCUS:
    {
        s_last_focus_sta = FALSE;
        gsa_engi_cnt[0] = 0;
        gsa_engi_cnt[1] = 0;
    } break;
    case MSG_LCD_SWITCH:
        break;
    case MSG_TIMER:
    {
        break;
    }
    case MSG_FULL_PAINT:
        WatchRec_MainwinDrawBG(win_id);
        TestWinFormParam(win_id);
        break;
    case MSG_APP_OK:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

LOCAL void AddItemProtocolImgRes(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    GUIRICHTEXT_ITEM_T item = {0};
    uint16 index            = 1;
    const wchar w_str[]     = L"欢迎您使用DuerOS产品及服务！我们非常重视儿童的隐私保护和个人信息。本隐私政策适用于您允许您所监护的儿童通过任何方式对DuerOS各项服务的访问和使用。\n完整隐私政策，请扫描下方二维码查看";
    
    item.img_type = GUIRICHTEXT_IMAGE_RES;
    item.img_data.res.id = IMAGE_DUERAPP_PROTOCOL;
    item.img_frame_type = GUIRICHTEXT_FRAME_NONE;
    item.img_set_size = TRUE;
    item.img_setting_width = MMI_MAINSCREEN_WIDTH;
    item.img_setting_height = MMI_MAINSCREEN_HEIGHT;
    item.img_set_align = TRUE;
    item.img_setting_align = GUIRICHTEXT_IMG_ALIGN_MIDDLE;
    
    item.text_type = GUIRICHTEXT_TEXT_BUF;
    item.text_data.buf.str_ptr = w_str;
    item.text_data.buf.len = MMIAPICOM_Wstrlen(w_str);
    item.text_set_font = TRUE;
    item.setting_font = WATCH_DEFAULT_NORMAL_FONT;
    item.text_set_font_color = TRUE;
    item.setting_font_color = MMI_WHITE_COLOR;
    
    item.tag_type = GUIRICHTEXT_TAG_IMG_NORMAL | GUIRICHTEXT_TAG_TXT_NORMAL;
    item.frame_type = GUIRICHTEXT_FRAME_NONE;
    item.text_set_align = TRUE;
    item.setting_align = ALIGN_VMIDDLE;
    item.align_type = GUIRICHTEXT_ALIGN_UP;
    
    GUIRICHTEXT_AddItem(ctrl_id, &item, &index);
}

#ifdef DUERAPP_USE_WATCH_CONFIG
LOCAL void HomeSyncRes(void)
{
    int32 res_cnt = 0;
    int32 skill_cnt = 0;
    int32 banner_cnt = 0;
    int32 discovery_cnt = 0;
    duer_draw_skill_form_t *skill_form = PNULL;
    duer_draw_banner_t *banner = PNULL;
    DUEROS_DISCOVERY_LIST_ITEM_ST *discovery_list = PNULL;
    DUEROS_BLOCK_LIST_ITEM_ST *block_list = PNULL;
    int32 i = 0, j = 0;

    if (PNULL == s_discovery_list) {
        return;
    }
    for (j = 0; j < DUERAPP_ARRAY_CNT(home_draws); j++) {
        if (DUER_DRAW_TYPE_SKILL_FORM == home_draws[j].type) {
            skill_cnt++;
            skill_form = home_draws[j].draw.skill_form;
            if (skill_cnt > s_home_block_list_size) {
                skill_form->is_hide = TRUE;
                skill_cnt++;
                continue;
            }
            block_list = &s_home_block_list[skill_cnt-1];
            skill_form->is_hide = FALSE;
            skill_form->title = block_list->block_name;
            res_cnt = skill_form->item_cnt;
            for (i = 0; i < res_cnt; i++) {
                if (discovery_cnt >= s_discovery_list_size) {
                    skill_form->items[i].is_hide = TRUE;
                    continue;
                }
                discovery_list = &s_discovery_list[discovery_cnt];
                if (skill_cnt != discovery_list->block_id) {
                    skill_form->items[i].is_hide = TRUE;
                    continue;
                }
                skill_form->items[i].txt = s_discovery_list[discovery_cnt].res_name;
                skill_form->items[i].is_hide = FALSE;
                skill_form->items[i].res_index = discovery_cnt;
                discovery_cnt++;
            }
        } else if (DUER_DRAW_TYPE_BANNER == home_draws[j].type) {
            banner_cnt++;
            banner = home_draws[j].draw.banner;
            if (banner_cnt > s_home_block_list_size) {
                banner->is_hide = TRUE;
                continue;
            }
            block_list = &s_home_block_list[banner_cnt-1];
            if (PNULL == block_list->banner_img_url) {
                banner->is_hide = TRUE;
                continue;
            }
            banner->res_index = banner_cnt - 1;
            banner->is_hide = FALSE;
            banner->txt = block_list->banner_name;
            banner->show_type = block_list->show_type;
        }
    }

}

LOCAL void HomeUpdateSkillFormRes(MMI_HANDLE_T form_ctrl_id)
{
    int32 res_cnt = 0;
    duer_draw_skill_form_t *skill_form = PNULL;
    duer_draw_banner_t *banner = PNULL;
    MMI_STRING_T tmp_string = {0};
    GUIFORM_CHILD_DISPLAY_E dis = GUIFORM_CHILD_DISP_NORMAL;
    GUIFORM_CHILD_DISPLAY_E form_dis = GUIFORM_CHILD_DISP_NORMAL;
    uint16 ver_space = 0;
    int32 i = 0, j = 0;
    duer_draw_skill_item_t *items = NULL;

    for (j = 0; j < DUERAPP_ARRAY_CNT(home_draws); j++) {
        if (DUER_DRAW_TYPE_SKILL_FORM == home_draws[j].type) {
            //tile
            skill_form = home_draws[j].draw.skill_form;
            form_dis = skill_form->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
            tmp_string.wstr_ptr = (skill_form->title != PNULL) ? skill_form->title : L"";
            tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
            GUILABEL_SetText(skill_form->tile_ctrl_id, &tmp_string, FALSE);
            CTRLFORM_SetChildDisplay(form_ctrl_id, skill_form->tile_ctrl_id, form_dis);
            ver_space = skill_form->is_hide ? 0 : 10;
            CTRLFORM_SetChildSpace(form_ctrl_id, skill_form->tile_ctrl_id, PNULL, &ver_space);

            res_cnt = skill_form->item_cnt;
            for (i = 0; i < res_cnt; i++) {
                items = &skill_form->items[i];

                ver_space = items->is_hide ? 0 : 5;
                CTRLFORM_SetChildSpace(form_ctrl_id, items->txt_form_ctrl_id, PNULL, &ver_space);
                ver_space = items->is_hide ? 0 : 10;
                CTRLFORM_SetChildSpace(form_ctrl_id, items->img_form_ctrl_id, PNULL, &ver_space);

                tmp_string.wstr_ptr = (items->txt != PNULL) ? items->txt : L"";
                tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
                GUILABEL_SetText(items->txt_ctrl_id ,&tmp_string, FALSE);

                dis = items->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
                CTRLFORM_SetChildDisplay(items->txt_form_ctrl_id, items->txt_ctrl_id, dis);
                CTRLFORM_SetChildDisplay(form_ctrl_id, items->img_form_ctrl_id, form_dis);
                CTRLFORM_SetChildDisplay(form_ctrl_id, items->txt_form_ctrl_id, form_dis);
            }
        }else if (DUER_DRAW_TYPE_BANNER == home_draws[j].type) {
            banner = home_draws[j].draw.banner;

            ver_space = banner->is_hide ? 0 : 10;
            CTRLFORM_SetChildSpace(form_ctrl_id, banner->img_form_ctrl_id, PNULL, &ver_space);
            ver_space = banner->is_hide ? 0 : 5;
            CTRLFORM_SetChildSpace(form_ctrl_id, banner->txt_ctrl_id, PNULL, &ver_space);

            tmp_string.wstr_ptr = (banner->txt != PNULL) ? banner->txt : L"";
            tmp_string.wstr_len = MMIAPICOM_Wstrlen(tmp_string.wstr_ptr);
            GUILABEL_SetText(banner->txt_ctrl_id, &tmp_string, FALSE);

            form_dis = banner->is_hide ? GUIFORM_CHILD_DISP_HIDE : GUIFORM_CHILD_DISP_NORMAL;
            CTRLFORM_SetChildDisplay(form_ctrl_id, banner->txt_ctrl_id, form_dis);
            CTRLFORM_SetChildDisplay(banner->img_form_ctrl_id, banner->img_ctrl_id, form_dis);
            CTRLFORM_SetChildDisplay(form_ctrl_id, banner->img_form_ctrl_id, form_dis);
        }
    }

}


LOCAL void HomeFreeSkillFormRes(void)
{
    int32 res_cnt = 0;
    duer_draw_skill_form_t *skill_form = PNULL;
    duer_draw_banner_t *banner = PNULL;
    int32 i = 0, j = 0;

    for (j = 0; j < DUERAPP_ARRAY_CNT(home_draws); j++) {
        if (DUER_DRAW_TYPE_SKILL_FORM == home_draws[j].type) {
            skill_form = home_draws[j].draw.skill_form;
            res_cnt = skill_form->item_cnt;
            skill_form->is_hide = FALSE;
            for (i = 0; i < res_cnt; i++) {
                skill_form->items[i].txt = DUERAPP_DEFAULT_LOAD_TXT;
                skill_form->items[i].img_loaded = FALSE;
                skill_form->items[i].is_hide = FALSE;
            }
        } else if (DUER_DRAW_TYPE_BANNER == home_draws[j].type) {
            banner = home_draws[j].draw.banner;
            banner->is_hide = FALSE;
            banner->img_loaded = FALSE;
            banner->txt = DUERAPP_DEFAULT_LOAD_TXT;
        }
    }
}
#endif

LOCAL void NetworkErrorTips(void)
{
      wchar *wstr_ptr = L"网络出错了";
      duerapp_show_toast(wstr_ptr);
}

#ifndef DUERAPP_USE_WATCH_CONFIG
LOCAL void DrawHomepageListItem( MMI_CTRL_ID_T ctrl_id )
{
    wchar *userCenterText = L"个人中心         >";
    wchar *dictionaryText = L"小度字典         >";
    wchar *translateText = L"小度翻译         >";
    wchar *writingText = L"测试入口3        >";

    MMI_STRING_T list_text = {0};
    uint16 curSelection = 0;
    GUILIST_SetMaxItem(ctrl_id, 3, FALSE);
    GUILIST_RemoveAllItems(ctrl_id);

    // set selected item
    GUILIST_SetSelectedItem(ctrl_id, curSelection, TRUE);
    // set current item
    GUILIST_SetCurItemIndex(ctrl_id, curSelection);

    list_text.wstr_len = MMIAPICOM_Wstrlen(userCenterText);
    list_text.wstr_ptr = userCenterText;
    WatchCOM_ListItem_Draw_1Icon_1Str(ctrl_id, IMAGE_DUERAPP_CENTER_BUTTON, list_text);

    list_text.wstr_len = MMIAPICOM_Wstrlen(dictionaryText);
    list_text.wstr_ptr = dictionaryText;
    WatchCOM_ListItem_Draw_1Icon_1Str(ctrl_id, IMAGE_DUERAPP_DICTIONARY_ICON, list_text);

    list_text.wstr_len = MMIAPICOM_Wstrlen(translateText);
    list_text.wstr_ptr = translateText;
    WatchCOM_ListItem_Draw_1Icon_1Str(ctrl_id, IMAGE_DUERAPP_INTERPRETER_ICON, list_text);

    // list_text.wstr_len = MMIAPICOM_Wstrlen(writingText);
    // list_text.wstr_ptr = writingText;
    // WatchCOM_ListItem_Draw_1Icon_1Str(ctrl_id, IMAGE_DUERAPP_CENTER_ABOUT_XIAODU, list_text);
}
#endif

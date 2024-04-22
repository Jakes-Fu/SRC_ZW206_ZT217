// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_center.c
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
#include "duerapp_slide_indicator.h"
#include "watch_common_btn.h"
#include "watch_common_list.h"
#include "watch_commonwin_export.h"
#include "mmi_event_api.h"
#include "mmk_app.h"
#include "mmi_applet_table.h"
#include "guirichtext.h"
// #include "mmiwifi_export.h"
#include "mmiphone_export.h"
#include "ctrlsetlist_export.h"
#include "guisetlist.h"

#include "dal_time.h"
#include "duerapp.h"
#include "mmicom_time.h"
#include "duerapp_openapi.h"
#include "duerapp_common.h"
#include "duerapp_login.h"
#include "duerapp_qrwin.h"
#include "duerapp_http.h"
#include "duerapp_statistics.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_res_down.h"
#include "duerapp_audio_play_utils.h"
#include "lightduer_log.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"

#include "duerapp_center.h"
/**--------------------------------------------------------------------------*/
/**                         MACRO DEFINITION                                 */
/**--------------------------------------------------------------------------*/
#define DUERAPP_FULL_SCREEN_RECT        {0,0,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT}

#define DUERAPP_CENTER_WIN_BG           RGB8882RGB565(0x000000)
#define DUERAPP_USERINO_BG              RGB8882RGB565(0x7B663D)
#define DUERAPP_RESOURCE_VIP_BG         RGB8882RGB565(0xFFB238)
#define DUERAPP_VIP_MASK_BG             RGB8882RGB565(0x000000)
#define DUERAPP_LOGIN_BTN_COLOR         RGB8882RGB565(0x333333)
#define DUERAPP_OK_BTN_COLOR            RGB8882RGB565(0x7F7F7F)
#define DUERAPP_CANCEL_BTN_COLOR        RGB8882RGB565(0x0099F5)
#define DUERAPP_QUIT_TEXT_COLOR         RGB8882RGB565(0x999999)

#define DUERAPP_NORMAL_COLOR            RGB8882RGB565(0x878EA0)
#define DUERAPP_LOGIN_COLOR             RGB8882RGB565(0x4A5C8E)
#define DUERAPP_VIP_COLOR               RGB8882RGB565(0xC19553)
#define DUERAPP_LOGIN_BTN_COLOR         RGB8882RGB565(0x001434)
#define DUERAPP_VIP_BTN_COLOR           RGB8882RGB565(0x623A02)

#define DUERAPP_USERINO_RECT            {10,15,230,115}
#define DUERAPP_USER_NAME_RECT          {20,25,190,47}
#define DUERAPP_USER_SUMMARY_RECT       {20,50,190,72}
#define DUERAPP_USER_FEEDBACK_RECT      {190,25,235,60}
#define DUERAPP_USER_LOGIN_RECT         {57,75,183,110}

#define DUERAPP_VIP_RESOURCE_RECT       {10,125,230,225}
#define DUERAPP_VIP_MASK_RECT           {10,155,230,225}
#define DUERAPP_VIP_TITLE_RECT          {10,130,216,157}
#define DUERAPP_VIP_RIGHTS1_RECT        {16,163,70,215}
#define DUERAPP_VIP_RIGHTS2_RECT        {84,160,156,215}
#define DUERAPP_VIP_RIGHTS3_RECT        {170,165,224,215}

#define DUERAPP_SUPPORT_TEXT_RECT       {0,0,240,192}
#define DUERAPP_LOGIN_OUT_BTN_RECT      {80,180,160,240}
#define DUERAPP_QUIT_TITLE_RECT         {10,20,230,50}
#define DUERAPP_QUIT_CONTENT_RECT       {25,60,215,180}
#define DUERAPP_QUIT_CANCEL_RECT        {25,187,105,217}
#define DUERAPP_QUIT_OK_RECT            {135,187,215,217}
/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          CONSTANT ARRAY                                   */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/
//  Description : 个人中心窗口的回调函数
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E CenterInfoWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           );

/*****************************************************************************/
//  Description : 个人中心退出窗口的回调函数
//  Global resource dependence : 
//  Author: shipeixuan
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E CenterQuitWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           );
/*****************************************************************************/
//  Description : 个人中心联系我们的回调函数
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E ContactUsWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           );
/*****************************************************************************/
//  Description : 个人中心关于小度的回调函数
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E AboutXiaoduWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           );                                        
/*****************************************************************************/
//  Description : 
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/

LOCAL void MMIDUERAPP_CenterOpenWin(void);

LOCAL void MMIDUERAPP_ContactUsOpenWin();

LOCAL void MMIDUERAPP_AboutXiaoduOpenWin();

/*****************************************************************************/
// 个人中心                                  
LOCAL void AddItemUserInfoRes(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id);
LOCAL void ContactUsCallBack(void);
LOCAL void AboutXiaoduCallBack(void);
LOCAL void CenterQuitCallBack(void);
LOCAL void LoginCallBack(void);
LOCAL void CenterBannerCallBack(void);
LOCAL void LoginOutCallBack(void);
LOCAL void duerapp_update_version_info(void);
LOCAL void duerapp_update_user_card(void);
LOCAL void duerapp_free_user();
LOCAL void duerapp_init_quitwin(void);
LOCAL void DrawCenterMainListItem( MMI_CTRL_ID_T ctrl_id );
/******************************************************************************/

// 定义各个窗口数组

WINDOW_TABLE( MMI_DUERAPP_CENTERINFO_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_CENTER_INFO_WIN_ID ),
    WIN_FUNC((uint32) CenterInfoWinMsg ),
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_CENTER_HOME_FORM_CTRL_ID),
#ifdef DUERAPP_USE_SLIDEPAGE
    CHILD_BUTTON_CTRL(TRUE, IMAGE_DUER_IC_POINT_2_OF_3, MMI_DUERAPP_CENTER_SLIDE_POINT_BUTTON_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM_CTRL_ID),
#endif    
    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_CENTER_HOME_FORM2_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM_CTRL_ID),
    CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,FALSE, MMI_DUERAPP_CENTER_LABEL1_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM2_CTRL_ID),
    CHILD_LABEL_CTRL(GUILABEL_ALIGN_LEFT,FALSE,MMI_DUERAPP_CENTER_LABEL2_CTRL_ID,MMI_DUERAPP_CENTER_HOME_FORM2_CTRL_ID), 
    CHILD_BUTTON_CTRL(FALSE,IMAGE_DUERAPP_CENTER_BUTTER_BG,MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM2_CTRL_ID),
#ifdef DUERAPP_USE_WATCH_CONFIG
    CHILD_ANIM_CTRL(FALSE, FALSE, MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM_CTRL_ID),
#endif
    CHILD_LIST_CTRL(FALSE, GUILIST_TYPE_NONE, MMI_DUERAPP_CENTER_HOME_LIST_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM_CTRL_ID),
    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_CENTER_HOME_FORM3_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE,IMAGE_NULL,MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, MMI_DUERAPP_CENTER_HOME_FORM3_CTRL_ID),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_CONTACT_US_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_CENTER_CONTACT_US_WIN_ID ),
    WIN_FUNC((uint32) ContactUsWinMsg ),
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_CONTACT_US_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE,IMAGE_DUERAPP_XIAODU,MMI_DUERAPP_CENTER_WEIXIN_BUTTON_CTRL_ID, MMI_DUERAPP_CONTACT_US_FORM_CTRL_ID),
    CHILD_TEXT_CTRL(FALSE, MMI_DUERAPP_CONTACT_TEXT_CTRL_ID, MMI_DUERAPP_CONTACT_US_FORM_CTRL_ID),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_OBOUT_XIAODU_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_CENTER_OBOUT_XIAODU_WIN_ID ),
    WIN_FUNC((uint32) AboutXiaoduWinMsg ),
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_ABOUT_XIAODU_FORM_CTRL_ID),
    CHILD_FORM_CTRL(FALSE, GUIFORM_LAYOUT_SBS, MMI_DUERAPP_ABOUT_XIAODU_FORM1_CTRL_ID, MMI_DUERAPP_ABOUT_XIAODU_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE,IMAGE_DUERAPP_CENTER_ABOUT_XIAODU_BIG,MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID, MMI_DUERAPP_ABOUT_XIAODU_FORM1_CTRL_ID),
    CHILD_TEXT_CTRL(FALSE, MMI_DUERAPP_ABOUT_XIAODU_TEXT1_CTRL_ID, MMI_DUERAPP_ABOUT_XIAODU_FORM_CTRL_ID),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_CENTERQUIT_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_CENTER_QUIT_WIN_ID ),
    WIN_FUNC((uint32) CenterQuitWinMsg ),
    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_QUIT_FORM_CTRL_ID),
    CHILD_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE,FALSE,MMI_DUERAPP_QUIT_LABEL_CTRL_ID,MMI_DUERAPP_QUIT_FORM_CTRL_ID),
    CHILD_TEXT_CTRL(FALSE, MMI_DUERAPP_QUIT_TEXT_CTRL_ID, MMI_DUERAPP_QUIT_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE,IMAGE_NULL,MMI_DUERAPP_QUIT_CANCEL_BUTTON_CTRL_ID, MMI_DUERAPP_QUIT_FORM_CTRL_ID),
    CHILD_BUTTON_CTRL(FALSE,IMAGE_NULL,MMI_DUERAPP_QUIT_OK_BUTTON_CTRL_ID, MMI_DUERAPP_QUIT_FORM_CTRL_ID),
    END_WIN
};

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

PUBLIC void MMIDUERAPP_CreateUserCenterWin()
{
    MMI_HANDLE_T win_handle = 0;
    MMI_WIN_ID_T win_id = MMI_DUERAPP_CENTER_INFO_WIN_ID;
    //GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    if (MMK_IsOpenWin(win_id)) {
        MMK_CloseWin(win_id);
    }
   win_handle = MMK_CreateWin((uint32*)MMI_DUERAPP_CENTERINFO_WIN_TAB, PNULL);

}

LOCAL MMI_HANDLE_T MMIDUERAPP_CreateUserContactUsWin()
{
    return MMK_CreateWin((uint32*)MMI_DUERAPP_CONTACT_US_WIN_TAB, PNULL);
}

LOCAL MMI_HANDLE_T MMIDUERAPP_CreateAboutXiaoduWin()
{
    return MMK_CreateWin((uint32*)MMI_DUERAPP_OBOUT_XIAODU_WIN_TAB, PNULL);
}

LOCAL MMI_HANDLE_T MMIDUERAPP_CreateUserCenterQuitWin()
{
    return MMK_CreateWin((uint32*)MMI_DUERAPP_CENTERQUIT_WIN_TAB, PNULL);
}
/*****************************************************************************/
//  Description : left keystoke callback
//  Global resource dependence :
//  Author:liuwenshuai
//  Note:
/*****************************************************************************/

Pt_DUER_USER_INFO *s_pt_user_info = NULL;

char s_feedback_code[20] = {0};
BOOLEAN s_if_update_banner = FALSE;

LOCAL void ContactUsCallBack(void)
{
    DUER_LOGI("ContactUsCallBack@xhn");
    MMIDUERAPP_CreateUserContactUsWin();
}

LOCAL void AboutXiaoduCallBack(void)
{
    MMIDUERAPP_CreateAboutXiaoduWin();
}

LOCAL void CenterQuitCallBack(void)
{
    DUER_LOGI("CenterQuitCallBack quit!@xhn");
    MMIDUERAPP_CreateUserCenterQuitWin();
}
LOCAL void LoginCallBack(void)
{
    int ret = -1;
    DUER_LOGI("login_test LoginCallBack !@xhn");
    // if(duerapp_is_login()){
    if (s_pt_user_info && *s_pt_user_info && duerapp_is_login()) {
        //if (!s_is_user_vip) {
            DUER_LOGI("login_test buy vip");
            ret = duer_send_link_click_url(LINKCLICK_BUY_VIP);
            if (0 == ret)
                DUER_LOGI("send buy vip link click over");
        //}
    } else {
        duerapp_user_login();
    } 
}

extern DUEROS_CENTER_CFG_ITEM_ST        *s_center_cfg;
LOCAL void CenterBannerCallBack(void)
{
    wchar *wstr_ptr = L"跳转中...";
    DUER_LOGI("%s", __FUNCTION__);

    if (s_center_cfg) {
        DUER_LOGI("%s s_center_cfg", __FUNCTION__);
        if (s_center_cfg->banner_url) {
            duer_audio_play_pause();
            duer_add_click_statistics(STATISTICS_CLICK_BANNER, 6, NULL);

            // wchar *wstr_ptr = L"跳转中...";
            duerapp_show_toast(wstr_ptr);
            DUER_LOGI("%s banner url=%s", __FUNCTION__, s_center_cfg->banner_url);
            MMIDUERAPP_OperateActionUrl(s_center_cfg->banner_url);
        }
    }
}

// 退出登录
PUBLIC void duerapp_login_out_callback(void)
{
    duerapp_free_user_info();

    #if 0 //更换接口，避免UI线程接口错误使用
    // wchar *login_out_success = L"退出成功";
    // duerapp_show_toast(login_out_success);
    if (MMK_IsOpenWin(MMI_DUERAPP_CENTER_INFO_WIN_ID)) {
        MMK_SendMsg(MMI_DUERAPP_CENTER_INFO_WIN_ID, MSG_FULL_PAINT, PNULL);  
      }
    #else
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_CENTER_INFO_WIN_ID, MSG_DUERAPP_LOGIN_OUT, PNULL, 0);
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_CENTER_INFO_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    #endif
    DUER_LOGI("duerapp_login_out_callback sucess");
}

LOCAL void OKCallBack(void)
{
    if (MMK_IsOpenWin(MMI_DUERAPP_CENTER_QUIT_WIN_ID)) {
        MMK_CloseWin(MMI_DUERAPP_CENTER_QUIT_WIN_ID);
    }
    // 无需判断，接口已经做了判断
    duerapp_user_login_out();
}

LOCAL void CancelCallBack(void)
{
    MMK_CloseWin(MMI_DUERAPP_CENTER_QUIT_WIN_ID);
}


LOCAL void AudioListCallBack(void)
{
    MMIDUERAPP_CreateIconWin(DUER_AUDIO_UNICAST_STORY);
}

LOCAL void SkillListCallBack(void)
{
    MMIDUERAPP_CreateIconWin(DUER_AUDIO_SKILL);
}

LOCAL void LessonListCallBack(void)
{
    MMIDUERAPP_CreateIconWin(DUER_AUDIO_LESSON);
}

/*****************************************************************************/

LOCAL void duerapp_check_user_info(MMI_WIN_ID_T win_id) {
    if (s_pt_user_info && *s_pt_user_info) {
        MMK_SendMsg(win_id,MSG_DUERAPP_LOGIN_USER_INFO,PNULL);
    } else {
        DUER_LOGI("duerapp_create_user_info...");
        duerapp_create_user_info();
    }
}

/*****************************************************************************/
// 个人中心
LOCAL MMI_RESULT_E CenterInfoWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{   
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    static BOOLEAN s_last_focus_sta = FALSE;
    int idx = -1;
    uint32 src_id = 0;
    wchar *login_out_success = L"退出成功";
    
    DUER_LOGI("%s msg=%x", __FUNCTION__, msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        duerapp_check_user_info(win_id);
        break; 
    case MSG_GET_FOCUS: // 窗口获得焦点
    {
        // 刷新用户信息,检查用户登录状态
        duerapp_check_user_info(win_id);
        if (!duerapp_is_login()) {
            StartLoginReqTimer();
        }
        if (!s_last_focus_sta) {
            duer_add_click_statistics(STATISTICS_CLICK_HOME_PAGE, STATISTICS_PAGE_SECOND, NULL);
        }
        s_last_focus_sta = TRUE;
    } break;
    case MSG_LOSE_FOCUS: // 窗口失去焦点
    case MSG_BACKLIGHT_TURN_OFF:
    {
        StopLoginReqTimer();
        s_last_focus_sta = FALSE;
    } break;
    case MSG_PRE_FULL_PAINT:
        break;
    case MSG_FULL_PAINT: // 窗口刷新
        MMIDUERAPP_CenterOpenWin();
        duerapp_update_user_card();
        break;
    case MSG_CLOSE_WINDOW: // 关闭窗口 
        s_if_update_banner = FALSE;
        duerapp_free_user_info();
        StopLoginReqTimer();
        break;
    case MSG_CTL_PENOK:
    {
        if (PNULL == param)
        {
            break;
        }
        src_id = ((MMI_NOTIFY_T*)param)->src_id;
        DUER_LOGI("src_id = %d", src_id);
        if (src_id == MMI_DUERAPP_CENTER_HOME_LIST_CTRL_ID)
        {
            idx = GUILIST_GetCurItemIndex(src_id);
            DUER_LOGI("list index = %d", idx);
            if ( 0 == idx)
            {
                MMIDUERAPP_CreateUserContactUsWin();
            }
            else if ( 1 == idx)
            { 
                MMIDUERAPP_CreateAboutXiaoduWin();
            }   
        }
        break;
    }
    case MSG_DUERAPP_LOGIN_USER_INFO:
        duerapp_update_user_card();
        break;  
    case MSG_DUERAPP_ANIM_PIC_UPDATE:
    {
        DUER_LOGI("CenterInfoWinMsg = MSG_DUERAPP_ANIM_PIC_UPDATE");
        // MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    }
        break;
    case MSG_APP_OK:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
    case MSG_DUERAPP_LOGIN_OUT:
        {
            duerapp_show_toast(login_out_success);
        }
        break;
    default:
		recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

LOCAL MMI_RESULT_E ContactUsWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{   
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T  ctrl_id  =  MMI_DUERAPP_CONTACT_US_FORM_CTRL_ID;
    //DUER_LOGI("CenterQUITWinMsg = %04x", msg_id);
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:

        MMIDUERAPP_ContactUsOpenWin();
        break; 
    case MSG_GET_FOCUS: //窗口获得焦点
        break;
    case MSG_LOSE_FOCUS: // 窗口失去焦点
        break;
    case MSG_FULL_PAINT: // 窗口刷新
        //WatchRec_MainwinDrawBG(); 
        break;
    case MSG_CLOSE_WINDOW: // 关闭窗口
        MMK_CloseWin(win_id);
        break;
    case MSG_DUERAPP_FEEDBACK_CALL_BACK:
        //duerapp_update_version_info();
        break;
    default:
		recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

LOCAL MMI_RESULT_E AboutXiaoduWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        duerapp_get_feedback();
        MMIDUERAPP_AboutXiaoduOpenWin();
        break; 
    case MSG_GET_FOCUS: //窗口获得焦点
        break;
    case MSG_LOSE_FOCUS: // 窗口失去焦点
        break;
    case MSG_FULL_PAINT: // 窗口刷新
        break;

    case MSG_DUERAPP_FEEDBACK_CALL_BACK:
        MMIDUERAPP_AboutXiaoduOpenWin();
        break;

    case MSG_CLOSE_WINDOW: // 关闭窗口
        MMK_CloseWin(win_id);
        break;
    default:
		recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

LOCAL MMI_RESULT_E CenterQuitWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        duerapp_init_quitwin();
        break; 
    case MSG_GET_FOCUS: //窗口获得焦点
        //MMIDUERAPP_CenterOpenWin();
        break;
    case MSG_LOSE_FOCUS: // 窗口失去焦点
        break;
    case MSG_FULL_PAINT: // 窗口刷新
        //WatchRec_MainwinDrawBG(); 
        break;
    case MSG_CLOSE_WINDOW: // 关闭窗口
        MMK_CloseWin(win_id);
        break;
    default:
		recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

/**************
 * 
 * 
 * ***************************/
LOCAL void MMIDUERAPP_CenterOpenWin() {
    MMI_HANDLE_T ctrl_id = MMI_DUERAPP_CENTER_HOME_FORM_CTRL_ID;
    MMI_HANDLE_T ctrl_id2 = MMI_DUERAPP_CENTER_HOME_FORM2_CTRL_ID;//for banner
    MMI_HANDLE_T ctrl_id3 = MMI_DUERAPP_CENTER_HOME_FORM3_CTRL_ID;//for quit

    wchar *title = L"请先登录";
    wchar *subTitle = L"未登录";
    BOOLEAN is_login = duerapp_is_login();
    wchar *button_text = is_login ? L"升级账号" : L"立即登录";

    int is_vip = 0;
    if (s_pt_user_info && *s_pt_user_info && is_login) {
        is_vip = (*s_pt_user_info)->is_vip;
    }

    {
        GUI_BG_T bg_info ={0};
        IGUICTRL_T *ictrl = NULL;
        GUI_RECT_T win_rect = {0};
        GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
#ifdef DUERAPP_USE_SLIDEPAGE
        GUI_PADDING_T padding = {10, DUERAPP_SLIDE_POINT_PADING_TOP, 10, 5};
#else
        GUI_PADDING_T padding = {10, 10, 10, 5};
#endif
        // 设置背景
        bg_info.bg_type = GUI_BG_COLOR;
        bg_info.color = MMI_BLACK_COLOR;
        GUIFORM_SetBg(ctrl_id, &bg_info);

        // GUIFORM_PermitChildBg(ctrl_id, FALSE);
        GUIFORM_PermitChildFont(ctrl_id, FALSE);
        GUIFORM_PermitChildBorder(ctrl_id,FALSE);

        ictrl = MMK_GetCtrlPtr(ctrl_id);
        // GUICTRL_SetProgress(ictrl, FALSE);

#ifdef DUERAPP_USE_SLIDEPAGE
        rect.top += DUERAPP_SLIDE_INDICATOR_HEIGHT;
#endif
        win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, MMI_DUERAPP_CENTER_INFO_WIN_ID, rect);
        GUIFORM_SetRect(ctrl_id, &win_rect);

        //set padding
        CTRLFORM_SetPadding(ctrl_id, &padding);
    }

    {
        GUI_BG_T bg_info ={0};
        GUI_PADDING_T padding = {10, 5, 10, 5};

        bg_info.color = is_login ? (is_vip ? DUERAPP_VIP_COLOR : DUERAPP_LOGIN_COLOR) : DUERAPP_NORMAL_COLOR;
        bg_info.shape = GUI_SHAPE_ROUNDED_RECT;
        bg_info.bg_type = GUI_BG_COLOR;

        GUIFORM_SetBg(ctrl_id2, &bg_info);
        CTRLFORM_SetPadding(ctrl_id2, &padding);
    }

    {
        GUIFORM_CHILD_WIDTH_T width = {240, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T height = {33, GUIFORM_CHILD_HEIGHT_FIXED};
        MMI_STRING_T title_text = {0};

        title_text.wstr_ptr = title;
        title_text.wstr_len = MMIAPICOM_Wstrlen(title);

        GUIFORM_SetChildWidth(ctrl_id2, MMI_DUERAPP_CENTER_LABEL1_CTRL_ID, &width);
        GUIFORM_SetChildHeight(ctrl_id2, MMI_DUERAPP_CENTER_LABEL1_CTRL_ID, &height);
        GUILABEL_SetFont(MMI_DUERAPP_CENTER_LABEL1_CTRL_ID, WATCH_DEFAULT_NORMAL_FONT, MMI_WHITE_COLOR);
        GUILABEL_SetAlign(MMI_DUERAPP_CENTER_LABEL1_CTRL_ID, GUILABEL_ALIGN_LEFT);
        GUILABEL_SetText(MMI_DUERAPP_CENTER_LABEL1_CTRL_ID ,&title_text, FALSE);
    }

    {
        GUIFORM_CHILD_WIDTH_T width = {185, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T height = {25, GUIFORM_CHILD_HEIGHT_FIXED};
        MMI_STRING_T subtitle_text = {0};

        subtitle_text.wstr_ptr = subTitle;
        subtitle_text.wstr_len = MMIAPICOM_Wstrlen(subTitle);

        GUIFORM_SetChildWidth(ctrl_id2, MMI_DUERAPP_CENTER_LABEL2_CTRL_ID, &width);
        GUIFORM_SetChildHeight(ctrl_id2, MMI_DUERAPP_CENTER_LABEL2_CTRL_ID, &height);
        GUILABEL_SetFont(MMI_DUERAPP_CENTER_LABEL2_CTRL_ID, WATCH_DEFAULT_NORMAL_FONT, MMI_WHITE_COLOR);
        GUILABEL_SetAlign(MMI_DUERAPP_CENTER_LABEL2_CTRL_ID, GUILABEL_ALIGN_LEFT);
        GUILABEL_SetText(MMI_DUERAPP_CENTER_LABEL2_CTRL_ID ,&subtitle_text, FALSE); 
    }

    {
        GUIFORM_CHILD_WIDTH_T width = {132, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T height = {45, GUIFORM_CHILD_HEIGHT_FIXED};
        GUI_BG_T btn_bg_rel = {0};
        GUI_FONT_ALL_T font_info = {0};
        MMI_STRING_T title_text = {0};

        btn_bg_rel.bg_type = GUI_BG_IMG;
        btn_bg_rel.img_id = IMAGE_DUERAPP_CENTER_BUTTER_BG;
        font_info.font = WATCH_DEFAULT_BIG_FONT;
        font_info.color = is_vip ? DUERAPP_VIP_BTN_COLOR : DUERAPP_LOGIN_BTN_COLOR;
        title_text.wstr_ptr = button_text;
        title_text.wstr_len = MMIAPICOM_Wstrlen(button_text);

        GUIFORM_SetChildWidth(ctrl_id2, MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, &width);
        GUIFORM_SetChildHeight(ctrl_id2, MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, &height);
        GUIBUTTON_SetTextAlign(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, ALIGN_HVMIDDLE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID,FALSE);
        GUIFORM_SetChildAlign(ctrl_id2, MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);

        GUIBUTTON_SetFg(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, &btn_bg_rel);
        GUIBUTTON_SetPressedFg(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, &btn_bg_rel);
        GUIBUTTON_SetFont(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID,&font_info);
        GUIBUTTON_SetText(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, title_text.wstr_ptr, title_text.wstr_len);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, LoginCallBack);
    }

    GUIANIM_SetBgColor(MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID, MMI_BLACK_COLOR);
#if DUERAPP_USE_WATCH_CONFIG
    if (s_if_update_banner == FALSE) {
        GUIANIM_DATA_INFO_T img_info = {0};
        img_dsc_t *resource = NULL;
        GUIANIM_CTRL_INFO_T     ctrl_info = {0};
        GUIANIM_DISPLAY_INFO_T  display_info = {0};
        MMI_HANDLE_T            anim_handle = NULL;

        // banner button
        GUIFORM_SetChildAlign(ctrl_id, MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);

        {
            GUIFORM_CHILD_WIDTH_T width = {220, GUIFORM_CHILD_WIDTH_FIXED};
            GUIFORM_CHILD_HEIGHT_T height = {68, GUIFORM_CHILD_HEIGHT_FIXED};

            GUIFORM_SetChildWidth(ctrl_id, MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID, &width);
            GUIFORM_SetChildHeight(ctrl_id, MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID, &height);
        }

        CTRLANIM_SetCallBackFunc(MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID, CenterBannerCallBack, NULL);

        resource = imageGetResource(IMG_DOWN_CENTER_BANNER, 0);
        if (resource && resource->data && resource->data_size > 0)
        {
            img_info.data_ptr = (resource)->data;
            img_info.data_size = (resource)->data_size;
            s_if_update_banner = TRUE;
        }
        else
        {
            DUER_LOGE("%s resource get err", __FUNCTION__);
            img_info.img_id = IMAGE_DUERAPP_BANNER1;
        }
        ctrl_info.is_ctrl_id        = TRUE;
        ctrl_info.ctrl_id           = MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID;
        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_auto_zoom_in = TRUE;
        display_info.is_update = TRUE;
        display_info.is_disp_one_frame = TRUE;
        display_info.is_zoom = TRUE;
        display_info.is_handle_transparent = TRUE;
        display_info.is_syn_decode = TRUE;
        GUIANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
        GUIANIM_SetVisible(MMI_DUERAPP_CENTER_BANNER_ANIM_CTRL_ID, TRUE, TRUE);
    }
#endif
    //contact us and xiaodu list
    DrawCenterMainListItem( MMI_DUERAPP_CENTER_HOME_LIST_CTRL_ID );

    //退出登录按钮
    if (duerapp_is_login()) {
        {
            GUI_PADDING_T padding = {10, 20, 10, 5};
            CTRLFORM_SetPadding(ctrl_id3, &padding);
        }

        {
            GUIFORM_CHILD_WIDTH_T width = {90, GUIFORM_CHILD_WIDTH_FIXED};
            GUIFORM_CHILD_HEIGHT_T height = {30, GUIFORM_CHILD_HEIGHT_FIXED};
            GUI_FONT_ALL_T  btn_font_info = {0};
            wchar quit_btn_text[] = L"退出登录";

            btn_font_info.font  = WATCH_DEFAULT_NORMAL_FONT;
            btn_font_info.color = MMI_WHITE_COLOR;

            GUIFORM_SetChildWidth(ctrl_id3, MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, &width);
            GUIFORM_SetChildHeight(ctrl_id3, MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, &height);
            GUIFORM_SetChildAlign(ctrl_id3, MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, GUIFORM_CHILD_ALIGN_HMIDDLE);
            GUIBUTTON_SetTextAlign(MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID,ALIGN_HVMIDDLE);
            GUIBUTTON_SetFont(MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, &btn_font_info);
            GUIBUTTON_SetHandleLong(MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, TRUE);
            GUIBUTTON_SetText(MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, quit_btn_text, MMIAPICOM_Wstrlen(quit_btn_text));
            GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, CenterQuitCallBack);
        }
    }
    MMK_SetAtvCtrl(MMI_DUERAPP_CENTER_INFO_WIN_ID, MMI_DUERAPP_CENTER_LABEL1_CTRL_ID);
}

LOCAL void MMIDUERAPP_ContactUsOpenWin() {

    GUI_BG_T        bg_info ={0};
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;

    MMI_HANDLE_T ctrl_id = MMI_DUERAPP_CONTACT_US_FORM_CTRL_ID;
    MMI_CTRL_ID_T   text_ctrl_id = MMI_DUERAPP_CONTACT_TEXT_CTRL_ID;
  
    wchar* hotLine = L"\n扫码关注微信公众号\n小度学习天地";
    GUI_BG_T bg_btn = {0};

    bg_info.bg_type = GUI_BG_COLOR;
    bg_info.color = MMI_BLACK_COLOR;
    
    //???????
    GUIFORM_SetBg(ctrl_id, &bg_info);
    GUIFORM_PermitChildBg(ctrl_id, FALSE);
    GUIFORM_PermitChildFont(ctrl_id, FALSE);
    GUIFORM_PermitChildBorder(ctrl_id,FALSE);
    GUIFORM_SetAlign(ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
    
    GUITEXT_SetAlign(text_ctrl_id, ALIGN_HMIDDLE );

    GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
    GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
    GUITEXT_SetString(text_ctrl_id, hotLine, MMIAPICOM_Wstrlen(hotLine), FALSE);

    bg_btn.bg_type = GUI_BG_IMG;
    bg_btn.img_id = IMAGE_DUERAPP_XIAODU;
    GUIBUTTON_SetBg(MMI_DUERAPP_CENTER_WEIXIN_BUTTON_CTRL_ID, &bg_btn);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_CENTER_WEIXIN_BUTTON_CTRL_ID,FALSE);
    GUIBUTTON_SetPressedBg(MMI_DUERAPP_CENTER_WEIXIN_BUTTON_CTRL_ID, &bg_btn);

    MMK_SetAtvCtrl(MMI_DUERAPP_CENTER_CONTACT_US_WIN_ID, text_ctrl_id);

}

LOCAL void MMIDUERAPP_AboutXiaoduOpenWin() {
    GUI_BG_T        bg_info ={0};
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;
    
    GUIFORM_CHILD_WIDTH_T   width = {0};
    GUIFORM_CHILD_HEIGHT_T height = {0};
    uint16 hor_space = 0;
    uint16 ver_space = 0;

    MMI_HANDLE_T ctrl_id = MMI_DUERAPP_ABOUT_XIAODU_FORM_CTRL_ID;
    MMI_HANDLE_T ctrl_id1 = MMI_DUERAPP_ABOUT_XIAODU_FORM1_CTRL_ID;
    MMI_CTRL_ID_T   text_ctrl_id = MMI_DUERAPP_ABOUT_XIAODU_TEXT1_CTRL_ID;
    GUI_BG_T bg_btn = {0};
    char verchar[200] = {0};
    int utf8_len = 0;
    wchar *wch_txt = NULL;

    bg_info.bg_type = GUI_BG_COLOR;
    bg_info.color = MMI_BLACK_COLOR;

    DUER_LOGI("MMIDUERAPP_AboutXiaoduOpenWin...");

    //???????
    GUIFORM_SetBg(ctrl_id, &bg_info);
    GUIFORM_PermitChildBg(ctrl_id, FALSE);
    GUIFORM_PermitChildFont(ctrl_id, FALSE);
    GUIFORM_PermitChildBorder(ctrl_id,FALSE);
    GUIFORM_SetAlign(ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
    GUIFORM_SetAlign(ctrl_id1, GUIFORM_CHILD_ALIGN_HMIDDLE);

    /*GUI_PADDING_T  padding;
    padding.top = 20;
  
    CTRLFORM_SetPadding(ctrl_id1, &padding);*/
    //???ù???С?????button
    hor_space = 86;
    ver_space = 25;
    CTRLFORM_SetChildSpace(ctrl_id1, MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID, &hor_space, &ver_space);
    width.type = GUIFORM_CHILD_WIDTH_FIXED;
    width.add_data = 86;
    GUIFORM_SetChildWidth(ctrl_id1,
                MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID,
                &width);
    height.type = GUIFORM_CHILD_HEIGHT_FIXED;
    height.add_data = 86;
    CTRLFORM_SetChildHeight(ctrl_id1,
                MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID,
                &height);
    bg_btn.bg_type = GUI_BG_IMG;
    bg_btn.img_id = IMAGE_DUERAPP_CENTER_ABOUT_XIAODU_BIG;
    GUIBUTTON_SetBg(MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID, &bg_btn);
    GUIBUTTON_SetRunSheen(MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID,FALSE);
    GUIBUTTON_SetPressedBg(MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID, &bg_btn);

    //?????????????????汾....
    ver_space = 20;
    CTRLFORM_SetChildSpace(ctrl_id, text_ctrl_id, PNULL, &ver_space);
    DUER_LOGI("duerapp_update_version_info...");

    GUITEXT_SetAlign(text_ctrl_id, ALIGN_HMIDDLE );
    GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
    GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);

    // "软件版本:%s\n反馈码:%s",
    sprintf(verchar,
        "\xe8\xbd\xaf\xe4\xbb\xb6\xe7\x89\x88\xe6\x9c\xac:%s\n\xe5\x8f\x8d\xe9\xa6\x88\xe7\xa0\x81:%s",
        DUERAPP_VERSION, s_feedback_code
    );
    utf8_len = strlen(verchar);
    wch_txt = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
    if (wch_txt) {
        GUI_UTF8ToWstr(wch_txt, utf8_len, verchar, utf8_len);
        GUITEXT_SetString(text_ctrl_id, wch_txt,MMIAPICOM_Wstrlen(wch_txt), TRUE);
        DUER_FREE(wch_txt);
    }

    MMK_SetAtvCtrl(MMI_DUERAPP_CENTER_OBOUT_XIAODU_WIN_ID, MMI_DUERAPP_ABOUT_XIAODU_BUTTON_CTRL_ID);

}

DUER_USER_INFO* duerapp_get_user_info(void)
{
    if (s_pt_user_info && *s_pt_user_info) {
        return *s_pt_user_info;
    } else {
        return NULL;
    }
}


LOCAL void duerapp_update_user_card(void)
{
    BOOLEAN is_login = duerapp_is_login();
    MMI_STRING_T title_text = {0};
    MMI_STRING_T subtitle_text = {0};

    int is_vip = 0;
    int utf8_len = 0;

    if (s_pt_user_info && *s_pt_user_info) {
        int vip_end_timestamp = (*s_pt_user_info)->end_timestamp;
        char *account_name = (*s_pt_user_info)->uname;
        char temp_name[128] = {0};
        wchar *wch_title_txt = NULL;
        wchar *wch_subtitle_txt = NULL;
        wchar *wch_name_txt = NULL;
        wchar *button_text = NULL;

        is_vip = (*s_pt_user_info)->is_vip;
        DUER_LOGI("name->%s, is_vip->%d, vip_end_time->%d", account_name, is_vip, vip_end_timestamp);
        {
            int utf8_len = 0;
            // 绑定账号：%s
            snprintf(temp_name, 128, "\xe7\xbb\x91\xe5\xae\x9a\xe8\xb4\xa6\xe5\x8f\xb7\xef\xbc\x9a%s", account_name);
            utf8_len = strlen(temp_name);
            wch_title_txt = DUER_CALLOC(sizeof(wchar), utf8_len + 1);
            if (wch_title_txt) {
                GUI_UTF8ToWstr(wch_title_txt, utf8_len, temp_name, utf8_len);
                title_text.wstr_ptr = wch_title_txt;
                title_text.wstr_len = MMIAPICOM_Wstrlen(title_text.wstr_ptr);
            }
        }

        if (is_vip && vip_end_timestamp > 0) {
            MMI_TM_T vip_end_tm = MMIAPICOM_Second2Tm(vip_end_timestamp - TEN_YEARS_1970_1980_SECONDS);
            // 会员到期：%d/%02d/%02d
            snprintf(temp_name, 128, "\xe4\xbc\x9a\xe5\x91\x98\xe5\x88\xb0\xe6\x9c\x9f\xef\xbc\x9a%d/%02d/%02d", vip_end_tm.tm_year, vip_end_tm.tm_mon, vip_end_tm.tm_mday);
        } else {
            // 会员到期：未开通
            snprintf(temp_name, 128, "\xe4\xbc\x9a\xe5\x91\x98\xe5\x88\xb0\xe6\x9c\x9f\xef\xbc\x9a\xe6\x9c\xaa\xe5\xbc\x80\xe9\x80\x9a");
        }

        {
            int utf8_len = strlen(temp_name);
            wch_subtitle_txt = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
            if (wch_subtitle_txt) {
                GUI_UTF8ToWstr(wch_subtitle_txt, utf8_len, temp_name, utf8_len);

                subtitle_text.wstr_ptr = wch_subtitle_txt;
                subtitle_text.wstr_len = MMIAPICOM_Wstrlen(subtitle_text.wstr_ptr);
            }
        }
        button_text = is_vip ? L"立即续费" : L"立即开通";

        GUILABEL_SetText(MMI_DUERAPP_CENTER_LABEL1_CTRL_ID ,&title_text, TRUE);
        GUILABEL_SetText(MMI_DUERAPP_CENTER_LABEL2_CTRL_ID ,&subtitle_text, TRUE);
        GUIBUTTON_SetText(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, button_text, MMIAPICOM_Wstrlen(button_text));

        if (wch_title_txt)
        {
            DUER_FREE(wch_title_txt);
            wch_title_txt = NULL;
        }
        if (wch_subtitle_txt)
        {
            DUER_FREE(wch_subtitle_txt);
            wch_subtitle_txt = NULL;
        }
        if (wch_name_txt)
        {
            DUER_FREE(wch_name_txt);
            wch_name_txt = NULL;
        }
    }
    else
    {
        wchar *button_text = is_login ? L"立即开通" : L"立即登录";

        title_text.wstr_ptr = L"绑定帐号：未绑定";
        title_text.wstr_len = MMIAPICOM_Wstrlen(title_text.wstr_ptr);
 
        subtitle_text.wstr_ptr = L"会员到期：未开通";
        subtitle_text.wstr_len = MMIAPICOM_Wstrlen(subtitle_text.wstr_ptr);

        GUILABEL_SetText(MMI_DUERAPP_CENTER_LABEL1_CTRL_ID ,&title_text, TRUE);
        GUILABEL_SetText(MMI_DUERAPP_CENTER_LABEL2_CTRL_ID ,&subtitle_text, TRUE);
        GUIBUTTON_SetText(MMI_DUERAPP_CENTER_BUTTON1_CTRL_ID, button_text, MMIAPICOM_Wstrlen(button_text));
    }

    {
        GUI_BG_T bg_info ={0};

        bg_info.color = is_login ? (is_vip ? DUERAPP_VIP_COLOR : DUERAPP_LOGIN_COLOR) : DUERAPP_NORMAL_COLOR;
        bg_info.shape = GUI_SHAPE_ROUNDED_RECT;
        bg_info.bg_type = GUI_BG_COLOR;

        GUIFORM_SetBg(MMI_DUERAPP_CENTER_HOME_FORM2_CTRL_ID, &bg_info);
    }
    GUIBUTTON_SetVisible(MMI_DUERAPP_CENTER_QUIT_BUTTON_CTRL_ID, is_login ? TRUE : FALSE, TRUE);
}


PUBLIC void duerapp_free_user_info(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    duerapp_user_info_release();
    if (s_pt_user_info && *s_pt_user_info) {
        DUER_FREE(*s_pt_user_info);
        *s_pt_user_info = NULL;
        s_pt_user_info = NULL;
    }
}

LOCAL void duerapp_init_quitwin(void)
{
    GUI_RECT_T okRect = DUERAPP_QUIT_OK_RECT;
    GUI_RECT_T cancelRect = DUERAPP_QUIT_CANCEL_RECT;
    GUI_RECT_T titleRect = DUERAPP_QUIT_TITLE_RECT;
    GUI_RECT_T contentRect = DUERAPP_QUIT_CONTENT_RECT;
    MMI_STRING_T    title_text = {0};
    GUI_FONT_ALL_T  font_info = {0};
    GUI_BG_T        bg_info ={0};
    GUI_BG_T        ok_bg_info ={0};
    GUI_BG_T        cancel_bg_info ={0};
    MMI_CTRL_ID_T   ok_btn_ctrl_id = MMI_DUERAPP_QUIT_OK_BUTTON_CTRL_ID;
    MMI_CTRL_ID_T   cancel_btn_ctrl_id = MMI_DUERAPP_QUIT_CANCEL_BUTTON_CTRL_ID;
    MMI_CTRL_ID_T   form_ctrl_id = MMI_DUERAPP_QUIT_FORM_CTRL_ID;
    MMI_CTRL_ID_T   label_ctrl_id = MMI_DUERAPP_QUIT_LABEL_CTRL_ID;
    MMI_CTRL_ID_T   text_ctrl_id = MMI_DUERAPP_QUIT_TEXT_CTRL_ID;
    GUIFORM_CHILD_WIDTH_T   button_width = {0};
    GUIFORM_CHILD_HEIGHT_T  button_height = {0};

    GUI_COLOR_T font_color = DUERAPP_QUIT_TEXT_COLOR;
    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;
    wchar *ok_button_txt = L"确认";
    wchar *cancel_button_txt = L"取消";
    wchar *remind_text = L"退出登录会导致付费内容无法使用\n确认退出吗？";
    uint16 ver_speace = 10;

    bg_info.bg_type = GUI_BG_COLOR;
    bg_info.color = MMI_BLACK_COLOR;
    font_info.font = WATCH_DEFAULT_NORMAL_FONT;
    font_info.color = MMI_WHITE_COLOR;

    GUIFORM_SetBg(form_ctrl_id, &bg_info);
    GUIFORM_PermitChildBg(form_ctrl_id, FALSE);
    GUIFORM_PermitChildFont(form_ctrl_id, FALSE);
    GUIFORM_PermitChildBorder(form_ctrl_id,FALSE);
    GUIFORM_SetSpace(form_ctrl_id, PNULL, &ver_speace);
    GUIFORM_SetAlign(form_ctrl_id,GUIFORM_CHILD_ALIGN_HMIDDLE);

    title_text.wstr_ptr = L"提示";
    title_text.wstr_len = MMIAPICOM_Wstrlen(title_text.wstr_ptr);
    GUILABEL_SetRect(label_ctrl_id, &titleRect, FALSE);
    GUILABEL_SetFont(label_ctrl_id, WATCH_DEFAULT_BIG_FONT, MMI_WHITE_COLOR);
    GUILABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_MIDDLE);
    GUILABEL_SetText(label_ctrl_id, &title_text, TRUE);

    GUITEXT_SetBg(text_ctrl_id,&bg_info);
    GUITEXT_SetRect(text_ctrl_id, &contentRect);
    GUITEXT_SetAlign(text_ctrl_id, ALIGN_HVMIDDLE );
    GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
    GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
    GUITEXT_SetString(text_ctrl_id, remind_text, MMIAPICOM_Wstrlen(remind_text), FALSE);

    button_width.add_data = 200;
    button_width.type = GUIFORM_CHILD_WIDTH_FIXED;
    button_height.add_data = 40;
    button_height.type = GUIFORM_CHILD_HEIGHT_FIXED;
    ok_bg_info.bg_type  = GUI_BG_COLOR;
    ok_bg_info.color    = DUERAPP_OK_BTN_COLOR;
    cancel_bg_info.bg_type  = GUI_BG_COLOR;
    cancel_bg_info.color    = DUERAPP_CANCEL_BTN_COLOR;
    GUIBUTTON_SetFg(ok_btn_ctrl_id, &ok_bg_info);
    GUIBUTTON_SetBg(ok_btn_ctrl_id, &ok_bg_info);
    //GUIBUTTON_SetRect(ok_btn_ctrl_id, &okRect);
    GUIFORM_SetChildWidth(form_ctrl_id, ok_btn_ctrl_id, &button_width);
    GUIFORM_SetChildHeight(form_ctrl_id, ok_btn_ctrl_id, &button_height);
    GUIBUTTON_SetTextAlign(ok_btn_ctrl_id, ALIGN_HVMIDDLE);
    GUIFORM_SetChildAlign(form_ctrl_id,ok_btn_ctrl_id,GUIFORM_CHILD_ALIGN_HMIDDLE);
    GUIBUTTON_SetRunSheen(ok_btn_ctrl_id, FALSE);
    GUIBUTTON_SetFont(ok_btn_ctrl_id, &font_info);
    GUIBUTTON_SetHandleLong(ok_btn_ctrl_id, TRUE);
    GUIBUTTON_SetCallBackFunc(ok_btn_ctrl_id, OKCallBack);
    GUIBUTTON_SetText(ok_btn_ctrl_id, ok_button_txt, MMIAPICOM_Wstrlen(ok_button_txt));

    GUIBUTTON_SetFg(cancel_btn_ctrl_id, &cancel_bg_info);
    GUIBUTTON_SetBg(cancel_btn_ctrl_id, &cancel_bg_info);
    //GUIBUTTON_SetRect(cancel_btn_ctrl_id, &cancelRect);
    GUIFORM_SetChildWidth(form_ctrl_id, cancel_btn_ctrl_id, &button_width);
    GUIFORM_SetChildHeight(form_ctrl_id, cancel_btn_ctrl_id, &button_height);
    GUIBUTTON_SetTextAlign(cancel_btn_ctrl_id, ALIGN_HVMIDDLE);
    GUIFORM_SetChildAlign(form_ctrl_id,cancel_btn_ctrl_id,GUIFORM_CHILD_ALIGN_HMIDDLE);
    GUIBUTTON_SetRunSheen(cancel_btn_ctrl_id, FALSE);
    GUIBUTTON_SetFont(cancel_btn_ctrl_id, &font_info);
    GUIBUTTON_SetHandleLong(cancel_btn_ctrl_id, TRUE);
    GUIBUTTON_SetCallBackFunc(cancel_btn_ctrl_id, CancelCallBack);
    GUIBUTTON_SetText(cancel_btn_ctrl_id, cancel_button_txt, MMIAPICOM_Wstrlen(cancel_button_txt));
}

PUBLIC void duerapp_userinfo_response(Pt_DUER_USER_INFO *pt_user_info){
    DUER_LOGI("duerapp_userinfo_response...");
    s_pt_user_info = pt_user_info;
    // MMK_duer_other_task_to_MMI(MMI_DUERAPP_CENTER_INFO_WIN_ID, MSG_DUERAPP_LOGIN_USER_INFO, PNULL, 0);
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_CENTER_INFO_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
}

PUBLIC void duerapp_feedback_callback(char *code)
{
    DUER_LOGI("duerapp_feedback_callback...%s",code);
    strcpy(s_feedback_code,code);
    MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_CENTER_OBOUT_XIAODU_WIN_ID,MSG_DUERAPP_FEEDBACK_CALL_BACK,PNULL,0);
}

LOCAL void DrawCenterMainListItem( MMI_CTRL_ID_T ctrl_id )
{
    wchar *contactUsText = L"联系我们         >";
    wchar *aboutXiaoduText = L"关于小度         >";
    MMI_STRING_T list_text = {0};
    uint16  curSelection  =  0;
    uint16 left_space = 0;
    GUILIST_SetMaxItem( ctrl_id, 2, FALSE );
    GUILIST_RemoveAllItems(ctrl_id);
    
    //set selected item
    GUILIST_SetSelectedItem(ctrl_id, curSelection, TRUE);
    //set current item
    GUILIST_SetCurItemIndex(ctrl_id,curSelection);

    //contact us
    list_text.wstr_len = MMIAPICOM_Wstrlen(contactUsText);
    list_text.wstr_ptr = contactUsText;
    WatchCOM_ListItem_Draw_1Icon_1Str(ctrl_id,IMAGE_DUERAPP_CENTER_CONTACT_US,list_text);
    //about xioadu

    list_text.wstr_len = MMIAPICOM_Wstrlen(aboutXiaoduText);
    list_text.wstr_ptr = aboutXiaoduText;
    WatchCOM_ListItem_Draw_1Icon_1Str(ctrl_id,IMAGE_DUERAPP_CENTER_ABOUT_XIAODU,list_text);

}
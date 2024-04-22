/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2020         liuwenshuai       Create                                   *
******************************************************************************/


#include "mmidisplay_data.h"
#include "std_header.h"
#include "window_parse.h"
#include "duerapp_qrwin.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "mmk_timer.h"
#include "guiimg.h"
#include "guistring.h"
#include "guires.h"
#include "guilcd.h"
#include "guitext.h"
#include "guilabel.h"
#include "duerapp_id.h"
#include "duerapp_image.h"
#include "lightduer_log.h"
#include "lightduer_memory.h"
#include "lightduer_lib.h"
#include "lightduer_log.h" 
#include "baidu_json.h"
#include "duerapp_common.h"
#include "duerapp_payload.h"
//#include "watch_commonwin_internal.h"
/*****************************************************************************/
// 默认使用的二维码版本
#define DEFAULT_QR_VERSION 3
// 默认缩放比例
#define DEFAULT_QR_SCALE   5
// 默认循环时间 ms
#define DEFAULT_QUERY_LOGIN_TIME 3000
LOCAL uint8 s_login_req_timer_id = 0;
Pt_QR_LOGIN_URL_INFO * qr_login_info = NULL;
char s_login_status_url[256] = {0};

LOCAL void StartLoginReq(void);

/*****************************************************************************/
LOCAL MMI_RESULT_E HandleQrWinMsg(
                                       MMI_WIN_ID_T       win_id,     // 窗口的ID
                                       MMI_MESSAGE_ID_E   msg_id,     // 窗口的内部消息ID
                                       DPARAM             param       // 相应消息的参数
                                       );


/*****************************************************************************/
// 文本显示窗口
WINDOW_TABLE( MMI_DUERAPP_QR_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_QR_WIN_ID ),
    WIN_FUNC((uint32) HandleQrWinMsg ),
    CREATE_TEXT_CTRL(MMI_DUERAPP_LOGIN_TIPS_CTRL_ID),
    END_WIN
}; 

/*****************************************************************************/

PUBLIC MMI_HANDLE_T MMIDUERAPP_CreateQrWin(ADD_DATA data)
{
    DUERAPP_ASSISTS_RESPONSE_TYPE_E *type = (DUERAPP_ASSISTS_RESPONSE_TYPE_E *)data;
    // MMI_STRING_T toast_str = {.wstr_ptr = L"跳转失败\n请重试",};
    MMI_STRING_T toast_str;
    toast_str.wstr_ptr = L"跳转失败\n请重试";

    if (type == NULL) {
        return 0;
    }
    
    switch (*type) {
        case DUER_ASSISTS_ERROR:
        {
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_MAIN_WIN_ID, MSG_DUERAPP_TOAST, &toast_str, sizeof(MMI_STRING_T));
            return 0;
        }

        case DUER_ASSISTS_NOLOGIN:
        {
            return MMK_CreateWin((uint32*)MMI_DUERAPP_QR_WIN_TAB, data);
        }

        case DUER_ASSISTS_LOGIN:
        {
            // return PUB_DUERAPP_AssistsWinOpen();
            return MMK_CreateWin((uint32*)MMI_DUERAPP_QR_WIN_TAB, data);
        }
    }
}
/*****************************************************************************/
LOCAL void WatchRec_MainwinDrawBG(void)
{
    GUI_POINT_T start_point = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = {0,0,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT};
    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
    GUIRES_DisplayImg(&start_point,PNULL,PNULL
            ,MMI_DUERAPP_QR_WIN_ID
            ,IMAGE_DUERAPP_QR_FRAME
            ,&lcd_dev_info);
}

/*****************************************************************************/
//  Description : handle timer
//  Global resource dependence : none
//  Author: 
//  Note:
/*****************************************************************************/
LOCAL void HandleLoginReqTimer(uint8 timer_id, uint32 param)
{
    DUER_LOGI("HandleLoginReqTimer...");
    // if (s_login_req_timer_id == timer_id)
    // {
    //     MMK_StopTimer(s_login_req_timer_id);
    //     s_login_req_timer_id = 0;
    // }
    StartLoginReq();
}
/*****************************************************************************/
//  Description : start timer
//  Global resource dependence : none
//  Author: 
//  Note:
/*****************************************************************************/
PUBLIC void StartLoginReqTimer(void)
{
    DUER_LOGI("StartLoginReqTimer...");
    if (0 != s_login_req_timer_id)
    {
        MMK_StopTimer(s_login_req_timer_id);
        s_login_req_timer_id = 0;
    }
    s_login_req_timer_id = MMK_CreateTimerCallback(DEFAULT_QUERY_LOGIN_TIME, HandleLoginReqTimer, NULL, TRUE);
}
/*****************************************************************************/
PUBLIC void StopLoginReqTimer(void)
{
    DUER_LOGI("StopLoginReqTimer...");
    if (0 != s_login_req_timer_id)
    {
        MMK_StopTimer(s_login_req_timer_id);
        s_login_req_timer_id = 0;
    }
}
/*****************************************************************************/
LOCAL void MMIDUERAPP_CreateLoginText(wchar *content)
{
    GUI_RECT_T loginRect = {10,184,230,240};
    GUI_BG_T bg = {0};
    GUI_FONT_T font_size = WATCH_DEFAULT_NORMAL_FONT;
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    MMI_STRING_T querySting = {0};

    bg.bg_type  =  GUI_BG_COLOR;
    bg.color    = MMI_BLACK_COLOR;
    GUITEXT_SetBg(MMI_DUERAPP_LOGIN_TIPS_CTRL_ID,&bg);
    GUITEXT_IsDisplayPrg(FALSE, MMI_DUERAPP_LOGIN_TIPS_CTRL_ID );
    GUITEXT_SetAlign(MMI_DUERAPP_LOGIN_TIPS_CTRL_ID, ALIGN_HMIDDLE );
    GUITEXT_SetRect(MMI_DUERAPP_LOGIN_TIPS_CTRL_ID, &loginRect);
    GUITEXT_SetClipboardEnabled(MMI_DUERAPP_LOGIN_TIPS_CTRL_ID,FALSE);
    
    GUITEXT_SetFont(MMI_DUERAPP_LOGIN_TIPS_CTRL_ID, &font_size, &font_color);
    querySting.wstr_ptr = content;
    querySting.wstr_len = MMIAPICOM_Wstrlen(querySting.wstr_ptr);
    GUITEXT_SetString(MMI_DUERAPP_LOGIN_TIPS_CTRL_ID ,querySting.wstr_ptr,querySting.wstr_len,FALSE);
}


LOCAL void StartLoginReq(void) 
{
    DUER_LOGW("StartLoginReq...");
    duerapp_query_login_status();
}

LOCAL void LoginSuccess() {
    wchar *login_success = L"登录成功";
    duerapp_show_toast(login_success);
    MMK_CloseWin(MMI_DUERAPP_QR_WIN_ID);
}

PUBLIC void MMIDUERAPP_QrLoginSuccess()
{
    // 用户登录成功后，主动获取一下用户信息
    strcpy(s_login_status_url, "");
    duerapp_create_user_info();
    #if 0 //更换接口，避免UI线程接口错误使用
    if (MMK_IsOpenWin(MMI_DUERAPP_QR_WIN_ID)) {
        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_QR_WIN_ID,MSG_DUERAPP_LOGIN_SUCCESS,NULL,0);
    }
    #else
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_QR_WIN_ID,MSG_DUERAPP_LOGIN_SUCCESS,NULL,0);
    #endif
}

PUBLIC void MMIDUERAPP_QrPaySuccess() 
{
    strcpy(s_login_status_url, "");

    #if 0 //更换接口，避免UI线程接口错误使用
    wchar *login_success = L"支付成功";
    duerapp_show_toast(login_success);
    if (MMK_IsOpenWin(MMI_DUERAPP_QR_WIN_ID)) {
        MMK_CloseWin(MMI_DUERAPP_QR_WIN_ID);
    }
    #else
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID,MSG_DUERAPP_QRPAY_SUCCESS,NULL,0);
    #endif

    // 用户支付成功后，主动刷新一下用户信息
    duerapp_create_user_info();
}


PUBLIC char *get_login_status_url() {
    if (qr_login_info && *qr_login_info) {
	    return (*qr_login_info)->url_status;
    } else if (strlen(s_login_status_url) > 0) {
        return s_login_status_url;
    }
	return NULL;
}

PUBLIC char *get_auth_or_pay_url() {
    if (qr_login_info && *qr_login_info) {
		return (*qr_login_info)->url_status;
    } 
	return NULL;
}

PUBLIC void duerapp_free_qr_info() {
    // 置空查询登录状态接口
    strcpy(s_login_status_url, "");
}
/*****************************************************************************/
LOCAL MMI_RESULT_E HandleQrWinMsg(
                                        MMI_WIN_ID_T        win_id,    //IN:
                                        MMI_MESSAGE_ID_E    msg_id,    //IN:
                                        DPARAM              param      //IN:
                                        )
{

    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    int ret = -1;
	#if 0
    qr_login_info = (Pt_QR_LOGIN_URL_INFO *)MMK_GetWinAddDataPtr(win_id);
    if (PNULL == qr_login_info || NULL == *qr_login_info)
    {
        MMK_CloseWin(win_id);
        return recode;
    }
	#endif
	
	qr_login_info = (Pt_QR_LOGIN_URL_INFO *)get_qr_url_info();
    // if (PNULL == qr_login_info || NULL == *qr_login_info)
    // {
    //     MMK_CloseWin(win_id);
    //     return recode;
    // }
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        // Always-on
        MMIDEFAULT_AllowTurnOffBackLight(FALSE);
        WatchRec_MainwinDrawBG();
        break;

        case MSG_GET_FOCUS:
        // Always-on
        MMIDEFAULT_AllowTurnOffBackLight(FALSE);
        //StartLoginReqTimer();
        break;

        case MSG_BACKLIGHT_TURN_ON:
        //StartLoginReqTimer();
        break;

        case MSG_BACKLIGHT_TURN_OFF:
        StopLoginReqTimer();
        break;

        case MSG_LOSE_FOCUS:
        // Always-on off
        MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        StopLoginReqTimer();
        break;

        case MSG_FULL_PAINT:
        WatchRec_MainwinDrawBG();
        if (qr_login_info && *qr_login_info) {
            strcpy(s_login_status_url, (*qr_login_info)->url_status);
            MMIDUERAPP_CreateLoginText((*qr_login_info)->msg);
            display_qrencoder((*qr_login_info)->url);
        } else {
            // // Always-on off
            // MMIDEFAULT_AllowTurnOffBackLight(TRUE);
            MMK_CloseWin(win_id);
        }
        break;

        case MSG_END_FULL_PAINT:
        break;

        case MSG_CTL_OK:
        case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        // // Always-on off
        // MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        break;
        case MSG_CLOSE_WINDOW:
        StopLoginReqTimer();
        if (qr_login_info && *qr_login_info) {
            // if (qr_login_info->msg != PNULL) {
            //     DUER_FREE(qr_login_info->msg);
            // }
            if ((*qr_login_info)->url != PNULL) {
                DUER_FREE((*qr_login_info)->url);
				(*qr_login_info)->url = NULL;
            }
            if ((*qr_login_info)->url_status != PNULL) {
                DUER_FREE((*qr_login_info)->url_status);
				(*qr_login_info)->url_status = NULL;
            }
            DUER_FREE(*qr_login_info);
            DUER_LOGI("DUER_FREE QR_LOGIN_URL_INFO...");
            *qr_login_info = NULL;  
			qr_login_info = NULL;
        }
        // Always-on off
        MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        break;
        case MSG_DUERAPP_LOGIN_SUCCESS:
        LoginSuccess();
        // // Always-on off
        // MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        break;
        default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return (recode);
}
/*****************************************************************************/
LOCAL void display_qrencoder(const char *url_data)
{
    wchar *error = L"二维码生成失败了";
    QRcode *code = NULL;
    if (!url_data) {
        duerapp_show_toast(error);
        return;
    }
    //const char *test_data = "https://www.baidu.com";
    code = QRcode_encodeString(url_data, DEFAULT_QR_VERSION, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (code == NULL) {
        DUER_LOGW("QRcode_encodeString error!");
        return;
    }
    MMIDUERAPP_DisplayQrImage(code);
    QRcode_free(code);
	StartLoginReqTimer();
}

/*****************************************************************************/
LOCAL void MMIDUERAPP_DisplayQrImage(QRcode *code)
{
    // 绘制二维码
    GUI_LCD_DEV_INFO    lcd_dev={GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    int offset_x   = 48;
    int offset_y   = 29;
    GUI_RECT_T box = {0};
    unsigned char *row = NULL;
    int i = 0, j = 0;

    for (i = 0; i < code->width; i++) {
        row = code->data + (i * code->width);
        for (j=0; j < code->width; j++) {
            if (row[j] & 0x1) {
                box.top = offset_y + i * DEFAULT_QR_SCALE;
                box.bottom = offset_y + (i+1) * DEFAULT_QR_SCALE;
                box.left = offset_x + j * DEFAULT_QR_SCALE;
                box.right = offset_x + (j+1) * DEFAULT_QR_SCALE;
                LCD_FillRect(&lcd_dev,box,MMI_BLACK_COLOR);
            }
        }
    }
}

/*****************************************************************************/
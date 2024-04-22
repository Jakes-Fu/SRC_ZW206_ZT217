// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_operate.c
 * Auth: Liuwenshuai (liuwenshuai@baidu.com)
 * Desc: duerapp operate.
 */
/******************************************************************************/
#include "mmidisplay_data.h"
#include "std_header.h"
#include "window_parse.h"
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
#include "guiform.h"
#include "guibutton.h"
#include "duerapp_id.h"
#include "duerapp_image.h"
#include "duerapp_common.h"
#include "duerapp_payload.h"
#include "duerapp_center.h"
#include "duerapp_login.h"
#include "duerapp_http.h"
#include "duerapp_statistics.h"
#include "duerapp_openapi.h"
#include "duerapp_res_down.h"
#include "duerapp_operate.h"
#include "duerapp_operate_comqr.h"
#include "duerapp_img_down.h"
#include "duerapp_main.h"

#include "lightduer_log.h"
#include "lightduer_lib.h"
#include "lightduer_dcs.h"
#include "lightduer_memory.h"
#include "baidu_json.h"
#include "lightduer_profile.h"
#include "lightduer_thread.h"
#include "lightduer_http_client.h"
#include "lightduer_http_client_ops.h"
#include "lightduer_dcs_router.h"
// #include "lightduer_timestamp.h"

/*****************************************************************************/

#define DUERAPP_OPERATE_WITH_PIC            1   //rel need 1
#define DUERAPP_OPERATE_FAKE                0   //rel need 0
#define DUERAPP_OPERATE_DEBUG               0   //rel need 0
#define DUERAPP_OPERATE_LONGPIC_URL         0   //rel need 0
#define DUERAPP_OPERATE_USE_BTN             0   //rel need 0
#define DUERAPP_LONG_PIC_USE_BTN            0   //rel need 0

#define OP_URL_DUEROS_PRE_TEXT              "dueros://"
#define OP_URL_NATIVE_PRE_TEXT              "RTOSnative://"

#define DUERAPP_WATCH_BIG_FONT              WATCH_DEFAULT_BIG_FONT
#define DUERAPP_WATCH_NORMAL_FONT           WATCH_DEFAULT_NORMAL_FONT

#define DUERAPP_MIGU_COLSE_RECT             {175,  10,  230,  56  }
#define DUERAPP_MIGU_PIC_RECT               {17,   10,  223,  230 }
#define DUERAPP_OPERATE_TITLE_RECT          {10,   15,  230,  40  }
#define DUERAPP_OPERATE_SUBTITLE_RECT       {10,   45,  230,  75  }
#define DUERAPP_OPERATE_BUTTON_RECT         {40,   190, 200,  230 }
#define DUERAPP_OPERATE_CONTENT_RECT        {5,    45,  235,  189 }
#define DUERAPP_OPERATE_PIC_ANIM_RECT       {36,   14,  203,  181 }
#define DUERAPP_OPERATE_PIC_CLOSE_RECT      {203,  3,   238,  38  }
#define DUERAPP_OPERATE_PIC_FULL_RECT       {0,    0,   239,  239 }
#define DUERAPP_OP_LONGPIC_ANIM_RECT        {40,   20,  200,  160 }
#define DUERAPP_OP_LONGPIC_BUTTON_RECT      {40,   190, 200,  230 }

typedef enum
{
    OPERATE_IMG_DOWN_NONE = 0,
    OPERATE_IMG_DOWN_WINDOW_PIC,
    OPERATE_IMG_DOWN_LONG_PIC,
} operate_img_down_type_e;

typedef enum
{
    OPERATE_ACT_URL_NONE = 0,
    OPERATE_ACT_URL_DUEROS,
    OPERATE_ACT_URL_NATIVE,
} operate_action_url_type_e;

typedef struct
{
    img_dsc_t                 *op_img;
    BOOLEAN                   op_anim_update;

    img_dsc_t                 *longpic_img;
    BOOLEAN                   longpic_update;
} operate_params_t;

typedef struct 
{
    int                       activity_id;
    // unsigned char *content;
    // char format[8];
    // unsigned int len;
    // duerapp_http_upload_cb_t cb;
    // void *user_data;
} get_activity_list_params_t;

/*****************************************************************************/

LOCAL MMI_RESULT_E HandleOperateWinMsg(
    MMI_WIN_ID_T       win_id,     // 窗口的ID
    MMI_MESSAGE_ID_E   msg_id,     // 窗口的内部消息ID
    DPARAM             param       // 相应消息的参数
);
LOCAL MMI_RESULT_E HandleOpLongpicWinMsg(
    MMI_WIN_ID_T       win_id,     // 窗口的ID
    MMI_MESSAGE_ID_E   msg_id,     // 窗口的内部消息ID
    DPARAM             param       // 相应消息的参数
);
LOCAL MMI_RESULT_E HandleMiguWinMsg(
    MMI_WIN_ID_T       win_id,     // 窗口的ID
    MMI_MESSAGE_ID_E   msg_id,     // 窗口的内部消息ID
    DPARAM             param       // 相应消息的参数
);

LOCAL void MMIDUERAPP_OperateWinOpen(MMI_WIN_ID_T win_id);
LOCAL void duerapp_free_operate_notify(void);

LOCAL void OperateImageArtFree(int type);
LOCAL void OperateImageArtDownloadCb(img_dsc_t *img, void *down_param);
LOCAL void OperateImageDownArtPicture(char *url, int type);
LOCAL void UpdateOperateImage(void);

LOCAL int32 getOperationActionType(char *url);
LOCAL void httpGetActivityListRequest(int activity_id);
LOCAL void MMIDUERAPP_OperateNativeAction(char *json_str);
/*****************************************************************************/

P_DUEROS_OPERATE_NOTIFY         *s_pt_operate_notify;
LOCAL operate_params_t          s_operate_params        = {0};

LOCAL volatile BOOLEAN          s_get_activity_list_inprogress  = FALSE;
static char                     *s_http_response_body           = NULL;     // 动态申请http返回结果
static DUEROS_OPERATE_NOTIFY    *s_activity_operation           = NULL;

/*****************************************************************************/

// 图片弹窗窗口
WINDOW_TABLE( MMI_DUERAPP_OPERATE_WIN_TAB ) =
{
#if (DUERAPP_OPERATE_WITH_PIC == 1)
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_OPERATE_WIN_ID ),
    WIN_FUNC((uint32) HandleOperateWinMsg ),
    CREATE_ANIM_CTRL(MMI_DUERAPP_OPERATE_IMG_ANIM_CTRL_ID, MMI_DUERAPP_OPERATE_WIN_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_OPERATE_CLICK_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_CLOSE, MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID),
    END_WIN
#else
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_OPERATE_WIN_ID ),
    WIN_FUNC((uint32) HandleOperateWinMsg ),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_OPERATE_TITLE_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID),
    //CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_OPERATE_FORM_CTRL_ID),
    //CHILD_TEXT_CTRL(FALSE, MMI_DUERAPP_OPERATE_TEXT_CTRL_ID, MMI_DUERAPP_OPERATE_FORM_CTRL_ID),
    //CHILD_BUTTON_CTRL(FALSE, IMAGE_NULL, MMI_DUERAPP_OPERATE_BUTTON_CTRL_ID, MMI_DUERAPP_OPERATE_FORM_CTRL_ID),
    CREATE_TEXT_CTRL(MMI_DUERAPP_OPERATE_CONTENT_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_OPERATE_CLICK_CTRL_ID),
    END_WIN
#endif
};

//长图页面
WINDOW_TABLE( MMI_DUERAPP_OP_LONGPIC_WIN_TAB ) =
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_OP_LONGPIC_WIN_ID ),
    WIN_FUNC((uint32) HandleOpLongpicWinMsg ),

    CREATE_FORM_CTRL(GUIFORM_LAYOUT_ORDER, MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID),
        CHILD_ANIM_CTRL(FALSE, TRUE, MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID, MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID),
        #if (1 == DUERAPP_LONG_PIC_USE_BTN)
            CHILD_BUTTON_CTRL(TRUE, IMAGE_NULL, MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID),
        #endif

    END_WIN
};

// 咪咕弹框
WINDOW_TABLE( MMI_DUERAPP_MIGU_WIN_TAB ) =
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_MIGU_WIN_ID ),
    WIN_FUNC((uint32) HandleMiguWinMsg ),
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_MIGU_BG, MMI_DUERAPP_MIGU_PIC_BUTTON_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_CLOSE, MMI_DUERAPP_MIGU_CLOSE_BUTTON_CTRL_ID),
    END_WIN
};

/*****************************************************************************/

PUBLIC void MMIDUERAPP_CreateOperateWin(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    MMK_CreateWin((uint32 *)MMI_DUERAPP_OPERATE_WIN_TAB, PNULL);
}

PUBLIC void MMIDUERAPP_CreateOpLongpicWin(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    MMK_CreateWin((uint32 *)MMI_DUERAPP_OP_LONGPIC_WIN_TAB, PNULL);
}

PUBLIC void MMIDUERAPP_CreateMiguWin(char *url)
{
    char *migu_link_url = NULL;
    MMI_HANDLE_T result = (MMI_HANDLE_T)0;
    if (url) {
        migu_link_url = DUER_STRDUP(url);
        if (!migu_link_url) {
            DUER_LOGE("alloc memory for migu_link_url failed");
            return;
        }
    }
    result = MMK_CreateWin((uint32 *)MMI_DUERAPP_MIGU_WIN_TAB, migu_link_url);
    if (result == 0)
    {
        DUER_LOGE("create MIGU_WIN_TAB failed");
        // migu_link_url
        if (migu_link_url) {
            DUER_FREE(migu_link_url);
            migu_link_url = NULL;
        }
    }
}

PUBLIC void MMIDUERAPP_OperateActionUrl(char *url)
{
    int32 url_type = 0;
    char *pos = NULL;
    char *json_pos = NULL;

    DUER_LOGI("%s", __FUNCTION__);
    if (!url) {
        return;
    }

#if (1 == DUERAPP_OPERATE_DEBUG)
    // char *test_url = "RTOSnative://{\"url_type\":4,\"tag\":\"singer\",\"activityID\":67}";
    // char *test_url = "RTOSnative://{\"url_type\":5,\"text\":\"测试测试请扫描二维码\",\"url\":\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wx7f0107df39d44f1f&response_type=code&scope=snsapi_base&state=&component_appid=wx95369199535b660d&redirect_uri=https%3A%2F%2Fduer-kids.baidu.com%2Fwechat%2Fwatch%2F%23%2FvipMall%3Ffrom%3Dpromoted_pay%26deviceUuid%3D6e0ef1f4b4c6c7cd2ae0b21fbcf7c31aa26a309ae5ae345497d6cebebaec225e%26clientId%3Drt1NjR38mf2eBFjrhlEFPMrOm8mT3ehN#wechat_redirect\"}";
    char *test_url = "dueros://0a7d4511-26d3-78ab-9652-2781dd6a073f/vipCode?activityId=117";
    url = test_url;
#endif

    DUER_LOGI("%s url=%s", __FUNCTION__, url);

    url_type = getOperationActionType(url);
    DUER_LOGI("%s operate action type=%d", __FUNCTION__, url_type);

    if (OPERATE_ACT_URL_DUEROS == url_type) {
        duer_dcs_dialog_cancel();
        duer_send_link_click_url(url);
    } else if (OPERATE_ACT_URL_NATIVE == url_type) {
        pos = DUER_STRSTR(url, OP_URL_NATIVE_PRE_TEXT);
        if (!pos) {
            DUER_LOGI("%s native pos err", __FUNCTION__);
            return;
        }
        json_pos = pos + strlen(OP_URL_NATIVE_PRE_TEXT);
        DUER_LOGI("%s json_str=%s", __FUNCTION__, json_pos);
        MMIDUERAPP_OperateNativeAction(json_pos);
    }
}

LOCAL void OperateClickButtonCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);

    if ((s_pt_operate_notify != NULL) &&
        ((*s_pt_operate_notify) != NULL) &&
        ((*s_pt_operate_notify)->link_url != NULL)) {

        duer_audio_play_pause();

        duer_add_click_statistics(STATISTICS_CLICK_START_OPERATE, 0, NULL);

        MMIDUERAPP_OperateActionUrl((*s_pt_operate_notify)->link_url);
    }
    MMK_CloseWin(MMI_DUERAPP_OPERATE_WIN_ID);
}

LOCAL void OperatePicButtonCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    DUER_LOGI("same deal with click button cb");
    OperateClickButtonCallBack();
}

LOCAL void OperateCloseButtonCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);

    MMK_CloseWin(MMI_DUERAPP_OPERATE_WIN_ID);
}

LOCAL void MiguCloseButtonCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);

    MMK_CloseWin(MMI_DUERAPP_MIGU_WIN_ID);
}


LOCAL void MiguPicButtonCallBack(void)
{
    char *migu_link_url = MMK_GetWinAddDataPtr(MMI_DUERAPP_MIGU_WIN_ID);
    char *temp_url = NULL;
    // migu_link_url is freed in CloseWindow event,
    // so we need dup, then send event and finally free it.
    if (migu_link_url) {
        DUER_LOGI("migu_link_url = %s", migu_link_url);
        temp_url = DUER_STRDUP(migu_link_url);
    }

    MMK_CloseWin(MMI_DUERAPP_MIGU_WIN_ID);
    duer_dcs_dialog_cancel();
    if (temp_url) {
        duer_dcs_on_link_clicked(temp_url);
        DUER_FREE(temp_url);
        temp_url = NULL;
    }
}

/*****************************************************************************/

LOCAL void WatchRec_MainwinDrawBG(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
}


LOCAL void MMIDUERAPP_MiguWinDrawBG(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T    close_ctrl_id      = MMI_DUERAPP_MIGU_CLOSE_BUTTON_CTRL_ID;
    MMI_CTRL_ID_T    migu_ctrl_id       = MMI_DUERAPP_MIGU_PIC_BUTTON_CTRL_ID;
    GUI_RECT_T       closeRect          = DUERAPP_MIGU_COLSE_RECT;
    GUI_RECT_T       miguRect           = DUERAPP_MIGU_PIC_RECT;
    GUI_POINT_T      start_point        = {0};
    int32 bg_color                      = RGB8882RGB565(0x000000);
    GUI_LCD_DEV_INFO lcd_dev_info       = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect                     = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};

    LCD_FillArgbRect(&lcd_dev_info, rect, bg_color,77);

    GUIBUTTON_SetRect(close_ctrl_id, &closeRect);
    GUIBUTTON_SetHandleLong(close_ctrl_id, TRUE);
    GUIBUTTON_SetRunSheen(close_ctrl_id,FALSE);
    GUIBUTTON_SetCallBackFunc(close_ctrl_id, MiguCloseButtonCallBack);

    GUIBUTTON_SetRect(migu_ctrl_id, &miguRect);
    GUIBUTTON_SetHandleLong(migu_ctrl_id, TRUE);
    GUIBUTTON_SetRunSheen(migu_ctrl_id,FALSE);
    GUIBUTTON_SetCallBackFunc(migu_ctrl_id, MiguPicButtonCallBack);
}

/*****************************************************************************/

LOCAL void MMIDUERAPP_OperateWinOpen(MMI_WIN_ID_T win_id)
{

#if (DUERAPP_OPERATE_WITH_PIC == 1)

    GUI_RECT_T          animRect    = DUERAPP_OPERATE_PIC_ANIM_RECT;
    GUI_RECT_T          picFullRect = DUERAPP_OPERATE_PIC_FULL_RECT;
    BOOLEAN             has_btn     = FALSE;
    char *              img_url         = NULL;
    GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    GUIANIM_DATA_INFO_T     img_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    MMI_HANDLE_T            anim_handle = NULL;
    GUI_RECT_T btnRect = animRect;

    MMI_STRING_T        button_txt      = {0};
    GUI_FONT_ALL_T      btn_font_info   = {DUERAPP_WATCH_NORMAL_FONT, MMI_RED_COLOR};
    GUI_BG_T            bg_btn          = {0};
    GUI_RECT_T          buttonRect      = DUERAPP_OPERATE_BUTTON_RECT;

    img_info.img_id = IMAGE_DUERAPP_RECT_ICON_LOAD;
    DUER_LOGI("%s", __FUNCTION__);
    {
        #if (1 == DUERAPP_OPERATE_USE_BTN)
            #if (1 == DUERAPP_OPERATE_FAKE)
                has_btn = TRUE;
            #else
                if (!s_pt_operate_notify || !(*s_pt_operate_notify)) {
                    DUER_LOGI("%s", __FUNCTION__);
                }
                has_btn = (*s_pt_operate_notify)->show_button;
            #endif
        #else
            has_btn = FALSE;
        #endif

        if (has_btn) {

        } else {
            animRect = picFullRect;
        }
        DUER_LOGI("has_btn=%d", has_btn);
    }

    /* image anim */
    {

        GUIAPICTRL_SetBothRect(MMI_DUERAPP_OPERATE_IMG_ANIM_CTRL_ID, &animRect);

        // 增加默认图
        ctrl_info.is_ctrl_id        = TRUE;
        ctrl_info.ctrl_id           = MMI_DUERAPP_OPERATE_IMG_ANIM_CTRL_ID;
        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_syn_decode = TRUE;
        GUIANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);


        #if (1 == DUERAPP_OPERATE_FAKE)
            img_url = "https://iot-paas-static.cdn.bcebos.com/XTC/hOlng1KQN3nvGI8DV4Gf7kb0G49os5cp/240/icon_vip_rights.png";
        #else
            if (s_pt_operate_notify && *s_pt_operate_notify) {
                if ((*s_pt_operate_notify)->img_url) {
                    img_url = (*s_pt_operate_notify)->img_url;
                }
            }
        #endif
        if (img_url) {
            DUER_LOGI("img_url=%s", img_url);
            OperateImageDownArtPicture(img_url, OPERATE_IMG_DOWN_WINDOW_PIC);
        }
    }

    /* pic button */
    {
        GUIBUTTON_SetRect(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, &btnRect);
        if (has_btn) {

        } else {
            GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, OperatePicButtonCallBack);
        }
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, FALSE);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, TRUE);
        GUIAPICTRL_SetState(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);
        GUIAPICTRL_SetBothRect(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, &btnRect);
        GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, TRUE, FALSE); //GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_IMG_BUTTON_CTRL_ID, FALSE, FALSE);
    }

    /* click button */
    {

        bg_btn.bg_type = GUI_BG_COLOR;
        bg_btn.color = MMI_GREEN_COLOR;

        #if (1 == DUERAPP_OPERATE_FAKE)
            button_txt.wstr_ptr = L"测试按键";
            bg_btn.color = MMI_WHITE_COLOR;
        #else
            if (s_pt_operate_notify && *s_pt_operate_notify) {
            button_txt.wstr_ptr = (*s_pt_operate_notify)->button_text;
            if ((*s_pt_operate_notify)->button_color) {
                btn_font_info.color = RGB8882RGB565((*s_pt_operate_notify)->button_color);
            }
            if ((*s_pt_operate_notify)->button_bg) {
                bg_btn.color = RGB8882RGB565((*s_pt_operate_notify)->button_bg);
            }
            }
        #endif

        if (has_btn) {
            button_txt.wstr_len = MMIAPICOM_Wstrlen(button_txt.wstr_ptr);

            GUIBUTTON_SetFg(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &bg_btn);
            GUIBUTTON_SetBg(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &bg_btn);
            GUIBUTTON_SetRect(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &buttonRect);
            GUIBUTTON_SetTextAlign(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, ALIGN_HVMIDDLE);
            GUIBUTTON_SetRunSheen(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, FALSE);
            GUIBUTTON_SetFont(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &btn_font_info);
            GUIBUTTON_SetHandleLong(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, TRUE);
            GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, OperateClickButtonCallBack);
            GUIBUTTON_SetText(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, button_txt.wstr_ptr, button_txt.wstr_len);
            GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, TRUE, FALSE);
        } else {
            GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, FALSE, FALSE);
        }
    }

    /* close button */
    {
        GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, FALSE, FALSE);

        // GUI_RECT_T closeBtnRect = DUERAPP_OPERATE_PIC_CLOSE_RECT;
        // GUIBUTTON_SetRect(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, &closeBtnRect);
        // GUIBUTTON_SetHandleLong(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, TRUE);
        // GUIBUTTON_SetRunSheen(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, FALSE);
        // GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, OperateCloseButtonCallBack);
        // GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, TRUE, TRUE);
    }
#else
    MMI_STRING_T button_txt = {0};
    MMI_STRING_T title_text = {0};
    MMI_STRING_T subtitle_text = {0};
    MMI_STRING_T querySting = {0};
    GUI_RECT_T titleRect = DUERAPP_OPERATE_TITLE_RECT;
    GUI_RECT_T subtitleRect = DUERAPP_OPERATE_SUBTITLE_RECT;
    GUI_RECT_T buttonRect = DUERAPP_OPERATE_BUTTON_RECT;
    GUI_RECT_T contentRect = DUERAPP_OPERATE_CONTENT_RECT;
    BOOLEAN has_btn = FALSE;

    GUI_BG_T bg = {0};
    GUI_FONT_T font_size = DUERAPP_WATCH_BIG_FONT;
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    GUI_BG_T bg_btn = {0};
    GUI_FONT_ALL_T  btn_font_info = {DUERAPP_WATCH_NORMAL_FONT, MMI_RED_COLOR};
    GUI_COLOR_T title_color = MMI_WHITE_COLOR;
    GUI_COLOR_T subtitle_color = MMI_WHITE_COLOR;

    bg.bg_type  = GUI_BG_COLOR;
    bg.color    = RGB8882RGB565(0x00000000);
    bg_btn.bg_type = GUI_BG_COLOR;
    bg_btn.color = MMI_GREEN_COLOR;

    #if (1 == DUERAPP_OPERATE_FAKE)
        title_text.wstr_ptr = L"\x4e3b\x6807\x9898"; //主标题
        subtitle_text.wstr_ptr = L"\x5b50\x6807\x9898\x5b50\x6807\x9898\x5b50\x6807\x9898\x5b50\x6807\x9898";//子标题
        querySting.wstr_ptr = L"\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9\x5185\x5bb9"; //内容
        button_txt.wstr_ptr = L"\x6309\x952e"; //按键
        has_btn = TRUE;
    #else
    if (s_pt_operate_notify && *s_pt_operate_notify) {
        title_text.wstr_ptr = ((*s_pt_operate_notify)->title != NULL) ? (*s_pt_operate_notify)->title : L"\x0020";
        subtitle_text.wstr_ptr = (*s_pt_operate_notify)->subtitle;//could be NULL
        querySting.wstr_ptr = (*s_pt_operate_notify)->content;
        button_txt.wstr_ptr = (*s_pt_operate_notify)->button_text;
        if ((*s_pt_operate_notify)->title_color) {
            title_color = RGB8882RGB565((*s_pt_operate_notify)->title_color);
        }
        if ((*s_pt_operate_notify)->subtitle_color) {
            subtitle_color = RGB8882RGB565((*s_pt_operate_notify)->subtitle_color);
        }
        if ((*s_pt_operate_notify)->content_color) {
            font_color = RGB8882RGB565((*s_pt_operate_notify)->content_color);
        }
        if ((*s_pt_operate_notify)->button_color) {
            btn_font_info.color = RGB8882RGB565((*s_pt_operate_notify)->button_color);
        }
        if ((*s_pt_operate_notify)->button_bg) {
            bg_btn.color = RGB8882RGB565((*s_pt_operate_notify)->button_bg);
        }
        has_btn = (*s_pt_operate_notify)->show_button;
    }
    #endif
    
    title_text.wstr_len = MMIAPICOM_Wstrlen(title_text.wstr_ptr);
    subtitle_text.wstr_len = MMIAPICOM_Wstrlen(subtitle_text.wstr_ptr);
    querySting.wstr_len = MMIAPICOM_Wstrlen(querySting.wstr_ptr);
    button_txt.wstr_len = MMIAPICOM_Wstrlen(button_txt.wstr_ptr);

    //title
    GUILABEL_SetRect(MMI_DUERAPP_OPERATE_TITLE_CTRL_ID, &titleRect, FALSE);
    GUILABEL_SetFont(MMI_DUERAPP_OPERATE_TITLE_CTRL_ID, DUERAPP_WATCH_NORMAL_FONT, title_color);
    //GUILABEL_SetAlign(MMI_DUERAPP_OPERATE_TITLE_CTRL_ID, GUILABEL_ALIGN_MIDDLE);
    GUILABEL_SetText(MMI_DUERAPP_OPERATE_TITLE_CTRL_ID, &title_text, TRUE);

    // 如果content有内容的话，就是一种样式（title+subtile+content）
    // 没有的话就是第二种，title + subtitle，这个时候subtitle作为内容展示
    if (querySting.wstr_ptr != NULL) {
        contentRect.top += 35;
        if (subtitle_text.wstr_ptr != NULL) {
            GUILABEL_SetRect(MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID, &subtitleRect, FALSE);
            GUILABEL_SetFont(MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID, DUERAPP_WATCH_BIG_FONT, subtitle_color);
            //GUILABEL_SetAlign(MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID, GUILABEL_ALIGN_MIDDLE);
            GUILABEL_SetText(MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID, &subtitle_text, TRUE);
            GUILABEL_SetVisible(MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID, TRUE, FALSE);
        } else {
            GUILABEL_SetVisible(MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID, FALSE, FALSE);
        }
    } else {
        GUILABEL_SetVisible(MMI_DUERAPP_OPERATE_SUBTITLE_CTRL_ID, FALSE, FALSE);
        if (subtitle_text.wstr_ptr != NULL) {
            querySting = subtitle_text;
            if (subtitle_color != MMI_WHITE_COLOR) {
                font_color = subtitle_color;
            }
        }
    }

    GUITEXT_SetRect(MMI_DUERAPP_OPERATE_CONTENT_CTRL_ID, &contentRect);
    GUITEXT_SetBg(MMI_DUERAPP_OPERATE_CONTENT_CTRL_ID, &bg);
    GUITEXT_SetAlign(MMI_DUERAPP_OPERATE_CONTENT_CTRL_ID, ALIGN_HMIDDLE );
    GUITEXT_SetClipboardEnabled(MMI_DUERAPP_OPERATE_CONTENT_CTRL_ID, FALSE);
    GUITEXT_SetFont(MMI_DUERAPP_OPERATE_CONTENT_CTRL_ID, &font_size, &font_color);
    GUITEXT_SetString(MMI_DUERAPP_OPERATE_CONTENT_CTRL_ID, querySting.wstr_ptr, querySting.wstr_len, FALSE);

    if (has_btn) {
        GUIBUTTON_SetFg(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &bg_btn);
        GUIBUTTON_SetBg(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &bg_btn);
        GUIBUTTON_SetRect(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &buttonRect);
        GUIBUTTON_SetTextAlign(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, ALIGN_HVMIDDLE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, FALSE);
        GUIBUTTON_SetFont(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, &btn_font_info);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, TRUE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, OperateClickButtonCallBack);
        GUIBUTTON_SetText(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, button_txt.wstr_ptr, button_txt.wstr_len);
        GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, TRUE, FALSE);
    } else {
        GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_CLICK_CTRL_ID, FALSE, FALSE);
    }
#endif
}

/*****************************************************************************/

LOCAL MMI_RESULT_E HandleOperateWinMsg(
    MMI_WIN_ID_T        win_id,    //IN:
    MMI_MESSAGE_ID_E    msg_id,    //IN:
    DPARAM              param      //IN:
)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    DUER_LOGI("msg_id=0x%x", msg_id);
    s_pt_operate_notify = (P_DUEROS_OPERATE_NOTIFY *)duerapp_get_operate_notify();
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            WatchRec_MainwinDrawBG();

            if (s_pt_operate_notify && *s_pt_operate_notify)
            {
                MMIDUERAPP_OperateWinOpen(win_id);
            }
            else
            {
                MMK_CloseWin(win_id);
            }
        }
        break;

    case MSG_FULL_PAINT:
        WatchRec_MainwinDrawBG();

        // if (s_pt_operate_notify && *s_pt_operate_notify)
        // {
        //     MMIDUERAPP_OperateWinOpen(win_id);
        // }
        // else
        // {
        //     MMK_CloseWin(win_id);
        // }
        break;

    case MSG_CTL_PENOK:
        DUER_LOGI("duerapp operate result->MSG_CTL_PENOK");
        break;

    case MSG_END_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        DUER_LOGI("%s", __FUNCTION__);
        OperateImageArtFree(OPERATE_IMG_DOWN_WINDOW_PIC);
        OperateImageArtFree(OPERATE_IMG_DOWN_LONG_PIC);
        duerapp_free_operate_notify();
        DUER_LOGI("%s", __FUNCTION__);
        break;
    case MSG_DUERAPP_ANIM_PIC_UPDATE:
        UpdateOperateImage();
        // if (MMK_IsOpenWin(MMI_DUERAPP_OPERATE_WIN_ID) && MMK_IsFocusWin(MMI_DUERAPP_OPERATE_WIN_ID)) {
        //     MMK_SendMsg(MMI_DUERAPP_OPERATE_WIN_ID, MSG_FULL_PAINT, PNULL);
        // }
        break;
    case MSG_NOTIFY_ANIM_DISPLAY_IND:
        if (MMK_IsOpenWin(MMI_DUERAPP_OPERATE_WIN_ID) && MMK_IsFocusWin(MMI_DUERAPP_OPERATE_WIN_ID)) {
            MMK_SendMsg(MMI_DUERAPP_OPERATE_WIN_ID, MSG_FULL_PAINT, PNULL);
        }
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return (recode);
}

/*****************************************************************************/

LOCAL MMI_RESULT_E HandleMiguWinMsg(
    MMI_WIN_ID_T        win_id,    //IN:
    MMI_MESSAGE_ID_E    msg_id,    //IN:
    DPARAM              param      //IN:
)
{
    char *migu_link_url = MMK_GetWinAddDataPtr(win_id);
    MMI_RESULT_E recode = MMI_RESULT_TRUE;

    DUER_LOGI("msg_id=0x%x", msg_id);
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
    {
        DUER_LOGD("MSG_OPEN_WINDOW, MiguWinMsg, migu_link_url=%x", migu_link_url);
        if (PNULL == migu_link_url)
        {
            DUER_LOGE("migu_link_url is NULL, close window");
            MMK_CloseWin(win_id);
        }
        break;
    }
    case MSG_FULL_PAINT:
        MMIDUERAPP_MiguWinDrawBG(win_id);
        break;

    case MSG_CTL_PENOK:

        break;

    case MSG_END_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        DUER_LOGD("MSG_CLOSE_WINDOW of MiguWin, param=%x", migu_link_url);
        if (migu_link_url) {
            MMK_FreeWinAddData(win_id);
            migu_link_url = NULL;
        }
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return (recode);
}

/*****************************************************************************/

LOCAL void duerapp_free_operate(P_DUEROS_OPERATE_NOTIFY *operate_notify)
{
    if (operate_notify && *operate_notify)
    {
        if ((*operate_notify)->title)
        {
            DUER_FREE((*operate_notify)->title);
            (*operate_notify)->title = NULL;
        }
        if ((*operate_notify)->subtitle)
        {
            DUER_FREE((*operate_notify)->subtitle);
            (*operate_notify)->subtitle = NULL;
        }
        if ((*operate_notify)->content)
        {
            DUER_FREE((*operate_notify)->content);
            (*operate_notify)->content = NULL;
        }
        if ((*operate_notify)->link_url)
        {
            DUER_FREE((*operate_notify)->link_url);
            (*operate_notify)->link_url = NULL;
        }
        if ((*operate_notify)->bg_img_url)
        {
            DUER_FREE((*operate_notify)->bg_img_url);
            (*operate_notify)->bg_img_url = NULL;
        }
        if ((*operate_notify)->img_url)
        {
            DUER_FREE((*operate_notify)->img_url);
            (*operate_notify)->img_url = NULL;
        }

        DUER_FREE(*operate_notify);
        *operate_notify = NULL;
        operate_notify = NULL;
    }
}

LOCAL void duerapp_free_operate_notify(void)
{
    if (s_pt_operate_notify && *s_pt_operate_notify)
    {
        if ((*s_pt_operate_notify)->title)
        {
            DUER_FREE((*s_pt_operate_notify)->title);
            (*s_pt_operate_notify)->title = NULL;
        }
        if ((*s_pt_operate_notify)->subtitle)
        {
            DUER_FREE((*s_pt_operate_notify)->subtitle);
            (*s_pt_operate_notify)->subtitle = NULL;
        }
        if ((*s_pt_operate_notify)->content)
        {
            DUER_FREE((*s_pt_operate_notify)->content);
            (*s_pt_operate_notify)->content = NULL;
        }
        if ((*s_pt_operate_notify)->link_url)
        {
            DUER_FREE((*s_pt_operate_notify)->link_url);
            (*s_pt_operate_notify)->link_url = NULL;
        }
        if ((*s_pt_operate_notify)->bg_img_url)
        {
            DUER_FREE((*s_pt_operate_notify)->bg_img_url);
            (*s_pt_operate_notify)->bg_img_url = NULL;
        }
        if ((*s_pt_operate_notify)->img_url)
        {
            DUER_FREE((*s_pt_operate_notify)->img_url);
            (*s_pt_operate_notify)->img_url = NULL;
        }

        DUER_FREE(*s_pt_operate_notify);
        *s_pt_operate_notify = NULL;
        s_pt_operate_notify = NULL;
    }
}

/*****************************************************************************/

LOCAL void OperateImageArtFree(int type)
{
    DUER_LOGI("%s", __FUNCTION__);

    switch (type) {
        case OPERATE_IMG_DOWN_WINDOW_PIC:
        {
            if (s_operate_params.op_img) {
                DUER_LOGI("%s", __FUNCTION__);
                if ((s_operate_params.op_img)->data) {
                    DUER_LOGI("%s", __FUNCTION__);
                    DUER_FREE((s_operate_params.op_img)->data);
                    (s_operate_params.op_img)->data = NULL;
                }

                DUER_FREE((s_operate_params.op_img));
                (s_operate_params.op_img) = NULL;
            }
        }
        break;
        case OPERATE_IMG_DOWN_LONG_PIC:
        {
            if (s_operate_params.longpic_img) {
                DUER_LOGI("%s", __FUNCTION__);
                if ((s_operate_params.longpic_img)->data) {
                    DUER_LOGI("%s", __FUNCTION__);
                    DUER_FREE((s_operate_params.longpic_img)->data);
                    (s_operate_params.longpic_img)->data = NULL;
                }

                DUER_FREE((s_operate_params.longpic_img));
                (s_operate_params.longpic_img) = NULL;
            }
        }
        break;
        default:
        break;
    }
}

LOCAL void OperateImageArtDownloadCb(img_dsc_t *img, void *down_param)
{
    int *p_index = NULL;
    DUER_LOGI("%s", __FUNCTION__);

    if (!img) {
        return;
    }

    p_index = (int *)down_param;
    if (p_index) {
        DUER_LOGI("image down index=%d,p=%p", *p_index, p_index);
    }

    switch (*p_index) {
        case OPERATE_IMG_DOWN_WINDOW_PIC:
        {
            OperateImageArtFree(OPERATE_IMG_DOWN_WINDOW_PIC);

            (s_operate_params.op_img) = img;
            s_operate_params.op_anim_update = TRUE;
            #if 0 //更换接口，避免UI线程接口错误使用
            if (MMK_IsOpenWin(MMI_DUERAPP_OPERATE_WIN_ID)) {
                MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_OPERATE_WIN_ID, MSG_DUERAPP_ANIM_PIC_UPDATE, PNULL, 0);
            }
            #else
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_OPERATE_WIN_ID, MSG_DUERAPP_ANIM_PIC_UPDATE, PNULL, 0);
            #endif
        }
        break;
        case OPERATE_IMG_DOWN_LONG_PIC:
        {
            OperateImageArtFree(OPERATE_IMG_DOWN_LONG_PIC);

            (s_operate_params.longpic_img) = img;
            s_operate_params.longpic_update = TRUE;
            #if 0 //更换接口，避免UI线程接口错误使用
            if (MMK_IsOpenWin(MMI_DUERAPP_HOME_WIN_ID)) {
                MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_ACTIVITY_SHOW, PNULL, 0);
            }
            #else
            MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_ACTIVITY_SHOW, PNULL, 0);
            #endif
        }
        break;
        default:
        break;
    }
}

LOCAL void OperateImageDownArtPicture(char *url, int type)
{
    int *p_index_b = NULL;
    DUER_LOGI("%s", __FUNCTION__);

    if (!url) {
        return;
    }

    OperateImageArtFree(type);

    p_index_b = DUER_CALLOC(1, sizeof(int));
    *p_index_b = type;
    DUER_LOGI("down index b=%d,p=%p", *p_index_b, p_index_b);
    duer_image_download(url, OperateImageArtDownloadCb, p_index_b);
}

LOCAL void UpdateOperateImage(void)
{
    GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    GUIANIM_DATA_INFO_T     data_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    MMI_HANDLE_T            anim_handle = NULL;

    DUER_LOGI("%s", __FUNCTION__);

    if (!MMK_IsOpenWin(MMI_DUERAPP_OPERATE_WIN_ID) ||
        !MMK_IsFocusWin(MMI_DUERAPP_OPERATE_WIN_ID))
    {
        return;
    }

    /* anim pic */
    {

        anim_handle = MMK_GetCtrlHandleByWin(MMI_DUERAPP_OPERATE_WIN_ID, MMI_DUERAPP_OPERATE_IMG_ANIM_CTRL_ID);
        ctrl_info.is_ctrl_id        = TRUE;
        ctrl_info.ctrl_id           = anim_handle;
        display_info.align_style    = GUIANIM_ALIGN_HVMIDDLE;
        display_info.bg.bg_type     = GUI_BG_COLOR;
        display_info.is_update      = TRUE;

        if ((s_operate_params.op_img)) {
            data_info.data_ptr = (s_operate_params.op_img)->data;
            data_info.data_size = (s_operate_params.op_img)->data_size;

            display_info.is_zoom = TRUE;
            display_info.is_disp_one_frame = TRUE;
        }

        GUIANIM_SetParam(&ctrl_info, &data_info, PNULL, &display_info);
        GUIANIM_SetVisible(MMI_DUERAPP_OPERATE_IMG_ANIM_CTRL_ID, TRUE, TRUE);
        GUIANIM_PlayAnim(anim_handle);
    }

    /* close button */
    {
        GUI_RECT_T closeBtnRect = DUERAPP_OPERATE_PIC_CLOSE_RECT;
        GUIBUTTON_SetRect(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, &closeBtnRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, FALSE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, OperateCloseButtonCallBack);
        GUIBUTTON_SetVisible(MMI_DUERAPP_OPERATE_CLOSE_BUTTON_CTRL_ID, TRUE, TRUE);
    }

    s_operate_params.op_anim_update = FALSE;
}


/***********************************
 * 长图页面
 * ******************************************/

LOCAL void OpLongpicClickButtonCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);

    if (((s_activity_operation) != NULL) &&
        ((s_activity_operation)->link_url != NULL)) 
    {
        DUER_LOGI("%s", __FUNCTION__);
        duer_audio_play_pause();

        duer_add_click_statistics(STATISTICS_CLICK_ACTIVITY_LONG_PIC, 0, NULL);

        MMIDUERAPP_OperateActionUrl((s_activity_operation)->link_url);
    }

    MMK_CloseWin(MMI_DUERAPP_OP_LONGPIC_WIN_ID);
}

LOCAL void UpdateOpLongpicImage(void)
{
    DUER_LOGI("%s", __FUNCTION__);

    MMIUpdateAnimImage(MMI_DUERAPP_OP_LONGPIC_WIN_ID, MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID, s_operate_params.longpic_img);
    (s_operate_params.longpic_update) = FALSE;
    return;
}

PUBLIC void MMIDUERAPP_OpLongpicDownload(void)
{
    char *long_img_url     = NULL;
    DUER_LOGI("%s", __FUNCTION__);

    #if (1 == DUERAPP_OPERATE_LONGPIC_URL)
        // long_img_url = "https://iot-paas-static.cdn.bcebos.com/XTC/hOlng1KQN3nvGI8DV4Gf7kb0G49os5cp/240/icon_vip_rights.png";
        // long_img_url = "https://iot-paas-static.cdn.bcebos.com/XTC/XXX/image/test/test_year_vip_40k.png";
        long_img_url = "https://iot-paas-static.cdn.bcebos.com/XTC/XXX/image/test/test_960.PNG";
    #else
        if (s_activity_operation) {
            if (s_activity_operation->img_url) {
                long_img_url = s_activity_operation->img_url;
            }
        }
    #endif
    if (long_img_url) {
        OperateImageDownArtPicture(long_img_url, OPERATE_IMG_DOWN_LONG_PIC);
    }
}

LOCAL void MMIDUERAPP_OpLongpicWinOpen(MMI_WIN_ID_T win_id)
{
    GUIFORM_CHILD_WIDTH_T       width       = {0};
    GUIFORM_CHILD_HEIGHT_T      height      = {0};
    uint16                      hor_space   = 0;
    uint16                      ver_space   = 0;
    BOOLEAN                     has_btn     = FALSE;
    GUI_BG_T    bg_info = {0};
    GUI_RECT_T win_rect = {0};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    GUI_PADDING_T   padding = {0};
    IGUICTRL_T *ictrl = PNULL;

    GUIANIM_DATA_INFO_T img_info = {0};
    GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    MMI_HANDLE_T            anim_handle = NULL;

    img_dsc_t *resource = NULL;

    MMI_STRING_T        button_txt      = {0};
    GUI_FONT_ALL_T      btn_font_info   = {DUERAPP_WATCH_NORMAL_FONT, MMI_RED_COLOR};
    GUI_BG_T            bg_btn          = {0};
    GUI_RECT_T          buttonRect      = DUERAPP_OPERATE_BUTTON_RECT;

    DUER_LOGI("%s", __FUNCTION__);

    {
        #if (1 == DUERAPP_LONG_PIC_USE_BTN)
            #if (1 == DUERAPP_OPERATE_FAKE)
                has_btn = TRUE;
            #else
                if (!s_activity_operation) {
                    DUER_LOGI("%s", __FUNCTION__);
                }
                has_btn = s_activity_operation->show_button;
            #endif
        #else
            has_btn = FALSE;
        #endif
    }

    /* form */
    {
        bg_info.bg_type = GUI_BG_COLOR;
        bg_info.color = MMI_BLACK_COLOR;
        CTRLFORM_SetBg(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, &bg_info);

        // rect.top += DUERAPP_SLIDE_INDICATOR_HEIGHT;
        win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, rect);
        GUIFORM_SetRect(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, &win_rect);

        CTRLFORM_SetPadding(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, &padding);

        // hor_space = 1;
        // GUIFORM_SetMargin(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, hor_space);

        ictrl = MMK_GetCtrlPtr(MMK_GetCtrlHandleByWin(win_id, MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID));
        // GUICTRL_SetProgress(ictrl, FALSE);
        GUIFORM_PermitChildBg(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, FALSE);     // 支持子控件背景
        GUIFORM_PermitChildFont(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, FALSE);   // 支持子控件字体
        GUIFORM_PermitChildBorder(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, FALSE);
    }

    /* image anim */
    {
        GUIANIM_SetVisible(MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID, TRUE, TRUE);

        width.type = GUIFORM_CHILD_WIDTH_FIXED;
        width.add_data = 240;
        GUIFORM_SetChildWidth(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID,
                    MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID,
                    &width);
        height.type = GUIFORM_CHILD_HEIGHT_FIXED;
        height.add_data = 750;
        GUIFORM_SetChildHeight(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID,
                    MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID,
                    &height);

        // CTRLANIM_SetBgColor(MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID, MMI_BLACK_COLOR);

        resource = s_operate_params.longpic_img; //imageGetResource(IMG_DOWN_CENTER_BANNER, 0);
        if (resource) {
            if (resource->data && resource->data_size > 0) {
                img_info.data_ptr = (resource)->data;
                img_info.data_size = (resource)->data_size;
            } else {
                DUER_LOGE("%s resource get err", __FUNCTION__);
                img_info.img_id = IMAGE_DUERAPP_BANNER1;
            }
        } else {
            DUER_LOGE("%s resource get err", __FUNCTION__);
            img_info.img_id = IMAGE_DUERAPP_BANNER1;
        }

        ctrl_info.is_ctrl_id        = TRUE;
        ctrl_info.ctrl_id           = MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID;
        display_info.align_style    = GUIANIM_ALIGN_HMIDDLE_TOP; //GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_auto_zoom_in = TRUE;
        display_info.is_handle_transparent = TRUE;

        display_info.is_syn_decode = TRUE;
        GUIANIM_SetParam(&ctrl_info, &img_info, PNULL, &display_info);
        GUIANIM_SetVisible(MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID, TRUE, TRUE);

        if (!has_btn) {
            CTRLANIM_SetCallBackFunc(MMI_DUERAPP_OP_LONGPIC_IMG_ANIM_CTRL_ID, OpLongpicClickButtonCallBack, NULL);
        }
    }

    /* click button */
    #if (1 == DUERAPP_LONG_PIC_USE_BTN)
    {
        if (has_btn) {
            #if (1 == DUERAPP_OPERATE_FAKE)
                button_txt.wstr_ptr = L"测试按键";
                bg_btn.color = MMI_WHITE_COLOR;
            #else
                if (!s_activity_operation) {
                    DUER_LOGI("%s", __FUNCTION__);
                }
                button_txt.wstr_ptr = (s_activity_operation)->button_text;
                if ((s_activity_operation)->button_color) {
                    btn_font_info.color = RGB8882RGB565((s_activity_operation)->button_color);
                }
                if ((s_activity_operation)->button_bg) {
                    bg_btn.color = RGB8882RGB565((s_activity_operation)->button_bg);
                }
            #endif

            button_txt.wstr_len = MMIAPICOM_Wstrlen(button_txt.wstr_ptr);

            GUIBUTTON_SetFg(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, &bg_btn);
            GUIBUTTON_SetBg(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, &bg_btn);
            GUIBUTTON_SetRect(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, &buttonRect);
            GUIBUTTON_SetTextAlign(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, ALIGN_HVMIDDLE);
            GUIBUTTON_SetRunSheen(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, FALSE);
            GUIBUTTON_SetFont(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, &btn_font_info);
            GUIBUTTON_SetHandleLong(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, TRUE);
            GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, OpLongpicClickButtonCallBack);
            GUIBUTTON_SetText(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, button_txt.wstr_ptr, button_txt.wstr_len);
            GUIBUTTON_SetVisible(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, TRUE, FALSE);

            CTRLFORM_SetChildAlign(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID,
                        MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID,
                        GUIFORM_CHILD_ALIGN_HMIDDLE);

            ver_space = 5;
            CTRLFORM_SetChildSpace(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID, 
                        MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, 
                        PNULL, 
                        &ver_space);

            width.type = GUIFORM_CHILD_WIDTH_FIXED;
            width.add_data = 140;
            GUIFORM_SetChildWidth(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID,
                        MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID,
                        &width);
            height.type = GUIFORM_CHILD_HEIGHT_FIXED;
            height.add_data = 50;
            GUIFORM_SetChildHeight(MMI_DUERAPP_OP_LONGPIC_FORM_CTRL_ID,
                        MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID,
                        &height);
        } else {
            GUIBUTTON_SetVisible(MMI_DUERAPP_OP_LONGPIC_CLICK_BUTTON_CTRL_ID, FALSE, FALSE);
        }
    }
    #endif
}

LOCAL MMI_RESULT_E HandleOpLongpicWinMsg(
    MMI_WIN_ID_T        win_id,    //IN:
    MMI_MESSAGE_ID_E    msg_id,    //IN:
    DPARAM              param      //IN:
)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;

    DUER_LOGI("msg_id=0x%x", msg_id);
    // s_pt_operate_notify = (P_DUEROS_OPERATE_NOTIFY *)duerapp_get_operate_notify();
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            WatchRec_MainwinDrawBG();

            if (s_activity_operation)
            {
                MMIDUERAPP_OpLongpicWinOpen(win_id);
            }
            else
            {
                MMK_CloseWin(win_id);
            }
        }
        break;

    case MSG_FULL_PAINT:
        WatchRec_MainwinDrawBG();

        // if (s_pt_operate_notify && *s_pt_operate_notify)
        // {
        //     MMIDUERAPP_OpLongpicWinOpen(win_id);
        // }
        // else
        // {
        //     MMK_CloseWin(win_id);
        // }
        break;

    case MSG_CTL_PENOK:
        DUER_LOGI("duerapp operate result->MSG_CTL_PENOK");
        break;

    case MSG_END_FULL_PAINT:
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
        OperateImageArtFree(OPERATE_IMG_DOWN_LONG_PIC);
        // duerapp_free_operate_notify();
        duerapp_free_operate(&s_activity_operation);
        break;
    // case MSG_DUERAPP_ANIM_PIC_UPDATE:
    //     UpdateOpLongpicImage();
    //     break;
    case MSG_NOTIFY_ANIM_DISPLAY_IND:
        if (MMK_IsOpenWin(MMI_DUERAPP_OPERATE_WIN_ID) && MMK_IsFocusWin(MMI_DUERAPP_OPERATE_WIN_ID)) {
            MMK_SendMsg(MMI_DUERAPP_OPERATE_WIN_ID, MSG_FULL_PAINT, PNULL);
        }
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return (recode);
}

/***********************************
 * NOTIFY url解析
 * ******************************************/
LOCAL int32 getOperationActionType(char *url)
{
    int32 result = OPERATE_ACT_URL_NONE;
    char *pos = NULL;

    DUER_LOGI("%s", __FUNCTION__);

    if (!url) {
        DUER_LOGE("%s url error", __FUNCTION__);
        return result;
    }

    DUER_LOGI("url=%s", url);

    pos = DUER_STRSTR(url, OP_URL_DUEROS_PRE_TEXT);
    if (pos) {
        result = OPERATE_ACT_URL_DUEROS;
        return result;
    }
    
    pos = DUER_STRSTR(url, OP_URL_NATIVE_PRE_TEXT);
    if (pos) {
        result = OPERATE_ACT_URL_NATIVE;
        return result;
    }

    return result;
}

LOCAL int getNowTimestampSec(void)
{
    return duer_real_timestamp();
}

// 获取活动配置
/**--------------------------------------------------------------------------*/
LOCAL int httpGetActivityListResponse(void *p_user_ctx, duer_http_data_pos_t pos,
                              const char *body, size_t len, const char *type)
{
    int ret = DUER_ERR_FAILED;
    char *msg = NULL;
    baidu_json *code_json = NULL;
    baidu_json *message_json = NULL;
    baidu_json *data_json = NULL;
    baidu_json *data_item_json = NULL;
    baidu_json *img_json = NULL;
    baidu_json *link_json = NULL;
    baidu_json *tmp_link_json = NULL;
    baidu_json *type_json = NULL;
    char *type_string = NULL;
    BOOLEAN show = (BOOLEAN)0;
    baidu_json *button_txt_json = NULL;
    char *button_txt = NULL;
    int utf8_len = 0;
    wchar *wch_txt = NULL;
    baidu_json *url_json = NULL;
    char *url_string = NULL;
    int url_utf8_len = 0;
    baidu_json *color_json = NULL;
    char *color_string = NULL;
    baidu_json *button_bg_json = NULL;
    char *button_bg = NULL;
    int body_len = 0;
    baidu_json *root = NULL;

    int img_url_utf8_len = 0;
    char *img_url_str = NULL;

    DUER_LOGI("pos=%d, len=%u", pos, len);
    if (body) {
        DUER_LOGD("body_len=%d, body=%s", strlen(body), body);
    }

    if (!s_http_response_body) {
        DUER_LOGI("%s", __FUNCTION__);
    }

    if (body && pos == DUER_HTTP_DATA_FIRST) {
        if (s_http_response_body) {
            DUER_FREE(s_http_response_body);
            s_http_response_body = NULL;
        }
        body_len = strlen(body);
        s_http_response_body = DUER_CALLOC(1,body_len + 1);
        strcpy(s_http_response_body,body);
        return DUER_OK;
    } else if (body && pos == DUER_HTTP_DATA_MID) {
        if (s_http_response_body)
        {
        int body_len = strlen(body);
        char *temp_ptr = DUER_REALLOC(s_http_response_body,strlen(s_http_response_body) + body_len + 1);
        DUER_LOGI("DUER_REALLOC body, temp_ptr=%p, s_http_response_body=%p", temp_ptr, s_http_response_body);
        if (temp_ptr) {
            s_http_response_body = temp_ptr;
        }
        strncat(s_http_response_body,body,body_len);
        }
        return DUER_OK;
    }

    if (!body && pos == DUER_HTTP_DATA_LAST) {
        DUER_LOGI("End of %s response", __FUNCTION__);
        DUER_LOGI("total=%p", s_http_response_body);
    }
    DUER_LOGI("%s baidu_json_Parse", __FUNCTION__);
    root = baidu_json_Parse(s_http_response_body);
    if (!root) {
        DUER_LOGW("%s Parse body error!", __FUNCTION__);
        if (s_http_response_body) {
            DUER_FREE(s_http_response_body);
            s_http_response_body = NULL;
        }
        s_get_activity_list_inprogress = FALSE;
        return ret;
    }

    /* for print */
    {
        msg = baidu_json_PrintBuffered(root, 2048, 0);
        if (msg) {
            duer_print_long_str(msg);
            baidu_json_release(msg);
            msg = NULL;
        }
    }

    /* code */
    {
        code_json = baidu_json_GetObjectItem(root, "code");
        if (code_json) {
            DUER_LOGI("%s code=%d,%s", __FUNCTION__, code_json->valueint, code_json->valuestring);
        }
    }

    /* message */
    {
        message_json = baidu_json_GetObjectItem(root, "message");
        if (message_json) {
            DUER_LOGI("%s message=%s", __FUNCTION__, message_json->valuestring);
        }
    }

    /* data */
    do {
        data_json = baidu_json_GetObjectItem(root, "data");
        if (!data_json) {
            DUER_LOGE("%s data json error", __FUNCTION__);
            break;
        }

#if 0
        int data_size = baidu_json_GetArraySize(data_json);
        if (data_size <= 0) {
            DUER_LOGE("%s data size=%d error", __FUNCTION__, data_size);
            break;
        }
        baidu_json *data_item_json = baidu_json_GetArrayItem(data_json, 0);
        if (!data_item_json) {
            DUER_LOGE("%s data_item_json error", __FUNCTION__);
            break;
        }
#else
        data_item_json = data_json;
#endif

        s_activity_operation = DUER_CALLOC(1, sizeof(DUEROS_OPERATE_NOTIFY));

        /* 解析image */
        {
            img_json = baidu_json_GetObjectItem(data_item_json, "imgUrl");
            if (img_json) {
                img_url_str = img_json->valuestring;
                if (!img_url_str) {
                    DUER_LOGE("img_url_str");
                }
                img_url_utf8_len = strlen(img_url_str);
                if (img_url_utf8_len > 0) {
                    s_activity_operation->img_url = DUER_CALLOC(1, img_url_utf8_len + 1);
                    strcpy(s_activity_operation->img_url, img_url_str);
                }
                if (img_url_str) {
                    DUER_LOGI("img_url_str:%s", img_url_str);
                }
            }
        }

        /* 解析link */
        {
            link_json = baidu_json_GetObjectItem(data_item_json, "link");
            if (link_json) {
                int size = baidu_json_GetArraySize(link_json);
                if (size > 0) {
                    tmp_link_json = baidu_json_GetArrayItem(link_json, 0);
                    type_json = baidu_json_GetObjectItem(tmp_link_json, "type");
                    type_string = type_json->valuestring;
                    if (!type_string) {
                        DUER_LOGE("%s link type_string", __FUNCTION__);
                    }
                    show = (strcmp("button", type_string) == 0);
                    s_activity_operation->show_button = show;

                    button_txt_json = baidu_json_GetObjectItem(tmp_link_json, "title");
                    button_txt = button_txt_json->valuestring;
                    if (!button_txt) {
                        DUER_LOGE("%s link button_txt", __FUNCTION__);
                    }
                    utf8_len = strlen(button_txt);
                    
                    wch_txt = DUER_CALLOC(1, (utf8_len + 1) * sizeof(wchar));
                    GUI_UTF8ToWstr(wch_txt, utf8_len, button_txt, utf8_len);
                    MMIAPICOM_Wstrncpy(s_activity_operation->button_text,wch_txt,
                        MIN(OPERATE_BUTTON_MAX - 1, MMIAPICOM_Wstrlen(wch_txt)));
                    DUER_FREE(wch_txt);

                    url_json = baidu_json_GetObjectItem(tmp_link_json, "url");
                    url_string = url_json->valuestring;
                    if (!url_string) {
                        DUER_LOGE("%s link url_string", __FUNCTION__);
                    }
                    url_utf8_len = strlen(url_string);
                    if (url_utf8_len > 0) {
                        s_activity_operation->link_url = DUER_CALLOC(1, url_utf8_len + 1);
                        strcpy(s_activity_operation->link_url,url_string);
                    }
            
                    color_json = baidu_json_GetObjectItem(tmp_link_json, "color");
                    color_string = color_json->valuestring;
                    if (color_string) {
                        s_activity_operation->button_color = duerapp_parse_color(color_string);
                    }

                    button_bg_json = baidu_json_GetObjectItem(tmp_link_json, "backgroundColor");
                    button_bg = button_bg_json->valuestring;
                    if (button_bg) {
                        s_activity_operation->button_bg = duerapp_parse_color(button_bg);
                    }

                    if (type_string) {
                        DUER_LOGI("%s link type_string:%s", __FUNCTION__, type_string);
                    }
                    if (button_txt) {
                        DUER_LOGI("%s link button_txt:%s", __FUNCTION__, button_txt);
                    }
                    if (url_string) {
                        DUER_LOGI("%s link url_string:%s", __FUNCTION__, url_string);
                    }
                }
            }
        }

        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_ACTIVITY_CALL_BACK, NULL,0);
    } while (0);

    if (s_http_response_body) {
        DUER_FREE(s_http_response_body);
        s_http_response_body = NULL;
    }
    if (root) {
        baidu_json_Delete(root);
    }
    ret = DUER_OK;
    s_get_activity_list_inprogress = FALSE;
    return ret;
}

LOCAL void httpGetActivityListTaskEntry(unsigned int argc, duer_thread_entry_args_t *params)
{
    get_activity_list_params_t *p_get_params = (get_activity_list_params_t *)params->args;
    char temp_url[150] = {0};
    DUER_LOGI("%s", __FUNCTION__);
    if (p_get_params) {
        sprintf(temp_url,"%s%s%d",
                OPEN_API_URL_RELEASE,
                ACCOUNT_ACTIVITY_INFO,
                p_get_params->activity_id);

        DUER_LOGI("%s temp_url=%s", __FUNCTION__, temp_url);
        if(duerapp_openapi_request(httpGetActivityListResponse, temp_url) != DUER_HTTP_OK) {
            s_get_activity_list_inprogress = FALSE;
        }

        DUER_FREE(p_get_params);
        p_get_params = NULL;
    } else {
        DUER_LOGE("%s params err", __FUNCTION__);

        s_get_activity_list_inprogress = FALSE;
    }

    duer_thread_exit(params, 1);
}

LOCAL void httpGetActivityListRequest(int activity_id)
{
    static int last_timestamp = 0;
    int now_timestamp = 0;
    const int diff = 5;
    int ret = 0;
    get_activity_list_params_t *p_get_params = NULL;
    s_get_activity_list_inprogress = TRUE;

    if (activity_id < 0) {
        DUER_LOGE("%s act id=%d err", __FUNCTION__, activity_id);
        return;
    }

    if (s_get_activity_list_inprogress) {
        now_timestamp = getNowTimestampSec();
        if (((last_timestamp - now_timestamp) >= diff) || (now_timestamp - last_timestamp) >= diff) {
            s_get_activity_list_inprogress = FALSE;
            DUER_LOGE("%s out time,now=%d,last=%d", __FUNCTION__, now_timestamp, last_timestamp);
        } else {
            DUER_LOGE("%s inprogress,now=%d,last=%d", __FUNCTION__, now_timestamp, last_timestamp);
            return;
        }
    }
    last_timestamp = getNowTimestampSec();
    DUER_LOGE("%s timestamp=%d", __FUNCTION__, last_timestamp);

    p_get_params = DUER_CALLOC(1, sizeof(get_activity_list_params_t));
    if (!p_get_params) {
        DUER_LOGE("calloc p_get_params failed!");
        return;
    }
    p_get_params->activity_id = activity_id;

    
    ret = duerapp_request_task_with_param(httpGetActivityListTaskEntry, p_get_params);
    if (ret < 0) {
        s_get_activity_list_inprogress = FALSE;
        DUER_LOGW("%s task error", __FUNCTION__);
    }
}

LOCAL void MMIDUERAPP_OperateJumpToWriting(void)
{
    static BOOLEAN first_flag = TRUE;
    DUER_LOGI("%s", __FUNCTION__);

    if (first_flag) {
        first_flag = FALSE;

        duerapp_writing_init();
    }
    duerapp_writing_start();
}

LOCAL void MMIDUERAPP_OperateJumpToSoundItem(char *tag)
{
    int index = 0;
    DUER_LOGI("%s", __FUNCTION__);
    if (!tag) {
        DUER_LOGE("%s tag err", __FUNCTION__);
        return;
    }

    index = DiscoveryGetIndexFromTag(tag);
    if (index < 0) {
        DUER_LOGE("%s index err", __FUNCTION__);
        return;
    }

    //通过index打开对应item
    DuerappAudioTypeSet(DUER_AUDIO_UNICAST_STORY);
    duerapp_res_list_enter(index);
}

LOCAL void MMIDUERAPP_OperateNativeAction(char *json_str)
{
    baidu_json *root = NULL;
    baidu_json *url_type_json = NULL;
    baidu_json *tag_json = NULL;

    char *tag = NULL;
    baidu_json *activity_id_json = NULL;
    int activity_id = 0;
    baidu_json *url_json = NULL;
    baidu_json *text_json = NULL;

    DUER_LOGI("%s", __FUNCTION__);

    if (!json_str) {
        DUER_LOGE("%s json_str err", __FUNCTION__);
        return;
    }

    root = baidu_json_Parse(json_str);
    if (!root) {
        DUER_LOGW("%s Parse body error!", __FUNCTION__);
        return;
    }
    do 
    {
        url_type_json = baidu_json_GetObjectItem(root, "url_type");
        if (!url_type_json) {
            break;
        }
        DUER_LOGI("%s url type=%d", __FUNCTION__, url_type_json->valueint);
        switch (url_type_json->valueint) {
            case NATIVE_OP_JUMP_TO_SECOND:
            {

            }
            break;
            case NATIVE_OP_JUMP_TO_SOUND_ITEM:
            {
                tag_json = baidu_json_GetObjectItem(root, "tag");
                if (tag_json) {
                    tag = tag_json->valuestring;
                    DUER_LOGI("%s tag=%s", __FUNCTION__, tag);
                    MMIDUERAPP_OperateJumpToSoundItem(tag);
                }
            }
            break;
            case NATIVE_OP_JUMP_TO_WRITTING:
            {
                MMIDUERAPP_OperateJumpToWriting();
            }
            break;
            case NATIVE_OP_GET_ACTIVITY_LONG_PIC:
            {
                activity_id_json = baidu_json_GetObjectItem(root, "activityID");
                if (activity_id_json) {
                    activity_id = activity_id_json->valueint;
                    DUER_LOGI("%s activity_id=%d", __FUNCTION__, activity_id);
                    httpGetActivityListRequest(activity_id);
                }
            }
            break;
            case NATIVE_OP_SHOW_COMMON_QR: {
                url_json = baidu_json_GetObjectItem(root, "url");
                text_json = baidu_json_GetObjectItem(root, "text");
                if (url_json && text_json) {
                    MMIDUERAPP_CreateOperateCommonQrWin(url_json->valuestring, text_json->valuestring);
                } else {
                    DUER_LOGE("%s json error, %x, %x", __FUNCTION__, text_json, url_json);
                }
            } break;
            default:
            break;
        }

    } while(0);

    if (root) {
        baidu_json_Delete(root);
    }
}

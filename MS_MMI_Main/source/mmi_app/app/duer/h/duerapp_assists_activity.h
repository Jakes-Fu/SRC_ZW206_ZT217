#ifndef _DUERAPP_ASSISTS_ACTIVITY_H_
#define _DUERAPP_ASSISTS_ACTIVITY_H_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_module.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define DUERAPP_ASSISTS_VIP_SCAN_QR_CODE
#undef DUERAPP_ASSISTS_VIP_TRANSMIT_MESSAGE
#undef DUERAPP_ASSISTS_VIP_MESSAGE_SELECT
#define DUERAPP_ASSISTS_COMMON_FUNCTION
#define DUERAPP_ASSISTS_PUBLIC_FUNCTION

#define DUERAPP_ASSISTS_WORDS_NUMBER              8 //助力模板句数�?
#define DUERAPP_ASSISTS_LONG_PICTURE_HEIGHT       50 //900 //长图高度
#define DUERAPP_ASSISTS_BINDING_USERLIST          10 //绑定用户列表数量
#define DUERAPP_ASSISTS_SHOW_BIND_SUPPORT         0 //显示微信绑定可选发送账�?

#define DUERAPP_ASSISTS_EDIT_BUTTON_FONT          SONG_FONT_28//编辑按钮
#define DUERAPP_ASSISTS_SEND_BUTTON_FONT          SONG_FONT_28//一键发送按�?
#define DUERAPP_ASSISTS_TITLE1_FONT               SONG_FONT_28//1.喊爸妈来助力 2.喊爸妈来扫码
#define DUERAPP_ASSISTS_CONTENT1_FONT             SONG_FONT_24//编辑助力内容发给家长 使用微信扫码开会员
#define DUERAPP_ASSISTS_COMMON_FONT               SONG_FONT_24
#define DUERAPP_ASSISTS_VOICE_TEXT_FONT           SONG_FONT_26//输入法语音文本框

//RTOS的图片不能大�?63KB
#define DUERAPP_ASSISTS_PNG_URL                   "https://iot-paas-static.cdn.bcebos.com/XTC/rt1NjR38mf2eBFjrhlEFPMrOm8mT3ehN/image/test/test_960.PNG"

/**************************************************************************************************************************
 *                                                          TYPEDEF TYPE                                                   
 **************************************************************************************************************************/

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/
 typedef struct _words_info_t{//助力模板�?
    BOOLEAN button_state;
    MMI_HANDLE_T button_ctrl_id;
    MMI_HANDLE_T textbox_ctrl_id;
}words_info_t;

typedef struct _bind_userlist_info_t
{
    BOOLEAN is_select;
    char openId[32];
    int subscribe;
    char nickname[32];
    char sex[32];
    char language[32];
    char city[32];
    char province[32];
    char country[32];
    char headImgUrl[32];
    char subscribeTime[32];
}bind_userlist_info_t;

typedef struct _assists_info_t
{
    int last_button_id;
    int binding_number;//绑定数量
    int checkbox_number;//选中的数�?
    char *http_response_body;
    wchar assists_wtxt_words[256];//发送的助力信息
    words_info_t button_info[DUERAPP_ASSISTS_WORDS_NUMBER];
    bind_userlist_info_t bind_info[DUERAPP_ASSISTS_BINDING_USERLIST];
}assists_info_t;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

PUBLIC int PUB_DUERAPP_UserListRequest(void);

PUBLIC int PUB_DUERAPP_ParentsAssistsRequest(void);

#if defined(DUERAPP_ASSISTS_VIP_MESSAGE_SELECT)
PUBLIC BOOLEAN PUB_DUERAPP_MessageSelectWinIsOpen(void);
#endif

PUBLIC void PUB_DUERAPP_ScreenInputMidResultEvent(char *text, int ret);

PUBLIC void PUB_DUERAPP_ScreenInputFinalResultEvent(char *text, int ret);

PUBLIC MMI_HANDLE_T PUB_DUERAPP_AssistsWinOpen(void);

#if defined(DUERAPP_ASSISTS_VIP_TRANSMIT_MESSAGE)
PUBLIC void PUB_DUERAPP_TransmitMessageWinOpen(void);
#endif

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif

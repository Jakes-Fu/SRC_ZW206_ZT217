#include "os_api.h"
#include "mmk_type.h"
#include "mmk_timer.h"
#include "guistring.h"
#include "ui_layer.h"
#include "graphics_draw.h"
#include "std_header.h"
#include "mn_type.h"
#include "sci_types.h"
#include <stdarg.h>

#include "watch_slidepage.h"
#include "watch_launcher_main.h"

#ifdef PDA_UI_DROPDOWN_WIN
#include "mmidropdownwin_export.h"
#endif

#include "watch_pb_view.h"
#include "watch_stopwatch.h"
#include "watch_gallery_export.h"
#include "watch_charge_win.h"
#ifdef MMI_RECORD_SUPPORT
#include "mmirecord_main.h"
#endif
#include "watch_launcher_editclockwin.h"
#include "watch_sms_main.h"
#include "mmicc_export.h" //for dialpad
#include "mmicom_trace.h"
#include "mmi_default.h"
#ifdef DROPDOWN_NOTIFY_SHORTCUT
#include "watch_notifycenter.h"
#endif
#ifdef ADULT_WATCH_SUPPORT
#include "watch_steps.h"
#include "watch_heartrate.h"
#include "watch_sms_message.h"
#include "mmieng_uitestwin.h"

#endif
#ifdef DYNAMIC_WATCHFACE_SUPPORT
#include "watchface_view.h"
#endif

#include "watch_extra_export.h"

#ifdef BAIDU_DRIVE_SUPPORT
#include "mmibaidu_export.h"
#endif

#include "baidu_rtc_client.h"
#include <stdarg.h>

#include "zdt_app.h"
#include "brtc_hangup_btn.h"

#define APPID_GET_FROM_BAIDU  "apppkdjeu7mxjr5"//"appjfkryqbzyns5" // 20231213--正式KEY--"apppkdjeu7mxjr5"

#define BRTC_LOG SCI_TRACE_LOW

static void* volatile gBrtcClient = NULL;


static char appid[20] = {0};
static char roomname[30] = {0};//"ZX";
static char userid[20] = {0};
static char display[30] = "videocall";
static char token[60] = "no_token";
static char customer_name[60] = "zhongxing";
static int connect_count = 0;
static int lcd_width = MMI_MAINSCREEN_WIDTH;//240; // sdk 20231130 BAIDU_VIDEO_SIZE
static int lcd_height = MMI_MAINSCREEN_HEIGHT;//240; // sdk 20231130 BAIDU_VIDEO_SIZE
//static char brtcserver[125] = "ws://rtc.exp.bcelive.com:8186/janus";
static char brtcserver[125] = "udp://rtc-ss-udp.exp.bcelive.com:8202";

static const char *ca_info = "-----BEGIN CERTIFICATE-----\r\n\
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\
-----END CERTIFICATE-----";

static void videocall_hangup(void *h);
static void videocall_reconnect(void *h);

static int volatile last_call_time = 0;

#ifndef PRId64
#define PRId64 "lld"
#endif

#ifdef RELEASE_INFO
#undef SCI_TraceLow
uint32 SCI_TraceLow(const char *pszFmt, ...)
{
    return 0;
}
#endif

#ifdef WIN32
void * brtc_create_client(void){return PNULL;}
void brtc_destroy_client(void* rtc_client){}
int  brtc_login_room(void* rtc_client, const char* room_name, const char* user_id, const char* display_name, const char* token){return 0;}
int  brtc_logout_room(void* rtc_client){return 0;}
void brtc_set_server_url(void* rtc_client, const char* url){}
void brtc_set_appid(void* rtc_client, const char* app_id){}
void brtc_set_cer(void* rtc_client, const char* cer_path){}
void brtc_register_message_listener(void* rtc_client, IRtcMessageListener msgListener){}
void brtc_send_message_to_user(void* rtc_client, const char* msg, const char* id){}
void brtc_start_publish(void* rtc_client){}
void brtc_stop_publish(void* rtc_client){}
void brtc_subscribe_streaming(void* rtc_client, const char * feed_id){}
void brtc_stop_subscribe_streaming(void* rtc_client, const char * feed_id){}
void brtc_set_auto_publish(void* rtc_client, int auto_publish){}
void brtc_set_auto_subscribe(void* rtc_client, int auto_subscribe){}
void brtc_set_videocodec(void* rtc_client, const char* vc){}
void brtc_set_audiocodec(void* rtc_client, const char* ac){}
void brtc_set_candidate_ip(void* rtc_client, const char *ip){}
void brtc_set_mediaserver_ip(void* rtc_client, const char *ip){}
void brtc_mute_camera(void* rtc_client, int is_muted){}
void brtc_mute_micphone(void* rtc_client, int is_muted){}
void brtc_set_volume(void* rtc_client, int volume){}
void brtc_set_livestreaming_url(void* rtc_client, const char *url){}
void brtc_send_audio(void* rtc_client, const void * data, int len){}
void brtc_send_image(void* rtc_client, const void * data, int len){}
void brtc_send_data(void* rtc_client, const void * data, int len){}
void brtc_register_audio_frame_observer(void* rtc_client, IAudioFrameObserver afo){}
void brtc_register_video_frame_observer(void* rtc_client, IVideoFrameObserver vfo){}
void brtc_set_usingvideo(void* rtc_client, int e){}
void brtc_set_usingaudio(void* rtc_client, int e){}
void brtc_set_receivingvideo(void* rtc_client, int e){}
void brtc_set_receivingaudio(void* rtc_client, int e){}
void brtc_set_userevent(void* rtc_client, int e){}
int  brtc_sdk_init(void){return 0;}
int  brtc_sdk_deinit(void){}
int  brtc_sdk_do_async(void (*func)(void *), void *arg, int time_out_ms){return 0;}
void brtc_sdk_enable_inner_av_devices(int e){}
void brtc_sdk_enable_log(int e){}
void brtc_sdk_enable_nack(int e){}
void brtc_sdk_set_netid(unsigned int id){}
unsigned int brtc_sdk_get_netid(void){return 0;}
void brtc_sdk_enable_neteq(int e){}
void brtc_sdk_enable_video_local_preview(int e){}
void brtc_sdk_register_log_handler(void (*log)(int level, const char *msg));
const char * brtc_sdk_get_device_mediainfo(void){return "123";}
const char * brtc_sdk_version(void){return "123";}
void brtc_sdk_set_video_display_size(uint16 lcd_width, uint16 lcd_height){}
void brtc_set_awn_signal(void* rtc_client, int e){}
void brtc_sdk_set_osd_area(int enable, const char *rgba8888, int left, int top, int right, int bottom){}
void brtc_set_customer_name(void* rtc_client, const char* customer_name){}
#endif

static void OnRtcMessage(RtcMessage* msg)
{
    BRTC_LOG("OnRtcMessage %d\r\n", msg->msgType);
    switch (msg->msgType)
    {
    case RTC_MESSAGE_ROOM_EVENT_LOGIN_OK:
        {
            BRTC_LOG("RTC_MESSAGE_ROOM_EVENT_LOGIN_OK");
            break;
        }
    case RTC_MESSAGE_ROOM_EVENT_LOGIN_ERROR:
        {
            BRTC_LOG("RTC_MESSAGE_ROOM_EVENT_LOGIN_ERROR");
            if (connect_count > 5) {
                last_call_time = 0;
                brtc_sdk_do_async(videocall_hangup, NULL, 50);
            } else {
                brtc_sdk_do_async(videocall_reconnect, NULL, 50);
            }
            connect_count++;
            break;

        }
    case RTC_MESSAGE_ROOM_EVENT_CONNECTION_LOST:
        {
            BRTC_LOG("RTC_MESSAGE_ROOM_EVENT_CONNECTION_LOST");
            brtc_sdk_do_async(videocall_reconnect, NULL, 50);
            break;
        }
    case RTC_MESSAGE_ROOM_EVENT_REMOTE_COMING:
        {
            BRTC_LOG("RTC_MESSAGE_ROOM_EVENT_REMOTE_COMING");
            break;
        }
    case RTC_MESSAGE_ROOM_EVENT_REMOTE_LEAVING:
        {
            BRTC_LOG("RTC_MESSAGE_ROOM_EVENT_REMOTE_LEAVING");
            break;
        }
    case RTC_ROOM_EVENT_ON_USER_JOINED_ROOM:
        {
            BRTC_LOG("RTC_ROOM_EVENT_ON_USER_JOINED_ROOM");
            break;
        }
    case RTC_ROOM_EVENT_ON_USER_LEAVING_ROOM:
        {
            //brtc_sdk_do_async(videocall_hangup, NULL, 100);// SDK 20231130 DEL it
            break;
        }
    case RTC_ROOM_EVENT_ON_USER_MESSAGE:
        {
            //BRTC_LOG("Got User Message id: %" PRId64 ", msg: %s.\n", msg->data.feedId, msg->extra_info);
            break;
        }
    case RTC_MESSAGE_ROOM_EVENT_SERVER_ERROR:
        {
            //BRTC_LOG("ServerEvent error code: %" PRId64 ", reason: %s.\n", msg->data.errorCode, msg->extra_info);
            break;
        }
    }

    Baidu_Video_Call_Status_Callback(msg->msgType); // SDK 20231130 DEL it // wuxx add, must add it. 20231206
}

GUI_RECT_T hangup_in_vodeo_rect_brtc = VIDEO_CALL_IN_VIDEO_HANGUP_RECT;


//extern unsigned char gImage_icon_videocall_connect_ringoffBtn_brtc;

static void baidu_init_sdk()
{
    int ret = 0;
    connect_count = 0;
    BRTC_LOG("BRTC: SDK Version %s\n", brtc_sdk_version());
    brtc_sdk_enable_inner_av_devices(1);
    brtc_sdk_enable_nack(0);
    brtc_sdk_enable_log(1);
    brtc_sdk_set_video_display_size(lcd_width, lcd_height);// SDK 20231130 BAIDU_VIDEO_SIZE
    brtc_sdk_set_osd_area(1, (const char*)&gImage_icon_videocall_connect_ringoffBtn_brtc, hangup_in_vodeo_rect_brtc.left, hangup_in_vodeo_rect_brtc.top, hangup_in_vodeo_rect_brtc.right, hangup_in_vodeo_rect_brtc.bottom); // 90, 156, 149, 211 // wuxx add from BAIDU SDK 20231214 // gImage_icon_videocall_connect_ringoffBtn_brtc
    //brtc_sdk_set_osd_area(1,   (const char*)&gImage_icon_videocall_connect_ringoffBtn_brtc, 90, 156, 149, 215); // 90, 156, 149, 211 // wuxx add from BAIDU SDK 20231214 // gImage_icon_videocall_connect_ringoffBtn_brtc

    ret = brtc_sdk_init();
    if (ret) {
        BRTC_LOG("%s, init failed, ret=%d\n", __FUNCTION__, ret);
        return ;
    }

    if (gBrtcClient == NULL) {
        gBrtcClient = brtc_create_client();
        BRTC_LOG("create client success\n");
    }

    if (gBrtcClient) {
        brtc_register_message_listener(gBrtcClient, OnRtcMessage);
        brtc_set_cer(gBrtcClient, ca_info);
        brtc_set_server_url(gBrtcClient, brtcserver);
        brtc_set_customer_name(gBrtcClient, customer_name);
        brtc_set_appid(gBrtcClient, appid);

        brtc_set_auto_publish(gBrtcClient, 1);
        brtc_set_auto_subscribe(gBrtcClient, 1);
        brtc_set_awn_signal(gBrtcClient, 1);

        brtc_set_audiocodec(gBrtcClient, "pcmu");
        brtc_set_videocodec(gBrtcClient, "jpeg");
    }

}

static void baidu_login_room()
{
    int ret = 0;
    if (gBrtcClient) {
        ret = brtc_login_room(gBrtcClient, roomname, userid, display, token);
        if (ret) {
            BRTC_LOG("brtc_login_room success\n");
        } else {
            BRTC_LOG("brtc_login_room failed\n");
        }
    }
}

static void baidu_logout_room()
{
    int ret = 0;
    if (gBrtcClient) {
        ret = brtc_logout_room(gBrtcClient);
        if (ret) {
            BRTC_LOG("logout room success\r\n");
        } else {
            BRTC_LOG("logout room failed\r\n");
        }
    }
}

static void baidu_deinit_sdk()
{
    if (gBrtcClient) {
        brtc_destroy_client(gBrtcClient);
        brtc_sdk_deinit();
        gBrtcClient = NULL;
        BRTC_LOG("destroy success\r\n");
    }
}

static void videocall_hangup(void *h)
{
#ifdef APPID_GET_FROM_BAIDU
    last_call_time = 0;
    baidu_logout_room();
    baidu_deinit_sdk();
#endif
}

static void videocall_reconnect(void *h)
{
    baidu_logout_room();
    brtc_sdk_do_async(baidu_login_room, NULL, 50);
}

#define  VCHAT_IMEI_LEN 15

uint16 videocall_get_imei(MN_DUAL_SYS_E dual_sys,char * imei_ptr)
{
	MN_IMEI_T imei;
	char tmp_buf1[20] = {0};
	uint8 i=0, j=0;

	MNNV_GetIMEIEx(dual_sys, imei);		

	for(i = 0, j = 0; i < 8; i++)
	{
		tmp_buf1[j++] = (imei[i] & 0x0F) + '0';
		tmp_buf1[j++] = ((imei[i] >> 4) & 0x0F) + '0';
	}

	for( i=0; i<VCHAT_IMEI_LEN; i++ )
	{
		imei_ptr[i] = tmp_buf1[i+1];
	}
    
	imei_ptr[VCHAT_IMEI_LEN] = 0x00;

	return VCHAT_IMEI_LEN;
}

void videocall_demo_open(void)
{
    char cur_imei[VCHAT_IMEI_LEN+1] = {0};
    if (last_call_time == 0) {
        
#ifdef APPID_GET_FROM_BAIDU
        last_call_time = 100;
        strncpy(appid, APPID_GET_FROM_BAIDU, sizeof(appid) - 1);
        videocall_get_imei(MN_DUAL_SYS_1,cur_imei);
        sprintf(userid,"%s",cur_imei);
        //sprintf(userid, "600%04d", rand() % 10000);
        BRTC_LOG("videocall_demo_open appid=%s, userid=%s,roomname=%s",appid,userid, roomname);
        baidu_init_sdk();
        baidu_login_room();
#endif
    }
}

void videocall_demo_close(void)
{
    videocall_hangup(NULL);
}

void baidu_videocall_open(uint8 * app_id_str,uint8 * room_str)
{
    char cur_imei[VCHAT_IMEI_LEN+1] = {0};
    if (last_call_time == 0) {
        
#ifdef APPID_GET_FROM_BAIDU
        last_call_time = 100;
        strncpy(appid, app_id_str, sizeof(appid) - 1);
        videocall_get_imei(MN_DUAL_SYS_1,cur_imei);
        sprintf(userid,"%s",cur_imei);
        //sprintf(userid, "600%04d", rand() % 10000);
        //strncpy(roomname, room_str, sizeof(*room_str));
        strcpy(roomname, room_str);
        videocall_get_imei(MN_DUAL_SYS_1,cur_imei);
        BRTC_LOG("videocall_demo_open appid=%s, userid=%s,roomname=%s",appid,userid, room_str);
        BRTC_LOG("videocall_demo_open appid=%s, userid=%s,roomname-get=%s",appid,userid, roomname);
        baidu_init_sdk();
        baidu_login_room();
#endif
    }
}

void baidu_videocall_close(void)
{
    videocall_hangup(NULL);
}

//wuxx add for hangup_button_show . gImage_icon_videocall_connect_ringoffBtn_brtc 数组要在一个文件中,因为USER 版本可能显示异常
// gImage_icon_videocall_connect_ringoffBtn_brtc 定义在C文件中时USER 版本通话中挂断按钮颜色花绿色,DEBUG是红色, 奇怪问题
// 所以, 定义在.h 文件中然后用函数给  hangup_button_show 调用
void baidu_videocall_set_incall_hangup_button(BOOLEAN show,uint16_t left,uint16_t top,uint16_t right,uint16_t bottom)
{
    brtc_sdk_set_osd_area(show, (const char*)&gImage_icon_videocall_connect_ringoffBtn_brtc, left, top, right, bottom); // 90, 156, 149, 211 // wuxx add from BAIDU SDK 20231214 // gImage_icon_videocall_connect_ringoffBtn_brtc
}

#if 1
#define  BAIDU_TEST_MENU_LEN  20
#define  BAIDU_TEST_MENU_NUM  3
const uint8 baidu_test_menu_text[BAIDU_TEST_MENU_NUM][BAIDU_TEST_MENU_LEN]= 
{
    "V Open",
    "V Close",
    "V End"
};

LOCAL MMI_RESULT_E HandleBAIDU_TestMainMenuWinMsg (
                                      MMI_WIN_ID_T   win_id, 	// ???ID
                                      MMI_MESSAGE_ID_E 	msg_id, 	// ???????ID
                                      DPARAM            param		// ???????
                                      );

WINDOW_TABLE( BAIDU_VIDEOCHAT_TEST_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleBAIDU_TestMainMenuWinMsg),    
    WIN_ID( BAIDU_VIDEOCHAT_TEST_WIN_ID ),
    WIN_TITLE(TXT_FULL_SERVICE),
    WIN_HIDE_STATUS,
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, BAIDU_VIDEOCHAT_TEST_CTRL_ID),
    END_WIN
};

BOOLEAN MMIBAIDU_OpenTestMenuWin(void)
{
    MMK_CreateWin((uint32*)BAIDU_VIDEOCHAT_TEST_WIN_TAB,PNULL);
}

LOCAL void MMIAPISET_CommonTextId(                                    
                                                MMI_TEXT_ID_T        text_id,
                                                MMI_TEXT_ID_T        left_softkey_id,
                                                MMI_TEXT_ID_T        middle_softkey_id,
                                                MMI_TEXT_ID_T        right_softkey_id,
                                                MMI_CTRL_ID_T        ctrl_id,
                                                GUIITEM_STYLE_E      item_style
                                                )
{
    GUILIST_ITEM_T      item_t      =   {0};/*lint !e64*/
    GUILIST_ITEM_DATA_T item_data   =   {0};/*lint !e64*/
    
    item_t.item_style    = item_style;
    item_t.item_data_ptr = &item_data;
    
    item_data.item_content[0].item_data_type    = GUIITEM_DATA_TEXT_ID;
    item_data.item_content[0].item_data.text_id = text_id;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = middle_softkey_id;
    item_data.softkey_id[2] = right_softkey_id;
    
    GUILIST_AppendItem( ctrl_id, &item_t );      
}
/*****************************************************************************/
// 	Description : to handle the message of atcmd test 
//	Global resource dependence : 
//  Author:jianshengqi
//	Note: 2006/3/19 
/*****************************************************************************/
//extern uint32 test_main_lcd_id[20];
//extern uint32 test_sub_lcd_id[20];

LOCAL MMI_RESULT_E  HandleBAIDU_TestMainMenuWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T            ctrl_id = BAIDU_VIDEOCHAT_TEST_CTRL_ID;
    uint16                    node_id = 0;
    static BOOLEAN            s_need_update   = FALSE;
  //  MMISET_VALIDATE_PRIVACY_RETURN_T    result_info = {MMISET_PROTECT_CL, FALSE};    
#if defined(DISABLE_STATUSBAR_DISPLAY) 
    GUI_RECT_T                          title_rect =   {0, 0, 239, 23};
#else
    GUI_RECT_T                          title_rect = {0, 18, 239, 18 + 23};
#endif
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            uint16 i;
            GUILIST_ITEM_T      item_t = {0};
            GUILIST_ITEM_DATA_T item_data = {0};
            MMI_STRING_T		    item_str = {0};
            uint8				copy_len = 0;
            wchar buff[GUILIST_STRING_MAX_NUM + 1] = {0};
            
            GUILIST_SetMaxItem(ctrl_id, BAIDU_TEST_MENU_NUM,FALSE);   
            
            for ( i = 0; i < BAIDU_TEST_MENU_NUM; i++ )
            {
                item_t.item_style    = GUIITEM_STYLE_ONE_LINE_ICON_TEXT;
                item_t.item_data_ptr = &item_data;
                copy_len = SCI_STRLEN(baidu_test_menu_text[i]);
                item_data.item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
                item_data.item_content[0].item_data.image_id = IMAGE_SIM1;
                item_data.item_content[1].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
                item_data.item_content[1].item_data.text_buffer.wstr_ptr = buff;
                GUI_GB2UCS(item_data.item_content[1].item_data.text_buffer.wstr_ptr, baidu_test_menu_text[i],copy_len );
                item_data.item_content[1].item_data.text_buffer.wstr_len = MIN( GUILIST_STRING_MAX_NUM, copy_len);
                
                item_data.softkey_id[0] = TXT_COMMON_OK;
                item_data.softkey_id[1] = TXT_NULL;
                item_data.softkey_id[2] = STXT_RETURN;
                
                GUILIST_AppendItem(ctrl_id, &item_t);      
            }
            
            MMK_SetAtvCtrl(win_id, ctrl_id);
        }

        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {
        node_id = GUILIST_GetCurItemIndex(  ctrl_id );
        node_id++;
        switch( node_id )
        {
            case  1:
                    videocall_demo_open();
                break;
                
            case  2:
                    videocall_demo_open();
                break;
                
            case  3:
                    MMK_CloseWin(win_id);
                break;
                
            default:
                break;
        }
    }          
        break;
       
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
            videocall_demo_close();
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}
#endif


/****************************************************************************
** File Name:      mmiphs_app.h                                                *
** Author:                                                                 *
** Date:           03/22/2006                                              *
** Copyright:      2006 TLT, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the PHS                   *
/****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2006       Jianshengqi         Create
** 
****************************************************************************/
#ifndef  _MMI_ZDT_APP_H_    
#define  _MMI_ZDT_APP_H_   

#ifdef __cplusplus
    extern "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "zdt_common.h"
#include "zdt_db.h"
#include "zdt_id.h"
#include "zdt_menutable.h"
#include "zdt_nv.h"
#include "zdt_text.h"
#include "zdt_image.h"
#include "zdt_anim.h"
#ifdef ZDT_LED_SUPPORT
#include "zdt_led.h"
#endif

#ifdef ZDT_SMS_SUPPORT
#include "zdt_sms.h"
#endif
#if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)
#include "zdt_gps.h"
#endif
#ifdef ZDT_GSENSOR_SUPPORT
#include "zdt_gsensor.h"
#endif
#ifdef ZDT_HSENSOR_SUPPORT
#include "zdt_hsensor.h"
#endif
#ifdef ZDT_NET_SUPPORT
#include "zdt_net.h"
#ifdef ZDT_PLAT_HYHL_SUPPORT
#include "zdt_hyhl.h"
#endif
#ifdef ZDT_PLAT_YX_SUPPORT
#include "zdt_yx.h"
#endif
#endif
#ifdef ZDT_PLAT_SHB_SUPPORT
#include "zdt_shb.h"
#endif
#ifdef ZDT_WIFI_SUPPORT
#include "wifisupp_api.h"
#endif

#ifdef ZTE_WATCH
#define APP_DOWNLOAD_URL "http://ausp.nubia.com/care/?ln=cn&type=%s&imei=%s"
#else
#define APP_DOWNLOAD_URL "http://%s:8080/app/newIndex.html?imei=%s"
#endif

#define ZDTAPP_TRACE            ZDT_Trace

typedef struct
{
       uint8 *        str;
	uint32         len; 
} MMI_ZDT_DATA_T;

typedef struct
{
	SIGNAL_VARS
       MMI_ZDT_DATA_T data;
} MMI_ZDT_SIG_T ;

extern BOOLEAN is_zdt_jt_mode;

extern void MMIZDT_AppInit(void);
extern void MMIZDT_RegWinIdNameArr(void);
extern void MMIZDT_InitModule(void);
extern void MMIZDT_RegMenuGroup(void);
extern void MMIZDT_RegNv(void);
extern BOOLEAN MMIZDT_MoCallByNum(MN_DUAL_SYS_E dual_sys,uint8 * num_str);

extern BOOLEAN  MMIZDT_SendSigTo_APP(ZDT_APP_SIG_E sig_id, uint8 * data_ptr, uint32 data_len);
extern void MMIZDT_ResetFactory(void);

#ifdef ZDT_WIFI_SUPPORT
typedef int (*MMIZDTWIFIHANDLER) (uint8 loc_ok,void * pWifiData);
typedef struct _MMIZDTWIFI_LINK_DATA_t
{
    uint32 timeout;
    MMIZDTWIFIHANDLER rcv_handle;
} MMIZDTWIFI_LINK_DATA_T;

typedef struct _MMIZDTWIFI_LINK_NODE_t
{
       MMIZDTWIFI_LINK_DATA_T  data;
       struct _MMIZDTWIFI_LINK_NODE_t * next_ptr;
} MMIZDTWIFI_LINK_NODE_T;

extern BOOLEAN MMIZDTWIFI_PowerOn_LocOnce(void);
extern BOOLEAN MMIZDTWIFI_PowerOff_LocOnce(void);
extern BOOLEAN MMIZDTWIFI_IsLocOnceStart(void);
extern void MMIZDT_WIFI_OnOff(uint8 val) ;
extern uint8 MMIZDT_WIFI_GetOpen(void);
extern void MMIZDT_WIFI_SetOpen(uint8 val);
extern BOOLEAN MMIZDTWIFI_API_LocStart(uint32 time_s,MMIZDTWIFIHANDLER loc_callback);
extern BOOLEAN MMIZDTWIFI_API_LocStop(void);

#endif

#if defined(ZDT_W217_FACTORY_WIFI)// WUXX_20231212 wifi
uint8 MMIZDTWIFI_Start(void);
void MMIZDTWIFI_Stop(void);

void ZDT_W217_WIFI_ON_OFF_for_Net(BOOLEAN isOn);

#endif

#if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)
typedef int (*MMIZDTGPSHANDLER) (uint8 loc_ok,void * pGpsData);
typedef struct _MMIZDTGPS_LINK_DATA_t
{
    uint32 timeout;
    MMIZDTGPSHANDLER rcv_handle;
} MMIZDTGPS_LINK_DATA_T;

typedef struct _MMIZDTGPS_LINK_NODE_t
{
       MMIZDTGPS_LINK_DATA_T  data;
       struct _MMIZDTGPS_LINK_NODE_t * next_ptr;
} MMIZDTGPS_LINK_NODE_T;

extern void MMIZDTGPS_PowerOn_Interval(uint16 time_min);
extern void MMIZDTGPS_PowerOff_Interval(void);
extern BOOLEAN MMIZDTGPS_IsNeedLoc(void);
extern  void MMIZDTGPS_PowerOn(void);
extern  void MMIZDTGPS_PowerOff(void);
extern BOOLEAN MMIZDTGPS_API_LocStart(uint32 time_s,MMIZDTGPSHANDLER loc_callback);
extern BOOLEAN MMIZDTGPS_API_LocEnd(void);

extern BOOLEAN MMIZDTGPS_PowerOff_LocOnce(void);

#endif

#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231127

extern void MMIZDTGPS_PowerOn_for_w217(void);
extern void MMIZDTGPS_PowerOff_for_w217(void);

// wuxx_20231219  gps
extern void MMIZDTGPS_ClearGpsData(void);

#endif


PUBLIC BOOLEAN ZDT_Reject_Call(uint8 *number);
PUBLIC BOOLEAN ZDT_Device_Disable( GUI_POINT_T tp_point);
PUBLIC MMI_IMAGE_ID_T ZDT_PB_GetDispInfoContent(uint8 *tele_num, uint8 tele_len, wchar *in_str, BOOLEAN is_mo_call);
PUBLIC uint8 MMIZDT_GetFirstPWON(void);
PUBLIC BOOLEAN MMIZDT_NV_SetFirstPWON(uint8 first_pwon);
#ifdef SETUP_WIZARD_SUPPORT
PUBLIC uint8 MMIZDT_Get_Setup_Wizard_Status(void);
PUBLIC void MMIZDT_NV_Set_Setup_Wizard(uint8 status);
#endif

#ifdef ZDT_VIDEOCHAT_SUPPORT
PUBLIC void Video_Call_RequestContactSuccess(VIDEO_USER_INFO *video_call_user_info);
PUBLIC uint8 Video_Call_Incoming(VIDEO_CALL_INFO video_call_info);
PUBLIC uint8 Video_Call_Incoming_Test();
PUBLIC void Video_Call_Remote_Hangup();
#endif

PUBLIC BOOLEAN MMIZDT_ShouldStayInClassModeWin();

//************dropdown win************************
#define DROPDOWN_TOOLS_MARGIN_BOTTOM 10
#define DROPDOWN_TOOLS_IMG_HEIGHT 60
#define DROPDOWN_TOOLS_IMG_WIDTH 60
#define DROPDOWN_TOOLS_TOP MMI_MAINSCREEN_HEIGHT-DROPDOWN_TOOLS_MARGIN_BOTTOM-DROPDOWN_TOOLS_IMG_HEIGHT
#define DROPDOWN_TOOLS_RECT {10,DROPDOWN_TOOLS_TOP,70,MMI_MAINSCREEN_HEIGHT-DROPDOWN_TOOLS_MARGIN_BOTTOM}
#define DROPDOWN_SOUND_RECT {90,DROPDOWN_TOOLS_TOP,150,MMI_MAINSCREEN_HEIGHT-DROPDOWN_TOOLS_MARGIN_BOTTOM}
#define DROPDOWN_SETTINGS_RECT {170,DROPDOWN_TOOLS_TOP,MMI_MAINSCREEN_HEIGHT-DROPDOWN_TOOLS_MARGIN_BOTTOM,MMI_MAINSCREEN_HEIGHT-DROPDOWN_TOOLS_MARGIN_BOTTOM}
#define MMIDROPDOWNWIN_HEIGHT MMI_MAINSCREEN_HEIGHT
#define MMIDROPDOWNWIN_WIDTH MMI_MAINSCREEN_WIDTH
#define MMIDROPDOWNWIN_RECT {0,0,MMIDROPDOWNWIN_WIDTH - 1,MMIDROPDOWNWIN_HEIGHT - 1}
#define MMIDROPDOWNWIN_BGCOLOR MMI_BLACK_COLOR
#define MMIDROPDOWNWIN_INIT_POS (-MMIDROPDOWNWIN_HEIGHT)
#define MMIDROPDOWNWIN_STOP_POS 0
#define MMIDROPDOWNWIN_END_POS (-MMIDROPDOWNWIN_HEIGHT)
#ifdef MAINLCD_DEV_SIZE_240X284
#define DROPDOWN_TOP_TOOLS_IMG_MARGIN_TOP 52
#define DROPDOWN_TOP_TOOLS_BG_MARGIN_TOP 43
#define DROPDOWN_PRESS_REGION_HEGIHT 50 //底部收起下拉栏区域高度
#else
#define DROPDOWN_TOP_TOOLS_IMG_MARGIN_TOP 37
#define DROPDOWN_TOP_TOOLS_BG_MARGIN_TOP 28
#define DROPDOWN_PRESS_REGION_HEGIHT 37 //底部收起下拉栏区域高度
#endif
#define MMIDROPDOWNWIN_ROIREGION {0,MMIDROPDOWNWIN_HEIGHT-DROPDOWN_PRESS_REGION_HEGIHT,MMIDROPDOWNWIN_WIDTH,MMIDROPDOWNWIN_HEIGHT}
#define DROPDOWN_TOP_TOOLS_IMG_MARGIN_LEFT 21
#define DROPDOWN_TOP_TOOLS_BG_MARGIN_LEFT 12
#define DROPDOWN_TOP_TOOLS_IMG_HEIGHT 48
#define DROPDOWN_TOP_TOOLS_IMG_WIDTH 48
#define DROPDOWN_TOP_TOOLS_BG_HEIGHT 66
#define DROPDOWN_TOP_TOOLS_BG_WIDTH 66
#define DROPDOWN_TOP_TOOLS_OFFSET 27
#define DROPDOWN_TOP_TOOLS_BG_OFFSET 9

#define BATTERY_IMAGE_WIDTH 30 
#define BATTERY_IMAGE_HEIGHT 23
#define BATTERY_IMAGE_MARGIN_RIGHT 10
#define BATTERY_IMAGE_MARGIN_TOP 4
#define BATTERY_IMAGE_RECT {MMIDROPDOWNWIN_WIDTH-BATTERY_IMAGE_MARGIN_RIGHT-BATTERY_IMAGE_WIDTH,BATTERY_IMAGE_MARGIN_TOP,MMIDROPDOWNWIN_WIDTH-BATTERY_IMAGE_MARGIN_RIGHT,BATTERY_IMAGE_MARGIN_TOP+BATTERY_IMAGE_HEIGHT}

#define SIGNAL_IMAGE_WIDTH 35 
#define SIGNAL_IMAGE_HEIGHT 36
#define SIGNAL_IMAGE_MARGIN_RIGHT 30
#define SIGNAL_IMAGE_MARGIN_TOP 2
#define SIGNAL_IMAGE_RECT {SIGNAL_IMAGE_MARGIN_RIGHT,SIGNAL_IMAGE_MARGIN_TOP,SIGNAL_IMAGE_MARGIN_RIGHT+SIGNAL_IMAGE_WIDTH,SIGNAL_IMAGE_MARGIN_TOP+SIGNAL_IMAGE_HEIGHT}

#define SIGNAL_TEXT_WIDTH 20 
#define SIGNAL_TEXT_HEIGHT 10
#define SIGNAL_TEXT_MARGIN_RIGHT 27
#define SIGNAL_TEXT_MARGIN_TOP 2
#define SIGNAL_TEXT_RECT {SIGNAL_TEXT_MARGIN_RIGHT,SIGNAL_TEXT_MARGIN_TOP,SIGNAL_TEXT_MARGIN_RIGHT+SIGNAL_TEXT_WIDTH,SIGNAL_TEXT_MARGIN_TOP+SIGNAL_TEXT_HEIGHT}

#define VOLTE_IMAGE_WIDTH 18 
#define VOLTE_IMAGE_HEIGHT 30
#define VOLTE_IMAGE_MARGIN_RIGHT 10
#define VOLTE_IMAGE_MARGIN_TOP 10
#define VOLTE_IMAGE_RECT {VOLTE_IMAGE_MARGIN_RIGHT,VOLTE_IMAGE_MARGIN_TOP,VOLTE_IMAGE_MARGIN_RIGHT+VOLTE_IMAGE_WIDTH,VOLTE_IMAGE_MARGIN_TOP+VOLTE_IMAGE_HEIGHT}

//#define DROPDOWN_TOP_TOOLS_LIGHT_RECT {10,DROPDOWN_TOP_TOOLS_MARGIN_TOP,70,DROPDOWN_TOP_TOOLS_MARGIN_TOP+DROPDOWN_TOP_TOOLS_IMG_HEIGHT}
//#define DROPDOWN_SOUND_RECT {90,DROPDOWN_TOOLS_TOP,150,MMI_MAINSCREEN_HEIGHT-DROPDOWN_TOOLS_MARGIN_BOTTOM}
//************************************************


//*****************setup wizard*******************************
#if defined(ZTE_SUPPORT_240X284)||defined(MAINLCD_DEV_SIZE_240X284)
#ifdef ZTE_WATCH
#define	SETUP_WIZARD_NEXT_RECT {12,214,228,264};
#define SETUP_WIZARD_NEXT_TIPS_RECT {12,150,228,200} //下次提醒
#define SETUP_WIZARD_HAS_BIND_RECT {12,90,228,140} //已绑定
#define SETUP_WIZARD_TO_BIND_PAY_RECT {12,100,228,150}
#define SETUP_WIZARD_NEXT_BIND_PAY_RECT {12,170,228,220}
#else
#define SETUP_WIZARD_NEXT_RECT {20,214,220,254};
#define SETUP_WIZARD_NEXT_TIPS_RECT {35,150,205,180} //下次提醒
#define SETUP_WIZARD_HAS_BIND_RECT {50,90,190,120} //已绑定
#define SETUP_WIZARD_TO_BIND_PAY_RECT {137,120,203,150}
#define SETUP_WIZARD_NEXT_BIND_PAY_RECT {37,120,103,150}
#endif
#define SETUP_WIZARD_QRCODE_RECT {58, 58, 128, 130}
#define SETUP_WIZARD_TITLE_RECT {0,0,240,45}
#define SETUP_WIZARD_QRCODE_BG_START_X 45
#define SETUP_WIZARD_QRCODE_BG_START_Y 47
#else
#ifdef ZTE_WATCH
#define	SETUP_WIZARD_NEXT_RECT {12,184,228,230};
#define SETUP_WIZARD_NEXT_TIPS_RECT {12,150,228,200} //下次提醒
#define SETUP_WIZARD_HAS_BIND_RECT {12,90,228,140} //已绑定
#define SETUP_WIZARD_TO_BIND_PAY_RECT {12,100,228,150}
#define SETUP_WIZARD_NEXT_BIND_PAY_RECT {12,170,228,220}
#else
#define	SETUP_WIZARD_NEXT_RECT {20,184,220,224};
#define SETUP_WIZARD_NEXT_TIPS_RECT {35,150,205,180} //下次提醒
#define SETUP_WIZARD_HAS_BIND_RECT {50,90,190,120} //已绑定
#define SETUP_WIZARD_TO_BIND_PAY_RECT {137,120,203,150}
#define SETUP_WIZARD_NEXT_BIND_PAY_RECT {37,120,103,150}
#endif
#define SETUP_WIZARD_QRCODE_RECT {58, 48, 128, 120}
#define SETUP_WIZARD_TITLE_RECT {0,0,240,35}
#define SETUP_WIZARD_QRCODE_BG_START_X 45
#define SETUP_WIZARD_QRCODE_BG_START_Y 30
#endif
#define SETUP_WIZARD_NEXT_BG_COLOR 0x1185FC
#define SETUP_WIZARD_BIND_PAY_TIPS_RECT {0,40,240,80}
#define SETUP_WIZARD_INSERT_SIM_START_X 50
#define SETUP_WIZARD_INSERT_SIM_START_Y 78
//************************************************

//****************VIDEO CALL*******************************
#define VIDEO_CALL_CONTACT_LIST_X 0
#define VIDEO_CALL_CONTACT_LIST_Y 30
#define VIDEO_CALL_INCOMING_HANGUP_RECT {34,240,86,292}
#define VIDEO_CALL_INCOMING_ACCRPT_RECT {154,240,206,292}
#ifdef BAIDU_VIDEOCHAT_SUPPORT // BAIDU VIDEO in call hangup button
#ifdef MAINLCD_DEV_SIZE_240X284
#define VIDEO_CALL_IN_VIDEO_HANGUP_RECT {90, 246, 149, 305}
#define VIDEO_CALL_OUT_HANGUP_RECT {90, 246, 149, 305}
#else
#define VIDEO_CALL_IN_VIDEO_HANGUP_RECT {90, 176, 149, 235}
#define VIDEO_CALL_OUT_HANGUP_RECT {90, 176, 149, 235}
#endif
#else
#define VIDEO_CALL_IN_VIDEO_HANGUP_RECT {94,155,146,235}
#define VIDEO_CALL_OUT_HANGUP_RECT {90, 176, 149, 235}
#endif
#define VIDEO_CALL_STATE_RECT {0,5,MMI_MAINSCREEN_WIDTH,35}
#define VIDEO_CALL_NAME_RECT {0,35,MMI_MAINSCREEN_WIDTH,35}
//***********************************************

//***************find friend************************
#define SEARCH_ING_STR_RECT {0,10,240,40}
#define SEARCH_ING_TIP_STR_RECT {60,177,180,235}
#define FIND_FRIEND_PING_X 25
#define FIND_FRIEND_PING_Y 73
#define FIND_FRIEND_PONG_X 100
#define FIND_FRIEND_PONG_Y 112
#define FIND_FRIEND_WAITING_X 145
#define FIND_FRIEND_WAITING_Y 73
#define FIND_FRIEND_TIME_OUT 30000
#define FIND_FRIEND_SHAKE_IMG_H 80
#define FIND_FRIEND_SHAKE_IMG_W 80
#define FIND_FRIEND_SHAKE_IMG_X (MMI_MAINSCREEN_WIDTH-FIND_FRIEND_SHAKE_IMG_W)/2
#define FIND_FRIEND_SHAKE_IMG_Y (MMI_MAINSCREEN_HEIGHT-FIND_FRIEND_SHAKE_IMG_H)/2
#define FIND_FRIEND_SHAKE_FINDING_IMG_H 90
#define FIND_FRIEND_SHAKE_FINDING_IMG_W 212
#define FIND_FRIEND_SHAKE_FINDING_IMG_X (MMI_MAINSCREEN_WIDTH-FIND_FRIEND_SHAKE_FINDING_IMG_W)/2
#define FIND_FRIEND_SHAKE_FINDING_IMG_Y (MMI_MAINSCREEN_HEIGHT-FIND_FRIEND_SHAKE_FINDING_IMG_H)/2
//****************************************

//****************all app list*******************************
#define APP_LIST_RECT {0,0,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT}
#ifdef APP_COLUMNS_3
#define APP_LIST_ITEM_MARGIN_SPACE {7,3,8,10} //icon_item_hspace;icon_item_vspace;iconlist_hmargin;iconlist_vmargin;
#define APP_LIST_ITEM_ICON_WIDTH 70
#define APP_LIST_ITEM_ICON_HEIGHT 70
#else
#define APP_LIST_ITEM_MARGIN_SPACE {40,10,15,10} //icon_item_hspace;icon_item_vspace;iconlist_hmargin;iconlist_vmargin;
#define APP_LIST_ITEM_ICON_WIDTH 80
#define APP_LIST_ITEM_ICON_HEIGHT 80
#endif
//***********************************************

//************dropup win************************
#define MMIDROPUPWIN_HEIGHT MMI_MAINSCREEN_HEIGHT+3 //解决滑动到顶后有几个像素的高度没刷新
#define MMIDROPUPWIN_WIDTH MMI_MAINSCREEN_WIDTH
#define MMIDROPUPWIN_RECT {0,0,MMIDROPUPWIN_WIDTH,MMIDROPUPWIN_HEIGHT}
#define MMIDROPUPWIN_BGCOLOR MMI_BLACK_COLOR
#define MMIDROPUPWIN_ROIREGION {0,0,MMIDROPUPWIN_WIDTH,2}
#define MMIDROPUPWIN_INIT_POS (MMIDROPUPWIN_HEIGHT)
#define MMIDROPUPWIN_STOP_POS 0
#define MMIDROPUPWIN_END_POS (MMIDROPUPWIN_HEIGHT)
//************************************************

//****************phone book and dialer*******************************
#define LAUNCHER_PHONEBOOK_LABEL_RECT {12,20,228,68}
#define LAUNCHER_DIALER_LABEL_RECT {12,76,228,124}
#define LAUNCHER_PHONEBOOK_AND_DIALER_LABEL_TEXT_SPACE 15
//***********************************************
#ifdef MAINLCD_DEV_SIZE_240X284
#define LAUNCHER_CALL_PAGE_LABEL_RECT {80,62,160,142}
#define LAUNCHER_CALL_PAGE_TITLE_LABEL_RECT {83,175,162,205}
#define LAUNCHER_WECHAT_PAGE_LABEL_RECT {80,62,160,142}
#define LAUNCHER_WECHAT_PAGE_TITLE_LABEL_RECT {88,175,160,205}
#define LAUNCHER_LOW_BATTERY_TIP_RECT  {0,0,MMI_MAINSCREEN_WIDTH,196}
#define LAUNCHER_WECHAT_PAGE_NEW_MESSAGE_RECT {143,67,161,85}
#else
#define LAUNCHER_CALL_PAGE_LABEL_RECT {80,52,160,132}
#define LAUNCHER_CALL_PAGE_TITLE_LABEL_RECT {83,155,162,185}
#define LAUNCHER_WECHAT_PAGE_LABEL_RECT {80,52,160,132}
#define LAUNCHER_WECHAT_PAGE_TITLE_LABEL_RECT {88,155,160,185}
#define LAUNCHER_LOW_BATTERY_TIP_RECT  {0,0,MMI_MAINSCREEN_WIDTH,180}
#define LAUNCHER_WECHAT_PAGE_NEW_MESSAGE_RECT {143,57,161,75}
#endif

//****************wechat contact list *******************************
#define LAUNCHER_WECHAT_CONTACT_LIST_RECT {0,20,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT}
//***********************************************

//****************watch face 表盘 *******************************
#ifdef MAINLCD_DEV_SIZE_240X284
#define WATCH_PANEL_PREVIEW_IMG_HEGHT 190  //表盘预览
#define WATCH_PANEL_PREVIEW_IMG_WIDTH 160
#else
#define WATCH_PANEL_PREVIEW_IMG_HEGHT 180  //表盘预览
#define WATCH_PANEL_PREVIEW_IMG_WIDTH 180
#endif
#define WATCH_PANEL_PREVIEW_LEFT (MMI_MAINSCREEN_WIDTH-WATCH_PANEL_PREVIEW_IMG_WIDTH)/2
#define WATCH_PANEL_PREVIEW_TOP (MMI_MAINSCREEN_HEIGHT-WATCH_PANEL_PREVIEW_IMG_HEGHT)/2
#define WATCH_PANEL_PREVIEW_RIGHT MMI_MAINSCREEN_WIDTH-WATCH_PANEL_PREVIEW_LEFT
#define WATCH_PANEL_PREVIEW_BOTTOM MMI_MAINSCREEN_HEIGHT-WATCH_PANEL_PREVIEW_TOP
#define WATCH_PANEL_PREVIEW_IMG_RECT {WATCH_PANEL_PREVIEW_LEFT,WATCH_PANEL_PREVIEW_TOP,WATCH_PANEL_PREVIEW_RIGHT,WATCH_PANEL_PREVIEW_BOTTOM}

//太空人表盘
#define WATCH_PANEL_ASTRONAUT_DATE_MARGIN_BOTTOM 27 //日期屏幕底部边距
#define WATCH_PANEL_ASTRONAUT_DATE_MARGIN_LEFT 146  //日期屏幕左边距
#define WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH 14    //日期图片宽度
#define WATCH_PANEL_ASTRONAUT_DATE_Y MMI_MAINSCREEN_HEIGHT - WATCH_PANEL_ASTRONAUT_DATE_MARGIN_BOTTOM
#define WATCH_PANEL_ASTRONAUT_DATE_START_X WATCH_PANEL_ASTRONAUT_DATE_MARGIN_LEFT
#define WATCH_PANEL_ASTRONAUT_YEAR_START_X 33
#define WATCH_PANEL_ASTRONAUT_TIME_IMG_WIDTH 26    //时间图片宽度
#define WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_IMG_WIDTH 14
#define WATCH_PANEL_ASTRONAUT_COLON_IMG_WIDTH 6
#ifdef MAINLCD_DEV_SIZE_240X284
#define WATCH_PANEL_ASTRONAUT_TIME_MARGIN_TOP 59
#define WATCH_PANEL_ASTRONAUT_TIME_START_X 34
#define WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_MARGIN_RIGHT 50
#define WATCH_PANEL_ASTRONAUT_BATTERY_MARGIN_TOP 10
#define WATCH_PANEL_ASTRONAUT_POWER_START_X 126
#define WATCH_PANEL_ASTRONAUT_WEATHER_MARGIN_LEFT 25
#define WATCH_PANEL_ASTRONAUT_WEATHER_ICON_MARGIN_TOP 7
#define WATCH_PANEL_ASTRONAUT_WEATHER_MARGIN_TOP 10
#define WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_ICON_START_X 93
#define WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH 14    //温度图片宽度
#define WATCH_PANEL_ASTRONAUT_TIME_SECOND_MARGIN_TOP 70
#define WATCH_PANEL_ASTRONAUT_TIME_SECOND_START_X 173
#define WATCH_PANEL_ASTRONAUT_TIME_SECOND_IMG_WIDTH 17
#define WATCH_PANEL_ASTRONAUT_TIME_X_OFFSET 3
#define WATCH_PANEL_ASTRONAUT_WEEK_MARGIN_BOTTOM 68
#else
#define WATCH_PANEL_ASTRONAUT_TIME_MARGIN_TOP 42
#define WATCH_PANEL_ASTRONAUT_TIME_START_X 65 
#define WATCH_PANEL_ASTRONAUT_BATTERY_MARGIN_TOP 6
#define WATCH_PANEL_ASTRONAUT_POWER_START_X 115
#define WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_MARGIN_RIGHT 38
#define WATCH_PANEL_ASTRONAUT_WEATHER_MARGIN_LEFT 2
#define WATCH_PANEL_ASTRONAUT_WEATHER_ICON_MARGIN_TOP 4
#define WATCH_PANEL_ASTRONAUT_WEATHER_MARGIN_TOP 6
#define WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_ICON_START_X 74
#define WATCH_PANEL_ASTRONAUT_TIME_X_OFFSET 0
#define WATCH_PANEL_ASTRONAUT_WEEK_MARGIN_BOTTOM 58
#endif
#define WATCH_PANEL_ASTRONAUT_WEEK_Y MMI_MAINSCREEN_HEIGHT-WATCH_PANEL_ASTRONAUT_WEEK_MARGIN_BOTTOM
#define WATCH_PANEL_ASTRONAUT_WEEK_X 174

//海豚表盘
#define WATCH_PANEL_DOLPHIN_WEEK_MARGIN_BOTTOM 30
#define WATCH_PANEL_DOLPHIN_WEEK_MARGIN_LEFT 90
#define WATCH_PANEL_DOLPHIN_WEEK_HEIGHT 20
#define WATCH_PANEL_DOLPHIN_WEEK_WIDTH 70

//宇宙飞船 spaceship
#define WATCH_PANEL_SPACESHIP_WEEK_MARGIN_BOTTOM 40
#define WATCH_PANEL_SPACESHIP_WEEK_MARGIN_LEFT 135
#define WATCH_PANEL_SPACESHIP_WEEK_HEIGHT 30
#define WATCH_PANEL_SPACESHIP_WEEK_WIDTH 70
#define WATCH_PANEL_SPACESHIP_DATE_MARGIN_BOTTOM 40
#define WATCH_PANEL_SPACESHIP_DATE_MARGIN_LEFT 20
#define WATCH_PANEL_SPACESHIP_DATE_HEIGHT 30
#define WATCH_PANEL_SPACESHIP_DATE_WIDTH 70

//太空站表盘
#define WATCH_PANEL_SPACE_STATION_DATE_MARGIN_BOTTOM 34 //日期屏幕底部边距
#define WATCH_PANEL_SPACE_STATION_DATE_MARGIN_LEFT 102  //日期屏幕左边距
#define WATCH_PANEL_SPACE_STATION_DATE_IMG_WIDTH 14    //日期图片宽度
#define WATCH_PANEL_SPACE_STATION_DATE_Y MMI_MAINSCREEN_HEIGHT - WATCH_PANEL_SPACE_STATION_DATE_MARGIN_BOTTOM
#define WATCH_PANEL_SPACE_STATION_DATE_START_X WATCH_PANEL_SPACE_STATION_DATE_MARGIN_LEFT
#define WATCH_PANEL_SPACE_STATION_YEAR_START_X 33
#define WATCH_PANEL_SPACE_STATION_TIME_IMG_WIDTH 28    //时间图片宽度
#define WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_IMG_WIDTH 14
#define WATCH_PANEL_SPACE_STATION_COLON_IMG_WIDTH 16
#ifdef MAINLCD_DEV_SIZE_240X284
#define WATCH_PANEL_SPACE_STATION_TIME_MARGIN_TOP 38
#define WATCH_PANEL_SPACE_STATION_TIME_START_X 56
#define WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_MARGIN_RIGHT 92 //电量百分百
#define WATCH_PANEL_SPACE_STATION_BATTERY_MARGIN_TOP 11
#define WATCH_PANEL_SPACE_STATION_POWER_START_X 83 //电量图标
#define WATCH_PANEL_SPACE_STATION_WEATHER_MARGIN_LEFT 25
#define WATCH_PANEL_SPACE_STATION_WEATHER_ICON_MARGIN_TOP 84
#define WATCH_PANEL_SPACE_STATION_WEATHER_MARGIN_TOP 10
#define WATCH_PANEL_SPACE_STATION_WEATHER_TEMPERATURE_MARGIN_TOP 115 //摄氏度
#define WATCH_PANEL_SPACE_STATION_WEATHER_TEMPERATURE_ICON_START_X 46 //摄氏度
#define WATCH_PANEL_SPACE_STATION_WEATHER_TEMPERATURE_IMG_WIDTH 14    //温度图片宽度
#define WATCH_PANEL_SPACE_STATION_TIME_X_OFFSET 1
#endif
#define WATCH_PANEL_SPACE_STATION_WEEK_Y 86
#define WATCH_PANEL_SPACE_STATION_WEEK_X 98

//***********************************************

//***************wechar 微聊************************
#ifdef MAINLCD_DEV_SIZE_240X284
#define WECHART_TIPS_TEXT_MARGIN_LEFT 45
#define WECHART_TIPS_TEXT_MARGIN_RIGHT 194
#define WECHART_TIPS_TEXT_MARGIN_TOP 167
#define WECHART_TIPS_TEXT_MARGIN_BOTTOM 190
#else
#define WECHART_TIPS_TEXT_MARGIN_LEFT 45
#define WECHART_TIPS_TEXT_MARGIN_RIGHT 194
#define WECHART_TIPS_TEXT_MARGIN_TOP 154
#define WECHART_TIPS_TEXT_MARGIN_BOTTOM 177
#endif

#define WECHART_RECORD_IMG_WIDTH 160  //按住录音背景宽度
#define WECHART_RECORD_IMG_MARGIN_BOTTOM 58 //按住录音 top距离底部距离
#define WECHART_RECORD_TEXT_MARGIN_LEFT 88
#define WECHART_RECORD_TEXT_MARGIN_BOTTOM 5
#define WECHART_RECORD_TEXT_RECT {WECHART_RECORD_TEXT_MARGIN_LEFT,WECHART_RECORD_IMG_Y,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT-WECHART_RECORD_TEXT_MARGIN_BOTTOM}
#define WECHART_RECORD_COUNTDOWN_TEXT_MARGIN_BOTTOM MMI_MAINSCREEN_HEIGHT-WECHART_RECORD_IMG_MARGIN_BOTTOM
#define WECHART_RECORD_COUNTDOWN_TEXT_RECT {100,75,141,100}//{0,WECHART_RECORD_COUNTDOWN_TEXT_MARGIN_BOTTOM,40,MMI_MAINSCREEN_HEIGHT} //录音倒计时
#define WECHART_RECORD_TIP_RECT {WECHART_TIPS_TEXT_MARGIN_LEFT,WECHART_TIPS_TEXT_MARGIN_TOP,WECHART_TIPS_TEXT_MARGIN_RIGHT,WECHART_TIPS_TEXT_MARGIN_BOTTOM}
#define WECHART_SECONDS_IMG_WIDTH 116  //计时录音背景宽度
#define WECHART_SECONDS_IMG_HEIGHT 116  //计时录音背景宽度
#define WECHART_RECORD_SECONDS_IMG_X (MMI_MAINSCREEN_WIDTH-WECHART_SECONDS_IMG_WIDTH)/2
#define WECHART_RECORD_SECONDS_IMG_Y (MMI_MAINSCREEN_HEIGHT-WECHART_RECORD_IMG_MARGIN_BOTTOM-WECHART_SECONDS_IMG_HEIGHT)/2
#ifdef WECHAT_SEND_EMOJI
#define WECHART_EMOJI_IMG_X  13
#define WECHART_EMOJI_IMG_Y MMI_MAINSCREEN_HEIGHT-WECHART_RECORD_IMG_MARGIN_BOTTOM+8 
#define WECHART_EMOJI_IMG_W  38
#define WECHART_EMOJI_IMG_H  38
#define WECHART_EMOJI_IMG_RECT {WECHART_EMOJI_IMG_X,WECHART_EMOJI_IMG_Y,WECHART_EMOJI_IMG_X+WECHART_EMOJI_IMG_W, WECHART_EMOJI_IMG_Y+WECHART_EMOJI_IMG_H}
#define WECHART_EMOJI_LIST_ITEM_MARGIN_SPACE {7,3,8,10} //icon_item_hspace;icon_item_vspace;iconlist_hmargin;iconlist_vmargin;
#define WECHART_RECORD_IMG_X (MMI_MAINSCREEN_WIDTH-WECHART_RECORD_IMG_WIDTH)/2 + WECHART_EMOJI_IMG_X + 10
#define WECHART_RECORD_IMG_Y MMI_MAINSCREEN_HEIGHT-WECHART_RECORD_IMG_MARGIN_BOTTOM
#define WECHART_EMOJI_ICON_WIDTH 48
#define WECHART_EMOJI_ICON_HEIGHT 48
#else
#define WECHART_RECORD_IMG_X (MMI_MAINSCREEN_WIDTH-WECHART_RECORD_IMG_WIDTH)/2
#define WECHART_RECORD_IMG_Y MMI_MAINSCREEN_HEIGHT-WECHART_RECORD_IMG_MARGIN_BOTTOM
#endif

//****************************************

//***************拨号盘************************
#define DIAL_NUMBER_START_Y  62
#define DIAL_NUMBER_START_X  9
#define DIAL_NUMBER_IMG_WIDTH  72
#define DIAL_NUMBER_IMG_HEIGHT  38
#define DIAL_NUMBER_MARGIN_X 3
#ifdef MAINLCD_DEV_SIZE_240X284
#define DIAL_NUMBER_MARGIN_Y 13
#elif defined MAINLCD_DEV_SIZE_240X320
#define DIAL_NUMBER_MARGIN_Y 16
#else
#define DIAL_NUMBER_MARGIN_Y 3
#endif
#define DIAL_NUMBER_NEXT_X (DIAL_NUMBER_IMG_WIDTH+DIAL_NUMBER_MARGIN_X)
#define DIAL_NUMBER_NEXT_Y (DIAL_NUMBER_IMG_HEIGHT+DIAL_NUMBER_MARGIN_Y)
//***********************************************

//*********************************************************
#define WEATHER_SIM_RECT {0,200,(MMI_MAINSCREEN_WIDTH -1),MMI_MAINSCREEN_HEIGHT}
#define WEATHER_CITY_RECT {42,10,(MMI_MAINSCREEN_WIDTH -1),40}
#define WEATHER_UPDATE_TIME_RECT {150,169,(MMI_MAINSCREEN_WIDTH -1),199}
#define WEATHER_TEMPERATURE_RECT {145,110,(MMI_MAINSCREEN_WIDTH -1),160}
#define WEATHER_TEXT_RECT {150,60,MMI_MAINSCREEN_WIDTH,100}
#define WEATHER_ICON_X 40
#define WEATHER_ICON_Y 60
//*********************************************************

//**********************计算器***********************************
//#define CALC_NUMBER_START_Y  65
#define CALC_NUMBER_START_X  7
#define CALC_NUMBER_IMG_WIDTH  55
#define CALC_NUMBER_IMG_HEIGHT  36
#define CALC_NUMBER_MARGIN_X 2
#ifdef MAINLCD_DEV_SIZE_240X284
#define CALC_NUMBER_START_Y  65
#define CALC_NUMBER_MARGIN_Y 14
#elif defined MAINLCD_DEV_SIZE_240X320
#define CALC_NUMBER_START_Y  85
#define CALC_NUMBER_MARGIN_Y 16
#else
#define CALC_NUMBER_START_Y  65
#define CALC_NUMBER_MARGIN_Y 3
#endif
#define CALC_NUMBER_NEXT_X (CALC_NUMBER_IMG_WIDTH+CALC_NUMBER_MARGIN_X)
#define CALC_NUMBER_NEXT_Y (CALC_NUMBER_IMG_HEIGHT+CALC_NUMBER_MARGIN_Y)
#define CALC_BACKSPACE_MARGIN_RIGHT 50
#define CALC_BACKSPACE_IMG_WIDTH 33
#define CALC_BACKSPACE_IMG_HEIGHT 20
#define CALC_BACKSPACE_TOP 22
#define CALC_BACKSPACE_LEFT MMI_MAINSCREEN_WIDTH-CALC_BACKSPACE_MARGIN_RIGHT
#define CALC_BACKSPACE_RIGHT CALC_BACKSPACE_LEFT+CALC_BACKSPACE_IMG_WIDTH
#define CALC_BACKSPACE_BOTTOM CALC_BACKSPACE_TOP+CALC_BACKSPACE_IMG_HEIGHT
#define CALC_BACKSPACE_RECT {CALC_BACKSPACE_LEFT,CALC_BACKSPACE_TOP,CALC_BACKSPACE_RIGHT,CALC_BACKSPACE_BOTTOM}
//************************计算器*********************************

//***************AI 机器人 图灵 ************************
#define AICHAT_RECORD_ANIM_DELAY 500
#define AICHAT_RECORD_ANIMAL_TOTAL_FRAME_NUM  4
#define AI_CHAT_MAX_RECORD_TIME_MS 20000
#define AI_CHAT_RECORD_IMG_WIDTH 128
#define AI_CHAT_RECORD_IMG_HEIGHT 42
#define AI_CHAT_RECORD_IMG_MARGIN_BOTTOM 58
#define AI_CHAT_RECORD_IMG_X (MMI_MAINSCREEN_WIDTH-AI_CHAT_RECORD_IMG_WIDTH)/2
#define AI_CHAT_RECORD_IMG_Y MMI_MAINSCREEN_HEIGHT-AI_CHAT_RECORD_IMG_MARGIN_BOTTOM
#define AI_CHAT_RECORD_TEXT_MARGIN_LEFT 90
#define AI_CHAT_RECORD_TEXT_MARGIN_BOTTOM 16
#define AI_CHAT_RECORD_TEXT_RECT {AI_CHAT_RECORD_TEXT_MARGIN_LEFT,AI_CHAT_RECORD_IMG_Y,AI_CHAT_RECORD_TEXT_MARGIN_LEFT+AI_CHAT_RECORD_IMG_WIDTH,MMI_MAINSCREEN_HEIGHT-AI_CHAT_RECORD_TEXT_MARGIN_BOTTOM}
#define AI_CHAT_BOTTOM_RECT {0,MMI_MAINSCREEN_HEIGHT-AI_CHAT_RECORD_IMG_MARGIN_BOTTOM,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT}
//****************************************

//***************GALLERY 相册 ************************
#define GALLERY_LIST_ITEM_MARGIN_SPACE {30,10,10,10}
#define GALLERY_LIST_ITEM_ICON_WIDTH 80
#define GALLERY_LIST_ITEM_ICON_HEIGHT 80
#define GALLERY_LIST_MARGIN_BOTTOM 20 //列表距离底部边距
#define GALLERY_PICK_LIST_MARGIN_BOTTOM 40
#define GALLERY_PICK_BUTTON_WIDTH 80 //取消、删除按钮宽度
#define GALLERY_PICK_BUTTON_HEIGHT 40 
#define GALLERY_PICK_BUTTON_LEFT_RIGHT_MARGIN 25 //取消、删除按钮左右边距
#define GALLERY_PICK_BUTTON_MARGIN_TOP_BOTTOM 10 //取消、删除按钮距离底部边距

#define GALLERY_PICK_BUTTON_TOP MMI_MAINSCREEN_HEIGHT - GALLERY_PICK_BUTTON_MARGIN_TOP_BOTTOM - GALLERY_PICK_BUTTON_HEIGHT
#define GALLERY_PICK_CANEL_BUTTON_RIGHT GALLERY_PICK_BUTTON_LEFT_RIGHT_MARGIN+GALLERY_PICK_BUTTON_WIDTH

#define GALLERY_PICK_DELETE_ICON_OFFSET 21
#if defined(ZTE_SUPPORT_240X284)||defined(MAINLCD_DEV_SIZE_240X284)
#define GALLERY_ICONLIST_RECT             {0, 40, MMI_MAINSCREEN_WIDTH-1, MMI_MAINSCREEN_HEIGHT-10}
#define GALLERY_ICONLIST_MARK_RECT  {0, 40, MMI_MAINSCREEN_WIDTH-1, MMI_MAINSCREEN_HEIGHT-50}
#define GALLERY_PICK_DELETE_RECT         {124, MMI_MAINSCREEN_HEIGHT-47, 228, MMI_MAINSCREEN_HEIGHT-5}
#define GALLERY_PICK_CANEL_RECT         {12, MMI_MAINSCREEN_HEIGHT-47, 116, MMI_MAINSCREEN_HEIGHT-5}
#else
#define GALLERY_ICONLIST_RECT             {0, 40, MMI_MAINSCREEN_WIDTH-1, MMI_MAINSCREEN_HEIGHT-10}
#define GALLERY_ICONLIST_MARK_RECT  {0, 40, MMI_MAINSCREEN_WIDTH-1, MMI_MAINSCREEN_HEIGHT-50}
#define GALLERY_PICK_DELETE_RECT         {124, MMI_MAINSCREEN_HEIGHT-47, 228, MMI_MAINSCREEN_HEIGHT-5}
#define GALLERY_PICK_CANEL_RECT         {12, MMI_MAINSCREEN_HEIGHT-47, 116, MMI_MAINSCREEN_HEIGHT-5}
#endif

#define CAMERA_OPEN_GALLERY_RECT DP2PX_RECT(40, 258,  87, 285)
#define CAMERA_CAPTURE_IMAGE_RECT DP2PX_RECT(96, 248, 143, 295)
#define CAMERA_SWITCH_CAMERA_RECT DP2PX_RECT(172, 258, 219, 285)
//****************************************************

//***************************************************
#ifdef MAINLCD_DEV_SIZE_240X284
#define SOS_ICON_START_Y 28
#define SOS_TIP_RECT {20,148,220,180}
#define SOS_CALL_BG_RECT {12,191,228,231}
#else
#define SOS_ICON_START_Y 18
#define SOS_TIP_RECT {20,138,220,170}
#define SOS_CALL_BG_RECT {12,181,228,221}
#endif
#define SOS_ICON_START_X 67

//**************************************************

#ifdef __cplusplus
    }
#endif

#endif

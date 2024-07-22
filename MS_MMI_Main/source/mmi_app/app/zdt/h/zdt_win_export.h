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
#ifndef  _MMI_ZDT_WIN_EXPORT_H_    
#define  _MMI_ZDT_WIN_EXPORT_H_   

#include "watch_slidewin.h"
#include "mmicl_export.h"
#include "mmialarm_export.h"
#ifdef ZTE_WATCH
#if defined(ZDT_GSENSOR_SUPPORT) &&  defined(ZTE_PEDOMETER_SUPPORT)
#include "pedometer_export.h"
#endif
#ifdef ZTE_WEATHER_SUPPORT
#include "zteweather_main.h"
#endif
#include "zte_stopwatch_win.h"
#endif

#ifdef __cplusplus
    extern "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
//微聊
PUBLIC void MMIAPIMENU_EnterTinyChat();

#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
PUBLIC void MMIZDT_OpenChatGroupWin(void);
#endif


PUBLIC void MMIZDT_OpenManualWin();
PUBLIC void MMIZDT_Open2VMWin(void);
PUBLIC void MMIZDT_OpenChargingWin();
PUBLIC void MMIAPILOWBATTERY_CloseWin();
PUBLIC void MMIAPILOWBATTERY_CreateLowBatteryWin();
//计步
PUBLIC void MMIZDT_OpenPedometerWin(void);
//拨号
PUBLIC void MMIZDT_OpenDialWin(void );
//插SIM卡提示
PUBLIC void MMIZDT_OpenInsertSimWin();
PUBLIC void MMIZDT_OpenTestTpWin(void);
//通讯录
PUBLIC void MMIZDT_OpenPBWin(void);
PUBLIC void MMIZDT_Open2VMSelectWin(void);
//天气
PUBLIC void MMIZDT_OpenWeatherWin();
PUBLIC BOOLEAN MMIZDT_IsClassModeWinOpen();
//上课禁用
PUBLIC void MMIZDT_OpenClassModeWin();
//没网络或者SIM卡
PUBLIC void MMIZDT_OpenNoSimOrDataWin();
//下滑栏
PUBLIC void MMIZDT_DropDown_EnterWin(eSlideWinStartUpMode launch_mode);
//是否在充电窗口
PUBLIC BOOLEAN MMIZDT_IsInChargingWin();
//加好友
PUBLIC void MMIZDT_OpenFindFriendWin();
PUBLIC void MMIZDT_OpenFriendPPWin(void);
//课程表
PUBLIC void MMIZDT_OpenScheduleWin(void);

PUBLIC void MMIZDT_OpenChatContactListWin(void);

#if defined(FOTA_SUPPORT)
//FOTA
PUBLIC void MMIAPISET_EnterFotaWin( void );
#endif
PUBLIC MMI_RESULT_E  HandleZDT_ChatGroupWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );


//PUBLIC MMI_RESULT_E HandleLauncher_ChatContactListWinMsg( MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);// wuxx del it because new CODE. WUXX_MODIFY_DEL_TMP_20231118

//铃声
PUBLIC void Settings_CallRingSelectWin_Enter( void );
//亮度设置
PUBLIC void MMIAPISET_ZdtBrightnessWin(void);
//背光超时
PUBLIC void Settings_BackLight_Enter( void );
//音量
PUBLIC void MMIAPISET_ZdtVolumeWin(void);
//恢复出厂
PUBLIC void MMIAPISET_ZdtPowerRestoreWin(void);
//关机
PUBLIC void MMIAPISET_ZdtPowerOffWin(void);
//重启
PUBLIC void MMIAPISET_ZdtPowerRestartWin(void);
//关于
PUBLIC void Settings_RegardWin_Enter(void);

PUBLIC void MMIZDT_CheckOpenClassModeWin();

//秒表
#ifdef ZTE_STOPWATCH_SUPPORT
PUBLIC void ZTE_STOPWATCH_OpenMainWin( void );
#endif


//工具箱
#if defined(ZDT_TOOLS_MENU_SUPPORT)// wuxx add.
PUBLIC void WatchTools_MainWin_Enter( void );
#endif

PUBLIC void ZDT_DisplayBattery(MMI_WIN_ID_T win_id);
PUBLIC void ZDT_DisplaySingal(MMI_WIN_ID_T win_id);

#if defined(HERO_ENGINE_SUPPORT)
#if defined(HERO_APP_WSTORE_OPEN)
PUBLIC void launcher_hero_appstore();
PUBLIC void launcher_hero_appstore_manager();
#endif
#endif

#ifdef ZDT_ZFB_SUPPORT
PUBLIC void MMIZFB_OpenMainWin();
#endif

#ifdef LEBAO_MUSIC_SUPPORT
PUBLIC void MMIAPIMENU_EnterLebao(void);
#endif

#if defined(XYSDK_SUPPORT)|| defined(XYSDK_SRC_SUPPORT)
#ifndef WIN32
PUBLIC void LIBXMLYAPI_CreateXysdkMainWin(void);
#endif
#endif

#ifdef FORMULA_SUPPORT
PUBLIC void MMI_CreateMathMnemonicWin(void);
#endif

#ifdef LISTENING_PRATICE_SUPPORT
PUBLIC void MMI_CreateListeningLocalWin(void);
#endif

#ifdef WORD_CARD_SUPPORT
PUBLIC void MMI_CreateWordWin(void);
#endif

#ifdef POETRY_LISTEN_SUPPORT
PUBLIC void MMI_CreatePoetryWin(void);
#endif

#ifdef HANZI_CARD_SUPPORT
PUBLIC void MMI_CreateHanziWin(void);
#endif

#ifdef ZMT_DIAL_STORE_SUPPORT
PUBLIC void MMI_CreateZmtDialStoreWin(void);
#endif

#ifdef ZMT_GPT_SUPPORT
PUBLIC void MMIZMT_CreateZmtGptWin(void);
#endif

#ifdef ZMT_CLASS_SUPPORT
PUBLIC void MMI_CreateClassMainWin(void);
#endif

#ifdef ZMT_PINYIN_SUPPORT
PUBLIC void MMI_CreatePinyinMainWin(void);
#endif

#ifdef ZMT_YINBIAO_SUPPORT
PUBLIC void MMI_CreateYinbiaoMainWin(void);
#endif

#ifdef MAINMENU_STYLE_SUPPORT
PUBLIC void WatchSET_MenuStyle_Enter( void );//菜单切换
#endif

#ifdef ZTE_WATCH
PUBLIC void WatchSET_LongRangeModeExitTipsWin(void);
PUBLIC MMI_RESULT_E  HandleZTE_LowBatteryWinMsg(MMI_WIN_ID_T  win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
//检测SIM卡,没插SIM则弹出未插SIM卡窗口 只检测SIM卡状态不检测数据连接状态
PUBLIC BOOLEAN MMIZDT_CheckSimStatus();
#endif

//****四宫格菜单**************//
//左上菜单点击区域
#define LEFT_TOP_RECT {10,10,110,110}
//右上菜单点击区域
#define RIGHT_TOP_RECT {130,10,230,110}
//左下菜单点击区域
#define LEFT_BOTTOM_RECT {10,130,110,230}
//右下菜单点击区域
#define RIGHT_BOMTTOM_RECT {130,130,230,230}
//第一个元素代表是否需要检测插卡提示 第二个点击范围，第三点击回调函数
#define MENU_PHONEBOOK {1,LEFT_TOP_RECT,MMIZDT_OpenPBWin} //通讯录
#define MENU_DIAL {0,RIGHT_TOP_RECT,MMIZDT_OpenDialWin/*WatchCL_ListWin_Enter*//*WatchCC_DialpadWin_Enter*/} //拨号
#define MENU_AICHART {1,LEFT_BOTTOM_RECT,MMIAPIMENU_EnterAiChat} //机器人
#define MENU_VIDEO_CALL {1,RIGHT_BOMTTOM_RECT,MMIVideo_Call_MainWin} //视频通话

#define MENU_WECHART {1,LEFT_TOP_RECT,MMIAPIMENU_EnterTinyChat} //微聊
#define MENU_CAMERA {0,RIGHT_TOP_RECT,MMIAPIDC_OpenPhotoWin} //相机
#define MENU_GALLERY {0,LEFT_BOTTOM_RECT,WatchGallery_MainWin_Enter} //相册
#define MENU_BIND_DEVICE {0,RIGHT_BOMTTOM_RECT,MMIZDT_Open2VMSelectWin} //绑定流程

#define MENU_SETTINGS {0,LEFT_TOP_RECT,WatchSET_MainWin_Enter} //设置
#define MENU_STOPWATCH {0,RIGHT_TOP_RECT,WatchStopWatch_MainWin_Enter} //秒表
#define MENU_CALCULATOR {0,LEFT_BOTTOM_RECT,MMIAPICALC_OpenMainWin} //计算器
#define MENU_ALARM {0,RIGHT_BOMTTOM_RECT,WatchAlarm_MainWin_Enter} //闹钟

#define MENU_WEATHER {0,LEFT_TOP_RECT,MMIZDT_OpenWeatherWin} //天气
#define MENU_PEDOMETER {0,RIGHT_BOMTTOM_RECT,MMIZDT_OpenPedometerWin} //计步
#define MENU_MANUAL {0,LEFT_BOTTOM_RECT,MMIZDT_OpenManualWin} //说明书
#ifdef LEBAO_MUSIC_SUPPORT
#define MENU_MUISE {0,RIGHT_TOP_RECT,MMIAPIMENU_EnterLebao} //乐宝音乐
#endif
#define MENU_CALLLOG {1,RIGHT_TOP_RECT,WatchCL_ListWin_Enter} //通话记录
#define MENU_FIND_FRIEND {1,RIGHT_TOP_RECT,MMIZDT_OpenFriendPPWin/*MMIZDT_OpenFindFriendWin*/} //加好友



#ifdef WIN32
extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MENU_VIDEO_INCOMING_TEST {0,RIGHT_BOMTTOM_RECT,Video_Call_Incoming_Test} //视频通话来电
#endif
#define MENU_SCHEDULE {0,LEFT_TOP_RECT,MMIZDT_OpenScheduleWin} //课程表


#if defined(ZDT_TOOLS_MENU_SUPPORT)// wuxx add.
#define MENU_WATCH_TOOLS {0,LEFT_BOTTOM_RECT,WatchTools_MainWin_Enter} //工具箱
#endif


#if defined(HERO_ENGINE_SUPPORT)
#if defined(HERO_APP_WSTORE_OPEN)
#define MENU_HERO_APPSTORE {0,LEFT_TOP_RECT,launcher_hero_appstore} //应用商城,应用市场
#define MENU_HERO_APPSTORE_MANAGER {0,RIGHT_TOP_RECT,launcher_hero_appstore_manager} //应用管理,应用中心
#endif
#endif

#ifdef ZDT_ZFB_SUPPORT
#define MENU_ALIPAY {1,LEFT_BOTTOM_RECT,MMIZFB_OpenMainWin} //支付宝
#endif

extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MENU_VIDEO_INCOMING_TEST {0,RIGHT_BOMTTOM_RECT,Video_Call_Incoming_Test} //视频通话来电


//****菜单**************//
//1-绑定，2-拨号，3-计算器，4-计步，5-交友，6-秒表，7-闹钟，8-设置，9-视频通话，10-天气，11-通话记录，12-通讯录，13-微聊，14-喜马拉雅，15-相册，16-相机，17-小度，18-支付宝
//****** 列表菜单***************//
#define MENU_TEXTBOOK {12, 1, textbook_text, res_app_ic_contact, textbook_bg, MMIZDT_OpenPBWin}
#define MENU_PHONEBOOK {12, 1, contact_text, res_app_ic_contact, bright_green_bg, MMIZDT_OpenPBWin}
#define MENU_CAMERA {16, 0, camera_text, res_app_ic_camera, light_cyan_bg, MMIAPIDC_OpenPhotoWin} //相机
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    #ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
	#define MENU_WECHART {13, 1, wechat_text, res_app_ic_wechat, bright_yellow_bg, MMIZDT_OpenChatGroupWin}//微聊
    #else       //ZDT_PLAT_YX_SUPPORT_FRIEND
    #define MENU_WECHART {13, 1, wechat_text, res_app_ic_wechat, bright_yellow_bg, MMIAPIMENU_EnterTinyChat}//微聊
    #endif      // end ZDT_PLAT_YX_SUPPORT_FRIEND
#endif // end ZDT_PLAT_YX_SUPPORT_VOICE
#define MENU_GALLERY {15, 0, gallery_text, res_app_ic_gallery,bright_purple_bg, WatchGallery_MainWin_Enter} //相册

#define MENU_SETTINGS {8, 0, settings_text, res_app_ic_settings, light_gray_bg, WatchSET_MainWin_Enter}
#define MENU_VIDEO_CALL {9, 1, video_call_text, res_app_ic_video_call, dark_red_bg, MMIVideo_Call_MainWin} //视频通话
//#define MENU_WEATHER  {10, 1, weather_text, res_app_ic_weather,dark_red_bg, ZTEWEATHER_CreateMainWin} //天气
#define MENU_WEATHER  {10, 1, weather_text, res_app_ic_weather,light_cyan_bg, MMIZDT_OpenWeatherWin}
#define MENU_CALLLOG {11, 0, calllog_text, res_app_ic_calllog, light_pink_bg, WatchCL_ListWin_Enter} //通话记录 
#define MENU_FIND_FRIEND  {5, 1, find_friend_text, res_app_ic_find_friend, bright_green_bg, MMIZDT_OpenFriendPPWin} //加好友
#define MENU_DIAL   {2, 0, dialer_text, res_app_ic_dailer, dark_blue_bg, MMIZDT_OpenDialWin}//拨号盘
#ifdef ZDT_ZFB_SUPPORT
#define MENU_ALIPAY  {18, 1, alipay_text, res_app_ic_alipay, light_blue_bg, MMIZFB_OpenMainWin} //支付宝
#endif
#ifdef TULING_AI_SUPPORT
#define MENU_AI_CHAT   {0, 1, ai_assistant_text, res_app_ic_ai_chat, bright_yellow_bg, MMIAPIMENU_EnterAiChat}//图灵AI
#endif
#ifdef BAIDU_AI_SUPPORT
#define MENU_XIAODU   {17, 1, xiaodu_text, res_app_ic_xiaodu, bright_yellow_bg, MMIAPIMENU_EnterAiChat}//小度
#endif
#if defined(XYSDK_SUPPORT)|| defined(XYSDK_SRC_SUPPORT)
#define MENU_XMLY {14, 1,xmla_text, res_app_ic_ximalaya, dark_orange_bg, LIBXMLYAPI_CreateXysdkMainWin}//喜马拉雅
#endif
#ifdef LEBAO_MUSIC_SUPPORT
#define MENU_MUSIC {0, 1,migu_music_text,res_app_ic_music, dark_red_bg, MMIAPIMENU_EnterLebao}//咪咕音乐
#endif
#ifdef WIN32
extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MENU_VIDEO_INCOMING_TEST {9, 0,video_call_text,res_app_ic_video_call, dark_red_bg, Video_Call_Incoming_Test} //视频通话来电
#endif

#ifdef FORMULA_SUPPORT
#define MENU_MNEMONICS {19, 1, mnemonics_text, res_app_ic_mnemonics, bright_purple_bg, MMI_CreateMathMnemonicWin} 
#endif

#ifdef LISTENING_PRATICE_SUPPORT
#define MENU_LISTEN {20, 1, listen_text, res_app_ic_listen, light_cyan_bg, MMI_CreateListeningLocalWin} 
#endif

#ifdef WORD_CARD_SUPPORT
#define MENU_LEARN_WORD {21, 1, learn_word_text, res_app_ic_learn_word, light_blue_bg, MMI_CreateWordWin} 
#endif

#ifdef POETRY_LISTEN_SUPPORT
#define MENU_POETRY {22, 1, poetry_text, res_app_ic_poetry, bright_yellow_bg, MMI_CreatePoetryWin} 
#endif

#ifdef HANZI_CARD_SUPPORT
#define MENU_HANZI {23, 1, hanzi_text, res_app_ic_hanzi, cherry_bg, MMI_CreateHanziWin}
#endif

#ifdef ZMT_GPT_SUPPORT
#define MENU_AI {24, 1, ai_text, res_app_ic_ai, dark_red_bg, MMIZMT_CreateZmtGptWin} 
#endif

#if defined(ZDT_TOOLS_MENU_SUPPORT)// wuxx add.
#define MENU_TOOLS {25, 0, tools_text, res_app_ic_tools, bright_purple_bg, WatchTools_MainWin_Enter} 
#endif

#ifdef ZMT_CLASS_SUPPORT
#define MENU_CLASS {26, 1, class_sync_text, res_app_ic_contact, class_sync_bg, MMI_CreateClassMainWin} 
#endif

#ifdef ZMT_PINYIN_SUPPORT
#define MENU_PINYIN {27, 1, pinyin_text, res_app_ic_pinyin, bright_yellow_bg, MMI_CreatePinyinMainWin} 
#endif

#ifdef ZMT_YINBIAO_SUPPORT
#define MENU_YINBIAO {28, 1, yinbiao_text, res_app_ic_yinbiao, bright_purple_bg, MMI_CreateYinbiaoMainWin} 
#endif
//*******菜单***************//

#ifdef __cplusplus
    }
#endif

#endif

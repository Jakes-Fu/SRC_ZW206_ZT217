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
#include "pedometer_export.h"
#include "zteweather_main.h"
#include "zte_stopwatch_win.h"
#endif

#ifdef __cplusplus
    extern "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
//΢��
PUBLIC void MMIAPIMENU_EnterTinyChat();
 
#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
PUBLIC void MMIZDT_OpenChatGroupWin(void);
#endif


PUBLIC void MMIZDT_OpenManualWin();
PUBLIC void MMIZDT_Open2VMWin(void);
PUBLIC void MMIZDT_OpenChargingWin();
PUBLIC void MMIAPILOWBATTERY_CloseWin();
PUBLIC void MMIAPILOWBATTERY_CreateLowBatteryWin();
//�Ʋ�
PUBLIC void MMIZDT_OpenPedometerWin(void);
//����
PUBLIC void MMIZDT_OpenDialWin(void );
//��SIM����ʾ
PUBLIC void MMIZDT_OpenInsertSimWin();
PUBLIC void MMIZDT_OpenTestTpWin(void);
//ͨѶ¼
PUBLIC void MMIZDT_OpenPBWin(void);
PUBLIC void MMIZDT_Open2VMSelectWin(void);
//����
PUBLIC void MMIZDT_OpenWeatherWin();
PUBLIC BOOLEAN MMIZDT_IsClassModeWinOpen();
//�Ͽν���
PUBLIC void MMIZDT_OpenClassModeWin();
//û�������SIM��
PUBLIC void MMIZDT_OpenNoSimOrDataWin();
//�»���
PUBLIC void MMIZDT_DropDown_EnterWin(eSlideWinStartUpMode launch_mode);
//�Ƿ��ڳ�細��
PUBLIC BOOLEAN MMIZDT_IsInChargingWin();
//�Ӻ���
PUBLIC void MMIZDT_OpenFindFriendWin();
PUBLIC void MMIZDT_OpenFriendPPWin(void);
//�γ̱�
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

//����
PUBLIC void Settings_CallRingSelectWin_Enter( void );
//��������
PUBLIC void MMIAPISET_ZdtBrightnessWin(void);
//���ⳬʱ
PUBLIC void Settings_BackLight_Enter( void );
//����
PUBLIC void MMIAPISET_ZdtVolumeWin(void);
//�ָ�����
PUBLIC void MMIAPISET_ZdtPowerRestoreWin(void);
//�ػ�
PUBLIC void MMIAPISET_ZdtPowerOffWin(void);
//����
PUBLIC void MMIAPISET_ZdtPowerRestartWin(void);
//����
PUBLIC void Settings_RegardWin_Enter(void);

PUBLIC void MMIZDT_CheckOpenClassModeWin();

//���
#ifdef ZTE_STOPWATCH_SUPPORT
PUBLIC void ZTE_STOPWATCH_OpenMainWin( void );
#endif


//������
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

#ifdef MATH_COUNT_SUPPORT
PUBLIC void MMIZMT_CreateMathCountWin(void);
#endif

#ifdef LISTENING_PRATICE_SUPPORT
PUBLIC void MMIREADBOY_CreateListeningLocalWin(void);
#endif

#ifdef WORD_CARD_SUPPORT
PUBLIC void MMI_CreateWordWin(void);
#endif

#ifdef POETRY_LISTEN_SUPPORT
PUBLIC void MMIREADBOY_CreatePoetryWin(void);
#endif

#ifdef MAINMENU_STYLE_SUPPORT
PUBLIC void WatchSET_MenuStyle_Enter( void );//�˵��л�
#endif

#ifdef ZTE_WATCH
PUBLIC void WatchSET_LongRangeModeExitTipsWin(void);
PUBLIC MMI_RESULT_E  HandleZTE_LowBatteryWinMsg(MMI_WIN_ID_T  win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);
//���SIM��,û��SIM�򵯳�δ��SIM������ ֻ���SIM��״̬�������������״̬
PUBLIC BOOLEAN MMIZDT_CheckSimStatus();
#endif

//****�Ĺ���˵�**************//
//���ϲ˵��������
#define LEFT_TOP_RECT {10,10,110,110}
//���ϲ˵��������
#define RIGHT_TOP_RECT {130,10,230,110}
//���²˵��������
#define LEFT_BOTTOM_RECT {10,130,110,230}
//���²˵��������
#define RIGHT_BOMTTOM_RECT {130,130,230,230}
//��һ��Ԫ�ش����Ƿ���Ҫ���忨��ʾ �ڶ��������Χ����������ص�����
#define MEUN_PHONEBOOK {1,LEFT_TOP_RECT,MMIZDT_OpenPBWin} //ͨѶ¼
#define MEUN_DIAL {0,RIGHT_TOP_RECT,MMIZDT_OpenDialWin/*WatchCL_ListWin_Enter*//*WatchCC_DialpadWin_Enter*/} //����
#define MEUN_AICHART {1,LEFT_BOTTOM_RECT,MMIAPIMENU_EnterAiChat} //������
#define MEUN_VIDEO_CALL {1,RIGHT_BOMTTOM_RECT,MMIVideo_Call_MainWin} //��Ƶͨ��

#define MEUN_WECHART {1,LEFT_TOP_RECT,MMIAPIMENU_EnterTinyChat} //΢��
#define MEUN_CAMERA {0,RIGHT_TOP_RECT,MMIAPIDC_OpenPhotoWin} //���
#define MEUN_GALLERY {0,LEFT_BOTTOM_RECT,WatchGallery_MainWin_Enter} //���
#define MEUN_BIND_DEVICE {0,RIGHT_BOMTTOM_RECT,MMIZDT_Open2VMSelectWin} //������

#define MEUN_SETTINGS {0,LEFT_TOP_RECT,WatchSET_MainWin_Enter} //����
#define MEUN_STOPWATCH {0,RIGHT_TOP_RECT,WatchStopWatch_MainWin_Enter} //���
#define MEUN_CALCULATOR {0,LEFT_BOTTOM_RECT,MMIAPICALC_OpenMainWin} //������
#define MEUN_ALARM {0,RIGHT_BOMTTOM_RECT,WatchAlarm_MainWin_Enter} //����

#define MEUN_WEATHER {0,LEFT_TOP_RECT,MMIZDT_OpenWeatherWin} //����
#define MEUN_PEDOMETER {0,RIGHT_BOMTTOM_RECT,MMIZDT_OpenPedometerWin} //�Ʋ�
#define MEUN_MANUAL {0,LEFT_BOTTOM_RECT,MMIZDT_OpenManualWin} //˵����
#ifdef LEBAO_MUSIC_SUPPORT
#define MEUN_MUISE {0,RIGHT_TOP_RECT,MMIAPIMENU_EnterLebao} //�ֱ�����
#endif
#define MEUN_CALLLOG {1,RIGHT_TOP_RECT,WatchCL_ListWin_Enter} //ͨ����¼
#define MEUN_FIND_FRIEND {1,RIGHT_TOP_RECT,MMIZDT_OpenFriendPPWin/*MMIZDT_OpenFindFriendWin*/} //�Ӻ���



#ifdef WIN32
extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MEUN_VIDEO_INCOMING_TEST {0,RIGHT_BOMTTOM_RECT,Video_Call_Incoming_Test} //��Ƶͨ������
#endif
#define MEUN_SCHEDULE {0,LEFT_TOP_RECT,MMIZDT_OpenScheduleWin} //�γ̱�


#if defined(ZDT_TOOLS_MENU_SUPPORT)// wuxx add.
#define MEUN_WATCH_TOOLS {0,LEFT_BOTTOM_RECT,WatchTools_MainWin_Enter} //������
#endif


#if defined(HERO_ENGINE_SUPPORT)
#if defined(HERO_APP_WSTORE_OPEN)
#define MEUN_HERO_APPSTORE {0,LEFT_TOP_RECT,launcher_hero_appstore} //Ӧ���̳�,Ӧ���г�
#define MEUN_HERO_APPSTORE_MANAGER {0,RIGHT_TOP_RECT,launcher_hero_appstore_manager} //Ӧ�ù���,Ӧ������
#endif
#endif

#ifdef ZDT_ZFB_SUPPORT
#define MEUN_ALIPAY {1,LEFT_BOTTOM_RECT,MMIZFB_OpenMainWin} //֧����
#endif

extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MEUN_VIDEO_INCOMING_TEST {0,RIGHT_BOMTTOM_RECT,Video_Call_Incoming_Test} //��Ƶͨ������


//****�Ĺ���˵�**************//
//1-�󶨣�2-���ţ�3-��������4-�Ʋ���5-���ѣ�6-���7-���ӣ�8-���ã�9-��Ƶͨ����10-������11-ͨ����¼��12-ͨѶ¼��13-΢�ģ�14-ϲ�����ţ�15-��ᣬ16-�����17-С�ȣ�18-֧����
//****** �б�˵�***************//
#ifdef LAUNCHER_ALL_APP_IN_PAGE
    #ifdef APP_ITEM_NAME//�Ƿ���ʾ�˵�����
		#define MEUN_PHONEBOOK {12, 1, TXT_CONTACT, res_app_ic_contact,  MMIZDT_OpenPBWin}
		#define MEUN_CAMERA {16, 0, TXT_LAUNCHER_CAMERA_TITLE, res_app_ic_camera,  MMIAPIDC_OpenPhotoWin} //���
        #ifdef ZDT_PLAT_YX_SUPPORT_VOICE
            #ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
		    #define MEUN_WECHART {13, 1, TXT_WECHART, res_app_ic_wechat,  MMIZDT_OpenChatGroupWin}//΢��
            #else       //ZDT_PLAT_YX_SUPPORT_FRIEND
            #define MEUN_WECHART {13, 1, TXT_WECHART, res_app_ic_wechat,  MMIAPIMENU_EnterTinyChat}//΢��
            #endif      // end ZDT_PLAT_YX_SUPPORT_FRIEND
        #endif // end ZDT_PLAT_YX_SUPPORT_VOICE
		#define MEUN_GALLERY {15, 0, TXT_GALLERY, res_app_ic_gallery, WatchGallery_MainWin_Enter} //���

		#define MEUN_SETTINGS {8, 0, TXT_LAUNCHER_SETTINGS_TITLE, res_app_ic_settings,WatchSET_MainWin_Enter}
		#define MEUN_VIDEO_CALL {9, 1, TXT_VIDEO_CALL, res_app_ic_video_call, MMIVideo_Call_MainWin} //��Ƶͨ��
		//#ifdef LOCAL_ALARM_CLOCK_SUPPORT
		//#define MEUN_ALARM  {0, TXT_ALARM_CLOCK, res_app_ic_alarm, MMIALARMCLOCK_CreateMainWin}
		//#else
		#define MEUN_ALARM  {7,0, TXT_ALARM_CLOCK, res_app_ic_alarm, WatchAlarm_MainWin_Enter}
		//#endif
		#define MEUN_CALCULATOR {3, 0, TXT_CALCULATOR, res_app_ic_calculator, MMIAPICALC_OpenMainWin} //������
		#ifdef ZTE_WATCH
		#define MEUN_PEDOMETER {4, 0, TXT_PEDOMETE, res_app_ic_pedometer, MMIPEDOMETER_CreateTodayWin} //�Ʋ�
		#define MEUN_WEATHER  {10, 1, TXT_WEATHER, res_app_ic_weather,ZTEWEATHER_CreateMainWin} //����
		#define MEUN_STOPWATCH  {6, 0, TXT_LAUNCHER_STOPWATCH_TITLE, res_app_ic_stopwatch, ZTE_STOPWATCH_OpenMainWin}
		#define MEUN_QRCODE {1, 0, TXT_QRCODE, res_app_ic_qrcode,MMIZDT_Open2VMWin}
		#else
		#define MEUN_PEDOMETER {4, 0, TXT_PEDOMETE, res_app_ic_pedometer, MMIZDT_OpenPedometerWin} //�Ʋ�
		#define MEUN_WEATHER  {10, 1, TXT_WEATHER, res_app_ic_weather,MMIZDT_OpenWeatherWin} //����
		#define MEUN_STOPWATCH  {6, 0, TXT_LAUNCHER_STOPWATCH_TITLE, res_app_ic_stopwatch, WatchStopWatch_MainWin_Enter} 
		#define MEUN_QRCODE {1, 0, TXT_QRCODE, res_app_ic_qrcode,MMIZDT_Open2VMSelectWin}
		#endif
		#define MEUN_CALLLOG {11, 0, TXT_CALL_LOG, res_app_ic_calllog, WatchCL_ListWin_Enter} //ͨ����¼ 
		#define MEUN_FIND_FRIEND  {5, 1, TXT_FIND_FRIEND, res_app_ic_find_friend, MMIZDT_OpenFriendPPWin} //�Ӻ���
		#define MEUN_DIAL   {2, 0, TXT_DIALER, res_app_ic_dailer, MMIZDT_OpenDialWin}//������
        #ifdef ZDT_ZFB_SUPPORT
        #define MEUN_ALIPAY  {18, 1, TXT_ALIPAY, res_app_ic_alipay, MMIZFB_OpenMainWin} //֧����
        #endif
		#ifdef TULING_AI_SUPPORT
        #define MEUN_AI_CHAT   {0, 1, TXT_AI_CHAT, res_app_ic_ai_chat, MMIAPIMENU_EnterAiChat}//ͼ��AI
        #endif
        #ifdef BAIDU_AI_SUPPORT
        #define MEUN_XIAODU   {17, 1, TXT_XIAODU, res_app_ic_xiaodu, MMIAPIMENU_EnterAiChat}//С��
        #endif
        #if defined(XYSDK_SUPPORT)|| defined(XYSDK_SRC_SUPPORT)
        #define MEUN_XMLY {14, 1,TXT_XMLY, res_xmla_icon,LIBXMLYAPI_CreateXysdkMainWin}//ϲ������
        #endif
        #ifdef LEBAO_MUSIC_SUPPORT
        #define MEUN_MUSIC {0, 1,TXT_MIGU_MUSIC,res_music_icon,MMIAPIMENU_EnterLebao}//�乾����
        #endif
		#ifdef WIN32
		extern PUBLIC uint8 Video_Call_Incoming_Test();
		#define MEUN_VIDEO_INCOMING_TEST {9, 0,TXT_VIDEO_CALL,res_app_ic_video_call,Video_Call_Incoming_Test} //��Ƶͨ������
#endif
    #else
        #define MEUN_PHONEBOOK {1, res_app_ic_contact,  MMIZDT_OpenPBWin}
        #define MEUN_CAMERA {0, res_app_ic_camera,  MMIAPIDC_OpenPhotoWin} //���
        #ifdef ZDT_PLAT_YX_SUPPORT_VOICE
        #ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
            #define MEUN_WECHART {1, res_app_ic_wechat,  MMIZDT_OpenChatGroupWin}//΢��
        #else       //ZDT_PLAT_YX_SUPPORT_FRIEND
            #define MEUN_WECHART {1, res_app_ic_wechat,  MMIAPIMENU_EnterTinyChat}//΢��
        #endif      //ZDT_PLAT_YX_SUPPORT_FRIEND
        #endif
        #define MEUN_GALLERY {0, res_app_ic_gallery, WatchGallery_MainWin_Enter} //���
        #define MEUN_QRCODE {0, res_app_ic_qrcode,MMIZDT_Open2VMSelectWin}
        #define MEUN_ALARM  {0, res_app_ic_alarm, WatchAlarm_MainWin_Enter}
        #define MEUN_SETTINGS {0, res_app_ic_settings,WatchSET_MainWin_Enter}
        #ifdef ZTE_STOPWATCH_SUPPORT
        #define MEUN_STOPWATCH  {0, res_app_ic_stopwatch, ZTE_STOPWATCH_OpenMainWin}
        #else
        #define MEUN_STOPWATCH  {0, res_app_ic_stopwatch, WatchStopWatch_MainWin_Enter}
        #endif
          
        #define MEUN_VIDEO_CALL {1, res_app_ic_video_call, MMIVideo_Call_MainWin} //��Ƶͨ��
        #define MEUN_PEDOMETER {0, res_app_ic_pedometer, MMIZDT_OpenPedometerWin} //�Ʋ�
        #define MEUN_CALCULATOR {0, res_app_ic_calculator, MMIAPICALC_OpenMainWin} //������
        #define MEUN_WEATHER  {1, res_app_ic_weather,MMIZDT_OpenWeatherWin} //����
        #define MEUN_CALLLOG {0, res_app_ic_calllog, WatchCL_ListWin_Enter} //ͨ����¼ 
        #define MEUN_FIND_FRIEND  {1, res_app_ic_find_friend, MMIZDT_OpenFriendPPWin} //�Ӻ���
        #define MEUN_DIAL   {0, res_app_ic_dailer, MMIZDT_OpenDialWin}//������
        #ifdef ZDT_ZFB_SUPPORT
        #define MEUN_ALIPAY  {1, res_app_ic_alipay, MMIZFB_OpenMainWin} //֧����
        #endif
        #ifdef TULING_AI_SUPPORT
        #define MEUN_AI_CHAT {1, res_app_ic_ai_chat, MMIAPIMENU_EnterAiChat}//ͼ��AI
        #endif
        #ifdef LEBAO_MUSIC_SUPPORT
        #define MEUN_MUSIC {1,res_music_icon,MMIAPIMENU_EnterLebao}//�乾����
        #endif
        #if defined(XYSDK_SUPPORT)|| defined(XYSDK_SRC_SUPPORT)
        #define MEUN_XMLY {1,res_xmla_icon,LIBXMLYAPI_CreateXysdkMainWin}//ϲ������
        #endif
        #ifdef MAINMENU_STYLE_SUPPORT
        #define MEUN_STYLE  {0, res_menu_style_icon, WatchSET_MenuStyle_Enter}//���˵�����л�
        #endif
        #define MEUN_PANEL_SWITCH {0, res_panel_switch_icon, WatchOpen_Panel_SelectWin}//�����л�
        #define MEUN_BRIGHTNESS {0, res_brightness_icon,MMIAPISET_ZdtBrightnessWin} //���ȵ���
        #define MEUN_DEVICE_ABOUT  {0, res_device_about_icon, Settings_RegardWin_Enter}//�����豸  
        #define MEUN_FACTORY_RESET {0, res_factory_reset_icon, MMIAPISET_ZdtPowerRestoreWin} //�ָ�����
		#if defined(FOTA_SUPPORT)
        #define MEUN_FOTA {0, res_fota_icon, MMIAPISET_EnterFotaWin} //fota����
		#endif
        #define MEUN_POWER_OFF {0, res_power_off_icon, MMIAPISET_ZdtPowerOffWin} //�ػ�
        #define MEUN_POWER_RESTART  {0, res_power_restart_icon,MMIAPISET_ZdtPowerRestartWin} //����
        #define MEUN_RING {0, res_ring_icon, Settings_CallRingSelectWin_Enter} //����
        #define MEUN_VOLUME {0, res_volume_icon, MMIAPISET_ZdtVolumeWin} //����
        #define MEUN_SCREEN_TIMEOUT {0, res_screen_timeout_icon,Settings_BackLight_Enter} //Ϣ��ʱ��
        #ifdef WIN32
        extern PUBLIC uint8 Video_Call_Incoming_Test();
        #define MEUN_VIDEO_INCOMING_TEST {0,res_app_ic_video_call,Video_Call_Incoming_Test} //��Ƶͨ������
        #endif
    #endif
#endif
//*******�б�˵�***************//

#ifdef __cplusplus
    }
#endif

#endif

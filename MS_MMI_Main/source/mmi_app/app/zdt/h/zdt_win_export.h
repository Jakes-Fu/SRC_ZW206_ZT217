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
#define MENU_PHONEBOOK {1,LEFT_TOP_RECT,MMIZDT_OpenPBWin} //ͨѶ¼
#define MENU_DIAL {0,RIGHT_TOP_RECT,MMIZDT_OpenDialWin/*WatchCL_ListWin_Enter*//*WatchCC_DialpadWin_Enter*/} //����
#define MENU_AICHART {1,LEFT_BOTTOM_RECT,MMIAPIMENU_EnterAiChat} //������
#define MENU_VIDEO_CALL {1,RIGHT_BOMTTOM_RECT,MMIVideo_Call_MainWin} //��Ƶͨ��

#define MENU_WECHART {1,LEFT_TOP_RECT,MMIAPIMENU_EnterTinyChat} //΢��
#define MENU_CAMERA {0,RIGHT_TOP_RECT,MMIAPIDC_OpenPhotoWin} //���
#define MENU_GALLERY {0,LEFT_BOTTOM_RECT,WatchGallery_MainWin_Enter} //���
#define MENU_BIND_DEVICE {0,RIGHT_BOMTTOM_RECT,MMIZDT_Open2VMSelectWin} //������

#define MENU_SETTINGS {0,LEFT_TOP_RECT,WatchSET_MainWin_Enter} //����
#define MENU_STOPWATCH {0,RIGHT_TOP_RECT,WatchStopWatch_MainWin_Enter} //���
#define MENU_CALCULATOR {0,LEFT_BOTTOM_RECT,MMIAPICALC_OpenMainWin} //������
#define MENU_ALARM {0,RIGHT_BOMTTOM_RECT,WatchAlarm_MainWin_Enter} //����

#define MENU_WEATHER {0,LEFT_TOP_RECT,MMIZDT_OpenWeatherWin} //����
#define MENU_PEDOMETER {0,RIGHT_BOMTTOM_RECT,MMIZDT_OpenPedometerWin} //�Ʋ�
#define MENU_MANUAL {0,LEFT_BOTTOM_RECT,MMIZDT_OpenManualWin} //˵����
#ifdef LEBAO_MUSIC_SUPPORT
#define MENU_MUISE {0,RIGHT_TOP_RECT,MMIAPIMENU_EnterLebao} //�ֱ�����
#endif
#define MENU_CALLLOG {1,RIGHT_TOP_RECT,WatchCL_ListWin_Enter} //ͨ����¼
#define MENU_FIND_FRIEND {1,RIGHT_TOP_RECT,MMIZDT_OpenFriendPPWin/*MMIZDT_OpenFindFriendWin*/} //�Ӻ���



#ifdef WIN32
extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MENU_VIDEO_INCOMING_TEST {0,RIGHT_BOMTTOM_RECT,Video_Call_Incoming_Test} //��Ƶͨ������
#endif
#define MENU_SCHEDULE {0,LEFT_TOP_RECT,MMIZDT_OpenScheduleWin} //�γ̱�


#if defined(ZDT_TOOLS_MENU_SUPPORT)// wuxx add.
#define MENU_WATCH_TOOLS {0,LEFT_BOTTOM_RECT,WatchTools_MainWin_Enter} //������
#endif


#if defined(HERO_ENGINE_SUPPORT)
#if defined(HERO_APP_WSTORE_OPEN)
#define MENU_HERO_APPSTORE {0,LEFT_TOP_RECT,launcher_hero_appstore} //Ӧ���̳�,Ӧ���г�
#define MENU_HERO_APPSTORE_MANAGER {0,RIGHT_TOP_RECT,launcher_hero_appstore_manager} //Ӧ�ù���,Ӧ������
#endif
#endif

#ifdef ZDT_ZFB_SUPPORT
#define MENU_ALIPAY {1,LEFT_BOTTOM_RECT,MMIZFB_OpenMainWin} //֧����
#endif

extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MENU_VIDEO_INCOMING_TEST {0,RIGHT_BOMTTOM_RECT,Video_Call_Incoming_Test} //��Ƶͨ������


//****�˵�**************//
//1-�󶨣�2-���ţ�3-��������4-�Ʋ���5-���ѣ�6-���7-���ӣ�8-���ã�9-��Ƶͨ����10-������11-ͨ����¼��12-ͨѶ¼��13-΢�ģ�14-ϲ�����ţ�15-��ᣬ16-�����17-С�ȣ�18-֧����
//****** �б�˵�***************//
#define MENU_TEXTBOOK {12, 1, textbook_text, res_app_ic_contact, textbook_bg, MMIZDT_OpenPBWin}
#define MENU_PHONEBOOK {12, 1, contact_text, res_app_ic_contact, bright_green_bg, MMIZDT_OpenPBWin}
#define MENU_CAMERA {16, 0, camera_text, res_app_ic_camera, light_cyan_bg, MMIAPIDC_OpenPhotoWin} //���
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    #ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
	#define MENU_WECHART {13, 1, wechat_text, res_app_ic_wechat, bright_yellow_bg, MMIZDT_OpenChatGroupWin}//΢��
    #else       //ZDT_PLAT_YX_SUPPORT_FRIEND
    #define MENU_WECHART {13, 1, wechat_text, res_app_ic_wechat, bright_yellow_bg, MMIAPIMENU_EnterTinyChat}//΢��
    #endif      // end ZDT_PLAT_YX_SUPPORT_FRIEND
#endif // end ZDT_PLAT_YX_SUPPORT_VOICE
#define MENU_GALLERY {15, 0, gallery_text, res_app_ic_gallery,bright_purple_bg, WatchGallery_MainWin_Enter} //���

#define MENU_SETTINGS {8, 0, settings_text, res_app_ic_settings, light_gray_bg, WatchSET_MainWin_Enter}
#define MENU_VIDEO_CALL {9, 1, video_call_text, res_app_ic_video_call, dark_red_bg, MMIVideo_Call_MainWin} //��Ƶͨ��
//#define MENU_WEATHER  {10, 1, weather_text, res_app_ic_weather,dark_red_bg, ZTEWEATHER_CreateMainWin} //����
#define MENU_WEATHER  {10, 1, weather_text, res_app_ic_weather,light_cyan_bg, MMIZDT_OpenWeatherWin}
#define MENU_CALLLOG {11, 0, calllog_text, res_app_ic_calllog, light_pink_bg, WatchCL_ListWin_Enter} //ͨ����¼ 
#define MENU_FIND_FRIEND  {5, 1, find_friend_text, res_app_ic_find_friend, bright_green_bg, MMIZDT_OpenFriendPPWin} //�Ӻ���
#define MENU_DIAL   {2, 0, dialer_text, res_app_ic_dailer, dark_blue_bg, MMIZDT_OpenDialWin}//������
#ifdef ZDT_ZFB_SUPPORT
#define MENU_ALIPAY  {18, 1, alipay_text, res_app_ic_alipay, light_blue_bg, MMIZFB_OpenMainWin} //֧����
#endif
#ifdef TULING_AI_SUPPORT
#define MENU_AI_CHAT   {0, 1, ai_assistant_text, res_app_ic_ai_chat, bright_yellow_bg, MMIAPIMENU_EnterAiChat}//ͼ��AI
#endif
#ifdef BAIDU_AI_SUPPORT
#define MENU_XIAODU   {17, 1, xiaodu_text, res_app_ic_xiaodu, bright_yellow_bg, MMIAPIMENU_EnterAiChat}//С��
#endif
#if defined(XYSDK_SUPPORT)|| defined(XYSDK_SRC_SUPPORT)
#define MENU_XMLY {14, 1,xmla_text, res_app_ic_ximalaya, dark_orange_bg, LIBXMLYAPI_CreateXysdkMainWin}//ϲ������
#endif
#ifdef LEBAO_MUSIC_SUPPORT
#define MENU_MUSIC {0, 1,migu_music_text,res_app_ic_music, dark_red_bg, MMIAPIMENU_EnterLebao}//�乾����
#endif
#ifdef WIN32
extern PUBLIC uint8 Video_Call_Incoming_Test();
#define MENU_VIDEO_INCOMING_TEST {9, 0,video_call_text,res_app_ic_video_call, dark_red_bg, Video_Call_Incoming_Test} //��Ƶͨ������
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
//*******�˵�***************//

#ifdef __cplusplus
    }
#endif

#endif

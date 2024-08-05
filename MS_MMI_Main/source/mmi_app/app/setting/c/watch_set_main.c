/*****************************************************************************
** File Name:      watch_set_main.c                                          *
** Author:         bin.wang1                                                 *
** Date:           02/17/2020                                                *
** Copyright:      All Rights Reserved.                                      *
** Description:                                                              *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE                 NAME                  DESCRIPTION                    *
** 02/17/2020           bin.wang1             Create                         *
******************************************************************************/

/**--------------------------------------------------------------------------*
**                         Include Files                                     *
**---------------------------------------------------------------------------*/

#include "guilistbox.h"
#include "watch_common_list.h"
#include "os_api.h"
#include "sci_types.h"
#include "window_parse.h"

#include "mmiset_image.h"
#include "mmiset_text.h"
#include "mmiset_id.h"
#include "watch_set_sound.h"
#include "watch_set_brightness.h"
#include "watch_set_alarmlist.h"
#include "watch_set_more.h"
#include "watch_set_clock.h"
#include "watch_set_about.h"
#include "watch_set_about.h"
#include "watch_commonwin_export.h"
//#ifdef BLUETOOTH_SUPPORT
#include "mmibt_export.h"
#include "watch_set_backlight.h"
#include "mmicc_export.h" //for dialpad
#include "watch_launcher_editclockwin.h"
#ifdef DYNAMIC_WATCHFACE_SUPPORT
#include "watchface_edit_win.h"
#include "watchface_set_flow.h"
#endif
//#endif
#ifdef ADULT_WATCH_SUPPORT
#include "watch_set_display.h"
#include "watch_set_wear.h"
#include "watch_set_system.h"
#include "mmiwlan_text.h"
#include "mmiwifi_export.h"
#include "adultwatch_bt_win.h"
#include "watch_set_bt_ble_manage.h"
#include "adultwatch_bt_view.h"
#endif
#ifdef SCREEN_SHAPE_CIRCULAR
#include "watch_alarm_main.h"
#endif
#include "watch_common.h"
#include "mmiset_menutable.h"
#include "mmidisplay_data.h"
#include "watch_launcher_main.h"
#include "mmiset_nv.h"


#include "guitext.h"
#include "watch_charge_win.h"


#ifndef TIP_NULL
#define TIP_NULL 0
#endif

//#define ZTE_WATCH 1

static uint8 LongRangeMode_Switch=0;

extern PUBLIC void MMIAPISET_ZdtVolumeWin(void);
extern PUBLIC void MMIAPISET_ZdtWatchInfoWin(void);
extern PUBLIC void MMIAPISET_ZdtPowerOffWin(void);
extern PUBLIC void MMIAPISET_ZdtPowerRestartWin(void);
extern PUBLIC void MMIAPISET_ZdtVolumeWin(void);
extern PUBLIC void MMIAPISET_ZdtBrightnessWin(void);
extern PUBLIC void MMIAPISET_ZdtPowerRestoreWin(void);
extern PUBLIC void Settings_CallRingSelectWin_Enter( void );
extern PUBLIC void MMIAPISET_CallVolumeWin(void);
extern PUBLIC void MMIAPISET_RingToneVolumeWin(void);
extern PUBLIC void Settings_NoteTypeSelectWin_Enter( void );
#ifdef ZTE_WATCH
PUBLIC void Watch_LongRangeMode_switch( void );
PUBLIC void WatchSET_LongRangeModeTipsWin(MMI_WIN_ID_T parent_winid);
PUBLIC void WatchSET_DisPlaySetting_Enter( void );
PUBLIC void Watch_Time_Format_Switch( void );
#endif
#ifdef MAINMENU_STYLE_SUPPORT
 PUBLIC void WatchSET_MenuStyle_Enter( void );
#endif
#ifdef FOTA_SUPPORT
PUBLIC void MMIAPISET_EnterFotaWin( void );
#endif
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#ifdef ADULT_WATCH_SUPPORT
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_wlan         = { TXT_SET_LIST_WLAN,                   IMAGE_SET_MAIN_AW_WLAN };
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_bluetooth    = { TXT_BLUETOOTH,                       IMAGE_SET_MAIN_AW_BLUETOOTH };
#ifdef DYNAMIC_WATCHFACE_SUPPORT
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_watch        = { TXT_SET_LIST_WATCH_EDIT,             IMAGE_SET_MAIN_AW_WATCH };
#else
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_watch        = { TXT_SET_LIST_WATCH,                  IMAGE_SET_MAIN_AW_WATCH };
#endif
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_display      = { TXT_SET_LIST_DISPLAY_BRIGHTNESS,     IMAGE_SET_MAIN_AW_BRIGHTNESS };
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_sound        = { TXT_SET_LIST_SOUND,                  IMAGE_SET_MAIN_AW_SOUND };
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_wear         = { TXT_SET_LIST_WEAR,                   IMAGE_SET_MAIN_AW_WEAR };
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_system       = { TXT_SET_LIST_SYSTEM,                 IMAGE_SET_MAIN_AW_SYSTEM };
//LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_bbm          = { TXT_SET_BT_ROLE_MANAGEMENT,          IMAGE_SET_MAIN_AW_BLUETOOTH };
LOCAL ADULTWATCHCOM_LIST_ITEM_STYLE_1STR_1ICON_ST   list_item_bt1          = { TXT_BLUETOOTH,                       IMAGE_SET_MAIN_AW_BLUETOOTH };

LOCAL WATCHCOM_LIST_ITEM__ST s_settings_main_text_list_data[] =
{
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_wlan,       WatchWIFI_MainWin_Enter },//todo
#ifndef BBM_ROLE_SUPPORT
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_bluetooth,  AdultBT_MainWin_Enter },
#else
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_bt1,        AdultWatchBT_MainWin_Enter },
#endif
#ifdef DYNAMIC_WATCHFACE_SUPPORT
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_watch,      WATCHFACE_Edit_Slidepage_Enter },
#else
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_watch,      WatchLAUNCHER_EditClockWin_Enter },
#endif
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_display,    Settings_DisplayWin_Enter },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_sound,      MMIAPISET_ZdtVolumeWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_wear,       Settings_WearWin_Enter },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_system,     Settings_SystemWin_Enter },
    //{ WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_ADULTWATCH, &list_item_bbm,        AdultSet_BBM_Enter },
};
#else

#ifdef  ZTE_WATCH
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_displaysetting    = { IMAGE_DISPLAYSET_ICON,     TXT_DISPLAY_SETTING_ZX };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_more         = { IMAGE_ABOUT_WATCH_ICON,          TEXT_WATCH_INFO };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_notetype    = { IMAGE_NOTETYPE_SET_ICON,     TXT_SET_SOUND_LIST_NOTE_MODE };
//LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_LongRangeMode   = { IMAGE_LONGTIME_SET_ICON,    TXT_LONG_RANGE_MODE_ZX };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_restore      = { IMAGE_RECOVER_SET_ICON,          qtn_sett_list_reset_settings_bc};
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_poweroff     = { IMAGE_SHUTDOWN_SET_ICON,          text_setting_poweroff};
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_restart     = { IMAGE_REBOOT_SET_ICON,          text_setting_powerrestart};
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_sound        = { IMAGE_SOUND_SET_ICON,         TXT_SOUND_CONTROL_ZX };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_ST list_item_backlight         = { TXT_AUTO_SCREEN_REST_ZX };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_ST list_item_brightness        = { TXT_SET_DISPLAY_BRIGHTNESS };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_watch     = {IMAGE_PANEL_SWITCH_SET_ICON, TXT_SETTINGS_WATCH_SWITCH };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_timeformat     = {IMAGE_TIME_FORMAT_ICON, TXT_TIME_FORMAT };

#ifdef FOTA_SUPPORT
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_fota          = { IMAGE_ABUP_FOTA_SET_ICON,TXT_SETTINGS_ABUP_FOTA_CHECK  };
#endif
#else
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_callring    = { IMAGE_RINGTONE_SET_ICON,     TXT_RING };
#ifdef SCREEN_SHAPE_CIRCULAR
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_backlight    = { IMAGE_DISPLAYTIME_SET_ICON,     TXT_SET_BRIGHT_SCREEN_TIME };
#else
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_backlight    = { IMAGE_DISPLAYTIME_SET_ICON,     TXT_SET_BACK_LIGHT };
#endif
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_sound        = { IMAGE_VOLUME_SET_ICON,         text_volume_settings };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_brightness   = { IMAGE_BRIGHTNESS_SET_ICON,    TXT_COMMON_COMMON_BRIGHTNESS };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_more         = { IMAGE_ABOUT_PHONE_ICON,          TEXT_WATCH_INFO };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_restore      = { IMAGE_RESTORE_ICON,          qtn_sett_list_reset_settings_bc};
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_poweroff     = { IMAGE_POWER_OFF_ICON,          text_setting_poweroff};
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_restart     = { IMAGE_RESTART_ICON,          text_setting_powerrestart};

LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_watch        = {IMAGE_WATCH_SWITCH_SET_ICON, TXT_SET_LIST_WATCH };
#ifdef MAINMENU_STYLE_SUPPORT
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_menustyle        = {IMAGE_SCENE_MODE_SET_ICON, TXT_MAINMENU_STYLE_SET };
#endif
#ifdef FOTA_SUPPORT
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_fota          = { IMAGE_ABUP_FOTA_SET_ICON,TXT_SETTINGS_ABUP_FOTA_CHECK  };
#endif

#endif


//LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_alarm        = { IMAGE_SET_MAIN_ALARM,         TXT_ALARM_CLOCK };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_alarm        = { IMAGE_SET_MAIN_ALARM,         TXT_ALARM_CLOCK };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_bluetooth    = { IMAGE_SET_MAIN_BLUETOOTH,     TXT_BLUETOOTH };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_dial         = { IMAGE_SET_MAIN_DIALPAD,       TXT_SET_LIST_DIAL };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_about        = { IMAGE_SET_MAIN_REGARDING,     TXT_SET_LIST_ABOUT };


#ifdef  ZTE_WATCH
LOCAL WATCHCOM_LIST_ITEM__ST s_settings_main_text_list_data[] =
{    
#ifdef  ZTE_SUPPORT_240X284
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_displaysetting, WatchSET_DisPlaySetting_Enter},
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_notetype,  Settings_NoteTypeSelectWin_Enter},
    //{ WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_LongRangeMode, Watch_LongRangeMode_switch},    
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_restore,       MMIAPISET_ZdtPowerRestoreWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_poweroff,       MMIAPISET_ZdtPowerOffWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_restart,       MMIAPISET_ZdtPowerRestartWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_sound,      MMIAPISET_ZdtVolumeWin},
#ifndef ZT217_LISENNING_PAD
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_watch,       WatchOpen_Panel_SelectWin },
#endif
#ifdef FOTA_SUPPORT
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_fota,      MMIAPISET_EnterFotaWin},
#endif
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_timeformat,      Watch_Time_Format_Switch},
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR_1LINE_BIG_LAYOUT1, &list_item_more,       Settings_RegardWin_Enter},
#else
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_displaysetting, WatchSET_DisPlaySetting_Enter},
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_notetype,  Settings_NoteTypeSelectWin_Enter},
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_LongRangeMode, Watch_LongRangeMode_switch},    
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_restore,       MMIAPISET_ZdtPowerRestoreWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_poweroff,       MMIAPISET_ZdtPowerOffWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_restart,       MMIAPISET_ZdtPowerRestartWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_sound,      MMIAPISET_ZdtVolumeWin},
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_watch,       WatchOpen_Panel_SelectWin },
#ifdef FOTA_SUPPORT
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_fota,      MMIAPISET_EnterFotaWin},
#endif
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_timeformat,      Watch_Time_Format_Switch},
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_more,       Settings_RegardWin_Enter},
#endif    
};

//LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_callvolume    = { IMAGE_CALL_VOLUME_ICON,     text_call_volume_setting };
//LOCAL WATCHCOM_LIST_ITEM_STYLE_1ICON_1STR_ST   list_item_ringtonevolume    = { IMAGE_RINGTONE_SET_ICON,     text_ringtone_volume_setting };

LOCAL WATCHCOM_LIST_ITEM__ST s_displaysetting_text_list_data[] =
{
/*#ifdef  ZTE_SUPPORT_240X284
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_1LINE_BIG_LAYOUT1,   &list_item_backlight,    Settings_BackLight_Enter },
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_1LINE_BIG_LAYOUT1,   &list_item_brightness,   MMIAPISET_ZdtBrightnessWin },
#else*/
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR,   &list_item_backlight,    Settings_BackLight_Enter },
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR,   &list_item_brightness,   MMIAPISET_ZdtBrightnessWin },
//#endif
};

LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_RADIO_ST s_time_format_12Hour  = { TXT_COMM_12_HOUR };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_RADIO_ST s_time_format_24Hour  = { TXT_COMM_24_HOUR };
LOCAL WATCHCOM_LIST_ITEM__ST s_time_format_list[] =
{
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_RADIO,   &s_time_format_12Hour, PNULL },
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_RADIO,   &s_time_format_24Hour, PNULL },
};



#else
LOCAL WATCHCOM_LIST_ITEM__ST s_settings_main_text_list_data[] =
{
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_callring, Settings_CallRingSelectWin_Enter/*Settings_SoundWin_Enter*/ },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_backlight,  Settings_BackLight_Enter},
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_brightness, MMIAPISET_ZdtBrightnessWin/*Settings_BrightnessWin_Enter*/ },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_sound,      MMIAPISET_ZdtVolumeWin/*Settings_SoundWin_Enter*/},
  
#ifdef MAINMENU_STYLE_SUPPORT
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_menustyle,       WatchSET_MenuStyle_Enter },
#endif
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_watch,       WatchOpen_Panel_SelectWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_restore,       MMIAPISET_ZdtPowerRestoreWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_poweroff,       MMIAPISET_ZdtPowerOffWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_restart,       MMIAPISET_ZdtPowerRestartWin },
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_more,       Settings_RegardWin_Enter/*MMIAPISET_ZdtWatchInfoWin*/ },
      
    
#ifdef BLUETOOTH_SUPPORT
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_bluetooth,  MMIAPIBT_MainWin_Enter },
#endif

#ifdef FOTA_SUPPORT
    { WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_fota,     MMIAPISET_EnterFotaWin },
#endif

    //{ WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_dial,       WatchCC_DialpadWin_Enter },
    //{ WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_about,      Settings_RegardWin_Enter },
    //{ WatchCOM_List_Item_Visible_Default, GUIITEM_STYLE_1ICON_1STR, &list_item_more,       Settings_MoreWin_Enter },

};



#endif
#ifdef MAINMENU_STYLE_SUPPORT
LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_RADIO_ST s_mainmenu_matrix_style  = { TXT_SET_MAINMENU_MATRIX };
LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_RADIO_ST s_mainmenu_rotate_style  = { TXT_SET_MAINMENU_ROTATE };
LOCAL WATCHCOM_LIST_ITEM__ST s_mainmenu_style_list[] =
{
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_RADIO,   &s_mainmenu_matrix_style,    PNULL },
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_RADIO,   &s_mainmenu_rotate_style, PNULL },
};
#endif


#endif
/**---------------------------------------------------------------------------------------------*
 **                         LOCAL FUNCTION DEFINITION                                           *
 **---------------------------------------------------------------------------------------------*/

LOCAL void Settings_Main_APP_OK( void )
{
    WatchCOM_List_Item_CallBack( MMISET_MAIN_LIST_CTRL_ID );
    #if 0 
      MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    GUIMENU_GetId(MMISET_MAIN_LIST_CTRL_ID,&group_id,&menu_id);
        switch (menu_id)
        {
        case ID_SETTINGS_CALLRING_SELECT:
            Settings_ChargeRingSelectWin_Enter();
            break;
        case ID_SETTINGS_BACKLIGHT_TIMEOUT:
            Settings_BackLight_Enter();
            break;
        case ID_SETTINGS_BRIGHTNESS:
            MMIAPISET_ZdtBrightnessWin();
            break;
        case ID_SETTINGS_VOLUME:
            MMIAPISET_ZdtVolumeWin();
            break;
        case ID_SETTINGS_RESTORE:
            MMIAPISET_ZdtPowerRestoreWin();
            break;
        case ID_SETTINGS_POWEROFF:
            MMIAPISET_ZdtPowerOffWin();
            break;
        case ID_SETTINGS_POWER_RESTART:
            MMIAPISET_ZdtPowerRestartWin();
            break;
        case ID_SETTINGS_WATCH_INFO:
            /*Settings_RegardWin_Enter();*/MMIAPISET_ZdtWatchInfoWin();
            break;
#ifdef FOTA_SUPPORT_ABUP
        case ID_SETTINGS_PHONE_ABUP_FOTA:
            break;
#endif
        default:
            break;
        }
    #endif
}

LOCAL void Settings_Main_APP_CANCEL( void )
{
    MMK_CloseWin( MMISET_MAIN_WIN_ID );
}

LOCAL void Settings_Main_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    uint32 listNum = 0;
#ifdef SCREEN_SHAPE_CIRCULAR
    GUI_RECT_T          list_rect = WATCHLIST_RECT;
#endif

    listNum = sizeof(s_settings_main_text_list_data)/sizeof(WATCHCOM_LIST_ITEM__ST);
#ifdef SCREEN_SHAPE_CIRCULAR
    GUILIST_SetRect(MMISET_MAIN_LIST_CTRL_ID,&list_rect);
#endif
    WatchCOM_TextList_Create( s_settings_main_text_list_data, listNum, MMISET_MAIN_LIST_CTRL_ID );
    MMK_SetAtvCtrl( MMISET_MAIN_WIN_ID, MMISET_MAIN_LIST_CTRL_ID );

#ifdef ZTE_WATCH
    WATCHCOM_DisplayBackground(win_id);
    WATCHCOM_DisplayTitleEx(win_id,STXT_MAIN_SETTING,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);
#else
    WATCHCOM_DisplayBackgroundEx(win_id,IMAGE_THEME_BLACK_BG_SPEC);
    WATCHCOM_DisplayTitleEx(win_id,STXT_MAIN_SETTING,MMI_SPECIAL_TITLE_HEIGHT,MMI_WHITE_COLOR,ALIGN_LVMIDDLE);
#endif    
}

/*****************************************************************************/
//  Description : to handle Settings window message
//  Global resource dependence :
//  Author:bin.wang1
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleSettingsMainWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            Settings_Main_OPEN_WINDOW(win_id);
            break;
        }
    //#ifdef SCREEN_SHAPE_CIRCULAR
    case MSG_FULL_PAINT:
        {
#ifdef ZTE_WATCH
            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,STXT_MAIN_SETTING,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);
#else
            WATCHCOM_DisplayBackgroundEx(win_id,IMAGE_THEME_BLACK_BG_SPEC);
            WATCHCOM_DisplayTitleEx(win_id,STXT_MAIN_SETTING,MMI_SPECIAL_TITLE_HEIGHT,MMI_WHITE_COLOR,ALIGN_LVMIDDLE);
#endif
            break;
        }    
//#endif
    case MSG_CTL_OK:
    case MSG_CTL_MIDSK:
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_CTL_PENOK:
#endif
        {
            Settings_Main_APP_OK();
            break;
        }
    case  MSG_TP_PRESS_UP:
        {
#ifndef ZTE_WATCH
            GUI_RECT_T   back_rect=WINDOW_BACK_RECT; 
            GUI_POINT_T   point = {0};
        
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if (GUI_PointIsInRect(point, back_rect))
                MMK_CloseWin( win_id);
#endif
            break;
        }
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            Settings_Main_APP_CANCEL();
            break;
        }
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}
#ifdef MAINMENU_STYLE_SUPPORT
PUBLIC void MMISET_SetWatchMenuStyle(uint8 style)
{
    uint8 temp_style = style;
    MMINV_WRITE(MMINV_SET_WATCH_MENU_STYLE, &temp_style);
}
PUBLIC uint8 MMISET_GetWatchMenuStyle(void)
{
    uint8 temp_style = 0;

    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;

    MMINV_READ(MMINV_SET_WATCH_MENU_STYLE, &temp_style, return_value);
    
    if (MN_RETURN_SUCCESS != return_value)
    {
        MMINV_WRITE(MMINV_SET_WATCH_MENU_STYLE, &temp_style);
    }
    return temp_style;
}

LOCAL MMI_RESULT_E HandleSelectSceneModeWindow( MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param )
{
    MMI_RESULT_E    recode =  MMI_RESULT_TRUE;
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            uint16 listNum=0;
            uint16 mainmenustyle_id =0;

            GUI_RECT_T list_rect = WATCHLIST_RECT;
        
            GUILIST_SetRect(MMISET_SCENE_MODE_SELECT_CTRL_ID,&list_rect);
            listNum = sizeof(s_mainmenu_style_list)/sizeof(WATCHCOM_LIST_ITEM__ST);
            WatchCOM_RadioList_Create(s_mainmenu_style_list,listNum,MMISET_SCENE_MODE_SELECT_CTRL_ID);
            mainmenustyle_id = MMISET_GetWatchMenuStyle( );
            //set selected item
            GUILIST_SetSelectedItem(MMISET_SCENE_MODE_SELECT_CTRL_ID, mainmenustyle_id, TRUE);

            //set current item
            GUILIST_SetCurItemIndex( MMISET_SCENE_MODE_SELECT_CTRL_ID, mainmenustyle_id );

            MMK_SetAtvCtrl( win_id, MMISET_SCENE_MODE_SELECT_CTRL_ID );
            break;
        }
    case MSG_NOTIFY_LIST_SET_SELECT:
        {
            uint16 curIdx = GUILIST_GetCurItemIndex( MMISET_SCENE_MODE_SELECT_CTRL_ID );
           

            /*if( SETTINGS_SOUND_NOTE_TYPE_RING == curIdx )
            {
            MMISET_SetWatchMenuStyle(curIdx)
            }*/
            MMISET_SetWatchMenuStyle(curIdx);
            MMK_CloseWin(win_id);
            break;
        }
    case  MSG_TP_PRESS_UP:
        {
            GUI_RECT_T   back_rect=WINDOW_BACK_RECT; 
            GUI_POINT_T   point = {0};
        
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if (GUI_PointIsInRect(point, back_rect))
            {
                MMK_CloseWin( win_id);
                return;
            }
            break;
        }
    case MSG_GET_FOCUS:// Bug 2113002
    case MSG_FULL_PAINT:
        {
            uint16 curSelection = 0;
            int32 listOffset = 0;
            uint16         notetype_id =0;

            WATCHCOM_DisplayBackgroundEx(win_id,IMAGE_THEME_BLACK_BG_SPEC);
            WATCHCOM_DisplayTitleEx(win_id,TXT_MAINMENU_STYLE_SET,MMI_SPECIAL_TITLE_HEIGHT,MMI_WHITE_COLOR,ALIGN_LVMIDDLE);

            GUILIST_GetTopItemOffset(MMISET_SCENE_MODE_SELECT_CTRL_ID, &listOffset);

            curSelection = GUILIST_GetCurItemIndex( MMISET_SCENE_MODE_SELECT_CTRL_ID );

            GUILIST_SetSelectedItem(MMISET_SCENE_MODE_SELECT_CTRL_ID, curSelection, TRUE);

            //set current item
            GUILIST_SetCurItemIndex(MMISET_SCENE_MODE_SELECT_CTRL_ID, curSelection);

            // set top item offset
            GUILIST_SetTopItemOffset(MMISET_SCENE_MODE_SELECT_CTRL_ID, listOffset);
            break;
        }
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_CTL_PENOK:
#endif
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:
        {
            /*if(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID ==((MMI_NOTIFY_T*)param)->src_id)
            {
                uint16 curIdx = GUILIST_GetCurItemIndex( MMISET_SCENE_MODE_SELECT_CTRL_ID );
                if( SETTINGS_SOUND_NOTE_TYPE_RING == curIdx )
                {
                MMIENVSET_SetActiveModeOptValue( CALL_RING_TYPE, MMISET_CALL_RING, dualSys );
                MMIENVSET_SetActiveModeOptValue( MSG_RING_TYPE, MMISET_MSG_RING, dualSys );
                MMIENVSET_SetActiveModeOptValue( ALARM_RING_TYPE, MMISET_MSG_RING, dualSys ); // alarm use msg type
                }
                else if( SETTINGS_SOUND_NOTE_TYPE_VIBRA == curIdx )
                {
                //MMISET_RING_TYPE_MAX not play ring
                MMIENVSET_SetActiveModeOptValue( CALL_RING_TYPE, MMISET_CALL_VIBRA, dualSys );
                MMIENVSET_SetActiveModeOptValue( MSG_RING_TYPE, MMISET_MSG_VIBRA, dualSys );
                MMIENVSET_SetActiveModeOptValue( ALARM_RING_TYPE, MMISET_MSG_VIBRA, dualSys );
                }
                MMK_CloseWin(win_id);
            }*/
            MMK_CloseWin(win_id);
            break;
        }
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            MMK_CloseWin( win_id );
            break;
        }
    case MSG_CLOSE_WINDOW:
        {
            break;
        }
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            MMK_CloseWin(win_id);
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}


#endif


#ifdef  ZTE_WATCH
LOCAL MMI_RESULT_E HandleSelectTimeFormatWindow( MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param )
{
    MMI_RESULT_E    recode =  MMI_RESULT_TRUE;
    MMI_CTRL_ID_T list_ctrl_id = MMISET_TIME_FORMAT_SELECT_CTRL_ID;
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            uint16 listNum=0;
            uint16 time_format =0;

            GUI_RECT_T list_rect = WATCHK1LIST_RECT;
            GUI_RECT_T savek_btn_rect =  WATCHK1_SETOK_RECT;
        
            GUILIST_SetRect(list_ctrl_id,&list_rect);
            listNum = sizeof(s_time_format_list)/sizeof(WATCHCOM_LIST_ITEM__ST);
            WatchCOM_RadioList_Create(s_time_format_list,listNum,list_ctrl_id);
            time_format = MMIAPISET_GetTimeDisplayType();
            //set selected item
            GUILIST_SetSelectedItem(list_ctrl_id, time_format, TRUE);

            //set current item
            GUILIST_SetCurItemIndex( list_ctrl_id, time_format );

            MMK_SetAtvCtrl( win_id, list_ctrl_id );

            GUIBUTTON_SetRect(MMISET_TIME_FORMAT_SAVE_BTN_CTRL_ID, &savek_btn_rect);
            break;
        }
    case  MSG_TP_PRESS_UP:
        break;
    case MSG_GET_FOCUS:// Bug 2113002
    case MSG_FULL_PAINT:
        {
            uint16 curSelection = 0;
            int32 listOffset = 0;

            WATCHCOM_DisplayBackground(win_id);
            //WATCHCOM_DisplayTitleEx(win_id,TXT_TIME_FORMAT,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);

            GUILIST_GetTopItemOffset(list_ctrl_id, &listOffset);

            curSelection = GUILIST_GetCurItemIndex( list_ctrl_id );

            GUILIST_SetSelectedItem(list_ctrl_id, curSelection, TRUE);

            //set current item
            GUILIST_SetCurItemIndex(list_ctrl_id, curSelection);

            // set top item offset
            GUILIST_SetTopItemOffset(list_ctrl_id, listOffset);
            break;
        }
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_CTL_PENOK:
#endif
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_APP_WEB:
    case MSG_CTL_MIDSK:{
		  MMI_NOTIFY_T *notify = (MMI_NOTIFY_T*)param;
			if(MMISET_TIME_FORMAT_SELECT_CTRL_ID == notify->src_id)
            {
                //选择中后再按OK键 记得在close window 调用 MMIAPI_ItemSelectedState(FALSE)
		
                if(MMIAPI_CheckOkKeyAndItemSelected(MMISET_TIME_FORMAT_SELECT_CTRL_ID,notify))
                {
				
               uint16 curIdx = GUILIST_GetCurItemIndex( list_ctrl_id );           
            MMIAPISET_SetTimeDisplayType(curIdx);
            MMK_CloseWin(win_id);
                
				}
			}else if(MMISET_TIME_FORMAT_SAVE_BTN_CTRL_ID == ((MMI_NOTIFY_T*)param)->src_id)//reset button被选中
        {
            uint16 curIdx = GUILIST_GetCurItemIndex( list_ctrl_id );
           
            MMIAPISET_SetTimeDisplayType(curIdx);
            MMK_CloseWin(win_id);
        }
					   }
        break;
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            MMK_CloseWin( win_id );
            break;
        }
    case MSG_CLOSE_WINDOW:
        {
			MMIAPI_ItemSelectedState(FALSE);
            break;
        }
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            MMK_CloseWin(win_id);
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}
LOCAL MMI_RESULT_E  HandleDisPlaySettingsWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
#if defined(ADD_TITLE_IN_ALL_SETTING_MENU)    
    GUI_RECT_T          rect         = MMITHEME_GetFullScreenRectEx(win_id);    
    GUI_RECT_T          content_rect=WATCHLIST_RECT;//{0,MMI_SPECIAL_TITLE_HEIGHT,240,240}; ////xiongkai ADD_TITLE_IN_ALL_SETTING_MENU    
    GUI_RECT_T          title_rect={0,0,240,MMI_SPECIAL_TITLE_HEIGHT};     
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    
    GUISTR_STYLE_T      text_style      = {0};/*lint !e64*/
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    MMI_STRING_T        string = {0};    
    GUI_RECT_T          text_rect={42,0,240,MMI_SPECIAL_TITLE_HEIGHT}; 
#endif
    uint32 listNum = 0;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TXT_DISPLAY_SETTING_ZX,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);

            listNum = sizeof(s_displaysetting_text_list_data)/sizeof(WATCHCOM_LIST_ITEM__ST);
            GUILIST_SetRect(MMISET_DISPLAYSETTING_LIST_CTRL_ID,&content_rect);
            WatchCOM_TextList_Create( s_displaysetting_text_list_data, listNum, MMISET_DISPLAYSETTING_LIST_CTRL_ID );
            MMK_SetAtvCtrl( MMISET_DISPLAYSETTING_WIN_ID, MMISET_DISPLAYSETTING_LIST_CTRL_ID );
            break;
         }
//#ifdef SCREEN_SHAPE_CIRCULAR
    case MSG_FULL_PAINT:
        {
            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TXT_DISPLAY_SETTING_ZX,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);

            break;
        }    
//#endif
    case MSG_CTL_OK:
    case MSG_CTL_MIDSK:
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_CTL_PENOK:
#endif
        {
            WatchCOM_List_Item_CallBack( MMISET_DISPLAYSETTING_LIST_CTRL_ID );
            break;
        }
    case  MSG_TP_PRESS_UP:
        {
           /* GUI_RECT_T   back_rect=WINDOW_BACK_RECT; 
            GUI_POINT_T   point = {0};
        
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if (GUI_PointIsInRect(point, back_rect))
                MMK_CloseWin( win_id);
            //Settings_Main_APP_CANCEL();*/
            break;
        }
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            MMK_CloseWin( MMISET_DISPLAYSETTING_WIN_ID );
            break;
        }
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}
PUBLIC void MMISET_SetLongRangeMode(uint8 onoff)
{
    uint8 temp_style = onoff;
    MMINV_WRITE(MMINV_SET_LONGRANGE_ON_OFF, &temp_style);
}
PUBLIC uint8 MMISET_GetLongRangeMode(void)
{

    uint8 onoff = 0;

    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;

    MMINV_READ(MMINV_SET_LONGRANGE_ON_OFF, &onoff, return_value);
    
    if (MN_RETURN_SUCCESS != return_value)
    {
        MMINV_WRITE(MMINV_SET_LONGRANGE_ON_OFF, &onoff);
    }
    return onoff;
}
LOCAL MMI_RESULT_E  HandleLongRangeModeWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
#if defined(ADD_TITLE_IN_ALL_SETTING_MENU)    
    GUI_RECT_T          rect         = MMITHEME_GetFullScreenRectEx(win_id);    
    GUI_RECT_T          title_rect={0,0,240,MMI_SPECIAL_TITLE_HEIGHT};     
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    
    GUISTR_STYLE_T      text_style      = {0};/*lint !e64*/
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    MMI_STRING_T        string = {0};    
    //GUI_RECT_T          text_rect={42,0,240,MMI_SPECIAL_TITLE_HEIGHT}; 
    GUI_RECT_T         yj_rect = {12,53,12+216,53+124};
    GUI_RECT_T       tip_rect = {27,104,213,166}; // tip_rect = {31,104,213,166};    
    GUI_RECT_T          text_switchrect={27,68,151,87}; 
    GUI_RECT_T          switchkey_rect={173,66,213,90}; 
    GUI_BG_T        text_bg = {0};
#endif
    uint32 listNum = 0;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            wchar*          pDisplayWStr = PNULL;
            uint16          displayWStrLen = 0;
            GUI_FONT_T      font = WATCH_FONT_16;
            GUI_COLOR_T     color = MMI_WHITE_COLOR;

            //LongRangeMode_Switch = 1;
            LongRangeMode_Switch = MMISET_GetLongRangeMode();
    
            pDisplayWStr = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(256) );
            if( PNULL == pDisplayWStr )
            {
                SCI_TRACE_LOW("pDisplayWStr is PNULL!!");
                return;
            }
            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TXT_LONG_RANGE_MODE_ZX,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);

            GUITEXT_SetRect( MMISET_LONGRANGEMODE_TEXT_CTRL_ID, &tip_rect);
            text_bg.color = MMI_BLACK_COLOR;//MMI_GRAY_WHITE_COLOR
            GUITEXT_SetBg(MMISET_LONGRANGEMODE_TEXT_CTRL_ID, &text_bg);
            GUITEXT_IsDisplayPrg(FALSE,MMISET_LONGRANGEMODE_TEXT_CTRL_ID);
            GUITEXT_SetClipboardEnabled (MMISET_LONGRANGEMODE_TEXT_CTRL_ID,FALSE);//bug 2087169
            GUITEXT_IsSlide(MMISET_LONGRANGEMODE_TEXT_CTRL_ID,FALSE);
            CTRLTEXT_SetHandleTpMsg(FALSE,MMISET_LONGRANGEMODE_TEXT_CTRL_ID);
            
            MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_LONGMODE_TIPS);
            GUITEXT_SetString( MMISET_LONGRANGEMODE_TEXT_CTRL_ID, pDisplayWStr, displayWStrLen, FALSE );
            SCI_FREE( pDisplayWStr );
            GUITEXT_SetFont( MMISET_LONGRANGEMODE_TEXT_CTRL_ID, &font, &color );
        
            MMI_GetLabelTextByLang(TXT_LONG_RANGE_MODE_ZX, &string);

            LCD_FillRoundedRect((const GUI_LCD_DEV_INFO *)&lcd_dev_info,  yj_rect, yj_rect,MMI_GRAY_WHITE_COLOR);

            text_style.align = ALIGN_LVMIDDLE;
            text_style.font = SONG_FONT_20;
            text_style.font_color = MMI_WHITE_COLOR;
            text_style.char_space = 0;
            
            GUISTR_DrawTextToLCDInRect( 
                (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                (const GUI_RECT_T      *)&text_switchrect,       //the fixed display area
                (const GUI_RECT_T      *)&text_switchrect,       //用户要剪切的实际区域
                (const MMI_STRING_T    *)&string,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );          
            break;
        }
//#ifdef SCREEN_SHAPE_CIRCULAR
    case MSG_FULL_PAINT:
        {
            MMI_IMAGE_ID_T        LongRangeMode_Switch_img_id;             //in:image id

            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TXT_LONG_RANGE_MODE_ZX,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);

//            LCD_FillRoundedRect((const GUI_LCD_DEV_INFO *)&lcd_dev_info,  yj_rect, yj_rect,MMI_DARK_GRAY2_COLOR);
            GUIRES_DisplayImg(PNULL,
                &yj_rect,
                PNULL,
                win_id,
                LongRangeMode_TIP_BG,
                &lcd_dev_info); 
            if(LongRangeMode_Switch){
                LongRangeMode_Switch_img_id = IMAGE_COMMON_KEYON_IMAG;
            }else{
                LongRangeMode_Switch_img_id = IMAGE_COMMON_KEYOFF_IMAG;
            }
            GUIRES_DisplayImg(PNULL,
                &switchkey_rect,
                PNULL,
                win_id,
                LongRangeMode_Switch_img_id,
                &lcd_dev_info);  

            text_style.align = ALIGN_LVMIDDLE;
            text_style.font = SONG_FONT_20;
            text_style.font_color = MMI_WHITE_COLOR;
            text_style.char_space = 0;
            MMI_GetLabelTextByLang(TXT_LONG_RANGE_MODE_ZX, &string);
            GUISTR_DrawTextToLCDInRect( 
                (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                (const GUI_RECT_T      *)&text_switchrect,       //the fixed display area
                (const GUI_RECT_T      *)&text_switchrect,       //用户要剪切的实际区域
                (const MMI_STRING_T    *)&string,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );
            //text_bg.color = MMI_BLACK_COLOR;//MMI_GRAY_WHITE_COLOR
            //GUITEXT_SetBg(MMISET_LONGRANGEMODE_TEXT_CTRL_ID, &text_bg);
            break;
        }    
//#endif
    case MSG_CTL_OK:
    case MSG_CTL_MIDSK:
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_CTL_PENOK:
#endif
        {
            break;
        }
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            GUI_RECT_T   back_rect=WINDOW_BACK_RECT; 
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);

            if((point.x > switchkey_rect.left && point.x <switchkey_rect.right)&&(point.y > switchkey_rect.top&&  point.y < switchkey_rect.bottom))
            {
                /*if(LongRangeMode_Switch)
                LongRangeMode_Switch = 0;
                else
                LongRangeMode_Switch = 1;*/
                WatchSET_LongRangeModeTipsWin(win_id);
                //MMK_SendMsg( win_id, MSG_FULL_PAINT, PNULL );
            }
            //else if (GUI_PointIsInRect(point, back_rect))
            //    MMK_CloseWin( win_id);
        }
        break;
    case MSG_APP_UP:
    case MSG_APP_DOWN:
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            MMK_CloseWin( MMISET_LONGRANGEMODE_WIN_ID );
            break;
        }
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}




LOCAL MMI_RESULT_E HandleSetLongRangeModeTipsWindow(
                                        MMI_WIN_ID_T    win_id, 
                                           MMI_MESSAGE_ID_E   msg_id, 
                                           DPARAM             param
                                           )
{
    MMI_RESULT_E            recode = MMI_RESULT_TRUE;
    GUI_RECT_T          rect         = MMITHEME_GetFullScreenRect();
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};    
    
    GUISTR_STYLE_T      text_style      = {0};/*lint !e64*/
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    MMI_STRING_T        string = {0};
    
    GUI_RECT_T leftBtnRect = SET_BTN_TWO_LEFT_RECT;
    GUI_RECT_T rightBtnRect = SET_BTN_TWO_RIGHT_RECT;

    MMI_TEXT_ID_T    text_id = 0;
    GUI_RECT_T    content_rect= MMITHEME_GetFullScreenRect();//{0,MMI_SPECIAL_TITLE_HEIGHT,240,240}; 
    MMI_WIN_ID_T parentwin_id = (uint32)MMK_GetWinAddDataPtr(win_id);
    rect.top =  0;
    content_rect.bottom = leftBtnRect.top;
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            GUI_FONT_T font = SONG_FONT_24;//SONG_FONT_16;
            GUI_COLOR_T color = MMI_WHITE_COLOR;    
            GUI_BG_T text_bg;
            wchar*                  pDisplayWStr = PNULL;
            uint16                  displayWStrLen = 0;
            wchar                   newLineCode = CR_CHAR;
            MMI_STRING_T        newLineStr = { 0 };

            LongRangeMode_Switch = MMISET_GetLongRangeMode();
                
            pDisplayWStr = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(255+1) );
            if( PNULL == pDisplayWStr )
            {
                SCI_TRACE_LOW("pDisplayWStr is PNULL!!");
                return;
            }
            newLineStr.wstr_ptr = &newLineCode;
            newLineStr.wstr_len = 1;
            LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR); 
                    
            GUIBUTTON_SetRect(MMISET_COMMON_L_BTN_CTRL_ID, &leftBtnRect);
            GUIBUTTON_SetRect(MMISET_COMMON_R_BTN_CTRL_ID, &rightBtnRect);


            if(LongRangeMode_Switch )
            {
                text_id = TXT_SET_LONGMODE_OFF_TIPS;
                //content_rect.left = content_rect.left+24;
                //content_rect.right = content_rect.right-24;
            }
            else
            {
                text_id= TXT_SET_LONGMODE_ON_TIPS;
                //content_rect.left = content_rect.left+2;
                //content_rect.right = content_rect.right-1;
            }
                    
            MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,text_id);

            CTRLTEXT_SetAlign(MMISET_SETLRTIPS_TEXT_CTRL_ID,ALIGN_HVMIDDLE);
            GUITEXT_IsDisplayPrg(FALSE,MMISET_SETLRTIPS_TEXT_CTRL_ID);
            GUITEXT_SetRect(MMISET_SETLRTIPS_TEXT_CTRL_ID,&content_rect);
            GUITEXT_SetFont(MMISET_SETLRTIPS_TEXT_CTRL_ID, &font, &color);
            text_bg.bg_type = GUI_BG_COLOR;
            text_bg.color = MMI_BLACK_COLOR;
            GUITEXT_SetBg(MMISET_SETLRTIPS_TEXT_CTRL_ID, &text_bg);
            GUITEXT_SetClipboardEnabled (MMISET_SETLRTIPS_TEXT_CTRL_ID,FALSE);//bug 2087169
            GUITEXT_IsSlide(MMISET_SETLRTIPS_TEXT_CTRL_ID,FALSE);
            GUITEXT_SetString( MMISET_SETLRTIPS_TEXT_CTRL_ID, pDisplayWStr, displayWStrLen, FALSE );

            SCI_FREE( pDisplayWStr );
        }
        break;
    case MSG_FULL_PAINT:
        LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
        break;

    case MSG_CTL_MIDSK:
    case MSG_KEYDOWN_OK:
    case MSG_CTL_OK:
    case MSG_APP_WEB:
#if defined( TOUCH_PANEL_SUPPORT)||defined(ZDT_TOUCHPANEL_TYPE_MULTITP) //IGNORE9527
    case MSG_CTL_PENOK:
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527
        //MMIAPIPHONE_PowerReset(); //开机
        if(MMISET_COMMON_L_BTN_CTRL_ID == ((MMI_NOTIFY_T*)param)->src_id)//reset button被选中
        {
            MMK_CloseWin(win_id); //取消
        }
        else if(MMISET_COMMON_R_BTN_CTRL_ID == ((MMI_NOTIFY_T*)param)->src_id)//play button被选中
        {
            if(LongRangeMode_Switch)
            {
                LongRangeMode_Switch = 0;
            }
            else
            {
                LongRangeMode_Switch = 1;
            }
            MMISET_SetLongRangeMode(LongRangeMode_Switch);
            WatchLAUNCHER_ZTE_LowBatterSwitch();
            if(parentwin_id == MMISET_LONGRANGEMODE_WIN_ID)
            {
                MMK_CloseWin(parentwin_id); 
            }
			MMK_CloseWin(win_id); 
        }
        break;
    case MSG_TP_PRESS_UP:
        break;
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:     
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

LOCAL void initLongRangeExitTipsCtrl()
{
    GUI_FONT_T font = SONG_FONT_24;//SONG_FONT_16;
    GUI_COLOR_T color = MMI_WHITE_COLOR;
    GUI_BG_T text_bg;
    GUI_RECT_T leftBtnRect = WATCH_SETOK_BTN_RECT;
    GUI_RECT_T content_rect = MMITHEME_GetFullScreenRect();
    content_rect.bottom = leftBtnRect.top;
    
    GUIBUTTON_SetRect(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID, &leftBtnRect);
    CTRLBUTTON_SetTextId(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID,STXT_EXIT );
    CTRLBUTTON_SetFontSize(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID,SONG_FONT_24 );
    CTRLBUTTON_SetFontColor(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID,MMI_WHITE_COLOR );
    
    CTRLTEXT_SetAlign(MMISET_SETLREXITTIPS_TEXT_CTRL_ID,ALIGN_HVMIDDLE);
    GUITEXT_IsDisplayPrg(FALSE,MMISET_SETLREXITTIPS_TEXT_CTRL_ID);
    GUITEXT_SetRect(MMISET_SETLREXITTIPS_TEXT_CTRL_ID,&content_rect);
    GUITEXT_SetFont(MMISET_SETLREXITTIPS_TEXT_CTRL_ID, &font, &color);
    text_bg.bg_type = GUI_BG_COLOR;
    text_bg.color = MMI_BLACK_COLOR;
    GUITEXT_SetBg(MMISET_SETLREXITTIPS_TEXT_CTRL_ID, &text_bg);
    GUITEXT_SetClipboardEnabled (MMISET_SETLREXITTIPS_TEXT_CTRL_ID,FALSE);//bug 2087169
    GUITEXT_IsSlide(MMISET_SETLREXITTIPS_TEXT_CTRL_ID,FALSE);
}

LOCAL void longRangeExitTips_setTips()
{
    wchar* pDisplayWStr = PNULL;
    uint16 displayWStrLen = 0;
    uint8 bat_cap = ZDT_GetBatteryPercent();

    LongRangeMode_Switch = MMISET_GetLongRangeMode();
    pDisplayWStr = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(255+1) );
    if( PNULL == pDisplayWStr )
    {
        SCI_TRACE_LOW("pDisplayWStr is PNULL!!");
        return;
    }

    if(bat_cap <= MMIPHONE_CHARGE_WARNING_CAP_LOW_POWER)
    {
        CTRLBUTTON_SetVisible(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID,FALSE,FALSE);
        MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_SET_LONGMODE_LBTIPS);
    }
    else
    {    
        CTRLBUTTON_SetVisible(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID,TRUE,FALSE);        
        MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_SET_LONGMODE_TIPS3);
    }

    GUITEXT_SetString( MMISET_SETLREXITTIPS_TEXT_CTRL_ID, pDisplayWStr, displayWStrLen, FALSE );

    SCI_FREE( pDisplayWStr );
}

LOCAL MMI_RESULT_E HandleSetLongRangeExitTipsWindow(
                                        MMI_WIN_ID_T    win_id, 
                                           MMI_MESSAGE_ID_E   msg_id, 
                                           DPARAM             param
                                           )
{
    MMI_RESULT_E            recode = MMI_RESULT_TRUE;
    GUI_RECT_T          rect         = MMITHEME_GetFullScreenRect();
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};    
    
    GUISTR_STYLE_T      text_style      = {0};/*lint !e64*/
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    MMI_STRING_T        string = {0};

    MMI_TEXT_ID_T    text_id = 0;
    MMI_WIN_ID_T parentwin_handle = (uint32)MMK_GetWinAddDataPtr(win_id);
    SCI_TRACE_LOW("win_id = 0x%0x, msg_id = 0x%0x.", win_id, msg_id);
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            initLongRangeExitTipsCtrl();
            longRangeExitTips_setTips();           
        }
        break;
    case MSG_FULL_PAINT:
        LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
        break;

    case MSG_CTL_MIDSK:
    case MSG_KEYDOWN_OK:
    case MSG_CTL_OK:
    case MSG_APP_WEB:
    case MSG_CTL_PENOK:
        if(MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID == ((MMI_NOTIFY_T*)param)->src_id)//reset button被选中
        {
            WatchSET_LongRangeModeTipsWin(win_id);
        }
        break;
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
        WatchLAUNCHER_ZTE_LowBatterToIdle();
        break;
    case MSG_GET_FOCUS:
        longRangeExitTips_setTips();
        MMK_PostMsg(win_id,MSG_FULL_PAINT,PNULL,PNULL);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}


WINDOW_TABLE( MMISET_DISPLAYSETTING_WIN_TAB ) =
{
    WIN_FUNC((uint32)HandleDisPlaySettingsWindow ),
    WIN_ID( MMISET_DISPLAYSETTING_WIN_ID ),
    WIN_HIDE_STATUS,
#ifdef ADULT_WATCH_SUPPORT
    WIN_TITLE(TXT_DISPLAY_SETTING_ZX),
#endif
    CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMISET_DISPLAYSETTING_LIST_CTRL_ID ),
#ifndef ADULT_WATCH_SUPPORT
   // WIN_SOFTKEY( STXT_OK, TXT_NULL, STXT_RETURN ),
#endif
    END_WIN
};

WINDOW_TABLE( MMISET_LONGRANGEMODE_WIN_TAB ) =
{
    WIN_FUNC((uint32)HandleLongRangeModeWindow ),
    WIN_ID( MMISET_LONGRANGEMODE_WIN_ID ),
    WIN_HIDE_STATUS,
    // CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMISET_DISPLAYSETTING_LIST_CTRL_ID ),
    CREATE_TEXT_CTRL( MMISET_LONGRANGEMODE_TEXT_CTRL_ID ),
#ifndef ADULT_WATCH_SUPPORT
    //WIN_SOFTKEY( STXT_OK, TXT_NULL, STXT_RETURN ),
#endif
    END_WIN
};

WINDOW_TABLE(MMISET_LONGRANGEMODTIPS_WIN_TAB) =
{
    //WIN_STATUSBAR,
    WIN_HIDE_STATUS,
    WIN_FUNC( (uint32)HandleSetLongRangeModeTipsWindow),
    WIN_ID(MMISET_LONGRANGEMODETIPS_WIN_ID),     
    CREATE_BUTTON_CTRL( res_common_set_cancel, MMISET_COMMON_L_BTN_CTRL_ID ),
    CREATE_BUTTON_CTRL( res_common_set_confirm, MMISET_COMMON_R_BTN_CTRL_ID ),
    //CREATE_BUTTON_CTRL( IMAGE_COMMON_ONEBTN_BG_IMAG, MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID),
   
    CREATE_TEXT_CTRL(MMISET_SETLRTIPS_TEXT_CTRL_ID),
    END_WIN
};

WINDOW_TABLE(MMISET_LONGRANGEEIXTTIPS_WIN_TAB) =
{
    //WIN_STATUSBAR,
    WIN_HIDE_STATUS,
    WIN_FUNC( (uint32)HandleSetLongRangeExitTipsWindow),
    WIN_ID(MMISET_LONGRANGEEIXTTIPS_WIN_ID),
    WIN_STYLE(WS_DISPATCH_TO_CHILDWIN |WS_DISABLE_RETURN_WIN | WS_DISABLE_FLING_CLOSE_WIN),
    //CREATE_BUTTON_CTRL( res_common_set_cancel, MMISET_COMMON_L_BTN_CTRL_ID ),
    //CREATE_BUTTON_CTRL( res_common_set_confirm, MMISET_COMMON_R_BTN_CTRL_ID ),
    CREATE_BUTTON_CTRL( IMAGE_COMMON_ONEBTN_KBBG_IMAG, MMISET_ZTE_WATCH_SETOK_BTN_CTRL_ID),
   
    CREATE_TEXT_CTRL(MMISET_SETLREXITTIPS_TEXT_CTRL_ID),
    END_WIN
};
WINDOW_TABLE( MMISET_TIME_FORMAT_WIN_TAB ) =
{
    WIN_FUNC((uint32)HandleSelectTimeFormatWindow),
    WIN_ID( MMISET_TIME_FORMAT_WIN_ID ),
    WIN_STYLE(WS_NO_DEFAULT_STYLE),
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMISET_TIME_FORMAT_SELECT_CTRL_ID),
    CREATE_BUTTON_CTRL( IMAGE_COMMON_ONEBTN_BG_IMAG, MMISET_TIME_FORMAT_SAVE_BTN_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

#endif

#ifdef MAINMENU_STYLE_SUPPORT

WINDOW_TABLE( MMISET_SCENE_MODE_WIN_TAB ) =
{
    WIN_FUNC((uint32)HandleSelectSceneModeWindow),
    WIN_ID( MMISETZDT_SCENE_MODE_WIN_ID ),
    WIN_STYLE(WS_NO_DEFAULT_STYLE),
    CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E, MMISET_SCENE_MODE_SELECT_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

#endif

WINDOW_TABLE( MMISET_MAIN_WIN_TAB ) =
{
    WIN_FUNC((uint32)HandleSettingsMainWindow ),
    WIN_ID( MMISET_MAIN_WIN_ID ),
    WIN_HIDE_STATUS,
#ifdef ADULT_WATCH_SUPPORT
    WIN_TITLE(TXT_SET_LIST_SETTING),
#endif
    CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMISET_MAIN_LIST_CTRL_ID ),
    //CREATE_MENU_CTRL(SETTINGS_MENULIST_ICON, MMISET_MAIN_LIST_CTRL_ID),
#ifndef ADULT_WATCH_SUPPORT
    //WIN_SOFTKEY( STXT_OK, TXT_NULL, STXT_RETURN ),
#endif
    END_WIN
};

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
 #ifdef  ZTE_WATCH
PUBLIC void Watch_Time_Format_Switch( void )
{
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    if (MMK_IsOpenWin(MMISET_TIME_FORMAT_WIN_ID))
    {
        MMK_CloseWin(MMISET_TIME_FORMAT_WIN_ID);
    }
    win_handle = MMK_CreateWin( (uint32 *)MMISET_TIME_FORMAT_WIN_TAB, PNULL );
    if(win_handle != NULL)
    {
        MMK_SetWinRect(win_handle, &rect);
    }
}

PUBLIC void Watch_LongRangeMode_switch( void )
{
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    if (MMK_IsOpenWin(MMISET_LONGRANGEMODE_WIN_ID))
    {
        MMK_CloseWin(MMISET_LONGRANGEMODE_WIN_ID);
    }
    win_handle = MMK_CreateWin( (uint32 *)MMISET_LONGRANGEMODE_WIN_TAB, PNULL );
    if(win_handle != NULL)
    {
        MMK_SetWinRect(win_handle, &rect);
    }
}

 PUBLIC void WatchSET_DisPlaySetting_Enter( void )
{
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    if (MMK_IsOpenWin(MMISET_DISPLAYSETTING_WIN_ID))
    {
        MMK_CloseWin(MMISET_DISPLAYSETTING_WIN_ID);
    }
    win_handle = MMK_CreateWin( (uint32 *)MMISET_DISPLAYSETTING_WIN_TAB, PNULL );
    if(win_handle != NULL)
        MMK_SetWinRect(win_handle, &rect);
}

PUBLIC void WatchSET_LongRangeModeTipsWin(MMI_WIN_ID_T parent_winid)
{
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    if (MMK_IsOpenWin(MMISET_LONGRANGEMODETIPS_WIN_ID))
    {
        MMK_CloseWin(MMISET_LONGRANGEMODETIPS_WIN_ID);
    }
    win_handle = MMK_CreateWin((uint32*)MMISET_LONGRANGEMODTIPS_WIN_TAB,(ADD_DATA)parent_winid);
    if(win_handle != NULL)
    {
        MMK_SetWinRect(win_handle, &rect);
    }
}

PUBLIC void WatchSET_LongRangeModeExitTipsWin(void)
{
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    if (MMK_IsOpenWin(MMISET_LONGRANGEEIXTTIPS_WIN_ID))
    {
        MMK_CloseWin(MMISET_LONGRANGEEIXTTIPS_WIN_ID);
    }
    win_handle = MMK_CreateWin((uint32*)MMISET_LONGRANGEEIXTTIPS_WIN_TAB,PNULL);
    if(win_handle != NULL)
    {
        MMK_SetWinRect(win_handle, &rect);
    }
}
#endif

#if MAINMENU_STYLE_SUPPORT
 PUBLIC void WatchSET_MenuStyle_Enter( void )
{

    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    if (MMK_IsOpenWin(MMISETZDT_SCENE_MODE_WIN_ID))
    {
        MMK_CloseWin(MMISETZDT_SCENE_MODE_WIN_ID);
    }
    win_handle = MMK_CreateWin((uint32*)MMISET_SCENE_MODE_WIN_TAB,PNULL);
    if(win_handle != NULL)
        MMK_SetWinRect(win_handle, &rect);
}
#endif

PUBLIC void WatchSET_MainWin_Enter( void )
{
    MMI_HANDLE_T win_handle;
    GUI_RECT_T rect ;
    if(MMK_IsOpenWin(MMISET_MAIN_WIN_ID))
    {
        MMK_CloseWin(MMISET_MAIN_WIN_ID);
    }
    win_handle = MMK_CreateWin( (uint32 *)MMISET_MAIN_WIN_TAB, PNULL );
    rect = MMITHEME_GetFullScreenRect();
    if(win_handle != NULL)
        MMK_SetWinRect(win_handle, &rect);
}

PUBLIC void WatchSET_MainWin_Exit( void )
{
    if(MMK_IsOpenWin(MMISET_MAIN_WIN_ID))
    {
        MMK_CloseWin(MMISET_MAIN_WIN_ID);
    }
}

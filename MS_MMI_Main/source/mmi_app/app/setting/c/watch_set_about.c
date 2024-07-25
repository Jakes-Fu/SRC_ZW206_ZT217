/*****************************************************************************
** File Name:      watch_set_about.c                                        *
** Author:         bin.wang1                                                 *
** Date:           03/26/2020                                                *
** Copyright:      All Rights Reserved.                                      *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 03/2020      bin.wang1              Creat
******************************************************************************/

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "guilistbox.h"
#include "watch_common_list.h"
#include "os_api.h"
#include "sci_types.h"
#include "window_parse.h"
//#include "guilabel.h"
//#include "guibutton.h"
#include "guitext.h"
#include "version.h"

#include "mmidisplay_data.h"
#include "mmicom_trace.h"
#include "mmiset_image.h"
#include "mmiset_text.h"
#include "mmiset_id.h"
#include "mmiset_export.h"
#include "watch_set_about.h"

#include "watch_set_position.h"
#include "mmisrveng.h"
#include "watch_common.h"
#include "mmieng_export.h"
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/

#define SETTINGS_VERSION_INFO_MAX_LEN           (255)
#ifdef ZTE_SUPPORT_240X284
#define WATCH_ABOUT_RECT      DP2PX_RECT(20,46,220,310)
#else
#ifdef SCREEN_SHAPE_CIRCULAR
#define WATCH_ABOUT_RECT      DP2PX_RECT(20,46,220,239)
#endif
#endif

/**--------------------------------------------------------------------------*
 **                         RECT DEFINITION                                  *
 **--------------------------------------------------------------------------*/



/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

#define WATCH_MODE_MANUF        ZDT_SFR_MANUF
#define WATCH_MODE_NAME        ZDT_SFR_TYPE
#define WATCH_HARDWARE_VERSION  ZDT_SFR_HW_VER
#define WATCH_SOFTWARE_VERSION  ZDT_CTA_SW_VER

#ifdef ZTE_WATCH

#define WATCH_NETFLG_ID      "ABCDEFGHIJKLMNOPQRSTU"


PUBLIC void Settings_DeviceInfo_Enter( void );
PUBLIC void Settings_CerficationMark_Enter( void );

LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_ST   list_item_deviceinfo    = { TEXT_DEVICE_INFO};
LOCAL WATCHCOM_LIST_ITEM_STYLE_1STR_ST   list_item_certification_mark    = { TEXT_CERTIFICATION_MARK };

LOCAL WATCHCOM_LIST_ITEM__ST s_deviceinfo_text_list_data[] =
{
/*#ifdef  ZTE_SUPPORT_240X284
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_1LINE_BIG_LAYOUT1,   &list_item_backlight,    Settings_BackLight_Enter },
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR_1LINE_BIG_LAYOUT1,   &list_item_brightness,   MMIAPISET_ZdtBrightnessWin },
#else*/
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR,   &list_item_deviceinfo,    Settings_DeviceInfo_Enter },
    { WatchCOM_List_Item_Visible_Default,   GUIITEM_STYLE_1STR,   &list_item_certification_mark,   Settings_CerficationMark_Enter },
//#endif
};



LOCAL void Settings_About_OPEN_WINDOW( MMI_WIN_ID_T win_id )
{
    const char*     pVersionStr = PNULL;
    uint16          versionStrLen = 0;
    wchar*          pDisplayWStr = PNULL;
    uint16          displayWStrLen = 0;
    wchar           newLineCode = CR_CHAR;
    MMI_STRING_T    newLineStr = { 0 };
    MMI_CTRL_ID_T   textCtrlId = MMISET_ABOUT_TEXT_CTRL_ID;
#ifdef SCREEN_SHAPE_CIRCULAR
    GUI_FONT_T      font = WATCH_FONT_16;
    GUI_COLOR_T     color = MMI_WHITE_COLOR;
    GUI_RECT_T      text_rect = WATCH_ABOUT_RECT;
    GUI_BG_T        text_bg = {0};
#else
    GUI_FONT_T      font = WATCH_DEFAULT_NORMAL_FONT;
    GUI_COLOR_T     color = MMI_BLACK_COLOR;
#endif
    MN_IMEISVN_T imeisvn = {0};/*lint !e64*/
    uint8 imei_str[(MN_MAX_IMEI_LENGTH<<1) + 1] = {0};
    char*           temp_ptr = WATCH_SOFTWARE_VERSION;//PNULL;
    uint16          temp_len = 0;
    uint16 	imei_len = 0;
	uint8 	i;
    pDisplayWStr = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(SETTINGS_VERSION_INFO_MAX_LEN+1) );
    if( PNULL == pDisplayWStr )
    {
        TRACE_APP_SETTINGS("pDisplayWStr is PNULL!!");
        return;
    }
    newLineStr.wstr_ptr = &newLineCode;
    newLineStr.wstr_len = 1;
#if 0
    //Hardware Version
#ifdef WIN32
    pVersionStr = (char*)"HW Version: uws317_watch";
#else
    pVersionStr = VERSION_GetInfo( HW_VERSION );
#endif
    versionStrLen = SCI_STRLEN( (char*)pVersionStr );
    MMIAPICOM_StrcatFromStrToUCS2( pDisplayWStr, &displayWStrLen, (uint8*)pVersionStr, versionStrLen );
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
	//MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_MODEL_NAME);
	//MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
    //MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line

    //Software Version
#ifdef WIN32
    pVersionStr = (char*)"Platform Version: MOCOR_W09.23_Debug";
#else
    pVersionStr = VERSION_GetInfo( PLATFORM_VERSION );   /*lint !e605*/
#endif
    versionStrLen = SCI_STRLEN( (char*)pVersionStr );
    MMIAPICOM_StrcatFromStrToUCS2( pDisplayWStr, &displayWStrLen, (uint8*)pVersionStr, versionStrLen );
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
    #endif

   // MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_MODEL_ZTE);
	//MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
   // MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, WATCH_NAME, strlen(WATCH_NAME));
  //  MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行

	MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_MODEL_NAME);

	MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
	//MMIAPICOM_ConvertAlphabetToUnicode(WATCH_MODE_NAME,pDisplayWStr,SETTINGS_VERSION_INFO_MAX_LEN);
	//displayWStrLen += MMIAPICOM_Wstrlen(pDisplayWStr);
	//MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, WATCH_MODE_NAME, strlen(WATCH_MODE_NAME));
	MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, WATCH_CTA_MODEL, strlen(WATCH_CTA_MODEL));
	MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行

    //MMI_GetLabelTextByLang(TXT_WATCH_IMEI, &title_str);
    //MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &title_str);
    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_IMEI);
    //MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行

#if 1
    for(i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MNNV_GetIMEISVNEx(i, &imeisvn))
        {		         
            MMIAPICOM_BcdToStr(PACKED_LSB_FIRST, (uint8 *)imeisvn.imei, MN_MAX_IMEI_LENGTH<<1, (char*)imei_str);
				 
            imei_len = (uint16)strlen((char *)imei_str) - 1;
            MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, (uint8 *)&imei_str[1], imei_len);
            MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
        }
    }
    #endif
    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_STRING_VERSION);
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, ZDT_SFR_SW_VER, strlen(ZDT_SFR_SW_VER));
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行		
		
		
    // SwVersionInfo(content_text);
    //yangyu add
    //MMI_GetLabelTextByLang(TXT_WATCH_SOFTWARE_VERSION, &title_str);
    //MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &title_str);
    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_SOFTWARE_VERSION);
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    //temp_ptr = ZDT_GetVersionInfo();
    temp_len = SCI_STRLEN((char*)temp_ptr);
    MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, (uint8*)temp_ptr, temp_len);
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    //yangyu end

		
    //MMI_GetLabelTextByLang(TXT_WATCH_HARDWARE_VERSION, &title_str);
    // MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &title_str);
//#ifndef ZTE_WATCH //努比亚要求去掉硬件版本
    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_HARDWARE_VERSION);
    // MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, WATCH_HARDWARE_VERSION, strlen(WATCH_HARDWARE_VERSION));
    //MMIAPICOM_StrcatFromSTRINGToUCS2(content_text, &uint16_str_len, &cr_s);//换行
//#endif	
    //content_str.wstr_ptr = content_text;
    //content_str.wstr_len = MMIAPICOM_Wstrlen(content_text);
		
    text_bg.bg_type = GUI_BG_COLOR;
    text_bg.color = MMI_BLACK_COLOR;
    CTRLTEXT_SetAlign(textCtrlId,ALIGN_HVMIDDLE);
    GUITEXT_IsDisplayPrg(FALSE,textCtrlId);
    GUITEXT_SetBg(textCtrlId, &text_bg);
    GUITEXT_SetRect( textCtrlId, &text_rect);
    GUITEXT_SetClipboardEnabled (textCtrlId,FALSE);//bug 2087169
    GUITEXT_IsSlide(textCtrlId,FALSE);
    GUITEXT_SetHandleTpMsg(FALSE,textCtrlId);

    GUITEXT_SetString( textCtrlId, pDisplayWStr, displayWStrLen, FALSE );
    GUITEXT_SetFont( textCtrlId, &font, &color );

    SCI_FREE( pDisplayWStr );
}
#else

LOCAL void Settings_About_OPEN_WINDOW( MMI_WIN_ID_T win_id )
{
    const char*     pVersionStr = PNULL;
    uint16          versionStrLen = 0;
    wchar*          pDisplayWStr = PNULL;
    uint16          displayWStrLen = 0;
    wchar           newLineCode = CR_CHAR;
    MMI_STRING_T    newLineStr = { 0 };
    MMI_CTRL_ID_T   textCtrlId = MMISET_ABOUT_TEXT_CTRL_ID;
#ifdef SCREEN_SHAPE_CIRCULAR
    GUI_FONT_T      font = WATCH_FONT_22;
    GUI_COLOR_T     color = MMI_WHITE_COLOR;
    GUI_RECT_T      text_rect = WATCH_ABOUT_RECT;
    GUI_BG_T        text_bg = {0};
#else
    GUI_FONT_T      font = WATCH_DEFAULT_NORMAL_FONT;
    GUI_COLOR_T     color = MMI_BLACK_COLOR;
#endif
	 MN_IMEISVN_T imeisvn = {0};/*lint !e64*/
    uint8 imei_str[(MN_MAX_IMEI_LENGTH<<1) + 1] = {0};
    char*           temp_ptr = WATCH_SOFTWARE_VERSION;//PNULL;
    uint16          temp_len = 0;
    uint16 	imei_len = 0;
	uint8 	i;
    pDisplayWStr = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(SETTINGS_VERSION_INFO_MAX_LEN+1) );
    if( PNULL == pDisplayWStr )
    {
        TRACE_APP_SETTINGS("pDisplayWStr is PNULL!!");
        return;
    }
    newLineStr.wstr_ptr = &newLineCode;
    newLineStr.wstr_len = 1;
#if 0
    //Hardware Version
#ifdef WIN32
    pVersionStr = (char*)"HW Version: uws317_watch";
#else
    pVersionStr = VERSION_GetInfo( HW_VERSION );
#endif
    versionStrLen = SCI_STRLEN( (char*)pVersionStr );
    MMIAPICOM_StrcatFromStrToUCS2( pDisplayWStr, &displayWStrLen, (uint8*)pVersionStr, versionStrLen );
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
	//MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_MODEL_NAME);
	//MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
    //MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line

    //Software Version
#ifdef WIN32
    pVersionStr = (char*)"Platform Version: MOCOR_W09.23_Debug";
#else
    pVersionStr = VERSION_GetInfo( PLATFORM_VERSION );   /*lint !e605*/
#endif
    versionStrLen = SCI_STRLEN( (char*)pVersionStr );
    MMIAPICOM_StrcatFromStrToUCS2( pDisplayWStr, &displayWStrLen, (uint8*)pVersionStr, versionStrLen );
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
#endif

    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_MODEL_NAME);

    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, WATCH_MODE_NAME, strlen(WATCH_MODE_NAME));
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行

    //MMI_GetLabelTextByLang(TXT_WATCH_IMEI, &title_str);
    //MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &title_str);
    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_IMEI);
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行

#if 1
    for(i = MN_DUAL_SYS_1; i < MMI_DUAL_SYS_MAX; i++)
    {
        if (MNNV_GetIMEISVNEx(i, &imeisvn))
        {
		         
            MMIAPICOM_BcdToStr(PACKED_LSB_FIRST, (uint8 *)imeisvn.imei, MN_MAX_IMEI_LENGTH<<1, (char*)imei_str);
				 
            imei_len = (uint16)strlen((char *)imei_str) - 1;
            MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, (uint8 *)&imei_str[1], imei_len);
            MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
        }
    }
#endif		
   
    // SwVersionInfo(content_text);
    //yangyu add
    //MMI_GetLabelTextByLang(TXT_WATCH_SOFTWARE_VERSION, &title_str);
    //MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &title_str);
    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_SOFTWARE_VERSION);
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    //temp_ptr = ZDT_GetVersionInfo();
    temp_len = SCI_STRLEN((char*)temp_ptr);
    MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, (uint8*)temp_ptr, temp_len);
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    //yangyu end

		
    //MMI_GetLabelTextByLang(TXT_WATCH_HARDWARE_VERSION, &title_str);
    // MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &title_str);
    MMIAPICOM_StrcatFromTextIDToUCS2(pDisplayWStr,&displayWStrLen,TXT_WATCH_HARDWARE_VERSION);
    MMIAPICOM_StrcatFromSTRINGToUCS2(pDisplayWStr, &displayWStrLen, &newLineStr);//换行
    MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, WATCH_HARDWARE_VERSION, strlen(WATCH_HARDWARE_VERSION));
    //MMIAPICOM_StrcatFromSTRINGToUCS2(content_text, &uint16_str_len, &cr_s);//换行
	
    //content_str.wstr_ptr = content_text;
    //content_str.wstr_len = MMIAPICOM_Wstrlen(content_text);
		
#ifdef SCREEN_SHAPE_CIRCULAR
    text_bg.bg_type = GUI_BG_COLOR;
    text_bg.color = MMI_BLACK_COLOR;

    GUITEXT_IsDisplayPrg(FALSE,textCtrlId);
    GUITEXT_SetBg(textCtrlId, &text_bg);
    GUITEXT_SetRect( textCtrlId, &text_rect);
    GUITEXT_SetClipboardEnabled (textCtrlId,FALSE);//bug 2087169
#endif
    GUITEXT_SetString( textCtrlId, pDisplayWStr, displayWStrLen, FALSE );
    GUITEXT_SetFont( textCtrlId, &font, &color );

    SCI_FREE( pDisplayWStr );
}
#endif

#ifdef ADULT_WATCH_SUPPORT
LOCAL void Settings_About_FullPaint( MMI_WIN_ID_T win_id )
{
    MMI_STRING_T	 str_ptr = { 0 };
    const char*      pVersionStr = PNULL;
    uint16           versionStrLen = 0;
    wchar*           pDisplayWStr = PNULL;
    uint16           displayWStrLen = 0;
    wchar            newLineCode = CR_CHAR;
    MMI_STRING_T     newLineStr = { 0 };
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUISTR_STYLE_T   text_style = {0};
    GUISTR_STATE_T   text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
    GUI_RECT_T       win_rect = DP2PX_RECT(0, 0, 240, 240);

    pDisplayWStr = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(SETTINGS_VERSION_INFO_MAX_LEN+1) );
    if( PNULL == pDisplayWStr )
    {
        TRACE_APP_SETTINGS("pDisplayWStr is PNULL!!");
        return;
    }
    newLineStr.wstr_ptr = &newLineCode;
    newLineStr.wstr_len = 1;

    //Hardware Version
#ifdef WIN32
    pVersionStr = (char*)"HW Version: uws317_watch";
#else
    pVersionStr = VERSION_GetInfo( HW_VERSION );
#endif
    versionStrLen = SCI_STRLEN( (char*)pVersionStr );
    MMIAPICOM_StrcatFromStrToUCS2( pDisplayWStr, &displayWStrLen, (uint8*)pVersionStr, versionStrLen );
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line

    //Software Version
#ifdef WIN32
    pVersionStr = (char*)"Platform Version: MOCOR_W09.23_Debug";
#else
    pVersionStr = VERSION_GetInfo( PLATFORM_VERSION );   /*lint !e605*/
#endif
    versionStrLen = SCI_STRLEN( (char*)pVersionStr );
    MMIAPICOM_StrcatFromStrToUCS2( pDisplayWStr, &displayWStrLen, (uint8*)pVersionStr, versionStrLen );
    MMIAPICOM_StrcatFromSTRINGToUCS2( pDisplayWStr, &displayWStrLen, &newLineStr );   //new line

    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    text_style.align = ALIGN_HMIDDLE;
    text_style.font = ADULT_WATCH_DEFAULT_NORMAL_FONT;
    text_style.font_color = MMI_WHITE_COLOR;
    win_rect.top += MMI_TITLE_THEME_HEIGHT;
    LCD_FillRect(&lcd_dev_info,win_rect,MMI_BLACK_COLOR);
    str_ptr.wstr_ptr = pDisplayWStr;
    str_ptr.wstr_len = displayWStrLen;

    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &win_rect,
        &win_rect,
        &str_ptr,	   
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
        );

    SCI_FREE( pDisplayWStr );
}
#endif
LOCAL void Settings_About_APP_CANCEL( MMI_WIN_ID_T win_id )
{
    MMK_CloseWin( win_id );
}

LOCAL MMI_RESULT_E  HandleSettingsRegardWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E reCode =  MMI_RESULT_TRUE;

    switch(msg_id)
    {
#ifndef ADULT_WATCH_SUPPORT
    case MSG_OPEN_WINDOW:
        {
            Settings_About_OPEN_WINDOW( win_id );
            break;
        }
#ifdef SCREEN_SHAPE_CIRCULAR
    case MSG_FULL_PAINT:
        {
            MMI_MENU_GROUP_ID_T         group_id        =   0;
            MMI_MENU_ID_T               menu_id         =   0;

            #ifdef ZTE_WATCH
            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TEXT_DEVICE_INFO,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);
            #else
            WATCHCOM_DisplayBackgroundEx(win_id,IMAGE_THEME_BLACK_BG_SPEC);
            WATCHCOM_DisplayTitleEx(win_id,TEXT_WATCH_INFO,MMI_SPECIAL_TITLE_HEIGHT,MMI_WHITE_COLOR,ALIGN_LVMIDDLE);
            #endif
			
            //WATCHCOM_DisplayBackground(win_id);
            break;
        }
#endif
#else
    case MSG_FULL_PAINT:
        Settings_About_FullPaint( win_id );
        break;
#endif
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            Settings_About_APP_CANCEL( win_id );
            break;
        }

    #ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
    case MSG_CTL_PENOK:
    #endif
    case MSG_KEYDOWN_5:
        {
            if(!MMIENG_IsMonkeyTestMode())
            {
                //MMIAPIENG_MainWinEnter( 0,FALSE );
            }
            break;
        }
    case MSG_TP_PRESS_UP:
        {
#ifndef ZTE_WATCH
            GUI_POINT_T   point = {0};
            GUI_RECT_T   back_rect=WINDOW_BACK_RECT; 
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);

            if (GUI_PointIsInRect(point, back_rect))
            {
                MMK_CloseWin(win_id);
                return;	 
            }
#endif
            break;
        }
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
    default:
        reCode = MMI_RESULT_FALSE;
        break;
    }

    return reCode;
}
LOCAL MMI_RESULT_E  HandleSettingsDeviceInfoWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E reCode =  MMI_RESULT_TRUE;
    MMI_HANDLE_T    listbox_id = MMISET_DEVICEINFO_LIST_CTRL_ID;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            uint32 listNum = 0;
            GUI_RECT_T        list_rect = WATCHLIST_RECT;
            
            
            //WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TEXT_WATCH_INFO,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);

            listNum = sizeof(s_deviceinfo_text_list_data)/sizeof(WATCHCOM_LIST_ITEM__ST);
            GUILIST_SetRect(listbox_id,&list_rect);
            WatchCOM_TextList_Create( s_deviceinfo_text_list_data, listNum, listbox_id );
            MMK_SetAtvCtrl( win_id, listbox_id );
            break;
        }
    case MSG_FULL_PAINT:
        {
            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TEXT_WATCH_INFO,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE);
            break;
        }
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            Settings_About_APP_CANCEL( win_id );
            break;
        }

    case MSG_CTL_OK:
    case MSG_CTL_MIDSK:
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_CTL_PENOK:
#endif
        {
            WatchCOM_List_Item_CallBack( MMISET_DEVICEINFO_LIST_CTRL_ID );
            break;
        }
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
    default:
        reCode = MMI_RESULT_FALSE;
        break;
    }

    return reCode;
}

LOCAL void Settings_CerficationMark_OPEN_WINDOW( MMI_WIN_ID_T win_id )
{
    wchar*          pDisplayWStr = PNULL;
    uint16          displayWStrLen = 0;
    MMI_CTRL_ID_T   textCtrlId = MMISET_CERFICATION_TEXT_CTRL_ID;

    GUI_FONT_T      font = SONG_FONT_24;
    GUI_COLOR_T     color = MMI_BLACK_COLOR;
    GUI_RECT_T      text_rect = CERTIFIATION_MODEL_RECT;
    GUI_BG_T        text_bg = {0};

    GUI_RECT_T         bg_rect = CERTIFIATION_BG_RECT;//{17,(MMI_MAINSCREEN_HEIGHT -104)/2+MMI_SPECIAL_TITLE_HEIGHT,95,17+206,95+104};

    GUI_RECT_T         	disp_rect = CERTIFIATION_MODEL_RECT;//{17,95+40,17+206,95+80};

    pDisplayWStr = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(50) );
    if( PNULL == pDisplayWStr )
    {
        TRACE_APP_SETTINGS("pDisplayWStr is PNULL!!");
        return;
    }
    SCI_MEMSET(pDisplayWStr, 0, 100*sizeof(char));
    MMIAPICOM_StrcatFromStrToUCS2(pDisplayWStr, &displayWStrLen, WATCH_CTA_MODEL, strlen(WATCH_CTA_MODEL));

    text_bg.bg_type = GUI_BG_COLOR;
    text_bg.color = MMI_WHITE_COLOR;
    CTRLTEXT_SetAlign(textCtrlId,ALIGN_HVMIDDLE);
    GUITEXT_IsDisplayPrg(FALSE,textCtrlId);
    GUITEXT_SetBg(textCtrlId, &text_bg);
    GUITEXT_SetRect( textCtrlId, &text_rect);
    GUITEXT_SetClipboardEnabled (textCtrlId,FALSE);//bug 2087169

    GUITEXT_SetString( textCtrlId, pDisplayWStr, displayWStrLen, FALSE );
    GUITEXT_SetFont( textCtrlId, &font, &color );

    SCI_FREE( pDisplayWStr );
}

LOCAL MMI_RESULT_E  HandleCerficationMarkWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E reCode =  MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            Settings_CerficationMark_OPEN_WINDOW(win_id);
            break;
        }

    case MSG_FULL_PAINT:
        { 
            GUI_RECT_T         bg_rect = CERTIFIATION_BG_RECT;//{17,(MMI_MAINSCREEN_HEIGHT -104)/2+MMI_SPECIAL_TITLE_HEIGHT,95,17+206,95+104};
            GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};

            MMI_STRING_T   model_string     = {0};
            GUISTR_STYLE_T text_style = {0};
            GUI_RECT_T         	disp_rect = CERTIFIATION_MODEL_RECT;//{17,95+40,17+206,95+80};
            uint16          displayWStrLen = 0;
            uint16 *model_str= NULL;//[100] = {0};
            char *temp_str= WATCH_CMIIT_ID;//WATCHCOM_GetDeviceModel();
            uint8 net_id[24] = {0};
            
            WATCHCOM_DisplayBackground(win_id);
            WATCHCOM_DisplayTitleEx(win_id,TEXT_CERTIFICATION_MARK,MMI_SPECIAL_TITLE_HEIGHT,MMI_CUSTOM_BLUE_COLOR,ALIGN_HVMIDDLE); 
            GUIRES_DisplayImg(PNULL,&bg_rect,PNULL,win_id,IMAGE_SETTING_CERFICATION_BG,&lcd_dev_info);
                
            disp_rect.top =  bg_rect.bottom+10;
            disp_rect.bottom =  disp_rect.top+30;
            text_style.font = SONG_FONT_24;
            text_style.font_color = MMI_WHITE_COLOR;
            text_style.align = ALIGN_HVMIDDLE;
            //disp_rect.bottom = disp_rect.top + title_height;
            
            model_str = (wchar*)SCI_ALLOC_APPZ( sizeof(wchar)*(100) );
            if( PNULL == model_str )
            {
                TRACE_APP_SETTINGS("pDisplayWStr is PNULL!!");
                return;
            }
            SCI_MEMSET(model_str, 0, 200*sizeof(char));
            MMIAPICOM_StrcatFromStrToUCS2(model_str, &displayWStrLen, temp_str, strlen((char*)temp_str));
            model_string.wstr_ptr = model_str;
            model_string.wstr_len = displayWStrLen;
            GUISTR_DrawTextToLCDInRect(
                &lcd_dev_info,
                (const GUI_RECT_T *)&disp_rect,       //the fixed display area
                (const GUI_RECT_T *)&disp_rect,       
                (const MMI_STRING_T *)&model_string,
                &text_style,
                GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
                GUISTR_TEXT_DIR_AUTO);

            MMIZDT_Get_CTA_Net_Id(net_id);
            disp_rect.top =  bg_rect.bottom-20;
            disp_rect.bottom =  bg_rect.bottom-2;
            text_style.font = SONG_FONT_14;
            text_style.font_color = MMI_BLACK_COLOR;
            text_style.align = ALIGN_HVMIDDLE;   
            displayWStrLen = 0;

            SCI_MEMSET(model_str, 0, 200*sizeof(char));
            MMIAPICOM_StrcatFromStrToUCS2(model_str, &displayWStrLen, net_id, strlen((char*)net_id));
            model_string.wstr_ptr = model_str;
            model_string.wstr_len = displayWStrLen;
            GUISTR_DrawTextToLCDInRect(
                &lcd_dev_info,
                (const GUI_RECT_T *)&disp_rect,       //the fixed display area
                (const GUI_RECT_T *)&disp_rect,       
                (const MMI_STRING_T *)&model_string,
                &text_style,
                GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
                GUISTR_TEXT_DIR_AUTO);
             SCI_FREE( model_str );
            break;
        }
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        {
            Settings_About_APP_CANCEL( win_id );
            break;
        }
            
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
    default:
        reCode = MMI_RESULT_FALSE;
        break;
    }

    return reCode;
}

//idle -> settings -> regard
WINDOW_TABLE( MMISET_CERTIFICATION_WIN_TAB) =
{    
    WIN_ID( MMISET_CERFICATIONMARK_WIN_ID ),
    WIN_HIDE_STATUS,
    WIN_FUNC( (uint32)HandleCerficationMarkWindow ),
    CREATE_TEXT_CTRL( MMISET_CERFICATION_TEXT_CTRL_ID ),
    END_WIN
};

//idle -> settings -> regard
WINDOW_TABLE( MMISET_DEVICEINFO_WIN_TAB ) =
{    
    WIN_ID( MMISET_DEVICEINFO_WIN_ID ),
    WIN_HIDE_STATUS,
    WIN_FUNC( (uint32)HandleSettingsRegardWindow ),
    CREATE_TEXT_CTRL( MMISET_ABOUT_TEXT_CTRL_ID ),
    END_WIN
};

//idle -> settings -> regard
WINDOW_TABLE( MMISET_ABOUT_WIN_TAB ) =
{    
    WIN_ID( MMISET_ABOUT_WIN_ID ),
    WIN_HIDE_STATUS,
#ifdef ZTE_WATCH
    WIN_FUNC( (uint32)HandleSettingsDeviceInfoWindow ),
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMISET_DEVICEINFO_LIST_CTRL_ID ),
#else
    WIN_FUNC( (uint32)HandleSettingsRegardWindow ),
    CREATE_TEXT_CTRL( MMISET_ABOUT_TEXT_CTRL_ID ),
    //WIN_SOFTKEY( TXT_NULL, TXT_NULL, STXT_RETURN ),
#endif
    END_WIN
};
PUBLIC void Settings_CerficationMark_Enter( void )
{
    
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
	if (MMK_IsOpenWin(MMISET_CERFICATIONMARK_WIN_ID))
    {
        MMK_CloseWin(MMISET_CERFICATIONMARK_WIN_ID);
    }
	win_handle = MMK_CreateWin( (uint32 *)MMISET_CERTIFICATION_WIN_TAB, PNULL );
    if(win_handle != NULL)
        MMK_SetWinRect(win_handle, &rect);
}

PUBLIC void Settings_DeviceInfo_Enter( void )
{
	MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
	if (MMK_IsOpenWin(MMISET_DEVICEINFO_WIN_ID))
    {
        MMK_CloseWin(MMISET_DEVICEINFO_WIN_ID);
    }
    win_handle = MMK_CreateWin( (uint32 *)MMISET_DEVICEINFO_WIN_TAB, PNULL );

    if(win_handle != NULL)
        MMK_SetWinRect(win_handle, &rect);
}

PUBLIC void Settings_RegardWin_Enter( void )
{
    MMI_HANDLE_T win_handle = 0;
    GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
    if (MMK_IsOpenWin(MMISET_ABOUT_WIN_ID))
    {
        MMK_CloseWin(MMISET_ABOUT_WIN_ID);
    }
    win_handle = MMK_CreateWin( (uint32 *)MMISET_ABOUT_WIN_TAB, PNULL );
    if(win_handle != NULL)
        MMK_SetWinRect(win_handle, &rect);
}


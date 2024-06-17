/****************************************************************************
** File Name:      mmiphsapp_wintab.c                                           *
** Author:          jianshengqi                                                       *
** Date:           03/22/2006                                              *
** Copyright:      2006 TLT, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the PHS                   *
/****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2006       Jianshq         Create
** 
****************************************************************************/
#ifndef _MMIZDT_WINTAB_C_
#define _MMIZDT_WINTAB_C_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmicc_export.h"
#include "guitext.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "guilistbox.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "mmi_common.h"

#include "zdt_app.h"
#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "img_dec_interface.h"
#include "guiownerdraw.h"
#include "graphics_draw.h"
#include "mmiparse_export.h"
#include "mmicl_export.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef ENG_SUPPORT
#include "mmieng_export.h"
#endif
#include "mmiset_export.h"
#include "mmiset_id.h"
#include "watch_launcher_main.h"
#include "watch_commonwin_export.h"
#ifdef SETUP_WIZARD_SUPPORT
#include "setup_wizard.h"
#endif
#include "watch_charge_win.h"
#include "watch_common.h"


#ifdef XYSDK_SUPPORT
#include "libxmly_api.h"
#endif

#ifdef ZDT_VIDEOCHAT_SUPPORT
#include "video_call.h"
#endif

#define SCALE  1

#define z_abs(x)  ((x) >= 0 ? (x) : (-(x)))

#ifdef TOUCH_PANEL_SUPPORT
#define MAINMENU_DIAL_PANLE_SUPPORT 1
#else
#define MAINMENU_DIAL_PANLE_SUPPORT 0
#endif

uint8 g_zdt_win__disp_timer_id = 0;
static BOOLEAN s_is_sos_call_send = FALSE;
static uint8 s_sos_call_index = 0;
static BOOLEAN s_is_sos_calling = FALSE;

#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
extern uint8 g_cur_mesage_index;
#endif

PUBLIC BOOLEAN MMICC_IsInActiveCallWin()
{
    BOOLEAN ret = FALSE;
    return ret;
}

static void MMIZDT_BackLight(BOOLEAN is_alway_on)
{
      if(is_alway_on)
      {
		MMIDEFAULT_SetAlwaysHalfOnBackLight(FALSE);
		MMIDEFAULT_AllowTurnOffBackLight(FALSE);
		MMIDEFAULT_TurnOnBackLight();
      }
      else
      {
		MMIDEFAULT_AllowTurnOffBackLight(TRUE);
		MMIDEFAULT_SetAlwaysHalfOnBackLight(FALSE);
      }
}

void MMIZDT_Display_TimeStart(uint32 time_ms,MMI_TIMER_FUNC  func)
{
    if(0 != g_zdt_win__disp_timer_id)
    {
        MMK_StopTimer(g_zdt_win__disp_timer_id);
        g_zdt_win__disp_timer_id = 0;
    }
    
    g_zdt_win__disp_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        func, 
                                                                        (uint32)PNULL, 
                                                                        FALSE);
}

void MMIZDT_Display_TimeStop(void)
{
    if(0 != g_zdt_win__disp_timer_id)
    {
        MMK_StopTimer(g_zdt_win__disp_timer_id);
        g_zdt_win__disp_timer_id = 0;
    }
}

//static BOOLEAN MMIZDT_MakeAll(uint8 *tele_num, uint8 tele_len)
PUBLIC BOOLEAN MMIZDT_MakeAll(uint8 *tele_num, uint8 tele_len, wchar *name)
{
	uint8				temp_str[100 + 1]  = {0};
	CC_CALL_SIM_TYPE_E	sim_type = CC_SIM1_CALL;
	MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
      uint16 name_wstr[100] = {0};
      uint16 name_len = 0;
      //MMI_STRING_T* name_ptr = PNULL;
      MMI_STRING_T name_ptr = {0};
      name_ptr.wstr_ptr = name;
      if(name != NULL)
      {
          name_ptr.wstr_len = (uint16) MMIAPICOM_Wstrlen (name);
      }
	if(MMIAPIPHONE_IsSimAvailable(MN_DUAL_SYS_1))
	 {
		dual_sys = MN_DUAL_SYS_1;
	 }
#ifndef MMI_MULTI_SIM_SYS_SINGLE
	 else if(MMIAPIPHONE_IsSimAvailable(MN_DUAL_SYS_2))
	 {
		dual_sys = MN_DUAL_SYS_2;
	 }
 #endif
	else
	{
		if(strcmp(tele_num, "112") != 0)
		{
			MMIAPIPHONE_AlertSimNorOKStatus();
			return;
		}
	}
    
	if (PNULL == tele_num || tele_len == 0)
	{
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
            MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
#endif
		return FALSE;
	}
	else
	{
              if(name == NULL)
              {
                    if(MMICC_GetYxDbRecCallName(tele_num, name_wstr, &name_len))
                    {

                        name_ptr.wstr_ptr = name_wstr;
                        name_ptr.wstr_len = name_len;
                    }
              }
		//convert string
		sim_type = MMIAPICC_GetCallSimType(dual_sys);
		
		MMIAPICC_MakeCall(dual_sys,
				tele_num,
				tele_len,
				PNULL,
				&name_ptr,//name_ptr,
				sim_type,
				CC_CALL_NORMAL_CALL,
				PNULL);
	}
}

BOOLEAN MMIZDT_Call_110(void)
{    
    MMIZDT_MakeAll("110",3,PNULL);
    return TRUE;
}

BOOLEAN MMIZDT_Call_112(void)
{    
    MMIZDT_MakeAll("112",3,PNULL);
    return TRUE;
}

BOOLEAN MMIZDT_Call_119(void)
{    
    MMIZDT_MakeAll("119",3,PNULL);
    return TRUE;
}

BOOLEAN MMIZDT_Call_120(void)
{    
    MMIZDT_MakeAll("120",3,PNULL);
    return TRUE;
}


BOOLEAN MMIZDT_Call_SPD1(void)
{    
#if defined(ZDT_PLAT_YX_SUPPORT_YS) || defined(ZDT_PLAT_YX_SUPPORT_FZD) || defined(ZDT_PLAT_YX_SUPPORT_TX)
    if(yx_DB_Sos_Reclist[0].db_id == 0 || SCI_STRLEN(yx_DB_Sos_Reclist[0].sos_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD1 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(yx_DB_Sos_Reclist[0].sos_num,SCI_STRLEN(yx_DB_Sos_Reclist[0].sos_num),PNULL);
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
    if(YX_API_MoCallLimit_IsValid())
    {
        ZDT_LOG("MMIZDT_Call_SPD1 MoCallLimit");
        return FALSE;
    }
    if(yx_DB_Spd_Reclist[0].db_id == 0 || SCI_STRLEN(yx_DB_Spd_Reclist[0].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD1 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(yx_DB_Spd_Reclist[0].spd_num,SCI_STRLEN(yx_DB_Spd_Reclist[0].spd_num),PNULL);
#endif

#ifdef ZDT_PLAT_SHB_SUPPORT
    if(SHB_DB_TIMER_IsValid_Slient())
    {
        ZDT_LOG("MMIZDT_Call_SPD1 InSlient");
        return FALSE;
    }
    if(shb_DB_Spd_Reclist[0].db_id == 0 || SCI_STRLEN(shb_DB_Spd_Reclist[0].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD1 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(shb_DB_Spd_Reclist[0].spd_num,SCI_STRLEN(shb_DB_Spd_Reclist[0].spd_num),PNULL);
#endif
    return TRUE;
}

BOOLEAN MMIZDT_Call_SPD2(void)
{
#if defined(ZDT_PLAT_YX_SUPPORT_YS) || defined(ZDT_PLAT_YX_SUPPORT_FZD) || defined(ZDT_PLAT_YX_SUPPORT_TX)
    if(yx_DB_Sos_Reclist[1].db_id == 0 || SCI_STRLEN(yx_DB_Sos_Reclist[1].sos_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD2 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(yx_DB_Sos_Reclist[1].sos_num,SCI_STRLEN(yx_DB_Sos_Reclist[1].sos_num),PNULL);
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
    if(YX_API_MoCallLimit_IsValid())
    {
        ZDT_LOG("MMIZDT_Call_SPD2 MoCallLimit");
        return FALSE;
    }
    if(yx_DB_Spd_Reclist[1].db_id == 0 || SCI_STRLEN(yx_DB_Spd_Reclist[1].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD2 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(yx_DB_Spd_Reclist[1].spd_num,SCI_STRLEN(yx_DB_Spd_Reclist[1].spd_num),PNULL);
#endif

#ifdef ZDT_PLAT_SHB_SUPPORT
    if(SHB_DB_TIMER_IsValid_Slient())
    {
        ZDT_LOG("MMIZDT_Call_SPD2 InSlient");
        return FALSE;
    }
    if(shb_DB_Spd_Reclist[1].db_id == 0 || SCI_STRLEN(shb_DB_Spd_Reclist[1].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD2 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(shb_DB_Spd_Reclist[1].spd_num,SCI_STRLEN(shb_DB_Spd_Reclist[1].spd_num),PNULL);
#endif
    return TRUE;
}

BOOLEAN MMIZDT_Call_SPD3(void)
{
#if defined(ZDT_PLAT_YX_SUPPORT_YS) || defined(ZDT_PLAT_YX_SUPPORT_FZD) || defined(ZDT_PLAT_YX_SUPPORT_TX)
    if(yx_DB_Sos_Reclist[2].db_id == 0 || SCI_STRLEN(yx_DB_Sos_Reclist[2].sos_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD3 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(yx_DB_Sos_Reclist[2].sos_num,SCI_STRLEN(yx_DB_Sos_Reclist[2].sos_num),PNULL);
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
    if(YX_API_MoCallLimit_IsValid())
    {
        ZDT_LOG("MMIZDT_Call_SPD3 MoCallLimit");
        return FALSE;
    }
    if(yx_DB_Spd_Reclist[2].db_id == 0 || SCI_STRLEN(yx_DB_Spd_Reclist[2].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD3 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
 #endif
       return FALSE;
    }
    MMIZDT_MakeAll(yx_DB_Spd_Reclist[2].spd_num,SCI_STRLEN(yx_DB_Spd_Reclist[2].spd_num),PNULL);
#endif

#ifdef ZDT_PLAT_SHB_SUPPORT
    if(SHB_DB_TIMER_IsValid_Slient())
    {
        ZDT_LOG("MMIZDT_Call_SPD3 InSlient");
        return FALSE;
    }
    if(shb_DB_Spd_Reclist[2].db_id == 0 || SCI_STRLEN(shb_DB_Spd_Reclist[2].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD3 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(shb_DB_Spd_Reclist[2].spd_num,SCI_STRLEN(shb_DB_Spd_Reclist[2].spd_num),PNULL);
#endif
    return TRUE;
}

BOOLEAN MMIZDT_Call_SPD4(void)
{
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
    if(YX_API_MoCallLimit_IsValid())
    {
        ZDT_LOG("MMIZDT_Call_SPD4 MoCallLimit");
        return FALSE;
    }
    if(yx_DB_Spd_Reclist[3].db_id == 0 || SCI_STRLEN(yx_DB_Spd_Reclist[3].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD4 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#else
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(yx_DB_Spd_Reclist[3].spd_num,SCI_STRLEN(yx_DB_Spd_Reclist[3].spd_num),PNULL);
#endif

#ifdef ZDT_PLAT_SHB_SUPPORT
    if(SHB_DB_TIMER_IsValid_Slient())
    {
        ZDT_LOG("MMIZDT_Call_SPD4 InSlient");
        return FALSE;
    }
    if(shb_DB_Spd_Reclist[3].db_id == 0 || SCI_STRLEN(shb_DB_Spd_Reclist[3].spd_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SPD4 No Num");
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NUMBER,1);
#endif
#endif
        return FALSE;
    }
    MMIZDT_MakeAll(shb_DB_Spd_Reclist[3].spd_num,SCI_STRLEN(shb_DB_Spd_Reclist[3].spd_num),PNULL);
#endif
    return TRUE;
}
BOOLEAN MMIZDT_Call_SOS(void)
{    
    uint8 sos_number_index = s_sos_call_index/YX_SOS_CALL_RETRY;
#if defined(ZDT_PLAT_YX_SUPPORT_YS) || defined(ZDT_PLAT_YX_SUPPORT_FZD) || defined(ZDT_PLAT_YX_SUPPORT_TX)
    //每个号码拨三轮
    if(yx_DB_Sos_Reclist[sos_number_index].db_id == 0 || SCI_STRLEN(yx_DB_Sos_Reclist[sos_number_index].sos_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SOS No Num s_sos_call_index:%d",s_sos_call_index);
        if(s_sos_call_index == 0)
        {
            MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
        }
        return FALSE;
    }
    s_is_sos_call_send = TRUE;
#ifdef ZDT_LED_SUPPORT
    ZDT_Led_Start_Call_Incoming();
#endif
    //每个号码拨YX_SOS_CALL_RETRY轮
    MMIZDT_MakeAll(yx_DB_Sos_Reclist[sos_number_index].sos_num,SCI_STRLEN(yx_DB_Sos_Reclist[sos_number_index].sos_num),PNULL);
    s_is_sos_calling = TRUE;
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
    if(yx_DB_Sos_Reclist[0].db_id == 0 || SCI_STRLEN(yx_DB_Sos_Reclist[0].sos_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SOS No Num");
        MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
        return FALSE;
    }
    s_is_sos_call_send = TRUE;
#ifdef ZDT_LED_SUPPORT
    ZDT_Led_Start_Call_Incoming();
#endif
    MMIZDT_MakeAll(yx_DB_Sos_Reclist[0].sos_num,SCI_STRLEN(yx_DB_Sos_Reclist[0].sos_num),PNULL);
#endif

#ifdef ZDT_PLAT_SHB_SUPPORT
    if(shb_DB_Sos_Reclist[0].db_id == 0 || SCI_STRLEN(shb_DB_Sos_Reclist[0].sos_num) == 0)
    {
        ZDT_LOG("MMIZDT_Call_SOS No Num");
        return FALSE;
    }
    s_is_sos_call_send = TRUE;
#ifdef ZDT_LED_SUPPORT
    ZDT_Led_Start_Call_Incoming();
#endif
    MMIZDT_MakeAll(shb_DB_Sos_Reclist[0].sos_num,SCI_STRLEN(shb_DB_Sos_Reclist[0].sos_num),PNULL);
#endif
    return TRUE;
}


BOOLEAN MMIZDT_Call_SOS_Over(void)
{    
    ZDT_LOG("[%s] s_is_sos_calling:%d s_sos_call_index:%d s_is_sos_call_send:%d",__FUNCTION__,s_is_sos_calling,s_sos_call_index,s_is_sos_call_send);
    if(s_is_sos_call_send)
    {
       s_is_sos_call_send = FALSE;
#ifdef ZDT_PLAT_YX_SUPPORT
        YX_API_SOS_PostAfter();
#endif

#ifdef ZDT_SMS_SUPPORT
        YX_API_SMS_Send_Admin_SOS();
#endif

#ifdef ZDT_PLAT_SHB_SUPPORT
        SHB_HTTP_SOS_Send();
#else
#ifdef ZDT_LED_SUPPORT
        ZDT_Led_Start_Call_Incoming_Over();
#endif
#endif
    }
    s_is_sos_calling = FALSE;
    return TRUE;
}

LOCAL void ZDT_SOS_RetryCall(uint8 timer_id, uint32 param)
{
    s_sos_call_index ++;
    if( timer_id != 0)
    {
        MMK_StopTimer(timer_id);
    }
    ZDT_LOG("[%s] s_is_sos_calling:%d s_sos_call_index:%d",__FUNCTION__,s_is_sos_calling,s_sos_call_index);
    if( s_sos_call_index == YX_SOS_CALL_RETRY*YX_DB_SOS_MAX_SUM)//每个号码拨YX_SOS_CALL_RETRY轮
    {
        MMIZDT_Call_SOS_Over();
        s_sos_call_index = 0;
    }
    else
    {
        if(!MMIZDT_Call_SOS())
        {
            MMIZDT_Call_SOS_Over();
            s_sos_call_index = 0;
        }
    }
}

LOCAL void ZDT_Start_SOS_Retry_Timer()
{
    MMK_CreateTimerCallback(3000,ZDT_SOS_RetryCall, NULL, FALSE);
}

PUBLIC void ZDT_SOS_Call_Fail()
{
    ZDT_LOG("[%s] s_is_sos_calling:%d s_sos_call_index:%d",__FUNCTION__,s_is_sos_calling,s_sos_call_index);
    if(s_is_sos_calling)
    {
        s_is_sos_calling = FALSE;
        ZDT_Start_SOS_Retry_Timer();
    }
}

PUBLIC BOOLEAN ZDT_SOS_Calling()
{
    ZDT_LOG("[%s] s_is_sos_calling:%d",__FUNCTION__,s_is_sos_calling);
    return s_is_sos_calling;
}

PUBLIC void ZDT_SOS_Call_Success()
{
    ZDT_LOG("[%s] s_is_sos_calling:%d",__FUNCTION__,s_is_sos_calling);
    if(s_is_sos_calling)
    {
        s_sos_call_index = 0;
        s_is_sos_calling = FALSE;
        MMIZDT_Call_SOS_Over();
    }
}

#ifdef ZTE_WATCH

LOCAL MMI_RESULT_E HandleZDT_SosMainWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id,DPARAM param)
{
    MMI_RESULT_E				recode			=	MMI_RESULT_TRUE;
    GUI_RECT_T		bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUI_POINT_T sos_icon_point = {SOS_ICON_START_X,SOS_ICON_START_Y};
    GUI_RECT_T sos_call_rect = SOS_CALL_BG_RECT;
    GUI_POINT_T sos_call_point = {0};
    MMI_STRING_T		str_data = {0};
    MMI_STRING_T		quit_str_data = {0};
    GUI_RECT_T	sos_tip_rect= SOS_TIP_RECT;
    GUISTR_STYLE_T		text_style = {0};
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
    text_style.font_color = MMI_RED_COLOR;
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
            break;
            
        case MSG_GET_FOCUS:
            break;
            
        case MSG_FULL_PAINT:
        {
            LCD_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
            GUIRES_DisplayImg(&sos_icon_point,PNULL,PNULL,win_id,IMAGE_SOS_ICON,&lcd_dev_info);
            sos_call_point.x = sos_call_rect.left;
            sos_call_point.y = sos_call_rect.top;
            GUIRES_DisplayImg(&sos_call_point,PNULL,PNULL,win_id,IMAGE_SOS_CALL_ICON,&lcd_dev_info);
            MMI_GetLabelTextByLang(TXT_SOS_TIP, &str_data);
            GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&sos_tip_rect,&sos_tip_rect,&str_data,&text_style,ALIGN_HVMIDDLE,GUISTR_TEXT_DIR_AUTO);
            MMI_GetLabelTextByLang(TXT_SOS_CALL, &quit_str_data);
            text_style.font = SONG_FONT_26;
            text_style.font_color = MMI_WHITE_COLOR;
            GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&sos_call_rect,&sos_call_rect,&quit_str_data,&text_style,ALIGN_HVMIDDLE,GUISTR_TEXT_DIR_AUTO);
        }
        break;
		
	    case MSG_TP_PRESS_DOWN:
		    break;
		
	    case MSG_TP_PRESS_UP:
		    {
			    GUI_POINT_T   point = {0};
			    point.x = MMK_GET_TP_X(param);
			    point.y = MMK_GET_TP_Y(param);
                if(GUI_PointIsInRect(point,sos_call_rect))
                {
                    if(yx_DB_Sos_Reclist[0].db_id == 0 || SCI_STRLEN(yx_DB_Sos_Reclist[0].sos_num) == 0)
                    {
                        MMI_WIN_ID_T    winId   = WATCH_SOS_CALL_TOAST_WIN_ID;
                        MMI_STRING_T    noteStr = {0};
                        WATCH_SOFTKEY_TEXT_ID_T softKey = {0};
                        MMI_GetLabelTextByLang(TXT_SOS_EMPTY_TIP, &noteStr);
                        WatchCOM_NoteWin_1Line_Enter(winId, &noteStr, NULL, softKey, NULL);
                    }
                    else
                    {
                        if(MMIZDT_CheckSimStatus())
                        {
                            MMIZDT_Call_SOS();
                        }
                    }
                }
		    }
		    break;
	    case MSG_CTL_CANCEL:
	    case MSG_APP_CANCEL:
		    MMK_CloseWin(win_id);
		    break;
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

WINDOW_TABLE( MMIZDT_SOS_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_SosMainWinMsg),    
    WIN_ID( MMI_SOS_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

BOOLEAN MMIZDT_OpenSosWin(void)
{
    if(MMK_IsOpenWin(MMI_SOS_WIN_ID) )
    {
       MMK_CloseWin(MMI_SOS_WIN_ID);
    }
    MMK_CreateWin((uint32*)MMIZDT_SOS_WIN_TAB,PNULL);
}

#endif



#if 1 // Log 测试窗口
#define  ZDT_TEST_MENU_LEN  20
#define  ZDT_TEST_MENU_NUM  5
const uint8 zdt_test_menu_text[ZDT_TEST_MENU_NUM][ZDT_TEST_MENU_LEN]= 
{
    "Trace Del",
    "Trace Save",
    "Log Enable",
    "Log Disable",
    "SCI_ASSERT"

};

LOCAL MMI_RESULT_E HandleZDT_TestMainMenuWinMsg (
                                      MMI_WIN_ID_T   win_id, 	// 窗口的ID
                                      MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                      DPARAM            param		// 相应消息的参数
                                      );

WINDOW_TABLE( MMIZDT_TEST_MENU_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_TestMainMenuWinMsg),    
    WIN_ID( MMIZDT_TEST_MENU_WIN_ID ),
    WIN_TITLE(TXT_ZDT_TEST),
    //WIN_STATUSBAR,
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMIZDT_TEST_MENU_CTRL_ID),
    END_WIN
};

BOOLEAN MMIZDT_OpenTestMenuWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_TEST_MENU_WIN_TAB,PNULL);
}

/*****************************************************************************/
// 	Description : to handle the message of atcmd test 
//	Global resource dependence : 
//  Author:jianshengqi
//	Note: 2006/3/19 
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleZDT_TestMainMenuWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T            ctrl_id = MMIZDT_TEST_MENU_CTRL_ID;
    uint16                    node_id = 0;
    static BOOLEAN            s_need_update   = FALSE;
  //  MMISET_VALIDATE_PRIVACY_RETURN_T    result_info = {MMISET_PROTECT_CL, FALSE};    
#if defined(DISABLE_STATUSBAR_DISPLAY) 
    GUI_RECT_T                          title_rect =   {0, 0, (MMI_MAINSCREEN_WIDTH -1), 23};
#else
    GUI_RECT_T                          title_rect = {0, 18, (MMI_MAINSCREEN_WIDTH -1), 18 + 23};
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

            
            if (MMK_IsIncludeStatusBar(win_id))
            {
                GUIWIN_SetTitleRect(win_id, title_rect);   
            }  
            
            GUILIST_SetMaxItem(ctrl_id, ZDT_TEST_MENU_NUM,FALSE);   
            
            for ( i = 0; i < ZDT_TEST_MENU_NUM; i++ )
            {
                item_t.item_style    = GUIITEM_STYLE_ONE_LINE_ICON_TEXT;
                item_t.item_data_ptr = &item_data;
                copy_len = SCI_STRLEN(zdt_test_menu_text[i]);
                item_data.item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
                item_data.item_content[0].item_data.image_id = IMAGE_SIM1;
                item_data.item_content[1].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
                item_data.item_content[1].item_data.text_buffer.wstr_ptr = buff;
                GUI_GB2UCS(item_data.item_content[1].item_data.text_buffer.wstr_ptr, zdt_test_menu_text[i],copy_len );
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
#ifndef WIN32
            case  1:
                    #ifdef FILE_LOG_SUPPORT
                        MMK_PostMsg(VIRTUAL_WIN_ID, MSG_USRE_LOG_DEL_ALL, 0,0);
                    #endif
                    MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SUCCESS,TXT_NULL,IMAGE_PUBWIN_SUCCESS,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
                break;
                
            case  2:
                    #ifdef FILE_LOG_SUPPORT
                        MMK_PostMsg(VIRTUAL_WIN_ID, MSG_USRE_LOG_SAVE, 0,0);
                        MMK_PostMsg(VIRTUAL_WIN_ID, MSG_USRE_LOG_AT_SAVE, 0,0);
                        MMK_PostMsg(VIRTUAL_WIN_ID, MSG_USRE_LOG_BUF_SAVE, 0,0);
                        MMK_PostMsg(VIRTUAL_WIN_ID, MSG_USRE_LOG_SOCKET_SAVE, 0,0);
                    #endif
                    MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SUCCESS,TXT_NULL,IMAGE_PUBWIN_SUCCESS,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
                break;
                
            case  3:
                {
                    REFPARAM_SetEnableArmLogFlag(TRUE);
                    #ifdef FILE_LOG_SUPPORT
                        TraceUser_Init();
                    #endif
                    MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SUCCESS,TXT_NULL,IMAGE_PUBWIN_SUCCESS,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
                }
                break;
                
            case  4:
                    REFPARAM_SetEnableArmLogFlag(FALSE);
                    MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SUCCESS,TXT_NULL,IMAGE_PUBWIN_SUCCESS,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
                break;
                                
            case  5:
                    SCI_ASSERT(0);
                break;
#endif
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
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}
#endif

#if 1  //计步窗口
static int s_jp_tp_x;
static int s_jp_tp_y;
static BOOLEAN s_is_jp_tp_down = FALSE;
static uint8 step_timer_id = 0;
 
LOCAL void MMIZDT_StartStepTimer()
{
    if(step_timer_id != 0)
    {
        MMK_StopTimer(step_timer_id);
    }
    step_timer_id =  MMK_CreateWinTimer(MMIZDT_WATCH_JP_WIN_ID, 800,TRUE);

}

LOCAL void MMIZDT_StopStepTimer()
{
    if(step_timer_id != 0)
    {
        MMK_StopTimer(step_timer_id);
    }
}

LOCAL MMI_RESULT_E  HandleZDT_JPWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T        str_data = {0};
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUISTR_INFO_T       text_info = {0};
    wchar  pedo_wstr[16]        = {0};
    uint8 pedo_str[16] = {0};
    uint16  pedo_len=0;
    MMI_STRING_T pedo_string = {0};
	
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
	        MMIZDT_StartStepTimer();
            break;
			
	case MSG_FULL_PAINT:				
            GUIRES_DisplayImg(PNULL,
                &bg_rect,
                PNULL,
                win_id,
                IMAGE_ZDT_JP_BG,
                &lcd_dev_info);
			
		MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_ZDT_CUR_PEDO, &str_data);  
			
	        text_style.align = ALIGN_HVMIDDLE; // 这里设置为Left,因为Label的位置自有自己调整区域
	        text_style.font_color = MMI_GREEN_COLOR;
	        text_style.angle = ANGLE_0;
	        text_style.char_space = 0;
	        text_style.effect = FONT_EFFECT_CUSTOM;
	        text_style.edge_color = 0;

#if defined(ZDT_MMI_USE_SMALL_FONT)	  
              text_style.font = SONG_FONT_22;//SONG_FONT_26;
#else
	        text_style.font = SONG_FONT_28;//SONG_FONT_26;
#endif	        
	        text_style.line_space = 0;
	        text_style.region_num = 0;
	        text_style.region_ptr = PNULL;		
	        text_display_rect.left = 0;
	        text_display_rect.top = 40*SCALE;
	        text_display_rect.right=MMI_MAINSCREEN_WIDTH;
	        text_display_rect.bottom=100*SCALE;	
	        GUISTR_GetStringInfo(&text_style, &str_data, state, &text_info);
            #if 0
    		GUISTR_DrawTextToLCDInRect( 
    		        MMITHEME_GetDefaultLcdDev(),
    		        (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
    		        (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
    		        (const MMI_STRING_T    *)&str_data,
    		        &text_style,
    		        state,
    		        GUISTR_TEXT_DIR_AUTO
    		        );	
	        #endif
#ifdef WIN32
		sprintf((char*) pedo_str, "2800");
#else
#ifdef ZDT_GSENSOR_SUPPORT
		sprintf((char*) pedo_str, "%d", ZDT_GSensor_GetStepOneDay());
#else
		sprintf((char*) pedo_str, "2800");
#endif
#endif
		pedo_len = strlen((char *)pedo_str);
		MMI_STRNTOWSTR( pedo_wstr,16, pedo_str, 16, pedo_len );
		pedo_string.wstr_ptr = pedo_wstr;
            	pedo_string.wstr_len = MMIAPICOM_Wstrlen(pedo_wstr);
				
		GUISTR_GetStringInfo(&text_style, &pedo_string, state, &text_info);
	        text_display_rect.left = 0;
	        text_display_rect.top = (100 + 40)*SCALE;
	        text_display_rect.right= MMI_MAINSCREEN_WIDTH;
	        text_display_rect.bottom=(160 + 40)*SCALE;		
		GUISTR_DrawTextToLCDInRect( 
		        MMITHEME_GetDefaultLcdDev(),
		        (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
		        (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
		        (const MMI_STRING_T    *)&pedo_string,
		        &text_style,
		        state,
		        GUISTR_TEXT_DIR_AUTO
		        );		
		break;
		
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            s_jp_tp_x = point.x;
            s_jp_tp_y = point.y;
            s_is_jp_tp_down = TRUE;
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(s_is_jp_tp_down)
            {
                offset_y =  point.y - s_jp_tp_y;
                offset_x =  point.x - s_jp_tp_x;
                if(offset_x <= -(20))	
                {
                    MMK_CloseWin(win_id);
                }	
                else if(offset_x >= (20))
                {
                    MMK_CloseWin(win_id);
                }

            }
            s_is_jp_tp_down = FALSE;
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

        //case MSG_APP_OK:
        //case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            break;
        case MSG_TIMER:
        {
            if (*(uint8*)param == step_timer_id)
            {
                MMK_SendMsg(MMK_ConvertIdToHandle(win_id), MSG_FULL_PAINT, PNULL);
            }
        }
        
        break;
        case MSG_LOSE_FOCUS:
        { 
            MMIZDT_StopStepTimer();
        }
        
        break;

        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            break;
        case MSG_KEYDOWN_RED:
            break;
        
        case MSG_KEYUP_RED:
            MMK_CloseWin(win_id);
            break;

        case MSG_CLOSE_WINDOW:
            MMIZDT_StopStepTimer();
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}

WINDOW_TABLE( MMIZDT_WATCH_JP_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_JPWinMsg),    
    WIN_ID( MMIZDT_WATCH_JP_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZDT_OpenPedometerWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_WATCH_JP_WIN_TAB,PNULL);
}

BOOLEAN MMIZDT_ClosePedometerWin(void)
{
    if(MMK_IsOpenWin(MMIZDT_WATCH_JP_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WATCH_JP_WIN_ID);
    }
    return TRUE;
}

BOOLEAN MMIZDT_UpdatePedometerWin(void)
{
    if(MMK_IsFocusWin(MMIZDT_WATCH_JP_WIN_ID))
    {
        MMK_PostMsg(MMIZDT_WATCH_JP_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
    return TRUE;
}

#endif

// 心率接口
#if defined(ZDT_HSENSOR_SUPPORT) && !defined(ZDT_HSENSOR_SUPPORT_TW)
static int s_hr_tp_x;
static int s_hr_tp_y;
static BOOLEAN s_is_hr_tp_down = FALSE;
#ifndef WIN32
extern BOOLEAN ZDT_HSensor_IsOpen();
#endif


LOCAL uint32 hr_win_open_time = 0;
LOCAL uint32 hr_has_value_time = 0;
LOCAL BOOLEAN hr_is_open = FALSE;


LOCAL uint8 hr_anim_index = 0 ;

void MMIZDT_HR_HandleTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
    if(MMK_IsFocusWin(MMIZDT_WATCH_HR_WIN_ID))
    {
        hr_anim_index++;
        if(hr_anim_index > 4/*6*/)
        {
            hr_anim_index = 0;
        }
        MMK_PostMsg(MMIZDT_WATCH_HR_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        MMIZDT_Display_TimerStart(400, MMIZDT_HR_HandleTimer);
    }
}

LOCAL BOOLEAN IsPointInRect(GUI_POINT_T point, GUI_RECT_T rect)
{
    BOOLEAN ret = FALSE;
    if(point.x >= rect.left && point.x <= rect.right && point.y >= rect.top && point.y <= rect.bottom)
    {
        ret = TRUE;
    }
    return ret;
}

LOCAL MMI_RESULT_E  HandleZDT_HRWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T        str_data = {0};
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUISTR_INFO_T       text_info = {0};
    wchar  pedo_wstr[50]        = {0};
    uint8 pedo_str[50] = {0};
    uint16 hr = 0;
    MMI_STRING_T hr_string = {0};
    GUI_RECT_T btn_rect = {60, 190, 60+120, 190+40};
    GUI_RECT_T hr_rect = {60, 150, 60+120, 150+30};
    GUI_RECT_T tip_rect = {60, 190, 60+120, 190+40};

	
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            //MMK_SetAtvCtrl(win_id,ctrl_id);
            //GUIRICHTEXT_SetFocusMode(ctrl_id,GUIRICHTEXT_FOCUS_SHIFT_ADVANCED);			
	    //GUICTRL_SetProgress(MMK_GetCtrlPtr(ctrl_id),FALSE);//hide scroll bar
            //ZDT_HSensor_Open();
            #ifndef WIN32
            hr_is_open = ZDT_HSensor_IsOpen();
            #endif
            MMIZDT_BackLight(TRUE);
            MMIZDT_Display_TimerStop();
            //MMIZDT_Display_TimerStart(400, MMIZDT_HR_HandleTimer);
            hr_anim_index = 0;
            hr_win_open_time = MMIAPICOM_GetCurTime();
            hr_has_value_time = 0;
            break;
			
	case MSG_FULL_PAINT:
	{
	    uint32 cur_time = MMIAPICOM_GetCurTime();
	    const uint16 img_top = 20;
	    const uint16 img_height = 112;//90;
	    
	    wchar wstr_start[] = {0x5F00, 0x59CB ,0};
	    wchar wstr_stop[] = {0x505C, 0x6B62 ,0};
	    GUI_POINT_T point = {(MMI_MAINSCREEN_WIDTH-img_height)/2, img_top};
	    MMI_IMAGE_ID_T img_id_list[5] = 
        {
            //IMAGE_HEARTRATE_ANIM_0,
            IMAGE_HEARTRATE_ANIM_1,
            IMAGE_HEARTRATE_ANIM_2,
            IMAGE_HEARTRATE_ANIM_3,
            IMAGE_HEARTRATE_ANIM_4,
            IMAGE_HEARTRATE_ANIM_5
            //IMAGE_HEARTRATE_ANIM_6
        };
        
	    #if 0
            GUIRES_DisplayImg(PNULL,
                &bg_rect,
                PNULL,
                win_id,
                IMAGE_ZDT_JP_BG,
                &lcd_dev_info);
        #endif
        GUI_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, img_id_list[hr_anim_index],&lcd_dev_info);

        
			
		MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_ZDT_MAINMENU_HR, &str_data);  
			
	        text_style.align = ALIGN_HVMIDDLE; // 这里设置为Left,因为Label的位置自有自己调整区域
	        text_style.font_color = MMI_GREEN_COLOR;
	        text_style.angle = ANGLE_0;
	        text_style.char_space = 0;
	        text_style.effect = FONT_EFFECT_CUSTOM;
	        text_style.edge_color = 0;

	        text_style.font = SONG_FONT_36;
	        text_style.line_space = 0;
	        text_style.region_num = 0;
	        text_style.region_ptr = PNULL;		
	        text_display_rect.left = 0;
	        text_display_rect.top = img_top + img_height + 10;//40;
	        text_display_rect.right=239;
	        text_display_rect.bottom=text_display_rect.top+60;//100;	
	        GUISTR_GetStringInfo(&text_style, &str_data, state, &text_info);
	        #if 0
		GUISTR_DrawTextToLCDInRect( 
		        MMITHEME_GetDefaultLcdDev(),
		        (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
		        (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
		        (const MMI_STRING_T    *)&str_data,
		        &text_style,
		        state,
		        GUISTR_TEXT_DIR_AUTO
		        );
	        #endif
            hr = ZDT_HSensor_GetHR();
            
            if( cur_time - hr_win_open_time <= 1)
            {
                hr = 0;
            }
            if(hr > 50)
            {
                if(hr_has_value_time == 0)
                {
                    hr_has_value_time = cur_time;
                }
                else if(cur_time - hr_has_value_time >= 10)
                {
                    YX_APP_T * pMe = &g_yx_app;
                    YX_Net_Send_HEART(pMe, hr);
                    MMIZDT_Display_TimerStop();
                    ZDT_HSensor_Close();  
                    hr_is_open = FALSE;
                    hr_has_value_time = 0;
                    
                        
                }
            }
            if(hr > 0)
            {
            
                sprintf((char*)pedo_str, (char*) " %d",hr);
            }
            else
            {
                sprintf((char*)pedo_str, (char*) "---",hr); 
            }

            hr_string.wstr_len = strlen(pedo_str);
            hr_string.wstr_ptr = pedo_wstr;
            MMI_STRNTOWSTR(hr_string.wstr_ptr, hr_string.wstr_len, (uint8*)pedo_str, hr_string.wstr_len, hr_string.wstr_len);
				
		GUISTR_GetStringInfo(&text_style, &hr_string, state, &text_info);
	        text_display_rect.left = 0;
	        text_display_rect.top = text_display_rect.bottom ;//100;
	        text_display_rect.right=239;
	        text_display_rect.bottom= 230;//160;
        
		GUISTR_DrawTextToLCDInRect( 
		        MMITHEME_GetDefaultLcdDev(),
		        (const GUI_RECT_T      *)&hr_rect,       //the fixed display area
		        (const GUI_RECT_T      *)&hr_rect,       //用户要剪切的实际区域
		        (const MMI_STRING_T    *)&hr_string,
		        &text_style,
		        state,
		        GUISTR_TEXT_DIR_AUTO
		        );
		 if(hr_is_open)
		 {
             str_data.wstr_ptr = wstr_stop;
		 }
		 else
		 {
		     str_data.wstr_ptr = wstr_start;
		 
}
		 str_data.wstr_len = MMIAPICOM_Wstrlen(str_data.wstr_ptr);
		 text_style.font_color = MMI_WHITE_COLOR;
	     LCD_FillRoundedRect(&lcd_dev_info, btn_rect, btn_rect , MMI_GREEN_COLOR); 
	     GUISTR_DrawTextToLCDInRect( 
		        MMITHEME_GetDefaultLcdDev(),
		        (const GUI_RECT_T      *)&tip_rect,       //the fixed display area
		        (const GUI_RECT_T      *)&tip_rect,       //用户要剪切的实际区域
		        (const MMI_STRING_T    *)&str_data,
		        &text_style,
		        state,
		        GUISTR_TEXT_DIR_AUTO
		        );
		 
    }
		break;
		
#if 1//def TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            s_hr_tp_x = point.x;
            s_hr_tp_y = point.y;
            s_is_hr_tp_down = TRUE;
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(s_is_hr_tp_down)
            {
                offset_y =  point.y - s_hr_tp_y;
                offset_x =  point.x - s_hr_tp_x;
                if(z_abs(offset_x) >= 20)	
                {
                    MMK_CloseWin(win_id);
                    
                }
                if(offset_x == 0 && point.x >= btn_rect.left && point.x <= btn_rect.right && point.y >= btn_rect.top )
                {
                    if(hr_is_open)
                    {
                        MMIZDT_Display_TimerStop();
                        ZDT_HSensor_Close();
                        hr_is_open = FALSE;
                        hr_has_value_time = 0;
                        MMK_SendMsg(win_id, MSG_FULL_PAINT, NULL);
                    }
                    else
                    {
                        uint32 cur_time = MMIAPICOM_GetCurTime();
                        hr_is_open = TRUE;
                        ZDT_HSensor_Open();
                        hr_win_open_time = cur_time;
                        MMIZDT_Display_TimerStart(400, MMIZDT_HR_HandleTimer);
                        MMK_SendMsg(win_id, MSG_FULL_PAINT, NULL);

                    }

                }

            }
            s_is_hr_tp_down = FALSE;
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

        //case MSG_APP_OK:
        //case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            break;
        case MSG_APP_CANCEL:
        //case MSG_KEYUP_CANCEL:
            MMK_CloseWin(win_id);
            break;
            
        case MSG_CLOSE_WINDOW:
            MMIZDT_BackLight(FALSE);
            MMIZDT_Display_TimerStop();
            ZDT_HSensor_Close();
            break;
            
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}

WINDOW_TABLE( MMIZDT_WATCH_HR_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_HRWinMsg),    
    WIN_ID( MMIZDT_WATCH_HR_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC BOOLEAN MMIZDT_OpenHRWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_WATCH_HR_WIN_TAB,PNULL);
    return TRUE;
}

BOOLEAN MMIZDT_CloseHRWin(void)
{
    if(MMK_IsOpenWin(MMIZDT_WATCH_HR_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WATCH_HR_WIN_ID);
    }
    return TRUE;
}

BOOLEAN MMIZDT_UpdateHRWin(void)
{
    if(MMK_IsFocusWin(MMIZDT_WATCH_HR_WIN_ID))
    {
        MMK_PostMsg(MMIZDT_WATCH_HR_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
    return TRUE;
}

PUBLIC BOOLEAN MMIZDT_IsInHRWin()
{
    BOOLEAN ret = MMK_IsFocusWin(MMIZDT_WATCH_HR_WIN_ID);
    return ret;
}
#endif

//体温窗口
#if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
static int s_tw_tp_x;
static int s_tw_tp_y;
static BOOLEAN s_is_tw_tp_down = FALSE;
static BOOLEAN just_open_tw = FALSE;
static BOOLEAN s_tw_hw_is_open = FALSE;

LOCAL uint8 tw_anim_index = 0 ;
LOCAL uint8 tw_anim_timer_id = 0 ;

LOCAL uint8 hr_tw_ui_timer_id = 0;
BOOLEAN  g_tw_is_win_testing =FALSE;
extern BOOLEAN g_tw_is_net_testing;

PUBLIC void MMIZDT_Display_TimerStop()
{
    if(hr_tw_ui_timer_id != 0)
    {
        MMK_StopTimer(hr_tw_ui_timer_id);
    }
}

PUBLIC void MMIZDT_Display_TimerStart(uint32 time_out, MMI_TIMER_FUNC func)
{
    if(hr_tw_ui_timer_id != 0)
    {
        MMK_StopTimer(hr_tw_ui_timer_id);
    }
    hr_tw_ui_timer_id = MMK_CreateTimerCallback(time_out, func, 0, TRUE);
}

LOCAL void MMIZDT_StartTwAnimTimer()
{
    if(tw_anim_timer_id != 0)
    {
        MMK_StopTimer(tw_anim_timer_id);
    }
    tw_anim_timer_id =  MMK_CreateWinTimer(MMIZDT_WATCH_TW_WIN_ID, 600,TRUE);
}

LOCAL void MMIZDT_StopTwAnimTimer()
{
    if(tw_anim_timer_id != 0)
    {
        MMK_StopTimer(tw_anim_timer_id);
		tw_anim_timer_id=0;
    }
}

void MMIZDT_TW_HandleTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
    if(MMK_IsFocusWin(MMIZDT_WATCH_TW_WIN_ID))
    {
        MMK_PostMsg(MMIZDT_WATCH_TW_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        MMIZDT_Display_TimerStart(2000, MMIZDT_TW_HandleTimer);
    }
}

LOCAL void DrawTwAnimation()
{
    GUI_RECT_T bg_rect = {0, 0, MMI_MAINSCREEN_WIDTH-1, 160};
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    const uint16 img_top = 16;
    const uint16 img_height = 133;//90;
    GUI_POINT_T point = {(MMI_MAINSCREEN_WIDTH-img_height)/2, img_top};
    MMI_IMAGE_ID_T img_id_list[5] = 
    {
        //IMAGE_HEARTRATE_ANIM_0,
        IMAGE_TW_ANIM_1,
        IMAGE_TW_ANIM_2,
        IMAGE_TW_ANIM_3,
        IMAGE_TW_ANIM_4,
        IMAGE_TW_ANIM_5
        //IMAGE_HEARTRATE_ANIM_6
    };
    GUI_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
    GUIRES_DisplayImg(&point,PNULL,PNULL,MMIZDT_WATCH_TW_WIN_ID, img_id_list[tw_anim_index],&lcd_dev_info);

}

void MMIZDT_TW_HW_Open(void)
{
    if(s_tw_hw_is_open == FALSE)
    {
#ifndef WIN32 
    #ifdef ZDT_HSENSOR_SUPPORT_TW
        ZDT_HSensor_TW_Open();
    #endif
    #ifdef ZDT_SUPPORT_TW_FM78102
        MMIZDT_Get_temp78102_delta();
    #endif
    #if defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
        Sensor_temperature_poweron();
    #endif
#endif
        s_tw_hw_is_open = TRUE;
    }
    return;
}

void MMIZDT_TW_HW_Close(void)
{
    if(s_tw_hw_is_open)
    {
#ifndef WIN32 
    #ifdef ZDT_HSENSOR_SUPPORT_TW
        ZDT_HSensor_TW_Close();
    #endif
    #if defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
        Sensor_temperature_poweroff();
    #endif
#endif
        s_tw_hw_is_open = FALSE;
    }
    return;
}

double MMIZDT_TW_HW_Get(double * p_real_tw)
{
    int16 m_zdt_tw = 0;
    double tw = 0;
    double real_tw = 0;
    if(s_tw_hw_is_open)
    {
#ifndef WIN32
    #ifdef ZDT_HSENSOR_SUPPORT_TW
        tw = ZDT_HSensor_GetTW();
        real_tw = ZDT_HSensor_GetTibiaoTW();
        *p_real_tw = real_tw;
    #elif defined(ZDT_SUPPORT_TW_GD609)
        m_zdt_tw = ZDT_temperature_GetTW();
        tw = m_zdt_tw /10.0;
    #elif defined(ZDT_SUPPORT_TW_FM78102)
        m_zdt_tw = ZDT_temperature_GetTW();
        tw = m_zdt_tw /100.0;
    #endif
#endif
    }
    return tw;
}

LOCAL MMI_RESULT_E  HandleZDT_TWWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T        str_data = {0};
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUISTR_INFO_T       text_info = {0};
    wchar  pedo_wstr[50]        = {0};
    uint8 pedo_str[50] = {0};
    double tw = 0;
    double real_tw = 0;
    MMI_STRING_T pedo_string = {0};
    

	
    switch(msg_id) {
        case MSG_OPEN_WINDOW:                
	          MMIZDT_StartTwAnimTimer();
	          MMIZDT_BackLight(TRUE);
                MMIZDT_TW_HW_Open();
                g_tw_is_win_testing = TRUE;
                MMIZDT_Display_TimerStop();
                MMIZDT_Display_TimerStart(2500, MMIZDT_TW_HandleTimer);
                just_open_tw = TRUE;
            break;
			
	case MSG_FULL_PAINT:
	{
	    wchar tibiao_wbuf[] = {0x4F53 , 0x8868 , 0x6E29 , 0x5EA6 , 0};
	    wchar notice_wbuf[32] = {0};
	    uint8 real_buf[8] = {0};
	    GUI_RECT_T notice_rect = {100, 220, 239, 239};
	    #if 0
            GUIRES_DisplayImg(PNULL,
                &bg_rect,
                PNULL,
                win_id,
                IMAGE_ZDT_JP_BG,
                &lcd_dev_info);
        #endif
        GUI_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
        DrawTwAnimation();
			
		MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_ZDT_CUR_TW, &str_data);  
			
	        text_style.align = ALIGN_HVMIDDLE; // 这里设置为Left,因为Label的位置自有自己调整区域
	        text_style.font_color = MMI_GREEN_COLOR;
	        text_style.angle = ANGLE_0;
	        text_style.char_space = 0;
	        text_style.effect = FONT_EFFECT_CUSTOM;
	        text_style.edge_color = 0;

	        text_style.font = SONG_FONT_26;
	        text_style.line_space = 0;
	        text_style.region_num = 0;
	        text_style.region_ptr = PNULL;		
	        text_display_rect.left = 0;
	        text_display_rect.top = 40;
	        text_display_rect.right=239;
	        text_display_rect.bottom=100;	
	        GUISTR_GetStringInfo(&text_style, &str_data, state, &text_info);
        #if 0
		GUISTR_DrawTextToLCDInRect( 
		        MMITHEME_GetDefaultLcdDev(),
		        (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
		        (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
		        (const MMI_STRING_T    *)&str_data,
		        &text_style,
		        state,
		        GUISTR_TEXT_DIR_AUTO
		        );
        #endif
        


           
		if(just_open_tw)
		{
		    uint16 testing_wstr[7] = {0x6D4B, 0x91CF , 0x4E2D , 0x002E , 0x002E , 0x002E, 0 };
		    just_open_tw = FALSE;
		    pedo_string.wstr_len = MMIAPICOM_Wstrlen(testing_wstr);
		    pedo_string.wstr_ptr = testing_wstr;
		}
		else
		{
                    tw = MMIZDT_TW_HW_Get(&real_tw);
                    #ifdef WIN32
                        tw = 35.5;
                    #endif
                    pedo_string.wstr_len = sprintf((char*)pedo_str, (char*) "%.1lf",tw);
                    pedo_string.wstr_ptr = pedo_wstr;
                    MMI_STRNTOWSTR(pedo_string.wstr_ptr, pedo_string.wstr_len, (uint8*)pedo_str, pedo_string.wstr_len, pedo_string.wstr_len);
		}
        
		GUISTR_GetStringInfo(&text_style, &pedo_string, state, &text_info);
	        text_display_rect.left = 0;
	        text_display_rect.top = 160;
	        text_display_rect.right= 239;
	        text_display_rect.bottom= 240;
	        text_style.font = SONG_FONT_30;
	        //text_style.font_color = MMI_WHITE_COLOR;
		GUISTR_DrawTextToLCDInRect( 
		        MMITHEME_GetDefaultLcdDev(),
		        (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
		        (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
		        (const MMI_STRING_T    *)&pedo_string,
		        &text_style,
		        state,
		        GUISTR_TEXT_DIR_AUTO
		        );	

#ifdef ZDT_HSENSOR_SUPPORT_TW        
            MMIAPICOM_Wstrcat(notice_wbuf, tibiao_wbuf);
            API_WstrAppendStr(notice_wbuf, 31, ": ", 2); 
            sprintf(real_buf, "%.1f", real_tw);
            API_WstrAppendStr(notice_wbuf, 31, real_buf, strlen(real_buf));
            pedo_string.wstr_ptr  = notice_wbuf;
            pedo_string.wstr_len = MMIAPICOM_Wstrlen(pedo_string.wstr_ptr);
            text_style.font_color = MMI_WHITE_COLOR;
            text_style.font = SONG_FONT_20;
            GUISTR_DrawTextToLCDInRect( 
        		        MMITHEME_GetDefaultLcdDev(),
        		        (const GUI_RECT_T      *)&notice_rect,       //the fixed display area
        		        (const GUI_RECT_T      *)&notice_rect,       //用户要剪切的实际区域
        		        (const MMI_STRING_T    *)&pedo_string,
        		        &text_style,
        		        state,
        		        GUISTR_TEXT_DIR_AUTO
        		        );	
#endif
		        
            if(tw > 35 && tw <= 42.5)
            {
                MMIZDT_Display_TimerStop();
                ZDT_SendTw(tw);
                MMIZDT_StopTwAnimTimer();
                if(g_tw_is_net_testing == FALSE)
                {
                    MMIZDT_TW_HW_Close();
                }
                g_tw_is_win_testing = FALSE;
            }
        }
		break;
		
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            s_tw_tp_x = point.x;
            s_tw_tp_y = point.y;
            s_is_tw_tp_down = TRUE;
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(s_is_tw_tp_down)
            {
                offset_y =  point.y - s_tw_tp_y;
                offset_x =  point.x - s_tw_tp_x;
                if(offset_x <= -(20))	
                {
                    MMK_CloseWin(win_id);
                }	
                else if(offset_x >= (20))
                {
                    MMK_CloseWin(win_id);
                }

            }
            s_is_tw_tp_down = FALSE;
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
       case MSG_KEYDOWN_DOWN:
	   	if(!g_tw_is_win_testing)
	   	{
                 MMIZDT_TW_HW_Open();
                 g_tw_is_win_testing = TRUE;
                 MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0); 
                 MMIZDT_Display_TimerStop();
                 MMIZDT_Display_TimerStart(2500, MMIZDT_TW_HandleTimer);
                 just_open_tw=TRUE;
                 
	   	}
            break;
        case MSG_TIMER:
        {
            if (*(uint8*)param == tw_anim_timer_id)
            {
                tw_anim_index++;
                if(tw_anim_index >= 5)
                {
                    tw_anim_index = 0;
                }
                DrawTwAnimation();
            }
        }
        
        break;    
        case MSG_APP_CANCEL:
        //case MSG_KEYUP_CANCEL:
            MMK_CloseWin(win_id);
            break;
            
        case MSG_CLOSE_WINDOW:
            MMIZDT_BackLight(FALSE);
            MMIZDT_StopTwAnimTimer();
            if(g_tw_is_net_testing == FALSE)
            {
                MMIZDT_TW_HW_Close();
            }
            g_tw_is_win_testing = FALSE;
            MMIZDT_Display_TimerStop();
            break;
            
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}

WINDOW_TABLE( MMIZDT_WATCH_TW_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_TWWinMsg),    
    WIN_ID( MMIZDT_WATCH_TW_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC BOOLEAN MMIZDT_OpenTWWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_WATCH_TW_WIN_TAB,PNULL);
    return TRUE;
}

PUBLIC BOOLEAN MMIZDT_IsTwWinOpen()
{
    BOOLEAN ret = FALSE;
    if(MMK_IsOpenWin(MMIZDT_WATCH_TW_WIN_ID))
    {
        ret = TRUE;
    }
    return ret;
}

BOOLEAN MMIZDT_CloseTWWin(void)
{
    if(MMK_IsOpenWin(MMIZDT_WATCH_TW_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WATCH_TW_WIN_ID);
    }
    return TRUE;
}

BOOLEAN MMIZDT_UpdateTWWin(void)
{
    if(MMK_IsFocusWin(MMIZDT_WATCH_TW_WIN_ID))
    {
        MMK_PostMsg(MMIZDT_WATCH_TW_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
    return TRUE;
}

#endif

#if MAINMENU_DIAL_PANLE_SUPPORT //拨号窗口
#define MMIZDT_DIAL_MAX_LEN    40

#define MMIZDT_DIAL_NUMBER_BG_WIDTH 79
#define MMIZDT_DIAL_NUMBER_BG_WIDTH_M 80
#define MMIZDT_DIAL_NUMBER_BG_HEIGHT 50
 
#define MMIZDT_DIAL_NUMBER_BG_WIDTH_H 79
#define MMIZDT_DIAL_NUMBER_BG_HEIGHT_H 50
 
#define MMIZDT_DIAL_BUTTONS_HEIGHT 50
#define MMIZDT_DIAL_EDIT_FORM_HEIGHT 55//40///140//45 45
#define MMIZDT_DIAL_LINE_WIDTH 1

#define MMIZDT_DIAL_BUTTONS_HEIGHT_H 35
#ifdef MAINLCD_DEV_SIZE_240X284
#define MMIZDT_DIAL_EDIT_FORM_HEIGHT_H 43
#else
#define MMIZDT_DIAL_EDIT_FORM_HEIGHT_H 33
#endif

#define MMIZDT_DIAL_EDIT_TOP_OFFSET 4

LOCAL BOOLEAN   s_is_switch_to_editor;
LOCAL BOOLEAN s_zdt_is_need_check_input = TRUE;
LOCAL uint16 d_tp_down_x = 0;
LOCAL uint16 d_tp_up_x = 0;
LOCAL BOOLEAN d_is_tp_long_press = FALSE;

PUBLIC void MMIZDT_CloseDialWin(void );

LOCAL BOOLEAN ZDT_IsImeiIpnut(MMI_STRING_T *str_ptr)
{
    BOOLEAN is_imei_parsered = FALSE;
    uint8 temp_str[MMIZDT_DIAL_MAX_LEN + 1]  = {0};

    if (PNULL == str_ptr || PNULL == str_ptr->wstr_ptr || 0 == str_ptr->wstr_len)
    {
        return FALSE;
    }

    MMI_WSTRNTOSTR((uint8 *)temp_str,MMIZDT_DIAL_MAX_LEN,
                                str_ptr->wstr_ptr, str_ptr->wstr_len,
                                MIN(str_ptr->wstr_len, MMIZDT_DIAL_MAX_LEN));


    is_imei_parsered = MMIAPIPARSE_ProcessImeiStr((uint8 *)temp_str, MIN(str_ptr->wstr_len, MMIZDT_DIAL_MAX_LEN));

    return is_imei_parsered;
}

LOCAL BOOLEAN ZDT_IsResetpnut(MMI_STRING_T *str_ptr)
{
    BOOLEAN is_imei_parsered = FALSE;
    uint8 temp_str[MMIZDT_DIAL_MAX_LEN + 1]  = {0};

    if (PNULL == str_ptr || PNULL == str_ptr->wstr_ptr || 7 != str_ptr->wstr_len)
    {
        return FALSE;
    }

    MMI_WSTRNTOSTR((uint8 *)temp_str,MMIZDT_DIAL_MAX_LEN,
                                str_ptr->wstr_ptr, str_ptr->wstr_len,
                                MIN(str_ptr->wstr_len, MMIZDT_DIAL_MAX_LEN));

   // is_imei_parsered = MMIAPIPARSE_ProcessImeiStr((uint8 *)temp_str, MIN(str_ptr->wstr_len, MMIZDT_DIAL_MAX_LEN));
    if (strncmp((char*)temp_str,"*#7370#",7) == 0)
    {
        is_imei_parsered = TRUE;
    }
    return is_imei_parsered;
}

#ifdef ENG_SUPPORT
/*****************************************************************************/
// 	Description : Is Eng Input
//	Global resource dependence :
//  Author:
//	Note:
/*****************************************************************************/
LOCAL BOOLEAN ZDT_IsEngInput(MMI_STRING_T *str_ptr)
{
    BOOLEAN is_pin_input = FALSE;
    uint8 temp_str[MMIZDT_DIAL_MAX_LEN + 1]  = {0};

    if (PNULL == str_ptr || PNULL == str_ptr->wstr_ptr || str_ptr->wstr_len < 5)
    {
        return FALSE;
    }

    if ('*' == str_ptr->wstr_ptr[0] || '#' == str_ptr->wstr_ptr[0])
    {
        MMIENG_IDLE_DIAL_NUM_TYPE_E dial_num_type = MMIZDT_DIAL_MAX_LEN;
        parsCtrlCodeE ctrl_code = PARS_CHANGE_PIN1;
        uint16 temp_str_len = MIN(str_ptr->wstr_len, MMIZDT_DIAL_MAX_LEN);

        MMI_WSTRNTOSTR((uint8 *)temp_str, MMIZDT_DIAL_MAX_LEN,
                                    str_ptr->wstr_ptr, str_ptr->wstr_len, temp_str_len
                                    );

        if (MMIAPIENG_ParseIdleDialNumString(temp_str, temp_str_len, &dial_num_type, &ctrl_code))
        {
            MMIAPIENG_DoIdleDialNumOpt(dial_num_type);
            MMIZDT_CloseDialWin();
            is_pin_input = TRUE;
        }
    }

    return is_pin_input;
}
#endif

LOCAL void ZDT_DisplayDialWinBg(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN}; 
    GUI_RECT_T full_rct = MMITHEME_GetFullScreenRect();
    LCD_FillRect(&lcd_dev_info, full_rct, MMI_BLACK_COLOR);
}


LOCAL void ZDT_DisplayDialNum(MMI_WIN_ID_T win_id)
{
    GUI_POINT_T         dis_point = {0};
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};

    uint8 i=0;
    MMI_IMAGE_ID_T img_id[12] = {
        IMAGE_ZDT_NUMBER_KEY1_ICON,
        IMAGE_ZDT_NUMBER_KEY2_ICON,
        IMAGE_ZDT_NUMBER_KEY3_ICON,
        IMAGE_ZDT_NUMBER_KEY4_ICON,
        IMAGE_ZDT_NUMBER_KEY5_ICON,
        IMAGE_ZDT_NUMBER_KEY6_ICON,
        IMAGE_ZDT_NUMBER_KEY7_ICON,
        IMAGE_ZDT_NUMBER_KEY8_ICON,
        IMAGE_ZDT_NUMBER_KEY9_ICON,
        IMAGE_ZDT_BUTTON_DIAL,
        IMAGE_ZDT_NUMBER_KEY0_ICON,
        IMAGE_ZDT_BUTTON_DELKEY,
    };
    for(i=0;i<12;i++)
    {
        dis_point.x = DIAL_NUMBER_START_X + DIAL_NUMBER_NEXT_X*(i%3);
        dis_point.y = DIAL_NUMBER_START_Y + DIAL_NUMBER_NEXT_Y*(i/3);
        GUIRES_DisplayImg(&dis_point,PNULL,PNULL,win_id,img_id[i],&lcd_dev_info);
    }
}

LOCAL void ZDT_DisplayTPDownDialNum(MMI_WIN_ID_T win_id,uint8 num)
{
    GUI_POINT_T         dis_point = {0};
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUI_RECT_T bg_rect = {0};
    GUI_RECT_T display_rect = {0};
    GUI_RECT_T img_rect = {0};

    uint8 i=0;
    MMI_IMAGE_ID_T img_id[12] = {
        IMAGE_ZDT_NUMBER_KEY1_ICON_P,
        IMAGE_ZDT_NUMBER_KEY2_ICON_P,
        IMAGE_ZDT_NUMBER_KEY3_ICON_P,
        IMAGE_ZDT_NUMBER_KEY4_ICON_P,
        IMAGE_ZDT_NUMBER_KEY5_ICON_P,
        IMAGE_ZDT_NUMBER_KEY6_ICON_P,
        IMAGE_ZDT_NUMBER_KEY7_ICON_P,
        IMAGE_ZDT_NUMBER_KEY8_ICON_P,
        IMAGE_ZDT_NUMBER_KEY9_ICON_P,
        IMAGE_ZDT_BUTTON_DIAL_P,
        IMAGE_ZDT_NUMBER_KEY0_ICON_P,
        IMAGE_ZDT_BUTTON_DELKEY_P,
    };
    if(num > -1 && num < 11)
    {
        dis_point.x = DIAL_NUMBER_START_X + DIAL_NUMBER_NEXT_X*(num%3);
        dis_point.y = DIAL_NUMBER_START_Y + DIAL_NUMBER_NEXT_Y*(num/3);
        GUIRES_DisplayImg(&dis_point,PNULL,PNULL,win_id,img_id[num],&lcd_dev_info);
    }	
}

LOCAL void ZDT_DisplayTPUpDialNum(
                            MMI_WIN_ID_T    win_id
                            )
{
    GUI_POINT_T         dis_point = {0,56};
    GUI_RECT_T bg_rect = {0};
    GUI_RECT_T display_rect = {0};
    GUI_RECT_T img_rect = {0};

    GUI_BOTH_RECT_T client_rect = MMITHEME_GetWinClientBothRect(win_id);  
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    #if 1
    GUIRES_DisplayImg(&dis_point,
        PNULL,
        PNULL,
        win_id,
      IMAGE_THEME_BLACK_BG,// IMAGE_THEME_BG,//IMAGE_THEME_BLACK_BG,/// IMAGE_THEME_BG,  ///bug 6544
        &lcd_dev_info);
    #endif   
    ZDT_DisplayDialNum(win_id);	
}

LOCAL void ZDT_HandleDialGreenUpMsg
(
	MMI_WIN_ID_T	win_id,
	MMI_CTRL_ID_T	ctrl_id,
	MN_DUAL_SYS_E	dual_sys
)
{
	uint8				temp_str[MMIZDT_DIAL_MAX_LEN + 1]  = {0};
	MMI_STRING_T		edit_str = {0};
	CC_CALL_SIM_TYPE_E	sim_type = CC_SIM1_CALL;
		
	//get edit string
	GUIEDIT_GetString(ctrl_id,&edit_str);

	if (0 == edit_str.wstr_len)
	{
		//MMIAPICL_OpenCallsListWindow(MMICL_CALL_DIALED);
	}
	else
	{
		//convert string
		MMI_WSTRNTOSTR((uint8 *)temp_str,MMIZDT_DIAL_MAX_LEN,
			edit_str.wstr_ptr,edit_str.wstr_len,
			edit_str.wstr_len);
#ifdef MMI_SMS_VOICE_MAIL_SUPPORT
		if (edit_str.wstr_len == 1
			  && !MMIAPICC_IsInState(CC_IN_CALL_STATE)
			  && MMICC_GetCallNum()<=0)
		{   
			if (0== strcmp(temp_str,"1")) MMK_SendMsg(win_id, MSG_KEYLONG_1, PNULL);
			else if (0== strcmp(temp_str,"2")) MMK_SendMsg(win_id, MSG_KEYLONG_2, PNULL);
			else if (0== strcmp(temp_str,"3")) MMK_SendMsg(win_id, MSG_KEYLONG_3, PNULL);
			else if (0== strcmp(temp_str,"4")) MMK_SendMsg(win_id, MSG_KEYLONG_4, PNULL);
			else if (0== strcmp(temp_str,"5")) MMK_SendMsg(win_id, MSG_KEYLONG_5, PNULL);
			else if (0== strcmp(temp_str,"6")) MMK_SendMsg(win_id, MSG_KEYLONG_6, PNULL);
			else if (0== strcmp(temp_str,"7")) MMK_SendMsg(win_id, MSG_KEYLONG_7, PNULL);
			else if (0== strcmp(temp_str,"8")) MMK_SendMsg(win_id, MSG_KEYLONG_8, PNULL);
			else if (0== strcmp(temp_str,"9")) MMK_SendMsg(win_id, MSG_KEYLONG_9, PNULL);
			return;
		}
#endif
        if(MMIZDT_CheckSimStatus())
        {
		    sim_type = MMIAPICC_GetCallSimType(dual_sys);		
		    MMIAPICC_MakeCall(dual_sys,temp_str,(uint8)edit_str.wstr_len,PNULL,PNULL,sim_type,CC_CALL_NORMAL_CALL,PNULL);
        }
	}
}

LOCAL void ZDT_PlayDialRing()
{
    //MMIAPISET_UiPlayRingByVolume(0, FALSE,0, 1, MMISET_RING_TYPE_KEY, PNULL, 7);//yangyu delete
}

LOCAL BOOLEAN           s_is_tp_7_key   = FALSE;  //
LOCAL BOOLEAN           s_is_tp_8_key   = FALSE;  //
LOCAL BOOLEAN           s_is_tp_9_key   = FALSE;  //
LOCAL BOOLEAN		 s_is_tp_0_key	= FALSE;  //

LOCAL int8 getRectIndex(GUI_POINT_T point)
{
    int8 i = 0;
    GUI_RECT_T rect = {0};
    for(i;i<12;i++)
    {
        rect.left = DIAL_NUMBER_START_X + DIAL_NUMBER_NEXT_X*((i)%3);
        rect.top = DIAL_NUMBER_START_Y + DIAL_NUMBER_NEXT_Y*((i)/3);
        rect.right = rect.left + DIAL_NUMBER_IMG_WIDTH;
        rect.bottom = rect.top + DIAL_NUMBER_IMG_HEIGHT;
        if(GUI_PointIsInRect(point, rect))
        {
            return i;
        }
    }
    return -1;
}

LOCAL void ZDT_DialNumTPDown(MMI_WIN_ID_T win_id,GUI_POINT_T point)
{
    int8 index = getRectIndex(point);
    if(index > -1)
    {
        ZDT_DisplayTPDownDialNum(win_id,index);
        if(index==6)
        {
            s_is_tp_7_key = TRUE;
        }
        else if(index==7)
        {
            s_is_tp_8_key = TRUE;
        }
        else if(index==8)
        {
            s_is_tp_9_key = TRUE;
        }
        else if(index==10)
        {
            s_is_tp_0_key = TRUE;
        }
    }
}

LOCAL void ZDT_DialNumTPUp(MMI_WIN_ID_T win_id,GUI_POINT_T point)
{
    int8 index = getRectIndex(point);
    if(index > -1)
    {
        if(index==6)
        {
            if(s_is_tp_7_key == TRUE)
			{
				MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_7, PNULL);
			}
        }
        else if(index==7)
        {
            if(s_is_tp_8_key == TRUE)
			{
				MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_8, PNULL);
			}
        }
        else if(index==8)
        {
            if(s_is_tp_9_key == TRUE)
			{
				MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_9, PNULL);
				ZDT_PlayDialRing();
			}
        }
        else if(index==10)
        {
            if(s_is_tp_0_key == TRUE)
			{
				MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_0, PNULL);
			}
        }
        else if(index==9)
        {
            MMI_CTRL_ID_T edit_ctrl_id = MMK_GetCtrlHandleByWin(win_id, MMIZDT_DIAL_EDIT_CTRL_ID);
            ZDT_HandleDialGreenUpMsg(win_id,edit_ctrl_id,MN_DUAL_SYS_MAX);
        }
        else if(index==11)
        {
            MMI_CTRL_ID_T edit_ctrl_id = MMK_GetCtrlHandleByWin(win_id, MMIZDT_DIAL_EDIT_CTRL_ID);
            if (MMK_IsActiveCtrl(edit_ctrl_id) || MMK_SetAtvCtrl(win_id,edit_ctrl_id))
			{
				MMI_STRING_T edit_str = {0};
				GUIEDIT_GetString(edit_ctrl_id,&edit_str);
				if(edit_str.wstr_len ==0)
                {
					//MMK_CloseWin(win_id);
                }
				else
                {
					MMK_SendMsg(MMK_GetActiveCtrl(win_id),MSG_NOTIFY_IM_BACKSPACE,PNULL);
                }
			}
        }
        else
        {
            MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_1+index, PNULL);
        }
    }
}

LOCAL void ZDT_DialNumFactoryCheck(MMI_WIN_ID_T win_id)
{
    MMI_STRING_T		edit_str = {0};
    uint8			numStr[MMIZDT_DIAL_MAX_LEN + 1]	= {0};
    MMI_CTRL_ID_T edit_ctrl_id = MMK_GetCtrlHandleByWin(win_id, MMIZDT_DIAL_EDIT_CTRL_ID);
    GUIEDIT_GetString(edit_ctrl_id,&edit_str);
                   
    if(edit_str.wstr_len > 0 )
    {
        MMI_WSTRNTOSTR((uint8 *)numStr,MMIZDT_DIAL_MAX_LEN,
    		edit_str.wstr_ptr,edit_str.wstr_len,
    		edit_str.wstr_len);
    	if(strcmp(numStr, "*#9999#*") == 0)
    	{
            extern BOOLEAN MMIZDT_OpenWatchListFactoryWin(void);
            MMIZDT_OpenWatchListFactoryWin();
    	}
    	if(strcmp(numStr, "*#8378#*") == 0)
    	{
			YX_API_SetLand();
			MMIZDT_CloseDialWin();
    	}		
    	if(strcmp(numStr, "*#4444#*") == 0)
    	{
            MMIZDT_Net_Reset(TRUE);
    	}
    #if defined(W217_AGING_TEST_CUSTOM)//wuxx add. do not del.
        else if (strcmp(numStr, "*#5555#*") == 0)
        {
            extern void ZdtWatch_Factory_Camera_AGING_TEST_Entry(void);
            ZdtWatch_Factory_Camera_AGING_TEST_Entry();
        }
        else if (strcmp(numStr, "*#6666#*") == 0)
        {
            extern void ZdtWatch_Factory_MAINLCD_AGING_TEST_Entry(void);
            ZdtWatch_Factory_MAINLCD_AGING_TEST_Entry();
        }
        else if (strcmp(numStr, "*#7777#*") == 0)
        {
            extern void ZdtWatch_Factory_VIB_AGING_TEST_Entry(void);
            ZdtWatch_Factory_VIB_AGING_TEST_Entry();
        }
        else if (strcmp(numStr, "*#8888#*") == 0)
        {
            extern void ZdtWatch_Factory_Speaker_AGING_TEST_Entry(void);
            ZdtWatch_Factory_Speaker_AGING_TEST_Entry();
        }
    #endif
    }
}

LOCAL void ZDT_DialNumTPLongUp(MMI_WIN_ID_T win_id,GUI_POINT_T point)
{
    int8 index = getRectIndex(point);
    if(index==6)
    {
        s_is_tp_7_key = FALSE;
  		{
  			MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_STAR, PNULL);
  			ZDT_PlayDialRing();
  		}
    }
    else if(index==8)
    {
        s_is_tp_9_key = FALSE;
		MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_HASH, PNULL);
		ZDT_PlayDialRing();
    }
    else if(index==10)
    {
		s_is_tp_0_key = FALSE;
		MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_STAR, PNULL);
		MMK_SendMsg(MMK_GetActiveCtrl(win_id), MSG_APP_STAR, PNULL);
		MMK_SendMsg(MMK_GetActiveCtrl(win_id),MSG_KEYUP_STAR,PNULL);
		ZDT_PlayDialRing();
    }
    else if(index==11)
    {
        MMI_STRING_T edit_str = {0};
        MMI_CTRL_ID_T edit_ctrl_id = MMK_GetCtrlHandleByWin(win_id, MMIZDT_DIAL_EDIT_CTRL_ID);
        if (MMK_IsActiveCtrl(edit_ctrl_id) || MMK_SetAtvCtrl(win_id,edit_ctrl_id))
        {
            GUIEDIT_GetString(edit_ctrl_id,&edit_str);
            if(edit_str.wstr_len > 0)
            {
                GUIEDIT_ClearAllStr(edit_ctrl_id);
            }
            ZDT_PlayDialRing();
        }
    }
}

LOCAL MMI_RESULT_E ZDT_HandleDialWinMsg
(
	MMI_WIN_ID_T		win_id,
	MMI_MESSAGE_ID_E	msg_id,
	DPARAM				param
)
{

    MMI_RESULT_E  result = MMI_RESULT_TRUE;
    MMI_STRING_T edit_str = {0};
    MMI_CTRL_ID_T edit_ctrl_id = MMK_GetCtrlHandleByWin(win_id, MMIZDT_DIAL_EDIT_CTRL_ID);
    LOCAL wchar temp_wstr[MMIZDT_DIAL_MAX_LEN + 1] = {0};
    LOCAL wchar temp_wstr_len = 0;

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:  
            MMK_SetAtvCtrl(win_id,edit_ctrl_id);
            GUIEDIT_SetHandleRedKey(FALSE, edit_ctrl_id);
    		s_is_switch_to_editor=TRUE;
		    s_is_tp_7_key = FALSE;	
    		s_is_tp_8_key	= FALSE; 
    		s_is_tp_9_key = FALSE; 
    		s_is_tp_0_key = FALSE; 
		    GUIEDIT_SetSoftkey(edit_ctrl_id,0,0,TXT_NULL,TXT_NULL,NULL); 
            IGUICTRL_SetCircularHandleUpDown(MMK_GetCtrlPtr(edit_ctrl_id), FALSE);
            GUIEDIT_GetString(edit_ctrl_id, &edit_str);
    	
            s_zdt_is_need_check_input = TRUE;

            SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
            temp_wstr_len = MIN(MMIZDT_DIAL_MAX_LEN, edit_str.wstr_len);
            MMI_WSTRNCPY(temp_wstr, MMIZDT_DIAL_MAX_LEN, edit_str.wstr_ptr, edit_str.wstr_len, temp_wstr_len);
        break;
    case MSG_FULL_PAINT:
            ZDT_DisplayDialWinBg(win_id);
            ZDT_DisplayDialNum(win_id);
        break;

	  case MSG_TP_PRESS_DOWN:
	  	{
		 	 GUI_POINT_T	point = {0};
			 point.x = MMK_GET_TP_X(param);
			 point.y = MMK_GET_TP_Y(param);
			 d_tp_down_x = point.x; //yangyu add
			 d_is_tp_long_press = FALSE;
             ZDT_DialNumTPDown(win_id,point);
	   }
		break;			 
	  case MSG_TP_PRESS_LONG:
		{
			GUI_POINT_T    point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			d_is_tp_long_press = TRUE;
			ZDT_DialNumTPLongUp(win_id,point);
	    }	
	  	break;
	  case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T	point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            d_tp_up_x = point.x;//yangyu add
				
            ZDT_DisplayTPUpDialNum(win_id);
            ZDT_DialNumTPUp(win_id, point);

            #if 0
            if(d_is_tp_long_press || d_tp_up_x != d_tp_down_x)
            {
	            d_is_tp_long_press = FALSE;
	            break;
            }
            #endif
            d_is_tp_long_press = FALSE;
            ZDT_DialNumFactoryCheck(win_id);
        }
        break;

    case MSG_BACKLIGHT_TURN_ON:           
        MMK_SendMsg(MMK_GetParentWinHandle(win_id), MSG_FULL_PAINT, PNULL);
        break;

   case MSG_LCD_SWITCH:
        MMK_WinInactiveCtrl(win_id, FALSE);
        MMK_SetAtvCtrl(win_id, edit_ctrl_id);
        break;

    case MSG_LOSE_FOCUS:
		s_is_tp_7_key = FALSE;
		s_is_tp_8_key	= FALSE; 
		s_is_tp_9_key = FALSE; 
		s_is_tp_0_key = FALSE; 

        break;

    case MSG_GET_FOCUS:
            GUIEDIT_GetString(edit_ctrl_id, &edit_str);
            GUILIST_SetCurItemIndex(edit_ctrl_id,0);
            MMIAPISMS_CheckSMSState(FALSE);
            MMK_SetAtvCtrl(win_id,edit_ctrl_id); 
        break;

    case MSG_NOTIFY_EDITBOX_UPDATE_STRNUM:
        {
            BOOLEAN is_need_check = FALSE;
            MMI_STRING_T temp_str_info = {0};
			uint8			temp_str[MMIZDT_DIAL_MAX_LEN + 1]	= {0};

			
            GUIEDIT_GetString(edit_ctrl_id, &edit_str);
            
            temp_str_info.wstr_ptr = (wchar *)temp_wstr;
            temp_str_info.wstr_len = temp_wstr_len;

            if (temp_str_info.wstr_len > 0 && edit_str.wstr_len > 0 && (edit_str.wstr_len == temp_str_info.wstr_len + 1)
                && !MMIAPICC_IsInState(CC_IN_CALL_STATE)    //in call state do not check ADN 
            )
            {
                if ('#' == edit_str.wstr_ptr[edit_str.wstr_len - 1]
                    && 0 == memcmp(temp_str_info.wstr_ptr, edit_str.wstr_ptr, temp_str_info.wstr_len * sizeof(wchar)))//只有#在最后输入才做匹配
                {
                    is_need_check = TRUE;
                }
            }

            if (!s_zdt_is_need_check_input)
            {
                s_zdt_is_need_check_input = TRUE;

                is_need_check = FALSE;
            }

            SCI_MEMSET(temp_wstr, 0x00, sizeof(temp_wstr));
            temp_wstr_len = MIN(MMIZDT_DIAL_MAX_LEN, edit_str.wstr_len);
            MMI_WSTRNCPY(temp_wstr, MMIZDT_DIAL_MAX_LEN, edit_str.wstr_ptr, edit_str.wstr_len, temp_wstr_len);

		MMI_WSTRNTOSTR((uint8 *)temp_str,MMIZDT_DIAL_MAX_LEN,
						edit_str.wstr_ptr,edit_str.wstr_len,
						edit_str.wstr_len);

            if (is_need_check && ZDT_IsImeiIpnut(&edit_str))
            {
                MMIAPIPHONE_OpenIMEIDisplayWin();
                MMK_CloseWin(win_id);
            }
			else if(is_need_check && MMIAPIPARSE_ParseString(temp_str,(uint8)edit_str.wstr_len))
			{
                MMK_CloseWin(win_id);
			}
            else if (is_need_check && ZDT_IsResetpnut(&edit_str))
            {
			 MMIAPISET_OpenInputResetPwdWin(TRUE);
			 MMIAPIIDLE_CloseDialWin();
            }
#ifdef ENG_SUPPORT
            else if (is_need_check && ZDT_IsEngInput(&edit_str))
            {
                MMK_CloseWin(win_id);
            }
#endif
        }
        break;

    case MSG_CTL_OK:
    case MSG_APP_MENU:
        break;

    case MSG_CTL_MIDSK:
            ZDT_HandleDialGreenUpMsg(win_id,edit_ctrl_id,MN_DUAL_SYS_MAX);
        break;

    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;

#if 0//def MMI_SMS_VOICE_MAIL_SUPPORT //yangyu delete
    case MSG_KEYLONG_1:
        GUIEDIT_GetString(edit_ctrl_id, &edit_str);
        if((0 == edit_str.wstr_len)||(1 == edit_str.wstr_len))MMIAPIIDLE_HandleVoiceMailFunction();
        break;
#endif
    case MSG_KEYLONG_2:
    case MSG_KEYLONG_3:
    case MSG_KEYLONG_4:
    case MSG_KEYLONG_5:
    case MSG_KEYLONG_6:
    case MSG_KEYLONG_7:
    case MSG_KEYLONG_8:
    case MSG_KEYLONG_9:
        GUIEDIT_GetString(edit_ctrl_id, &edit_str);
        //if ((0 == edit_str.wstr_len)||(1 == edit_str.wstr_len))MMIAPIIDLE_HandleSpeedDialFunction(msg_id);//yangyu delete
        break;

    case MSG_APP_0:
    case MSG_APP_1:
    case MSG_APP_2:
    case MSG_APP_3:
    case MSG_APP_4:
    case MSG_APP_5:
    case MSG_APP_6:
    case MSG_APP_7:
    case MSG_APP_8:
    case MSG_APP_9:
    case MSG_APP_HASH:
    case MSG_APP_STAR:

        if (MMK_IsActiveCtrl(edit_ctrl_id) || MMK_SetAtvCtrl(win_id,edit_ctrl_id))
        {
            MMK_SendMsg(edit_ctrl_id, msg_id, PNULL);
        }
        break;

    case MSG_APP_UP:
    case MSG_APP_DOWN:
        {
       }
        break;

    case MSG_IDLE_DIAL_RESET_EDIT_IND:
        GUIEDIT_ClearAllStrEx(edit_ctrl_id, MMK_IsFocusWin(win_id));
        break;
	//yangyu add
	case MSG_APP_CANCEL:
		GUIEDIT_ClearAllStr(edit_ctrl_id);
		MMK_CloseWin(win_id);
		break;
    case MSG_KEYDOWN_RED:
        break;
    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
	//yangyu end
        
    default:
        result = MMI_RESULT_FALSE;
        break;

    }    
    return (result);
}

LOCAL void ZDT_InitDialEdit(MMI_WIN_ID_T win_id)
{

    // GUI_BG_T                bg_info = {0};
    MMI_CTRL_ID_T           edit_ctrl_id = MMIZDT_DIAL_EDIT_CTRL_ID;
    GUIEDIT_INIT_DATA_T     edit_init = {0};    
    MMI_CONTROL_CREATE_T    edit_ctrl = {0};     
    GUI_BOTH_RECT_T         client_rect = {0};

    client_rect = MMITHEME_GetWinClientBothRect(win_id);    
        
    edit_init.both_rect.v_rect.left = 0;
    edit_init.both_rect.v_rect.top = 0;
    edit_init.both_rect.v_rect.right = client_rect.v_rect.right;
    edit_init.both_rect.v_rect.bottom = edit_init.both_rect.v_rect.top + MMIZDT_DIAL_EDIT_FORM_HEIGHT;

    edit_init.both_rect.h_rect.left = 0;
    edit_init.both_rect.h_rect.top = 0;
    edit_init.both_rect.h_rect.right = client_rect.h_rect.right;
    edit_init.both_rect.h_rect.bottom = edit_init.both_rect.h_rect.top + MMIZDT_DIAL_EDIT_FORM_HEIGHT_H;

    edit_init.type        = GUIEDIT_TYPE_PHONENUM;
    edit_init.str_max_len = MMIZDT_DIAL_MAX_LEN;
    
    //create phone number edit control
    edit_ctrl.ctrl_id           = edit_ctrl_id;
    edit_ctrl.guid              = SPRD_GUI_EDITBOX_ID;
    edit_ctrl.init_data_ptr     = &edit_init;

    edit_ctrl.parent_win_handle = win_id;
    MMK_CreateControl(&edit_ctrl);	
    //set im not handle tp
    GUIEDIT_SetImTp(edit_ctrl_id, FALSE);

    //set delete after full
    GUIEDIT_SetPhoneNumStyle(FALSE, edit_ctrl_id);
    
    //set display direction
    GUIEDIT_SetPhoneNumDir(edit_ctrl_id,GUIEDIT_DISPLAY_DIR_RB);
    //多行但没有滚动条
    GUIEDIT_SetStyle(edit_ctrl_id, GUIEDIT_STYLE_SINGLE);//@fen.xie
    
    GUIEDIT_SetAlign(edit_ctrl_id, ALIGN_LVMIDDLE);
    //set border & font color & bg
    {
	GUI_BG_T		edit_bg = {GUI_BG_COLOR,GUI_SHAPE_RECT,IMAGE_NULL,MMI_BLACK_COLOR/*0xffff*/,FALSE};
	GUI_BORDER_T	edit_border = {1,MMI_BACKGROUND_COLOR,GUI_BORDER_NONE};
	GUI_COLOR_T 	edit_font_color = MMI_WHITE_COLOR;// MMI_BLACK_COLOR;

        GUIEDIT_SetBg(edit_ctrl_id, &edit_bg);
        GUIEDIT_SetBorder(edit_ctrl_id, &edit_border);
        GUIEDIT_SetFontColor(edit_ctrl_id, edit_font_color);
        GUIEDIT_PermitBorder(edit_ctrl_id,FALSE);
        {
            GUIEDIT_FONT_T  editfont ={0};
            editfont.is_valid = TRUE;
            editfont.big_font = SONG_FONT_28;
            editfont.mid_font = SONG_FONT_28;
            editfont.small_font = SONG_FONT_28;
            GUIEDIT_SetFontEx(MMK_GetCtrlHandleByWin(win_id, edit_ctrl_id), &editfont);
            GUIEDIT_SetDialEditAdaptive(MMK_GetCtrlHandleByWin(win_id, edit_ctrl_id), TRUE, TRUE);
        }
    }
}
PUBLIC void MMIZDT_OpenDialWin(void )
{
    MMI_WIN_ID_T                win_id = MMIZDT_WATCH_DIAL_WIN_ID;
    MMI_WINDOW_CREATE_T         win_create = {0};

    if (!MMK_IsOpenWin(win_id))
    {
        //create window
        win_create.applet_handle = MMK_GetFirstAppletHandle();
        win_create.win_id        = win_id;
        win_create.func          = ZDT_HandleDialWinMsg;
        win_create.win_priority  = WIN_ONE_LEVEL;
        win_create.window_style |= WS_NO_DEFAULT_STYLE ;/// WS_HAS_STATUSBAR;
        MMK_CreateWindow(&win_create);

        ZDT_InitDialEdit(win_id);
    }
    else
    {
        MMK_WinGrabFocus(win_id);
    }

}

PUBLIC void MMIZDT_CloseDialWin(void )
{
    MMK_CloseWin(MMIZDT_WATCH_DIAL_WIN_ID);
}

PUBLIC BOOLEAN MMIZDT_IsOpenDialWin()
{
    BOOLEAN ret = FALSE;
    if(MMK_IsOpenWin(MMIZDT_WATCH_DIAL_WIN_ID))
    {
        ret = TRUE;
    }
   
    return ret;
}
#else
PUBLIC void MMIZDT_OpenDialWin(void )
{
    return;
}

PUBLIC void MMIZDT_CloseDialWin(void )
{
    return;
}

PUBLIC BOOLEAN MMIZDT_IsOpenDialWin()
{
    return FALSE;
}
#endif

#ifdef ZDT_MAIN_MENU_SUPPORT // 主菜单窗口
#define MAIN_MENU_NEED_SUB_CAM 0
#define MAIN_MENU_NEED_HEALTH 0

typedef void (*ZDTMENUENTRYHANDLER)(uint32 image_id,uint32 text_id);
typedef void (*ZDTMENUEXITHANDLER)(void);
typedef struct _MENU_STATUS_METHOD_
{
    uint32 image_id;
    uint32 text_id;
    ZDTMENUENTRYHANDLER      DlgStart;
    ZDTMENUEXITHANDLER      DlgEnd;
}ZdtMenuMethodType;

typedef enum
{
    ZDT_MAINMENU_BEGIN = 0,
#if MAINMENU_DIAL_PANLE_SUPPORT
    ZDT_MAINMENU_DIAL,
#endif
#if MAIN_MENU_NEED_HEALTH
    ZDT_MAINMENU_HEALTH, 
    ZDT_MAINMENU_TRAVEL_CARD, 
    ZDT_MAINMENU_NUCLEIC_ACID, 	
    ZDT_MAINMENU_NUCLEIC_VACC,
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
    ZDT_MAINMENU_NOTICE,
#endif
#ifdef MMI_ADD_STUDENT_INFO
    ZDT_MAINMENU_STUIMAGE,
    ZDT_MAINMENU_STUINFO,
#endif
    ZDT_MAINMENU_PB,
#ifdef ZDT_MAINMENU_ADD_CL_REMOVE_ALIPAY
    ZDT_MAINMENU_CL,
#endif
#ifdef ZDT_GSENSOR_SUPPORT
    ZDT_MAINMENU_JP,
#endif
#ifdef TORCH_SUPPORT
    ZDT_MAINMENU_TORCH,
#endif
#if defined(ZDT_HSENSOR_SUPPORT) && !defined(ZDT_HSENSOR_SUPPORT_TW)
    ZDT_MAINMENU_HR,
#endif
#if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
    ZDT_MAINMENU_TW,
#endif
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    ZDT_MAINMENU_WCHAT,
#endif
#ifdef ZDT_ClASS_ANSWER_SUPPORT
    ZDT_MAINMENU_CLASS,
#endif

#ifdef CAMERA_SUPPORT
    ZDT_MAINMENU_CAM,
#if MAIN_MENU_NEED_SUB_CAM
    ZDT_MAINMENU_CAM_SUB,
#endif
    ZDT_MAINMENU_PHOTO,
#endif
#ifdef BROWSER_SUPPORT
    ZDT_MAINMENU_BROWSER,
#endif
#ifdef TULING_AI_SUPPORT 
    ZDT_MAINMENU_AI,
#endif
#ifdef BAIRUI_VIDEOCHAT_SUPPORT 
    ZDT_MAINMENU_AV,
#endif
#ifdef LEBAO_MUSIC_SUPPORT
    ZDT_MAINMENU_MG,
#endif
#ifdef XYSDK_SUPPORT
    ZDT_MAINMENU_XMLY,
#endif
#ifdef ZDT_ZFB_SUPPORT
    ZDT_MAINMENU_ZFB,
#endif
#ifdef ZDT_CYHEALTH_SUPPORT
    ZDT_MAINMENU_CY_HEALTH_1,
    ZDT_MAINMENU_CY_HEALTH_2,
#endif

#ifdef LOCAL_ALARM_CLOCK_SUPPORT
	ZDT_MAINMENU_ALARM,
#endif

    ZDT_MAINMENU_SETTING,
    ZDT_MAINMENU_2VM,
    ZDT_MAINMENU_END
}ZDT_MAINMENU_STAT_E;

typedef struct
{
    int x;
    int y;
} zdt_menu_pen_point_struct;

void ZdtWatch_MainMenu_Common_Exit(void);
void ZdtWatch_MainMenu_Common_Entry(uint32 image_id,uint32 text_id);

#ifdef MAIN_MENU_FOUR_GRID_STYLE
static const ZdtMenuMethodType ZdtMenuDlgEventHandlers[] = {
      NULL,NULL,NULL,NULL,
#if MAINMENU_DIAL_PANLE_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_DIAL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if MAIN_MENU_NEED_HEALTH
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef MMI_ADD_STUDENT_INFO
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_PB,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#ifdef ZDT_MAINMENU_ADD_CL_REMOVE_ALIPAY
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_CL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_GSENSOR_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_JB,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef TORCH_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_TORCH,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if defined(ZDT_HSENSOR_SUPPORT) && !defined(ZDT_HSENSOR_SUPPORT_TW)
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_HR,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_TW,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_WCHAT,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_ClASS_ANSWER_SUPPORT
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif

#ifdef CAMERA_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_CAM,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#if MAIN_MENU_NEED_SUB_CAM
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_CAM_SUB,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_PHOTO,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef TULING_AI_SUPPORT 
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_AI,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef BAIRUI_VIDEOCHAT_SUPPORT 
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_AV,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef LEBAO_MUSIC_SUPPORT 
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_MG,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef XYSDK_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_XMLY,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_ZFB_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_ZFB,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_CYHEALTH_SUPPORT
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_TINYCHAT_MIC,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef LOCAL_ALARM_CLOCK_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_ALARM,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef BROWSER_SUPPORT
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_BROW,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_SET,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_TINYCHAT_MIC,TXT_ZDT_MAINMENU_2VM,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
      NULL,NULL,NULL,NULL,
};
#else
static const ZdtMenuMethodType ZdtMenuDlgEventHandlers[] = {
      NULL,NULL,NULL,NULL,
#if MAINMENU_DIAL_PANLE_SUPPORT
	IMAGE_MAIN_MENU_ZDT_DIAL,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if MAIN_MENU_NEED_HEALTH
	IMAGE_MAIN_MENU_ZDT_HEALTH,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_MAIN_MENU_ZDT_TRAVEL,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_MAIN_MENU_ZDT_NUCLEICACID,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_MAIN_MENU_ZDT_VAA,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
	IMAGE_MAIN_MENU_ZDT_MESSAGE,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef MMI_ADD_STUDENT_INFO
	IMAGE_MAIN_MENU_ZDT_STUDENT_IMAGE,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_MAIN_MENU_ZDT_STUDENT_INFO,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
	IMAGE_MAIN_MENU_ZDT_PB,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#ifdef ZDT_MAINMENU_ADD_CL_REMOVE_ALIPAY
	IMAGE_MAIN_MENU_ZDT_CL,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_GSENSOR_SUPPORT
	IMAGE_MAIN_MENU_ZDT_JB,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef TORCH_SUPPORT
	IMAGE_MAIN_MENU_ZDT_TORCH,TXT_ZDT_MAINMENU_TORCH,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if defined(ZDT_HSENSOR_SUPPORT) && !defined(ZDT_HSENSOR_SUPPORT_TW)
	IMAGE_MAIN_MENU_ZDT_HR,TXT_ZDT_MAINMENU_HR,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
	IMAGE_MAIN_MENU_ZDT_TW,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
	IMAGE_MAIN_MENU_ZDT_WL,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_ClASS_ANSWER_SUPPORT
	IMAGE_MAIN_MENU_ZDT_CLASS,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef CAMERA_SUPPORT
	IMAGE_MAIN_MENU_ZDT_CAM,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#if MAIN_MENU_NEED_SUB_CAM
	IMAGE_MAIN_MENU_ZDT_CAM_SUB,TXT_ZDT_MAINMENU_CAM_SUB,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
	IMAGE_MAIN_MENU_ZDT_PHOTO,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef TULING_AI_SUPPORT 
	IMAGE_MAIN_MENU_ZDT_AI,TXT_ZDT_MAINMENU_AI,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef BAIRUI_VIDEOCHAT_SUPPORT 
	IMAGE_MAIN_MENU_ZDT_AV,TXT_ZDT_MAINMENU_AV,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef LEBAO_MUSIC_SUPPORT 
	IMAGE_MAIN_MENU_ZDT_MG,TXT_ZDT_MAINMENU_MG,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef XYSDK_SUPPORT
	IMAGE_MAIN_MENU_ZDT_XMLY,TXT_ZDT_MAINMENU_XMLY,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_ZFB_SUPPORT
	IMAGE_MAIN_MENU_ZDT_ZFB,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef ZDT_CYHEALTH_SUPPORT
	IMAGE_MAIN_MENU_ZDT_HEALTH_1,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_MAIN_MENU_ZDT_HEALTH_2,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef LOCAL_ALARM_CLOCK_SUPPORT
	IMAGE_MAIN_MENU_ZDT_ALARM,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
#ifdef BROWSER_SUPPORT
	IMAGE_MAIN_MENU_ZDT_BROW,TXT_ZDT_MAINMENU_BROW,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
#endif
	IMAGE_MAIN_MENU_ZDT_SET,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
	IMAGE_MAIN_MENU_ZDT_EWM,NULL,ZdtWatch_MainMenu_Common_Entry, ZdtWatch_MainMenu_Common_Exit,
      NULL,NULL,NULL,NULL,
};
#endif

extern BOOLEAN ZdtWatch_MainMenu_StartNext(void);
extern BOOLEAN ZdtWatch_MainMenu_StartPre(void);

ZDT_MAINMENU_STAT_E g_zdt_mainmenu_idx = ZDT_MAINMENU_BEGIN;
static ZDT_MAINMENU_STAT_E mainmenu_cur_active = 0;
static BOOLEAN zdt_mainmenu_one = FALSE;

void ZdtWatch_MainMenu_Show(uint32 image_id,uint32 text_id)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_STRING_T    cur_str_t   = {0};
    GUI_RECT_T   cur_rect          = {0};
    GUI_RECT_T   rect        = {0}; 
    GUISTR_STYLE_T      text_style      = {0};
    GUI_POINT_T                 dis_point = {0,0};
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    GUI_FONT_T f_big =SONG_FONT_42;

    rect.left   = 0;
    rect.top    = 0; 
    rect.right  = MMI_MAINSCREEN_WIDTH-1;
    rect.bottom = MMI_MAINSCREEN_HEIGHT-1;
    
    GUIRES_DisplayImg(&dis_point,
                    &rect,
                    PNULL,
                    MMIZDT_WATCH_MAINMENU_WIN_ID,
                    image_id,
                    &lcd_dev_info);
    
    //LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

    cur_rect = rect;
    cur_rect.top    = 190; 

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = f_big;
    text_style.font_color = MMI_WHITE_COLOR;

    MMI_GetLabelTextByLang((MMI_TEXT_ID_T)text_id,&cur_str_t);

    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        (const GUI_RECT_T      *)&cur_rect,       
        (const GUI_RECT_T      *)&cur_rect,     
        (const MMI_STRING_T    *)&cur_str_t,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
        ); 
    MMITHEME_UpdateRect();
    return;
}

void ZdtWatch_MainMenu_Common_Exit(void)
{
    return;
}

void ZdtWatch_MainMenu_Common_Entry(uint32 image_id,uint32 text_id)
{
    ZdtWatch_MainMenu_Show(image_id,text_id);
    return;
}

static void watch_mainmenu_key_enter_hdlr(void)
{
#if MAINMENU_DIAL_PANLE_SUPPORT
    if(g_zdt_mainmenu_idx == ZDT_MAINMENU_DIAL)
    {
        MMIZDT_OpenDialWin();
    }
    else 
#endif
    if(g_zdt_mainmenu_idx == ZDT_MAINMENU_PB)
    {
        MMIZDT_OpenPBWin();
        zdt_DB_AppCount_Rec.ADDRESSBOOK++;
        zdt_DB_AppCount_Rec.index++;
    }
#ifdef MMI_ADD_STUDENT_INFO
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_STUIMAGE)
    {
        MMIAPISET_MainmenuStudentImageWin();
    }
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_STUINFO)
    {
    	   MMIAPISET_MainmenuStudentInfoWin();
    }	
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
   else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_NOTICE)
    {
        g_cur_mesage_index=0;
	MMIZDT_OpenNoticeWin();
      zdt_DB_AppCount_Rec.MESSAGE++;
      zdt_DB_AppCount_Rec.index++;
    }
#endif
#if MAIN_MENU_NEED_HEALTH
   else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_HEALTH)
    {
    	memset(&yx_health_msg_Rec,0,sizeof(yx_health_msg_Rec));
    	YX_Net_Send_HealthCode_Request();
	MMIZDT_OpenHealthWin();
    }
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_TRAVEL_CARD)
    {
    	memset(&yx_health_msg_Rec,0,sizeof(yx_health_msg_Rec));
    	YX_Net_Send_TravelCard_Request();
	MMIZDT_OpenTravelCardWin();
    }
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_NUCLEIC_ACID)
    {
    	memset(&yx_health_msg_Rec,0,sizeof(yx_health_msg_Rec));
    	YX_Net_Send_NUCLEICACID_Request();
	MMIZDT_OpenNucleiCacidWin();
    }
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_NUCLEIC_VACC)
    {
    	memset(&yx_health_msg_Rec,0,sizeof(yx_health_msg_Rec));
    	YX_Net_Send_GETVACC_Request();
	MMIZDT_OpenVaccWin();
    }
#endif
#ifdef ZDT_MAINMENU_ADD_CL_REMOVE_ALIPAY
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_CL)
    {
        //MMICL_OpenMainWindow();
        zdt_DB_AppCount_Rec.CALL_RECORD++;
        zdt_DB_AppCount_Rec.index++;
    }
#endif
#ifdef ZDT_GSENSOR_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_JP)
    {
        MMIZDT_OpenJPWin();
        zdt_DB_AppCount_Rec.PEDOMETER++;
        zdt_DB_AppCount_Rec.index++;
    }
#endif
#ifdef TORCH_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_TORCH)
    {
        //MMIACC_OpenFlashlighWin();
    }
#endif
#if defined(ZDT_HSENSOR_SUPPORT) && !defined(ZDT_HSENSOR_SUPPORT_TW)
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_HR)
    {
        MMIZDT_OpenHRWin();
    }
#endif
#if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_TW)
    {
        MMIZDT_OpenTWWin();
    }
#endif
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_WCHAT)
    {
        MMIZDT_OpenTinyChatWin();
    }
#endif
#ifdef ZDT_ClASS_ANSWER_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_CLASS)
    {
        MMIWLDT_OpenClassWin();
        zdt_DB_AppCount_Rec.ANSWERCARD++;
        zdt_DB_AppCount_Rec.index++;
    }
#endif

#ifdef ZDT_CYHEALTH_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_CY_HEALTH_1)
    {
        MMIZDT_OpenHealthWin();
        zdt_DB_AppCount_Rec.HEALTHCODE++;
        zdt_DB_AppCount_Rec.index++;
    }
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_CY_HEALTH_2)
    {
        MMIZDT_OpenHealth2Win();
        zdt_DB_AppCount_Rec.IDCODE++;
        zdt_DB_AppCount_Rec.index++;
    }
#endif

#ifdef CAMERA_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_CAM)
    {
        //MMIAPIDC_OpenPhotoWinMain();
    }
#if MAIN_MENU_NEED_SUB_CAM
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_CAM_SUB)
    {
        MMIAPIDC_OpenPhotoWinSub();
    }
#endif
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_PHOTO)
    {
        MMIAPIPICVIEW_OpenPicViewer();
    }
#endif
#ifdef TULING_AI_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_AI)
    {
        MMIAPIMENU_EnterAiChat();
    }
#endif
#ifdef BAIRUI_VIDEOCHAT_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_AV)
    {
        MMI_OpenVideoChatWin();
    }
#endif
#ifdef LEBAO_MUSIC_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_MG)
    {
        MMIAPIMENU_EnterLebao();
    }
#endif
#ifdef XYSDK_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_XMLY)
    {
        LIBXMLYAPI_CreateXysdkMainWin();
    }
#endif
#ifdef ZDT_ZFB_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_ZFB)
    {
        MMIZFB_OpenMainWin();
        zdt_DB_AppCount_Rec.ALIPAY++;
        zdt_DB_AppCount_Rec.index++;
    }
#endif
#ifdef LOCAL_ALARM_CLOCK_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_ALARM)
    {
		MMIALARMCLOCK_CreateMainWin();//本地闹钟
            zdt_DB_AppCount_Rec.CLOCK++;
            zdt_DB_AppCount_Rec.index++;
    }
#endif

#ifdef BROWSER_SUPPORT
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_BROWSER)
    {
        MMIBROWSER_ENTRY_PARAM entry_param = {0};

        entry_param.type = MMIBROWSER_ACCESS_MAINMENU;
        entry_param.dual_sys = MN_DUAL_SYS_MAX;

        MMIAPIBROWSER_Entry (&entry_param);
    }
#endif
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_SETTING)
    {
        MMIAPIMENU_EnterSetting();
    }
    else if(g_zdt_mainmenu_idx == ZDT_MAINMENU_2VM)
    {
#if defined(ZDT_PLAT_YX_SUPPORT_YS) || defined(ZDT_PLAT_YX_SUPPORT_FZD) || defined(ZDT_PLAT_YX_SUPPORT_TX)
        MMIZDT_OpenManualWin();
        //MMIZDT_Open2VMWin();
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) 
        // MMIZDT_OpenBindWin();
         MMIZDT_OpenBindStep2Win_Ex();   //
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_CY) 
        //MMIZDT_OpenBindWin();
        MMIZDT_OpenBindStep2Win_Ex();   //
#endif
    }
    else
    {
    }
    return;
}

static void watch_mainmenu_key_power_hdlr(void)
{
    ZdtWatch_MainMenu_StartNext();
}

#ifdef TOUCH_PANEL_SUPPORT
#define ZDT_MAINMENU_TP_OFFSET_VALUE 20
uint8  MainMenu_pen_timer_falg=0;
static zdt_menu_pen_point_struct mainmenu_tp_old_point = {0,0};
uint8 g_zdt_mainmenu_disp_timer_id = 0;
uint8 g_zdt_mainmenu_tp_timer_id = 0;
BOOLEAN g_zdt_mainmenu_is_tp_down = FALSE;

void watch_mainmenu_timer_handler(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
	MainMenu_pen_timer_falg=0;
      if(0 != g_zdt_mainmenu_tp_timer_id)
      {
          MMK_StopTimer(g_zdt_mainmenu_tp_timer_id);
          g_zdt_mainmenu_tp_timer_id = 0;
      }
}

/*****************************************************************************
 * FUNCTION
 *  watch_mainmenu_register_input_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void watch_mainmenu_pen_down_hdlr(zdt_menu_pen_point_struct pt)
{
    mainmenu_tp_old_point = pt;
    g_zdt_mainmenu_is_tp_down = TRUE;
    #if 1
        MainMenu_pen_timer_falg = 0;
    #else
    MainMenu_pen_timer_falg=1;
    if(0 != g_zdt_mainmenu_tp_timer_id)
    {
        MMK_StopTimer(g_zdt_mainmenu_tp_timer_id);
        g_zdt_mainmenu_tp_timer_id = 0;
    }
    
    g_zdt_mainmenu_tp_timer_id = MMK_CreateTimerCallback(100, 
                                                                        watch_mainmenu_timer_handler, 
                                                                        (uint32)PNULL, 
                                                                        FALSE);
    #endif
}

static void watch_mainmenu_pen_left(void)
{
    ZdtWatch_MainMenu_StartNext();
}
static void watch_mainmenu_pen_right(void)
{
    ZdtWatch_MainMenu_StartPre();
}

static void watch_mainmenu_pen_enter(void)
{
    watch_mainmenu_key_enter_hdlr();
}

static void watch_mainmenu_pen_up_hdlr(zdt_menu_pen_point_struct pt)
{
    if(g_zdt_mainmenu_is_tp_down)
    {
        int offset_y =  pt.y - mainmenu_tp_old_point.y;//up to down
        int offset_x =  pt.x - mainmenu_tp_old_point.x;//up to down
    
	if(offset_x <= -(ZDT_MAINMENU_TP_OFFSET_VALUE))	
	{
            watch_mainmenu_pen_left();
	}	
	else if(offset_x >= (ZDT_MAINMENU_TP_OFFSET_VALUE))
	{
            watch_mainmenu_pen_right();
	}
	else if(MainMenu_pen_timer_falg==0)
	{
            if((pt.x>40)&&(pt.x<200)&&(pt.y>40)&&(pt.y<200))
            {
                watch_mainmenu_pen_enter();
            }
	}
    }
      g_zdt_mainmenu_is_tp_down = FALSE;
}
#endif /*__MMI_TOUCH_SCREEN__*/

BOOLEAN ZdtWatch_MainMenu_StartCur(void)
{
    if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgStart != NULL)
    {
        ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgStart(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].image_id,ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].text_id);
        return TRUE;
    }
    return FALSE;
}

BOOLEAN ZdtWatch_MainMenu_EndCur(void)
{
    if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd != NULL)
    {
        ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd();
        return TRUE;
    }
    return FALSE;
}

BOOLEAN ZdtWatch_MainMenu_StartNext(void)
{
    if(g_zdt_mainmenu_idx >=  ZDT_MAINMENU_END)
    {
        return FALSE;
    }
    if(zdt_mainmenu_one)
    {
        zdt_mainmenu_one = FALSE;
        if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd != NULL)
        {
            ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd();
        }
        MMK_CloseWin(MMIZDT_WATCH_MAINMENU_WIN_ID);
        return FALSE;
    }
    if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd != NULL)
    {
        ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd();
    }
    g_zdt_mainmenu_idx++;
    mainmenu_cur_active = g_zdt_mainmenu_idx;
#if 0
    if(g_zdt_mainmenu_idx >= ZDT_MAINMENU_END)
    {
        MMK_CloseWin(MMIZDT_WATCH_MAINMENU_WIN_ID);
        return FALSE;
    }
#else
    if(g_zdt_mainmenu_idx >= ZDT_MAINMENU_END)
    {
		 g_zdt_mainmenu_idx=ZDT_MAINMENU_BEGIN+1;
		 mainmenu_cur_active=g_zdt_mainmenu_idx;
    }
#endif
    if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgStart != NULL)
    {
        ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgStart(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].image_id,ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].text_id);
        return TRUE;
    }
    return FALSE;
}

BOOLEAN ZdtWatch_MainMenu_StartPre(void)
{
    if(g_zdt_mainmenu_idx >=  ZDT_MAINMENU_END)
    {
        return FALSE;
    }
    if(zdt_mainmenu_one)
    {
        zdt_mainmenu_one = FALSE;
        if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd != NULL)
        {
            ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd();
        }
        MMK_CloseWin(MMIZDT_WATCH_MAINMENU_WIN_ID);
        return FALSE;
    }
    if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd != NULL)
    {
        ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgEnd();
    }
    if(g_zdt_mainmenu_idx > ZDT_MAINMENU_BEGIN)
    {
        g_zdt_mainmenu_idx--;
    }
    if(g_zdt_mainmenu_idx == ZDT_MAINMENU_BEGIN)
    {
        g_zdt_mainmenu_idx = ZDT_MAINMENU_END-1;
#if 0

        MMK_CloseWin(MMIZDT_WATCH_MAINMENU_WIN_ID);
        return FALSE;
#endif
    }
    mainmenu_cur_active = g_zdt_mainmenu_idx;
#if 0
    if(g_zdt_mainmenu_idx >= ZDT_MAINMENU_END)
    {
        MMK_CloseWin(MMIZDT_WATCH_MAINMENU_WIN_ID);
        return FALSE;
    }
#endif
    if(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgStart != NULL)
    {
        ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].DlgStart(ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].image_id,ZdtMenuDlgEventHandlers[g_zdt_mainmenu_idx].text_id);
        return TRUE;
    }
    return FALSE;
}
void ZdtWatch_MainMenu_Exit(void)
{
    ZDT_LOG("MainMenuWatch_MainMenu_Exit");
    ZdtWatch_MainMenu_EndCur();
    mainmenu_cur_active = ZDT_MAINMENU_END;
    zdt_mainmenu_one = FALSE;
    g_zdt_mainmenu_idx = ZDT_MAINMENU_BEGIN;
    return;
}

void ZdtWatch_MainMenu_Entry(void)
{
    ZDT_LOG("ZdtWatch_MainMenu_Entry");
    if(g_zdt_mainmenu_idx > ZDT_MAINMENU_BEGIN && g_zdt_mainmenu_idx < ZDT_MAINMENU_END)
    {
        //ZDT_LOG("ZdtWatch_MainMenu_Entry %d",g_zdt_mainmenu_idx);
    }
    else
    {
        g_zdt_mainmenu_idx = ZDT_MAINMENU_BEGIN+1;
        //ZDT_LOG("ZdtWatch_MainMenu_Entry Start SWVER");
    }
    return;
}

void ZdtWatch_MainMenu_Entry_App(void)
{
    ZDT_LOG("ZdtWatch_MainMenu_Entry_App");
    zdt_mainmenu_one = FALSE;
    mainmenu_cur_active = g_zdt_mainmenu_idx;
    ZdtWatch_MainMenu_Entry();
    return;
}

void ZdtWatch_MainMenu_Entry_AppExt(uint8 menu_idx)
{
    ZDT_LOG("ZdtWatch_MainMenu_Entry_AppExt menu_idx=%d",menu_idx);
    if(menu_idx > ZDT_MAINMENU_BEGIN && menu_idx < ZDT_MAINMENU_END)
    {
        zdt_mainmenu_one = FALSE;
        mainmenu_cur_active = menu_idx;
        g_zdt_mainmenu_idx = menu_idx;
        ZdtWatch_MainMenu_Entry_App();
    }
    return;
}

void ZdtWatch_MainMenu_Entry_AppStart(void)
{
    ZDT_LOG("ZdtWatch_MainMenu_Entry_AppStart");
    zdt_mainmenu_one = FALSE;
    mainmenu_cur_active = ZDT_MAINMENU_BEGIN+1;
    g_zdt_mainmenu_idx = ZDT_MAINMENU_BEGIN+1;;
    ZdtWatch_MainMenu_Entry_App();
    return;
}

void ZdtWatch_MainMenu_Entry_AppEnd(void)
{
    ZDT_LOG("ZdtWatch_MainMenu_Entry_AppEnd");
    zdt_mainmenu_one = FALSE;
    mainmenu_cur_active = ZDT_MAINMENU_END-1;
    g_zdt_mainmenu_idx = ZDT_MAINMENU_END-1;;
    ZdtWatch_MainMenu_Entry();
    return;
}


BOOLEAN ZdtWatch_MainMenu_IsInDail(void)
{
#if MAINMENU_DIAL_PANLE_SUPPORT
    if(mainmenu_cur_active == ZDT_MAINMENU_DIAL)
    {
        return TRUE;
    }
#endif
    return FALSE;
}

BOOLEAN ZdtWatch_MainMenu_IsOpen(void)
{
    MMI_WIN_ID_T win_id = MMK_GetFocusWinId();
    return win_id == MMIZDT_WATCH_MAINMENU_WIN_ID ;
}

LOCAL MMI_RESULT_E HandleZDT_WatchMainMenuWinMsg (
                                      MMI_WIN_ID_T   win_id, 	// 窗口的ID
                                      MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                      DPARAM            param		// 相应消息的参数
                                      );

WINDOW_TABLE( MMIZDT_WATCH_MAINMENU_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_WatchMainMenuWinMsg),    
    WIN_ID( MMIZDT_WATCH_MAINMENU_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

BOOLEAN MMIZDT_OpenWatchMainMenuWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_WATCH_MAINMENU_WIN_TAB,PNULL);
    return TRUE;
}

BOOLEAN MMIZDT_CloseWatchMainMenuWin(void)
{
    if(MMK_IsOpenWin(MMIZDT_WATCH_MAINMENU_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WATCH_MAINMENU_WIN_ID);
    }
    return TRUE;
}

BOOLEAN MMIZDT_OpenWatchMainMenuWinIndex(uint8 index)
{
    uint8 * p_idx = SCI_ALLOC_APP(sizeof(uint8));
    *p_idx = index;
    MMK_CreateWin((uint32*)MMIZDT_WATCH_MAINMENU_WIN_TAB,(ADD_DATA)p_idx);
    return TRUE;
}

BOOLEAN MMIZDT_OpenWatchMainMenuWinStart(void)
{
    uint8 * p_idx = SCI_ALLOC_APP(sizeof(uint8));
    *p_idx = ZDT_MAINMENU_END-1;
    MMK_CreateWin((uint32*)MMIZDT_WATCH_MAINMENU_WIN_TAB,(ADD_DATA)p_idx);
    return TRUE;
}

BOOLEAN MMIZDT_OpenWatchMainMenuWinEnd(void)
{
    uint8 * p_idx = SCI_ALLOC_APP(sizeof(uint8));
    *p_idx = ZDT_MAINMENU_BEGIN+1;
    MMK_CreateWin((uint32*)MMIZDT_WATCH_MAINMENU_WIN_TAB,(ADD_DATA)p_idx);
    return TRUE;
}

/*****************************************************************************/
// 	Description : to handle the message of atcmd test 
//	Global resource dependence : 
//  Author:jianshengqi
//	Note: 2006/3/19 
/*****************************************************************************/
//extern uint32 test_main_lcd_id[20];
//extern uint32 test_sub_lcd_id[20];

LOCAL MMI_RESULT_E  HandleZDT_WatchMainMenuWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    uint8 * p_index = PNULL;
    p_index = (uint8 *) MMK_GetWinAddDataPtr(win_id);
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            uint32 idx = 0;
            if(p_index != PNULL)
            {
                idx = *p_index;
                ZdtWatch_MainMenu_Entry_AppExt(idx);
            }
            else
            {
                ZdtWatch_MainMenu_Entry_App();
            }
        }
        break;
    case MSG_SET_CLEAN_DATE_IND:
        {
                //清除所有数据
                MMISET_CleanUserData();
                MMISET_ResetFactorySetting();
                MMK_CloseWin(win_id);
        }
        break;

        case MSG_FULL_PAINT:
                ZdtWatch_MainMenu_StartCur();
            break;
            
        case MSG_GET_FOCUS:
                ZdtWatch_MainMenu_StartCur();
            break;
            
        case MSG_LOSE_FOCUS:
                //ZdtWatch_MainMenu_EndCur();
            break;
            
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            zdt_menu_pen_point_struct tp_point = {0,0};
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            tp_point.x = point.x;
            tp_point.y = point.y;
            ZDT_LOG("MAINMENU TP MSG_TP_PRESS_DOWN, x=%d,y=%d",point.x,point.y);
            watch_mainmenu_pen_down_hdlr(tp_point);
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            zdt_menu_pen_point_struct tp_point = {0,0};
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            tp_point.x = point.x;
            tp_point.y = point.y;
            ZDT_LOG("MAINMENU TP MSG_TP_PRESS_UP, x=%d,y=%d",point.x,point.y);
            watch_mainmenu_pen_up_hdlr(tp_point);
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            zdt_menu_pen_point_struct tp_point = {0,0};
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            tp_point.x = point.x;
            tp_point.y = point.y;
            ZDT_LOG("MAINMENU TP MSG_TP_PRESS_MOVE, x=%d,y=%d",point.x,point.y);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            zdt_menu_pen_point_struct tp_point = {0,0};
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            tp_point.x = point.x;
            tp_point.y = point.y;
            ZDT_LOG("MAINMENU TP MSG_TP_PRESS_LONG, x=%d,y=%d",point.x,point.y);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

    case MSG_KEYDOWN_RED:
        break;
        
    case MSG_KEYUP_RED:
		//watch_mainmenu_key_power_hdlr();                    
        break;
        
    case MSG_KEYDOWN_OK:     
    case MSG_KEYDOWN_WEB:
        break;
        
    case MSG_KEYUP_OK:     
    case MSG_KEYUP_WEB:
            watch_mainmenu_key_enter_hdlr();
        break;
        
    case MSG_KEYDOWN_CANCEL:
            MMK_CloseWin(win_id);
        break;
        
    case MSG_KEYDOWN_DOWN:
    case MSG_KEYDOWN_RIGHT:
             ZdtWatch_MainMenu_StartNext();
        break;
        
    case MSG_KEYDOWN_UP:
    case MSG_KEYDOWN_LEFT:
	     ZdtWatch_MainMenu_StartPre();
        break;

    case MSG_CLOSE_WINDOW:
            ZdtWatch_MainMenu_Exit();
            if(p_index != PNULL)
            {
                SCI_FREE(p_index);
            }
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}
#endif

#if 1 //重启复位窗口
void ZdtWatch_Reset_ShowData(MMI_WIN_ID_T        win_id)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    char disp_str[100] = {0};
    wchar disp_wstr[100] = {0};
    MMI_STRING_T    cur_str_t   = {0};
    GUI_RECT_T   cur_rect          = {0};
    GUI_RECT_T   rect        = {0}; 
    GUISTR_STYLE_T      text_style      = {0};  
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    uint16 tittle[10] = {0x6062, 0x590D, 0x51FA, 0x5382, 0x9ED8, 0x8BA4,0x503C,0}; //恢复出厂默认值6062 590D 51FA 5382 9ED8 8BA4 503C 
    GUI_FONT_T f_big =MMI_DEFAULT_BIG_FONT;
    GUI_FONT_T f_mid =MMI_DEFAULT_NORMAL_FONT;
    BOOLEAN is_calibration = FALSE; //zdt_app_is_calibrationed();

    rect.left   = 0;
    rect.top    = 0; 
    rect.right  = MMI_MAINSCREEN_WIDTH-1;
    rect.bottom = MMI_MAINSCREEN_HEIGHT-1;

    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

    cur_rect = rect;
    cur_rect.top    = 2; 
    cur_rect.bottom = cur_rect.top + 24;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = f_big;
    text_style.font_color = MMI_WHITE_COLOR;
    
    MMIRES_GetText(TXT_SET_SUCCESS, MMK_ConvertIdToHandle(win_id), &cur_str_t);

    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        (const GUI_RECT_T      *)&cur_rect,       
        (const GUI_RECT_T      *)&cur_rect,     
        (const MMI_STRING_T    *)&cur_str_t,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
        ); 
    
    cur_str_t.wstr_len = MMIAPICOM_Wstrlen(tittle);
    cur_str_t.wstr_ptr = tittle;
    cur_rect.top    = 120; 
    cur_rect.bottom = cur_rect.top + 34;
    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        (const GUI_RECT_T      *)&cur_rect,       
        (const GUI_RECT_T      *)&cur_rect,     
        (const MMI_STRING_T    *)&cur_str_t,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
        ); 

    MMITHEME_UpdateRect();
    return;
}

static MMI_RESULT_E HandleResetWaitWinZDT(
                                                 MMI_WIN_ID_T    win_id, // 窗口的ID
                                                 MMI_MESSAGE_ID_E   msg_id, // 窗口的内部消息ID
                                                 DPARAM             param   // 相应消息的参数
                                                 )
{
    MMI_RESULT_E    recode  =   MMI_RESULT_TRUE;
    static uint32   s_waitting_num = 0;
    static BOOLEAN is_cleandata =FALSE;
    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        s_waitting_num = 0;
       GUIWIN_SeSoftkeytButtonIconId(win_id, IMAGE_NULL,RIGHT_BUTTON,TRUE);
       GUIWIN_SeSoftkeytButtonIconId(win_id, IMAGE_NULL, LEFT_BUTTON, TRUE);
       GUIWIN_SeSoftkeytButtonTextId(win_id, IMAGE_NULL, MIDDLE_BUTTON, TRUE);
        //recode  =   MMI_RESULT_FALSE;
        recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
        break;

    case MSG_APP_RED:
        break;
        
    case MSG_SET_RESET_NEED_WAIT_IND:
        //add one wait action
        s_waitting_num++;
        break;
        
    case MSG_SET_RESET_FACTORY_OVER_IND:
        if(s_waitting_num > 0)
        {
            //finished one, wait number -1
            s_waitting_num --;
        }
        //no one to wait, finished
        if(s_waitting_num == 0)
        {
            //MMK_CloseWin(win_id);
            MMIAPIPHONE_PowerReset();
        }
        break;

    case MSG_SET_CLEAN_USER_DATA_OVER_IND:
        is_cleandata = TRUE;
        break;

    case MSG_CLOSE_WINDOW:
#ifdef BLUETOOTH_SUPPORT
        MMIAPIBT_ClearAllDevice();
#endif
        recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
        break;
    default:
        recode = MMIPUB_HandleAlertWinMsg(win_id, msg_id, param);
        break;
    }

    return recode;
}

void ZdtWatch_Reset_Start(MMI_WIN_ID_T win_id)
{
    MMI_WIN_ID_T                wait_win_id             =   0;

    wait_win_id = MMISET_RESET_FACTORY_WAITING_WIN_ID;
    {
        MMI_STRING_T    wait_text = {0};
        MMI_GetLabelTextByLang (STR_NOTE_WAITING, &wait_text);
        MMIPUB_OpenWaitWin (1, &wait_text, PNULL, PNULL, wait_win_id,
            COMMON_IMAGE_NULL, ANIM_PUBWIN_WAIT, WIN_ONE_LEVEL,
            MMIPUB_SOFTKEY_NONE, HandleResetWaitWinZDT);
    }

    MMK_PostMsg(win_id, MSG_SET_CLEAN_DATE_IND, PNULL,PNULL);
}

LOCAL MMI_RESULT_E  HandleZDT_WatchResetWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    uint8 * p_index = PNULL;
    p_index = (uint8 *) MMK_GetWinAddDataPtr(win_id);
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        break;
        
    case MSG_SET_CLEAN_DATE_IND:
        {
                //清除所有数据
                MMISET_CleanUserData();
                MMISET_ResetFactorySetting();
                MMK_CloseWin(win_id);
        }
        break;

    case MSG_FULL_PAINT:
            ZdtWatch_Reset_ShowData(win_id);
        break;
            
    case MSG_KEYDOWN_RED:
        break;
        
    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_KEYDOWN_CANCEL:
        break;
        
    case MSG_KEYUP_CANCEL:
        break;

    case MSG_CLOSE_WINDOW:
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

WINDOW_TABLE( MMIZDT_WATCH_RESET_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_WatchResetWinMsg),    
    WIN_ID( MMIZDT_WATCH_RESET_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

BOOLEAN MMIZDT_OpenResetFactoryWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_WATCH_RESET_WIN_TAB,PNULL);
    ZdtWatch_Reset_Start(MMIZDT_WATCH_RESET_WIN_ID);
    return TRUE;
}
#endif

#if 1 //充电窗口

#define ENG_TEST_VERSION 

LOCAL uint8 charging_image_index = 0;
LOCAL BOOLEAN test_mark = FALSE; //for test use
LOCAL uint16 click_count = 0 ;  //for test use
LOCAL uint8 charging_timer_id = 0;
LOCAL uint32 anim_total_count = 0;
LOCAL uint32 charging_win_open_ms = 0;
LOCAL BOOLEAN show_charge_tip = TRUE; 

LOCAL MMI_RESULT_E  HandleZDT_ChargingWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );

WINDOW_TABLE( MMIZDT_CHARGING_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_ChargingWinMsg),    
    WIN_ID( MMIZDT_CHARGING_WIN_ID ),
    WIN_STYLE(WS_DISABLE_FLING_CLOSE_WIN | WS_DISABLE_RETURN_WIN),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL void MMIZDT_StartChargingTimer()
{
    if(charging_timer_id != 0)
    {
        MMK_StopTimer(charging_timer_id);
    }
    charging_timer_id =  MMK_CreateWinTimer(MMIZDT_CHARGING_WIN_ID, 500,TRUE);

}

LOCAL void MMIZDT_StopChargingTimer()
{
    if(charging_timer_id != 0)
    {
        MMK_StopTimer(charging_timer_id);
    }
}

PUBLIC BOOLEAN MMIZDT_IsInChargingWin()
{
    MMI_WIN_ID_T win_id = MMK_GetFocusWinId() ;

    return win_id == MMIZDT_CHARGING_WIN_ID;
}

PUBLIC BOOLEAN MMIZDT_ShouldStayInChargingWin()
{
    BOOLEAN isCharging = ZDT_GetIsCharge() ;
    BOOLEAN isInChargingWin = MMIZDT_IsInChargingWin() ;
    return  isCharging || isInChargingWin;
}


PUBLIC void MMIZDT_CheckOpenChargingWin()
{
#ifdef ZDT_VIDEOCHAT_SUPPORT
    if(VideoChat_IsInCall())//解决视频通话中连接电源闪充电动画 更合理是挂断视频通话
    {
        return;
    }
#endif
    if(ZDT_GetIsCharge() && !test_mark && STARTUP_NORMAL == MMIAPIPHONE_GetStartUpCondition() && !MMICC_IsInActiveCallWin())
    {
        if(MMK_GetFocusWinId() != MMIZDT_CHARGING_WIN_ID && !ZDT_IsInFactoryWin())//工厂测试不进充电界面
        {
            if(MMK_IsOpenWin(MMIZDT_CHARGING_WIN_ID))
            {
                //MMK_CloseWin(MMIZDT_CHARGING_WIN_ID);
                MMK_WinGrabFocus(MMK_ConvertIdToHandle(MMIZDT_CHARGING_WIN_ID)); 
            }
            else
            {
                uint32 cur_ms = MMIAPICOM_GetFfsTime();
                if(z_abs(cur_ms - charging_win_open_ms) > 300)
                {
                    charging_win_open_ms = cur_ms;
                    #ifdef CAMERA_SUPPORT
                        #if defined(VIDEO_CALL_AGORA_IN_CAMERA_SCREEN_INCOMMING_ERR)
                        External_CloseDCApplet();// 退出CAMER CLOSE 
                        #endif
                    #endif
                    MMK_CreateWin((uint32*)MMIZDT_CHARGING_WIN_TAB,PNULL);
                }
  
            }
        }
    }
}


PUBLIC void MMIZDT_OpenChargingWin()
{
#ifdef ZDT_VIDEOCHAT_SUPPORT
   if(VideoChat_IsInCall())//解决视频通话中连接电源闪充电动画 更合理是挂断视频通话
   {
      return;
   }
#endif
   if(MMK_GetFocusWinId() != MMIZDT_CHARGING_WIN_ID && !ZDT_IsInFactoryWin())//工厂测试不进充电界面
   {

        //MMK_CreateWin((uint32*)MMIZDT_CHARGING_WIN_TAB,PNULL);
        uint32 cur_ms = MMIAPICOM_GetFfsTime();
        if(z_abs(cur_ms - charging_win_open_ms) > 300)
        {
            charging_win_open_ms = cur_ms;
            #ifdef CAMERA_SUPPORT
                #if defined(VIDEO_CALL_AGORA_IN_CAMERA_SCREEN_INCOMMING_ERR)
                External_CloseDCApplet();// 退出CAMER CLOSE 不退出在相机预览界面闪现充电动画
                #endif
            #endif
            if(MMK_IsOpenWin(MMIZDT_CHARGING_WIN_ID))
            {
                MMK_CloseWin(MMIZDT_CHARGING_WIN_ID);
            }
            MMK_CreateWin((uint32*)MMIZDT_CHARGING_WIN_TAB,PNULL);
        }

   }
}

#ifdef ZTE_WATCH
//电量低于20%时自动进入长续航模式这里check 一下自动退出
PUBLIC void MMIZDT_CheckLowBatteryMode()
{
    if(MMIZDT_ShouldStayInChargingWin() || MMIZDT_ShouldStayInClassModeWin()) //上课禁用和充电中不用管
    {
        SCI_TRACE_LOW("MMIZDT_CheckLowBatteryMode in charging or class mode");
    }
    else
    {
       uint8 battery = ZDT_GetBatteryPercent();
       uint8 mode = MMISET_GetLongRangeMode();
       uint8 auto_low_battery_flag = MMIZDT_Get_Auto_Low_Battery_Flag();
       uint8 auto_low_battery_status = MMIZDT_Get_Auto_Low_Battery_Status();
       if(battery <= MMIPHONE_CHARGE_WARNING_CAP_LOW_POWER && mode == 0 && auto_low_battery_status == 1)
       {
           //MMK_ReturnIdleWin();
           SCI_TRACE_LOW("[%s] go to low battery battery:%d mode:%d, flag:%d, status:%d, ",__FUNCTION__,battery,mode,auto_low_battery_flag,auto_low_battery_status);
           MMIZDT_NV_Set_Auto_Low_Battery_Flag(1);
           MMISET_SetLongRangeMode(1);
           WatchLAUNCHER_ZTE_LowBatterSwitch();
       }
       else if(auto_low_battery_flag == 1 && battery > MMIPHONE_CHARGE_WARNING_CAP_LOW_POWER)
       {
           //MMK_ReturnIdleWin();
           SCI_TRACE_LOW("[%s]  go out low battery battery:%d mode:%d, flag:%d, status:%d, ",__FUNCTION__,battery,mode,auto_low_battery_flag,auto_low_battery_status);
           MMIZDT_NV_Set_Auto_Low_Battery_Flag(0);
           MMISET_SetLongRangeMode(0);
           WatchLAUNCHER_ZTE_LowBatterSwitch();
       }
       else if(auto_low_battery_status == 0 && auto_low_battery_flag == 1) //电量低于20%自动进长续航 APP端关闭长续航
       {
           SCI_TRACE_LOW("[%s]  go out low battery battery:%d mode:%d, flag:%d, status:%d, ",__FUNCTION__,battery,mode,auto_low_battery_flag,auto_low_battery_status);
           MMIZDT_NV_Set_Auto_Low_Battery_Flag(0);
           MMISET_SetLongRangeMode(0);
           WatchLAUNCHER_ZTE_LowBatterSwitch();
       }
    }
}

#endif


PUBLIC void MMK_CloseChargingWin()
{
    if(MMK_IsOpenWin(MMIZDT_CHARGING_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_CHARGING_WIN_ID);
    }
#ifdef ZTE_WATCH
    MMIZDT_CheckLowBatteryMode();
#endif
}


#ifdef ZTE_WATCH

#ifdef MAINLCD_DEV_SIZE_240X284
    #define TIME_RECT DP2PX_RECT(40,30,200,80)
    #define DATE_RECT DP2PX_RECT(40,90,200,110)
#ifdef BATTERY_PERCENT_TEXT_SHOW
    #define ANIM_RECT DP2PX_RECT(55, 119, 132, 191)
    #define BAT_RECT DP2PX_RECT(130,119,185,191)
#else
    #define ANIM_RECT DP2PX_RECT(81,119,159,191)
    #define BAT_RECT DP2PX_RECT(190,119,230,191)
#endif
    #define TIP_RECT DP2PX_RECT(40,195,200,250)
    #define CHARGE_TIP_ICON_RECT DP2PX_RECT(78,49,162,133)
    #define CHARGE_TIP_TEXT_RECT DP2PX_RECT(12,152,238,240)
    #define LONG_POWER_ICON_RECT DP2PX_RECT(106,205,134,233)
    #define LONG_POWER_TEXT_RECT DP2PX_RECT(60,233,180,264)
    #define IN_CLASS_ICON_RECT DP2PX_RECT(106,205,134,233)
    #define IN_CLASS_TEXT_RECT DP2PX_RECT(60,233,180,264)
#else
    #define TIME_RECT DP2PX_RECT(40,20,200,70)
    #define DATE_RECT DP2PX_RECT(40,80,200,100)
#ifdef BATTERY_PERCENT_TEXT_SHOW
    #define ANIM_RECT DP2PX_RECT(55, 99, 132, 171)
    #define BAT_RECT DP2PX_RECT(130,99,185,171)
#else
    #define ANIM_RECT DP2PX_RECT(81, 99, 159, 171)
    #define BAT_RECT DP2PX_RECT(190,99,230,171)
#endif
    #define TIP_RECT DP2PX_RECT(40,170,200,220)
    #define CHARGE_TIP_ICON_RECT DP2PX_RECT(78,27,162,111)
    #define CHARGE_TIP_TEXT_RECT DP2PX_RECT(12,130,238,218)
    #define LONG_POWER_ICON_RECT DP2PX_RECT(106,170,134,198)
    #define LONG_POWER_TEXT_RECT DP2PX_RECT(60,198,180,240)
    #define IN_CLASS_ICON_RECT DP2PX_RECT(106,170,134,198)
    #define IN_CLASS_TEXT_RECT DP2PX_RECT(60,198,180,240)
#endif

LOCAL void Draw_Charge_Tip(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO  lcd_dev_info)
{
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_RECT_T charge_tip_icon_rect = CHARGE_TIP_ICON_RECT;
    GUI_RECT_T  text_display_rect=CHARGE_TIP_TEXT_RECT;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T  tip_str = {0};
#ifdef CHGMNG_PSE_SUPPORT
    CHGMNG_STATE_INFO_T* p_chgmng_info = CHGMNG_GetModuleState();
#endif
    GUI_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
    GUIRES_DisplayImg(PNULL, &charge_tip_icon_rect,PNULL,win_id,IMAGE_ZDT_CHARGING_TIP_ICON,&lcd_dev_info);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
#ifdef CHGMNG_PSE_SUPPORT
    //if(p_chgmng_info->charging_stop_reason == CHGMNG_OVERTEMP || p_chgmng_info->charging_stop_reason == CHGMNG_HOTTEMP)
    if(p_chgmng_info->charging_temperature >= 1045)
    {
        MMI_GetLabelTextByLang(TXT_CHARGE_OVER_TEMP,&tip_str);
    }
    //else if(p_chgmng_info->charging_stop_reason == CHGMNG_LOWTEMP)
    else if(p_chgmng_info->charging_temperature <= 1000)
    {
        MMI_GetLabelTextByLang(TXT_CHARGE_LOW_TEMP,&tip_str);
    }
    else
#endif
    {
        MMI_GetLabelTextByLang(TXT_CHARGE_TIP,&tip_str);
    }
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&text_display_rect,&text_display_rect,&tip_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO);
}

LOCAL void Draw_Charge_Battery_Anim(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO  lcd_dev_info)
{
    GUI_RECT_T anim_rect = ANIM_RECT;
    MMI_IMAGE_ID_T img_id_list[15] = 
	{   IMAGE_ZDT_CHARGING_ANIM1,
        IMAGE_ZDT_CHARGING_ANIM2,
        IMAGE_ZDT_CHARGING_ANIM3,
        IMAGE_ZDT_CHARGING_ANIM4,
        IMAGE_ZDT_CHARGING_ANIM5,
        IMAGE_ZDT_CHARGING_ANIM6,
        IMAGE_ZDT_CHARGING_ANIM7,
        IMAGE_ZDT_CHARGING_ANIM8,
        IMAGE_ZDT_CHARGING_ANIM9,
        IMAGE_ZDT_CHARGING_ANIM10,
        IMAGE_ZDT_CHARGING_ANIM11,
        IMAGE_ZDT_CHARGING_ANIM12,
        IMAGE_ZDT_CHARGING_ANIM13,
        IMAGE_ZDT_CHARGING_ANIM14,
        IMAGE_ZDT_CHARGING_FULL
	};
    if(ZDT_GetIsBatFull() || ZDT_IsChargeDone())
	{
        charging_image_index = 14; 
        GUIRES_DisplayImg(PNULL,&anim_rect,PNULL,win_id,img_id_list[charging_image_index],&lcd_dev_info);
	}
	else
	{
        GUIRES_DisplayImg(PNULL,&anim_rect,PNULL,win_id,img_id_list[charging_image_index],&lcd_dev_info);
    }	      
}

LOCAL void Draw_Low_Battery_Tip(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO  lcd_dev_info)
{
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_str = {0};
    GUI_RECT_T long_power_icon = LONG_POWER_ICON_RECT;
    GUI_RECT_T long_power_text = LONG_POWER_TEXT_RECT;
    GUIRES_DisplayImg(PNULL,&long_power_icon,PNULL,win_id,IMAGE_ZDT_LONG_POWER_ICON,&lcd_dev_info);
    MMI_GetLabelTextByLang(TXT_CHARGE_LONG_POWER,&text_str);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_22;
    text_style.font_color = GUI_RGB2RGB565(40,176,84);
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&long_power_text,&long_power_text,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO); 
}

LOCAL void Draw_In_Class_Tip(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_str = {0};
    GUI_RECT_T in_class_icon_rect = IN_CLASS_ICON_RECT;
    GUI_RECT_T in_class_text_rect = IN_CLASS_TEXT_RECT;
    GUIRES_DisplayImg(PNULL,&in_class_icon_rect,PNULL,win_id,IMAGE_ZDT_IN_CLASS_ICON,&lcd_dev_info);
    MMI_GetLabelTextByLang(TXT_IN_CLASS,&text_str);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_22;
    text_style.font_color = GUI_RGB2RGB565(254,48,42);
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&in_class_text_rect,&in_class_text_rect,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO); 
}

LOCAL void Draw_Battery_Charging_Tip(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO  lcd_dev_info)
{
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_str = {0};
    GUI_RECT_T tip_text_rect = TIP_RECT;
    MMI_TEXT_ID_T tip_text_id = TXT_CHARGE_DISABLE;
    if(ZDT_GetIsBatFull() || ZDT_IsChargeDone())
    {
        tip_text_id = TXT_CHARGE_FULL;
    }
    MMI_GetLabelTextByLang(tip_text_id,&text_str);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_24;
    text_style.font_color = GUI_RGB2RGB565(800,800,800);
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&tip_text_rect,&tip_text_rect,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO);  
}

LOCAL void Draw_Charge_DateAndTime(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO  lcd_dev_info, BOOLEAN is_charging)
{
    char week_table[][20] =
    {
        "星期日",//       TXT_SHORT_IDLE_SUNDAY,
        "星期一",//       TXT_SHORT_IDLE_MONDAY,
        "星期二",//      TXT_SHORT_IDLE_TUESDAY,
        "星期三",//       TXT_SHORT_IDLE_WEDNESDAY,
        "星期四",//       TXT_SHORT_IDLE_THURSDAY,
        "星期五",//       TXT_SHORT_IDLE_FRIDAY,
        "星期六",//       TXT_SHORT_IDLE_SATURDAY
    };
    char c_tmp[20]={0};
    uint16 w_tmp[20] = {0};
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_RECT_T time_text_rect = TIME_RECT;
    GUI_RECT_T date_text_rect = DATE_RECT;
    GUI_RECT_T bat_text_rect = BAT_RECT;
    MMI_STRING_T text_str = {0};
    GUISTR_STYLE_T text_style = {0};
    SCI_DATE_T cur_date = {0};
    SCI_TIME_T cur_time = {0};
    uint32  bat_level ;
    CHGMNG_STATE_INFO_T* p_chgmng_info = CHGMNG_GetModuleState();

    if(!ZDT_IsRealCharingVol() && is_charging)
    {
        MMK_SendMsg(win_id, MSG_CLOSE_WINDOW, NULL);
        return;
    }
	GUI_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
#ifdef BATTERY_PERCENT_TEXT_SHOW
    bat_level = p_chgmng_info->bat_remain_cap;
    sprintf(c_tmp, "%d%%", bat_level);
    GUI_GBToWstr(w_tmp, c_tmp, strlen(c_tmp)+1);
    text_str.wstr_ptr = w_tmp;
	text_str.wstr_len = MMIAPICOM_Wstrlen(w_tmp);
    text_style.align = ALIGN_RVMIDDLE;
    text_style.font = SONG_FONT_22;
    text_style.font_color = GUI_RGB2RGB565(16,138,255);
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&bat_text_rect,&bat_text_rect,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO);
#endif

    TM_GetSysDate(&cur_date);
    TM_GetSysTime(&cur_time);

    sprintf(c_tmp,"%02d:%02d", cur_time.hour, cur_time.min);
    GUI_GBToWstr(w_tmp, c_tmp, strlen(c_tmp)+1);
    text_str.wstr_ptr = w_tmp;
	text_str.wstr_len = MMIAPICOM_Wstrlen(w_tmp);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_56;
    text_style.font_color = GUI_RGB2RGB565(255,255,255);
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&time_text_rect,&time_text_rect,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO);

    sprintf(c_tmp,"%02d月%02d日 %s", cur_date.mon, cur_date.mday,week_table[cur_date.wday]);
    GUI_GBToWstr(w_tmp, c_tmp, strlen(c_tmp)+1);
    text_str.wstr_ptr = w_tmp;
	text_str.wstr_len = MMIAPICOM_Wstrlen(w_tmp);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_24;
    text_style.font_color = GUI_RGB2RGB565(255,255,255);
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&date_text_rect,&date_text_rect,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO);
}

//电量低于20% 或者用户自己进去
PUBLIC MMI_RESULT_E  HandleZTE_LowBatteryWinMsg(MMI_WIN_ID_T  win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = {0};
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            break;
		case MSG_GET_FOCUS:
            MMK_PostMsg(win_id,MSG_FULL_PAINT,PNULL,PNULL);
	        break;
	    case MSG_FULL_PAINT:
	    {    
            charging_image_index = ZDT_GetBatteryPercent()*14/100; //这里偷懒了应该计算充电动画长度，如果充电动画改了这里就不准确了
            Draw_Charge_DateAndTime(win_id,lcd_dev_info,FALSE);
            Draw_Charge_Battery_Anim(win_id,lcd_dev_info);
            Draw_Low_Battery_Tip(win_id,lcd_dev_info);
        }
		break;
		
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
            break;
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            if(MMIDEFAULT_IsBacklightOn())
            {
                MMIDEFAULT_TurnOffBackLight();
                MMIDEFAULT_CloseAllLight_Watch();//close LCD
            }
            else
            {
                MMIDEFAULT_TurnOnBackLight();
            }
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_ZTE_LOWBATTERY_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZTE_LowBatteryWinMsg),    
    WIN_ID(MMI_ZTE_LOWBATTERY_WIN_ID),
    WIN_STYLE(WS_DISPATCH_TO_CHILDWIN |WS_DISABLE_RETURN_WIN | WS_DISABLE_FLING_CLOSE_WIN),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZTE_LowBatteryWinOpen(void)
{
    if(MMK_IsOpenWin(MMI_ZTE_LOWBATTERY_WIN_ID) )
    {
       MMK_CloseWin(MMI_ZTE_LOWBATTERY_WIN_ID);
    }
    MMK_CreateWin(MMI_ZTE_LOWBATTERY_WIN_TAB, NULL);
    MMK_WinGrabFocus(MMI_ZTE_LOWBATTERY_WIN_ID);
    MMK_SetFocusWin(MMK_ConvertIdToHandle(MMI_ZTE_LOWBATTERY_WIN_ID));
    
}

LOCAL void Draw_InClassWin(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO  lcd_dev_info)
{
    charging_image_index = ZDT_GetBatteryPercent()*14/100; //这里偷懒了应该计算充电动画长度，如果充电动画改了这里就不准确了
    Draw_Charge_DateAndTime(win_id,lcd_dev_info,FALSE);
    Draw_Charge_Battery_Anim(win_id,lcd_dev_info);
    Draw_In_Class_Tip(win_id,lcd_dev_info);
}

PUBLIC MMI_RESULT_E  HandleZTE_InClassWinMsg(MMI_WIN_ID_T  win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = {0};
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            break;
		case MSG_GET_FOCUS:
	        break;
	    case MSG_FULL_PAINT:
	    {    
            Draw_InClassWin(win_id,lcd_dev_info);
        }
		break;
		
        case MSG_TP_PRESS_DOWN:
        case MSG_TP_PRESS_UP:
        case MSG_TP_PRESS_MOVE:
            break;
        case MSG_CTL_MIDSK:
            break;
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
            break;
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            if(MMIDEFAULT_IsBacklightOn())
            {
                MMIDEFAULT_TurnOffBackLight();
                MMIDEFAULT_CloseAllLight_Watch();//close LCD
            }
            else
            {
                MMIDEFAULT_TurnOnBackLight();
            }
            break;
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_ZTE_INCLASS_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZTE_InClassWinMsg),    
    WIN_ID(MMI_ZTE_INCLASS_WIN_ID),
    WIN_STYLE(WS_DISABLE_FLING_CLOSE_WIN),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZTE_InClassWinOpen(void)
{
    if(MMK_IsOpenWin(MMI_ZTE_INCLASS_WIN_ID))
    {
       MMK_CloseWin(MMI_ZTE_INCLASS_WIN_ID);
    }
    MMK_CreateWin(MMI_ZTE_INCLASS_WIN_TAB, NULL);
    MMK_WinGrabFocus(MMI_ZTE_INCLASS_WIN_ID);
    MMK_SetFocusWin(MMK_ConvertIdToHandle(MMI_ZTE_INCLASS_WIN_ID));
    
}

PUBLIC void MMIZTE_InClassWinClose(void)
{
    if(MMK_IsOpenWin(MMI_ZTE_INCLASS_WIN_ID))
    {
       MMK_CloseWin(MMI_ZTE_INCLASS_WIN_ID);
    }
}

LOCAL MMI_RESULT_E  HandleZDT_ChargingWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    
    switch(msg_id) {
        case MSG_OPEN_WINDOW:

    	    charging_image_index = 0;
    	    anim_total_count = 0;
	        MMIZDT_StartChargingTimer();
	        click_count = 0;//for test
            show_charge_tip = TRUE;
            break;
		case MSG_GET_FOCUS:
		    charging_image_index = 0;
		    anim_total_count = 0;
	        MMIZDT_StartChargingTimer();
	        break;
	    case MSG_FULL_PAINT:
	    {    
#ifdef CHGMNG_PSE_SUPPORT
            CHGMNG_STATE_INFO_T* p_chgmng_info = CHGMNG_GetModuleState();
            if(p_chgmng_info->charging_temperature >= 1045)
            {
                charging_image_index = 0;
                anim_total_count = 0;
                click_count = 0;
                show_charge_tip = TRUE;
                Draw_Charge_Tip(win_id,lcd_dev_info);
            }
            else if(p_chgmng_info->charging_temperature <= 1000)
            {
                charging_image_index = 0;
                anim_total_count = 0;
                click_count = 0;
                show_charge_tip = TRUE;
                Draw_Charge_Tip(win_id,lcd_dev_info);
            }
            else
#endif
            {
                if(charging_image_index < 6 && show_charge_tip)
                {
                    Draw_Charge_Tip(win_id,lcd_dev_info);
                }
                else
                {
                    show_charge_tip = FALSE;
                    Draw_Charge_DateAndTime(win_id,lcd_dev_info,TRUE);
                    Draw_Charge_Battery_Anim(win_id,lcd_dev_info);
                    Draw_Battery_Charging_Tip(win_id,lcd_dev_info);
                }
            }
        }
		break;
		
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
           
            #ifdef ENG_TEST_VERSION
            if(point.x < 80 && point.y > 160)
            {
              click_count += 1;
            }
            if(click_count >= 9)
            {
              test_mark = TRUE;
              
              MMK_CloseWin(win_id);
            }
            #endif
        }
        break;
        
        case MSG_TP_PRESS_MOVE:
            {
                GUI_POINT_T   point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
            }
            break;
        case MSG_TP_PRESS_LONG:
            {
                GUI_POINT_T   point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
            }
            break;
        case MSG_CTL_MIDSK:
            break;
        case MSG_TIMER:
        {
            if (*(uint8*)param == charging_timer_id && MMK_GetFocusWinId() == win_id)
            {
                charging_image_index++;
                if(charging_image_index >= 14)
                {
                    charging_image_index = 0;
                }
                anim_total_count++;
                SCI_TRACE_LOW("yyu anim_total_count=%d",anim_total_count);
                if(anim_total_count == 1)
                {
                    SCI_TRACE_LOW("yyu MMIDEFAULT_StartLcdBackLightTimer");
                }
                if(anim_total_count >= 14)
                {
                    SCI_TRACE_LOW("yyu MMIDEFAULT_StartLcdBackLightTimer2");
                    anim_total_count = 0;    
                }
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
        }
        
            break;
        case MSG_BACKLIGHT_TURN_ON:
        case MSG_BACKLIGHT_TURN_OFF:
            anim_total_count = 0;
            break;
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
        { 
            MMIZDT_StopChargingTimer();
            anim_total_count = 0;
        }   
            break;
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            if(MMIDEFAULT_IsBacklightOn())
            {
                MMIDEFAULT_TurnOffBackLight();
                MMIDEFAULT_CloseAllLight_Watch();//close LCD
            }
            else
            {
                MMIDEFAULT_TurnOnBackLight();
            }
            break;
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
#else
LOCAL MMI_RESULT_E  HandleZDT_ChargingWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T        str_data = {0};
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUISTR_INFO_T       text_info = {0};
    wchar  pedo_wstr[16]        = {0};
    uint8 pedo_str[16] = {0};
    uint16  pedo_len=0;
    MMI_STRING_T pedo_string = {0};
	
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            //MMK_SetAtvCtrl(win_id,ctrl_id);
            //GUIRICHTEXT_SetFocusMode(ctrl_id,GUIRICHTEXT_FOCUS_SHIFT_ADVANCED);			
	    //GUICTRL_SetProgress(MMK_GetCtrlPtr(ctrl_id),FALSE);//hide scroll bar
    	    charging_image_index = 0;
    	    anim_total_count = 0;
	        MMIZDT_StartChargingTimer();
	        click_count = 0;//for test
	        //MMIDEFAULT_StartLcdBackLightTimer(); //yangyu add for bug
            break;
		case MSG_GET_FOCUS:
		    charging_image_index = 0;
		    anim_total_count = 0;
	        MMIZDT_StartChargingTimer();
	        //MMIDEFAULT_StartLcdBackLightTimer(); //yangyu add for bug
	        break;
	case MSG_FULL_PAINT:
	{
	        GUI_RECT_T anim_rect = {23, 23, 217, 217};
	        MMI_IMAGE_ID_T img_id_list[8] = 
	        {
                IMAGE_ZDT_CHARGING_ANIM1,
                IMAGE_ZDT_CHARGING_ANIM2,
                IMAGE_ZDT_CHARGING_ANIM3,
                IMAGE_ZDT_CHARGING_ANIM4,
                IMAGE_ZDT_CHARGING_ANIM5,
                IMAGE_ZDT_CHARGING_ANIM6,
                IMAGE_ZDT_CHARGING_ANIM7,
                IMAGE_ZDT_CHARGING_FULL
	        };
	        if(!ZDT_IsRealCharingVol())
	        {
                MMK_SendMsg(win_id, MSG_CLOSE_WINDOW, NULL);
                break;
	        }
	        
	        GUI_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
	        //ZDT_GetBatteryPercent();
	        if(ZDT_GetIsBatFull() || ZDT_IsChargeDone() || ZDT_GetBatteryPercent() == 100)
	        {
               charging_image_index = 7; 
               GUIRES_DisplayImg(PNULL,
                &bg_rect,
                PNULL,
                win_id,
                img_id_list[charging_image_index],
                &lcd_dev_info);
	        }
	        else
	        {
                GUIRES_DisplayImg(PNULL,
                    &anim_rect,
                    PNULL,
                    win_id,
                    img_id_list[charging_image_index],
                    &lcd_dev_info);
                GUIRES_DisplayImg(PNULL,
                    &bg_rect,
                    PNULL,
                    win_id,
                    IMAGE_ZDT_CHARGING_BG,
                    &lcd_dev_info);
            }	        
    }
		break;
		
#if 1//def TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
           
            #ifdef ENG_TEST_VERSION
            if(point.x < 80 && point.y > 160)
            {
              click_count += 1;
            }
            if(click_count >= 9)
            {
              test_mark = TRUE;
              
              MMK_CloseWin(win_id);
            }
            #endif
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

        //case MSG_APP_OK:
        //case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            break;
        case MSG_TIMER:
        {
            if (*(uint8*)param == charging_timer_id)
            {
                charging_image_index++;
                if(charging_image_index >= 7)
                {
                    charging_image_index = 0;
                }
                anim_total_count++;
                SCI_TRACE_LOW("yyu anim_total_count=%d",anim_total_count);
                if(anim_total_count == 1)
                {
                    SCI_TRACE_LOW("yyu MMIDEFAULT_StartLcdBackLightTimer");
                   // MMIDEFAULT_StartLcdBackLightTimer();
                }
                if(anim_total_count >= 14)
                {
                    SCI_TRACE_LOW("yyu MMIDEFAULT_StartLcdBackLightTimer2");
                    anim_total_count = 0;
                    //MMK_SendMsg(win_id, MSG_APP_CANCEL, PNULL);
                    
                }

              
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
        }
        
        break;
        case MSG_BACKLIGHT_TURN_ON:
        case MSG_BACKLIGHT_TURN_OFF:
            anim_total_count = 0;
        break;
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
        { 
            MMIZDT_StopChargingTimer();
            anim_total_count = 0;
        }
        
        break;
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            //MMIDEFAULT_TurnOffBackLight();//yangyu add   
            if(MMIDEFAULT_IsBacklightOn())
            {
                MMIDEFAULT_TurnOffBackLight();
                MMIDEFAULT_CloseAllLight_Watch();//close LCD
            }
            else
            {
                MMIDEFAULT_TurnOnBackLight();
            }
            break;

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
#endif
#endif 


#if 1 //上课禁用

LOCAL uint8 classmode_timer_id = 0;
LOCAL uint16 classwin_click_count = 0;

LOCAL MMI_RESULT_E  HandleZDT_ClassModeWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );

WINDOW_TABLE( MMIZDT_CLASSMODE_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_ClassModeWinMsg),    
    WIN_ID( MMIZDT_CLASSMODE_WIN_ID ),
    WIN_STYLE( WS_DISABLE_FLING_CLOSE_WIN | WS_DISABLE_RETURN_WIN),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL void MMIZDT_StartClassModeTimer()
{
    if(classmode_timer_id != 0)
    {
        MMK_StopTimer(classmode_timer_id);
    }
    classmode_timer_id =  MMK_CreateWinTimer(MMIZDT_CLASSMODE_WIN_ID, 60*1000,TRUE);

}

LOCAL void MMIZDT_StopClassModeTimer()
{
    if(classmode_timer_id != 0)
    {
        MMK_StopTimer(classmode_timer_id);
    }
}

PUBLIC BOOLEAN MMIZDT_IsInClassModeWin()
{
    BOOLEAN ret = FALSE;
    MMI_WIN_ID_T win_id = MMK_GetFocusWinId() ;

    if( win_id == MMIZDT_CLASSMODE_WIN_ID)
    {
        ret = TRUE;
    }
    return ret;
}

PUBLIC BOOLEAN MMIZDT_ShouldStayInClassModeWin()
{
    BOOLEAN ret = FALSE;
    BOOLEAN isInClassModeWin = FALSE;
#if defined(ZDT_PLAT_YX_SUPPORT_YS) || defined(ZDT_PLAT_YX_SUPPORT_FZD) || defined(ZDT_PLAT_YX_SUPPORT_TX)
    ret = YX_IsInClassSilentTime();
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ) || defined(ZDT_PLAT_YX_SUPPORT_CY)
    ret = YX_DB_TIMER_IsValid_Class();
#endif
    isInClassModeWin = MMIZDT_IsInClassModeWin() ;
    return  ret && isInClassModeWin;
}


PUBLIC void MMIZDT_CheckOpenClassModeWin()
{
#if defined(ZDT_PLAT_YX_SUPPORT_YS) || defined(ZDT_PLAT_YX_SUPPORT_FZD) || defined(ZDT_PLAT_YX_SUPPORT_TX)
    if(YX_IsInClassSilentTime() /*&& !test_mark*/ && STARTUP_NORMAL == MMIAPIPHONE_GetStartUpCondition())
    {
        if(MMK_GetFocusWinId() != MMIZDT_CLASSMODE_WIN_ID && !MMICC_IsInActiveCallWin() && !MMIZDT_ShouldStayInChargingWin())
        {
            if(MMK_IsOpenWin(MMIZDT_CLASSMODE_WIN_ID))
            {
                MMK_CloseWin(MMIZDT_CLASSMODE_WIN_ID);
            }
            #ifdef CAMERA_SUPPORT
                #if defined(VIDEO_CALL_AGORA_IN_CAMERA_SCREEN_INCOMMING_ERR)
                External_CloseDCApplet();// 退出CAMER CLOSE
                #endif
            #endif
            MMK_CreateWin((uint32*)MMIZDT_CLASSMODE_WIN_TAB,PNULL);
        }
    }
    else
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_LZ)
    if(YX_DB_TIMER_IsValid_Class() /*&& !test_mark*/ && STARTUP_NORMAL == MMIAPIPHONE_GetStartUpCondition()
        &&!(YX_API_SOSMtCall_IsValid()&&CC_INCOMING_CALL_STATE==MMICC_GetCurrentCallStatus())
	)
    {
        if(MMK_GetFocusWinId() != MMIZDT_CLASSMODE_WIN_ID && !MMICC_IsInActiveCallWin())
        {
            if(MMK_IsOpenWin(MMIZDT_CLASSMODE_WIN_ID))
            {
                MMK_CloseWin(MMIZDT_CLASSMODE_WIN_ID);
            }
            MMK_CreateWin((uint32*)MMIZDT_CLASSMODE_WIN_TAB,PNULL);
        }
    }
    else
#endif
#if defined(ZDT_PLAT_YX_SUPPORT_CY)
    if(YX_DB_TIMER_IsValid_Class() /*&& !test_mark*/ && STARTUP_NORMAL == MMIAPIPHONE_GetStartUpCondition()
        &&!(YX_API_SOSMtCall_IsValid()&&CC_INCOMING_CALL_STATE==MMICC_GetCurrentCallStatus())
	)
    {
        if(MMK_GetFocusWinId() != MMIZDT_CLASSMODE_WIN_ID && !MMICC_IsInActiveCallWin())
        {
            if(MMK_IsOpenWin(MMIZDT_CLASSMODE_WIN_ID))
            {
                MMK_CloseWin(MMIZDT_CLASSMODE_WIN_ID);
            }
            MMK_CreateWin((uint32*)MMIZDT_CLASSMODE_WIN_TAB,PNULL);
        }
    }
    else
#endif

    {
        if(MMK_IsOpenWin(MMIZDT_CLASSMODE_WIN_ID))
        {
            MMK_CloseWin(MMIZDT_CLASSMODE_WIN_ID);
        }
        MMIZDT_CheckLowBatteryMode();

    }
}

PUBLIC BOOLEAN MMIZDT_IsClassModeWinOpen()
{
    BOOLEAN ret = FALSE;
    if(MMK_IsOpenWin(MMIZDT_CLASSMODE_WIN_ID))
    {
        ret = TRUE;
    }
    return ret;

}
PUBLIC void MMIZDT_OpenClassModeWin()
{
   if(MMK_GetFocusWinId() != MMIZDT_CLASSMODE_WIN_ID)
   {
        #ifdef CAMERA_SUPPORT
            #if defined(VIDEO_CALL_AGORA_IN_CAMERA_SCREEN_INCOMMING_ERR)
            External_CloseDCApplet();// 退出CAMER CLOSE
            #endif
        #endif
        MMK_CreateWin((uint32*)MMIZDT_CLASSMODE_WIN_TAB,PNULL);
   }
}

PUBLIC void MMK_CloseClassModeWin()
{
    if(MMK_IsOpenWin(MMIZDT_CLASSMODE_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_CLASSMODE_WIN_ID);
    }
    MMIZDT_CheckLowBatteryMode();
}

LOCAL void DrawClassModeTime(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T anim_rect = {0,141, (MMI_MAINSCREEN_WIDTH -1), 141+78};
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    SCI_TIME_T time = {0};
    GUI_POINT_T point = {0};
    const uint16 img_width = 30;
    const uint16 img_height = 30;
    const uint16 blank_gap  = 0;
    const uint16 x_move = img_width + blank_gap;
    const uint16 start_x  = (MMI_MAINSCREEN_WIDTH - (x_move)*5)/2 - 1;
    uint16  mmi_main_time_y = 124;//(MMI_MAINSCREEN_HEIGHT - img_height)/2 -1;
    MMI_IMAGE_ID_T colon_img_id = IMAGE_CLASSMODE_NUM_COLON;
    MMI_IMAGE_ID_T num_img_id_list[10] = 
    {
        IMAGE_CLASSMODE_NUM_0,
        IMAGE_CLASSMODE_NUM_1,
        IMAGE_CLASSMODE_NUM_2,
        IMAGE_CLASSMODE_NUM_3,
        IMAGE_CLASSMODE_NUM_4,
        IMAGE_CLASSMODE_NUM_5,
        IMAGE_CLASSMODE_NUM_6,
        IMAGE_CLASSMODE_NUM_7,
        IMAGE_CLASSMODE_NUM_8,
        IMAGE_CLASSMODE_NUM_9
    };
    
    TM_GetSysTime(&time);

    point.x = start_x  ;
    point.y = mmi_main_time_y ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, num_img_id_list[time.hour/10],&lcd_dev_info);
    point.x += x_move;
    point.y = mmi_main_time_y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, num_img_id_list[time.hour%10],&lcd_dev_info);
    point.x += x_move;
    point.y = mmi_main_time_y ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, colon_img_id,&lcd_dev_info);
    point.x += x_move;
    point.y = mmi_main_time_y ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, num_img_id_list[time.min/10],&lcd_dev_info);
    point.x += x_move;
    point.y = mmi_main_time_y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, num_img_id_list[time.min%10],&lcd_dev_info);     


}

LOCAL MMI_RESULT_E  HandleZDT_ClassModeWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T        str_data = {0};
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;

	
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            classwin_click_count = 0;

	        MMIZDT_StartClassModeTimer();

            break;
		case MSG_GET_FOCUS:

	        MMIZDT_StartClassModeTimer();

	        break;
	case MSG_FULL_PAINT:
	{
    #ifdef ZTE_WATCH
        Draw_InClassWin(win_id,lcd_dev_info);
    #else
	    GUI_FillRect(&lcd_dev_info,bg_rect, MMI_BLACK_COLOR);    
        GUIRES_DisplayImg(PNULL,&bg_rect,PNULL,win_id,IMAGE_CLASSMODE_BG,&lcd_dev_info);
        DrawClassModeTime(win_id);
    #endif
    }
		break;
		
#if 1//def TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
           
            #ifdef ENG_TEST_VERSION
            if(point.x < 80 && point.y > 160)
            {
              classwin_click_count += 1;
            }
            if(classwin_click_count >= 25)
            {
              //test_mark = TRUE;
              
              MMK_CloseWin(win_id);
#ifdef _SW_ZDT_PRODUCT_
            //MMIZDT_OpenSettingWin();
#else
            //MMIAPIMENU_EnterSetting();
#endif
            }
            #endif
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

        //case MSG_APP_OK:
        //case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            break;
        case MSG_TIMER:
        {
            if (*(uint8*)param == classmode_timer_id)
            {
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
        }
        
        break;
        case MSG_BACKLIGHT_TURN_ON:
        case MSG_BACKLIGHT_TURN_OFF:

        break;
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
        { 
            MMIZDT_StopClassModeTimer();

        }
        break;
        case MSG_KEYDOWN_RED:
            break;      
        
        case MSG_KEYUP_RED:
            if(MMIDEFAULT_IsBacklightOn())
            {
                MMIDEFAULT_TurnOffBackLight();
                MMIDEFAULT_CloseAllLight_Watch();//close LCD
            }
            else
            {
                MMIDEFAULT_TurnOnBackLight();
            }
            break;
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            //MMK_CloseWin(win_id);
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}
#endif 

#if 1 // 插入SIM卡的接口
LOCAL MMI_RESULT_E  HandleZDT_InsertSimWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );



WINDOW_TABLE( MMIZDT_INSERTSIM_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_InsertSimWinMsg),    
    WIN_ID( MMIZDT_INSERTSIM_WIN_ID ),
    WIN_HIDE_STATUS,
    WIN_STYLE(WS_DISABLE_FLING_CLOSE_WIN|WS_DISABLE_RETURN_WIN),
    END_WIN
};



PUBLIC BOOLEAN MMIZDT_IsInsertSimWin()
{
    MMI_WIN_ID_T win_id = MMK_GetFocusWinId() ;

    return win_id == MMIZDT_INSERTSIM_WIN_ID;
}


PUBLIC BOOLEAN MMIZDT_IsInsertSimWinOpen()
{
    BOOLEAN ret = FALSE;
    if(MMK_IsOpenWin(MMIZDT_INSERTSIM_WIN_ID))
    {
        ret = TRUE;
    }
    return ret;

}
PUBLIC void MMIZDT_OpenInsertSimWin()
{
   if(MMK_GetFocusWinId() != MMIZDT_INSERTSIM_WIN_ID)
   {
        MMK_CreateWin((uint32*)MMIZDT_INSERTSIM_WIN_TAB,PNULL);
   }
}

PUBLIC void MMK_CloseInsertSimWin()
{
    if(MMK_IsOpenWin(MMIZDT_CLASSMODE_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_CLASSMODE_WIN_ID);
    }
}


LOCAL MMI_RESULT_E  HandleZDT_InsertSimWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T        str_data = {0};
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
#if defined(ZDT_NO_SIM_ICON_GIF)// 未插SIM 卡提示图标用动画显示
    GUIANIM_CTRL_INFO_T             ctrl_info = {0};
    GUIANIM_DATA_INFO_T             data_info = {0};
    GUIANIM_DISPLAY_INFO_T          display_info = {0};
    GUIANIM_INIT_DATA_T             anim_init = {0};
    MMI_CONTROL_CREATE_T            anim_ctrl = {0};
    GUIANIM_RESULT_E        anim_result = GUIANIM_RESULT_SUCC;
#endif
	
    switch(msg_id) {
        case MSG_OPEN_WINDOW:

            break;
		case MSG_GET_FOCUS:
            if(ZDT_SIM_Exsit()) //提示插入SIM时 插入SIM卡关闭掉窗口
            {
            #ifdef SETUP_WIZARD_SUPPORT
                if(MMIZDT_Get_Setup_Wizard_Status() == 0) //没进过开机向导需要跳去开机向导
                {
                    Setup_Wizard_MainWin_Enter();
                }
                else
            #endif
                {
                    MMK_CloseWin(win_id);
                }
            }
	        break;
	case MSG_FULL_PAINT:
	{
	    //GUI_FillRect(&lcd_dev_info,bg_rect, MMI_BLACK_COLOR);    
        #if defined(ZDT_NO_SIM_ICON_GIF)// 未插SIM 卡提示图标用动画显示
        {
            ctrl_info.is_ctrl_id = TRUE;
            ctrl_info.ctrl_id =MMIZDT_INSERTSIM_ANIM_CTRL_ID;
            anim_init.both_rect = MMITHEME_GetFullScreenBothRect();   
            data_info.img_id = IMAGE_INSERT_SIM_TIP;     
            anim_ctrl.ctrl_id           = MMIZDT_INSERTSIM_ANIM_CTRL_ID;
            anim_ctrl.guid              = SPRD_GUI_ANIM_ID;
            anim_ctrl.init_data_ptr     = &anim_init;
            anim_ctrl.parent_win_handle = MMK_ConvertIdToHandle(MMIZDT_INSERTSIM_WIN_ID);
            MMK_CreateControl(&anim_ctrl);
        
            display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
            display_info.is_play_once = FALSE;
            display_info.is_bg_buf      = TRUE;
            display_info.bg.bg_type     = GUI_BG_COLOR;
            display_info.bg.color       = MMI_WINDOW_BACKGROUND_COLOR;
             display_info.is_update = TRUE;
            anim_result = GUIANIM_SetParam(&ctrl_info,&data_info,PNULL,&display_info);
        }
        #else
        LCD_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
        GUIRES_DisplayImg(PNULL,
            &bg_rect,
            PNULL,
            win_id,
            IMAGE_INSERT_SIM_TIP,
            &lcd_dev_info);
        #endif

    }
		break;
		
#if 1//def TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
           
           
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

        //case MSG_APP_OK:
        //case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            break;

        case MSG_BACKLIGHT_TURN_ON:
        case MSG_BACKLIGHT_TURN_OFF:

        break;
        case MSG_LOSE_FOCUS:
            break;
        case MSG_CLOSE_WINDOW:
            WatchOpen_IdleWin();
            break;
        case MSG_KEYDOWN_RED:
            break;    
			  
        case MSG_TP_PRESS_FLING:
        {
        	MMI_TP_MSG_PARAM_T    *para = PNULL;
        	para = param; // MMK_GetWinAddDataPtr(win_id);
        	if(para != PNULL)
        	{
        		if(para->tp_fling_direction ==  TP_FLING_RIGHT)
        		{
        			MMK_CloseWin(win_id);
        		}
        	}
        }
		break;
        case MSG_KEYUP_RED:			//bug   待机界面按红键会去主菜单
                MMK_CloseWin(win_id);
      	    break;
			
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            //WatchOpen_IdleWin();
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}
#endif

#if 1 // TP测试接口
LOCAL MMI_RESULT_E  HandleZDT_TestTpWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );


WINDOW_TABLE( MMIZDT_TEST_TP_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_TestTpWinMsg),    
    WIN_ID( MMIZDT_TEST_TP_WIN_ID ),
    //WIN_TITLE(TXT_ZDT_TEST),
    //WIN_STATUSBAR,
    //WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    //CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMIZDT_TEST_MENU_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZDT_OpenTestTpWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_TEST_TP_WIN_TAB,PNULL);
}

LOCAL void drawTpPoint(GUI_POINT_T   point, BOOLEAN isMove)
{
    GUI_RECT_T rect = {0};
    GUI_RECT_T full_rect = {0, 0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_WIDTH -1)};
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    rect.left = point.x;
    rect.top = point.y;
    rect.right = rect.left+1;
    rect.bottom = rect.top+1;
    if(!isMove)
    {
        GUI_FillRect(&lcd_dev_info, full_rect, MMI_BLACK_COLOR);

    }
    GUI_FillRect(&lcd_dev_info, rect, MMI_WHITE_COLOR);
}

LOCAL void drawTpUpPoint(GUI_POINT_T   point)
{
    GUI_RECT_T rect = {0};
    GUI_RECT_T full_rect = {0, 0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_WIDTH -1)};
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    rect.left = point.x;
    rect.top = point.y;
    rect.right = rect.left+1;
    rect.bottom = rect.top+1;
  
    GUI_FillRect(&lcd_dev_info, rect, MMI_BLUE_COLOR);
}




LOCAL MMI_RESULT_E  HandleZDT_TestTpWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T            ctrl_id = MMIZDT_TEST_MENU_CTRL_ID;
    uint16                    node_id = 0;
    static BOOLEAN            s_need_update   = FALSE;
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
  //  MMISET_VALIDATE_PRIVACY_RETURN_T    result_info = {MMISET_PROTECT_CL, FALSE};    
#if defined(DISABLE_STATUSBAR_DISPLAY) 
    GUI_RECT_T                          title_rect =   {0, 0, (MMI_MAINSCREEN_WIDTH -1), 23};
#else
    GUI_RECT_T                          title_rect = {0, 18, (MMI_MAINSCREEN_WIDTH -1), 18 + 23};
#endif
    GUI_RECT_T                          full_rect = {0, 0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_WIDTH -1)};

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
    {
      
    }

        break;
    case MSG_FULL_PAINT:
    {
        GUI_FillRect(&lcd_dev_info, full_rect, MMI_BLACK_COLOR);
    }

        break;        
    case MSG_TP_PRESS_DOWN:
    {
        GUI_POINT_T   point = {0};
        point.x = MMK_GET_TP_X(param);
        point.y = MMK_GET_TP_Y(param);
        drawTpPoint(point, FALSE);

    }          
        break;        
    case MSG_TP_PRESS_MOVE:
    {
       GUI_POINT_T   point = {0};
       point.x = MMK_GET_TP_X(param);
       point.y = MMK_GET_TP_Y(param);
       drawTpPoint(point, TRUE);

    }          
        break;
    case MSG_TP_PRESS_UP:
    {
        GUI_POINT_T   point = {0};
       point.x = MMK_GET_TP_X(param);
       point.y = MMK_GET_TP_Y(param);
       drawTpUpPoint(point);

    }          
        break;        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {

    }          
        break;

    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
            //MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}
#endif

#ifdef BAIDU_AI_SUPPORT
#include "lightduer_log.h"
#include "duerapp_homepage.h"
PUBLIC void MMIAPIMENU_EnterAiChat(void)
{
#ifndef WIN32
    extern int duer_set_device_name(const char *device_name);
    duer_set_device_name("ZTE"); 
    MMIDUERAPP_CreateWinEnter();
#endif
}
#endif

#endif


#ifdef CHGMNG_PSE_SUPPORT
PUBLIC void MMIZDT_CheckTempAlert(void)
{
    CHGMNG_STATE_INFO_T* p_chgmng_info = CHGMNG_GetModuleState();
    //if(p_chgmng_info->charging_stop_reason == CHGMNG_OVERTEMP || p_chgmng_info->charging_stop_reason == CHGMNG_HOTTEMP)
    if(p_chgmng_info->charging_temperature >= 1045)
    {
        if(FALSE == MMK_IsFocusWin(WATCH_LAUNCHER_PANEL_WIN_ID) && FALSE == ZDT_IsInFactoryWin())
        {
            if(MMICC_GetCallNum() > 0)
            {
                MMIAPICC_ReleaseCallByRedkey();
                MMIPUB_OpenAlertTimerWin(TXT_TEMP_AERT_LONG);
            }
            else
            {
                MMK_ReturnIdleWin();
            #ifdef ZDT_VIDEOCHAT_SUPPORT
                Video_Call_Check();//视频通话不允许return idle 所以这里判断挂断一下
            #endif
                MMIPUB_OpenAlertTimerWin(TXT_TEMP_AERT_LONG);
            }
        }
    }

    return;
}
#endif

#ifdef ZTE_WATCH

LOCAL uint8 get_index_with_point(GUI_POINT_T point, uint8 call_volume_index)
{
    uint8 cur_item_index = 0;
    uint8  volume_item_total = MMISET_VOL_MAX+1;
    GUI_RECT_T progressrect = SET_PROGRESSBAR_RECT;
    GUI_RECT_T prgtouchrect = SET_PROGRESSBAR_TOUCHRECT;
    cur_item_index =  ((float)(volume_item_total)*(point.x - progressrect.left)/(progressrect.right-progressrect.left)); 
    if(cur_item_index > volume_item_total)
    {
        cur_item_index = volume_item_total -1;
    }
    if(cur_item_index < 0)
    {
        cur_item_index = 0;
    }
    if(cur_item_index != call_volume_index)
    {
        if(MMISET_VOL_MAX < cur_item_index)
        {
            cur_item_index = MMISET_VOL_MAX;
        }
    }
    return cur_item_index;
}

LOCAL MMI_RESULT_E HandleCallVolumeSetWindow(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUI_POINT_T         display_point = {0};
    GUI_RECT_T img_rect = {0};

    GUI_RECT_T   rect = MMITHEME_GetFullScreenRectEx(win_id);	
    MMI_IMAGE_ID_T  res_progress_bg = IMAGE_ZTE_CONTROL_PROGRESS_BG;
    MMI_IMAGE_ID_T  res_progress_fg= IMAGE_ZTE_CONTROL_PROGRESS;
    MMI_IMAGE_ID_T  res_progress_hand = IMAGE_ZTE_PROGRESS_HAND;
    GUI_RECT_T progressrect = SET_PROGRESSBAR_RECT;
    GUI_RECT_T prgtouchrect = SET_PROGRESSBAR_TOUCHRECT;
    uint8      cur_item_index = 0;
    uint8  volume_item_total = MMISET_VOL_MAX+1;
    LOCAL call_volume_index = MMISET_VOL_SIX;
    LOCAL uint8 progress_down_flag = 0;
    LOCAL uint8 last_call_volume_index = 0;

    uint32 Volume_Level[6]={ 0,1,3,5,7,9};
    switch (msg_id)
    {
        case MSG_OPEN_WINDOW:
            progress_down_flag = 0;
            call_volume_index = ual_tele_call_get_call_volume();
            last_call_volume_index = call_volume_index;
            SCI_TRACE_LOW("[%s]: call volume=%d",__FUNCTION__,call_volume_index);
            if(MMISET_VOL_MAX < call_volume_index)
            {
                call_volume_index = MMISET_VOL_MAX;
            }
            WATCHCOM_ProgressBarByIndex(win_id, progressrect, call_volume_index,volume_item_total,res_progress_bg, res_progress_fg, res_progress_hand);
            break;
        case MSG_FULL_PAINT:
            LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
            display_point.x = BRIGHT_ICON_VOL_X; // 43*BIG_SCALE;//81*SCALE;//client_rect.v_rect.left;
            display_point.y = BRIGHT_ICON_VOL_Y; //  14*BIG_SCALE;//25*SCALE;///client_rect.v_rect.top;	

            GUIRES_DisplayImg(&display_point,PNULL,PNULL,win_id,IMAGE_VOLUME_ICON,&lcd_dev_info);
            WATCHCOM_ProgressBarByIndex(win_id, progressrect, call_volume_index,volume_item_total,res_progress_bg, res_progress_fg, res_progress_hand);
            break;
		
        case MSG_CTL_MIDSK:
        case MSG_KEYDOWN_OK:
        case MSG_CTL_OK:
        case MSG_APP_WEB:
            break;
        case MSG_TP_PRESS_DOWN:
            {
                GUI_POINT_T   point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if (GUI_PointIsInRect(point, prgtouchrect))
                {
                    progress_down_flag = 1;
                    cur_item_index = get_index_with_point(point,call_volume_index);                 
                    if(cur_item_index != call_volume_index)
                    {
                        call_volume_index = cur_item_index;
                        WATCHCOM_ProgressBarByIndex(win_id, progressrect, call_volume_index,volume_item_total,res_progress_bg, res_progress_fg, res_progress_hand);
                        MMK_UpdateScreen();
                    }
                }
            }
            break;
        
        case MSG_TP_PRESS_MOVE:
            {			
                GUI_POINT_T   point = {0};              
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if (progress_down_flag && GUI_PointIsInRect(point, prgtouchrect))
                {
                    cur_item_index = get_index_with_point(point,call_volume_index); 
                    SCI_TRACE_LOW("[%s]: move call volume=%d cur_item_index:%d",__FUNCTION__,call_volume_index, cur_item_index);
                    if(cur_item_index != call_volume_index)
                    {
                        call_volume_index = cur_item_index;
                        WATCHCOM_ProgressBarByIndex(win_id, progressrect, call_volume_index,volume_item_total,res_progress_bg, res_progress_fg, res_progress_hand);
                        SCI_TRACE_LOW("[%s]: call volume=%d",__FUNCTION__,call_volume_index);
                        ual_tele_call_set_call_volume(call_volume_index);
                        MMK_UpdateScreen();
                    }
                }
                else
                {
                    progress_down_flag = 0;
                }
            }
            break;
    
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T   point = {0};              
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if (progress_down_flag && GUI_PointIsInRect(point, prgtouchrect))
                {
                    cur_item_index = get_index_with_point(point,call_volume_index); 
                    call_volume_index = cur_item_index;
                    WATCHCOM_ProgressBarByIndex(win_id, progressrect, call_volume_index,volume_item_total,res_progress_bg, res_progress_fg, res_progress_hand);
                    SCI_TRACE_LOW("[%s]: up call volume=%d",__FUNCTION__,call_volume_index);
                    ual_tele_call_set_call_volume(call_volume_index);
                    MMK_UpdateScreen();
                  
                }
                else
                {
                    progress_down_flag = 0;
                }
            }
            break;
  
        case MSG_KEYUP_RED:
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            break;
        case MSG_CLOSE_WINDOW:
            break;
	    case MSG_KEYDOWN_RED:
            break; 

	    default:
	        recode = MMI_RESULT_FALSE;
	        break;
    }
	return recode;
}

WINDOW_TABLE( MMIZDT_CALL_VOLUME_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleCallVolumeSetWindow),    
    WIN_ID( MMI_WATCH_CALL_VOLUME_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZDT_OpenCallVolumeWin()
{
   if(MMK_IsOpenWin(MMI_WATCH_CALL_VOLUME_WIN_ID))
   {
        MMK_CloseWin(MMI_WATCH_CALL_VOLUME_WIN_ID);
   }
   MMK_CreateWin((uint32*)MMIZDT_CALL_VOLUME_WIN_TAB,PNULL);
}

PUBLIC void MMIZDT_CheckCallVolumeWin()
{
   if(MMK_IsOpenWin(MMI_WATCH_CALL_VOLUME_WIN_ID))
   {
       MMK_CloseWin(MMI_WATCH_CALL_VOLUME_WIN_ID);
   }
}

#endif




#include "std_header.h"
#include <stdlib.h>
#include "window_parse.h"
#include "mmidisplay_data.h"
#include "mmiacc_text.h"
#include "mmipub.h"
#include "mmicc_id.h"
#include "mmi_modu_main.h"
#include "mmi_textfun.h"
#include "mmi_string.h"
#include "guilcd.h"
#include "guifont.h"
#include "guistring.h"
#include "guitext.h"
#include "guibutton.h"
#include "guilistbox.h"
#include "mmk_timer.h"
#include "mmi_common.h"
#include "mmiacc_event.h"
#include "alarmClock_export.h"
#include "alarmClock_image.h"
#include "alarmClock_text.h"
#include "alarmClock_id.h"
#include "alarmClock_nv.h"
#include "mmialarm_export.h"
#include "mmischedule_export.h"
#include "mmicc_export.h"
#include "mmicc_id.h"
#include "mmi_appmsg.h"
#include "mmikl_export.h"
#include "mmimms_export.h"
#include "mmiset_export.h"

#define ALERT_RING_DURATION 10000

LOCAL uint8 alarmclock_ring_timer_id = 0;
LOCAL uint16 clock_alive_tp_x = 0;
LOCAL uint16 clock_alive_tp_y = 0;

static void MMIAlarmClock_BackLight(BOOLEAN is_alway_on)
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

LOCAL MMI_RESULT_E _callBack_ClickDelayAlarmClock()
{
	MMI_WIN_ID_T win_id = MMI_ALARMCLOCK_ALARM_CLOCK_WIN_ID;
	SETTINGS_ALARM_ARRIVED_WIN* clock_event = {0};
	BOOLEAN is_click_delay = TRUE;

	clock_event = (SETTINGS_ALARM_ARRIVED_WIN *)MMK_GetWinAddDataPtr( win_id );

	if (NULL == clock_event)
	{
		SCI_TRACE_LOW("_callBack_ClickDelayAlarmClock arrived_event == NULL");
		return MMI_RESULT_FALSE;
	}

	MMI_WriteNVItem(MMINV_ALARMCLOCK_IS_DELAY_NV,&is_click_delay);

	StopRingOrVib(TRUE);
	SetDelayAlarmTime(clock_event->event_id, &clock_event->s_arrived_event, FALSE);
#ifdef CALENDAR_SUPPORT
	MMIAPISCH_DeleteUnreadEvent(clock_event->event_id);
#endif

	if(((clock_event->s_arrived_event.event_fast_info.is_valid)
		&& !MMIAPICC_IsInState(CC_IN_CALL_STATE))
		&&(0==clock_event->sch_list_total))
	{
		CheckandStartExpiredEvent();
	}else
	{
		CheckandStartExpiredEvent();
	}
	MMK_CloseWin(win_id);
}

LOCAL void AlarmClockRing_TimerCallback(uint8 timer_id, uint32 param)
{
	if(timer_id == alarmclock_ring_timer_id){
		MMK_StopTimer(alarmclock_ring_timer_id);
		alarmclock_ring_timer_id = 0;
		_callBack_ClickDelayAlarmClock();
	}
}

LOCAL MMI_RESULT_E HandleAlarmClockWinMsg_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
	GUI_LCD_DEV_INFO	lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T		ctrl_id = MMI_ALARMCLOCK_EXTEND_CTRL_ID;
	uint8				s_event_timer_id = 0;
	MMI_HANDLE_T		win_handle = 0;

	if(alarmclock_ring_timer_id){
		MMK_StopTimer(alarmclock_ring_timer_id);
	}
	alarmclock_ring_timer_id = MMK_CreateTimerCallback(60*1000, AlarmClockRing_TimerCallback, PNULL, FALSE);

	StartRingOrVib();
}

LOCAL MMI_RESULT_E HandleAlarmClockWinMsg_FULL_PAINT(MMI_WIN_ID_T win_id)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	GUI_RECT_T clientRect = MMITHEME_GetFullScreenRect();
	GUI_RECT_T win_rect = {0};
#ifdef ZTE_SUPPORT_240X284
	GUI_RECT_T img_rect = {70,92,200,200};
	GUI_RECT_T bottom_img_rect = {12,214,240,260};
#elif ZTE_SUPPORT_240X240
	GUI_RECT_T img_rect = {70,72,200,180};
	GUI_RECT_T bottom_img_rect = {12,190,240,236};
#endif
	GUI_POINT_T start_point = {0,175};
	GUI_POINT_T clock_point = {-5,95};
	MMI_STRING_T close_text = {0};
	MMI_STRING_T delay_text = {0};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T clock_text = {0};

	win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, clientRect);
	GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);

	{
		SETTINGS_ALARM_ARRIVED_WIN* arrived_event = {0};
		MMI_STRING_T clock_text = {0};
		GUI_RECT_T txt_rect = {40, 20, 200, 50};
		char clock_time[20] = {0};
		wchar clock_wchar[20] = {0};
		uint8 clock_hour = 0;
		uint8 clock_minute = 0;	

		arrived_event = (SETTINGS_ALARM_ARRIVED_WIN *)MMK_GetWinAddDataPtr( win_id );
		if (NULL == arrived_event)
		{
			SCI_TRACE_LOW("HandleAlarmClockWinMsg_FULL_PAINT NULL == arrived_event");
			return MMI_RESULT_FALSE;
		}

		text_style.align = ALIGN_HVMIDDLE;
		text_style.font_color = MMI_WHITE_COLOR;
		text_style.font = SONG_FONT_16;
		MMIRES_GetText(TXT_CUR_ALARM, win_id, &clock_text);
		GUISTR_DrawTextToLCDInRect(
			&lcd_dev_info,
			&txt_rect,
			&txt_rect,
			&clock_text,
			&text_style,
			text_state,
			GUISTR_TEXT_DIR_AUTO
			);

		SCI_TRACE_LOW("HandleAlarmClockWinMsg_FULL_PAINT event_fast_info.hour = %d",arrived_event->s_arrived_event.event_fast_info.hour);
		SCI_TRACE_LOW("HandleAlarmClockWinMsg_FULL_PAINT event_fast_info.minute = %d",arrived_event->s_arrived_event.event_fast_info.minute);
		clock_hour = arrived_event->s_arrived_event.event_fast_info.hour;
		clock_minute = arrived_event->s_arrived_event.event_fast_info.minute;

		MMIAPISET_FormatTimeStrByTime(clock_hour,clock_minute,(uint8*)clock_time,MMIALM_TIME_STR_12HOURS_LEN + 1);
		SCI_TRACE_LOW("HandleAlarmClockWinMsg_FULL_PAINT clock_time = %s",clock_time);

		clock_text.wstr_ptr = MMIAPICOM_StrToWstr((uint8 *)clock_time,clock_wchar);
		clock_text.wstr_len = MMIAPICOM_Wstrlen(clock_text.wstr_ptr);
		text_style.font = SONG_FONT_30;
		txt_rect.top = 50;
		txt_rect.bottom = 80;
		GUISTR_DrawTextToLCDInRect(
			&lcd_dev_info,
			&txt_rect,
			&txt_rect,
			&clock_text,
			&text_style,
			text_state,
			GUISTR_TEXT_DIR_AUTO
			);

	}

	img_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP,win_id,img_rect);
	GUIRES_DisplayImg(NULL,&img_rect,NULL,win_id,IMAGE_ALARM_RING,&lcd_dev_info);

	bottom_img_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP,win_id,bottom_img_rect);
	GUIRES_DisplayImg(NULL,&bottom_img_rect,NULL,win_id,IMAGE_COMMON_ONEBTN_BG_IMAG,&lcd_dev_info);

	return MMI_RESULT_TRUE;
}

LOCAL MMI_RESULT_E HandleAlarmClockWinMsg_Close_Clock(MMI_WIN_ID_T win_id)
{
	SETTINGS_ALARM_ARRIVED_WIN* clock_event = {0};
	uint16 event_id = 0;
	BOOLEAN is_click_delay = FALSE;

	clock_event = (SETTINGS_ALARM_ARRIVED_WIN *)MMK_GetWinAddDataPtr( win_id );

	if (NULL == clock_event)
	{
		SCI_TRACE_LOW("HandleAlarmClockWinMsg_CLOSE_WINDOW NULL == clock_event");
		return MMI_RESULT_FALSE;
	}

	if (clock_event->event_id < EVENT_SCH_0)
	{
		if(clock_event->event_id <= EVENT_ALARM_MAX)
		{
			event_id = clock_event->event_id;
		}else if(clock_event->event_id >= EVENT_ALARM_DELAY_0 && clock_event->event_id <= EVENT_ALARM_DELAY_MAX)
		{
			event_id = clock_event->event_id - EVENT_ALARM_DELAY_0; 
		}
#ifdef CALENDAR_SUPPORT
		MMIAPISCH_DeleteUnreadEvent(event_id);
#endif            
		StopRingOrVib(TRUE);

		MMI_ReadNVItem(MMINV_ALARMCLOCK_IS_DELAY_NV,&is_click_delay);
		SCI_TRACE_LOW("HandleAlarmClockWinMsg_CLOSE_WINDOW is_click_delay = %d",is_click_delay);
	
		if (!is_click_delay)
		{
			CloseDelayAlarm(clock_event->event_id, clock_event->s_arrived_event.event_fast_info, FALSE);
		}
		is_click_delay = FALSE;
		MMI_WriteNVItem(MMINV_ALARMCLOCK_IS_DELAY_NV,&is_click_delay);

		if(((clock_event->s_arrived_event.event_fast_info.is_valid) 
			&& !MMIAPICC_IsInState(CC_IN_CALL_STATE) 
			&& !MMIAPIMMS_IsSendingOrRecving())
			&&(0 == clock_event->sch_list_total)
			)
		{
			CheckandStartExpiredEvent();
		}else
		{
			CheckandStartExpiredEvent();
			MMK_CloseWin(win_id);
		}

	}

}

PUBLIC MMI_RESULT_E HandleAlarmClockWinMsg(
	MMI_WIN_ID_T      win_id,
	MMI_MESSAGE_ID_E  msg_id, 
	DPARAM            param
	)
{
	MMI_RESULT_E    result = MMI_RESULT_TRUE;
	
	switch (msg_id)
	{
	case MSG_OPEN_WINDOW:
		{
			HandleAlarmClockWinMsg_OPEN_WINDOW(win_id);
		}
		break;
	case MSG_FULL_PAINT:
		{
			HandleAlarmClockWinMsg_FULL_PAINT(win_id);
		}
		break;
	case MSG_APP_WEB:
	case MSG_APP_OK:
	/*case MSG_APP_CANCEL:
		{
			SCI_TRACE_LOW("HandleAlarmClockWinMsg MSG_APP_CANCEL");
			MMIDEFAULT_AllowTurnOffBackLight(TRUE);
			HandleAlarmClockWinMsg_Close_Clock(win_id);
			MMK_CloseWin(win_id);
		}*/
		break;
	case MSG_CTL_CANCEL:
	case MSG_APP_CANCEL:
	    MMK_CloseWin(win_id);
	    break;
	case MSG_CLOSE_WINDOW:
		{
			SCI_TRACE_LOW("HandleAlarmClockWinMsg MSG_CLOSE_WINDOW");
			MMIDEFAULT_AllowTurnOffBackLight(TRUE);
			HandleAlarmClockWinMsg_Close_Clock(win_id);
			if(alarmclock_ring_timer_id){
				MMK_StopTimer(alarmclock_ring_timer_id);
				alarmclock_ring_timer_id = 0;
			}
		}
		break;
	case MSG_TP_PRESS_DOWN:
		{
			GUI_POINT_T   point = {0};
	    		point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			clock_alive_tp_x = point.x;
			clock_alive_tp_y = point.y;
		}
		break;
	case MSG_TP_PRESS_UP:
		{
			BOOLEAN from_list = TRUE;
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			if((point.x > 10 && point.x < 230) && (point.y > 210 &&  point.y < 260))
			{
				_callBack_ClickDelayAlarmClock();
				MMIDEFAULT_AllowTurnOffBackLight(TRUE);
			}else if((clock_alive_tp_x - point.x) > 60)
			{
				MMK_CloseWin(win_id);
			}
		}
		break;
	case MSG_KEYDOWN_RED:
		break;
	case MSG_KEYUP_RED:
		MMK_CloseWin(win_id);
		break; 
	default:
		{
			result = MMI_RESULT_FALSE;
		}
		break;
	}

	return result;
}

WINDOW_TABLE(MMI_ALARMCLOCK_ALIVE_WIN_TAB) = 
{
	WIN_ID(MMI_ALARMCLOCK_ALARM_CLOCK_WIN_ID),
	WIN_FUNC((uint32)HandleAlarmClockWinMsg),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMIALARMCLOCK_CreateAliveWin( SETTINGS_ALARM_ARRIVED_WIN arrived_event )
{
	SETTINGS_ALARM_ARRIVED_WIN* pWinData = (SETTINGS_ALARM_ARRIVED_WIN*)SCI_ALLOC_APPZ( sizeof(SETTINGS_ALARM_ARRIVED_WIN) );

	if( PNULL == pWinData )
	{
		SCI_TRACE_LOW("MMIALARMCLOCK_CreateAliveWin pWinData is PNULL!!");
		return;
	}

	if((MMIAPICC_IsInState(CC_MT_CONNECTING_STATE) && MMK_IsOpenWin(MMICC_ANIMATION_WIN_ID)) 
		|| MMIAPICC_IsInState(CC_MO_CONNECTING_STATE) || MMIAPICC_IsInState(CC_CALL_CONNECTED_STATE)
#ifdef  ZDT_VIDEOCHAT_SUPPORT
		|| MMIVIDEO_IsOpenVideoWin()
#endif		
		){
		SCI_TRACE_LOW("%s: not allow clock ring!!", __FUNCTION__);
		SCI_FREE(pWinData);
		pWinData = NULL;
		return ;
	}

	*pWinData = arrived_event;
	SCI_TRACE_LOW("MMIALARMCLOCK_CreateAliveWin pWinData->event_id = %d",pWinData->event_id);
	SCI_TRACE_LOW("MMIALARMCLOCK_CreateAliveWin event_clock_time = %ls",pWinData->event_clock_time.wstr_ptr);
	MMK_CreateWin( (uint32 *)MMI_ALARMCLOCK_ALIVE_WIN_TAB, (ADD_DATA)pWinData );

	//turn on backlight,use after create win
	#if 1	
	{
		MMI_WIN_ID_T win_id = MMI_ALARMCLOCK_ALARM_CLOCK_WIN_ID;
		MMI_HANDLE_T win_handle = MMK_ConvertIdToHandle(win_id);

		MMIPUB_HandleWaitWinMsg(win_id, MSG_FULL_PAINT, PNULL);
				
		if (STARTUP_ALARM != MMIAPIPHONE_GetStartUpCondition())
		{
			if( MMK_GetWinPriority(win_handle) != WIN_LOWEST_LEVEL)
			{
				MMIDEFAULT_TurnOnBackLight();
				MMIDEFAULT_AllowTurnOffBackLight(FALSE);
			}
		}else
		{
			MMIDEFAULT_TurnOnBackLight();
			MMIDEFAULT_AllowTurnOffBackLight(FALSE);
		}
	}
	#endif
}

PUBLIC BOOLEAN MMIALARM_IsOpenClockRingWin()
{
	return MMK_IsOpenWin(MMI_ALARMCLOCK_ALARM_CLOCK_WIN_ID);
}
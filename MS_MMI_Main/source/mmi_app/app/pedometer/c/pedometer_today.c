
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

#include "pedometer_export.h"
#include "pedometer_id.h"
#include "pedometer_image.h"
#include "pedometer_text.h"
#include "pedometer_nv.h"
#include "msensor_drv.h"
#include "mmi_common.h"
#include "zdt_gsensor.h"
#include "zdt_yx_db.h"

LOCAL uint8 timer_id = 0;

extern BOOLEAN MMIZDT_IsInChargingWin();

LOCAL void PedometerTodayWin_showStepBg(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info, uint32 target_step, uint32 step)
{
	GUI_POINT_T point = {120, 112};
	GUI_RECT_T clicp_rect = {0, 0, 239, 239};
	int16 angle = 0;
	float percent = 0.00;
	
	LCD_FillArc(&lcd_dev_info, &clicp_rect, point.x, point.y, 60, 0, 360, GUI_RGB2RGB565(0x24,0x24,0x24));
	//SCI_TRACE_LOW("step = %d, target_step = %d", step, target_step);
	if(step >= target_step){
		LCD_FillArc(&lcd_dev_info, &clicp_rect, point.x, point.y, 59, 0, 360, GUI_RGB2RGB565(0x0b,0x84,0xff));
	}else if(step * 4 <= target_step){
		percent = (float)step / ((float)target_step/4);
		SCI_TRACE_LOW("01percent = %f", percent);
		angle = 90 - 90 * percent;
		LCD_FillArc(&lcd_dev_info, &clicp_rect, point.x, point.y, 59, angle, 90, GUI_RGB2RGB565(0x0b,0x84,0xff));
	}else{
		percent = (float)step / (float)target_step;
		SCI_TRACE_LOW("02percent = %f", percent);
		angle = 360 - 270 * percent;
		LCD_FillArc(&lcd_dev_info, &clicp_rect, point.x, point.y, 59, angle, 90, GUI_RGB2RGB565(0x0b,0x84,0xff));
	}
	LCD_FillArc(&lcd_dev_info, &clicp_rect, point.x, point.y, 51, 0, 360, GUI_RGB2RGB565(0x24,0x24,0x24));
	LCD_FillArc(&lcd_dev_info, &clicp_rect, point.x, point.y, 50, 0, 360, MMI_BLACK_COLOR);

}

LOCAL void PedometerTodayWin_fullPaint(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
	wchar txt_str[30] = {0};
	char step_str[30] = {0};
	uint32 target_step = 3000;
	int new_step = 0;
	int percent = 0;
	float km = 0;
	float cal = 0;
	MMI_STRING_T text_str = {0};
	MMI_STRING_T text_string = {0};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
#ifdef ZTE_SUPPORT_240X284
	GUI_RECT_T title_rect = {12, 20, 100, 40};
	GUI_RECT_T count_bg_rect = {66,58,174,166};
	GUI_RECT_T count_img_rect = {66,58,174,166};
	GUI_RECT_T km_cal_rect = {50,179,190,199};
	GUI_RECT_T week_bg_rect = {28, 210, 220, 260};
	GUI_RECT_T bg_rect = {66, 58, 200, 200};
#elif ZTE_SUPPORT_240X240
	GUI_RECT_T title_rect = {12, 10, 100, 30};
	GUI_RECT_T count_bg_rect = {66,28,174,136};
	GUI_RECT_T count_img_rect = {66,28,174,136};
	GUI_RECT_T km_cal_rect = {50,149,190,169};
	GUI_RECT_T week_bg_rect = {28, 180, 220, 230};
	GUI_RECT_T bg_rect = {66, 28, 200, 170};
#endif

	MMI_IMAGE_ID_T step_bg[11] = {
		IMAGE_PEDOMETER_BG_0, IMAGE_PEDOMETER_BG_1, IMAGE_PEDOMETER_BG_2, IMAGE_PEDOMETER_BG_3,
		IMAGE_PEDOMETER_BG_4, IMAGE_PEDOMETER_BG_5, IMAGE_PEDOMETER_BG_6, IMAGE_PEDOMETER_BG_7,
		IMAGE_PEDOMETER_BG_8, IMAGE_PEDOMETER_BG_9, IMAGE_PEDOMETER_BG_10
	};

	text_style.align = ALIGN_LVMIDDLE;
	text_style.font = SONG_FONT_20;
	text_style.font_color = MMI_WHITE_COLOR;
	MMIRES_GetText(TXT_PEDOMETER_COUNT,win_id,&text_string);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&title_rect,
		&title_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

#ifdef WIN32
	new_step = 200;
#else
	new_step = ZDT_GSensor_GetStepOneDay();
#endif
    target_step = MMIZDT_Get_Target_Step();
    target_step = target_step>100?target_step:100;
	percent = (((float)new_step)/((float)target_step))*10;
	if(percent > 10){
		percent = 10;
	}
	//SCI_TRACE_LOW("%s: new_step = %d, target_step = %d, percent = %d",
	//	__FUNCTION__, new_step, target_step, percent);
	GUIRES_DisplayImg(PNULL, &bg_rect, PNULL, win_id, step_bg[percent], &lcd_dev_info);
	//PedometerTodayWin_showStepBg(win_id, lcd_dev_info, target_step, new_step);
	
	itoa(new_step,step_str,10);
	text_string.wstr_ptr = MMIAPICOM_StrToWstr((uint8 *)step_str,txt_str);
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	count_bg_rect.bottom -= 30;
	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_16;
	text_style.font_color = GUI_RGB2RGB565(0x0b, 0x84, 0xff);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&count_bg_rect,
		&count_bg_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	MMIAPICOM_CustomGenerateNumReplaceStrByTextId(TXT_PEDOMETER_TARGET, L"%N", target_step, 50, &text_string);
	count_bg_rect.top += 50;
	text_style.font_color = 0x7BEF;
	text_style.font = SONG_FONT_16;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&count_bg_rect,
		&count_bg_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	memset(&step_str, 0, 30);
	memset(&txt_str, 0, 30);
	km = (float)(new_step * 0.3 * yx_DB_Set_Rec.height) / (1000.00 * 120);
	sprintf(step_str, "%.2f公里", km);
	GUI_GBToWstr(txt_str, step_str, strlen(step_str));
	text_string.wstr_ptr = txt_str;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	text_style.align = ALIGN_LVMIDDLE;
	text_style.font = SONG_FONT_16;
	text_style.font_color = MMI_WHITE_COLOR;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&km_cal_rect,
		&km_cal_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	memset(&step_str, 0, 30);
	memset(&txt_str, 0, 30);
	cal	= (float)(km * 0.8214 * yx_DB_Set_Rec.weight);//距离 * weight * 0.8214
	sprintf(step_str, "%.2f卡路里", cal);
	GUI_GBToWstr(txt_str, step_str, strlen(step_str));
	text_string.wstr_ptr = txt_str;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	text_style.align = ALIGN_RVMIDDLE;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&km_cal_rect,
		&km_cal_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	GUIRES_DisplayImg(PNULL, &week_bg_rect, PNULL, win_id, IMAGE_PEDOMETER_WEEK_BG, &lcd_dev_info);
	week_bg_rect.left = 55;
#ifdef ZTE_SUPPORT_240X284
	week_bg_rect.top = 212;
#elif ZTE_SUPPORT_240X240
	week_bg_rect.top = 182;
#endif
	GUIRES_DisplayImg(PNULL, &week_bg_rect, PNULL, win_id, IMAGE_PEDOMETER_TOTAL, &lcd_dev_info);

	text_style.align = ALIGN_LEFT;
	text_style.font = SONG_FONT_24;
	week_bg_rect.left = 100;
#ifdef ZTE_SUPPORT_240X284
	week_bg_rect.top = 224;
#elif ZTE_SUPPORT_240X240
	week_bg_rect.top = 194;
#endif
	MMIRES_GetText(TXT_PEDOMETER_WEEK_STEP,win_id,&text_string);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&week_bg_rect,
		&week_bg_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);
}

LOCAL void PedometerTodayRefresh(uint8 time_id, void * param)
{
	MMK_SendMsg(MMI_PEDOMETER_TODAY_EXERCISE_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL MMI_RESULT_E HandlePedometerTodayWin( 
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CheckAllocatedMemInfo();

	switch(msg_id)
	{
	case MSG_OPEN_WINDOW:
		{
			timer_id = MMK_CreateWinTimer(win_id, 1500,TRUE);
		}
		break;
	case MSG_GET_FOCUS:
		{
			if(timer_id != 0){
				MMK_StopTimer(timer_id);
				timer_id = 0;
			}
			if(!MMIZDT_IsInChargingWin())
            {
                timer_id = MMK_CreateWinTimer(win_id, 1500,TRUE);
			}
		}
		break;
	case MSG_FULL_PAINT:
		{
			GUI_RECT_T win_rect = MMITHEME_GetFullScreenRect();
			GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);
			PedometerTodayWin_fullPaint(win_id, lcd_dev_info);
		}
		break;
    case MSG_LOSE_FOCUS:
        if(timer_id != 0)
        {
			MMK_StopTimer(timer_id);
			timer_id = 0;
		}
        break;
	case MSG_CLOSE_WINDOW:
		{
			if(timer_id != 0){
				MMK_StopTimer(timer_id);
				timer_id = 0;
			}
		}
		break;
	case MSG_APP_CANCEL:
	case MSG_CTL_CANCEL:
		{
			MMK_CloseWin(win_id);
			if(timer_id != 0){
				MMK_StopTimer(timer_id);
				timer_id = 0;
			}
		}
		break;
	case MSG_TP_PRESS_DOWN:
    case MSG_TP_PRESS_MOVE:
		break;
	case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		#ifdef ZTE_SUPPORT_240X284
			if((point.x > 20 && point.x < 230) && (point.y > 210 && point.y < 260))
		#elif ZTE_SUPPORT_240X240	
			if((point.x > 20 && point.x < 230) && (point.y > 180 && point.y < 230))
		#endif
			{
				MMIPEDOMETER_CreateStepWeekWin();
			}
		}
		break;
	case MSG_KEYDOWN_RED:
		break;
	case MSG_KEYUP_RED:
		MMK_CloseWin(win_id);
		break; 
    case MSG_TIMER:
        {
            if (*(uint8*)param == timer_id && MMK_GetFocusWinId() == win_id)
            {
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
        }
        break;
	default:
        recode = MMI_RESULT_FALSE;
		break;
	}

	return recode;

}

WINDOW_TABLE(MMI_PEDOMETER_TODAY_WIN_TAB) = 
{
	WIN_ID(MMI_PEDOMETER_TODAY_EXERCISE_WIN_ID),
	WIN_FUNC((uint32)HandlePedometerTodayWin),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMIPEDOMETER_CreateTodayWin()
{
	MMI_WIN_ID_T win_id = MMI_PEDOMETER_TODAY_EXERCISE_WIN_ID;
	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}
    MMK_CreateWin((uint32*)MMI_PEDOMETER_TODAY_WIN_TAB, NULL);
}


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
#include "guilistbox.h"
#include "guibutton.h"
#include "guicommon.h"
#include "mmk_timer.h"
#include "mmi_common.h"
#include "mmiacc_event.h"
#include "zteweather_main.h"
#include "zteweather_http.h"
#include "zteweather_id.h"
#include "zteweather_text.h"
#include "zteweather_image.h"
#include "zdt_app.h"
#include "mmicc_export.h"
#include "zdt_yx_net.h"

static int16 s_zteweather_tp_x;
static int16 s_zteweather_tp_y;

const ZTEWEATHER_FORECAST_T forecast[] = {
	{zteweather_tomorrow_bg, TXT_ZTEWEATHER_TOMORROW},
	{zteweather_after_tomorrow, TXT_ZTEWEATHER_AFTER_TOMORROW},
};
// 1-晴  2—多云  3-阴  4—雨  5—雷雨 6-雪 
const MMI_IMAGE_ID_T status_img[] = {
		zteweather_sun, zteweather_more_cloud, zteweather_cloud_sun,
		zteweather_sun_rain,zteweather_more_rain, zteweather_snow
	};

uint8 zteWeather_timer_id = 0;

ZTEWEATHER_NORMAL_INFO_T zte_normal_info = {0};

extern YX_WEATHER_INFO_T zte_wt_info[4];
LOCAL SCI_TIME_T last_wt_time = {0};
#define WEATHER_EXPIRE_MINUTES 60

#define LOC_IMG_LEFT  74
#define LOC_IMG_RIGHT  110

#define LOC_TXT_LEFT  110
#define LOC_TXT_RIGHT  180

#define WEATHER_BG_LEFT  38
#define WEATHER_BG_RIGHT  202

#define WEATHER_STATUS_LEFT  85
#define WEATHER_STATUS_RIGHT  155

#define WEATHER_TEMPERATURE_LEFT  76
#define WEATHER_TEMPERATURE_RIGHT  165

#define WEATHER_TEMPERATURE_RANGE_LEFT  52
#define WEATHER_TEMPERATURE_RANGE_RIGHT  188

#ifdef ZTE_SUPPORT_240X284
#define LOC_IMG_TOP  23
#define LOC_IMG_BOTTOM  60

#define LOC_TXT_TOP  30
#define LOC_TXT_BOTTOM  50

#define WEATHER_BG_TOP  60
#define WEATHER_BG_BOTTOM  224

#define WEATHER_STATUS_TOP  80
#define WEATHER_STATUS_BOTTOM  150

#define WEATHER_TEMPERATURE_TOP  160
#define WEATHER_TEMPERATURE_BOTTOM  202

#define WEATHER_TEMPERATURE_RANGE_TOP  245
#define WEATHER_TEMPERATURE_RANGE_BOTTOM  265
#else
#define LOC_IMG_TOP  8
#define LOC_IMG_BOTTOM  45

#define LOC_TXT_TOP  15//20
#define LOC_TXT_BOTTOM  35//40

#define WEATHER_BG_TOP  46
#define WEATHER_BG_BOTTOM  210

#define WEATHER_STATUS_TOP  66
#define WEATHER_STATUS_BOTTOM  187

#define WEATHER_TEMPERATURE_TOP  146
#define WEATHER_TEMPERATURE_BOTTOM  188

#define WEATHER_TEMPERATURE_RANGE_TOP  215
#define WEATHER_TEMPERATURE_RANGE_BOTTOM  235
  
#endif

#define LOC_IMG_RECT    DP2PX_RECT(LOC_IMG_LEFT,LOC_IMG_TOP, LOC_IMG_RIGHT, LOC_IMG_BOTTOM)
#define LOC_TXT_RECT    DP2PX_RECT(LOC_TXT_LEFT,LOC_TXT_TOP, LOC_TXT_RIGHT, LOC_TXT_BOTTOM)
#define WEATHER_BG_RECT     DP2PX_RECT(WEATHER_BG_LEFT,WEATHER_BG_TOP,WEATHER_BG_RIGHT,WEATHER_BG_BOTTOM)
#define WEATHER_STATUS_RECT     DP2PX_RECT(WEATHER_STATUS_LEFT,WEATHER_STATUS_TOP,WEATHER_STATUS_RIGHT,WEATHER_STATUS_BOTTOM)
#define WEATHER_TEMPERATURE_RECT     DP2PX_RECT(WEATHER_TEMPERATURE_LEFT,WEATHER_TEMPERATURE_TOP,WEATHER_TEMPERATURE_RIGHT,WEATHER_TEMPERATURE_BOTTOM)
#define WEATHER_TEMPERATURE_RANGE_RECT     DP2PX_RECT(WEATHER_TEMPERATURE_RANGE_LEFT,WEATHER_TEMPERATURE_RANGE_TOP,WEATHER_TEMPERATURE_RANGE_RIGHT,WEATHER_TEMPERATURE_RANGE_BOTTOM)


LOCAL BOOLEAN Is_WeatherExpired()
{
    BOOLEAN ret = TRUE;    
    uint8 year_str[5] = {0};
    uint8 mon_str[3] = {0};
    uint8 day_str[3] = {0};
    
    SCI_TIME_T time = {0};
    SCI_DATE_T date = {0};
    TM_GetSysTime(&time);
    TM_GetSysDate(&date);
    if(time.hour < last_wt_time.hour //next day
        || (time.hour*60 + time.min) - (last_wt_time.hour*60 + last_wt_time.min) >= WEATHER_EXPIRE_MINUTES

    )
    {
        ret = TRUE;
        last_wt_time = time;

    }
    else
    {
        ret = FALSE;

    }
    return ret;
}

BOOLEAN Is_ZTE_WeatherGetFromServer()
{
    BOOLEAN ret = TRUE;
    if((zte_wt_info[0].min_degree == 0 &&  zte_wt_info[0].max_degree == 0) || Is_WeatherExpired())
    {
        ret = FALSE;
    }
    return ret;
}

LOCAL void ZteWeather_RequsetTimerCallback(uint8 timer_id, uint32 param)
{
    if(zteWeather_timer_id != 0)
	{
		MMK_StopTimer(zteWeather_timer_id);
        zteWeather_timer_id = 0;
	}
    //请求超时，上一次请求成功，且不超过6个小时
    if(zte_normal_info.status == 1 && (TM_GetTotalSeconds() - zte_normal_info.last_time) < 6*60*60)
    {

    }
    else
    {
		zte_normal_info.status = param;
    }
	SCI_TRACE_LOW("%s: zte_normal_info.status = %d", __FUNCTION__, zte_normal_info.status);
	if(MMK_IsOpenWin(ZTEWEATHER_MAIN_WIN_ID))
	{
		MMK_SendMsg(ZTEWEATHER_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
	}
}

LOCAL void ZteWeather_RequsetInfoTimer()
{
	if(zteWeather_timer_id != 0){
		MMK_StopTimer(zteWeather_timer_id);
	}
	zteWeather_timer_id = MMK_CreateTimerCallback(35*1000, ZteWeather_RequsetTimerCallback, 2, FALSE);
}

LOCAL void ZteWeather_GetNewWeatherInfo()
{
	MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
	uint32 cur_times = TM_GetTotalSeconds();
	if(!MMIAPIPHONE_GetSimExistedStatus(dual_sys))
	{
		zte_normal_info.status = 0;
		return;
	}
	SCI_TRACE_LOW("%s: cur_times = %d, last_time = %d", __FUNCTION__, cur_times, zte_normal_info.last_time);
	SCI_TRACE_LOW("%s: zte_normal_info.status = %d", __FUNCTION__, zte_normal_info.status);
	if(cur_times - zte_normal_info.last_time > 60*60 || zte_normal_info.status != 1)
	{
	#ifdef WIN32
		ZteWeather_InitTestWeatherInfo();
	#else
		YX_API_WT_Send();
	#endif
        if(zte_normal_info.status == 2) //上一次是超时状态
        {
            zte_normal_info.status = -1;//界面显示正在请求
        }
		ZteWeather_RequsetInfoTimer();
	}
}

LOCAL void ZteWeatherForecast_FullPaint(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
	GUI_RECT_T win_rect = MMITHEME_GetFullScreenRect();
	GUI_RECT_T forecast_bg_rect = {12,26,228,120};
	GUI_RECT_T day_rect = {24,48,216,68};
	GUI_RECT_T temp_rect = {146,38,216,108};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_string = {0};
	char txt_char[20] = {0};
	wchar txt_wchar[20] = {0};
	int i = 0;
    uint8 img_size = sizeof(status_img)/sizeof(MMI_IMAGE_ID_T);
    uint8 img_index = 0;
    

	GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);

	text_style.align = ALIGN_LEFT;
	text_style.font = SONG_FONT_24;
	text_style.font_color = MMI_WHITE_COLOR;

	//tomorrow
	GUIRES_DisplayImg(PNULL, &forecast_bg_rect, PNULL, win_id, forecast[0].bg, &lcd_dev_info);
		
	MMIRES_GetText(forecast[0].txt,win_id,&text_string);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&day_rect,
		&day_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	sprintf(txt_char, "%d℃~%d℃", zte_wt_info[1].min_degree, zte_wt_info[1].max_degree);
	GUI_GBToWstr(txt_wchar, txt_char, strlen(txt_char));
	text_string.wstr_ptr = txt_wchar;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	day_rect.top += 34;
	day_rect.bottom += 34;
	text_style.font = SONG_FONT_22;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&day_rect,
		&day_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

    img_index = zte_wt_info[1].num>img_size? 0:zte_wt_info[1].num-1; //天气编号从1开始
	GUIRES_DisplayImg(PNULL, &temp_rect, PNULL, win_id, status_img[img_index], &lcd_dev_info);

	//after tomorrow
	forecast_bg_rect.top = 128;
	forecast_bg_rect.bottom = 222;
	GUIRES_DisplayImg(PNULL, &forecast_bg_rect, PNULL, win_id, forecast[1].bg, &lcd_dev_info);

	day_rect.top = 150;
	day_rect.bottom = 170;
	text_style.font = SONG_FONT_24;
	MMIRES_GetText(forecast[1].txt,win_id,&text_string);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&day_rect,
		&day_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	sprintf(txt_char, "%d℃~%d℃", zte_wt_info[2].min_degree, zte_wt_info[2].max_degree);
	GUI_GBToWstr(txt_wchar, txt_char, strlen(txt_char));
	text_string.wstr_ptr = txt_wchar;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	day_rect.top += 34;
	day_rect.bottom += 34;
	text_style.font = SONG_FONT_22;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&day_rect,
		&day_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	temp_rect.top = 140;
	temp_rect.bottom = 210;
    img_index = zte_wt_info[1].num>img_size? 0:zte_wt_info[2].num-1;
	GUIRES_DisplayImg(PNULL, &temp_rect, PNULL, win_id, status_img[img_index], &lcd_dev_info);
}

LOCAL MMI_RESULT_E HandleZteWeatherForecastWin(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
	case MSG_OPEN_WINDOW:
		{
			
		}
		break;
	case MSG_FULL_PAINT:
		{
			ZteWeatherForecast_FullPaint(win_id, lcd_dev_info);
		}
		break;
	
	case MSG_TP_PRESS_DOWN:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			s_zteweather_tp_x = point.x;
			s_zteweather_tp_y = point.y;
		}
		break;
	case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T   point = {0};
			uint16 tp_offset_x = 0;
			uint16 tp_offset_y = 0;
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			tp_offset_x = point.x - s_zteweather_tp_x;
			tp_offset_y = point.y - s_zteweather_tp_y;
			if(ABS(tp_offset_x) > ABS(tp_offset_y)){
				if((point.x - s_zteweather_tp_x) > 60  && tp_offset_x > 0)
				{
					MMK_CloseWin(win_id);
					break;
				}
			}
		}
		break;
	case MSG_APP_CANCEL:
	case MSG_CTL_CANCEL:
		{
			MMK_CloseWin(win_id);
		}
		break;
	case MSG_CLOSE_WINDOW:
		{
			
		}
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

WINDOW_TABLE(MMI_ZTEWEATHER_FORECAST_WIN_TAB) = 
{
	WIN_ID(ZTEWEATHER_FORECAST_WIN_ID),
	WIN_FUNC((uint32)HandleZteWeatherForecastWin),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void ZTEWEATHER_CreateForecastwin()
{
	MMI_WIN_ID_T win_id = ZTEWEATHER_FORECAST_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_ZTEWEATHER_FORECAST_WIN_TAB, NULL);
	MMK_SetWinRect(win_handle, &rect);

}

LOCAL void ZteWeatherMain_FullPaint(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_RECT_T win_rect = MMITHEME_GetFullScreenRect();
    GUI_RECT_T loc_img_rect = LOC_IMG_RECT;//{74,23,110,60};
    GUI_RECT_T loc_txt_rect = LOC_TXT_RECT;//{110,30,180,50};
    GUI_RECT_T bg_rect = WEATHER_BG_RECT;//{38,60,202,224};
    GUI_RECT_T status_rect = WEATHER_STATUS_RECT;//{85,80,155,150};
    GUI_RECT_T temperature_rect = WEATHER_TEMPERATURE_RECT;
    GUI_RECT_T temperature_range_rect = WEATHER_TEMPERATURE_RANGE_RECT;
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    char txt_char[20] = {0};
    wchar txt_wchar[20] = {0};
    uint8 img_size = sizeof(status_img)/sizeof(MMI_IMAGE_ID_T);

	GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);

	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_20;
	text_style.font_color = MMI_WHITE_COLOR;
    SCI_TRACE_LOW("%s: status:%d", __FUNCTION__,zte_normal_info.status);
	if(zte_normal_info.status == -1){
		MMIRES_GetText(TXT_ZTEWEATHER_LOADING,win_id,&text_string);
		GUISTR_DrawTextToLCDInRect(
			(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
			&win_rect,
			&win_rect,
			&text_string,
			&text_style,
			text_state,
			GUISTR_TEXT_DIR_AUTO
			);
		return ;
	}else if(zte_normal_info.status == 0)
	{
		MMIRES_GetText(TXT_ZTEWEATHER_ERROR,win_id,&text_string);
		GUISTR_DrawTextToLCDInRect(
			(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
			&win_rect,
			&win_rect,
			&text_string,
			&text_style,
			text_state,
			GUISTR_TEXT_DIR_AUTO
			);
		return ;
	}else if(zte_normal_info.status == 2)
	{
		MMIRES_GetText(TXT_ZTEWEATHER_TIMEOUT,win_id,&text_string);
		GUISTR_DrawTextToLCDInRect(
			(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
			&win_rect,
			&win_rect,
			&text_string,
			&text_style,
			text_state,
			GUISTR_TEXT_DIR_AUTO
			);
		return ;
	}

	GUIRES_DisplayImg(PNULL, &loc_img_rect, PNULL, win_id, zteweather_location, &lcd_dev_info);

	text_string.wstr_ptr = zte_wt_info[0].city_name;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	text_style.align = ALIGN_LVMIDDLE;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&loc_txt_rect,
		&loc_txt_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	GUIRES_DisplayImg(PNULL, &bg_rect, PNULL, win_id, zteweather_today_bg, &lcd_dev_info);
    memset(&txt_char, 0, 20);
	memset(&txt_wchar, 0, 20);
    if(zte_wt_info[0].num > img_size || zte_wt_info[0].num < 1)//防止非法数据造成死机
    {
        sprintf(txt_char, "ERROR", zte_wt_info[0].cur_degree);
    }
    else
    {
        GUIRES_DisplayImg(PNULL, &status_rect, PNULL, win_id, status_img[zte_wt_info[0].num-1], &lcd_dev_info);
        sprintf(txt_char, "%d℃", zte_wt_info[0].cur_degree);
    }

	GUI_GBToWstr(txt_wchar, txt_char, strlen(txt_char));
	text_string.wstr_ptr = txt_wchar;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	text_style.align = ALIGN_HMIDDLE;
	text_style.font = SONG_FONT_46;
	text_style.font_color = MMI_WHITE_COLOR;
	//bg_rect.top = 160;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&temperature_rect,
		&temperature_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	memset(&txt_char, 0, 20);
	memset(&txt_wchar, 0, 20);
	sprintf(txt_char, "%d℃~%d℃", zte_wt_info[0].min_degree, zte_wt_info[0].max_degree);
	GUI_GBToWstr(txt_wchar, txt_char, strlen(txt_char));
	text_string.wstr_ptr = txt_wchar;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_22;
	text_style.font_color = GUI_RGB2RGB565(0x0b, 0x84, 0xff);
	//bg_rect.top = 245;
	//bg_rect.bottom = 265;
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&temperature_range_rect,
		&temperature_range_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);
}

LOCAL MMI_RESULT_E HandleZteWeatherMainWin(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
	case MSG_OPEN_WINDOW:
		{
			ZteWeather_GetNewWeatherInfo();
		}
		break;
	case MSG_FULL_PAINT:
		{
            if(MMK_GetFocusWinId() == win_id)
            {
			    ZteWeatherMain_FullPaint(win_id, lcd_dev_info);
            }
		}
		break;
	
	case MSG_TP_PRESS_DOWN:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			s_zteweather_tp_x = point.x;
			s_zteweather_tp_y = point.y;
		}
		break;
	case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T   point = {0};
			uint16 tp_offset_x = 0;
			uint16 tp_offset_y = 0;
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			tp_offset_x = point.x - s_zteweather_tp_x;
			tp_offset_y = point.y - s_zteweather_tp_y;
			if(ABS(tp_offset_x) > ABS(tp_offset_y)){
				if((point.x - s_zteweather_tp_x) > 60  && tp_offset_x > 0)
				{
					MMK_CloseWin(win_id);
					break;
				}
			}
			if((s_zteweather_tp_x - point.x) > 60)
			{
				if(zte_normal_info.status == 1 || zte_normal_info.status == 3)
				{
					ZTEWEATHER_CreateForecastwin();
				}
			}
		}
		break;
	case MSG_APP_CANCEL:
	case MSG_CTL_CANCEL:
		{
			MMK_CloseWin(win_id);
		}
		break;
	case MSG_CLOSE_WINDOW:
		{
			if(zteWeather_timer_id != 0){
				MMK_StopTimer(zteWeather_timer_id);
				zteWeather_timer_id = 0;
			}
		}
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

WINDOW_TABLE(MMI_ZTEWEATHER_MAIN_WIN_TAB) = 
{
	WIN_ID(ZTEWEATHER_MAIN_WIN_ID),
	WIN_FUNC((uint32)HandleZteWeatherMainWin),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void ZTEWEATHER_CreateMainWin()
{
	MMI_WIN_ID_T win_id = ZTEWEATHER_MAIN_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_ZTEWEATHER_MAIN_WIN_TAB, NULL);
	MMK_SetWinRect(win_handle, &rect);
}

PUBLIC void ZTEWEATHER_UpdateWin()
{
	if(zteWeather_timer_id != 0){
		MMK_StopTimer(zteWeather_timer_id);
		zteWeather_timer_id = 0;
	}
    zte_normal_info.last_time = TM_GetTotalSeconds();
	zte_normal_info.status = 1;
	SCI_TRACE_LOW("%s: ", __FUNCTION__);
	if(MMK_IsOpenWin(ZTEWEATHER_MAIN_WIN_ID))
	{
		MMK_SendMsg(ZTEWEATHER_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL);
	}
}


#include "std_header.h"
#include <stdlib.h>
#include "window_parse.h"
#include "sci_types.h"
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
#include "pedometer_export.h"
#include "pedometer_id.h"
#include "pedometer_image.h"
#include "pedometer_text.h"
#include "dal_time.h"

PUBLIC void PedometerCloseWin()
{
	if (MMK_IsOpenWin(MMI_PEDOMETER_TODAY_EXERCISE_WIN_ID))
	{
		MMK_CloseWin(MMI_PEDOMETER_TODAY_EXERCISE_WIN_ID);
	}
	if (MMK_IsOpenWin(MMI_PEDOMETER_STEP_LIST_WIN_ID))
	{
		MMK_CloseWin(MMI_PEDOMETER_STEP_LIST_WIN_ID);
	}
	if (MMK_IsOpenWin(MMI_PEDOMETER_STEP_WEEK_WIN_ID))
	{
		MMK_CloseWin(MMI_PEDOMETER_STEP_WEEK_WIN_ID);
	}
	if (MMK_IsOpenWin(MMI_PEDOMETER_LOADING_WIN_ID))
	{
		MMK_CloseWin(MMI_PEDOMETER_LOADING_WIN_ID);
	}
}

LOCAL int date_map(int month, int sum_t)
{
	switch(month)
	{
	case 1: sum_t = 0; break;
	case 2: sum_t = 31; break;
	case 3: sum_t = 59; break;
	case 4: sum_t = 90; break;
	case 5: sum_t = 120; break;
	case 6: sum_t = 151; break;
	case 7: sum_t = 181; break;
	case 8: sum_t = 212; break;
	case 9: sum_t = 243; break;
	case 10: sum_t = 273; break;
	case 11: sum_t = 304; break;
	case 12: sum_t = 334; break;
	default: break;
	}

	if (month != -1) return sum_t;

	if (month == -1)
	{   
		do{
			switch(sum_t)
			{
			case 0: month = 1; break;
			case 31: month = 2; break;
			case 59: month = 3; break;
			case 90: month = 4; break;
			case 120: month = 5; break;
			case 151: month = 6; break;
			case 181: month = 7; break;
			case 212: month = 8; break;
			case 243: month = 9; break;
			case 273: month = 10; break;
			case 304: month = 11; break;
			case 334: month = 12; break;
			default: break;
			}

			if (month != -1) break;

		}while(sum_t--);
	}
	if (sum_t != -1) return month;
}


PUBLIC void getTextDay(wchar *day_wchar,int beforeDay,BOOLEAN getDayOrMon)
{

	SCI_DATE_T  sys_date = {0};
	char text_day[10] = {0};
	int i = 0;
	int year = 0;
	int	month = 0;
	int	day = 0;
	int days = 0;
	int sum = 0;

	TM_GetSysDate(&sys_date);

	year = sys_date.year;
	month = sys_date.mon;
	day = sys_date.mday;
	days = beforeDay;
	//Trace_Readboy_Log("getTextDay year = %d,month = %d,day = %d,days = %d",year,month,day,days);

	// ������꿪ʼ�����ڵ�����
	sum = date_map(month, -1) + day;

	if (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0))
	{
		if(month > 2) sum++;
	}

	if (days < sum)// ����(days)С�ڽ��꿪ʼ�����ڵ�����(sum)
	{
		if (days < day)// ����(days)С�ڵ�ǰ�·ݵ�����(day)
		{
			//Trace_Readboy_Log("getTextDay days < day, year = %d,month = %d,day = %d",year,month,(day-days));
			day = day-days;
		} 
		else
		{// �������ڵ�ǰ�·ݵ�����(day)С�ڽ��꿪ʼ�����ڵ�����(sum)
			i = sum - days;
			month = date_map(-1, i);
			day = (i - date_map(month, -1));
			//Trace_Readboy_Log("getTextDay days > day,year = %d,month = %d,day = %d",year,month,(i - date_map(month, -1)));
		}
	} 
	else 
	{// ����(days)���ڽ��꿪ʼ�����ڵ�����(sum)	
		i = 365 - days + sum;
		month = date_map(-1, i);
		day = (i - date_map(month, -1));
		//Trace_Readboy_Log("getTextDay days > sum, year = %d,month = %d,day = %d",(year - 1),month,(i - date_map(month, -1)));
	}
	if (getDayOrMon)
	{
		if (day == 1)
		{
			sprintf(text_day,"%d��",month);
		}else if (day == 0)
		{
			sprintf(text_day,"%d",31);
		}else
		{
			sprintf(text_day,"%d",day);
		}
		GUI_GBToWstr(day_wchar, (uint8 *)text_day, strlen(text_day));
		//Trace_Readboy_Log("getTextDay getDay day_wchar = %ls",day_wchar);
	}else
	{
		if (day == 0)
		{
			day	= 31;
			month = month - 1;
		}
		sprintf(text_day,"%d��%d��",month,day);
		GUI_GBToWstr(day_wchar, (uint8 *)text_day, strlen(text_day));
		//Trace_Readboy_Log("getTextDay getMonAndDay day_wchar = %ls",day_wchar);
	}

}

PUBLIC int get_the_month_of_days(int cur_year, int cur_month)
{
	int days = 0;
	switch(cur_month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 0:
			days = 31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			days = 30;
			break;
		case 2:
			if (cur_year % 400 == 0 || (cur_year % 4 == 0 && cur_year % 100 != 0))
			{
				days = 29;
			}else{
				days = 28;
			}
		default:
			break;
	}
	return days;
}

PUBLIC int get_number_of_days_between(int cur_year, int cur_month, int cur_day, int e_day)
{
	int bet_day = 0;
	int days = 0;

	if (cur_day <= e_day)
	{
		days = get_the_month_of_days(cur_year, cur_month - 1);
		cur_day += days;
	}
	bet_day = cur_day - e_day;

	SCI_TRACE_LOW("%s: bet_day = %d", __FUNCTION__, bet_day);

	return bet_day;
}


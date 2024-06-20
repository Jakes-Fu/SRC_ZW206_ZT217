
#include "std_header.h"


#include "sci_types.h"
#include "mmi_modu_main.h"
#include "pedometer_nv.h"
#include "dal_time.h"
#include "pedometer_export.h"

#ifdef WIN32
void ZDT_GSensor_Reset(){};
#endif

const uint16 pedometer_nv_len[] =
{
	sizeof(BOOLEAN),

	sizeof(uint8),//date

	sizeof(int),//praise

	sizeof(int),//day1 step 
	sizeof(int),//day2 step 
	sizeof(int),//day3 step 
	sizeof(int),//day4 step 
	sizeof(int),//day5 step 
	sizeof(int),//day6 step 
	sizeof(int),//day7 step 
	
};


PUBLIC void MMI_RegPedometerNv(void)
{
	MMI_RegModuleNv(MMI_MODULE_PEDOMETER, pedometer_nv_len, ARR_SIZE(pedometer_nv_len));
}

PUBLIC void MMI_Pedometer_Factory(void)
{
    BOOLEAN is_first_open = FALSE;
    uint8 date = 0;
    int step = 0;
	MMI_WriteNVItem(MMINV_PEDOMETER_FLAG,&is_first_open);
	MMI_WriteNVItem(MMINV_PEDOMETER_DATE_NV,&date);
	MMI_WriteNVItem(MMINV_PEDOMETER_DAY1_STEP_NV,&step);
	MMI_WriteNVItem(MMINV_PEDOMETER_DAY2_STEP_NV,&step);
	MMI_WriteNVItem(MMINV_PEDOMETER_DAY3_STEP_NV,&step);
	MMI_WriteNVItem(MMINV_PEDOMETER_DAY4_STEP_NV,&step);
	MMI_WriteNVItem(MMINV_PEDOMETER_DAY5_STEP_NV,&step);
	MMI_WriteNVItem(MMINV_PEDOMETER_DAY6_STEP_NV,&step);
	MMI_WriteNVItem(MMINV_PEDOMETER_DAY7_STEP_NV,&step);
}

LOCAL uint32 last_step = 0;
PUBLIC uint32 MMI_Pedometer_WriteInfoNvm(void)
{
	SCI_DATE_T dateValue = { 0 };
	uint32 day_step[7] = {0};
	uint8 day_num = 0;
	uint32 step = 0;
	uint8 i = 0;
    uint8 week_index;

	//先把截至今天存储的7条数据读出
	MMI_ReadNVItem(MMINV_PEDOMETER_DATE_NV,&day_num);

	TM_GetSysDate(&dateValue);
    week_index = dateValue.wday;
    if(week_index == 0) //星期天
    {
        week_index = 7;
    }

#ifdef WIN32
	step = 200;
#else
	step = ZDT_GSensor_GetStepOneDay();
#endif
	
	SCI_TRACE_LOW("%s: step = %d", __FUNCTION__, step);
	SCI_TRACE_LOW("%s: day1_step = %d", __FUNCTION__, day_step[0]);
	SCI_TRACE_LOW("%s: dateValue.mday = %d", __FUNCTION__, dateValue.mday);
	SCI_TRACE_LOW("%s: day_num = %d", __FUNCTION__, day_num);

    if(day_num != dateValue.mday && week_index == 1)//新的一周开始
    {
        int init_step = 0;
        PEDOMETER_NV_ITEM_E nv_item;
		for (i = 0;i < 7;i++)
		{
            nv_item = MMINV_PEDOMETER_DAY1_STEP_NV + i;
            MMI_WriteNVItem(nv_item,&init_step);
		}
    }

	if (day_num != dateValue.mday || step != last_step)
	{
        PEDOMETER_NV_ITEM_E nv_item;
		if (day_num == dateValue.mday && step != last_step)
		{
            nv_item = MMINV_PEDOMETER_DAY1_STEP_NV + week_index - 1;
            MMI_WriteNVItem(nv_item,&step);
		}
        else
		{
			int bet_day = 0;
			bet_day = get_number_of_days_between(dateValue.year, dateValue.mon, dateValue.mday, day_num);
			SCI_TRACE_LOW("%s: bet_day = %d", __FUNCTION__, bet_day);
            MMI_WriteNVItem(MMINV_PEDOMETER_DATE_NV,&dateValue.mday);
			if (bet_day < 7)
			{
                nv_item = MMINV_PEDOMETER_DAY1_STEP_NV + week_index - 1;                  
                MMI_WriteNVItem(nv_item,&step); //当天的记录
                if(bet_day > 1) //上一次记录到现在间隔大于2天
                {
                    int init_step = 0;
                    for(i=0; i<bet_day-1; i++) //清空当天之前间隔的记录
                    {
                        nv_item = MMINV_PEDOMETER_DAY1_STEP_NV + week_index - 2 - i;
                        if(nv_item < MMINV_PEDOMETER_DAY1_STEP_NV || nv_item > MMINV_PEDOMETER_DAY7_STEP_NV)
                        {
                            continue;
                        }
                        MMI_WriteNVItem(nv_item,&init_step);
                    }
                    for(i=week_index; i<7; i++) //清空当天之后的记录
                    {
                        nv_item = MMINV_PEDOMETER_DAY1_STEP_NV + i;
                        if(nv_item < MMINV_PEDOMETER_DAY1_STEP_NV || nv_item > MMINV_PEDOMETER_DAY7_STEP_NV)
                        {
                            continue;
                        }
                        MMI_WriteNVItem(nv_item,&init_step);
                    }
                }
			}
            else //上一次记录到现在间隔大于7天
			{
                int init_step = 0;
                MMI_WriteNVItem(MMINV_PEDOMETER_DATE_NV,&dateValue.mday);
				for (i = 0;i < 7;i++)
				{
                    if((week_index-1) == i)
                    {
                        init_step = step;
                    }
                    else
                    {
                        init_step = 0;
                    }
                    nv_item = MMINV_PEDOMETER_DAY1_STEP_NV + i;
                    MMI_WriteNVItem(nv_item,&init_step);
				}
			}
		}
	}
    last_step = step;
	return step;
}
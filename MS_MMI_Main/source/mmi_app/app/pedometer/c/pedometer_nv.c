
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

	//先把截至今天存储的7条数据读出
	MMI_ReadNVItem(MMINV_PEDOMETER_DATE_NV,&day_num);
	MMI_ReadNVItem(MMINV_PEDOMETER_DAY1_STEP_NV,&day_step[0]);
	MMI_ReadNVItem(MMINV_PEDOMETER_DAY2_STEP_NV,&day_step[1]);
	MMI_ReadNVItem(MMINV_PEDOMETER_DAY3_STEP_NV,&day_step[2]);
	MMI_ReadNVItem(MMINV_PEDOMETER_DAY4_STEP_NV,&day_step[3]);
	MMI_ReadNVItem(MMINV_PEDOMETER_DAY5_STEP_NV,&day_step[4]);
	MMI_ReadNVItem(MMINV_PEDOMETER_DAY6_STEP_NV,&day_step[5]);
	MMI_ReadNVItem(MMINV_PEDOMETER_DAY7_STEP_NV,&day_step[6]);

	TM_GetSysDate(&dateValue);

#ifdef WIN32
	step = 200;
#else
	step = ZDT_GSensor_GetStepOneDay();
#endif
	
	SCI_TRACE_LOW("%s: step = %d", __FUNCTION__, step);
	SCI_TRACE_LOW("%s: day1_step = %d", __FUNCTION__, day_step[0]);
	SCI_TRACE_LOW("%s: dateValue.mday = %d", __FUNCTION__, dateValue.mday);
	SCI_TRACE_LOW("%s: day_num = %d", __FUNCTION__, day_num);

	if (day_num != dateValue.mday || step != day_step[0])
	{
		if (day_num == dateValue.mday && step != day_step[0])
		{
			if (step > day_step[0])
			{
				day_step[0] = step;
			}
            else	
			{
				SCI_TRACE_LOW("%s: last_step = %d", __FUNCTION__, last_step);
				if (step > last_step)
                {
					day_step[0] += step - last_step;
				}
			}
		}
        else
		{
			int bet_day = 0;
			if (day_num != 0)
			{
				bet_day = get_number_of_days_between(dateValue.year, dateValue.mon, dateValue.mday, day_num);
				SCI_TRACE_LOW("%s: bet_day = %d", __FUNCTION__, bet_day);
				if (bet_day < 7)
				{
					for (i = 6;i > 0;i--)
					{
						if (i - bet_day >= 0)
						{
							day_step[i] = day_step[i - bet_day];
						}
                        else
						{
							day_step[i] = 0;
						}
					}
				}else
				{
					for (i = 0;i < 7;i++)
					{
						day_step[i] = 0;
					}
				}
			}
			SCI_TRACE_LOW("%s: day_step[%d] = %d", __FUNCTION__, bet_day, day_step[bet_day]);
			if(step >= day_step[bet_day])
			{
				day_step[0] = step - day_step[bet_day];
				if(day_num != 0)
                {
					ZDT_GSensor_Reset();
				}
			}
            else
            {
				day_step[0] = step;
			}
			day_num = dateValue.mday;
		}
		last_step = day_step[0];
		for(i = 0;i < 7;i++)
        {
			MMI_WriteNVItem(MMINV_PEDOMETER_DATE_NV,&day_num);
			MMI_WriteNVItem(MMINV_PEDOMETER_DAY1_STEP_NV,&day_step[0]);
			MMI_WriteNVItem(MMINV_PEDOMETER_DAY2_STEP_NV,&day_step[1]);
			MMI_WriteNVItem(MMINV_PEDOMETER_DAY3_STEP_NV,&day_step[2]);
			MMI_WriteNVItem(MMINV_PEDOMETER_DAY4_STEP_NV,&day_step[3]);
			MMI_WriteNVItem(MMINV_PEDOMETER_DAY5_STEP_NV,&day_step[4]);
			MMI_WriteNVItem(MMINV_PEDOMETER_DAY6_STEP_NV,&day_step[5]);
			MMI_WriteNVItem(MMINV_PEDOMETER_DAY7_STEP_NV,&day_step[6]);
		}
	}
	return day_step[0];
}
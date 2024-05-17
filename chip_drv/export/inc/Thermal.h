/******************************************************************************
** File Name:    	Thermal_phy.c                                                				*
** Author:         	Tianyu.Yang                                                   				*
** DATE:           	22/02/2023                                               					*
** Copyright:      	2023 Spreadtrum, Incoporated. All Rights Reserved.         		*
** Description:    	This file defines the basic thermalr manage operation process.   	*
******************************************************************************

******************************************************************************
**                        Edit History                                       						*
** -------------------------------------------------------------------------
** DATE           	NAME             	DESCRIPTION                               			*
** 22/02/2023    	Tianyu.Yang     	Create.                                  				*
******************************************************************************/

#ifndef _Thermal_H_
#define _Thermal_H_

#include "sci_api.h"

typedef struct THERMALSVR_SIG_tag {
	SIGNAL_VARS
	uint32  sig_param;
}THERMALSVR_SIG_T;

typedef enum
{
    THERMAL_CLOSE_BUSINESS = 0x1,
    THERMAL_REOPEN_BUSINESS,
    THERMAL_POWER_OFF,
    THERMAL_MSG_MAX_NUM
} THERMAL_SVR_MSG_SERVICE_E;

typedef enum {
	THERMAL_MONITOR_MSG = 1 ,
	THERMAL_MAX_MSG
} THERMAL_MSG_E;

typedef enum THERMAL_StateCode_enum
{
	IDLE_TEMP = 0,
	SKIN_TEMP,
	BOARD_TEMP,
	RF_TEMP,
	STOP_TERMAL
} THERMAL_STATE_E;

typedef struct
{
    THERMAL_STATE_E          thermal_state;
    int32                  		skin_temp;
    int32                  		board_temp;
    int32                  		rf_temp;
}THERMAL_STATE_INFO_T;

LOCAL THERMAL_STATE_INFO_T thermal_module_state =
{
    IDLE_TEMP,        //thermal_state
    25,                  //skin_temp
    0,                  //board_temp
    0,                  //rf_temp
};

LOCAL const int16 skin_temp_table[][2]=
{
	{1110       , -30},
        {1107       , -29},
        {1104       , -28},
        {1101       , -27},
        {1098       , -26},
        {1095       , -25},
        {1091       , -24},
        {1087       , -23},
        {1083       , -22},
        {1079       , -21},
        {1075       , -20},
        {1070       , -19},
        {1066       , -18},
        {1061       , -17},
        {1056       , -16},
        {1050       , -15},
        {1045       , -14},
        {1039       , -13},
        {1033       , -12},
        {1027       , -11},
        {1020       , -10},
        {1013       , -9 },
        {1006       , -8 },
        {999        , -7 },
        {992        , -6 },
        {984        , -5 },
        {976        , -4 },
        {968        , -3 },
        {959        , -2 },
        {950        , -1 },
        {941        , 0  },
        {932        , 1  },
        {923        , 2  },
        {913        , 3  },
        {903        , 4  },
        {893        , 5  },
        {882        , 6  },
        {872        , 7  },
        {861        , 8  },
        {850        , 9  },
        {839        , 10 },
        {827        , 11 },
        {816        , 12 },
        {804        , 13 },
        {792        , 14 },
        {780        , 15 },
        {768        , 16 },
        {755        , 17 },
        {743        , 18 },
        {730        , 19 },
        {718        , 20 },
        {705        , 21 },
        {692        , 22 },
        {680        , 23 },
        {667        , 24 },
        {654        , 25 },
        {641        , 26 },
        {629        , 27 },
        {616        , 28 },
        {603        , 29 },
        {591        , 30 },
        {578        , 31 },
        {565        , 32 },
        {553        , 33 },
        {541        , 34 },
        {528        , 35 },
        {516        , 36 },
        {504        , 37 },
        {492        , 38 },
        {480        , 39 },
        {469        , 40 },
        {457        , 41 },
        {446        , 42 },
        {435        , 43 },
        {424        , 44 },
        {413        , 45 },
        {403        , 46 },
        {392        , 47 },
        {382        , 48 },
        {372        , 49 },
        {362        , 50 },
        {353        , 51 },
        {343        , 52 },
        {334        , 53 },
        {325        , 54 },
        {316        , 55 },
        {307        , 56 },
        {299        , 57 },
        {291        , 58 },
        {283        , 59 },
        {275        , 60 },
        {267        , 61 },
        {260        , 62 },
        {252        , 63 },
        {245        , 64 },
        {238        , 65 },
        {232        , 66 },
        {225        , 67 },
        {219        , 68 },
        {212        , 69 },
        {206        , 70 },
        {201        , 71 },
        {195        , 72 },
        {189        , 73 },
        {184        , 74 },
        {179        , 75 },
        {174        , 76 },
        {169        , 77 },
        {164        , 78 },
        {159        , 79 },
        {155        , 80 },
        {150        , 81 },
        {146        , 82 },
        {142        , 83 },
        {138        , 84 },
        {134        , 85 },
        {130        , 86 },
        {126        , 87 },
        {123        , 88 },
        {119        , 89 },
        {116        , 90 },
        {113        , 91 },
        {110        , 92 },
        {106        , 93 },
        {104        , 94 },
        {101        , 95 },
        {98        , 96  },
        {95        , 97  },
        {93        , 98  },
        {90        , 99  },
        {87        , 100 },
};

PUBLIC void thermal_init(void);
LOCAL void _THERMAL_SrvThread (uint32 argc, void *argv);
LOCAL uint32 _THERMAL_TimerHandler (uint32 param);
PUBLIC void THERMAL_SendMsgToTHMTask(THERMAL_MSG_E sig,uint32 sig_param);
PUBLIC int32 THERMAL_Skin_Temp_Read(void);
PUBLIC uint32 THERMAL_Ctrl_Get (void);
PUBLIC void THERMAL_Ctrl_Set (uint32 value);
LOCAL void THERMAL_Read_Temperature (void);
LOCAL void THERMAL_SrvDefault (BLOCK_ID id, uint32 argc, void *argv);
LOCAL void THERMAL_Skin_Temp_Action (int32 skin_temp);
#endif
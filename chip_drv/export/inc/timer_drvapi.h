/******************************************************************************
 ** File Name:      timer_drvapi.h                                               *
 ** Author:         Richard Yang                                              *
 ** DATE:           04/02/2002                                                *
 ** Copyright:      2002 Spreadtrum, Incoporated. All Rights Reserved.         *
 ** Description:    This file defines the basic operation interfaces of       *
 **                 time count and rtc device. It manages init, get and set   *
 **                 new time interface.                                       *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 04/02/2002     Richard.Yang     Create.                                   *
 ** 04/25/2002     Lin.liu          Added rtc function prototype              *
 ** 10/08/2002     Xueliang.Wang    Modified.                                 *
 ******************************************************************************/

#ifndef _TIMER_DRVAPI_H_
#define _TIMER_DRVAPI_H_

#include "timer_ap.h"
#include "../../chip_plf/export/inc/chip_plf_export.h"

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         Debugging Flag                                    *
 **---------------------------------------------------------------------------*/
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
extern   "C"
{
#endif

#define TIMER1_BASE                     (TIMER_CTL_BASE + 0x0020)
#define TM1_LOAD                        (TIMER1_BASE + 0x0000)          //Write to this register will reload the timer with the new value.
#define TM1_VALUE                       (TIMER1_BASE + 0x0004)          //Return the current timer value.
#define TM1_CTL                         (TIMER1_BASE + 0x0008)
#define TM1_CLR                         (TIMER1_BASE + 0x000C)          //Write to this register will clear the interrupt generated by this timer.

#define TIMER2_BASE                     (TIMER_CTL_BASE + 0x0040)
#define TM2_LOAD                        (TIMER2_BASE + 0x0000)          //Write to this register will reload the timer with the new value.
#define TM2_VALUE                       (TIMER2_BASE + 0x0004)          //Return the current timer value.
#define TM2_CTL                         (TIMER2_BASE + 0x0008)
#define TM2_CLR                         (TIMER2_BASE + 0x000C)          //Write to this register will clear the interrupt generated by this timer.

#define TIMER_RUN_CTL            BIT_7
#define TIMER_INT_CLR_BIT        BIT_3
#define TIMER_INT_RAW_STS        BIT_1
#define MAX_POLLING_CNT          0xFFFF
#define TIMER2_FULL_COUNT        0xFFFF

#define REG32(x)      (*((volatile uint32 *)(x)))

#if !defined(PLATFORM_UWS6121E)
/*if timer2 is in use, delay (_x_)us first and then reload the count and execute the rest delay*/
/*the max count of timer2 is 0xFFFF*/
#define TIMER_DELAY_US(_x_)\
    {\
        volatile uint32 tm_i = 0, time_out = 0;\
        volatile uint32 flash_delay_count = 0;\
        if(REG32(TM2_CTL)  & TIMER_RUN_CTL)\
        {\
            REG32(TM2_CTL) &= ~TIMER_RUN_CTL;\
            for(tm_i = 0; tm_i < 10; tm_i++){};\
        }\
        flash_delay_count = _x_ * 26;\
        if(flash_delay_count > TIMER2_FULL_COUNT)\
        {\
           flash_delay_count = TIMER2_FULL_COUNT;\
        }\
        REG32(TM2_LOAD) = flash_delay_count;\
        for(tm_i = 0; tm_i < 10; tm_i++){};\
        REG32(TM2_CLR) |= TIMER_INT_CLR_BIT;\
        REG32(TM2_CTL) |= TIMER_RUN_CTL;\
        while(0 == (REG32(TM2_CLR) & TIMER_INT_RAW_STS))\
        {\
            time_out ++;\
            if(time_out > MAX_POLLING_CNT)\
            {\
                break;\
            }\
        }\
        REG32(TM2_CLR) |= TIMER_INT_CLR_BIT;\
        REG32(TM2_CTL) &= ~TIMER_RUN_CTL;\
    }
#else

#define TIMER2_FULL_COUNT        0xFFFFFFFF

#define GET_TIMEER_TICK() (hwp_timer2->hwtimer_curval_l)

#define TIMER_DELAY_US(_x_)\
    do{\
        volatile uint32 start = 0, ticks = 0;\
        start = GET_TIMEER_TICK(); \
		ticks = _x_*2;  \
		 while(!(((unsigned)((GET_TIMEER_TICK()) - start)) > ticks)); \
    }while(0)


#if 0
#define TIMER_DELAY_US(_x_)\
    {\
        volatile uint32 time_1 = 0, time_2 = 0;\
        volatile uint32 delay_count = 0;\
        delay_count = _x_ * 2;\
        time_1 = REG32(TIMER4_HWTIMER_CURVAL_L);\
        time_2 = time_1 + delay_count;\
        if (time_2 < time_1)\
        {\
            delay_count -= ((TIMER4_FULL_COUNT - time_1) + 1);\
            while(REG32(TIMER4_HWTIMER_CURVAL_L) >= time_1);\
            while(delay_count > REG32(TIMER4_HWTIMER_CURVAL_L));\
        }\
        else\
        {\
            while(time_2 >= REG32(TIMER4_HWTIMER_CURVAL_L));\
        }\
    }
#endif

#endif

/**---------------------------------------------------------------------------*
 **                         Constant Variables                                *
 **---------------------------------------------------------------------------*/
typedef enum
{
    TIMER_0,
    TIMER_1,
    TIMER_2,
    TIMER_3,
    TIMER_4,
    TIMER_5,
    TIMER_MAX,
} TIMER_ID_E; //the timer index

#if defined(PLATFORM_UWS6121E)
#define   OS_TIMER     TIMER_1
#endif

typedef enum
{
    TIMER_ONESHOT_MODE,
    TIMER_PERIOD_MODE,
} TIMER_WORK_MODE_E; //the timer work mode

typedef enum
{
    TIMER_UNIT_us,
    TIMER_UNIT_ms,
} TIMER_UNIT_E; //identify the config unit

// TIMER callback function prototype.
typedef void (* TIMER_CALLBACK_FUNC) (void);
typedef TIMER_CALLBACK_FUNC TB_TIMER_CALLBACK;

typedef struct
{
    TIMER_WORK_MODE_E mode;
    uint32 unit_number;
    TIMER_UNIT_E unit_type;
    TIMER_CALLBACK_FUNC call_back;
} TIMER_CONFIG_T;

typedef struct
{
    TIMER_UNIT_E unit_type;
    uint32 unit_number;
} TIMER_REMAINING_T;

#define TIMER_MILSEC_PER_SECOND 1000
#define TIMER_MICSEC_PER_MILSEC 1000

/* the command definition */
#define TIMER_IOCTL_CMD_CONFIG 1001
#define TIMER_IOCTL_CMD_START 1002
#define TIMER_IOCTL_CMD_STOP 1003
#define TIMER_IOCTL_CMD_GET_REMAINING 1004

/* the error code definition */
#define TIMER_SUCCESS 0
#define TIMER_ERR_POINTER_NULL 3001
#define TIMER_ERR_ABILITY_FAIL 3002
#define TIMER_ERR_SET_FAIL 3003
#define TIMER_ERR_INIT_FAIL 3004
#define TIMER_ERR_QUERY_FAIL 3005
#define TIMER_ERR_DISABLE_FAIL 3006
#define TIMER_ERR_CLRINT_FAIL 3007
#define TIMER_ERR_NOT_OPEN 3008
#define TIMER_ERR_ENABLE_FAIL 3009
#define TIMER_ERR_PARAM 3010
#define TIMER_ERR_OPEN_FAIL 3011
#define TIMER_ERR_NO_SUCH_TIMER 3012

/**---------------------------------------------------------------------------*
 **                      Function  Prototype
 **---------------------------------------------------------------------------*/
/*********************************************************************************************************
** Function name:
** Descriptions:
** input parameters:
**
**
**
** output parameters:
** Returned value:
*********************************************************************************************************/
int32 TIMER_HAL_Open (TIMER_ID_E timer_id);

/*********************************************************************************************************
** Function name:
** Descriptions:
** input parameters:
**
**
**
** output parameters:
** Returned value:
*********************************************************************************************************/
int32 TIMER_HAL_Close (TIMER_ID_E timer_id);

uint32 TIMER_CreatePeriodTimer (TIMER_ID_E timer_id,uint32 expire_time, TIMER_CALLBACK_FUNC tm_func,uint32 is_autoactive);
uint32 TIMER_ActivePeriodTimer(TIMER_ID_E timer_id,uint32 is_active);

/*********************************************************************************************************
** Function name:
** Descriptions:
** input parameters:
**
**
**
** output parameters:
** Returned value:
*********************************************************************************************************/
int32 TIMER_HAL_IOctl (TIMER_ID_E timer_id, uint32 cmd, void *arg);

/*********************************************************************************************************
** Function name:
** Descriptions:
** input parameters:
**
**
**
** output parameters:
** Returned value:
*********************************************************************************************************/
uint32 TIMER_GetSystemCounterReg (void);

/*********************************************************************************************************
** Function name:
** Descriptions:
** input parameters:
**
**
**
** output parameters:
** Returned value:
*********************************************************************************************************/
void TIMER_DelayUs (uint32 us);

/*********************************************************************************************************
** Function name:
** Descriptions:
** input parameters:
**
**
**
** output parameters:
** Returned value:
*********************************************************************************************************/
void TIMER_ReLoad (void);

uint32 TIMER_GetCntValue (TIMER_ID_E timer_id);

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
}
#endif

#endif  // _TIMER_DRVAPI_H_


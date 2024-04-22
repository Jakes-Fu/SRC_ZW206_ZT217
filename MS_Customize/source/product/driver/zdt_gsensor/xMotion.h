/*******************************************************************************/
 /**
  ******************************************************************************
  * @file    xMotion.h
  * @author  ycwang@miramems.com
  * @version V1.0
  * @date    26-Nov-2014
  * @brief
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MiraMEMS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 MiraMEMS</center></h2>
  */
/*******************************************************************************/
#ifndef __XMOTION_h
#define __XMOTION_h

/*******************************************************************************
Includes
********************************************************************************/
#include "mira_std.h"

/*******************************************************************************
Macro definitions - Algorithm Build Options
********************************************************************************/
#define PEDOMETER_ENABLE    1
#define FALL_ENABLE         1
#define SLEEP_ENABLE        1
#define ACTION_ENABLE       1
#define SEDENTARY_ENABLE    1
#define CALORIE_ENABLE      1
#define FLIP_ENABLE         1
#define RAISE_ENABLE        1
#define SHAKE_ENABLE        1

/*******************************************************************************
Typedef definitions
********************************************************************************/
typedef enum {
  EVENT_NONE,
  EVENT_STEP_NOTIFY,
  EVENT_FALL_NOTIFY,
  EVENT_SLEEP_NOTIFY,
  EVENT_ACTION_NOTIFY,
  EVENT_SEDENTARY_NOTIFY,
  EVENT_CALORIE_NOTIFY,
  EVENT_FLIP_NOTIFY,
  EVENT_RAISE_NOTIFY,
  EVENT_SHAKE_NOTIFY
}XMOTION_EVENT;

typedef enum {
  NONE_X,
  PEDOMETER_X,
  FALL_X,
  SLEEP_X,
  ACTION_X,
  SEDENTARY_X,
  CALORIE_X,
  FLIP_X,
  RAISE_X,
  SHAKE_X
}Algorithm;

typedef enum {
  DISABLE_X,
  ENABLE_X
}SwitchCmd;

typedef enum {
  SLEEP_FALL,
  SLEEP_LIGHT_1,
  SLEEP_DEEP,
  SLEEP_LIGHT_2,
  SLEEP_UNWEAR
}Tsleep;	//sleep state

typedef enum {
  MOTIONLESS,
  WALK,
  RUN
}Taction;	//action state

typedef enum {
  RAISE_NONE = 0,
  RAISE_ON   = 1,
  RAISE_OFF  = -1
}Traise;	//raise state

typedef enum {
  SHAKE_NONE  = 0x00,
  SHAKE_RIGHT = 0x01,          
  SHAKE_LEFT  = 0x02,
  SHAKE_FRONT = 0x04,
  SHAKE_BACK  = 0x08,
  SHAKE_UP    = 0x10,
  SHAKE_DOWN  = 0x20
}Tshake;	//shake state

struct xMotion_op_s {
  s8_m (*event_send)(XMOTION_EVENT event, s32_m data);
  s8_m (*mir3da_register_read)(u8_m addr, u8_m *data, u8_m len);
  s8_m (*mir3da_register_write)(u8_m addr, u8_m data);
  s8_m (*mir3da_save_cali_file)(s16_m x, s16_m y,s16_m z);
  s8_m (*mir3da_get_cali_file)(s16_m *x, s16_m *y,s16_m *z);
};  

/*******************************************************************************
Global variables and functions
********************************************************************************/

/*******************************************************************************
* Function Name: xMotion_Init
* Description  : This function initializes the xMotion.
* Arguments    : None
* Return Value : 0: OK; -1: Type Error; -2: OP Error; -3: Chip Error
********************************************************************************/
s8_m xMotion_Init(struct xMotion_op_s *ops);

#if SEDENTARY_ENABLE
/*******************************************************************************
* Function Name: xMotion_Set_Sedentary_Parma
* Description  : This function sets sedentary parmas.
* Arguments    : time - the time to monitor sedentary, default is 30 mins(1~120)
                 interval - the interval to report the event, default is 0 mins(0~30)
* Return Value : None
********************************************************************************/
void xMotion_Set_Sedentary_Parma(u16_m time, u16_m interval);
#endif

#if CALORIE_ENABLE
/*******************************************************************************
* Function Name: xMotion_Set_Calorie_Parma
* Description  : This function sets calorie parmas.
* Arguments    : height - the user's height, default is 180 cm
                 weight - the user's weight, default is 70 kg
* Return Value : None
********************************************************************************/
void xMotion_Set_Calorie_Parma(u16_m height, u16_m weight);
#endif

#if RAISE_ENABLE
/*******************************************************************************
* Function Name: xMotion_Set_Raise_Parma
* Description  : This function sets raise parmas.
* Arguments    : dir - direction of coordinate axes, default is 0 (0~7)
* Return Value : None
********************************************************************************/
void xMotion_Set_Raise_Parma(u8_m dir);
#endif

#if SHAKE_ENABLE
/*******************************************************************************
* Function Name: xMotion_Set_Shake_Parma
* Description  : This function sets shark parmas.
* Arguments    : level - the sensitivity(1~6), default is 2
* Return Value : None
********************************************************************************/
void xMotion_Set_Shake_Parma(u8_m level);
#endif

/*******************************************************************************
* Function Name: xMotion_Control
* Description  : This function initializes the xMotion.
* Arguments    : name - select which algorithm to control
                 cmd - enable/disable
* Return Value : None
********************************************************************************/
void xMotion_Control(Algorithm name, SwitchCmd cmd);

/*******************************************************************************
* Function Name: xMotion_Process_Data
* Description  : This function runs the xMotion at 20Hz.
* Arguments    : None
* Return Value : None
********************************************************************************/
void xMotion_Process_Data(void);

/*******************************************************************************
* Function Name: xMotion_Get_Version
* Description  : This function gets xMotion version.
* Arguments    : None
* Return Value : Version Num
********************************************************************************/
u8_m xMotion_Get_Version(void);

/*******************************************************************************
* Function Name: xMotion_QueryControl
* Description  : This function gets algorithm state.
* Arguments    : name - select which algorithm
* Return Value : SwitchCmd - enable/disable
********************************************************************************/
SwitchCmd xMotion_QueryControl(Algorithm name);

/*******************************************************************************
* Function Name: xMotion_QueryEvent
* Description  : This function gets algorithm value.
* Arguments    : event - select which event
* Return Value : Correspond event value
********************************************************************************/
s32_m xMotion_QueryEvent(XMOTION_EVENT event);

/*******************************************************************************
* Function Name: xMotion_chip_check_id
* Description  : This function checks the chip id.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m xMotion_chip_check_id(void);

/*******************************************************************************
* Function Name: xMotion_chip_read_xyz
* Description  : This function reads the chip xyz.
* Arguments    : x, y, z - acc data
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m xMotion_chip_read_xyz(s16_m *x,s16_m *y,s16_m *z);

/*******************************************************************************
* Function Name: xMotion_chip_calibrate_offset
* Description  : This function calibrates the offset.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m xMotion_chip_calibrate_offset(void);

/*******************************************************************************
* Function Name: xMotion_chip_power_on
* Description  : This function enables the chip.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m xMotion_chip_power_on(void);

/*******************************************************************************
* Function Name: xMotion_chip_power_off
* Description  : This function disables on the chip.
* Arguments    : None
* Return Value : 0: OK; -1: Error
********************************************************************************/
s8_m xMotion_chip_power_off(void);

#endif 

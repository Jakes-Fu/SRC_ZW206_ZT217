/******************************************************************************
 ** File Name:      msensor_mir3da.c                                          *
 ** Author:          Yc.Wang                                                *
 ** DATE:            2015.08.31                                                *
 ** Copyright:      2012 Spreadtrum, Incoporated. All Rights Reserved.        *
 ** Description:                                                              *
 **                                                                           *
 **                                                                           *
 ******************************************************************************

 ******************************************************************************
 **                               Edit History                                *
 ** ------------------------------------------------------------------------- *
 **     DATE          NAME             DESCRIPTION                            *
 **  2015.08.31   Yc.Wang            Create.                               *
 ******************************************************************************/
/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "ms_customize_trc.h"
#include "i2c_api.h"
#include "os_api.h"
#include "gpio_prod_api.h"
#include "gpio_prod_cfg.h"
#include "Power_cfg.h"
#include "mira_std.h"
#include "xMotion.h"
#include "zdt_gsensor.h"

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern   "C" 
{
#endif
/**---------------------------------------------------------------------------*
 **                         Macro Definition                                  *
 **---------------------------------------------------------------------------*/

#ifdef WIN32
#define __func__ 
#endif


#define DATA_VECTOR_LENGTH 4
#define SHAKE_ABSTH MIR3DA_THRESHOLD_2_0G

// according to datasheet, 1 mg/digit on 0--2G scale, MOCOR's unit: mg
#define MIR3DA_DIGIT_TO_MG             4

#define MIR3DA_THRESHOLD_0_2G            200
#define MIR3DA_THRESHOLD_0_4G            350
#define MIR3DA_THRESHOLD_0_5G            512
#define MIR3DA_THRESHOLD_0_6G            600
#define MIR3DA_THRESHOLD_0_7G            700
#define MIR3DA_THRESHOLD_0_8G            800
#define MIR3DA_THRESHOLD_0_9G            900
#define MIR3DA_THRESHOLD_1_0G            1024  

#define MIR3DA_THRESHOLD_1_1G            1100
#define MIR3DA_THRESHOLD_1_2G            1200
#define MIR3DA_THRESHOLD_1_5G            1500
#define MIR3DA_THRESHOLD_1_8G            1800
#define MIR3DA_THRESHOLD_2_0G            2000  
#define MIR3DA_THRESHOLD_2_5G            2500  


#define TIMER_PERIOD_MIR3DA         50

#define SHAKE_INIT  0x00
#define SHAKE_FIRST_PEAK 0x01
#define SHAKE_SENCOND_PEAK 0x02 
#define SHAKE_THIRD_JUDGE 0x03
#define SHAKE_WAIT  0x04

#define SHAKEDELAY_TIMEOUT  10
#define SHAKEDELAY_INTERVAL 15

/* Register define for NSA asic */
#define NSA_REG_SPI_I2C                 0x00
#define NSA_REG_WHO_AM_I                0x01
#define NSA_REG_ACC_X_LSB               0x02
#define NSA_REG_ACC_X_MSB               0x03
#define NSA_REG_ACC_Y_LSB               0x04
#define NSA_REG_ACC_Y_MSB               0x05
#define NSA_REG_ACC_Z_LSB               0x06
#define NSA_REG_ACC_Z_MSB               0x07 
#define NSA_REG_G_RANGE                 0x0f
#define NSA_REG_ODR_AXIS_DISABLE        0x10
#define NSA_REG_POWERMODE_BW            0x11
#define NSA_REG_SWAP_POLARITY           0x12
#define NSA_REG_FIFO_CTRL               0x14
#define NSA_REG_INTERRUPT_SETTINGS1     0x16
#define NSA_REG_INTERRUPT_SETTINGS2     0x17
#define NSA_REG_INTERRUPT_MAPPING1      0x19
#define NSA_REG_INTERRUPT_MAPPING2      0x1a
#define NSA_REG_INTERRUPT_MAPPING3      0x1b
#define NSA_REG_INT_PIN_CONFIG          0x20
#define NSA_REG_INT_LATCH               0x21
#define NSA_REG_ACTIVE_DURATION         0x27
#define NSA_REG_ACTIVE_THRESHOLD        0x28
#define NSA_REG_TAP_DURATION            0x2A
#define NSA_REG_TAP_THRESHOLD           0x2B
#define NSA_REG_CUSTOM_OFFSET_X         0x38
#define NSA_REG_CUSTOM_OFFSET_Y         0x39
#define NSA_REG_CUSTOM_OFFSET_Z         0x3a
#define NSA_REG_ENGINEERING_MODE        0x7f
#define NSA_REG_SENSITIVITY_TRIM_X      0x80
#define NSA_REG_SENSITIVITY_TRIM_Y      0x81
#define NSA_REG_SENSITIVITY_TRIM_Z      0x82
#define NSA_REG_COARSE_OFFSET_TRIM_X    0x83
#define NSA_REG_COARSE_OFFSET_TRIM_Y    0x84
#define NSA_REG_COARSE_OFFSET_TRIM_Z    0x85
#define NSA_REG_FINE_OFFSET_TRIM_X      0x86
#define NSA_REG_FINE_OFFSET_TRIM_Y      0x87
#define NSA_REG_FINE_OFFSET_TRIM_Z      0x88
#define NSA_REG_SENS_COMP               0x8c
#define NSA_REG_MEMS_OPTION             0x8f
#define NSA_REG_CHIP_INFO               0xc0
#define NSA_REG_SENS_COARSE_TRIM        0xd1
                                         
#define MIR3DA_ODR_50HZ                  0
#define MIR3DA_ODR_100HZ                 1
#define MIR3DA_ODR_200HZ                 2


#define MIR3DA_I2C_WRITE_ADDR       0x4e
#define MIR3DA_I2C_READ_ADDR         0x4f
#define MIR3DA_I2C_ERROR 1
#define MIR3DA_I2C_SUCCESS 0


/**---------------------------------------------------------------------------*
 **                         DataStruct Definition                             *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         Variables and function                            *
 **---------------------------------------------------------------------------*/ 

LOCAL void   MIR3DA_TimerCallback(uint32 param);


LOCAL BOOLEAN              s_MIR3DA_is_init = FALSE;
LOCAL int32              s_MIR3DA_IIC_Handler = -1;
LOCAL SCI_TIMER_PTR      s_MIR3DA_timer;

LOCAL I2C_DEV MIR3DA_I2C_cfg =
{
    1,          
    100000,                   // freq is standard
    0,                          // bus
    MIR3DA_I2C_WRITE_ADDR,  // slave_addr
    1,                          // reg_addr_num
    1,                          // check_ack
    0                           // no_stop
};


LOCAL uint32 _Get_MIR3DA_I2C_Status(void)
{
    if(s_MIR3DA_IIC_Handler==-1)
    {
        return MIR3DA_I2C_ERROR;
    }
    else
    {
        return MIR3DA_I2C_SUCCESS;
    }
}

LOCAL uint32 MIR3DA_I2C_Open(void)
{    
    if(s_MIR3DA_IIC_Handler == -1) 
    {
        if((s_MIR3DA_IIC_Handler = I2C_HAL_Open(&MIR3DA_I2C_cfg)) == -1)
        {
            return MIR3DA_I2C_ERROR;
        } 
        else
        {
            return MIR3DA_I2C_SUCCESS;
        }
    }
    else
    {
        return MIR3DA_I2C_SUCCESS;
    }
}

/******************************************************************************/
// Description: MIR3DA_I2C_Close
// Dependence: 
// Note:
/******************************************************************************/

LOCAL uint32 MIR3DA_I2C_Close(void)
{
    if(_Get_MIR3DA_I2C_Status() == MIR3DA_I2C_SUCCESS)
    {

        I2C_HAL_Close(s_MIR3DA_IIC_Handler);
        s_MIR3DA_IIC_Handler = -1;
    }	
    return MIR3DA_I2C_SUCCESS;
}
/******************************************************************************/
// Description: MIR3DA_I2C_Write
// Dependence: 
// Note:
/******************************************************************************/
LOCAL void MIR3DA_I2C_Write(uint8 *reg, uint8 *addr, uint32 cnt)
{
    if(_Get_MIR3DA_I2C_Status() == MIR3DA_I2C_SUCCESS)
        I2C_HAL_Write(s_MIR3DA_IIC_Handler, reg, addr, cnt);
}

/******************************************************************************/
// Description: MIR3DA_I2C_Read
// Dependence: 
// Note:
/******************************************************************************/
LOCAL void MIR3DA_I2C_Read(uint8 *reg, uint8 *addr, uint32 cnt)
{
    if(_Get_MIR3DA_I2C_Status() == MIR3DA_I2C_SUCCESS)
        I2C_HAL_Read(s_MIR3DA_IIC_Handler,reg, addr, cnt);
}

unsigned char mir3da_register_read(unsigned char addr,unsigned char *data)
{
    MIR3DA_I2C_Read(&addr,data,1);
    return 0;
}

unsigned char mir3da_register_read_cnt(unsigned char addr,unsigned char *data,unsigned char len)
{
    MIR3DA_I2C_Read(&addr,data,len);
    return 0;
}

unsigned char mir3da_register_write(unsigned char addr,unsigned char data)
{
    MIR3DA_I2C_Write(&addr,&data,1);
    return 0;
}

LOCAL void MIR3DA_read_data(short *x,short *y,short *z)
{
    xMotion_chip_read_xyz(x,y,z);
    return;
}


LOCAL void MIR3DA_open_interrupt(int num)
{
    mir3da_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x03);
    mir3da_register_write(NSA_REG_ACTIVE_DURATION,0x03 );
/*
    Threshold of active interrupt= active _th*mg/LSB
    LSB = 3.91mg (2g range)
    LSB = 7.81mg (4g range)
    LSB = 15.625mg (8g range)
    LSB = 31.25mg (16g range)
*/
    mir3da_register_write(NSA_REG_ACTIVE_THRESHOLD,0x1B ); //Old: 0x1B
			
    switch(num){

    	case 0:
    		mir3da_register_write(NSA_REG_INTERRUPT_MAPPING1,0x04 );
    		break;

    	case 1:
    		mir3da_register_write(NSA_REG_INTERRUPT_MAPPING3,0x04 );
    		break;
    }
    return;
}

LOCAL void MIR3DA_close_interrupt(int num)
{
	mir3da_register_write(NSA_REG_INTERRUPT_SETTINGS1,0x00 );
			
	switch(num){

		case 0:
			mir3da_register_write(NSA_REG_INTERRUPT_MAPPING1,0x00 );
			break;

		case 1:
			mir3da_register_write(NSA_REG_INTERRUPT_MAPPING3,0x00 );
			break;
	}

	return;
}

char sendEvent(XMOTION_EVENT event,int data)
{
    switch(event)
    {
        case  EVENT_FALL_NOTIFY:
                ZDT_GSensor_Fail();
        	break;
            
        case  EVENT_STEP_NOTIFY:
                #ifdef FILE_LOG_SUPPORT
                    ZDT_GSensor_Step((uint32)data);
                #endif
            break;
        default:
            break;
    }
    return 0;
}

struct xMotion_op_s     ops_handle = {sendEvent,mir3da_register_read_cnt, mir3da_register_write,NULL,NULL};

BOOLEAN  MIR3DA_init(void)
{
    uint8   device_id = 0;
    uint32 ret = 0;

    ret = MIR3DA_I2C_Open();
    if(ret)
    {
        ZDT_GSENSOR_TRACE("MIR3DA_init ERR I2C");
        return FALSE;
    }
    SCI_Sleep(20);
    mir3da_register_read(NSA_REG_WHO_AM_I,&device_id);
    
    if(0x13 != device_id)
    {
        MIR3DA_I2C_Close();
        MIR3DA_I2C_cfg.slave_addr = 0x4c;
        MIR3DA_I2C_Open();
        mir3da_register_read(NSA_REG_WHO_AM_I,&device_id);
        if(0x13 != device_id){
            MIR3DA_I2C_Close();
            ZDT_GSENSOR_TRACE("MIR3DA_init ERR ID 0x%x",device_id);
            return FALSE;
        }
    }
    mir3da_register_write(NSA_REG_SPI_I2C, 0x24);
    SCI_Sleep(20);
    
    //xMotion_install_general_ops(&ops_handle);

    xMotion_Init(&ops_handle);
    SCI_Sleep(20);

    //xMotion_chip_calibrate_offset();
    //xMotion_Set_Pedometer_Parma(5, 0, 0, 0);
    
    s_MIR3DA_is_init = TRUE;
    ZDT_GSENSOR_TRACE("MIR3DA_init OK 0x%x",device_id);
    
    return TRUE;
}

/*****************************************************************************/
//  Description:    open g-sensor chip
//  Author:         
//  Note:           
/*****************************************************************************/
BOOLEAN MIR3DA_open(void)
{  
    ZDT_GSENSOR_TRACE("MIR3DA_open is_init=%d",s_MIR3DA_is_init);
    if(s_MIR3DA_is_init == FALSE)
    {
        return FALSE;
    }
    MIR3DA_I2C_Open();
    mir3da_register_write(NSA_REG_POWERMODE_BW, 0x5E);
    xMotion_Control(PEDOMETER_X,ENABLE_X);
    xMotion_Control(FALL_X,ENABLE_X);

    if(s_MIR3DA_timer == NULL)
    {
        s_MIR3DA_timer = SCI_CreateTimer("MIR3DA_Timer", MIR3DA_TimerCallback,
                                    1, TIMER_PERIOD_MIR3DA, SCI_AUTO_ACTIVATE);
    }
    else
    {
        SCI_ChangeTimer(s_MIR3DA_timer, MIR3DA_TimerCallback, TIMER_PERIOD_MIR3DA);
        SCI_ActiveTimer(s_MIR3DA_timer);	
    }
    return TRUE;
}	
/*****************************************************************************/
//  Description:    close g-sensor chip
//  Author:         
//  Note:           
/*****************************************************************************/
BOOLEAN MIR3DA_close(void)
{
    if(s_MIR3DA_is_init == FALSE)
    {
        return TRUE;
    }
    
    if(s_MIR3DA_timer != NULL)
    {
        SCI_DeactiveTimer(s_MIR3DA_timer);
    }
    
    xMotion_Control(FALL_X,DISABLE_X);
    xMotion_Control(PEDOMETER_X,DISABLE_X);
    mir3da_register_write(NSA_REG_POWERMODE_BW,0x80);

    MIR3DA_I2C_Close();
    ZDT_GSENSOR_TRACE("MIR3DA_close");
    return TRUE;
}

BOOLEAN MIR3DA_Reset(void)
{
    xMotion_Control(PEDOMETER_X,DISABLE_X);
    SCI_Sleep(2);
    xMotion_Control(PEDOMETER_X,ENABLE_X);
    return TRUE;
}

/******************************************************************************/
// Description:   Handle timer event
// Dependence: 
// Author:        Yc.Wang
// Note:
/******************************************************************************/
LOCAL void MIR3DA_TimerCallback(uint32 param)
{	    
    ZDT_GSensor_TimerOut();
    SCI_ChangeTimer(s_MIR3DA_timer, MIR3DA_TimerCallback, TIMER_PERIOD_MIR3DA);
    SCI_ActiveTimer(s_MIR3DA_timer);
    return ;
}


BOOLEAN MIR3DA_GetEvent(void)
{    
    short x = 0;
    short y = 0;
    short z = 0;
    MIR3DA_read_data(&x,&y,&z);
    xMotion_Process_Data();
    //ZDT_GSENSOR_TRACE("MIR3DA_GetEvent x=%d,y=%d,z=%d",x,y,z);
    return TRUE;
}

#ifdef __cplusplus
}
#endif  // End of file



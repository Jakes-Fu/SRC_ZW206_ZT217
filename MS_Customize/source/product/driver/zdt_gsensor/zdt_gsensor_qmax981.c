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
#include "zdt_gsensor_qmax981.h"

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

/******************************************************
	I2C header (slaver address|W/R)   macro define
*******************************************************/

/******************************************************
	GPIO macro define for software I2C
*******************************************************/

//==============================
// 供上层查询用的+-2G 范围
//==============================
#if defined(QMAX981_STEPCOUNTER)
#define ACC_0G_X      2048
#define ACC_1G_X      (2048+64)
#define ACC_MINUS1G_X (2048-64)
#define ACC_0G_Y      2048   
#define ACC_1G_Y      (2048+64)
#define ACC_MINUS1G_Y (2048-64)
#define ACC_0G_Z      2048       
#define ACC_1G_Z      (2048+64)
#define ACC_MINUS1G_Z (2048-64)
#else
#define ACC_0G_X      2048
#define ACC_1G_X      (2048+128)
#define ACC_MINUS1G_X (2048-128)
#define ACC_0G_Y      2048   
#define ACC_1G_Y      (2048+128)
#define ACC_MINUS1G_Y (2048-128)
#define ACC_0G_Z      2048       
#define ACC_1G_Z      (2048+128)
#define ACC_MINUS1G_Z (2048-128)
#endif
/*
//ADC value configure of 0g 1g and -1g in X,Y,Z axis

#define ACC_0G_X      (2048) 			
#define ACC_1G_X      (2048+256)
#define ACC_MINUS1G_X (2048-256)
#define ACC_0G_Y      (2048)
#define ACC_1G_Y      (2048+256)
#define ACC_MINUS1G_Y (2048-256)
#define ACC_0G_Z      (2048)
#define ACC_1G_Z      (2048+256)
#define ACC_MINUS1G_Z (2048-256)
*/
//==============================
// 供底层轮询模拟中断用的+-16G 范围
//==============================
#define ACC_POLL_0G_X      16384
#define ACC_POLL_1G_X      (16384+1024)
#define ACC_POLL_MINUS1G_X (16384-1024)
#define ACC_POLL_0G_Y      16384   
#define ACC_POLL_1G_Y      (16384+1024)
#define ACC_POLL_MINUS1G_Y (16384-1024)
#define ACC_POLL_0G_Z      16384       
#define ACC_POLL_1G_Z      (16384+1024)
#define ACC_POLL_MINUS1G_Z (16384-1024)

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
typedef struct 
{
    uint16  X;
    uint16  Y;
    uint16  Z;  
} SPARAMETERS;

/*========================================================================================================
										D E B U G
========================================================================================================*/
#define QMAX981_DEBUG
#if defined(QMAX981_DEBUG)
#define QMAX981_INFO			SCI_TRACE_LOW
#define QMAX981_ERROR			SCI_TRACE_LOW
#else
#ifndef WIN32
#define QMAX981_INFO(fmt, arg...)			do {} while (0)
#define QMAX981_ERROR(fmt, arg...)			do {} while (0)
#endif
#endif
/*========================================================================================================
										E X T E R N 
========================================================================================================*/
/*========================================================================================================
										D E F I N E
========================================================================================================*/
//-------------------------------------
// I2C
//-------------------------------------
#define QMAX981_SLAVE_ADDR			0x24   // FAE: 这个芯片I2C 7位地址只有0x12 0x13 两种
#define QMAX981_SLAVE_ADDR_2		0x26   // FAE: 这个芯片I2C 7位地址只有0x12 0x13 两种
#define QMAX981_I2C_ERROR 1
#define QMAX981_I2C_SUCCESS 0

//#define QMAX981_USE_SW_IIC

/*========================================================================================================
										T Y P E D E F
========================================================================================================*/
enum
{
	QMAX981_AXIS_X     =     0,
	QMAX981_AXIS_Y     =     1,
	QMAX981_AXIS_Z     =     2,
	QMAX981_AXES_NUM   =     3
};

struct hwmsen_convert {
	short sign[4];
	short map[4];
};

struct qmaX981_data
{
	uint8			chip_id;
	qmaX981_chip_type	chip_type;	
	uint8			layout;
	int16			lsb_1g;					
	uint32			step;
#if defined(QMAX981_USE_INT1) 
	uint8			int1_no;
	uint8			int1_level;
#endif
#if 0//defined(QMA6891_EXTRA_FUNC_3) 
	uint8			tap_int1_no;
	uint8			tap_int1_level;
	uint8			step_int1_level;
#endif
};

/*========================================================================================================
										V A R I A B L E S
========================================================================================================*/

LOCAL struct qmaX981_data g_qmaX981;
LOCAL struct hwmsen_convert g_map;

LOCAL int32              s_QMAX981_IIC_Handler = -1;
LOCAL SCI_TIMER_PTR      s_QMAX981_timer;

//#define QMAX981_USE_CALI

#if defined(QMAX981_USE_CALI)
#define QMAX981_CALI_FILE		L"Z:\\qmax981cali.conf"
#define QMAX981_LSB_1G			64			// mg
#define QMAX981_CALI_NUM		20    
LOCAL int qmax981_cali[3]={0, 0, 0};
LOCAL char qmax981_calied_flag = 0;
LOCAL void qmax981_read_file(uint16 * filename, char *data, int len);
LOCAL void qmax981_write_file(uint16 * filename, char *data, int len);
#endif


#if defined(QMAX981_CHECK_ABNORMAL_DATA)
extern int qmaX981_check_abnormal_data(int data_in, int *data_out);
#endif
#if defined(QMAX981_STEP_COUNTER_USE_INT)
extern void qmaX981_step_debounce_reset(void);
extern int qmaX981_step_debounce_int_work(int data, unsigned char irq_level);
extern int qmaX981_step_debounce_read_data(int result);
#endif


LOCAL I2C_DEV QMAX981_I2C_cfg =
{
    1,          
    100000,                   // freq is standard
    0,                          // bus
    QMAX981_SLAVE_ADDR,  // slave_addr
    1,                          // reg_addr_num
    1,                          // check_ack
    0                           // no_stop
};


LOCAL uint32 _Get_QMAX981_I2C_Status(void)
{
    if(s_QMAX981_IIC_Handler==-1)
    {
        return QMAX981_I2C_ERROR;
    }
    else
    {
        return QMAX981_I2C_SUCCESS;
    }
}

LOCAL uint32 QMAX981_I2C_Open(void)
{    
    if(s_QMAX981_IIC_Handler == -1) 
    {
        if((s_QMAX981_IIC_Handler = I2C_HAL_Open(&QMAX981_I2C_cfg)) == -1)
        {
            return QMAX981_I2C_ERROR;
        } 
        else
        {
            return QMAX981_I2C_SUCCESS;
        }
    }
    else
    {
        return QMAX981_I2C_SUCCESS;
    }
}

/******************************************************************************/
// Description: QMAX981_I2C_Close
// Dependence: 
// Note:
/******************************************************************************/

LOCAL uint32 QMAX981_I2C_Close(void)
{
    if(_Get_QMAX981_I2C_Status() == QMAX981_I2C_SUCCESS)
    {

        I2C_HAL_Close(s_QMAX981_IIC_Handler);
        s_QMAX981_IIC_Handler = -1;
    }	
    return QMAX981_I2C_SUCCESS;
}
/******************************************************************************/
// Description: QMAX981_I2C_Write
// Dependence: 
// Note:
/******************************************************************************/
LOCAL void QMAX981_I2C_Write(uint8 *reg, uint8 *addr, uint32 cnt)
{
    if(_Get_QMAX981_I2C_Status() == QMAX981_I2C_SUCCESS)
        I2C_HAL_Write(s_QMAX981_IIC_Handler, reg, addr, cnt);
}

/******************************************************************************/
// Description: QMAX981_I2C_Read
// Dependence: 
// Note:
/******************************************************************************/
LOCAL void QMAX981_I2C_Read(uint8 *reg, uint8 *addr, uint32 cnt)
{
    if(_Get_QMAX981_I2C_Status() == QMAX981_I2C_SUCCESS)
        I2C_HAL_Read(s_QMAX981_IIC_Handler,reg, addr, cnt);
}
/*========================================================================================================
										F U N C T I O N----LOCAL
========================================================================================================*/
LOCAL void qmax981_delay_ms(uint32 delay)
{
    SCI_Sleep(delay);
}


//======================================
// [G-sensor]: i2c 读写函数
//======================================
BOOLEAN qmax981_i2c_write_byte(uint8 ucBufferIndex, uint8 pucData)
{
    QMAX981_I2C_Write(&ucBufferIndex,&pucData,1);
    return TRUE;
}

//#define qmax981_i2c_read_bytes(reg_no,buffer_name,length) ms_i2c_receive(QMAX981_SLAVE_ADDR, reg_no, buffer_name, length)

BOOLEAN qmax981_i2c_read_bytes(uint8 reg_no, uint8* buffer_name, uint32 length)
{
    QMAX981_I2C_Read(&reg_no,buffer_name,length);
    return TRUE;
}

/*========================================================================================================
										F U N C T I O N----public
========================================================================================================*/
#if defined(QMAX981_STEP_COUNTER_USE_INT)
void qmaX981_step_eint_hisr(void)
{
	BOOLEAN ret;
	unsigned char data[2];
	int result, i;

	for(i=0; i<3; i++)
	{
		ret = qmax981_i2c_read_bytes(QMAX981_STEP_CNT_L, data, 2);
		if(ret)
		{
			break;
		}
	}
	result = (data[1]<<8)|data[0];
#ifndef WIN32
	QMAX981_INFO( "[%s]: hisr read data = %d!", __func__, result);
#endif
	if(g_qmaX981.int1_level == 0)
	{
		g_qmaX981.int1_level = 1;
		EINT_Set_Polarity(g_qmaX981.int1_no, TRUE); 
	}
	else
	{
		g_qmaX981.int1_level = 0;
		EINT_Set_Polarity(g_qmaX981.int1_no, FALSE); 
	}

	if(ret == FALSE)
	{
	#ifndef WIN32
		QMAX981_ERROR( "qmaX981_sensor_eint1_hisr read step error!!");
	#endif
		return;
	}

	qmaX981_step_debounce_int_work(result, g_qmaX981.int1_level);
}
#endif

#if defined(QMA6891_EXTRA_FUNC_3)
extern BOOLEAN             kbd_push_assert;
extern kbd_buffer_struct    kbd_buffer;
LOCAL void tap_kbd_push_onekey(kbd_event event,uint8 key)  
{
	ASSERT(kbd_push_assert == FALSE);
	kbd_push_assert = TRUE;
	kbd_buffer.kbd_data_buffer[kbd_buffer.kbd_data_buffer_windex].Keyevent = event;
	kbd_buffer.kbd_data_buffer[kbd_buffer.kbd_data_buffer_windex].Keydata[0] = key;
	//kbd_push_time_stamp();
	kbd_buffer.kbd_data_buffer_windex++;
	kbd_buffer.kbd_data_buffer_windex &= (kbd_buffer_size-1);
	kbd_push_assert = FALSE;
}

void qmaX981_tap_eint_hisr(void)
{
	ilm_struct 	*ilm_ptr = NULL;

#ifndef WIN32
	QMAX981_ERROR( "qmaX981_tap_eint_hisr ");
#endif
	tap_kbd_push_onekey(kbd_onekey_press, DEVICE_KEY_END);
	DRV_BuildPrimitive(ilm_ptr,
				MOD_DRVKBD,
				MOD_UEM,
				MSG_ID_DRVUEM_KEYPAD_IND,
				NULL);
	msg_send_ext_queue(ilm_ptr); 

	tap_kbd_push_onekey(kbd_onekey_release, DEVICE_KEY_END);
	DRV_BuildPrimitive(ilm_ptr,
				MOD_DRVKBD,
				MOD_UEM,
				MSG_ID_DRVUEM_KEYPAD_IND,
				NULL);
	msg_send_ext_queue(ilm_ptr); 
}
#endif

#if defined(QMAX981_USE_INT1)
void qmaX981_eint1_enable(unsigned char flag)
{
	if(flag)
		EINT_Mask(g_qmaX981.int1_no);
	else
		EINT_UnMask(g_qmaX981.int1_no);
}

void qmaX981_sensor_eint1_hisr(void)
{
#if defined(QMA6891_EXTRA_FUNC_3)
	qmaX981_tap_eint_hisr();
#else 
#if defined(QMAX981_STEP_COUNTER_USE_INT)
	qmaX981_step_eint_hisr();
#endif
#endif
}

LOCAL void qmaX981_setup_int1(void)
{
	g_qmaX981.int1_no = custom_eint_get_channel(motion_senosr_eint_chann);
#ifndef WIN32
	QMAX981_INFO("[%s]: g_qmaX981.int1_no = %d!\r\n", __func__, g_qmaX981.int1_no);
#endif
#if defined(QMAX981_STEP_COUNTER_USE_INT)
	qmaX981_step_debounce_reset();
#endif
	//EINT_SW_Debounce_Modify(g_qmaX981.int1_no, 10);
	//EINT_Set_HW_Debounce(g_qmaX981.int1_no, 10);
	EINT_Set_Sensitivity(g_qmaX981.int1_no, EDGE_SENSITIVE);
	EINT_Registration(g_qmaX981.int1_no, TRUE, FALSE, qmaX981_sensor_eint1_hisr, TRUE);	
	EINT_Set_Polarity(g_qmaX981.int1_no,FALSE);	
	EINT_UnMask(g_qmaX981.int1_no);
}
#endif

#if 0//defined(QMA6891_EXTRA_FUNC_3)
extern BOOLEAN             kbd_push_assert;
extern kbd_buffer_struct    kbd_buffer;
LOCAL void tap_kbd_push_onekey(kbd_event event,uint8 key)  
{
	ASSERT(kbd_push_assert == FALSE);
	kbd_push_assert = TRUE;
	kbd_buffer.kbd_data_buffer[kbd_buffer.kbd_data_buffer_windex].Keyevent = event;
	kbd_buffer.kbd_data_buffer[kbd_buffer.kbd_data_buffer_windex].Keydata[0] = key;
	//kbd_push_time_stamp();
	kbd_buffer.kbd_data_buffer_windex++;
	kbd_buffer.kbd_data_buffer_windex &= (kbd_buffer_size-1);
	kbd_push_assert = FALSE;
}

void tap_press_one_key(uint32 key_code)
{
	ilm_struct 	*Kp_ilm = NULL;

	tap_kbd_push_onekey(kbd_onekey_press, key_code);
	DRV_BuildPrimitive(Kp_ilm,
						MOD_DRVKBD,
						MOD_UEM,
						MSG_ID_DRVUEM_KEYPAD_IND,
						NULL);
	msg_send_ext_queue(Kp_ilm);

	tap_kbd_push_onekey(kbd_onekey_release, key_code);
	DRV_BuildPrimitive(Kp_ilm,
						MOD_DRVKBD,
						MOD_UEM,
						MSG_ID_DRVUEM_KEYPAD_IND,
						NULL);
	msg_send_ext_queue(Kp_ilm);
}

void qmaX981_sensor_tap_eint1_hisr(void)
{
	ilm_struct 	*ilm_ptr = NULL;
	BOOLEAN ret;
	unsigned char reg_data[4];
	int result, i;

	for(i=0; i<3; i++)
	{
		ret = qmax981_i2c_read_bytes(0x0a, reg_data, 2);
		if(ret)
		{
			break;
		}
	}

	if(reg_data[0] & 0x20)
	{	
#ifndef WIN32
		DRV_BuildPrimitive(ilm_ptr,
					MOD_DRVKBD,
					MOD_UEM,
				#if defined(__FISE_FZD_SERVICE__)
					MSG_ID_APP_MAKE_FRIEND_IND,
				#else
					MSG_ID_DRVUEM_KEYPAD_IND,		
				#endif
					NULL);
		msg_send_ext_queue(ilm_ptr); 

#endif
	}

	if(reg_data[0] & 0x08)
	{
#ifndef WIN32
		if(g_qmaX981.step_int1_level == 0)
		{
			for(i=0; i<3; i++)
			{
				ret = qmax981_i2c_read_bytes(QMAX981_STEP_CNT_L, reg_data, 2);
				if(ret)
				{
					break;
				}
			}
			result = (reg_data[1]<<8)|reg_data[0];		
			QMAX981_INFO( "step_int_start step=%d \n", result);
			g_qmaX981.step_int1_level = 1;
			qmaX981_step_debounce_int_work(result, 1);
			//ret = qmax981_i2c_write_byte(0x16, 0x24);
			ret = qmax981_i2c_write_byte(0x19, 0x24);
		}
#endif
	}
	else if(reg_data[0] & 0x04)
	{
#ifndef WIN32
		if(g_qmaX981.step_int1_level == 1)
		{
			for(i=0; i<3; i++)
			{
				ret = qmax981_i2c_read_bytes(QMAX981_STEP_CNT_L, reg_data, 2);
				if(ret)
				{
					break;
				}
			}
			result = (reg_data[1]<<8)|reg_data[0];		
			QMAX981_INFO("step_int_end step=%d \n", result);
			g_qmaX981.step_int1_level = 0;
			qmaX981_step_debounce_int_work(result, 0);
			//ret = qmax981_i2c_write_byte(0x16, 0x28);
			ret = qmax981_i2c_write_byte(0x19, 0x28);
		}
#endif
	}
}
#endif

void qmaX981_get_raw_data(int32 raw_data[3])
{
	BOOLEAN bResult=FALSE;	
	uint8 databuf[6] = {0};	
	int16 read_data[3];
	int16	i;

	bResult = qmax981_i2c_read_bytes(QMAX981_XOUTL, databuf, 6);
	if(FALSE == bResult)
	{
		#ifndef WIN32
		QMAX981_ERROR( "[E]----[%s]: get data fail!\r\n", __func__);
		#endif
		return;
	}

	if(g_qmaX981.chip_type == QMAX981_TYPE_6981)
	{
		read_data[0] = (int16)((databuf[1]<<2) |( databuf[0]>>6));
		read_data[1] = (int16)((databuf[3]<<2) |( databuf[2]>>6));
		read_data[2] = (int16)((databuf[5]<<2) |( databuf[4]>>6));
		for(i=0; i<3; i++)	// 三轴数据 	 
		{								   
			if (read_data[i] == 0x0200)		 // 防止溢出 10bit resolution, 512= 2^(10-1)
			{
				read_data[i]= -512;		 
			}
			else if ( read_data[i] & 0x0200 )  // 有符号位的，去符号位，其余各位取反加一 
			{							
				read_data[i] -= 0x1; 		
				read_data[i] = ~read_data[i]; 	
				read_data[i] &= 0x01ff;		
				read_data[i] = -read_data[i]; 
			}
		}
		read_data[0] -= QMAX981_OFFSET_X;
		read_data[1] -= QMAX981_OFFSET_Y;
		read_data[2] -= QMAX981_OFFSET_Z;
	}
	else if((g_qmaX981.chip_type == QMAX981_TYPE_7981)||(g_qmaX981.chip_type == QMAX981_TYPE_6100))
	{
		read_data[0] = (int16)(((int16)databuf[1]<<8) |(databuf[0]));
		read_data[1] = (int16)(((int16)databuf[3]<<8) |(databuf[2]));
		read_data[2] = (int16)(((int16)databuf[5]<<8) |(databuf[4]));

		read_data[0] = read_data[0]>>2;
		read_data[1] = read_data[1]>>2;
		read_data[2] = read_data[2]>>2;
	}

	raw_data[g_map.map[0]] = (int32)(g_map.sign[0] * read_data[0]);
	raw_data[g_map.map[1]] = (int32)(g_map.sign[1] * read_data[1]);
	raw_data[g_map.map[2]] = (int32)(g_map.sign[2] * read_data[2]);

#if defined(QMAX981_USE_CALI)
	if(qmax981_calied_flag == 1)
	{
		raw_data[0] += qmax981_cali[0];
		raw_data[1] += qmax981_cali[1];
		raw_data[2] += qmax981_cali[2];
	}
#endif

}

void qmaX981_custom_get_data(int16 *x_adc, int16 *y_adc, int16 *z_adc)
{
	int32 data_acc[3]= {0};

	qmaX981_get_raw_data(data_acc);
	*x_adc = (int16 )data_acc[0];
	*y_adc = (int16 )data_acc[1];
	*z_adc = (int16 )data_acc[2];
	
#ifndef WIN32
	QMAX981_INFO( "qmaX981_custom_get_data [%d %d %d]", data_acc[0], data_acc[1], data_acc[2]);
#endif
}


BOOLEAN qmaX981_get_acc_mg(int32 *x_adc, int32 *y_adc, int32 *z_adc)
{
	BOOLEAN bResult=FALSE; 
	int32 raw_data[3];

	qmaX981_get_raw_data(raw_data);

	*x_adc = (raw_data[0]*GRAVITY_1G)/g_qmaX981.lsb_1g;
	*y_adc = (raw_data[1]*GRAVITY_1G)/g_qmaX981.lsb_1g;
	*z_adc = (raw_data[2]*GRAVITY_1G)/g_qmaX981.lsb_1g;

	QMAX981_INFO( "qmaX981_get_acc_mg [%d %d %d]", *x_adc, *y_adc, *z_adc);
}

uint8 qmaX981_custom_get_chip_id(void)
{
    return g_qmaX981.chip_id;
}

#if defined(QMAX981_STEPCOUNTER)
BOOLEAN qmaX981_custom_get_step(uint32 *step)
{
	BOOLEAN bResult=FALSE;	
	uint8 databuf[3] = {0};
	int32	result;

	bResult = qmax981_i2c_read_bytes(QMAX981_STEP_CNT_L, databuf, 2);
	if(bResult == FALSE)
	{
		QMAX981_ERROR( "[E]----[%s]: Motion Sensor get step fail!\r\n", __func__);
		return FALSE;
	}
	
	if(g_qmaX981.chip_type == QMAX981_TYPE_6981)
	{
		result = (databuf[1]<<8)|databuf[0];
#if defined(QMAX981_CHECK_ABNORMAL_DATA)
		if(qmaX981_check_abnormal_data(result, &result) == 0)
		{
			QMAX981_ERROR( "qmaX981_check_abnormal_data error!!!\n");
			return FALSE;
		}
#endif
#if defined(QMAX981_STEP_COUNTER_USE_INT)
		result = qmaX981_step_debounce_read_data(result);
#endif
	}
	else if(g_qmaX981.chip_type == QMAX981_TYPE_7981)
	{
		bResult = qmax981_i2c_read_bytes(0x0e, &databuf[2], 1);
		if(bResult == FALSE)
		{
			QMAX981_ERROR( "[E]----[%s]: Motion Sensor get step fail!\r\n", __func__);
			return FALSE;
		}
		result = (((int)databuf[2]<<16)|((int)databuf[1]<<8)|databuf[0]);		
#if defined(QMAX981_CHECK_ABNORMAL_DATA)
		if(qmaX981_check_abnormal_data(result, &result) == 0)
		{
			QMAX981_ERROR( "qmaX981_check_abnormal_data error!!!\n");
			return FALSE;
		}
#endif
	}
	else if(g_qmaX981.chip_type == QMAX981_TYPE_6100)
	{
		bResult = qmax981_i2c_read_bytes(0x0d, &databuf[2], 1);
		if(bResult == FALSE)
		{
			QMAX981_ERROR( "[E]----[%s]: Motion Sensor get step fail!\r\n", __func__);
			return FALSE;
		}
		result = (((int)databuf[2]<<16)|((int)databuf[1]<<8)|databuf[0]);
#if defined(QMAX981_CHECK_ABNORMAL_DATA)
		if(qmaX981_check_abnormal_data(result, &result) == 0)
		{
			QMAX981_ERROR( "qma6100_check_abnormal_data error!!!\n");
			return FALSE;
		}
#endif
	}
      *step = result;

	return TRUE;
}

void qmaX981_custom_reset_step(void){	
    unsigned char databuf[2] = {0}; 
    unsigned char value_13; 
    BOOLEAN bResult=FALSE;     
#if defined(QMAX981_STEP_COUNTER_USE_INT) 
    qmaX981_step_debounce_reset();
#endif    
    bResult = qmax981_i2c_read_bytes(0x13, databuf, 1); 
    if(bResult == FALSE){          
        return; 
    }   
    value_13 = databuf[0];      
    bResult = qmax981_i2c_write_byte(0x13, 0x80);   
    if(bResult == FALSE){           
        return; 
    }   
    qmax981_delay_ms(20);   
    bResult = qmax981_i2c_write_byte(0x13, value_13);   
    if(bResult == FALSE){           
        return; 
    }
}
#endif

LOCAL void qmaX981_set_layout(int layout)
{
	if(layout == 0)
	{
		g_map.sign[QMAX981_AXIS_X] = 1;
		g_map.sign[QMAX981_AXIS_Y] = 1;
		g_map.sign[QMAX981_AXIS_Z] = 1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
	else if(layout == 1)
	{
		g_map.sign[QMAX981_AXIS_X] = -1;
		g_map.sign[QMAX981_AXIS_Y] = 1;
		g_map.sign[QMAX981_AXIS_Z] = 1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
	else if(layout == 2)
	{
		g_map.sign[QMAX981_AXIS_X] = -1;
		g_map.sign[QMAX981_AXIS_Y] = -1;
		g_map.sign[QMAX981_AXIS_Z] = 1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
	else if(layout == 3)
	{
		g_map.sign[QMAX981_AXIS_X] = 1;
		g_map.sign[QMAX981_AXIS_Y] = -1;
		g_map.sign[QMAX981_AXIS_Z] = 1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}	
	else if(layout == 4)
	{
		g_map.sign[QMAX981_AXIS_X] = -1;
		g_map.sign[QMAX981_AXIS_Y] = 1;
		g_map.sign[QMAX981_AXIS_Z] = -1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
	else if(layout == 5)
	{
		g_map.sign[QMAX981_AXIS_X] = 1;
		g_map.sign[QMAX981_AXIS_Y] = 1;
		g_map.sign[QMAX981_AXIS_Z] = -1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
	else if(layout == 6)
	{
		g_map.sign[QMAX981_AXIS_X] = 1;
		g_map.sign[QMAX981_AXIS_Y] = -1;
		g_map.sign[QMAX981_AXIS_Z] = -1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
	else if(layout == 7)
	{
		g_map.sign[QMAX981_AXIS_X] = -1;
		g_map.sign[QMAX981_AXIS_Y] = -1;
		g_map.sign[QMAX981_AXIS_Z] = -1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
	else		
	{
		g_map.sign[QMAX981_AXIS_X] = 1;
		g_map.sign[QMAX981_AXIS_Y] = 1;
		g_map.sign[QMAX981_AXIS_Z] = 1;
		g_map.map[QMAX981_AXIS_X] = QMAX981_AXIS_X;
		g_map.map[QMAX981_AXIS_Y] = QMAX981_AXIS_Y;
		g_map.map[QMAX981_AXIS_Z] = QMAX981_AXIS_Z;
	}
}

LOCAL BOOLEAN qmaX981_soft_reset(void)
{
    BOOLEAN  bResult=FALSE;
	
	bResult = qmax981_i2c_write_byte(0x36, 0xb6);
	qmax981_delay_ms(50); 
	bResult = qmax981_i2c_write_byte(0x36, 0x00); 
      qmax981_delay_ms(5);
	return bResult;
}

LOCAL BOOLEAN qma6100_soft_reset(void)
{
    BOOLEAN  bResult=FALSE;
    unsigned char reg_0x11 = 0;
    unsigned char reg_0x33 = 0;
    int retry = 0;
    
    bResult = qmax981_i2c_write_byte(0x36, 0xb6);
    qmax981_delay_ms(5); 
    bResult = qmax981_i2c_write_byte(0x36, 0x00);
    qmax981_delay_ms(5); 
    
    retry = 0;
    while(reg_0x11 != 0x84)
    {
        qmax981_i2c_write_byte(0x11, 0x84);
        qmax981_delay_ms(2);
        qmax981_i2c_read_bytes(0x11, &reg_0x11,1);
        if(retry++ > 100)
        	break;
    }

    // load otp
    qmax981_i2c_write_byte(0x33, 0x08);
    qmax981_delay_ms(5);

    retry = 0;
    while(reg_0x33 != 0x05)
    {
        qmax981_i2c_read_bytes(0x33, &reg_0x33,1);
        qmax981_delay_ms(2);
        if(retry++ > 100)
        	break;
    }

	return bResult;
}

uint8 qmaX981_get_chipid_ext(void)
{
    return g_qmaX981.chip_id;
}

LOCAL BOOLEAN qmaX981_get_chipid(void)
{
    uint8 chip_id = 0;
    BOOLEAN  bResult=FALSE;
    uint8 count = 0;

	while(count++ <= 3)
	{
	    qmax981_i2c_read_bytes(QMAX981_CHIP_ID, &chip_id, 1);
		#ifndef WIN32
		QMAX981_INFO( "[%s]: read chip id(%x) !", __func__, chip_id);
		#endif
		g_qmaX981.chip_id = chip_id;
	    if((chip_id==0xa9)||(chip_id>=0xb0 && chip_id<=0xb9))
	    {
	    	g_qmaX981.chip_type = QMAX981_TYPE_6981;
            bResult = TRUE;
			break;
	    }
	    else if((chip_id>=0xe0) && (chip_id<=0xe7))
	    {
	    	g_qmaX981.chip_type = QMAX981_TYPE_7981;
            bResult = TRUE;
			break;
	    }		
	    else if(chip_id==0xe8 || chip_id==0xfa || chip_id==0x90)
	    {
	    	g_qmaX981.chip_type = QMAX981_TYPE_6100;
            bResult = TRUE;
            break;
	    }
          qmax981_delay_ms(5);
	}
	return bResult;
}

LOCAL BOOLEAN qmaX981_set_mode(BOOLEAN mode)
{
    BOOLEAN  bResult=FALSE;
    uint8 data = 0x80;
    if(g_qmaX981.chip_type == QMAX981_TYPE_6100)
    {
        data = 0x84;
    }
#if defined(QMAX981_STEPCOUNTER)
	mode =  TRUE;
#endif
	if(mode == TRUE)	
	{
        	bResult = qmax981_i2c_write_byte(QMAX981_MODE, data); 
	}
	else
	{
        	bResult = qmax981_i2c_write_byte(QMAX981_MODE, 0x00); 
	}
      return bResult;
}


LOCAL BOOLEAN qmaX981_set_range(unsigned char range)
{
    BOOLEAN  bResult=FALSE;

	//range = QMAX981_RANGE_8G;
	bResult = qmax981_i2c_write_byte(QMAX981_RANGE, range);

	if(g_qmaX981.chip_type == QMAX981_TYPE_6981)
	{
		if(range == QMAX981_RANGE_2G)
			g_qmaX981.lsb_1g = 256;
		else if(range == QMAX981_RANGE_4G)
			g_qmaX981.lsb_1g = 128;
		else if(range == QMAX981_RANGE_8G)
			g_qmaX981.lsb_1g = 64;
		else
			g_qmaX981.lsb_1g = 256;
	}
	else if((g_qmaX981.chip_type == QMAX981_TYPE_7981)||(g_qmaX981.chip_type == QMAX981_TYPE_6100))
	{
		if(range == QMAX981_RANGE_2G)
			g_qmaX981.lsb_1g = 4096;
		else if(range == QMAX981_RANGE_4G)
			g_qmaX981.lsb_1g = 2048;
		else if(range == QMAX981_RANGE_8G)
			g_qmaX981.lsb_1g = 1024;
		else if(range == QMAX981_RANGE_16G)
			g_qmaX981.lsb_1g = 512;
		else if(range == QMAX981_RANGE_32G)
			g_qmaX981.lsb_1g = 256;
		else
			g_qmaX981.lsb_1g = 4096;
	}

	return bResult;
} 

//yangyu add begin, for fifo
LOCAL int16 x_buf[20] = {0};
LOCAL int16 y_buf[20] = {0};
LOCAL int16 z_buf[20] = {0};
LOCAL uint8 last_record_index = 0;

void QQMA6981_ReadXYZ(int16 *X, int16 *Y, int16 *Z);

extern void ReadGsensorFIFO(int16 *x_array, int16 *y_array, int16 *z_array, uint16 *read_len)
{
    uint8 fifo_count = 0;
    //int8  buffer[6*32] = {0};
    uint8 read_cnt = 0;
    int i = 0;
    uint32 index = 0;
    if(1)
    {
        *read_len = last_record_index + 1;
        for(i = 0 ; i < *read_len ; i++)
        {
            x_array[i] = x_buf[i];
            y_array[i] = y_buf[i];
            z_array[i] = z_buf[i];
            SCI_TRACE_LOW("QMA6981 fifo_read i=%d x,y,z= %d, %d, %d,",i, x_array[i],y_array[i],z_array[i]);
        }
        return;
    }
    #if 0
    QQMA6981_ReadXYZ(x_array, y_array, z_array);
    qmax981_i2c_read_bytes(0x0E, &fifo_count, 1);
    SCI_TRACE_LOW("QMA6981 fifo_count = %d", fifo_count);
    //if(fifo_count == 0)  fifo_count = 32;// yangyu add test
    if(fifo_count > 0)
    {
        if(fifo_count > 32)
        {
            fifo_count = 32;
        }
        *read_len = fifo_count;
        while(read_cnt+8 < fifo_count*6)
        {
            qmax981_i2c_read_bytes(0x3f, buffer+read_cnt, 8);
            read_cnt += 8;
        }
        if(read_cnt < fifo_count*6)
        {
            qmax981_i2c_read_bytes(0x3f, buffer+read_cnt, fifo_count*6-read_cnt);
        }
        for(i = 0; i < fifo_count*6; i=i+6)
        {
            x_array[index] = buffer[i+0]|(buffer[i+1]<<8);
            y_array[index] = buffer[i+2]|(buffer[i+3]<<8);
            z_array[index] = buffer[i+4]|(buffer[i+5]<<8);
            SCI_TRACE_LOW("QMA6981 fifo_read index=%d x,y,z= %d, %d, %d,",index, x_array[index],y_array[index],z_array[index]);
            index++;
        }
        //qmax981_i2c_write_byte(0x3E, 0x40); //FIFO_MODE   FIFO  ,0x3F for read x y z ;0x0E read count;
        
    }
    #endif
    
}



#define  UNIT_CHANGE    1000

void QQMA6981_ReadXYZ(int16 *X, int16 *Y, int16 *Z)  
{
	//Read 3 continous bytes of X, Y and Z 6-bit data
	uint8 databuf[6] = {0},i;
	//uint8 temp_data;
	BOOLEAN temp_result = FALSE;
	int16 data[3]= {0};
	int x, y, z ;

	qmax981_i2c_read_bytes(0x01, databuf, 6);//qma6981_read_bytes(0x01, databuf, 6);

	if(FALSE == temp_result)
	{
		//NRF_LOG_PRINTF("Motion Sensor get data fail!\n\r");
	}
	else
	{
		//NRF_LOG_PRINTF("Acc_x:%d, Acc_y:%d, Acc_z:%d \n\r", databuf[0], databuf[1], databuf[2]);
	}

 	data[0] = (int32)((databuf[1]<<2) |( databuf[0]>>6));
	data[1] = (int32)((databuf[3]<<2) |( databuf[2]>>6));
	data[2] = (int32)((databuf[5]<<2) |( databuf[4]>>6));

	//printk("RAW_DATA_AFTER %d,%d,%d\n",data[0],data[1],data[2]);


	for(i=0;i<3;i++)
	{
		if ( data[i] == 0x0200 )
			;//data[i]= -512;
		else if ( data[i] & 0x0200 ){
			data[i] -= 0x1; 
			data[i] = ~data[i];
			data[i] &= 0x01ff;
			data[i] = -data[i];
		}
	}
	
#ifdef STEP_COUNT
 if(cust_offset==1)
 {
	
	data[1] += 0x80;//0x80
	 data[2] += 0x80;
 }
else{
	data[0] += 0x80;//0x80
	data[2] += 0x80;
}
#endif
#if 0//def QMA6981_RANGE_8G //yangyu

	x = ((data[0]*UNIT_CHANGE)>>6);
	y = ((data[1]*UNIT_CHANGE)>>6);
	z = ((data[2]*UNIT_CHANGE)>>6);
	//GSENSOR_LOG("After Acc_x:%d, Acc_y:%d, Acc_z:%d \r\n", *X, *Y, *Z);
#else
	
	x = ((data[0]*UNIT_CHANGE)>>8);
	y = ((data[1]*UNIT_CHANGE)>>8);
	z = ((data[2]*UNIT_CHANGE)>>8);

#endif
   *X = x;
   *Y = y;
   *Z = z;



   SCI_TRACE_LOW("After Acc_x:%d, Acc_y:%d, Acc_z:%d \r\n", *X, *Y, *Z);
}

extern void QMA6981_RecordOneXYZ()
{
    int  i = 0;
    SCI_TRACE_LOW("QMA6981_RecordOneXYZ :%d, ", last_record_index);
    if(last_record_index < 19)
    {
        QQMA6981_ReadXYZ(&x_buf[last_record_index], &y_buf[last_record_index], &z_buf[last_record_index]);
        last_record_index++;
    }
    else if(last_record_index == 19)
    {
        if(x_buf[last_record_index] == 0 &&  y_buf[last_record_index] == 0 && z_buf[last_record_index] == 0)
        {
            QQMA6981_ReadXYZ(&x_buf[last_record_index], &y_buf[last_record_index], &z_buf[last_record_index]);
        }
        else
        {
            for(i = 0 ; i < 19; i++)
            {
                x_buf[i] = x_buf[i+1];
                y_buf[i] = y_buf[i+1];
                y_buf[i] = y_buf[i+1];
            }
            QQMA6981_ReadXYZ(&x_buf[last_record_index], &y_buf[last_record_index], &z_buf[last_record_index]);
        }
    }
}

LOCAL BOOLEAN qma6981_custom_init(void)
{
    BOOLEAN  bResult=FALSE;
    uint8 range=0;
    uint8 reg_0x16=0;
    uint8 reg_0x19=0;
    qmaX981_soft_reset();
    
	bResult = qmax981_i2c_write_byte(0x11, 0x80);
	bResult = qmaX981_set_range(QMAX981_RANGE_4G);
	bResult = qmax981_i2c_write_byte(QMAX981_ODR, 0x05);
	bResult = qmax981_i2c_write_byte(0x27, 0x00);
	bResult = qmax981_i2c_write_byte(0x28, 0x00);
	bResult = qmax981_i2c_write_byte(0x29, 0x00);

	bResult = qmax981_i2c_write_byte(0x16, reg_0x16);
	bResult = qmax981_i2c_write_byte(0x19, reg_0x19);
#if defined(QMAX981_STEPCOUNTER)
	#if 0
	bResult = qmax981_i2c_write_byte(QMAX981_ODR, 0x06);	
	bResult = qmax981_i2c_write_byte(0x11, 0x89);
	bResult = qmaX981_set_range(QMAX981_RANGE_8G);
	bResult = qmax981_i2c_write_byte(0x12, 0x8f);
	bResult = qmax981_i2c_write_byte(0x13, 0x10);
	bResult = qmax981_i2c_write_byte(0x14, 0x28);
	bResult = qmax981_i2c_write_byte(0x15, 0x20);
	#else	
	bResult = qmax981_i2c_write_byte(QMAX981_ODR, 0x2a);
	bResult = qmaX981_set_range(QMAX981_RANGE_8G);
	bResult = qmax981_i2c_write_byte(0x12, 0x8f);
	bResult = qmax981_i2c_write_byte(0x13, 0x0c);		// old 0x10
	bResult = qmax981_i2c_write_byte(0x14, 0x13);		// old 0x14
	bResult = qmax981_i2c_write_byte(0x15, 0x10);
	#endif

	if(g_qmaX981.layout%2)
		bResult = qmax981_i2c_write_byte(0x32, 0x02);
	else
		bResult = qmax981_i2c_write_byte(0x32, 0x01);
	
	bResult = qmax981_i2c_write_byte(0x27, QMAX981_OFFSET_X);
	bResult = qmax981_i2c_write_byte(0x28, QMAX981_OFFSET_Y);
	bResult = qmax981_i2c_write_byte(0x29, QMAX981_OFFSET_Z);

	reg_0x16 |= 0x0c;
	bResult = qmax981_i2c_write_byte(0x16, reg_0x16);
	#if defined(QMAX981_STEP_COUNTER_USE_INT)
	reg_0x19 |= 0x08;
	bResult = qmax981_i2c_write_byte(0x19, reg_0x19);
	#endif
#endif

#if defined(QMA6891_EXTRA_FUNC_1)
	bResult = qmax981_i2c_write_byte(0x0f, 0x04); // 0x01 :+-2g(1g=256)    0x04:+-8g(1g=64)
	bResult = qmax981_i2c_write_byte(0x10, 0x2b);
	bResult = qmax981_i2c_write_byte(0x12, 0x0f);
	bResult = qmax981_i2c_write_byte(0x27, 0x00);
	bResult = qmax981_i2c_write_byte(0x28, 0x00);
	bResult = qmax981_i2c_write_byte(0x29, 0x00);

	bResult = qmax981_i2c_write_byte(0x19, 0x00);
	bResult = qmax981_i2c_write_byte(0x16, 0x00);

	if(QMA6891_EXTRA_FUNC_1 & QMAX981_FOB_FLAG)
	{
		//Front/back z axis threshold, the actual g value is FB_Z_TH<7:0>*3.91mg+0.1g, independent of the selected g range
		bResult = qmax981_i2c_write_byte(0x30, 0xB4);
	}

	if(QMA6891_EXTRA_FUNC_1 & QMAX981_ORIENT_FLAG)
	{
		// Up/down x axis threshold, the actual g value is UD_X_TH<7:0>*3.91mg, independent of the selected g range,
		// the default value is 0.64g, corresponding to 40 degree
		bResult = qmax981_i2c_write_byte(0x2D, 0xBE);
		
		// Right/left y axis threshold, the actual g value is RL_Y_TH<7:0>*3.91mg, independent of the selected g range,
		// the default value is 0.64g, corresponding to 40 degree
		bResult = qmax981_i2c_write_byte(0x2F, 0xBE);
		
		//UD_Z_TH:
		//Up/down z axis threshold, the actual g value is UD_Z_TH<7:0>*3.91mg+0.1g, independent of the selected g range
		bResult = qmax981_i2c_write_byte(0x2C, 0x99);
		
		//L_Z_TH:
		//Right/left z axis threshold, the actual g value is RL_Z_TH<7:0>*3.91mg+0.1g, independent of the selected g range
		bResult = qmax981_i2c_write_byte(0x2E, 0x99);
	}

	reg_0x16 |= QMA6891_EXTRA_FUNC_1;
	reg_0x19 |= QMA6891_EXTRA_FUNC_1;
	bResult = qmax981_i2c_write_byte(0x16, QMA6891_EXTRA_FUNC_1);
	bResult = qmax981_i2c_write_byte(0x19, QMA6891_EXTRA_FUNC_1);
#elif defined(QMA6891_EXTRA_FUNC_2)
	bResult = qmax981_i2c_write_byte(0x0f, 0x04); // 0x01 :+-2g(1g=256)    0x04:+-8g(1g=64)
	bResult = qmax981_i2c_write_byte(0x10, 0x2b);
	bResult = qmax981_i2c_write_byte(0x12, 0x0f);
	bResult = qmax981_i2c_write_byte(0x27, 0x00);
	bResult = qmax981_i2c_write_byte(0x28, 0x00);
	bResult = qmax981_i2c_write_byte(0x29, 0x00);

	bResult = qmax981_i2c_write_byte(0x19, 0x00);
	bResult = qmax981_i2c_write_byte(0x16, 0x00);

	if((QMA6891_EXTRA_FUNC_2&QMAX981_HIGH_G_X_FLAG)
		||(QMA6891_EXTRA_FUNC_2&QMAX981_HIGH_G_Y_FLAG)
		||(QMA6891_EXTRA_FUNC_2&QMAX981_HIGH_G_Z_FLAG))
	{
		bResult = qmax981_i2c_write_byte(0x24, 0x82);
	// HIGH_DUR
		bResult = qmax981_i2c_write_byte(0x25, 0x0f);
		bResult = qmax981_i2c_write_byte(0x26, 0x40);
		bResult = qmax981_i2c_write_byte(0x17, QMA6891_EXTRA_FUNC_2);
		bResult = qmax981_i2c_write_byte(0x1a, 0x04);
	}
	 bResult = qmax981_i2c_write_byte(0x11, 0x80); 
#elif defined(QMA6891_EXTRA_FUNC_3)
	#if !defined(QMAX981_STEPCOUNTER)
	bResult = qmax981_i2c_write_byte(0x10, 0x06); // 6:ODR是500HZ采样  5:250HZ	建议是6，5不太灵敏
	#endif
	// TAP_QUIET<7>: tap quiet time, 1: 30ms, 0: 20ms 
	// TAP_SHOCK<6>: tap shock time, 1: 50ms, 0: 75ms
	// TAP_DUR<2:0>: the time window of the second tap event for double tap
	
	//TAP_DUR			Duration of TAP_DUR
	//000					50ms
	//001					100ms
	//010					150ms
	//011					200ms
	//100					250ms
	//101					375ms
	//110					500ms
	//111					700ms
	
	bResult = qmax981_i2c_write_byte(0x2A, 0x80); // 

	// TAP_TH<4:0>
	// 62.5*9=562.5 mg, TAP_TH is 62.5mg in 2g-range, 125mg in 4g-range, 250mg in 8g-range. 
	bResult = qmax981_i2c_write_byte(0x2B, 0x04); // 

	// register 0x16 bit5 S_TAP_EN, bit4 D_TAP_EN
	reg_0x16 |= 0x20;
	reg_0x19 |= 0x20;
	bResult = qmax981_i2c_write_byte(0x16, 0x20); // 
	bResult = qmax981_i2c_write_byte(0x19, 0x20); // 
#endif
	bResult = qmax981_i2c_write_byte(0x20, 0x00); // 低电平或者下降沿触发
	//bResult = qmax981_i2c_write_byte(0x20, 0x05);// 高电平或者上升沿触发
	//bResult = qmax981_i2c_write_byte(0x21, 0x01); // latch mode
	
	return bResult;
}

#if defined(QMA7981_HAND_UP_DOWN)
void qma7981_set_hand_up_down(int layout)
{
#if 1//defined(QMA7981_SWAP_XY)
	unsigned char reg_0x42 = 0;
#endif
	unsigned char reg_0x1e = 0;
	unsigned char reg_0x34 = 0;
	unsigned char yz_th_sel = 4;
	char y_th = -3; //-2;				// -16 ~ 15
	unsigned char x_th = 6;		// 0--7.5
	char z_th = 6;				// -8--7

#if 1//defined(QMA7981_SWAP_XY)	// swap xy
	if(layout%2)
	{
		qmax981_i2c_read_bytes(0x42, &reg_0x42, 1);
		reg_0x42 |= 0x80;		// 0x42 bit 7 swap x and y
		qmax981_i2c_write_byte(0x42, reg_0x42);
	}
#endif

	if((layout >=0) && (layout<=3))
	{
		z_th = 3;
		if((layout == 2)||(layout == 3))
			y_th = 3; 
		else if((layout == 0)||(layout == 1))	
			y_th = -3;
	}
	else if((layout >=4) && (layout<=7))
	{
		z_th = -3;
		
		if((layout == 6)||(layout == 7))
			y_th = 3; 
		else if((layout == 4)||(layout == 5))	
			y_th = -3;
	}

	// 0x34 YZ_TH_SEL[7:5]	Y_TH[4:0], default 0x9d  (YZ_TH_SEL   4   9.0 m/s2 | Y_TH  -3  -3 m/s2)
	//qmax981_i2c_write_byte(0x34, 0x9d);	//|yz|>8 m/s2, y>-3 m/m2
	if((y_th&0x80))
	{
		reg_0x34 |= yz_th_sel<<5;
		reg_0x34 |= (y_th&0x0f)|0x10;
		qmax981_i2c_write_byte(0x34, reg_0x34);
	}
	else
	{	
		reg_0x34 |= yz_th_sel<<5;
		reg_0x34 |= y_th;
		qmax981_i2c_write_byte(0x34, reg_0x34);	//|yz|>8m/s2, y<3 m/m2
	}
	//Z_TH<7:4>: -8~7, LSB 1 (unit : m/s2)	X_TH<3:0>: 0~7.5, LSB 0.5 (unit : m/s2) 
	//qmax981_i2c_write_byte(0x1e, 0x68);	//6 m/s2, 4 m/m2

	qmax981_i2c_write_byte(0x2a, (0x19|(0x03<<6)));			// 12m/s2 , 0.5m/s2
	qmax981_i2c_write_byte(0x2b, (0x7c|(0x03>>2)));
	//qmax981_i2c_write_byte(0x2a, (0x19|(0x02<<6)));			// 12m/s2 , 0.5m/s2
	//qmax981_i2c_write_byte(0x2b, (0x7c|(0x02)));

	//qmaX981_readreg(0x1e, &reg_0x1e, 1);
	if((z_th&0x80))
	{
		reg_0x1e |= (x_th&0x0f);
		reg_0x1e |= ((z_th<<4)|0x80);
		qmax981_i2c_write_byte(0x1e, reg_0x1e);
	}
	else
	{
		reg_0x1e |= (x_th&0x0f);
		reg_0x1e |= (z_th<<4);
		qmax981_i2c_write_byte(0x1e, reg_0x1e);
	}
}
#endif

#define STEP_W_TIME_L	300
#define STEP_W_TIME_H	250

LOCAL BOOLEAN qma7981_custom_init(void)
{
	unsigned char reg_0x10 = 0; 
	unsigned char reg_0x11 = 0;
#if defined(QMAX981_STEPCOUNTER)
	unsigned char reg_0x14 = 0;
	unsigned char reg_0x15 = 0;
#endif
	unsigned char reg_0x16 = 0;
	unsigned char reg_0x18 = 0;
	unsigned char reg_0x19 = 0;
	unsigned char reg_0x1a = 0;
#if defined(QMA7981_ANY_MOTION)||defined(QMA7981_NO_MOTION)
	unsigned char reg_0x2c = 0;
#endif
      qmaX981_soft_reset();
	qmaX981_set_range(QMAX981_RANGE_4G);	// 0.488 mg
	//0xe0	[65 hz		15.48 ms]
	//0xe1	[129 hz 	7.74 ms]
	//0xe2	[258 hz 	3.87 ms]
	reg_0x10 = 0xe1;
	qmax981_i2c_write_byte(0x10, reg_0x10);
	reg_0x11 = 0x80;
	qmax981_i2c_write_byte(0x11, reg_0x11);
//	qmax981_i2c_write_byte(0x4a, 0x08);	//Force I2C I2C interface
	qmax981_i2c_write_byte(0x5f, 0x80);
	qmax981_i2c_write_byte(0x5f, 0x00);
	qmax981_delay_ms(20);
// read reg
	qmax981_i2c_read_bytes(0x16, &reg_0x16, 1);
	qmax981_i2c_read_bytes(0x18, &reg_0x18, 1);
	qmax981_i2c_read_bytes(0x19, &reg_0x19, 1);
	qmax981_i2c_read_bytes(0x1a, &reg_0x1a, 1);
	
	QMAX981_INFO("read reg[%d %d %d %d] \n", reg_0x16, reg_0x18, reg_0x19, reg_0x1a);
// read reg
	
#if defined(QMAX981_STEPCOUNTER)
	reg_0x10 = 0xe1;
	reg_0x11 = 0x80;

	if(reg_0x11 == 0x80)		// 500K
	{
		reg_0x14 = (((STEP_W_TIME_L*100)/771)+1);		// 0xe1 odr 129.7hz, 7.71ms
		reg_0x15 = (((STEP_W_TIME_H*100)/771)+1);
		if(reg_0x10 == 0xe0)		// odr 65hz
		{
			reg_0x14 = (reg_0x14>>1);
			reg_0x15 = (reg_0x15>>1);
		}
		else if(reg_0x10 == 0xe1)	// 129.7hz
		{
		}
		else if(reg_0x10 == 0xe5)	// odr 32.5hz
		{
			reg_0x14 = (reg_0x14>>2);
			reg_0x15 = (reg_0x15>>2);
		}
	}
	else if(reg_0x11 == 0x81)	// 333K
	{
		reg_0x14 = (((STEP_W_TIME_L*100)/581)+1);	// 0xe2 odr 172.0930233 hz, 5.81ms
		reg_0x15 = (((STEP_W_TIME_H*100)/581)+1);
		if(reg_0x10 == 0xe2)	// 172.0930233 hz
		{
		}
		else if(reg_0x10 == 0xe1)	// 86.38132296 hz
		{			
			reg_0x14 = (reg_0x14>>1);
			reg_0x15 = (reg_0x15>>1);
		}
		else if(reg_0x10 == 0xe0)		// 43.2748538
		{
			reg_0x14 = (reg_0x14>>2);
			reg_0x15 = (reg_0x15>>2);
		}
	}
	else if(reg_0x11 == 0x82)		// 200K
	{
		reg_0x14 = (((STEP_W_TIME_L*100)/967)+1);	//0xe2 103.3591731 hz, 9.675 ms
		reg_0x15 = (((STEP_W_TIME_H*100)/967)+1);
		if(reg_0x10 == 0xe2)	// 103.3591731 hz
		{
		}
		else if(reg_0x10 == 0xe1)
		{			
			reg_0x14 = (reg_0x14>>1);		// 51.88067445 hz
			reg_0x15 = (reg_0x15>>1);
		}
		else if(reg_0x10 == 0xe3)
		{				
			reg_0x14 = (reg_0x14<<1);		// 205.1282051 hz				
			reg_0x15 = (reg_0x15<<1);
		}
	}		
	else if(reg_0x11 == 0x83)		// 100K
	{
		reg_0x14 = (((STEP_W_TIME_L*100)/975)+1);	// 0xe3 102.5641026 hz, 9.75 ms
		reg_0x15 = (((STEP_W_TIME_H*100)/975)+1);
		if(reg_0x10 == 0xe3)
		{
		}
		else if(reg_0x10 == 0xe2)
		{
			reg_0x14 = (reg_0x14>>1);		// 51.67958656 hz
			reg_0x15 = (reg_0x15>>1);
		}
	}

	QMAX981_INFO("0x14[%d] 0x15[%d] \n", reg_0x14, reg_0x15);
	qmax981_i2c_write_byte(0x12, 0x94);
	qmax981_i2c_write_byte(0x13, 0x80);		// clear step
	qmax981_i2c_write_byte(0x13, 0x00);		// 0x7f(1/16) 0x00(1/8)
	qmax981_i2c_write_byte(0x14, reg_0x14);		// STEP_TIME_LOW<7:0>*(1/ODR) 
	qmax981_i2c_write_byte(0x15, reg_0x15);		// STEP_TIME_UP<7:0>*8*(1/ODR) 

	//qmax981_i2c_write_byte(0x1f, 0x09); 	// 0 step
	//qmax981_i2c_write_byte(0x1f, 0x29); 	// 4 step
	qmax981_i2c_write_byte(0x1f, 0x49); 	// 8 step
	//qmax981_i2c_write_byte(0x1f, 0x69); 	// 12 step
	//qmax981_i2c_write_byte(0x1f, 0x89); 	// 16 step
	//qmax981_i2c_write_byte(0x1f, 0xa9);		// 24 step
	//qmax981_i2c_write_byte(0x1f, 0xc9); 	// 32 step
	//qmax981_i2c_write_byte(0x1f, 0xe9); 	// 40 step

	// step int
#if defined(QMA7981_STEP_INT)
	reg_0x16 |= 0x08;
	reg_0x19 |= 0x08;
	qmax981_i2c_write_byte(0x16, reg_0x16);
	qmax981_i2c_write_byte(0x19, reg_0x19);
#endif
#if defined(QMA7981_SIGNIFICANT_STEP)
	qmax981_i2c_write_byte(0x1d, 0x26);		//every 30 step
	reg_0x16 |= 0x40;
	reg_0x19 |= 0x40;
	qmax981_i2c_write_byte(0x16, reg_0x16);
	qmax981_i2c_write_byte(0x19, reg_0x19);
#endif

#endif

//RANGE<3:0> Acceleration range Resolution
//0001 2g 244ug/LSB
//0010 4g 488ug/LSB
//0100 8g 977ug/LSB
//1000 16g 1.95mg/LSB
//1111 32g 3.91mg/LSB
//Others 2g 244ug/LSB

//0x2c
//Duration = (NO_MOT_DUR<3:0> + 1) * 1s, if NO_MOT_DUR<5:4> =b00 
//Duration = (NO_MOT_DUR<3:0> + 4) * 5s, if NO_MOT_DUR<5:4> =b01 
//Duration = (NO_MOT_DUR<3:0> + 10) * 10s, if NO_MOT_DUR<5:4> =b1x 
//ANY_MOT_DUR<1:0>: any motion interrupt will be triggered when slope > ANY_MOT_TH for (ANY_MOT_DUR<1:0> + 1) samples 

//0x2e ANY MOTION MOT_CONF2
//TH= ANY_MOT_TH<7:0> * 16 * LSB 

#if defined(QMA7981_ANY_MOTION)
	reg_0x18 |= 0x07;
	reg_0x1a |= 0x01;
	reg_0x2c |= 0x00;	//0x01; 	// 0x00
	
	qmax981_i2c_write_byte(0x18, reg_0x18);
	qmax981_i2c_write_byte(0x1a, reg_0x1a);
	qmax981_i2c_write_byte(0x2c, reg_0x2c);
	//qmax981_i2c_write_byte(0x2e, 0x18);		// 0.488*16*20 = 156mg
	//qmax981_i2c_write_byte(0x2e, 0xc0);		// 0.488*16*192 = 1.5g
	//qmax981_i2c_write_byte(0x2e, 0x80);		// 0.488*16*128 = 1g
	//qmax981_i2c_write_byte(0x2e, 0x60);		// 0.488*16*96 = 750mg
	qmax981_i2c_write_byte(0x2e, 0xC0);		// 0.488*16*64 = 500mg  40
#if defined(QMA7981_SIGNIFICANT_MOTION)
	//qmax981_i2c_write_byte(0x2f, 0x0c|0x01);
	qmax981_i2c_write_byte(0x2f, 0x01);
	reg_0x19 |= 0x01;
	qmax981_i2c_write_byte(0x19, reg_0x19);
#endif
#endif

#if defined(QMA7981_NO_MOTION)
	reg_0x18 |= 0xe0;
	reg_0x1a |= 0x80;
	reg_0x2c |= 0x00;	//1s			//0x24;

	qmax981_i2c_write_byte(0x18, reg_0x18);
	qmax981_i2c_write_byte(0x1a, reg_0x1a);
	qmax981_i2c_write_byte(0x2c, reg_0x2c);
	qmax981_i2c_write_byte(0x2d, 0x14);
#endif
 
#if defined(QMA7981_HAND_UP_DOWN)
	qma7981_set_hand_up_down(0);
	reg_0x16 |= 0x02;	// hand down
	reg_0x19 |= 0x02;
	qmax981_i2c_write_byte(0x16, reg_0x16);
	qmax981_i2c_write_byte(0x19, reg_0x19);
	reg_0x16 |= 0x04;	// hand down
	reg_0x19 |= 0x04;
	qmax981_i2c_write_byte(0x16, reg_0x16);
	qmax981_i2c_write_byte(0x19, reg_0x19);
#endif

#if defined(QMA7981_DATA_READY)
	reg_0x1a |= 0x10;
	qmax981_i2c_write_byte(0x17, 0x10);
	qmax981_i2c_write_byte(0x1a, reg_0x1a);
#endif

#if defined(QMA7981_INT_LATCH)
	qmax981_i2c_write_byte(0x21, 0x1f);	// default 0x1c, step latch mode
#endif
// int level set high, defalut low 
	qmax981_i2c_write_byte(0x20, 0x00);		// default 0x05
// int default level set

	return TRUE;
}
#if defined(QMA7981_ANY_MOTION)
void qma6100_anymotion_config(int int_map, int enable)
{
	uint8 reg_0x18 = 0;
	uint8 reg_0x1a = 0;
	uint8 reg_0x1c = 0;
	uint8 reg_0x2c = 0;
#if defined(QMA6100_SIGNIFICANT_MOTION)
	uint8 reg_0x19 = 0;
	uint8 reg_0x1b = 0;
#endif

	qmax981_i2c_read_bytes(0x18, &reg_0x18, 1);
	qmax981_i2c_read_bytes(0x1a, &reg_0x1a, 1);
	qmax981_i2c_read_bytes(0x1c, &reg_0x1c, 1);
	qmax981_i2c_read_bytes(0x2c, &reg_0x2c, 1);
	reg_0x2c |= 0x00;

	qmax981_i2c_write_byte(0x2c, reg_0x2c);
	qmax981_i2c_write_byte(0x2e, 0xC0);		// 0.488*16*32 = 250mg 10
	// add by yang, tep counter, raise wake, and tap detector,any motion by pass LPF
	qmax981_i2c_write_byte(0x30, 0x80|0x40|0x3f);	// default 0x3f
	// add by yang, tep counter, raise wake, and tap detector,any motion by pass LPF
	if(enable)
	{
		reg_0x18 |= 0x07;
		reg_0x1a |= 0x01;
		reg_0x1c |= 0x01;

		qmax981_i2c_write_byte(0x18, reg_0x18);	// enable any motion
		if(int_map == QMA6100_MAP_INT1)
			qmax981_i2c_write_byte(0x1a, reg_0x1a);
		else if(int_map == QMA6100_MAP_INT2)
			qmax981_i2c_write_byte(0x1c, reg_0x1c);
	}
	else
	{
		reg_0x18 &= (~0x07);
		reg_0x1a &= (~0x01);
		reg_0x1c &= (~0x01);
		
		qmax981_i2c_write_byte(0x18, reg_0x18);
		qmax981_i2c_write_byte(0x1a, reg_0x1a);
		qmax981_i2c_write_byte(0x1c, reg_0x1c);
	}
	
#if defined(QMA6100_SIGNIFICANT_MOTION)
	qmax981_i2c_read_bytes(0x19, &reg_0x19, 1);
	qmax981_i2c_read_bytes(0x1b, &reg_0x1b, 1);
	
	qmax981_i2c_write_byte(0x2f, 0x01);		// bit0: selecat significant motion
	if(enable)
	{
		reg_0x19 |= 0x01;
		reg_0x1b |= 0x01;
		if(int_map == QMA6100_MAP_INT1)
			qmax981_i2c_write_byte(0x19, reg_0x19);
		else if(int_map == QMA6100_MAP_INT2)
			qmax981_i2c_write_byte(0x1b, reg_0x1b);
	}
	else
	{
		reg_0x19 &= (~0x01);
		reg_0x1b &= (~0x01);
		qmax981_i2c_write_byte(0x19, reg_0x19);
		qmax981_i2c_write_byte(0x1b, reg_0x1b);
	}
#endif	
// int level set high, defalut low 
	qmax981_i2c_write_byte(0x20, 0x00);		// default 0x05
// int default level set
}
#endif

void qma6100_stepcounter_config(int enable)
{	
 
//  int odr = 27;   // 100Hz
  unsigned char reg_12 = 0x00;
  unsigned char reg_14 = 0x00;
  unsigned char reg_15 = 0x00;
  unsigned char reg_1e = 0x00;

  qmax981_i2c_write_byte(0x13, 0x80);
  qmax981_delay_ms(1);
  qmax981_i2c_write_byte(0x13, 0x00);     //  0x7f(P2P/16), 0.977*16*LSB // 0x7f --0x00, // 0x00 step will less. // wuxx add 20231209. modify by yang yu.
  // Windows time
  if(enable)
  {
#if defined(QMA6100_ODR_27)
    reg_12 = 0x84;
    reg_14 = 0x05;      //((200*odr)/(1000));      // about:200 ms
    reg_15 = 0x07;      //(((2200/8)*odr)/1000);   // 2000 ms
#elif defined(QMA6100_ODR_55)
    reg_12 = 0x89;  //0x89
    reg_14 = 0x0c;//0x0b;    //0x0c  //((200*odr)/(1000));  //0x0b-0x0c // wuxx add 20231209. modify by yang yu.
    reg_15 = 0x0e;//0x0c;     //0x0e //(((2200/8)*odr)/1000);   // 2000 ms //0x0c-0x0e // wuxx add 20231209. modify by yang yu.
#elif defined(QMA6100_ODR_100)
    reg_12 = 0x8f;
    reg_14 = 0x1c;//0x1f;      //((280*odr)/(1000));      // about:280 ms
    reg_15 = 0x19;      //(((2200/8)*odr)/1000);   // 2000 ms
#endif
    //QMA6100_IRQ_LOG("step time config 0x14=0x%x 0x15=0x%x\n", reg_14, reg_15);
    qmax981_i2c_write_byte(0x12, reg_12);
    qmax981_i2c_write_byte(0x14, reg_14);
    qmax981_i2c_write_byte(0x15, reg_15);
    // lpf
    qmax981_i2c_read_bytes(0x1e, &reg_1e,1);
    reg_1e &= 0x3f;
    qmax981_i2c_write_byte(0x1e, (unsigned char)(reg_1e|QMA6100_STEP_LPF_4));   // default 0x08 // QMA6100_STEP_LPF_0 --QMA6100_STEP_LPF_4, // QMA6100_STEP_LPF_8 step is more less.//  wuxx add 20231209. modify by yang yu. QMA6100_STEP_LPF_4 is W306
    // start count, p2p, fix peak
    qmax981_i2c_write_byte(0x1f, (unsigned char)QMA6100_STEP_START_24|0x08);    // 0x10
  }

}

LOCAL BOOLEAN qma6100_custom_init(void)
{
	unsigned char reg_0x10 = 0; 
      qma6100_soft_reset();
      
	qmax981_i2c_write_byte(0x4a, 0x20);
	qmax981_i2c_write_byte(0x50, 0x51);   //0x49  0x51
	qmax981_i2c_write_byte(0x56, 0x01);
	qmaX981_set_range(QMAX981_RANGE_4G);	// 0.488 mg
	
#if defined(QMA6100_ODR_27)
	qmax981_i2c_write_byte(0x10, 0x06);
#elif defined(QMA6100_ODR_55)
	qmax981_i2c_write_byte(0x10, 0x05);
#elif defined(QMA6100_ODR_100)
	qmax981_i2c_write_byte(0x10, 0x00);
#endif
	qmax981_i2c_write_byte(0x11, 0x84);

	qmax981_i2c_write_byte(0x4A, 0x28);
	qmax981_delay_ms(5);
	qmax981_i2c_write_byte(0x20, 0x05);
	qmax981_delay_ms(5);
	qmax981_i2c_write_byte(0x5F, 0x80);
	qmax981_delay_ms(5);
	qmax981_i2c_write_byte(0x5F, 0x00);
	qmax981_delay_ms(20);

#if defined(QMAX981_STEPCOUNTER)
	qma6100_stepcounter_config(1);
#endif
 #if defined(QMA7981_ANY_MOTION)
     qma6100_anymotion_config(QMA6100_MAP_INT1, QMA6100_ENABLE);
 #endif
	return TRUE;
}
PUBLIC qmaX981_chip_type Get_Cur_Gsensor_Chip(void)
{
	return g_qmaX981.chip_type;
}

LOCAL BOOLEAN qmaX981_custom_init(void)
{
      BOOLEAN  bResult=FALSE;
	memset(&g_qmaX981, 0, sizeof(g_qmaX981));
	g_qmaX981.layout = 0;
	qmaX981_set_layout(g_qmaX981.layout);
	g_qmaX981.chip_type = QMAX981_TYPE_UNKNOW;
	bResult = qmaX981_get_chipid();
      if(bResult)
      {
          if(g_qmaX981.chip_type == QMAX981_TYPE_6981)
          {
          	qma6981_custom_init();
          }
          else if(g_qmaX981.chip_type == QMAX981_TYPE_7981)
          {
          	qma7981_custom_init();
          }
          else if(g_qmaX981.chip_type == QMAX981_TYPE_6100)
          {
          	qma6100_custom_init();
          }
          else
          {
          	bResult = FALSE;
          }

#if defined(QMAX981_USE_INT1)
        	qmaX981_setup_int1();
#endif	

            if(bResult)
            {
            	#ifndef WIN32
                QMAX981_INFO( "[I]----[%s]: success!", __func__);
        		#endif
            }
            else
            {
            	#ifndef WIN32
                QMAX981_ERROR( "[E]----[%s]: failure!", __func__);
        		#endif
            }
#if defined(QMAX981_USE_CALI)
        	qmax981_read_file(QMAX981_CALI_FILE, (char *)qmax981_cali, sizeof(qmax981_cali));
#endif
      }

    return bResult;
}

BOOLEAN qmaX981_query_gesture(uint16 ms_gest_type)
{
    BOOLEAN  bResult=FALSE;
#if defined(QMAX981_STEPCOUNTER)
     bResult = qmaX981_custom_get_step(&g_qmaX981.step);
#endif
    return bResult;
}

void qmaX981_custom_pwr_up(void)
{
	qmaX981_set_mode(1);  
}


void qmaX981_custom_pwr_down(void)
{
	qmaX981_set_mode(0);
}

#if defined(QMAX981_USE_CALI)
LOCAL void qmax981_write_file(uint16 * filename, char *data, int len)
{
	FS_HANDLE fd = -1;

	if((fd = FS_Open(filename, FS_CREATE | FS_READ_WRITE)) < FS_NO_ERROR)	  /* mre is not folder */
	{		
		QMAX981_INFO("%s open file error!\r\n", __func__);
		FS_Close(fd);
		return;
	}
	else
	{
		FS_Write(fd, data, len, NULL);
		FS_Close(fd);
		return;
	}
	
}

LOCAL void qmax981_read_file(uint16 * filename, char *data, int len)
{
	FS_HANDLE fd = -1;
	uint32 file_size = 0;
	
	if((fd = FS_Open(filename, FS_READ_ONLY)) < FS_NO_ERROR)	  /* FS_CREATE */
	{
		FS_Close(fd);
		QMAX981_INFO("%s open file error!\r\n", __func__);
		qmax981_calied_flag = 0;
		qmax981_cali[0] = 0;
		qmax981_cali[1] = 0;
		qmax981_cali[2] = 0;
		return;
	}
	else
	{
		FS_Read(fd, data, len, NULL);
		FS_Close(fd);
		QMAX981_INFO("%s open file OK!\r\n", __func__);
		qmax981_calied_flag = 1;
		return;
	}
}

void qmax981_do_cali(void)
{
	int32 data[3], data_avg[3];
	int icount, z_max, z_min;

	data_avg[0] = 0;
	data_avg[1] = 0;
	data_avg[2] = 0;

	qmax981_calied_flag = 0;	
	qmax981_cali[0] = 0;
	qmax981_cali[1] = 0;
	qmax981_cali[2] = 0;
	for(icount=0; icount<QMAX981_CALI_NUM; icount++)
	{
		qmaX981_get_raw_data(data);
		data_avg[0] += data[0];
		data_avg[1] += data[1];
		data_avg[2] += data[2];
		// add by yangzhiqiang check vibrate
		if(icount == 0)
		{
			z_max = data[2];
			z_min = data[2];
		}
		else
		{
			z_max = (data[2]>z_max)?data[2]:z_max;
			z_min = (data[2]<z_min)?data[2]:z_min;
		}
		// add by yangzhiqiang check vibrate
		qmax981_delay_ms(5);
	}
	// add by yangzhiqiang check vibrate
	if((z_max-z_min)>(g_qmaX981.lsb_1g*3/10))
	{
		QMAX981_INFO("qmax981_cali_store check vibrate cali ingore!\n");
		return;
	}
	// add by yangzhiqiang check vibrate

	data_avg[0] = data_avg[0]/QMAX981_CALI_NUM;
	data_avg[1] = data_avg[1]/QMAX981_CALI_NUM;
	data_avg[2] = data_avg[2]/QMAX981_CALI_NUM;

	data[0] = 0-data_avg[0];
	data[1] = 0-data_avg[1];
	data[2] = g_qmaX981.lsb_1g-data_avg[2];
	qmax981_cali[0] = data[0];
	qmax981_cali[1] = data[1];
	qmax981_cali[2] = data[2];
	QMAX981_INFO("qmax981_cali_store qmax981_cali[%d %d %d]\n", qmax981_cali[0], qmax981_cali[1], qmax981_cali[2]);
	qmax981_write_file(QMAX981_CALI_FILE, (char *)qmax981_cali, sizeof(qmax981_cali));	
	qmax981_calied_flag = 1;
}

void MDrv_Gsensor_Custom_Cali(void)
{
	qmax981_do_cali();
}
#endif
LOCAL BOOLEAN              s_QMAX981_is_init = FALSE;
LOCAL SCI_TIMER_PTR      s_QMAX981_timer;

BOOLEAN  QMAX981_init(void)
{
    BOOLEAN res = FALSE;
    if(s_QMAX981_is_init == FALSE)
    {
        QMAX981_INFO("QMAX981_init");
        QMAX981_I2C_Open();
        if(qmaX981_custom_init())
        {
            res = TRUE;
            s_QMAX981_is_init = TRUE;
        }
        else
        {
            QMAX981_I2C_Close();
        }
    }
    return res;
}

//#define TIMER_PERIOD_QMAX981         1000
uint16  TIMER_PERIOD_QMAX981    =     1000;

LOCAL void QMAX981_TimerCallback(uint32 param)
{	    
    ZDT_GSensor_TimerOut();
    SCI_ChangeTimer(s_QMAX981_timer, QMAX981_TimerCallback, TIMER_PERIOD_QMAX981);
    SCI_ActiveTimer(s_QMAX981_timer);
    return ;
}

extern void QMAX981_SetTimerPeroid(uint16 ms)
{
    TIMER_PERIOD_QMAX981 = ms;
    SCI_TRACE_LOW("QMAX981_SetTimerPeroid ms=%d , %d", ms, s_QMAX981_timer == NULL);
    //SCI_ChangeTimer(s_QMAX981_timer, QMAX981_TimerCallback, TIMER_PERIOD_QMAX981);
    //SCI_ActiveTimer(s_QMAX981_timer);
    #if 1
    if(s_QMAX981_timer == NULL)
    {
        s_QMAX981_timer = SCI_CreateTimer("QMAX981_Timer", QMAX981_TimerCallback,
                                    1, TIMER_PERIOD_QMAX981, SCI_AUTO_ACTIVATE);
    }
    else
    {
        SCI_ChangeTimer(s_QMAX981_timer, QMAX981_TimerCallback, TIMER_PERIOD_QMAX981);
        SCI_ActiveTimer(s_QMAX981_timer);	
    }
    #endif
}


BOOLEAN  QMAX981_open(void)
{
    if(s_QMAX981_is_init == FALSE)
    {
        return FALSE;
    }
    qmaX981_custom_pwr_up();
    if(s_QMAX981_timer == NULL)
    {
        s_QMAX981_timer = SCI_CreateTimer("QMAX981_Timer", QMAX981_TimerCallback,
                                    1, TIMER_PERIOD_QMAX981, SCI_AUTO_ACTIVATE);
    }
    else
    {
        SCI_ChangeTimer(s_QMAX981_timer, QMAX981_TimerCallback, TIMER_PERIOD_QMAX981);
        SCI_ActiveTimer(s_QMAX981_timer);	
    }
    
    return TRUE;
}

BOOLEAN  QMAX981_close(void)
{
    if(s_QMAX981_is_init == FALSE)
    {
        return FALSE;
    }
    if(s_QMAX981_timer != NULL)
    {
        SCI_DeactiveTimer(s_QMAX981_timer);
    }
    qmaX981_custom_pwr_down();
    return TRUE;
}

BOOLEAN  QMAX981_step_reset(void)
{
    qmaX981_custom_reset_step();
    return TRUE;
}

#ifdef __cplusplus
}
#endif  // End of file



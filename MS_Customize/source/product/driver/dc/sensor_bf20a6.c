/******************************************************************************
 ** File Name:    Sensor_BF20A6.c                                         *
 ** Author:         Haydn_He                                                 *
 ** Date:           2012-11-16	                                         *
 ** Platform:       SP6530
 ** Copyright:    Spreadtrum All Rights Reserved.        *
 ** Description:   implementation of digital camera register interface       *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log$
 ** DATE             NAME             DESCRIPTION                                 *
 ** 2012-11-16   Haydn            Fristly Released
 *******************************************************************************/
#ifndef _BF20A6_C_

#define _BF20A6_C_
#include "ms_customize_trc.h"
#include "sensor_cfg.h"
#include "sensor_drv.h"
//#include "i2c_drv.h"
#include "os_api.h"
#include "chip.h"
#include "dal_dcamera.h"


/**---------------------------------------------------------------------------*
 ** 						Compiler Flag									  *
 **---------------------------------------------------------------------------*/
#ifdef	 __cplusplus
	extern	 "C"
	{
#endif

/**---------------------------------------------------------------------------*
 **                         Const variables                                   *
 **---------------------------------------------------------------------------*/
#define BF20A6_I2C_ADDR_W    			0xdc
#define BF20A6_I2C_ADDR_R    			0xdd
#define BF20A6_I2C_ACK					0x0

#ifdef FILE_LOG_SUPPORT
#define BF20A6_LOG Trace_Log_Print
#else
#define BF20A6_LOG SCI_TRACE_LOW
#endif


LOCAL BOOLEAN  	bl_GC_50Hz_BF20A6		= FALSE;

/**---------------------------------------------------------------------------*
     BF20A6 SUPPORT 5 SERIAL MODE:
     CCIR601_8BIT, SPI, CCIR656_2BIT, CCIR656_4BIT, ONE2ONE(BF20A6&GC6103)
 **---------------------------------------------------------------------------*/


// use next two interface
#define BF20A6_OUTPUT_MODE_CCIR656_2BIT
//#define BF20A6_OUTPUT_MODE_PACKET_DDR_2BIT 

//#define BF20A6_SERIAL_LOAD_FROM_T_FLASH 

/**---------------------------------------------------------------------------*
 **                     Local Function Prototypes                             *
 **---------------------------------------------------------------------------*/
LOCAL uint32 BF20A6_Power_On(uint32 power_on);
LOCAL uint32 BF20A6_Identify(uint32 param);
LOCAL void   BF20A6_WriteReg( uint8  subaddr, uint8 data );
LOCAL uint8  BF20A6_ReadReg( uint8  subaddr);
LOCAL void   BF20A6_Write_Group_Regs( SENSOR_REG_T* sensor_reg_ptr );
LOCAL void   BF20A6_Delayms (uint32 ms);
//LOCAL uint32 Set_BF20A6_Mirror(uint32 level);
//LOCAL uint32 Set_BF20A6_Flip(uint32 level);
LOCAL uint32 Set_BF20A6_Brightness(uint32 level);
LOCAL uint32 Set_BF20A6_Contrast(uint32 level);
LOCAL uint32 Set_BF20A6_Image_Effect(uint32 effect_type);
LOCAL uint32 Set_BF20A6_Ev(uint32 level);
LOCAL uint32 Set_BF20A6_Anti_Flicker(uint32 mode);
LOCAL uint32 Set_BF20A6_Preview_Mode(uint32 preview_mode);
LOCAL uint32 Set_BF20A6_AWB(uint32 mode);
LOCAL uint32 BF20A6_AE_AWB_Enable(uint32 ae_enable, uint32 awb_enable);
LOCAL uint32 BF20A6_Before_Snapshot(uint32 para);
LOCAL uint32 BF20A6_After_Snapshot(uint32 para);
LOCAL uint32 Set_BF20A6_Video_Mode(uint32 mode);
LOCAL uint32 BF20A6_GetPclkTab(uint32 param);
/**---------------------------------------------------------------------------*
 ** 						Local Variables 								 *
 **---------------------------------------------------------------------------*/
#ifdef BF20A6_SERIAL_LOAD_FROM_T_FLASH  
#include "sfs.h"
#include "ffs.h"
LOCAL uint32 Load_BF20A6_RegTab_From_T_Flash(void);
#endif



__align(4) const SENSOR_REG_T BF20A6_YUV_640X480[] =
{
	{0xf2,0x01},	
	{0x12,0xc0},
#ifdef BF20A6_OUTPUT_MODE_CCIR656_1BIT
	{0x3a,0x01},
	{0xe1,0xd3},
	{0xe3,0x02},	
#elif defined(BF20A6_OUTPUT_MODE_CCIR656_2BIT)
	{0x3a,0x02},
	{0xe1,0x92},
	{0xe3,0x12},		
#elif defined(BF20A6_OUTPUT_MODE_CCIR656_4BIT)
	{0x3a,0x03},
	{0xe1,0x51},
	{0xe3,0x12},
#else
	{0x3a,0x01},
	{0xe1,0x93},
#endif	
	{0xe0,0x00},
	{0x2a,0x98},
	{0x0e,0x47},
	{0x0f,0x60},
	{0x10,0x57},
	{0x11,0x60},
	{0x30,0x61},
	{0x62,0xcd},
	{0x63,0x1a},
	{0x64,0x38},
	{0x65,0x52},
	{0x66,0x68},
	{0x67,0xc2},
	{0x68,0xa7},
	{0x69,0xab},
	{0x6a,0xad},
	{0x6b,0xa9},
	{0x6c,0xc4},
	{0x6d,0xc5},
	{0x6e,0x18},
	{0xc0,0x20},
	{0xc1,0x24},
	{0xc2,0x29},
	{0xc3,0x25},
	{0xc4,0x28},
	{0xc5,0x2a},
	{0xc6,0x41},
	{0xca,0x23},
	{0xcd,0x34},
	{0xce,0x32},
	{0xcf,0x35},
	{0xd0,0x6c},
	{0xd1,0x6e},
	{0xd2,0xcb},
	{0xe2,0x00},
	{0xe4,0x73},
	{0xe5,0x22},
	{0xe6,0x24},
	{0xe7,0x64},
	{0xe8,0xf2},//VDDIO=1.8v 0xe8[2]=1,VDDIO=2.8v 0xe8[2]=0
	{0x4a,0x08},  // 04  // 0x00 -- 0x04  --08 --ok --BY song , wuxx add --ZW206 08
	{0x00,0x03},
	{0x1f,0x02},
	{0x22,0x02},
	{0x0c,0x31},
	{0x00,0x00},
	{0x60,0x81},
	{0x61,0x81},
	{0xa0,0x08},
	{0x01,0x1d},
	{0x01,0x1d},
	{0x01,0x1d},
	{0x02,0x15},
	{0x02,0x15},
	{0x02,0x15},
	{0x13,0x08},
	{0x8a,0x96},
	{0x8b,0x06},
	{0x87,0x18},
	{0x34,0x48},//lens
	{0x35,0x40},
	{0x36,0x40},
	{0x71,0x44},//add 10.27	
	{0x72,0x47},//22
	{0x74,0x62},//42
	{0x75,0xa9},
	{0x78,0x23},
	{0x79,0xa0},
	{0x7a,0x94},
	{0x7c,0x97},
	{0x40,0x20},
	{0x41,0x30},
	{0x42,0x30},
	{0x43,0x28},
	{0x44,0x22},
	{0x45,0x1a},
	{0x46,0x13},
	{0x47,0x10},
	{0x48,0x0D},
	{0x49,0x0a},
	{0x4B,0x09},
	{0x4C,0x08},
	{0x4E,0x07},
	{0x4F,0x06},
	{0x50,0x04},
	{0x5f,0x29},
	{0x23,0x33},
	{0xa1,0x10},//AWB
	{0xa2,0x0d},//0c
	{0xa3,0x25},//23
	{0xa4,0x03},//07
	{0xa5,0x1e},//23
	{0xa6,0x56},
	{0xa7,0x18},
	{0xa8,0x1a},
	{0xa9,0x12},
	{0xaa,0x12},
	{0xab,0x16},
	{0xac,0xb1},
	{0xba,0x12},
	{0xbb,0x12},
	{0xad,0x12},
	{0xae,0x56},//56
	{0xaf,0x0a},
	{0x3b,0x30},
	{0x3c,0x12},
	{0x3d,0x22},
	{0x3e,0x3f},
	{0x3f,0x28},
	{0xb8,0xc3},
	{0xb9,0xA3},
	{0x39,0x47},//pure color threshold
	{0x26,0x13},
	{0x27,0x16},
	{0x28,0x14},
	{0x29,0x18},
	{0xee,0x0d},    
	{0xef,0x00},
	{0x13,0x05},
	{0x24,0x3c},//40
	{0x81,0x20},
	{0x82,0x40},
	{0x83,0x30},
	{0x84,0x58},
	{0x85,0x30},
	{0x92,0x08},
	{0x86,0xA0},
#ifdef BF20A6_OUTPUT_MODE_CCIR656_1BIT
	{0x8a,0x4b},
#else
	{0x8a,0x96},
#endif	
	{0x91,0xff},
	{0x94,0x62},
	{0x9a,0x18},//outdoor threshold
	{0xf0,0x46},
	{0x51,0x0c},//color normal 17
	{0x52,0x05},
	{0x53,0x3d},
	{0x54,0x47},
	{0x55,0x86},
	{0x56,0x26},
	{0x7e,0x17},
	{0x57,0x36},//A?color
	{0x58,0x2A},
	{0x59,0xAA},
	{0x5a,0xA8},
	{0x5b,0x43},
	{0x5c,0x10},
	{0x5d,0x00},
	{0x7d,0x36},
	{0x5e,0x10},
	{0xd6,0x95},//contrast
	{0xd5,0x20},//?????
	{0xb0,0x84},//????????
	{0xb5,0x08},//????????
	{0xb1,0xc8},//saturation
	{0xb2,0xc8},
	{0xb3,0xd0},
	{0xb4,0xB0},
	{0x32,0x10},
	{0xa0,0x09},
	{0x00,0x03},
	{0x0b,0x00},
	{0x15,0x80},
	{0x16,0x73},	
};


LOCAL SENSOR_REG_TAB_INFO_T s_BF20A6_resolution_Tab_YUV[]=
{
#if defined(BF20A6_OUTPUT_MODE_PACKET_DDR_2BIT)
    {ADDR_AND_LEN_OF_ARRAY(BF20A6_YUV_640X480), 640, 480, 26, SENSOR_IMAGE_FORMAT_YUV422},
    // YUV422 PREVIEW 1
    {PNULL, 0, 640, 480, 26, SENSOR_IMAGE_FORMAT_YUV422},
    {PNULL, 0, 0, 0, 0, 0},
    {PNULL, 0, 0, 0, 0, 0},
    {PNULL, 0, 0, 0, 0, 0},

#elif defined(BF20A6_OUTPUT_MODE_CCIR656_2BIT)
    {ADDR_AND_LEN_OF_ARRAY(BF20A6_YUV_640X480), 640, 480, 24, SENSOR_IMAGE_FORMAT_YUV422},
    // YUV422 PREVIEW 1
    {PNULL, 0, 640, 480, 24, SENSOR_IMAGE_FORMAT_YUV422},
    {PNULL, 0, 0, 0, 0, 0},
    {PNULL, 0, 0, 0, 0, 0},
    {PNULL, 0, 0, 0, 0, 0},
#endif
     // YUV422 PREVIEW 2
    {PNULL, 0, 0, 0, 0, 0},
    {PNULL, 0, 0, 0, 0, 0},
    {PNULL, 0, 0, 0, 0, 0},
    {PNULL, 0, 0, 0, 0, 0},

};

/******************************************************************************/
// Description:
// Global resource dependence:
// Author:
// Note:
//
/******************************************************************************/
LOCAL uint32 BF20A6_set_flash_enable(uint32 enable)
{
	BF20A6_LOG("david sensor set flash enable. %d", enable);
	GPIO_SetFlashLight(enable);
    return 0;
}

#ifdef BF20A6_SERIAL_LOAD_FROM_T_FLASH
LOCAL SENSOR_IOCTL_FUNC_TAB_T s_BF20A6_ioctl_func_tab =
{
// Internal
    PNULL,
    BF20A6_Power_On,
    PNULL,
    BF20A6_Identify,
    PNULL,
    PNULL,

    PNULL,
    BF20A6_GetPclkTab,

// External
    PNULL,
    PNULL, //Set_BF20A6_Mirror,
    PNULL, //Set_BF20A6_Flip,

    PNULL,//Set_BF20A6_Brightness,
    PNULL,//Set_BF20A6_Contrast,
    PNULL,
    PNULL,
    Set_BF20A6_Preview_Mode,

    PNULL,//Set_BF20A6_Image_Effect,
    PNULL,//BF20A6_Before_Snapshot,
    PNULL,//BF20A6_After_Snapshot,

    PNULL,//BF20A6_set_flash_enable,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,//Set_BF20A6_AWB,
    PNULL,
    PNULL,
    PNULL,//Set_BF20A6_Ev,
    PNULL,
    PNULL,
    PNULL,

    PNULL,
    PNULL,
    PNULL,//Set_BF20A6_Anti_Flicker,
    PNULL,//Set_BF20A6_Video_Mode,
    PNULL,
};
#else
LOCAL SENSOR_IOCTL_FUNC_TAB_T s_BF20A6_ioctl_func_tab =
{
// Internal
    PNULL,
    BF20A6_Power_On,
    PNULL,
    BF20A6_Identify,
    PNULL,
    PNULL,

    PNULL,
    BF20A6_GetPclkTab,

// External
    PNULL,
    PNULL, //Set_BF20A6_Mirror,
    PNULL, //Set_BF20A6_Flip,

    Set_BF20A6_Brightness,
    Set_BF20A6_Contrast,
    PNULL,
    PNULL,
    Set_BF20A6_Preview_Mode,

    Set_BF20A6_Image_Effect,
    PNULL,//BF20A6_Before_Snapshot,
    PNULL,//BF20A6_After_Snapshot,

    PNULL,//BF20A6_set_flash_enable,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    PNULL,
    Set_BF20A6_AWB,
    PNULL,
    PNULL,
    Set_BF20A6_Ev,
    PNULL,
    PNULL,
    PNULL,
    PNULL,

    PNULL,
    Set_BF20A6_Anti_Flicker,
    Set_BF20A6_Video_Mode,
    PNULL,
    PNULL,
};
#endif


/**---------------------------------------------------------------------------*
 ** 						Global Variables								  *
 **---------------------------------------------------------------------------*/
PUBLIC SENSOR_INFO_T g_BF20A6_yuv_info =
{
	BF20A6_I2C_ADDR_W,				// salve i2c write address
	BF20A6_I2C_ADDR_R, 				// salve i2c read address

	0,								// bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit
									// bit2: 0: i2c register addr  is 8 bit, 1: i2c register addr  is 16 bit
									// other bit: reseved
	
#if defined(BF20A6_OUTPUT_MODE_PACKET_DDR_2BIT)
	SENSOR_HW_SIGNAL_PCLK_P|\
	SENSOR_HW_SIGNAL_VSYNC_N|\
	SENSOR_HW_SIGNAL_HSYNC_N,
#elif defined(BF20A6_OUTPUT_MODE_CCIR656_2BIT)
	SENSOR_HW_SIGNAL_PCLK_P|\
	SENSOR_HW_SIGNAL_VSYNC_N|\
	SENSOR_HW_SIGNAL_HSYNC_N,
#endif

									// bit0: 0:negative; 1:positive -> polarily of pixel clock
									// bit2: 0:negative; 1:positive -> polarily of horizontal synchronization signal
									// bit4: 0:negative; 1:positive -> polarily of vertical synchronization signal
									// other bit: reseved

	// preview mode
	SENSOR_ENVIROMENT_NORMAL|\
	SENSOR_ENVIROMENT_NIGHT|\
	SENSOR_ENVIROMENT_SUNNY,

	// image effect
	SENSOR_IMAGE_EFFECT_NORMAL|\
	SENSOR_IMAGE_EFFECT_BLACKWHITE|\
	SENSOR_IMAGE_EFFECT_RED|\
	SENSOR_IMAGE_EFFECT_GREEN|\
	SENSOR_IMAGE_EFFECT_BLUE|\
	SENSOR_IMAGE_EFFECT_YELLOW|\
	SENSOR_IMAGE_EFFECT_NEGATIVE|\
	SENSOR_IMAGE_EFFECT_CANVAS,

	// while balance mode
	0,

	7,								// bit[0:7]: count of step in brightness, contrast, sharpness, saturation
									// bit[8:31] reseved

	SENSOR_LOW_PULSE_RESET,		// reset pulse level
	100,								// reset pulse width(ms)

	SENSOR_HIGH_LEVEL_PWDN,			// 1: high level valid; 0: low level valid

	2,								// count of identify code
	{{0xfc, 0x20},						// supply two code to identify sensor.
	{0xfd, 0xa6}},						// for Example: index = 0-> Device id, index = 1 -> version id

	SENSOR_AVDD_2800MV,				// voltage of avdd

	640,							// max width of source image
	480,							// max height of source image
	"BF20A6",						// name of sensor

	SENSOR_IMAGE_FORMAT_YUV422,		// define in SENSOR_IMAGE_FORMAT_E enum,
									// if set to SENSOR_IMAGE_FORMAT_MAX here, image format depent on SENSOR_REG_TAB_INFO_T
	SENSOR_IMAGE_PATTERN_YUV422_YUYV,	// pattern of input image form sensor;

	s_BF20A6_resolution_Tab_YUV,	// point to resolution table information structure
	&s_BF20A6_ioctl_func_tab,		// point to ioctl function table

	PNULL,							// information and table about Rawrgb sensor
	PNULL,							// extend information about sensor
	SENSOR_AVDD_1800MV,                     // iovdd
	SENSOR_AVDD_1800MV,                      // dvdd
	2,                     // skip frame num before preview
	2,                      // skip frame num before capture
	0,                     // deci frame num during preview;
	2,                     // deci frame num during video preview;
	0,                     // threshold enable
	0,                     // threshold mode
	0,                     // threshold start postion
	0,                     // threshold end postion

#if defined(BF20A6_OUTPUT_MODE_CCIR656_2BIT)
	SENSOR_OUTPUT_MODE_CCIR656_2BIT,
#elif defined(BF20A6_OUTPUT_MODE_PACKET_DDR_2BIT)
    SENSOR_OUTPUT_MODE_PACKET_DDR_2BIT, 
#else
	SENSOR_OUTPUT_MODE_CCIR656_2BIT,
#endif

#if defined(BF20A6_OUTPUT_MODE_CCIR656_2BIT)
	SENSOR_OUTPUT_ENDIAN_BIG
#elif defined(BF20A6_OUTPUT_MODE_PACKET_DDR_2BIT)
    SENSOR_OUTPUT_ENDIAN_LITTLE
#else
	SENSOR_OUTPUT_ENDIAN_BIG
#endif
};

/**---------------------------------------------------------------------------*
 ** 							Function  Definitions
 **---------------------------------------------------------------------------*/
LOCAL void BF20A6_WriteReg( uint8  subaddr, uint8 data )
{

	#ifndef	_USE_DSP_I2C_
		uint8 cmd[2];
		cmd[0]	=	subaddr;
		cmd[1]	=	data;
		//I2C_WriteCmdArr(BF20A6_I2C_ADDR_W, cmd, 2, SCI_TRUE);
		Sensor_WriteReg(subaddr, data);
	#else
		DSENSOR_IICWrite((uint16)subaddr, (uint16)data);
	#endif

	//BF20A6_LOG("SENSOR: BF20A6_WriteReg reg/value(%x,%x) !!", subaddr, data);

}

LOCAL uint8 BF20A6_ReadReg( uint8  subaddr)
{
	uint8 value = 0;

	#ifndef	_USE_DSP_I2C_
		//I2C_WriteCmdArr(BF20A6_I2C_ADDR_W, &subaddr, 1, SCI_TRUE);
		//I2C_ReadCmd(BF20A6_I2C_ADDR_R, &value, SCI_TRUE);
		value =Sensor_ReadReg( subaddr);
	#else
		value = (uint16)DSENSOR_IICRead((uint16)subaddr);
	#endif

	BF20A6_LOG("SENSOR: BF20A6_ReadReg reg/value(%x,%x) !!", subaddr, value);

	return value;
}


LOCAL void BF20A6_Write_Group_Regs( SENSOR_REG_T* sensor_reg_ptr )
{
    uint32 i;

    for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) || (0xFF != sensor_reg_ptr[i].reg_value) ; i++)
    {
        Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
    }

}

LOCAL void BF20A6_Delayms (uint32 ms)
{
	uint32 t1, t2;

	t1 = t2 = SCI_GetTickCount ();

	do{
		t2 = SCI_GetTickCount ();
	}while (t2 < (t1+ms));
}


/**---------------------------------------------------------------------------*
 **                         Function Definitions                              *
 **---------------------------------------------------------------------------*/
LOCAL uint32 BF20A6_Power_On(uint32 power_on)
{
    SENSOR_AVDD_VAL_E		dvdd_val=g_BF20A6_yuv_info.dvdd_val;
    SENSOR_AVDD_VAL_E		avdd_val=g_BF20A6_yuv_info.avdd_val;
    SENSOR_AVDD_VAL_E		iovdd_val=g_BF20A6_yuv_info.iovdd_val;  
    BOOLEAN 				power_down=(BOOLEAN )g_BF20A6_yuv_info.power_down_level;	    
    BOOLEAN 				reset_level=g_BF20A6_yuv_info.reset_pulse_level;
    uint32 				    reset_width=g_BF20A6_yuv_info.reset_pulse_width;	    
    
    if(SCI_TRUE==power_on)
    {
        Sensor_SetVoltage(dvdd_val, avdd_val, iovdd_val);
        
        GPIO_SetSensorPwdn(!power_down);

        // Open Mclk in default frequency
        Sensor_SetMCLK(SENSOR_DEFALUT_MCLK);   
        
        SCI_Sleep(20);
        Sensor_SetResetLevel(reset_level);
        SCI_Sleep(reset_width);
        Sensor_SetResetLevel(!reset_level);
        SCI_Sleep(100);
    }
    else
    {
        GPIO_SetSensorPwdn(power_down);

        Sensor_SetMCLK(SENSOR_DISABLE_MCLK);           

        Sensor_SetVoltage(SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);        
    }
    
    SCI_TRACE_LOW("SENSOR: BF20A6_Power_On(1:on, 0:off): %d", power_on);    
    
    return SCI_SUCCESS;
}


/******************************************************************************/
// Description: sensor probe function
// Author:     benny.zou
// Input:      none
// Output:     result
// Return:     0           successful
//             others      failed
// Note:       this function only to check whether sensor is work, not identify
//              whitch it is!!
/******************************************************************************/
uint8 g_bf20a6_id[2] = {0};
LOCAL uint32 BF20A6_Identify(uint32 param)
{
	uint32 i;
	uint32 nLoop;
	uint8 ret;
	uint32 err_cnt = 0;
	uint8 reg[2] = {0xfC, 0xfD};
	uint8 value[2] = {0x20, 0xa6};

	SCI_TRACE_LOW("BF20A6_Identify: start");

	for(i = 0; i<2; )
	{
		nLoop = 1000;
		ret = BF20A6_ReadReg(reg[i]);
                SCI_TRACE_LOW("BF20A6_Identify: ret=%x", ret);
            g_bf20a6_id[i] = ret;
		if( ret != value[i])
		{
			err_cnt++;
			if(err_cnt>3)
			{
				return SCI_ERROR;
			}
			else
			{
				//Masked by frank.yang,SCI_Sleep() will cause a  Assert when called in boot precedure
				while(nLoop--);
				continue;
			}
		}
        err_cnt = 0;
		i++;
	}

	SCI_TRACE_LOW("BF20A6_Identify: it is BF20A6");
	return (uint32)SCI_SUCCESS;
}


LOCAL SENSOR_TRIM_T s_BF20A6_Pclk_Tab[]=
{
	// COMMON INIT
	{0, 0, 0, 0,  6},

	// YUV422 PREVIEW 1
	{0, 0, 0, 0,  6},
	{0, 0, 0, 0,  6},
	{0, 0, 0, 0,  6},
	{0, 0, 0, 0,  6},

	// YUV422 PREVIEW 2
	{0, 0, 0, 0,  0},
	{0, 0, 0, 0,  0},
	{0, 0, 0, 0,  0},
	{0, 0, 0, 0,  0}
};

LOCAL uint32 BF20A6_GetPclkTab(uint32 param)
{
    return (uint32)s_BF20A6_Pclk_Tab;
}


__align(4) const SENSOR_REG_T BF20A6_brightness_tab[][2]=
{
	{{0x24, 0x1f}, {0xff, 0xff}},
	{{0x24, 0x2f}, {0xff, 0xff}},
	{{0x24, 0x3f}, {0xff, 0xff}},
	{{0x24, 0x3c}, {0xff, 0xff}},
	{{0x24, 0x50}, {0xff, 0xff}},
	{{0x24, 0x60}, {0xff, 0xff}},
	{{0x24, 0x70}, {0xff, 0xff}}, 
};

LOCAL uint32 Set_BF20A6_Brightness(uint32 level)
{

    SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)BF20A6_brightness_tab[level];

    SCI_ASSERT(PNULL != sensor_reg_ptr);

    BF20A6_Write_Group_Regs(sensor_reg_ptr);

    BF20A6_LOG("david sensor set_BF20A6_brightness: level = %d", level);

    return 0;
}



__align(4) const SENSOR_REG_T BF20A6_contrast_tab[][2]=
{
	{{0xd6,0x50}, {0xff,0xff}},
	{{0xd6,0x60}, {0xff,0xff}},
	{{0xd6,0x70}, {0xff,0xff}},
	{{0xd6,0x88}, {0xff,0xff}},
	{{0xd6,0xa0}, {0xff,0xff}},
	{{0xd6,0xb0}, {0xff,0xff}},
	{{0xd6,0xc0}, {0xff,0xff}}, 
};

LOCAL uint32 Set_BF20A6_Contrast(uint32 level)
{

    SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)BF20A6_contrast_tab[level];

    SCI_ASSERT(PNULL != sensor_reg_ptr);

    BF20A6_Write_Group_Regs(sensor_reg_ptr);

    BF20A6_LOG("david sensor set_BF20A6_contrast: level = %d", level);

    return 0;
}


__align(4) const SENSOR_REG_T BF20A6_image_effect_tab[][5]=
{
		// effect normal
			{
				{0x15,0x00},
				{0x0b,0x02},
				{0x1c,0x80},
				{0x1d,0x80},
				{0xff,0xff}
			},
			//effect BLACKWHITE
			{
				{0x15,0x00},
				{0x0b,0x22},
				{0x1c,0x80},
				{0x1d,0x80},
				{0xff,0xff}
			},
			// effect RED pink
			{
				{0x15,0x00},
				{0x0b,0x22},
				{0x1c,0x60},
				{0x1d,0xe0},
				{0xff,0xff}
			},
			// effect GREEN
			{
				{0x15,0x00},
				{0x0b,0x22},
				{0x1c,0x60},
				{0x1d,0x60},
				{0xff,0xff}
			},
			// effect  BLUE
			{
				{0x15,0x00},
				{0x0b,0x22},
				{0x1c,0xa0},
				{0x1d,0x60},
				{0xff,0xff}
			},
			// effect  YELLOW
			{
				{0x15,0x00},
				{0x0b,0x22},
				{0x1c,0x50},
				{0x1d,0x90},
				{0xff,0xff}
			},
			// effect NEGATIVE
			{
				{0x15,0x02},
				{0x0b,0x02},
				{0x1c,0x80},
				{0x1d,0x80},
				{0xff,0xff}
			},
			//effect ANTIQUE
			{
				{0x15,0x00},
				{0x0b,0x22},
				{0x1c,0x50},
				{0x1d,0x90},
				{0xff,0xff}
			},
};

LOCAL uint32 Set_BF20A6_Image_Effect(uint32 effect_type)
{

    SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)BF20A6_image_effect_tab[effect_type];

    SCI_ASSERT(PNULL != sensor_reg_ptr);

    BF20A6_Write_Group_Regs(sensor_reg_ptr);

    BF20A6_LOG("david sensor set_BF20A6_image_effect: effect_type = %d", effect_type);

    return 0;
}



__align(4) const SENSOR_REG_T BF20A6_ev_tab[][2]=
{
	{{0x24, 0x1f}, {0xff, 0xff}},
	{{0x24, 0x2f}, {0xff, 0xff}},
	{{0x24, 0x3f}, {0xff, 0xff}},
	{{0x24, 0x3c}, {0xff, 0xff}},
	{{0x24, 0x50}, {0xff, 0xff}},
	{{0x24, 0x60}, {0xff, 0xff}},
	{{0x24, 0x70}, {0xff, 0xff}}, 
};

LOCAL uint32 Set_BF20A6_Ev(uint32 level)
{

    SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)BF20A6_ev_tab[level];

	BF20A6_LOG("david sensor set_BF20A6_ev: level = %d", level);
    SCI_ASSERT(PNULL != sensor_reg_ptr);
    SCI_ASSERT(level < 7);

    BF20A6_Write_Group_Regs(sensor_reg_ptr );

    return 0;
}

/******************************************************************************/
// Description: anti 50/60 hz banding flicker
// Global resource dependence:
// Author:
// Note:
//
/******************************************************************************/
LOCAL uint32 Set_BF20A6_Anti_Flicker(uint32 mode)
{

	BF20A6_LOG("david sensor set_BF20A6_anti_flicker-mode=%d", mode);
	switch(mode)
	{
		case DCAMERA_FLICKER_50HZ:
		#ifdef BF20A6_OUTPUT_MODE_CCIR656_1BIT
		BF20A6_WriteReg(0x8a, 0x4b);
		#else
		BF20A6_WriteReg(0x8a, 0x96);
		#endif
		break;

		case DCAMERA_FLICKER_60HZ:
		#ifdef BF20A6_OUTPUT_MODE_CCIR656_1BIT
		BF20A6_WriteReg(0x8a, 0x3f);
		#else
		BF20A6_WriteReg(0x8a, 0x7e);
		#endif
		break;

		default:
		break;
    }

    
    OS_TickDelay(100);

    return 0;
}

LOCAL uint32 Set_BF20A6_Preview_Mode(uint32 preview_mode)
{
	switch (preview_mode)
	{
		case DCAMERA_ENVIRONMENT_NORMAL:
		{
			BF20A6_WriteReg(0x86, 0xa0);
			BF20A6_WriteReg(0xf0, 0x46);
			break;									
		}	
		case DCAMERA_ENVIRONMENT_NIGHT:
		{
			BF20A6_WriteReg(0x86, 0xff);
			BF20A6_WriteReg(0xf0, 0x54);
			break;							
		}				
		default:
			break;
	}

#if 0 //def ZDT_PCBA_ZW31_SUPPORT
	if(Sensor_Main_Sub_Switch_Get())
	{
		BF20A6_WriteReg(0x17,0x55);//BF20A6_WriteReg(0x17,0x56); //mirror
	}
	else
	{
		BF20A6_WriteReg(0x17,0x55);//BF20A6_WriteReg(0x17,0x54);//mirror
	}
#endif

#ifdef BF20A6_SERIAL_LOAD_FROM_T_FLASH  
	 Load_BF20A6_RegTab_From_T_Flash();
#endif	

	return 0;
}

#if 0//wuxx del
__align(4) const SENSOR_REG_T GC032a_video_mode_nor_tab[][4]=
{
    // normal mode 
    {{0xff, 0xff}},  
    
    //video mode    fps 15 
    {{0xfe,0x01},{0x3c,0x00},{0xfe,0x00},{0xff,0xff}},
    
    // UPCC  mode     
    {{0xff, 0xff}}   
};  
#endif


__align(4) const SENSOR_REG_T BF20A6_awb_tab[][6]=
{
	//AUTO
	{{0xa0, 0x09},{0x5f,0x29},{0x23,0x33},{0x01, 0x19},{0x02, 0x15},{0xff, 0xff}},    
	//INCANDESCENCE:
	{{0xa0, 0x08},{0x5f,0x19},{0x23,0x33},{0x01, 0x2a},{0x02, 0x0a},{0xff, 0xff}},
	//U30
	{{0xa0, 0x08},{0x5f,0x19},{0x23,0x33},{0x01, 0x24},{0x02, 0x10},{0xff, 0xff}},  
	//CWF  //
	{{0xa0, 0x08},{0x5f,0x19},{0x23,0x33},{0x01, 0x15},{0x02, 0x3c},{0xff, 0xff}},    
	//FLUORESCENT:
	{{0xa0, 0x08},{0x5f,0x19},{0x23,0x33},{0x01, 0x13},{0x02, 0x1e},{0xff, 0xff}},
	//SUN:
	{{0xa0, 0x08},{0x5f,0x19},{0x23,0x33},{0x01, 0x0f},{0x02, 0x26},{0xff, 0xff}},
	//CLOUD:
	{{0xa0, 0x08},{0x5f,0x19},{0x23,0x33},{0x01, 0x0b},{0x02, 0x2e},{0xff, 0xff}},
};

LOCAL uint32 Set_BF20A6_AWB(uint32 mode)
{

	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)BF20A6_awb_tab[mode];

	SCI_ASSERT(mode < 7);
	SCI_ASSERT(PNULL != sensor_reg_ptr);

	BF20A6_Write_Group_Regs(sensor_reg_ptr);

	BF20A6_LOG("david sensor set_BF20A6_awb_mode: mode = %d", mode);

	return 0;
}

LOCAL uint32 BF20A6_AE_AWB_Enable(uint32 ae_enable, uint32 awb_enable)
{
	uint16 ae_value = 0 , awb_value=0;

	awb_value = BF20A6_ReadReg(0x13);

	if(0x01==ae_enable)
	{
		ae_value = awb_value|0x01;
	}
	else if(0x00==ae_enable)
	{
		ae_value =awb_value&~0x01;
	}

	BF20A6_WriteReg(0x13, ae_value);

	BF20A6_LOG("SENSOR: set_ae_awb_enable: ae_enable = %d  awb_enable = %d", ae_enable ,awb_enable);
	return 0;
}


LOCAL uint32 BF20A6_Before_Snapshot(uint32 para)
{
	uint8 reg_val = 0;
	uint16 shutter=0x00;

	BF20A6_AE_Enable(0x00);   // close aec

	reg_val = BF20A6_ReadReg(0x8d);
	shutter=reg_val&0x00ff;
	reg_val = BF20A6_ReadReg(0x8c);
	shutter|=((reg_val&0x00ff)<<0x08);

	//BF20A6_WriteReg(0xca, 0xc2);

	shutter = shutter>>1;
		
	if(shutter<1)
	{
		shutter=1;
	}

	reg_val=shutter&0x00ff;
	//BF20A6_WriteReg(0x8d, reg_val);
	reg_val=(shutter&0xff00)>>8;
	//BF20A6_WriteReg(0x8c, reg_val);

	BF20A6_Delayms(400);

	SCI_TRACE_LOW("SENSOR_BF20A6: Before Snapshot");

	return 0;
}



LOCAL uint32 BF20A6_After_Snapshot(uint32 para)
{
	BF20A6_AE_Enable(0x01);   // Open aec awb

	BF20A6_Delayms(400);

	BF20A6_LOG("david sensor SENSOR_BF20A6: After Snapshot");

	return 0;
}



/******************************************************************************/
// Description: set video mode
// Global resource dependence:
// Author:
// Note:
//
/******************************************************************************/
LOCAL uint32 Set_BF20A6_Video_Mode(uint32 mode)
{

	SCI_ASSERT(mode <=DCAMERA_MODE_MAX);

	if(1==mode ||2==mode )
	{
		SCI_TRACE_LOW("set_BF20A6_video_mode=%d",mode);
	}

	return 0;
}


typedef enum
{
	BF20A6_ID_KINGCOM = 0,
	BF20A6_ID_CXT,	
};

// KINGCOM Pin9_id1 :1 Pin10_id2: 0
// CXT     Pin9_id1 :1 Pin10_id2: 1
LOCAL uint32 BF20A6_Get_Factory_ID(void)
{
	return 0;
}

#ifdef BF20A6_SERIAL_LOAD_FROM_T_FLASH  
__align(4) static SENSOR_REG_T BF20A6_YUV_Init_Reg[1000] = {{0x00,0x00},}; 
//#define READ_BUFFER_SIZE  (274*12)    //must be divisible by 12
LOCAL uint32 Load_BF20A6_RegTab_From_T_Flash(void)
{
    SFS_HANDLE    file_handle = 0;
    FFS_ERROR_E   ffs_err = FFS_NO_ERROR;
  //  char *file_name = "C:\\BF20A6_SERIAL_Initialize_Setting.Bin";
   	static wchar unicode_file_name[256] = {0};
    int regNum = 0; //uint32 regNo = 0;
    int i = 0;  //uint32 i = 0;
    uint8 *curr_ptr = NULL;
    uint8 *buffer_ptr = NULL;  //char buffer_ptr[READ_BUFFER_SIZE] = {0};
    uint32 read_size = 0;
	uint32 file_size = 0;
    uint8 func_ind[3] = {0};  /* REG or DLY */
   // for(i=0;i<25;i++)
       // unicode_file_name[i] = file_name[i];

	SCI_MEM16CPY(unicode_file_name,L"E:\\BF20A6_SERIAL_Initialize_Setting.Bin",sizeof(L"E:\\BF20A6_SERIAL_Initialize_Setting.Bin"));
   

	/* Search the setting file in all of the user disk. */
	#if 0
	curr_ptr = (uint8 *)unicode_file_name;
	while (file_handle == 0)
    {
        if ((*curr_ptr >= 'c' && *curr_ptr <= 'z') || (*curr_ptr >= 'C' && *curr_ptr <= 'Z'))
        {
			file_handle = SFS_CreateFile((const uint16 *)unicode_file_name, 0x0030|SFS_MODE_READ, NULL, NULL);	//	FFS_MODE_OPEN_EXISTING
            if (file_handle > 0)
            {
                break; /* Find the setting file. */
            }
            *curr_ptr = *curr_ptr + 1;
        }
        else
        {
            break ;
        }
    }
	#else
	file_handle = SFS_CreateFile((const uint16 *)unicode_file_name, 0x0030|SFS_MODE_READ, NULL, NULL);	//	FFS_MODE_OPEN_EXISTING
	#endif
	
	if(file_handle == 0) //read file error
    {
		BF20A6_LOG("!!! Warning, Can't find the initial setting file!!!");
		return SCI_ERROR;
    }


	SFS_GetFileSize(file_handle,&file_size);
	if(file_size < 10)
	{
		BF20A6_LOG("!!! Warning, Invalid setting file!!!");
		return SCI_ERROR;
	}
	
	buffer_ptr =SCI_ALLOCA(file_size);///
	
	if (buffer_ptr == NULL)
	{
    	BF20A6_LOG("!!! Warning, Memory not enoughed...");
    	return SCI_ERROR;        /* Memory not enough */
	}
    ffs_err = SFS_ReadFile(file_handle, buffer_ptr, file_size, &read_size, NULL);
	curr_ptr = buffer_ptr;
    if(SFS_NO_ERROR == ffs_err)
    {
		while(curr_ptr < (buffer_ptr + read_size))
        {     
        	while ((*curr_ptr == ' ') || (*curr_ptr == '\t')) /* Skip the Space & TAB */
        		curr_ptr++;
			if (((*curr_ptr) == '/') && ((*(curr_ptr + 1)) == '*'))
   			{
   	    		 while (!(((*curr_ptr) == '*') && ((*(curr_ptr + 1)) == '/')))
       		 	{
            		curr_ptr++;    /* Skip block comment code. */
       		 	}
        		while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
        		{
            		curr_ptr++;
        		}
        		curr_ptr += 2;            /* Skip the enter line */
        		continue ;
    		}
			if (((*curr_ptr) == '/') && ((*(curr_ptr+1)) == '/'))   /* Skip // block comment code. */
    		{
        		while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
        		{
            		curr_ptr++;
        		}
        		curr_ptr += 2;            /* Skip the enter line */
        		continue ;
   			}

			/* This just content one enter line. */
    		if (((*curr_ptr) == 0x0D) && ((*(curr_ptr + 1)) == 0x0A))
    		{
        		curr_ptr += 2;
        		continue ;
    		}
			SCI_MEMCPY(func_ind, curr_ptr,3);
			curr_ptr += 4;  
			if (strcmp((const char *)func_ind, "REG") == 0)    /* REG */
			{
            BF20A6_YUV_Init_Reg[regNum].reg_addr = (uint16)strtol(curr_ptr, NULL, 16);
            curr_ptr += 5;
            BF20A6_YUV_Init_Reg[regNum].reg_value = (uint16)strtol(curr_ptr, NULL, 16);
            //strNum += 5;
			
			}
			regNum++;	
			/* Skip to next line directly. */
    		while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
    		{
        		curr_ptr++;
    		}		
            curr_ptr += 2;    
        }
    }
	BF20A6_YUV_Init_Reg[regNum].reg_addr = 0xff;
	BF20A6_YUV_Init_Reg[regNum].reg_value =0xff;
	
	BF20A6_LOG("%d register read...", i);
 
	SCI_FREE(buffer_ptr);
 	buffer_ptr = NULL;	
    ffs_err = SFS_CloseFile( file_handle);
	file_handle = NULL;

	BF20A6_LOG("Start apply initialize setting.");
    /* Start apply the initial setting to sensor. */
	SCI_ASSERT(PNULL != BF20A6_YUV_Init_Reg);
    BF20A6_Write_Group_Regs(BF20A6_YUV_Init_Reg);

	return SCI_SUCCESS;
}
#endif

#endif



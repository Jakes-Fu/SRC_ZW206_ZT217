
#ifndef __qma_x981_h
#define __qma_x981_h


/******************************************************
	Motion sensor controller register macro define
*******************************************************/
//#define QMAX981_HAND_LIGHT
#define QMAX981_STEPCOUNTER
//#define QMA7981_HAND_UP_DOWN
#define QMA7981_ANY_MOTION
//#define QMA7981_SIGNIFICANT_MOTION
//#define QMA7981_NO_MOTION
//#define QMA7981_HAND_UP_DOWN
//#define QMA7981_INT_LATCH

//#define QMAX981_USE_INT1

#if defined(QMAX981_STEPCOUNTER)
//#define QMAX981_STEP_COUNTER_USE_INT		// for qma6981
#define QMAX981_CHECK_ABNORMAL_DATA
#endif

/***********QST TEAM***************************************/
#define GRAVITY_1G			9807

#define QMAX981_RANGE_2G        (1<<0)
#define QMAX981_RANGE_4G        (1<<1)
#define QMAX981_RANGE_8G        (1<<2)
#define QMAX981_RANGE_16G       (1<<3)
#define QMAX981_RANGE_32G       0x0f
	
#if defined(QMAX981_STEPCOUNTER)
#define QMAX981_OFFSET_X		0x60
#define QMAX981_OFFSET_Y		0x60
#define QMAX981_OFFSET_Z		0x60
#else
#define QMAX981_OFFSET_X		0x00
#define QMAX981_OFFSET_Y		0x00
#define QMAX981_OFFSET_Z		0x00
#endif
	
#define QMAX981_CHIP_ID			0x00
#define QMAX981_XOUTL			0x01	// 4-bit output value X
#define QMAX981_XOUTH			0x02	// 6-bit output value X
#define QMAX981_YOUTL			0x03	
#define QMAX981_YOUTH			0x04	
#define QMAX981_ZOUTL			0x05	
#define QMAX981_ZOUTH			0x06
#define QMAX981_STEP_CNT_L		0x07
	
#define QMAX981_RANGE			0x0f
#define QMAX981_ODR				0x10
#define QMAX981_MODE			0x11
	
#define QMAX981_INT_MAP0		0x19	// INT MAP
#define QMAX981_INT_STAT		0x0a    //interrupt statues
	
#define QMAX981_FIFO_WTMK		0x31	// FIFO water mark level
#define QMAX981_FIFO_CONFIG		0x3e	// fifo configure
#define QMAX981_FIFO_DATA		0x3f	//fifo data out 
	
#define SINGLE_TAP 1
#define DOUBLE_TAP 2
	
#define YZQ_ABS(X) ((X) < 0 ? (-1 * (X)) : (X))
	
//-------------------------------------
// 中断处理
//-------------------------------------
// add by qst for test
#define QMAX981_LOW_G_FLAG		0x08
#define QMAX981_HIGH_G_Z_FLAG	0x04
#define QMAX981_HIGH_G_Y_FLAG	0x02
#define QMAX981_HIGH_G_X_FLAG	0x01
	
//#define QMAX981_FUNC		QMAX981_LOW_G_FLAG
	
//-------------------------------------
// 中断处理
//-------------------------------------
#define QMAX981_D_TAP_FLAG	0x10
#define QMAX981_S_TAP_FLAG	0x20
#define QMAX981_ORIENT_FLAG	0x40
#define QMAX981_FOB_FLAG	0x80
	
#define QMAX981_LOW_G_FLAG		0x08
#define QMAX981_HIGH_G_Z_FLAG	0x04
#define QMAX981_HIGH_G_Y_FLAG	0x02
#define QMAX981_HIGH_G_X_FLAG	0x01
	
//#define QMA6891_EXTRA_FUNC_1		(QMAX981_FOB_FLAG|QMAX981_ORIENT_FLAG)
//#define QMA6891_EXTRA_FUNC_2		(QMAX981_HIGH_G_Z_FLAG|QMAX981_HIGH_G_Y_FLAG|QMAX981_HIGH_G_X_FLAG)
//#define QMA6891_EXTRA_FUNC_3		(QMAX981_S_TAP_FLAG)


//#define QMA6100_ODR_27
#define QMA6100_ODR_55
//#define QMA6100_ODR_100

typedef enum
{
	QMA6100_DISABLE = 0,
	QMA6100_ENABLE = 1
}qma6100_enable;

typedef enum
{
	QMA6100_MODE_SLEEP,
	QMA6100_MODE_ACTIVE,
	QMA6100_MODE_MAX
}qma6100_power_mode;

typedef enum
{
	QMA6100_FIFO_MODE_NONE,
	QMA6100_FIFO_MODE_FIFO,
	QMA6100_FIFO_MODE_STREAM,
	QMA6100_FIFO_MODE_BYPASS,
	QMA6100_FIFO_MODE_MAX
}qma6100_fifo_mode;

typedef enum
{
	QMA6100_TAP_SINGLE = 0x80,
	QMA6100_TAP_DOUBLE = 0x20,
	QMA6100_TAP_TRIPLE = 0x10,
	QMA6100_TAP_QUARTER = 0x01,
	QMA6100_TAP_MAX = 0xff
}qma6100_tap;

typedef enum
{
	QMA6100_MAP_INT1,
	QMA6100_MAP_INT2,
	QMA6100_MAP_INT_NONE
}qma6100_int_map;

typedef enum
{
	QMA6100_MCLK_500K = 0x00,
	QMA6100_MCLK_333K = 0x01,
	QMA6100_MCLK_200K = 0x02,
	QMA6100_MCLK_100K = 0x03,
	QMA6100_MCLK_50K = 0x04,
	QMA6100_MCLK_20K = 0x05,
	QMA6100_MCLK_10K = 0x06,
	QMA6100_MCLK_5K = 0x07,
	QMA6100_MCLK_RESERVED = 0xff
}qma6100_mclk;

typedef enum
{
	QMA6100_DIV_512 = 0x00,
	QMA6100_DIV_256 = 0x01,
	QMA6100_DIV_128 = 0x02,
	QMA6100_DIV_64 = 0x03,
	QMA6100_DIV_32 = 0x04,
	QMA6100_DIV_1024 = 0x05,
	QMA6100_DIV_2048 = 0x06,
	QMA6100_DIV_4096 = 0x07,
	QMA6100_DIV_RESERVED = 0xff
}qma6100_div;

typedef enum
{
	QMA6100_LPF_0 = (0x00<<5),
	QMA6100_LPF_2 = (0x01<<5),
	QMA6100_LPF_4 = (0x02<<5),
	QMA6100_LPF_16 = (0x03<<5),
	QMA6100_LPF_RESERVED = 0xff
}qma6100_lpf;

typedef enum
{
  QMA6100_STEP_LPF_0 = (0x00<<6),
  QMA6100_STEP_LPF_2 = (0x01<<6),
  QMA6100_STEP_LPF_4 = (0x02<<6),
  QMA6100_STEP_LPF_8 = (0x03<<6),
  QMA6100_STEP_LPF_RESERVED = 0xff
}qma6100_step_lpf;

typedef enum
{
  QMA6100_STEP_AXIS_ALL = 0x00,
  QMA6100_STEP_AXIS_YZ = 0x01,
  QMA6100_STEP_AXIS_XZ = 0x02,
  QMA6100_STEP_AXIS_XY = 0x03,
  QMA6100_STEP_AXIS_RESERVED = 0xff
}qma6100_step_axis;

typedef enum
{
  QMA6100_STEP_START_0 = 0x00,
  QMA6100_STEP_START_4 = 0x20,
  QMA6100_STEP_START_8 = 0x40,
  QMA6100_STEP_START_12 = 0x60,
  QMA6100_STEP_START_16 = 0x80,
  QMA6100_STEP_START_24 = 0xa0,
  QMA6100_STEP_START_32 = 0xc0,
  QMA6100_STEP_START_40 = 0xe0,
  QMA6100_STEP_START_RESERVED = 0xff
}qma6100_step_start_cnt;

typedef enum
{
	QMAX981_TYPE_UNKNOW=0x00,
	QMAX981_TYPE_6981,
	QMAX981_TYPE_7981,
	QMAX981_TYPE_6100,
	QMAX981_TYPE_MAX
}qmaX981_chip_type;


#endif

/******************************************************************************
 ** File Name:     tft_ST7789p3.c                                             *
 ** Description:															  *
 **    This file contains driver for color LCD.(CSTN)						  *
 ** Author:         sky.wang                                                 *
 ** DATE:           20/07/2018                                                *
 ** Copyright:      2004 Spreatrum, Incoporated. All Rights Reserved.         *
 ** Description:                                                              *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 02/102010      lichd    	    Create.
 ******************************************************************************/

#include "ms_customize_trc.h"
#include "os_api.h"
#include "dal_lcd.h"
#include "lcd_cfg.h"
#include "lcm_drvapi.h"
#include "spi_drvapi.h"
#include "timer_drvapi.h"

#include "hal_gouda.h"
#include "ldo_drvapi.h"
#include "lcm_cfg.h"

/**---------------------------------------------------------------------------*
 **                            Macro Define
 **---------------------------------------------------------------------------*/

#ifdef   __cplusplus
    extern   "C" 
    {
#endif

#define SCI_TraceLow_Test SCI_TraceLow

LOCAL uint16 st7789p3_line_mode = 0;
LOCAL uint16 st7789p3_farmk = 0;

#define ST7789P3_X_OFFSET 1
#define ST7789P3_Y_OFFSET 3

/*lint -save -e767 -e718*/
//#define LCD_CtrlWrite_ST7789p3( _cmd ) LCD_SendCmd( (_cmd ), 0 );

//#define LCD_DataWrite_ST7789p3( _data ) LCD_SendData( (_data), 0 );



#define LCM_WR_REG(Addr, Data)                              \
    {                                                       \
        while (hal_GoudaWriteReg(Addr, Data) != HAL_ERR_NO) \
            ;                                               \
    }
#define LCD_DataWrite_ST7789p3(Data)                               \
    {                                                  \
        while (hal_GoudaWriteData(Data) != HAL_ERR_NO) \
            ;                                          \
    }
#define LCD_CtrlWrite_ST7789p3(Cmd)                              \
    {                                                \
        while (hal_GoudaWriteCmd(Cmd) != HAL_ERR_NO) \
            ;                                        \
    }

//#define LCD_CtrlData_ST7789p3(cmd, data)  LCD_CtrlWrite_ST7789p3( cmd ); LCD_DataWrite_ST7789p3( data );

/*lint -restore*/

#ifdef CHIP_ENDIAN_LITTLE
typedef union {
	struct {
		unsigned int resv   :2;
		unsigned int mh    :1;	// display data latch order
		unsigned int rgb    :1;	// rgb order
		unsigned int ml    :1;		// line address order
		unsigned int mv    :1;	// x/y exchanged
		unsigned int mx    :1;	// horizontal mirror
		unsigned int my    :1;	// vertical mirror 
	} bitsval;
	unsigned int dataval;
} MEM_DATA_ACCESS_CTRL_U;
#else
typedef union {
	struct {
		unsigned int my    :1;	// vertical mirror 
		unsigned int mx    :1;	// horizontal mirror
		unsigned int mv    :1;	// x/y exchanged
		unsigned int ml    :1;		// line address order
		unsigned int rgb    :1;	// rgb order
		unsigned int mh    :1;	// display data latch order
		unsigned int resv   :2;
	} bitsval;
	unsigned int dataval;
} MEM_DATA_ACCESS_CTRL_U;
#endif // CHIP_ENDIAN_LITTLE

extern void hal_LcdDelay (int ms_delay);

LOCAL MEM_DATA_ACCESS_CTRL_U s_lcd_madc;


/******************************************************************************/
//  Description:   Close the lcd.(include sub lcd.)
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:
/******************************************************************************/
LOCAL void ST7789p3_Close(void);

/******************************************************************************/
//  Description:   Enter/Exit sleep mode .
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:
/******************************************************************************/
LOCAL ERR_LCD_E  ST7789p3_EnterSleep(BOOLEAN is_sleep);

/*********************************************************************/
//  Description:   Initialize color LCD : GC9304
//  Input:
//      None.
//  Return:
//      None.
//	Note:           
/*********************************************************************/
LOCAL void ST7789p3_Init(void);

LOCAL ERR_LCD_E ST7789p3_Invalidate(void);
/******************************************************************************/
//  Description:   Copy a retangle data from clcd_buffer to display RAM.
//                     then the rectangle display is to be refreshed
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:       
//     To improve speed, lcd is operate in HIGH SPEED RAM WRITE MODE(4
//     uint16 are write continuously always.) So, some dummy uint16 
//     should be inserted to satisfy this mode.   Please refer to spec.
/******************************************************************************/
LOCAL ERR_LCD_E ST7789p3_InvalidateRect(
    uint16 left, //the left value of the rectangel
    uint16 top, //top of the rectangle
    uint16 right, //right of the rectangle
    uint16 bottom //bottom of the rectangle
    );

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:
/******************************************************************************/
LOCAL void ST7789p3_set_display_window(
    uint16 left, // start Horizon address
    uint16 right, // end Horizon address
    uint16 top, // start Vertical address
    uint16 bottom // end Vertical address
    );

LOCAL uint32 ST7789p3_ReadID(uint16 lcd_cs, uint16 lcd_cd, uint16 lcd_id);

/**---------------------------------------------------------------------------*
 **                      Function  Definitions
 **---------------------------------------------------------------------------*/

LOCAL ERR_LCD_E ST7789p3_SetDirection(LCD_DIRECT_E direct_type)
{
   gouda_event(0x90001101);

    SCI_TraceLow_Test("ST7789p3_SetDirection = %d",direct_type);

    switch(direct_type)
    {
        case LCD_DIRECT_NORMAL:
            s_lcd_madc.bitsval.mx = 0;
            s_lcd_madc.bitsval.my = 0;
            s_lcd_madc.bitsval.mv = 0;
            break;
        case LCD_DIRECT_ROT_90:
            s_lcd_madc.bitsval.mx = 1;
            s_lcd_madc.bitsval.my = 0;
            s_lcd_madc.bitsval.mv = 1;
            break;
        case LCD_DIRECT_ROT_180:
            s_lcd_madc.bitsval.mx = 1;
            s_lcd_madc.bitsval.my = 1;
            s_lcd_madc.bitsval.mv = 0;
            break;
        case LCD_DIRECT_ROT_270:
            s_lcd_madc.bitsval.mx = 0;
            s_lcd_madc.bitsval.my = 1;
            s_lcd_madc.bitsval.mv = 1;
            break;
        case LCD_DIRECT_MIR_H:
            s_lcd_madc.bitsval.mx = 1;
            s_lcd_madc.bitsval.my = 0;
            s_lcd_madc.bitsval.mv = 0;
            break;
        case LCD_DIRECT_MIR_V:
            s_lcd_madc.bitsval.mx = 0;
            s_lcd_madc.bitsval.my = 1;
            s_lcd_madc.bitsval.mv = 0;
            break;
        case LCD_DIRECT_MIR_HV:
            s_lcd_madc.bitsval.mx = 1;
            s_lcd_madc.bitsval.my = 1;
            s_lcd_madc.bitsval.mv = 0;
            break;
        default:
            s_lcd_madc.bitsval.mx = 0;
            s_lcd_madc.bitsval.my = 0;
            s_lcd_madc.bitsval.mv = 0;
            break;
    }

    s_lcd_madc.bitsval.mx = ~s_lcd_madc.bitsval.mx;
    s_lcd_madc.bitsval.rgb = 1;
    LCD_CtrlWrite_ST7789p3(0x36);
    LCD_DataWrite_ST7789p3(s_lcd_madc.dataval);
    LCD_CtrlWrite_ST7789p3(0x2c);
    return ERR_LCD_NONE;

}

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
//	Global resource dependence: 
//	Note:
/******************************************************************************/
LOCAL void ST7789p3_set_display_window(
    uint16 left, // start Horizon address
    uint16 top, // start Vertical address
    uint16 right, // end Horizon address
    uint16 bottom // end Vertical address
    )
{   
    uint16 newleft   = left + ST7789P3_X_OFFSET ; 
    uint16 newtop    = top + ST7789P3_Y_OFFSET;
    uint16 newright  = right+ ST7789P3_X_OFFSET ; 
    uint16 newbottom = bottom+ ST7789P3_Y_OFFSET;
    //SCI_TraceLow_Test("lcd: [DRV_LCD]ST7789p3_set_display_window L = %d, top = %d, R = %d, bottom = %d",left,top,right,bottom);
   gouda_event(0x90001102);
#if 0
//hal_GoudaChangeSerialMode(HAL_GOUDA_SPI_LINE_3);
    LCD_CtrlWrite_ST7789p3(0x2a);	// set hori start , end (left, right)
    LCD_DataWrite_ST7789p3((newleft>>8)&0xff); // left high 8 b
    LCD_DataWrite_ST7789p3(newleft&0xff); // left low 8 b
    LCD_DataWrite_ST7789p3((newright>>8)&0xff); // right high 8 b
    LCD_DataWrite_ST7789p3(newright&0xff); // right low 8 b

    LCD_CtrlWrite_ST7789p3(0x2b);	// set vert start , end (top, bot)
    LCD_DataWrite_ST7789p3((newtop>>8)&0xff); // top high 8 b
    LCD_DataWrite_ST7789p3(newtop&0xff); // top low 8 b
    LCD_DataWrite_ST7789p3((newbottom>>8)&0xff); // bot high 8 b
    LCD_DataWrite_ST7789p3(newbottom&0xff); // bot low 8 b
    LCD_CtrlWrite_ST7789p3(0x2c);	 // recover memory write mode
//hal_GoudaChangeSerialMode(HAL_GOUDA_SPI_LINE_3_2_LANE);
#else
	//SSD1963
	LCD_CtrlWrite_ST7789p3(0x2a);   
	LCD_DataWrite_ST7789p3(newleft>>8);
	LCD_DataWrite_ST7789p3(newleft);
	LCD_DataWrite_ST7789p3(newright>>8);
	LCD_DataWrite_ST7789p3(newright);

	LCD_CtrlWrite_ST7789p3(0x2b);   
	LCD_DataWrite_ST7789p3(newtop>>8);
  LCD_DataWrite_ST7789p3(newtop);
	LCD_DataWrite_ST7789p3(newbottom>>8);
	LCD_DataWrite_ST7789p3(newbottom);

	LCD_CtrlWrite_ST7789p3(0x2c);
#endif
}

/**************************************************************************************/
// Description: initialize all LCD with LCDC MCU MODE and LCDC mcu mode
// Global resource dependence:
// Author: Jianping.wang
// Note:
/**************************************************************************************/
LOCAL void ST7789p3_Init(void)
{
#if 1

	LCD_CtrlWrite_ST7789p3(0x0011); 		
	hal_LcdDelay(300);

	LCD_CtrlWrite_ST7789p3(0x00b1); 		
    	LCD_DataWrite_ST7789p3(0x0002);
  	LCD_DataWrite_ST7789p3(0x0035);
	LCD_DataWrite_ST7789p3(0x0036);

	LCD_CtrlWrite_ST7789p3(0x00b2); 		
    	LCD_DataWrite_ST7789p3(0x0002);
	LCD_DataWrite_ST7789p3(0x0035);
	LCD_DataWrite_ST7789p3(0x0036);

	LCD_CtrlWrite_ST7789p3(0x00b3); 		
    	LCD_DataWrite_ST7789p3(0x0002);
	LCD_DataWrite_ST7789p3(0x0035);
	LCD_DataWrite_ST7789p3(0x0036);
    	LCD_DataWrite_ST7789p3(0x0002);
	LCD_DataWrite_ST7789p3(0x0035);
	LCD_DataWrite_ST7789p3(0x0036);



	LCD_CtrlWrite_ST7789p3(0x00b4); 		
    	LCD_DataWrite_ST7789p3(0x0000);



	LCD_CtrlWrite_ST7789p3(0x00c0); 		
    	LCD_DataWrite_ST7789p3(0x00a2);
	LCD_DataWrite_ST7789p3(0x0002);
	LCD_DataWrite_ST7789p3(0x0084);

	
	LCD_CtrlWrite_ST7789p3(0x00c1); 		
    	LCD_DataWrite_ST7789p3(0x00c5);


	LCD_CtrlWrite_ST7789p3(0x00c2); 		
    	LCD_DataWrite_ST7789p3(0x000d);
	LCD_DataWrite_ST7789p3(0x0000);


  	LCD_CtrlWrite_ST7789p3(0x00c3); 		
    	LCD_DataWrite_ST7789p3(0x008a);
	LCD_DataWrite_ST7789p3(0x002a);



	LCD_CtrlWrite_ST7789p3(0x00c4); 		
    	LCD_DataWrite_ST7789p3(0x008d);
	LCD_DataWrite_ST7789p3(0x00ee);




   	LCD_CtrlWrite_ST7789p3(0x00c5); 		
    	LCD_DataWrite_ST7789p3(0x0012);



  	LCD_CtrlWrite_ST7789p3(0x0036); 		
    	LCD_DataWrite_ST7789p3(0x00c8);




 	LCD_CtrlWrite_ST7789p3(0x00e0); 		
    	LCD_DataWrite_ST7789p3(0x0012);
	LCD_DataWrite_ST7789p3(0x001c);
	LCD_DataWrite_ST7789p3(0x0010);
	LCD_DataWrite_ST7789p3(0x0018);
	LCD_DataWrite_ST7789p3(0x0033);
	LCD_DataWrite_ST7789p3(0x002c);
	LCD_DataWrite_ST7789p3(0x0025);
	LCD_DataWrite_ST7789p3(0x0028);
	LCD_DataWrite_ST7789p3(0x0028);
	LCD_DataWrite_ST7789p3(0x0027);
	LCD_DataWrite_ST7789p3(0x002f);
	LCD_DataWrite_ST7789p3(0x003c);
	LCD_DataWrite_ST7789p3(0x0000);
	LCD_DataWrite_ST7789p3(0x0003);
	LCD_DataWrite_ST7789p3(0x0003);
	LCD_DataWrite_ST7789p3(0x0010);
	hal_LcdDelay(120);

 	LCD_CtrlWrite_ST7789p3(0x00e1); 		
    	LCD_DataWrite_ST7789p3(0x0012);
	LCD_DataWrite_ST7789p3(0x001c);
	LCD_DataWrite_ST7789p3(0x0010);
	LCD_DataWrite_ST7789p3(0x0018);
	LCD_DataWrite_ST7789p3(0x002d);
	LCD_DataWrite_ST7789p3(0x0028);
	LCD_DataWrite_ST7789p3(0x0023);
	LCD_DataWrite_ST7789p3(0x0028);
	LCD_DataWrite_ST7789p3(0x0028);
	LCD_DataWrite_ST7789p3(0x0026);
	LCD_DataWrite_ST7789p3(0x002f);
	LCD_DataWrite_ST7789p3(0x003b);
	LCD_DataWrite_ST7789p3(0x0000);
	LCD_DataWrite_ST7789p3(0x0003);
	LCD_DataWrite_ST7789p3(0x0003);
	LCD_DataWrite_ST7789p3(0x0010);


	LCD_CtrlWrite_ST7789p3(0x003a); 		
    	LCD_DataWrite_ST7789p3(0x0005);

  	LCD_CtrlWrite_ST7789p3(0x2a);
	LCD_DataWrite_ST7789p3(0x00);
	LCD_DataWrite_ST7789p3(0x02);
	LCD_DataWrite_ST7789p3(0x00);
	LCD_DataWrite_ST7789p3(0x81);

	LCD_CtrlWrite_ST7789p3(0x2b);
	LCD_DataWrite_ST7789p3(0x00);
	LCD_DataWrite_ST7789p3(0x03);
	LCD_DataWrite_ST7789p3(0x00);
	LCD_DataWrite_ST7789p3(0xa2);


   

	LCD_CtrlWrite_ST7789p3(0x29); 

 	LCD_CtrlWrite_ST7789p3(0x2C);

    hal_LcdDelay(20);

#endif
}


/******************************************************************************/
//  Description:   Enter/Exit sleep mode .
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:
/******************************************************************************/
LOCAL ERR_LCD_E  ST7789p3_EnterSleep(
    BOOLEAN is_sleep //SCI_TRUE: enter sleep mode;SCI_FALSE:exit sleep mode.
    )
{
     SCI_TraceLow("lcd: ST7789p3_EnterSleep, is_sleep = %d", is_sleep);
   gouda_event(0x90001104);

    if(is_sleep)
    {
        gouda_event(0x900f2dc4);
        LCD_CtrlWrite_ST7789p3(0x28); //display off
        hal_LcdDelay(120);
        LCD_CtrlWrite_ST7789p3(0x10); // enter sleep mode
        //hal_LcdDelay(150);
        //_GoudaClkDisable();
       //  LDO_TurnOffLDO(LDO_LDO_LCD);
        
    }
    else
    {
        gouda_event(0x9f0f2dd4);
#if 1// wuxx add. ��Ҫ����,��Ȼ���ܵ���,Ϩ����������// 20231014
        LCD_CtrlWrite_ST7789p3(0x11); // sleep out mode
        hal_LcdDelay(120);
        LCD_CtrlWrite_ST7789p3(0x29); // display on
#endif
        ST7789p3_Init();
    }

    return ERR_LCD_NONE;
}

/******************************************************************************/
//  Description:   Close the lcd.(include sub lcd.)
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:
/******************************************************************************/
LOCAL void ST7789p3_Close(void)
{
     SCI_TraceLow("lcd: LCD: in GC9304_Close");
	   gouda_event(0x90001105);

    ST7789p3_EnterSleep( SCI_TRUE );
}

/******************************************************************************/
//  Description:   invalidate a rectang of in LCD
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:
/******************************************************************************/
LOCAL ERR_LCD_E ST7789p3_Invalidate(void)
{
   gouda_event(0x90001106);

    ST7789p3_set_display_window (0x0, 0x0, LCD_WIDTH - 1, LCD_HEIGHT -1);
    return ERR_LCD_NONE;
}

/******************************************************************************/
//  Description:   Copy a retangle data from clcd_buffer to display RAM.
//                     then the rectangle display is to be refreshed
//	Global resource dependence: 
//  Author:         Jim.zhang
//	Note:       
//     To improve speed, lcd is operate in HIGH SPEED RAM WRITE MODE(4
//     uint16 are write continuously always.) So, some dummy uint16 
//     should be inserted to satisfy this mode.   Please refer to spec.
/******************************************************************************/
PUBLIC ERR_LCD_E ST7789p3_InvalidateRect(
    uint16 left, //the left value of the rectangel
    uint16 top, //top of the rectangle
    uint16 right, //right of the rectangle
    uint16 bottom //bottom of the rectangle
    )
{
   gouda_event(0x90001107);

    left = (left >=LCD_WIDTH) ? LCD_WIDTH-1 : left;
    right = (right >= LCD_WIDTH) ? LCD_WIDTH-1 : right;
    top = (top >= LCD_HEIGHT) ? LCD_HEIGHT-1 : top;
    bottom = (bottom >= LCD_HEIGHT) ? LCD_HEIGHT-1 : bottom;

    if ( ( right < left ) || ( bottom < top ) )
    {
        return ERR_LCD_OPERATE_FAIL;
    }     

    ST7789p3_set_display_window(left, top, right, bottom);
    return ERR_LCD_NONE;
}

LOCAL ERR_LCD_E ST7789p3_RotationInvalidateRect(uint16 left,uint16 top,uint16 right,uint16 bottom,LCD_ANGLE_E angle)
{
   gouda_event(0x90001108);

    SCI_TraceLow_Test(" ST7789p3_RotationInvalidateRect");
    switch(angle)
    {
        case LCD_ANGLE_0:
            ST7789p3_set_display_window(left, top, right, bottom);
            break;
        case LCD_ANGLE_90:
            ST7789p3_set_display_window(left, top, bottom,right);
            break;
        case LCD_ANGLE_180:
            ST7789p3_set_display_window(left, top, right, bottom);
            break;
        case LCD_ANGLE_270:
            ST7789p3_set_display_window(left, top, bottom,right);
            break;
        default:
            ST7789p3_set_display_window(left, top, right, bottom);
            break;
    }
    return ERR_LCD_NONE;
}

LOCAL ERR_LCD_E   ST7789p3_SetContrast(uint16  contrast)
{
   gouda_event(0x90001109);

    SCI_TraceLow_Test(" ST7789p3_SetContrast");
    return ERR_LCD_FUNC_NOT_SUPPORT;
} 
/*****************************************************************************/
LOCAL ERR_LCD_E   ST7789p3_SetBrightness(uint16 brightness)
{
    SCI_TraceLow_Test(" ST7789p3_SetBrightness");
	   gouda_event(0x9000110a);

    return ERR_LCD_FUNC_NOT_SUPPORT;
}

//  Description:    Enable lcd to partial display mode, so can save power.
//	Global resource dependence: 
//  Author:         Jim.zhang
//  Return:         SCI_TRUE:SUCCESS ,SCI_FALSE:failed.
//	Note:           If all input parameters are 0, exit partial display mode.
/*****************************************************************************/
LOCAL ERR_LCD_E ST7789p3_SetDisplayWindow(
    uint16 left, //left of the window
    uint16 top, //top of the window
    uint16 right, //right of the window
    uint16 bottom //bottom of the window
    )
{
   gouda_event(0x9000110b);

    SCI_TraceLow_Test(" ST7789p3_SetDisplayWindow");
    ST7789p3_set_display_window(left, top, right, bottom);
    return ERR_LCD_NONE;
}

LOCAL void ST7789p3_InvalidatePixel(uint16 x, uint16 y, uint32 data)
{
    SCI_TraceLow_Test(" ST7789p3_InvalidatePixel");
	   gouda_event(0x9000110c);

    ST7789p3_InvalidateRect(x,y,x,y);
    LCD_DataWrite_ST7789p3(data);
}

LOCAL void ST7789p3_GetConfig(void)
{
	uint32 count,k;
	LCM_CFG_INFO_T_PTR cfg_ptr = PNULL;
	cfg_ptr = LCM_GetCfgInfo(0,&count);
	for(k=0;k<count;k++)
	{
		if(cfg_ptr[k].dev_id==LCD_DRV_ID_ST7789V2)
		{
			st7789p3_line_mode = cfg_ptr[k].adc_min;
			st7789p3_farmk = cfg_ptr[k].adc_max;
			
			break;
		}
	}
    SCI_TraceLow("lcd: ST7789p3_GetConfig LCM: g_two_date = %d , fmark =%d\n",st7789p3_line_mode,st7789p3_farmk);  //0x80,0xf5,0x3e
}
uint32 ST7789p3_Id[4] = {0};
LOCAL uint32 ST7789p3_ReadID(uint16 lcd_cs, uint16 lcd_cd, uint16 lcd_id)
{
    uint8 id[4] = {0};
	uint8 i = 0;
	uint32 idlcd=0;
	ST7789p3_GetConfig();
    hal_gouda_sim_init(st7789p3_line_mode,st7789p3_farmk,0x10000);
	mon_Event(0x990b7789);

    hal_LcdDelay(140);
    hwp_analogReg->pad_spi_lcd_cfg2 |= ANALOG_REG_PAD_SPI_LCD_SIO_SPU;
    hal_LcdDelay(10);
    hal_GoudaReadData(0x04,id,4);
    hwp_analogReg->pad_spi_lcd_cfg2 =  hwp_analogReg->pad_spi_lcd_cfg2&(~ANALOG_REG_PAD_SPI_LCD_SIO_SPU);
    hal_LcdDelay(10);
    SCI_TraceLow("lcd: ST7789vx_ReadID: id1->0x%0x, id2->0x%x, id3->0x%0x\n",id[3], id[2], id[1] );
    //id[3]=id1   id[2]=id2    id[1]=id3
	
    mon_Event((id[3]<<16)|(id[2] <<8)| id[1]);

    hal_gouda_term();
	idlcd = ((id[3]<<16)|((id[2] <<8)| id[1]));
	mon_Event(idlcd);

	for (i = 0; i < 4; i++) {
		ST7789p3_Id[i] = id[i];
	}	
	/*if((idlcd == LCD_DRV_ID_ST7789V2)||(idlcd == LCD_DRV_ID_ST7789V3)||(idlcd == LCD_DRV_ID_ST7789V4)||(LCD_DRV_ID_ST7789V5==idlcd))
		{
			idlcd = LCD_DRV_ID_ST7789V2;
			
		}*/
	idlcd = LCD_DRV_ID_ST7789P3;// FIH need to support all other LDDs,if id is not exit.
	
    return idlcd;
}

const LCD_OPERATIONS_T ST7789p3_operations = 
{
    ST7789p3_Init,
    ST7789p3_EnterSleep,
    ST7789p3_SetContrast,
    ST7789p3_SetBrightness,
    ST7789p3_SetDisplayWindow,
    ST7789p3_InvalidateRect,
    ST7789p3_Invalidate,
    ST7789p3_Close,
    ST7789p3_RotationInvalidateRect,
    ST7789p3_SetDirection,
    NULL, 
    ST7789p3_ReadID	
};

LOCAL const LCD_TIMING_U s_ST7789p3_lcmtiming =
{
    // LCM_CYCLE_U start(ns)
    5,  // CS setup time for LCM read (optional)  
    150, // low pulse width for LCM read (according spec)	
    150, // high pulse width for LCM read (according spec)
    10,   // CS setup time for LCM write  (optional) 	
    50,  // low pulse width for LCM write (according spec)
    50,  // high pulse width for LCM write (according spec)	
    // LCM_CYCLE_U end(ns)
};

LOCAL const LCD_TIMING_U s_ST7789p3_spitiming =
{
#ifdef FPGA_VERIFICATION
    39000000,
#else
    52000000,
#endif
    0,0,
/*    SPI_CLK_52MHZ,        // clk frequency support (unit:MHz)
    SPI_CLK_IDLE_LOW,     // CPOL: 0--SPI_CLK_IDLE_LOW, 1--SPI_CLK_IDLE_HIGH
    SPI_SAMPLING_RISING,  // CPHA: 0--SPI_SAMPLING_RISING,  1--SPI_SAMPLING_FALLING 
*/    
    8,                    // tx bit length: 8/16bits refer to lcm driver ic 
    0,
    0
};

#ifdef MAINLCM_INTERFACE_SPI
const LCD_SPEC_T g_lcd_ST7789p3 = 
{
    LCD_WIDTH, 
    LCD_HEIGHT, 
    SCI_NULL, 
    SCI_NULL,
    WIDTH_8,
    (LCD_TIMING_U*)&s_ST7789p3_spitiming, 
    (LCD_OPERATIONS_T*)&ST7789p3_operations,
    1,
    0
};
#else // not verify lcm
const LCD_SPEC_T g_lcd_ST7789p3 = 
{
    LCD_WIDTH, 
    LCD_HEIGHT, 
    LCD_MCU, 
    BUS_MODE_8080,
    WIDTH_8,
    (LCD_TIMING_U*)&s_ST7789p3_lcmtiming, 
    (LCD_OPERATIONS_T*)&ST7789p3_operations,
    1,
    0
};
#endif

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif 

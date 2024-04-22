#ifndef _ZDT_GPS_H
#define _ZDT_GPS_H

#ifdef   __cplusplus
extern   "C"
{
#endif

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "sc_reg.h"
#include "isr_drvapi.h"
#include "rtc_drvapi.h"
#ifndef WIN32
#include "sio.h"
#endif

#include "timer_drvapi.h"
#include "adc_drvapi.h"
#include "chip.h"
#include "sig_code.h"

#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231127
#include "zdt_gps_parse_nmea.h"
#else
#include "uart_drvapi.h"
#include "zdt_gps_uart.h"
#include "zdt_gps_parse_nmea.h"
#include "zdt_gps_parse_ubx.h"
#include "zdt_gps_parse_td.h"
#include "zdt_gps_parse_casic.h"
#include "zdt_gps_parse_allystar.h"
#endif

#define GPS_TRACE                 ZDT_GPSTrace

typedef struct
{
    SIGNAL_VARS
    uint8 *        str;
    uint32         len; 
} ZDT_GPS_SIG_T;

#ifdef ZDT_GPS_SUPPORT_AGPS
typedef struct 
{
    uint8  * eph_data;
    uint32 eph_times;
    uint32 eph_data_len;
}GPS_EPH_DATA_T;//调用WAP STACK时传入的参数结构

extern int ZDT_GPS_EphSetStart(void);
extern int ZDT_GPS_EphSetEnd(void);
extern BOOLEAN AGPS_Set_Start(uint32 delay_times);
extern BOOLEAN AGPS_Set_Next(void);
extern uint32 AGPS_Set_End(void);
extern void AGPS_TCP_Get(void);

extern GPS_EPH_DATA_T g_gps_eph_data;
extern BOOLEAN g_gps_eph_is_set;
extern BOOLEAN g_gps_eph_set_end_pwoff;
#else
extern BOOLEAN g_gps_eph_is_set;
#endif


#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231127
////sBOOLEAN g_gps_eph_is_set = FALSE; // add for compile

#else
extern BOOLEAN  APP_SendSigTo_GPS(ZDT_GPS_TASK_SIG_E sig_id, const char * data_ptr,uint32 data_len);

extern int ZDT_GPS_Init(void);
extern int ZDT_GPS_PowerOn(void);
extern int ZDT_GPS_PowerOff(void);
extern int ZDT_GPS_UartSend(uint8 * send_buf, uint32 send_len);
extern int ZDT_GPS_CheckHW_Start(void);
extern int ZDT_GPS_CheckHW_End(void);
extern BOOLEAN ZDT_GPS_Hw_IsOn(void);
extern void ZDT_GPS_Set_PowerNoOff(BOOLEAN no_off);

extern BOOLEAN gps_hw_is_ok(void);
extern BOOLEAN gps_hwic_type_is_td(void);
extern uint8        g_gps_data_valid_cnt;
#endif

#ifdef   __cplusplus
}
#endif

#endif/*_ZDT_GPS_H*/

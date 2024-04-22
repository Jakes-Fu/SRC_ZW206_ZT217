#ifndef _ZDT_GPS_NMEA_H
#define _ZDT_GPS_NMEA_H
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

#include "uart_drvapi.h"

#define  GPS_DATA_VALID_SUM 3
#define  GPS_NMEA_MAX_SVVIEW             32

typedef  enum
{
    ATRSP_TXT = 1,
    ATRSP_GGA,
    ATRSP_GLL,
    ATRSP_GSA,
    ATRSP_GPGSV,
    ATRSP_BDGSV,
    ATRSP_RMC,    
    ATRSP_VTG,
    ATRSP_ZDA,
    ATRSP_INF,
    ATRSP_MAX_AT
}GPS_ATCMD_TYPE_E;

typedef struct
{
    uint16      wAtId;
    void  (*CBFunc)  (uint8*, uint16);
} GPS_AT_MAP_TBL;

typedef  enum
{
    GPS_HWIC_TYPE_NULL,
    GPS_HWIC_TYPE_UBX,
    GPS_HWIC_TYPE_TD,
    GPS_HWIC_TYPE_CASIC,
    GPS_HWIC_TYPE_ALLYSTAR,
    GPS_HWIC_TYPE_MAX
}GPS_HWIC_TYPE_E;

#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
typedef struct GPS_DATA_t
{
    uint8 is_valid;
    float Latitude;
    uint8 Lat_Dir;
    float Longitude;
    uint8 Long_Dir;
    uint32 date;
    uint32 time;
    uint32 speed;  //运行速度
    uint16 hdop;	// 水平精度
    uint16 cog;     //方向角度
    uint8 sate_num;	// 卫星颗数
    uint32 altitude;    //海拔高度
    uint8 alt_Dir;          //海拔方向
    uint32 Lat_m; 
} GPS_DATA_T;

typedef struct GPS_GGA_DATA_t
{
    uint8 valid; //0:无效 1:定位有效6:估算模式
    uint32 time;
    float Latitude;
    uint8 Lat_Dir;
    float Longitude;
    uint8 Long_Dir;
    uint8 sate_num;
    uint16 hdop;
    uint32 altitude;
    uint8 alt_Dir;
} GPS_GGA_DATA_T;

#else
typedef struct GPS_DATA_t
{
    uint8 is_valid;
    uint32 Latitude;
    uint8 Lat_Dir;
    uint32 Longitude;
    uint8 Long_Dir;
    uint32 date;
    uint32 time;
    uint32 speed;  //运行速度
    uint16 hdop;	// 水平精度
    uint16 cog;     //方向角度
    uint8 sate_num;	// 卫星颗数
    uint32 altitude;    //海拔高度
    uint8 alt_Dir;          //海拔方向
    uint32 Lat_m; 
} GPS_DATA_T;

typedef struct GPS_GGA_DATA_t
{
    uint8 valid; //0:无效 1:定位有效6:估算模式
    uint32 time;
    uint32 Latitude;
    uint8 Lat_Dir;
    uint32 Longitude;
    uint8 Long_Dir;
    uint8 sate_num;
    uint16 hdop;
    uint32 altitude;
    uint8 alt_Dir;
} GPS_GGA_DATA_T;
#endif

typedef struct GPS_GSA_DATA_t
{
    uint8 sys_mode; // M: 手动A:自动
    uint8 sys_id;   // 1:  GPS 系统2:  GLONASS 系统4:  BDS 系统
    uint8 status;  // 1:无效2: 2D定位3: 3D定位
    uint16 pdop;
    uint16 hdop;
    uint16 vdop;
} GPS_GSA_DATA_T;

typedef struct GPS_GSV_DATA_t
{
    uint8 satellite_dbh_pos_num;
    uint8 satellite_dbh_valid_num;
    int     msg_sum;              /*total number of messages*/
    int     msg_index;            /*message number*/
    int     sates_in_view;        /*satellites in view*/
    int     max_snr;              /*Max snr*/
    int     min_snr;              /*Min snr*/
    struct
    {
        uint8 is_valid;
        uint8 sate_id;              /*satellite id*/
        uint8 elevation;            /*elevation in degrees*/
        int azimuth;              /*azimuth in degrees to true*/
        uint8 snr;                  /*SNR in dB*/
    }rsv[GPS_NMEA_MAX_SVVIEW];
} GPS_GSV_DATA_T;

#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
typedef struct GPS_RMC_DATA_t
{
    uint8 is_valid;// 0:无效1:有效
    uint32 date;
    uint32 time;
    float Latitude;
    uint32 Lat_m;
    uint8 Lat_Dir;
    float Longitude;
    uint8 Long_Dir;
    uint32 speed;
    uint16 cog;
} GPS_RMC_DATA_T;
#else
typedef struct GPS_RMC_DATA_t
{
    uint8 is_valid;// 0:无效1:有效
    uint32 date;
    uint32 time;
    uint32 Latitude;
    uint32 Lat_m;
    uint8 Lat_Dir;
    uint32 Longitude;
    uint8 Long_Dir;
    uint32 speed;
    uint16 cog;
} GPS_RMC_DATA_T;
#endif

typedef struct GPS_WORK_PARA_t
{
    uint16  open_time; // GPS开启时间
    uint16  search_time;  // GPS周期时间
    uint16  eph_time;	   // 星历下载时间 
    uint8    valid_sate_num;
    uint8    valid_gsv_dbh_val;
    uint16  valid_hdop;
} GPS_WORK_PARA_T;

extern const char* g_gps_AtStr[ATRSP_MAX_AT];
extern void GPS_SetDefault_Param(void);
extern uint32 GPS_Data_Get_D(uint32 data);
extern uint32 GPS_Data_Get_M(uint32 data);
#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
extern uint16 GPS_Data_Get_StrLong(uint8 dir,float Longitude,char * buf);
extern BOOLEAN GPS_Data_Get_StrLong_M(uint32 Longitude,char * buf);
extern uint16 GPS_Data_Get_StrLat(uint8 dir,float Latitude,char * buf);
extern BOOLEAN GPS_Data_Get_StrLat_M(uint32 Latitude,uint32 Lat_m,char * buf);
extern BOOLEAN GPS_Data_Get_StrLongD(uint32 Longitude,char * buf);
extern BOOLEAN GPS_Data_Get_StrLatD(uint32 Latitude,uint32 Lat_m,char * buf);
extern BOOLEAN GPS_Data_Get_StrCogD(uint16 cog,char * buf);
extern BOOLEAN GPS_Data_Get_StrSpeed(uint32 speed,char * buf);
extern BOOLEAN GPS_Data_Get_StrTime(uint32 time,char * buf);
extern BOOLEAN GPS_Data_Get_StrDate(uint32 date,char * buf);
extern uint16 GPS_Data_Get_WStrLong(uint8 dir,uint32 Longitude,uint16 * wbuf);
extern uint16 GPS_Data_Get_WStrLat(uint8 dir,uint32 Latitude,uint16 * wbuf);
extern uint16 GPS_Data_Get_WStrLocalDateTime(uint32 date,uint32 time,uint16 * wbuf);

extern uint8 GPS_UDP_Get_StrDateTime(uint8 * pData,uint32 date,uint32 time);
extern uint8 GPS_UDP_Get_GPSLat(uint8 * pData,uint32 Latitude);
extern uint8 GPS_UDP_Get_GPSLong(uint8 * pData,uint32 Longitude);
extern uint8 GPS_UDP_Get_GPSSpeed(uint8 * pData,uint32 speed);
extern uint8 GPS_UDP_Get_GPSCog(uint8 * pData,uint16 cog);

#else
extern uint16 GPS_Data_Get_StrLong(uint8 dir,uint32 Longitude,char * buf);
extern BOOLEAN GPS_Data_Get_StrLong_M(uint32 Longitude,char * buf);
extern uint16 GPS_Data_Get_StrLat(uint8 dir,uint32 Latitude,char * buf);
extern BOOLEAN GPS_Data_Get_StrLat_M(uint32 Latitude,uint32 Lat_m,char * buf);
extern BOOLEAN GPS_Data_Get_StrLongD(uint32 Longitude,char * buf);
extern BOOLEAN GPS_Data_Get_StrLatD(uint32 Latitude,uint32 Lat_m,char * buf);
extern BOOLEAN GPS_Data_Get_StrCogD(uint16 cog,char * buf);
extern BOOLEAN GPS_Data_Get_StrSpeed(uint32 speed,char * buf);
extern BOOLEAN GPS_Data_Get_StrTime(uint32 time,char * buf);
extern BOOLEAN GPS_Data_Get_StrDate(uint32 date,char * buf);
extern uint16 GPS_Data_Get_WStrLong(uint8 dir,uint32 Longitude,uint16 * wbuf);
extern uint16 GPS_Data_Get_WStrLat(uint8 dir,uint32 Latitude,uint16 * wbuf);
extern uint16 GPS_Data_Get_WStrLocalDateTime(uint32 date,uint32 time,uint16 * wbuf);

extern uint8 GPS_UDP_Get_StrDateTime(uint8 * pData,uint32 date,uint32 time);
extern uint8 GPS_UDP_Get_GPSLat(uint8 * pData,uint32 Latitude);
extern uint8 GPS_UDP_Get_GPSLong(uint8 * pData,uint32 Longitude);
extern uint8 GPS_UDP_Get_GPSSpeed(uint8 * pData,uint32 speed);
extern uint8 GPS_UDP_Get_GPSCog(uint8 * pData,uint16 cog);
#endif

extern void HANDLE_GPSAT_TXT(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_RMC(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_GGA(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_GPGSV(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_BDGSV(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_VTG(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_GSA(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_GLL(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_MSS(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_ZDA(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_UBX(uint8 * str, uint16 len);
extern void HANDLE_GPSAT_INF(uint8 * str, uint16 len);

extern void GPS_API_ReceiveNEMA(uint8* pCmd, uint16 len);

extern int GPS_API_GetLastPos(GPS_DATA_T * last_pos);
extern int GPS_API_ClearLastPos();

extern int GPS_API_GetCurPos(GPS_DATA_T * last_pos);
extern int GPS_API_GetCurGSV(GPS_GSV_DATA_T * last_gsv);
extern int GPS_API_GetCurGPGSV(GPS_GSV_DATA_T * last_gsv);
extern int GPS_API_GetCurBDGSV(GPS_GSV_DATA_T * last_gsv);

#endif  // _ZDT_GPS_NMEA_H

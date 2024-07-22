
#include "zdt_app.h"
#ifdef ZDT_GSENSOR_SUPPORT
#include "zdt_gsensor.h"
#endif
#include "mmialarm_export.h"

#ifdef ZTE_WEATHER_SUPPORT
#include "zteweather_main.h"
#endif
#include "watch_common.h"

#define z_abs(x)  ((x) >= 0 ? (x) : (-(x)))
#ifdef ZDT_PLAT_YX_SUPPORT
#if defined(ZDT_PLAT_YX_SUPPORT_FZD)
//防走丢后台
#define YX_TCP_LK_TYPE 0

#elif defined( ZDT_PLAT_YX_SUPPORT_YS)
//易赛后台 安康守护
#define YX_TCP_LK_TYPE 1

#elif defined( ZDT_PLAT_YX_SUPPORT_TX)
//童信后台百合
#define YX_TCP_LK_TYPE 0

#else
//防走丢后台
#define YX_TCP_LK_TYPE 0

#endif
#define YX_TCP_LK_TIME_OUT 600
#define YX_TCP_LK_TIME_OUT_PWON 300

#ifndef ONE_DAY_SECONDS
#define ONE_DAY_SECONDS        86400
#endif
#if !defined(ZDT_GPS_SUPPORT)&&!defined(ZDT_W217_FACTORY_GPS) // no define
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
    uint16 hdop;    // 水平精度
    uint16 cog;     //方向角度
    uint8 sate_num;    // 卫星颗数
    uint32 altitude;    //海拔高度
    uint8 alt_Dir;          //海拔方向
    uint32 Lat_m;          //精确到小数点后4位
} GPS_DATA_T;
#endif

int YX_Net_Reg(void);

static uint8 s_yx_is_first_need_stkq = 0;
static uint8 s_yx_is_first_need_tkq = 0;
static uint8 s_yx_is_first_need_tk2q = 0;
static uint8 s_yx_is_first_info_times = 0;
static BOOLEAN s_yx_is_first_get_all_info = FALSE;
static BOOLEAN s_yx_is_first_get_band_info = FALSE;// 默认绑定状态
static uint16 s_yx_is_vchat_tk_err_time = 0;
static uint16 s_yx_is_vchat_tk2_err_time = 0;
static uint16 s_yx_is_vchat_stk_err_time = 0;
static BOOLEAN s_yx_is_first_land = FALSE;
static uint16 g_yx_loc_timer_interval = 0;
static uint16 g_yx_loc_timer_lk_num = 0;
static uint16 g_yx_lk_timers_num = 0;
static char g_yx_net_call_number[41] = {0};
static char g_yx_net_monitorl_number[41] = {0};
static BOOLEAN g_is_yx_is_locing = FALSE;
static EVENT_CURRENT_DATA_T  s_yx_current_event_info = {0, 0, {0}};
static BOOLEAN is_auto_unbind = FALSE;

BOOLEAN g_is_yx_power_off_ing = FALSE;
BOOLEAN g_is_yx_czsb_is_playing = FALSE;

BOOLEAN g_yx_cur_heart_change = FALSE;
uint16 g_yx_cur_heart_time = YX_TCP_LK_TIME_OUT_PWON;

extern uint8 is_cdma_apn_mode;
uint16 g_yx_power_on_time_min = 0;

YX_WEATHER_INFO_T g_yx_wt_info = {0};
BOOLEAN g_yx_wt_recieve_ok = FALSE;
uint8 g_yx_wt_oneday_start = 0;

#ifdef ZDT_PLAT_YX_SUPPORT_YS_AES
#define YX_RCV_DATA_MAX_LEN 4096

static uint8 yx_cmd_line_buffer[YX_RCV_DATA_MAX_LEN];     /*  buffer for AT cmd line  */
static uint32 yx_cmd_line_len = 0;
static uint32 yx_data_need_len = 0;
static uint8 *yx_cmd_ptr;                     /* next loc in buffer       */
static uint8 yx_cmd_status = 0;
uint8 * yx_tcp_rx_data_buf = NULL;
uint8  g_yx_imei_len = ZDT_IMEI_LEN;

uint8  g_yx_iv_str[17] = {0};

uint8  YX_RCV_CMD_ProcessOneChar(uint8 cc)
{
    uint8 res = 0;
    switch ( yx_cmd_status )
    {
        case 0:
            /*-----------------------------------------------------------------
              In the HUNT state, the characters 'A' or 'a' are looked for.  
              Everything else is ignored. If 'A' or 'a' is detected, the   
              machine goes into the FOUND_A state.                              
            -----------------------------------------------------------------*/
            if (cc == YX_NET_MSG_START)
            {
                yx_cmd_ptr = &yx_cmd_line_buffer[0];
                *yx_cmd_ptr++ = cc;
                yx_cmd_line_len = 1;
                yx_cmd_status = 1;
            }
            break;
            
        case 1:
            {
                if ( yx_cmd_ptr >= (yx_cmd_line_buffer + YX_RCV_DATA_MAX_LEN - 2) )
                {
                    yx_cmd_status = 0;
                    yx_cmd_ptr[0] = '\0';
                    break;
                }
                /*------------------------------------------------------------
                  If no overflow, fill buffer.  
                ------------------------------------------------------------*/
                else
                {
                    *yx_cmd_ptr++ = cc;
                    yx_cmd_line_len++;
                }

                if(cc == YX_NET_MSG_END)
                {
                    res = 1;
                    yx_cmd_status = 0;
                }
            }
            break;
    }  /* end switch ( at_cmd_prep_state ) */

    return res;
}

static void YX_RCV_CMD_Reset(void)
{
    yx_cmd_status = 0;
    return;
}

static uint32 YX_RCV_CMD_Check(uint8 * pData, uint32 Data_len)
{
    return 0;
}

static uint32 YX_CMD_CheckAll(uint8 * pData, int Data_len,uint8 **ppCmdBuf, uint32 * cmd_len)
{
    uint32 end_len = 0;
    uint32 i = 0;
    uint8 res = 0;
    uint8 * pTmp = PNULL;
    if(pData == PNULL || Data_len <= 0)
    {
        return 0;

    }
    end_len = YX_RCV_CMD_Check(pData,Data_len);
    if(end_len > 0)
    {
        YX_RCV_CMD_Reset();
        pTmp = (uint8 *)SCI_ALLOC_APPZ(end_len);
        if(pTmp == NULL)
        {
            ZDT_LOG("YX_CMD_CheckAll Err Mem 1");
            return 0;
        }
        SCI_MEMCPY(pTmp,pData,end_len);
        *ppCmdBuf = pTmp;
        *cmd_len = end_len;
        return end_len;
    }
    else
    {
        for(i = 0; i < Data_len; i++)
        {
            res = YX_RCV_CMD_ProcessOneChar(pData[i]);
            if(res > 0)
            {
                uint8 * decrypt_char;
                uint32 decrypt_len = 0;
                ZDT_TCPRcvTrace(yx_cmd_line_buffer,yx_cmd_line_len);
                if(yx_cmd_line_buffer[3] == '*')
                {
                    //不需要解密
                    pTmp = (uint8 *)SCI_ALLOC_APPZ(yx_cmd_line_len);
                    if(pTmp == NULL)
                    {
                        return 0;
                    }
                    SCI_MEMCPY(pTmp,yx_cmd_line_buffer,yx_cmd_line_len);
                    ZDT_TCPRcvTrace(yx_cmd_line_buffer,yx_cmd_line_len);
                    *ppCmdBuf = pTmp;
                    *cmd_len = yx_cmd_line_len;
                }
                else
                {
                    decrypt_char = YX_Func_DecryptByIV(yx_cmd_line_buffer+1,yx_cmd_line_len-2,&decrypt_len);
                    if(decrypt_char == PNULL)
                    {
                        ZDT_LOG("YX_CMD_CheckAll Err YX_Func_Decrypt");
                        return 0;
                   }
                    //ZDT_LOG("[TCP] Rcv %d: %s",decrypt_len,decrypt_char);
                    ZDT_TCPRcvTrace(decrypt_char,decrypt_len);
                    //ZDT_TCPRcvTraceU8(decrypt_char,decrypt_len);
                    *ppCmdBuf = decrypt_char;
                    *cmd_len = decrypt_len;
                }
                end_len = i+1;
                break;
            }
        }
    }
    return end_len;
}
#else
#define YX_RCV_DATA_MAX_LEN 2048

static uint8 yx_cmd_line_buffer[YX_RCV_DATA_MAX_LEN];     /*  buffer for AT cmd line  */
static uint32 yx_cmd_line_len = 0;
static uint32 yx_data_need_len = 0;
static uint32 yx_data_head_len = 0;
static uint8 *yx_cmd_ptr;                     /* next loc in buffer       */
static uint8 yx_cmd_status = 0;
uint8 * yx_tcp_rx_data_buf = NULL;
uint8  g_yx_imei_len = 15;

extern uint8 YX_MMI_Get_Bingd_Statues(void);
extern void YX_MMI_Set_Bingd_Statues(uint8 statues);

uint8  YX_RCV_CMD_ProcessOneChar(uint8 cc)
{
    uint8 res = 0;
    uint16 head_len = g_yx_imei_len+10;
    switch ( yx_cmd_status )
    {
        case 0:
            /*-----------------------------------------------------------------
              In the HUNT state, the characters 'A' or 'a' are looked for.  
              Everything else is ignored. If 'A' or 'a' is detected, the   
              machine goes into the FOUND_A state.                              
            -----------------------------------------------------------------*/
            if (cc == YX_NET_MSG_START)
            {
                yx_cmd_ptr = &yx_cmd_line_buffer[0];
                *yx_cmd_ptr++ = cc;
                yx_cmd_line_len = 1;
                yx_cmd_status = 1;
                yx_data_need_len = 0;
                yx_data_head_len = head_len;
            }
            break;

        case 1: 
            {
                //X
                //if ( cc == YX_NET_MSG_RCV_CS_1 )
                if(1)
                {
                    *yx_cmd_ptr++ = cc;
                    yx_cmd_line_len++;
                    yx_cmd_status = 2;
                    yx_data_need_len = 0;
                }
                else
                {
                    yx_cmd_status = 0;        
                }
            }
            break;
            
        case 2: 
            {
                //Y
                //if ( cc == YX_NET_MSG_RCV_CS_2 )
                if(1)
                {
                    *yx_cmd_ptr++ = cc;
                    yx_cmd_line_len++;
                    yx_cmd_status = 3;
                    yx_data_need_len = 0;
                }
                else
                {
                    yx_cmd_status = 0;        
                }
            }
            break;
            
        case 3: 
            {
                //
                if ( cc == '*' )
                {
                    *yx_cmd_ptr++ = cc;
                    yx_cmd_line_len++;
                    yx_cmd_status = 4;
                    yx_data_need_len = head_len;
                    yx_data_head_len = head_len;
                }
                else
                {
                    *yx_cmd_ptr++ = cc;
                    yx_cmd_line_len++;
                    yx_cmd_status = 4;
                    head_len += 32;
                    yx_data_need_len = head_len;
                    yx_data_head_len = head_len;
                }
            }
            break;
            
        case 4:
            {
                if ( yx_cmd_ptr >= (yx_cmd_line_buffer + YX_RCV_DATA_MAX_LEN - 2) )
                {
                    yx_cmd_status = 0;
                    yx_cmd_ptr[0] = '\0';
                    break;
                }
                /*------------------------------------------------------------
                  If no overflow, fill buffer.  
                ------------------------------------------------------------*/
                else
                {
                    *yx_cmd_ptr++ = cc;
                    yx_cmd_line_len++;
                }
                
                if(yx_cmd_line_len == yx_data_head_len)
                {                                
                    uint16 content_len = 0;
                    if ( yx_cmd_line_buffer[yx_data_head_len-6] != '*' ||  yx_cmd_line_buffer[yx_data_head_len-1] != '*')
                    {
                        yx_cmd_status = 0;
                        yx_cmd_ptr[0] = '\0';
                        break;
                    }
                    content_len = YX_Func_HexCharToInt(yx_cmd_line_buffer[yx_data_head_len-5]);
                    content_len = content_len<<4;
                    content_len += YX_Func_HexCharToInt(yx_cmd_line_buffer[yx_data_head_len-4]);
                    content_len = content_len<<4;
                    content_len += YX_Func_HexCharToInt(yx_cmd_line_buffer[yx_data_head_len-3]);
                    content_len = content_len<<4;
                    content_len += YX_Func_HexCharToInt(yx_cmd_line_buffer[yx_data_head_len-2]);
                    yx_data_need_len = content_len + yx_data_head_len + 1;
                    if ( yx_data_need_len > YX_RCV_DATA_MAX_LEN )
                    {
                        if(yx_tcp_rx_data_buf != NULL)
                        {
                            SCI_FREE(yx_tcp_rx_data_buf);
                            yx_tcp_rx_data_buf = NULL;
                        }
                        yx_tcp_rx_data_buf = (uint8 *)SCI_ALLOC_APPZ(yx_data_need_len+1);
                        if(yx_tcp_rx_data_buf == NULL)
                        {
                            TCP_LOG("TCP YX CMD ERR SCI_ALLOC_APPZ %d",yx_data_need_len+1);
                            yx_cmd_status = 0;
                            yx_cmd_ptr[0] = '\0';
                        }
                        else
                        {
                            yx_cmd_status = 5;
                            SCI_MEMCPY(yx_tcp_rx_data_buf,yx_cmd_line_buffer,yx_data_head_len);
                            yx_cmd_ptr = &yx_tcp_rx_data_buf[yx_data_head_len];
                        }
                        break;
                    }
                }
                else
                {
                    if(yx_cmd_line_len >= yx_data_need_len)
                    {
                        if(yx_cmd_line_buffer[yx_cmd_line_len-1] == YX_NET_MSG_END)
                        {
                            res = 1;
                            //yx_process_cmd_line(pMe,yx_cmd_line_buffer,yx_cmd_line_len,FALSE);
                        }
                        else
                        {
                            TCP_LOG("TCP YX CMD(%d) ERR End 0x%x",yx_data_need_len,yx_cmd_line_buffer[yx_cmd_line_len-1]);
                        }
                        yx_cmd_status = 0;
                    }
                }
            }

            break;
        case 5:
            {
                if ( yx_cmd_ptr >= (yx_tcp_rx_data_buf + yx_data_need_len) )
                {
                    yx_cmd_status = 0;
                    yx_cmd_ptr[0] = '\0';
                    break;
                }
                /*------------------------------------------------------------
                  If no overflow, fill buffer.  
                ------------------------------------------------------------*/
                else
                {
                    *yx_cmd_ptr++ = cc;
                    yx_cmd_line_len++;
                }

                if(yx_cmd_line_len >= yx_data_need_len)
                {
                    if(yx_tcp_rx_data_buf[yx_cmd_line_len-1] == YX_NET_MSG_END)
                    {
                        res = 2;
                        //yx_process_cmd_line(pMe,yx_tcp_rx_data_buf,yx_cmd_line_len,TRUE);
                        //yx_tcp_rx_data_buf = NULL;
                    }
                    else
                    {
                        TCP_LOG("TCP YX CMD(%d) ERR End 0x%x",yx_data_need_len,yx_tcp_rx_data_buf[yx_cmd_line_len-1]);
                        if(yx_tcp_rx_data_buf != NULL)
                        {
                            SCI_FREE(yx_tcp_rx_data_buf);
                            yx_tcp_rx_data_buf = NULL;
                        }
                    }
                    yx_cmd_status = 0;
                }
            }

            break;
    }  /* end switch ( at_cmd_prep_state ) */

    return res;
}

static void YX_RCV_CMD_Reset(void)
{
    yx_cmd_status = 0;
    return;
}

static uint32 YX_RCV_CMD_Check(uint8 * pData, uint32 Data_len)
{
    uint32 ContentLen = 0;
    uint32 head_len = g_yx_imei_len + 10;
    if(pData == NULL || Data_len < (head_len+1))
    {
        return 0;
    }
    
    if(pData[0] != YX_NET_MSG_START 
        //|| pData[1] != YX_NET_MSG_RCV_CS_1 
        //|| pData[2] != YX_NET_MSG_RCV_CS_2 
        || pData[3] != '*'
        || pData[head_len-6] != '*'
        || pData[head_len-1] != '*'
        )
    {
        return 0;
    }
    
    ContentLen = YX_Func_HexCharToInt(pData[head_len-5]);
    ContentLen = ContentLen<<4;
    ContentLen += YX_Func_HexCharToInt(pData[head_len-4]);
    ContentLen = ContentLen<<4;
    ContentLen += YX_Func_HexCharToInt(pData[head_len-3]);
    ContentLen = ContentLen<<4;
    ContentLen += YX_Func_HexCharToInt(pData[head_len-2]);
    
    if((ContentLen+head_len+1) > Data_len)
    {
        return 0;
    }
    
    
    if(YX_NET_MSG_END != pData[ContentLen+head_len])
    {
        return 0;
    }
        
    return ContentLen+head_len+1;
}

static uint32 YX_CMD_CheckAll(uint8 * pData, int Data_len,uint8 **ppCmdBuf, uint32 * cmd_len)
{
    uint32 end_len = 0;
    uint32 i = 0;
    uint8 res = 0;
    uint8 * pTmp = PNULL;
    if(pData == PNULL || Data_len <= 0)
    {
        return 0;
    }
    end_len = YX_RCV_CMD_Check(pData,Data_len);
    if(end_len > 0)
    {
        YX_RCV_CMD_Reset();
        pTmp = (uint8 *)SCI_ALLOC_APPZ(end_len);
        if(pTmp == NULL)
        {
            return 0;
        }
        SCI_MEMCPY(pTmp,pData,end_len);
        *ppCmdBuf = pTmp;
        *cmd_len = end_len;
        return end_len;
    }
    else
    {
        for(i = 0; i < Data_len; i++)
        {
            res = YX_RCV_CMD_ProcessOneChar(pData[i]);
            if(res > 0)
            {
                if(res == 2)
                {
                    *ppCmdBuf = yx_tcp_rx_data_buf;
                    *cmd_len = yx_cmd_line_len;
                     yx_tcp_rx_data_buf = NULL;
                }
                else
                {
                    pTmp = (uint8 *)SCI_ALLOC_APPZ(yx_cmd_line_len);
                    if(pTmp == NULL)
                    {
                        return 0;
                    }
                    SCI_MEMCPY(pTmp,yx_cmd_line_buffer,yx_cmd_line_len);
                    *ppCmdBuf = pTmp;
                    *cmd_len = yx_cmd_line_len;
                }
                end_len = i+1;
                break;
            }
        }
    }
    return end_len;
}
#endif

static int YX_Net_GetContentLen_Send(YX_APP_T *pMe,uint8 * pData,uint32 Data_len)
{
    uint16 MsgLen = 0;
    uint16 head_len = g_yx_imei_len + 10;
    #if FEATURE_YX_SEND_MD5
        head_len = g_yx_imei_len + 42;
    #endif
    if(pData == NULL || Data_len < (head_len+1))
    {
        ZDT_LOG("YX_Net_GetContentLen_Send Err NULL");
        return -1;
    }
    #if FEATURE_YX_SEND_MD5
        if(pData[0] != YX_NET_MSG_START 
            || pData[33] != YX_NET_MSG_SEND_CS_1 
            || pData[34] != YX_NET_MSG_SEND_CS_2
            || pData[35] != '*'
            || pData[head_len-6] != '*'
            || pData[head_len-1] != '*'
            )
        {
            ZDT_LOG("YX_Net_GetContentLen_Send Err Start");
            return -1;
        }

        MsgLen = YX_Func_HexCharToInt(pData[head_len-5]);
        MsgLen = MsgLen<<4;
        MsgLen += YX_Func_HexCharToInt(pData[head_len-4]);
        MsgLen = MsgLen<<4;
        MsgLen += YX_Func_HexCharToInt(pData[head_len-3]);
        MsgLen = MsgLen<<4;
        MsgLen += YX_Func_HexCharToInt(pData[head_len-2]);
    #else
    if(pData[0] != YX_NET_MSG_START 
        || pData[1] != YX_NET_MSG_SEND_CS_1 
        || pData[2] != YX_NET_MSG_SEND_CS_2 
        || pData[3] != '*'
        || pData[head_len-6] != '*'
        || pData[head_len-1] != '*'
        )
    {
        ZDT_LOG("YX_Net_GetContentLen_Send Err Start");
        return -1;
    }

    MsgLen = YX_Func_HexCharToInt(pData[head_len-5]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-4]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-3]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-2]);
    #endif
    
    if((MsgLen+head_len+1) > Data_len)
    {
        ZDT_LOG("YX_Net_GetContentLen_Send Err Len %d>%d",(MsgLen+head_len+1),Data_len);
        return -1;
   }
            
    return (int)MsgLen;
}

int YX_Net_GetContentLen_RCV(YX_APP_T *pMe,uint8 * pData,uint32 Data_len)
{
    uint16 MsgLen = 0;
    uint16 head_len = g_yx_imei_len + 10;
    
    if(pData == NULL || Data_len < (head_len+1))
    {
        ZDT_LOG("YX_Net_GetContentLen_RCV Err NULL");
        return -1;
    }
    
    if(pData[0] != YX_NET_MSG_START 
        //|| pData[1] != YX_NET_MSG_RCV_CS_1 
        //|| pData[2] != YX_NET_MSG_RCV_CS_2 
        || pData[3] != '*'
        || pData[head_len-6] != '*'
        || pData[head_len-1] != '*'
        )
    {
        ZDT_LOG("YX_Net_GetContentLen_RCV Err Start");
        return -1;
    }

    MsgLen = YX_Func_HexCharToInt(pData[head_len-5]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-4]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-3]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-2]);
    
    if((MsgLen+head_len+1) > Data_len)
    {
        ZDT_LOG("YX_Net_GetContentLen_RCV Err Len %d>%d",(MsgLen+head_len+1),Data_len);
        return -1;
   }
            
    return (int)MsgLen;
}

static uint16 YX_Net_Get_AL_CDMA_AlertType(YX_APP_T *pMe,uint8 * pCont,uint16 ContentLen)
{
    uint8 * pContent = pCont;
    uint32 content_len = ContentLen;
    char buf[101] = {0};
    int ret = 0;
    uint16 alert = 0;
    uint16 type = YX_NET_ALERT_TYPE_NULL;
    ret = YX_Func_GetPosPara(&pContent, &content_len,buf,100,15);
    if(ret > 4)
    {
        alert = YX_Func_HexCharToInt(buf[0]);
        alert = content_len<<4;
        alert += YX_Func_HexCharToInt(buf[1]);
        alert = content_len<<4;
        alert += YX_Func_HexCharToInt(buf[2]);
        alert = content_len<<4;
        alert += YX_Func_HexCharToInt(buf[3]);
        if((alert & 0x0001) > 0)
        {
            type = YX_NET_ALERT_TYPE_SOS;
        }
        else if((alert & 0x0002) > 0)
        {
            type = YX_NET_ALERT_TYPE_LOWBAT;
        }
        else if((alert & 0x0010) > 0)
        {
            type = YX_NET_ALERT_TYPE_LOSE;
        }
        else if((alert & 0x0020) > 0)
        {
            type = YX_NET_ALERT_TYPE_FALLDOWN;
        }
    }
    return type;
}

#if FEATURE_YX_SEND_MD5
void YX_Net_MD5(uint8 * pDate, uint16 data_len, char out_str[32])
{
    // 私钥 tJcZar4toZrCOfT9VDguc6h23Q4BGcaa
    char input_str[133] = {0};
    unsigned int input_len = 0;
    uint8 * pStr = pDate;
    uint32 len = data_len;
    int ret = 0;
    ret = YX_Func_GetNextPara(&pStr, &len, input_str, 100);
    if(ret > 0)
    {
        strcat(input_str,YX_NET_MD5_KEY);
        input_len = SCI_STRLEN(input_str);
        //ZDT_LOG("YX_Net_MD5: Input %s",input_str);
        YX_Func_MD5_Generate((uint8 *)input_str,input_len,out_str);
    }
    else
    {
        SCI_MEMCPY(out_str,YX_NET_MD5_KEY,32);
    }
 }
#endif

BOOLEAN         g_yx_is_land = FALSE;
BOOLEAN YX_Net_Is_Land(void)
{
    #ifdef WIN32
        return TRUE;
    #endif
    return  g_yx_is_land;
}

void YX_Net_SetIdleYXIcon(BOOLEAN is_disp)
{
    static BOOLEAN s_is_yx_icon_disp = FALSE;
    if(s_is_yx_icon_disp == is_disp)
    {
        return;
    }
    s_is_yx_icon_disp = is_disp;
    //MMIAPICOM_StatusAreaSetYxLandState(is_disp);
}
void YX_Net_Set_Land(BOOLEAN is_land)
{
    if(g_yx_is_land != is_land)
    {
        YX_Net_SetIdleYXIcon(is_land);
            if(is_land)
            {
                #ifdef ZDT_LED_SUPPORT
                    ZDT_Led_Start_Charge();
                #endif
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
                    MMIAPISET_PlayCustomRing(MMISET_CUST_NET_OK,1);
#endif
#endif
            }
            else
            {
                #ifdef ZDT_LED_SUPPORT
                    ZDT_Led_Start_ChargeOver();
                #endif
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
                MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NET,1);
#endif
#endif
            }
        g_yx_is_land = is_land;
    }
    return;
}

extern uint32 YX_Net_TCPSend(char * meid,uint8 * pContent,uint16 ContentLen,uint8 priority,uint8 repeat_num,uint32 timeout,TCPRCVHANDLER rcv_handle)
{
    ZDT_TCP_LINK_DATA_T * cur_data = NULL;
    uint16 len = 0;
    uint16 msg_len = 0;
    char len_str[5] = {0};
    uint8 meid_len = 0;
#if FEATURE_YX_SEND_MD5
    char md5_str[32] = {0};
#endif
    if(meid == NULL || ContentLen == 0 || pContent == NULL )
    {
        ZDT_LOG("YX_Net_TCPSend: ERR NULL");
        return 0;
    }
    
    meid_len = SCI_STRLEN(meid);

    cur_data=(ZDT_TCP_LINK_DATA_T *)SCI_ALLOC_APPZ(sizeof(ZDT_TCP_LINK_DATA_T));
    if(cur_data == NULL)
    {
        ZDT_LOG("YX_Net_TCPSend: ERR Malloc");
        return 0;
    }
#if FEATURE_YX_SEND_MD5
    msg_len = ContentLen +meid_len+43;
#else
    msg_len = ContentLen +meid_len+11;
#endif
    cur_data->str= (uint8 *)SCI_ALLOC_APPZ(msg_len);
    if(cur_data->str == NULL)
    {
        ZDT_LOG("YX_Net_TCPSend: ERR Malloc len=%d",msg_len);
        SCI_FREE(cur_data);
        return 0;
    }
    
    SCI_MEMSET(cur_data->str,0,msg_len);
#if FEATURE_YX_SEND_MD5
    //[XY*
    cur_data->str[0] = YX_NET_MSG_START;
    cur_data->str[33] = YX_NET_MSG_SEND_CS_1;
    cur_data->str[34] = YX_NET_MSG_SEND_CS_2;
    cur_data->str[35] = '*';
    //MEID
    SCI_MEMCPY(cur_data->str+36,meid,meid_len);
    len = meid_len + 36;
    cur_data->str[len++] = '*';
    
    //包长度
    sprintf(len_str,"%04X",ContentLen);
    SCI_MEMCPY(cur_data->str+len,len_str,4);
    len += 4;
    cur_data->str[len++] = '*';
    
    //数据
    SCI_MEMCPY(cur_data->str+len,pContent,ContentLen);
    len += ContentLen;

    YX_Net_MD5(cur_data->str+33,len-33,md5_str);
    SCI_MEMCPY(cur_data->str+1,md5_str,32);
    
    //结束]
    cur_data->str[len++] = YX_NET_MSG_END;
 #else
    //[XY*
    cur_data->str[0] = YX_NET_MSG_START;
    cur_data->str[1] = YX_NET_MSG_SEND_CS_1;
    cur_data->str[2] = YX_NET_MSG_SEND_CS_2;
    cur_data->str[3] = '*';
    
    //MEID
    SCI_MEMCPY(cur_data->str+4,meid,meid_len);
    len = meid_len + 4;
    cur_data->str[len++] = '*';
    
    //包长度
    sprintf(len_str,"%04X",ContentLen);
    SCI_MEMCPY(cur_data->str+len,len_str,4);
    len += 4;
    cur_data->str[len++] = '*';
    
    //数据
    SCI_MEMCPY(cur_data->str+len,pContent,ContentLen);
    len += ContentLen;

    //结束]
    cur_data->str[len++] = YX_NET_MSG_END;
#endif

    cur_data->pMe = g_yx_app.m_yx_tcp_reg_interface;
    cur_data->len = len;
    cur_data->times = repeat_num;
    cur_data->timeout = timeout;
    cur_data->priority = priority;
    cur_data->rcv_handle = rcv_handle;
    ZDT_TCP_PostEvtTo_APP(ZDT_APP_SIGNAL_TCP_WRITE_ADD,cur_data,len);
    
    ZDT_LOG("YX_Net_TCPSend Handle=0x%x",(uint32)cur_data);
    return (uint32)cur_data;
}

extern uint32 YX_Net_TCPRespond(char * meid,char * pContent,uint16 ContentLen)
{
    ZDT_TCP_LINK_DATA_T * cur_data = NULL;
    uint16 len = 0;
    uint16 msg_len = 0;
    char len_str[5] = {0};
    uint8 meid_len = 0;
#if FEATURE_YX_SEND_MD5
    char md5_str[32] = {0};
#endif
    
    if(meid == NULL || ContentLen == 0 || pContent == NULL )
    {
        ZDT_LOG("YX_Net_TCPRespond: ERR NULL");
        return 0;
    }
    meid_len = SCI_STRLEN(meid);
    
    cur_data=(ZDT_TCP_LINK_DATA_T *)SCI_ALLOC_APPZ(sizeof(ZDT_TCP_LINK_DATA_T));
    if(cur_data == NULL)
    {
        ZDT_LOG("YX_Net_TCPRespond: ERR Malloc");
        return 0;
    }
    
#if FEATURE_YX_SEND_MD5
    msg_len = meid_len + ContentLen + 43;
#else
    msg_len = meid_len + ContentLen + 11;
#endif
    cur_data->str= (uint8 *)SCI_ALLOC_APPZ(msg_len);
    if(cur_data->str == NULL)
    {
        ZDT_LOG("YX_Net_TCPRespond: ERR Malloc len=%d",msg_len);
        SCI_FREE(cur_data);
        return 0;
    }
    
    SCI_MEMSET(cur_data->str,0,msg_len);
#if FEATURE_YX_SEND_MD5
    //[XY*
    cur_data->str[0] = YX_NET_MSG_START;
    cur_data->str[33] = YX_NET_MSG_SEND_CS_1;
    cur_data->str[34] = YX_NET_MSG_SEND_CS_2;
    cur_data->str[35] = '*';
    //MEID
    SCI_MEMCPY(cur_data->str+36,meid,meid_len);
    len = meid_len + 36;
    cur_data->str[len++] = '*';
    
    //包长度
    sprintf(len_str,"%04X",ContentLen);
    SCI_MEMCPY(cur_data->str+len,len_str,4);
    len += 4;
    cur_data->str[len++] = '*';
    
    //数据
    SCI_MEMCPY(cur_data->str+len,pContent,ContentLen);
    len += ContentLen;
    
    YX_Net_MD5(cur_data->str+33,len-33,md5_str);
    SCI_MEMCPY(cur_data->str+1,md5_str,32);
    //结束]
    cur_data->str[len++] = YX_NET_MSG_END;
 #else
    //[XY*
    cur_data->str[0] = YX_NET_MSG_START;
    cur_data->str[1] = YX_NET_MSG_SEND_CS_1;
    cur_data->str[2] = YX_NET_MSG_SEND_CS_2;
    cur_data->str[3] = '*';
    //MEID
    SCI_MEMCPY(cur_data->str+4,meid,meid_len);
    len = meid_len + 4;
    cur_data->str[len++] = '*';
    
    //包长度
    sprintf(len_str,"%04X",ContentLen);
    SCI_MEMCPY(cur_data->str+len,len_str,4);
    len += 4;
    cur_data->str[len++] = '*';
    
    //数据
    SCI_MEMCPY(cur_data->str+len,pContent,ContentLen);
    len += ContentLen;

    //结束]
    cur_data->str[len++] = YX_NET_MSG_END;
#endif
    cur_data->pMe = g_yx_app.m_yx_tcp_reg_interface;
    cur_data->len = len;
    cur_data->times = 0;
    cur_data->timeout = 0;
    cur_data->priority = 150;
    cur_data->rcv_handle = NULL;
    ZDT_TCP_PostEvtTo_APP(ZDT_APP_SIGNAL_TCP_WRITE_ADD,cur_data,len);
    
    ZDT_LOG("YX_Net_TCPRespond Handle=0x%x",(uint32)cur_data);
    return (uint32)cur_data;
}

static void YX_Heart_TimeOut(
                                uint8 timer_id,
                                uint32 param
                                )
{
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_HEART,NULL,0);
    return;
}

void YX_Heart_TimeStart(YX_APP_T *pMe,uint16 time_s)
{
    if(0 != pMe->m_yx_heart_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_heart_timer_id);
        pMe->m_yx_heart_timer_id = 0;
    }
    
    pMe->m_yx_heart_timer_id = MMK_CreateTimerCallback(time_s*999, 
                                                                        YX_Heart_TimeOut, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
    return;
}

void YX_Heart_TimeStop(YX_APP_T *pMe)
{
    if(0 != pMe->m_yx_heart_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_heart_timer_id);
        pMe->m_yx_heart_timer_id = 0;
    }
}

BOOLEAN YX_LocReport_JudgeSend(void)
{
    BOOLEAN res = FALSE;
    res = YX_LocReport_Judge();
    YX_LocReport_Reset();
    return res;
}

static void YX_LocReport_TimeOut(
                                uint8 timer_id,
                                uint32 param
                                )
{
    YX_APP_T *pMe = (YX_APP_T *)param;
    if(g_is_yx_is_locing)
    {
        SCI_TraceLow("wuxx, YX_LocReport_TimeOut, 01,");// wuxx_20231219 
        MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOCREPORT,NULL,0);
    }
    else
    {
        if(YX_LocReport_JudgeSend())
        {
            SCI_TraceLow("wuxx, YX_LocReport_TimeOut, 02,");// wuxx_20231219 
            MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOCREPORT,NULL,0);
        }
        else
        {
            SCI_TraceLow("wuxx, YX_LocReport_TimeOut, 03,");// wuxx_20231219 
            if(g_yx_loc_timer_interval != 0)
            {
                SCI_TraceLow("wuxx, YX_LocReport_TimeOut, 04,");// wuxx_20231219 
                MMK_StartTimerCallback(pMe->m_yx_pos_timer_id,g_yx_loc_timer_interval*990, 
                                                                        YX_LocReport_TimeOut, 
                                                                        (uint32)pMe,
                                                                        FALSE);
            }
        }
    }
}

void YX_LocReport_TimeStart(YX_APP_T *pMe,uint16 time_s)
{
    if(0 != pMe->m_yx_pos_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_pos_timer_id);
        pMe->m_yx_pos_timer_id = 0;
    }
    SCI_TraceLow("wuxx, YX_LocReport_TimeStart, 01,time_s=%d", time_s);// wuxx_20231219 
    if(time_s == 0)
    {
        return;
    }
    SCI_TraceLow("wuxx, YX_LocReport_TimeStart, 02,time_s=%d", time_s);// wuxx_20231219 
    pMe->m_yx_pos_timer_id = MMK_CreateTimerCallback(time_s*990, 
                                                                        YX_LocReport_TimeOut, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
}

void YX_LocReport_TimeStop(YX_APP_T *pMe)
{
    if(0 != pMe->m_yx_pos_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_pos_timer_id);
        pMe->m_yx_pos_timer_id = 0;
    }
}

void YX_LocReport_TimeSet(uint16 time_val)
{
    uint16 left = 0;
    uint16 lk_num = 0;
    
    g_yx_loc_timer_lk_num = 0;
    
#if defined(ZDT_PLAT_YX_SUPPORT_ZTE)// wuxx_20231219 // wuxx add fot ZTE,  ZTE_WATCH // 其它项目也可如此修改
    g_yx_loc_timer_interval = time_val;
   SCI_TraceLow("wuxx, YX_LocReport_TimeStart, 01,g_yx_loc_timer_interval=%d", g_yx_loc_timer_interval);

#else
    g_yx_loc_timer_interval = 0;
    if(time_val != 0)
    {
        lk_num = time_val/g_yx_cur_heart_time;
        left = time_val%g_yx_cur_heart_time;
        if(lk_num > 0)
        {
            if(left <= 60)
            {
                g_yx_loc_timer_lk_num = lk_num;
            }
            else if( (g_yx_cur_heart_time-left) <= 60)
            {
                g_yx_loc_timer_lk_num = lk_num + 1;
            }
            else
            {
                g_yx_loc_timer_interval = time_val;
            }
        }
        else
        {
            if((g_yx_cur_heart_time-left) <= 60)
            {
                g_yx_loc_timer_lk_num = 1;
            }
            else
            {
                g_yx_loc_timer_interval = time_val;
            }
        }
    }
#endif
}

BOOLEAN YX_LocReport_In_LK(uint16 num)
{
    uint16 cur_times = 0;
    if(g_yx_loc_timer_interval == 0 && g_yx_loc_timer_lk_num > 0)
    {
        cur_times = num%g_yx_loc_timer_lk_num;
        if(cur_times == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}
static void YX_LocCR_TimeOut(
                                uint8 timer_id,
                                uint32 param
                                )
{
    YX_APP_T *pMe = (YX_APP_T *)param;
    uint16 left = 0;
    SCI_TraceLow("wuxx, YX_LocCR_TimeOut, 01,yx_DB_Set_Rec.time_interval=%d", yx_DB_Set_Rec.time_interval);
    YX_LocReport_TimeSet(yx_DB_Set_Rec.time_interval);
    
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129
    YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_OFF,NULL,0);
#endif
    g_is_yx_is_locing = FALSE;
    YX_LocReport_TimeStop(pMe);
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOCREPORT,NULL,0);
}

void YX_LocCR_TimeStart(YX_APP_T *pMe,uint16 time_s)
{
    if(time_s == 0)
    {
        time_s = 180;
    }
    if(0 != pMe->m_yx_cr_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_cr_timer_id);
        pMe->m_yx_cr_timer_id = 0;
    }
    
    pMe->m_yx_cr_timer_id = MMK_CreateTimerCallback(time_s*1000, 
                                                                        YX_LocCR_TimeOut, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
    
}

void YX_LocCR_TimeStop(YX_APP_T *pMe)
{
    if(0 != pMe->m_yx_cr_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_cr_timer_id);
        pMe->m_yx_cr_timer_id = 0;
    }
}

static void YX_Find_TimeOut(
                                uint8 timer_id,
                                uint32 param
                                )
{
    YX_API_CZSB_Stop();
}

void YX_Find_TimeStart(YX_APP_T *pMe,uint16 time_s)
{
    if(time_s == 0)
    {
        time_s = 60;
    }
    if(0 != pMe->m_yx_find_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_find_timer_id);
        pMe->m_yx_find_timer_id = 0;
    }
    
    pMe->m_yx_find_timer_id = MMK_CreateTimerCallback(time_s*1000, 
                                                                        YX_Find_TimeOut, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
}

void YX_Find_TimeStop(YX_APP_T *pMe)
{
    if(0 != pMe->m_yx_find_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_find_timer_id);
        pMe->m_yx_find_timer_id = 0;
    }
}
static int YX_Net_Send_DLT_CB(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint8 * pContent;
    uint32 Content_len = 0;
    int cont_len = 0;
    char buf[21] = {0};
    int ret = 0;
    YX_APP_T *pMe = (YX_APP_T *)&g_yx_app;
    
    cont_len = YX_Net_GetContentLen_RCV(pMe,pRcv,Rcv_len);
    if(cont_len > 0)
    {
        pContent = pRcv+(g_yx_imei_len + 10);
        Content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &Content_len,buf,20);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
        if(ret == 3 &&  strncmp( (char *)buf, "DLT", ret ) == 0)
        {
            ZDT_LOG("YX_Net_Send_DLT_CB Success");
            return ZDT_TCP_RET_OK;
        }
    }

    ZDT_LOG("YX_Net_Send_DLT_CB  ERR cont_len=%d",cont_len);
    return ZDT_TCP_RET_ERR;
}

uint32 YX_Net_Send_DLT(YX_APP_T *pMe,uint8 repeat_num,uint32 timeout)
{
/*  解绑
发送[YX*YYYYYYYYYY*LEN*DLT]   实例:[SG*9403094122*0003*DLT]
回复[YX*YYYYYYYYYY*LEN*DLT]   实例:[SG*9403094122*0003*DLT]

*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
    send_buf[0] = 'D';
    send_buf[1] = 'L';
    send_buf[2] = 'T';
    send_len = 3;
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,200,repeat_num,timeout,YX_Net_Send_DLT_CB);
    ZDT_LOG("YX_Net_Send_DLT Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

static int YX_Net_Send_LK_CB(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint8 * pContent;
    uint32 Content_len = 0;
    int cont_len = 0;
    char buf[21] = {0};
    int ret = 0;
    YX_APP_T *pMe = (YX_APP_T *)&g_yx_app;
    
    cont_len = YX_Net_GetContentLen_RCV(pMe,pRcv,Rcv_len);
    if(cont_len > 0)
    {
        pContent = pRcv+(g_yx_imei_len + 10);
        Content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &Content_len,buf,20);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
        if(ret == 2 &&  strncmp( (char *)buf, "LK", ret ) == 0)
        {
            ZDT_LOG("YX_Net_Send_LK_CB Success");
            return ZDT_TCP_RET_OK;
        }
    }

    ZDT_LOG("YX_Net_Send_LK_CB  ERR cont_len=%d",cont_len);
    return ZDT_TCP_RET_ERR;
}

uint32 YX_Net_Send_LK(YX_APP_T *pMe,uint8 repeat_num,uint32 timeout)
{
/*  链路保持
发送[CS*YYYYYYYYYY*LEN*LK]   实例:[SG*8800000015*0002*LK]
回复[CS*YYYYYYYYYY*LEN*LK]   实例:[SG*8800000015*0002*LK]

发送2  [CS*YYYYYYYYYY*LEN*LK,步数,翻滚次数,电量百分数] 实例:[SG*8800000015*000D*LK,50,100,100]
回复2  [CS*YYYYYYYYYY*LEN*LK]   实例:[SG*8800000015*0002*LK]
*/
    uint8 send_buf[200] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_LK Err NoReg");
        return 0;
    }
    #if YX_TCP_LK_TYPE
        send_buf[0] = 'L';
        send_buf[1] = 'K';
        send_buf[2] = ',';
        send_len = 3;
        send_len += YX_Func_GetJPNum(send_buf+send_len);
        send_len += YX_Func_GetFGNum(send_buf+send_len);
        send_len += YX_Func_GetBatPercent(send_buf+send_len);
        send_len--;
    #else
        send_buf[0] = 'L';
        send_buf[1] = 'K';
        send_len = 2;
    #endif
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,200,repeat_num,timeout,YX_Net_Send_LK_CB);
    
    ZDT_LOG("YX_Net_Send_LK Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

static int YX_Net_Send_TIME_CB(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint8 * pContent;
    uint32 Content_len = 0;
    int cont_len = 0;
    char buf[32] = {0};
    int ret = 0;
    YX_APP_T *pMe = (YX_APP_T *)pUser;
    
    cont_len = YX_Net_GetContentLen_RCV(pMe,pRcv,Rcv_len);
    if(cont_len > 0)
    {
        pContent = pRcv+(g_yx_imei_len + 10);
        Content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &Content_len,buf, 31);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
        if(ret == 4 &&  strncmp( (char *)buf, "TIME", ret ) == 0)
        {
            char time_buf[20] = {0};
            long time_s = 0;
            ZDT_LOG("ZDT__LOG YX_Net_Send_TIME_CB Success ,cont_len=%d, pContent =%s", cont_len, buf);
            SCI_MEMSET(buf, 0, 32);
            ret = YX_Func_GetNextPara(&pContent, &Content_len,buf, 31);
            ZDT_LOG("ZDT__LOG YX_Net_Send_TIME_CB Success ,cont_len=%d, time_buf =%s", Content_len, buf);
            if(is_all_number(buf , strlen(buf)))
            {
                SCI_DATE_T  new_date = {0}; 
                SCI_TIME_T  new_time = {0};
                SCI_DATE_T  sys_date = {0}; 
                SCI_TIME_T  sys_time = {0};
                TM_GetSysDate(&sys_date);
                TM_GetSysTime(&sys_time);
                time_s = atol(buf);
                timestampToDateTime(time_s , &new_date , &new_time);
                if(sys_date.year != new_date.year 
                   || sys_date.mon != new_date.mon 
                   || sys_date.mday != new_date.mday 
                   || sys_time.hour != new_time.hour 
                   || sys_time.min != new_time.min 
                )
                {
                    ZDT_LOG("ZDT__LOG YX_Net_Send_TIME_CB Success ,set new date time");
                    MMIAPICOM_SetSysData(new_date);
                    MMIAPICOM_SetSysTime(new_time);
                }

                

            }
            ZDT_LOG("YX_Net_Send_TIME_CB Success");
            return ZDT_TCP_RET_OK;
        }
    }

    ZDT_LOG("YX_Net_Send_TIME_CB  ERR cont_len=%d",cont_len);
    return ZDT_TCP_RET_ERR;
}

uint32 YX_Net_Send_TIME(YX_APP_T *pMe)
{
/*  
    获取系统时间默认北京时间
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_TIME Err NoReg");
        return 0;
    }
    send_buf[0] = 'T';
    send_buf[1] = 'I';
    send_buf[2] = 'M';
    send_buf[3] = 'E';
    send_len = 4;
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,YX_Net_Send_TIME_CB);
    ZDT_LOG("YX_Net_Send_TIME Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

static int YX_Net_Send_LGZONE_CB(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint8 * pContent;
    uint32 Content_len = 0;
    int cont_len = 0;
    char buf[21] = {0};
    int ret = 0;
    YX_APP_T *pMe = (YX_APP_T *)&g_yx_app;
    
    cont_len = YX_Net_GetContentLen_RCV(pMe,pRcv,Rcv_len);
    if(cont_len > 0)
    {
        pContent = pRcv+(g_yx_imei_len + 10);
        Content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &Content_len,buf,20);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
        if(ret == 6 &&  strncmp( (char *)buf, "LGZONE", ret ) == 0)
        {
            ZDT_LOG("YX_Net_Send_LGZONE_CB Success");
            return ZDT_TCP_RET_OK;
        }
    }

    ZDT_LOG("YX_Net_Send_LGZONE_CB  ERR cont_len=%d",cont_len);
    return ZDT_TCP_RET_ERR;
}

uint32 YX_Net_Send_LGZONE(YX_APP_T *pMe)
{
/*  
    获取系统时间带时区
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_LGZONE Err NoReg");
        return 0;
    }
    send_buf[0] = 'L';
    send_buf[1] = 'G';
    send_buf[2] = 'Z';
    send_buf[3] = 'O';
    send_buf[4] = 'N';
    send_buf[5] = 'E';
    send_len = 6;
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,YX_Net_Send_LGZONE_CB);
    ZDT_LOG("YX_Net_Send_LGZONE Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_UD(YX_APP_T* pMe,uint8 loc_ok)
{
/* 
    [CS*YYYYYYYYYY*LEN*UD_CDMA,位置数据(见附录二)]
    [SG*9403094122*00CD*UD_CDMA,180916,025723,A,22.570733,N,113.8626083,E,0.00,249.5,0.0,
    6,100,60,0,0,00000010,sid,nid,bid,signal]
    UD,命令号
    180916,日期
    025723,时间
    A,gps 定位有效
    22.570733,纬度
    N,纬度表示
    113.8626083,经度
    E,经度表示
    0.00,速度
    4
    249.5,方向
    0.0,海拔
    6,卫星个数
    100,gsm 信号强度
    60,电量
    0,计步数
    0,翻转次数
    00000010,终端状态，数据为 16 进制，解析成二进制为 0000 0000 0000 0000 0000 0000 0001 0000
    前面 4 个字节表示状态，后面 4 个字节表示报警，数据中第 4 位为 1，对照文档最后的数据解析，
    表示手表静止状态。详细可参看文档最后部分。
    电信基站数据
    sid 表示 cdma 系统识别码
    nid 表示 cdma 网络识别码
    bid 表示 cdma 小区唯一识别码
    signal 表示信号强度
    WiFi 数据
*/

    uint8 send_buf[1000] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    GPS_DATA_T last_gps = {0};

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
#if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)
    GPS_API_GetLastPos(&last_gps);
    if((loc_ok&0x01) == 0)
    {
        last_gps.is_valid = 0;
    }
    SCI_TRACE_LOW("YX_Net_Send_UD loc_ok=%d, gps_is_valid=%d",loc_ok,last_gps.is_valid);
#endif
    
    SCI_MEMCPY(send_buf,"UD,",3);
    send_len = 3;
    send_len += YX_Func_GetDateTime(send_buf+send_len);
    if(last_gps.is_valid)
    {
        send_buf[send_len++] = 'A';
    }
    else
    {
        send_buf[send_len++] = 'V';
    }
    send_buf[send_len++] = ',';
    send_len += YX_Func_GetGPSLat(send_buf+send_len,last_gps.Latitude,last_gps.Lat_m,last_gps.Lat_Dir);
    send_len += YX_Func_GetGPSLong(send_buf+send_len,last_gps.Longitude,last_gps.Long_Dir);
    send_len += YX_Func_GetGPSSpeed(send_buf+send_len,last_gps.speed);
    send_len += YX_Func_GetGPSCog(send_buf+send_len,last_gps.cog);
    send_len += YX_Func_GetGPSAlt(send_buf+send_len,last_gps.altitude,last_gps.alt_Dir);
    send_len += YX_Func_GetGPSSateNum(send_buf+send_len,last_gps.sate_num);
    send_len += YX_Func_GetRssiPercent(send_buf+send_len);
    send_len += YX_Func_GetBatPercent(send_buf+send_len);
    send_len += YX_Func_GetJPNum(send_buf+send_len);
    send_len += YX_Func_GetFGNum(send_buf+send_len);
    send_len += YX_Func_GetStatus(send_buf+send_len,0);
    send_len += YX_Func_GetSSInfo(send_buf+send_len);
    send_len += YX_Func_GetWIFIInfo(send_buf+send_len,loc_ok);

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,50,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_UD Handle=0x%x",send_hande);
    return send_hande;
}

uint32 YX_Net_Send_UD2(YX_APP_T* pMe,uint8 loc_ok)
{
    uint8 send_buf[800] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    GPS_DATA_T last_gps = {0};

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
#if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)
    GPS_API_GetLastPos(&last_gps);
    if((loc_ok&0x01) == 0)
    {
        #if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
        // NOT
        #else
        last_gps.is_valid = 0;
        #endif
    }
#endif
    SCI_MEMCPY(send_buf,"UD2,",4);
    send_len = 4;
    send_len += YX_Func_GetDateTime(send_buf+send_len);
    if(last_gps.is_valid)
    {
        send_buf[send_len++] = 'A';
    }
    else
    {
        send_buf[send_len++] = 'V';
    }
    send_buf[send_len++] = ',';
    send_len += YX_Func_GetGPSLat(send_buf+send_len,last_gps.Latitude,last_gps.Lat_m,last_gps.Lat_Dir);
    send_len += YX_Func_GetGPSLong(send_buf+send_len,last_gps.Longitude,last_gps.Long_Dir);
    send_len += YX_Func_GetGPSSpeed(send_buf+send_len,last_gps.speed);
    send_len += YX_Func_GetGPSCog(send_buf+send_len,last_gps.cog);
    send_len += YX_Func_GetGPSAlt(send_buf+send_len,last_gps.altitude,last_gps.alt_Dir);
    send_len += YX_Func_GetGPSSateNum(send_buf+send_len,last_gps.sate_num);
    send_len += YX_Func_GetRssiPercent(send_buf+send_len);
    send_len += YX_Func_GetBatPercent(send_buf+send_len);
    send_len += YX_Func_GetJPNum(send_buf+send_len);
    send_len += YX_Func_GetFGNum(send_buf+send_len);
    send_len += YX_Func_GetStatus(send_buf+send_len,0);
    send_len += YX_Func_GetSSInfo(send_buf+send_len);
    send_len += YX_Func_GetWIFIInfo(send_buf+send_len,loc_ok);

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,50,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_UD2 Handle=0x%x",send_hande);
    return send_hande;
}

static int YX_Net_Send_AL_CB(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint8 * pContent;
    uint32 Content_len = 0;
    int cont_len = 0;
    char buf[21] = {0};
    int ret = 0;
    YX_APP_T *pMe = (YX_APP_T *)&g_yx_app;
    cont_len = YX_Net_GetContentLen_RCV(pMe,pRcv,Rcv_len);
    if(cont_len > 0)
    {
        pContent = pRcv+(g_yx_imei_len + 10);
        Content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &Content_len,buf,20);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
        if(ret == 2 &&  strncmp( (char *)buf, "AL", ret ) == 0)
        {
            ZDT_LOG("YX_Net_Send_AL_CB Success");
            return ZDT_TCP_RET_OK;
        }
        else if(ret == 7 &&  strncmp( (char *)buf, "SENDSOS", ret ) == 0) //努比亚sos发送位置
        {
            ZDT_LOG("YX_Net_Send_ASENDSOS_CB Success");
            return ZDT_TCP_RET_OK;
        }
    }

    ZDT_LOG("YX_Net_Send_AL_CB  ERR cont_len=%d",cont_len);
    return ZDT_TCP_RET_ERR;
}

PUBLIC void YX_StartSosAlertLocation()
{
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_ALERT, NULL, YX_NET_ALERT_TYPE_SOS);
}


uint32 YX_Net_Send_AL(YX_APP_T* pMe,uint16 alert_type,uint8 loc_ok)
{
    //报警数据
    uint8 send_buf[800] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    GPS_DATA_T last_gps = {0};

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
#if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)
    GPS_API_GetLastPos(&last_gps);
    if((loc_ok&0x01) == 0)
    {
        #if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
        // NOT
        #else
        last_gps.is_valid = 0;
        #endif
    }
#endif
    SCI_MEMCPY(send_buf,"SENDSOS,",8);
    send_len = 8;
    send_len += YX_Func_GetDateTime(send_buf+send_len);
    if(last_gps.is_valid)
    {
        send_buf[send_len++] = 'A';
    }
    else
    {
        send_buf[send_len++] = 'V';
    }
    send_buf[send_len++] = ',';
    send_len += YX_Func_GetGPSLat(send_buf+send_len,last_gps.Latitude,last_gps.Lat_m,last_gps.Lat_Dir);
    send_len += YX_Func_GetGPSLong(send_buf+send_len,last_gps.Longitude,last_gps.Long_Dir);
    send_len += YX_Func_GetGPSSpeed(send_buf+send_len,last_gps.speed);
    send_len += YX_Func_GetGPSCog(send_buf+send_len,last_gps.cog);
    send_len += YX_Func_GetGPSAlt(send_buf+send_len,last_gps.altitude,last_gps.alt_Dir);
    send_len += YX_Func_GetGPSSateNum(send_buf+send_len,last_gps.sate_num);
    send_len += YX_Func_GetRssiPercent(send_buf+send_len);
    send_len += YX_Func_GetBatPercent(send_buf+send_len);
    send_len += YX_Func_GetJPNum(send_buf+send_len);
    send_len += YX_Func_GetFGNum(send_buf+send_len);
    send_len += YX_Func_GetStatus(send_buf+send_len,alert_type);
    send_len += YX_Func_GetSSInfo(send_buf+send_len);
    send_len += YX_Func_GetWIFIInfo(send_buf+send_len,loc_ok);

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,110,10,30000,YX_Net_Send_AL_CB);
    
    ZDT_LOG("YX_Net_Send_AL Handle=0x%x",send_hande);
    return send_hande;
}

#ifdef ZDT_VIDEOCHAT_SUPPORT
//yangyu add begin, for video chat 


/*
    UPGVUL , upGetVideoUserList
*/
uint32 YX_Net_Send_GetVideoUsers(YX_APP_T* pMe)
{
/*  
    发送视频通话联系人消息
*/
    uint8 send_buf[20] = "UPGVUL";
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("ZDT__LOG YX_Net_Send_GetVideoUsers Err NoReg");
        return 0;
    }
    //sprintf((char *)send_buf,"HEART,%d",cur_hr);
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,120,0,0,PNULL);
    ZDT_LOG("ZDT__LOG YX_Net_Send_GetVideoUsers Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

PUBLIC void YX_API_GetVideoUsersFromServer()
{
    YX_APP_T * pMe = &g_yx_app;
    YX_Net_Send_GetVideoUsers(pMe);
#ifdef WIN32
    {
        int i;
        VIDEO_USER_INFO video_users[MAX_VIDEO_USERS_COUNT];
        SCI_MEMSET(video_users, 0, MAX_VIDEO_USERS_COUNT*sizeof(VIDEO_USER_INFO));
        for(i = 0 ; i < MAX_VIDEO_USERS_COUNT ; i++)
        {
            sprintf(&video_users[i].phone_user_id, "uid:%d", i*123+1);
            //sprintf(&video_users[i].family_relative, "name:%d", i);
            video_users[i].family_relative[0] = 0x7238;
            sprintf(&video_users[i].phone_num, "%d", 138100 + i);
            video_users[i].video_status = i%3;            
        }
        Video_Call_RequestContactSuccess(video_users);
    }
#endif
}

//WATCH call to APP
uint32 YX_Net_Send_StartVideoCall(VIDEO_USER_INFO user_info)
{

    uint8 send_buf[64] = "UPVCALLAPP";
    uint16 send_len = 0;
    uint32 send_hande = 0;
    YX_APP_T * pMe = &g_yx_app;

#ifdef WIN32
    Video_Call_Incoming_Test();
#endif

    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("ZDT__LOG YX_Net_Send_StartVideoCall Err NoReg");
        return 0;
    }

    if(strlen(user_info.phone_user_id) == 0)
    {
        ZDT_LOG("ZDT__LOG YX_Net_Send_StartVideoCall error user_id");
        return 0;
    }


    strcat(send_buf, ",");
    strcat(send_buf, user_info.phone_user_id);//user-id

    strcat(send_buf, ",");
    strcat(send_buf, g_zdt_phone_imei);//imei

    strcat(send_buf, ",");
#ifdef BAIDU_VIDEOCHAT_SUPPORT
    strcat(send_buf, "BD");//video call type
#elif defined(VIDEO_CALL_AGORA_SUPPORT)
    strcat(send_buf, "SW");//video call type
#else
    strcat(send_buf, "BR");//video call type
#endif
    
    //sprintf((char *)send_buf,"HEART,%d",cur_hr);
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,150,0,0,PNULL);
    ZDT_LOG("ZDT__LOG YX_Net_Send_StartVideoCall Handle=0x%x, %s",send_hande,send_buf);
    return send_hande;
}

//after login success ,replay app, then app will call to watch
uint32 YX_Net_Send_Reply_DOWNVCWW()
{

    YX_APP_T * pMe = &g_yx_app;

    uint8 send_buf[20] = "DOWNVCWW";
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("ZDT__LOG YX_Net_Send_GetVideoUsers Err NoReg");
        return 0;
    }
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,120,0,0,PNULL);
    ZDT_LOG("ZDT__LOG YX_Net_Send_Reply_DOWNVCWW Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

//视频通话未接通时手表主动挂断
uint32 YX_Net_Send_UPWATCHHANGUP(uint8 * video_id)
{

    YX_APP_T * pMe = &g_yx_app;

    uint8 send_buf[128] = "UPWATCHHANGUP";
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("ZDT__LOG YX_Net_Send_GetVideoUsers Err NoReg");
        return 0;
    }
    strcat(send_buf, ",");
    strcat(send_buf, video_id);
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,120,0,0,PNULL);
    ZDT_LOG("ZDT__LOG YX_Net_Send_Reply_DOWNVCWW Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}


//APP视频通话后主动挂断未接通时挂断
uint32 YX_Net_Send_Reply_DOWNAPPHANGUP()
{
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    YX_APP_T * pMe = &g_yx_app;

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
    SCI_MEMCPY(send_buf,"DOWNAPPHANGUP",13);
    send_len = 13;

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,120,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_Reply_DOWNAPPHANGUP Handle=0x%x",send_hande);
    return send_hande;
}

//yangyu end

//视频通话挂断后,终端上报视频通话时长-秒, wuxx add 20231122
uint32 YX_Net_Send_UPVCALLTIME(uint8 * video_id, uint32 duration)
{
    uint8 send_buf[256] = {"UPVCALLTIME"};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    uint8 duration_buf[64] = {0};
        
    YX_APP_T * pMe = &g_yx_app;

    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("ZDT__LOG YX_Net_Send_UPVCALLTIME Err NoReg");
        return 0;
    }
    strcat(send_buf, ",");
    sprintf(duration_buf, "%d", duration);
    strcat(send_buf, duration_buf);
    strcat(send_buf, ",");
    strcat(send_buf, video_id);

    send_len = strlen((char *)send_buf);

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,120,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_UPVCALLTIME Handle=0x%x",send_hande);
    return send_hande;
}





#endif
uint32 YX_Net_Send_WT(YX_APP_T* pMe, uint8 loc_ok)
{
    uint8 send_buf[800] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    GPS_DATA_T last_gps = {0};

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
#if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)
    GPS_API_GetLastPos(&last_gps);
    if((loc_ok&0x01) == 0)
    {
        #if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
        // NOT
        #else
        last_gps.is_valid = 0;
        #endif
    }
#endif
#ifdef ZTE_WEATHER_SUPPORT
    SCI_MEMCPY(send_buf,"WT_FUTR,",8);
    send_len = 8;
#else
    SCI_MEMCPY(send_buf,"WT,",3);
    send_len = 3;
#endif
    send_len += YX_Func_GetDateTime(send_buf+send_len);
    if(last_gps.is_valid)
    {
        send_buf[send_len++] = 'A';
    }
    else
    {
        send_buf[send_len++] = 'V';
    }
    send_buf[send_len++] = ',';
    send_len += YX_Func_GetGPSLat(send_buf+send_len,last_gps.Latitude,last_gps.Lat_m,last_gps.Lat_Dir);
    send_len += YX_Func_GetGPSLong(send_buf+send_len,last_gps.Longitude,last_gps.Long_Dir);
    send_len += YX_Func_GetGPSSpeed(send_buf+send_len,last_gps.speed);
    send_len += YX_Func_GetGPSCog(send_buf+send_len,last_gps.cog);
    send_len += YX_Func_GetGPSAlt(send_buf+send_len,last_gps.altitude,last_gps.alt_Dir);
    send_len += YX_Func_GetGPSSateNum(send_buf+send_len,last_gps.sate_num);
    send_len += YX_Func_GetRssiPercent(send_buf+send_len);
    send_len += YX_Func_GetBatPercent(send_buf+send_len);
    send_len += YX_Func_GetJPNum(send_buf+send_len);
    send_len += YX_Func_GetFGNum(send_buf+send_len);
    send_len += YX_Func_GetStatus(send_buf+send_len,0);
    send_len += YX_Func_GetSSInfo(send_buf+send_len);
    send_len += YX_Func_GetWIFIInfo(send_buf+send_len,loc_ok);

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_WT Handle=0x%x",send_hande);
    ZDT_LOG("YX_Net_Send_WT Handle=%s",send_buf);
    return send_hande;
}

uint32 YX_Net_Send_WT_Check(uint8 loc_ok)
{
    if(g_yx_wt_recieve_ok == FALSE)
    {
        return YX_Net_Send_WT(&g_yx_app,loc_ok);
    }
    return 0;
}

#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
static int YX_Net_Send_PP_CB(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint8 * pContent;
    uint32 Content_len = 0;
    int cont_len = 0;
    char buf[21] = {0};
    int ret = 0;
    YX_APP_T *pMe = (YX_APP_T *)&g_yx_app;
    cont_len = YX_Net_GetContentLen_RCV(pMe,pRcv,Rcv_len);
    if(cont_len > 0)
    {
        pContent = pRcv+(g_yx_imei_len + 10);
        Content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &Content_len,buf,20);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
        if(ret == 2 &&  strncmp( (char *)buf, "PP", ret ) == 0)
        {
            ZDT_LOG("YX_Net_Send_PP_CB Success");
            return ZDT_TCP_RET_OK;
        }
    }

    ZDT_LOG("YX_Net_Send_PP_CB  ERR cont_len=%d",cont_len);
    return ZDT_TCP_RET_ERR;
}

uint32 YX_Net_Send_PP(YX_APP_T* pMe,uint8 loc_ok)
{
    //碰碰交友
    uint8 send_buf[1000] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    GPS_DATA_T last_gps = {0};

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
#if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)
    GPS_API_GetLastPos(&last_gps);
    if((loc_ok&0x01) == 0)
    {
        #if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
        // NOT
        #else
        last_gps.is_valid = 0;
        #endif
    }
#endif
    SCI_MEMCPY(send_buf,"PP,",3);
    send_len = 3;
    send_len += YX_Func_GetDateTime(send_buf+send_len);
    if(last_gps.is_valid)
    {
        send_buf[send_len++] = 'A';
    }
    else
    {
        send_buf[send_len++] = 'V';
    }
    send_buf[send_len++] = ',';
    send_len += YX_Func_GetGPSLat(send_buf+send_len,last_gps.Latitude,last_gps.Lat_m,last_gps.Lat_Dir);
    send_len += YX_Func_GetGPSLong(send_buf+send_len,last_gps.Longitude,last_gps.Long_Dir);
    send_len += YX_Func_GetGPSSpeed(send_buf+send_len,last_gps.speed);
    send_len += YX_Func_GetGPSCog(send_buf+send_len,last_gps.cog);
    send_len += YX_Func_GetGPSAlt(send_buf+send_len,last_gps.altitude,last_gps.alt_Dir);
    send_len += YX_Func_GetGPSSateNum(send_buf+send_len,last_gps.sate_num);
    send_len += YX_Func_GetRssiPercent(send_buf+send_len);
    send_len += YX_Func_GetBatPercent(send_buf+send_len);
    send_len += YX_Func_GetJPNum(send_buf+send_len);
    send_len += YX_Func_GetFGNum(send_buf+send_len);
    send_len += YX_Func_GetStatus(send_buf+send_len,0);
    send_len += YX_Func_GetSSInfo(send_buf+send_len);
    send_len += YX_Func_GetWIFIInfo(send_buf+send_len,loc_ok);
    //UI界面超时是30秒，所以设置长比30秒短的时间28秒
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,120,0,28000,YX_Net_Send_PP_CB);//默认10 秒 服务器还没返回结果
    
    ZDT_LOG("YX_Net_Send_PP Handle=0x%x",send_hande);
    return send_hande;
}

uint32 YX_Net_Send_PPQ(YX_APP_T* pMe)
{
/*  
    获取好友信息
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_land == FALSE)
    {
        ZDT_LOG("YX_Net_Send_PPQ");
        return 0;
    }
    SCI_MEMCPY(send_buf,"PPQ",3);
    send_len = 3;
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_PPQ Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_UPGCUL(YX_APP_T* pMe)
{
/*  
    获取系统好友信息
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_land == FALSE)
    {
        ZDT_LOG("YX_Net_Send_UPGCUL");
        return 0;
    }
    SCI_MEMCPY(send_buf,"UPGCUL",6);
    send_len = 6;
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_UPGCUL Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_PPR(YX_APP_T* pMe, uint8 * friend_id)
{
/*  
    解除碰碰交友
*/
    uint8 send_buf[100] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_land == FALSE)
    {
        ZDT_LOG("YX_Net_Send_PPR");
        return 0;
    }
    sprintf(send_buf,"PPR,%s",friend_id);
    send_len = strlen(send_buf);
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,120,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_PPR Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}
#endif

uint32 YX_Net_Send_TKQ(YX_APP_T* pMe)
{
    //请求录音下发
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
    SCI_MEMCPY(send_buf,"TKQ",3);
    send_len = 3;

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,3,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_TKQ Handle=0x%x",send_hande);
    return send_hande;
}

uint32 YX_Net_Send_TKQ2(YX_APP_T* pMe)
{
    //请求录音下发
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
    SCI_MEMCPY(send_buf,"TKQ2",4);
    send_len = 4;

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,1,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_TKQ2 Handle=0x%x",send_hande);
    return send_hande;
}

uint32 YX_Net_Send_STKQ(YX_APP_T* pMe)
{
    //请求录音下发
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;

    if(pMe->m_yx_is_land == FALSE)
    {
        return 0;
    }
    SCI_MEMCPY(send_buf,"STKQ",4);
    send_len = 4;

    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,2,0,0,NULL);
    
    ZDT_LOG("YX_Net_Send_STKQ Handle=0x%x",send_hande);
    return send_hande;
}

uint32 YX_Net_Send_PHLQ(YX_APP_T* pMe)
{
/*  
    获取电话本
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_PHLQ Err NoReg");
        return 0;
    }
    SCI_MEMCPY(send_buf,"PHLQ",4);
    send_len = 4;
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_PHLQ Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

///电话本+白名单
uint32 YX_Net_Send_PHL(YX_APP_T* pMe)
{
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_PHL Err NoReg");
        return 0;
    }
    SCI_MEMCPY(send_buf,"PHL",3);
    send_len = 3;
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_PHL Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_SOSQ(YX_APP_T* pMe)
{
/*  
    获取SOS号码
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_SOSQ Err NoReg");
        return 0;
    }
    SCI_MEMCPY(send_buf,"SOSQ",4);
    send_len = 4;
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_SOSQ Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_BANDQ(YX_APP_T* pMe)
{
/*	
	获取绑定状态
*/
	uint8 send_buf[30] = {0};
	uint16 send_len = 0;
	uint32 send_hande = 0;
	if(pMe->m_yx_is_reg == FALSE)
	{
		ZDT_LOG("ZDT__LOG YX_Net_Send_BANDQ Err NoReg");
		return 0;
	}
	SCI_MEMCPY(send_buf,"BANDQ",5);
	send_len = 5;
	send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
	ZDT_LOG("YX_Net_Send_BANDQ Handle=0x%x,%s",send_hande,send_buf);
	return send_hande;
}

uint32 YX_Net_Send_REMINDQ(YX_APP_T* pMe)
{
/*  
    获取闹钟信息
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_REMINDQ Err NoReg");
        return 0;
    }
    SCI_MEMCPY(send_buf,"REMINDQ",7);
    send_len = 7;
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_REMINDQ Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_HEART(YX_APP_T* pMe,uint16 cur_hr)
{
/*  
    发送心率消息
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_HEART heartrate Err NoReg");
        return 0;
    }
    sprintf((char *)send_buf,"HEART,%d",cur_hr);
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_HEART Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_TEMP(YX_APP_T* pMe,double tw)
{
/*  
    发送体温消息
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_REMINDQ Err NoReg");
        return 0;
    }
    sprintf((char *)send_buf,"TEMP,%.2f",tw);//yangyu modify
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_REMINDQ Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_BLOOD(YX_APP_T* pMe,uint16 b_h, uint16 b_l)
{
/*  
    发送血压消息
*/
    uint8 send_buf[30] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_REMINDQ Err NoReg");
        return 0;
    }
    sprintf((char *)send_buf,"BLOOD,%d,%d",b_h,b_l);
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_REMINDQ Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

static uint32 U8_To_Hex(uint8 *buf,uint32 len,uint8 * dest_buf,uint32 dest_len)
{
    uint8 low = 0;
    uint8 high = 0;
    uint32 i = 0;
    uint32 j = 0;

    if((len*2) > dest_len)
    {
        len = dest_len/2;
    }
    for(i = 0; i < len; i++)
    {
        high = buf[i]>>4;
        dest_buf[j++] = (char)ZDT_Nibble2HexChar(high);
        low = buf[i] & 0x0F;
        dest_buf[j++] = (char)ZDT_Nibble2HexChar(low);
    }
    dest_buf[j] = 0;
    return j;
}

uint32 YX_Net_Send_SMS(uint8 * sms, uint8 sms_size, uint8 * phone_number)
{
    if(yx_DB_Set_Rec.sms_to_app)
    {
        uint16 send_len = 0;
        uint32 send_hande = 0;
        uint8 *send_buf = PNULL;
        uint8 *sms_buf = PNULL;
        YX_APP_T * pMe = &g_yx_app;
        send_buf = (uint8*)SCI_ALLOC_APPZ(2*sms_size+256);
        if(send_buf == PNULL)
        {
            ZDT_LOG("YX_Net_Send_SMS memery error");
            return 0;
        }
        sms_buf = (uint8*)SCI_ALLOC_APPZ(2*sms_size+2);
        if(sms_buf == PNULL)
        {
            SCI_FREE(send_buf);
            ZDT_LOG("YX_Net_Send_SMS memery error");
            return 0;
        }
        if(pMe->m_yx_is_reg == FALSE)
        {
            ZDT_LOG("YX_Net_Send_SMS Err NoReg");
        #ifndef WIN32
            SCI_FREE(send_buf);
            SCI_FREE(sms_buf);
            return 0;
        #endif
        }
        SCI_MEMCPY(send_buf,"SMS,",4);
        send_len = 4;
        send_len += YX_Func_GetDateTime(send_buf+send_len);
        sprintf(send_buf+send_len,"%s,,",phone_number);
        send_len = strlen((char *)send_buf);
        U8_To_Hex(sms,sms_size,sms_buf,2*sms_size);// deng, wuxx add. 加大数组，避免NET SMS 数组超出死机,表现为来电挂断或者长短信死机 256--1024
        //SCI_MEMCPY(send_buf+send_len,sms,sms_size);
        sprintf(send_buf+send_len,"%s",sms_buf);
        send_len =  strlen((char *)send_buf);
    
        send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
        ZDT_LOG("YX_Net_Send_SMS Handle=0x%x,%s",send_hande,send_buf);
        SCI_FREE(send_buf);
        SCI_FREE(sms_buf);
        return send_hande;
    }
    else
    {
        ZDT_LOG("YX_Net_Send_SMS sms to app close");
        return 0;
    }
}

uint32 YX_Net_Send_SCHEDULEQ(YX_APP_T* pMe)
{
	//请求课程表下发
	uint8 send_buf[30] = {0};
	uint16 send_len = 0;
	uint32 send_hande = 0;

	if(pMe->m_yx_is_land == FALSE)
	{
		return 0;
	}
	SCI_MEMCPY(send_buf,"SCHEDULEQ",9);
	send_len = 9;

	send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,0,0,0,NULL);
	
	ZDT_LOG("ZDT__LOG YX_Net_Send_SCHEDULEQ Handle=0x%x",send_hande);
	return send_hande;
}

uint32 YX_Net_Send_DeviceModelVersion(YX_APP_T* pMe)
{
/*  
    [XT*YYYYYYYYYYYYYYY*LEN*DEVICE,MODE,VERSION]
*/
    uint8 send_buf[64] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_DeviceModelVersion NoReg");
        return 0;
    }
    sprintf((char *)send_buf,"DEVICE,%s,%s,%s", WATCH_CTA_MODEL, WATCH_NAME, WATCHCOM_GetSoftwareVersion());
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_DeviceModelVersion Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

//send sync to get all app settings
uint32 YX_Net_Send_RESYNC(YX_APP_T* pMe)
{
/*  
    [XT*YYYYYYYYYYYYYYY*LEN*DEVICE,MODE,VERSION]
*/
    uint8 send_buf[16] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("YX_Net_Send_RESYNC NoReg");
        return 0;
    }
    SCI_STRCPY(send_buf, "RESYNC");
    send_len = strlen((char *)send_buf);
    
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("YX_Net_Send_RESYNC Handle=0x%x,%s",send_hande,send_buf);
    return send_hande;
}

uint32 YX_Net_Send_ICCID(YX_APP_T* pMe)
{
/*  
     [XT*YYYYYYYYYYYYYYY*LEN*SIM,ICCID]
*/
    uint8 send_buf[64] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE || strlen(g_zdt_sim_iccid) == 0)
    {
        ZDT_LOG("[%s] NoReg or iccid is empty",__FUNCTION__);
        return 0;
    }
    sprintf((char *)send_buf,"SIM,%s", g_zdt_sim_iccid);
    send_len = strlen((char *)send_buf);
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("[%s] Handle=0x%x,%s",__FUNCTION__,send_hande,send_buf);
    return send_hande;
}

/*  
[XT*YYYYYYYYYYYYYYY*LEN*CTA]
*/
uint32 YX_Net_Send_CTA(YX_APP_T* pMe)
{
    uint8 send_buf[64] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    if(pMe->m_yx_is_reg == FALSE || strlen(g_zdt_phone_imei) == 0)
    {
        ZDT_LOG("[%s] NoReg or g_zdt_phone_imei is empty",__FUNCTION__);
        return 0;
    }
    SCI_STRCPY(send_buf, "CTA");
    send_len = strlen((char *)send_buf);
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("[%s] Handle=0x%x,%s",__FUNCTION__,send_hande,send_buf);
    return send_hande;
}

/*
[XT*YYYYYYYYYYYYYYY*LEN*UGSTATE,升级状态(1:开始)]
*/
uint32 YX_Net_Send_Upgrade_Start()
{
    uint8 send_buf[64] = {0};
    uint16 send_len = 0;
    uint32 send_hande = 0;
    YX_APP_T * pMe = &g_yx_app;
    if(pMe->m_yx_is_reg == FALSE)
    {
        ZDT_LOG("[%s] NoReg ",__FUNCTION__);
        return 0;
    }
    sprintf((char *)send_buf,"UGSTATE,1");
    send_len = strlen((char *)send_buf);
    send_hande = YX_Net_TCPSend(g_zdt_phone_imei,send_buf,send_len,100,0,0,PNULL);
    ZDT_LOG("[%s] Handle=0x%x,%s",__FUNCTION__,send_hande,send_buf);
    return send_hande;
}

#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
extern uint16 g_pp_wstr_pp_name[41];
extern  uint16 g_pp_wstr_pp_num[41];
extern uint8 g_pp_friend_id[YX_DB_FRIEND_MAX_ID_SIZE+1];
uint16 YX_Net_Friend_End(BOOLEAN is_ok,uint16 err_id,uint8 * friend_name,uint8 * friend_number)
{
    ZDT_LOG("YX_Net_Friend_End is_ok =%d, err_id=%d",err_id);
    if(friend_name != PNULL)
    {
        SCI_MEMSET(g_pp_wstr_pp_name,0,sizeof(g_pp_wstr_pp_name));
        ZDT_UCS_Str16_to_uint16((uint8*)friend_name, SCI_STRLEN(friend_name) ,g_pp_wstr_pp_name, 40);
    }
    if(friend_number != PNULL)
    {
        SCI_MEMSET(g_pp_wstr_pp_num,0,sizeof(g_pp_wstr_pp_num));
        GUI_UTF8ToWstr(g_pp_wstr_pp_num, 40, friend_number, SCI_STRLEN(friend_number));
    }
    MMIZDT_UpdateFriendPPWinOpen(is_ok);
    return 0;
}

uint16 YX_Net_Friend_Start(YX_APP_T * pMe)
{
    ZDT_LOG("ZDT__LOG YX_Net_Friend_Start");
    if(ZDT_SIM_Exsit() == FALSE)
    {
        YX_Net_Friend_End(FALSE,YX_FRIEND_NO_CARD,PNULL,PNULL);
    }
    else if( FALSE == YX_Net_Is_Land())
    {
        if(YX_Net_IsAllow(pMe))
        {
            MMIZDT_Net_Reset(TRUE);
        }
        YX_Net_Friend_End(FALSE,YX_FRIEND_NO_LAND,PNULL,PNULL);
    }
    else
    {
        YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_FRIEND,NULL,0);
        //YX_Net_Send_PP(pMe,0);
    }
    return YX_FRIEND_SEARCH;
}

#endif

#if defined(ZDT_PLAT_YX_SUPPORT_ZTE)// wuxx_20231219
uint16 g_YX_Net_Receive_UPLOAD_min_interval = 0;
#endif

int YX_Net_Receive_UPLOAD(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 min_interval = 0;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        min_interval = atoi(buf);
        
        #if defined(ZDT_PLAT_YX_SUPPORT_ZTE)// wuxx_20231219 FOR ZTE 
        // [YS*851321111111126*000A*UPLOAD,180] // 3 分钟
        // [YS*851321111111126*000A*UPLOAD,360] // 6 分钟
        // [YS*851321111111126*000B*UPLOAD,1200] // 20 分钟
        // 目前ZTE 服务器开机下发了60 下来// YS 服务器应该是分钟,ZTE 是秒
        g_YX_Net_Receive_UPLOAD_min_interval = min_interval; // g_YX_Net_Receive_UPLOAD_min_interval 仅作为打印日志用

        if (min_interval>=180) // 定位周期必须大于等于180 秒// 不用分钟
        #else
        if(min_interval > 0) 
        #endif
        {
        yx_DB_Set_Rec.time_interval = min_interval;
        }
        YX_DB_SET_ListModify();
        if(g_is_yx_is_locing == FALSE)
        {
            YX_LocReport_TimeStop(pMe);
            SCI_TraceLow("wuxx, YX_Net_Receive_UPLOAD, 01,yx_DB_Set_Rec.time_interval=%d", yx_DB_Set_Rec.time_interval);
            YX_LocReport_TimeSet(yx_DB_Set_Rec.time_interval);
            MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOCREPORT,NULL,0);
        }
    }
    
    ZDT_LOG("YX_Net_Receive_UPLOAD ret=%d,time_interval = %d",ret,yx_DB_Set_Rec.time_interval);

    YX_Net_TCPRespond(g_zdt_phone_imei,"UPLOAD",6);
    
    return 0;
}

int YX_Net_Receive_CENTER(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[MAX_YX_CENTER_NUMBER_SIZE+1] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,MAX_YX_CENTER_NUMBER_SIZE);
    
    if(ret > 0)
    {
        SCI_MEMSET(yx_DB_Set_Rec.center_number,0,MAX_YX_CENTER_NUMBER_SIZE+1);
        SCI_MEMCPY(yx_DB_Set_Rec.center_number,buf,ret);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_CENTER ret =%d, center_number = %s",ret,yx_DB_Set_Rec.center_number);

    YX_Net_TCPRespond(g_zdt_phone_imei,"CENTER",6);
    
    return 0;
}

int YX_Net_Receive_PW(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[MAX_YX_PASSWORD_SIZE+1] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,MAX_YX_PASSWORD_SIZE);
    if(ret > 0)
    {
        SCI_MEMSET(yx_DB_Set_Rec.password,0,MAX_YX_PASSWORD_SIZE+1);
        SCI_MEMCPY(yx_DB_Set_Rec.password,buf,ret);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_PW ret =%d, password = %s",ret,yx_DB_Set_Rec.password);

    YX_Net_TCPRespond(g_zdt_phone_imei,"PW",2);
    
    return 0;
}

int YX_Net_Receive_CALL(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    ret = YX_Func_GetNextPara(&str, &len,buf,40);
    if(ret > 0)
    {
        SCI_MEMSET(g_yx_net_call_number,0,41);
        SCI_MEMCPY(g_yx_net_call_number,buf,ret);
    }
    
    ZDT_LOG("YX_Net_Receive_CALL ret =%d, call_number = %s",ret,g_yx_net_call_number);

    YX_Net_TCPRespond(g_zdt_phone_imei,"CALL",4);
    
    return 0;
}

int YX_Net_Receive_MONITOR(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    ret = YX_Func_GetNextPara(&str, &len,buf,40);
    if(ret > 0)
    {
        SCI_MEMSET(g_yx_net_monitorl_number,0,41);
        SCI_MEMCPY(g_yx_net_monitorl_number,buf,ret);
    }
    else
    {
        len = SCI_STRLEN((char *)yx_DB_Set_Rec.center_number);
        if(len > 0)
        {
            SCI_MEMSET(g_yx_net_monitorl_number,0,41);
            SCI_MEMCPY(g_yx_net_monitorl_number,yx_DB_Set_Rec.center_number,len);
        }
    }
    
    ZDT_LOG("YX_Net_Receive_MONITOR ret =%d, momitor_number = %s",ret,g_yx_net_monitorl_number);
#ifdef ZDT_PLAT_YX_SUPPORT
#if defined(ZDT_PLAT_YX_SUPPORT_VOICE)
#ifdef ZDT_VIDEOCHAT_SUPPORT
    if(!VideoChat_IsInCall())
#endif
    {
        YX_Voice_HandleStop(pMe);
    }
#endif
#endif
#ifndef WIN32    
#ifdef LEBAO_MUSIC_SUPPORT
    lebao_exit();
#endif
#ifdef XYSDK_SUPPORT
        LIBXMLYAPI_AppExit();
#endif
#endif
    ZMTApp_CloseRecordAndPlayer();
    YX_Net_TCPRespond(g_zdt_phone_imei,"MONITOR",7);
#ifdef ZDT_VIDEOCHAT_SUPPORT
    if(!VideoChat_IsInCall())
#endif
    {
        //CC_MakeParentMonitorCall(g_yx_net_monitorl_number);
    }
    
    return 0;
}

#ifdef ZDT_PLAT_YX_SUPPORT_SOS
int YX_Net_Receive_SOS(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen,uint16 idx)
{
    int ret = 0;
    char buf[YX_DB_SOS_MAX_NUMBER_SIZE+1] = {0};
    uint16 buf_len = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 num = 0;
    uint16 i = 0;
    
    ZDT_LOG("YX_Net_Receive_SOS idx=%d",idx);
    if(ContentLen == 0)
    {
        return 0;
    }

    if(idx == 0)
    {
        num = YX_DB_SOS_MAX_SUM;
        for(i = 0; i < num; i++)
        {
            SCI_MEMSET(buf,0,YX_DB_SOS_MAX_NUMBER_SIZE+1);
            ret = YX_Func_GetNextPara(&str, &len,buf,YX_DB_SOS_MAX_NUMBER_SIZE);
            if(ret > 0)
            {
                YX_DB_SOS_ListModifyBuf(i,buf,SCI_STRLEN(buf));
#ifdef SOS_KEYLONGWEB
                if(i == 0)
                {
                    //MMIAPISet_SosCall_Num1(buf);
                }
                else if(i == 1)
                {
                    //MMIAPISet_SosCall_Num2(buf);
                }
                else if(i == 2)
                {
                    //MMIAPISet_SosCall_Num3(buf);
                }
#endif
            }
            else
            {
                YX_DB_SOS_ListDelBuf(i);
#ifdef SOS_KEYLONGWEB
                if(i == 0)
                {
                    //MMIAPISet_SosCall_Num1("");
                }
                else if(i == 1)
                {
                    //MMIAPISet_SosCall_Num2("");
                }
                else if(i == 2)
                {
                    //MMIAPISet_SosCall_Num3("");
                }
#endif
            }
        }
        YX_DB_SOS_ListSaveBuf();
        YX_Net_TCPRespond(g_zdt_phone_imei,"SOS",3);
    }
    else
    {
        if(idx <= YX_DB_SOS_MAX_SUM)
        {
            ret = YX_Func_GetNextPara(&str, &len,buf,YX_DB_SOS_MAX_NUMBER_SIZE);
            if(ret > 0)
            {
                YX_DB_SOS_ListModify(idx-1,buf,SCI_STRLEN(buf));
#ifdef SOS_KEYLONGWEB
                if(idx == 1)
                {
                    //MMIAPISet_SosCall_Num1("");
                }
                else if(i == 2)
                {
                    //MMIAPISet_SosCall_Num2("");
                }
                else if(i == 3)
                {
                    //MMIAPISet_SosCall_Num3("");
                }
#endif
            }
            else
            {
                YX_DB_SOS_ListDel(idx-1);
#ifdef SOS_KEYLONGWEB
                if(i == 1)
                {
                    //MMIAPISet_SosCall_Num1("");
                }
                else if(i == 2)
                {
                    //MMIAPISet_SosCall_Num2("");
                }
                else if(i == 3)
                {
                    //MMIAPISet_SosCall_Num3("");
                }
#endif
            }
            SCI_MEMSET(buf,0,YX_DB_SOS_MAX_NUMBER_SIZE+1);
            sprintf((char *)buf,"SOS%d",idx);
            buf_len = SCI_STRLEN((char *)buf);
            YX_Net_TCPRespond(g_zdt_phone_imei,buf,buf_len);
        }
    }
    return 0;
}

PUBLIC void YX_SOS_GetSosNum(char number[], uint8 index)
{
    if(index < YX_DB_SOS_MAX_SUM)
    {
        if(SCI_STRLEN(yx_DB_Sos_Reclist[index].sos_num) > 0)
        {
            SCI_STRCPY(number, yx_DB_Sos_Reclist[index].sos_num);
            ZDT_LOG("YX_SOS_GetSosNum sos_num[%d] = %s ", index, yx_DB_Sos_Reclist[index].sos_num);
        }
    }
}
#endif
int YX_Net_Receive_IP(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[MAX_YX_IP_DOMAIN_SIZE+1] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,MAX_YX_IP_DOMAIN_SIZE);
    
    if(ret > 0)
    {
        SCI_MEMSET(yx_DB_Set_Rec.ip_domain,0,MAX_YX_IP_DOMAIN_SIZE+1);
        SCI_MEMCPY(yx_DB_Set_Rec.ip_domain,buf,ret);
    }

    SCI_MEMSET(buf,0,MAX_YX_IP_DOMAIN_SIZE+1);
    ret = YX_Func_GetNextPara(&str, &len,buf,MAX_YX_IP_DOMAIN_SIZE);
    if(ret > 0)
    {
        yx_DB_Set_Rec.ip_port = atoi(buf);
    }
    
    YX_DB_SET_ListModify();
    
    ZDT_LOG("YX_Net_Receive_IP ret =%d, ip = %s : %d",ret,yx_DB_Set_Rec.ip_domain,yx_DB_Set_Rec.ip_port);
    
    YX_Net_Reset(pMe);
        
    return 0;
}

int YX_Net_Receive_FACTORY(YX_APP_T *pMe)
{    
    //htNVMgr_RestoreFactorySetting();
    YX_Net_TCPRespond(g_zdt_phone_imei,"FACTORY",7);
    return 0;
}

int YX_Net_Receive_LZ(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[31] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,30);
    if(ret > 0)
    {
        yx_DB_Set_Rec.language = atoi(buf);
    }
    
    SCI_MEMSET(buf,0,31);
    ret = YX_Func_GetNextPara(&str, &len,buf,30);
    if(ret > 0)
    {
        yx_DB_Set_Rec.zone = atoi(buf);
    }
    
    YX_DB_SET_ListModify();
    
    ZDT_LOG("YX_Net_Receive_LZ ret =%d, language = %d, zone=%d",ret,yx_DB_Set_Rec.language,yx_DB_Set_Rec.zone);

    YX_Net_TCPRespond(g_zdt_phone_imei,"LZ",2);
    
    return 0;
}

int YX_Net_Receive_SOSSMS(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.alert_sos_sms = atoi(buf);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_SOSSMS ret=%d,alert_sos_sms = %d",ret,yx_DB_Set_Rec.alert_sos_sms);

    YX_Net_TCPRespond(g_zdt_phone_imei,"SOSSMS",6);
    
    return 0;
}

int YX_Net_Receive_LOWBAT(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.alert_low_bat = atoi(buf);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_LOWBAT ret=%d,alert_low_bat = %d",ret,yx_DB_Set_Rec.alert_low_bat);

    YX_Net_TCPRespond(g_zdt_phone_imei,"LOWBAT",6);
    
    return 0;
}

int YX_Net_Receive_VERNO(YX_APP_T *pMe)
{
    ZDT_LOG("YX_Net_Receive_VERNO Start");
    YX_Net_TCPRespond(g_zdt_phone_imei,"VERNO,V2.0",10);
    return 0;
}

int YX_Net_Receive_RESET(YX_APP_T *pMe)
{
    ZDT_LOG("YX_Net_Receive_RESET");
    MMIAPIPHONE_PowerReset();
    #if 0
    if(pMe->m_zdt_is_in_call == 0)
    {
        //phoneState = PHONE_POWER_OFF;
        //clk_busy_wait(1500000);
        //hw_reset();
    }
    #endif
    return 0;
}

PUBLIC int YX_SMS_Receive_CR(YX_APP_T *pMe)
{
    ZDT_LOG("YX_SMS_Receive_CR");
    //YX_Net_TCPRespond(g_zdt_phone_imei,"CR",2);
    YX_LocReport_TimeStop(pMe);
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)
    YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_CR,NULL,0);
#else
    YX_Net_Send_UD(pMe,0);
#endif
    //省电去掉
    //g_is_yx_is_locing = TRUE;
    //g_yx_loc_timer_interval = 10;
    //g_yx_loc_timer_lk_num = 0;
    //YX_LocCR_TimeStart(pMe,180);
    return 0;
}

int YX_Net_Receive_CR(YX_APP_T *pMe)
{
    ZDT_LOG("YX_Net_Receive_CR");
    YX_Net_TCPRespond(g_zdt_phone_imei,"CR",2);
    YX_LocReport_TimeStop(pMe);
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)
    YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_CR,NULL,0);
#else
    YX_Net_Send_UD(pMe,0);
#endif
    //省电去掉
    //g_is_yx_is_locing = TRUE;
    //g_yx_loc_timer_interval = 10;
    //g_yx_loc_timer_lk_num = 0;
    //YX_LocCR_TimeStart(pMe,180);
    return 0;
}

void YX_Net_GJ_TimeOut(
                                uint8 timer_id,
                                uint32 param
                                )
{
    MMIAPICC_QuitForPowerOff();
    //MMIAPIPHONE_PowerOff();
    MMIAPIPHONE_PowerOffEx();
    return;
}

int YX_Net_Receive_POWEROFF(YX_APP_T *pMe)
{
    ZDT_LOG("YX_Net_Receive_POWEROFF g_is_yx_power_off_ing=%d",g_is_yx_power_off_ing);
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    
    YX_Net_TCPRespond(g_zdt_phone_imei,"CR",2);
    g_is_yx_power_off_ing = TRUE;
    if(0 != pMe->m_yx_gj_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_gj_timer_id);
        pMe->m_yx_gj_timer_id = 0;
    }
#ifndef WIN32    
#ifdef LEBAO_MUSIC_SUPPORT
    lebao_exit();
#endif
#ifdef XYSDK_SUPPORT
        LIBXMLYAPI_AppExit();
#endif
#endif    
    ZMTApp_CloseRecordAndPlayer();
    pMe->m_yx_gj_timer_id = MMK_CreateTimerCallback(3*1000, 
                                                                        YX_Net_GJ_TimeOut, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
    return 0;
}

int YX_Net_Receive_REMOVE(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.alert_loser = atoi(buf);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_REMOVE ret=%d,alert_loser = %d",ret,yx_DB_Set_Rec.alert_loser);

    YX_Net_TCPRespond(g_zdt_phone_imei,"REMOVE",6);
    
    return 0;
}

int YX_Net_Receive_REMOVESMS(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.alert_loser_sms = atoi(buf);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_REMOVESMS ret=%d,alert_loser_sms = %d",ret,yx_DB_Set_Rec.alert_loser_sms);

    YX_Net_TCPRespond(g_zdt_phone_imei,"REMOVESMS",9);
    
    return 0;
}

int YX_Net_Receive_PEDO(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.alert_jp = atoi(buf);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_PEDO ret=%d,alert_jp = %d",ret,yx_DB_Set_Rec.alert_jp);

    YX_Net_TCPRespond(g_zdt_phone_imei,"PEDO",4);
    
    return 0;
}

int YX_Net_Receive_WALKTIME(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    uint8 timer_type = 0;
    uint32 timer_start = 0;
    uint32 timer_end = 0;
    char tmp_str[51] = {0};
    char timer_str[11] = {0};
    uint8 start_hour = 0;
    uint8 start_min = 0;
    uint8 end_hour = 0;
    uint8 end_min = 0;
    
    ZDT_LOG("YX_Net_Receive_WALKTIME");

    YX_DB_TIMER_ListDel(YX_TIME_TYPE_JP_1);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_JP_2);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_JP_3);
    for(i = YX_TIME_TYPE_JP_1; i <= YX_TIME_TYPE_JP_3; i++)
    {
        start_hour = 0;
        start_min = 0;
        end_hour = 0;
        end_min = 0;
        SCI_MEMSET(tmp_str,0,51);
        ret = YX_Func_GetNextPara(&str, &len,tmp_str,50);
        if(ret < 0)
        {
            break;
        }
        if(ret > 5)
        {
            pTime = (uint8 *)&tmp_str[0];
            con_len = ret;
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_min = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_min = atoi(timer_str);
            }
            
            timer_start = (start_hour * 3600) + (start_min * 60);
            timer_end = (end_hour * 3600) + (end_min * 60);
            timer_type = 0xFF;
            ZDT_LOG("YX_Net_Receive_WALKTIME Rcv idx=%d,timer=%ld-%ld,type=0x%x",i,timer_start,timer_end,timer_type);
            YX_DB_TIMER_ListModify(i,timer_type,timer_start,timer_end);
        }
    }    
    YX_DB_TIMER_ListSave();

    YX_Net_TCPRespond(g_zdt_phone_imei,"WALKTIME",8);
    
    return 0;
}

int YX_Net_Receive_SLEEPTIME(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    uint8 timer_type = 0;
    uint32 timer_start = 0;
    uint32 timer_end = 0;
    char tmp_str[51] = {0};
    char timer_str[11] = {0};
    uint8 start_hour = 0;
    uint8 start_min = 0;
    uint8 end_hour = 0;
    uint8 end_min = 0;
    
    ZDT_LOG("YX_Net_Receive_SLEEPTIME");

    YX_DB_TIMER_ListDel(YX_TIME_TYPE_FG_1);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_FG_2);
    for(i = YX_TIME_TYPE_FG_1; i <= YX_TIME_TYPE_FG_2; i++)
    {
        start_hour = 0;
        start_min = 0;
        end_hour = 0;
        end_min = 0;
        SCI_MEMSET(tmp_str,0,51);
        ret = YX_Func_GetNextPara(&str, &len,tmp_str,50);
        if(ret < 0)
        {
            break;
        }
        if(ret > 5)
        {
            pTime = (uint8 *)tmp_str[0];
            con_len = ret;
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_min = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_min = atoi(timer_str);
            }
            
            timer_start = (start_hour * 3600) + (start_min * 60);
            timer_end = (end_hour * 3600) + (end_min * 60);
            timer_type = 0xFF;
            ZDT_LOG("YX_Net_Receive_SLEEPTIME Rcv idx=%d,timer=%ld-%ld,type=0x%x",i,timer_start,timer_end,timer_type);
            YX_DB_TIMER_ListModify(i,timer_type,timer_start,timer_end);
        }
    }    
    
    YX_DB_TIMER_ListSave();
    YX_Net_TCPRespond(g_zdt_phone_imei,"SLEEPTIME",9);
    
    return 0;
}

//yangyu add begin

PUBLIC BOOLEAN YX_IsInClassSilentTime()
{
    BOOLEAN ret = FALSE;
    uint16 i = 0;
    for(i = YX_TIME_TYPE_SLIENT_1; i <= YX_TIME_TYPE_SLIENT_5; i++)
    {
       ret = YX_DB_TIMER_IsValid(i);
       if(ret)
       {
            break;
       }

    }
    ZDT_LOG("YX_IsInClassSilentTime ret=%d", ret);
    return ret;
    
}


PUBLIC void YX_CheckClassSilentOnOff()
{
    ZDT_LOG("YX_CheckClassSilentOnOff");
    if(YX_IsInClassSilentTime())
    {
        MMIAPIENVSET_ActiveMode(MMIENVSET_SILENT_MODE);
        //MMIZDT_CheckOpenClassModeWin();
        MMIZDT_OpenClassModeWin();
    }
    else
    {
        MMK_CloseClassModeWin();
        MMIAPIENVSET_ActiveMode(MMIENVSET_STANDARD_MODE);
    }
}

//yangyu end

int YX_Net_Receive_SILENCETIME(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    uint8 timer_type = 0;
    uint32 timer_start = 0;
    uint32 timer_end = 0;
    char tmp_str[51] = {0};
    char timer_str[11] = {0};
    uint8 start_hour = 0;
    uint8 start_min = 0;
    uint8 end_hour = 0;
    uint8 end_min = 0;
    
    ZDT_LOG("YX_Net_Receive_SILENCETIME");

    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_1);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_2);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_3);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_4);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_5);
    for(i = YX_TIME_TYPE_SLIENT_1; i <= YX_TIME_TYPE_SLIENT_5; i++)
    {
        start_hour = 0;
        start_min = 0;
        end_hour = 0;
        end_min = 0;
        SCI_MEMSET(tmp_str,0,51);
        ret = YX_Func_GetNextPara(&str, &len,tmp_str,50);
        if(ret < 0)
        {
            break;
        }
        if(ret > 5)
        {
            pTime = (uint8 *)&tmp_str[0];
            con_len = ret;
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_min = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_min = atoi(timer_str);
            }
            
            timer_start = (start_hour * 3600) + (start_min * 60);
            timer_end = (end_hour * 3600) + (end_min * 60);
            timer_type = 0x3F;
            ZDT_LOG("YX_Net_Receive_SILENCETIME Rcv idx=%d,timer=%ld-%ld,type=0x%x",i,timer_start,timer_end,timer_type);
            YX_DB_TIMER_ListModify(i,timer_type,timer_start,timer_end);
        }
    }    
    
    YX_DB_TIMER_ListSave();
    YX_Net_TCPRespond(g_zdt_phone_imei,"SILENCETIME",11);
    
    return 0;
}

int YX_Net_Receive_SILENCETIME2(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    uint8 timer_type = 0;
    uint32 timer_start = 0;
    uint32 timer_end = 0;
    char tmp_str[51] = {0};
    char timer_str[11] = {0};
    uint8 start_hour = 0;
    uint8 start_min = 0;
    uint8 end_hour = 0;
    uint8 end_min = 0;
    uint8 is_on = 0;
    uint8 week_type = 0;
    
    ZDT_LOG("YX_Net_Receive_SILENCETIME2");

    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_1);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_2);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_3);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_4);
    YX_DB_TIMER_ListDel(YX_TIME_TYPE_SLIENT_5);
    for(i = YX_TIME_TYPE_SLIENT_1; i <= YX_TIME_TYPE_SLIENT_5; i++)
    {
        start_hour = 0;
        start_min = 0;
        end_hour = 0;
        end_min = 0;
        is_on = 0;
        week_type = 0;
        SCI_MEMSET(tmp_str,0,51);
        ret = YX_Func_GetNextPara(&str, &len,tmp_str,50);
        if(ret < 0)
        {
            break;
        }
        if(ret > 5)
        {
            pTime = (uint8 *)&tmp_str[0];
            con_len = ret;
            SCI_MEMSET(timer_str,0,11);
            timer_type = 0;
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            //获取格式: 日一二三四五六
            if(ret > 6)
            {
                
                //转换为: 六五四三二一日ONOFF
                if(timer_str[0] == '1')
                {
                    timer_type = (0x01 << 1);
                }
                
                for(j = 1; j < 7; j++)
                {
                    if(timer_str[j] == '1')
                    {
                        timer_type = timer_type | (0x01 << (j+1));
                    }
                }
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);//开关
            if(ret > 0)
            {
                is_on = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);//am pm
            if(ret > 0)
            {
                ZDT_LOG("YX_Net_Receive_SILENCETIME2 Rcv idx=%d,am/pm=%s",i,timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                start_min = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_TimerGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                end_min = atoi(timer_str);
            }
            timer_start = (start_hour * 3600) + (start_min * 60);
            timer_end = (end_hour * 3600) + (end_min * 60);
            if(is_on == 1) //防止开关状态为2
            {
                timer_type = timer_type | 0x01;
            }
            else
            {
                timer_type = timer_type & 0xFE;
            }
            ZDT_LOG("YX_Net_Receive_SILENCETIME2 Rcv idx=%d(%d),timer=%ld-%ld,type=0x%x",i,is_on,timer_start,timer_end,timer_type);
            YX_DB_TIMER_ListModify(i,timer_type,timer_start,timer_end);
        }
    }    
    
    YX_DB_TIMER_ListSave();
    YX_Net_TCPRespond(g_zdt_phone_imei,"SILENCETIME2",12);
    
    return 0;
}

#ifdef WIN32
PUBLIC void YX_Net_SILENCETIME2_Test()
{
    uint8 * pContent = "1111111-2-am-11:29-11:29,1111111-1-pm-14:29-23:29,1111111-1-am-11:29-11:35,0000111-1-pm-16:29-19:29,0011111-1-am-07:29-11:29,0011111-1-pm-15:29-23:29]";
    uint32 len = 0;
    YX_APP_T *pMe = &g_yx_app;
    len = strlen(pContent);
    YX_Net_Receive_SILENCETIME2(pMe,pContent,len);
}
#endif

PUBLIC void YX_SMS_Receive_Find(YX_APP_T *pMe)
{
    MMIDEFAULT_TurnOnLCDBackLight();
    MMIZDT_FindWatchWin();
    //yangyu end
    
    //g_is_yx_czsb_is_playing = TRUE;//yangyu modify
   //ZdtWatch_Factory_Speaker_Start();//yangyu modify
#ifdef _SW_ZDT_PRODUCT_
   if(!MMIZDT_IsClassModeWinOpen())
#endif
   {
       g_is_yx_czsb_is_playing = TRUE;
       ZdtWatch_Factory_Speaker_Start();
   }
   //yangyu end
    YX_Find_TimeStart(pMe,60);

}

int YX_Net_Receive_FIND(YX_APP_T *pMe)
{
    ZDT_LOG("YX_Net_Receive_FIND");
    YX_Net_TCPRespond(g_zdt_phone_imei,"FIND",4);
#ifdef XYSDK_SUPPORT
    LIBXMLYAPI_AppExit();
#endif
    ZMTApp_CloseRecordAndPlayer();
    //if(pMe->m_zdt_is_in_call == 0)
    {
        //yangyu add begin
        MMIDEFAULT_TurnOnLCDBackLight();
        MMIZDT_FindWatchWin();
        //yangyu end
        
        //g_is_yx_czsb_is_playing = TRUE;//yangyu modify
       //ZdtWatch_Factory_Speaker_Start();//yangyu modify
#ifdef _SW_ZDT_PRODUCT_
       if(!MMIZDT_IsClassModeWinOpen())
#endif
       {
           g_is_yx_czsb_is_playing = TRUE;
           ZdtWatch_Factory_Speaker_Start();
       }
       //yangyu end
        YX_Find_TimeStart(pMe,60);
    }
    return 0;
}

int YX_Net_Receive_FLOWER(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.flower = atoi(buf);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_REMOVE ret=%d,flower = %d",ret,yx_DB_Set_Rec.flower);

    YX_Net_TCPRespond(g_zdt_phone_imei,"FLOWER",6);
    
    return 0;
}

//yangyu add auto power on off
/*
idx:        0 poweron , 1 power off
clock_type: frequency 
*/
static BOOLEAN YX_AutoPowerOnOffClock_SaveNV(uint16 idx,uint16 Hour,uint16 Minute,uint8 onoff,uint8 clock_type,uint8 clock_week)
{

    uint8 i = 0;
    uint32 pwrofftime = 0;
    uint8 is_on = onoff;
    uint16 cur_id = EVENT_AUTO_POWERON;
    MMIACC_SMART_EVENT_E type = SMART_EVENT_AUTOPOWERON;
    uint32          cur_seconds = 0;
    uint32          res_seconds = 0;
    SCI_TIME_T          time;
    SCI_DATE_T          date;
    TM_GetSysDate(&date);
    TM_GetSysTime(&time);

    if(idx > 1)
    {
        ZDT_LOG("YX_AutoPowerOnOffClock_SaveNV idx wrong");
        return FALSE;
    }
    if(idx == 0)
    {
        cur_id = EVENT_AUTO_POWERON;
        type = SMART_EVENT_AUTOPOWERON;
    }
    else if(idx == 1)
    {
        cur_id = EVENT_AUTO_POWEROFF;
        type = SMART_EVENT_AUTOPOWEROFF;
    }

    

    if(Hour >= 24)
    {
        return FALSE;
    }
    if(Minute >= 60)
    {
        return FALSE;
    }
    SCI_MEMSET(&s_yx_current_event_info,0,sizeof(EVENT_CURRENT_DATA_T));
    
    s_yx_current_event_info.id = cur_id;
    s_yx_current_event_info.type = type;
    s_yx_current_event_info.event.event_fast_info.type = type;

    MMIAPIALM_FormInteEventInfo(s_yx_current_event_info.id,&s_yx_current_event_info.event);
    //MMIAPIALM_GetRingInfo(s_yx_current_event_info.id,&s_yx_current_event_info.event);
    
    //s_yx_current_event_info.type = SMART_EVENT_ALARM;
    //s_yx_current_event_info.event.event_fast_info.is_valid = TRUE;
    if(onoff)
    {
        s_yx_current_event_info.event.event_fast_info.is_on = TRUE;
    }
    else
    {
        s_yx_current_event_info.event.event_fast_info.is_on = FALSE;
    }
    //s_yx_current_event_info.event.event_fast_info.snooze_type = ALM_SNOOZE_TIME_10_MIN;
    s_yx_current_event_info.event.event_fast_info.hour = Hour;
    s_yx_current_event_info.event.event_fast_info.minute = Minute;

    //s_yx_current_event_info.event.event_fast_info.year = date.year;
    //s_yx_current_event_info.event.event_fast_info.mon = date.mon;
    //s_yx_current_event_info.event.event_fast_info.day = date.mday;

    #if 0 
    if(clock_type == 1)
    {
        //一次
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_ONCE;
        
    }
    else if(clock_type == 2)
    {
        //每天
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_EVERYDAY;
    }
    else if(clock_type == 3)
    {
        //自定义
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_EVERYWEEK;
        s_yx_current_event_info.event.event_fast_info.fre = clock_week;
    }
    else
    {
        //一次
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_ONCE;
    }
    #endif
    s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_EVERYDAY;
    s_yx_current_event_info.event.event_fast_info.fre = clock_week;

    //s_yx_current_event_info.event.event_content_info.ring = 1;
    //s_yx_current_event_info.event.event_content_info.is_add = TRUE;
    //s_yx_current_event_info.event.event_content_info.w_length = 3;
    //s_yx_current_event_info.event.event_content_info.w_content[0] = 0x95F9;
    //s_yx_current_event_info.event.event_content_info.w_content[1] = 0x949F;
    //s_yx_current_event_info.event.event_content_info.w_content[2] = 0x0031+idx;
    // SetDelayAlarmExt(s_yx_current_event_info.id, &s_yx_current_event_info.event, FALSE);
     // 修改闹钟设置
     MMIAPIALM_SaveInteEventInfo(s_yx_current_event_info.id,&s_yx_current_event_info.event);    
     //MMIALARM_ModifyOneAlarm( cur_id, &s_yx_current_event_info.event.event_fast_info);
    // MMIALM_ModifyEvent(s_yx_current_event_info.id, &s_current_event_info.event.event_fast_info); // no need

    return TRUE;
}

//yangyu end


static BOOLEAN YX_Clock_SaveNV(uint16 idx,uint16 Hour,uint16 Minute,uint8 onoff,uint8 clock_type,uint8 clock_week)
{
    uint8 i = 0;
    uint32 pwrofftime = 0;
    uint8 is_on = onoff;
    uint16 cur_id = idx+EVENT_ALARM_0;
    uint32          cur_seconds = 0;
    uint32          res_seconds = 0;
    SCI_TIME_T          time;
    SCI_DATE_T          date;
    TM_GetSysDate(&date);
    TM_GetSysTime(&time);
    

    if(Hour >= 24)
    {
        return FALSE;
    }
    if(Minute >= 60)
    {
        return FALSE;
    }
    SCI_MEMSET(&s_yx_current_event_info,0,sizeof(EVENT_CURRENT_DATA_T));
    
    s_yx_current_event_info.id = cur_id;

    MMIAPIALM_FormInteEventInfo(s_yx_current_event_info.id,&s_yx_current_event_info.event);
    
    s_yx_current_event_info.type = SMART_EVENT_ALARM;
    s_yx_current_event_info.event.event_fast_info.type = SMART_EVENT_ALARM;
    s_yx_current_event_info.event.event_fast_info.is_valid = TRUE;
    if(onoff)
    {
        s_yx_current_event_info.event.event_fast_info.is_on = TRUE;
    }
    else
    {
        s_yx_current_event_info.event.event_fast_info.is_on = FALSE;
    }
    //s_yx_current_event_info.event.event_fast_info.snooze_type = ALM_SNOOZE_TIME_10_MIN;
    s_yx_current_event_info.event.event_fast_info.hour = Hour;
    s_yx_current_event_info.event.event_fast_info.minute = Minute;

#if 0
    cur_seconds = MMIAPICOM_Tm2Second(time.sec, time.min, time.hour, date.mday, date.mon, date.year);
    res_seconds = MMIAPICOM_Tm2Second(0, Minute, Hour, date.mday, date.mon, date.year);

    if(cur_seconds > res_seconds)
    {
        //推迟一天
        
    }
#endif

    s_yx_current_event_info.event.event_fast_info.year = date.year;
    s_yx_current_event_info.event.event_fast_info.mon = date.mon;
    s_yx_current_event_info.event.event_fast_info.day = date.mday;
    
    if(clock_type == 1)
    {
        //一次
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_ONCE;
        
    }
    else if(clock_type == 2)
    {
        //每天
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_EVERYDAY;
    }
    else if(clock_type == 3)
    {
        //自定义
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_EVERYWEEK;
        s_yx_current_event_info.event.event_fast_info.fre = clock_week;
    }
    else
    {
        //一次
        s_yx_current_event_info.event.event_fast_info.fre_mode = ALM_MODE_ONCE;
    }

    s_yx_current_event_info.event.event_content_info.ring = 1;
    //s_yx_current_event_info.event.event_content_info.is_add = TRUE;
    s_yx_current_event_info.event.event_content_info.w_length = 3;
    s_yx_current_event_info.event.event_content_info.w_content[0] = 0x95F9;
    s_yx_current_event_info.event.event_content_info.w_content[1] = 0x949F;
    s_yx_current_event_info.event.event_content_info.w_content[2] = 0x0031+idx;
    SetDelayAlarmExt(s_yx_current_event_info.id, &s_yx_current_event_info.event, FALSE);
     // 修改闹钟设置
    // MMIAPIALM_SaveInteEventInfo(s_yx_current_event_info.id,&s_yx_current_event_info.event);    

     MMIALARM_ModifyOneAlarm( cur_id, &s_yx_current_event_info.event.event_fast_info);

    return TRUE;
}

//yangyu add begin, auto power on off
int atoi_by_len(const char *str , const uint32 len)
{
    char str2[256] = {0};
    uint32 i = 0;
    int ret = 0;
    

    if(str == NULL || len > strlen(str) || len == 0)
    {
        return 0;
    }
    
    for(i = 0 ; i < len ;i++)
    {
        str2[i] = str[i];
    }
    ret = atoi(str2);

    return ret;
    

}

//yangyu add begin
PUBLIC void ZDT_NV_SetFirstBind(BOOLEAN isOn)
{
    BOOLEAN status = isOn;
    MMINV_WRITE(ZDT_FIRST_BIND, &status);
}

PUBLIC BOOLEAN ZDT_NV_GetFirstBinded()
{
    BOOLEAN status = FALSE;
    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;

    MMINV_READ(ZDT_FIRST_BIND, &status, return_value);
    
    if (MN_RETURN_SUCCESS != return_value)
    {
        MMINV_WRITE(ZDT_FIRST_BIND, &status);
    }
    ZDT_LOG("ZDT_NV_GetFirstBinded status = %d " ,status );
    return status;
}

extern int yx_auto_power_on_rand(void)
{
    static int ali_rand_offset = 27458;
    int radom = 0;
    srand(SCI_GetTickCount()+ali_rand_offset);
    ali_rand_offset++;
    radom = rand();
    SCI_TRACE_LOW("yx_auto_power_on_rand radom=%d",radom);
    return radom;
}

PUBLIC void YX_SetDefaultBootOnShutDownTime()
{
    ZDT_LOG("YX_SetDefaultBootOnShutDownTime");
    if(!ZDT_NV_GetFirstBinded())
    {
        uint16 Hour = 0;
        uint16 Minute = 0;
        int radom = yx_auto_power_on_rand();
        if(radom < 0)
        {
            radom = 0 - radom;
        }
        
        Minute = radom%60;
        if((radom%2) == 0)
        {
            //偶数加
            Hour = 6;
        }
        else
        {
            //奇数减
            Hour = 5;
        }
            
        ZDT_NV_SetFirstBind(TRUE);
        YX_AutoPowerOnOffClock_SaveNV(0, Hour, Minute, 1, 3, 127);
        YX_AutoPowerOnOffClock_SaveNV(1, 23, 0, 1, 3, 127);
    }
}

//yangyu end

int YX_Net_Receive_BOOTOFF(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 * pTime = NULL;
    uint16 con_len = 0;
    uint8 clock_idx = 0;
    uint8 boot_hour = 0;
    uint8 boot_min = 0;
    uint8 shutdown_hour = 0;
    uint8 shutdown_min = 0;
    uint8 clock_onoff = 0;
    uint8 clock_type = 3;//every week
    uint8 clock_week = 0;
    uint32 clock_time = 0;
    char tmp_str[101] = {0};
    char timer_str[11] = {0};
    //nv_alarmclk_save_type alarm_clk[ALARM_CLOCK_NUMBER];

    //htNVMgr_GetGroupAlarmclksType(alarm_clk);

    //MMIZDT_RemoveAllAlarm();//add by James li
    //yangyu add
    ZDT_LOG("YX_Net_Receive_BOOTOFF str = %s , len=%d" ,pContent ,ContentLen);
    SCI_MEMSET(tmp_str,0,101);
    ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
    if(ret != 1)
    {
        ZDT_LOG("YX_Net_Receive_BOOTOFF error data 1");
        return 1; 
    }
    
    clock_onoff = atoi(tmp_str);
    if(clock_onoff > 1  )
    {
        ZDT_LOG("YX_Net_Receive_BOOTOFF clock_onoff over");
        return 1; 
    }
    SCI_MEMSET(tmp_str,0,101);
    ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
    if(ret != 5)
    {
        ZDT_LOG("YX_Net_Receive_BOOTOFF error data 2");
        return 1; 
    }
    boot_hour = atoi_by_len(tmp_str, 2);
    boot_min = atoi_by_len(tmp_str+3, 2);
    SCI_MEMSET(tmp_str,0,101);
    ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
    if(ret != 5)
    {
        ZDT_LOG("YX_Net_Receive_BOOTOFF error data 3");
        return 1; 
    }
    shutdown_hour = atoi_by_len(tmp_str, 2);
    shutdown_min = atoi_by_len(tmp_str+3, 2);
    SCI_MEMSET(tmp_str,0,101);
    ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
    if(ret != 7)
    {
        ZDT_LOG("YX_Net_Receive_BOOTOFF error data 4");
        //return 1; 
    }
   
    for(j = 0; j < 7; j++)
    {
        if(tmp_str[j] == '1')
        {
            clock_week = clock_week | (0x01 << j);
        }
    }
    ZDT_LOG("YX_Net_Receive_BOOTOFF %d %d:%d  %d:%d  %d  %d", clock_onoff, boot_hour,boot_min ,shutdown_hour, shutdown_min, clock_type, clock_week );
    YX_AutoPowerOnOffClock_SaveNV(0, boot_hour, boot_min, clock_onoff, clock_type, clock_week);
    YX_AutoPowerOnOffClock_SaveNV(1, shutdown_hour, shutdown_min, clock_onoff, clock_type, clock_week);

    ZDT_LOG("YX_Net_Receive_BOOTOFF Set End");
    YX_Net_TCPRespond(g_zdt_phone_imei,"BOOTOFF",7);
    return 0;

    
    //yangyu end

}

    
//yangyu end

#ifdef WIN32
PUBLIC void YX_AddTestPowerOffAlarm()
{
    uint8 * pContent = "1,01:05,01:03,1111111]";
    uint32 len = 0;
    YX_APP_T *pMe = &g_yx_app;
    len = strlen(pContent);
    
    YX_Net_Receive_BOOTOFF(pMe, pContent, len);

}
#endif

#ifdef WIN32
PUBLIC void YX_AddTestAlarm()
{
    uint8 * pContent = ",09:00-1-3-0111110,19:20-0-3-0111110]";
    uint32 len = 0;
    YX_APP_T *pMe = &g_yx_app;
    len = strlen(pContent);
    
    YX_Net_Receive_REMIND(pMe, pContent, len);

}
#endif

//[YS*312020050100001*002A*REMIND,08:00-1-3-0111110,15:08-1-3-0111110]
int YX_Net_Receive_REMIND2(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    uint8 clock_idx = 0;
    uint8 clock_hour = 0;
    uint8 clock_min = 0;
    uint8 clock_onoff = 0;
    uint8 clock_type = 0;
    uint8 clock_week = 0;
    uint32 clock_time = 0;
    char tmp_str[101] = {0};
    char timer_str[11] = {0};

    MMIALARM_DeleteAllAlarm();
#ifdef YX_DB_NEED_CLOCK
    for(i = 0; i < EVENT_ALARM_MAX; i++)
#else
    for(i = 0; i < 3; i++)
#endif
    {
        clock_hour = 0;
        clock_min = 0;
        clock_onoff = 0;
        clock_type = 0;
        clock_week = 0;
        SCI_MEMSET(tmp_str,0,101);
        ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
        if(ret < 0)
        {
            clock_idx = i;
            //alarm_clk[clock_idx].onoff = FALSE;
#ifdef YX_DB_NEED_CLOCK
            YX_DB_CLOCK_ListDelBuf(i);
#endif
            continue;
        }
        
        if(ret > 5)
        {
            clock_idx = i;
            pTime = (uint8 *)&tmp_str[0];
            con_len = ret;
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_min = atoi(timer_str);
            }
            
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_onoff = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_type = atoi(timer_str);
            }
            
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            
            //获取格式: 日一二三四五六
            if(ret > 6)
            {
                //转换为:六五四三二一日
                for(j = 0; j < 7; j++)
                {
                    if(timer_str[j] == '1')
                    {
                        clock_week = clock_week | (0x01 << j);
                    }
                }
            }
            
            clock_time = clock_hour;
            clock_time = (clock_time<<8);
            clock_time += clock_min;
            ZDT_LOG("YX_Net_Receive_REMIND Rcv clock_idx=%d(%d),clock_hour=%d,clock_min=%d,clock_type=%d,%s",clock_idx,clock_onoff,clock_hour,clock_min,clock_type,timer_str);
#ifdef YX_DB_NEED_CLOCK
            YX_DB_CLOCK_ListModifyBuf(i,clock_onoff,clock_type,clock_week,clock_time);
#endif
            YX_Clock_SaveNV(clock_idx,clock_hour,clock_min,clock_onoff,clock_type,clock_week);  
        }
    }
#ifdef YX_DB_NEED_CLOCK
    YX_DB_CLOCK_ListSaveBuf();
#endif
    ZDT_LOG("YX_Net_Receive_REMIND Set End");
    YX_Net_TCPRespond(g_zdt_phone_imei,"REMIND2",6);
    return 0;
}

//[YS*312020050100001*002A*REMIND,08:00-1-3-0111110,15:08-1-3-0111110]
int YX_Net_Receive_REMIND(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    uint8 clock_idx = 0;
    uint8 clock_hour = 0;
    uint8 clock_min = 0;
    uint8 clock_onoff = 0;
    uint8 clock_type = 0;
    uint8 clock_week = 0;
    uint32 clock_time = 0;
    char tmp_str[101] = {0};
    char timer_str[11] = {0};

    MMIALARM_DeleteAllAlarm();
#ifdef YX_DB_NEED_CLOCK
    for(i = 0; i < EVENT_ALARM_MAX; i++)
#else
    for(i = 0; i < 3; i++)
#endif
    {
        clock_hour = 0;
        clock_min = 0;
        clock_onoff = 0;
        clock_type = 0;
        clock_week = 0;
        SCI_MEMSET(tmp_str,0,101);
        ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
        if(ret < 0)
        {
            clock_idx = i;
            //alarm_clk[clock_idx].onoff = FALSE;
#ifdef YX_DB_NEED_CLOCK
            YX_DB_CLOCK_ListDelBuf(i);
#endif
            continue;
        }
        
        if(ret > 5)
        {
            clock_idx = i;
            pTime = (uint8 *)&tmp_str[0];
            con_len = ret;
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_hour = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_min = atoi(timer_str);
            }
            
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_onoff = atoi(timer_str);
            }
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            if(ret > 0)
            {
                clock_type = atoi(timer_str);
            }
            
            SCI_MEMSET(timer_str,0,11);
            ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
            
            //获取格式: 日一二三四五六
            if(ret > 6)
            {
                //转换为:六五四三二一日
                for(j = 0; j < 7; j++)
                {
                    if(timer_str[j] == '1')
                    {
                        clock_week = clock_week | (0x01 << j);
                    }
                }
            }
            
            clock_time = clock_hour;
            clock_time = (clock_time<<8);
            clock_time += clock_min;
            ZDT_LOG("YX_Net_Receive_REMIND Rcv clock_idx=%d(%d),clock_hour=%d,clock_min=%d,clock_type=%d,%s",clock_idx,clock_onoff,clock_hour,clock_min,clock_type,timer_str);
#ifdef YX_DB_NEED_CLOCK
            YX_DB_CLOCK_ListModifyBuf(i,clock_onoff,clock_type,clock_week,clock_time);
#endif
            if(clock_idx == 0 && clock_onoff == 0 && clock_hour ==0 && clock_min == 0 && clock_type == 0)
            {
                  ZDT_LOG("YX_Net_Receive_REMIND Rcv clock is empty");
            }
            else
            {
                YX_Clock_SaveNV(clock_idx,clock_hour,clock_min,clock_onoff,clock_type,clock_week);  
            }
        }
    }
#ifdef YX_DB_NEED_CLOCK
    YX_DB_CLOCK_ListSaveBuf();
#endif
    ZDT_LOG("YX_Net_Receive_REMIND Set End");
    YX_Net_TCPRespond(g_zdt_phone_imei,"REMIND",7);
    return 0;
}

#if 1
/*[YS*869251026788788*002C*STK,569c86a7375b071c05820e7a,-1,7A817A817A81]*/
/*[YS*869251026788788*163A*STK,569c86a7375b071c05820e7a,3,#!AMR*/
int YX_Net_Receive_STK(YX_APP_T *pMe,uint8 * pContent,uint32 ContentLen)
{
    int ret = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint32 offset = 0;
    int msg_type = 0;
    char group_id[101] = {0};
    char tmp_str[101] = {0};
    YX_GROUP_INFO_DATA_T * pInfo = PNULL;
    uint message_id = 0;
    char respond[32] = {0};
    uint8 success = 0;
    
    ZDT_LOG("ZDT__LOG YX_Net_Receive_STK ContentLen=%d",ContentLen);
    //获取ID
    ret = YX_Func_GetNextPara(&str, &len,group_id,100);
    if(ret > 0)
    {
        offset = ret+1;
    }
    if(len == 1)
    {
        ZDT_LOG("ZDT__LOG YX_Net_Receive_STK RSP %d",str[0]);
        return -1;
    }
    if(0==YX_MMI_Get_Bingd_Statues())
    {
        //未绑定时不接收微聊消息
        if(s_yx_is_first_get_band_info)
        {
            YX_Net_TCPRespond(g_zdt_phone_imei,"STK,1",5);
        }
        else
        {
            s_yx_is_first_need_stkq = 1;
        }
        ZDT_LOG("ZDT__LOG YX_Net_Receive_STK ERR BIND %d",s_yx_is_first_get_band_info);
        return -1;
    }
    ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
    if(ret > 0 && ret < 4)
    {
        msg_type = atoi(tmp_str);
        offset += ret;
        offset += 1;
    }
    if(msg_type == -10)//发送语音服务器回复成功
    {
        ZDT_LOG("ZDT__LOG YX_Net_Receive_STK response ok");
        return 0;
    }
    pInfo = YX_VCHAT_GetGroupInfoByID(group_id);
    if(pInfo == PNULL)
    {
        if(s_yx_is_first_get_all_info)
        {
            s_yx_is_vchat_stk_err_time++;
            if(s_yx_is_vchat_stk_err_time > 3)
            {
                YX_Net_TCPRespond(g_zdt_phone_imei,"STK,1",5);
                YX_Net_Send_STKQ(pMe);
                s_yx_is_vchat_stk_err_time = 0;
            }
            else
            {
                YX_Net_Send_UPGCUL(pMe);
                YX_Net_Send_STKQ(pMe);
            }
        }
        else
        {
            s_yx_is_first_need_stkq = 2;
        }
        ZDT_LOG("ZDT__LOG YX_Net_Receive_STK ERR GROUP %d",s_yx_is_first_get_all_info);
        return -1;
    }
    s_yx_is_first_need_stkq = 0;
    s_yx_is_vchat_stk_err_time = 0;
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    if(FALSE == YX_Voice_Allow_Receive(pMe))
    {
        ZDT_LOG("ZDT__LOG YX_Net_Receive_STK Not Allow");
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_NO_SPACE,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return -1;
    }
    ZDT_LOG("ZDT__LOG YX_Net_Receive_STK offset=%d,ret=%d,%s",offset,ret,tmp_str);
#ifdef ZTE_WATCH //努比亚需求（解决消息重复下发导致消息重复问题）
    {
        char message_id_str[11] = {0};
        ret = YX_Func_GetNextPara(&str, &len,message_id_str,10);
        if(ret > 0)
        {
            message_id = atoi(message_id_str);
            if(FALSE == YX_VCHAT_MessageId_Check(pInfo,message_id))
            {
                ZDT_LOG("YX_Net_Receive_STK repeat message_id=%d",message_id);
                sprintf(respond,"STK,1,%d",message_id);
                YX_Net_TCPRespond(g_zdt_phone_imei,respond,strlen(respond));
                return 0;
            }
        }
    }
#endif
    if(YX_Net_Receive_STK_VocFile(pMe,pContent+offset, ContentLen-offset,group_id,msg_type))
    {
        YX_Net_Send_STKQ(pMe);
        success = 1;
    }
    if(message_id !=0)
    {
        sprintf(respond,"STK,%d,%d",success,message_id);
    }
    else
    {
        sprintf(respond,"STK,%d",success);
    }
    YX_Net_TCPRespond(g_zdt_phone_imei,respond,strlen(respond));
#endif
    return 0;
}
#endif
/*[YS*869251026788788*1773*TK,3,#!AMR*/
int YX_Net_Receive_TK(YX_APP_T *pMe,uint8 * pContent,uint32 ContentLen)
{
    int ret = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint32 offset = 0;
    int msg_type = 0;
    char group_id[101] = {0};
    char tmp_str[101] = {0};
    YX_GROUP_INFO_DATA_T * pInfo = PNULL;
    uint message_id = 0;
    char respond[16] = {0};
    uint8 success = 0;
    
    ZDT_LOG("YX_Net_Receive_TK ContentLen=%d",ContentLen);
    if(len == 1)
    {
        ZDT_LOG("YX_Net_Receive_TK RSP %d",str[0]);
        return -1;
    }
	#if 0 
    //获取ID
    ret = YX_Func_GetNextPara(&str, &len,group_id,100);
    if(ret > 0)
    {
        offset = ret+1;
    }
    if(0==YX_MMI_Get_Bingd_Statues())
    {
        //未绑定时不接收微聊消息
        if(s_yx_is_first_get_band_info)
        {
            YX_Net_TCPRespond(g_zdt_phone_imei,"TK,1",4);
        }
        else
        {
            s_yx_is_first_need_tkq = 1;
        }
        ZDT_LOG("YX_Net_Receive_TK ERR BIND %d",s_yx_is_first_get_band_info);
        return -1;
    }
    pInfo = YX_VCHAT_GetGroupInfoByID(group_id);
    if(pInfo == PNULL)
    {
        if(s_yx_is_first_get_all_info)
        {
            s_yx_is_vchat_tk_err_time++;
            if(s_yx_is_vchat_tk_err_time > 3)
            {
                YX_Net_TCPRespond(g_zdt_phone_imei,"TK,1",4);
                YX_Net_Send_TKQ(pMe);
                s_yx_is_vchat_tk_err_time = 0;
            }
            else
            {
                YX_Net_Send_UPGCUL(pMe);
                YX_Net_Send_TKQ(pMe);
            }
        }
        else
        {
            s_yx_is_first_need_tkq = 2;
        }
        ZDT_LOG("YX_Net_Receive_TK ERR GROUP %d",s_yx_is_first_get_all_info);
        return -1;
    }
	#endif
    s_yx_is_first_need_tkq = 0;
    s_yx_is_vchat_tk_err_time = 0;
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    if(FALSE == YX_Voice_Allow_Receive(pMe))
    {
        ZDT_LOG("YX_Net_Receive_TK Not Allow");
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_NO_SPACE,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return -1;
    }
    ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
    if(ret > 0 && ret < 4)
    {
        msg_type = atoi(tmp_str);
        offset += ret;
        offset += 1;
    }
#ifdef ZTE_WATCH //努比亚需求（解决消息重复下发导致消息重复问题）
    {
        char message_id_str[11] = {0};
        ret = YX_Func_GetNextPara(&str, &len,message_id_str,10);
        if(ret > 0)
        {
            message_id = atoi(message_id_str);
            if(FALSE == YX_VCHAT_MessageId_Check(pInfo,message_id))
            {
                ZDT_LOG("YX_Net_Receive_TK repeat message_id=%d",message_id);
                sprintf(respond,"TK,1,%d",message_id);
                YX_Net_TCPRespond(g_zdt_phone_imei,respond,strlen(respond));
                return 0;
            }
        }
    }
#endif    
    ZDT_LOG("YX_Net_Receive_TK offset=%d,ret=%d,%s",offset,ret,tmp_str);
    if(YX_Net_Receive_TK_VocFile(pMe,pContent+offset, ContentLen-offset,group_id,msg_type))
    {
        YX_Net_TCPRespond(g_zdt_phone_imei,"TK,1",4);
        YX_Net_Send_TKQ(pMe);
        success = 1;
    }
    if(message_id !=0)
    {
        sprintf(respond,"TK,%d,%d",success,message_id);
    }
    else
    {
        sprintf(respond,"TK,%d",success);
    }
    YX_Net_TCPRespond(g_zdt_phone_imei,respond,strlen(respond));
#endif
    return 0;
}

#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
int YX_Net_Receive_TK2(YX_APP_T *pMe,uint8 * pContent,uint32 ContentLen)
{
    int ret = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint32 offset = 0;
    int msg_type = 0;
    char group_id[101] = {0};
    char tmp_str[101] = {0};
    YX_GROUP_INFO_DATA_T * pInfo = PNULL;
    uint message_id = 0;
    char respond[16] = {0};
    uint8 success = 0;
    
    ZDT_LOG("YX_Net_Receive_TK2 ContentLen=%d",ContentLen);
    //获取ID
    ret = YX_Func_GetNextPara(&str, &len,group_id,100);
    if(ret > 0)
    {
        offset = ret+1;
    }
    if(0==YX_MMI_Get_Bingd_Statues())
    {
        //未绑定时不接收微聊消息
        if(s_yx_is_first_get_band_info)
        {
            YX_Net_TCPRespond(g_zdt_phone_imei,"TK2,1",5);
        }
        else
        {
            s_yx_is_first_need_tk2q = 1;
        }
        ZDT_LOG("ZDT__LOG YX_Net_Receive_TK2 ERR BIND %d",s_yx_is_first_get_band_info);
        return -1;
    }
    ret = YX_Func_GetNextPara(&str, &len,tmp_str,100);
    if(ret > 0 && ret < 4)
    {
        msg_type = atoi(tmp_str);
        offset += ret;
        offset += 1;
        if(msg_type == -10)//发送语音服务器回复成功
        {
            ZDT_LOG("ZDT__LOG YX_Net_Receive_TK2 response ok");
            return 0;
        }
    }
    pInfo = YX_VCHAT_GetGroupInfoByID(group_id);
    if(pInfo == PNULL)
    {
        if(s_yx_is_first_get_all_info)
        {
            s_yx_is_vchat_tk2_err_time++;
            if(s_yx_is_vchat_tk2_err_time > 3)
            {
                YX_Net_TCPRespond(g_zdt_phone_imei,"TK2,1",5);
                YX_Net_Send_TKQ2(pMe);
                s_yx_is_vchat_tk2_err_time = 0;
            }
            else
            {
                YX_Net_Send_PPQ(pMe);
                YX_Net_Send_TKQ2(pMe);
            }
        }
        else
        {
            s_yx_is_first_need_tk2q = 2;
        }
        ZDT_LOG("ZDT__LOG YX_Net_Receive_TK2 ERR GROUP %d",s_yx_is_first_get_all_info);
        return -1;
    }
    s_yx_is_first_need_tk2q = 0;
    s_yx_is_vchat_tk2_err_time = 0;
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    if(FALSE == YX_Voice_Allow_Receive(pMe))
    {
        ZDT_LOG("YX_Net_Receive_TK2 Not Allow");
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_NO_SPACE,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return -1;
    }
#ifdef ZTE_WATCH //努比亚需求（解决消息重复下发导致消息重复问题）
    {
        char message_id_str[11] = {0};
        ret = YX_Func_GetNextPara(&str, &len,message_id_str,10);
        if(ret > 0)
        {
            message_id = atoi(message_id_str);
            if(FALSE == YX_VCHAT_MessageId_Check(pInfo,message_id))
            {
                ZDT_LOG("YX_Net_Receive_TK2 repeat message_id=%d",message_id);
                sprintf(respond,"TK2,1,%d",message_id);
                YX_Net_TCPRespond(g_zdt_phone_imei,respond,strlen(respond));
                return 0;
            }
        }
    }
#endif 
    ZDT_LOG("YX_Net_Receive_TK2 offset=%d,ret=%d,%s",offset,ret,tmp_str);
    if(YX_Net_Receive_TK2_VocFile(pMe,pContent+offset, ContentLen-offset,group_id,msg_type)) 
    {
        YX_Net_Send_TKQ2(pMe);
        success = 1;
    }          
    if(message_id !=0)
    {
        sprintf(respond,"TK2,%d,%d",success,message_id);
    }
    else
    {
        sprintf(respond,"TK2,%d",success);
    }
    YX_Net_TCPRespond(g_zdt_phone_imei,respond,strlen(respond));
#endif
    return 0;
}
#endif

int YX_Net_Receive_MESSAGE(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{    
    uint8 * str = NULL;
    uint16 len = 0;
    ZDT_LOG("YX_Net_Receive_MESSAGE");
    if(ContentLen == 0 || pContent == NULL)
    {
        YX_Net_TCPRespond(g_zdt_phone_imei,"MESSAGE",7);
        return -1;
    }
    
    str=(uint8 *)SCI_ALLOC_APPZ(ContentLen+1);
    if(str == NULL)
    {
        YX_Net_TCPRespond(g_zdt_phone_imei,"MESSAGE",7);
        return -2;
    }
    SCI_MEMSET(str,0,ContentLen+1);
    SCI_MEMCPY(str,pContent,ContentLen);
    len = ContentLen;

    ZDT_LOG("YX_Net_Receive_MESSAGE len=%d",len);

    if(str != NULL && len  != 0)
    {
        YX_Net_TCPRespond(g_zdt_phone_imei,"MESSAGE",7);
    }
    else
    {
        YX_Net_TCPRespond(g_zdt_phone_imei,"MESSAGE",7);
    }
    SCI_FREE(str);
    return 0;
}

int YX_Net_Receive_PHB(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen,uint16 idx)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 white_name[YX_DB_WHITE_MAX_NAME_SIZE+1] = {0};
    uint8 white_num[YX_DB_WHITE_MAX_NUMBER_SIZE+1] = {0};
    
    ZDT_LOG("YX_Net_Receive_PHB idx=%d,ContentLen=%d",idx,ContentLen);

    if(idx == 0)
    {
        YX_DB_WHITE_ListDelAllBuf();
        if(len != 0)
        {
            for(i = 0; i < YX_DB_WHITE_MAX_SUM; i++)
            {
                SCI_MEMSET(white_name,0,YX_DB_WHITE_MAX_NAME_SIZE+1);
                SCI_MEMSET(white_num,0,YX_DB_WHITE_MAX_NUMBER_SIZE+1);
                ret = YX_Func_GetNextPara(&str, &len,(char *)white_num,YX_DB_WHITE_MAX_NUMBER_SIZE);
                if(ret < 0)
                {
                    ZDT_LOG("YX_Net_Receive_PHB white_num NULL Exit(%d)",i);
                    break;
                }
                
                ret = YX_Func_GetNextPara(&str, &len,(char *)white_name,YX_DB_WHITE_MAX_NAME_SIZE);
                if(ret > 0)
                {
                    YX_DB_WHITE_ListModifyBuf(i,(char *)white_num,SCI_STRLEN((char *)white_num),(char *)white_name,SCI_STRLEN((char *)white_name));
                }
            }
        }
        YX_DB_WHITE_ListSaveBuf();
        YX_Net_TCPRespond(g_zdt_phone_imei,"PHB",3);
    }
    else if(idx == 1)
    {
        YX_DB_WHITE_ListDelAllBuf();
        if(len != 0)
        {
            j = 5;
            if(j > YX_DB_WHITE_MAX_SUM)
            {
                j = YX_DB_WHITE_MAX_SUM;
            }
            for(i = 0; i < j; i++)
            {
                SCI_MEMSET(white_name,0,YX_DB_WHITE_MAX_NAME_SIZE+1);
                SCI_MEMSET(white_num,0,YX_DB_WHITE_MAX_NUMBER_SIZE+1);
                ret = YX_Func_GetNextPara(&str, &len,(char *)white_num,YX_DB_WHITE_MAX_NUMBER_SIZE);
                if(ret < 0)
                {
                    ZDT_LOG("YX_Net_Receive_PHB white_num NULL Exit(%d)",i);
                    break;
                }
                
                ret = YX_Func_GetNextPara(&str, &len,(char *)white_name,YX_DB_WHITE_MAX_NAME_SIZE);
                if(ret > 0)
                {
                    YX_DB_WHITE_ListModifyBuf(i,(char *)white_num,SCI_STRLEN((char *)white_num),(char *)white_name,SCI_STRLEN((char *)white_name));
                }
            }
        }
        YX_DB_WHITE_ListSaveBuf();
        YX_Net_TCPRespond(g_zdt_phone_imei,"PHB1",4);
    }
    else
    {
        if(len != 0)
        {
            i = (idx-1)*5;
            j = idx*5;
            if(j > YX_DB_WHITE_MAX_SUM)
            {
                j = YX_DB_WHITE_MAX_SUM;
            }
            if(i < j)
            {
                for(; i < j; i++)
                {
                    SCI_MEMSET(white_name,0,YX_DB_WHITE_MAX_NAME_SIZE+1);
                    SCI_MEMSET(white_num,0,YX_DB_WHITE_MAX_NUMBER_SIZE+1);
                    ret = YX_Func_GetNextPara(&str, &len,(char *)white_num,YX_DB_WHITE_MAX_NUMBER_SIZE);
                    if(ret < 0)
                    {
                        ZDT_LOG("YX_Net_Receive_PHB white_num NULL Exit(%d)",i);
                        break;
                    }
                    
                    ret = YX_Func_GetNextPara(&str, &len,(char *)white_name,YX_DB_WHITE_MAX_NAME_SIZE);
                    if(ret > 0)
                    {
                        YX_DB_WHITE_ListModifyBuf(i,(char *)white_num,SCI_STRLEN((char *)white_num),(char *)white_name,SCI_STRLEN((char *)white_name));
                    }
                }
            }
        }
        
        YX_DB_WHITE_ListSaveBuf();
        ZDT_LOG("YX_Net_Receive_PHB Save OK");
        
        SCI_MEMSET(white_num,0,YX_DB_WHITE_MAX_NUMBER_SIZE+1);
        sprintf((char *)white_num,"PHB%d",idx);
        len = SCI_STRLEN((char *)white_num);
        YX_Net_TCPRespond(g_zdt_phone_imei,(char *)white_num,len);
    }
    return 0;
}

int YX_Net_Receive_PHL(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen,uint16 idx)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 white_name[YX_DB_WHITE_MAX_NAME_SIZE+1] = {0};
    uint8 white_num[YX_DB_WHITE_MAX_NUMBER_SIZE+1] = {0};
    uint8 relation_buf[2+1] = {0};
    ZDT_LOG("YX_Net_Receive_PHL idx=%d,ContentLen=%d",idx,ContentLen);
    YX_DB_WHITE_ListDelAllBuf();
    if(len != 0)
    {
        for(i = 0; i < YX_DB_WHITE_MAX_SUM; i++)
        {
            SCI_MEMSET(white_name,0,YX_DB_WHITE_MAX_NAME_SIZE+1);
            SCI_MEMSET(white_num,0,YX_DB_WHITE_MAX_NUMBER_SIZE+1);
            ret = YX_Func_GetNextPara(&str, &len,(char *)white_num,YX_DB_WHITE_MAX_NUMBER_SIZE);
            if(ret < 0)
            {
                ZDT_LOG("YX_Net_Receive_PHL white_num NULL Exit(%d)",i);
                break;
            }
            ret = YX_Func_GetNextPara(&str, &len,(char *)white_name,YX_DB_WHITE_MAX_NAME_SIZE);
            if(ret > 0)
            {
                YX_DB_WHITE_ListModifyBuf(i,(char *)white_num,SCI_STRLEN((char *)white_num),(char *)white_name,SCI_STRLEN((char *)white_name));
            }
	     ret = YX_Func_GetNextPara(&str, &len,(char *)relation_buf,3);
            if(ret > 0)
            {
                yx_DB_White_Reclist[i].relation_id=atoi(relation_buf);
			  ZDT_LOG("YX_Net_Receive_PHL relation_buf=%s,relation_id=%d",relation_buf,yx_DB_White_Reclist[i].relation_id);
            }
        }
    }
    YX_DB_WHITE_ListSaveBuf();
    YX_Net_TCPRespond(g_zdt_phone_imei,"PHL",3);

    return 0;
}

int YX_Net_Receive_WHITELIST(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen,uint16 idx)
{
    int ret = 0;
    uint16 i = 0;
    uint8 * str = pContent;
    uint16 len = ContentLen;
    uint8 white_num[YX_DB_WHITE_MAX_NUMBER_SIZE+1] = {0};
    
    ZDT_LOG("YX_Net_Receive_WHITELIST idx=%d,ContentLen=%d",idx,ContentLen);

    if(idx == 0)
    {
        YX_Net_TCPRespond(g_zdt_phone_imei,"WHITELIST",9);
    }
    else if(idx == 1)
    {
        YX_Net_TCPRespond(g_zdt_phone_imei,"WHITELIST1",10);
    }
    else
    {
        SCI_MEMSET(white_num,0,YX_DB_WHITE_MAX_NUMBER_SIZE+1);
        sprintf((char *)white_num,"WHITELIST%d",idx);
        len = SCI_STRLEN((char *)white_num);
        YX_Net_TCPRespond(g_zdt_phone_imei,(char *)white_num,len);
    }
    return 0;
}

#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
int YX_Net_Receive_PPQ(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 friend_id[YX_DB_FRIEND_MAX_ID_SIZE+1] = {0};
    uint8 friend_name[YX_DB_FRIEND_MAX_NAME_SIZE+1] = {0};
    uint8 friend_num[YX_DB_FRIEND_MAX_NUMBER_SIZE+1] = {0};
    BOOLEAN has_friend = FALSE;
    
    ZDT_LOG("ZDT__LOG YX_Net_Receive_PPQ ContentLen=%d",ContentLen);
    YX_DB_FRIEND_ListDelAllBuf();
    if(len != 0)
    {
        for(i = 0; i < YX_DB_FRIEND_MAX_SUM; i++)
        {
            SCI_MEMSET(friend_id,0,YX_DB_FRIEND_MAX_ID_SIZE+1);
            SCI_MEMSET(friend_name,0,YX_DB_FRIEND_MAX_NAME_SIZE+1);
            SCI_MEMSET(friend_num,0,YX_DB_FRIEND_MAX_NUMBER_SIZE+1);
            ret = YX_Func_GetNextPara(&str, &len,(char *)friend_id,YX_DB_FRIEND_MAX_ID_SIZE);
            if(ret <= 0)
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_PPQ friend_id NULL Exit(%d)",i);
                break;
            }
            ret = YX_Func_GetNextPara(&str, &len,(char *)friend_name,YX_DB_FRIEND_MAX_NAME_SIZE);
            if(ret <= 0)
            {
                SCI_MEMCPY(friend_name,"672A547D540D",SCI_STRLEN("672A547D540D"));
            }
            has_friend = TRUE;
            ret = YX_Func_GetNextPara(&str, &len,(char *)friend_num,YX_DB_FRIEND_MAX_NUMBER_SIZE);
            YX_DB_FRIEND_ListModifyBuf(i,friend_id,SCI_STRLEN((char *)friend_id),friend_num,SCI_STRLEN((char *)friend_num),(char *)friend_name,SCI_STRLEN((char *)friend_name));
        }
    }
    YX_DB_FRIEND_ListSaveBuf();
    YX_VCHAT_CheckAllGroupUser();
    YX_VCHAT_GetAllGroupInfo();
    s_yx_is_first_info_times |= 0x01;
    //if(s_yx_is_first_info_times == 0x03)
    if(has_friend)
    {
        if(0==YX_MMI_Get_Bingd_Statues())
        {
        	YX_MMI_Set_Bingd_Statues(1);
        }
        if(s_yx_is_first_get_band_info == FALSE)
        {
            s_yx_is_first_get_band_info = TRUE;
            if(yx_DB_Set_Rec.band_status == 0)
            {
                yx_DB_Set_Rec.band_status = 1;
                YX_DB_SET_ListModify();
            }
        }
        
        if(s_yx_is_first_get_all_info == FALSE)
        {
            s_yx_is_first_get_all_info = TRUE;
            if(s_yx_is_first_need_tk2q)
            {
                YX_Net_Send_TKQ2(pMe);
            }
            if(s_yx_is_first_need_tkq)
            {
                YX_Net_Send_TKQ(pMe);
            }
            if(s_yx_is_first_need_stkq)
            {
                YX_Net_Send_STKQ(pMe);
            }
       }
    }
    return 0;
}

int YX_Net_Receive_UPGCUL(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint16 i = 0;
    uint16 j = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 appuser_id[YX_DB_APPUSER_MAX_ID_SIZE+1] = {0};
    uint8 appuser_name[YX_DB_APPUSER_MAX_NAME_SIZE+1] = {0};
    uint8 appuser_num[YX_DB_APPUSER_MAX_NUMBER_SIZE+1] = {0};
    uint8 reverse[21] = {0};
    BOOLEAN has_binded = FALSE;
    ZDT_LOG("ZDT__LOG YX_Net_Receive_UPGCUL ContentLen=%d",ContentLen);
    YX_DB_APPUSER_ListDelAllBuf();
    if(len != 0)
    {
        for(i = 0; i < YX_DB_APPUSER_MAX_SUM; i++)
        {
            SCI_MEMSET(appuser_id,0,YX_DB_APPUSER_MAX_ID_SIZE+1);
            SCI_MEMSET(appuser_name,0,YX_DB_APPUSER_MAX_NAME_SIZE+1);
            SCI_MEMSET(appuser_num,0,YX_DB_APPUSER_MAX_NUMBER_SIZE+1);
            ret = YX_Func_GetNextPara(&str, &len,(char *)appuser_id,YX_DB_APPUSER_MAX_ID_SIZE);
            if(ret <= 0)
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_UPGCUL appuser_id NULL Exit(%d)",i);
                break;
            }
            ret = YX_Func_GetNextPara(&str, &len,(char *)appuser_name,YX_DB_APPUSER_MAX_NAME_SIZE);
            if(ret <= 0)
            {
                SCI_MEMCPY(appuser_name,"672A547D540D",SCI_STRLEN("672A547D540D"));
            }
            has_binded = TRUE;//有绑定
            ret = YX_Func_GetNextPara(&str, &len,(char *)appuser_num,YX_DB_APPUSER_MAX_NUMBER_SIZE);
            ret = YX_Func_GetNextPara(&str, &len,(char *)reverse,20);
            YX_DB_APPUSER_ListModifyBuf(i,appuser_id,SCI_STRLEN((char *)appuser_id),appuser_num,SCI_STRLEN((char *)appuser_num),(char *)appuser_name,SCI_STRLEN((char *)appuser_name));
        }
    }
    YX_DB_APPUSER_ListSaveBuf();
    YX_VCHAT_CheckAllGroupUser();
    YX_VCHAT_GetAllGroupInfo();
    s_yx_is_first_info_times |= 0x02;
    if(has_binded)
    {
        if(0==YX_MMI_Get_Bingd_Statues())
        {
        	YX_MMI_Set_Bingd_Statues(1);
        }
        if(s_yx_is_first_get_band_info == FALSE)
        {
            s_yx_is_first_get_band_info = TRUE;
            if(yx_DB_Set_Rec.band_status == 0)
            {
                yx_DB_Set_Rec.band_status = 1;
                YX_DB_SET_ListModify();
            }
        }
        
        if(s_yx_is_first_get_all_info == FALSE)
        {
            s_yx_is_first_get_all_info = TRUE;
            if(s_yx_is_first_need_tkq)
            {
                YX_Net_Send_TKQ(pMe);
            }
            if(s_yx_is_first_need_stkq)
            {
                YX_Net_Send_STKQ(pMe);
            }
        }
    }
    //YX_Net_TCPRespond(g_zdt_phone_imei,"UPGCUL",6);//这个回复指令不对
    return 0;
}

int YX_Net_Receive_PP(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    int friend_idx = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 friend_id[YX_DB_FRIEND_MAX_ID_SIZE+1] = {0};
    uint8 friend_name[YX_DB_FRIEND_MAX_NAME_SIZE+1] = {0};
    uint8 friend_num[YX_DB_FRIEND_MAX_NUMBER_SIZE+1] = {0};
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,friend_id,YX_DB_FRIEND_MAX_ID_SIZE);
    if(ret < 2)
    {
        //交友失败
        YX_Net_Friend_End(FALSE,YX_FRIEND_SEARCH_NULL,PNULL,PNULL);
        return 0;
    }

    ret = YX_Func_GetNextPara(&str, &len,friend_name,YX_DB_FRIEND_MAX_NAME_SIZE);
    if(ret <= 0)
    {
        SCI_MEMCPY(friend_name,"672A547D540D",SCI_STRLEN("672A547D540D"));
    }
    ret = YX_Func_GetNextPara(&str, &len,friend_num,YX_DB_FRIEND_MAX_NUMBER_SIZE);

    friend_idx = YX_DB_Friend_Add(friend_id,SCI_STRLEN((char *)friend_id),friend_num,SCI_STRLEN((char *)friend_num),friend_name,SCI_STRLEN((char *)friend_name));
    if(friend_idx > 0)
    {
        //交友成功
        SCI_MEMSET(g_pp_friend_id,0,sizeof(g_pp_friend_id));
        SCI_MEMCPY(g_pp_friend_id,friend_id,SCI_STRLEN((char *)friend_id));
        YX_Net_Friend_End(TRUE,YX_FRIEND_NULL,friend_name,friend_num);
    }
    else
    {
        //交友已满
        YX_Net_Friend_End(FALSE,YX_FRIEND_SEARCH_FULL,PNULL,PNULL);
    }
    
    ZDT_LOG("YX_Net_Receive_PP ret =%d, friend_id = %s, name=%s, num=%s",ret,friend_id,friend_name,friend_num);
    return 0;
}

int YX_Net_Receive_PPR(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint8 friend_id[YX_DB_FRIEND_MAX_ID_SIZE+1] = {0};
    
    ZDT_LOG("ZDT__LOG YX_Net_Receive_PPR ContentLen=%d",ContentLen);
    ret = YX_Func_GetNextPara(&str, &len,(char *)friend_id,YX_DB_FRIEND_MAX_ID_SIZE);
    if(ret > 0)
    {
        ZDT_Delete_CheckClose_Tiny_Chat_Win(friend_id);
        YX_DB_Friend_DeleteByFrinedID(friend_id);
        YX_VCHAT_DeleteGroupFile(friend_id);
        YX_VCHAT_GetAllGroupInfo();
        MMIZDT_UpdateChatGroupWin(TRUE);
    }
    return 0;
}
#endif

int YX_Net_Receive_PROFILE(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.profile = atoi(buf);
        #if 0
            if(yx_DB_Set_Rec.profile == 1)
            {
                if (htNVMgr_GetProfileType() != PROFILE_OUTDOOR)
                {
                    htNVMgr_SetProfileType(PROFILE_OUTDOOR);
                    IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(), ANNUN_FIELD_PROFILE, PROFILE_OUTDOOR);
                }
            }
            else if(yx_DB_Set_Rec.profile == 2)
            {
                if (htNVMgr_GetProfileType() != PROFILE_STANDARD)
                {
                    htNVMgr_SetProfileType(PROFILE_STANDARD);
                    IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_PROFILE, PROFILE_STANDARD);
                }
            }
            else if(yx_DB_Set_Rec.profile == 3)
            {
                if (htNVMgr_GetProfileType() != PROFILE_MEETING)
                {
                    htNVMgr_SetProfileType(PROFILE_MEETING);
                    IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_PROFILE, PROFILE_MEETING);
                }
            }
            else if(yx_DB_Set_Rec.profile == 4)
            {
                if (htNVMgr_GetProfileType() != PROFILE_SILENT)
                {
                    htNVMgr_SetProfileType(PROFILE_SILENT);
                    IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_PROFILE, PROFILE_SILENT);
                }
            }
            else
            {
                if (htNVMgr_GetProfileType() != PROFILE_STANDARD)
                {
                    htNVMgr_SetProfileType(PROFILE_STANDARD);
                    IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_PROFILE, PROFILE_STANDARD);
                }
            }
            #endif
            YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_PROFILE ret=%d,profile = %d",ret,yx_DB_Set_Rec.profile);

    YX_Net_TCPRespond(g_zdt_phone_imei,"profile",7);
    
    return 0;
}

int YX_Net_Receive_FALLDOWN(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
        yx_DB_Set_Rec.falldown = atoi(buf);
        YX_DB_SET_ListModify();
    }
    
    ZDT_LOG("YX_Net_Receive_FALLDOWN ret=%d,falldown = %d",ret,yx_DB_Set_Rec.falldown);

    YX_Net_TCPRespond(g_zdt_phone_imei,"FALLDOWN",8);
    
    return 0;
}

int YX_Net_Receive_RCAPTURE(YX_APP_T *pMe)
{
    ZDT_LOG("YX_Net_Receive_RCAPTURE");
#ifndef WIN32    
#ifdef LEBAO_MUSIC_SUPPORT
    lebao_exit();
#endif
#ifdef XYSDK_SUPPORT
    LIBXMLYAPI_AppExit();
#endif
#endif
    ZMTApp_CloseRecordAndPlayer();
#ifdef ZDT_VIDEOCHAT_SUPPORT
    if(!VideoChat_IsInCall())
#endif
    {
        YX_API_Captrue_Start(0);
    }
    YX_Net_TCPRespond(g_zdt_phone_imei,"RCAPTURE",8);
    return 0;
}

int YX_Net_Receive_HRTSTART(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    
    if(ret > 0)
    {
         if(atoi(buf) != 1)
        {
	        yx_DB_Set_Rec.heart= atoi(buf);
	        YX_DB_SET_ListModify();
        }
    }
    #if defined(ZDT_HSENSOR_SUPPORT) && !defined(ZDT_HSENSOR_SUPPORT_TW)
    ZDT_StartHeartrate(yx_DB_Set_Rec.heart);
    #endif
    
    ZDT_LOG("YX_Net_Receive_HRTSTART ret=%d,heart = %d",ret,yx_DB_Set_Rec.heart);

    YX_Net_TCPRespond(g_zdt_phone_imei,"hrtstart",8);
    
    return 0;
}

#if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
int YX_Net_Receive_WDSTART(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[22] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    
    if(ContentLen == 0)
    {
        return 0;
    }
    
    ret = YX_Func_GetNextPara(&str, &len,buf,20);
    if(ret > 0)
    {
        yx_DB_Set_Rec.body_tw_peroid = atoi(buf);
        //yx_DB_Set_Rec.body_tw_peroid = 120;//yangyu add test
        YX_DB_SET_ListModify();
    }
    
    ZDT_StartBodyTw(yx_DB_Set_Rec.body_tw_peroid);
   
    
    ZDT_LOG("YX_Net_Receive_WDSTART ret=%d,body_tw_peroid = %d",ret,yx_DB_Set_Rec.body_tw_peroid);

    YX_Net_TCPRespond(g_zdt_phone_imei,"WDSTART",7);
    
    return 0;
}
#endif

#ifdef ZTE_WEATHER_SUPPORT
/*[XT*334588000000156*009F*WT_FUTR,
	年-月-日,时:分:秒,城市名,当前温度,天气描述,天气编号,最高温,最低温,
	明天日期,天气描述,天气编号,最高温,最低温,
	后天日期,天气描述,天气编号,最高温,最低温,
	大后天日期,天气描述,天气编号,最高温,最低温]*/
/*[YS*334588000000156*008C*WT_FUTR,
	2023-06-08,10:39:56,4FDD5C715E02,19,96E8,4,23,17,
	2023-06-09,4E2D96E8,2,24,17,
	2023-06-10,4E2D96E8,2,22,17,
	2023-06-11,5C0F96E8,2,22,16]*/
/*[YS*355865062078503*0037*WT,2020-04-08,11:52:32,591A4E91,0,21,21,21,53575C71533A]*/

YX_WEATHER_INFO_T zte_wt_info[4];	

#ifdef WIN32
int32 YX_Net_Receive_WT_FUTR(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen);

void ZteWeather_InitTestWeatherInfo()
{
	uint8 * info = "[\
	2023-06-08,10:39:56,4FDD5C715E02,19,96E8,4,23,17,\
	2023-06-09,4E2D96E8,2,24,17,\
	2023-06-10,4E2D96E8,2,22,17,\
	2023-06-11,5C0F96E8,2,22,16]";

	YX_Net_Receive_WT_FUTR(PNULL, info, strlen(info));
}
#endif

int32 YX_Net_Receive_WT_FUTR(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;
    char wt_str[202] = {0};
    uint8 gb_str[101] = {0};  
    char d_str[21] = {0};  
    char t_str[21] = {0};  

    ZDT_LOG("YX_Net_Receive_WT_FUTR Content_len=%d",ContentLen);
    
    if(len > 10)//防止返回空的天气
    {
    	SCI_MEMSET(zte_wt_info, 0, 4*sizeof(YX_WEATHER_INFO_T));
		for(i = 0;i < 3;i++)
		{
			SCI_MEMSET(wt_str, 0 , sizeof(wt_str));
	        SCI_MEMSET(gb_str, 0 , sizeof(gb_str));
	        SCI_MEMSET(d_str, 0 , sizeof(d_str));
	        SCI_MEMSET(t_str, 0 , sizeof(t_str));
			
	        //年-月-日
	        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,20);
	        if(ret > 0)
	        {
	            GUI_GB2UCS(zte_wt_info[i].date_str,d_str,ret);
	        }
			if(i==0) //当天
            {
		        //时:分:秒
		        ret = YX_Func_GetNextPara(&str, &len,(char *)t_str,20);
		        if(ret > 0)
		        {
		            GUI_GB2UCS(zte_wt_info[i].time_str,t_str,ret);
		        }
            }
		    //天气描述
		    SCI_MEMSET(wt_str, 0, sizeof(wt_str));
		    ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,200);
		    if(ret > 0)
		    {
		        ZDT_UCS_Str16_to_uint16(wt_str,ret,zte_wt_info[i].content,100);
		    }

		    //天气编号： 1-晴  2—多云  3-阴  4—雨  5—雷雨 6-雪 
		    SCI_MEMSET(wt_str, 0, sizeof(wt_str));
		    ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
		    if(ret > 0)
		    {
		        uint16 num =  (uint16)atoi(wt_str);
		        zte_wt_info[i].num = num;
		    }

            if(i==0) //当天
            {
		        //当前温度
		        SCI_MEMSET(wt_str, 0, sizeof(wt_str));
		        ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
		        if(ret > 0)
		        {
		            zte_wt_info[i].cur_degree = atoi(wt_str);
		        }
            }
		        
		    //最低温
		    SCI_MEMSET(wt_str, 0, sizeof(wt_str));
		    ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
		    if(ret > 0)
		    {
		        zte_wt_info[i].min_degree = atoi(wt_str);
		    }
		        
		    //最高温
		    SCI_MEMSET(wt_str, 0, sizeof(wt_str));
		    ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
		    if(ret > 0)
		    {
		        zte_wt_info[i].max_degree = atoi(wt_str);
		    }
		        
		    //城市名
		    SCI_MEMSET(wt_str, 0, sizeof(wt_str));
		    ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
		    if(ret > 0)
		    {
		        //GUI_GB2UCS(g_yx_wt_info.city_name,wt_str,ret);
		        ZDT_UCS_Str16_to_uint16(wt_str,ret,zte_wt_info[i].city_name,100);
            }	
        }
        g_yx_wt_recieve_ok = TRUE;
        ZTEWEATHER_UpdateWin();
    }
	return 0;
}
#endif

/*
[YS*355865062078503*0037*WT,2020-04-08,11:52:32,591A4E91,0,21,21,21,53575C71533A]
*/
int32 YX_Net_Receive_WT(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;
    char wt_str[202] = {0};
    uint8 gb_str[101] = {0};  
    char d_str[21] = {0};  
    char t_str[21] = {0};  

    ZDT_LOG("YX_Net_Receive_WT Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        SCI_MEMSET(&g_yx_wt_info,0,sizeof(YX_WEATHER_INFO_T));
        //年-月-日
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,20);
        if(ret > 0)
        {
            GUI_GB2UCS(g_yx_wt_info.date_str,d_str,ret);
        }
        //时:分:秒
        ret = YX_Func_GetNextPara(&str, &len,(char *)t_str,20);
        if(ret > 0)
        {
            GUI_GB2UCS(g_yx_wt_info.time_str,t_str,ret);
        }
        //天气描述
        ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,200);
        if(ret > 0)
        {
            ZDT_UCS_Str16_to_uint16(wt_str,ret,g_yx_wt_info.content,100);
        }

        //天气编号：0——晴  1——阴   2——雨  3——雪
        ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
        if(ret > 0)
        {
            uint16 num =  (uint16)atoi(wt_str);
            g_yx_wt_info.num = num+1;
        }

        //当前温度
        ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
        if(ret > 0)
        {
            g_yx_wt_info.cur_degree = atoi(wt_str);
        }
        
        //最低温
        ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
        if(ret > 0)
        {
            g_yx_wt_info.min_degree = atoi(wt_str);
        }
        
        //最高温
        ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
        if(ret > 0)
        {
            g_yx_wt_info.max_degree = atoi(wt_str);
        }

        //城市名
        ret = YX_Func_GetNextPara(&str, &len,(char *)wt_str,100);
        if(ret > 0)
        {
            //GUI_GB2UCS(g_yx_wt_info.city_name,wt_str,ret);
            ZDT_UCS_Str16_to_uint16(wt_str,ret,g_yx_wt_info.city_name,100);
        }
        g_yx_wt_recieve_ok = TRUE;
        ZDT_LOG("YX_Net_Receive_WT_CDMA ret=%d,num = %d,degree=%d,%d-%d",ret,g_yx_wt_info.num,g_yx_wt_info.cur_degree,g_yx_wt_info.min_degree,g_yx_wt_info.max_degree);
    }
    MMIZDT_UpdateWeatherWin();//yangyu add
    return 0;
}

//add_schedule
void Get_EndHour(char * s1,int n)
{
	int i = 0;
	int k = 0;
	int j = 0;

	char s3[100] = {0};
	if(n < 0 || s1 == NULL )//判定
	{
	return;
	}

	for (j = 0; s1[j] != ' ' && s1[j] != '\0'; j++);//得出传进来的s1的长度j

		if(n > j)//当然，第n个字符的位置不可能大于s1的长度j
		{
			return;
		}
		else
		{
				s3[0]=s1[4];
				s3[1]=s1[5];
				
			for (i = 0; i < j; i++)//之后放心的将s2的字符串添加进来
			{
				s1[i]=s3[i];
			}

		}	
}

uint8 Get_number(char * s1)
{
	uint8 ret = 0;
	
	if( s1 == NULL )//判定
	{
		return 0;
	}


	if(SCI_MEMCMP(s1, "0031", 4) == 0)
 	{
            ret = 1;
 	}
	else if(SCI_MEMCMP(s1, "0032", 4) == 0)
 	{
            ret = 2;
 	}
	else if(SCI_MEMCMP(s1, "0033", 4) == 0)
 	{
            ret = 3;
 	}
	else if(SCI_MEMCMP(s1, "0034", 4) == 0)
 	{
            ret = 4;
 	}
	else if(SCI_MEMCMP(s1, "0035", 4) == 0)
 	{
            ret = 5;
 	}
	else if(SCI_MEMCMP(s1, "0036", 4) == 0)
 	{
            ret = 6;
 	}
	else if(SCI_MEMCMP(s1, "0037", 4) == 0)
 	{
            ret = 7;
 	}
	else if(SCI_MEMCMP(s1, "0038", 4) == 0)
 	{
            ret = 8;
 	}
	else if(SCI_MEMCMP(s1, "0039", 4) == 0)
 	{
            ret = 9;
 	}
	if(SCI_MEMCMP(s1, "00310030", 8) == 0)
 	{
            ret = 10;
 	}

	return ret;
}

//[3G*3916111448*0071*SCHEDULE,07000800-00000000-00000000-00000000-00000000-00000000-00000000-00000000,0031005f0031005f8bed6587]
//7：00-8:00，1_1_语文，即星期一第一节课语文，时间为7点到8点

int YX_Net_Receive_SCHEDULE(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
	int ret = 0;
	char buf[1600] = {0};
	uint8 * str = pContent;
	uint32 len = ContentLen;
	double temp = 0;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    char timer_str[64] = {0};

    uint8 * pCon0 = NULL;
    uint32 con0_len = 0;
    char con0_str[64] = {0};

    uint8 * pCon1 = NULL;
    uint32 con1_len = 0;
    char con1_str[64] = {0};
    uint8 * pCon2 = NULL;
    uint32 con2_len = 0;
    char con2_str[64] = {0};
    uint8 * pCon3 = NULL;
    uint32 con3_len = 0;
    char con3_str[64] = {0};
	
    char timecon_str[64] = {0};

	
	if(ContentLen == 0)
	{
		ZDT_LOG("ZDT__LOG YX_Net_Receive_SCHEDULE ERR");
		return 0;
	}

	 SCI_MEMSET(&yx_schedule_Rec, 0, sizeof(YX_SCHEDULE_Type));

	//时间
	ret = YX_Func_GetNextPara(&str, &len,buf,101);
	if(ret > 0)
	{
		 int i = 0;
		 pTime = (uint8 *)&buf[0];
         con_len = ret;
		
		 ZDT_LOG("ZDT__LOG YX_Net_Receive_SCHEDULE TIME ret=%d buf=%s",ret,buf);
		for(i = 0; i <  SCHEDULE_TIME_MUN_MAX; i++)
		{
			SCI_MEMSET(timer_str,0,64);
			ret = YX_Func_ClockGetNextPara(&pTime,&con_len,timer_str,10);
			if(ret > 0)
			{
				uint8 index =0;
				insert(timer_str,":",2);
				insert(timer_str,"-",5);
				insert(timer_str,":",8);
				//Get_EndHour(timer_str,2);
			    //index =  (uint8)atoi(timer_str);
				//yx_schedule_Rec.schedule_time_end[i]= index;	,yx_schedule_Rec.schedule_time_end[i]			
				SCI_MEMCPY(yx_schedule_Rec.schedule_time[i],timer_str,ret+4);
				ZDT_LOG("ZDT__LOG  TIME ret=%d buf=%s [%d]=%d",ret,timer_str,i);
			}	
		}
	}
	//课程
	ret = YX_Func_GetNextPara(&str, &len,buf,1450);
	if(ret > 0)
	{
		int i = 0;
		uint8 class_time = 0;
		uint8 class_num = 0;
		
		pCon0 = (uint8 *)&buf[0];
        con0_len = ret;
		for( i = 0; i < SCHEDULE_TIME_MUN_MAX * SCHEDULE_DAY_MAX ; i++)
		{
			SCI_MEMSET(con0_str,0,64);
			ret =YX_Func_UnicodeGetNextPara(&pCon0,&con0_len,con0_str,64);
			if(ret > 0)
			{
			 	pCon1 = (uint8 *)&con0_str[0];
        		con1_len = ret;
				ret =YX_Func_TimerUnicodeGetNextPara(&pCon1,&con1_len,con1_str,64);
				if(ret > 0)//星期几
				{
					class_time = Get_number(con1_str);
					ZDT_LOG("ZDT__LOG  CLASS ret=%d con1_str=%s",ret,con1_str);
				}	

				ret =YX_Func_TimerUnicodeGetNextPara(&pCon1,&con1_len,con2_str,64);
				if(ret > 0)//第几节课
				{
					class_num = Get_number(con2_str);
					ZDT_LOG("ZDT__LOG  CLASS ret=%d con2_str=%s",ret,con2_str);
				}		

				ret =YX_Func_TimerUnicodeGetNextPara(&pCon1,&con1_len,con3_str,64);
				if(ret > 0)//课程
				{
					char g_yx_con[41] = {0};
					wchar temp_wstr[64+1]={0};

			        SCI_MEMSET(yx_schedule_Rec.class_content_Type[class_time-1][class_num-1].class_content_str,0,41);
			        SCI_MEMCPY(yx_schedule_Rec.class_content_Type[class_time-1][class_num-1].class_content_str,con3_str,ret);
					ZDT_LOG("ZDT__LOG  CLASS ret=%d [%d][%d]=[%s]  con3_str-%s ",ret,class_time,class_num,yx_schedule_Rec.class_content_Type[class_time][class_num].class_content_str,con3_str);
				} 
				
				ZDT_LOG("ZDT__LOG  CLASS ret=%d con0_str=%s ",ret,con0_str);
			}

		}

		
		ZDT_LOG("ZDT__LOG YX_Net_Receive_SCHEDULE CLASS ret=%d buf=%s",ret,buf);
	}
	DB_Schedule_ListSaveBuf();

	YX_Net_TCPRespond(g_zdt_phone_imei,"SCHEDULE",8);

	return 0;
}

#if 1//def WIN32
PUBLIC void YX_AddTestSCHEDULE()
{
    //uint8 * pContent = "07000800-00000000-00000000-00000000-00000000-00000000-00000000-00000000,0031005f0031005f8bed6587";
	uint8 * pContent = "07000800-13001400-00000000-17001800-00000000-00000000-00000000-00000000,0031005F0031005F8BED6587002D0034005F0034005F596552297ED9002D0031005F0032005F53165B66002D0033005F0034005F63D25361002D0032005F0034005F51B251B2002D0031005F0034005F62DC62DC002D0037005F0031005F72697406002D0036005F0031005F7B148BB0002D0035005F0031005F4F308BA1002D0034005F0031005F5B664F1A002D0037005F0034005F4E0D67654E86002D0033005F0031005F751F7269002D0036005F0034005F7B148BB0002D0032005F0031005F65705B66002D0035005F0034005F753552A88F66";
	uint32 len = 0;
    YX_APP_T *pMe = &g_yx_app;
    len = strlen(pContent);
    YX_Net_Receive_SCHEDULE(pMe, pContent, len);
}
#endif


//yangyu add begin, for reject unknown people call
extern void MMIZDT_StartdelayTimer();//xx_modify_h03d


PUBLIC void ZDT_NV_SetRejectUnknownCall(BOOLEAN isOn)
{
    uint8 status = isOn;
    MMI_WriteNVItem(MMI_ZDT_NV_REJECT_CALL_ONOFF, &status);
    return;
}

PUBLIC uint8 ZDT_NV_GetRejectUnknownCall()
{
    uint8 status = 0;
    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;

    return_value = MMI_ReadNVItem(MMI_ZDT_NV_REJECT_CALL_ONOFF, &status);
    ZDT_LOG("ZDT_NV_GetRejectUnknownCall return_value=%d,status=%d",return_value,status);
    if (MN_RETURN_SUCCESS != return_value || status > 1)
    {
        status = 0;
        MMI_WriteNVItem(MMI_ZDT_NV_REJECT_CALL_ONOFF, &status);
    }
    return status;
}

PUBLIC BOOLEAN YX_IsRejectUnknownCallOpen()
{
    BOOLEAN ret = FALSE;
    ret = ZDT_NV_GetRejectUnknownCall();
    ZDT_LOG("ZDT__ YX_IsRejectUnknownCallOpen ret=%d",ret);
    return ret;
}


int32 YX_Net_Receive_REJECT(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[21] = {0};  
    char t_str[21] = {0};  
    uint8 reject_call_status = 0;

    ZDT_LOG("YX_Net_Receive_REJECT Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,20);
        if(ret > 0)
        {
            reject_call_status = atoi(d_str);
            ZDT_LOG("YX_Net_Receive_REJECT reject_call_status=%d",reject_call_status);
            ZDT_NV_SetRejectUnknownCall(reject_call_status);
        }
    }

}

int32 YX_Net_Receive_BAND(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
	int ret = 0;
	char buf[41] = {0};
	uint8 * str = pContent;
	uint32 len = ContentLen;
	uint16 i = 0;

	char d_str[21] = {0};  
	char t_str[21] = {0};  
	uint8 band_status = 0;

	ZDT_LOG("ZDT__LOG YX_Net_Receive_BAND Content_len=%d",ContentLen);
	
	if(len != 0)
	{
		ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,20);
		if(ret > 0)
		{
			band_status = atoi(d_str);
			ZDT_LOG("ZDT__LOG YX_Net_Receive_BAND band_status=%d",band_status);
			yx_DB_Set_Rec.band_status = band_status;
			YX_DB_SET_ListModify();
		}
	}
      s_yx_is_first_get_band_info = TRUE;

}

#ifdef ZDT_VIDEOCHAT_SUPPORT
int32 YX_Net_Receive_VideoUsers(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;
    uint16 j = 0;
    char wt_str[202] = {0};
    uint8 gb_str[101] = {0};  
    uint8 tmp_str[64+1] = {0};  
    char t_str[21] = {0}; 
    VIDEO_USER_INFO video_users[MAX_VIDEO_USERS_COUNT];

    ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers Content_len=%d",ContentLen);

    if(len > 0)
    {
        SCI_MEMSET(video_users, 0, MAX_VIDEO_USERS_COUNT*sizeof(VIDEO_USER_INFO));
        for(i = 0 ; i < MAX_VIDEO_USERS_COUNT ; i++)
        {
            //user_id
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                SCI_MEMCPY(video_users[i].phone_user_id , tmp_str, 64);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers i=%d ,user_id=%s",i, video_users[i].phone_user_id);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers parse finish");
                break;
            }
 
            //family_relative
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            
            if(ret > 0)
            {
               
                ZDT_UCS_Str16_to_uint16(tmp_str, strlen(tmp_str), video_users[i].family_relative , 15);
                //SCI_MEMCPY(video_users[i].family_relative , tmp_str, 32);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers i=%d ,family_relative=%s",i, video_users[i].family_relative);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers error ,break2");
                break;
            }

            //has_avatar
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                //SCI_MEMCPY(video_users[i].family_relative , tmp_str, 64);
                video_users[i].has_avatar = atoi(tmp_str);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers i=%d ,has_avatar=%d",i, video_users[i].has_avatar);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers error ,break3");
                break;
            }

            //phone_num
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                SCI_MEMCPY(video_users[i].phone_num, tmp_str, 24);
                
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers i=%d ,phone_num=%s",i, video_users[i].phone_num);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers error ,break4");
                //break; //maybe no number
            }

            //video_status
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                //SCI_MEMCPY(video_users[i].family_relative , tmp_str, 64);
                video_users[i].video_status = atoi(tmp_str);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers i=%d ,video_status=%d",i, video_users[i].video_status);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_VideoUsers error ,break5");
                break;
            }
            
        }
    }

    //YX_Net_Send_StartVideoCall(video_users[0]); //yangyu add test
#ifdef BAIRUI_VIDEOCHAT_SUPPORT //佰锐视频通话
    MMI_AnyChatRefreshListWin();
#endif

    Video_Call_RequestContactSuccess(video_users);

    return 0;
}



//watch call to APP
int32 YX_Net_Receive_UPVCALLAPP(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;
    char wt_str[202] = {0};
    uint8 gb_str[101] = {0};  
    uint8 tmp_str[64+1] = {0};  
    char t_str[21] = {0}; 
    VIDEO_CALL_INFO videoCallInfo ={0};


    ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP Content_len=%d",ContentLen);

    if(len > 0)
    {
         SCI_MEMSET(&videoCallInfo, 0, sizeof(VIDEO_CALL_INFO)); 
        {
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);//BR
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);//KEY
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.appId, tmp_str, 64);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP  ,appId=%s", videoCallInfo.appId);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP parse appId error 1");
                return;
            }

             #ifdef BAIDU_VIDEOCHAT_SUPPORT
             SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
             ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
             if(ret > 0)
             {
                     SCI_MEMCPY(videoCallInfo.romId, tmp_str, 64);
                 ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP  ,romId=%s", tmp_str);
             }
             #endif

             #ifndef BAIDU_VIDEOCHAT_SUPPORT// NO DEF
             //deviceId
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                
                SCI_MEMCPY(videoCallInfo.deviceId, tmp_str, 64);
                
                ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP  ,deviceId=%s", tmp_str);
            }
            #endif
           

            //channel 
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.channel_name, tmp_str, 32);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP  ,channel_name=%s", videoCallInfo.channel_name);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP error ,token");
            }
            
            #if 1//ndef BAIDU_VIDEOCHAT_SUPPORT
            //token
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);
            #endif
            
            //video_id
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.video_id, tmp_str, 64);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_UPVCALLAPP  ,video_id=%s", videoCallInfo.video_id);
            }           
            
        }
         videoCallInfo.call_type = VIDEO_CALL_OUT;
        Video_Call_Incoming(videoCallInfo);
    }
#ifdef BAIRUI_VIDEOCHAT_SUPPORT //佰锐视频通话
    ZDT_WatchVideoChatLogin(mVideoCallInfo.mUseIdStr, mVideoCallInfo.appUserIdStr , TRUE);//yangyu add test
#endif

    return 0;
}

//app call to watch
//[YS*801001234567891*0068*DOWNVCWW,BR,433b14ab-df23-4d26-b4fb-b5e8eca9731f,1631,123456,801001234567891-201107200716,300,60,120,160]
int32 YX_Net_Receive_DOWNVCWW(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;
    char wt_str[202] = {0};
    uint8 gb_str[101] = {0};  
    uint8 tmp_str[64+1] = {0};  
    char t_str[21] = {0};
    VIDEO_CALL_INFO videoCallInfo ={0};


    ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW Content_len=%d",ContentLen);

#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
   // if(YX_API_Record_IsRecording() || ZYB_StreamPlayer_IsPlaying())
    if(YX_API_Record_IsRecording())
    {
        return 0;
    }
    #ifdef ZYB_APP_SUPPORT
    if(ZYB_StreamPlayer_IsPlaying())
    {
        return 0;
    }
    #endif 
#endif
    if(len > 0)
    {
        SCI_MEMSET(&videoCallInfo, 0, sizeof(VIDEO_CALL_INFO));
        
        {
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);//BR

            //app id
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);//KEY
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.appId, tmp_str, 64);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW  ,appId=%s", videoCallInfo.appId);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW parse appId error 1");
                return;
            }
             //deviceId romID
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                #ifdef BAIDU_VIDEOCHAT_SUPPORT
                    SCI_MEMCPY(videoCallInfo.romId, tmp_str, 64);
                #endif
               ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW  ,deviceId=%s", tmp_str);
            }
           
            //mUseIdStr userid
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.mUseIdStr, tmp_str, 32);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW  ,mUseIdStr=%s", videoCallInfo.mUseIdStr);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW parse error 1");
                return;
            }

             //family_relative contact
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);
            if(ret > 0)
            {
                ZDT_UCS_Str16_to_uint16(tmp_str, strlen(tmp_str), videoCallInfo.family_relative , 15);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW family_relative=%s", videoCallInfo.family_relative);
            }

            //channel token
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.channel_name, tmp_str, 32);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW  ,token=%s", videoCallInfo.channel_name);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW error ,token");
            }
            
            #ifndef BAIDU_VIDEOCHAT_SUPPORT
            //token
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);
            #endif
            //video_id
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 64);
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.video_id, tmp_str, 64);
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW  ,video_id=%s", videoCallInfo.video_id);
            }
            
            #if 0 //no have app userid
            //appUserIdStr
            SCI_MEMSET(tmp_str, 0, 65*sizeof(char));
            ret = YX_Func_GetNextPara(&str, &len,(char *)tmp_str, 32);
            if(ret > 0)
            {
                SCI_MEMCPY(videoCallInfo.appUserIdStr , tmp_str, 32);
                
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW  ,appUserIdStr=%s", videoCallInfo.appUserIdStr);
            }
            else
            {
                ZDT_LOG("ZDT__LOG YX_Net_Receive_DOWNVCWW error ,break3");
                return;
            }
            #endif
 
            
        }
    }
#ifdef BAIRUI_VIDEOCHAT_SUPPORT //佰锐视频通话
    ZDT_WatchVideoChatLogin(videoCallInfo.mUseIdStr, videoCallInfo.appUserIdStr , FALSE);//yangyu add test
#endif
#ifdef XYSDK_SUPPORT
    LIBXMLYAPI_AppExit();
#endif
    ZMTApp_CloseRecordAndPlayer();
    if(Video_Call_Device_Idle_Check()) //在打电话上课禁用直接回复挂断
    {
        YX_Net_Send_UPWATCHHANGUP(&videoCallInfo.video_id);
        return 0;
    }
    else
    {
        videoCallInfo.call_type = VIDEO_CALL_INCOMING; //incoming
        Video_Call_Incoming(videoCallInfo);  
    }
    return 0;
}

//APP主动挂断 (在手表还未进入通话的情况下) 下发
uint32 YX_Net_Recevice_DOWNAPPHANGUP(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    ZDT_LOG("ZDT__LOG YX_Net_Recevice_UPWATCHHANGUP Content_len=%d",ContentLen);
    if(ContentLen > 0)
    {
        Video_Call_Remote_Hangup();
        YX_Net_Send_Reply_DOWNAPPHANGUP();
    }
    return 0;
}

#endif

//yangyu add begin
int32 YX_Net_Receive_DeviceBody(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
///[YS*352273017386344*000B*DEVICE_BODY,170,80]

    int ret = 0;
    //char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[16] = {0};  
    uint16 height;
    uint16 weight;

    uint8 reject_call_status = 0;

    ZDT_LOG("YX_Net_Receive_DeviceBody Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,15);
        if(ret > 0)
        {
           	height = atoi(d_str);
        }
		SCI_MEMSET(d_str, 0, sizeof(d_str));
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,15);
        if(ret > 0)
        {
           	weight = atoi(d_str);
        }

        if(height > 0 && weight > 0)
        {
			if(yx_DB_Set_Rec.height != height || yx_DB_Set_Rec.weight != weight)
			{
				yx_DB_Set_Rec.height = height;
				yx_DB_Set_Rec.weight = weight;
				YX_DB_SET_ListModify();
                YX_Net_TCPRespond(g_zdt_phone_imei,"DEVICE_BODY", 11);
			}
        }
        else
        {
			ZDT_LOG("YX_Net_Receive_DeviceBody error params");
        }
    }

}

///长续航模式 电量低于20%自动进入 长续航模式并不允许用户退出
///平台发送[XT*YYYYYYYYYY*LEN*LONG,1] (1打开，0关闭)
///终端回复: [XT*YYYYYYYYYY*LEN*LONG]
int32 YX_Net_Receive_LongEndurance(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    //char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[16] = {0};  
    int val;

    ZDT_LOG("YX_Net_Receive_LongEndurance Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,15);
        if(ret > 0)
        {
           	val = atoi(d_str);
            MMIZDT_NV_Set_Auto_Low_Battery(val == 1 ? 1:0);
        }
        
		YX_Net_TCPRespond(g_zdt_phone_imei,"LONG", 4);
        MMIZDT_CheckLowBatteryMode();
    }

}

///绑定用户设置拨打超过15秒没人接听就自动接听
///平台发送[XT*YYYYYYYYYY*LEN*AUTO,1] (1打开，0关闭)
///终端回复: [XT*YYYYYYYYYY*LEN*AUTO]
int32 YX_Net_Receive_AutoAnswer(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    //char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[16] = {0};  
    int val;
    
    ZDT_LOG("YX_Net_Receive_AutoAnswer Content_len=%d",ContentLen);    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,15);
        if(ret > 0)
        {
           	val = atoi(d_str);
            MMIZDT_NV_Set_Auto_Answer_Call_Status(val == 1?1:0); 	
        }
        
		YX_Net_TCPRespond(g_zdt_phone_imei,"AUTO", 4);
    }

}

int32 YX_Net_Receive_FamilyInCallMode(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
///上课禁用允许联系家长：
///[XT*YYYYYYYYYY*LEN*FAMILY,1] (1打开，0关闭)
///终端回复: [XT*YYYYYYYYYY*LEN*FAMILY]
    int ret = 0;
    //char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[16] = {0};  
    int val;
    ZDT_LOG("YX_Net_Receive_FamilyInCallMode Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,15);
        if(ret > 0)
        {
           	val = atoi(d_str);
           
           	yx_DB_Set_Rec.allow_family_call = (val == 1 ? 1:0);
           	
           	YX_DB_SET_ListModify();
        }
        
		YX_Net_TCPRespond(g_zdt_phone_imei,"FAMILY", 6);
    }

}

int32 YX_Net_Receive_CPSMS(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
//短信代收开关(新增)
//平台发送:[XT*334588000000156*0005*CPSMS,1]  (1代表开，0代表关，手表 默认关)
//终端回复:[XT*YYYYYYYYYYYYYYY*LEN*CPSMS]

    int ret = 0;
    //char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[16] = {0};  
    int val;


    
    ZDT_LOG("YX_Net_Receive_CPSMS Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,15);
        if(ret > 0)
        {
           	val = atoi(d_str);
           	
           	if(yx_DB_Set_Rec.sms_to_app != val)
           	{
            	yx_DB_Set_Rec.sms_to_app = (val == 1 ? 1:0);
            	YX_DB_SET_ListModify();
            }
        }
        
		YX_Net_TCPRespond(g_zdt_phone_imei,"CPSMS", 5);
    }
	return 0;
}

int32 YX_Net_Receive_UPGRADE(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
	//平台发送:[XT*YYYYYYYYYYYYYYY*LEN*UPGRADE] 
	//终端回复:[XT*YYYYYYYYYYYYYYY*0007*UPGRADE]


    int ret = 0;
    //char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[16] = {0};  
    int val;


    
    ZDT_LOG("YX_Net_Receive_UPGRADE Content_len=%d",ContentLen);
    
   
        
	YX_Net_TCPRespond(g_zdt_phone_imei,"UPGRADE", 7);
	
#ifdef FOTA_SUPPORT_CMIOT
    //MMIAPISET_DirectFotaUpgrade();
    MMIAPISET_FotaFirstCheck(); //需要提示框
#endif

    return 0;

}

int32 YX_Net_Receive_APP(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
	/**
	手表应用开关(新增)
	平台发送:
	[XT*YYYYYYYYYYYYYYY*LEN*APP,应用编号-启用状态,应用编号-启用状态]（1打开、0关闭）
	实例:[XT*334588000000156*0008*APP,1-1,2-0]
	终端回复:
	[XT*YYYYYYYYYYYYYYY*LEN*APP]
	实例:[XT*334588000000156*0003*APP]
	手表应用功能列表清单：
	1-绑定，2-拨号，3-计算器，4-计步，5-交友，6-秒表，7-闹钟，8-设置，9-视频通话，10-天气，11-通话记录，12-通讯录，13-微聊，14-喜马拉雅，15-相册，16-相机，17-小度，18-支付宝
	**/
    int ret = 0;
    //char buf[41] = {0};
    uint8 * str = pContent;
    uint32 len = ContentLen;
    uint16 i = 0;

    char d_str[16] = {0};  
    char splits[2][8] = {0};
    int index = 0;
    int val = 0;
    
    ZDT_LOG("YX_Net_Receive_APP Content_len=%d",ContentLen);
    if(len == 0)
    {
		return 0;
    }
    
    for(i = 0 ; i < MAX_YX_APP_MAX_SIZE; i++)
    {
		SCI_MEMSET(d_str, 0, sizeof(d_str));
        val = 0;
	    ret = YX_Func_GetNextPara(&str, &len,(char *)d_str,15);
	    if(ret > 0)
	    {
            char *split_str;
            split_str = strtok(d_str, "-");
            ZDT_LOG("YX_Net_Receive_APP split_str =%s",split_str);
            if (NULL != split_str)
            {
                index = atoi(split_str);
            }
            split_str = strtok(NULL, "-");
            if (NULL != split_str)
            {
                val = atoi(split_str);
            }
			if(index < MAX_YX_APP_MAX_SIZE)
			{
                ZDT_LOG("YX_Net_Receive_APP index:%d val:%d",index,val);
				yx_DB_Set_Rec.app_on[index] = val;
			}
	    }
	    else
	    {
	    	ZDT_LOG("YX_Net_Receive_APP break , i=%d",i);
			break;
	    }
    }
    YX_DB_SET_ListModify(); 
	YX_Net_TCPRespond(g_zdt_phone_imei,"APP", 3);
   	return 0; 

}

#ifdef WIN32
PUBLIC void YX_AddTestAPP()
{
    uint8 * pContent = "1-0,2-0,3-1,4-1,5-0,6-1,7-1,8-0,9-1,10-0,11-1,12-0,13-0,14-1,15-0,16-0,17-1,18-0]";
    uint32 len = 0;
    YX_APP_T *pMe = &g_yx_app;
    len = strlen(pContent);
    
    YX_Net_Receive_APP(pMe, pContent, len);

}
#endif

//获取电子网标数据(新增)
//终端发送: [XT*YYYYYYYYYYYYYYY*LEN*CTA]
//平台回复:
//[XT*YYYYYYYYYYYYYYY*0003*CTA,进网许可编号]
//实例：[XT*334588000000156*0003*CTA,23927125N5X58T0077NXY]
int32 YX_Net_Receive_CTA(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    char net_id_str[24] = {0};     
    ZDT_LOG("YX_Net_Receive_CTA Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)net_id_str,24);
        if(ret > 0)
        {
           	MMIZDT_Set_CTA_Net_Id(net_id_str);
        }       
		//YX_Net_TCPRespond(g_zdt_phone_imei,"CTA", 3); //不需要回复
    }
	return 0;
}

//设置步数目标
//平台发送:
//[XT*YYYYYYYYYYYYYYY*LEN*STEPS,步数]
//实例:[XT*334588000000156*000B*STEPS,10000]
//终端回复:
//[XT*YYYYYYYYYYYYYYY*LEN*STEPS]
//实例:[XT*334588000000156*0005*STEPS]
int32 YX_Net_Receive_STEPS(YX_APP_T *pMe,uint8 * pContent,uint16 ContentLen)
{
    int ret = 0;
    uint8 * str = pContent;
    uint32 len = ContentLen;
    char step_str[24] = {0};     
    ZDT_LOG("YX_Net_Receive_STEPS Content_len=%d",ContentLen);
    
    if(len != 0)
    {
        ret = YX_Func_GetNextPara(&str, &len,(char *)step_str,24);
        if(ret > 0)
        {
            uint step = atoi(step_str);
           	MMIZDT_NV_Set_Target_Step(step);
        }       
		YX_Net_TCPRespond(g_zdt_phone_imei,"STEPS", 5);
    }
	return 0;
}

static int YX_Net_ReceiveHandle(YX_APP_T *pMe,uint8 * pData,uint32 DataLen,uint32 Content_len)
{
    uint8 * pContent;
    uint32 cont_len = Content_len;
    uint16 head_len = g_yx_imei_len + 10;
    char buf[101] = {0};
    int ret = 0;
    BOOLEAN is_talk= FALSE;
    ZDT_LOG("YX_Net_ReceiveHandle: DataLen=%d, Content_len=%d",DataLen, Content_len);
    //ZDT_LOG("YX_Net_ReceiveHandle: pData = %s" , pData);
    pContent = pData+head_len;
    ret = YX_Func_GetNextPara(&pContent, &cont_len,buf,100);
    if(ret > 0)
    {
        YX_Func_strupr(buf);
    }
    if(ret == 6 &&  strncmp( (char *)buf, "UPLOAD", ret ) == 0)
    {
        // 数据上传时间间隔设置
        YX_Net_Receive_UPLOAD(pMe,pContent,cont_len);
    }
    else if(ret == 6 &&  strncmp( (char *)buf, "CENTER", ret ) == 0)
    {
        // 中心号码 设置
        YX_Net_Receive_CENTER(pMe,pContent,cont_len);
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "PW", ret ) == 0)
    {
        //控制密码
        YX_Net_Receive_PW(pMe,pContent,cont_len);
    }
    else if(ret == 4 &&  strncmp( (char *)buf, "CALL", ret ) == 0)
    {
        //拨打电话
        YX_Net_Receive_CALL(pMe,pContent,cont_len);
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "MONITOR", ret ) == 0)
    {
        //监听
        YX_Net_Receive_MONITOR(pMe,pContent,cont_len);
    }
    else if(ret == 6 &&  strncmp( (char *)buf, "SOSSMS", ret ) == 0)
    {
        //SOS 短信报警设置
        YX_Net_Receive_SOSSMS(pMe,pContent,cont_len);
    }
#ifdef ZDT_PLAT_YX_SUPPORT_SOS
    else if(ret >= 3 &&  strncmp( (char *)buf, "SOS", 3 ) == 0)
    {
        //SOS 号码设置
        uint16 idx = 0;
        if(ret > 3)
        {
            idx = atoi(buf+3);
        }
        YX_Net_Receive_SOS(pMe,pContent,cont_len,idx);
    }
#endif
    else if(ret == 2 &&  strncmp( (char *)buf, "IP", ret ) == 0)
    {
        //IP 端口设置
        YX_Net_Receive_IP(pMe,pContent,cont_len);
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "FACTORY", ret ) == 0)
    {
        //恢复出厂设置
        YX_Net_Receive_FACTORY(pMe);
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "LZ", ret ) == 0)
    {
        //设置语言和时区
        YX_Net_Receive_LZ(pMe,pContent,cont_len);
    }
    else if(ret == 6 &&  strncmp( (char *)buf, "LOWBAT", ret ) == 0)
    {
        //低电报警设置
        YX_Net_Receive_LOWBAT(pMe,pContent,cont_len);
    }
    else if(ret == 5 &&  strncmp( (char *)buf, "VERNO", ret ) == 0)
    {
        //版本号查询
        YX_Net_Receive_VERNO(pMe);
    }
    else if(ret == 5 &&  strncmp( (char *)buf, "RESET", ret ) == 0)
    {
        //重启
        YX_Net_Receive_RESET(pMe);
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "CR", ret ) == 0)
    {
        //定位
        YX_Net_Receive_CR(pMe);
    }
    else if(ret == 8 &&  strncmp( (char *)buf, "POWEROFF", ret ) == 0)
    {
        //关机
        YX_Net_Receive_POWEROFF(pMe);
    }
    else if(ret == 6 &&  strncmp( (char *)buf, "REMOVE", ret ) == 0)
    {
        //取下手环网络报警开关
        YX_Net_Receive_REMOVE(pMe,pContent,cont_len);
    }
    else if(ret == 9 &&  strncmp( (char *)buf, "REMOVESMS", ret ) == 0)
    {
        //取下手环短信报警开关
        YX_Net_Receive_REMOVESMS(pMe,pContent,cont_len);
    }
    else if(ret == 4 &&  strncmp( (char *)buf, "PEDO", ret ) == 0)
    {
        //计步功能开关
        YX_Net_Receive_PEDO(pMe,pContent,cont_len);
    }
    else if(ret == 8 &&  strncmp( (char *)buf, "WALKTIME", ret) == 0)
    {
        //计步时间段设置
        YX_Net_Receive_WALKTIME(pMe,pContent,cont_len);
    }
    else if(ret == 9 &&  strncmp( (char *)buf, "SLEEPTIME", ret ) == 0)
    {
        //. 翻转检测时间段设置
        YX_Net_Receive_SLEEPTIME(pMe,pContent,cont_len);
    }
    else if(ret == 11 &&  strncmp( (char *)buf, "SILENCETIME", ret ) == 0)
    {
        //免打扰时间段设置
        YX_Net_Receive_SILENCETIME(pMe,pContent,cont_len);
        YX_CheckClassSilentOnOff();//yangyu add
    }
    else if(ret == 12 &&  strncmp( (char *)buf, "SILENCETIME2", ret ) == 0)
    {
        //免打扰时间段设置
        YX_Net_Receive_SILENCETIME2(pMe,pContent,cont_len);
        YX_CheckClassSilentOnOff();//yangyu add
    }
    else if(ret == 4 &&  strncmp( (char *)buf, "FIND", ret ) == 0)
    {
        //找手表
        YX_Net_Receive_FIND(pMe);
    }
    else if(ret == 6 &&  strncmp( (char *)buf, "FLOWER", ret ) == 0)
    {
        // 小红花个数设置指令
        YX_Net_Receive_FLOWER(pMe,pContent,cont_len);
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "REMIND2", ret ) == 0)
    {
    #ifndef ZTE_WATCH
        //闹钟设置指令 
        YX_Net_Receive_REMIND2(pMe,pContent,cont_len);
    #endif
    }
    else if(ret == 6 &&  strncmp( (char *)buf, "REMIND", ret ) == 0)
    {
    #ifndef ZTE_WATCH
        //闹钟设置指令 
        YX_Net_Receive_REMIND(pMe,pContent,cont_len);
    #endif
    }
    //yangyu add 
    else if(ret == 7 &&  strncmp( (char *)buf, "BOOTOFF", ret ) == 0)
    {
        //闹钟设置指令 
        YX_Net_Receive_BOOTOFF(pMe,pContent,cont_len);
    }
    //yangyu end
    else if(ret == 2 &&  strncmp( (char *)buf, "TK", ret ) == 0)
    {
        // 对讲功能
        is_talk = TRUE;
        YX_Net_Receive_TK(pMe,pContent,cont_len);
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "MESSAGE", ret ) == 0)
    {
        //短语显示设置指令
        YX_Net_Receive_MESSAGE(pMe,pContent,cont_len);
    }
    else if(ret >= 3 &&  strncmp( (char *)buf, "PHB", 3 ) == 0)
    {
        //设置电话本
        uint16 idx = 0;
        if(ret > 3)
        {
            idx = atoi(buf+3);
        }
        YX_Net_Receive_PHB(pMe,pContent,cont_len,idx);
        #if 0 //不需要定时开关机
        if(SCI_STRLEN(yx_DB_White_Reclist[0].white_name) > 0 || SCI_STRLEN(yx_DB_White_Reclist[0].white_num) > 0)
        {
            YX_SetDefaultBootOnShutDownTime();
        }
        #endif
    }
    else if(ret >= 3 &&  strncmp( (char *)buf, "PHL", 3 ) == 0)
    {
        //设置电话本50条
        YX_Net_Receive_PHL(pMe,pContent,cont_len,0);
        #if 0 //不需要定时开关机
        if(SCI_STRLEN(yx_DB_White_Reclist[0].white_name) > 0 || SCI_STRLEN(yx_DB_White_Reclist[0].white_num) > 0)
        {
            YX_SetDefaultBootOnShutDownTime();
        }
        #endif
    }
    else if(ret >= 9 &&  strncmp( (char *)buf, "WHITELIST", 9) == 0)
    {
        //设置白名单
        uint16 idx = 0;
        if(ret > 9)
        {
            idx = atoi(buf+9);
        }
        YX_Net_Receive_WHITELIST(pMe,pContent,cont_len,idx);
    }
#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
    else if(ret == 6 &&  strncmp( (char *)buf, "UPGCUL", ret ) == 0)
    {
        //获取系统好友信息
        YX_Net_Receive_UPGCUL(pMe,pContent,cont_len);
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "PPQ", ret ) == 0)
    {
        //获取好友信息
        YX_Net_Receive_PPQ(pMe,pContent,cont_len);
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "PP", ret ) == 0)
    {
        //碰碰交友返回
        YX_Net_Receive_PP(pMe,pContent,cont_len);
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "PPR", ret ) == 0)
    {
        //解除碰碰交友
        YX_Net_Receive_PPR(pMe,pContent,cont_len);
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "TK2", ret ) == 0)
    {
        // 单聊
        is_talk = TRUE;
        YX_Net_Receive_TK2(pMe,pContent,cont_len);
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "STK", ret ) == 0)
    {
        // 单聊
        is_talk = TRUE;       
        YX_Net_Receive_STK(pMe,pContent,cont_len);
    }
#endif
    else if(ret == 7 &&  strncmp( (char *)buf, "PROFILE", ret ) == 0)
    {
        // 情景模式
        YX_Net_Receive_PROFILE(pMe,pContent,cont_len);
    }
    else if(ret == 8 &&  strncmp( (char *)buf, "FALLDOWN", ret ) == 0)
    {
        //跌倒提醒开关
        YX_Net_Receive_FALLDOWN(pMe,pContent,cont_len);
    }
    else if(ret == 8 &&  strncmp( (char *)buf, "RCAPTURE", ret ) == 0)
    {
        //  远程拍照
        YX_Net_Receive_RCAPTURE(pMe);
    }
    else if(ret == 8 &&  strncmp( (char *)buf, "HRTSTART", ret ) == 0)
    {
        //心率协议
        YX_Net_Receive_HRTSTART(pMe,pContent,cont_len);
    }
    //yangyu add
#if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
    else if(ret == 7 &&  strncmp( (char *)buf, "WDSTART", ret ) == 0)
    {
       //体温
       YX_Net_Receive_WDSTART(pMe,pContent,cont_len);
    }
#endif

    //yangyu end
    else if(ret == 2 &&  strncmp( (char *)buf, "WT", ret ) == 0)
    {
        //天气
        YX_Net_Receive_WT(pMe,pContent,cont_len);
    }
    else if(ret == 4 &&  strncmp( (char *)buf, "BAND", ret ) == 0)
    {
        //获取绑定状态
        YX_Net_Receive_BAND(pMe,pContent,cont_len);
    }
#ifdef ZTE_WEATHER_SUPPORT
    else if(ret == 7 &&  strncmp( (char *)buf, "WT_FUTR", ret ) == 0)
    {
        //天气 未来三天天气信息
        YX_Net_Receive_WT_FUTR(pMe,pContent,cont_len);
    }
#endif
    else if(ret == 8 &&  strncmp( (char *)buf, "SCHEDULE", ret ) == 0)//add_schedule
    {
        //课程表
        YX_Net_Receive_SCHEDULE(pMe,pContent,cont_len);
    }	
    else if(ret == 6 &&  strncmp( (char *)buf, "REJECT", ret ) == 0)
    {
        //reject unknown call
        YX_Net_Receive_REJECT(pMe,pContent,cont_len);
    }
#ifdef ZDT_VIDEOCHAT_SUPPORT
    else if(ret == 6 &&  strncmp( (char *)buf, "UPGVUL", ret ) == 0)
    {
        //get video users
        YX_Net_Receive_VideoUsers(pMe,pContent,cont_len);
    }
    else if(ret == 10 &&  strncmp( (char *)buf, "UPVCALLAPP", ret ) == 0)
    {
        //watch call to app
        YX_Net_Receive_UPVCALLAPP(pMe,pContent,cont_len);
    }
    else if(ret == 8 &&  strncmp( (char *)buf, "DOWNVCWW", ret ) == 0)
    {
        //app call to watch
        YX_Net_Receive_DOWNVCWW(pMe,pContent,cont_len);
    }
    else if(ret ==13 && strncmp( (char *)buf, "DOWNAPPHANGUP", ret ) == 0)
    {
        YX_Net_Recevice_DOWNAPPHANGUP(pMe,pContent,cont_len);
    }

#endif
	//yangyu add below
	else if(ret == 11 && strncmp( (char *)buf, "DEVICE_BODY", ret ) == 0)	
	{
		YX_Net_Receive_DeviceBody(pMe,pContent,cont_len);
	}
	else if(ret == 4 && strncmp( (char *)buf, "LONG", ret ) == 0)	
	{
		//长续航模式
		YX_Net_Receive_LongEndurance(pMe,pContent,cont_len);
	}
	else if(ret == 4 && strncmp( (char *)buf, "AUTO", ret ) == 0)	
	{
		//自动接听
		YX_Net_Receive_AutoAnswer(pMe,pContent,cont_len);
	}
	else if(ret == 6 && strncmp( (char *)buf, "FAMILY", ret ) == 0)	
	{
		//上课禁用允许联系家长
		YX_Net_Receive_FamilyInCallMode(pMe,pContent,cont_len);
	}
	else if(ret == 5 && strncmp( (char *)buf, "CPSMS", ret ) == 0)	
	{
		//短信代收开关(新增)
		YX_Net_Receive_CPSMS(pMe,pContent,cont_len);
	}
	else if(ret == 7 && strncmp( (char *)buf, "UPGRADE", ret ) == 0)	
	{
		//设置远程升级
		YX_Net_Receive_UPGRADE(pMe,pContent,cont_len);
	}
	else if(ret == 3 && strncmp( (char *)buf, "APP", ret ) == 0)	
	{
		//手表应用开关
		YX_Net_Receive_APP(pMe,pContent,cont_len);
	}
    else if(ret == 3 && strncmp( (char *)buf, "CTA", ret ) == 0)	
	{
		//电子网标号码
		YX_Net_Receive_CTA(pMe,pContent,cont_len);
	}
    else if(ret == 5 && strncmp( (char *)buf, "STEPS", ret ) == 0)
    {
        YX_Net_Receive_STEPS(pMe,pContent,cont_len);
    }
    
    if(is_talk)
    {
        //Trace_Log_Buf_Data(pData, DataLen);
        //Trace_Log_Buf_Data("\r\n",2);
    }
    else
    {
        //Trace_Log_Buf_Data(pData, DataLen);
        //Trace_Log_Buf_Data("\r\n",2);
    }

    return 0;
}

static int YX_Net_Receive(YX_APP_T *pMe,uint8 * pData,uint32 Data_len)
{
    uint8 is_md5_head = 0;
    uint32 MsgLen = 0;
    uint16 head_len = g_yx_imei_len + 10;
    if(pData == NULL || Data_len <= head_len)
    {
        ZDT_LOG("YX_Net_Receive Err NULL");
        return 0;
    }
    ZDT_LOG("YX_Net_Receive Start Data_len = %d",Data_len);
    //Trace_Log_Buf_Data((char *)pData,Data_len);
    //Trace_Log_Buf_Data("\r\n",2);

    if(pData[0] != YX_NET_MSG_START 
        //|| pData[1] != YX_NET_MSG_RCV_CS_1 
        //|| pData[2] != YX_NET_MSG_RCV_CS_2 
        || pData[3] != '*'
        || pData[head_len-6] != '*'
        || pData[head_len-1] != '*'
        )
    {
        //ZDT_LOG("ZDT__LOG YX_Net_Receive Err Start");
        head_len += 32;
        if(pData[0] != YX_NET_MSG_START 
            || pData[head_len-6] != '*'
            || pData[head_len-1] != '*'
            )
        {
            return 0;
        }
        is_md5_head = 1;
    }

    MsgLen = YX_Func_HexCharToInt(pData[head_len-5]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-4]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-3]);
    MsgLen = MsgLen<<4;
    MsgLen += YX_Func_HexCharToInt(pData[head_len-2]);
    
    if((MsgLen+head_len) >= Data_len)
    {
        ZDT_LOG("YX_Net_Receive Err Len %d>%d",(MsgLen+head_len),Data_len);
        return 0;
    }
    
    
    if(YX_NET_MSG_END != pData[MsgLen+head_len])
    {
        ZDT_LOG("YX_Net_Receive Err END 0x%02x,0x%02x",pData[MsgLen+head_len]);
    }
    
    if(is_md5_head)
    {
        pData[32] = YX_NET_MSG_START;
        return YX_Net_ReceiveHandle(pMe,pData+32,Data_len-32,MsgLen);
    }
    else
    {
        return YX_Net_ReceiveHandle(pMe,pData,Data_len,MsgLen);
    }
    
}

static BOOLEAN YX_Net_Call_Start(YX_APP_T *pMe,char * number)
{
    uint8 i = 0;

    if(number == NULL)
    {
        return FALSE;
    }
    
    if(SCI_STRLEN(number) == 0)
    {
        return FALSE;
    }
    
    //ZdtCALL_StartByNumber(pMe,number);
    return TRUE;
}

static int YX_Net_HandleSendSuccess(YX_APP_T *pMe,uint8 * pData,uint32 Data_len)
{
    int ret = 0;
    int cont_len = 0;
    uint8 * pContent = NULL;
    uint32 content_len = 0;
    char buf[101] = {0};
    
    cont_len = YX_Net_GetContentLen_Send(pMe,pData,Data_len);
    if(cont_len > 0)
    {
    #if FEATURE_YX_SEND_MD5
        pContent = pData+(g_yx_imei_len+42);
    #else
        pContent = pData+(g_yx_imei_len+10);
    #endif
        content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
    }
    
    if(ret == 2 &&  strncmp( (char *)buf, "LK", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendSuccess--LK--  is_land=%d",pMe->m_yx_is_land);
    #ifdef ZTE_WATCH
        if(pMe->m_yx_lk_send_time > 0) //ZTE 开机累计联网10分钟上报设备信息
        {
            MMIZDT_NV_Set_Netword_Connect_Time(ZDT_Get_TimeSecond()-pMe->m_yx_lk_send_time);
        }       
    #endif
        pMe->m_yx_lk_send_time = ZDT_Get_TimeSecond();
        //IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_ROAM,ANNUN_STATE_ON);
        pMe->m_yx_send_err_num = 0;
        g_yx_lk_timers_num++;
        if(g_yx_lk_timers_num > 0xFFF0)
        {
            g_yx_lk_timers_num = 1;
        }
        MMIZDT_Net_Reset_Restore();
        if(pMe->m_yx_is_land == FALSE)
        {
            YX_Net_Set_Land(TRUE);
            pMe->m_yx_is_land = TRUE;
            YX_LocReport_TimeStop(pMe);
            if(s_yx_is_first_land == FALSE)
            {
                s_yx_is_first_land = TRUE;
                #if 0 // 不自动解绑
                if(MMIZDT_IsICCIDChanged(g_zdt_sim_iccid))
                {
                    is_auto_unbind = TRUE;
                    MMIYX_UnBindDevice();
                    MMIZDT_NVSetICCID(g_zdt_sim_iccid);
                }
                else
                #endif
                {   
                #ifdef ZTE_WATCH
                    YX_Net_Send_RESYNC(pMe);
                #endif
                    YX_Net_Send_TIME(pMe);
                    YX_Net_Send_PHLQ(pMe);
                    YX_Net_Send_WT(pMe,0);
                    YX_Net_Send_SOSQ(pMe);
                    YX_Net_Send_REMINDQ(pMe);
				#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
                    YX_Net_Send_PPQ(pMe);
                    YX_Net_Send_UPGCUL(pMe);
				#endif
                    #if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)
                        YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_CR,NULL,0);
                    #else
                        YX_Net_Send_UD(pMe,0);
                    #endif
                #ifdef ZTE_WATCH
					YX_Net_Send_DeviceModelVersion(pMe);
                    YX_Net_Send_ICCID(pMe);
                    if(MMIZDT_Get_CTA_Net_Id_Status() == 1)
                    {
                        YX_Net_Send_CTA(pMe);
                    }
                    if(MMIZDT_IsICCIDChanged(g_zdt_sim_iccid))
                    {
                        MMIZDT_NVSetICCID(g_zdt_sim_iccid);
                    }
                #else
                    YX_Net_Send_SCHEDULEQ(pMe);
                #endif
                }
            }
            else
            {
                YX_Net_Send_UD(pMe,0);
            }
            //CoreApp_RestBatStauts();
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
            YX_Net_Send_TKQ(pMe);
            YX_Net_Send_TKQ2(pMe);
            YX_Net_Send_STKQ(pMe);
#endif
        }
        else
        {
        #ifdef ZTE_WATCH //热插拔换卡
            if(MMIZDT_IsICCIDChanged(g_zdt_sim_iccid))
            {
                MMIZDT_NVSetICCID(g_zdt_sim_iccid);
                YX_Net_Send_ICCID(pMe);
            }
        #endif
            if(YX_LocReport_In_LK(g_yx_lk_timers_num))
            {
                if(YX_LocReport_JudgeSend())
                {
                   SCI_TraceLow("wuxx, YX_Net_HandleSendSuccess, 01,yx_DB_Set_Rec.time_interval=%d", yx_DB_Set_Rec.time_interval);
                    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOCREPORT,NULL,0);
                }
            }
        }
        pMe->m_yx_allow_rereg = TRUE;
        if(g_yx_cur_heart_change == FALSE)
        {
            uint32 cur_sec = ZDT_Get_TimeSecond();
            if((cur_sec - g_yx_power_on_time_min ) > 600)
            {
                g_yx_cur_heart_time = YX_TCP_LK_TIME_OUT;
                YX_LocReport_TimeSet(yx_DB_Set_Rec.time_interval);
                SCI_TraceLow("wuxx, YX_Net_HandleSendSuccess, 02,yx_DB_Set_Rec.time_interval=%d", yx_DB_Set_Rec.time_interval);
                MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOCREPORT,NULL,0);
                g_yx_cur_heart_change = TRUE;
            }
        }
        YX_Heart_TimeStart(pMe,g_yx_cur_heart_time);
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "UD", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendSuccess--UD-- time_interval=%d",yx_DB_Set_Rec.time_interval);
        SCI_TraceLow("wuxx, YX_Net_HandleSendSuccess, 02,g_yx_loc_timer_interval=%d", g_yx_loc_timer_interval);
        SCI_TraceLow("wuxx, YX_Net_HandleSendSuccess, 02,yx_DB_Set_Rec.time_interval=%d", yx_DB_Set_Rec.time_interval);
        #if defined(ZDT_PLAT_YX_SUPPORT_ZTE)// wuxx_20231219
        SCI_TraceLow("wuxx, YX_Net_HandleSendSuccess, 02,pre,recv,g_YX_Net_Receive_UPLOAD_min_interval=%d", g_YX_Net_Receive_UPLOAD_min_interval);
        #endif
        YX_LocReport_TimeStart(pMe,g_yx_loc_timer_interval);
    }
    else if(ret == 4 &&  strncmp( (char *)buf, "CALL", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendSuccess--CALL-- g_yx_net_call_number=%s",g_yx_net_call_number);
        YX_Net_Call_Start(pMe,g_yx_net_call_number);
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "FACTORY", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendSuccess--FACTORY--");
        YX_RestorFactory();
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "MONITOR", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendSuccess--MONITOR-- g_yx_net_monitorl_number=%s",g_yx_net_monitorl_number);
        if(YX_Net_Call_Start(pMe,g_yx_net_monitorl_number))
        {
            is_zdt_jt_mode = TRUE;
        }
    }
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    else if(ret == 2 &&  strncmp( (char *)buf, "TK", ret ) == 0)
    {
        SCI_MEMSET (buf, 0, sizeof (buf))
        ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
        ZDT_LOG("YX_Net_HandleSendSuccess--STK:msg_type:%s",buf);
        if(content_len > 10)
        {
            ZDT_LOG("YX_Net_HandleSendSuccess--TK-- Data_len=%d ",Data_len);
            SCI_MEMSET (buf, 0, sizeof (buf))
            ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
            ZDT_LOG("YX_Net_HandleSendSuccess--msg_type:%s",buf);
            if(ret == 2 && strncmp( (char *)buf, "-4", ret ) == 0)//表情
            {
                ZDT_LOG("YX_Net_HandleSendSuccess--TK emoji");
            }
            else
            {
                YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_SUCCESS);
            }           
        }
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "TK2", ret ) == 0)
    {
        if(content_len > 10)
        {
            ZDT_LOG("ZDT__LOG YX_Net_HandleSendSuccess--TK2-- Data_len=%d ",Data_len);
            SCI_MEMSET (buf, 0, sizeof (buf))
            ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
            ZDT_LOG("YX_Net_HandleSendSuccess--TK2-- uid:%s",buf);
            SCI_MEMSET (buf, 0, sizeof (buf))
            ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
            ZDT_LOG("YX_Net_HandleSendSuccess--TK2:msg_type:%s",buf);
            if(ret == 2 && strncmp( (char *)buf, "-4", ret ) == 0)//表情
            {
                ZDT_LOG("YX_Net_HandleSendSuccess--TK2 emoji");
            }
            else
            {
                YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_SUCCESS);
            }
        }
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "STK", ret ) == 0)
    {
        if(content_len > 10)
        {
            ZDT_LOG("YX_Net_HandleSendSuccess--STK-- Data_len=%d ",Data_len);
            SCI_MEMSET (buf, 0, sizeof (buf))
            ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
            ZDT_LOG("YX_Net_HandleSendSuccess--STK-- uid:%s",buf);
            SCI_MEMSET (buf, 0, sizeof (buf))
            ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
            ZDT_LOG("YX_Net_HandleSendSuccess--STK:msg_type:%s",buf);
            if(ret == 2 && strncmp( (char *)buf, "-4", ret ) == 0) //表情
            {
                ZDT_LOG("YX_Net_HandleSendSuccess--STK emoji");
            }
            else
            {
               YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_SUCCESS);
            }
        }
    }
#endif
    else if(ret == 3 &&  strncmp( (char *)buf, "IMG", ret ) == 0)
    {
            ZDT_LOG("YX_Net_HandleSendSuccess--IMG-- Data_len=%d ",Data_len);
            YX_Net_Send_CapFile_End(pMe,YX_CAPSEND_SUCCESS);
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "AL", ret ) == 0)
    {
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "DLT", ret ) == 0)
    {
        if(is_auto_unbind)
        {
            YX_RestorBind();
            is_auto_unbind = FALSE;
        }
        else
        {
            YX_RestorFactory();
            MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_UNBIND_SUCCESS,TXT_NULL,IMAGE_PUBWIN_SUCCESS,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        }
    }

    return ret;
}

static int YX_Net_HandleSendFail(YX_APP_T *pMe,uint8 * pData,uint32 Data_len,uint16 err_id)
{
    int ret = 0;
    int cont_len = 0;
    uint8 * pContent = NULL;
    uint32 content_len = 0;
    char buf[101] = {0};
    BOOLEAN is_land_fail = FALSE;
    
    cont_len = YX_Net_GetContentLen_Send(pMe,pData,Data_len);
    if(cont_len > 0)
    {
    #if FEATURE_YX_SEND_MD5
        pContent = pData+(g_yx_imei_len+42);
    #else
        pContent = pData+(g_yx_imei_len+10);
    #endif
        content_len = cont_len;
        ret = YX_Func_GetNextPara(&pContent, &content_len,buf,100);
        if(ret > 0)
        {
            YX_Func_strupr(buf);
        }
    }
    
    if(ret == 2 &&  strncmp( (char *)buf, "LK", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendFail(%d)--LK--  is_land=%d,m_yx_reg_err_num=%d",err_id,pMe->m_yx_is_land,pMe->m_yx_send_err_num);
        if(err_id != TCP_ERR_CLEAR && err_id != TCP_ERR_INCALL)
        {
            is_land_fail = TRUE;
            pMe->m_yx_send_err_num++;
            if(pMe->m_yx_send_err_num > 1)
            {
                if(pMe->m_yx_net_is_init)
                {
                    MMIZDT_Net_Reset(FALSE);
                }
            }
            YX_Heart_TimeStart(pMe,g_yx_cur_heart_time);
        }
        else if(err_id == TCP_ERR_INCALL)
        {
            YX_Heart_TimeStart(pMe,g_yx_cur_heart_time);
        }
        pMe->m_yx_allow_rereg = TRUE;
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "UD", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendFail(%d)--UD-- time_interval=%d",err_id,yx_DB_Set_Rec.time_interval);
        if(err_id != TCP_ERR_CLEAR)
        {
            YX_LocReport_TimeStart(pMe,g_yx_loc_timer_interval);
        }
    }
    else if(ret == 2 &&  strncmp( (char *)buf, "AL", ret ) == 0)
    {
        if(err_id != TCP_ERR_CLEAR)
        {
            uint16 alert_type = YX_Net_Get_AL_CDMA_AlertType(pMe,pContent,content_len);
            ZDT_LOG("YX_Net_HandleSendFail(%d)--AL--Type(%d)",err_id,alert_type);
            YX_Net_Send_AL(pMe,alert_type,0);
        }
    }
    else if(ret == 4 &&  strncmp( (char *)buf, "CALL", ret ) == 0)
    {
        if(err_id != TCP_ERR_CLEAR)
        {
            YX_Net_Call_Start(pMe,g_yx_net_call_number);
        }
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "FACTORY", ret ) == 0)
    {
        ZDT_LOG("YX_Net_HandleSendFail(%d)--FACTORY--",err_id);
        YX_RestorFactory();
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "MONITOR", ret ) == 0)
    {
        if(err_id != TCP_ERR_CLEAR)
        {
            YX_Net_Call_Start(pMe,g_yx_net_monitorl_number);
        }
    }
#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
    else if(ret == 2 &&  strncmp( (char *)buf, "PP", ret ) == 0)
    {
        YX_Net_Friend_End(FALSE,YX_FRIEND_SEARCH_ERR,PNULL,PNULL);
    }
#endif
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    else if(ret == 2 &&  strncmp( (char *)buf, "TK", ret ) == 0)
    {
        if(content_len > 10)
        {
            if(err_id != TCP_ERR_CLEAR)
            {
                if(err_id == TCP_ERR_READ)
                {
                    YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_FILE);
                }
                else
                {
                    YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_NET);
                }
            }
            else
            {
                YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_UNREG);
            }
        }
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "TK2", ret ) == 0)
    {
        if(content_len > 10)
        {
            if(err_id != TCP_ERR_CLEAR)
            {
                if(err_id == TCP_ERR_READ)
                {
                    YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_FILE);
                }
                else
                {
                    YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_NET);
                }
            }
            else
            {
                YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_UNREG);
            }
        }
    }
    else if(ret == 3 &&  strncmp( (char *)buf, "STK", ret ) == 0)
    {
        if(content_len > 10)
        {
            if(err_id != TCP_ERR_CLEAR)
            {
                if(err_id == TCP_ERR_READ)
                {
                    YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_FILE);
                }
                else
                {
                    YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_NET);
                }
            }
            else
            {
                YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_UNREG);
            }
        }
    }
#endif
    else if(ret == 3 &&  strncmp( (char *)buf, "IMG", ret ) == 0)
    {
            ZDT_LOG("YX_Net_HandleSendFail--IMG-- Data_len=%d ",Data_len);
            if(err_id != TCP_ERR_CLEAR)
            {
                if(err_id == TCP_ERR_READ)
                {
                    YX_Net_Send_CapFile_End(pMe,YX_CAPSEND_ERR_FILE);
                }
                else
                {
                    YX_Net_Send_CapFile_End(pMe,YX_CAPSEND_ERR_NET);
                }
            }
            else
            {
                YX_Net_Send_CapFile_End(pMe,YX_CAPSEND_ERR_UNREG);
            }
    }

    else if(ret == 3 &&  strncmp( (char *)buf, "DLT", ret ) == 0)
    {
        if(is_auto_unbind)
        {
            YX_RestorBind();
            is_auto_unbind = FALSE;
        }
        else
        {
            //解绑失败!  
         YX_RestorFactory();
            MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_UNBIND_FAIL,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        }

    }
    
    if(TCP_ERR_NET != err_id && TCP_ERR_CLEAR != err_id && TCP_ERR_READ != err_id && TCP_ERR_INCALL != err_id)
    {
        pMe->m_yx_rsp_err_num++;
        if(pMe->m_yx_rsp_err_num >= 5)
        {
            ZDT_LOG("YX_Net_HandleSendFail RESET");
            if(is_land_fail == FALSE)
            {
                MMIZDT_Net_Reset(FALSE);
            }
            pMe->m_yx_rsp_err_num = 0;
        }
    }
    else if(ret == 7 &&  strncmp( (char *)buf, "SENDSOS", ret ) == 0)//努比亚sos上报位置
    {
        if(err_id != TCP_ERR_CLEAR)
        {
            pMe->m_yx_send_err_num++;
            ZDT_LOG("YX_Net_HandleSendFail(%d)--SENDSOS--Type(%d) retry(%d)",err_id,YX_NET_ALERT_TYPE_SOS,pMe->m_yx_send_err_num);
            if(pMe->m_yx_send_err_num < 3)
            {
                YX_Net_Send_AL(pMe,YX_NET_ALERT_TYPE_SOS,0);
            }
        }
    }
    return ret;
}

static void YX_Land_TimeOut(
                                uint8 timer_id,
                                uint32 param
                                )
{
    YX_Net_Reg();
    return;
}

static void YX_Land_TimeStart(YX_APP_T *pMe)
{
    if(0 != pMe->m_yx_land_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_land_timer_id);
        pMe->m_yx_land_timer_id = 0;
    }
    
    pMe->m_yx_land_timer_id = MMK_CreateTimerCallback(5000, 
                                                                        YX_Land_TimeOut, 
                                                                        (uint32)pMe, 
                                                                        FALSE);
    return;
}

static void YX_Land_TimeStop(YX_APP_T *pMe)
{
    if(0 != pMe->m_yx_land_timer_id)
    {
        MMK_StopTimer(pMe->m_yx_land_timer_id);
        pMe->m_yx_land_timer_id = 0;
    }
    
    return;
}

static int YX_Net_Land_CallBack(void * pUser,TCP_CB_TYPE_E cb_type,uint16 wParam, uint32 dwParam)
{
    int res = 0;
    ZDT_TCP_INTERFACE_T *pTcp = (ZDT_TCP_INTERFACE_T *)pUser;
    YX_APP_T *pMe = &g_yx_app;
    ZDT_LOG("YX_Net_Land_CallBack: pTcp=0x%xcb_type=%d,Result=%d,Data=0x%x",pTcp,cb_type,wParam,dwParam);
    #if 0
    if(phoneState == PHONE_POWER_OFF)
    {
        return 0;
    }
    #endif
    
    switch(cb_type)
    {
        case TCP_CB_DISCONNECTED:
                ZDT_LOG("YX_Net TCP_CB_DISCONNECTED m_yx_is_land=%d",pMe->m_yx_is_land);
                if(pMe->m_yx_is_land)
                {
                    ZDT_LOG("YX_Net ReLand NET_DISCONNECTED--------------");
                    pMe->m_yx_is_land = FALSE;
                    YX_Net_Set_Land(FALSE);
                 //IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_ROAM,ANNUN_STATE_OFF);        
                    MMIZDT_Net_Reset(FALSE);
                }
            break;
            
        case TCP_CB_REG:
            {
                uint16 ret = wParam;
                uint32 err_id =  dwParam;
                if(ret == TCP_SUCCESS)
                {
                    //注册成功
                    ZDT_LOG("YX_Net_Land TCPReg OK --------------");
                    pMe->m_yx_is_reg = TRUE;
                    pMe->m_yx_is_land = FALSE;
                    YX_Net_Set_Land(FALSE);
                    //pMe->m_yx_send_err_num = 0;
                    pMe->m_yx_rsp_err_num = 0;
                    //IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_ROAM,ANNUN_STATE_OFF);
                    YX_Heart_TimeStop(pMe);
                    YX_LocReport_TimeStop(pMe);
                    g_yx_lk_timers_num = 0;
                    if(pMe->m_yx_net_first_reg == FALSE)
                    {
                        YX_Net_Send_LK(pMe,5,10000);
                        pMe->m_yx_net_first_reg = TRUE;
                    }
                    else
                    {
                        YX_Net_Send_LK(pMe,5,10000);
                    }
                    YX_Land_TimeStop(pMe);
                    #if defined(ZDT_HSENSOR_SUPPORT) && !defined(ZDT_HSENSOR_SUPPORT_TW)
                    ZDT_StartHeartrate(yx_DB_Set_Rec.heart);//yangyu add
                    #endif
                    #if defined(ZDT_HSENSOR_SUPPORT_TW) || defined(ZDT_SUPPORT_TW_GD609) || defined(ZDT_SUPPORT_TW_FM78102)
                    ZDT_StartBodyTw(yx_DB_Set_Rec.body_tw_peroid);
                    #endif
                    
                }
                else
                {
                    //注册失败
                    ZDT_LOG("YX_Net_Land TCPReg ERR --------------");
                    YX_Heart_TimeStop(pMe);
                    YX_LocReport_TimeStop(pMe);
                    if(err_id == TCP_ERR_REGEXIST)
                    {
                        #if 0
                        if(pMe->m_zdt_is_in_call > 0 && CallUtil_Get_Calldraw_Status() == CALLDRAW_STATUS_NONE )
                        {
                            ZdtCALL_API_End();
                        }
                        #endif
                        YX_Land_TimeStart(pMe);
                    }
                    else
                    {
                        pMe->m_yx_is_reg = FALSE;
                        pMe->m_yx_is_land = FALSE;
                        YX_Net_Set_Land(FALSE);
                    }
                }
            }
            break;
            
        case TCP_CB_UNREG:
            {
                uint16 ret = wParam;
                uint32 err_id =  dwParam;
                if(ret == TCP_SUCCESS)
                {
                    //注销成功
                    ZDT_LOG("YX_Net_Land TCPUNReg OK --------------");
                    pMe->m_yx_is_land = FALSE;
                    YX_Net_Set_Land(FALSE);
                }
                else
                {
                    //注销失败
                    ZDT_LOG("YX_Net_Land TCPUNReg ERR --------------");
                }
            }
            break;
            
        case TCP_CB_SEND:
            {
                uint16 ret = wParam;
                ZDT_TCP_LINK_DATA_T * SendData = (ZDT_TCP_LINK_DATA_T*)dwParam;
                #if 0
                if(pMe->m_zdt_is_in_call > 0 && CallUtil_Get_Calldraw_Status() == CALLDRAW_STATUS_NONE )
                {
                    ZdtCALL_API_End();
                }
                #endif
                if(ret == TCP_SUCCESS)
                {
                    //发送成功
                    ZDT_LOG("YX_Net_TCPSend Result Success Handle=0x%x",SendData);
                    if(SendData != NULL && SendData->str != NULL)
                    {
                        YX_Net_HandleSendSuccess(pMe,SendData->str,SendData->len);                        
                    }
                }
                else
                {
                    //发送失败
                    ZDT_LOG("YX_Net_TCPSend Result FAIL Handle=0x%x",SendData);
                    if(SendData != NULL && SendData->str != NULL)
                    {
                        YX_Net_HandleSendFail(pMe,SendData->str,SendData->len,ret);
                    }
                    
                }
            }
            break;
            
        case TCP_CB_RCV:
            {
                uint16 ret = wParam;
                ZDT_TCP_RCV_DATA_T * RcvData = (ZDT_TCP_RCV_DATA_T *) dwParam;
                if(ret == TCP_SUCCESS)
                {
                    //接收成功
                    if(RcvData != NULL && RcvData->pRcv!= NULL)
                    {
                        res = YX_Net_Receive(pMe,RcvData->pRcv,RcvData->len);
                    }
                    ZDT_LOG("YX_Net_Receive Over");
                }
                else
                {
                    //接收失败
                    ZDT_LOG("YX_Net_Receive FAIL");
                }
            }
            break;
                        
        default:
            break;
    }
    return res;
}

int YX_Net_Rereg(YX_APP_T* pMe)
{
    if(FALSE == YX_Net_IsInit())
    {
        return 2;
    }

    if(pMe->m_yx_is_land == FALSE && pMe->m_yx_allow_rereg)
    {
        ZDT_LOG("YX_Net_Rereg %s:%d",yx_DB_Set_Rec.ip_domain,yx_DB_Set_Rec.ip_port);
        pMe->m_yx_allow_rereg = FALSE;
        YX_Net_Reset(pMe);
    }
    return 0;
}

int YX_Net_Reg(void)
{
    ZDT_LOG("YX_Net_Reg %s:%d",yx_DB_Set_Rec.ip_domain,yx_DB_Set_Rec.ip_port);
    if(FALSE == YX_Net_IsInit())
    {
        return ZDT_TCP_RET_ERR;
    }

    ZDT_API_TCP_UnReg(g_yx_app.m_yx_tcp_reg_interface);
    ZDT_API_TCP_Reg(g_yx_app.m_yx_tcp_reg_interface,(char *)yx_DB_Set_Rec.ip_domain,yx_DB_Set_Rec.ip_port,YX_Net_Land_CallBack);
    return 0;
}

extern void YX_Net_CloseAllTimer(YX_APP_T *pMe)
{
    YX_Land_TimeStop(pMe);
    YX_Heart_TimeStop(pMe);
    YX_LocReport_TimeStop(pMe);
    YX_Find_TimeStop(pMe);
    return;
}

extern BOOLEAN YX_Net_IsAllow(YX_APP_T* pMe)
{
    if(yx_DB_Set_Rec.net_open &&  pMe->m_yx_net_is_init)
    {
        return TRUE;
    }
    ZDT_LOG("YX_Net_IsAllow FALSE");
    return FALSE;
}

extern BOOLEAN YX_Net_IsOpen(YX_APP_T* pMe)
{
    return pMe->m_yx_net_is_open;
}

extern int YX_Net_Open(YX_APP_T* pMe)
{
    ZDT_LOG("YX_Net_Open");
    pMe->m_yx_is_reg = FALSE;
    pMe->m_yx_is_land = FALSE;
    YX_Net_Set_Land(FALSE);
    //IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_ROAM,ANNUN_STATE_OFF);    
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    if(pMe->m_yx_voc_is_sending)
    {
        YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_UNREG);
        pMe->m_yx_voc_is_sending = FALSE;
    }
    else
    {
        YX_VOC_Send_Link_DelAll(pMe);
    }
#endif
    YX_Net_Reg();
    pMe->m_yx_net_is_open = TRUE;
    return 0;
}

extern int YX_Net_Close(YX_APP_T* pMe)
{
    ZDT_LOG("YX_Net_Close");
    pMe->m_yx_is_reg = FALSE;
    pMe->m_yx_is_land = FALSE;
    YX_Net_Set_Land(FALSE);
    //IANNUNCIATOR_SetField((IAnnunciator*)CoreApp_GetAnnunciator(),ANNUN_FIELD_ROAM,ANNUN_STATE_OFF);    
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    if(pMe->m_yx_voc_is_sending)
    {
        YX_Net_Send_TK_VocFile_End(pMe,YX_VOCSEND_ERR_UNREG);
        pMe->m_yx_voc_is_sending = FALSE;
    }
    else
    {
        YX_VOC_Send_Link_DelAll(pMe);
    }
#endif
    ZDT_API_TCP_UnReg(g_yx_app.m_yx_tcp_reg_interface);
    pMe->m_yx_net_is_open = FALSE;
   return 0;
}

#ifdef ZDT_PLAT_YX_SUPPORT_YS_AES
static int yx_net_iv_rand(void)
{
    static int yx_rand_offset = 16345;
    char temp_buf[100] = {0};
    uint16 temp_len = 0;
    int radom = 0;
    srand(SCI_GetTickCount()+yx_rand_offset);
    yx_rand_offset++;
    radom = rand();
    sprintf(temp_buf,"YS%d",radom);
    temp_len = SCI_STRLEN(temp_buf);
    if(temp_len > 16)
    {
        temp_len = 16;
    }
    SCI_MEMCPY(g_yx_iv_str,"zdtonetttecellsz",16);
    SCI_MEMCPY(g_yx_iv_str,temp_buf,temp_len);
    return radom;
}
#endif

extern int YX_Net_Init(void)
{
    YX_APP_T * pMe = &g_yx_app;
    ZDT_LOG("YX_Net_Init");
    g_yx_power_on_time_min = ZDT_Get_TimeSecond();
#ifdef ZDT_PLAT_YX_SUPPORT_YS_AES
    yx_net_iv_rand();
#endif
    if(ZdtTCP_NetOpen(&(g_yx_app.m_yx_tcp_reg_interface),YX_CMD_CheckAll,10240))
    {
        ZDT_LOG("YX_Net_Init Reg OK pMe=0x%x",g_yx_app.m_yx_tcp_reg_interface);
    }
    pMe->m_yx_net_is_init = TRUE;
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    //YX_Voice_FileSendAllRemove(pMe);
#endif
    SCI_TraceLow("wuxx, YX_Net_Init, 01,yx_DB_Set_Rec.time_interval=%d", yx_DB_Set_Rec.time_interval);
    YX_LocReport_TimeSet(yx_DB_Set_Rec.time_interval);
#if 0
    if(YX_Net_IsAllow(pMe))
    {
        YX_Net_Open(pMe);
    }
#endif
    return 0;
}

BOOLEAN YX_Net_IsInit(void)
{
    return g_yx_app.m_yx_net_is_init;
}

extern int YX_Net_Reset(YX_APP_T* pMe)
{
    ZDT_LOG("YX_Net_Reset");
    if(FALSE == YX_Net_IsInit())
    {
        return 1;
    }
    if(YX_Net_IsAllow(pMe))
    {
        YX_Net_Open(pMe);
    }
    else
    {
        YX_Net_Close(pMe);
    }
    return 0;
}

extern int YX_Net_TimerCheck_WT(SCI_DATE_T dt, SCI_TIME_T tm)
{
    if(g_yx_wt_oneday_start == 0)
    {
        if(tm.hour>= 6)
        {
            g_yx_wt_recieve_ok = FALSE;
            g_yx_wt_oneday_start = 1;
        }
    }
    else if(g_yx_wt_oneday_start == 1)
    {
        if(tm.hour >= 12)
        {
            g_yx_wt_recieve_ok = FALSE;
            g_yx_wt_oneday_start = 2;
        }
    }
    else if(g_yx_wt_oneday_start == 2)
    {
        if(tm.hour >= 18)
        {
            g_yx_wt_recieve_ok = FALSE;
            g_yx_wt_oneday_start = 3;
        }
    }
    return 0;
}
extern int YX_Net_TimerCheck(SCI_DATE_T dt, SCI_TIME_T tm)
{
    YX_APP_T* pMe = &g_yx_app;
    uint32 cur_time = ZDT_Get_TimeSecond();
    uint32 last_time = pMe->m_yx_lk_send_time;
    static uint32  s_yx_net_check_times = 0;
    s_yx_net_check_times++;
    if(s_yx_net_check_times > 0xFFFFFFF0)
    {
        s_yx_net_check_times = 0;
    }
    
    YX_Net_TimerCheck_WT(dt,tm);

    if(last_time == 0)
    {
        if((s_yx_net_check_times%3) == 0 && ZDT_SIM_Exsit() && YX_Net_IsAllow(pMe))
        {
            ZDT_LOG("YX_Net_TimerCheck Err IN  PDP");
            MMIZDT_Net_Reset(FALSE);
            return 1;
        }
    }
    else
    {
        //判断网络连接是否出错当大于20分钟没有发送心跳包时网络重启
        if(ZDT_SIM_Exsit() && YX_Net_IsAllow(pMe) && last_time != 0 && (cur_time - last_time) > 1300)
        {
            ZDT_LOG("YX_Net_TimerCheck Err IN  LK");
            pMe->m_yx_lk_send_time = last_time;
            MMIZDT_Net_Reset(FALSE);
            return 1;
        }
    }
    return 0;
}
extern int YX_API_Net_Reset(void)
{
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_NET_RESET,NULL,0);
    return 0;
}

extern int YX_API_CZSB_Stop(void)
{
#if 1//xiongkai add   for bug: 设备充电灭屏时-APP查找设备-设备被查找界面短按HOME键-界面消失，其铃声不会立即停止-拔掉充电线，设备仍显示查找界面；
    YX_APP_T * pMe = &g_yx_app;
    YX_Find_TimeStop(pMe); 
#endif

    if(g_is_yx_czsb_is_playing)
    {
        ZdtWatch_Factory_Speaker_Stop();
        g_is_yx_czsb_is_playing = FALSE;
    }
    return 0;
}

extern int YX_API_SeNetOpen(void)
{
    if(yx_DB_Set_Rec.net_open == 0)
    {
        yx_DB_Set_Rec.net_open = 1;
        YX_DB_SET_ListModify();
        MMIZDT_Net_Reset(TRUE);
        return 1;
    }
    return 0;
}

extern int YX_API_SeNetClose(void)
{
    if(yx_DB_Set_Rec.net_open > 0)
    {
        yx_DB_Set_Rec.net_open = 0;
        YX_DB_SET_ListModify();
        MMIZDT_Net_Close();
        return 1;
    }
    return 0;
}

extern uint8 YX_API_SetLand(void)
{
    if(yx_DB_Set_Rec.net_open == 0)
    {
        YX_API_SeNetOpen();
    }
    else
    {
        YX_API_SeNetClose();
    }
    SCI_TRACE_LOW("YX_API_SetLand is_land=%d ",yx_DB_Set_Rec.is_land);
    return 1;
}

extern int YX_API_WT_Send(void)
{
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_WT,NULL,0);
    return 0;
}

extern int YX_API_DLT_Send(void)
{
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_UNBIND,NULL,0);
    return 0;
}

extern int YX_API_WT_Reset(void)
{
    g_yx_wt_oneday_start = 0;
    return 0;
}


BOOLEAN MMIYX_SetNetOpen(void)
{
    YX_API_SeNetOpen();
    //MMIZDT_OpenWatchFactoryWin();
    return 0;
}

BOOLEAN MMIYX_SetNetClose(void)
{
   YX_API_SeNetClose();
    //MMIZDT_OpenWatchFactoryWin();
    return 0;
}

BOOLEAN MMIYX_UnBindDevice(void)
{
    YX_API_DLT_Send();
    return 0;
}


#endif

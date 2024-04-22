/*****************************************************************************
** Copyright 2023 Unisoc(Shanghai) Technologies Co.,Ltd                      *
** Licensed under the Unisoc General Software License,                       *
** version 1.0 (the License);                                                *
** you may not use this file except in compliance with the License.          *
** You may obtain a copy of the License at                                   *
** https://www.unisoc.com/en_us/license/UNISOC_GENERAL_LICENSE_V1.0-EN_US    *
** Software distributed under the License is distributed on an "AS IS" BASIS,*
** WITHOUT WARRANTIES OF ANY KIND, either express or implied.                *
** See the Unisoc General Software License, version 1.0 for more details.    *
******************************************************************************/

/*******************************************************************************
** File Name:       mmisfr_cmcc_lwm2m.h                                        *
** Author:          miao.liu2                                                  *
** Date:            03/04/2022                                                 *
** Description:    This file is used to define cmcc lwm2m export api           *
********************************************************************************
**                         Important Edit History                              *
** ----------------------------------------------------------------------------*
** DATE                 NAME                  DESCRIPTION                      *
** 03/04/2022          miao.liu2               Create                          *
********************************************************************************/

#ifndef _MMISFR_CMCC_LWM2M_H_
#define _MMISFR_CMCC_LWM2M_H_

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
#include "time.h"
#include "ual_common.h"

/*---------------------------------------------------------------------------*
**                            MACRO DEFINITION                               *
**---------------------------------------------------------------------------*/
#ifdef SFR_SUPPORT_CMCC
#ifndef WIN32
#define TRACE_SFR_CMCC(_format,...) \
            do \
            { \
                SCI_TraceLow("{%s}[%s]<%d> "_format, "SFR_CMCC", __FUNCTION__, __LINE__, ##__VA_ARGS__);\
            }while(0)

#else
#define TRACE_SFR_CMCC(_format,...) \
            do \
            { \
                printf("{%s}<%d>"_format, "SFR_CMCC",  __LINE__, ##__VA_ARGS__);\
            }while(0)
#endif
#endif

/**--------------------------------------------------------------------------*
**                         TYPE DEFINITION                                   *
**---------------------------------------------------------------------------*/
#define MMISFR_CMCC_STR_LEN (64)

typedef enum
{
    MSG_MMISFR_CMCC_REGISTER_SUCCESS,
    MSG_MMISFR_CMCC_REGISTER_FAIL,
    MSG_MMISFR_CMCC_UPDATE_FAIL,
    MSG_MMISFR_CMCC_IP_CHANGED,
    MSG_MMISFR_CMCC_MAX
}MMISFR_CMCC_MSG_E;

//register info
typedef enum
{
    MMISFR_CMCC_INDEX_BRAND,
    MMISFR_CMCC_INDEX_MODEL,
    MMISFR_CMCC_INDEX_SDKVERSION,
    MMISFR_CMCC_INDEX_APIVERSION,
    MMISFR_CMCC_INDEX_TEMPLATEID,
    MMISFR_CMCC_INDEX_IMEI,
    MMISFR_CMCC_INDEX_IMEI2,
    MMISFR_CMCC_INDEX_ISMI,
    MMISFR_CMCC_INDEX_DMVERSION,
    MMISFR_CMCC_INDEX_APPKEY,
    MMISFR_CMCC_INDEX_PASSWORD,
    MMISFR_CMCC_INDEX_OPTURL,
    MMISFR_CMCC_INDEX_MAX
}MMISFR_CMCC_INDEX_E;

//device info
typedef enum
{
    MMISFR_CMCC_DEVICEINFO_INDEX_IMSI,
    MMISFR_CMCC_DEVICEINFO_INDEX_SN,
    MMISFR_CMCC_DEVICEINFO_INDEX_DEVINFO,
    MMISFR_CMCC_DEVICEINFO_INDEX_MAC,
    MMISFR_CMCC_DEVICEINFO_INDEX_ROM,
    MMISFR_CMCC_DEVICEINFO_INDEX_RAM,
    MMISFR_CMCC_DEVICEINFO_INDEX_CPU,
    MMISFR_CMCC_DEVICEINFO_INDEX_SYS_VERSION,
    MMISFR_CMCC_DEVICEINFO_INDEX_SOFTWARE_VER,
    MMISFR_CMCC_DEVICEINFO_INDEX_SOFTWARE_NAME,
    MMISFR_CMCC_DEVICEINFO_INDEX_NETTYPE,
    MMISFR_CMCC_DEVICEINFO_INDEX_PHONENUMBER,
    MMISFR_CMCC_DEVICEINFO_INDEX_BATTERYCAPACITY,
    MMISFR_CMCC_DEVICEINFO_INDEX_SCREENSIZE,
    MMISFR_CMCC_DEVICEINFO_INDEX_NETWORKSTATUS,
    MMISFR_CMCC_DEVICEINFO_INDEX_WEARINGSTATUS,
    MMISFR_CMCC_DEVICEINFO_INDEX_ROUTER_MAC,
    MMISFR_CMCC_DEVICEINFO_INDEX_BT_MAC,
    MMISFR_CMCC_DEVICEINFO_INDEX_GPU,
    MMISFR_CMCC_DEVICEINFO_INDEX_BOARD,
    MMISFR_CMCC_DEVICEINFO_INDEX_RESOLUTION,
    MMISFR_CMCC_DEVICEINFO_INDEX_APP_INFO,
    MMISFR_CMCC_DEVICEINFO_INDEX_IMSI2,
    MMISFR_CMCC_DEVICEINFO_INDEX_BATTERYCAPACITYCURR,
    MMISFR_CMCC_DEVICEINFO_INDEX_MAX
}MMISFR_CMCC_DEVICEINFO_INDEX_E;

typedef enum
{
    MMISFR_CMCC_RULE_CONFIG_REPORTTIME = 0,
    MMISFR_CMCC_RULE_CONFIG_REPORTNUM,
    MMISFR_CMCC_RULE_CONFIG_RETRYINTERVAL,
    MMISFR_CMCC_RULE_CONFIG_RETRYNUM,
    MMISFR_CMCC_RULE_CONFIG_HEARTBEATTIME,
    MMISFR_CMCC_RULE_CONFIG_MAX
}MMISFR_CMCC_RULE_CONFIG_E;

typedef struct
{
    uint32      msg_id;
    uint32      time_out;
} MMISFR_CMCC_MSG_T;

typedef struct 
{
    SIGNAL_VARS
    MMISFR_CMCC_MSG_T    *p_content;
}MMISFR_CMCC_SIGNAL_T;

/**--------------------------------------------------------------------------*
**                         FUNCTION DECLARE                                 *
**--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description:nexe loop
//  Parameter: [In] is_successful
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_NextLoop(BOOLEAN is_update, BOOLEAN is_successful);

/*****************************************************************************/
//  Description:清除lwm2m数据
//  Parameter: [In] is_need_stop
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_Lwm2mStop(BOOLEAN is_need_stop);

/*****************************************************************************/
//  Description:init or update lwm2m
//  Parameter: [In] none
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_LwM2MEnter(void);

/*****************************************************************************/
//  Description :set appkey ,password等信息
//  Param :str_index,str
//  Author:
//  Note:miao.liu2
/*****************************************************************************/
PUBLIC BOOLEAN MMISFR_CMCC_SetRegisterInfo(MMISFR_CMCC_INDEX_E str_index, char str[MMISFR_CMCC_STR_LEN]);
/*****************************************************************************/
//  Description :set device info
//  Param :str_index,str
//  Author:
//  Note:miao.liu2
/*****************************************************************************/
PUBLIC BOOLEAN MMISFR_CMCC_SetDeviceInfo(MMISFR_CMCC_DEVICEINFO_INDEX_E deviceinfo_index, char str[MMISFR_CMCC_STR_LEN]);

/*****************************************************************************/
//  Description:收到主动上报网络消息不好的处理
//  Parameter: [In] none
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_ReceivePdpDeactiveInd(void);

/*****************************************************************************/
//  Description:获取rule config的配置
//  Parameter: [In] rule_config
//             [Return] rule_config对应的具体值
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_E rule_config);

/*****************************************************************************/
//  Description:设置rule config的值
//  Parameter: [In] rule_config
//             [In] rule_config对应的具体值
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetRuleConfig(MMISFR_CMCC_RULE_CONFIG_E rule_config, uint32 value);

/*****************************************************************************/
//  Description:获取上报总数
//  Parameter: [In] none
//             [Return] 上报总数
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMISFR_CMCC_GetTotalRoportNum(void);

/*****************************************************************************/
//  Description:设置上报总数
//  Parameter: [In] total_report_num
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetTotalRoportNum(uint32 total_report_num);

/*****************************************************************************/
//  Description:获取报错次数
//  Parameter: [In] none
//             [Return] 报错次数
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMISFR_CMCC_GetFailNum(void);

/*****************************************************************************/
//  Description:设置报错次数
//  Parameter: [In] rule_config
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetFailNum(uint32 fail_num);

/*****************************************************************************/
//  Description:获取自注册开始的时间
//  Parameter: [In] none
//             [Return] 自注册开始的时间
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC time_t MMISFR_CMCC_GetLwm2mStartTime(void);

/*****************************************************************************/
//  Description:设置自注册开始的时间
//  Parameter: [In]  自注册开始的时间
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetLwm2mStartTime(time_t start_time);

#endif

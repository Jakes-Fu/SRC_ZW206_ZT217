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

/*****************************************************************************
** File Name:      mmisfr_cmcc_lwm2m.c                                       *
** Author:         miao.liu2                                                 *
** Date:           3/4/2023                                                  *
** Description:    This file is used to define cmcc self register function   *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 04/2023      miao.liu2             Create                                 *
******************************************************************************/

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/
//#include "std_header.h"
#include <stdio.h>
#include "mn_type.h"
#include "os_api.h"
#include "sig_code.h"
#include "sci_api.h"
#include "ual_timer.h"
#include "time.h"
#include "mmisfr_cmcc_lwm2m.h"
#include "lwm2msdk.h"
#include "liblwm2m.h"
#include "ual_tele_radio.h"
#include "ual_tele_sim.h"
#include "tasks_id.h"

#include "nvitem.h"//add_get_mac

/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                 *
**--------------------------------------------------------------------------*/
#define CMCC_REG_HEART_BEAT_TIME (15*60*1000) //milliseconds - 24 hours (24*60*60*1000)
#define CMCC_REG_REPORT_TIME (24*60*60*1000) //milliseconds - 24 hours
#define CMCC_REG_RETRY_TIME_INTERVAL (10*60*1000) //milliseconds-10mins
#define CMCC_REG_RETRY_MAX_TIMES (1)  //retry max times
#define CMCC_REG_REPORT_NUM (8)
#define CMCC_REG_RESTART_TO_CONNEXT_TIME (2*60*1000) //milliseconds - 2 MIN

/**--------------------------------------------------------------------------*
**                         EXTERNAL DECLARE                                 *
**--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
**                         GLOBAL DEFINITION                                *
**--------------------------------------------------------------------------*/
BOOLEAN s_run_result = FALSE;
BOOLEAN s_in_register = FALSE;

uint32 s_reportTime = CMCC_REG_REPORT_TIME;
uint32 s_reportNum = CMCC_REG_REPORT_NUM;
uint32 s_heartBeatTime = CMCC_REG_HEART_BEAT_TIME;
uint32 s_retryInterval = CMCC_REG_RETRY_TIME_INTERVAL;
uint32 s_retryNum = CMCC_REG_RETRY_MAX_TIMES;

uint32 s_total_reportnum = 0;
time_t s_report_start = 0;
uint32 s_retry_fail_times = 0;
BOOLEAN s_first_report_in_reportTime = TRUE;
#ifdef ZT217_LISENNING_PAD
#define ZDT_CMCC_SFR_MANUF 			"LT"

#define ZDT_CMCC_SFR_TYPE 			"LT-T01"
//软件版本号
#define ZDT_CMCC_SFR_SW_VER 			"ZT217_LT-T01V1.0.1"//"ZW206_K1_RDA_240X284_V01"
//自注册编码
#define ZDT_CMCC_SFR_MODEL 			"NM-ESC-A"
//硬件版本号
#define ZDT_CMCC_SFR_HW_VER 			"ZT217_MB_V1.0.1" // UWS6121E
#else
#ifdef ZTE_WATCH
//型号ZW206--ZW35-K1 -EW2302
//型号ZW202--ZW22-K1 ENTRY-EW2303


//厂商
#define ZDT_CMCC_SFR_MANUF 			"ZTE"
//型号
#ifdef ZTE_WATCH_K1
#define ZDT_CMCC_SFR_TYPE 			"ZW35"
//软件版本号
#define ZDT_CMCC_SFR_SW_VER 			"ZTE_ZW35V1.0.0B12"//"ZW206_K1_RDA_240X284_V01"
//自注册编码
#define ZDT_CMCC_SFR_MODEL 			"NM-ESC-A"
//硬件版本号
#define ZDT_CMCC_SFR_HW_VER 			"ZW206" // UWS6121E
#else
//型号
#define ZDT_CMCC_SFR_TYPE 			"ZW22"
//自注册编码
#define ZDT_CMCC_SFR_MODEL 			"NM-ESC-A"
//软件版本号
#define ZDT_CMCC_SFR_SW_VER 			"ZTE_ZW22V1.0.0B12"//"ZW202_K1_ENTRY_RDA_240X240_V01"//"ZW202_K1_ENTRY_V01"//"NM-ESC-A_V2.3"
//硬件版本号
#define ZDT_CMCC_SFR_HW_VER 			"ZW202" // UWS6121E
#endif

#else
//厂商
#define ZDT_CMCC_SFR_MANUF 			"NM-ESC-A"
//型号
#define ZDT_CMCC_SFR_TYPE 			"H05"
//自注册编码
#define ZDT_CMCC_SFR_MODEL 			"NM-ESC-A"
//软件版本号
#define ZDT_CMCC_SFR_SW_VER 			"1.0.0"
//硬件版本号
#define ZDT_CMCC_SFR_HW_VER 			"1.0.0"

#endif
#endif


char s_cmcc_register_info[MMISFR_CMCC_INDEX_MAX][MMISFR_CMCC_STR_LEN + 1] =
{
    "LT",   //MMISFR_CMCC_INDEX_BRAND
    "LT-T01",   //MMISFR_CMCC_INDEX_MODEL
    "4.0.1",   //MMISFR_CMCC_INDEX_SDKVERSION
    "4.0.1",   //MMISFR_CMCC_INDEX_APIVERSION
    "TY000014",  //MMISFR_CMCC_INDEX_TEMPLATEID
    "",   //MMISFR_CMCC_INDEX_IMEI
    "",   //MMISFR_CMCC_INDEX_IMEI2
    "",   //MMISFR_CMCC_INDEX_ISMI
    "V4.0",  //MMISFR_CMCC_INDEX_DMVERSION
    "M100000514",     //MMISFR_CMCC_INDEX_APPKEY
    "To7Z21d3f205HpQ92d26p51dmwZ9Jl66", //MMISFR_CMCC_INDEX_PASSWORD
    "m.fxltsbl.com",  //MMISFR_CMCC_INDEX_OPTURL
    //"b.fxltsbl.com",  //MMISFR_CMCC_INDEX_OPTURL
};

char s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_MAX][MMISFR_CMCC_STR_LEN + 1] =
{
    "",  //IMSI,MMISFR_CMCC_DEVICEINFO_INDEX_IMSI
    "***",  //SN,MMISFR_CMCC_DEVICEINFO_INDEX_SN
    "***",  //DEVINFO,MMISFR_CMCC_DEVICEINFO_INDEX_DEVINFO
    "",  //MAC,MMISFR_CMCC_DEVICEINFO_INDEX_MAC
    "***",  //ROM,MMISFR_CMCC_DEVICEINFO_INDEX_ROM
    "***",  //RAM,MMISFR_CMCC_DEVICEINFO_INDEX_RAM
    "***",  //CPU,MMISFR_CMCC_DEVICEINFO_INDEX_CPU
    "RTOS",  //SYSVERSION,MMISFR_CMCC_DEVICEINFO_INDEX_SYS_VERSION
    "1.0.0.1",  //SOFTWAREVER,MMISFR_CMCC_DEVICEINFO_INDEX_SOFTWARE_VER
    "TEST",  //SOFTWARENAME,MMISFR_CMCC_DEVICEINFO_INDEX_SOFTWARE_NAME
    "",     //NETTYPE,MMISFR_CMCC_DEVICEINFO_INDEX_NETTYPE
    "",     //PHONENUMBER,MMISFR_CMCC_DEVICEINFO_INDEX_PHONENUMBER
    "***",     //BATTERYCAPACITY,MMISFR_CMCC_DEVICEINFO_INDEX_BATTERYCAPACITY
    "2.4",     //SCREENSIZE,MMISFR_CMCC_DEVICEINFO_INDEX_SCREENSIZE
    "***",     //NETWORKSTATUS,MMISFR_CMCC_DEVICEINFO_INDEX_NETWORKSTATUS
    "0",     //WEARINGSTATUS,MMISFR_CMCC_DEVICEINFO_INDEX_WEARINGSTATUS
    "***",     //ROUTERMAC,MMISFR_CMCC_DEVICEINFO_INDEX_ROUTER_MAC
    "***",     //BLUETOOTHMAC,MMISFR_CMCC_DEVICEINFO_INDEX_BT_MAC
    "***",     //GPU,MMISFR_CMCC_DEVICEINFO_INDEX_GPU
    "***",     //BOARD,MMISFR_CMCC_DEVICEINFO_INDEX_BOARD
    "240*320",     //RESOLUTION,MMISFR_CMCC_DEVICEINFO_INDEX_RESOLUTION
    "***",     //PP_INFO,MMISFR_CMCC_DEVICEINFO_INDEX_APP_INFO
    "***",     //IMSI2,MMISFR_CMCC_DEVICEINFO_INDEX_IMSI2
    "***",     //BATTERYCAPACITYCURR,MMISFR_CMCC_DEVICEINFO_INDEX_BATTERYCAPACITYCURR
};
LOCAL int s_srv_port = 5683;
/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description:生成移动自注册需要的信息
//  Parameter: [In] none
//             [Return] Options
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
LOCAL Options_V4 sfrCmccGenerateData(void)
{
    Options_V4 options = {0};    
    char *p_api_type = "I";
    char *p_dm_version = "v4.0";
    char *p_temp1 = "***";
    char temp[32] = {0};
    ual_tele_radio_result_e result = UAL_TELE_RADIO_RES_MAX;
    ual_tele_sim_result_e res = UAL_TELE_SIM_RES_MAX;
    int temp_size = sizeof(temp);

    TRACE_SFR_CMCC("enter");
    //brand
    if(0 != strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_BRAND],""))
    {
        SCI_MEMCPY(options.brand, s_cmcc_register_info[MMISFR_CMCC_INDEX_BRAND], MMISFR_CMCC_STR_LEN);
        TRACE_SFR_CMCC("brand=%s", options.brand);
    }
    else
    {
        TRACE_SFR_CMCC("brand is null");
    }
    //model
    if(0 != strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_MODEL],""))
    {
        SCI_MEMCPY(options.model, s_cmcc_register_info[MMISFR_CMCC_INDEX_MODEL], MMISFR_CMCC_STR_LEN);
        TRACE_SFR_CMCC("model=%s", options.model);
    }
     else
    {
        TRACE_SFR_CMCC("model is null");
    }
    //sdkVersion
    if(0 != strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_SDKVERSION],""))
    {
        SCI_MEMCPY(options.sdkVersion, s_cmcc_register_info[MMISFR_CMCC_INDEX_SDKVERSION], MMISFR_CMCC_STR_LEN);
        TRACE_SFR_CMCC("sdkVersion=%s", options.sdkVersion);    
    }
    else
    {
        TRACE_SFR_CMCC("sdkVersion is null");
    }
    //apiVersion
    if(0 != strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_APIVERSION],""))
    {
        SCI_MEMCPY(options.apiVersion, s_cmcc_register_info[MMISFR_CMCC_INDEX_SDKVERSION], MMISFR_CMCC_STR_LEN);
        TRACE_SFR_CMCC("apiVersion=%s", options.apiVersion);    
    }
    else
    {
        TRACE_SFR_CMCC("apiVersion is null");
    }
    //apiType
    SCI_MEMSET(options.apiType, 0, MMISFR_CMCC_STR_LEN);
    SCI_MEMCPY(options.apiType, p_api_type, strlen(p_api_type));
    //templateId
    if(0 != strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_TEMPLATEID],""))
    {
        SCI_MEMCPY(options.templateId, s_cmcc_register_info[MMISFR_CMCC_INDEX_TEMPLATEID], MMISFR_CMCC_STR_LEN);
        TRACE_SFR_CMCC("templateId=%s", options.templateId);    
    }
    else
    {
        TRACE_SFR_CMCC("templateId[DMCMCC] g_dm_str[DM_STR_INDEX_TEMPLATEID] is null");
    }
    //szCMEI_IMEI
    if(0 == strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_IMEI],""))
    {
        SCI_MEMSET(temp, 0, temp_size);
        result = ual_tele_radio_get_imei(0, temp); // size > 16
        SCI_MEMCPY(options.szCMEI_IMEI, temp, 16);
        TRACE_SFR_CMCC("result=%d,IMEI=%s", result, options.szCMEI_IMEI);
    }
    else
    {
        SCI_MEMSET(options.szCMEI_IMEI, 0, MMISFR_CMCC_STR_LEN)
        SCI_MEMCPY(options.szCMEI_IMEI, s_cmcc_register_info[MMISFR_CMCC_INDEX_IMEI], MMISFR_CMCC_STR_LEN);    
        TRACE_SFR_CMCC("IMEI=%s", options.szCMEI_IMEI);
    }
    //szCMEI_IMEI2
    if(0 == strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_IMEI2],""))
    {
        SCI_MEMSET(options.szCMEI_IMEI2, 0, MMISFR_CMCC_STR_LEN);
        SCI_MEMCPY(options.szCMEI_IMEI2, p_temp1, strlen(p_temp1));
        TRACE_SFR_CMCC("IMEI2 =%s", options.szCMEI_IMEI2);
    }
    else
    {
        SCI_MEMSET(options.szCMEI_IMEI2, 0, MMISFR_CMCC_STR_LEN)
        SCI_MEMCPY(options.szCMEI_IMEI2, s_cmcc_register_info[MMISFR_CMCC_INDEX_IMEI2], MMISFR_CMCC_STR_LEN);    
        TRACE_SFR_CMCC("IMEI2 =%s", options.szCMEI_IMEI2);
    }
    //szIMSI
    if(0 == strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_ISMI],""))
    {
        SCI_MEMSET(temp, 0, temp_size);
        res = ual_tele_sim_get_imsi(0, temp); // size >16
        SCI_MEMCPY(options.szIMSI, temp, 16);
        TRACE_SFR_CMCC("res=%d,IMSI=%s", res,options.szIMSI);
    }
    else
    {
        SCI_MEMSET(options.szIMSI, 0, MMISFR_CMCC_STR_LEN);
        SCI_MEMCPY(options.szIMSI, s_cmcc_register_info[MMISFR_CMCC_INDEX_ISMI], MMISFR_CMCC_STR_LEN);    
        TRACE_SFR_CMCC("IMSI=%s", options.szIMSI);
    }
    //szDMv -- DM version
    SCI_MEMSET(options.szDMv, 0, 16);
    SCI_MEMCPY(options.szDMv, p_dm_version, strlen(p_dm_version));
    //szAppKey
    SCI_MEMSET(options.szAppKey, 0, MMISFR_CMCC_STR_LEN);
    SCI_MEMCPY(options.szAppKey, s_cmcc_register_info[MMISFR_CMCC_INDEX_APPKEY], MMISFR_CMCC_STR_LEN);    
    TRACE_SFR_CMCC("szAppKey=%s", options.szAppKey);
    //szPwd
    SCI_MEMSET(options.szPwd, 0, MMISFR_CMCC_STR_LEN);
    SCI_MEMCPY(options.szPwd, s_cmcc_register_info[MMISFR_CMCC_INDEX_PASSWORD], MMISFR_CMCC_STR_LEN);    
    TRACE_SFR_CMCC("szPwd=%s", options.szPwd);
    //nAddressFamily
    options.nAddressFamily = 4;
    //szSrvIP
    if(0 != strcmp(s_cmcc_register_info[MMISFR_CMCC_INDEX_OPTURL],""))
    {
        SCI_MEMCPY(options.szSrvIP, s_cmcc_register_info[MMISFR_CMCC_INDEX_OPTURL], strlen(s_cmcc_register_info[MMISFR_CMCC_INDEX_OPTURL]));
        TRACE_SFR_CMCC("szSrvIP=%s", options.szSrvIP);
        {
            char  * str  = NULL;
            str = strrchr(options.szSrvIP,':');
            if(str != NULL)
            {                
                TRACE_SFR_CMCC("str is :%s\n", str);
                options.nSrvPort = atoi(str+1);
                TRACE_SFR_CMCC("opt_v4.nSrvPort is :%d\n", options.nSrvPort);
            }
            else
            {
                options.nSrvPort = s_srv_port;
            }
        }
    }    
    //nLifetime
    options.nLifetime = 300; //update interval 300s(test),86400s(rel)
    //nLocalPort
    options.nLocalPort = 4683;
    //nBootstrap
    options.nBootstrap = 0;
    TRACE_SFR_CMCC("exit");
    return options;
}

/*****************************************************************************/
//  Description:注册时lwm2m的回复通知
//  Parameter: [In] optNotifyParam
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
LOCAL void  sfrCmccLwm2mNotifyMsg(OptNotifyParam *optNotifyParam)
{
    struct addressConfig *p_address_temp = PNULL;
    MMISFR_CMCC_SIGNAL_T *p_signal = NULL;
    MMISFR_CMCC_MSG_T msg = {0};

    if (PNULL == optNotifyParam)
    {
        TRACE_SFR_CMCC("optNotifyParam is pnull");
        return;
    }

    TRACE_SFR_CMCC("type=%0x,code=%d,msg=%s\n",optNotifyParam->notify_type, optNotifyParam->notify_code, optNotifyParam->notify_msg);
    switch (optNotifyParam->notify_type)
    {
        case NOTIFY_TYPE_SYS:
        {
            TRACE_SFR_CMCC("receive NOTIFY_TYPE_SYS");
            if(TRUE == s_run_result && NOTIFY_CODE_OK == optNotifyParam->notify_code)
            {
                s_reportTime = (LWM2M_SDK_UPDATE_RuleConfig(reportTime)!= 0)?(LWM2M_SDK_UPDATE_RuleConfig(reportTime)*60*1000):s_reportTime;
                s_reportNum = (LWM2M_SDK_UPDATE_RuleConfig(reportNum)!= 0)?LWM2M_SDK_UPDATE_RuleConfig(reportNum):s_reportNum;
                s_heartBeatTime = (LWM2M_SDK_UPDATE_RuleConfig(heartBeatTime)!= 0)?(LWM2M_SDK_UPDATE_RuleConfig(heartBeatTime)*60*1000):s_heartBeatTime;
                s_retryInterval = (LWM2M_SDK_UPDATE_RuleConfig(retryInterval)!= 0)?(LWM2M_SDK_UPDATE_RuleConfig(retryInterval)*60*1000):s_retryInterval;
                s_retryNum = (LWM2M_SDK_UPDATE_RuleConfig(retryNum)!= 0)?LWM2M_SDK_UPDATE_RuleConfig(retryNum):s_retryNum;  
                //s_heartBeatTime = s_heartBeatTime - 10*1000;
                //TO GET NEW ADDRESS FROM SERVER  and restart lwm2m for new server
                p_address_temp = LWM2M_SDK_UPDATE_AddressConfig();
                if(PNULL != p_address_temp && PNULL != p_address_temp->value && 0 != p_address_temp->port)
                {                    
                    TRACE_SFR_CMCC("address_temp->value: %s ,len :%d  ,port : %d\n",p_address_temp->value,strlen(p_address_temp->value),p_address_temp->port);
                    memset(s_cmcc_register_info[MMISFR_CMCC_INDEX_OPTURL], 0, strlen(s_cmcc_register_info[MMISFR_CMCC_INDEX_OPTURL]));
                    memcpy(s_cmcc_register_info[MMISFR_CMCC_INDEX_OPTURL],p_address_temp->value,strlen(p_address_temp->value));
                    s_srv_port = p_address_temp->port;

                    TRACE_SFR_CMCC("opt_v4.szSrvIP: %s , opt_v4.nSrvPort : %d\n",s_cmcc_register_info[MMISFR_CMCC_INDEX_OPTURL], s_srv_port);
                    //send msg to app or thinmodem
                    msg.msg_id = MSG_MMISFR_CMCC_IP_CHANGED;
                    msg.time_out = CMCC_REG_RESTART_TO_CONNEXT_TIME;
                    SCI_CREATE_SIGNAL(p_signal, APP_SFR_LWM2M_NOTIFY_MSG, sizeof(MMISFR_CMCC_SIGNAL_T), SCI_IdentifyThread());
                    p_signal->p_content = &msg; 
#ifndef THIN_MODEM_SUPPORT
                    SCI_SEND_SIGNAL(p_signal, P_APP);
#else
                    SCI_SEND_SIGNAL(p_signal, P_THIN_MODEM);
#endif
                    return ;
                }
                //send msg to app or thinmodem
                msg.msg_id = MSG_MMISFR_CMCC_REGISTER_SUCCESS;
                SCI_CREATE_SIGNAL(p_signal, APP_SFR_LWM2M_NOTIFY_MSG, sizeof(MMISFR_CMCC_SIGNAL_T), SCI_IdentifyThread());
                p_signal->p_content = &msg; 
#ifndef THIN_MODEM_SUPPORT
                SCI_SEND_SIGNAL(p_signal, P_APP);
#else
                SCI_SEND_SIGNAL(p_signal, P_THIN_MODEM);
#endif
            }
            break;
        }

        case NOTIFY_TYPE_REGISTER:
        {
            TRACE_SFR_CMCC("receive NOTIFY_TYPE_REGISTER");
            if (NOTIFY_CODE_OK == optNotifyParam->notify_code)
            {
                s_run_result = TRUE;
                s_retry_fail_times = 0;
            }
            else
            {
                s_run_result = FALSE;
                TRACE_SFR_CMCC("s_retry_fail_times=%d, s_retryNum=%d", s_retry_fail_times, s_retryNum);
                if (s_retry_fail_times++ <= s_retryNum)              
                {
                    //send msg to app or thinmodem
                    msg.msg_id = MSG_MMISFR_CMCC_REGISTER_FAIL;
                    SCI_CREATE_SIGNAL(p_signal, APP_SFR_LWM2M_NOTIFY_MSG, sizeof(MMISFR_CMCC_SIGNAL_T), SCI_IdentifyThread());
                    p_signal->p_content = &msg; 
#ifndef THIN_MODEM_SUPPORT
                    SCI_SEND_SIGNAL(p_signal, P_APP);
                    SCI_TRACE_LOW("lmlmlm code :0x%x",APP_SFR_LWM2M_NOTIFY_MSG);
#else
                    SCI_SEND_SIGNAL(p_signal, P_THIN_MODEM);
#endif
                }
            }
            break;
        }

        case NOTIFY_TYPE_DEREGISTER://HeartBeat result
        {
            s_run_result = FALSE;
            break;
        }

        case NOTIFY_TYPE_REG_UPDATE:
        {
            TRACE_SFR_CMCC("receive NOTIFY_TYPE_REG_UPDATE");
            if (NOTIFY_CODE_OK == optNotifyParam->notify_code)
            {
                s_run_result = TRUE;
                s_retry_fail_times = 0;
            }
            else
            {
                s_run_result = FALSE;
                TRACE_SFR_CMCC("s_retry_fail_times=%d, s_retryNum=%d", s_retry_fail_times, s_retryNum);
                if (s_retry_fail_times++ < s_retryNum)
                {
                    //send msg to app or thinmodem
                    msg.msg_id = MSG_MMISFR_CMCC_UPDATE_FAIL;
                    SCI_CREATE_SIGNAL(p_signal, APP_SFR_LWM2M_NOTIFY_MSG, sizeof(MMISFR_CMCC_SIGNAL_T), SCI_IdentifyThread());
                    p_signal->p_content = &msg; 
#ifndef THIN_MODEM_SUPPORT
                    SCI_SEND_SIGNAL(p_signal, P_APP);
#else
                    SCI_SEND_SIGNAL(p_signal, P_THIN_MODEM);
#endif
                }
            }
            break;
        }
        default:
            break;
    }
}

LOCAL uint8 *sfrCmccGetNetworkType(void)
{
    ual_tele_radio_current_network_type_e network_type = UAL_TELE_RADIO_CURRENT_NETWORK_TYPE_MAX;
    static uint8 nettype[3] = {0};
    network_type = ual_tele_radio_get_current_network_type(SIM_ID_1);
    TRACE_SFR_CMCC("network_type = %d", network_type);
    switch (network_type)
    {
        case UAL_TELE_RADIO_CURRENT_NETWORK_TYPE_4G:
            SCI_MEMCPY(nettype, "4G", 3);
            break;
        case UAL_TELE_RADIO_CURRENT_NETWORK_TYPE_3G:
            SCI_MEMCPY(nettype, "3G", 3);
            break;
        case UAL_TELE_RADIO_CURRENT_NETWORK_TYPE_2G:
            SCI_MEMCPY(nettype, "2G", 3);
            break;
        default:
            SCI_MEMCPY(nettype, "2G", 3);
            break;
    }
    return nettype;

}


#if 1//add_get_mac start
typedef struct _GET_BT_ADDRESS {
	uint8 addr[6];
} GET_BT_ADDRESS;

typedef struct _BT_NV_PARAM {
    GET_BT_ADDRESS    bd_addr;
    uint16        xtal_dac;
} BT_NV_PARAM;

#define DEV_MAC_LEN       17
static int ls_rand(void)
{
    static int ali_rand_offset = 12345;
    int radom = 0;
    srand(SCI_GetTickCount()+ali_rand_offset);
    ali_rand_offset++;
    radom = rand();
    return radom;
}


//默认FF:FF:FF:FF:FF:FF  随机: 8C:9B:76:DA:45:40
 int get_dev_mac(uint8_t *mac, uint32_t *len)
{
    int i = 0;
    NVITEM_ERROR_E  status  = NVERR_NONE;
    BT_NV_PARAM    nv_param;
    uint8 mac_addr[6] = {0};
    char mac_buf[100] = {0};
    uint16 mac_len = 0;
    if (mac == NULL ||  len == NULL) {
		TRACE_SFR_CMCC("get_dev_mac NULL mac=%s len=%d",mac,len);
        return -1;
    }

    if (*len < DEV_MAC_LEN) {
        if (*len != 0) {
        }
        *len = DEV_MAC_LEN;
		TRACE_SFR_CMCC("get_dev_mac error mac=%s len=%d",mac,len);
        return -1;
    }
    
    status = EFS_NvitemRead(401, sizeof(BT_NV_PARAM), (uint8*)&nv_param);
    if(NVERR_NONE != status)
    {
        uint32_t cur_radom = ls_rand();
        mac_addr[0] = 0x8C;
        mac_addr[1] = 0x9B;
        mac_addr[2] = 0x76;
        mac_addr[3] = 0xDA;
        mac_addr[4] = (cur_radom>>8) & 0x000000FF;
        mac_addr[5] = cur_radom & 0x000000FF;
        SCI_MEMCPY(nv_param.bd_addr.addr,mac_addr,6);
        status = NVITEM_UpdateCali(401, sizeof(nv_param), (uint8*)&nv_param);
    }
    else
    {
        for(i = 0 ; i < 6; i++)
        {
            if(nv_param.bd_addr.addr[i] != 0xFF)
            {
				TRACE_SFR_CMCC("get_dev_mac 0xFF mac=%s len=%d",mac,len);
                break;
            }
        }
        if(i == 6)
        {
            uint32_t cur_radom = ls_rand();
            mac_addr[0] = 0x8C;
            mac_addr[1] = 0x9B;
            mac_addr[2] = 0x76;
            mac_addr[3] = 0xDA;
            mac_addr[4] = (cur_radom>>8) & 0x000000FF;
            mac_addr[5] = cur_radom & 0x000000FF;
            SCI_MEMCPY(nv_param.bd_addr.addr,mac_addr,6);
            status = NVITEM_UpdateCali(401, sizeof(nv_param), (uint8*)&nv_param);
        }
        else
        {
            SCI_MEMCPY(mac_addr,(uint8*)&(nv_param.bd_addr.addr),  6);
        }
    }
    
    sprintf(mac_buf,"%02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    *len = strlen(mac_buf);
    SCI_MEMCPY(mac,mac_buf,(*len));
	TRACE_SFR_CMCC("mac_buf=%s",mac_buf);
    return 0;
}
#endif//add_get_mac end

LOCAL int sfrCmccDmReadInfo(int resId, char **outbuff)
{
    int iret = 0;
    char imsi_str[UAL_TELE_SIM_IMSI_MAX_LEN+1] = {0};
    char msisdn_str[UAL_TELE_SIM_MSISDN_MAX_LEN + 1] = {0};
    ual_tele_sim_result_e res = UAL_TELE_SIM_RES_MAX;
    BOOLEAN  is_volte_state;
    char buff[512] = {0};
    uint8 temp_buf[101] = {0};
    uint32 temp_len = 0;
    uint8 mac_addr[6] = {0};
    int  buflen = sizeof(buff);
    memset(buff, 0, sizeof(buff));
    SCI_MEMSET(temp_buf,0,sizeof(temp_buf));

    snprintf(buff, buflen, "%s", "unknown"); //default
    *outbuff = NULL;
    TRACE_SFR_CMCC(":resId=%d",resId);
    switch (resId)
    {
        case 6601://devinfo
			snprintf(buff, buflen, "%s", "");
            break;
        case 6603://mac
		#if 1//def ZDT_ZFB_SUPPORT	
		{
			temp_len = 100;
			get_dev_mac(temp_buf,&temp_len);
			snprintf(buff, buflen, temp_buf);
		}
		#else
			/*if(WIFISUPP_GetMac(mac_addr))
			{
				snprintf(buff, buflen, mac_addr);
			}else{
				snprintf(buff, buflen, "");
			}*/
		snprintf(buff, buflen, "");
		#endif
            break;
        case 6604://rom
			snprintf(buff, buflen, "16M");
            break;
        case 6605://ram
			snprintf(buff, buflen, "16M");
            break;
        case 6606://CPU
			snprintf(buff, buflen, "ARM A53");
            break;
        case 6607://SYS VERSION
			snprintf(buff, buflen, "MOCOR20B");
            break;
        case 6608://FIRMWARE VERSION
			snprintf(buff, buflen, ZDT_CMCC_SFR_SW_VER);
            break;
        case 6609://FIRMWARE NAME
			snprintf(buff, buflen, ZDT_CMCC_SFR_TYPE);
            break;
        case 6610://Volte
        {
            is_volte_state = ual_tele_radio_get_volte_state(0);
            if (is_volte_state == TRUE)
            {
                snprintf(buff, buflen, "1");
            }
            else
            {
                snprintf(buff, buflen, "0");
            }
            break;
        }
        case 6611://NetType
        {
            if(0 != strcmp(s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_NETTYPE],""))
            {
                snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_NETTYPE]);
            }
            else
            {
                snprintf(buff, buflen, sfrCmccGetNetworkType());
            }
            break;
        }
        case 6612://BATTERYCAPACITY 
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_BATTERYCAPACITY]);
            break;
        case 6613://PhoneNumber
        {
            if(strcmp(s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_PHONENUMBER],"") == 0)
            {
                res = ual_tele_sim_get_msisdn(SIM_ID_1, msisdn_str);
                snprintf(buff, buflen, msisdn_str);
                TRACE_SFR_CMCC("res=%d,get_phone_number buff =%s", res,buff);
            }
            else
            {
                snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_PHONENUMBER]);
                TRACE_SFR_CMCC("[DMCMCC] mcu set number buff =%s", buff);
            }
            break;
        }
        case 6614://SCREENSIZE  
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_SCREENSIZE]);
            break;
            //Below is added for CMCC DM 4.0
        case 6615://LTE_IMSI
        {
            if(0 == strcmp(s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_IMSI],""))
            {
                res = ual_tele_sim_get_imsi(0, imsi_str);
                snprintf(buff, buflen,imsi_str);
                TRACE_SFR_CMCC("res=%d,IMSI=%s", res,imsi_str);
            }
            else
            {
                snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_IMSI]);
                TRACE_SFR_CMCC("IMSI=%s", s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_IMSI]);
            }
            break;
        }
        case 6616://SN
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_SN]);
            break;
        case 6617://ROUTER_MAC
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_ROUTER_MAC]);
            break;
        case 6618://BT_MAC
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_BT_MAC]);
            break;
        case 6619://GPU
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_GPU]);
            break;
        case 6620://BOARD
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_BOARD]);
            break;
        case 6621://RESOLUTION
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_RESOLUTION]);
            break;
        case 6622://NETWORKSTATUS 
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_NETWORKSTATUS ]);
            break;
        case 6623://WEARINGSTATUS
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_WEARINGSTATUS]);
            break;
        case 6624://APP_INFO
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_APP_INFO]);
            break;
        case 6625://IMSI2
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_IMSI2]);
            break;
        case 6626://BATTERYCAPACITYCURR 
            snprintf(buff, buflen, s_cmcc_device_info[MMISFR_CMCC_DEVICEINFO_INDEX_BATTERYCAPACITYCURR]);
            break;
        default:
            iret = -1;
            break;
    }

    if (iret == 0)
    {
        //apply for buffer, sdk will free buffer
        *outbuff = (char *)malloc(strlen(buff) + 1);
        strcpy(*outbuff, buff);
    }

    TRACE_SFR_CMCC("value=%s,iret=%d\n", buff,iret);
    return iret;
}


/*****************************************************************************/
//  Description:lwm2m库的init callback
//  Parameter: [In] none
//             [Return] OptFuncs
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
LOCAL OptFuncs sfrCmccInitCallback(void)
{
    OptFuncs optfuncs = {NULL, NULL};
    optfuncs.NotifyMsg  = sfrCmccLwm2mNotifyMsg;
    optfuncs.DMReadInfo = sfrCmccDmReadInfo;

    return optfuncs;
}

/*****************************************************************************/
//  Description:向lwm2m库post 数据
//  Parameter: [In] none
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
LOCAL void sfrCmccPostLwm2mData(void)
{
    int ret_stop = LWM2M_SDK_STOP();
    int ret_run = LWM2M_SDK_RUN(1);
    TRACE_SFR_CMCC("exit");
}

/*****************************************************************************/
//  Description:重新设置相关flag
//  Parameter: [In] none
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
LOCAL void sfrCmccResetFlags()
{
    s_in_register = TRUE;
    s_run_result = FALSE;
    s_retry_fail_times = 0;
}

/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description:nexe loop
//  Parameter: [In] is_successful
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_NextLoop(BOOLEAN is_update, BOOLEAN is_successful)
{
    TRACE_SFR_CMCC("is_update = %d, is_successful = %d", is_update, is_successful);
    if (TRUE == is_update)
    {
        TRACE_SFR_CMCC("s_total_reportnum = %d, s_reportnum = %d", s_total_reportnum, s_reportNum);
        if(s_total_reportnum <= s_reportNum)
        {
            MMISFR_CMCC_RetryUpdate(is_successful);
        }
    }
    else
    {
        MMISFR_CMCC_RetryRegister();
    }
}

/*****************************************************************************/
//  Description:清除lwm2m数据
//  Parameter: [In] is_need_stop
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_Lwm2mStop(BOOLEAN is_need_stop)
{
    TRACE_SFR_CMCC("is_need_stop = %d", is_need_stop);

    if (is_need_stop)
    {
        LWM2M_SDK_STOP();
    }
    LWM2M_SDK_FINI();
}

/*****************************************************************************/
//  Description:init or update lwm2m
//  Parameter: [In] none
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_LwM2MEnter(void)
{
    TRACE_SFR_CMCC("enter");
    s_total_reportnum++;
    TRACE_SFR_CMCC("s_total_reportnum=%d, s_first_report_in_reportTime=%d",s_total_reportnum,s_first_report_in_reportTime);
    if(TRUE == s_first_report_in_reportTime)
    {
        s_report_start = lwm2m_gettime();
        TRACE_SFR_CMCC("s_report_start=%d",s_report_start);
        s_first_report_in_reportTime = FALSE;
    }
    sfrCmccResetFlags();
    if (LWM2M_SDK_IS_RUN())
    {
        s_in_register = FALSE;
        LWM2M_SDK_UPDATE_REG();
        TRACE_SFR_CMCC("update");
    }
    else
    {
        Options_V4 opt = sfrCmccGenerateData();
        OptFuncs optfuncs = sfrCmccInitCallback();
        int ret = LWM2M_SDK_INIT_V4(&opt, &optfuncs);
        sfrCmccPostLwm2mData();
    }
}

/*****************************************************************************/
//  Description :set appkey ,password等信息
//  Param :str_index,str
//  Author:
//  Note:miao.liu2
/*****************************************************************************/
PUBLIC BOOLEAN MMISFR_CMCC_SetRegisterInfo(MMISFR_CMCC_INDEX_E str_index, char str[MMISFR_CMCC_STR_LEN])
{
    uint8 in_str_len = 0;
    if(str_index >= MMISFR_CMCC_INDEX_MAX )
    {
        TRACE_SFR_CMCC("not support : %d  max is %d", str_index, MMISFR_CMCC_INDEX_MAX - 1);
        return FALSE;
    }
    in_str_len = strlen(str);
    TRACE_SFR_CMCC("index: %d, str: %s , str len %d", str_index, str, in_str_len);

    SCI_MEMSET(s_cmcc_register_info[str_index], 0, MMISFR_CMCC_STR_LEN + 1);
    SCI_MEMCPY(s_cmcc_register_info[str_index], str, in_str_len);
    return TRUE;
}

/*****************************************************************************/
//  Description :set device info
//  Param :str_index,str
//  Author:
//  Note:miao.liu2
/*****************************************************************************/
PUBLIC BOOLEAN MMISFR_CMCC_SetDeviceInfo(MMISFR_CMCC_DEVICEINFO_INDEX_E deviceinfo_index, char str[MMISFR_CMCC_STR_LEN])
{
    uint8 in_str_len = 0;
    if(deviceinfo_index >= MMISFR_CMCC_DEVICEINFO_INDEX_MAX )
    {
        TRACE_SFR_CMCC("not support : %d  max is %d", deviceinfo_index, MMISFR_CMCC_INDEX_MAX - 1);
        return FALSE;
    }
    in_str_len = strlen(str);
    TRACE_SFR_CMCC("index: %d, str: %s , str len %d", deviceinfo_index, str, in_str_len);

    SCI_MEMSET(s_cmcc_device_info[deviceinfo_index], 0, MMISFR_CMCC_STR_LEN + 1);
    SCI_MEMCPY(s_cmcc_device_info[deviceinfo_index], str, in_str_len);
    return TRUE;
}

/*****************************************************************************/
//  Description:收到主动上报网络消息不好的处理
//  Parameter: [In] none
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_ReceivePdpDeactiveInd(void)
{
    TRACE_SFR_CMCC("enter");
    MMISFR_CMCC_Lwm2mStop(TRUE);
    MMISFR_CMCC_NextLoop(FALSE, FALSE);
}

/*****************************************************************************/
//  Description:获取rule config的配置
//  Parameter: [In] rule_config
//             [Return] rule_config对应的具体值
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_E rule_config)
{
    uint32 value = 0;
    TRACE_SFR_CMCC("rule_config=%d",rule_config);
    switch (rule_config)
    {
        case MMISFR_CMCC_RULE_CONFIG_REPORTTIME:
        {
            value = s_reportTime;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_REPORTNUM:
        {
            value = s_reportNum;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_RETRYINTERVAL:
        {
            value = s_retryInterval;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_RETRYNUM:
        {
            value = s_retryNum;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_HEARTBEATTIME:
        {
            value = s_heartBeatTime;
            break;
        }
        default:
            break;
    }
    TRACE_SFR_CMCC("value=%d",value);
    return value;
}

/*****************************************************************************/
//  Description:设置rule config的值
//  Parameter: [In] rule_config
//             [In] rule_config对应的具体值
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetRuleConfig(MMISFR_CMCC_RULE_CONFIG_E rule_config, uint32 value)
{
    TRACE_SFR_CMCC("rule_config=%d, value=%d",rule_config,value);
    switch (rule_config)
    {
        case MMISFR_CMCC_RULE_CONFIG_REPORTTIME:
        {
            s_reportTime = value;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_REPORTNUM:
        {
            s_reportNum = value;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_RETRYINTERVAL:
        {
            s_retryInterval = value;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_RETRYNUM:
        {
            s_retryNum = value;
            break;
        }
        case MMISFR_CMCC_RULE_CONFIG_HEARTBEATTIME:
        {
            s_heartBeatTime = value;
            break;
        }
        default:
            break;
    }
    return;
}

/*****************************************************************************/
//  Description:获取上报总数
//  Parameter: [In] none
//             [Return] 上报总数
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMISFR_CMCC_GetTotalRoportNum(void)
{
    TRACE_SFR_CMCC("total_report_num=%d",s_total_reportnum);
    return s_total_reportnum;
}

/*****************************************************************************/
//  Description:设置上报总数
//  Parameter: [In] total_report_num
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetTotalRoportNum(uint32 total_report_num)
{
    TRACE_SFR_CMCC("total_report_num=%d",total_report_num);
    s_total_reportnum = total_report_num;
    return;
}

/*****************************************************************************/
//  Description:获取报错次数
//  Parameter: [In] none
//             [Return] 报错次数
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC uint32 MMISFR_CMCC_GetFailNum(void)
{
    TRACE_SFR_CMCC("fail_num=%d",s_retry_fail_times);
    return s_retry_fail_times;
}

/*****************************************************************************/
//  Description:设置报错次数
//  Parameter: [In] rule_config
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetFailNum(uint32 fail_num)
{
    TRACE_SFR_CMCC("total_report_num=%d",fail_num);
    s_retry_fail_times = fail_num;
    return;
}

/*****************************************************************************/
//  Description:获取自注册开始的时间
//  Parameter: [In] none
//             [Return] 自注册开始的时间
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC time_t MMISFR_CMCC_GetLwm2mStartTime(void)
{
    TRACE_SFR_CMCC("fail_num=%d",s_report_start);
    return s_report_start;
}

/*****************************************************************************/
//  Description:设置自注册开始的时间
//  Parameter: [In]  自注册开始的时间
//             [Return] none
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetLwm2mStartTime(time_t start_time)
{
    TRACE_SFR_CMCC("start_time=%d",start_time);
    s_report_start = start_time;
    return;
}


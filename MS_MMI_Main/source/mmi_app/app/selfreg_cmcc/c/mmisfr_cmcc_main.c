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
** File Name:      mmisfr_cmcc_main.c                                        *
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
#include "os_api.h"
#include "time.h"
#include "sci_types.h"
#include "in_message.h"
#include "socket.h"
#include "mmisfr_cmcc_lwm2m.h"
#include "ual_tele_sim.h"
#include "ual_tele_data.h"
#include "ual_tele_radio.h"
#include "ual_timer.h"
#include "liblwm2m.h"
/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                 *
**--------------------------------------------------------------------------*/
#define CMCC_CLEAN_FAIL_NUM_TIME (1000)
/**--------------------------------------------------------------------------*
**                         GLOBAL DEFINITION                                *
**--------------------------------------------------------------------------*/
// allow to use pdp or not
LOCAL BOOLEAN s_allow_use_pdp = TRUE;
LOCAL uint32 s_srfcmcc_reg_data_handle = 0;
LOCAL uint32 s_srfcmcc_reg_sim_handle = 0;
LOCAL uint32 s_clean_fail_num_timer_id = 0;
/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description :register data的函数
//  Param :none
//  Author:miao.liu2
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN SFR_CMCC_RegisterData(void);

/*****************************************************************************/
//Description : timer to clean report num and fail num
//Parameter: [In] param
//           [Out] None
//           [Return] None
//Author: miao.liu2
//Note:
/*****************************************************************************/
LOCAL void SFR_CMCC_CleanFailNum(uint32 param)
{
    BOOLEAN result = FALSE;

    TRACE_SFR_CMCC("enter");
    if(0 != s_clean_fail_num_timer_id)
    {
        result = ual_timer_stop(s_clean_fail_num_timer_id);
        TRACE_SFR_CMCC("result=%d",result);
        s_clean_fail_num_timer_id = 0;
    }
    //clean total_report_num and fail_num
    MMISFR_CMCC_SetTotalRoportNum(0);
    MMISFR_CMCC_SetFailNum(0);
    //stop lwm2m
    MMISFR_CMCC_Lwm2mStop(TRUE);
}
/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : 收到network status消息时进行移动自注册入口
//  Param :None
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_HandleNetworkStatus(void)
{
    ual_tele_sim_id_e sim_id = SIM_ID_1;
    ual_tele_sim_status_e sim_status = UAL_TELE_SIM_STATUS_MAX;
    uint32 total_report_num = 0;
    uint32 reportNum = 0;
    uint32 fail_num = 0;
    uint32 retryNum = 0;
    uint32 reportTime = 0;
    time_t current_time = 0;
    time_t start_time = 0;
    TRACE_SFR_CMCC("s_allow_use_pdp=%d", s_allow_use_pdp);

    sim_status = ual_tele_sim_get_sim_status(sim_id);
    TRACE_SFR_CMCC("sim_status=%d", sim_status);
    if (s_allow_use_pdp && (UAL_TELE_SIM_STATUS_READY == sim_status)
            && (TRUE == ual_tele_radio_get_gprs_state(sim_id)))
    {
        total_report_num = MMISFR_CMCC_GetTotalRoportNum();
        reportNum = MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_REPORTNUM);
        TRACE_SFR_CMCC("total_report_num=%d, reportNum=%d", total_report_num, reportNum);
        if(total_report_num >= reportNum)
        {
            current_time = lwm2m_gettime();
            start_time = MMISFR_CMCC_GetLwm2mStartTime();
            reportTime = MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_REPORTTIME);
            TRACE_SFR_CMCC("current_time=%d, start_time=%d, reportTime=%d", current_time, start_time, reportTime);
            if((current_time - start_time) < (reportTime/1000))
            {
                s_clean_fail_num_timer_id = ual_timer_start(CMCC_CLEAN_FAIL_NUM_TIME, SFR_CMCC_CleanFailNum, 0, FALSE);
                if(PNULL == s_clean_fail_num_timer_id)
                {
                    TRACE_SFR_CMCC("create timer fail");
                    return;
                }
                return;
            }
            else
            {
                //clean total_report_num and fail_num
                MMISFR_CMCC_SetTotalRoportNum(0);
                MMISFR_CMCC_SetFailNum(0);
            }
        }
        //get fail num
        fail_num = MMISFR_CMCC_GetFailNum();
        retryNum = MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_RETRYNUM);
        TRACE_SFR_CMCC("fail_num=%d, retryNum=%d", fail_num, retryNum);
        if(fail_num < retryNum)
        {
            SFR_CMCC_RegisterData();
        }
        else
        {
            //stop lwm2m
            MMISFR_CMCC_Lwm2mStop(TRUE);
        }
    }
}

/*****************************************************************************/
//  Description :Active Pdp的回调函数
//  Param :MMIPDP_CNF_INFO_T
//  Author:miao.liu2
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN SfrCmcc_HandleTeleDataMsg(ual_cms_msg_t param)
{
    uint32 net_id = 0;
    if( PNULL == param.p_body )
    {
        TRACE_SFR_CMCC("param is PNULL!!");
        return FALSE;  // param_ptr invalid, all not proccess it    
    }
    TRACE_SFR_CMCC(" msg_id = %d", param.msg_id);
    switch(param.msg_id)
    {
        case MSG_UAL_TELE_DATA_ACTIVE_CNF:
        {
            net_id = ual_tele_data_get_netid();
            TRACE_SFR_CMCC(" net_id = %d", net_id);
            if(0 != net_id)
            {
                socket_SetNetId(net_id);
                MMISFR_CMCC_LwM2MEnter();
            }
            break;
        }
        case MSG_UAL_TELE_DATA_DEACTIVE_CNF:
        {
            break;        
        }
        case MSG_UAL_TELE_DATA_DEACTIVE_IND:
        {
            ual_tele_data_unregister(s_srfcmcc_reg_data_handle);
            MMISFR_CMCC_ReceivePdpDeactiveInd();
            break;
        }
        default:            
            break;
    }
}

/*****************************************************************************/
//  Description :register data的函数
//  Param :none
//  Author:miao.liu2
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN SFR_CMCC_RegisterData(void)
{
    ual_tele_data_result_e register_data_ret = UAL_TELE_DATA_RES_MAX;
    uint32 net_id = 0;

    TRACE_SFR_CMCC("s_allow_use_pdp=%d", s_allow_use_pdp);
    if (FALSE == s_allow_use_pdp)
    {
        return FALSE;
    }
    s_allow_use_pdp = FALSE;

    net_id = ual_tele_data_get_netid();
    TRACE_SFR_CMCC(" net_id = %d", net_id);
    register_data_ret = ual_tele_data_register(SfrCmcc_HandleTeleDataMsg, &s_srfcmcc_reg_data_handle);
    if(UAL_TELE_DATA_RES_SUCCESS != register_data_ret)
    {
        TRACE_SFR_CMCC(" register tele data failed");
        return FALSE;
    }
    if(0 != net_id)
    {
        socket_SetNetId(net_id);
        MMISFR_CMCC_LwM2MEnter();
    }
    else
    {
        TRACE_SFR_CMCC(" net_id = 0");
        return FALSE;
    }
    return TRUE;
}


/*****************************************************************************/
//  Description :Set Allow Active Pdp
//  Param :is_allow_use_pdp
//  Author:
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_SetAllowActivePdp(BOOLEAN is_allow_use_pdp)
{
    s_allow_use_pdp = is_allow_use_pdp;
    TRACE_SFR_CMCC("is_allow_use_pdp=%d", is_allow_use_pdp);
}

/*****************************************************************************/
//  Description :register sim的回调函数
//  Param :ual_common_msg_t
//  Author:miao.liu2
//  Note:
/*****************************************************************************/
LOCAL BOOLEAN SfrCmcc_HandleTeleSimMsg(ual_cms_msg_t param)
{
    if( PNULL == param.p_body )
    {
        TRACE_SFR_CMCC("param is PNULL!!");
        return FALSE;  // param_ptr invalid, all not proccess it    
    }
    TRACE_SFR_CMCC(" msg_id = %d", param.msg_id);
    switch(param.msg_id)
    {
        case MSG_UAL_TELE_SIM_PLUG_IN_IND:
        {
            MMISFR_CMCC_SetAllowActivePdp(TRUE);
            break;
        }
        case MSG_UAL_TELE_SIM_PLUG_OUT_IND:
        {
            break;
        }
        default:
            break;
    }
}

LOCAL void sfr_cmcc_handle_restart_timer(uint8  timer_id, uint32 param)
{
    BOOLEAN result = FALSE;

    TRACE_SFR_CMCC("enter");
    if(0 != timer_id)
    {
        result = ual_timer_stop(timer_id);
        TRACE_SFR_CMCC("result=%d",result);
    }
    MMISFR_CMCC_SetAllowActivePdp(TRUE);

    MMISFR_CMCC_HandleNetworkStatus();
}

/*****************************************************************************/
//  Description : 模块初始化函数
//  Param :None
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC void MMISFR_CMCC_InitModule(void)
{
    ual_tele_sim_result_e register_result = UAL_TELE_SIM_RES_MAX;
    BOOLEAN regiter_res = FALSE;
    TRACE_SFR_CMCC("enter");
    
    register_result = ual_tele_sim_register(SfrCmcc_HandleTeleSimMsg, &s_srfcmcc_reg_sim_handle);
    if(UAL_TELE_SIM_RES_SUCCESS != register_result)
    {
        TRACE_SFR_CMCC(" register tele sim failed");
        return;
    }
}

LOCAL void sfr_cmcc_handle_regupdate_timer(uint8  timer_id, uint32 param)
{
    BOOLEAN result = FALSE;

    TRACE_SFR_CMCC("enter");
    if(0 != timer_id)
    {
        result = ual_timer_stop(timer_id);
        TRACE_SFR_CMCC("result=%d",result);
    }
    s_allow_use_pdp = TRUE;

    MMISFR_CMCC_HandleNetworkStatus();
}

PUBLIC void MMISFR_CMCC_RetryRegister(void)
{
    uint32 timer = 0;
    ual_tele_data_result_e unregister_data_ret = UAL_TELE_DATA_RES_MAX;
    uint32 retryInterval_time = 0;

    TRACE_SFR_CMCC("enter");
    unregister_data_ret = ual_tele_data_unregister(s_srfcmcc_reg_data_handle);
    retryInterval_time = MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_RETRYINTERVAL);
    
    timer = ual_timer_start(retryInterval_time, sfr_cmcc_handle_regupdate_timer, NULL, FALSE);
    TRACE_SFR_CMCC("unregister_data_ret=%d,retryInterval_time=%d,timer=%d", unregister_data_ret,retryInterval_time,timer);
}

PUBLIC void MMISFR_CMCC_RetryUpdate(BOOLEAN is_success)
{
    uint32 timer = 0;
    ual_tele_data_result_e unregister_data_ret = UAL_TELE_DATA_RES_MAX;
    uint32 retryInterval_time = 0;
    uint32 heartBeatTime = 0;

    TRACE_SFR_CMCC("is_success=%d", is_success);
    unregister_data_ret = ual_tele_data_unregister(s_srfcmcc_reg_data_handle);
    retryInterval_time = MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_RETRYINTERVAL);
    heartBeatTime = MMISFR_CMCC_GetRuleConfig(MMISFR_CMCC_RULE_CONFIG_HEARTBEATTIME);
    if (TRUE == is_success)
    {
        timer = ual_timer_start(heartBeatTime, sfr_cmcc_handle_regupdate_timer, NULL, FALSE);
    }
    else
    {
        timer = ual_timer_start(retryInterval_time, sfr_cmcc_handle_regupdate_timer, NULL, FALSE);
    }
    TRACE_SFR_CMCC("unregister_data_ret=%d,retryInterval_time=%d,heartBeatTime=%d,timer=%d", unregister_data_ret,retryInterval_time,heartBeatTime,timer);
}

/*****************************************************************************/
//  Description: sfr监听sfr切换线程消息的接口
//  Parameter: [In] p_sig    //signal
//             [Out] none
//             [Return] 错误码
//  Author: miao.liu2
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN MMISFR_CMCC_ProcessLwm2mMsg(void* p_sig)
{
    MMISFR_CMCC_MSG_T    *p_content = PNULL;
    uint32 timer = 0;
    //入参有效性判断
    if (PNULL == p_sig)
    {
        TRACE_SFR_CMCC("p_sig == NULL.");
        return;
    }
    p_content = ((MMISFR_CMCC_SIGNAL_T *)p_sig)->p_content;
    if(PNULL == p_content)
    {
        TRACE_SFR_CMCC("p_content == NULL.");
        return;
    }

    switch(p_content->msg_id)
    {
        case MSG_MMISFR_CMCC_REGISTER_SUCCESS:
        {
            MMISFR_CMCC_Lwm2mStop(FALSE);
            MMISFR_CMCC_NextLoop(TRUE,TRUE);
            break;
        }
        case MSG_MMISFR_CMCC_REGISTER_FAIL:
        {
            MMISFR_CMCC_Lwm2mStop(TRUE);
            MMISFR_CMCC_NextLoop(FALSE,FALSE);
            break;
        }
        case MSG_MMISFR_CMCC_UPDATE_FAIL:
        {
            MMISFR_CMCC_Lwm2mStop(TRUE);
            MMISFR_CMCC_NextLoop(TRUE,FALSE);
            break;
        }
        case MSG_MMISFR_CMCC_IP_CHANGED:
        {
            //stop and restart to connect new server
            MMISFR_CMCC_Lwm2mStop(TRUE);
            timer = ual_timer_start(p_content->time_out, sfr_cmcc_handle_restart_timer, NULL, FALSE);
            TRACE_SFR_CMCC("timer=%d,time_out=%d", timer,p_content->time_out);
            break;
        }
        default:
            break;
    }
}


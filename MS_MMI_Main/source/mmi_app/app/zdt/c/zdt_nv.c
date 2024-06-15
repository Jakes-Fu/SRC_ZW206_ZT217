/*************************************************************************
 ** File Name:      mmicc_nv.c                                          *
 ** Author:         bruce.chi                                           *
 ** Date:           2006/09/25                                           *
 ** Copyright:      2006 Spreadtrum, Incorporated. All Rights Reserved.    *
 ** Description:     This file defines the function about nv             *
 *************************************************************************
 *************************************************************************
 **                        Edit History                                  *
 ** ---------------------------------------------------------------------*
 ** DATE           NAME             DESCRIPTION                          *
 ** 2006/09/25     bruce.chi        Create.                              *
*************************************************************************/


#define _MMIZDT_NV_C_


/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "std_header.h"
#include "sci_types.h"
#include "zdt_nv.h"
#include "zdt_net.h"
#include "mmi_modu_main.h"
#include "zdthttp_api.h"
#if defined(ZDT_GSENSOR_SUPPORT) &&  defined(ZTE_PEDOMETER_SUPPORT)
#include "pedometer_nv.h"
#endif
#include "version.h"

LOCAL ZDT_ICCID s_zdt_iccid = {0};


#ifdef ZDT_NFC_SUPPORT
LOCAL ZDT_NFC_INFO s_zdt_nfc_info = {0};
#endif

/*the length of set nv*/
const uint16 zdt_nv_len[] =
{
    sizeof(uint8),
    sizeof(uint8),//MMI_ZDT_NV_FIRST_PWON
#ifdef ZDT_NET_SUPPORT
    sizeof(uint8), //MMI_ZDT_NV_NET_SYS
    sizeof(ZDT_ICCID), // MMI_ZDT_NV_ICCID
    sizeof(ZDT_RFID), //MMI_ZDT_NV_RFID
#endif
#ifdef ZDT_WIFI_SUPPORT
    sizeof(uint8), //MMI_ZDT_NV_WIFI_ONOFF
#endif
#ifdef ZDT_PLAT_SHB_SUPPORT
    sizeof(uint8),//MMI_ZDT_NV_SHB_ALERT
#endif

#ifdef ZDT_LED_SUPPORT
    sizeof(uint8), //MMI_ZDT_NV_LED_CHARGE
    sizeof(uint8), //MMI_ZDT_NV_LED_SMS
    sizeof(uint8), //MMI_ZDT_NV_LED_CALL
    sizeof(uint8), //MMI_ZDT_NV_LED_POWER
    sizeof(uint8), //MMI_ZDT_NV_LED_FLIP
#endif
    sizeof(uint8), //MMI_ZDT_NV_NEED_RESET
    sizeof(uint8),//yangyu add for MMI_ZDT_NV_REJECT_CALL_ONOFF
    sizeof(uint8),//yangyu add ZDT_FIRST_BIND
    sizeof(STEP_NV_T), //MMI_ZDT_STEPS
    sizeof(uint8), //MMI_ZDT_AUTO_POWER
#ifdef ZDT_NFC_SUPPORT
    sizeof(ZDT_NFC_INFO),
#endif
    sizeof(uint8),//MMI_ZDT_NV_NEED_WHITECALL
#ifdef SETUP_WIZARD_SUPPORT
    sizeof(uint8),//MMI_ZDT_NV_SETUP_WIZARD 开机向导
#endif
#ifdef ZTE_WATCH
    sizeof(uint8), // MMI_ZDT_NV_AUTO_LOW_BATTERY 努比亚需求电量低于20%时进入长续航模式开关
    sizeof(uint8), // MMI_ZDT_NV_AUTO_ANSWER 努比亚需求绑定成员拨打电话超过15S没接听自动接听开关
    sizeof(uint8), // MMI_ZDT_NV_SMS_REPORT 努比亚需求短信代收开关
    sizeof(uint8), // MMI_ZDT_NV_DEVICE_REPORT 努比亚需求联网累计10分钟后上报设备信息
    sizeof(uint8), // MMI_ZDT_NV_AUTO_LOW_BATTERY_FLAG 努比亚需求电量低于20%时自动进入长续航模式，充电电量大于20%时自动退出
    sizeof(uint), // MMI_ZDT_NV_TARGET_STEP 努比亚需求目标步数
    sizeof(uint), // MMI_ZDT_NV_NETWORK_TIMES 开机联网累计时间
#endif
#ifdef W217_AGING_TEST_CUSTOM //new_aginglist
    sizeof(uint32), //MMI_ZDT_AGING_TIMES 老化时间
#endif
    24*sizeof(uint8),//网标号码
    sizeof(uint8),  //MMI_ZDT_NV_INFO_RESET
    sizeof(ZDT_SWVER), //sw version
    sizeof(MMI_ZDT_FACTORY_T), //工测模式保留数据
    sizeof(uint8),  //max
};

/*****************************************************************************/
// 	Description : register set module nv len and max item
//	Global resource dependence : none
//  Author: bruce.chi
//	Note:
/*****************************************************************************/
PUBLIC void MMIZDT_RegNv(void)
{
    MMI_RegModuleNv(MMI_MODULE_ZDT, zdt_nv_len, ARR_SIZE(zdt_nv_len));
}

LOCAL uint8  s_zdt_first_pwon  = TRUE;

PUBLIC uint8 MMIZDT_NV_GetFirstPWON(void)
{
    MN_RETURN_RESULT_E      return_value = MN_RETURN_FAILURE;
    uint8 first_pwon = TRUE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_FIRST_PWON,&first_pwon);
    if (MN_RETURN_SUCCESS != return_value)
    {
        //自动识别
        first_pwon = TRUE;
        MMI_WriteNVItem(MMI_ZDT_NV_FIRST_PWON,&first_pwon);
    }
    s_zdt_first_pwon = first_pwon;
    return first_pwon;
}

PUBLIC BOOLEAN  MMIZDT_NV_SetFirstPWON(uint8 first_pwon) 
{
    uint8 cur_val = first_pwon;
    if(s_zdt_first_pwon != first_pwon)
    {
        MMI_WriteNVItem(MMI_ZDT_NV_FIRST_PWON,&cur_val);
        s_zdt_first_pwon = first_pwon;
        return TRUE;
    }
    return FALSE;
}

PUBLIC uint8 MMIZDT_GetFirstPWON(void)
{
    return s_zdt_first_pwon;
}

#if 1
LOCAL uint8  s_zdt_need_reset  = TRUE;
PUBLIC uint8 MMIZDT_NV_GetNeedReset(void)
{
    MN_RETURN_RESULT_E      return_value = MN_RETURN_FAILURE;
    uint8 need_reset = TRUE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_NEED_RESET,&need_reset);
    if (MN_RETURN_SUCCESS != return_value)
    {
        //自动识别
        need_reset = TRUE;
        MMI_WriteNVItem(MMI_ZDT_NV_NEED_RESET,&need_reset);
    }
    s_zdt_need_reset = need_reset;
    return need_reset;
}

PUBLIC BOOLEAN  MMIZDT_NV_SetNeedReset(uint8 need_reset) 
{
    uint8 cur_val = need_reset;
    if(s_zdt_need_reset != need_reset)
    {
        MMI_WriteNVItem(MMI_ZDT_NV_NEED_RESET,&cur_val);
        s_zdt_need_reset = need_reset;
        return TRUE;
    }
    return FALSE;
}

PUBLIC uint8 MMIZDT_GetNeedReset(void)
{
    return s_zdt_need_reset;
}
#endif

#ifdef ZDT_NET_SUPPORT
PUBLIC char * MMIZDT_NVGetICCID(void)
{
    if (MN_RETURN_SUCCESS != MMI_ReadNVItem(MMI_ZDT_NV_ICCID, &s_zdt_iccid))
    {
        SCI_MEMSET(&s_zdt_iccid, 0, sizeof(ZDT_ICCID));
        MMI_WriteNVItem(MMI_ZDT_NV_ICCID, &s_zdt_iccid);
    }
   return s_zdt_iccid.num;
}

PUBLIC void MMIZDT_NVSetICCID(char *iccid)
{
    SCI_MEMSET(&s_zdt_iccid, 0, sizeof(ZDT_ICCID));
    SCI_MEMCPY(&s_zdt_iccid.num, iccid, ZDT_ICCID_LEN);

    MMI_WriteNVItem(MMI_ZDT_NV_ICCID, &s_zdt_iccid);
}

PUBLIC BOOLEAN MMIZDT_IsICCIDChanged(char * sim_iccid)
{
    char *p_nv_iccid;
    char nv_iccid[ZDT_ICCID_LEN + 1] = {0};
    if(sim_iccid == NULL)
    {
        return FALSE;
    }
    p_nv_iccid = MMIZDT_NVGetICCID();

    if (p_nv_iccid[0]== NULL)
    {
    	MMIZDT_NVSetICCID(sim_iccid);
        return FALSE;
    }

    SCI_MEMCPY(nv_iccid, p_nv_iccid, ZDT_ICCID_LEN);

    if (SCI_MEMCMP(sim_iccid, nv_iccid, ZDT_ICCID_LEN) == 0)
    {
        SCI_TraceLow("MMIZDT_IsICCIDChanged not changed.");
        return FALSE;
    }

    SCI_TraceLow("MMIZDT_IsICCIDChanged changed [%s]", sim_iccid);
    return TRUE;
}
#endif

#ifdef ZDT_NET_SUPPORT
PUBLIC char * MMIZDT_NVGetRFID(void)
{
    if (MN_RETURN_SUCCESS != MMI_ReadNVItem(MMI_ZDT_NV_RFID, &g_zdt_phone_rfid))
    {
        SCI_MEMSET(&g_zdt_phone_rfid, 0, sizeof(ZDT_RFID));
        MMI_WriteNVItem(MMI_ZDT_NV_RFID, &g_zdt_phone_rfid);
    }
   return g_zdt_phone_rfid.num;
}

PUBLIC void MMIZDT_NVSetRFID(char *rfid)
{
    SCI_MEMSET(&g_zdt_phone_rfid, 0, sizeof(ZDT_RFID));
    SCI_MEMCPY(&g_zdt_phone_rfid.num, rfid, ZDT_RFID_LEN);

    MMI_WriteNVItem(MMI_ZDT_NV_RFID, &g_zdt_phone_rfid);
}

PUBLIC void MMIZDT_NVClearRFID(void)
{
    SCI_MEMSET(&g_zdt_phone_rfid, 0, sizeof(ZDT_RFID));
    MMI_WriteNVItem(MMI_ZDT_NV_RFID, &g_zdt_phone_rfid);
}

PUBLIC BOOLEAN MMIZDT_IsRFIDChanged(char * phone_rfid)
{
    char *p_nv_rfid;
    char nv_rfid[ZDT_RFID_LEN + 1] = {0};
    if(phone_rfid == NULL)
    {
        return FALSE;
    }
    p_nv_rfid = g_zdt_phone_rfid.num;

    if (p_nv_rfid[0]== NULL)
    {
    	 MMIZDT_NVSetRFID(phone_rfid);
        return FALSE;
    }

    SCI_MEMCPY(nv_rfid, p_nv_rfid, ZDT_RFID_LEN);

    if (SCI_MEMCMP(phone_rfid, nv_rfid, ZDT_RFID_LEN) == 0)
    {
        SCI_TraceLow("MMIZDT_IsRFIDChanged not changed.");
        return FALSE;
    }

    SCI_TraceLow("MMIZDT_IsRFIDChanged changed [%s]", phone_rfid);
    return TRUE;
}
#endif

PUBLIC void ZDT_NV_SetIsAutoPower(BOOLEAN isOn)
{
    BOOLEAN status = isOn;
    MMINV_WRITE(MMI_ZDT_AUTO_POWER, &status);
}

PUBLIC BOOLEAN ZDT_NV_GetIsAutoPower()
{
    BOOLEAN status = FALSE;
    MN_RETURN_RESULT_E  return_value    =   MN_RETURN_FAILURE;

    MMINV_READ(MMI_ZDT_AUTO_POWER, &status, return_value);
    
    if (MN_RETURN_SUCCESS != return_value)
    {
        MMINV_WRITE(MMI_ZDT_AUTO_POWER, &status);
    }
    SCI_TraceLow("ZDT_NV_GetIsAutoPower status = %d " ,status );
    return status;
}

PUBLIC void  MMIZDT_NV_SetRejectUnknownCall(uint8 isOn) 
{
     uint8 status = isOn;
    MMI_WriteNVItem(MMI_ZDT_NV_REJECT_CALL_ONOFF, &status);
}

#ifdef ZDT_NFC_SUPPORT
PUBLIC BOOLEAN MMIZDT_NVGetNFC_INFO(uint8 * info_buf)
{
    BOOLEAN res = TRUE;
    if (MN_RETURN_SUCCESS != MMI_ReadNVItem(MMI_ZDT_NV_NFC_INFO, &s_zdt_nfc_info))
    {
        SCI_MEMSET(&s_zdt_nfc_info, 0, sizeof(ZDT_NFC_INFO));
        MMI_WriteNVItem(MMI_ZDT_NV_NFC_INFO, &s_zdt_nfc_info);
        res = FALSE;
    }
    if(s_zdt_nfc_info.info[0] != 0)
    {
        SCI_MEMCPY(info_buf,s_zdt_nfc_info.info,ZDT_NFC_INFO_LEN);
    }
   return res;
}

PUBLIC void MMIZDT_NVSetNFC_INFO(uint8 *info_buf)
{
    SCI_MEMSET(&s_zdt_nfc_info, 0, sizeof(ZDT_NFC_INFO));
    SCI_MEMCPY(&s_zdt_nfc_info.info, info_buf, ZDT_NFC_INFO_LEN);
    MMI_WriteNVItem(MMI_ZDT_NV_NFC_INFO, &s_zdt_nfc_info);
}
#endif

#if 1
LOCAL uint8  s_zdt_need_white_call  = TRUE;
PUBLIC uint8 MMIZDT_NV_GetNeedWhiteCall(void)
{
    MN_RETURN_RESULT_E      return_value = MN_RETURN_FAILURE;
    uint8 need_white_call = TRUE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_NEED_WHITECALL,&need_white_call);
    if (MN_RETURN_SUCCESS != return_value)
    {
        need_white_call = FALSE;
        MMI_WriteNVItem(MMI_ZDT_NV_NEED_WHITECALL,&need_white_call);
    }
    s_zdt_need_white_call = need_white_call;
    return need_white_call;
}

PUBLIC BOOLEAN  MMIZDT_NV_SetNeedWhiteCall(uint8 need_white_call) 
{
    uint8 cur_val = need_white_call;
    if(s_zdt_need_white_call != need_white_call)
    {
        MMI_WriteNVItem(MMI_ZDT_NV_NEED_WHITECALL,&cur_val);
        s_zdt_need_white_call = need_white_call;
        return TRUE;
    }
    return FALSE;
}

PUBLIC uint8 MMIZDT_GetNeedWhiteCall(void)
{
    return s_zdt_need_white_call;
}
#endif

#ifdef SETUP_WIZARD_SUPPORT
LOCAL uint8  s_get_setup_wizard_status  = FALSE;
PUBLIC uint8 MMIZDT_Get_Setup_Wizard_Status(void)
{
    return s_get_setup_wizard_status;
}

LOCAL void Init_Setup_Wizard_Status(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint8 status = TRUE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_SETUP_WIZARD,&status);
    if (MN_RETURN_SUCCESS != return_value)
    {
        status = TRUE;
        MMI_WriteNVItem(MMI_ZDT_NV_SETUP_WIZARD,&status);
    }
    s_get_setup_wizard_status = status;
}

PUBLIC void MMIZDT_NV_Set_Setup_Wizard(uint8 status)
{
    uint8 set_status = status;
    MMI_WriteNVItem(MMI_ZDT_NV_SETUP_WIZARD,&set_status);
    s_get_setup_wizard_status = status;
}

#endif

#ifdef ZTE_WATCH
//低电时自动进入长续航模式开关同步服务器
LOCAL uint8  s_auto_low_battery_status  = TRUE;
//低电时自动进入长续航模式标记 自动进入自动退出
LOCAL uint8  s_auto_low_battery_flag  = FALSE;
//短信代收开关同步服务器
LOCAL uint8  s_report_sms_status  = TRUE;
//绑定成员拨打电话超过15S没接听自动接听开关
LOCAL uint8  s_auto_answer_call_status  = FALSE;
//开机联网累计10分钟后上报
LOCAL uint8  s_device_report_status  = FALSE;

LOCAL uint s_device_network_connect_time  = 0;

PUBLIC uint8 MMIZDT_Get_Auto_Low_Battery_Status(void)
{
    return s_auto_low_battery_status;
}

LOCAL void Init_Auto_Low_Battery_Status(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint8 status = TRUE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_AUTO_LOW_BATTERY,&status);
    if (MN_RETURN_SUCCESS != return_value)
    {
        status = TRUE;
        MMI_WriteNVItem(MMI_ZDT_NV_AUTO_LOW_BATTERY,&status);
    }
    s_auto_low_battery_status = status;
}

PUBLIC void MMIZDT_NV_Set_Auto_Low_Battery(uint8 status)
{
    if(status != s_auto_low_battery_status)
    {
        uint8 set_status = status;
        MMI_WriteNVItem(MMI_ZDT_NV_AUTO_LOW_BATTERY,&set_status);
        s_auto_low_battery_status = status;
    }
}

PUBLIC uint8 MMIZDT_Get_Auto_Low_Battery_Flag(void)
{
    return s_auto_low_battery_flag;
}

LOCAL void Init_Auto_Low_Battery_Flag(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint8 status = FALSE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_AUTO_LOW_BATTERY_FLAG,&status);
    if (MN_RETURN_SUCCESS != return_value)
    {
        status = FALSE;
        MMI_WriteNVItem(MMI_ZDT_NV_AUTO_LOW_BATTERY_FLAG,&status);
    }
    s_auto_low_battery_flag = status;
}

PUBLIC void MMIZDT_NV_Set_Auto_Low_Battery_Flag(uint8 status)
{
    if(status != s_auto_low_battery_flag)
    {
        uint8 set_status = status;
        MMI_WriteNVItem(MMI_ZDT_NV_AUTO_LOW_BATTERY_FLAG,&set_status);
        s_auto_low_battery_flag = status;
    }
}

PUBLIC uint8 MMIZDT_Get_Report_Sms_Status(void)
{
    return s_report_sms_status;
}

LOCAL void Init_Report_Sms_Status(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint8 status = FALSE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_SMS_REPORT,&status);
    if (MN_RETURN_SUCCESS != return_value)
    {
        status = FALSE;
        MMI_WriteNVItem(MMI_ZDT_NV_SMS_REPORT,&status);
    }
    s_report_sms_status = status;
}

PUBLIC void MMIZDT_NV_Set_Report_Sms_Status(uint8 status)
{
    uint8 set_status = status;
    MMI_WriteNVItem(MMI_ZDT_NV_SMS_REPORT,&set_status);
    s_report_sms_status = status;
}

PUBLIC uint8 MMIZDT_Get_Auto_Answer_Call_Status(void)
{
#ifdef WIN32
    return 1;
#endif
    return s_auto_answer_call_status;
}

LOCAL void Init_Auto_Answer_Call_Status(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint8 status = FALSE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_AUTO_ANSWER,&status);
    if (MN_RETURN_SUCCESS != return_value)
    {
        status = FALSE;
        MMI_WriteNVItem(MMI_ZDT_NV_AUTO_ANSWER,&status);
    }
    s_auto_answer_call_status = status;
}

PUBLIC void MMIZDT_NV_Set_Auto_Answer_Call_Status(uint8 status)
{
    if(status != s_auto_answer_call_status)
    {
        uint8 set_status = status;
        MMI_WriteNVItem(MMI_ZDT_NV_AUTO_ANSWER,&set_status);
        s_auto_answer_call_status = status;
    }
}

PUBLIC uint8 MMIZDT_Get_Device_Report_Status(void)
{
    return s_device_report_status;
}

LOCAL void Init_Device_Report_Status(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint8 status = FALSE;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_DEVICE_REPORT,&status);
    if (MN_RETURN_SUCCESS != return_value)
    {
        status = FALSE;
        MMI_WriteNVItem(MMI_ZDT_NV_DEVICE_REPORT,&status);
    }
    s_device_report_status = status;
}

PUBLIC void MMIZDT_NV_Set_Device_Report_Status(uint8 status)
{
    if(status != s_device_report_status)
    {
        uint8 set_status = status;
        MMI_WriteNVItem(MMI_ZDT_NV_DEVICE_REPORT,&set_status);
        s_device_report_status = status;
    }
}

PUBLIC uint MMIZDT_Get_Target_Step(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint step = 3000;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_TARGET_STEP,&step);
    if (MN_RETURN_SUCCESS != return_value)
    {
        MMI_WriteNVItem(MMI_ZDT_NV_TARGET_STEP,&step);
    }
    return step;
}

PUBLIC void MMIZDT_NV_Set_Target_Step(uint step)
{
    MMI_WriteNVItem(MMI_ZDT_NV_TARGET_STEP,&step);
}

LOCAL void Init_Netword_Time(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint time = 0;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_NETWORK_TIMES,&time);
    if (MN_RETURN_SUCCESS != return_value)
    {
        time = 0;
        MMI_WriteNVItem(MMI_ZDT_NV_NETWORK_TIMES,&time);
    }
    s_device_network_connect_time = time;
}

//累计联网时间
PUBLIC void MMIZDT_NV_Set_Netword_Connect_Time(uint time)
{
    SCI_TraceLow("MMIZDT_NV_Set_Netword_Connect_Time s_device_network_connect_time:%d",s_device_network_connect_time);
    if(s_device_network_connect_time < 10*60) //10分钟
    {
        s_device_network_connect_time += time;
        MMI_WriteNVItem(MMI_ZDT_NV_NETWORK_TIMES,&time);
    }
    else
    {
        SCI_TraceLow("MMIZDT_NV_Set_Netword_Connect_Time s_device_report_status:%d",s_device_report_status);
        if(s_device_report_status == 0)
        {
            MMIZDT_HTTP_ZTE_Device_Report();
        }
    }
}

#endif //ZTE_WATCH end

//电子网标ID
LOCAL uint8  s_net_id_status = TRUE;

PUBLIC uint8 MMIZDT_Get_CTA_Net_Id_Status()
{
    return s_net_id_status;
}

PUBLIC void MMIZDT_Get_CTA_Net_Id(uint8 *net_id)
{
    if(net_id != PNULL)
    {
        MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
        MMI_ReadNVItem(MMI_ZDT_NV_NET_ID,net_id);
        if(strlen(net_id) > 0)
        {
            s_net_id_status = 0;
        }
    }
}

LOCAL void Init_CTA_Net_Id_Status(void)
{
    uint8 net_id[24] = {0};
    MMIZDT_Get_CTA_Net_Id(net_id);
}

PUBLIC void MMIZDT_Set_CTA_Net_Id(uint8 *net_id)
{
    if(net_id != PNULL)
    {
        if(strlen(net_id) > 0)
        {
            MMI_WriteNVItem(MMI_ZDT_NV_NET_ID,net_id);
            s_net_id_status = 0;
        }
    }
}

PUBLIC void MMIZDT_NV_Init()
{
    MMIZDT_NV_GetFirstPWON();
    MMIZDT_NVGetRFID();
    MMIZDT_NVGetSWVER();
    MMIZDT_NV_GetNeedWhiteCall();
#ifdef SETUP_WIZARD_SUPPORT
    Init_Setup_Wizard_Status();
#endif
#ifdef ZTE_WATCH
    Init_Auto_Low_Battery_Status();
    Init_Auto_Low_Battery_Flag();
    Init_Report_Sms_Status();
    Init_Auto_Answer_Call_Status();
    Init_Device_Report_Status();
    Init_CTA_Net_Id_Status();
    Init_Netword_Time();
#endif
}

PUBLIC void MMIZDT_NV_ResetFactory()
{
    MMIZDT_NV_SetFirstPWON(1);
    MMIZDT_NV_SetRejectUnknownCall(0);
#ifdef SETUP_WIZARD_SUPPORT
    MMIZDT_NV_Set_Setup_Wizard(0);
#endif
#ifdef ZTE_WATCH
    MMIZDT_NV_Set_Auto_Answer_Call_Status(0);
    MMIZDT_NV_Set_Report_Sms_Status(0);
    MMIZDT_NV_Set_Auto_Low_Battery_Flag(0);
    MMIZDT_NV_Set_Auto_Low_Battery(0);
    MMIZDT_Set_CTA_Net_Id("");
    MMIZDT_NV_Set_Target_Step(3000); //默认3000
#if defined(ZDT_GSENSOR_SUPPORT) &&  defined(ZTE_PEDOMETER_SUPPORT)
    MMI_Pedometer_Factory();
#endif
    MMIZDT_NV_Set_Device_Report_Status(0);
#endif
}


#ifdef W217_AGING_TEST_CUSTOM //new_aginglist

PUBLIC uint32 MMIZDT_Get_Aging_Times(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint32 times = 0;
    return_value = MMI_ReadNVItem(MMI_ZDT_AGING_TIMES,&times);
    if (MN_RETURN_SUCCESS != return_value)
    {
        times = 0;
        MMI_WriteNVItem(MMI_ZDT_AGING_TIMES,&times);
    }
    return times;
}

PUBLIC void MMIZDT_NV_Set_Aging_Times(uint32 times)
{
    uint32 set_times = times;
    MMI_WriteNVItem(MMI_ZDT_AGING_TIMES,&set_times);
}

#endif 



PUBLIC uint8 MMIZDT_Get_InfoReset(void)
{
    MN_RETURN_RESULT_E  return_value = MN_RETURN_FAILURE;
    uint8 info = 0;
    return_value = MMI_ReadNVItem(MMI_ZDT_NV_INFO_RESET,&info);
    if (MN_RETURN_SUCCESS != return_value)
    {
        info = 0;
        MMI_WriteNVItem(MMI_ZDT_NV_INFO_RESET,&info);
    }
    return info;
}

PUBLIC void MMIZDT_NV_Set_InfoReset(uint8 info)
{
    uint8 set_info = info;
    MMI_WriteNVItem(MMI_ZDT_NV_INFO_RESET,&set_info);
}

#if 1  //FOTA升级后版本号改变需要把NV复位
ZDT_SWVER  g_zdt_phone_swver = {0};

PUBLIC char * MMIZDT_NVGetSWVER(void)
{
    if (MN_RETURN_SUCCESS != MMI_ReadNVItem(MMI_ZDT_NV_SWVER, &g_zdt_phone_swver))
    {
        SCI_MEMSET(&g_zdt_phone_swver, 0, sizeof(ZDT_SWVER));
        SCI_MEMCPY(&g_zdt_phone_swver.num, ZDT_SFR_SW_VER, SCI_STRLEN(ZDT_SFR_SW_VER));
        MMI_WriteNVItem(MMI_ZDT_NV_SWVER, &g_zdt_phone_swver);
    }
   return g_zdt_phone_swver.num;
}

PUBLIC void MMIZDT_NVSetSWVER(char *swver)
{
    SCI_MEMSET(&g_zdt_phone_swver, 0, sizeof(ZDT_SWVER));
    SCI_MEMCPY(&g_zdt_phone_swver.num, swver, SCI_STRLEN(swver));

    MMI_WriteNVItem(MMI_ZDT_NV_SWVER, &g_zdt_phone_swver);
}

PUBLIC void MMIZDT_NVClearSWVER(void)
{
    SCI_MEMSET(&g_zdt_phone_swver, 0, sizeof(ZDT_SWVER));
    MMI_WriteNVItem(MMI_ZDT_NV_SWVER, &g_zdt_phone_swver);
}

PUBLIC void MMIZDT_NV_ResetFOTA()
{
#if 0
#ifdef ZTE_WATCH
    MMIZDT_NV_Set_Auto_Low_Battery(0);
    MMIZDT_NV_Set_Auto_Answer_Call_Status(0);
    MMIZDT_NV_Set_Report_Sms_Status(0);
    MMIZDT_NV_Set_Device_Report_Status(0);
    MMIZDT_NV_Set_Auto_Low_Battery_Flag(0);
    MMIZDT_Set_CTA_Net_Id("");
    MMIZDT_NV_Set_Target_Step(3000);
    MMI_Pedometer_Factory();
    MMIZDT_NV_Set_Netword_Connect_Time(0);
#endif
#ifdef W217_AGING_TEST_CUSTOM //new_aginglist
    MMIZDT_NV_Set_Aging_Times(0);
#endif
#endif
    MMIZDT_NVSetSWVER(ZDT_SFR_SW_VER);
}
PUBLIC BOOLEAN MMIZDT_IsSWVERChanged(void)
{
    char *p_nv_swver;
    char nv_swver[ZDT_SWVER_LEN + 1] = {0};

    p_nv_swver = g_zdt_phone_swver.num;

    if (p_nv_swver[0]== NULL)
    {
    	 MMIZDT_NVSetSWVER(ZDT_SFR_SW_VER);
        return FALSE;
    }

    SCI_MEMCPY(nv_swver, p_nv_swver, ZDT_SWVER_LEN);

    if (strncmp(nv_swver, ZDT_SFR_SW_VER,SCI_STRLEN(ZDT_SFR_SW_VER)) == 0)
    {
        SCI_TraceLow("MMIZDT_IsSWVERChanged not changed.");
        return FALSE;
    }

    SCI_TraceLow("MMIZDT_IsSWVERChanged changed [%s]", ZDT_SFR_SW_VER);
    MMIZDT_NV_ResetFOTA();
    return TRUE;
}
#endif

static  MMI_ZDT_FACTORY_T em_test = {0};

PUBLIC uint8* MMIZDT_NVGetFactory_Tese_Init()
{
     if (MN_RETURN_SUCCESS != MMI_ReadNVItem(MMI_ZDT_NV_FACTORY_TEST, &em_test))
     {
         SCI_MEMSET(&em_test, 0, sizeof(MMI_ZDT_FACTORY_T));
         MMI_WriteNVItem(MMI_ZDT_NV_FACTORY_TEST, &em_test);
     }
    return em_test.em_test_is_ok;
}

PUBLIC void MMIZDT_NVSet_Factory_Tese(uint8 *em_is_ok,uint em_is_ok_len)
{
    SCI_MEMSET(&em_test, 0, sizeof(MMI_ZDT_FACTORY_T));
    SCI_MEMCPY(&em_test.em_test_is_ok, em_is_ok, em_is_ok_len);
    MMI_WriteNVItem(MMI_ZDT_NV_FACTORY_TEST, &em_test);
}

PUBLIC void MMIZDT_NVClear_Factory_Tese(void)
{
    SCI_MEMSET(&em_test, 0, sizeof(MMI_ZDT_FACTORY_T));
    MMI_WriteNVItem(MMI_ZDT_NV_FACTORY_TEST, &em_test);
}



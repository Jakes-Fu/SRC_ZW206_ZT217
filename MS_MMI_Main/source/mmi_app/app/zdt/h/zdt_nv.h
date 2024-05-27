/*************************************************************************
 ** File Name:      mmicc_nv.h                                           *
 ** Author:         bruce.chi                                            *
 ** Date:           2006/09/25                                           *
 ** Copyright:      2006 Spreadtrum, Incorporated. All Rights Reserved.  *
 ** Description:     This file defines the function about nv             *
 *************************************************************************
 *************************************************************************
 **                        Edit History                                  *
 ** ---------------------------------------------------------------------*
 ** DATE           NAME             DESCRIPTION                          *
 ** 2006/09/25     bruce.chi        Create.                               *
*************************************************************************/

#ifndef _MMIZDT_NV_H_
#define _MMIZDT_NV_H_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "mmi_module.h"
#include "mn_type.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/
#ifdef ZDT_NFC_SUPPORT
#define  ZDT_NFC_INFO_LEN 35

typedef struct _ZDT_NFC_INFO
{
    uint8 info[ZDT_NFC_INFO_LEN];
} ZDT_NFC_INFO;
#endif

typedef struct
{
    uint32 steps;
    uint8 month;
    uint8 day;
}STEP_NV_T;

typedef struct _MMI_ZDT_FACTORY_T
{
   uint8 em_test_is_ok[30+1];
} MMI_ZDT_FACTORY_T ;


typedef enum
{
        MMI_ZDT_NV_RUNMODE = (MMI_MODULE_ZDT << 16),
        MMI_ZDT_NV_FIRST_PWON,
#ifdef ZDT_NET_SUPPORT
        MMI_ZDT_NV_NET_SYS,
        MMI_ZDT_NV_ICCID,
        MMI_ZDT_NV_RFID,
#endif
#ifdef ZDT_WIFI_SUPPORT
        MMI_ZDT_NV_WIFI_ONOFF,
#endif
#ifdef ZDT_PLAT_SHB_SUPPORT
        MMI_ZDT_NV_SHB_ALERT,
#endif
#ifdef ZDT_LED_SUPPORT
        MMI_ZDT_NV_LED_CHARGE,
        MMI_ZDT_NV_LED_SMS,
        MMI_ZDT_NV_LED_CALL,
        MMI_ZDT_NV_LED_POWER,
        MMI_ZDT_NV_LED_FLIP,
#endif
        MMI_ZDT_NV_NEED_RESET,
        MMI_ZDT_NV_REJECT_CALL_ONOFF,
        ZDT_FIRST_BIND,
        MMI_ZDT_STEPS,
        MMI_ZDT_AUTO_POWER,
#ifdef ZDT_NFC_SUPPORT
        MMI_ZDT_NV_NFC_INFO,
#endif
        MMI_ZDT_NV_NEED_WHITECALL,
#ifdef SETUP_WIZARD_SUPPORT
        MMI_ZDT_NV_SETUP_WIZARD,
#endif
#ifdef ZTE_WATCH
        MMI_ZDT_NV_AUTO_LOW_BATTERY, //Ŭ���������������20%ʱ���볤����ģʽ����
        MMI_ZDT_NV_AUTO_ANSWER, // Ŭ��������󶨳�Ա����绰����15Sû�����Զ���������
        MMI_ZDT_NV_SMS_REPORT,//Ŭ����������Ŵ��տ���
        MMI_ZDT_NV_DEVICE_REPORT,//����2���Ӻ��ϱ�
        MMI_ZDT_NV_AUTO_LOW_BATTERY_FLAG,//�Զ����볤����ģʽ���
        MMI_ZDT_NV_TARGET_STEP,//�Ʋ�Ŀ�경��
        MMI_ZDT_NV_NETWORK_TIMES,//�Ʋ�Ŀ�경��
#endif
#ifdef W217_AGING_TEST_CUSTOM //new_aginglist
        MMI_ZDT_AGING_TIMES,
#endif
        MMI_ZDT_NV_NET_ID,//�������
        MMI_ZDT_NV_INFO_RESET,  // 1==��ˢ�����Ѿ��ָ��������ù�
        MMI_ZDT_NV_SWVER, //sw version
        MMI_ZDT_NV_FACTORY_TEST, //����ģʽ��������
        MMI_ZDT_NV_MAX_ID
}ZDT_NV_ITEM_E;



/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/

PUBLIC void MMIZDT_NV_Init();
PUBLIC void MMIZDT_NV_ResetFactory();
#ifdef ZTE_WATCH
PUBLIC uint8 MMIZDT_Get_Auto_Low_Battery_Flag(void);
PUBLIC void MMIZDT_NV_Set_Auto_Low_Battery_Flag(uint8 status);
PUBLIC uint8 MMIZDT_Get_Auto_Answer_Call_Status(void);
PUBLIC void MMIZDT_NV_Set_Device_Report_Status(uint8 status);
PUBLIC void MMIZDT_NV_Get_Device_Report_Status(uint8 status);
PUBLIC uint8 MMIZDT_Get_CTA_Net_Id_Status();
PUBLIC void MMIZDT_Get_CTA_Net_Id(uint8 *net_id);
PUBLIC void MMIZDT_Set_CTA_Net_Id(uint8 *net_id);
PUBLIC void MMIZDT_NV_Set_Netword_Connect_Time(uint time);
#endif
PUBLIC char * MMIZDT_NVGetSWVER(void);

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif //_MMICC_NV_H_


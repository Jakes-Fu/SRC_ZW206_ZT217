/******************************************************************************
 ** File Name:      version.h                                                 *
 ** Author:         Tony Yao                                                  *
 ** DATE:           23/08/2002                                                *
 ** Copyright:      2002 Spreatrum, Incoporated. All Rights Reserved.         *
 ** Description:                                                              *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 23/08/2002     Tony.Yao         Create.                                   *
 ** 03/11/2003     Xueliang.Wang    Modify.                                   *
 ******************************************************************************/
#ifndef _VERSION_H
#define _VERSION_H

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "sci_types.h"
#include "arm_reg.h"

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif


#ifdef _SW_ZDT_PRODUCT_
#ifdef ZTE_WATCH
//�ͺ�ZW206--ZW35-K1 -EW2302
//�ͺ�ZW202--ZW22-K1 ENTRY-EW2303


//����
#define ZDT_SFR_MANUF 			"ZTE"
//�ͺ�
#ifdef ZTE_WATCH_K1
#define ZDT_SFR_TYPE 			"EW2302"//"ZW35" //app ��Ƶ��������������ж�240X284
//����汾��
#define ZDT_SFR_SW_VER 			"ZT217_LT-T01V1.0.1"//"ZTE_ZW35V1.0.0B17.0"//"ZW206_K1_RDA_240X284_V01"
//��ע�����
#define ZDT_SFR_MODEL 			"NM-ESC-A"
//Ӳ���汾��
#define ZDT_SFR_HW_VER 			"ZT217_MB_V1.01" // UWS6121E
#else
//�ͺ�
#define ZDT_SFR_TYPE 			"EW2303"//"ZW22" //app ��Ƶ��������������ж�240X240
//��ע�����
#define ZDT_SFR_MODEL 			"NM-ESC-A"
//����汾��
#define ZDT_SFR_SW_VER 			"ZT217_LT-T01V1.0.1"////"ZTE_ZW22V1.0.0B17.0""ZW202_K1_ENTRY_RDA_240X240_V01"//"ZW202_K1_ENTRY_V01"//"NM-ESC-A_V2.3"
//Ӳ���汾��
#define ZDT_SFR_HW_VER 			"ZT217_MB_V1.01" // UWS6121E
#endif

#else
//����
#define ZDT_SFR_MANUF 			"NM-ESC-A"
//�ͺ�
#define ZDT_SFR_TYPE 			"H05"
//��ע�����
#define ZDT_SFR_MODEL 			"NM-ESC-A"
//����汾��
#define ZDT_SFR_SW_VER 			"1.0.0"
//Ӳ���汾��
#define ZDT_SFR_HW_VER 			"ZT217_MB_V1.01"

#endif

#define ZDT_FOTA_VER 			ZDT_SFR_SW_VER // "K1 Entry 1.0"

#define ZDT_PlatOS 			"RTOS"
#define ZDT_PlatVer 			"MOCOR20B"
#define ZDT_PlatRAM 			"16M"
#define ZDT_PlatROM 			"16M"
#define ZDT_PlatCPU 			"500MHz"

#define ZDT_CMCC_APP_KEY "M100001047"//"M100001039"
#define ZDT_CMCC_AES_KEY "04145Mk2u4KxlZ4qIADB9l0iR3118VLJ"//"39UT82ajn2pYEh3007A73S5bgR203X53"
#define ZDT_CMCC_AES_KEYLEN (32)
#endif

typedef enum
{
    PLATFORM_VERSION,
    PROJECT_VERSION,
    MMI_VERSION,
    AP_SVN_VERSION,
    CP_SVN_VERSION,
    BASE_VERSION,
    HW_VERSION,
    CALIBRATION_VERSION,
    DOWNLOAD_VERSION,
    BUILD_TIME,
    LOG_VERSION,
    MEMORY_NAME,
#if defined(GPS_SUPPORT) && defined(GPS_CHIP_VER_GREENEYE2)
    GE2_VER,
#endif
    MAX_VERSION_NUM 
}VERSION_TYPE_E;


const char*  VERSION_GetInfo(uint32 type);

uint32  VERSION_GetInfoNum(void);


/*****************************************************************************/
//  Description:    return the project version
//	Global resource dependence:
//  Author:         fancier.fan
//	Note: 2004 1 16
/*****************************************************************************/
const char * COMMON_GetProjectVersionInfo(void);     // return software version 
#ifdef _SW_ZDT_PRODUCT_
const char * COMMON_GetATVersionInfo(void);
#endif

/*+CR79304*/
/*****************************************************************************/
//  Description:    return the version specified by version_type param
//	Global resource dependence:
//  Author:         minqian.qian
//	Note: 
/*****************************************************************************/
const char * COMMON_GetVersionInfo(
                uint8 version_type      //refer to VERSION_TYPE_E
                );
/*-CR79304*/
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
    }
#endif

#endif /* End of version.h*/

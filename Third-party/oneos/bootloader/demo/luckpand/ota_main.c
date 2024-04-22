/**
***********************************************************************************************************************
* Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
*
* @file        ota_main.c
*
* @brief
*
* @revision
* Date         Author       Notes
* 2021-06-16   gongsui      First Version
***********************************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "ota_lib.h"

/**
***********************************************************************************************************************
* @brief    Related Macro Definitions
***********************************************************************************************************************
*/

/* Flash device Configuration */
#define ONCHIP_FLASH_NAME              "onchip_flash"
#define ONCHIP_FLASH_SECTOR_SIZE       (2 * 1024)
#define ONCHIP_FLASH_SIZE              (512 * 1024)

#define EXTERN_FLASH_NAME              "nor_flash"
#define EXTERN_FLASH_SECTOR_SIZE       (4 * 1024)
#define EXTERN_FLASH_SIZE              (16 * 1024 * 1024)

#define OTA_UPGRADE_RETRY_TIMES        1

/* app0 */
#define OTA_APP0_PART_ADDR             ((ota_uint32)0x0800a800)
#define OTA_APP0_PART_SIZE             (90 * 1024)

/* app1 */
#define OTA_APP1_PART_ADDR             ((ota_uint32)0x08021000)
#define OTA_APP1_PART_SIZE             (90 * 1024)

/* app2 */
#define OTA_APP2_PART_ADDR             ((ota_uint32)0x08037800)
#define OTA_APP2_PART_SIZE             (140 * 1024)

/* download */
#define OTA_DL_PART_ADDR               ((ota_uint32)0x0805a800)
#define OTA_DL_PART_SIZE               (150 * 1024)

/* extern_app */
#define OTA_EX_APP_PART_ADDR           ((ota_uint32)0x00000000)
#define OTA_EX_APP_PART_SIZE           (1024 * 1024)

/**
***********************************************************************************************************************
* @brief    Flash operation function
***********************************************************************************************************************
*/

extern ota_int32 stm32_flash_read(ota_uint32 addr, ota_uint8 *buf, ota_size_t size);
extern ota_int32 stm32_flash_write(ota_uint32 addr, const ota_uint8 *buf, ota_size_t size);
extern ota_int32 stm32_flash_erase(ota_uint32 addr, ota_size_t size);

extern ota_int32 exernal_read(ota_uint32 addr, ota_uint8 *buf, ota_size_t size);
extern ota_int32 exernal_write(ota_uint32 addr, const ota_uint8 *buf, ota_size_t size);
extern ota_int32 exernal_erase(ota_uint32 addr, ota_size_t size);

/**
***********************************************************************************************************************
* @brief    FLASH and partition table definition
***********************************************************************************************************************
*/
ota_fal_flash_t fal_flash_table[] =
{
    {ONCHIP_FLASH_NAME, ONCHIP_FLASH_SIZE,  ONCHIP_FLASH_SECTOR_SIZE,  { NULL, stm32_flash_read, stm32_flash_write, stm32_flash_erase} },
    {EXTERN_FLASH_NAME, EXTERN_FLASH_SIZE,  EXTERN_FLASH_SECTOR_SIZE,  { NULL, exernal_read,     exernal_write,     exernal_erase    } },
};

ota_fal_part_info_t fal_part_table[] =
{
    { "app0",      ONCHIP_FLASH_NAME,      OTA_APP0_PART_ADDR,     OTA_APP0_PART_SIZE,    FAL_UPGRADE_PART  },
    { "app1",      ONCHIP_FLASH_NAME,      OTA_APP1_PART_ADDR,     OTA_APP1_PART_SIZE,    FAL_UPGRADE_PART  },
    { "app2",      ONCHIP_FLASH_NAME,      OTA_APP2_PART_ADDR,     OTA_APP2_PART_SIZE,    FAL_UPGRADE_PART  },
    { "download",  ONCHIP_FLASH_NAME,      OTA_DL_PART_ADDR,       OTA_DL_PART_SIZE,      FAL_DOWNLOAD_PART },
    { "ex_app",    EXTERN_FLASH_NAME,      OTA_EX_APP_PART_ADDR,   OTA_EX_APP_PART_SIZE,  FAL_UPGRADE_PART  },
};

#define TABLE_SIZE(x)   (sizeof(x)/sizeof(x[0]))

/**
***********************************************************************************************************************
* @brief   Print Log
***********************************************************************************************************************
*/
void ota_printf(const ota_int8 *fmt, ...)
{
    /* Set it as an empty function when the print is not required */
#if 1
    static ota_int8 info_buf[128] = { 0 };

    va_list      args;
    va_start(args, fmt);
    vsnprintf(info_buf, sizeof(info_buf), fmt, args);
    va_end(args);

    printf("%s", info_buf);
    fflush(stdout);
#endif
}

ota_int32 cmiot_ota_main()
{
    ota_int32 ret = 0;

    ret = ota_fal_init(fal_flash_table, TABLE_SIZE(fal_flash_table), fal_part_table, TABLE_SIZE(fal_part_table));
    if(ret < 0)
    {
        return -1;
    }

    ret = ota_start_up(OTA_UPGRADE_RETRY_TIMES);
    return ret;
}


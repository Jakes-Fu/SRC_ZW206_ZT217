/**
***********************************************************************************************************************
* Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
*
* @file        os_ota_main.c
*
* @brief
*
* @revision
* Date         Author          gongsui
* 2022-03-16   OneOS Team      First Version
***********************************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "ota_lib.h"
#include "ota_type.h"

#include "os_flash.h"
#include "os_ota_main.h"

#include "flash_drvapi.h"
#include "mem_cfg.h"


#define MMI_RES_LOGIC_ADDR    CMIOT_FOTA_ADDRESS1
#define DELTA_NV_LOGIC_ADDR   CMIOT_FOTA_ADDRESS2
#define CP_LOGIC_ADDR         CMIOT_FOTA_ADDRESS3
#define PS_LOGIC_ADDR         CMIOT_FOTA_ADDRESS4
#define XML_LOGIC_ADDR        CMIOT_FOTA_ADDRESS5
#define NV_ITEM_LOGIC_ADDR    CMIOT_FOTA_ADDRESS6

#define XML_PHY_ADDR          XML_LOGIC_ADDR     /*any addr is ok */
#define XML_PART_SIZE         FLASH_SECTOR_SIZE  /*any non-zero value is ok*/

#define DOWNLOAD_LOGIC_ADDR   OTA_DL_PART_ADDR /*defined @fdl_bootloader\nor_fdl\src\fdl_main.c: FOTA_PACKAGE_LOGIC_ADDRESS*/
enum 
{
    IDX_MMI,
    IDX_DELTA_NV,
    IDX_CP,
    IDX_PS,
    IDX_XML,
    IDX_NV_ITEM,
    IDX_DL,
    IDX_MAX,
};
ota_fal_map_t map_info[IDX_MAX] = {0};

#define HEAP_BLOCK_SIZE      (128)
#define HEAP_BUF_MAX_SIZE    (CMIOT_FOTA_AVAILABLE_RAM_SIZE)
static ota_uint8 g_work_heap_buf[HEAP_BUF_MAX_SIZE] = { 0 };

struct cmiot_update_state
{
    unsigned char  inited;
    unsigned short update_result;
};


/**
***********************************************************************************************************************
* @brief    Related Macro Definitions
***********************************************************************************************************************
*/
extern const NOR_FLASH_CONFIG_T s_platform_patitiion_config;
/**
***********************************************************************************************************************
* @brief    FLASH and partition table definition
***********************************************************************************************************************
*/
ota_fal_flash_t fal_flash_table[] =
{
    //{ONCHIP_FLASH_NAME, ONCHIP_FLASH_SIZE,  ONCHIP_FLASH_SECTOR_SIZE,  { NULL, onchip_flash_read, onchip_flash_write, onchip_flash_erase} },
    {EXTERN_FLASH_NAME, EXTERN_FLASH_SIZE,  EXTERN_FLASH_SECTOR_SIZE,  { NULL, nor_flash_read,    nor_flash_write,     nor_flash_erase} },
};

ota_fal_part_info_t fal_part_table[] =
{
    { "mmi_res",      EXTERN_FLASH_NAME,      OTA_APP0_PART_ADDR,     OTA_APP0_PART_SIZE,    FAL_UPGRADE_PART  },
    { "delta_nv",     EXTERN_FLASH_NAME,      OTA_APP1_PART_ADDR,     OTA_APP1_PART_SIZE,    FAL_UPGRADE_PART  },
    { "cp",           EXTERN_FLASH_NAME,      OTA_APP2_PART_ADDR,     OTA_APP2_PART_SIZE,    FAL_UPGRADE_PART  },
    { "stone",        EXTERN_FLASH_NAME,      OTA_APP3_PART_ADDR,     OTA_APP3_PART_SIZE,    FAL_UPGRADE_PART  },
    { "xml",          EXTERN_FLASH_NAME,      OTA_APP4_PART_ADDR,     OTA_APP4_PART_SIZE,    FAL_UPGRADE_PART  },
    { "nvitem",       EXTERN_FLASH_NAME,      OTA_APP5_PART_ADDR,     OTA_APP5_PART_SIZE,    FAL_UPGRADE_PART  },
    { "download",     EXTERN_FLASH_NAME,      OTA_DL_PART_ADDR,       OTA_DL_PART_SIZE,      FAL_DOWNLOAD_PART },
};

#define TABLE_SIZE(x)   (sizeof(x)/sizeof(x[0]))

void ota_printf(const char *fmt, ...)
{
    static char info_buf[128] = { 0 };

    va_list      args;
    va_start(args, fmt);
    vsnprintf(info_buf, sizeof(info_buf), fmt, args);
    va_end(args);

    rprintf("%s", info_buf);
}

/*show update bar, eg: lcd show*/
void ota_update_proc_bar(int percentage)
{
    ota_printf("\r\nupdate ratio: %d%%\r\n", percentage);
    TF_ShowProgress(percentage,100); 
}

static void flash_set_update_flag(void)
{
    struct cmiot_update_state st;

    st.inited = 8;
    st.update_result = 100;

    ota_uint32 addr = map_info[IDX_DL].phy_addr + fal_part_table[IDX_DL].size - EXTERN_FLASH_SECTOR_SIZE;

    nor_flash_read(addr, (ota_uint8*)&st, sizeof(struct cmiot_update_state));

    if (st.update_result != 100)
    {
        nor_flash_erase(addr, EXTERN_FLASH_SECTOR_SIZE);
        st.inited = 8;
        st.update_result = 100;
        nor_flash_write(addr, (ota_uint8*)&st, sizeof(struct cmiot_update_state));
    }
}

static void flash_part_init(void)
{
    map_info[IDX_MMI].logic_addr = MMI_RES_LOGIC_ADDR;
    map_info[IDX_MMI].phy_addr   = s_platform_patitiion_config.mmi_res;
    fal_part_table[IDX_MMI].size = s_platform_patitiion_config.mmi_res_size;
    
    map_info[IDX_DELTA_NV].logic_addr = DELTA_NV_LOGIC_ADDR;
    map_info[IDX_DELTA_NV].phy_addr   = s_platform_patitiion_config.operator_data_addr;
    fal_part_table[IDX_DELTA_NV].size = s_platform_patitiion_config.operator_data_size;

    map_info[IDX_CP].logic_addr = CP_LOGIC_ADDR;
    map_info[IDX_CP].phy_addr   = s_platform_patitiion_config.cp_addr;
    fal_part_table[IDX_CP].size = s_platform_patitiion_config.cp_size;

    map_info[IDX_PS].logic_addr = PS_LOGIC_ADDR;
    map_info[IDX_PS].phy_addr   = s_platform_patitiion_config.ps_addr;
    fal_part_table[IDX_PS].size = s_platform_patitiion_config.fota_bootloader_addr - s_platform_patitiion_config.ps_addr;

    map_info[IDX_XML].logic_addr = XML_LOGIC_ADDR;
    map_info[IDX_XML].phy_addr   = XML_PHY_ADDR;
    fal_part_table[IDX_XML].size = XML_PART_SIZE;
    
    map_info[IDX_NV_ITEM].logic_addr = NV_ITEM_LOGIC_ADDR;
    map_info[IDX_NV_ITEM].phy_addr   = s_platform_patitiion_config.fixnv_addr;
    fal_part_table[IDX_NV_ITEM].size = FIXNV_SECTOR_NUM*FLASH_SECTOR_SIZE;

    /*download part no logic addr*/
    map_info[IDX_DL].logic_addr = DOWNLOAD_LOGIC_ADDR;
    map_info[IDX_DL].phy_addr   = s_platform_patitiion_config.fota_addr;
    fal_part_table[IDX_DL].size = s_platform_patitiion_config.fota_size;

    for (int i = IDX_MMI; i< IDX_MAX; i++)
    {
        /*init fal part*/
        fal_part_table[i].address = map_info[i].logic_addr;
    }
}

int cmiot_ota_main(void)
{
    int ret = 0;
    int i;

    flash_part_init();

    ret = ota_fal_map_init(&map_info, IDX_MAX);
    ota_printf("address map info: \r\n");
    for (i = 0; i < IDX_MAX; i++)
    {
        ota_printf("map[%d]: phy addr = %08x, logic addr = %08x\r\n", i, map_info[i].phy_addr, map_info[i].logic_addr);
    }

    if (ret != 0)
    {
        ota_printf("fal map init failed\r\n");
        return -1;
    }

    ret = ota_fal_init(fal_flash_table, TABLE_SIZE(fal_flash_table), fal_part_table, TABLE_SIZE(fal_part_table));
    if (ret != 0)
    {
        ota_printf("fal init failed\r\n");
        return -1;
    }
#if 0
    flash_set_update_flag();
#endif
    if(ota_heap_init(g_work_heap_buf, HEAP_BUF_MAX_SIZE, HEAP_BLOCK_SIZE) < 0)
    {
        ota_printf("heap buff init error\r\n");
        return -1;
    }
    ota_printf("ota heap addr[%08x], size[%08x]\r\n", g_work_heap_buf, HEAP_BUF_MAX_SIZE);

    if(ret == 0)
    {
        ota_printf("----------------enter process-------------\r\n");
        ret = ota_start_up(1);
        if (ret == 1000 || ret ==1001)
        {
            ret = 0;
            ota_printf("last bin does NOT need update.\r\n");
        }
        ota_printf("----------------exit status[%d]-----------\r\n", ret);
    }

    return ret;
}


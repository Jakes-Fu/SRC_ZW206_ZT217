#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "flash_drvapi.h"
#include "flash_drv.h"
#include "fota_update_fixnv.h"

#include "ota_lib.h"
#include "ota_type.h"
#include "os_flash.h"
#include "os_ota_main.h"
#include "os_nv_update.h"


#define OS_NV_LOG        rprintf

#define APP_XML             (0x9000DADA)    /*should be equal to UWS6121E.xml.bin addr in oneos_config.h*/
#define APP_NV              (0x90000001)
#define XML_BUFF_SIZE       (16 * 1024)     /*UWS6121E.xml is about 9KB*/


extern const NOR_FLASH_CONFIG_T s_platform_patitiion_config;

typedef struct
{
    ota_uint32 offset;
    ota_uint32 size;
}pkg_info_t;

static pkg_info_t xml_info   = {0, 0};
static pkg_info_t delta_info = {0, 0};
static uint8 xml_buff[XML_BUFF_SIZE];

static void save_delta_info(ota_uint32 app, ota_uint32 addr, ota_uint32 size)
{
    if (app == APP_XML)
    {
        xml_info.offset = addr;
        xml_info.size   = size;
    }
    else if (app == APP_NV)
    {
        delta_info.offset = addr;
        delta_info.size   = size;
    }
    else
    {
        OS_NV_LOG("non supported app: 0x%08x\r\n", app);
    }
}

/*load xml from diff pkg*/
uint8 *fota_GetXMLfile(void)
{
    uint32 addr = s_platform_patitiion_config.fota_addr + xml_info.offset;
    
    if (nor_flash_read(addr, xml_buff, xml_info.size) != xml_info.size)
    {
        OS_NV_LOG("get xml file failed\r\n");
        return NULL;
    }
    else
    {
        return xml_buff;
    }
}

/*load nvitem from diff pkg*/
int32 load_new_nvitem(uint8 *buff)
{
    uint32 addr = s_platform_patitiion_config.fota_addr + delta_info.offset;
    
    if (nor_flash_read(addr, buff, delta_info.size) != delta_info.size)
    {
        OS_NV_LOG("load new nv failed\r\n");
        return 0;
    }
    else
    {
        return delta_info.size;
    }
}

ota_int32 extern_full_patch_handler(ota_uint32 app, ota_uint32 addr, ota_uint32 size)
{
    save_delta_info(app, addr, size);
    
    OS_NV_LOG("%s:%d, app = %08x, addr = %08x, size = %08x\r\n", __FUNCTION__, __LINE__, app, addr, size);

    if (app == APP_XML)
    {
        OS_NV_LOG("get xml info ok\r\n");
        return OTA_TRUE;
    }
    else if (app == APP_NV)
    {
        if (xml_info.size == 0)
            return OTA_FALSE;

        if (fota_do_nv_upgrade() == 1)
        {
            OS_NV_LOG("fota_do_nv_upgrade successs\r\n");
            return OTA_TRUE;
        }
        else
        {
            OS_NV_LOG("fota_do_nv_upgrade failed\r\n");
            return OTA_FALSE;
        }
    }
}



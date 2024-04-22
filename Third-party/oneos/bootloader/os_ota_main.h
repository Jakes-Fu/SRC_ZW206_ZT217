#ifndef __OS_OTA_MAIN_H__
#define __OS_OTA_MAIN_H__

#include <oneos_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Flash device Configuration */

#define ONCHIP_FLASH_NAME              "onchip_flash"
#define ONCHIP_FLASH_SECTOR_SIZE       (0x10000)
#define ONCHIP_FLASH_SIZE              (256 * ONCHIP_FLASH_SECTOR_SIZE)


#define EXTERN_FLASH_NAME              "nor_flash"
#define EXTERN_FLASH_SECTOR_SIZE       (0x10000)
#define EXTERN_FLASH_SIZE              (256 * EXTERN_FLASH_SECTOR_SIZE)

/* app0 */
#define OTA_APP0_PART_ADDR             CMIOT_FOTA_ADDRESS1
#define OTA_APP0_PART_SIZE             (0 * EXTERN_FLASH_SECTOR_SIZE)

/* app1 */
#define OTA_APP1_PART_ADDR             CMIOT_FOTA_ADDRESS2
#define OTA_APP1_PART_SIZE             (0 * EXTERN_FLASH_SECTOR_SIZE)

/* app2 */
#define OTA_APP2_PART_ADDR             CMIOT_FOTA_ADDRESS3
#define OTA_APP2_PART_SIZE             (0 * EXTERN_FLASH_SECTOR_SIZE)

/* app3 */
#define OTA_APP3_PART_ADDR             CMIOT_FOTA_ADDRESS4
#define OTA_APP3_PART_SIZE             (0 * EXTERN_FLASH_SECTOR_SIZE)

/* app4 */
#define OTA_APP4_PART_ADDR             CMIOT_FOTA_ADDRESS5
#define OTA_APP4_PART_SIZE             (0 * EXTERN_FLASH_SECTOR_SIZE)

/* app5 */
#define OTA_APP5_PART_ADDR             CMIOT_FOTA_ADDRESS6
#define OTA_APP5_PART_SIZE             (0 * EXTERN_FLASH_SECTOR_SIZE)


/* download */
/*defined @fdl_bootloader\nor_fdl\src\fdl_main.c: FOTA_PACKAGE_LOGIC_ADDRESS*/
#define OTA_DL_PART_ADDR               0x8000000f
#define OTA_DL_PART_SIZE               (0 * EXTERN_FLASH_SECTOR_SIZE)

/* extern_app */
//#define OTA_EX_APP_PART_ADDR           ((unsigned int)0x00000000)
//#define OTA_EX_APP_PART_SIZE           (1024 * 1024)

int cmiot_ota_main(void);

#ifdef __cplusplus
}
#endif

#endif


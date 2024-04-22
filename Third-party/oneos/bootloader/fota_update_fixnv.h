#ifndef __FOTA_UPDATE_FIXNV_H__
#define __FOTA_UPDATE_FIXNV_H__
/*FOTA_SUPPORT_CMIOT*/
#include "sci_types.h"
#include "sfc_drvapi.h"
#include "update_fixnv.h"
#include "fdl_stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*copied from tf_parse_nor.h*/
#define MAX_PATITION_NUM			 	16
#define MAX_PS_PARSE_SIZE                  	8*1024
#define SINGLE_CS_MAX_FLASH_SIZE         (0x1000000)
#define DUAL_CS_OFFSET					0xc000000	
#define MAX_SECARCH_RANGE      	      	       0x1000
#define FLASH_CFG_OFFSET                         0x60	//6800H: 0x60 09A: 0x40
#define FLASH_START_ADRESS             	       0x0
//#define SRAM_BASE_ADDR 				0x4000000 ////move to common.h
#define SLAVE_SECTOR_BUF_ADDESS           (0x04180000)
#define FIX_NV_LEN_OFFSET				(16)
#define PRD_TAIL_INFO_LEN              		(8)
#define PRD_INFO_CRC_OFFSET			(6)
#define PRD_INFO_OFFSET	        		(4)
#define PRD_STATUS_OFFSET				(2)
#define SECTOR_STATUS_OFFSET			(2)
#define ERR_NV_LEN                              		0xffff
#define ITEM_MASK_ID            		      		0x1fff         
#define EFS_INVALID_ITEM         	     		0xffff
#define ERR_VALID_NV_ADDR              		0xffffffff
#define FLASH_START_ADRESS_CS3      	0x3C000000

/*copied from fdl_main.c*/
#define PROD_INFO_MIN_SIZE_512        (512)


/*redefined*/
#ifndef HAL_GET_REF32K
//#define HAL_GET_REF32K                          (hwp_idle->IDL_32K_REF)
#define HAL_GET_REF32K              0xff
#endif



typedef struct
{
    uint32   start_addr;  // == 0xFFFFFFFF means reach tail
    uint32   size;        // == 0
}FLASH_INFOR_T;

BOOLEAN fota_do_nv_upgrade(void);

#ifdef __cplusplus
}
#endif

#endif


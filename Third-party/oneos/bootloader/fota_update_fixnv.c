/******************************************************************************
 ** File Name:      fdl_update_fixnv.c                                                *
 ** Author:         Leo.Feng                                                  *
 ** DATE:           27/06/2002                                                *
 ** Copyright:      2001 Spreatrum, Incoporated. All Rights Reserved.         *
 ** Description:                                                              *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 27/06/2002     Leo.Feng         Create.                                   *
 ** 11/12/2003     Zhemin.Lin       Enhanced according to cr6198              *
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "sci_types.h"
//#include "fdl_main.h"
#include "flash.h"
#include "flash_drv.h"
#include "fdl_crc.h"
//#include "update_fixnv.h"
#include "mem_cfg.h"
#include "common.h"
#ifdef FLASH_TYPE_SPI
#include "spiflash.h"
#endif

/*FOTA_SUPPORT_CMIOT*/
#include "fota_update_fixnv.h"

//#define FIXNV_SECTOR_NUM 2

#if defined(PLATFORM_UWS6121E)
#define FDL_SendTrace rprintf
#define FDL_SendErrLog rprintf
#endif

#define FIX_NV_LEN_OFFSET           (16)
#define PRD_TAIL_INFO_LEN           (8)
#define PRD_INFO_CRC_OFFSET         (6)
#define PRD_INFO_OFFSET_ADDR     (12)
#define PRD_STATUS_OFFSET           (2)
#define SECTOR_STATUS_OFFSET        (2)
#define FLASH_START_ADRESS          (0x0)
#define INVALID_ADDR            (0xFFFFFFFF)

#define FIX_NV_IMEI_FLAG_OFFSET     (4)
#define FIX_NV_IMEI_FLAG_MAGIC      (0x13e1)
#define SRAM_SIZE               (0x40000)

#define SECTOR_GOOD_STATUS        (0x55aa)   // The sector is "GOOD"
#define FLASH_SIZE_2MB              (0x200000)

#define INVALID_VALUE         (0xFFFFFFFF)
#define INVALID_VALUE_U16     (0xFFFF)
#define ERR_NV_LEN                    0xffff
#define ITEM_MASK_ID                  0x1fff
#define EFS_INVALID_ITEM              0xffff

//extern uint32    s_intel_m18_serial;
/*FOTA_SUPPORT_CMIOT*/
extern uint8 *fota_GetXMLfile(void);
extern int32 load_new_nvitem(uint8 *buff);

/**---------------------------------------------------------------------------*
 **                         Local variables                                   *
 **---------------------------------------------------------------------------*/
static uint32   master_address=0,slave_address=0,sector_size=0;

/**---------------------------------------------------------------------------*
 **                         Function Propertype                               *
 **---------------------------------------------------------------------------*/

uint32 get_valid_nv_address(void);

extern const NOR_FLASH_CONFIG_T s_platform_patitiion_config;
extern SPIFLASH_SPEC_T *pSpiFlashSpec[2];

//FLASH_PARTITION_PTR    s_flash_partition_info_ptr ;
uint32 g_prod_info_offset_addr = 0;

/*FOTA_SUPPORT_CMIOT*/
typedef struct
{
    uint32 app;
    uint32 offset;
    uint32 size;
}nvitem_pkg_info_t;

nvitem_pkg_info_t delta_info;


/***************************************************************************************
    APIs added by oneos team
***************************************************************************************/
/**
 * crc16 - compute the CRC-16 for the data buffer
 * @crc:        previous CRC value
 * @buffer:     data pointer
 * @len:        number of bytes in the buffer
 *
 * Returns the updated CRC value.
 */
/*lint -e765 ,"crc16" refered by fdl_update_fixnv.c */
static uint16 crc16 (uint16 crc, uint8 const *buffer, uint32 len)
{
    uint16 crc2 = 0;
    uint16 c    = 0;
    uint16 j    = 0;

    while (len--)
    {
        crc2 = 0;
        c = (crc^ (*buffer++)) &0xff;

        for (j=0; j<8; j++)
        {

            if ( (crc2 ^ c) & 0x0001)
            {
                crc2 = (crc2 >> 1) ^ 0xA001;
            }
            else
            {
                crc2 =   crc2 >> 1;
            }

            c = c >> 1;
        }

        crc = (uint16) ( (crc >> 8) ^crc2);
    }

    return crc;
}


/***************************************************************************************
    APIs end
***************************************************************************************/
uint32 Get_productinfo_offset (void)
{
    return g_prod_info_offset_addr;
}

/************************end**********************************/

void initialize_nv_status (NOR_FLASH_CONFIG_PTR ptr,uint32 base_address)
{
    uint32 efs_size;
    uint32 efs_start_address;

    if (ptr)
    {
        sector_size = ptr->sect_size ;
        master_address = ptr->fixnv_addr + base_address;
        efs_start_address = ptr->efs_start_addr + base_address;
		efs_size = ptr->sect_size* ptr->sect_num;
		slave_address = efs_start_address ;
        g_prod_info_offset_addr =  ptr->prodinfo_addr - ptr->fixnv_addr;

    }
}

uint32 calculate_fixNV_length (uint32  start_addr, uint32 end_addr)
{
    uint32  cur_addr;
    uint16  id, len;
    uint32  fix_dedicate_len;

    if ( (start_addr != master_address) && (start_addr != slave_address))
    {
        return ERR_NV_LEN;
    }

    fix_dedicate_len = FDL_FlashReadDWord (end_addr - FIX_NV_LEN_OFFSET);

    cur_addr = start_addr + sizeof (uint32); // skip update flag.

    while (cur_addr < end_addr)
    {

        id   = FDL_FlashReadWord (cur_addr);
        len = FDL_FlashReadWord (cur_addr+2);

        //FDL_SendTrace("%s %d id=0x%x, len=0x%x 0x%x %d", __func__, __LINE__, id, len, cur_addr, is_sram_fixnv);

        cur_addr += 4;

        if (EFS_INVALID_ITEM == id)  // Reach tail
        {
            cur_addr -= 4;
            break; // Stop loop
        }
        else if (id > ITEM_MASK_ID)
        {
            FDL_SendTrace ("ERROR! %s %d id=0x%x, start_addr=0x%x", __func__, __LINE__, id, start_addr);
            return ERR_NV_LEN;
        }

        if ( (len == 0x0) || (id == 0))
        {
            FDL_SendTrace ("ERROR! %s %d id=0x%x, len=0x%x, start_addr=0x%x", __func__, __LINE__, id, len, start_addr);
            return ERR_NV_LEN;
        }

        cur_addr += len + (len & 0x1); // WORD boundary
        cur_addr +=3;
        cur_addr &=0xfffffffc;
    }

    if (fix_dedicate_len != (uint32) (cur_addr - start_addr))
    {
        FDL_SendTrace ("ERROR! %s %d fix_dedicate_len=0x%x, cur_addr=0x%x, start_addr=0x%x",
                       __func__, __LINE__, fix_dedicate_len, cur_addr, start_addr);
        return ERR_NV_LEN;
    }

    //FDL_SendTrace("%s start_addr=0x%x fix_dedicate_len=0x%x", __func__, start_addr, fix_dedicate_len);
    return fix_dedicate_len;
}


#ifdef FLASH_TYPE_SPI

LOCAL uint16 fixnv_crc = INVALID_VALUE_U16;

/**********************************************************************************/
//  Description :
//
//    Global resource dependence :
//      sector_size
//  Author:
//      henry.he 2012.4.8
//    Note:
//
/**********************************************************************************/
LOCAL BOOLEAN verify_fixnv(uint32 addr)
{
    uint16 fixnv_status;
    uint16 prod_sec_dedicate_crc = 0;
    uint16 prod_sec_figure_crc = 1;
    uint16 prod_sec_figure_crc_old = 1;
    uint16 fixnv_dedicate_crc = 0;
    uint16 fixnv_figure_crc = 1;
    uint32 calc_fixnv_len;
    uint32 prod_crc_length;
    uint32 productinfo_offset;
    uint32 fixnv_length = 0;
    uint32 fixnv_address = addr;


    fixnv_status = FDL_FlashReadWord(fixnv_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET);

    if ((fixnv_status != NV_BOTH_INVALID) && (fixnv_status != NV_MOK_SINVALID)
            && (fixnv_status != NV_MOK_SOK) && (fixnv_status != NV_MINVALID_SOK)
            && (fixnv_status != NV_SOK_PRD_INFO) && (fixnv_status != NV_BOTH_INVALID_OLD)
            && (fixnv_status != NV_MINVALID_SOK_OLD) && (fixnv_status != NV_MOK_SINVALID_OLD)
            && (fixnv_status != NV_MOK_SOK_OLD) && (fixnv_status != NV_SOK_PRD_INFO_OLD)
       )
    {
        FDL_SendTrace("verify_fixnv STATUS ERR! %s %d fixnv_status=0x%x, fixnv_address=0x%x\n",
                      __func__, __LINE__, fixnv_status, fixnv_address);

        return SCI_FALSE;
    }

    if ((fixnv_address >= SRAM_START_ADDR) && fixnv_address < (SRAM_START_ADDR + SRAM_SIZE))
    {
        fixnv_length = *(uint16 *)(fixnv_address + FIXNV_SECTOR_NUM*sector_size - FIX_NV_LEN_OFFSET);
    }
    else
    {
        fixnv_length = FDL_FlashReadDWord(fixnv_address + FIXNV_SECTOR_NUM*sector_size - FIX_NV_LEN_OFFSET);
    }

    calc_fixnv_len = calculate_fixNV_length(fixnv_address, fixnv_address + FIXNV_SECTOR_NUM*sector_size);

    if ((fixnv_length > FIXNV_SECTOR_NUM*sector_size - FIX_NV_LEN_OFFSET)
            || (fixnv_length <= sizeof(uint16))
            || (fixnv_length != calc_fixnv_len)
       )
    {
        FDL_SendTrace("%s %d  LENGTH ERROR! fixnv_length=0x%x, fixnv_address=0x%x, calc_fixnv_len=0x%x\n",
                      __func__, __LINE__, fixnv_length, fixnv_address, calc_fixnv_len);

        return SCI_FALSE;
    }

    if (0/*(fixnv_address >= SRAM_START_ADDR) && fixnv_address < (SRAM_START_ADDR + SRAM_SIZE)*/)
    {
        fixnv_dedicate_crc = *(uint16 *)(fixnv_address);
    }
    else
    {
        fixnv_dedicate_crc = FDL_FlashReadWord(fixnv_address);
    }

    if (0/*(INVALID_VALUE_U16 != fixnv_crc)
            && (fixnv_address >= SRAM_START_ADDR)*/
        )
    {
        if ((fixnv_dedicate_crc != fixnv_crc)
                 && (fixnv_dedicate_crc != EndianConv_16(fixnv_crc))
            )
        {
            FDL_SendTrace("%s %d CRC ERROR!  fixnv_dedicate_crc=0x%x, fixnv_crc=0x%x, fixnv_address=0x%x\n",
                          __func__, __LINE__, fixnv_dedicate_crc, fixnv_crc, fixnv_address);

            return SCI_FALSE;
        }
    }
    else
    {
        fixnv_figure_crc = crc16(0, (uint8 const *)fixnv_address + sizeof(uint16), fixnv_length - sizeof(uint16));

        if ((fixnv_dedicate_crc != fixnv_figure_crc)
                && (fixnv_dedicate_crc != EndianConv_16(fixnv_figure_crc))
           )
        {
            FDL_SendErrLog("%s %d figure crc ERROR!  fixnv_dedicate_crc=0x%x, fixnv_figure_crc=0x%x, fixnv_address=0x%x\n",
                          __func__, __LINE__, fixnv_dedicate_crc, fixnv_figure_crc, fixnv_address);

            return SCI_FALSE;
        }
    }

    if (0/*(fixnv_address >= SRAM_START_ADDR) && fixnv_address < (SRAM_START_ADDR + SRAM_SIZE)*/)
    {
        prod_sec_dedicate_crc = *(uint16 *)(fixnv_address + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_CRC_OFFSET);
        productinfo_offset = *(uint32 *)(fixnv_address + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_OFFSET_ADDR);
    }
    else
    {
        prod_sec_dedicate_crc = FDL_FlashReadWord(fixnv_address + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_CRC_OFFSET);
        productinfo_offset = FDL_FlashReadDWord (fixnv_address + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_OFFSET_ADDR);
    }

    if (prod_sec_dedicate_crc != INVALID_VALUE_U16)
    {
        //productinfo_offset = Get_productinfo_offset();
        prod_crc_length = FIXNV_SECTOR_NUM*sector_size - productinfo_offset - FIX_NV_LEN_OFFSET;

        if ((prod_crc_length >= FIXNV_SECTOR_NUM*sector_size) || (prod_crc_length < FIX_NV_LEN_OFFSET))
        {
            FDL_SendErrLog("%s %d ERROR!  prod_crc_length=0x%x\n", __func__, __LINE__, prod_crc_length);
        }

        prod_sec_figure_crc = crc16(0, fixnv_address + productinfo_offset, prod_crc_length);

        if ((prod_sec_dedicate_crc != prod_sec_figure_crc)
                && (prod_sec_dedicate_crc != EndianConv_16(prod_sec_figure_crc))
           )
        {
            prod_sec_figure_crc_old = crc16(0, fixnv_address + sizeof(uint16), FIXNV_SECTOR_NUM*sector_size - FIX_NV_LEN_OFFSET);

            if ((prod_sec_dedicate_crc == prod_sec_figure_crc_old)
                    || (prod_sec_dedicate_crc == EndianConv_16(prod_sec_figure_crc_old))
                )
            {
                return SCI_TRUE;
            }

            FDL_SendErrLog("%s %d ERROR!  prod_sec_dedicate_crc=0x%x prod_sec_figure_crc=0x%x fixnv_address=0x%x prod_sec_figure_crc_old=0x%x\n",
                          __func__, __LINE__, prod_sec_dedicate_crc, prod_sec_figure_crc, fixnv_address, prod_sec_figure_crc_old);

            return SCI_FALSE;
        }
    }
    else
    {
        NV_STATUS_TYPE_E  fixnv_status;

        if ((fixnv_address >= SRAM_START_ADDR) && fixnv_address < (SRAM_START_ADDR + SRAM_SIZE))
        {
            fixnv_status = *(uint16 *)(fixnv_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET);
        }
        else
        {
            fixnv_status = FDL_FlashReadWord(fixnv_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET);
        }

        if (fixnv_status == NV_SOK_PRD_INFO)
        {
            FDL_SendErrLog("%s %d PROD STATUS ERROR!  fixnv_status=0x%x, fixnv_address=0x%x\n",
                          __func__, __LINE__, fixnv_status, fixnv_address);

            return SCI_FALSE;
        }
    }

    //FDL_SendTrace("fixnv_crc=0x%x, fixnv_dedicate_crc=0x%x, fixnv_figure_crc=0x%x, fixnv_address=0x%x",
    //                fixnv_crc, fixnv_dedicate_crc, fixnv_figure_crc, fixnv_address);
    FDL_SendTrace("%s addr=0x%x pass\n", __func__, addr);

    return SCI_TRUE;
}

/**********************************************************************************/
//  Description :
//
//    Global resource dependence :
//      master_address, slave_address, sector_size
//  Author:
//      henry.he 2012.4.8
//    Note:
//
/**********************************************************************************/
uint32  update_fixnv(char * fix_nv_address, uint32 fix_nv_size)
{
    uint32 explore_fixnv_addr = INVALID_ADDR;
    uint32 valide_fixnv_addr=INVALID_ADDR;
    uint32 calc_fixnv_len;
    uint32 prod_crc_length;
    uint32 productinfo_offset = 0xFFFFFFFF;
    uint32 min_productinfo_length;
    uint16 prod_sec_dedicate_crc = 0;
    uint16 prod_sec_figure_crc = 1;
    uint32 valid_prod_offset_addr = 0;
    uint16 valid_productinfo_crc = 0;
    BOOLEAN is_dload_phasecheck;
    FDL_SendTrace ("[%d]%s %d enter\n",HAL_GET_REF32K,__func__, __LINE__);

    // 这里假设上层函数改写了FIXNV 但没有改写 CRC，所以重写CRC
    // fixnv 起始地址的两字节存放 fixnv 的 CRC
    // PC tool 传来CRC 是大端，这里改成小端
    fixnv_crc = * (uint16 *) fix_nv_address ;
    fixnv_crc = EndianConv_16(fixnv_crc);
    * (uint16 *) fix_nv_address = fixnv_crc;

    * ((uint16 *) &fix_nv_address[FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET]) = NV_MINVALID_SOK;
    /* To be compatible with the latest nv tool,
    add the following code snippet, @vine 2016.10.27 */
    if ( fix_nv_size >= 0x8 &&
        *((uint32*)&fix_nv_address[fix_nv_size - 8]) == 0xFFFFFFFF &&
        *((uint32*)&fix_nv_address[fix_nv_size - 4]) == 0xFFFFFFFF )
    {
        fix_nv_size -= 0x8;
        *(uint16 *)fix_nv_address = crc16(0, fix_nv_address + sizeof(uint16),
            fix_nv_size - sizeof(uint16));
    }

    * ((uint32 *) &fix_nv_address[FIXNV_SECTOR_NUM*sector_size - FIX_NV_LEN_OFFSET]) = fix_nv_size;
    * ((uint16 *) &fix_nv_address[fix_nv_size]) = INVALID_VALUE_U16;

    productinfo_offset = Get_productinfo_offset();
    *(uint32 *)(fix_nv_address + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_OFFSET_ADDR) = productinfo_offset;
    if (1/*!is_dload_phasecheck*/)
    {
        /*nor flash, pc tool has no phase check download,
         *product info need back up, if the product info in new upgrade software is different with the current flash in phone
         *we should read out from the phone then update*/
        valide_fixnv_addr = get_valid_nv_address();
        FDL_SendTrace("%s %d valide_fixnv_addr:0x%x\n", __func__, __LINE__, valide_fixnv_addr);
        
        FDL_memset((void *)(fix_nv_address + fix_nv_size), 0xFF, FIXNV_SECTOR_NUM*sector_size - fix_nv_size - FIX_NV_LEN_OFFSET);
#if 1
        prod_crc_length = FIXNV_SECTOR_NUM*sector_size - productinfo_offset - FIX_NV_LEN_OFFSET;
        if ((prod_crc_length >= FIXNV_SECTOR_NUM*sector_size) || (prod_crc_length < FIX_NV_LEN_OFFSET))
        {
            FDL_SendTrace("%s %d  ERROR! prod_crc_length=0x%x\n", __func__, __LINE__, prod_crc_length);
        }
        prod_sec_figure_crc = crc16(0, fix_nv_address + productinfo_offset, prod_crc_length);
        *(uint16 *)(fix_nv_address + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_CRC_OFFSET) = prod_sec_figure_crc;
#else
        *(uint16 *)(fix_nv_address + sector_size - PRD_INFO_CRC_OFFSET) = INVALID_VALUE;
#endif
        if (valide_fixnv_addr !=INVALID_ADDR)
        {
            valid_productinfo_crc = FDL_FlashReadWord(valide_fixnv_addr + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_CRC_OFFSET);
            valid_prod_offset_addr = FDL_FlashReadDWord(valide_fixnv_addr + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_OFFSET_ADDR);

            if( (valid_productinfo_crc != INVALID_VALUE_U16)
                && (valid_prod_offset_addr != INVALID_VALUE)
            )
            {
                // 如果升级软件和flash里保存的valid_prod_offset_addr 不一样，productinfo 要能读写到新的地址。
                if ((uint16)productinfo_offset == valid_prod_offset_addr)
                {
                    memcpy((void *)((uint32)fix_nv_address + productinfo_offset),
                            (void *)(valide_fixnv_addr + productinfo_offset),
                            FIXNV_SECTOR_NUM*sector_size - productinfo_offset - FIX_NV_LEN_OFFSET);
                }
                else
                {


                    min_productinfo_length = MIN(FIXNV_SECTOR_NUM*sector_size - valid_prod_offset_addr, FIXNV_SECTOR_NUM*sector_size - productinfo_offset);
                    min_productinfo_length -= FIX_NV_LEN_OFFSET;

                    memcpy((void *)((uint32)fix_nv_address + productinfo_offset),
                            (void *)(valide_fixnv_addr + valid_prod_offset_addr),
                            min_productinfo_length);
                }

                prod_crc_length = FIXNV_SECTOR_NUM*sector_size - productinfo_offset - FIX_NV_LEN_OFFSET;

                if ((prod_crc_length >= FIXNV_SECTOR_NUM*sector_size) || (prod_crc_length < FIX_NV_LEN_OFFSET))
                {
                    FDL_SendTrace(" %s %d  ERROR!prod_crc_length=0x%x", __func__, __LINE__, prod_crc_length);
                }

                prod_sec_figure_crc = crc16(0, fix_nv_address + productinfo_offset, prod_crc_length);
                *(uint16 *)(fix_nv_address + FIXNV_SECTOR_NUM*sector_size - PRD_INFO_CRC_OFFSET) = prod_sec_figure_crc;
            }
        }
    }

    //FDL_SendTrace("%s %d %d", __func__, __LINE__, verify_fixnv((uint32)fix_nv_address));

    if (FDL_FlashWriteArea(slave_address, fix_nv_address, FIXNV_SECTOR_NUM*sector_size) == FIXNV_SECTOR_NUM*sector_size)
    {
        if (!verify_fixnv(slave_address))
        {
            FDL_SendTrace(" %s %d verify slave ERROR!", __func__, __LINE__);
            return 0;
        }

        * ((uint16 *) &fix_nv_address[FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET]) = NV_MOK_SOK;

        if (FDL_FlashWriteArea(master_address, (char *) fix_nv_address, FIXNV_SECTOR_NUM*sector_size) == FIXNV_SECTOR_NUM*sector_size)
        {
            if (!verify_fixnv(master_address))
            {
                FDL_SendTrace("%s %d:verify  master ERROR!", __func__, __LINE__);
                return 0;
            }

            FDL_FlashWriteWord(slave_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET, NV_MOK_SINVALID);
            FDL_FlashWriteWord(master_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET, NV_MOK_SINVALID);
            FDL_FlashErase_Block(slave_address, FIXNV_SECTOR_NUM*sector_size);
            FDL_FlashWriteWord(slave_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET, SECTOR_GOOD_STATUS);

            return 1;
        }
    }

    return 0;
}

/**********************************************************************************/
//  Description :
//
//    Global resource dependence :
//      sector_size
//  Author:
//      henry.he 2012.4.8
//    Note:
//
/**********************************************************************************/
void update_sector_by_slave(void)
{
    uint16 valid_fixnv_status;
    uint32 valid_fixnv_addr = INVALID_ADDR;
    uint8 * sector_buf_ptr = (uint8 *)SRAM_START_ADDR;

    valid_fixnv_addr = get_valid_nv_address();

    FDL_SendTrace("%s %d valid_fixnv_addr=0x%x, slave_address=0x%x, master_address=0x%x\n",
                  __func__, __LINE__, valid_fixnv_addr, slave_address, master_address);

    if (INVALID_ADDR == valid_fixnv_addr)
    {
        FDL_SendTrace("I can not find valid fixnv! %s %d", __func__, __LINE__);
        return;
    }

    valid_fixnv_status = FDL_FlashReadWord(valid_fixnv_addr + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET);

    if (valid_fixnv_addr == master_address)
    {
        FDL_SendTrace("%s %d valid_fixnv_addr=0x%x", __func__, __LINE__, valid_fixnv_addr);

        if ((valid_fixnv_status == NV_SOK_PRD_INFO)
                || (valid_fixnv_status == NV_MINVALID_SOK)
                || (valid_fixnv_status == NV_MOK_SOK)
           )
        {
            FDL_FlashWriteWord(master_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET, NV_MOK_SINVALID);
        }
    }
    else    //valid nv in slave address
    {
        // 如果slave 是有效的，就拷贝slave 到master
        if ((valid_fixnv_status == NV_SOK_PRD_INFO)
                || (valid_fixnv_status == NV_MINVALID_SOK)
                || (valid_fixnv_status == NV_MOK_SOK)
                || (valid_fixnv_status == NV_MINVALID_SOK_OLD)
                || (valid_fixnv_status == NV_MOK_SINVALID_OLD)
                || (valid_fixnv_status == NV_MOK_SOK_OLD)
                || (valid_fixnv_status == NV_SOK_PRD_INFO_OLD)
           )
        {
            FDL_FlashRead(valid_fixnv_addr, sector_buf_ptr, FIXNV_SECTOR_NUM*sector_size);

            *(uint16 *)(sector_buf_ptr + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET) = NV_MOK_SOK;

            if (FDL_FlashWriteArea(master_address, (char *) sector_buf_ptr, FIXNV_SECTOR_NUM*sector_size) == FIXNV_SECTOR_NUM*sector_size)
            {
                if (!verify_fixnv(master_address))
                {
                    FDL_SendErrLog(" %s %d verify_fixnv ERROR!", __func__, __LINE__);
                }

                if ((valid_fixnv_status == NV_SOK_PRD_INFO)
                        || (valid_fixnv_status == NV_MINVALID_SOK)
                        || (valid_fixnv_status == NV_MOK_SOK)
                    )
                {
                    FDL_FlashWriteWord(valid_fixnv_addr + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET, NV_MOK_SINVALID);
                }

                FDL_FlashWriteWord(master_address + FIXNV_SECTOR_NUM*sector_size - SECTOR_STATUS_OFFSET, NV_MOK_SINVALID);
                FDL_FlashErase_Block(valid_fixnv_addr, FIXNV_SECTOR_NUM*sector_size);
            }
        }
    }

    return;
}
#if 0 /*FOTA_SUPPORT_CMIOT*/
extern uint32 flash_cs0_size;
extern uint32 flash_cs1_size;
#endif
/**********************************************************************************/
//  Description :
//
//    Global resource dependence :
//      sector_size
//  Author:
//      henry.he 2012.4.8
//    Note:
//
/**********************************************************************************/
uint32 get_valid_nv_address(void)
{
    uint32 i;
    uint32 explore_fixnv_addr = INVALID_ADDR;
    static uint32 valide_fixnv_addr = INVALID_ADDR;

    if ((valide_fixnv_addr != INVALID_ADDR)
            && (fixnv_crc == INVALID_VALUE_U16)
        )
    {
        if (verify_fixnv(valide_fixnv_addr))
        {
            return valide_fixnv_addr;
        }
    }

    if (fixnv_crc != INVALID_VALUE_U16)
    {
        //explore_fixnv_addr = s_flash_partition_info_ptr->NvAdress;
        explore_fixnv_addr = FLASH_START_ADRESS  + pSpiFlashSpec[SPIFLASH_CS0]->spiflash_cfg->fixnv_addr;

        if (verify_fixnv(explore_fixnv_addr))
        {
            valide_fixnv_addr = explore_fixnv_addr;
            return valide_fixnv_addr;
        }
        else
        {
            FDL_SendTrace("%s %d fixnv_crc=0x%x explore_fixnv_addr=0x%x\n",
                            __func__, __LINE__, fixnv_crc, explore_fixnv_addr);

            return INVALID_ADDR;
        }
    }
    if (fixnv_crc == INVALID_VALUE_U16)
    {
        //explore_fixnv_addr = s_flash_partition_info_ptr->NvAdress;
        explore_fixnv_addr = FLASH_START_ADRESS  + pSpiFlashSpec[SPIFLASH_CS0]->spiflash_cfg->fixnv_addr;

        if (verify_fixnv(explore_fixnv_addr))
        {
            valide_fixnv_addr = explore_fixnv_addr;
            return valide_fixnv_addr;
        }
    }

    //explore_fixnv_addr = s_flash_partition_info_ptr->EfsAdress;
    explore_fixnv_addr = FLASH_START_ADRESS  + pSpiFlashSpec[SPIFLASH_CS0]->spiflash_cfg->efs_start_addr;

    if (verify_fixnv(explore_fixnv_addr))
    {
        valide_fixnv_addr = explore_fixnv_addr;
        return valide_fixnv_addr;
    }
//    FDL_SendTrace("%s %d explore_fixnv_addr=0x%x valide_fixnv_addr=0x%x",
//                    __func__, __LINE__, explore_fixnv_addr, valide_fixnv_addr);

    return INVALID_ADDR;
}
#endif

/*****coming from mem_cfg.h*******/
//#define FIXNV_SECTOR_NUM            3
//#define  FLASH_SECTOR_SIZE              0x00010000   //Block size

#define     FIXNV_TOTAL_LENGTH      (FIXNV_SECTOR_NUM * FLASH_SECTOR_SIZE)  

uint8       old_nv_buf[FIXNV_TOTAL_LENGTH];
uint8       new_nv_buf[FIXNV_TOTAL_LENGTH];

void print_nv_data(char *buf)
{
    uint16 i=0,j=0;
    for(i=0;i<32;i++){
        for(j=0;j<16;j++){
        FDL_SendTrace ("%2x ", buf[j+i*16]);   
        }
        FDL_SendTrace ("\n");  
    }
}
/*
* FIXNV upgrade process
* OUTPUT:
* 0 : NV upgrade fail
* 1 : NV upgrade sucess
*/
BOOLEAN fota_do_nv_upgrade(void)
{
    uint32 i = 0;
    BOOLEAN bRet = 0;
    uint32 valid_fixnv_addr = INVALID_ADDR;
    uint32 oldNvLength = 0;
    uint32 newNvLength = 0;
    uint8 *xml_buf = NULL;
    //FDL_SendTrace ("[%d]%s %d,slave:0x%x,master:0x%x\n",HAL_GET_REF32K, __func__, __LINE__,slave_address,master_address);
    rprintf("%s %d Start\n", __func__, __LINE__);
    
    // 1.鍒濆鍖杗v flash鍒嗗尯淇℃伅
    //this function should be called after flash initiallized;refer to nor_fld/fdl_main.c
    //initialize_nv_status (pSpiFlashSpec[0]->spiflash_cfg, FLASH_START_ADRESS);
    /*
    bRet = get_flash_partition_info(s_flash_partition_info_ptr);
    if (bRet != TRUE)
    {
        FDL_SendTrace ("%s %d,get_flash_partition_info failed\n",__func__, __LINE__);
        return 0;
    }
    */
    
    /*get target nv data and size from ota package to new_nv_buf*/
    newNvLength = load_new_nvitem(new_nv_buf);
    if (newNvLength > FIXNV_TOTAL_LENGTH)  {
        rprintf ("%s %d,nv size too big(0x%x > 0x%x)\n",__func__, __LINE__, newNvLength, FIXNV_TOTAL_LENGTH);
        return 0;
    }
    else if (newNvLength == 0)  {
        rprintf("nv flash read failed\n");
        return 0;
    }
    rprintf("%s %d ===>222\n", __func__, __LINE__);
    //memcpy(new_nv_buf, buff, size);
    
    //FDL_SendTrace ("new_nv_buffer:\n");
    //print_nv_data(new_nv_buf);

    // //2.target nv --------------------to be implement!!!!!!!
    //get target nv from ota package to new_nv_buf
    //get traget nv size from ota package to newNvLength
    //get traget xml to xml_buf
    xml_buf = (uint8*)fota_GetXMLfile();
    rprintf("%s %d ===>3333 xml_buf=%x\n", __func__, __LINE__,xml_buf);

    if (!xml_buf)  {
        rprintf ("%s %d,get xml file fail\n", __func__, __LINE__);
        return 0;
    }

    // 3.read old nv and length from proper address
    valid_fixnv_addr = get_valid_nv_address();
    if(valid_fixnv_addr == INVALID_ADDR)
    {
        rprintf (" [%d]%s %d, cannot find valid nv address!\n",HAL_GET_REF32K, __func__, __LINE__);
        return 0;
    }
    
    //mdload_log (" %s %d, valid_fixnv_addr:0x%x\n", __func__, __LINE__,valid_fixnv_addr);
    oldNvLength = FDL_FlashReadDWord(valid_fixnv_addr + FIXNV_SECTOR_NUM*sector_size - FIX_NV_LEN_OFFSET);
    memcpy ( (uint8 *) &old_nv_buf[0], (char *) valid_fixnv_addr, FIXNV_SECTOR_NUM*sector_size);
    rprintf ("[%d]%s %d:,oldNvLength:%d newNvLength:%d\n",HAL_GET_REF32K,__func__, __LINE__,oldNvLength,newNvLength);

    //mdload_log ("old_nv_buf:\n");
    //print_nv_data(old_nv_buf);

    // 4.do nvmerge
    bRet = nvmerge (old_nv_buf, oldNvLength, new_nv_buf, newNvLength, xml_buf);
    if(bRet == FALSE)
    {
        rprintf ("[%d] %s %d,  nvmerge ERROR!\n", HAL_GET_REF32K,__func__, __LINE__);
        return bRet;
    }

    //5.update fixnv_crc
    *((uint16 *)&new_nv_buf[0]) = crc16(0, (uint8*)new_nv_buf+sizeof(uint16), newNvLength-sizeof(uint16));

    //6.update_fixnv,including phase check backup
    bRet = 0;
    for (i=0; i<3; i++)
    {
        bRet = update_fixnv ( (char *) new_nv_buf, newNvLength);

        if (bRet )
        {
            rprintf ("[%d]%s %d, nv upgraded success!\n",HAL_GET_REF32K, __func__, __LINE__);
            break;
        }
        else
        {
            rprintf ("[%d]%s %d, update_fixnv ERROR! ret=%d\n",HAL_GET_REF32K, __func__, __LINE__,bRet);
        }
    }

    if (i == 3)
    {
        //Write error happened
       rprintf ("[%d]%s %d mdloader_do_nv_upgrade FAILED! ret=%d\n",HAL_GET_REF32K, __func__, __LINE__, bRet);
    }
    rprintf("%s %d Over bRet=%d\n", __func__, __LINE__,bRet);
    return bRet;
}


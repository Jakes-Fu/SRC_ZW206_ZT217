/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _DRV_EMMC_IMP_H_
#define _DRV_EMMC_IMP_H_

#include "osi_compiler.h"
//#include "drv_names.h"
//#include "drv_config.h"
//#include "osi_api.h"
//#include "osi_clock.h"
//#include "hwregs.h"
#include "emmc.h"
#include "mcd_m.h"
//#include "drv_ifc.h"
//#include <assert.h>

//OSI_EXTERN_C_BEGIN

#define BIT(nr) (1UL << (nr))

#define EMMC_CMD8_ARG 0x000001AA
#define EMMC_ACMD41_ARG 0x41FF8000
#define OCR_TIMEOUT_US (1000000)
#define DEFAULT_BLOCK_LEN 512

#define DMA_GROUP_MAX 1
#define CTRL_MAX 2

#define CMD_HAVE_DATA (1 << 10)
#define TRANS_MODE_CHECK_RESP (1 << 9)
#define TRANS_MODE_MULTI_BLOCK (1 << 8)
#define TRANS_MODE_READ (1 << 7)
#define TRANS_MODE_AUTO_CMD12 (1 << 6)
#define TRANS_MODE_AUTO_CMD23 (1 << 5)
#define TRANS_MODE_AUTO_AUTO_CMD12 (1 << 4)
#define TRANS_MODE_AUTO_AUTO_CMD23 (1 << 3)
#define TRANS_MODE_BLOCK_COUNT_EN (1 << 2)
#define TRANS_MODE_DMA_EN (1 << 0)

#define CMD6_BIT_MODE_OFFSET_ACCESS 24
#define CMD6_BIT_MODE_MASK_ACCESS 0x03000000
#define CMD6_BIT_MODE_OFFSET_INDEX 16
#define CMD6_BIT_MODE_MASK_INDEX 0x00FF0000
#define CMD6_BIT_MODE_OFFSET_VALUE 8
#define CMD6_BIT_MODE_MASK_VALUE 0x0000FF00
#define CMD6_BIT_MODE_OFFSET_CMD_SET 0
#define CMD6_BIT_MODE_MASK_CMD_SET 0x00000003
#define CMD6_ACCESS_MODE_WRITE_BYTE (3 << CMD6_BIT_MODE_OFFSET_ACCESS)

#define EXT_CSD_PARTITION_CFG_INDEX 179
#define EXT_CSD_BUS_WIDTH_INDEX 183
#define EXT_CSD_HS_TIMING_INDEX 185
#define EXT_CSD_CARD_TYPE_INDEX 196

uint8_t ext_csd_buf[512] __attribute__((aligned(32))) = {0};

/* DLL_STS0 */
#define DLL_PHASE1 (1 << 20)
#define DLL_PHASE2 (1 << 19)
#define DLL_LOCKED (1 << 18)
#define DLL_ERROR (1 << 17)
#define DLL_CPST_ST (1 << 16)
#define GET_DLL_ST(x) (((x) >> 8) & 0xf)
#define SET_DLL_ST(x) ((((x)&0xf) << 8))
#define DLL_ST_SHIFT (0x8)
#define GET_DLL_CNT(x) (((x) >> 0) & 0xff)
#define SET_DLL_CNT(x) ((((x)&0xff) << 0))
#define DLL_CNT_SHIFT (0x0)

#define PAL_RSP_NO_RSP EMMC_CMD_COMPLETE, CMD_NO_RSP, 0
#define PAL_RSP_R1 EMMC_CMD_COMPLETE, CMD_RSP_R1, EMMC_RESP_ERROR | EMMC_CMD_IND_ERR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_TIMEOUT_ERR
#define PAL_RSP_R2 EMMC_CMD_COMPLETE, CMD_RSP_R2, EMMC_RESP_ERROR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_TIMEOUT_ERR
#define PAL_RSP_R3 EMMC_CMD_COMPLETE, CMD_RSP_R3, EMMC_RESP_ERROR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_TIMEOUT_ERR
#define PAL_RSP_R4 EMMC_CMD_COMPLETE, CMD_RSP_R4, EMMC_RESP_ERROR | EMMC_CMD_IND_ERR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_TIMEOUT_ERR
#define PAL_RSP_R5 EMMC_CMD_COMPLETE, CMD_RSP_R5, EMMC_RESP_ERROR | EMMC_CMD_IND_ERR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_TIMEOUT_ERR
#define PAL_RSP_R6 EMMC_CMD_COMPLETE, CMD_RSP_R6, EMMC_RESP_ERROR | EMMC_CMD_IND_ERR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_TIMEOUT_ERR
#define PAL_RSP_R7 EMMC_CMD_COMPLETE, CMD_RSP_R7, EMMC_RESP_ERROR | EMMC_CMD_IND_ERR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_TIMEOUT_ERR
#define PAL_RSP_R1B EMMC_CMD_COMPLETE, CMD_RSP_R1B, EMMC_RESP_ERROR | EMMC_CMD_IND_ERR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_TIMEOUT_ERR
#define SIG_ALL (EMMC_CMD_COMPLETE | EMMC_TR_COMPLETE | EMMC_DMA_INT | EMMC_CARD_INT | EMMC_ERR_INT)
#define ERR_ALL (EMMC_CMD_TIMEOUT_ERR | EMMC_CMD_CRC_ERROR | EMMC_CMD_END_BIT_ERR | EMMC_CMD_IND_ERR | EMMC_DATA_TIMEOUT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_END_BIT_ERR | EMMC_AUTO_CMD12_ERR | EMMC_ADMA_ERROR | EMMC_RESP_ERROR | EMMC_AXI_RESP_ERR)
#define HOST_CMD_TYPE_ABORT (3 << 22)
#define HOST_CMD_TYPE_NML (0 << 22)
#define HOST_CMD_DATA_PRE_SEL (1 << 21)
#define HOST_CMD_IND_CHK_EN (1 << 20)
#define HOST_CMD_CRC_CHK_EN (1 << 19)
#define HOST_CMD_SUB_CMD_FLG (1 << 18)
#define HOST_CMD_NO_RSP (0x00 << 16)
#define HOST_CMD_RSP_136 (0x01 << 16)
#define HOST_CMD_RSP_48 (0x02 << 16)
#define HOST_CMD_RSP_48_BUSY (0x03 << 16)

#define HOST_NO_RSP 0x0
#define HOST_R1 (HOST_CMD_RSP_48 | HOST_CMD_IND_CHK_EN | HOST_CMD_CRC_CHK_EN)
#define HOST_R2 (HOST_CMD_RSP_136 | HOST_CMD_CRC_CHK_EN)
#define HOST_R3 HOST_CMD_RSP_48
#define HOST_R4 HOST_CMD_RSP_48
#define HOST_R5 (HOST_CMD_RSP_48 | HOST_CMD_IND_CHK_EN | HOST_CMD_CRC_CHK_EN)
#define HOST_R6 (HOST_CMD_RSP_48 | HOST_CMD_IND_CHK_EN | HOST_CMD_CRC_CHK_EN)
#define HOST_R7 (HOST_CMD_RSP_48 | HOST_CMD_IND_CHK_EN | HOST_CMD_CRC_CHK_EN)
#define HOST_R1B (HOST_CMD_RSP_48_BUSY | HOST_CMD_IND_CHK_EN | HOST_CMD_CRC_CHK_EN)
#define HOST_R5B (HOST_CMD_RSP_48_BUSY | HOST_CMD_IND_CHK_EN | HOST_CMD_CRC_CHK_EN)

#define HOST_TRANS_CMD_LINE_BOOT (1 << 30)
#define HOST_TRANS_RESP_INT_DIS (1 << 8)
#define HOST_TRANS_RESP_ERR_CHK_EN (1 << 7)
#define HOST_TRANS_RESP_TYPE (1 << 6)
#define HOST_TRANS_MULT_BLK_SEL (1 << 5)
#define HOST_TRANS_DATA_DIR_SEL (1 << 4)
#define HOST_TRANS_AUTO_CMD_MASK (3 << 2)
#define HOST_TRANS_AUTO_CMD_DISABLE (0 << 2)
#define HOST_TRANS_AUTO_CMD12_EN (1 << 2)
#define HOST_TRANS_AUTO_CMD23_EN (2 << 2)
#define HOST_TRANS_AUTO_CMD_AUTO (3 << 2)
#define HOST_TRANS_BLK_CNT_EN (1 << 1)
#define HOST_TRANS_DMA_EN (1 << 0)

#define BOOT_PARTITION_EN_OFFSET 3
#define BOOT_PARTITION_EN_MASK 0x38
#define BOOT_PARTITION_EN_NONE (0 << BOOT_PARTITION_EN_OFFSET)
#define BOOT_PARTITION_EN_BOOT1 (1 << BOOT_PARTITION_EN_OFFSET)
#define BOOT_PARTITION_EN_BOOT2 (2 << BOOT_PARTITION_EN_OFFSET)
#define BOOT_PARTITION_EN_RPMB (0 << BOOT_PARTITION_EN_OFFSET)
#define BOOT_PARTITION_EN_USER (7 << BOOT_PARTITION_EN_OFFSET)

#define BOOT_PARTITION_ACCESS_OFFSET 00
#define BOOT_PARTITION_ACCESS_MASK 0x07
#define BOOT_PARTITION_ACCESS_USER 0
#define BOOT_PARTITION_ACCESS_BOOT1 1
#define BOOT_PARTITION_ACCESS_BOOT2 2
#define BOOT_PARTITION_ACCESS_RPMB 3
#define BOOT_PARTITION_ACCESS_GENERAL_P1 4
#define BOOT_PARTITION_ACCESS_GENERAL_P2 5
#define BOOT_PARTITION_ACCESS_GENERAL_P3 6
#define BOOT_PARTITION_ACCESS_GENERAL_P4 7
#define CMD_TIMEOUT_US (2000000)

typedef enum
{
    PAL_ERR_NONE = 0,
    PAL_ERR_CMD_TIMEOUT = (0x1 << 16),
    PAL_ERR_CMD_CRC = (0x1 << 17),
    PAL_ERR_CMD_END_BIT = (0x1 << 18),
    PAL_ERR_CMD_IND = (0x1 << 19),
    PAL_ERR_DATA_TIMEOUT = (0x1 << 20),
    PAL_ERR_DATA_CRC = (0x1 << 21),
    PAL_ERR_DATA_END_BIT = (0x1 << 22),
    PAL_ERR_CUR_LMT_ERR = (0x1 << 23),
    PAL_ERR_AUTO_CMD12 = (0x1 << 24),
    PAL_ERR_ADMA = (0x1 << 25),
    PAL_ERR_RSP = (0x1 << 27),
    PAL_ERR_AXI_RESP = (0x1 << 28)
} emmcSdpalError_t;

typedef struct
{
    uint32_t v[4];
} drvEmmcResp_t;

typedef struct
{
    uint8_t *buffer;
    uint32_t buff_size;
} dmaBuff_t;

typedef struct
{

    dmaBuff_t dma_buffer[1];
    uint32_t buff_cnt;
} dmaBuffInfo_t;

typedef enum
{
    PARTITION_USER,
    PARTITION_BOOT1,
    PARTITION_BOOT2,
    PARTITION_RPMB,
    PARTITION_GENERAL_P1,
    PARTITION_GENERAL_P2,
    PARTITION_GENERAL_P3,
    PARTITION_GENERAL_P4,
    PARTITION_MAX
} emmcPartition_t;

typedef struct
{
    uint16_t cur_val;
    uint16_t grp6_supprt;
    uint16_t grp5_supprt;
    uint16_t grp4_supprt;
    uint16_t grp3_supprt;
    uint16_t grp2_supprt;
    uint16_t grp1_supprt;

    uint8_t grp6_swth_rslt;
    uint8_t grp5_swth_rslt;
    uint8_t grp4_swth_rslt;
    uint8_t grp3_swth_rslt;
    uint8_t grp2_swth_rslt;
    uint8_t grp1_swth_rslt;

    uint8_t version;

    uint16_t grp6_bsy_st;
    uint16_t grp5_bsy_st;
    uint16_t grp4_bsy_st;
    uint16_t grp3_bsy_st;
    uint16_t grp2_bsy_st;
    uint16_t grp1_bsy_st;

} sdCmd6Status_t;

typedef struct
{
    uint32_t blk_size;
    uint32_t blk_cnt;
    bool blk_addr_mode;
    uint8_t *data_buf;
    uint32_t dma_addr;
    dmaBuffInfo_t *dma_buff_info;
} dataParam_t;

typedef struct
{
    uint8_t csd_structure;
    uint8_t spec_vers;
    uint8_t reserved1;
    uint8_t taac;
    uint8_t nsac;
    uint8_t tran_spd;
    uint16_t ccc;
    uint8_t read_bl_len;
    uint8_t read_bl_partial;
    uint8_t write_blk_misalign;
    uint8_t read_blk_misalign;
    uint8_t dsr_imp;
    uint8_t reserved2;
    uint16_t c_size;
    uint8_t vdd_r_curr_min;
    uint8_t vdd_r_curr_max;
    uint8_t vdd_w_curr_min;
    uint8_t vdd_w_curr_max;
    uint8_t c_size_mult;
    uint8_t erase_grp_size;
    uint8_t erase_grp_mult;
    uint8_t wp_grp_size;
    uint8_t wp_grp_enable;
    uint8_t default_ecc;
    uint8_t r2w_factor;
    uint8_t write_bl_len;
    uint8_t write_bl_partial;
    uint8_t reserved3;
    uint8_t content_prot_app;
    uint8_t file_format_grp;
    uint8_t copy;
    uint8_t perm_write_protect;
    uint8_t tmp_write_protect;
    uint8_t file_format;
    uint8_t ecc;
} emmcMmcCsd_t;

typedef struct
{
    uint8_t csd_structure;      //2 [127:126]
    uint8_t reserved1;          //6 [125:120]
    uint8_t taac;               //8 [119:112]
    uint8_t nsac;               //8 [111:104]
    uint8_t tran_spd;           //8 [103:96]
    uint16_t ccc;               //12 [95:84]
    uint8_t read_bl_len;        //4 [83:80]
    uint8_t read_bl_partial;    //1 [79:79]
    uint8_t write_blk_misalign; //1 [78]
    uint8_t read_blk_misalign;  //1 [77]
    uint8_t dsr_imp;            //1 [76]
    uint8_t reserved2;          //2 [75:74]
    uint16_t c_size;            //12 [73:62]
    uint8_t vdd_r_curr_min;     //3 [61:59]
    uint8_t vdd_r_curr_max;     //3 [58:56]
    uint8_t vdd_w_curr_min;     //3 [55:53]
    uint8_t vdd_w_curr_max;     //3 [52:50]
    uint8_t c_size_mult;        //3 [49:47]
    uint8_t erase_blk_en;       //1 [46]
    uint8_t sector_size;        //7 [45:39]
    uint8_t wp_grp_size;        //7 [38:32]
    uint8_t wp_grp_enable;      //1 [31]
    uint8_t reserved3;          //2 [30:29]
    uint8_t r2w_factor;         //3 [28:26]
    uint8_t write_bl_len;       //4 [25:22]
    uint8_t write_bl_partial;   //1 [21]
    uint8_t reserved4;          //5 [20:16]
    uint8_t file_format_grp;    //1 [15]
    uint8_t copy;               //1 [14]
    uint8_t perm_write_protect; //1 [13]
    uint8_t tmp_write_protect;  //1 [12]
    uint8_t file_format;        //2 [11:10]
    uint8_t reserved5;          //2 [9:8]
    //crc                       //7 [7:1]
    //always 1                  //1 [0]
} emmcSdCsdV1_t;

typedef struct
{
    uint8_t csd_structure;
    uint8_t reserved1;
    uint8_t taac;
    uint8_t nsac;
    uint8_t tran_spd;
    uint16_t ccc;
    uint8_t read_bl_len;
    uint8_t read_bl_partial;
    uint8_t write_blk_misalign;
    uint8_t read_blk_misalign;
    uint8_t dsr_imp;
    uint8_t reserved2;
    uint32_t c_size;
    uint8_t reserved3;
    uint8_t erase_blk_en;
    uint8_t sector_size;
    uint8_t wp_grp_size;
    uint8_t wp_grp_enable;
    uint8_t reserved4;
    uint8_t r2w_factor;
    uint8_t write_bl_len;
    uint8_t write_bl_partial;
    uint8_t reserved5;
    uint8_t file_format_grp;
    uint8_t copy;
    uint8_t perm_write_protect;
    uint8_t tmp_write_protect;
    uint8_t file_format;
    uint8_t reserved6;
} emmcSdCsdV2_t;

typedef struct
{
    uint8_t scr_structure;
    uint8_t sd_spec;
    uint8_t data_stat_after_erase;
    uint8_t sd_security;
    uint8_t sd_bus_widths;
    uint8_t sd_spec3;
    uint8_t ex_security;
    uint16_t reserved0;
    uint8_t cmd_support;
    uint32_t reserved1;
} emmcScr_t;

/**
 * emmc /sdv1/sdv2 csd
 */
typedef union {
    emmcMmcCsd_t mmc_csd;
    emmcSdCsdV1_t sd_csd10;
    MCD_CSD_T sd_csd20;
} emmcCsd_t;
/**
 * Command supported by the protocol
 */
typedef enum
{
    CMD0_GO_IDLE_STATE,
    CMD0_READ_BOOT_DATA,        //SD/MMC
    CMD1_SEND_OP_COND,          //MMC
    CMD2_ALL_SEND_CID,          //SD/MMC //3
    CMD3_SEND_RELATIVE_ADDR,    //SD/SDIO/MMC //4
    CMD3_SET_RELATIVE_ADDR,     //MMC
    CMD4_SET_DSR,               //MMC
    CMD5_SEND_OP_COND,          //SDIO  //7
    CMD5_SLEEP_AWAKE,           //MMC
    CMD6_SWITCH_FUNC_SD,        //SD //9
    CMD6_SWITCH_FUNC_MMC,       //MMC
    CMD7_SEL_DESELECT_CARD_SD,  //SD  //11
    CMD7_SEL_DESELECT_CARD_MMC, //MMC
    CMD8_SEND_IF_COND_MMC,      //MMC
    CMD8_SEND_IF_COND_SD,       //SD    //14
    CMD9_SEND_CSD,              //SD/MMC //15
    CMD10_SEND_CID,             //SD/MMC //16
    CMD11_VOLTAGE_SWITCH,       //SDIO
    CMD12_STOP_TRANSMISSION,    //SD/MMC
    CMD13_SEND_STATUS,          //SD/MMC
    CMD15_GO_INACTIVE_STATE,    //SD/SDIO/MMC
    CMD16_SET_BLOCKLEN,         //SD/MMC //21
    CMD17_READ_SINGLE_BLOCK,    //SD/MMC //22
    CMD18_READ_MULTIPLE_BLOCK,  //SD/MMC //23
    // CMD19_SEND_TUNING_BLOCK,               //SD
    // CMD20_SPEED_CLASS_CONTROL,             //SD
    // CMD21_SEND_TUNING_BLOCK,               //MMC
    CMD23_SET_BLOCK_COUNT,      //SD/MMC //24
    CMD24_WRITE_BLOCK,          //SD/MMC //25
    CMD25_WRITE_MULTIPLE_BLOCK, //SD/MMC //26
    // CMD26_PROGRAM_CID,                     //MMC
    // CMD27_PROGRAM_CSD,                     //SD/MMC
    // CMD28_SET_WRITE_PROT,                  //SD/MMC
    // CMD29_CLR_WRITE_PROT,                  //SD/MMC
    // CMD30_SEND_WRITE_PROT,                 //SD/MMC
    // CMD31_SEND_WRITE_PROT_TYPE,            //MMC
    // CMD32_ERASE_WR_BLK_START,              //SD
    // CMD33_ERASE_WR_BLK_END,                //SD
    // CMD35_ERASE_GROUP_START,               //MMC
    // CMD36_ERASE_GROUP_END,                 //MMC
    // CMD38_ERASE,                           //SD/SDIO/MMC
    // CMD39_FAST_IO,                         //MMC
    // CMD40_GO_IRQ_STATE,                    //MMC
    // CMD42_LOCK_UNLOCK_SD,                  //SD
    // CMD42_LOCK_UNLOCK_MMC,                 //MMC
    CMD52_IO_RW_DIRECT,              //SDIO
    CMD53_READ_BYTES,                //SDIO
    CMD53_READ_BLOCKS,               //SDIO
    CMD53_WRITE_BYTES,               //SDIO
    CMD53_WRITE_BLOCKS,              //SDIO
    CMD53_READ_BLOCKS_NO_TRANS_COMP, //SDIO
    CMD55_APP_CMD,                   //SD/MMC //33
    // CMD56_GEN_CMD_SD,                      //SD

    ACMD6_SET_BUS_WIDTH,           //SD //34
    ACMD13_SD_STATUS,              //SD
    ACMD22_SEND_NUM_WR_BLCOKS,     //SD
    ACMD23_SET_WR_BLK_ERASE_COUNT, //SD
    ACMD41_SD_SEND_OP_COND,        //SD //38
    ACMD42_SET_CLR_CARD_DETECT,    //SD
    ACMD51_SEND_SCR,               //SD //40

    CMD_MAX
} emmcCmd_t;

typedef enum
{
    CMD_NO_RSP = 0,
    CMD_RSP_R1,
    CMD_RSP_R2,
    CMD_RSP_R3,
    CMD_RSP_R4,
    CMD_RSP_R5,
    CMD_RSP_R6,
    CMD_RSP_R7,
    CMD_RSP_R1B,
    CMD_RSP_R5B
} emmcResp_t;

typedef enum
{
    CMD_TYPE_NORMAL = 0,
    CMD_TYPE_SUSPEND,
    CMD_TYPE_RESUME,
    CMD_TYPE_ABORT
} emmcCmdType_t;

/**
 * Support (EMMC/SD/SDIO) three types of devices
 */
typedef enum
{
    CARD_TYPE_SD = 0,
    CARD_TYPE_SDIO = 1,
    CARD_TYPE_MMC = 2,
    CARD_TYPE_UNKNOWN
} emmcCardType_t;

typedef enum
{
    CARD_CAP_UNKONWN,
    SD_CAP_1_X,
    SD_CAP_SDSC,
    SD_CAP_SDHC,
    SD_CAP_SDXC,
    MMC_CAP_STANDARD,
    MMC_CAP_HIGHCAP
} emmcCapVer_t;

/**
 * emmc 8bit; sd 4bit;sdio 4bit;
 */
typedef enum
{
    BUS_1BIT_WIDTH,
    BUS_4BIT_WIDTH,
    BUS_8BIT_WIDTH,
    BUS_4BIT_WIDTH_DDR,
    BUS_8BIT_WIDTH_DDR,
    BUS_8BIT_WIDTH_DDR_STB,
    BUS_BUS_WIDTH_MAX
} emmcDataBusWidth_t;

typedef enum
{
    LOW_SPEED,  //(25M)emmc/sdio/sd
    HIGH_SPEED, //(48M)emmc/sdio/sd
    SDR_12,     //emmc/sdio/sd
    SDR_25,     //(M)emmc/sdio/sd
    SDR_50,     //(100M)sdio/sd
    SDR_104,    //(200M)sdio/sd
    DDR_50,     //emmc/sdio/sd
    HS_200,     //emmc
    HS_400,     //emmc
    HS_401,     //emmc
    DDR_200,    //sdio/sd
    SPEED_MAX
} emmcSpeed_t;

typedef struct
{
    emmcCmd_t cmd;
    uint32_t cmd_index;
    uint32_t int_filter;
    emmcResp_t response;
    uint32_t err_filter;
    uint32_t trans_mode;
    uint32_t cmd_type;
} emmcCmdInfo_t;

typedef struct
{
    uint8_t mid;
    uint16_t oid;
    uint8_t pnm[5];
    uint8_t prv;
    uint32_t psn;
    uint16_t mdt;
} sdCid_t;

typedef struct
{
    uint8_t mid;
    uint16_t oid;
    uint8_t pnm[6];
    uint8_t prv;
    uint32_t psn;
    uint8_t mdt;
} mmcCid_t;

typedef union {
    sdCid_t sd_cid;
    mmcCid_t mmc_cid;
} emmcCid_t;

typedef enum
{
    CARD_SPEC_UNKONWN,
    SD_SPEC_1_0,
    SD_SPEC_1_1,
    SD_SPEC_2_0,
    SD_SPEC_3_0,
    MMC_SPEC_4_0,
    MMC_SPEC_4_1,
    MMC_SPEC_4_2,
    MMC_SPEC_4_3,
    MMC_SPEC_4_4_1,
    MMC_SPEC_4_5,
    MMC_SPEC_5_0,
    MMC_SPEC_5_1,
    SDIO_SPEC_1_0,
    SDIO_SPEC_1_1,
    SDIO_SPEC_1_2,
    SDIO_SPEC_2_0,
    SDIO_SPEC_3_0,
    SDIO_SPEC_4_0
} emmcCardSpecVer_t;

typedef enum
{
    SDMA,
    ADMA2,
    ADMA3,
    DMA_MAX
} emmcDmaType_t;

typedef enum
{
    ADMA2_DATA_LEN_INVALID,
    ADMA2_DATA_LEN_16,
    ADMA2_DATA_LEN_26,
    ADMA2_DATA_LEN_MAX
} emmcAdma2DataLen_t;

typedef enum
{
    DMA_ADDR_LEN_32,
    DMA_ADDR_LEN_64,
    DMA_ADDR_LEN_MAX
} emmcDmaAddrLen_t;

typedef struct
{
    uint32_t neg_rd_dly;
    uint32_t pos_rd_dly;
    uint32_t cmd_rd_dly;
    uint32_t data_wr_dly;
} emmcDly_t;

typedef struct
{
    uint32_t neg_rd_dly_inv;
    uint32_t neg_rd_dly_offset;
    uint32_t pos_rd_dly_inv;
    uint32_t pos_rd_dly_offset;
    uint32_t cmd_rd_dly_inv;
    uint32_t cmd_rd_dly_offset;
    uint32_t data_wr_dly_inv;
    uint32_t data_wr_dly_offset;
} emmcDlyOffset_t;
emmcDly_t HOST_delay[CTRL_MAX][SPEED_MAX] =
    {
        {
            /*sd*/
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}, //SDR104
            {0, 0, 0, 0}, /*ddr50*/
            {0, 0, 0, 0}, //DDR200
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        },

        {
            /*emmc*/
            {0, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0x44, 0x44, 0x4a, 0x7b}, /*ddr50*/
            {0x96, 0x96, 0x91, 0x7f}, //HS200
            {0, 0, 0, 0xf0},
            {0, 0, 0, 0xf0},
        }};

emmcDlyOffset_t HOST_delay_offset[CTRL_MAX][SPEED_MAX] =
    {

        {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0}, /*ddr50*/
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
        },
        {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0}, /*ddr50*/
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
        }};

typedef enum
{
    CLK_400_K = 0,
    CLK_25_M,
    CLK_50_M,
    CLK_100_M,
    CLK_200_M,
    CLK_MAX
} emmcClkValue_t;

struct drvEmmc
{
    //uint32_t name;
    HWP_EMMC_T *hwp;
    //osiMutex_t *lock;
    uint32_t irqn;
    //osiPmSource_t *pm_source;

    uint32_t block_count;
    drvEmmcResp_t resp;
    emmcCid_t cid;
    bool open_flag;
    uint32_t flag;
    emmcCardType_t card_type;
    emmcCapVer_t cap_ver;
    uint16_t rca;
    emmcDataBusWidth_t bus_width;
    uint32_t blk_len;
    uint32_t blk_len_fn0;
    emmcSpeed_t spd_mode;
    uint32_t grp_size;
    uint32_t capacity;  //KB
    uint32_t rpmb_capacity;
    uint32_t boot1_capacity;
    uint32_t boot2_capacity;
    emmcPartition_t cur_partition;
    uint32_t spprt_spd_mode; //supported speed mode
    bool if_spprt_cmd23;
    bool if_use_cmd23;
    bool support_1_8;
    bool if_switch_1_8;
    bool if_spprt_auto_stop;
    uint16_t ccc;
    uint32_t user_capacity;
    uint32_t ext_csd_cache_size;
    uint32_t ext_csd_cache_ctrl_enable;
    uint32_t max_blk_len;
    emmcCsd_t csd;
    //osiClockConstrainRegistry_t clk_constrain;
    volatile uint32_t card_event;
    uint32_t err_code;
    uint32_t err_filter;
    uint32_t io_cnt;
    uint32_t buff_cnt;
    emmcDmaType_t dma_sel;
    emmcDmaAddrLen_t dma_addr_len;
    emmcAdma2DataLen_t adma2_data_len;
    emmcCardSpecVer_t spec_ver;
    struct
    {
        unsigned emmc_trans_speed;
    } pm_ctx;
};

OSI_EXTERN_C_END
#endif
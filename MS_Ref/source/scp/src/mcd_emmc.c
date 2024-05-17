/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

//#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('E', 'M', 'M', 'C')
// #define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_DEBUG
//#include "drv_emmc.h"
#include "drv_emmc_imp.h"
//#include "hal_adi_bus.h"
//#include "drv_gpio.h"
//#include "osi_log.h"
//#include "hal_chip.h"
#include "os_api.h"
#include "timer_drvapi.h"
#include "stdlib.h"
#include "stdio.h"
#include "aon_clk.h"

//MCD_STATUS_T      g_mcdStatus = MCD_STATUS_NOTOPEN_PRESENT;
BOOLEAN              g_mcdCardIsSdhc = FALSE;
BOOLEAN              g_mcdCardIsBusy = FALSE;
//BOOLEAN              g_mcdCardIsError = FALSE;
SCI_SEMAPHORE_PTR   s_emmc_sm_ptr = PNULL;

typedef struct drvEmmc drvEmmc_t;

drvEmmc_t gDrEmmc;
MCD_HANDLE_CDS_T g_handleCsd;

extern PUBLIC void SCI_SDIO_EnableDeepSleep (uint32 mode);

#define EMMC_LOG_TRACE(...) SCI_TraceLow(__VA_ARGS__)
#define EMMC_ERR_TRACE(...) SCI_TraceLow(__VA_ARGS__)


emmcCmdInfo_t cmd_detail[] =
    {
        //#define CMDname   , cmdindex  ,data int filter    +    (cmd int filter+)rsp(+cmd error filter)    +    ,data error filter   ,transmode
        {CMD0_GO_IDLE_STATE, 0, 0 | PAL_RSP_NO_RSP | 0, 0, CMD_TYPE_NORMAL},
        {CMD0_READ_BOOT_DATA, 0, EMMC_TR_COMPLETE | EMMC_CMD_COMPLETE, CMD_NO_RSP, EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_MULTI_BLOCK | TRANS_MODE_READ | TRANS_MODE_BLOCK_COUNT_EN | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD1_SEND_OP_COND, 1, PAL_RSP_R3, 0, CMD_TYPE_NORMAL},
        {CMD2_ALL_SEND_CID, 2, PAL_RSP_R2, 0, CMD_TYPE_NORMAL},
        {CMD3_SEND_RELATIVE_ADDR, 3, PAL_RSP_R6, 0, CMD_TYPE_NORMAL},
        {CMD3_SET_RELATIVE_ADDR, 3, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {CMD4_SET_DSR, 4, PAL_RSP_NO_RSP, 0, CMD_TYPE_NORMAL},
        {CMD5_SEND_OP_COND, 5, PAL_RSP_R4, 0, CMD_TYPE_NORMAL},
        {CMD5_SLEEP_AWAKE, 5, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {CMD6_SWITCH_FUNC_SD, 6, EMMC_TR_COMPLETE | PAL_RSP_R1 | EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_READ | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD6_SWITCH_FUNC_MMC, 6, EMMC_TR_COMPLETE | PAL_RSP_R1B, 0, CMD_TYPE_NORMAL},
        {CMD7_SEL_DESELECT_CARD_SD, 7, PAL_RSP_R1B, 0, CMD_TYPE_NORMAL},
        {CMD7_SEL_DESELECT_CARD_MMC, 7, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {CMD8_SEND_IF_COND_MMC, 8, EMMC_TR_COMPLETE | PAL_RSP_R1 | EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_READ | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD8_SEND_IF_COND_SD, 8, PAL_RSP_R7, 0, CMD_TYPE_NORMAL},
        {CMD9_SEND_CSD, 9, PAL_RSP_R2, 0, CMD_TYPE_NORMAL},
        {CMD10_SEND_CID, 10, PAL_RSP_R2, 0, CMD_TYPE_NORMAL},
        {CMD11_VOLTAGE_SWITCH, 11, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {CMD12_STOP_TRANSMISSION, 12, EMMC_TR_COMPLETE | PAL_RSP_R1B, 0, CMD_TYPE_NORMAL},
        {CMD13_SEND_STATUS, 13, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {CMD15_GO_INACTIVE_STATE, 15, PAL_RSP_NO_RSP, 0, CMD_TYPE_NORMAL},
        {CMD16_SET_BLOCKLEN, 16, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {CMD17_READ_SINGLE_BLOCK, 17, EMMC_TR_COMPLETE | PAL_RSP_R1 | EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_READ | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD18_READ_MULTIPLE_BLOCK, 18, EMMC_TR_COMPLETE | PAL_RSP_R1 | EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_MULTI_BLOCK | TRANS_MODE_READ | TRANS_MODE_BLOCK_COUNT_EN | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD23_SET_BLOCK_COUNT, 23, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {CMD24_WRITE_BLOCK, 24, EMMC_TR_COMPLETE | PAL_RSP_R1 | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD25_WRITE_MULTIPLE_BLOCK, 25, EMMC_TR_COMPLETE | PAL_RSP_R1 | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_MULTI_BLOCK | TRANS_MODE_BLOCK_COUNT_EN | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD52_IO_RW_DIRECT, 52, PAL_RSP_R5, 0, CMD_TYPE_NORMAL},
        {CMD53_READ_BYTES, 53, EMMC_TR_COMPLETE | PAL_RSP_R5 | EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_READ | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD53_READ_BLOCKS, 53, EMMC_TR_COMPLETE | PAL_RSP_R5 | EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_MULTI_BLOCK | TRANS_MODE_READ | TRANS_MODE_BLOCK_COUNT_EN | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD53_WRITE_BYTES, 53, EMMC_TR_COMPLETE | PAL_RSP_R5 | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD53_WRITE_BLOCKS, 53, EMMC_TR_COMPLETE | PAL_RSP_R5 | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_MULTI_BLOCK | TRANS_MODE_BLOCK_COUNT_EN | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},
        {CMD53_READ_BLOCKS_NO_TRANS_COMP, 53, PAL_RSP_R5 | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_MULTI_BLOCK | TRANS_MODE_BLOCK_COUNT_EN | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},

        {CMD55_APP_CMD, 55, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {ACMD6_SET_BUS_WIDTH, 6, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {ACMD13_SD_STATUS, 13, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {ACMD22_SEND_NUM_WR_BLCOKS, 22, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {ACMD23_SET_WR_BLK_ERASE_COUNT, 23, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {ACMD41_SD_SEND_OP_COND, 41, PAL_RSP_R3, 0, CMD_TYPE_NORMAL},
        {ACMD42_SET_CLR_CARD_DETECT, 42, PAL_RSP_R1, 0, CMD_TYPE_NORMAL},
        {ACMD51_SEND_SCR, 51, EMMC_TR_COMPLETE | PAL_RSP_R1 | EMMC_DATA_END_BIT_ERR | EMMC_DATA_CRC_ERR | EMMC_DATA_TIMEOUT_ERR, TRANS_MODE_READ | TRANS_MODE_DMA_EN | CMD_HAVE_DATA, CMD_TYPE_NORMAL},

        {CMD_MAX, 100, PAL_RSP_NO_RSP, 0, CMD_TYPE_NORMAL}};

#define  REGT_FIELD_CHANGE(reg, type, f1, v1)\
    {\
    type _val;\
    _val.v = reg;\
    _val.b.f1 = v1;\
    reg = _val.v;\
    _val.v;\
  }

void mcd_delay_sd1(uint32 ms)
{
    SCI_Sleep(ms);   //tktnum is tick(16384)
}

void mcd_TickDelay(uint32 ticks)
{
    uint32 time_1 = 0, time_2 = 0;
    uint32 delay_count = ticks;
    //delay_count = (ticks * 16384) / 1000;
    time_1 = timer_TimRealTickGet(OS_TIMER);
    time_2 = time_1 + delay_count;
    if (time_2 < time_1)
    {
        delay_count -= ((0xFFFFFFFF - time_1) + 1);
        while(timer_TimRealTickGet(OS_TIMER) >= time_1);
        while(delay_count > timer_TimRealTickGet(OS_TIMER));
    }
    else
    {
        while(time_2 >= timer_TimRealTickGet(OS_TIMER));
    }

}

#define PWR_WR_PROT_MAGIC (0x6e7f)//0x6e7f is used to unlock write protect bit status

static void _prvUnlockPowerReg(void)
{

    ANA_REG_SET(ANA_PWR_WR_PROT_VALUE, PWR_WR_PROT_MAGIC);
    do
    {
        while (!(ANA_REG_GET(ANA_PWR_WR_PROT_VALUE) == 0x8000)){
            SCI_TraceLow("PWR_WR_PROT is on write protect bit status");
        }
    } while (0);

}

static void prvEmmcClkDiv(drvEmmc_t *d, uint32_t div_num)
{
    uint32_t clk_ctrl1 = d->hwp->clk_ctrl;
    clk_ctrl1 &= ~(0xff << 8);
    clk_ctrl1 |= (div_num & 0xff) << 8;
    clk_ctrl1 &= ~(0x3 << 6);
    clk_ctrl1 |= (div_num >> 8) << 6;
    d->hwp->clk_ctrl = clk_ctrl1;
}

/**
 * Set hardware clock registers
 */
static void prvEmmcSetClk(drvEmmc_t *d, emmcClkValue_t work_clk)
{
    //close sd clk and close inter clk
    d->hwp->clk_ctrl &= ~EMMC_SDCLK_EN;
    d->hwp->clk_ctrl &= ~EMMC_INT_CLK_EN;

    switch (work_clk)
    {
    case CLK_400_K:
        prvEmmcClkDiv(d, 0xfb);
        break;

    case CLK_25_M:
        prvEmmcClkDiv(d, 4);
        break;

    case CLK_50_M:
        prvEmmcClkDiv(d, 2);
        break;

    case CLK_100_M:
        prvEmmcClkDiv(d, 1);
        break;

    case CLK_200_M:
        prvEmmcClkDiv(d, 0);
        break;

    default:
        break;
    }
    //open inter clk and open  sd clk
    d->hwp->clk_ctrl |= EMMC_INT_CLK_EN;
    while (0 == (d->hwp->clk_ctrl & EMMC_INT_CLK_EN))
    {
    };
    d->hwp->clk_ctrl |= EMMC_SDCLK_EN;

}
/**
 * set dma_type/dma_addr_len/adma2_data_length
 */
static void prvEmmcDmaSet(drvEmmc_t *d)
{
    // set_dma_type
    d->hwp->host_ctrl1 &= ~(EMMC_DMA_SEL(3));
    d->hwp->host_ctrl1 |= EMMC_DMA_SEL(d->dma_sel);
    // set_dma_addr_len
    if (DMA_ADDR_LEN_32 == d->dma_addr_len)
        d->hwp->host_ctrl2 &= ~EMMC_ADDR_64BIT_EN;
    else if (DMA_ADDR_LEN_64 == d->dma_addr_len)
        d->hwp->host_ctrl2 |= EMMC_ADDR_64BIT_EN;
    // set_adma2_data_lenght
    if (ADMA2_DATA_LEN_16 == d->adma2_data_len)
        d->hwp->host_ctrl2 &= ~EMMC_ADMA2_LEN_MODE;
    else if (ADMA2_DATA_LEN_26 == d->adma2_data_len)
        d->hwp->host_ctrl2 |= EMMC_ADMA2_LEN_MODE;
}
/**
 * set host_delay line
 */
static bool prvEmmcSetDelayLine(drvEmmc_t *d)
{
    uint32_t sd_index = 0;
    uint32_t dly = 0;
    uint32_t dly_offset = 0;

    switch (d->card_type)
    {
    case CARD_TYPE_MMC:
        sd_index = 1;
        break;

    case CARD_TYPE_SD:
        sd_index = 0;
        break;

    default:
        break;
    }

    dly = ((HOST_delay[sd_index][d->spd_mode].neg_rd_dly << 24) | (HOST_delay[sd_index][d->spd_mode].pos_rd_dly << 16) | (HOST_delay[sd_index][d->spd_mode].cmd_rd_dly << 8) | HOST_delay[sd_index][d->spd_mode].data_wr_dly);
    dly_offset = ((HOST_delay_offset[sd_index][d->spd_mode].neg_rd_dly_inv << 29) | (HOST_delay_offset[sd_index][d->spd_mode].neg_rd_dly_offset << 24) | (HOST_delay_offset[sd_index][d->spd_mode].pos_rd_dly_inv << 21) | (HOST_delay_offset[sd_index][d->spd_mode].pos_rd_dly_offset << 16) | (HOST_delay_offset[sd_index][d->spd_mode].cmd_rd_dly_inv << 13) | (HOST_delay_offset[sd_index][d->spd_mode].cmd_rd_dly_offset << 8) | (HOST_delay_offset[sd_index][d->spd_mode].data_wr_dly_inv << 5) | HOST_delay_offset[sd_index][d->spd_mode].data_wr_dly_offset);

    d->hwp->dll_dly = dly;
    d->hwp->dll_dly_offset = dly_offset;
    return true;
}

/**
 * prvEnDllCfg
 */
static void prvEmmcEnableDllCfg(drvEmmc_t *d, uint32_t bit, bool en)
{
    if (en)
        d->hwp->dll_cfg |= bit;
    else
        d->hwp->dll_cfg &= ~(bit);
}

static uint32_t prvEmmcGetDllCount(drvEmmc_t *d)
{
    uint32_t dll_counter;

    prvEmmcEnableDllCfg(d, 1, false);
    prvEmmcEnableDllCfg(d, BIT(21), false);
    if (d->spd_mode == DDR_50)
        prvEmmcEnableDllCfg(d, BIT(22), true);

    prvEmmcEnableDllCfg(d, BIT(27) | BIT(26) | BIT(25) | BIT(24), true);
    prvEmmcEnableDllCfg(d, BIT(18), true);
    prvEmmcEnableDllCfg(d, BIT(31) | BIT(30) | BIT(29) | BIT(28), true);
    prvEmmcEnableDllCfg(d, BIT(16), true);
    prvEmmcEnableDllCfg(d, 0x2 << 4, true);
    prvEmmcEnableDllCfg(d, 0x2, true);
    prvEmmcEnableDllCfg(d, 0xd << 8, true);
    prvEmmcEnableDllCfg(d, BIT(21), true);
    // get dll sts0
    while (!(DLL_LOCKED & d->hwp->dll_sts0))
        ; //wait dll locked
    while (DLL_ERROR & d->hwp->dll_sts0)
    {
        prvEmmcEnableDllCfg(d, BIT(20), true);
        mcd_delay_sd1(1);
        prvEmmcEnableDllCfg(d, BIT(20), false);

    } // make sure there is no dll error
    mcd_delay_sd1(1);
    dll_counter = (d->hwp->dll_sts0);
    dll_counter = GET_DLL_CNT(dll_counter);

    return dll_counter;
}

//
static void prvEmmcGetResp(drvEmmc_t *d, emmcResp_t rsp_type, uint8_t *rsp_buf)
{
    d->resp.v[0] = d->hwp->resp0;
    d->resp.v[1] = d->hwp->resp1;
    d->resp.v[2] = d->hwp->resp2;
    d->resp.v[3] = d->hwp->resp3;

    switch (rsp_type)
    {
    case CMD_NO_RSP:
        break;
    case CMD_RSP_R1:
    case CMD_RSP_R1B:
    case CMD_RSP_R3:
    case CMD_RSP_R4:
    case CMD_RSP_R5:
    case CMD_RSP_R6:
    case CMD_RSP_R7:
    case CMD_RSP_R5B:
        rsp_buf[0] = (uint8_t)((d->resp.v[0] >> 24) & 0xFF);
        rsp_buf[1] = (uint8_t)((d->resp.v[0] >> 16) & 0xFF);
        rsp_buf[2] = (uint8_t)((d->resp.v[0] >> 8) & 0xFF);
        rsp_buf[3] = (uint8_t)(d->resp.v[0] & 0xFF);
        break;

    case CMD_RSP_R2:
        rsp_buf[0] = (uint8_t)((d->resp.v[3] >> 16) & 0xFF);
        rsp_buf[1] = (uint8_t)((d->resp.v[3] >> 8) & 0xFF);
        rsp_buf[2] = (uint8_t)(d->resp.v[3] & 0xFF);

        rsp_buf[3] = (uint8_t)((d->resp.v[2] >> 24) & 0xFF);
        rsp_buf[4] = (uint8_t)((d->resp.v[2] >> 16) & 0xFF);
        rsp_buf[5] = (uint8_t)((d->resp.v[2] >> 8) & 0xFF);
        rsp_buf[6] = (uint8_t)(d->resp.v[2] & 0xFF);

        rsp_buf[7] = (uint8_t)((d->resp.v[1] >> 24) & 0xFF);
        rsp_buf[8] = (uint8_t)((d->resp.v[1] >> 16) & 0xFF);
        rsp_buf[9] = (uint8_t)((d->resp.v[1] >> 8) & 0xFF);
        rsp_buf[10] = (uint8_t)(d->resp.v[1] & 0xFF);

        rsp_buf[11] = (uint8_t)((d->resp.v[0] >> 24) & 0xFF);
        rsp_buf[12] = (uint8_t)((d->resp.v[0] >> 16) & 0xFF);
        rsp_buf[13] = (uint8_t)((d->resp.v[0] >> 8) & 0xFF);
        rsp_buf[14] = (uint8_t)(d->resp.v[0] & 0xFF);
        break;

    default:
        break;
    }
}
/**
 * set register Tranmode
 */
static void prvEmmcSetTranMode(drvEmmc_t *d, uint32_t cmd_index, uint32_t trans_mode, emmcCmdType_t cmd_type, emmcResp_t rsp_type)
{
    uint32_t tmp_reg = d->hwp->tr_mode;

    tmp_reg &= (~0xFFFF01FC);

    if (0 != (HOST_TRANS_CMD_LINE_BOOT & trans_mode))
        tmp_reg |= HOST_TRANS_CMD_LINE_BOOT;

    if (0 != (TRANS_MODE_MULTI_BLOCK & trans_mode))
        tmp_reg |= HOST_TRANS_MULT_BLK_SEL;
    if (0 != (TRANS_MODE_READ & trans_mode))
        tmp_reg |= HOST_TRANS_DATA_DIR_SEL;

    if (0 != (TRANS_MODE_AUTO_AUTO_CMD12 & trans_mode))
    {
        tmp_reg &= ~HOST_TRANS_AUTO_CMD_MASK;
        tmp_reg |= HOST_TRANS_AUTO_CMD_AUTO;
        d->hwp->host_ctrl2 &= (~BIT(27));
    }
    else if (0 != (TRANS_MODE_AUTO_AUTO_CMD23 & trans_mode))
    {
        tmp_reg &= ~HOST_TRANS_AUTO_CMD_MASK;
        tmp_reg |= HOST_TRANS_AUTO_CMD_AUTO;
        d->hwp->host_ctrl2 |= BIT(27);
    }
    else if (0 != (TRANS_MODE_AUTO_CMD12 & trans_mode))
    {
        tmp_reg &= ~HOST_TRANS_AUTO_CMD_MASK;
        tmp_reg |= HOST_TRANS_AUTO_CMD12_EN;
    }
    else if (0 != (TRANS_MODE_AUTO_CMD23 & trans_mode))
    {
        tmp_reg &= ~HOST_TRANS_AUTO_CMD_MASK;
        tmp_reg |= HOST_TRANS_AUTO_CMD23_EN;
    }
    else
    {
        tmp_reg &= ~HOST_TRANS_AUTO_CMD_MASK;
        d->hwp->host_ctrl2 &= (~BIT(27));
    }

    if (0 != (TRANS_MODE_BLOCK_COUNT_EN & trans_mode))
        tmp_reg |= HOST_TRANS_BLK_CNT_EN;

    if (0 != (TRANS_MODE_DMA_EN & trans_mode))
        tmp_reg |= HOST_TRANS_DMA_EN;

    if (0 != (TRANS_MODE_CHECK_RESP & trans_mode))
        tmp_reg |= HOST_TRANS_RESP_INT_DIS | HOST_TRANS_RESP_ERR_CHK_EN;

    if (0 != (CMD_HAVE_DATA & trans_mode))
        tmp_reg |= HOST_CMD_DATA_PRE_SEL;
    else
        tmp_reg |= HOST_CMD_SUB_CMD_FLG;

    switch (cmd_type)
    {
    case CMD_TYPE_NORMAL:
        tmp_reg |= HOST_CMD_TYPE_NML;
        break;

    case CMD_TYPE_ABORT:
        tmp_reg |= HOST_CMD_TYPE_ABORT;
        break;

    default:
        break;
    }

    switch (rsp_type)
    {
    case CMD_NO_RSP:
        tmp_reg |= HOST_NO_RSP;
        break;

    case CMD_RSP_R1:
        tmp_reg |= HOST_R1;
        break;

    case CMD_RSP_R2:
        tmp_reg |= HOST_R2;
        break;

    case CMD_RSP_R3:
        tmp_reg |= HOST_R3;
        break;

    case CMD_RSP_R4:
        tmp_reg |= HOST_R4;
        break;

    case CMD_RSP_R5:
        tmp_reg |= HOST_R5;
        if (0 != (TRANS_MODE_CHECK_RESP & trans_mode))
            tmp_reg |= HOST_TRANS_RESP_TYPE;
        break;

    case CMD_RSP_R6:
        tmp_reg |= HOST_R6;
        break;

    case CMD_RSP_R7:
        tmp_reg |= HOST_R7;
        break;

    case CMD_RSP_R1B:
        tmp_reg |= HOST_R1B;
        break;

    case CMD_RSP_R5B:
        tmp_reg |= HOST_R5B;
        break;

    default:
        break;
    }
    tmp_reg |= (cmd_index << 24);
    d->hwp->tr_mode = tmp_reg;
}

//set_error_filte prvEmmcSetErrorFilter
static void prvEmmcSetErrorFilter(drvEmmc_t *d, uint32_t err_msg)
{
    d->err_filter = err_msg;
}
static void init_card_event(drvEmmc_t *d)
{
    d->err_code = 0;
    d->card_event = 0;
}

// hwp timeout   save to analy driver and hwp problem
static uint32_t sprd_sdhc_calc_emmc_timeout(uint32_t clock, uint32_t timeout_value)
{
    uint32_t count = 0;
    uint32_t current_timeout = 1 << 16;
    uint32_t target_timeout;

    switch (clock)
    {
    case LOW_SPEED:
    case SDR_12:
        clock = 25000000;
        break;
    case HIGH_SPEED:
    case SDR_25:
    case DDR_50:
        clock = 50000000;
        break;
    case SDR_104:
    case HS_200:
    case HS_400:
    case HS_401:
        clock = 200000000;
        break;
    default:
        clock = 400000;
        break;
    }
    timeout_value = timeout_value > 0xb ? 0x0b : timeout_value;

    target_timeout = timeout_value * clock;

    while (target_timeout > current_timeout)
    {
        count++;
        current_timeout <<= 1;
    }

    count--;

    if (count >= 0xF)
        count = 0xE;

    return count;
}
static void sdhc_set_data_timeout(drvEmmc_t *d, uint32_t timeout_value)
{
    uint32_t tmp_reg = d->hwp->clk_ctrl;
    tmp_reg &= ~(0xF << 16);
    tmp_reg |= (timeout_value << 16);
    d->hwp->clk_ctrl = tmp_reg;
}

static void prvEmmcSetAdma2Addr(drvEmmc_t *d, uint64_t adma2_addr)
{
    d->hwp->adma2_addr_h = (uint32_t)((adma2_addr >> 32) & 0xFFFFFFFF);
    d->hwp->adma2_addr_l = (uint32_t)(adma2_addr & 0xFFFFFFFF);
}

static void prvEmmcSetAdma3Addr(drvEmmc_t *d, uint64_t adma3_addr)
{
    d->hwp->adma3_addr_h = (uint32_t)((adma3_addr >> 32) & 0xFFFFFFFF);
    d->hwp->adma3_addr_l = (uint32_t)(adma3_addr & 0xFFFFFFFF);
}
#if defined(OS_NONE)   //for nor_bootloader compiler

static void prvEmmcIsrPolling(void)
{
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;
    //get int status
    REG_EMMC_INT_ST_T cause;
    cause.v= d->hwp->int_st;
    //EMMC_LOG_TRACE( "%s interrupt cause 0x%08x",__FUNCTION__, cause.v);
    //write 1 to clean
    d->hwp->int_st = cause.v;
    //add to get sd status
    if (0 != (EMMC_ERR_INT & cause.v))
    {
        d->err_code = cause.v & (0xffff << 16);
        d->card_event |= EMMC_ERR_INT;
        EMMC_ERR_TRACE( "d->err_code %x", d->err_code);
        return;
    }
    if (0 != (EMMC_TR_COMPLETE & cause.v))
        d->card_event |= EMMC_TR_COMPLETE;
    else if (0 != (EMMC_DMA_INT & cause.v))
        d->card_event |= EMMC_DMA_INT;

    if (0 != (EMMC_ADMA3_COMPLETE & cause.v))
        d->card_event |= EMMC_ADMA3_COMPLETE;

    if (0 != (EMMC_CMD_COMPLETE & cause.v))
        d->card_event |= EMMC_CMD_COMPLETE;

    EMMC_LOG_TRACE( "card event %x", d->card_event);

}
#endif
static void prvEmmcWaitCardEvent(drvEmmc_t *d, uint32_t event_id)
{
    uint32 startTime ;
    startTime = timer_TimRealTickGet(OS_TIMER);

    do
    {
     /* Note: bootloader has no os,no isr,need adaptive to polling*/
    #if defined(OS_NONE)
        prvEmmcIsrPolling();
    #endif
        if (SCI_GetAssertFlag()/*osiIsPanic()*/)
        {
            d->card_event = d->hwp->int_st;
        }

        if (d->err_code)
            break;

        if((timer_TimRealTickGet(OS_TIMER) - startTime) > 16384*2 ) //2s
        {
           EMMC_ERR_TRACE("%s cmd timeout sts: 0x%x",__FUNCTION__, d->card_event);
           d->err_code = EMMC_CMD_TIMEOUT_ERR;
           break;
        }

    } while (event_id != ((d->card_event) & event_id));
}
/**
 * send command,get response
 */
static bool prvEmmcCommand(drvEmmc_t *d, emmcCmd_t cmd, uint32_t argument, dataParam_t *data_param, uint8_t *rsp_buf)
{
    uint32_t emmc_timeout;
    uint32_t cmd_index = cmd_detail[cmd].cmd_index;
    uint32_t trans_mode = cmd_detail[cmd].trans_mode;
    uint32_t err_filter = cmd_detail[cmd].err_filter;
    uint32_t int_filter = cmd_detail[cmd].int_filter;
    uint32_t cmd_type = cmd_detail[cmd].cmd_type;
    emmcResp_t response = cmd_detail[cmd].response;
    //EMMC_LOG_TRACE("%s enter.cmd:%d,argument:0x%x",__FUNCTION__,cmd_index,argument);

    //disable int signal/status
    d->hwp->int_sig_en &= ~SIG_ALL;
    d->hwp->int_st_en &= ~SIG_ALL;
    // clear_int_status
    d->hwp->int_st = SIG_ALL | ERR_ALL;

    emmc_timeout = sprd_sdhc_calc_emmc_timeout(d->spd_mode, 0x0a); // O 0x3
    sdhc_set_data_timeout(d, emmc_timeout);

    if (data_param && (ADMA2 == d->dma_sel ||
                       ADMA3 == d->dma_sel))
    {
        err_filter |= EMMC_ADMA_ERROR | EMMC_RESP_ERROR;
        trans_mode |= TRANS_MODE_CHECK_RESP;
        int_filter &= ~EMMC_CMD_COMPLETE;
    }

    prvEmmcSetErrorFilter(d, err_filter);

    if (err_filter)
        int_filter |= EMMC_ERR_INT;
    if (data_param)
        int_filter |= EMMC_DMA_INT;
    //

    if (ADMA3 == d->dma_sel)
        int_filter |= EMMC_ADMA3_COMPLETE;

    //enable int_signal and int_status
    d->hwp->int_sig_en |= int_filter | d->err_filter;
    d->hwp->int_st_en |= int_filter | d->err_filter;
    // disable_blk_gap_int
    if (CMD53_READ_BLOCKS_NO_TRANS_COMP == cmd)
    {
        d->hwp->int_sig_en |= EMMC_CARD_INT;
        d->hwp->int_st_en |= EMMC_CARD_INT;
        // enable_blk_gap_int
        d->hwp->host_ctrl1 |= EMMC_INT_AT_BLK_GAP;
    }
    else
        // disable_blk_gap_int
        d->hwp->host_ctrl1 &= ~EMMC_INT_AT_BLK_GAP;

    init_card_event(d);
    if (data_param)
    {
        switch (d->dma_sel)
        {
        case SDMA:
        case ADMA2:
            d->hwp->blk_size = data_param->blk_size;
            d->hwp->blk_cnt = data_param->blk_cnt;
            d->hwp->argument = argument;
            prvEmmcSetAdma2Addr(d, data_param->dma_addr);
            prvEmmcSetTranMode(d, cmd_index, trans_mode, cmd_type, response);
            break;

        case ADMA3:
            // set_descriptor(d,  cmd, argument, data_param);
            // set_adma3_addr
            prvEmmcSetAdma3Addr(d, data_param->dma_addr);
            prvEmmcSetTranMode(d, cmd_index, trans_mode, cmd_type, response);
            break;

        default:
            break;
        }
    }
    else
    {
        d->hwp->argument = argument;
        prvEmmcSetTranMode(d, cmd_index, trans_mode, cmd_type, response);
    }

    mcd_delay_sd1(1);

    if (data_param && (ADMA3 == d->dma_sel))
    {
        prvEmmcWaitCardEvent(d, int_filter & (EMMC_CMD_COMPLETE | EMMC_ADMA3_COMPLETE));
    }
    else
    {
        prvEmmcWaitCardEvent(d, int_filter & (EMMC_CMD_COMPLETE | EMMC_TR_COMPLETE));
    }

    if (0 != d->err_code)
    {
        if (!((cmd == CMD5_SEND_OP_COND) && (argument == 0)))
        {
            d->io_cnt = d->hwp->blk_cnt_io;
            d->buff_cnt = d->hwp->blk_cnt_buf;
        }

        EMMC_ERR_TRACE("EMMC driver send %d argument 0x%x fail,err_code:0x%x", cmd_index, argument, d->err_code);
        return false;
    }

    // sw_reset_cmd_data
    d->hwp->clk_ctrl |= EMMC_SW_RST_CMD;
    d->hwp->clk_ctrl |= EMMC_SW_RST_DAT;
    prvEmmcGetResp(d, response, rsp_buf);

    mcd_TickDelay(1);//for bug 2562084
    //EMMC_LOG_TRACE("EMMC driver send %d argument 0x%x response 0x%x", cmd_index, argument, d->resp.v[0]);
    return true;
}

/**
 * Set hardware data width register
 */
static void prvEmmcSetDataWidth(drvEmmc_t *d, emmcDataBusWidth_t width)
{
    uint32_t tmp_reg = d->hwp->host_ctrl1;
    tmp_reg &= (~(0x1 << 5));
    tmp_reg &= (~(0x1 << 1));
    switch (width)
    {
    case BUS_1BIT_WIDTH:
        break;

    case BUS_4BIT_WIDTH:
    case BUS_4BIT_WIDTH_DDR:
        tmp_reg |= (0x1 << 1);
        break;

    case BUS_8BIT_WIDTH:
    case BUS_8BIT_WIDTH_DDR:
    case BUS_8BIT_WIDTH_DDR_STB:
        tmp_reg |= (0x1 << 5);
        break;

    default:
        break;
    }
    d->hwp->host_ctrl1 = tmp_reg;
}

/**
 * mmc switch to use cmd6
 */
static bool mmcSwitchCmd(drvEmmc_t *d, uint32_t index, uint32_t value, uint32_t acess, uint32_t cmd_set)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t argu = acess;
    argu |= index << CMD6_BIT_MODE_OFFSET_INDEX;
    argu |= value << CMD6_BIT_MODE_OFFSET_VALUE;
    argu |= cmd_set << CMD6_BIT_MODE_OFFSET_CMD_SET;

    if (rsp_buf == NULL)
        return false;

    if (!prvEmmcCommand(d, CMD6_SWITCH_FUNC_MMC, argu, NULL, rsp_buf))
        return false;
    return true;
}
static bool mmcSetBusWidth(drvEmmc_t *d)
{
    uint32_t value = 0;
    uint32_t tmp_reg = d->hwp->host_ctrl1;

    tmp_reg &= (~(0x1 << 5));
    tmp_reg &= (~(0x1 << 1));

    switch (d->bus_width)
    {
    case BUS_1BIT_WIDTH:
        value = 0;
        break;

    case BUS_4BIT_WIDTH:
        value = 1;
        tmp_reg |= (0x1 << 1);
        break;

    case BUS_8BIT_WIDTH:
        value = 2;
        tmp_reg |= (0x1 << 5);
        break;

    case BUS_4BIT_WIDTH_DDR:
        value = 5;
        tmp_reg |= (0x1 << 1);
        break;

    case BUS_8BIT_WIDTH_DDR:
        value = 6;
        tmp_reg |= (0x1 << 5);
        break;

    default:
        break;
    }
    if (!mmcSwitchCmd(d, EXT_CSD_BUS_WIDTH_INDEX, value, CMD6_ACCESS_MODE_WRITE_BYTE, 0))
        return false;
    d->hwp->host_ctrl1 = tmp_reg;
    return true;
}

static bool sdSetBusWidth(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t argu = d->rca << 16;
    if (!prvEmmcCommand(d, CMD55_APP_CMD, argu, NULL, rsp_buf))
        return false;

    switch (d->bus_width)
    {
    case BUS_1BIT_WIDTH:
        argu = 0;
        break;

    case BUS_4BIT_WIDTH:
        argu = 2;
        break;

    default:
        break;
    }

    if (!prvEmmcCommand(d, ACMD6_SET_BUS_WIDTH, argu, NULL, rsp_buf))
        return false;

    prvEmmcSetDataWidth(d, d->bus_width);
    return true;
}

bool mmcSetSpeedMode(drvEmmc_t *d)
{
    volatile uint32_t idx1 = EXT_CSD_HS_TIMING_INDEX;
    volatile uint32_t value1 = 0;
    emmcClkValue_t clk = 0;

    uint32_t tmp_reg = d->hwp->host_ctrl2;
    tmp_reg &= (~(0x7 << 16));

    switch (d->spd_mode)
    {
    case LOW_SPEED:
    case SDR_12:
        if (ext_csd_buf[196] & 1)
        {
            idx1 = EXT_CSD_HS_TIMING_INDEX;
            value1 = 0;
            clk = CLK_25_M;
        }
        break;

    case HIGH_SPEED:
    case SDR_25:
        if (ext_csd_buf[196] & (1 << 1))
        {
            idx1 = EXT_CSD_HS_TIMING_INDEX;
            value1 = 1;
            clk = CLK_50_M;
        }
        if (d->spd_mode == SDR_25)
            tmp_reg |= (0x1 << 16);
        break;

    case DDR_50:
        if ((ext_csd_buf[196] & (1 << 2)) || (ext_csd_buf[196] & (1 << 3)))
        {
            EMMC_ERR_TRACE("%s INTO 50",__FUNCTION__);
            idx1 = EXT_CSD_HS_TIMING_INDEX;
            value1 = 1;
            clk = CLK_50_M;
        }
        tmp_reg |= (0x4 << 16);
        break;

    case HS_200:
        if ((ext_csd_buf[196] & (1 << 4)) || (ext_csd_buf[196] & (1 << 5)))
        {
            EMMC_ERR_TRACE("%s INTO 200",__FUNCTION__);
            idx1 = EXT_CSD_HS_TIMING_INDEX;
            value1 = 0x12;
            clk = CLK_200_M;
        }
        tmp_reg |= (0x5 << 16);
        break;

    default:
        break;
    }

    if (!mmcSwitchCmd(d, idx1, value1, CMD6_ACCESS_MODE_WRITE_BYTE, 0))
        return false;
    //set speed mode
    d->hwp->host_ctrl2 = tmp_reg;
    d->hwp->clk_ctrl &= ~EMMC_SDCLK_EN;
    prvEmmcSetClk(d, clk);
    d->hwp->clk_ctrl |= EMMC_SDCLK_EN;
    if (d->spd_mode == HS_200 || d->spd_mode == DDR_50)
        prvEmmcGetDllCount(d);
    else
        d->hwp->dll_cfg &= ~(1 << 21);

    prvEmmcSetDelayLine(d);
    return true;
}
/**
 * Set hardware speed mode register
 */
static void prvEmmcSetSpeedMode(drvEmmc_t *d, emmcSpeed_t speed_mode)
{
    uint32_t tmp_reg = d->hwp->host_ctrl2;
    tmp_reg &= (~(0x7 << 16));

    switch (speed_mode)
    {
    case LOW_SPEED:
    case HIGH_SPEED:
    case SDR_12:
        break;

    case SDR_25:
        tmp_reg |= (0x1 << 16);
        break;

    case SDR_50:
        tmp_reg |= (0x2 << 16);
        break;

    case SDR_104:
        tmp_reg |= (0x3 << 16);
        break;

    case DDR_50:
        tmp_reg |= (0x4 << 16);
        break;

    case HS_200:
        tmp_reg |= (0x5 << 16);
        break;

    case HS_400:
        tmp_reg |= (0x6 << 16);
        break;

    case HS_401:
        tmp_reg |= (0x7 << 16);
        break;

    default:
        break;
    }
    d->hwp->host_ctrl2 = tmp_reg;
}

void prvEmmcSetDriver(uint32_t drv_1, uint32_t drv_2)
{
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_clk, REG_IOMUX_PAD_SDMMC1_CLK_T, drv, drv_1);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_cmd, REG_IOMUX_PAD_SDMMC1_CMD_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_0, REG_IOMUX_PAD_SDMMC1_DATA_0_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_1, REG_IOMUX_PAD_SDMMC1_DATA_1_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_2, REG_IOMUX_PAD_SDMMC1_DATA_2_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_3, REG_IOMUX_PAD_SDMMC1_DATA_3_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_4, REG_IOMUX_PAD_SDMMC1_DATA_4_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_5, REG_IOMUX_PAD_SDMMC1_DATA_5_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_6, REG_IOMUX_PAD_SDMMC1_DATA_6_T, drv, drv_2);
    REGT_FIELD_CHANGE(hwp_iomux->pad_sdmmc1_data_7, REG_IOMUX_PAD_SDMMC1_DATA_7_T, drv, drv_2);
}
void  emmc_EnablePower(uint8 sdId,BOOLEAN on)
{
    EMMC_ERR_TRACE("[EMMC] emmc_EnablePower on:%d",on);
    _prvUnlockPowerReg();
    mcd_delay_sd1(50);

    if(on == FALSE)
    {
        ANA_REG_OR(ANA_POWER_PD_SW0,BIT_1);
        ANA_REG_OR(ANA_SLP_LDO_PD_CTRL0,BIT_5);
    }
    else
    {
        ANA_REG_AND(ANA_POWER_PD_SW0,~BIT_1);
        ANA_REG_AND(ANA_SLP_LDO_PD_CTRL0,~BIT_5);
    }

}

/**
 * Platform power on for emmc (HAL_POWER_SD ldo_vosel_3.b.rg_ldo_mmc_vosel)
 */
static void prvEmmcPowerOn(drvEmmc_t *d)
{
    //switch emmc clock to fast clock(get apll_400M)
    REGT_FIELD_CHANGE(hwp_aonClk->cgm_sdio_2x_sel_cfg, REG_AON_CLK_CGM_SDIO_2X_SEL_CFG_T, cgm_sdio_2x_sel, 0x4);

    /* //VDDSDCORE的跳帽是否有接上？
    //for 8850 emmc power on
    drvGpioConfig_t cfg = {
        .mode = DRV_GPIO_OUTPUT,
        .out_level = 0,
    };

    //iomux keyin0 keyin1 to power (enable LDO)

    REGT_FIELD_CHANGE(hwp_iomux->keyin_1, REG_IOMUX_KEYIN_1_T, func_sel, 1);
    REGT_FIELD_CHANGE(hwp_iomux->keyin_0, REG_IOMUX_KEYIN_0_T, func_sel, 1);
    REGT_FIELD_CHANGE(hwp_iomux->keyout_4, REG_IOMUX_KEYOUT_4_T, func_sel, 1);
    drvGpio_t *gpio29 = drvGpioOpen(29, &cfg, NULL, NULL);
    drvGpio_t *gpio28 = drvGpioOpen(28, &cfg, NULL, NULL);
    drvGpio_t *gpio10 = drvGpioOpen(10, &cfg, NULL, NULL); //VDDSDCORE_EN
    drvGpioWrite(gpio29, 1);
    drvGpioWrite(gpio28, 1);
    */
    //set driver strenleh
    prvEmmcSetDriver(0x5, 0x3);
    //drvGpioWrite(gpio10, 0);
    emmc_EnablePower(MCD_CARD_ID_0, FALSE);
    //osiThreadSleepUS(10000);
    mcd_delay_sd1(10);
    //drvGpioWrite(gpio10, 1);                     //gpio10 enable VDDSDCORE_EN pin
    emmc_EnablePower(MCD_CARD_ID_0, TRUE);
    //osiThreadSleepUS(1000);
    mcd_delay_sd1(1);

    EMMC_ERR_TRACE("[EMMC] driver capacility: clk:0x%x; cmd/data:0x%x (bit22~19)",CHIP_REG_GET(0x51510564),CHIP_REG_GET(0x51510560));
}

#ifdef EMMC_SUSPEND_SUPPORT
/**
 * Suspend hook for power manegement
 */
static void prvEmmcSuspend(void *ctx, osiSuspendMode_t mode)
{
    drvEmmc_t *d = (drvEmmc_t *)ctx;

    if (!d->open_flag)
        return;
    //emmc driver div to clk
    d->pm_ctx.emmc_trans_speed = d->hwp->clk_ctrl;
}

/**
 * Resume hook for power manegement
 */
static void prvEmmcResume(void *ctx, osiSuspendMode_t mode, uint32_t source)
{
    drvEmmc_t *d = (drvEmmc_t *)ctx;

    if (!d->open_flag)
        return;

    if (source & OSI_RESUME_ABORT)
        return;

    d->hwp->clk_ctrl = d->pm_ctx.emmc_trans_speed;
    prvEmmcSetDataWidth(d, d->bus_width);
}

/**
 * PM source operations
 */
static const osiPmSourceOps_t gEmmcPmOps = {
    .suspend = prvEmmcSuspend,
    .resume = prvEmmcResume,
};
#endif
/**
 * ISR of sdmmc controller (sdhost_irq_handle)
 */
static ISR_EXE_T prvEmmcISR(uint32 irq_num)
{
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;
    //get int status
    REG_EMMC_INT_ST_T cause;
    cause.v= d->hwp->int_st;
    //CHIPDRV_DisableIRQINT(d->irqn);

    //EMMC_LOG_TRACE( "%s interrupt cause 0x%08x",__FUNCTION__, cause.v);
    //write 1 to clean
    d->hwp->int_st = cause.v;
    //add to get sd status
    if (0 != (EMMC_ERR_INT & cause.v))
    {
        d->err_code = cause.v & (0xffff << 16);
        d->card_event |= EMMC_ERR_INT;
        EMMC_ERR_TRACE( "[EMMC]d->err_code %x", d->err_code);
        return ISR_DONE;
    }
    if (0 != (EMMC_TR_COMPLETE & cause.v))
        d->card_event |= EMMC_TR_COMPLETE;
    else if (0 != (EMMC_DMA_INT & cause.v))
        d->card_event |= EMMC_DMA_INT;

    if (0 != (EMMC_ADMA3_COMPLETE & cause.v))
        d->card_event |= EMMC_ADMA3_COMPLETE;

    if (0 != (EMMC_CMD_COMPLETE & cause.v))
        d->card_event |= EMMC_CMD_COMPLETE;

    //EMMC_LOG_TRACE( "card event %x", d->card_event);

    //CHIPDRV_EnableIRQINT(d->irqn);
    return ISR_DONE;
}
/**
 * Platform power off for emmc
 */
static void prvEmmcPowerOff(drvEmmc_t *d)
{
    emmc_EnablePower(MCD_CARD_ID_0, FALSE);
    //switch emmc clock to deault clock (26M)
    REGT_FIELD_CHANGE(hwp_aonClk->cgm_sdio_2x_sel_cfg, REG_AON_CLK_CGM_SDIO_2X_SEL_CFG_T, cgm_sdio_2x_sel, 0);
}

void Emmc_Poweroff(void)
{
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;

    prvEmmcPowerOff(d);
}

void prvEmmcIomuxSwitch(void)
{
    // iomux switch to function0
    hwp_iomux->sdmmc1_clk &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_cmd &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_0 &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_1 &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_2 &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_3 &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_4 &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_5 &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_6 &= IOMUX_FUNC_SEL(0);
    hwp_iomux->sdmmc1_data_7 &= IOMUX_FUNC_SEL(0);
}

void drvEmmcCreate(void)
{
    unsigned irqn;
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;
    prvEmmcIomuxSwitch();
    _SdioReset();
	mcd_delay_sd1(1);
	_Sdio0Enable();
	mcd_delay_sd1(1);
    EMMC_LOG_TRACE("%s enter",__FUNCTION__);

    //d->name = name;
    d->hwp = hwp_emmc;
    d->irqn = TB_SDIO_INT;//SYS_IRQ_ID_EMMC
    d->open_flag = false;
    d->flag = true;
    d->blk_len = 0;
    d->blk_len_fn0 = 0;
    d->max_blk_len = 512;
    d->rca = 0x1000;
    d->bus_width = BUS_1BIT_WIDTH;
    d->spd_mode = SPEED_MAX;
    d->card_type = CARD_TYPE_UNKNOWN;
    d->cap_ver = CARD_CAP_UNKONWN;
    d->spec_ver = CARD_SPEC_UNKONWN;

    d->cur_partition = PARTITION_MAX;
    d->spprt_spd_mode = 0;
    d->if_spprt_cmd23 = false;
    d->support_1_8 = true;
    d->if_switch_1_8 = false;
    d->if_spprt_auto_stop = false;
    //d->lock = osiMutexCreate();
    //osiClockConstrainInit(&d->clk_constrain, name);
    d->card_event = 0;
    d->err_code = 0;
    d->io_cnt = 0;
    d->buff_cnt = 0;
    d->dma_sel = SDMA;
    d->dma_addr_len = DMA_ADDR_LEN_64;
    d->adma2_data_len = ADMA2_DATA_LEN_26;
    prvEmmcDmaSet(d);
    //osiIrqSetHandler(d->irqn, prvEmmcISR, d);
   #if !defined(OS_NONE)   //for nor_bootloader compiler
    ISR_RegHandler(d->irqn, prvEmmcISR);
   #endif
    //osiIrqEnable(d->irqn);

}

/**
 * Read sdcard cid
 */
static bool prvEmmcReadCid(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};

    if (rsp_buf == NULL)
        return false;
    if (!prvEmmcCommand(d, CMD2_ALL_SEND_CID, 0, NULL, rsp_buf))
        return false;
    else
        return true;
}

static void sdCsd10Analyze(uint8_t *csd_buf, emmcSdCsdV1_t *csd)
{
    uint8_t tmp8 = csd_buf[0] & 0xC0;
    uint16_t tmp16;
    csd->csd_structure = tmp8 >> 6;

    tmp8 = csd_buf[0] & 0x3F;
    csd->reserved1 = tmp8;

    tmp8 = csd_buf[1];
    csd->taac = tmp8;

    tmp8 = csd_buf[2];
    csd->nsac = tmp8;

    tmp8 = csd_buf[3];
    csd->tran_spd = tmp8;

    tmp16 = csd_buf[4];
    tmp16 = tmp16 << 4;
    tmp8 = csd_buf[5] & 0xF0;
    tmp8 = tmp8 >> 4;
    tmp16 += tmp8;
    csd->ccc = tmp16;

    tmp8 = csd_buf[5] & 0x0F;
    csd->read_bl_len = tmp8;

    tmp8 = csd_buf[6] & 0x80;
    csd->read_bl_partial = tmp8 >> 7;

    tmp8 = csd_buf[6] & 0x40;
    csd->write_blk_misalign = tmp8 >> 6;

    tmp8 = csd_buf[6] & 0x20;
    csd->read_blk_misalign = tmp8 >> 5;

    tmp8 = csd_buf[6] & 0x10;
    csd->dsr_imp = tmp8 >> 4;

    tmp8 = csd_buf[6] & 0x0C;
    csd->reserved2 = tmp8 >> 2;

    tmp16 = csd_buf[6] & 0x03;
    tmp16 = tmp16 << 8;
    tmp16 += csd_buf[7];
    tmp16 = tmp16 << 2;
    tmp8 = csd_buf[8] & 0xC0;
    tmp8 = tmp8 >> 6;
    tmp16 = tmp16 + tmp8;
    csd->c_size = tmp16;

    tmp8 = csd_buf[8] & 0x38;
    csd->vdd_r_curr_min = tmp8 >> 3;

    tmp8 = csd_buf[8] & 0x07;
    csd->vdd_r_curr_max = tmp8;

    tmp8 = csd_buf[9] & 0xE0;
    csd->vdd_w_curr_min = tmp8 >> 5;

    tmp8 = csd_buf[9] & 0x1C;
    csd->vdd_w_curr_max = tmp8 >> 2;

    tmp8 = csd_buf[9] & 0x03;
    tmp8 = tmp8 << 1;
    tmp8 = tmp8 + ((csd_buf[10] & 0x80) >> 7);
    csd->c_size_mult = tmp8;

    tmp8 = csd_buf[10] & 0x40;
    csd->erase_blk_en = tmp8 >> 6;

    tmp8 = csd_buf[10] & 0x3F;
    tmp8 = tmp8 << 1;
    tmp8 = tmp8 + ((csd_buf[11] & 0x80) >> 7);
    csd->sector_size = tmp8;

    tmp8 = csd_buf[11] & 0x7F;
    csd->wp_grp_size = tmp8;

    tmp8 = csd_buf[12] & 0x80;
    csd->wp_grp_enable = tmp8 >> 7;

    tmp8 = csd_buf[12] & 0x60;
    csd->reserved3 = tmp8 >> 5;

    tmp8 = csd_buf[12] & 0x1C;
    csd->r2w_factor = tmp8 >> 2;

    tmp8 = csd_buf[12] & 0x03;
    tmp8 = tmp8 << 2;
    tmp8 = tmp8 + ((csd_buf[13] & 0xC0) >> 6);
    csd->write_bl_len = tmp8;

    tmp8 = csd_buf[13] & 0x20;
    csd->write_bl_partial = tmp8 >> 5;

    tmp8 = csd_buf[13] & 0x1F;
    csd->reserved4 = tmp8;

    tmp8 = csd_buf[14] & 0x80;
    csd->file_format_grp = tmp8 >> 7;

    tmp8 = csd_buf[14] & 0x40;
    csd->copy = tmp8 >> 6;

    tmp8 = csd_buf[14] & 0x20;
    csd->perm_write_protect = tmp8 >> 5;

    tmp8 = csd_buf[14] & 0x10;
    csd->tmp_write_protect = tmp8 >> 4;

    tmp8 = csd_buf[14] & 0x0C;
    csd->file_format = tmp8 >> 2;
    tmp8 = csd_buf[14] & 0x03;
    csd->reserved5 = tmp8;
}

static void sdCsd20Analyze(uint8_t *csd_buf, MCD_CSD_T *csd)
{
    uint8_t tmp8 = csd_buf[0] & 0xC0;
    uint16_t tmp16;
    uint32_t tmp32;
    csd->csdStructure = tmp8 >> 6;

    tmp8 = csd_buf[0] & 0x3F;
    csd->specVers = tmp8;

    tmp8 = csd_buf[1];
    csd->taac = tmp8;

    tmp8 = csd_buf[2];
    csd->nsac = tmp8;

    tmp8 = csd_buf[3];
    csd->tranSpeed = tmp8;

    tmp16 = csd_buf[4];
    tmp16 = tmp16 << 4;
    tmp8 = csd_buf[5] & 0xF0;
    tmp8 = tmp8 >> 4;
    tmp16 += tmp8;
    csd->ccc = tmp16;

    tmp8 = csd_buf[5] & 0x0F;
    csd->readBlLen = tmp8;

    tmp8 = csd_buf[6] & 0x80;
    csd->readBlPartial = tmp8 >> 7;

    tmp8 = csd_buf[6] & 0x40;
    csd->writeBlkMisalign = tmp8 >> 6;

    tmp8 = csd_buf[6] & 0x20;
    csd->readBlkMisalign = tmp8 >> 5;

    tmp8 = csd_buf[6] & 0x10;
    csd->dsrImp = tmp8 >> 4;
/*
    tmp8 = csd_buf[6] & 0x0F;
    tmp8 = tmp8 << 2;
    tmp8 += ((csd_buf[7] & 0xC0) >> 6);
    csd->reserved2 = tmp8;
*/
    tmp32 = csd_buf[7] & 0x3F;
    tmp32 = tmp32 << 8;
    tmp32 += csd_buf[8];
    tmp32 = tmp32 << 8;
    tmp32 += csd_buf[9];
    csd->cSize = tmp32;
/*
    tmp8 = csd_buf[10] & 0x80;
    tmp8 = tmp8 >> 7;
    csd->reserved3 = tmp8;
*/
    tmp8 = csd_buf[10] & 0x40;
    csd->eraseBlkEnable = tmp8 >> 6;

    tmp8 = csd_buf[10] & 0x3F;
    tmp8 = tmp8 << 1;
    tmp8 = tmp8 + ((csd_buf[11] & 0x80) >> 7);
    csd->sectorSize = tmp8;

    tmp8 = csd_buf[11] & 0x7F;
    csd->wpGrpSize = tmp8;

    tmp8 = csd_buf[12] & 0x80;
    csd->wpGrpEnable = tmp8 >> 7;
/*
    tmp8 = csd_buf[12] & 0x60;
    csd->reserved3 = tmp8 >> 5;
*/
    tmp8 = csd_buf[12] & 0x1C;
    csd->r2wFactor = tmp8 >> 2;

    tmp8 = csd_buf[12] & 0x03;
    tmp8 = tmp8 << 2;
    tmp8 = tmp8 + ((csd_buf[13] & 0xC0) >> 6);
    csd->writeBlLen = tmp8;

    tmp8 = csd_buf[13] & 0x20;
    csd->writeBlPartial = tmp8 >> 5;
/*
    tmp8 = csd_buf[13] & 0x1F;
    csd->reserved4 = tmp8;
*/
    tmp8 = csd_buf[14] & 0x80;
    csd->fileFormatGrp = tmp8 >> 7;

    tmp8 = csd_buf[14] & 0x40;
    csd->copy = tmp8 >> 6;

    tmp8 = csd_buf[14] & 0x20;
    csd->permWriteProtect = tmp8 >> 5;

    tmp8 = csd_buf[14] & 0x10;
    csd->tmpWriteProtect = tmp8 >> 4;

    tmp8 = csd_buf[14] & 0x0C;
    csd->fileFormat = tmp8 >> 2;
/*
    tmp8 = csd_buf[14] & 0x03;
    csd->reserved5 = tmp8;
 */
    tmp8 = csd_buf[15] & 0xFE;
    csd->crc = tmp8 >> 1;

    // Other fields are undefined --> for sd v1.0
    csd->vddRCurrMin = 0;
    csd->vddRCurrMax = 0;
    csd->vddWCurrMin = 0;
    csd->vddWCurrMax = 0;
    csd->cSizeMult   = 0;
    csd->blockNumber  = (csd->cSize + 1) * 1024;

}

/**
 * Read mmc csd
 */
static bool mmcReadCsd(drvEmmc_t *d, emmcCsd_t *csd)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t argu;
    uint32_t tmp16;
    uint8_t tmp8;
    if (rsp_buf == NULL)
        return false;

    if (csd == NULL)
        return false;

    argu = d->rca << 16;
    if (!prvEmmcCommand(d, CMD9_SEND_CSD, argu, NULL, rsp_buf))
        return false;

    tmp16 = ((rsp_buf[6] & 0x03) << 8) + rsp_buf[7];
    tmp16 = tmp16 << 2;
    tmp8 = (rsp_buf[8] & 0xC0) >> 6;
    csd->mmc_csd.c_size = (tmp16 + tmp8);

    if (csd->mmc_csd.c_size >= 0xFFF)
        d->cap_ver = MMC_CAP_HIGHCAP;
    else
        d->cap_ver = MMC_CAP_STANDARD;
    return true;
}
/**
 * Read mmc ext csd
 */
static bool mmcReadExtCsd(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};
    dataParam_t data;
    uint32_t emmc_capability;
    data.blk_size = 512;
    data.blk_cnt = 1;
    data.dma_addr = (uint32_t)&ext_csd_buf;

    if (rsp_buf == NULL)
        return false;

    if (CARD_CAP_UNKONWN == d->cap_ver || MMC_CAP_STANDARD == d->cap_ver)
        data.blk_addr_mode = false;
    else
        data.blk_addr_mode = true;

    //osiDCacheInvalidate(ext_csd_buf, data.blk_size * data.blk_cnt);
    MMU_DmaCacheSync((uint32)ext_csd_buf, data.blk_size * data.blk_cnt,DMABUFFER_FROM_DEVICE);

    if (!prvEmmcCommand(d, CMD8_SEND_IF_COND_MMC, 0, &data, rsp_buf))
        return false;

    emmc_capability = ((uint32_t)(ext_csd_buf[215]) << 24) | ((uint32_t)(ext_csd_buf[214]) << 16) | ((uint32_t)(ext_csd_buf[213]) << 8) | ((uint32_t)(ext_csd_buf[212]));
    d->block_count = emmc_capability;
    emmc_capability = emmc_capability >> 21;
    EMMC_LOG_TRACE("emmc: the emmc capability is %d GB", emmc_capability);
    EMMC_LOG_TRACE("emmc: the emmc sector count is %d ", d->block_count);
    return true;
}

/**
 * Read sdcard csd
 */
static bool sdReadCsd(drvEmmc_t *d, emmcCsd_t *csd)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t argu;
    if (rsp_buf == NULL)
        return false;

    if (csd == NULL)
        return false;

    argu = (d->rca) << 16;
    if (!prvEmmcCommand(d, CMD9_SEND_CSD, argu, NULL, rsp_buf))
        return false;

    if ((rsp_buf[0] >> 6) == 0x1)  //csd structure version
    {
        sdCsd20Analyze(rsp_buf, &(csd->sd_csd20));
        d->ccc = csd->sd_csd20.ccc;

        if (csd->sd_csd20.cSize >= 0xFFFF)
            d->cap_ver = SD_CAP_SDXC;
        else
            d->cap_ver = SD_CAP_SDHC;
        d->max_blk_len = (1 << csd->sd_csd20.readBlLen); //512B
        /*memory capacity = (C_SIZE+1)*512K byte*/
        d->block_count = (csd->sd_csd20.cSize +1) * 1024;  //1block=512B
        d->capacity = (csd->sd_csd20.cSize + 1) << 9; //unit is KB

    }
    else
    {
        sdCsd10Analyze(rsp_buf, &(csd->sd_csd10));
        d->ccc = csd->sd_csd10.ccc;
        d->max_blk_len = (1 << csd->sd_csd10.read_bl_len); // max_blk_len is 10 for 2G card
        d->block_count = (csd->sd_csd10.c_size + 1) << (csd->sd_csd10.c_size_mult + 2 + csd->sd_csd10.read_bl_len - 9);//1block=512B,9 is represent 512B
        d->capacity = (csd->sd_csd10.c_size + 1) << (csd->sd_csd10.c_size_mult + 2 + csd->sd_csd10.read_bl_len - 10); //unit is KB,10 is represent 1024B
    }
    //capacity actually is block_count * 512B
    g_handleCsd.Capacity = d->block_count;
    return true;
}
static void sdScrAnalyze(uint8_t *scr_buf, emmcScr_t *scr)
{
    uint8_t tmp8 = (scr_buf[0] & 0xF0) >> 4;
    scr->scr_structure = tmp8;

    tmp8 = scr_buf[0] & 0xF;
    scr->sd_spec = tmp8;

    tmp8 = (scr_buf[1] & 0x80) >> 7;
    scr->data_stat_after_erase = tmp8;

    tmp8 = (scr_buf[1] & 0x70) >> 4;
    scr->sd_security = tmp8;

    tmp8 = scr_buf[1] & 0xF;
    scr->sd_bus_widths = tmp8;

    tmp8 = (scr_buf[2] & 0x80) >> 7;
    scr->sd_spec3 = tmp8;

    tmp8 = (scr_buf[2] & 0x78) >> 3;
    scr->ex_security = tmp8;

    tmp8 = scr_buf[3] & 0x3;
    scr->cmd_support = tmp8;
}
static bool sdReadScr(drvEmmc_t *d, emmcScr_t *scr)
{
    uint8_t rsp_buf[16] = {0};
    dataParam_t data_param;
    uint8_t buf[8] = {0};
    if (scr == NULL)
        return false;

    if (!prvEmmcCommand(d, CMD55_APP_CMD, d->rca << 16, NULL, rsp_buf))
        return false;

    if (CARD_CAP_UNKONWN == d->cap_ver || SD_CAP_SDSC == d->cap_ver)
        data_param.blk_addr_mode = false;
    else
        data_param.blk_addr_mode = true;

    data_param.blk_size = 8;
    data_param.blk_cnt = 1;
    data_param.dma_addr = (uint32_t)(&buf);
    data_param.dma_buff_info = NULL;

    //osiDCacheInvalidate(buf, data_param.blk_size * data_param.blk_cnt);
    MMU_DmaCacheSync((uint32)buf,data_param.blk_size * data_param.blk_cnt,DMABUFFER_FROM_DEVICE);

    if (!prvEmmcCommand(d, ACMD51_SEND_SCR, 0, &data_param, rsp_buf))
        return false;

    sdScrAnalyze(buf, scr);

    switch (scr->sd_spec)
    {
    case 0:
        d->spec_ver = SD_SPEC_1_0;
        break;

    case 1:
        d->spec_ver = SD_SPEC_1_1;
        break;

    case 2:
        if (scr->sd_spec3 == 1)
            d->spec_ver = SD_SPEC_3_0;
        else
            d->spec_ver = SD_SPEC_2_0;
        break;

    default:
        break;
    }
    EMMC_LOG_TRACE("Sdcard support spec ver %d", d->cap_ver);
    if (scr->cmd_support & 0x2)
    {
        d->if_spprt_cmd23 = true;
        EMMC_ERR_TRACE("This card support cmd23");
    }
    else
    {
        d->if_spprt_cmd23 = false;
        EMMC_ERR_TRACE("This card Not support cmd23");
    }

    return true;
}

static void sdCmd6Analyze(uint8_t *res_buf, sdCmd6Status_t *cmd_status)
{
    cmd_status->cur_val = ((res_buf[0] << 8) | (res_buf[1]));
    cmd_status->grp6_supprt = ((res_buf[2] << 8) | (res_buf[3]));
    cmd_status->grp5_supprt = ((res_buf[4] << 8) | (res_buf[5]));
    cmd_status->grp4_supprt = ((res_buf[6] << 8) | (res_buf[7]));
    cmd_status->grp3_supprt = ((res_buf[8] << 8) | (res_buf[9]));
    cmd_status->grp2_supprt = ((res_buf[10] << 8) | (res_buf[11]));
    cmd_status->grp1_supprt = ((res_buf[12] << 8) | (res_buf[13]));

    cmd_status->grp6_swth_rslt = ((res_buf[14] & 0xf0) >> 4);
    cmd_status->grp5_swth_rslt = (res_buf[14] & 0x0f);
    cmd_status->grp4_swth_rslt = ((res_buf[15] & 0xf0) >> 4);
    cmd_status->grp3_swth_rslt = (res_buf[15] & 0x0f);
    cmd_status->grp2_swth_rslt = ((res_buf[16] & 0xf0) >> 4);
    cmd_status->grp1_swth_rslt = (res_buf[16] & 0x0f);

    cmd_status->version = res_buf[17];

    cmd_status->grp6_bsy_st = ((res_buf[18] << 8) | (res_buf[19]));
    cmd_status->grp5_bsy_st = ((res_buf[20] << 8) | (res_buf[21]));
    cmd_status->grp4_bsy_st = ((res_buf[22] << 8) | (res_buf[23]));
    cmd_status->grp3_bsy_st = ((res_buf[24] << 8) | (res_buf[25]));
    cmd_status->grp2_bsy_st = ((res_buf[26] << 8) | (res_buf[27]));
    cmd_status->grp1_bsy_st = ((res_buf[28] << 8) | (res_buf[29]));
}

/**
 * set speed mode
 */
static bool sdSetSpeedMode(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};
    dataParam_t data_param;
    uint8_t buf[64];
    uint32_t func = 0;
    emmcClkValue_t clk;
    dmaBuffInfo_t w_dma_buff_info[DMA_GROUP_MAX];
    sdCmd6Status_t cmd_status;
    memset(w_dma_buff_info, 0, sizeof(w_dma_buff_info));

    w_dma_buff_info[0].dma_buffer[0].buffer = buf;
    w_dma_buff_info[0].dma_buffer[0].buff_size = 64;
    w_dma_buff_info[0].buff_cnt = 1;

    data_param.blk_size = 64;
    data_param.blk_cnt = 1;
    data_param.dma_addr = (uint32_t)(&buf);
    data_param.dma_buff_info = w_dma_buff_info;

    if (CARD_CAP_UNKONWN == d->cap_ver || SD_CAP_SDSC == d->cap_ver)
        data_param.blk_addr_mode = false;
    else
        data_param.blk_addr_mode = true;

    switch (d->spd_mode)
    {
        case HIGH_SPEED:
        case SDR_25:
            clk = CLK_50_M;
            func = 1;
            break;

        case SDR_50:
            func = 2;
            clk = CLK_100_M;
            break;

        case SDR_104:
            func = 3;
            clk = CLK_200_M;
            break;

        case DDR_50:
            func = 4;
            clk = CLK_50_M;
            break;

        case DDR_200:
            func = 5;
            clk = CLK_200_M;
            break;
            
        case LOW_SPEED:
        case SDR_12:
        default:
            func = 0;
            clk = CLK_25_M;
            break;
    }

    if (!prvEmmcCommand(d, CMD6_SWITCH_FUNC_SD, (0x80FFFFF0 + func), &data_param, rsp_buf))
        return false;
    sdCmd6Analyze(buf, &cmd_status);

    prvEmmcSetSpeedMode(d, d->spd_mode);

    prvEmmcSetClk(d, clk);
    if (d->spd_mode == SDR_50 || d->spd_mode == SDR_104 || d->spd_mode == DDR_200)
        prvEmmcGetDllCount(d);
    return true;
}

bool sdReadRca(drvEmmc_t *d, uint16_t *rca)
{
    uint8_t rsp_buf[16] = {0};
    uint16_t tmp_rca ;

    if (rca == NULL)
        return false;

    if (rsp_buf == NULL)
        return false;

    if (!prvEmmcCommand(d, CMD3_SEND_RELATIVE_ADDR, 0, NULL, rsp_buf))
        return false;

    tmp_rca = rsp_buf[0];
    tmp_rca = tmp_rca << 8;
    tmp_rca |= rsp_buf[1];

    *rca = tmp_rca;
    d->rca = tmp_rca;
    return true;
}

static bool mmcSetPartition(drvEmmc_t *d, emmcPartition_t card_partition)
{
    uint32_t value = 0;
    if (card_partition == d->cur_partition)
        return true;

    switch (card_partition)
    {
    case PARTITION_USER:
        value = (BOOT_PARTITION_EN_USER | BOOT_PARTITION_ACCESS_USER);
        break;
    case PARTITION_BOOT1:
        value = (BOOT_PARTITION_EN_BOOT1 | BOOT_PARTITION_ACCESS_BOOT1);
        break;
    case PARTITION_BOOT2:
        value = (BOOT_PARTITION_EN_BOOT2 | BOOT_PARTITION_ACCESS_BOOT2);
        break;
    case PARTITION_RPMB:
        value = (BOOT_PARTITION_EN_RPMB | BOOT_PARTITION_ACCESS_RPMB);
        break;
    case PARTITION_GENERAL_P1:
    case PARTITION_GENERAL_P2:
    case PARTITION_GENERAL_P3:
    case PARTITION_GENERAL_P4:
    default:
        break;
    }

    if (!mmcSwitchCmd(d, EXT_CSD_PARTITION_CFG_INDEX, value, CMD6_ACCESS_MODE_WRITE_BYTE, 1 << 0)) //CMD6_CMD_SET???
        return false;

    d->cur_partition = card_partition;
    return true;
}

static bool prvEmmcCardReady(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t argu = (d->rca) << 16;
    uint32 startTime = 0;

    if (rsp_buf == NULL)
        return false;

    startTime = timer_TimRealTickGet(OS_TIMER);

    do
    {
        if (!prvEmmcCommand(d, CMD13_SEND_STATUS, argu, NULL, rsp_buf))
            return false;

        if (0 != (rsp_buf[2] & 1))
            return true;

        if((timer_TimRealTickGet(OS_TIMER) - startTime) > 16384 ) //1s
        {
            SCI_TraceLow( "%4c CMD13 retry timeout,emmc set partion error");
            return false;
        }
    } while (1);
    return true;
}

/**
 * emmc init
 */
static bool mmcInitCard(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};
    uint32 startTime = 0;
    if (d == NULL)
        return false;

    if (rsp_buf == NULL)
        return false;

    EMMC_LOG_TRACE("%s:emmc open ...",__FUNCTION__);

    startTime = timer_TimRealTickGet(OS_TIMER);
    do
    {
        if (!prvEmmcCommand(d, CMD1_SEND_OP_COND, 0x40FF8080, NULL, rsp_buf))
            goto fail;
        if (0 != (rsp_buf[0] & (1 << 7)))
        {
            d->cap_ver = MMC_CAP_STANDARD;
            break;
        }
        if((timer_TimRealTickGet(OS_TIMER) - startTime) > 2*1000*1000 ) //1s
        {
            EMMC_ERR_TRACE("%s CMD1_SEND_OP_COND retry timeout",__FUNCTION__);
            goto fail;
        }
    } while (1);

    if (!prvEmmcReadCid(d))
        goto fail;

    if (!prvEmmcCommand(d, CMD3_SET_RELATIVE_ADDR, 1 << 16, NULL, rsp_buf))
        goto fail;
    d->rca = 1;

    if (!mmcReadCsd(d, &(d->csd)))
        goto fail;

    if (!prvEmmcCommand(d, CMD7_SEL_DESELECT_CARD_MMC, 1 << 16, NULL, rsp_buf))
        goto fail;

    if (!mmcReadExtCsd(d))
        goto fail;

    if (!mmcSwitchCmd(d, EXT_CSD_HS_TIMING_INDEX, 1, CMD6_ACCESS_MODE_WRITE_BYTE, 0))
    {
        EMMC_ERR_TRACE("emmc set high speed error");
        goto fail;
    }
    d->blk_len = 512;

    if (!prvEmmcCommand(d, CMD16_SET_BLOCKLEN, d->blk_len, NULL, rsp_buf))
        return false;

    mmcSwitchCmd(d, EXT_CSD_BUS_WIDTH_INDEX, (1 << 7), CMD6_ACCESS_MODE_WRITE_BYTE, 0);
    //emmc enable cache
    if (!mmcSwitchCmd(d, 33, 1, CMD6_ACCESS_MODE_WRITE_BYTE, 0))
        goto fail;

    d->bus_width = BUS_4BIT_WIDTH;
    d->spd_mode = HIGH_SPEED;
    d->dma_sel = SDMA;
    d->adma2_data_len = ADMA2_DATA_LEN_16;
    d->dma_addr_len = DMA_ADDR_LEN_32;

    if (d->spd_mode == HS_200)
        prvEmmcSetDriver(0x7, 0x7);

    if (!mmcSetBusWidth(d))
        goto fail;

    if (!mmcSetSpeedMode(d))
        goto fail;

    if (!mmcSetPartition(d, PARTITION_USER))
        goto fail;

    if (!prvEmmcCardReady(d))
        goto fail;

    prvEmmcDmaSet(d);

    EMMC_LOG_TRACE("%s emmc open done",__FUNCTION__);
    d->open_flag = true;
    d->card_type = CARD_TYPE_MMC;
    //osiMutexUnlock(d->lock);
    return true;

fail:
    EMMC_ERR_TRACE("%s emmc open fail",__FUNCTION__);
    d->open_flag = false;
    prvEmmcPowerOff(d);
    //osiMutexUnlock(d->lock);
    return false;
}
/**
 * sdcard init
 */
static bool sdInitCard(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};
    uint16_t rca = 0;
    //emmcCsd_t csd;
    emmcScr_t scr;
    uint32 startTime = 0;
    uint32_t argu;
    //osiElapsedTimer_t timer;
    if (d == NULL)
        return false;
    d->card_type = CARD_TYPE_SD;


    //output clk auto gate/internal clk auto gate
    d->hwp->busy_posi |= EMMC_OUTR_CLK_AUTO_EN;
    d->hwp->busy_posi |= EMMC_INNR_CLK_AUTO_EN;

    EMMC_LOG_TRACE("%s:sd open ",__FUNCTION__);

    //osiElapsedTimerStart(&timer);

    startTime = timer_TimRealTickGet(OS_TIMER);
    do
    {
        if (!prvEmmcCommand(d, CMD55_APP_CMD, 0, NULL, rsp_buf))
            goto fail;
        if (!prvEmmcCommand(d, ACMD41_SD_SEND_OP_COND, EMMC_ACMD41_ARG, NULL, rsp_buf))
            goto fail;
        if (0 != (rsp_buf[0] & (1 << 7)))
        {
            if (0 != (rsp_buf[0] & (1 << 6)))
            {
                d->cap_ver = SD_CAP_SDHC;
                if ((0 == (rsp_buf[0] & 1)) && d->support_1_8)
                {
                    //OSI_LOGI(0x10009f75, "sd card not support 1.8V!");
                    EMMC_ERR_TRACE("%s:sd card not support 1.8V! ",__FUNCTION__);
                    d->support_1_8 = false;
                }
            }
            else
            {
                d->cap_ver = SD_CAP_SDSC;
                d->support_1_8 = false;
            }
            break;
        }

        //if (osiElapsedTimeUS(&timer) > OCR_TIMEOUT_US)
        if((timer_TimRealTickGet(OS_TIMER) - startTime) > 2*1000*1000 ) //1s
        {
            //OSI_LOGI(0x10007e17, "%4c ACMD41 retry timeout", d->name);
            EMMC_ERR_TRACE("%s:ACMD41 retry timeout",__FUNCTION__);
            goto fail;
        }
    } while (1);

    //if support 1.8
    // if (d->support_1_8)
    // {
    //     if (!prvEmmcCommand(d, CMD11_VOLTAGE_SWITCH, 0, NULL, rsp_buf))
    //         goto fail;
    //     //close sd clock /close inter clk
    //     d->hwp->clk_ctrl &= ~EMMC_SDCLK_EN;
    //     d->hwp->clk_ctrl &= ~EMMC_INT_CLK_EN;

    //     halPmuSwitchPower(HAL_POWER_SD, false, false);
    //     halPmuSetPowerLevel(HAL_POWER_SD, POWER_LEVEL_1800MV);
    //     osiThreadSleepUS(1000);
    //     halPmuSetPowerLevel(HAL_POWER_SD, POWER_LEVEL_1800MV);
    //     halPmuSwitchPower(HAL_POWER_SD, true, true);
    //     osiThreadSleepUS(1000);
    //     osiDelayUS(4000);
    //     //open inter clock and sd clock
    //     d->hwp->clk_ctrl |= EMMC_INT_CLK_EN;
    //     while (0 == (d->hwp->clk_ctrl & EMMC_INT_CLK_STABLE))
    //         ;
    //     d->hwp->clk_ctrl |= EMMC_SDCLK_EN;

    //     OSI_LOGI(0, "switch to 1.8v");
    //     osiDelayUS(1000);
    // }

    if (!prvEmmcReadCid(d))
        goto fail;

    if (!sdReadRca(d, &rca))
        goto fail;
    d->rca = rca;
    EMMC_LOG_TRACE("%s:CMD3 done, resp/0x%x, rca/0x%x",__FUNCTION__,d->resp.v[0], rca);

    if (!sdReadCsd(d, &(d->csd)))
        goto fail;

    EMMC_ERR_TRACE("%s:capacity:0x%x,max_blk_len:%d,blk_cnt:%d",__FUNCTION__,d->capacity,d->max_blk_len,d->block_count);

    argu = d->rca << 16;
    if (!prvEmmcCommand(d, CMD7_SEL_DESELECT_CARD_SD, argu, NULL, rsp_buf))
        goto fail;

    d->blk_len = DEFAULT_BLOCK_LEN;
    if (!prvEmmcCommand(d, CMD16_SET_BLOCKLEN, DEFAULT_BLOCK_LEN, NULL, rsp_buf))
        goto fail;

    d->bus_width = BUS_4BIT_WIDTH;
    d->spd_mode = HIGH_SPEED;
    d->dma_sel = SDMA;
    d->adma2_data_len = ADMA2_DATA_LEN_16;
    d->dma_addr_len = DMA_ADDR_LEN_32;

    prvEmmcDmaSet(d);

    if (d->spd_mode == SDR_104)
        prvEmmcSetDriver(0x7, 0x5);

    if (!sdSetBusWidth(d))
        goto fail;

    if (!sdReadScr(d, &scr))
        goto fail;

    if (!sdSetSpeedMode(d))
        goto fail;

    EMMC_LOG_TRACE("%s:sd open done",__FUNCTION__);
    prvEmmcSetDelayLine(d);
    d->open_flag = true;
    //osiMutexUnlock(d->lock);
    return true;

fail:
    EMMC_ERR_TRACE("%s:sd open fail",__FUNCTION__);
    d->open_flag = false;
    prvEmmcPowerOff(d);
    //osiReleaseClk(&d->clk_constrain);
    //osiMutexUnlock(d->lock);
    return false;
}

static bool sdioInitCard(drvEmmc_t *d)
{
    uint8_t rsp_buf[16] = {0};
    uint32 startTime = 0;
    uint32_t argu;
    uint32_t tmp_addr = 0x88000000;
    uint32_t cmd52_resp;

    EMMC_LOG_TRACE("%s:sdio open ",__FUNCTION__);

    if (d == NULL || rsp_buf == NULL)
        return false;

    d->hwp->int_st_en &= (~EMMC_CARD_INT_EN);
    // drvSdioClearMask();
    mcd_delay_sd1(50);
    //osiMutexLock(d->lock);
    //osiRequestSysClkActive(&d->clk_constrain);

    //OSI_LOGI(0x10009f77, "delay 50ms to delay wait 5955 normol mode ");
    startTime = timer_TimRealTickGet(OS_TIMER);
    do
    {
        if (!prvEmmcCommand(d, CMD5_SEND_OP_COND, 0x00300000, NULL, rsp_buf))
            goto fail;
        //BIT_7 to break  resp  903fc000
        if (0 != (rsp_buf[0] & BIT(7))) //ip
            break;
        if((timer_TimRealTickGet(OS_TIMER) - startTime) > 2*1000*1000 ) //1s
        {
            EMMC_ERR_TRACE("%s CMD5 retry timeout", __FUNCTION__);
            goto fail;
        }

    } while (1);

    //read rca
    if (!prvEmmcCommand(d, CMD3_SEND_RELATIVE_ADDR, 0, NULL, rsp_buf))
        goto fail;

    argu = d->hwp->resp0 & 0xffff0000;

    if (!prvEmmcCommand(d, CMD7_SEL_DESELECT_CARD_SD, argu, NULL, rsp_buf))
        goto fail;

    //enable function 1,write 1 to bit 1 in register 0x02 in CCCR (enable sdio function)
    prvEmmcCommand(d, CMD52_IO_RW_DIRECT, 0x88000402, NULL, rsp_buf);
    //enable function1 interrupt, write 1 to bit 1 in register 0x04 in CCCR (enable sdio card int)
    prvEmmcCommand(d, CMD52_IO_RW_DIRECT, 0x88000803, NULL, rsp_buf);

    //enable_read_wait
    d->hwp->host_ctrl1 |= EMMC_RD_WAIT_CTRL;

    d->bus_width = BUS_1BIT_WIDTH;
    d->spd_mode = LOW_SPEED;
    d->dma_sel = SDMA;
    d->adma2_data_len = ADMA2_DATA_LEN_16;
    d->dma_addr_len = DMA_ADDR_LEN_32;

    prvEmmcDmaSet(d);

    //enable funtion1 init mask
    tmp_addr = tmp_addr | (1 << 28) | (0x04 << 9) | 0x37; //0x98000837
    prvEmmcCommand(d, CMD52_IO_RW_DIRECT, tmp_addr, NULL, rsp_buf);

    cmd52_resp = d->hwp->resp0;
    if (cmd52_resp == 0x1200)
    {
        EMMC_ERR_TRACE("%s CMD52 respons is invalid function num was requested", __FUNCTION__);
        goto fail;
    }
    else
    {
        //set speed mode
        prvEmmcCommand(d, CMD52_IO_RW_DIRECT, 0x00000600, NULL, rsp_buf);
        prvEmmcSetSpeedMode(d, LOW_SPEED);

        prvEmmcCommand(d, CMD52_IO_RW_DIRECT, 0x88000E02, NULL, rsp_buf);
        mcd_delay_sd1(1);
        prvEmmcSetDataWidth(d, BUS_1BIT_WIDTH);
        EMMC_ERR_TRACE("%s sdio device set bus width", __FUNCTION__);
    }

    // enable_read_wait
    d->hwp->host_ctrl1 |= EMMC_RD_WAIT_CTRL;
    EMMC_LOG_TRACE("%s sdio equipment open done", __FUNCTION__);
    d->open_flag = true;
    d->card_type = CARD_TYPE_SDIO;

    //osiMutexUnlock(d->lock);
    return true;

fail:
    EMMC_ERR_TRACE("%s sdio open fail", __FUNCTION__);
    d->open_flag = false;
    prvEmmcPowerOff(d);
    //osiReleaseClk(&d->clk_constrain);
    //osiMutexUnlock(d->lock);
    return false;
}

static bool prvEmmcRead_CMD18_2_CMD17(drvEmmc_t *d, uint32_t start_blk, uint32_t blk_cnt, uint32_t dma_addr, dmaBuffInfo_t *dma_buff_info)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t addr = 0xFFFFFFFF;
    dataParam_t data_param;
    bool ret = true;

    uint32 i = 0;
    uint32_t tmp_stark_blk = start_blk;
    uint32_t tmp_blk_cnt = 1;

    do{
        data_param.blk_size = d->blk_len;
        data_param.blk_cnt = 1;
        data_param.dma_addr = (uint32_t)dma_addr +(tmp_stark_blk-start_blk)*512;
        data_param.dma_buff_info = dma_buff_info;

        if (CARD_CAP_UNKONWN == d->cap_ver || MMC_CAP_STANDARD == d->cap_ver || SD_CAP_SDSC == d->cap_ver)
        {
            addr = tmp_stark_blk * d->blk_len;
            data_param.blk_addr_mode = false;
        }
        else
        {
            addr = tmp_stark_blk;
            data_param.blk_addr_mode = true;
        }

        if (!prvEmmcCommand(d, CMD17_READ_SINGLE_BLOCK, addr, &data_param, rsp_buf)) {
            ret = false;
            EMMC_ERR_TRACE("%s: start_blk:%d,blk_cnt:%d,ret=%d",__FUNCTION__,tmp_stark_blk,1,ret);
            goto done;
        }

        tmp_stark_blk++;
        blk_cnt--;
    }while(blk_cnt);

    done:
    //EMMC_ERR_TRACE("%s: start_blk:%d,blk_cnt:%d,ret=%d",__FUNCTION__,start_blk,blk_cnt,ret);
    return ret;
}

static bool prvEmmcReadBlocks(drvEmmc_t *d, uint32_t start_blk, uint32_t blk_cnt, uint32_t dma_addr, dmaBuffInfo_t *dma_buff_info)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t addr = 0xFFFFFFFF;
    dataParam_t data_param;
    bool ret = true;
    data_param.blk_size = d->blk_len;
    data_param.blk_cnt = blk_cnt;
    data_param.dma_addr = (uint32_t)dma_addr;
    data_param.dma_buff_info = dma_buff_info;

    if (CARD_CAP_UNKONWN == d->cap_ver || MMC_CAP_STANDARD == d->cap_ver || SD_CAP_SDSC == d->cap_ver)
    {
        addr = start_blk * d->blk_len;
        data_param.blk_addr_mode = false;
    }
    else
    {
        addr = start_blk;
        data_param.blk_addr_mode = true;
    }

    if (blk_cnt == 1)
    {
        if (!prvEmmcCommand(d, CMD17_READ_SINGLE_BLOCK, addr, &data_param, rsp_buf)) {
            ret = false;
            goto done;
        }
    }
    else
    {
        if (d->if_spprt_cmd23)
        {
            if (!prvEmmcCommand(d, CMD23_SET_BLOCK_COUNT, data_param.blk_cnt, NULL, rsp_buf)) {
                ret = false;
                goto done;
            }

            if (!prvEmmcCommand(d, CMD18_READ_MULTIPLE_BLOCK, addr, &data_param, rsp_buf))
            {
                prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf);
                //retry cmd18
                if (!prvEmmcCommand(d, CMD18_READ_MULTIPLE_BLOCK, addr, &data_param, rsp_buf)){
                    //retry cmd17
                    ret = prvEmmcRead_CMD18_2_CMD17(d,start_blk,blk_cnt,dma_addr,dma_buff_info);
                    if(ret == false)
                    {
                        goto done;
                    }
                }
                else if (!prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf)){
                    ret = false;
                    goto done;
                }

            }
        }
        else
        {
            if (!prvEmmcCommand(d, CMD18_READ_MULTIPLE_BLOCK, addr, &data_param, rsp_buf))
            {
                prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf);

                //retry cmd18
                if (!prvEmmcCommand(d, CMD18_READ_MULTIPLE_BLOCK, addr, &data_param, rsp_buf)){
                    //retry cmd17
                    ret = prvEmmcRead_CMD18_2_CMD17(d,start_blk,blk_cnt,dma_addr,dma_buff_info);
                    if(ret == false)
                    {
                        goto done;
                    }
                }
                else if (!prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf)){
                    ret = false;
                    goto done;
                }

            }
            else if (!prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf)){
                ret = false;
                goto done;
            }

        }
    }
    done:
    //EMMC_LOG_TRACE("%s: start_blk:%d,blk_cnt:%d,ret=:%d",__FUNCTION__,start_blk,blk_cnt,ret);
    mcd_TickDelay(2);//for bug 2562084
    return ret;
}

static bool prvEmmcWriteBlocks(drvEmmc_t *d, uint32_t start_blk, uint32_t blk_cnt, uint32_t dma_addr, dmaBuffInfo_t *dma_buff_info)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t addr = 0;
    dataParam_t data_param;
    bool ret = true;
    data_param.blk_size = d->blk_len;
    data_param.blk_cnt = blk_cnt;
    data_param.dma_addr = (uint32_t)dma_addr;
    data_param.dma_buff_info = dma_buff_info;

    if (CARD_CAP_UNKONWN == d->cap_ver || MMC_CAP_STANDARD == d->cap_ver || SD_CAP_SDSC == d->cap_ver)
    {
        addr = start_blk * d->blk_len;
        data_param.blk_addr_mode = false;
    }
    else
    {
        addr = start_blk;
        data_param.blk_addr_mode = true;
    }

    //osiDCacheClean((uint32_t *)dma_addr, d->blk_len * blk_cnt);
    // osiDelayUS(1000);
    MMU_DmaCacheSync((uint32)dma_addr, d->blk_len * blk_cnt,DMABUFFER_TO_DEVICE);

    if (blk_cnt == 1)
    {
        if (!prvEmmcCommand(d, CMD24_WRITE_BLOCK, addr, &data_param, rsp_buf)){
            ret = false;
            goto done;
        }
    }
    else
    {
        if (d->if_spprt_cmd23)
        {
            if (!prvEmmcCommand(d, CMD23_SET_BLOCK_COUNT, data_param.blk_cnt, NULL, rsp_buf)){
                ret = false;
                goto done;
            }

            if (!prvEmmcCommand(d, CMD25_WRITE_MULTIPLE_BLOCK, addr, &data_param, rsp_buf))
            {
                prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf);
                ret = false;
                goto done;
            }
        }
        else
        {
            if (!prvEmmcCommand(d, CMD25_WRITE_MULTIPLE_BLOCK, addr, &data_param, rsp_buf))
            {
                prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf);
                ret = false;
                goto done;
            }
            if (!prvEmmcCommand(d, CMD12_STOP_TRANSMISSION, 0, NULL, rsp_buf)){
                ret = false;
                goto done;
             }
        }
    }
    done:
    EMMC_ERR_TRACE("%s:.start_blk:%d,blk_cnt:%d,ret=%d",__FUNCTION__,start_blk,blk_cnt,ret);
    mcd_TickDelay(2);//for bug 2562084
    return ret;
}

/**
 * Read API
 */
//bool drvEmmcRead(void *ctx, uint32_t nr, void *buf, uint32_t size)
MCD_ERR_T mcd_Read(uint32 startBlk, uint8* buf, uint32 size)
{
    //startAddr- needs TBD!!!!!!!
    uint32_t blk_cnt;
    uint32_t dma_addr;
    MCD_ERR_T ret = MCD_ERR_NO;
    static dmaBuffInfo_t r_dma_buff_info[DMA_GROUP_MAX];

    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;

    if (size == 0)
        return MCD_ERR_NO;

    if (d == NULL || !d->open_flag || buf == NULL)
        return MCD_ERR;

    dma_addr = (uint32_t)buf;
    r_dma_buff_info->dma_buffer[0].buffer = (uint8_t *)dma_addr;
    r_dma_buff_info->dma_buffer[0].buff_size = size;
    r_dma_buff_info->buff_cnt = 1;
    blk_cnt = size / 512;

    SCI_SDIO_EnableDeepSleep(SCI_FALSE);
    SCI_GetSemaphore(s_emmc_sm_ptr,SCI_WAIT_FOREVER);

    if(g_mcdCardIsBusy == FALSE)
    {
        g_mcdCardIsBusy  = TRUE;
    }

    //osiDCacheInvalidate((uint32_t *)dma_addr, 512 * blk_cnt);
    MMU_DmaCacheSync((uint32)dma_addr,512 * blk_cnt,DMABUFFER_FROM_DEVICE);

    if (!prvEmmcReadBlocks(d, startBlk, blk_cnt, dma_addr, r_dma_buff_info))
        ret = MCD_ERR;

    SCI_PutSemaphore(s_emmc_sm_ptr);
    g_mcdCardIsBusy = FALSE;
    SCI_SDIO_EnableDeepSleep(SCI_TRUE);
    return ret;
}

/**
 * Write API
 */
//bool drvEmmcWrite(void *ctx, uint32_t nr, const void *buf, uint32_t size)
MCD_ERR_T mcd_Write(uint32 startBlk,  CONST uint8* buf, uint32 size)
{
    //startAddr- needs TBD!!!!!!!
    uint32_t blk_cnt;
    uint32_t dma_addr;
    MCD_ERR_T ret = MCD_ERR_NO;
    static dmaBuffInfo_t w_dma_buff_info[DMA_GROUP_MAX];

    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;

    if (size == 0)
        return MCD_ERR_NO;
    if (d == NULL || !d->open_flag || buf == NULL)
        return MCD_ERR;


    dma_addr = (uint32_t)buf;
    w_dma_buff_info->dma_buffer[0].buffer = (uint8_t *)dma_addr;
    w_dma_buff_info->dma_buffer[0].buff_size = size;
    w_dma_buff_info->buff_cnt = 1;
    blk_cnt = size / 512;
    SCI_SDIO_EnableDeepSleep(SCI_FALSE);

    //osiMutexLock(d->lock);
    //osiRequestSysClkActive(&d->clk_constrain);
    //osiPmWakeLock(d->pm_source);
    SCI_GetSemaphore(s_emmc_sm_ptr,SCI_WAIT_FOREVER);
    if(g_mcdCardIsBusy == FALSE)
    {
        g_mcdCardIsBusy  = TRUE;
    }
    d->blk_len = 512;

    if (!prvEmmcWriteBlocks(d, startBlk, blk_cnt, dma_addr, w_dma_buff_info))
        ret = MCD_ERR;

    //osiPmWakeUnlock(d->pm_source);
    //osiReleaseClk(&d->clk_constrain);
    //osiMutexUnlock(d->lock);
    SCI_PutSemaphore(s_emmc_sm_ptr);
    SCI_SDIO_EnableDeepSleep(SCI_TRUE);
    g_mcdCardIsBusy = FALSE;
    return ret;
}
BOOLEAN Sdio_open(void)
{
    MCD_ERR_T ret;
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;
    SCI_SDIO_EnableDeepSleep(SCI_FALSE);
    drvEmmcCreate();
    ret = mcd_Open(MCD_CARD_ID_0,&(d->csd.sd_csd20),MCD_CARD_V2);
    if(ret != MCD_ERR_NO)
    {

        SCI_SDIO_EnableDeepSleep(SCI_TRUE);
        return FALSE;
    }

    //mcd_delay_sd1(2000);

    //test_mcd_card();
    SCI_SDIO_EnableDeepSleep(SCI_TRUE);

    return TRUE;


}

MCD_HANDLE_CDS_T * mcd_get_mcdCsd(void)
{
    //MCD_HANDLE_CDS_T g_handleCsd;

    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;
    if (d == NULL)
        return NULL;
    g_handleCsd.vertion = d->cap_ver;
    g_handleCsd.Capacity = d->block_count;
    g_handleCsd.bus_width = d->bus_width;
    g_handleCsd.RCA = d->rca << 16;
    //g_handleCsd.csd = d->csd;

    EMMC_LOG_TRACE( "mcd_get_mcdCsd ver %d,cc %d ",g_handleCsd.vertion,g_handleCsd.Capacity);

    return &g_handleCsd;
}


/**
 * Open emmc driver
 */
extern BOOLEAN g_LdoEmmcPwrOffFlag;
MCD_ERR_T mcd_Open(MCD_CARD_ID mcdId,MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer)
{
    uint8_t rsp_buf[16] = {0};
    uint32_t cmd8_resp;
    uint32_t cmd5_resp;
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;
    EMMC_LOG_TRACE("%s enter",__FUNCTION__);

    if (d->open_flag)
    {
        if(g_LdoEmmcPwrOffFlag)
        {
            EMMC_LOG_TRACE("mcd: Has enter LP mode, close it first,then open again");
            mcd_Close_no_sleep(0);
            g_LdoEmmcPwrOffFlag=FALSE;
        }
        else
        {
            // Already opened, just return OK
            EMMC_LOG_TRACE( "mcd: mcd_Open: Already opened");
            //*mcdCsd = g_mcdLatestCsd;
            return MCD_ERR_NO;
        }
    }
    //SCI_DisableIRQ();
    if (s_emmc_sm_ptr == PNULL)
    {

        // Create semaphore and go on with the driver.

        // NOTE: This semaphore is never deleted for now, as
        // 1. sema deletion while other task is waiting for it will cause an error;
        // 2. sema deletion will make trouble if already-open state is considered OK.
        s_emmc_sm_ptr   = SCI_CreateSemaphore("SDMMC OPERATION SEMAPHORE", 1);
		 SCI_ASSERT (PNULL != s_emmc_sm_ptr);
    }
    //SCI_RestoreIRQ();
    EMMC_LOG_TRACE(  "mcd: mcd_Open: opened start");
    // Following operation should be kept protected
    //SCI_GetSemaphore(s_emmc_sm_ptr,SCI_WAIT_FOREVER);

    prvEmmcPowerOn(d);
    mcd_delay_sd1(10);
    prvEmmcSetClk(d, CLK_400_K);

    mcd_delay_sd1(1);
    #if !defined(OS_NONE)   //for nor_bootloader compiler
    CHIPDRV_EnableIRQINT (d->irqn);
    #endif

    prvEmmcCommand(d, CMD0_GO_IDLE_STATE, 0, NULL, rsp_buf);
    mcd_delay_sd1(1);
    //sdio spec3.0  p18
    if(prvEmmcCommand(d, CMD8_SEND_IF_COND_SD, EMMC_CMD8_ARG, NULL, rsp_buf) == true){
        cmd8_resp = d->hwp->resp0;
        if (cmd8_resp == 0x0)
        {
            prvEmmcCommand(d, CMD5_SEND_OP_COND, 0, NULL, rsp_buf);

            cmd5_resp = d->hwp->resp0;
            if (cmd5_resp == 0x0)
            {
                //switch to 1.8v for emmc
                //halPmuSetPowerLevel(HAL_POWER_SD, POWER_LEVEL_1800MV);
                ANA_REG_MSK_OR(ANA_LDO_VOSEL3,(0x07<<10),(0x3FUL<<10));//set SD power level to 1.8V
                if (!mmcInitCard(d))
                    goto fail;
            }
            else
            {
                if (!sdioInitCard(d))
                    goto fail;
            }
        }
        else
        {
            if (!sdInitCard(d))
                goto fail;
        }
    }
    else
    {
        if (!sdInitCard(d))
            goto fail;
    }

    g_handleCsd.vertion = d->cap_ver;
    g_handleCsd.Capacity = d->block_count;
    g_handleCsd.bus_width = d->bus_width;
    g_handleCsd.RCA = d->rca << 16;

    //SCI_PutSemaphore(s_emmc_sm_ptr);
    return MCD_ERR_NO;

fail:
    EMMC_ERR_TRACE(  "mcd: mcd_Open: opened fail");
    prvEmmcPowerOff(d);
    #if !defined(OS_NONE)   //for nor_bootloader compiler
    CHIPDRV_DisableIRQINT(d->irqn);
    #endif
    //SCI_PutSemaphore(s_emmc_sm_ptr);
    //osiReleaseClk(&d->clk_constrain);
    //osiMutexUnlock(d->lock);
    return MCD_ERR;
}
MCD_ERR_T mcd_Close_no_sleep(MCD_CARD_ID mcdId)
{
    mcd_Close(MCD_CARD_ID_0);
    return MCD_ERR_NO;
}

/**
 * Close emmc driver
 */
//void drvEmmcClose(void *ctx)
MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId)
{
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;

    if (!d->open_flag)
    {
        EMMC_ERR_TRACE("%s: Never Opened before",__FUNCTION__);
        return MCD_ERR_NO;
    }
    if(g_mcdCardIsBusy){
        //stop transfer
        EMMC_ERR_TRACE( "%s: card is in transfer",__FUNCTION__);
		while(g_mcdCardIsBusy)
		{
			mcd_delay_sd1(100);
		}
		EMMC_ERR_TRACE( "%s: transfer done,can be closed",__FUNCTION__);

    }

    //osiMutexLock(d->lock);
    d->open_flag = false;
    // d->hwp->sdmmc_int_mask = 0;
    prvEmmcPowerOff(d);
    //osiMutexUnlock(d->lock);
    #if !defined(OS_NONE)   //for nor_bootloader compiler
    CHIPDRV_DisableIRQINT(d->irqn);
	SCI_DeleteSemaphore(s_emmc_sm_ptr);
    #endif
    SCI_SDIO_EnableDeepSleep(SCI_TRUE);
    return MCD_ERR_NO;
}
MCD_ERR_T mcd_GetCardSize(MCD_CARD_SIZE_T* size)
{
    drvEmmc_t *d = (drvEmmc_t *)&gDrEmmc;
    SCI_DisableIRQ();
    size->blockLen = d->blk_len;
    size->nbBlock  = d->block_count;
    SCI_RestoreIRQ();
    return MCD_ERR_NO;
}

/**
 * Delete emmc driver instance
 */
void drvEmmcDestroy(void *ctx)
{
    drvEmmc_t *d = (drvEmmc_t *)ctx;
    if (d == NULL)
        return;

    mcd_Close(MCD_CARD_ID_0);
    // osiSemaphoreDelete(d->tx_done_sema);
    // osiSemaphoreDelete(d->rx_done_sema);
}

/**
 * Get block number
 */
uint32_t drvEmmcGetBlockNum(void *ctx)
{
    drvEmmc_t *d = (drvEmmc_t *)ctx;
    if (d == NULL)
        return 0;
    return d->block_count;
}

/**
 * Get  type id to distinguish hardware device
 *     0 SDCARD     1 EMMC
 */
uint32_t drvEmmcGetType(void *ctx)
{
    drvEmmc_t *d = (drvEmmc_t *)ctx;
    if (d == NULL)
        return 0;
    return d->card_type;
}

#ifdef _TEST_SDMMC_
MCD_ERR_T  test_open(void)
{
    MCD_CSD_T mcdCsd;
    return mcd_Open(0,&mcdCsd,MCD_CARD_V2);

}
MCD_ERR_T test_mcd_Read(uint32 startAddr, uint8* rdDataBlock, uint32 blockLength)
{
    return mcd_Read(startAddr,rdDataBlock,blockLength);
}

MCD_ERR_T test_mcd_Write(uint32 blockStartAddr, CONST uint8* blockWr, uint32 blockLength)
{

    return mcd_Write( blockStartAddr,  blockWr, blockLength);
}

void test_close()
{

    mcd_Close(0);

}

void test_reopenspi()
{


}
void test_mcd_Write_4block(uint32 blockStartAddr, CONST uint8* blockWr, uint32 blockLength)
{
    //mcd_Write_mutil(blockStartAddr,  blockWr, blockLength);
}

///#define HAL_UNCACHED_DATA __attribute__((section(".ucdata")))

//uint8 HAL_UNCACHED_DATA  test_m_w[1024];
//uint8  HAL_UNCACHED_DATA test_m_r[1024];
uint8   test_m_w[1024];
uint8   test_m_r[1024];



void test_mcd_card(void)
{
    uint32 k,m;
    uint32 add=0x1000;
    uint32 wait_time = 0;
    uint32 wait_time2 = 0;
    for(k=0; k<1024; k++)
    {
        test_m_w[k]=k;
    }
    /*
    if(MCD_ERR_NO ==  test_open())
    {
        EMMC_ERR_TRACE("%s open success",__FUNCTION__);
    }
    else
    {
        EMMC_ERR_TRACE("%s open fail",__FUNCTION__);
        return;

    }
    */
testmcd :

    for(k=0; k<1024; k++)
    {
        test_m_w[k]=0xa5a5a5a5;
    }
    wait_time = timer_TimRealTickGet(OS_TIMER);

    if(test_mcd_Write(add,test_m_w,1024) != MCD_ERR_NO){
        EMMC_ERR_TRACE("%s[%d] test_mcd_Write FAIL",__FUNCTION__,__LINE__);
        return;
    }
    wait_time2 = timer_TimRealTickGet(OS_TIMER);

    EMMC_ERR_TRACE("%s write 1024B cost time:%dms",__FUNCTION__,1000/((wait_time2-wait_time)*1000/16384));
    wait_time = timer_TimRealTickGet(OS_TIMER);

    if(test_mcd_Read(add,test_m_r,1024)!= MCD_ERR_NO){
        EMMC_ERR_TRACE("%s[%d] test_mcd_Read FAIL",__FUNCTION__,__LINE__);
        return;
    }
    wait_time2 = timer_TimRealTickGet(OS_TIMER);

    EMMC_ERR_TRACE("%s read 1024B cost time:%dms",__FUNCTION__,1000/((wait_time2-wait_time)*1000/16384));


    for(k=0; k<1024; k++)
    {
        if(test_m_w[k]!=test_m_r[k])
        {
            EMMC_ERR_TRACE("%s error byte[%d] in add:0x%x,w:0x%x,r:0x%x",__FUNCTION__,k,add,test_m_w[k],test_m_r[k]);
            while(1) {};
            break;
        }
    }


    for(k=0; k<1024; k++)
    {
        test_m_w[k]=0x5a5a5a5a;
    }

    if(add >0xace000) add = 0x1000;
    if(test_mcd_Write(add,test_m_w,1024) != MCD_ERR_NO)
    {
        EMMC_ERR_TRACE("%s[%d] test_mcd_Write FAIL",__FUNCTION__,__LINE__);
        return;
    }
    if(test_mcd_Read(add,test_m_r,1024)!= MCD_ERR_NO){
        EMMC_ERR_TRACE("%s[%d] test_mcd_Read FAIL",__FUNCTION__,__LINE__);
        return;
    }
    for(k=0; k<1024; k++)
    {
        if(test_m_w[k]!=test_m_r[k])
        {
            EMMC_ERR_TRACE("%s error byte[%d] in add:0x%x,w:0x%x,r:0x%x",__FUNCTION__,k,add,test_m_w[k],test_m_r[k]);
            while(1) {};
            break;
        }
    }

    EMMC_ERR_TRACE( "test card addr     = %d  \n",add   );

    mcd_delay_sd1(100);
    add+=1024;
    goto testmcd;
    //mcd_Close(0);
    //while(1) {};

}

#endif


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

#ifndef _ANALOG_G2_H_
#define _ANALOG_G2_H_

// Auto generated by dtools(see dtools.txt for its version).
// Don't edit it manually!

#define REG_ANALOG_G2_SET_OFFSET (1024)
#define REG_ANALOG_G2_CLR_OFFSET (2048)

#define REG_ANALOG_G2_BASE (0x5150f000)

typedef volatile struct
{
    uint32_t analog_usb20_usb20_test_pin;       // 0x00000000
    uint32_t analog_usb20_usb20_utmi_ctl1;      // 0x00000004
    uint32_t analog_usb20_usb20_batter_pll;     // 0x00000008
    uint32_t analog_usb20_usb20_utmi_ctl2;      // 0x0000000c
    uint32_t analog_usb20_usb20_trimming;       // 0x00000010
    uint32_t analog_usb20_reg_sel_cfg_0;        // 0x00000014
    uint32_t __24[250];                         // 0x00000018
    uint32_t analog_usb20_usb20_test_pin_set;   // 0x00000400
    uint32_t analog_usb20_usb20_utmi_ctl1_set;  // 0x00000404
    uint32_t analog_usb20_usb20_batter_pll_set; // 0x00000408
    uint32_t analog_usb20_usb20_utmi_ctl2_set;  // 0x0000040c
    uint32_t __1040[1];                         // 0x00000410
    uint32_t analog_usb20_reg_sel_cfg_0_set;    // 0x00000414
    uint32_t __1048[250];                       // 0x00000418
    uint32_t analog_usb20_usb20_test_pin_clr;   // 0x00000800
    uint32_t analog_usb20_usb20_utmi_ctl1_clr;  // 0x00000804
    uint32_t analog_usb20_usb20_batter_pll_clr; // 0x00000808
    uint32_t analog_usb20_usb20_utmi_ctl2_clr;  // 0x0000080c
    uint32_t __2064[1];                         // 0x00000810
    uint32_t analog_usb20_reg_sel_cfg_0_clr;    // 0x00000814
} HWP_ANALOG_G2_T;

#define hwp_analogG2 ((HWP_ANALOG_G2_T *)REG_ACCESS_ADDRESS(REG_ANALOG_G2_BASE))

// analog_usb20_usb20_test_pin
typedef union {
    uint32_t v;
    struct
    {
        uint32_t analog_usb20_usb20_lpbk_end : 1;       // [0], read only
        uint32_t analog_usb20_usb20_t2rcomp : 1;        // [1], read only
        uint32_t analog_usb20_usb20_bist_mode : 5;      // [6:2]
        uint32_t analog_usb20_usb20_testdataout : 4;    // [10:7], read only
        uint32_t analog_usb20_usb20_testdataoutsel : 1; // [11]
        uint32_t analog_usb20_usb20_testaddr : 4;       // [15:12]
        uint32_t analog_usb20_usb20_testdatain : 8;     // [23:16]
        uint32_t analog_usb20_usb20_testclk : 1;        // [24]
        uint32_t __31_25 : 7;                           // [31:25]
    } b;
} REG_ANALOG_G2_ANALOG_USB20_USB20_TEST_PIN_T;

// analog_usb20_usb20_utmi_ctl1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t __15_0 : 16;                          // [15:0]
        uint32_t analog_usb20_usb20_vbusvldext : 1;    // [16]
        uint32_t analog_usb20_usb20_bypass_out_dm : 1; // [17], read only
        uint32_t analog_usb20_usb20_bypass_out_dp : 1; // [18], read only
        uint32_t analog_usb20_usb20_bypass_in_dm : 1;  // [19]
        uint32_t analog_usb20_usb20_bypass_in_dp : 1;  // [20]
        uint32_t analog_usb20_usb20_bypass_fs : 1;     // [21]
        uint32_t analog_usb20_usb20_bypass_drv_dm : 1; // [22]
        uint32_t analog_usb20_usb20_bypass_drv_dp : 1; // [23]
        uint32_t analog_usb20_usb20_rxerror : 1;       // [24], read only
        uint32_t analog_usb20_usb20_reset : 1;         // [25]
        uint32_t analog_usb20_usb20_porn : 1;          // [26]
        uint32_t analog_usb20_usb20_suspendm : 1;      // [27]
        uint32_t analog_usb20_usb20_databus16_8 : 1;   // [28]
        uint32_t __31_29 : 3;                          // [31:29]
    } b;
} REG_ANALOG_G2_ANALOG_USB20_USB20_UTMI_CTL1_T;

// analog_usb20_usb20_batter_pll
typedef union {
    uint32_t v;
    struct
    {
        uint32_t analog_usb20_usb20_sampler_sel : 1; // [0]
        uint32_t analog_usb20_usb20_dmpullup : 1;    // [1]
        uint32_t analog_usb20_usb20_rextenable : 1;  // [2]
        uint32_t __31_3 : 29;                        // [31:3]
    } b;
} REG_ANALOG_G2_ANALOG_USB20_USB20_BATTER_PLL_T;

// analog_usb20_usb20_utmi_ctl2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t analog_usb20_usb20_sleepm : 1;            // [0]
        uint32_t analog_usb20_usb20_txbitstuffenableh : 1; // [1]
        uint32_t analog_usb20_usb20_txbitstuffenable : 1;  // [2]
        uint32_t analog_usb20_usb20_dmpulldown : 1;        // [3]
        uint32_t analog_usb20_usb20_dppulldown : 1;        // [4]
        uint32_t __31_5 : 27;                              // [31:5]
    } b;
} REG_ANALOG_G2_ANALOG_USB20_USB20_UTMI_CTL2_T;

// analog_usb20_usb20_trimming
typedef union {
    uint32_t v;
    struct
    {
        uint32_t analog_usb20_usb20_tuneplls : 2;  // [1:0]
        uint32_t analog_usb20_usb20_tuneeq : 3;    // [4:2]
        uint32_t analog_usb20_usb20_tunesq : 4;    // [8:5]
        uint32_t analog_usb20_usb20_tunedsc : 2;   // [10:9]
        uint32_t analog_usb20_usb20_tuneotg : 3;   // [13:11]
        uint32_t analog_usb20_usb20_tunerise : 2;  // [15:14]
        uint32_t analog_usb20_usb20_tfhsres : 5;   // [20:16]
        uint32_t analog_usb20_usb20_tfregres : 6;  // [26:21]
        uint32_t analog_usb20_usb20_tunehsamp : 2; // [28:27]
        uint32_t __31_29 : 3;                      // [31:29]
    } b;
} REG_ANALOG_G2_ANALOG_USB20_USB20_TRIMMING_T;

// analog_usb20_reg_sel_cfg_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbg_sel_analog_usb20_usb20_sleepm : 1;        // [0]
        uint32_t dbg_sel_analog_usb20_usb20_dmpulldown : 1;    // [1]
        uint32_t dbg_sel_analog_usb20_usb20_dppulldown : 1;    // [2]
        uint32_t dbg_sel_analog_usb20_usb20_sampler_sel : 1;   // [3]
        uint32_t dbg_sel_analog_usb20_usb20_dmpullup : 1;      // [4]
        uint32_t dbg_sel_analog_usb20_usb20_rextenable : 1;    // [5]
        uint32_t dbg_sel_analog_usb20_usb20_bypass_in_dm : 1;  // [6]
        uint32_t dbg_sel_analog_usb20_usb20_bypass_fs : 1;     // [7]
        uint32_t dbg_sel_analog_usb20_usb20_bypass_drv_dm : 1; // [8]
        uint32_t dbg_sel_analog_usb20_usb20_reset : 1;         // [9]
        uint32_t dbg_sel_analog_usb20_usb20_porn : 1;          // [10]
        uint32_t dbg_sel_analog_usb20_usb20_suspendm : 1;      // [11]
        uint32_t __31_12 : 20;                                 // [31:12]
    } b;
} REG_ANALOG_G2_ANALOG_USB20_REG_SEL_CFG_0_T;

// analog_usb20_usb20_test_pin
#define ANALOG_G2_ANALOG_USB20_USB20_LPBK_END (1 << 0)
#define ANALOG_G2_ANALOG_USB20_USB20_T2RCOMP (1 << 1)
#define ANALOG_G2_ANALOG_USB20_USB20_BIST_MODE(n) (((n)&0x1f) << 2)
#define ANALOG_G2_ANALOG_USB20_USB20_TESTDATAOUT(n) (((n)&0xf) << 7)
#define ANALOG_G2_ANALOG_USB20_USB20_TESTDATAOUTSEL (1 << 11)
#define ANALOG_G2_ANALOG_USB20_USB20_TESTADDR(n) (((n)&0xf) << 12)
#define ANALOG_G2_ANALOG_USB20_USB20_TESTDATAIN(n) (((n)&0xff) << 16)
#define ANALOG_G2_ANALOG_USB20_USB20_TESTCLK (1 << 24)

// analog_usb20_usb20_utmi_ctl1
#define ANALOG_G2_ANALOG_USB20_USB20_VBUSVLDEXT (1 << 16)
#define ANALOG_G2_ANALOG_USB20_USB20_BYPASS_OUT_DM (1 << 17)
#define ANALOG_G2_ANALOG_USB20_USB20_BYPASS_OUT_DP (1 << 18)
#define ANALOG_G2_ANALOG_USB20_USB20_BYPASS_IN_DM (1 << 19)
#define ANALOG_G2_ANALOG_USB20_USB20_BYPASS_IN_DP (1 << 20)
#define ANALOG_G2_ANALOG_USB20_USB20_BYPASS_FS (1 << 21)
#define ANALOG_G2_ANALOG_USB20_USB20_BYPASS_DRV_DM (1 << 22)
#define ANALOG_G2_ANALOG_USB20_USB20_BYPASS_DRV_DP (1 << 23)
#define ANALOG_G2_ANALOG_USB20_USB20_RXERROR (1 << 24)
#define ANALOG_G2_ANALOG_USB20_USB20_RESET (1 << 25)
#define ANALOG_G2_ANALOG_USB20_USB20_PORN (1 << 26)
#define ANALOG_G2_ANALOG_USB20_USB20_SUSPENDM (1 << 27)
#define ANALOG_G2_ANALOG_USB20_USB20_DATABUS16_8 (1 << 28)

// analog_usb20_usb20_batter_pll
#define ANALOG_G2_ANALOG_USB20_USB20_SAMPLER_SEL (1 << 0)
#define ANALOG_G2_ANALOG_USB20_USB20_DMPULLUP (1 << 1)
#define ANALOG_G2_ANALOG_USB20_USB20_REXTENABLE (1 << 2)

// analog_usb20_usb20_utmi_ctl2
#define ANALOG_G2_ANALOG_USB20_USB20_SLEEPM (1 << 0)
#define ANALOG_G2_ANALOG_USB20_USB20_TXBITSTUFFENABLEH (1 << 1)
#define ANALOG_G2_ANALOG_USB20_USB20_TXBITSTUFFENABLE (1 << 2)
#define ANALOG_G2_ANALOG_USB20_USB20_DMPULLDOWN (1 << 3)
#define ANALOG_G2_ANALOG_USB20_USB20_DPPULLDOWN (1 << 4)

// analog_usb20_usb20_trimming
#define ANALOG_G2_ANALOG_USB20_USB20_TUNEPLLS(n) (((n)&0x3) << 0)
#define ANALOG_G2_ANALOG_USB20_USB20_TUNEEQ(n) (((n)&0x7) << 2)
#define ANALOG_G2_ANALOG_USB20_USB20_TUNESQ(n) (((n)&0xf) << 5)
#define ANALOG_G2_ANALOG_USB20_USB20_TUNEDSC(n) (((n)&0x3) << 9)
#define ANALOG_G2_ANALOG_USB20_USB20_TUNEOTG(n) (((n)&0x7) << 11)
#define ANALOG_G2_ANALOG_USB20_USB20_TUNERISE(n) (((n)&0x3) << 14)
#define ANALOG_G2_ANALOG_USB20_USB20_TFHSRES(n) (((n)&0x1f) << 16)
#define ANALOG_G2_ANALOG_USB20_USB20_TFREGRES(n) (((n)&0x3f) << 21)
#define ANALOG_G2_ANALOG_USB20_USB20_TUNEHSAMP(n) (((n)&0x3) << 27)

// analog_usb20_reg_sel_cfg_0
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_SLEEPM (1 << 0)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_DMPULLDOWN (1 << 1)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_DPPULLDOWN (1 << 2)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_SAMPLER_SEL (1 << 3)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_DMPULLUP (1 << 4)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_REXTENABLE (1 << 5)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_BYPASS_IN_DM (1 << 6)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_BYPASS_FS (1 << 7)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_BYPASS_DRV_DM (1 << 8)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_RESET (1 << 9)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_PORN (1 << 10)
#define ANALOG_G2_DBG_SEL_ANALOG_USB20_USB20_SUSPENDM (1 << 11)

#endif // _ANALOG_G2_H_

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

#ifndef _LPS_APB_H_
#define _LPS_APB_H_

// Auto generated by dtools(see dtools.txt for its version).
// Don't edit it manually!

#define REG_LPS_APB_SET_OFFSET (1024)
#define REG_LPS_APB_CLR_OFFSET (2048)

#define REG_LPS_APB_BASE (0x51705000)

typedef volatile struct
{
    uint32_t reset_sys_soft;             // 0x00000000
    uint32_t reset_lps_soft;             // 0x00000004
    uint32_t efuse_por_read_disable;     // 0x00000008
    uint32_t lps_clk_en;                 // 0x0000000c
    uint32_t lps_clk_auto_sel;           // 0x00000010
    uint32_t lps_clk_force_en;           // 0x00000014
    uint32_t lps_clk_gate_en_status;     // 0x00000018
    uint32_t lps_clk_busy_status;        // 0x0000001c
    uint32_t cfg_clk_uart1;              // 0x00000020
    uint32_t cfg_clk_rc26m;              // 0x00000024
    uint32_t cfg_debug_bond_option;      // 0x00000028
    uint32_t cfg_psram_half_slp;         // 0x0000002c
    uint32_t cfg_lps_ahb_clock_sel;      // 0x00000030
    uint32_t cfg_uart1_clock_sel;        // 0x00000034
    uint32_t cfg_gpt_lite_clock_sel;     // 0x00000038
    uint32_t cfg_boot_mode;              // 0x0000003c
    uint32_t cfg_reset_enable;           // 0x00000040
    uint32_t reset_cause;                // 0x00000044
    uint32_t __72[1];                    // 0x00000048
    uint32_t cfg_plls;                   // 0x0000004c
    uint32_t apll_wait_number;           // 0x00000050
    uint32_t mpll_wait_number;           // 0x00000054
    uint32_t iispll_wait_number;         // 0x00000058
    uint32_t aon_iram_ctrl;              // 0x0000005c
    uint32_t iomux_g4_func_sel_latch;    // 0x00000060
    uint32_t cfg_por_usb_phy;            // 0x00000064
    uint32_t efs_por_read_block3;        // 0x00000068
    uint32_t efs_por_read_block89;       // 0x0000006c
    uint32_t rc26m_pu_ctrl;              // 0x00000070
    uint32_t aon_ahb_lp_ctrl;            // 0x00000074
    uint32_t usb_uart_swj_share_cfg;     // 0x00000078
    uint32_t pu_clk26m_lp_iso_cfg;       // 0x0000007c
    uint32_t cfg_io_deep_sleep;          // 0x00000080
    uint32_t cfg_lps_io_core_ie;         // 0x00000084
    uint32_t cfg_simc_io;                // 0x00000088
    uint32_t __140[221];                 // 0x0000008c
    uint32_t reset_sys_soft_set;         // 0x00000400
    uint32_t reset_lps_soft_set;         // 0x00000404
    uint32_t efuse_por_read_disable_set; // 0x00000408
    uint32_t lps_clk_en_set;             // 0x0000040c
    uint32_t lps_clk_auto_sel_set;       // 0x00000410
    uint32_t lps_clk_force_en_set;       // 0x00000414
    uint32_t __1048[2];                  // 0x00000418
    uint32_t cfg_clk_uart1_set;          // 0x00000420
    uint32_t cfg_clk_rc26m_set;          // 0x00000424
    uint32_t __1064[6];                  // 0x00000428
    uint32_t cfg_reset_enable_set;       // 0x00000440
    uint32_t reset_cause_set;            // 0x00000444
    uint32_t __1096[1];                  // 0x00000448
    uint32_t cfg_plls_set;               // 0x0000044c
    uint32_t __1104[3];                  // 0x00000450
    uint32_t aon_iram_ctrl_set;          // 0x0000045c
    uint32_t __1120[1];                  // 0x00000460
    uint32_t cfg_por_usb_phy_set;        // 0x00000464
    uint32_t __1128[2];                  // 0x00000468
    uint32_t rc26m_pu_ctrl_set;          // 0x00000470
    uint32_t aon_ahb_lp_ctrl_set;        // 0x00000474
    uint32_t usb_uart_swj_share_cfg_set; // 0x00000478
    uint32_t __1148[1];                  // 0x0000047c
    uint32_t cfg_io_deep_sleep_set;      // 0x00000480
    uint32_t cfg_lps_io_core_ie_set;     // 0x00000484
    uint32_t cfg_simc_io_set;            // 0x00000488
    uint32_t __1164[221];                // 0x0000048c
    uint32_t reset_sys_soft_clr;         // 0x00000800
    uint32_t reset_lps_soft_clr;         // 0x00000804
    uint32_t efuse_por_read_disable_clr; // 0x00000808
    uint32_t lps_clk_en_clr;             // 0x0000080c
    uint32_t lps_clk_auto_sel_clr;       // 0x00000810
    uint32_t lps_clk_force_en_clr;       // 0x00000814
    uint32_t __2072[2];                  // 0x00000818
    uint32_t cfg_clk_uart1_clr;          // 0x00000820
    uint32_t cfg_clk_rc26m_clr;          // 0x00000824
    uint32_t __2088[6];                  // 0x00000828
    uint32_t cfg_reset_enable_clr;       // 0x00000840
    uint32_t reset_cause_clr;            // 0x00000844
    uint32_t __2120[1];                  // 0x00000848
    uint32_t cfg_plls_clr;               // 0x0000084c
    uint32_t __2128[3];                  // 0x00000850
    uint32_t aon_iram_ctrl_clr;          // 0x0000085c
    uint32_t __2144[1];                  // 0x00000860
    uint32_t cfg_por_usb_phy_clr;        // 0x00000864
    uint32_t __2152[2];                  // 0x00000868
    uint32_t rc26m_pu_ctrl_clr;          // 0x00000870
    uint32_t aon_ahb_lp_ctrl_clr;        // 0x00000874
    uint32_t usb_uart_swj_share_cfg_clr; // 0x00000878
    uint32_t __2172[1];                  // 0x0000087c
    uint32_t cfg_io_deep_sleep_clr;      // 0x00000880
    uint32_t cfg_lps_io_core_ie_clr;     // 0x00000884
    uint32_t cfg_simc_io_clr;            // 0x00000888
} HWP_LPS_APB_T;

#define hwp_lpsApb ((HWP_LPS_APB_T *)REG_ACCESS_ADDRESS(REG_LPS_APB_BASE))

// reset_sys_soft
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_sys_soft_reset : 1;   // [0]
        uint32_t cp_sys_soft_reset : 1;   // [1]
        uint32_t lte_sys_soft_reset : 1;  // [2]
        uint32_t gnss_sys_soft_reset : 1; // [3]
        uint32_t usb_sys_soft_reset : 1;  // [4]
        uint32_t pub_sys_soft_reset : 1;  // [5]
        uint32_t rf_sys_soft_reset : 1;   // [6]
        uint32_t chip_soft_reset : 1;     // [7]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_LPS_APB_RESET_SYS_SOFT_T;

// reset_lps_soft
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpt1_soft_reset : 1;        // [0]
        uint32_t gpio1_soft_reset : 1;       // [1]
        uint32_t keypad_soft_reset : 1;      // [2]
        uint32_t uart1_soft_reset : 1;       // [3]
        uint32_t ana_wrap3_soft_reset : 1;   // [4]
        uint32_t iomux_g4_soft_reset : 1;    // [5]
        uint32_t rc26m_calib_soft_reset : 1; // [6]
        uint32_t rtc_timer_soft_reset : 1;   // [7]
        uint32_t __31_8 : 24;                // [31:8]
    } b;
} REG_LPS_APB_RESET_LPS_SOFT_T;

// efuse_por_read_disable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_por_read_disable : 1; // [0]
        uint32_t efuse_sel_flag : 2;         // [2:1]
        uint32_t __31_3 : 29;                // [31:3]
    } b;
} REG_LPS_APB_EFUSE_POR_READ_DISABLE_T;

// lps_clk_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpio1_en : 1;       // [0]
        uint32_t gpt1_en : 1;        // [1]
        uint32_t idle_lps_en : 1;    // [2]
        uint32_t keypad_en : 1;      // [3]
        uint32_t uart1_en : 1;       // [4]
        uint32_t pwrctrl_en : 1;     // [5]
        uint32_t rtc_timer_en : 1;   // [6]
        uint32_t lps_ahb_aon_en : 1; // [7]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_LPS_APB_LPS_CLK_EN_T;

// cfg_clk_uart1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_clk_uart1_num : 10;   // [9:0]
        uint32_t __15_10 : 6;              // [15:10]
        uint32_t cfg_clk_uart1_demod : 14; // [29:16]
        uint32_t __30_30 : 1;              // [30]
        uint32_t cfg_clk_uart1_update : 1; // [31]
    } b;
} REG_LPS_APB_CFG_CLK_UART1_T;

// cfg_clk_rc26m
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_clk_rc26m_num : 10;   // [9:0]
        uint32_t __15_10 : 6;              // [15:10]
        uint32_t cfg_clk_rc26m_demod : 14; // [29:16]
        uint32_t __30_30 : 1;              // [30]
        uint32_t cfg_clk_rc26m_update : 1; // [31]
    } b;
} REG_LPS_APB_CFG_CLK_RC26M_T;

// cfg_debug_bond_option
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bond_option_wr_flag : 1;      // [0], read only
        uint32_t bond_swd_jtag_en : 1;         // [1]
        uint32_t bond_ap_ca5_dap_deviceen : 1; // [2]
        uint32_t bond_ap_ca5_dbgen : 1;        // [3]
        uint32_t bond_ap_ca5_niden : 1;        // [4]
        uint32_t bond_ap_ca5_spiden : 1;       // [5]
        uint32_t bond_ap_ca5_spniden : 1;      // [6]
        uint32_t bond_cp_ca5_dap_deviceen : 1; // [7]
        uint32_t bond_cp_ca5_dbgen : 1;        // [8]
        uint32_t bond_cp_ca5_niden : 1;        // [9]
        uint32_t bond_cp_ca5_spiden : 1;       // [10]
        uint32_t bond_cp_ca5_spniden : 1;      // [11]
        uint32_t bond_fdma_en : 1;             // [12]
        uint32_t bond_swd_dbg_sys_en : 1;      // [13]
        uint32_t bond_djtag_en : 1;            // [14]
        uint32_t bond_pad_jtag_en : 1;         // [15]
        uint32_t bond_dbghost_en : 1;          // [16]
        uint32_t bond_fdma_boot_cpu_en : 1;    // [17]
        uint32_t bond_sec_dap_en : 1;          // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_LPS_APB_CFG_DEBUG_BOND_OPTION_T;

// cfg_psram_half_slp
typedef union {
    uint32_t v;
    struct
    {
        uint32_t half_slp_req : 1; // [0]
        uint32_t __31_1 : 31;      // [31:1]
    } b;
} REG_LPS_APB_CFG_PSRAM_HALF_SLP_T;

// cfg_lps_ahb_clock_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cgm_lps_ahb_sel : 2; // [1:0]
        uint32_t __31_2 : 30;         // [31:2]
    } b;
} REG_LPS_APB_CFG_LPS_AHB_CLOCK_SEL_T;

// cfg_uart1_clock_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cgm_uart1_bf_div_sel : 2; // [1:0]
        uint32_t __31_2 : 30;              // [31:2]
    } b;
} REG_LPS_APB_CFG_UART1_CLOCK_SEL_T;

// cfg_gpt_lite_clock_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cgm_gpt_lite_sel : 2; // [1:0]
        uint32_t __31_2 : 30;          // [31:2]
    } b;
} REG_LPS_APB_CFG_GPT_LITE_CLOCK_SEL_T;

// cfg_boot_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t boot_mode_pin : 3;      // [2:0]
        uint32_t __3_3 : 1;              // [3]
        uint32_t boot_mode_sw : 6;       // [9:4]
        uint32_t function_test_mode : 1; // [10], read only
        uint32_t __31_11 : 21;           // [31:11]
    } b;
} REG_LPS_APB_CFG_BOOT_MODE_T;

// cfg_reset_enable
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_wdt_reset_enable : 1;                 // [0]
        uint32_t cp_wdt_reset_enable : 1;                 // [1]
        uint32_t rf_wdt_reset_enable : 1;                 // [2]
        uint32_t invalid_clk_32k_reset_enable : 1;        // [3]
        uint32_t invalid_clk_26m_reset_enable : 1;        // [4]
        uint32_t ap_mem_fw_invalid_reset_enable : 1;      // [5]
        uint32_t aon_mem_fw_invalid_reset_enable : 1;     // [6]
        uint32_t pub_mem_fw_invalid_reset_enable : 1;     // [7]
        uint32_t ap_mem_fw_invalid_reset_raw_enable : 1;  // [8]
        uint32_t aon_mem_fw_invalid_reset_raw_enable : 1; // [9]
        uint32_t pub_mem_fw_invalid_reset_raw_enable : 1; // [10]
        uint32_t dbghost_reset_enable : 1;                // [11]
        uint32_t dbghost_reset_ap_cpu_enable : 1;         // [12]
        uint32_t __31_13 : 19;                            // [31:13]
    } b;
} REG_LPS_APB_CFG_RESET_ENABLE_T;

// reset_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_wdt_reset : 1;                 // [0]
        uint32_t cp_wdt_reset : 1;                 // [1]
        uint32_t rf_wdt_reset : 1;                 // [2]
        uint32_t invalid_clk_32k_reset : 1;        // [3]
        uint32_t invalid_clk_26m_reset : 1;        // [4]
        uint32_t ap_mem_fw_invalid_reset : 1;      // [5]
        uint32_t aon_mem_fw_invalid_reset : 1;     // [6]
        uint32_t pub_mem_fw_invalid_reset : 1;     // [7]
        uint32_t ap_mem_fw_invalid_reset_raw : 1;  // [8]
        uint32_t aon_mem_fw_invalid_reset_raw : 1; // [9]
        uint32_t pub_mem_fw_invalid_reset_raw : 1; // [10]
        uint32_t dbghost_reset : 1;                // [11]
        uint32_t dbghost_reset_ap_cpu : 1;         // [12]
        uint32_t funcdma_reset_ap_cpu : 1;         // [13]
        uint32_t soft_reset : 1;                   // [14]
        uint32_t __31_15 : 17;                     // [31:15]
    } b;
} REG_LPS_APB_RESET_CAUSE_T;

// cfg_plls
typedef union {
    uint32_t v;
    struct
    {
        uint32_t apll_pd_force : 1;          // [0]
        uint32_t apll_pd_sel : 1;            // [1]
        uint32_t apll_clkout_en_force : 1;   // [2]
        uint32_t apll_clkout_en_sel : 1;     // [3]
        uint32_t apll_clkout_en_mode : 1;    // [4]
        uint32_t apll_aon_sel : 1;           // [5]
        uint32_t apll_pub_sel : 1;           // [6]
        uint32_t apll_cp_sel : 1;            // [7]
        uint32_t apll_ap_sel : 1;            // [8]
        uint32_t mpll_pd_force : 1;          // [9]
        uint32_t mpll_pd_sel : 1;            // [10]
        uint32_t mpll_clkout_en_force : 1;   // [11]
        uint32_t mpll_clkout_en_sel : 1;     // [12]
        uint32_t mpll_clkout_en_mode : 1;    // [13]
        uint32_t mpll_pub_sel : 1;           // [14]
        uint32_t iispll_pd_force : 1;        // [15]
        uint32_t iispll_pd_sel : 1;          // [16]
        uint32_t iispll_clkout_en_force : 1; // [17]
        uint32_t iispll_clkout_en_sel : 1;   // [18]
        uint32_t iispll_clkout_en_mode : 1;  // [19]
        uint32_t bbpll_pd_force : 1;         // [20]
        uint32_t bbpll_pd_sel : 1;           // [21]
        uint32_t __31_22 : 10;               // [31:22]
    } b;
} REG_LPS_APB_CFG_PLLS_T;

// apll_wait_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t apll_rst_low : 5;         // [4:0]
        uint32_t apll_precharge_high : 11; // [15:5]
        uint32_t apll_clkout_en_high : 14; // [29:16]
        uint32_t __31_30 : 2;              // [31:30]
    } b;
} REG_LPS_APB_APLL_WAIT_NUMBER_T;

// mpll_wait_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mpll_rst_low : 5;         // [4:0]
        uint32_t mpll_precharge_high : 11; // [15:5]
        uint32_t mpll_clkout_en_high : 14; // [29:16]
        uint32_t __31_30 : 2;              // [31:30]
    } b;
} REG_LPS_APB_MPLL_WAIT_NUMBER_T;

// iispll_wait_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t iispll_rst_low : 5;         // [4:0]
        uint32_t iispll_precharge_high : 11; // [15:5]
        uint32_t iispll_clkout_en_high : 14; // [29:16]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_LPS_APB_IISPLL_WAIT_NUMBER_T;

// aon_iram_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t aon_iram0_ctrl_mode_sel : 1; // [0]
        uint32_t aon_iram1_ctrl_mode_sel : 1; // [1]
        uint32_t aon_iram2_ctrl_mode_sel : 1; // [2]
        uint32_t __3_3 : 1;                   // [3]
        uint32_t aon_iram0_ctrl_sw : 2;       // [5:4]
        uint32_t aon_iram1_ctrl_sw : 2;       // [7:6]
        uint32_t aon_iram2_ctrl_sw : 2;       // [9:8]
        uint32_t aon_iram0_ctrl_hw : 2;       // [11:10]
        uint32_t aon_iram1_ctrl_hw : 2;       // [13:12]
        uint32_t aon_iram2_ctrl_hw : 2;       // [15:14]
        uint32_t aon_iram0_pu_delay : 1;      // [16], read only
        uint32_t aon_iram1_pu_delay : 1;      // [17], read only
        uint32_t aon_iram2_pu_delay : 1;      // [18], read only
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_LPS_APB_AON_IRAM_CTRL_T;

// iomux_g4_func_sel_latch
typedef union {
    uint32_t v;
    struct
    {
        uint32_t iomux_g4_func_sel_latch : 1; // [0]
        uint32_t __31_1 : 31;                 // [31:1]
    } b;
} REG_LPS_APB_IOMUX_G4_FUNC_SEL_LATCH_T;

// cfg_por_usb_phy
typedef union {
    uint32_t v;
    struct
    {
        uint32_t usb_ps_pd_s : 1;   // [0]
        uint32_t usb_ps_pd_l : 1;   // [1]
        uint32_t usb_iso_sw_en : 1; // [2]
        uint32_t usb_por_rst : 1;   // [3]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_LPS_APB_CFG_POR_USB_PHY_T;

// rc26m_pu_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc26m_pu_ctrl_mode : 1; // [0]
        uint32_t rc26m_pu_sw : 1;        // [1]
        uint32_t __31_2 : 30;            // [31:2]
    } b;
} REG_LPS_APB_RC26M_PU_CTRL_T;

// aon_ahb_lp_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lps_ahb_dslp_ctrl_en : 1;      // [0]
        uint32_t aon_ahb_dslp_fast_ctrl_en : 1; // [1]
        uint32_t aon_ahb_dslp_slow_ctrl_en : 1; // [2]
        uint32_t aon_ahb_dslp_sel : 1;          // [3]
        uint32_t aon_ahb_lslp_ctrl_en_ap : 1;   // [4]
        uint32_t aon_ahb_lslp_ctrl_en_cp : 1;   // [5]
        uint32_t aon_ahb_lslp_sel : 1;          // [6]
        uint32_t __31_7 : 25;                   // [31:7]
    } b;
} REG_LPS_APB_AON_AHB_LP_CTRL_T;

// usb_uart_swj_share_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart_swj_sel : 1;  // [0]
        uint32_t usb_bypass_fs : 1; // [1]
        uint32_t __31_2 : 30;       // [31:2]
    } b;
} REG_LPS_APB_USB_UART_SWJ_SHARE_CFG_T;

// pu_clk26m_lp_iso_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pu_clk26m_lp_iso : 1; // [0]
        uint32_t __31_1 : 31;          // [31:1]
    } b;
} REG_LPS_APB_PU_CLK26M_LP_ISO_CFG_T;

// cfg_io_deep_sleep
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dslp_io_sys0_sw : 1; // [0]
        uint32_t dslp_wp_sys0_sw : 1; // [1]
        uint32_t dslp_io_sys1_sw : 1; // [2]
        uint32_t dslp_wp_sys1_sw : 1; // [3]
        uint32_t dslp_io_sys2 : 1;    // [4]
        uint32_t dslp_wp_sys2 : 1;    // [5]
        uint32_t dslp_io_sys3 : 1;    // [6]
        uint32_t dslp_wp_sys3 : 1;    // [7]
        uint32_t dslp_io_sys4 : 1;    // [8]
        uint32_t dslp_wp_sys4 : 1;    // [9]
        uint32_t dslp_io_sys5 : 1;    // [10]
        uint32_t dslp_wp_sys5 : 1;    // [11]
        uint32_t __15_12 : 4;         // [15:12]
        uint32_t dslp_wp_io_sys0 : 1; // [16]
        uint32_t dslp_wp_io_sys1 : 1; // [17]
        uint32_t __31_18 : 14;        // [31:18]
    } b;
} REG_LPS_APB_CFG_IO_DEEP_SLEEP_T;

// cfg_simc_io
typedef union {
    uint32_t v;
    struct
    {
        uint32_t core_out_sim_0_clk_sw : 1;  // [0]
        uint32_t core_out_sim_0_dio_sw : 1;  // [1]
        uint32_t core_oe_sim_0_dio_sw : 1;   // [2]
        uint32_t core_out_sim_0_rst_sw : 1;  // [3]
        uint32_t core_out_sim_1_clk_sw : 1;  // [4]
        uint32_t core_out_sim_1_dio_sw : 1;  // [5]
        uint32_t core_oe_sim_1_dio_sw : 1;   // [6]
        uint32_t core_out_sim_1_rst_sw : 1;  // [7]
        uint32_t core_out_sim_0_clk_sel : 1; // [8]
        uint32_t core_out_sim_0_dio_sel : 1; // [9]
        uint32_t core_oe_sim_0_dio_sel : 1;  // [10]
        uint32_t core_out_sim_0_rst_sel : 1; // [11]
        uint32_t core_out_sim_1_clk_sel : 1; // [12]
        uint32_t core_out_sim_1_dio_sel : 1; // [13]
        uint32_t core_oe_sim_1_dio_sel : 1;  // [14]
        uint32_t core_out_sim_1_rst_sel : 1; // [15]
        uint32_t __31_16 : 16;               // [31:16]
    } b;
} REG_LPS_APB_CFG_SIMC_IO_T;

// reset_sys_soft
#define LPS_APB_AP_SYS_SOFT_RESET (1 << 0)
#define LPS_APB_CP_SYS_SOFT_RESET (1 << 1)
#define LPS_APB_LTE_SYS_SOFT_RESET (1 << 2)
#define LPS_APB_GNSS_SYS_SOFT_RESET (1 << 3)
#define LPS_APB_USB_SYS_SOFT_RESET (1 << 4)
#define LPS_APB_PUB_SYS_SOFT_RESET (1 << 5)
#define LPS_APB_RF_SYS_SOFT_RESET (1 << 6)
#define LPS_APB_CHIP_SOFT_RESET (1 << 7)

// reset_lps_soft
#define LPS_APB_GPT1_SOFT_RESET (1 << 0)
#define LPS_APB_GPIO1_SOFT_RESET (1 << 1)
#define LPS_APB_KEYPAD_SOFT_RESET (1 << 2)
#define LPS_APB_UART1_SOFT_RESET (1 << 3)
#define LPS_APB_ANA_WRAP3_SOFT_RESET (1 << 4)
#define LPS_APB_IOMUX_G4_SOFT_RESET (1 << 5)
#define LPS_APB_RC26M_CALIB_SOFT_RESET (1 << 6)
#define LPS_APB_RTC_TIMER_SOFT_RESET (1 << 7)

// efuse_por_read_disable
#define LPS_APB_EFUSE_POR_READ_DISABLE (1 << 0)
#define LPS_APB_EFUSE_SEL_FLAG(n) (((n)&0x3) << 1)

// lps_clk_en
#define LPS_APB_GPIO1_EN (1 << 0)
#define LPS_APB_GPT1_EN (1 << 1)
#define LPS_APB_IDLE_LPS_EN (1 << 2)
#define LPS_APB_KEYPAD_EN (1 << 3)
#define LPS_APB_UART1_EN (1 << 4)
#define LPS_APB_PWRCTRL_EN (1 << 5)
#define LPS_APB_RTC_TIMER_EN (1 << 6)
#define LPS_APB_LPS_AHB_AON_EN (1 << 7)

// cfg_clk_uart1
#define LPS_APB_CFG_CLK_UART1_NUM(n) (((n)&0x3ff) << 0)
#define LPS_APB_CFG_CLK_UART1_DEMOD(n) (((n)&0x3fff) << 16)
#define LPS_APB_CFG_CLK_UART1_UPDATE (1 << 31)

// cfg_clk_rc26m
#define LPS_APB_CFG_CLK_RC26M_NUM(n) (((n)&0x3ff) << 0)
#define LPS_APB_CFG_CLK_RC26M_DEMOD(n) (((n)&0x3fff) << 16)
#define LPS_APB_CFG_CLK_RC26M_UPDATE (1 << 31)

// cfg_debug_bond_option
#define LPS_APB_BOND_OPTION_WR_FLAG (1 << 0)
#define LPS_APB_BOND_SWD_JTAG_EN (1 << 1)
#define LPS_APB_BOND_AP_CA5_DAP_DEVICEEN (1 << 2)
#define LPS_APB_BOND_AP_CA5_DBGEN (1 << 3)
#define LPS_APB_BOND_AP_CA5_NIDEN (1 << 4)
#define LPS_APB_BOND_AP_CA5_SPIDEN (1 << 5)
#define LPS_APB_BOND_AP_CA5_SPNIDEN (1 << 6)
#define LPS_APB_BOND_CP_CA5_DAP_DEVICEEN (1 << 7)
#define LPS_APB_BOND_CP_CA5_DBGEN (1 << 8)
#define LPS_APB_BOND_CP_CA5_NIDEN (1 << 9)
#define LPS_APB_BOND_CP_CA5_SPIDEN (1 << 10)
#define LPS_APB_BOND_CP_CA5_SPNIDEN (1 << 11)
#define LPS_APB_BOND_FDMA_EN (1 << 12)
#define LPS_APB_BOND_SWD_DBG_SYS_EN (1 << 13)
#define LPS_APB_BOND_DJTAG_EN (1 << 14)
#define LPS_APB_BOND_PAD_JTAG_EN (1 << 15)
#define LPS_APB_BOND_DBGHOST_EN (1 << 16)
#define LPS_APB_BOND_FDMA_BOOT_CPU_EN (1 << 17)
#define LPS_APB_BOND_SEC_DAP_EN (1 << 18)

// cfg_psram_half_slp
#define LPS_APB_HALF_SLP_REQ (1 << 0)

// cfg_lps_ahb_clock_sel
#define LPS_APB_CGM_LPS_AHB_SEL(n) (((n)&0x3) << 0)

// cfg_uart1_clock_sel
#define LPS_APB_CGM_UART1_BF_DIV_SEL(n) (((n)&0x3) << 0)

// cfg_gpt_lite_clock_sel
#define LPS_APB_CGM_GPT_LITE_SEL(n) (((n)&0x3) << 0)

// cfg_boot_mode
#define LPS_APB_BOOT_MODE_PIN(n) (((n)&0x7) << 0)
#define LPS_APB_BOOT_MODE_SW(n) (((n)&0x3f) << 4)
#define LPS_APB_FUNCTION_TEST_MODE (1 << 10)

// cfg_reset_enable
#define LPS_APB_AP_WDT_RESET_ENABLE (1 << 0)
#define LPS_APB_CP_WDT_RESET_ENABLE (1 << 1)
#define LPS_APB_RF_WDT_RESET_ENABLE (1 << 2)
#define LPS_APB_INVALID_CLK_32K_RESET_ENABLE (1 << 3)
#define LPS_APB_INVALID_CLK_26M_RESET_ENABLE (1 << 4)
#define LPS_APB_AP_MEM_FW_INVALID_RESET_ENABLE (1 << 5)
#define LPS_APB_AON_MEM_FW_INVALID_RESET_ENABLE (1 << 6)
#define LPS_APB_PUB_MEM_FW_INVALID_RESET_ENABLE (1 << 7)
#define LPS_APB_AP_MEM_FW_INVALID_RESET_RAW_ENABLE (1 << 8)
#define LPS_APB_AON_MEM_FW_INVALID_RESET_RAW_ENABLE (1 << 9)
#define LPS_APB_PUB_MEM_FW_INVALID_RESET_RAW_ENABLE (1 << 10)
#define LPS_APB_DBGHOST_RESET_ENABLE (1 << 11)
#define LPS_APB_DBGHOST_RESET_AP_CPU_ENABLE (1 << 12)

// reset_cause
#define LPS_APB_AP_WDT_RESET (1 << 0)
#define LPS_APB_CP_WDT_RESET (1 << 1)
#define LPS_APB_RF_WDT_RESET (1 << 2)
#define LPS_APB_INVALID_CLK_32K_RESET (1 << 3)
#define LPS_APB_INVALID_CLK_26M_RESET (1 << 4)
#define LPS_APB_AP_MEM_FW_INVALID_RESET (1 << 5)
#define LPS_APB_AON_MEM_FW_INVALID_RESET (1 << 6)
#define LPS_APB_PUB_MEM_FW_INVALID_RESET (1 << 7)
#define LPS_APB_AP_MEM_FW_INVALID_RESET_RAW (1 << 8)
#define LPS_APB_AON_MEM_FW_INVALID_RESET_RAW (1 << 9)
#define LPS_APB_PUB_MEM_FW_INVALID_RESET_RAW (1 << 10)
#define LPS_APB_DBGHOST_RESET (1 << 11)
#define LPS_APB_DBGHOST_RESET_AP_CPU (1 << 12)
#define LPS_APB_FUNCDMA_RESET_AP_CPU (1 << 13)
#define LPS_APB_SOFT_RESET (1 << 14)

// cfg_plls
#define LPS_APB_APLL_PD_FORCE (1 << 0)
#define LPS_APB_APLL_PD_SEL (1 << 1)
#define LPS_APB_APLL_CLKOUT_EN_FORCE (1 << 2)
#define LPS_APB_APLL_CLKOUT_EN_SEL (1 << 3)
#define LPS_APB_APLL_CLKOUT_EN_MODE (1 << 4)
#define LPS_APB_APLL_AON_SEL (1 << 5)
#define LPS_APB_APLL_PUB_SEL (1 << 6)
#define LPS_APB_APLL_CP_SEL (1 << 7)
#define LPS_APB_APLL_AP_SEL (1 << 8)
#define LPS_APB_MPLL_PD_FORCE (1 << 9)
#define LPS_APB_MPLL_PD_SEL (1 << 10)
#define LPS_APB_MPLL_CLKOUT_EN_FORCE (1 << 11)
#define LPS_APB_MPLL_CLKOUT_EN_SEL (1 << 12)
#define LPS_APB_MPLL_CLKOUT_EN_MODE (1 << 13)
#define LPS_APB_MPLL_PUB_SEL (1 << 14)
#define LPS_APB_IISPLL_PD_FORCE (1 << 15)
#define LPS_APB_IISPLL_PD_SEL (1 << 16)
#define LPS_APB_IISPLL_CLKOUT_EN_FORCE (1 << 17)
#define LPS_APB_IISPLL_CLKOUT_EN_SEL (1 << 18)
#define LPS_APB_IISPLL_CLKOUT_EN_MODE (1 << 19)
#define LPS_APB_BBPLL_PD_FORCE (1 << 20)
#define LPS_APB_BBPLL_PD_SEL (1 << 21)

// apll_wait_number
#define LPS_APB_APLL_RST_LOW(n) (((n)&0x1f) << 0)
#define LPS_APB_APLL_PRECHARGE_HIGH(n) (((n)&0x7ff) << 5)
#define LPS_APB_APLL_CLKOUT_EN_HIGH(n) (((n)&0x3fff) << 16)

// mpll_wait_number
#define LPS_APB_MPLL_RST_LOW(n) (((n)&0x1f) << 0)
#define LPS_APB_MPLL_PRECHARGE_HIGH(n) (((n)&0x7ff) << 5)
#define LPS_APB_MPLL_CLKOUT_EN_HIGH(n) (((n)&0x3fff) << 16)

// iispll_wait_number
#define LPS_APB_IISPLL_RST_LOW(n) (((n)&0x1f) << 0)
#define LPS_APB_IISPLL_PRECHARGE_HIGH(n) (((n)&0x7ff) << 5)
#define LPS_APB_IISPLL_CLKOUT_EN_HIGH(n) (((n)&0x3fff) << 16)

// aon_iram_ctrl
#define LPS_APB_AON_IRAM0_CTRL_MODE_SEL (1 << 0)
#define LPS_APB_AON_IRAM1_CTRL_MODE_SEL (1 << 1)
#define LPS_APB_AON_IRAM2_CTRL_MODE_SEL (1 << 2)
#define LPS_APB_AON_IRAM0_CTRL_SW(n) (((n)&0x3) << 4)
#define LPS_APB_AON_IRAM1_CTRL_SW(n) (((n)&0x3) << 6)
#define LPS_APB_AON_IRAM2_CTRL_SW(n) (((n)&0x3) << 8)
#define LPS_APB_AON_IRAM0_CTRL_HW(n) (((n)&0x3) << 10)
#define LPS_APB_AON_IRAM1_CTRL_HW(n) (((n)&0x3) << 12)
#define LPS_APB_AON_IRAM2_CTRL_HW(n) (((n)&0x3) << 14)
#define LPS_APB_AON_IRAM0_PU_DELAY (1 << 16)
#define LPS_APB_AON_IRAM1_PU_DELAY (1 << 17)
#define LPS_APB_AON_IRAM2_PU_DELAY (1 << 18)

// iomux_g4_func_sel_latch
#define LPS_APB_IOMUX_G4_FUNC_SEL_LATCH (1 << 0)

// cfg_por_usb_phy
#define LPS_APB_USB_PS_PD_S (1 << 0)
#define LPS_APB_USB_PS_PD_L (1 << 1)
#define LPS_APB_USB_ISO_SW_EN (1 << 2)
#define LPS_APB_USB_POR_RST (1 << 3)

// rc26m_pu_ctrl
#define LPS_APB_RC26M_PU_CTRL_MODE (1 << 0)
#define LPS_APB_RC26M_PU_SW (1 << 1)

// aon_ahb_lp_ctrl
#define LPS_APB_LPS_AHB_DSLP_CTRL_EN (1 << 0)
#define LPS_APB_AON_AHB_DSLP_FAST_CTRL_EN (1 << 1)
#define LPS_APB_AON_AHB_DSLP_SLOW_CTRL_EN (1 << 2)
#define LPS_APB_AON_AHB_DSLP_SEL (1 << 3)
#define LPS_APB_AON_AHB_LSLP_CTRL_EN_AP (1 << 4)
#define LPS_APB_AON_AHB_LSLP_CTRL_EN_CP (1 << 5)
#define LPS_APB_AON_AHB_LSLP_SEL (1 << 6)

// usb_uart_swj_share_cfg
#define LPS_APB_UART_SWJ_SEL (1 << 0)
#define LPS_APB_USB_BYPASS_FS (1 << 1)

// pu_clk26m_lp_iso_cfg
#define LPS_APB_PU_CLK26M_LP_ISO (1 << 0)

// cfg_io_deep_sleep
#define LPS_APB_DSLP_IO_SYS0_SW (1 << 0)
#define LPS_APB_DSLP_WP_SYS0_SW (1 << 1)
#define LPS_APB_DSLP_IO_SYS1_SW (1 << 2)
#define LPS_APB_DSLP_WP_SYS1_SW (1 << 3)
#define LPS_APB_DSLP_IO_SYS2 (1 << 4)
#define LPS_APB_DSLP_WP_SYS2 (1 << 5)
#define LPS_APB_DSLP_IO_SYS3 (1 << 6)
#define LPS_APB_DSLP_WP_SYS3 (1 << 7)
#define LPS_APB_DSLP_IO_SYS4 (1 << 8)
#define LPS_APB_DSLP_WP_SYS4 (1 << 9)
#define LPS_APB_DSLP_IO_SYS5 (1 << 10)
#define LPS_APB_DSLP_WP_SYS5 (1 << 11)
#define LPS_APB_DSLP_WP_IO_SYS0 (1 << 16)
#define LPS_APB_DSLP_WP_IO_SYS1 (1 << 17)

// cfg_simc_io
#define LPS_APB_CORE_OUT_SIM_0_CLK_SW (1 << 0)
#define LPS_APB_CORE_OUT_SIM_0_DIO_SW (1 << 1)
#define LPS_APB_CORE_OE_SIM_0_DIO_SW (1 << 2)
#define LPS_APB_CORE_OUT_SIM_0_RST_SW (1 << 3)
#define LPS_APB_CORE_OUT_SIM_1_CLK_SW (1 << 4)
#define LPS_APB_CORE_OUT_SIM_1_DIO_SW (1 << 5)
#define LPS_APB_CORE_OE_SIM_1_DIO_SW (1 << 6)
#define LPS_APB_CORE_OUT_SIM_1_RST_SW (1 << 7)
#define LPS_APB_CORE_OUT_SIM_0_CLK_SEL (1 << 8)
#define LPS_APB_CORE_OUT_SIM_0_DIO_SEL (1 << 9)
#define LPS_APB_CORE_OE_SIM_0_DIO_SEL (1 << 10)
#define LPS_APB_CORE_OUT_SIM_0_RST_SEL (1 << 11)
#define LPS_APB_CORE_OUT_SIM_1_CLK_SEL (1 << 12)
#define LPS_APB_CORE_OUT_SIM_1_DIO_SEL (1 << 13)
#define LPS_APB_CORE_OE_SIM_1_DIO_SEL (1 << 14)
#define LPS_APB_CORE_OUT_SIM_1_RST_SEL (1 << 15)

#endif // _LPS_APB_H_

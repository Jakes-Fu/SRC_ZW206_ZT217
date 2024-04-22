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

#ifndef _EFUSE_H_
#define _EFUSE_H_

// Auto generated by dtools(see dtools.txt for its version).
// Don't edit it manually!
#undef uint32_t
#define uint32_t uint32
#define REG_EFUSE_CTRL_BASE (0x51200000)

typedef volatile struct
{
    uint32_t __0[2];                // 0x00000000
    uint32_t efuse_all0_index;      // 0x00000008
    uint32_t efuse_mode_ctrl;       // 0x0000000c
    uint32_t efuse_cfg1;            // 0x00000010
    uint32_t efuse_ip_ver;          // 0x00000014
    uint32_t efuse_cfg0;            // 0x00000018
    uint32_t efuse_cfg2;            // 0x0000001c
    uint32_t efuse_ns_en;           // 0x00000020
    uint32_t efuse_ns_err_flag;     // 0x00000024
    uint32_t efuse_ns_flag_clr;     // 0x00000028
    uint32_t efuse_ns_magic_number; // 0x0000002c
    uint32_t __48[4];               // 0x00000030
    uint32_t efuse_s_en;            // 0x00000040
    uint32_t efuse_s_err_flag;      // 0x00000044
    uint32_t efuse_s_flag_clr;      // 0x00000048
    uint32_t efuse_s_magic_number;  // 0x0000004c
    uint32_t efuse_fw_cfg;          // 0x00000050
    uint32_t efuse_pw_swt;          // 0x00000054
    uint32_t __88[4];               // 0x00000058
    uint32_t pw_on_rd_end_flag;     // 0x00000068
    uint32_t ns_s_flag;             // 0x0000006c
    uint32_t por_read_data_sp;      // 0x00000070
    uint32_t por_read_data_sp1;     // 0x00000074
    uint32_t block3;                // 0x00000078
    uint32_t block89;               // 0x0000007c
    uint32_t efuse_enc_bypass_en;   // 0x00000080
} HWP_EFUSE_T;

#define hwp_efuse ((HWP_EFUSE_T *)REG_ACCESS_ADDRESS(REG_EFUSE_CTRL_BASE))

// efuse_all0_index
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_all0_end_index : 16;   // [15:0]
        uint32_t efuse_all0_start_index : 16; // [31:16]
    } b;
} REG_EFUSE_EFUSE_ALL0_INDEX_T;

// efuse_mode_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_all0_check_start : 1; // [0]
        uint32_t __31_1 : 31;                // [31:1]
    } b;
} REG_EFUSE_EFUSE_MODE_CTRL_T;

// efuse_cfg1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tpgm_time_cnt1 : 9; // [8:0]
        uint32_t __15_9 : 7;         // [15:9]
        uint32_t tpgm_time_cnt2 : 9; // [24:16]
        uint32_t __31_25 : 7;        // [31:25]
    } b;
} REG_EFUSE_EFUSE_CFG1_T;

// efuse_ip_ver
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_ip_ver : 16; // [15:0], read only
        uint32_t efuse_type : 2;    // [17:16], read only
        uint32_t __31_18 : 14;      // [31:18]
    } b;
} REG_EFUSE_EFUSE_IP_VER_T;

// efuse_cfg0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tpgm_time_cnt : 9;          // [8:0]
        uint32_t __15_9 : 7;                 // [15:9]
        uint32_t efuse_strobe_low_width : 8; // [23:16]
        uint32_t clk_efs_div : 8;            // [31:24]
    } b;
} REG_EFUSE_EFUSE_CFG0_T;

// efuse_cfg2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t tpgm_time_cnt3 : 9; // [8:0]
        uint32_t __15_9 : 7;         // [15:9]
        uint32_t tpgm_time_bist : 9; // [24:16]
        uint32_t __31_25 : 7;        // [31:25]
    } b;
} REG_EFUSE_EFUSE_CFG2_T;

// efuse_ns_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ns_vdd_en : 1;            // [0]
        uint32_t ns_auto_check_enable : 1; // [1]
        uint32_t double_bit_en_ns : 1;     // [2]
        uint32_t ns_margin_rd_enable : 1;  // [3]
        uint32_t ns_lock_bit_wr_en : 1;    // [4]
        uint32_t __31_5 : 27;              // [31:5]
    } b;
} REG_EFUSE_EFUSE_NS_EN_T;

// efuse_ns_err_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ns_word0_err_flag : 1;  // [0], read only
        uint32_t ns_word1_err_flag : 1;  // [1], read only
        uint32_t __3_2 : 2;              // [3:2]
        uint32_t ns_word0_prot_flag : 1; // [4], read only
        uint32_t ns_word1_prot_flag : 1; // [5], read only
        uint32_t __7_6 : 2;              // [7:6]
        uint32_t ns_pg_en_wr_flag : 1;   // [8], read only
        uint32_t ns_vdd_on_rd_flag : 1;  // [9], read only
        uint32_t ns_block0_rd_flag : 1;  // [10], read only
        uint32_t ns_magnum_wr_flag : 1;  // [11], read only
        uint32_t ns_enk_err_flag : 1;    // [12], read only
        uint32_t ns_all0_check_flag : 1; // [13], read only
        uint32_t __31_14 : 18;           // [31:14]
    } b;
} REG_EFUSE_EFUSE_NS_ERR_FLAG_T;

// efuse_ns_flag_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ns_word0_err_clr : 1;  // [0]
        uint32_t ns_word1_err_clr : 1;  // [1]
        uint32_t __3_2 : 2;             // [3:2]
        uint32_t ns_word0_prot_clr : 1; // [4]
        uint32_t ns_word1_prot_clr : 1; // [5]
        uint32_t __7_6 : 2;             // [7:6]
        uint32_t ns_pg_en_wr_clr : 1;   // [8]
        uint32_t ns_vdd_on_rd_clr : 1;  // [9]
        uint32_t ns_block0_rd_clr : 1;  // [10]
        uint32_t ns_magnum_wr_clr : 1;  // [11]
        uint32_t ns_enk_err_clr : 1;    // [12]
        uint32_t ns_all0_check_clr : 1; // [13]
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_EFUSE_EFUSE_NS_FLAG_CLR_T;

// efuse_ns_magic_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ns_magic_nubmer : 16; // [15:0]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_EFUSE_EFUSE_NS_MAGIC_NUMBER_T;

// efuse_s_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t s_vdd_en : 1;            // [0]
        uint32_t s_auto_check_enable : 1; // [1]
        uint32_t double_bit_en_s : 1;     // [2]
        uint32_t s_margin_rd_enable : 1;  // [3]
        uint32_t s_lock_bit_wr_en : 1;    // [4]
        uint32_t __31_5 : 27;             // [31:5]
    } b;
} REG_EFUSE_EFUSE_S_EN_T;

// efuse_s_err_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t s_word0_err_flag : 1;  // [0], read only
        uint32_t s_word1_err_flag : 1;  // [1], read only
        uint32_t __3_2 : 2;             // [3:2]
        uint32_t s_word0_prot_flag : 1; // [4], read only
        uint32_t s_word1_prot_flag : 1; // [5], read only
        uint32_t __7_6 : 2;             // [7:6]
        uint32_t s_pg_en_wr_flag : 1;   // [8], read only
        uint32_t s_vdd_on_rd_flag : 1;  // [9], read only
        uint32_t s_block0_rd_flag : 1;  // [10], read only
        uint32_t s_magnum_wr_flag : 1;  // [11], read only
        uint32_t s_enk_err_flag : 1;    // [12], read only
        uint32_t s_all0_check_flag : 1; // [13], read only
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_EFUSE_EFUSE_S_ERR_FLAG_T;

// efuse_s_flag_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t s_word0_err_clr : 1;  // [0]
        uint32_t s_word1_err_clr : 1;  // [1]
        uint32_t __3_2 : 2;            // [3:2]
        uint32_t s_word0_prot_clr : 1; // [4]
        uint32_t s_word1_prot_clr : 1; // [5]
        uint32_t __7_6 : 2;            // [7:6]
        uint32_t s_pg_en_wr_clr : 1;   // [8]
        uint32_t s_vdd_on_rd_clr : 1;  // [9]
        uint32_t s_block0_rd_clr : 1;  // [10]
        uint32_t s_magnum_wr_clr : 1;  // [11]
        uint32_t s_enk_err_clr : 1;    // [12]
        uint32_t s_all0_check_clr : 1; // [13]
        uint32_t __31_14 : 18;         // [31:14]
    } b;
} REG_EFUSE_EFUSE_S_FLAG_CLR_T;

// efuse_s_magic_number
typedef union {
    uint32_t v;
    struct
    {
        uint32_t s_magic_nubmer : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_EFUSE_EFUSE_S_MAGIC_NUMBER_T;

// efuse_fw_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t conf_prot : 1;   // [0]
        uint32_t access_prot : 1; // [1]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_EFUSE_EFUSE_FW_CFG_T;

// efuse_pw_swt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efs_enk1_on : 1; // [0]
        uint32_t efs_enk2_on : 1; // [1]
        uint32_t ns_s_pg_en : 1;  // [2]
        uint32_t __31_3 : 29;     // [31:3]
    } b;
} REG_EFUSE_EFUSE_PW_SWT_T;

// pw_on_rd_end_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pw_on_rd_end_flag : 1; // [0], read only
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_EFUSE_PW_ON_RD_END_FLAG_T;

// ns_s_flag
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ns_s_flag : 1; // [0], read only
        uint32_t __31_1 : 31;   // [31:1]
    } b;
} REG_EFUSE_NS_S_FLAG_T;

// efuse_enc_bypass_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_enc_bypass_en : 8; // [7:0]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_EFUSE_EFUSE_ENC_BYPASS_EN_T;

// efuse_all0_index
#define EFUSE_EFUSE_ALL0_END_INDEX(n) (((n)&0xffff) << 0)
#define EFUSE_EFUSE_ALL0_START_INDEX(n) (((n)&0xffff) << 16)

// efuse_mode_ctrl
#define EFUSE_EFUSE_ALL0_CHECK_START (1 << 0)

// efuse_cfg1
#define EFUSE_TPGM_TIME_CNT1(n) (((n)&0x1ff) << 0)
#define EFUSE_TPGM_TIME_CNT2(n) (((n)&0x1ff) << 16)

// efuse_ip_ver
#define EFUSE_EFUSE_IP_VER(n) (((n)&0xffff) << 0)
#define EFUSE_EFUSE_TYPE(n) (((n)&0x3) << 16)

// efuse_cfg0
#define EFUSE_TPGM_TIME_CNT(n) (((n)&0x1ff) << 0)
#define EFUSE_EFUSE_STROBE_LOW_WIDTH(n) (((n)&0xff) << 16)
#define EFUSE_CLK_EFS_DIV(n) (((n)&0xff) << 24)

// efuse_cfg2
#define EFUSE_TPGM_TIME_CNT3(n) (((n)&0x1ff) << 0)
#define EFUSE_TPGM_TIME_BIST(n) (((n)&0x1ff) << 16)

// efuse_ns_en
#define EFUSE_NS_VDD_EN (1 << 0)
#define EFUSE_NS_AUTO_CHECK_ENABLE (1 << 1)
#define EFUSE_DOUBLE_BIT_EN_NS (1 << 2)
#define EFUSE_NS_MARGIN_RD_ENABLE (1 << 3)
#define EFUSE_NS_LOCK_BIT_WR_EN (1 << 4)

// efuse_ns_err_flag
#define EFUSE_NS_WORD0_ERR_FLAG (1 << 0)
#define EFUSE_NS_WORD1_ERR_FLAG (1 << 1)
#define EFUSE_NS_WORD0_PROT_FLAG (1 << 4)
#define EFUSE_NS_WORD1_PROT_FLAG (1 << 5)
#define EFUSE_NS_PG_EN_WR_FLAG (1 << 8)
#define EFUSE_NS_VDD_ON_RD_FLAG (1 << 9)
#define EFUSE_NS_BLOCK0_RD_FLAG (1 << 10)
#define EFUSE_NS_MAGNUM_WR_FLAG (1 << 11)
#define EFUSE_NS_ENK_ERR_FLAG (1 << 12)
#define EFUSE_NS_ALL0_CHECK_FLAG (1 << 13)

// efuse_ns_flag_clr
#define EFUSE_NS_WORD0_ERR_CLR (1 << 0)
#define EFUSE_NS_WORD1_ERR_CLR (1 << 1)
#define EFUSE_NS_WORD0_PROT_CLR (1 << 4)
#define EFUSE_NS_WORD1_PROT_CLR (1 << 5)
#define EFUSE_NS_PG_EN_WR_CLR (1 << 8)
#define EFUSE_NS_VDD_ON_RD_CLR (1 << 9)
#define EFUSE_NS_BLOCK0_RD_CLR (1 << 10)
#define EFUSE_NS_MAGNUM_WR_CLR (1 << 11)
#define EFUSE_NS_ENK_ERR_CLR (1 << 12)
#define EFUSE_NS_ALL0_CHECK_CLR (1 << 13)

// efuse_ns_magic_number
#define EFUSE_NS_MAGIC_NUBMER(n) (((n)&0xffff) << 0)

// efuse_s_en
#define EFUSE_S_VDD_EN (1 << 0)
#define EFUSE_S_AUTO_CHECK_ENABLE (1 << 1)
#define EFUSE_DOUBLE_BIT_EN_S (1 << 2)
#define EFUSE_S_MARGIN_RD_ENABLE (1 << 3)
#define EFUSE_S_LOCK_BIT_WR_EN (1 << 4)

// efuse_s_err_flag
#define EFUSE_S_WORD0_ERR_FLAG (1 << 0)
#define EFUSE_S_WORD1_ERR_FLAG (1 << 1)
#define EFUSE_S_WORD0_PROT_FLAG (1 << 4)
#define EFUSE_S_WORD1_PROT_FLAG (1 << 5)
#define EFUSE_S_PG_EN_WR_FLAG (1 << 8)
#define EFUSE_S_VDD_ON_RD_FLAG (1 << 9)
#define EFUSE_S_BLOCK0_RD_FLAG (1 << 10)
#define EFUSE_S_MAGNUM_WR_FLAG (1 << 11)
#define EFUSE_S_ENK_ERR_FLAG (1 << 12)
#define EFUSE_S_ALL0_CHECK_FLAG (1 << 13)

// efuse_s_flag_clr
#define EFUSE_S_WORD0_ERR_CLR (1 << 0)
#define EFUSE_S_WORD1_ERR_CLR (1 << 1)
#define EFUSE_S_WORD0_PROT_CLR (1 << 4)
#define EFUSE_S_WORD1_PROT_CLR (1 << 5)
#define EFUSE_S_PG_EN_WR_CLR (1 << 8)
#define EFUSE_S_VDD_ON_RD_CLR (1 << 9)
#define EFUSE_S_BLOCK0_RD_CLR (1 << 10)
#define EFUSE_S_MAGNUM_WR_CLR (1 << 11)
#define EFUSE_S_ENK_ERR_CLR (1 << 12)
#define EFUSE_S_ALL0_CHECK_CLR (1 << 13)

// efuse_s_magic_number
#define EFUSE_S_MAGIC_NUBMER(n) (((n)&0xffff) << 0)

// efuse_fw_cfg
#define EFUSE_CONF_PROT (1 << 0)
#define EFUSE_ACCESS_PROT (1 << 1)

// efuse_pw_swt
#define EFUSE_EFS_ENK1_ON (1 << 0)
#define EFUSE_EFS_ENK2_ON (1 << 1)
#define EFUSE_NS_S_PG_EN (1 << 2)

// pw_on_rd_end_flag
#define EFUSE_PW_ON_RD_END_FLAG (1 << 0)

// ns_s_flag
#define EFUSE_NS_S_FLAG (1 << 0)

// efuse_enc_bypass_en
#define EFUSE_EFUSE_ENC_BYPASS_EN(n) (((n)&0xff) << 0)

#define  REGT_FIELD_CHANGE1(reg, type, f1, v1)\
    {\
    type _val;\
    _val.v = reg;\
    _val.b.f1 = v1;\
    reg = _val.v;\
    _val.v;\
  } 

#define REGT_FIELD_CHANGE2(reg, type, f1,v1,f2,v2)\
    { \
    type _val;     \
    _val.v = reg;    \
    _val.b.f1 = v1;                   \
    _val.b.f2 = v2;                   \
    reg = _val.v;                              \
    _val.v;                                    \
}

#define REGT_FIELD_CHANGE3(reg, type, f1,v1,f2,v2,f3,v3) \
{ \
    type _val;     \
    _val.v = reg;       \
    _val.b.f1 = v1;                   \
    _val.b.f2 = v2;                   \
    _val.b.f3 = v3;                   \
    reg = _val.v;                              \
    _val.v;                                    \
}

#undef uint32_t
#endif // _EFUSE_H_

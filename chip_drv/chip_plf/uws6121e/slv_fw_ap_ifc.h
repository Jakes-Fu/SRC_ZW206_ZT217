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

#ifndef _SLV_FW_AP_IFC_H_
#define _SLV_FW_AP_IFC_H_

#undef uint32_t
#define uint32_t uint32
// Auto generated by dtools(see dtools.txt for its version).
// Don't edit it manually!

#define REG_SLV_FW_AP_IFC_BASE (0x51321000)

typedef volatile struct
{
    uint32_t port0_default_address_0; // 0x00000000
    uint32_t port_int_en;             // 0x00000004
    uint32_t port_int_clr;            // 0x00000008
    uint32_t port_int_raw;            // 0x0000000c
    uint32_t port_int_fin;            // 0x00000010
    uint32_t rd_sec_0;                // 0x00000014
    uint32_t wr_sec_0;                // 0x00000018
    uint32_t id0_first_addr_0;        // 0x0000001c
    uint32_t id0_last_addr_0;         // 0x00000020
    uint32_t id0_mstid_0;             // 0x00000024
    uint32_t id0_mstid_1;             // 0x00000028
    uint32_t id0_mstid_2;             // 0x0000002c
    uint32_t id0_mstid_3;             // 0x00000030
    uint32_t id0_mstid_4;             // 0x00000034
    uint32_t id0_mstid_5;             // 0x00000038
    uint32_t id0_mstid_6;             // 0x0000003c
    uint32_t id0_mstid_7;             // 0x00000040
    uint32_t id1_first_addr_0;        // 0x00000044
    uint32_t id1_last_addr_0;         // 0x00000048
    uint32_t id1_mstid_0;             // 0x0000004c
    uint32_t id1_mstid_1;             // 0x00000050
    uint32_t id1_mstid_2;             // 0x00000054
    uint32_t id1_mstid_3;             // 0x00000058
    uint32_t id1_mstid_4;             // 0x0000005c
    uint32_t id1_mstid_5;             // 0x00000060
    uint32_t id1_mstid_6;             // 0x00000064
    uint32_t id1_mstid_7;             // 0x00000068
    uint32_t id2_first_addr_0;        // 0x0000006c
    uint32_t id2_last_addr_0;         // 0x00000070
    uint32_t id2_mstid_0;             // 0x00000074
    uint32_t id2_mstid_1;             // 0x00000078
    uint32_t id2_mstid_2;             // 0x0000007c
    uint32_t id2_mstid_3;             // 0x00000080
    uint32_t id2_mstid_4;             // 0x00000084
    uint32_t id2_mstid_5;             // 0x00000088
    uint32_t id2_mstid_6;             // 0x0000008c
    uint32_t id2_mstid_7;             // 0x00000090
    uint32_t id3_first_addr_0;        // 0x00000094
    uint32_t id3_last_addr_0;         // 0x00000098
    uint32_t id3_mstid_0;             // 0x0000009c
    uint32_t id3_mstid_1;             // 0x000000a0
    uint32_t id3_mstid_2;             // 0x000000a4
    uint32_t id3_mstid_3;             // 0x000000a8
    uint32_t id3_mstid_4;             // 0x000000ac
    uint32_t id3_mstid_5;             // 0x000000b0
    uint32_t id3_mstid_6;             // 0x000000b4
    uint32_t id3_mstid_7;             // 0x000000b8
    uint32_t id4_first_addr_0;        // 0x000000bc
    uint32_t id4_last_addr_0;         // 0x000000c0
    uint32_t id4_mstid_0;             // 0x000000c4
    uint32_t id4_mstid_1;             // 0x000000c8
    uint32_t id4_mstid_2;             // 0x000000cc
    uint32_t id4_mstid_3;             // 0x000000d0
    uint32_t id4_mstid_4;             // 0x000000d4
    uint32_t id4_mstid_5;             // 0x000000d8
    uint32_t id4_mstid_6;             // 0x000000dc
    uint32_t id4_mstid_7;             // 0x000000e0
    uint32_t id5_first_addr_0;        // 0x000000e4
    uint32_t id5_last_addr_0;         // 0x000000e8
    uint32_t id5_mstid_0;             // 0x000000ec
    uint32_t id5_mstid_1;             // 0x000000f0
    uint32_t id5_mstid_2;             // 0x000000f4
    uint32_t id5_mstid_3;             // 0x000000f8
    uint32_t id5_mstid_4;             // 0x000000fc
    uint32_t id5_mstid_5;             // 0x00000100
    uint32_t id5_mstid_6;             // 0x00000104
    uint32_t id5_mstid_7;             // 0x00000108
    uint32_t clk_gate_bypass;         // 0x0000010c
} HWP_SLV_FW_AP_IFC_T;

#define hwp_slvFwApIfc ((HWP_SLV_FW_AP_IFC_T *)REG_ACCESS_ADDRESS(REG_SLV_FW_AP_IFC_BASE))

// port0_default_address_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t port0_default_address_0 : 16; // [15:0]
        uint32_t __31_16 : 16;                 // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_PORT0_DEFAULT_ADDRESS_0_T;

// port_int_en
typedef union {
    uint32_t v;
    struct
    {
        uint32_t port_0_w_en : 1; // [0]
        uint32_t port_0_r_en : 1; // [1]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_SLV_FW_AP_IFC_PORT_INT_EN_T;

// port_int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t port_0_w_clr : 1; // [0], write clear
        uint32_t port_0_r_clr : 1; // [1], write clear
        uint32_t __31_2 : 30;      // [31:2]
    } b;
} REG_SLV_FW_AP_IFC_PORT_INT_CLR_T;

// port_int_raw
typedef union {
    uint32_t v;
    struct
    {
        uint32_t port_0_w_raw : 1; // [0], read only
        uint32_t port_0_r_raw : 1; // [1], read only
        uint32_t __31_2 : 30;      // [31:2]
    } b;
} REG_SLV_FW_AP_IFC_PORT_INT_RAW_T;

// port_int_fin
typedef union {
    uint32_t v;
    struct
    {
        uint32_t port_0_w_fin : 1; // [0], read only
        uint32_t port_0_r_fin : 1; // [1], read only
        uint32_t __31_2 : 30;      // [31:2]
    } b;
} REG_SLV_FW_AP_IFC_PORT_INT_FIN_T;

// rd_sec_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_ifc_rd_sec : 2; // [1:0]
        uint32_t camera_rd_sec : 2; // [3:2]
        uint32_t sdmmc_rd_sec : 2;  // [5:4]
        uint32_t uart6_rd_sec : 2;  // [7:6]
        uint32_t uart5_rd_sec : 2;  // [9:8]
        uint32_t uart4_rd_sec : 2;  // [11:10]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_SLV_FW_AP_IFC_RD_SEC_0_T;

// wr_sec_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ap_ifc_wr_sec : 2; // [1:0]
        uint32_t camera_wr_sec : 2; // [3:2]
        uint32_t sdmmc_wr_sec : 2;  // [5:4]
        uint32_t uart6_wr_sec : 2;  // [7:6]
        uint32_t uart5_wr_sec : 2;  // [9:8]
        uint32_t uart4_wr_sec : 2;  // [11:10]
        uint32_t __31_12 : 20;      // [31:12]
    } b;
} REG_SLV_FW_AP_IFC_WR_SEC_0_T;

// id0_first_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID0_FIRST_ADDR_0_T;

// id0_last_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t last_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID0_LAST_ADDR_0_T;

// id1_first_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID1_FIRST_ADDR_0_T;

// id1_last_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t last_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID1_LAST_ADDR_0_T;

// id2_first_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID2_FIRST_ADDR_0_T;

// id2_last_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t last_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID2_LAST_ADDR_0_T;

// id3_first_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID3_FIRST_ADDR_0_T;

// id3_last_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t last_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID3_LAST_ADDR_0_T;

// id4_first_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID4_FIRST_ADDR_0_T;

// id4_last_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t last_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID4_LAST_ADDR_0_T;

// id5_first_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID5_FIRST_ADDR_0_T;

// id5_last_addr_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t last_addr_0 : 16; // [15:0]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_SLV_FW_AP_IFC_ID5_LAST_ADDR_0_T;

// clk_gate_bypass
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_gate_bypass : 1; // [0]
        uint32_t fw_resp_en : 1;      // [1]
        uint32_t __31_2 : 30;         // [31:2]
    } b;
} REG_SLV_FW_AP_IFC_CLK_GATE_BYPASS_T;

// port0_default_address_0
#define SLV_FW_AP_IFC_PORT0_DEFAULT_ADDRESS_0(n) (((n)&0xffff) << 0)

// port_int_en
#define SLV_FW_AP_IFC_PORT_0_W_EN (1 << 0)
#define SLV_FW_AP_IFC_PORT_0_R_EN (1 << 1)

// port_int_clr
#define SLV_FW_AP_IFC_PORT_0_W_CLR (1 << 0)
#define SLV_FW_AP_IFC_PORT_0_R_CLR (1 << 1)

// port_int_raw
#define SLV_FW_AP_IFC_PORT_0_W_RAW (1 << 0)
#define SLV_FW_AP_IFC_PORT_0_R_RAW (1 << 1)

// port_int_fin
#define SLV_FW_AP_IFC_PORT_0_W_FIN (1 << 0)
#define SLV_FW_AP_IFC_PORT_0_R_FIN (1 << 1)

// rd_sec_0
#define SLV_FW_AP_IFC_AP_IFC_RD_SEC(n) (((n)&0x3) << 0)
#define SLV_FW_AP_IFC_CAMERA_RD_SEC(n) (((n)&0x3) << 2)
#define SLV_FW_AP_IFC_SDMMC_RD_SEC(n) (((n)&0x3) << 4)
#define SLV_FW_AP_IFC_UART6_RD_SEC(n) (((n)&0x3) << 6)
#define SLV_FW_AP_IFC_UART5_RD_SEC(n) (((n)&0x3) << 8)
#define SLV_FW_AP_IFC_UART4_RD_SEC(n) (((n)&0x3) << 10)

// wr_sec_0
#define SLV_FW_AP_IFC_AP_IFC_WR_SEC(n) (((n)&0x3) << 0)
#define SLV_FW_AP_IFC_CAMERA_WR_SEC(n) (((n)&0x3) << 2)
#define SLV_FW_AP_IFC_SDMMC_WR_SEC(n) (((n)&0x3) << 4)
#define SLV_FW_AP_IFC_UART6_WR_SEC(n) (((n)&0x3) << 6)
#define SLV_FW_AP_IFC_UART5_WR_SEC(n) (((n)&0x3) << 8)
#define SLV_FW_AP_IFC_UART4_WR_SEC(n) (((n)&0x3) << 10)

// id0_first_addr_0
#define SLV_FW_AP_IFC_FIRST_ADDR_0(n) (((n)&0xffff) << 0)

// id0_last_addr_0
#define SLV_FW_AP_IFC_LAST_ADDR_0(n) (((n)&0xffff) << 0)

// id1_first_addr_0
#define SLV_FW_AP_IFC_FIRST_ADDR_0(n) (((n)&0xffff) << 0)

// id1_last_addr_0
#define SLV_FW_AP_IFC_LAST_ADDR_0(n) (((n)&0xffff) << 0)

// id2_first_addr_0
#define SLV_FW_AP_IFC_FIRST_ADDR_0(n) (((n)&0xffff) << 0)

// id2_last_addr_0
#define SLV_FW_AP_IFC_LAST_ADDR_0(n) (((n)&0xffff) << 0)

// id3_first_addr_0
#define SLV_FW_AP_IFC_FIRST_ADDR_0(n) (((n)&0xffff) << 0)

// id3_last_addr_0
#define SLV_FW_AP_IFC_LAST_ADDR_0(n) (((n)&0xffff) << 0)

// id4_first_addr_0
#define SLV_FW_AP_IFC_FIRST_ADDR_0(n) (((n)&0xffff) << 0)

// id4_last_addr_0
#define SLV_FW_AP_IFC_LAST_ADDR_0(n) (((n)&0xffff) << 0)

// id5_first_addr_0
#define SLV_FW_AP_IFC_FIRST_ADDR_0(n) (((n)&0xffff) << 0)

// id5_last_addr_0
#define SLV_FW_AP_IFC_LAST_ADDR_0(n) (((n)&0xffff) << 0)

// clk_gate_bypass
#define SLV_FW_AP_IFC_CLK_GATE_BYPASS (1 << 0)
#define SLV_FW_AP_IFC_FW_RESP_EN (1 << 1)

#undef uint32_t

#endif // _SLV_FW_AP_IFC_H_

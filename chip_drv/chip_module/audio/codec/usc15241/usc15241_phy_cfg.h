/******************************************************************************
 ** File Name:      usc15241_phy_cfg.h                                         *
 ** Author:         ken.kuang                                                 *
 ** DATE:           12/12/2011                                                *
 ** Copyright:      2011 Spreadtrum, Incoporated. All Rights Reserved.        *
 ** Description:    Rigister definition for physical layer.                   *
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE           NAME             DESCRIPTION                               *
 ** 12/12/2011     ken.kuang          Create.                                   *
 *****************************************************************************/

#ifndef __USC15241_PHY_CFG_H__
#define __USC15241_PHY_CFG_H__

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern   "C"
{
#endif

/**---------------------------------------------------------------------------*
 **                         Macro defination                                  *
 **---------------------------------------------------------------------------*/
#define USC15241_PGA_GOD             (0x0001)        //4bit
#define USC15241_PGA_GOB             (0x0002)        //5bit
#define USC15241_PGA_GO              (0x0004)        //5bit
#define USC15241_PGA_GOBTL           (0x0008)        //1bit
#define USC15241_PGA_GI              (0x0010)        //4bit
#define USC15241_PGA_GIM             (0x0020)        //1bit
#define USC15241_PGA_GOLR            (0x0040)        //3bit

#define USC15241_PGA_MAX_BIT         (7)

/**---------------------------------------------------------------------------*
 **                         Data Structures                                   *
 **---------------------------------------------------------------------------*/

typedef struct
{
    uint32                  mode;
    int32                   (*pFun)(AUDIO_CTL_PGA_T*);
} USC15241_PGA_FUN_T;

/**---------------------------------------------------------------------------*
 **                         Local variables                                   *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                         Function Prototypes                               *
 **---------------------------------------------------------------------------*/
PUBLIC USC15241_PGA_FUN_T* USC15241_Get_PGA_OPS(uint32 param);
PUBLIC int32 USC15241_PGA_Init_Set(uint32 param);
PUBLIC int32 USC15241_PGA_Set(AUDIO_CTL_PGA_T * pga);

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif //__USC15241_PHY_CFG_H__

/*****************************************************************************
** File Name:       watch_common.h                                           *
** Author:           fangfang.yao                                            *
** Date:             02/20/2020                                              *
** Copyright:                                                                *
** Description:    This file is used to define common part                   *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE                 NAME                  DESCRIPTION                    *
** 02/07/2020           fangfang.yao          Create                         *
******************************************************************************/
#ifndef _WATCH_COMMON_H_
#define _WATCH_COMMON_H_

#include "mmk_type.h"

#ifdef ZTE_SUPPORT_240X284
#ifdef ZTE_WATCH
#define SET_PROGRESSBAR_RECT DP2PX_RECT(12,112, 228, 133)
#define SET_PROGRESSBAR_TOUCHRECT DP2PX_RECT(5,100, 235, 145)
#define WATCH_SETOK_BTN_RECT     DP2PX_RECT(12,196,228,236)
#define SET_BTN_TWO_LEFT_RECT     DP2PX_RECT(5,204,110,254)
#define SET_BTN_TWO_RIGHT_RECT    DP2PX_RECT(130,204,240,254)

#define SET_PROGRESSBAR_TOTAL_LENGTH    216
#define SET_PROGRESSBAR_PERLEVEL_LENGTH 36
#define PROGRESSBAR_BRIGTH_LEVEL_MAX 6
#define BRIGHT_ICON_B_X	90
#define BRIGHT_ICON_B_Y	30
#define BRIGHT_ICON_VOL_X	102
#define BRIGHT_ICON_VOL_Y	57

#define CERTIFIATION_BG_WIDTH 206
#define CERTIFIATION_BG_HEIGHT 104
#define CERTIFIATION_BG_X   ((MMI_MAINSCREEN_WIDTH -CERTIFIATION_BG_WIDTH)/2)
#define CERTIFIATION_BG_Y   ((MMI_MAINSCREEN_HEIGHT -CERTIFIATION_BG_HEIGHT)/2+MMI_SPECIAL_TITLE_HEIGHT-30)
#define CERTIFIATION_BG_RECT DP2PX_RECT(CERTIFIATION_BG_X,CERTIFIATION_BG_Y, CERTIFIATION_BG_X+CERTIFIATION_BG_WIDTH, CERTIFIATION_BG_Y+CERTIFIATION_BG_HEIGHT)
#define CERTIFIATION_MODEL_RECT DP2PX_RECT(CERTIFIATION_BG_X+5,CERTIFIATION_BG_Y+40, CERTIFIATION_BG_X+CERTIFIATION_BG_WIDTH-5, CERTIFIATION_BG_Y+75)
	

#else
#define SET_BTN_TWO_LEFT_RECT     DP2PX_RECT(5,234,110,284)
#define SET_BTN_TWO_RIGHT_RECT    DP2PX_RECT(130,234,240,284)

#define SET_BTN_MINUS_RECT     DP2PX_RECT(22,195,80,250)
#define SET_BTN_PLUS_RECT    DP2PX_RECT(168,195,220,250)

#define BRIGHT_IMAGE_X	66
#define BRIGHT_IMAGE_Y	80

#define BRIGHT_ICON_X	104
#define BRIGHT_ICON_Y	205
#endif

#else

#ifdef ZTE_WATCH
#define SETOK_BTN_HEIGHT 40
#define SET_PROGRESSBAR_RECT DP2PX_RECT(12,112, 228, 133)
#define SET_PROGRESSBAR_TOUCHRECT DP2PX_RECT(5,100, 235, 145)
#define WATCH_SETOK_BTN_RECT     DP2PX_RECT(12,MMI_MAINSCREEN_HEIGHT - SETOK_BTN_HEIGHT-20,228,MMI_MAINSCREEN_HEIGHT -20)

#define SET_PROGRESSBAR_TOTAL_LENGTH    216
#define SET_PROGRESSBAR_PERLEVEL_LENGTH 36
#define PROGRESSBAR_BRIGTH_LEVEL_MAX 6
#define BRIGHT_ICON_B_X	90
#define BRIGHT_ICON_B_Y	20
#define BRIGHT_ICON_VOL_X	102
#define BRIGHT_ICON_VOL_Y	37
#define SET_BTN_TWO_LEFT_RECT     DP2PX_RECT(5,190,110,240)
#define SET_BTN_TWO_RIGHT_RECT    DP2PX_RECT(130,190,240,240)

#define CERTIFIATION_BG_WIDTH 206
#define CERTIFIATION_BG_HEIGHT 104
#define CERTIFIATION_BG_X   ((MMI_MAINSCREEN_WIDTH -CERTIFIATION_BG_WIDTH)/2)
#define CERTIFIATION_BG_Y   ((MMI_MAINSCREEN_HEIGHT -CERTIFIATION_BG_HEIGHT)/2+MMI_SPECIAL_TITLE_HEIGHT-30)
#define CERTIFIATION_BG_RECT DP2PX_RECT(CERTIFIATION_BG_X,CERTIFIATION_BG_Y, CERTIFIATION_BG_X+CERTIFIATION_BG_WIDTH, CERTIFIATION_BG_Y+CERTIFIATION_BG_HEIGHT)
#define CERTIFIATION_MODEL_RECT DP2PX_RECT(CERTIFIATION_BG_X+5,CERTIFIATION_BG_Y+40, CERTIFIATION_BG_X+CERTIFIATION_BG_WIDTH-5, CERTIFIATION_BG_Y+75)


#else
#define SET_BTN_TWO_LEFT_RECT     DP2PX_RECT(5,190,110,240)
#define SET_BTN_TWO_RIGHT_RECT    DP2PX_RECT(130,190,240,240)

#define SET_BTN_MINUS_RECT     DP2PX_RECT(22,155,80,210)
#define SET_BTN_PLUS_RECT    DP2PX_RECT(168,155,220,210)

#define BRIGHT_IMAGE_X	66
#define BRIGHT_IMAGE_Y	54

#define BRIGHT_ICON_X	104
#define BRIGHT_ICON_Y	165
#endif

#endif

#ifdef ZTE_WATCH
    #ifdef ZTE_WATCH_K1
        #define WATCH_NAME      "ZTE Watch Z12"
        #define WATCH_CTA_MODEL      "LT-T01" //"ZW35"
        #define WATCH_CMIIT_ID      "  " //"CMIIT ID:2023CP18922"
    #elif ZTE_WATCH_K1_ENTRY
        #define WATCH_NAME      "ZTE Watch Z11"
        #define WATCH_CTA_MODEL      "LT-T01" //"ZW22"
        #define WATCH_CMIIT_ID      "  " //"CMIIT ID:2023CP19082"
    #else
        #define WATCH_NAME      "ZTE Watch"
    #endif
#endif


/*****************************************************************************/
//  Description : draw window background
//  Parameter: [In] win_id: the list control ID
//             [Out] None
//             [Return] None
//  Author: fangfang.yao
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_DisplayBackground( MMI_WIN_ID_T win_id );

PUBLIC void WATCHCOM_DisplayTitle( MMI_WIN_ID_T win_id, MMI_TEXT_ID_T title_id);

PUBLIC void WATCHCOM_DisplayTips( MMI_WIN_ID_T win_id, MMI_TEXT_ID_T title_id);
/*****************************************************************************/
//  Description : fill rounded rect 填充一个圆角矩形
//  Parameter: [In] win_id: 
//             [in] rect:矩形区域
//             [in] radius:圆角半径
//             [in] color:填充颜色
//             [Return] None
//  Author: deng
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_FillRoundedRect(MMI_WIN_ID_T win_id, GUI_RECT_T rect, uint16 radius, GUI_COLOR_T color);
/*****************************************************************************/
//  Description : 画个进度条
//  Parameter: [In] win_id: 
//             [in] rect:进度条区域
//             [in] percent:百分比
//             [in] bg_img_id:背景图片
//             [in] fg_img_id:前景图片
//             [in] hand_img_id:滑块图片 可以为空
//             [Return] None
//  Author: fangfang.yao
//  Note: 如果滑块图片高度高于进度条,窗口背景不是黑色;则会看到填充的黑色背景
/*****************************************************************************/
PUBLIC void WATCHCOM_ProgressBar(MMI_WIN_ID_T win_id, GUI_RECT_T rect, uint8 percent, MMI_IMAGE_ID_T bg_img_id, MMI_IMAGE_ID_T fg_img_id, MMI_IMAGE_ID_T hand_img_id);
/*****************************************************************************/
//  Description : 画个进度条
//  Parameter: [In] win_id: 
//             [in] rect:进度条区域
//             [in] cur_index:当前所处条序号
//             [in] total_item_num:总条数
//             [in] fg_img_id:前景图片
//             [in] hand_img_id:滑块图片 可以为空
//             [Return] None
//  Author: fangfang.yao
//  Note: 如果滑块图片高度高于进度条,窗口背景不是黑色;则会看到填充的黑色背景
/*****************************************************************************/
PUBLIC void WATCHCOM_ProgressBarByIndex(MMI_WIN_ID_T win_id, GUI_RECT_T rect, uint32 cur_index, uint32 total_item_num,MMI_IMAGE_ID_T bg_img_id, MMI_IMAGE_ID_T fg_img_id, MMI_IMAGE_ID_T hand_img_id);

PUBLIC MMI_IMAGE_ID_T WATCHCOM_GetAvaterImageId(wchar *name);
#ifdef ZTE_WATCH
PUBLIC MMI_IMAGE_ID_T WATCHCOM_GetAvaterBigImageId(wchar *name);
PUBLIC uint8* WATCHCOM_GetDeviceModel();
PUBLIC uint8* WATCHCOM_GetSoftwareVersion();
#endif

PUBLIC void WATCHCOM_Backlight(BOOLEAN is_alway_on);

#define WINDOW_TITLE_RECT {0,0,240,30}

#define WINDOW_BACK_RECT {0,0,45,MMI_SPECIAL_TITLE_HEIGHT}

#endif


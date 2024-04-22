/*****************************************************************************
** File Name:      watch_charge_win.h                                           *
** Author:                                                                                           *
** Date:           03/16/2020                                                                  *
** Copyright:      2020 Spreadtrum, Incoporated. All Rights Reserved.         *
** Description:    edit clock win
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 03/16/2020  zhikun.lv        Create
******************************************************************************/

#ifndef WATCH_CHARGE_WIN_H
#define WATCH_CHARGE_WIN_H
#include "mmi_common.h"
#include "mmi_nv.h"
#include "mmidisplay_data.h"
#include "window_parse.h"
#include "guires.h"
#include "mmk_timer.h"

#undef SCREEN_SHAPE_CIRCULAR

#ifdef ADULT_WATCH_SUPPORT
#define LAUNCHER_CHARGE_DEFAULT_FONT_SIZE DP_FONT_40
#define LAUNCHER_CHARGE_ANIM_DURATION 500
#define LAUNCHER_CHARGE_TIP_DURATION 1500
#define LAUNCHER_CHARGE_BACKGROUND_COLOR MMI_BLACK_COLOR

#define LAUNCHER_CHARGE_VALUE_RECT DP2PX_RECT(0, 96, 240, 96+48)
#define LAUNCHER_CHARGE_ICON_RECT DP2PX_RECT(104, 152, 104+32, 152+32)
#define LAUNCHER_CHARGE_ANIM_RECT DP2PX_RECT(0, 0, 240, 240)
#define LAUNCHER_CHARGE_WIN_RECT DP2PX_RECT(0, 0, 240, 240)
#else
#define LAUNCHER_CHARGE_DEFAULT_FONT_SIZE SONG_FONT_24
#define LAUNCHER_CHARGE_TIP_DURATION 1500
#define LAUNCHER_CHARGE_BACKGROUND_COLOR MMI_BLACK_COLOR

#ifdef SCREEN_SHAPE_CIRCULAR
#define LAUNCHER_CHARGE_IMAGE_RECT DP2PX_RECT(0, 0, 240, 240)
#define LAUNCHER_CHARGE_VALUE_RECT DP2PX_RECT(0, 160, 240, 220)
#define LAUNCHER_CHARGE_ANIM_RECT DP2PX_RECT(80, 80, 160, 160)
#define LAUNCHER_CHARGE_WIN_RECT DP2PX_RECT(0, 0, 240, 240)
#define LAUNCHER_BATTERYLOW_TIP_RECT DP2PX_RECT(0, 160, 240, 200)
#define LAUNCHER_BATTERYLOW_IMAGE_RECT DP2PX_RECT(66, 52, 174, 160)
#ifdef CHGMNG_PSE_SUPPORT
#define LAUNCHER_CHARGE_TEMP_RECT DP2PX_RECT(0, 190, 240, 230)
#endif
#else
#define LAUNCHER_CHARGE_STATE_RECT {0, 8, 240, 40}
#define LAUNCHER_CHARGE_VALUE_RECT {0, 42, 240, 62}
#define LAUNCHER_CHARGE_IMAGE_RECT {70, 68, 170, 208}
#define LAUNCHER_BATTERYLOW_IMAGE_RECT {84, 22, 84+72, 22+72}
#define LAUNCHER_BATTERYLOW_TIP_RECT {12, 22+72+12, 228, 240-72}
#ifdef CHGMNG_PSE_SUPPORT
#define LAUNCHER_CHARGE_TEMP_RECT DP2PX_RECT(0, 210, 240, 235)
#endif
#endif
#define LAUNCHER_BATTERYLOW_CONFIRM_RECT {12, 240-12 -48, 228, 240-12}
#endif

#ifdef ZTE_WATCH
#define MMIPHONE_CHARGE_WARNING_CAP_TWENTY     20  //电量20%提示一次低电量
#define MMIPHONE_CHARGE_WARNING_CAP_LOW_POWER     20   // 20%进入长续航模式
#endif

/*****************************************************************************
//  Description : open charge win
//  Author: zhikun.lv
//  Param:N/A
//  Return:
//  Note:
*****************************************************************************/
PUBLIC void WatchLAUNCHER_Charge_Enter();

/*****************************************************************************
//  Description : open charge tip win
//  Author: zhikun.lv
//  Param:N/A
//  Return:
//  Note:
*****************************************************************************/
PUBLIC void WatchLAUNCHER_ChargeTip_Enter();

/*****************************************************************************
//  Description : close charge tip win
//  Author:
//  Param:N/A
//  Return:
//  Note:
*****************************************************************************/
PUBLIC void WatchLAUNCHER_ChargeTip_Exist();

/*****************************************************************************
//  Description : open low battery tip win
//  Author: zhikun.lv
//  Param:N/A
//  Return:
//  Note:
*****************************************************************************/
PUBLIC void WatchLAUNCHER_BatteryLow_Enter();
#endif

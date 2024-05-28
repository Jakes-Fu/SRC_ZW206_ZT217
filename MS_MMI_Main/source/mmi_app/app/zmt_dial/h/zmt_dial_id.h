/*****************************************************************************
** File Name:      zmt_dial_id.h                                            *
** Author:           fys                                                        *
** Date:           2024/05/08                                                 *
** Copyright:       *
** Description:                       *
******************************************************************************

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "mmi_module.h"
#include "mmk_type.h"
#include "os_api.h"
#include "sci_types.h"


/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/

#define WIN_ID_DEF(win_id) win_id

// window ID
typedef enum {
    ZMT_DIAL_WIN_ID_START = (MMI_MODULE_ZMT_DIAL << 16),

    ZMT_DIAL_STORE_WIN_ID,
    ZMT_DIAL_STORE_PREVIEW_1_WIN_ID,
    ZMT_DIAL_STORE_PREVIEW_2_WIN_ID,
  
    ZMT_DIAL_MAX_WIN_ID

} ZMT_DIAL_WINDOW_ID_E;

#undef WIN_ID_DEF

// control ID
typedef enum {
    ZMT_DIAL_WIN_CTRL_ID_START = ZMT_DIAL_MAX_WIN_ID,

    ZMT_DIAL_STORE_MAIN_WIN_LIST_CTRL_ID,
    ZMT_DIAL_STORE_PREVIEW_BUTTON_CTRL_ID,
    ZMT_DIAL_STORE_PREVIEW_FORM_CTRL_ID,
    ZMT_DIAL_STORE_PREVIEW_IMG_1_CTRL_ID,
    ZMT_DIAL_STORE_PREVIEW_IMG_2_CTRL_ID,
    ZMT_DIAL_STORE_PREVIEW_ICONLIST_CTRL_ID,

    ZMT_DIAL_WIN_BG_CTRL_ID,
    ZMT_DIAL_WIN_ANALOGTIME_CTRL_ID,
    ZMT_DIAL_WIN_HOUR_TEN_CTRL_ID,
    ZMT_DIAL_WIN_HOUR_UNIT_CTRL_ID,
    ZMT_DIAL_WIN_MIN_TEN_CTRL_ID,
    ZMT_DIAL_WIN_MIN_UNIT_CTRL_ID,

    ZMT_DIAL_WIN_WEEK_DIGIT_CTRL_ID,
    ZMT_DIAL_WIN_WEEK_HAND_CTRL_ID,

    ZMT_DIAL_WIN_BATTERY_SIGN_CTRL_ID,
    ZMT_DIAL_WIN_SIGNAL_SIGN_CTRL_ID,
  
    ZMT_DIAL_MAX_WIN_CTRL_ID

} ZMT_DIAL_CONTROL_ID_E;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif


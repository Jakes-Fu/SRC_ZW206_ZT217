/*****************************************************************************
** File Name:      zmt_class_id.h                                               *
** Author:         fys                                               *
** Date:           2024/06/27                                                *
******************************************************************************/

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
#define CLASS_READ_CTRL_ID_MAX 50

#define WIN_ID_DEF(win_id) win_id

// window ID
typedef enum {
    MMIZMT_CLASS_WIN_ID_START = (MMI_MODULE_ZMT_CLASS << 16),

    ZMT_CLASS_MAIN_WIN_ID,
    ZMT_CLASS_GRADE_WIN_ID,
    ZMT_CLASS_BOOK_WIN_ID,
    ZMT_CLASS_SECTION_WIN_ID,
    ZMT_CLASS_READ_WIN_ID,
    ZMT_CLASS_READ_SET_WIN_ID,

    MMIZMT_CLASS_MAX_WIN_ID
} MMIZMT_CLASS_WINDOW_ID_E;

#undef WIN_ID_DEF

// control ID
typedef enum {
    MMIZMT_CLASS_CTRL_ID_START = MMIZMT_CLASS_MAX_WIN_ID,

    ZMT_CLASS_MAIN_LIST_CTRL_ID,
    ZMT_CLASS_GRADE_ICONLIST_CTRL_ID,
    ZMT_CLASS_BOOK_LIST_CTRL_ID,
    ZMT_CLASS_SECTION_LIST_CTRL_ID,
    ZMT_CLASS_SECTION_LEFT_BUTTON_CTRL_ID,
    ZMT_CLASS_SECTION_RIGHT_BUTTON_CTRL_ID,

    ZMT_CLASS_READ_PLAY_CTRL_ID,
    ZMT_CLASS_READ_SIGLE_CTRL_ID,
    ZMT_CLASS_READ_SETTING_CTRL_ID,
    ZMT_CLASS_READ_FORM_CTRL_ID,
    ZMT_CLASS_READ_FORM_TEXT_CTRL_ID,
    ZMT_CLASS_READ_MAX_CTRL_ID = ZMT_CLASS_READ_FORM_TEXT_CTRL_ID + CLASS_READ_CTRL_ID_MAX,

    ZMT_CLASS_READ_SET_LABEL_1_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_1_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_2_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_3_CTRL_ID,
    ZMT_CLASS_READ_SET_LABEL_2_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_4_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_5_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_6_CTRL_ID,
    ZMT_CLASS_READ_SET_LABEL_3_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_7_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_8_CTRL_ID,
    ZMT_CLASS_READ_SET_BUTTON_9_CTRL_ID,
    
    MMIZMT_CLASS_MAX_CTRL_ID
} MMIZMT_CLASS_CONTROL_ID_E;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif


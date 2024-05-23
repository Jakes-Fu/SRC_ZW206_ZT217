/*****************************************************************************
** File Name:      zmt_gpt_id.h                                            *
** Author:           fys                                                        *
** Date:           2024/05/16                                                 *
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
    ZMT_GPT_WIN_ID_START = (MMI_MODULE_ZMT_GPT << 16),

    ZMT_GPT_MAIN_WIN_ID,

    ZMT_GPT_ZUOWEN_WIN_ID,

    ZMT_GPT_KOUYU_TALK_WIN_ID,
    ZMT_GPT_KOUYU_TOPIC_WIN_ID,
  
    ZMT_GPT_MAX_WIN_ID

} ZMT_GPT_WINDOW_ID_E;

#undef WIN_ID_DEF

// control ID
typedef enum {
    ZMT_GPT_CTRL_ID_START = ZMT_GPT_MAX_WIN_ID,

    ZMT_GPT_MAIN_LIST_CTRL_ID,

    ZMT_GPT_ZUOWEN_LIST_CTRL_ID,

    ZMT_GPT_KOUYU_TOPIC_LIST_CTRL_ID,
    ZMT_GPT_KOUYU_LIST_CTRL_ID,

    ZMT_GPT_FORM_CTRL_ID,
    ZMT_GPT_FORM_ANIM_CTRL_ID,
    ZMT_GPT_FORM_TEXT_1_CTRL_ID,
    ZMT_GPT_FORM_TEXT_2_CTRL_ID,
    ZMT_GPT_FORM_TEXT_3_CTRL_ID,
    ZMT_GPT_FORM_TEXT_4_CTRL_ID,
    ZMT_GPT_FORM_TEXT_5_CTRL_ID,
    ZMT_GPT_FORM_TEXT_6_CTRL_ID,
    ZMT_GPT_FORM_TEXT_7_CTRL_ID,
    ZMT_GPT_FORM_TEXT_8_CTRL_ID,
    ZMT_GPT_FORM_TEXT_9_CTRL_ID,
    ZMT_GPT_FORM_TEXT_10_CTRL_ID,
    ZMT_GPT_FORM_TEXT_11_CTRL_ID,
    ZMT_GPT_FORM_TEXT_12_CTRL_ID,
    ZMT_GPT_FORM_TEXT_13_CTRL_ID,
    ZMT_GPT_FORM_TEXT_14_CTRL_ID,
    ZMT_GPT_FORM_TEXT_15_CTRL_ID,
    ZMT_GPT_FORM_TEXT_16_CTRL_ID,
    ZMT_GPT_FORM_TEXT_17_CTRL_ID,
    ZMT_GPT_FORM_TEXT_18_CTRL_ID,
    ZMT_GPT_FORM_TEXT_19_CTRL_ID,
    ZMT_GPT_FORM_TEXT_20_CTRL_ID,
  
    ZMT_GPT_MAX_CTRL_ID

} ZMT_GPT_CONTROL_ID_E;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif


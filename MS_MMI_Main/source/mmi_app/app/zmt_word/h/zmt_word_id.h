/*****************************************************************************
** File Name:      mmiacc_id.h                                               *
** Author:         xiaoqing.lu                                               *
** Date:           2009/04/03                                                *
** Copyright:      2006 Spreadtrum, Incoporated. All Rights Reserved.        *
** Description:    This file is used to describe ACC win and control id      *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 2009/04/03     xiaoqing.lu       Create                                   *
******************************************************************************/

#ifndef _MMIWORD_ID_H_
#define _MMIWORD_ID_H_

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
  MMIZMT_WORD_WIN_ID_START = (MMI_MODULE_ZMT_WORD << 16),
  MMI_WORD_CARD_WIN_ID,
  MMI_WORD_WIN_ID,
  MMI_WORD_CH_WIN_ID,
  MMI_WORD_LISTENING_ID,
  MMI_WORD_LISTENING_SETTING_ID,

    MMI_WORD_MAIN_WIN_ID,
    MMI_WORD_CHAPTER_WIN_ID,
    MMI_WORD_DETAIL_WIN_ID,
  
    MMIZMT_WORD_MAX_WIN_ID

} MMIZMT_WORD_WINDOW_ID_E;

#undef WIN_ID_DEF

// control ID
typedef enum {
  MMIZMT_WORD_WIN_CTRL_ID_START = MMIZMT_WORD_MAX_WIN_ID,

  MMI_ZMT_WORD_MAIN_LABEL_NUM_ID,
  MMI_ZMT_WORD_MAIN_BUTTON_BACK,
  MMI_ZMT_WORD_MAIN_LABEL_BACK,
  MMI_ZMT_WORD_MAIN_PAD_LABEL_BACK,
  MMI_ZMT_WORD_MAIN_LIST_CONTENT,
  MMI_ZMT_WORD_MAIN_BUTTON_REFRESH,

  MMI_ZMT_WORD_CH_LABEL_NUM_ID,
  MMI_ZMT_WORD_CH_BUTTON_BACK,
  MMI_ZMT_WORD_CH_BUTTON_DISAUTOPLAY,
  MMI_ZMT_WORD_CH_BUTTON_AUTOPLAY,
  MMI_ZMT_WORD_CH_LABEL_BACK,
  MMI_ZMT_WORD_CH_PAD_LABEL_BACK,
  MMI_ZMT_WORD_CH_LIST_CONTENT,
  MMI_ZMT_WORD_CH_BUTTON_EXERCISE,
  MMI_ZMT_WORD_CH_BUTTON_PACTISE,
  MMI_ZMT_WORD_CH_BUTTON_REFRESH,

  MMI_ZMT_WORD_LABEL_TITLE_ID,
  MMI_ZMT_WORD_PAD_LABEL_TITLE_ID,
  MMI_ZMT_WORD_LABEL_MESSAGE_ID,
  MMI_ZMT_WORD_LABEL_NUM_ID,
  MMI_ZMT_WORD_BUTTON_OK_ID,
  MMI_ZMT_WORD_BUTTON_CACEL_ID,
  MMI_ZMT_WORD_BUTTON_NEXT_ID,
  MMI_ZMT_WORD_BUTTON_PRE_ID,
  MMI_ZMT_WORD_BUTTON_DELETE_ID,
  MMI_ZMT_WORD_BUTTON_BACK_ID,
  MMI_ZMT_WORD_MSG_LABEL_ID,
  MMI_ZMT_WORD_MSG_SP1_ID,
  MMI_ZMT_WORD_MSG_SP1_BTN_ID,
  MMI_ZMT_WORD_INFO_TEXT_ID,
  MMI_ZMT_WORD_BUTTON_REFRESH,

    MMI_ZMT_WORD_MAIN_LABEL_BACK_CTRL_ID,
    MMI_ZMT_WORD_MAIN_LIST_CTRL_ID,
    MMI_ZMT_WORD_MAIN_LABEL_NUM_CTRL_ID,

    MMI_ZMT_WORD_CHAPTER_LABEL_BACK_CTRL_ID,
    MMI_ZMT_WORD_CHAPTER_AUTO_PLAY_CTRL_ID,
    MMI_ZMT_WORD_CHAPTER_LIST_CTRL_ID,
    MMI_ZMT_WORD_CHAPTER_LABEL_NUM_CTRL_ID,
    MMI_ZMT_WORD_CHAPTER_LEFT_CTRL_ID,
    MMI_ZMT_WORD_CHAPTER_RIGHT_CTRL_ID,
    
    MMI_ZMT_WORD_DETAIL_LABEL_BACK_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_LABEL_NUM_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_LABEL_WORD_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_LABEL_PINYIN_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_BUTTON_AUDIO_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_TEXT_INFO_CTRL_ID,
    MMI_ZMT_WORD_MSG_TIPS_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_LEFT_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_RIGHT_CTRL_ID,
    MMI_ZMT_WORD_DETAIL_DELETE_CTRL_ID,

  MMI_ZMT_WORD_POPUP,
  
  MMIZMT_WORD_MAX_WIN_CTRL_ID

} MMIZMT_WORD_CONTROL_ID_E;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif  //_MMIHELLO_ID_H_


#ifndef _MMIHELLO_ID_H_
#define _MMIHELLO_ID_H_ 

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_module.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/

#define WIN_ID_DEF(win_id)          win_id

// ´°¿ÚID
typedef enum
{
    LISTENING_WIN_ID_START = (MMI_MODULE_ZMT_LISTENING << 16),
	
	//win id
	LISTENING_ALBUM_LIST_WIN_ID,
	LISTENING_AUDIO_LIST_WIN_ID,
	LISTENING_PLAYER_WIN_ID,
	LISTENING_PLAYER_LRC_WIN_ID,
	LISTENING_LRC_WIN_ID,

	LISTENING_LOCAL_ALBUM_WIN_ID,
	LISTENING_LOCAL_AUDIO_WIN_ID,
	LISTENING_LOCAL_DELETE_WIN_ID,

	LISTENING_TIP_WIN_ID,
	
    LISTENING_MAX_WIN_ID
}MMILISTENING_WINDOW_ID_E;

#undef WIN_ID_DEF

// ¿Ø¼þID
typedef enum
{
    LISTENING_CTRL_ID_ID_START = LISTENING_MAX_WIN_ID,

	//ctrl id
	LISTENING_LISTBOX_ALBUM_CTRL_ID,
	LISTENING_LISTBOX_AUDIO_CTRL_ID,
	
	LISTENING_BUTTON_PRE_CTRL_ID,
	LISTENING_BUTTON_PRI_CTRL_ID,
	LISTENING_BUTTON_JUR_CTRL_ID,

	LISTENING_BUTTON_NEXT_CTRL_ID,
	LISTENING_BUTTON_PREV_CTRL_ID,
	LISTENING_BUTTON_PLAY_CTRL_ID,

	LISTENING_LISTBOX_LOCAL_ALBUM_CTRL_ID,
	LISTENING_LISTBOX_LOCAL_AUDIO_CTRL_ID,

	LISTENING_TEXTBOX_PLAYER_CTRL_ID,
	LISTENING_TEXTBOX_PLAYER_LRC_CTRL_ID,

    LISTENING_MAX_CTRL_ID
}MMILISTENING_CONTROL_ID_E;

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif //_MMIHELLO_ID_H_

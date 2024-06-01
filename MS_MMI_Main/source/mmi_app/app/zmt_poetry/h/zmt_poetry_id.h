
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
    POETRY_WIN_ID_START = (MMI_MODULE_ZMT_POETRY << 16),
	
	//win id
	POETRY_MAIN_WIN_ID,
	POETRY_ITEM_WIN_ID,
	POETRY_DETAIL_WIN_ID,
    DEPOSIT_WIN_ID,
    DEPOSIT_TIP_WIN_ID,
    POETRY_MAX_WIN_ID
}MMIPOETRY_WINDOW_ID_E;

#undef WIN_ID_DEF

// ¿Ø¼þID
typedef enum
{
    POETRY_CTRL_ID_ID_START = POETRY_MAX_WIN_ID,

	//ctrl id
    MMI_ZMT_POETRY_BUTTON_REFRESH,
    MMI_ZMT_POETRY_BUTTON_DETAIL_REFRESH,
    MMI_ZMT_POETRY_APPRE_TEXT_ID,

    MMI_ZMT_POETRY_GRADE_LIST_CTRL_ID,
    MMI_ZMT_POETRY_ITEM_LIST_CTRL_ID,
    
    POETRY_MAX_CTRL_ID
}MMIPOETRY_CONTROL_ID_E;

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

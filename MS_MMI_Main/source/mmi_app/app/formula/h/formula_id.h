#ifndef _MMIFORMULA_ID_H_
#define _MMIFORMULA_ID_H_ 

#include "sci_types.h"
#include "os_api.h"
#include "mmk_type.h"
#include "mmi_module.h"


PUBLIC void MMIFORMULA_CreateIconlistWin(void);
//MMI_APPLICATION_T   		mmi_mp_formula_app;
#ifdef __cplusplus
    extern   "C"
    {
#endif

#define WIN_ID_DEF(win_id, win_id_name)        win_id,
 
// window ID
typedef enum
{
    MMI_FORMULA_WIN_ID_START = (MMI_MODULE_FORMULA_WIN_ID << 16),
	FORMULA_WIN_ID,
#include "formula_id.def"

    MMI_FORMULA_MAX_WIN_ID
}MMIFORMULA_ID_E;


#define WIN_ID_DEF(win_id)          win_id

// window ID

typedef enum {
  FORMULA_CTRL_ID_START = MMI_FORMULA_MAX_WIN_ID,
  FORMULA_CTRL_START,
  FORMULA_CTRL_STOP,
  FORMULA_CTRL_RESET,
   
  FORMULA_CTRL_TIME,
  /**/
FORMULA_CTRL_TEST,
  FORMULA_MAX_CTRL_ID
} FORMULA_CONTROL_ID_E;
#undef WIN_ID_DEF

// control ID

#undef WIN_ID_DEF

#ifdef   __cplusplus
    }
#endif

#endif //_MMIFORMULA_ID_H_

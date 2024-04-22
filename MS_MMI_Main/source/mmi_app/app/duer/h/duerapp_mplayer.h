
/*****************************************************************************/
#ifndef _DUERAPP_MPLAYER_H_
#define _DUERAPP_MPLAYER_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmi_appmsg.h"
#include "sci_types.h"
#include "mmk_type.h"
#include "baidu_json.h"
#include "lightduer_connagent.h"
#include "duerapp_payload.h"
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/ 

#ifdef _cplusplus
	extern   "C"
    {
#endif
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
PUBLIC MMI_HANDLE_T MMIDUERAPP_CreatePlayInfoWin(ADD_DATA data);
PUBLIC MMI_HANDLE_T MMIDUERAPP_CreatePlayListWin(ADD_DATA data);
PUBLIC void duerapp_mplayer_netchecked_refresh(void);

/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/




#ifdef _cplusplus
	}
#endif

#endif //_DUERAPP_MPLAYER_H_

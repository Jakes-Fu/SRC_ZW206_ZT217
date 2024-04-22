
/*****************************************************************************/
#ifndef _DUERAPP_RES_DOWN_H_
#define _DUERAPP_RES_DOWN_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmi_appmsg.h"
#include "sci_types.h"
#include "mmk_type.h"
#include "baidu_json.h"
#include "lightduer_connagent.h"
#include "duerapp_payload.h"
#include "duerapp_img_down.h"
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
typedef enum
{
    IMG_DOWN_NONE = 0,
    IMG_DOWN_DISCOVERY,
    IMG_DOWN_HOME_BANNER,
    IMG_DOWN_SKILL_BANNER,
    IMG_DOWN_SKILL,
    IMG_DOWN_CENTER_BANNER,
    IMG_DOWN_RENDER_STANDARDCARD,
    IMG_DOWN_RENDER_LISTCARD,
    IMG_DOWN_PARENTS_ASSISTS,
    IMG_DOWN_MAX
} img_down_type_e;

typedef struct
{
    int type;       //img_down_type_e
    int index;
} img_down_param_t;

/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
PUBLIC void imageFreeOneType(int type);
PUBLIC void imageArtFreeAll(void);
PUBLIC void imageStartDownResource(int type);
PUBLIC void imageContinueDownResource(int type);
PUBLIC BOOLEAN imageIfDownOver(int type);
PUBLIC img_dsc_t *imageGetResource(int type, int index);
PUBLIC void DebugimageMMIDealDownCbMsg(MMI_HANDLE_T win_handle, void *param);
PUBLIC void MMIUpdateAnimImage(MMI_HANDLE_T win_handle, MMI_CTRL_ID_T ctrl_id, img_dsc_t *pic_img);

/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/




#ifdef _cplusplus
	}
#endif

#endif //_DUERAPP_RES_DOWN_H_

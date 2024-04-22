
/*****************************************************************************/
#ifndef _DUERAPP_SLIDE_INDICATOR_H_
#define _DUERAPP_SLIDE_INDICATOR_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmk_type.h"
#include "watch_slidepage.h"


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
#define DUERAPP_SLIDE_INDICATOR_WIDTH           240
#if 1 //修改导航点的显示为固定图
#define DUERAPP_SLIDE_INDICATOR_HEIGHT          0
#define DUERAPP_SLIDE_POINT_PADING_TOP          5
#else
#define DUERAPP_SLIDE_INDICATOR_HEIGHT          20
#endif

typedef enum {
    DUERAPP_SLIDE_IND_POS_TYPE_TOP,
    DUERAPP_SLIDE_IND_POS_TYPE_BOTTOM,
    DUERAPP_SLIDE_IND_POS_TYPE_NUM
} duerapp_silde_indicator_position_t;

typedef struct {
    duerapp_silde_indicator_position_t ind_pos;
    uint8 page_cnt;
    MMI_HANDLE_T slide_handle;
} duerapp_silde_indicator_setting_t;

/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
PUBLIC int32 DUERAPP_SlideSettingInit(duerapp_silde_indicator_setting_t *set);
PUBLIC MMI_RESULT_E DUERAPP_SlideHandleCb(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param);

/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/


#ifdef _cplusplus
	}
#endif

#endif//_DUERAPP_SLIDE_INDICATOR_H_

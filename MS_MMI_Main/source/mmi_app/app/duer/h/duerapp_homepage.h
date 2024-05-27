
/*****************************************************************************/
#ifndef _DUERAPP_HOMEPAGE_H_
#define _DUERAPP_HOMEPAGE_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmi_appmsg.h"
#include "sci_types.h"
#include "mmk_type.h"
#include "baidu_json.h"
#include "lightduer_connagent.h"
#include "duerapp_payload.h"
#include "duerapp_main.h"
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/ 

#ifdef _cplusplus
	extern   "C"
    {
#endif
/*----------------------------------------------------------------------------*/


#define DUERAPP_ARRAY_CNT(array)        ARR_SIZE(array)

/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/
typedef enum {
    DUER_DRAW_TYPE_SKILL_FORM,
    DUER_DRAW_TYPE_BANNER,
} duerapp_draw_type_t;

typedef MMI_RESULT_E (*duer_guianim_callback)(void *usr_data);


typedef struct {
    MMI_IMAGE_ID_T          img_id;
    BOOLEAN                 is_hide;
    MMI_CTRL_ID_T           img_form_ctrl_id;
    MMI_CTRL_ID_T           img_ctrl_id;
    MMI_CTRL_ID_T           txt_form_ctrl_id;
    MMI_CTRL_ID_T           txt_ctrl_id;
    wchar                   *txt;
    duer_guianim_callback   item_click_cb;
    void                    *usr_data;
    int32                   res_index;
    BOOLEAN                 img_loaded;

} duer_draw_skill_item_t;

// draw type struct define
typedef struct {
    wchar                   *title;
    BOOLEAN                 is_hide;
    MMI_CTRL_ID_T           tile_ctrl_id;
    uint16                  item_cnt;
    duer_draw_skill_item_t  *items;
} duer_draw_skill_form_t;

typedef struct {
    BOOLEAN                 is_hide;
    MMI_CTRL_ID_T           img_form_ctrl_id;
    MMI_CTRL_ID_T           img_ctrl_id;
    MMI_CTRL_ID_T           txt_ctrl_id;
    uint16                  width;
    uint16                  height;
    MMI_IMAGE_ID_T          img_id;
    wchar                   *txt;
    duer_guianim_callback   banner_click_cb;
    void                    *usr_data;
    int32                   res_index;
    BOOLEAN                 img_loaded;
    char                    *show_type;
} duer_draw_banner_t;

typedef int32(*duerapp_draw_func)(MMI_HANDLE_T ctrl_id, void *arg);


typedef struct {
    duerapp_draw_type_t type;
    duerapp_draw_func func;
    union {
    duer_draw_skill_form_t *skill_form;
    duer_draw_banner_t *banner;
    } draw;
} duerapp_draw_t;

/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/

PUBLIC void MMIDUERAPP_InitModule(void);

PUBLIC void MMIDUERAPP_StartApplet(void);

PUBLIC void MMIDUERAPP_CreateWinEnter(void);

PUBLIC void EnterAppBaiduMainWin(CAF_HANDLE_T app_handle);

PUBLIC void MMIDUERAPP_InitState(int status);

PUBLIC int MMIDUERAPP_GetInitStatus(void);

PUBLIC int32 DUERAPP_HomeSkillFormCreate(MMI_HANDLE_T ctrl_id, void *arg);

PUBLIC int32 DUERAPP_HomeBannerCreate(MMI_HANDLE_T ctrl_id, void *arg);

PUBLIC void DUERAPP_FormItemCreate(MMI_HANDLE_T form_ctrl_id, duerapp_draw_t* draws, uint16 draw_cnt);

PUBLIC void MMIDUERAPP_MediaPlayStateEvent(MMIDUERAPP_MEDIA_STATE_E state);

PUBLIC void MMIDUERAPP_WatchRequireLogin(baidu_json *payload);

PUBLIC void duerapp_create_qr_callback(DUERAPP_ASSISTS_RESPONSE_TYPE_E type);

PUBLIC void duerapp_operate_notify_callback(void);

PUBLIC void duerapp_other_task_to_MMI_home(MMI_MESSAGE_ID_E msg_id,
                                     DPARAM           param_ptr,
                                     uint32           size_of_param
                                     );

PUBLIC void MMIDUERAPP_HomePageOpenWinInit(void);

PUBLIC void MMIDUERAPP_HomePageCloseWinDeinit(void);

PUBLIC void MMIDUERAPP_StartInputAsrSet(void);

PUBLIC void MMIDUERAPP_StartNormalAsrSet(void);

PUBLIC void MMIDUERAPP_StartDictionaryAsrSet(void);

PUBLIC void MMIDUERAPP_StartInterpreterAsrSet(void);

PUBLIC void MMIDUERAPP_Exit(void);

#ifdef _cplusplus
	}
#endif

#endif//_DUERAPP_HOMEPAGE_H_

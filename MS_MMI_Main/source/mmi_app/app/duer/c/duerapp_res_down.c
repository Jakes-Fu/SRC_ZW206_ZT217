// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_res_down.c
 * Auth: shichenyu01 (shichenyu01@baidu.com)
 * Desc: duerapp resource download.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2021        shichenyu01      Create                                    *
******************************************************************************/

/**--------------------------------------------------------------------------*/
/**                         Include Files                                    */
/**--------------------------------------------------------------------------*/
#include "std_header.h"
#include "window_parse.h"
#include "guilcd.h"
#include "mmidisplay_data.h"
#include "guifont.h"
#include "mmi_textfun.h"
#include "mmipub.h"
#include "guilistbox.h"
#include "guibutton.h"
#include "guitext.h"
#include "guilabel.h"
#include "duerapp_id.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "mmi_theme.h"
#include "duerapp_text.h"
#include "duerapp_image.h"
#include "duerapp_anim.h"
#include "duerapp_nv.h"
#include "duerapp_main.h"
#include "duerapp_mplayer.h"
#include "watch_common_btn.h"
#include "watch_common_list.h"
#include "watch_commonwin_export.h"
#include "mmi_event_api.h"
#include "mmk_app.h"
#include "mmi_applet_table.h"
#include "guirichtext.h"
// #include "mmiwifi_export.h"
#include "mmiphone_export.h"
#include "ctrlsetlist_export.h"
#include "guisetlist.h"
#include "os_api.h"

#include "dal_time.h"
#include "duerapp.h"
#include "mmicom_time.h"
#include "duerapp_common.h"
#include "duerapp_login.h"
#include "duerapp_qrwin.h"
#include "duerapp_http.h"
#include "duerapp_img_down.h"
#include "duerapp_operate.h"
#include "duerapp_res_down.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_audio_play_utils.h"
#include "duerapp_center.h"
#include "duerapp_assists_activity.h"
#include "baidu_json.h"
#include "lightduer_log.h"
#include "lightduer_random.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_dcs.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"
#include "lightduer_lib.h"
#include "lightduer_audio_codec_ops.h"
#include "lightduer_audio_adapter.h"
#include "lightduer_audio_player.h"
#include "lightduer_audio_codec_adapter.h"
#include "lightduer_timestamp.h"
/**--------------------------------------------------------------------------*/
/**                         MACRO DEFINITION                                 */
/**--------------------------------------------------------------------------*/
#define USE_MUTEX_PROTECT 1
/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          CONSTANT ARRAY                                   */
/*---------------------------------------------------------------------------*/
#define BANNER_IMG_MAX_SIZE             15
#define DISCOVERY_IMG_MAX_SIZE          50
#define SKILL_IMG_MAX_SIZE              30

#define IMG_DOWN_MUTEX_AUTO_UNLOCK_TIME_SEC     30

/**--------------------------------------------------------------------------*/
/**                         EXTERNAL DECLARE                                 */
/**--------------------------------------------------------------------------*/
extern DUEROS_DISCOVERY_LIST_ITEM_ST    *s_discovery_list;
extern int                              s_discovery_list_size;
extern DUEROS_SKILL_LIST_ITEM_ST        *s_real_skill_list;
extern int                              s_skill_list_size;
extern DUEROS_BLOCK_LIST_ITEM_ST        *s_home_block_list;
extern DUEROS_BLOCK_LIST_ITEM_ST        *s_skill_block_list;
extern int                              s_home_block_list_size;
extern int                              s_skill_block_list_size;
extern DUEROS_CENTER_CFG_ITEM_ST        *s_center_cfg;
extern duerapp_render_card_t            *s_render_card;

/**--------------------------------------------------------------------------*/
/**                         STATIC DEFINITION                                */
/**--------------------------------------------------------------------------*/
LOCAL img_dsc_t         s_discovery_img_item[DISCOVERY_IMG_MAX_SIZE]    = {{0}};
LOCAL int               s_discovery_img_size                            = 0;
LOCAL img_dsc_t         s_home_banner_img_item[BANNER_IMG_MAX_SIZE]     = {{0}};
LOCAL int               s_home_banner_img_size                          = 0;
LOCAL img_dsc_t         s_skill_banner_img_item[BANNER_IMG_MAX_SIZE]    = {{0}};
LOCAL int               s_skill_banner_img_size                         = 0;
LOCAL img_dsc_t         s_skill_img_item[SKILL_IMG_MAX_SIZE]            = {{0}};
LOCAL int               s_skill_img_size                                = 0;
LOCAL img_dsc_t         s_center_banner_img                             = {0};
LOCAL img_dsc_t         s_rendercard_standardcard_img                   = {0};
LOCAL img_dsc_t         s_listcard_img_item[DUER_LIST_CARD_NUMBER_MAX]  = {{0}};
LOCAL int               s_listcard_img_size                             = 0;
LOCAL img_dsc_t         s_parents_assists_img                           = {0};

#if (USE_MUTEX_PROTECT == 1)
LOCAL BOOLEAN           s_img_down_ing[IMG_DOWN_MAX]                    = {FALSE};
#endif
/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
LOCAL void SendDownCbMMIMsgFromOtherTask(MMI_HANDLE_T win_id, int type, int index)
{
    img_down_param_t msg_param = {0};
    msg_param.type = type;
    msg_param.index = index;
    MMK_duer_other_task_to_MMI(win_id, MSG_DUERAPP_ANIM_PIC_UPDATE, &msg_param, sizeof(img_down_param_t));
}

LOCAL void imageDownloadCb(img_dsc_t *img, void *down_param)
{
    img_down_param_t *p_param = NULL;

    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);

    if (!img ) {
        DUER_LOGE("%s img err", __FUNCTION__);
        if (down_param) {
            DUER_FREE(down_param);
        }
        return;
    }

    p_param = (img_down_param_t *)down_param;
    if (p_param) {
        DUER_LOGI("(%s)[duer_watch]:down cb type(%d),index(%d),p(%p)", __FUNCTION__, p_param->type, p_param->index, p_param);
    } else {
        DUER_LOGE("%s param err", __FUNCTION__);
        return;
    }

    switch (p_param->type) {
        case IMG_DOWN_DISCOVERY:
        {
            if (s_discovery_img_item[p_param->index].data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_discovery_img_item[p_param->index].data);
                s_discovery_img_item[p_param->index].data = NULL;
            }
            s_discovery_img_item[p_param->index].data_size = 0;

            s_discovery_img_item[p_param->index] = *img;
            s_discovery_img_size++;

            //send msg to UI,UI judge if continue
            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_HOME_WIN_ID, p_param->type, p_param->index);
        }
        break;
        case IMG_DOWN_HOME_BANNER:
        {
            if (s_home_banner_img_item[p_param->index].data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_home_banner_img_item[p_param->index].data);
                s_home_banner_img_item[p_param->index].data = NULL;
            }
            s_home_banner_img_item[p_param->index].data_size = 0;

            s_home_banner_img_item[p_param->index] = *img;
            s_home_banner_img_size++;

            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_HOME_WIN_ID, p_param->type, p_param->index);
        }
        break;
        case IMG_DOWN_SKILL_BANNER:
        {
            if (s_skill_banner_img_item[p_param->index].data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_skill_banner_img_item[p_param->index].data);
                s_skill_banner_img_item[p_param->index].data = NULL;
            }
            s_skill_banner_img_item[p_param->index].data_size = 0;

            s_skill_banner_img_item[p_param->index] = *img;
            s_skill_banner_img_size++;

            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_HOME_WIN_ID, p_param->type, p_param->index);
        }
        break;
        case IMG_DOWN_SKILL:
        {
            if (s_skill_img_item[p_param->index].data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_skill_img_item[p_param->index].data);
                s_skill_img_item[p_param->index].data = NULL;
            }
            s_skill_img_item[p_param->index].data_size = 0;

            s_skill_img_item[p_param->index] = *img;
            s_skill_img_size++;

            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_HOME_WIN_ID, p_param->type, p_param->index);
        }
        break;
        case IMG_DOWN_CENTER_BANNER:
        {
            if (s_center_banner_img.data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_center_banner_img.data);
                s_center_banner_img.data = NULL;
            }
            s_center_banner_img.data_size = 0;

            s_center_banner_img = *img;

            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_CENTER_INFO_WIN_ID, p_param->type, p_param->index);
        }
        break;
        case IMG_DOWN_RENDER_STANDARDCARD:
        {
            if (s_rendercard_standardcard_img.data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_rendercard_standardcard_img.data);
                s_rendercard_standardcard_img.data = NULL;
            }
            s_rendercard_standardcard_img.data_size = 0;

            s_rendercard_standardcard_img = *img;

            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_MAIN_WIN_ID, p_param->type, p_param->index);
        }
        break;
        case IMG_DOWN_RENDER_LISTCARD:
        {
            if (s_listcard_img_item[p_param->index].data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_listcard_img_item[p_param->index].data);
                s_listcard_img_item[p_param->index].data = NULL;
            }
            s_listcard_img_item[p_param->index].data_size = 0;

            s_listcard_img_item[p_param->index] = *img;
            s_listcard_img_size++;

            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_MAIN_WIN_ID, p_param->type, p_param->index);
        }
        break;
        case IMG_DOWN_PARENTS_ASSISTS:
        {
            if (s_parents_assists_img.data) {
                DUER_LOGE("already exist img,type(%d),index(%d)", p_param->type, p_param->index);
                DUER_FREE(s_parents_assists_img.data);
                s_parents_assists_img.data = NULL;
            }
            s_parents_assists_img.data_size = 0;

            s_parents_assists_img = *img;

            SendDownCbMMIMsgFromOtherTask(MMI_DUERAPP_VIP_TRANSMIT_MESSAGE_WIN_ID, p_param->type, p_param->index);
        }
        break;
        default:
        break;
    }
    DUER_LOGI("(%s)[duer_watch]:img free", __FUNCTION__);
    DUER_FREE(img);
    img = NULL;

#if (USE_MUTEX_PROTECT == 1)
    s_img_down_ing[p_param->type] = FALSE;
#endif
}

LOCAL void imageDownResourcePicture(char *url, int type, int index)
{
    static unsigned int last_lock_timestamp[IMG_DOWN_MAX] = {0};
    unsigned int now_timestamp = 0;
    unsigned int diff = 0;
    img_down_param_t *p_param = NULL;
    DUER_LOGI("(%s)[duer_watch]:type(%d),index(%d)", __FUNCTION__, type, index);
    if (!url) {
        return;
    }

#if (USE_MUTEX_PROTECT == 1)

    now_timestamp = duer_real_timestamp();
    diff = DUER_TIME_DIFF(last_lock_timestamp[type], now_timestamp);
    if (diff > IMG_DOWN_MUTEX_AUTO_UNLOCK_TIME_SEC) {
        DUER_LOGI("auto unlock[%d]", type);
        s_img_down_ing[type] = FALSE;
    }

    if (s_img_down_ing[type]) {
        DUER_LOGE("now img downing[%d]", type);
        return;
    }
    s_img_down_ing[type] = TRUE;
    last_lock_timestamp[type] = now_timestamp;
#endif
    p_param = DUER_CALLOC(1, sizeof(img_down_param_t));
    if (p_param) {
        p_param->type = type;
        p_param->index = index;
        DUER_LOGI("(%s)[duer_watch]:down type(%d),index(%d),p(%p)", __FUNCTION__, p_param->type, p_param->index, p_param);
        duer_image_download(url, imageDownloadCb, p_param);
    }
}

PUBLIC void MMIUpdateAnimImage(MMI_HANDLE_T win_handle, MMI_CTRL_ID_T ctrl_id, img_dsc_t *pic_img)
{
    GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    GUIANIM_DATA_INFO_T     data_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    MMI_HANDLE_T            anim_handle = NULL;

    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);

    if (!MMK_IsOpenWin(win_handle))
        //|| !MMK_IsFocusWin(win_handle))
    {
        return;
    }

    if (ctrl_id < 0) {
        DUER_LOGE("%s ctrl id(%d) err", __FUNCTION__, ctrl_id);
        return;
    }

    DUER_LOGI("(%s)[duer_watch]:win id(%d),ctrl id(%d)", __FUNCTION__, win_handle, ctrl_id);

    {
        anim_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
        ctrl_info.is_ctrl_id        = TRUE;
        ctrl_info.ctrl_id           = anim_handle;
        display_info.align_style    = GUIANIM_ALIGN_HVMIDDLE;
        display_info.bg.bg_type     = GUI_BG_COLOR;
        display_info.is_update      = TRUE;
        

        if ((pic_img)) {
            data_info.data_ptr = (pic_img)->data;
            data_info.data_size = (pic_img)->data_size;

            display_info.is_zoom = TRUE;
            display_info.is_disp_one_frame = TRUE;
            display_info.is_handle_transparent = TRUE;
        }

        GUIANIM_SetParam(&ctrl_info, &data_info, PNULL, &display_info);
        GUIANIM_SetVisible(ctrl_id, TRUE, TRUE);
        GUIANIM_PlayAnim(anim_handle);
    }

}

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
PUBLIC void imageFreeOneType(int type)
{
    int i = 0;

    DUER_LOGI("(%s)[duer_watch]:type(%d)", __FUNCTION__, type);

    switch (type) {
        case IMG_DOWN_DISCOVERY:
        {
            for (i = 0; i < DISCOVERY_IMG_MAX_SIZE; i++) {
                if (s_discovery_img_item[i].data) {
                    DUER_FREE(s_discovery_img_item[i].data);
                    s_discovery_img_item[i].data = NULL;
                }
                s_discovery_img_item[i].data_size = 0;
            }
            s_discovery_img_size = 0;
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        case IMG_DOWN_HOME_BANNER:
        {
            for (i = 0; i < BANNER_IMG_MAX_SIZE; i++) {
                if (s_home_banner_img_item[i].data) {
                    DUER_FREE(s_home_banner_img_item[i].data);
                    s_home_banner_img_item[i].data = NULL;
                }
                s_home_banner_img_item[i].data_size = 0;
            }
            s_home_banner_img_size = 0;
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        case IMG_DOWN_SKILL_BANNER:
        {
            for (i = 0; i < BANNER_IMG_MAX_SIZE; i++) {
                if (s_skill_banner_img_item[i].data) {
                    DUER_FREE(s_skill_banner_img_item[i].data);
                    s_skill_banner_img_item[i].data = NULL;
                }
                s_skill_banner_img_item[i].data_size = 0;
            }
            s_skill_banner_img_size = 0;
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        case IMG_DOWN_SKILL:
        {
            for (i = 0; i < SKILL_IMG_MAX_SIZE; i++) {
                if (s_skill_img_item[i].data) {
                    DUER_FREE(s_skill_img_item[i].data);
                    s_skill_img_item[i].data = NULL;
                }
                s_skill_img_item[i].data_size = 0;
            }
            s_skill_img_size = 0;
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        case IMG_DOWN_CENTER_BANNER:
        {
            if (s_center_banner_img.data) {
                DUER_FREE(s_center_banner_img.data);
                s_center_banner_img.data = NULL;
            }
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        case IMG_DOWN_RENDER_STANDARDCARD:
        {
            if (s_rendercard_standardcard_img.data) {
                DUER_FREE(s_rendercard_standardcard_img.data);
                s_rendercard_standardcard_img.data = NULL;
            }
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        case IMG_DOWN_RENDER_LISTCARD:
        {
            for (i = 0; i < DUER_LIST_CARD_NUMBER_MAX; i++) {
                if (s_listcard_img_item[i].data) {
                    DUER_FREE(s_listcard_img_item[i].data);
                    s_listcard_img_item[i].data = NULL;
                }
                s_listcard_img_item[i].data_size = 0;
            }
            s_listcard_img_size = 0;
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        case IMG_DOWN_PARENTS_ASSISTS:
        {
            if (s_parents_assists_img.data) {
                DUER_FREE(s_parents_assists_img.data);
                s_parents_assists_img.data = NULL;
            }
            DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
        }
        break;
        default:
        break;
    }
}

PUBLIC void imageArtFreeAll(void)
{
    int i = 0;
    for (i = IMG_DOWN_NONE; i < IMG_DOWN_MAX; i++) {
        imageFreeOneType(i);
    }
}

PUBLIC void imageStartDownResource(int type)
{
    int start_index = 0;
    int i = 0;

    DUER_LOGI("(%s)[duer_watch]:type(%d)", __FUNCTION__, type);
    imageFreeOneType(type);

    switch (type) {
        case IMG_DOWN_DISCOVERY:
        {
            if (s_discovery_list && s_discovery_list_size > 0) {
                for (i = 0; i < s_discovery_list_size; i++) {
                    if (s_discovery_list[i].img_url) {
                        start_index = i;
                        break;
                    } else {
                        s_discovery_img_size++;
                    }
                }
                if (s_discovery_list[start_index].img_url) {
                    imageDownResourcePicture(s_discovery_list[start_index].img_url, type, start_index);
                }
            }
        }
        break;
        case IMG_DOWN_HOME_BANNER:
        {
            // int start_index = 0;
            if (s_home_block_list && s_home_block_list_size > 0) {
                for (i = 0; i < s_home_block_list_size; i++) {
                    if (s_home_block_list[i].banner_img_url) {
                        start_index = i;
                        break;
                    } else {
                        s_home_banner_img_size++;
                    }
                }
                if (s_home_block_list[start_index].banner_img_url) {
                    imageDownResourcePicture(s_home_block_list[start_index].banner_img_url, type, start_index);
                }
            }
        }
        break;
        case IMG_DOWN_SKILL_BANNER:
        {
            // int start_index = 0;
            if (s_skill_block_list && s_skill_block_list_size > 0) {
                for (i = 0; i < s_skill_block_list_size; i++) {
                    if (s_skill_block_list[i].banner_img_url) {
                        start_index = i;
                        break;
                    } else {
                        s_skill_banner_img_size++;
                    }
                }
                if (s_skill_block_list[start_index].banner_img_url) {
                    imageDownResourcePicture(s_skill_block_list[start_index].banner_img_url, type, start_index);
                }
            }
        }
        break;
        case IMG_DOWN_SKILL:
        {
            // int start_index = 0;
            if (s_real_skill_list && s_skill_list_size > 0) {
                for (i = 0; i < s_skill_list_size; i++) {
                    if (s_real_skill_list[i].img_url) {
                        start_index = i;
                        break;
                    } else {
                        s_skill_img_size++;
                    }
                }
                if (s_real_skill_list[start_index].img_url) {
                    imageDownResourcePicture(s_real_skill_list[start_index].img_url, type, start_index);
                }
            }
        }
        break;
        case IMG_DOWN_CENTER_BANNER:
        {
            if (s_center_cfg->banner_img_url) {
                DUER_LOGI("(%s)[duer_watch]:type(%d),url(%s)", __FUNCTION__, type, s_center_cfg->banner_img_url);
                imageDownResourcePicture(s_center_cfg->banner_img_url, type, 0);
            }
        }
        break;
        case IMG_DOWN_RENDER_STANDARDCARD:
        {
            if(s_render_card->standardcard == NULL) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            
            if (s_render_card->standardcard->image_url == NULL) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            DUER_LOGI("(%s)[duer_watch]:type(%d),url(%s)", __FUNCTION__, type, s_render_card->standardcard->image_url);
            imageDownResourcePicture(s_render_card->standardcard->image_url, type, 0);
        }
        break;
        case IMG_DOWN_RENDER_LISTCARD:
        {
            if (s_render_card->listcard == NULL) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            
            if (s_render_card->listcard->cnt == 0) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            
            if (s_render_card->listcard->image_src[s_render_card->listcard->index] == NULL) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            DUER_LOGI("(%s)[duer_watch]:type(%d), index(%d), url(%s)", __FUNCTION__, type, s_render_card->listcard->index, s_render_card->listcard->image_src[s_render_card->listcard->index]);
            imageDownResourcePicture(s_render_card->listcard->image_src[s_render_card->listcard->index], type, s_render_card->listcard->index);
        }
        break;
        case IMG_DOWN_PARENTS_ASSISTS:
        {
            DUER_LOGI("(%s)[duer_watch]:type(%d), index(%d), url(%s)", __FUNCTION__, type, 0, DUERAPP_ASSISTS_PNG_URL);
            imageDownResourcePicture(DUERAPP_ASSISTS_PNG_URL, type, 0);
        }
        break;
        default:
        break;
    }
}

PUBLIC BOOLEAN imageIfDownOver(int type)
{
    DUER_LOGI("(%s)[duer_watch]:type(%d)", __FUNCTION__, type);
    switch (type) {
        case IMG_DOWN_DISCOVERY:
        {
            if (s_discovery_img_size >= s_discovery_list_size) {
                DUER_LOGI("(%s)[duer_watch]:down type(%d) over,total size(%d)", __FUNCTION__, type, s_discovery_img_size);
                return TRUE;
            }
        }
        break;
        case IMG_DOWN_HOME_BANNER:
        {
            if (s_home_banner_img_size >= s_home_block_list_size) {
                DUER_LOGI("(%s)[duer_watch]:down type(%d) over,total size(%d)", __FUNCTION__, type, s_home_banner_img_size);
                return TRUE;
            }
        }
        break;
        case IMG_DOWN_SKILL_BANNER:
        {
            if (s_skill_banner_img_size >= s_skill_block_list_size) {
                DUER_LOGI("(%s)[duer_watch]:down type(%d) over,total size(%d)", __FUNCTION__, type, s_skill_banner_img_size);
                return TRUE;
            }
        }
        break;
        case IMG_DOWN_SKILL:
        {
            if (s_skill_img_size >= s_skill_list_size) {
                DUER_LOGI("(%s)[duer_watch]:down type(%d) over,total size(%d)", __FUNCTION__, type, s_skill_img_size);
                return TRUE;
            }
        }
        break;
        case IMG_DOWN_CENTER_BANNER:
        {
            if (s_center_banner_img.data) {
                DUER_LOGI("(%s)[duer_watch]:type(%d) down over", __FUNCTION__, type);
                return TRUE;
            } else {
                DUER_LOGI("(%s)[duer_watch]:type(%d) not down over", __FUNCTION__, type);
            }
        }
        break;
        case IMG_DOWN_RENDER_STANDARDCARD:
        {
            if (s_rendercard_standardcard_img.data) {
                DUER_LOGI("(%s)[duer_watch]:type(%d) down over", __FUNCTION__, type);
                return TRUE;
            } else {
                DUER_LOGI("(%s)[duer_watch]:type(%d) not down over", __FUNCTION__, type);
            }
        }
        break;
        case IMG_DOWN_RENDER_LISTCARD:
        {
            if(s_render_card->listcard == NULL) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            
            if (s_render_card->listcard->index >= s_render_card->listcard->cnt) {
                DUER_LOGI("(%s)[duer_watch]:down type(%d) over,total size(%d)", __FUNCTION__, type, s_render_card->listcard->index);
                return TRUE;
            }
        }
        break;
        case IMG_DOWN_PARENTS_ASSISTS:
        {
            if (s_parents_assists_img.data) {
                DUER_LOGI("(%s)[duer_watch]:type(%d) down over", __FUNCTION__, type);
                return TRUE;
            } else {
                DUER_LOGI("(%s)[duer_watch]:type(%d) not down over", __FUNCTION__, type);
            }
        }
        break;
        default:
        break;
    }

    return FALSE;
}

PUBLIC void imageContinueDownResource(int type)
{
    int i = 0;
    int continue_index = 0;
    switch (type) {
        case IMG_DOWN_DISCOVERY:
        {
            if (imageIfDownOver(type) == FALSE) {
                continue_index = s_discovery_img_size;
                for (i = s_discovery_img_size; i < s_discovery_list_size; i++) {
                    if (s_discovery_list[i].img_url) {
                        continue_index = i;
                        break;
                    } else {
                        s_discovery_img_size++;
                    }
                }
                DUER_LOGI("(%s)[duer_watch]:down type(%d) continue down next index(%d)", __FUNCTION__, type, continue_index);
                imageDownResourcePicture(s_discovery_list[continue_index].img_url, type, continue_index);
            }
        }
        break;
        case IMG_DOWN_HOME_BANNER:
        {
            if (imageIfDownOver(type) == FALSE) {
                continue_index = s_home_banner_img_size;
                for (i = s_home_banner_img_size; i < s_home_block_list_size; i++) {
                    if (s_home_block_list[i].banner_img_url) {
                        continue_index = i;
                        break;
                    } else {
                        s_home_banner_img_size++;
                    }
                }
                DUER_LOGI("(%s)[duer_watch]:down type(%d) continue down next index(%d)", __FUNCTION__, type, continue_index);
                imageDownResourcePicture(s_home_block_list[continue_index].banner_img_url, type, continue_index);
            }
        }
        break;
        case IMG_DOWN_SKILL_BANNER:
        {
            if (imageIfDownOver(type) == FALSE) {
                continue_index = s_skill_banner_img_size;
                for (i = s_skill_banner_img_size; i < s_skill_block_list_size; i++) {
                    if (s_skill_block_list[i].banner_img_url) {
                        continue_index = i;
                        break;
                    } else {
                        s_skill_banner_img_size++;
                    }
                }
                DUER_LOGI("(%s)[duer_watch]:down type(%d) continue down next index(%d)", __FUNCTION__, type, continue_index);
                imageDownResourcePicture(s_skill_block_list[continue_index].banner_img_url, type, continue_index);
            }
        }
        break;
        case IMG_DOWN_SKILL:
        {
            if (imageIfDownOver(type) == FALSE) {
                continue_index = s_skill_img_size;
                for (i = s_skill_img_size; i < s_skill_list_size; i++) {
                    if (s_real_skill_list[i].img_url) {
                        continue_index = i;
                        break;
                    } else {
                        s_skill_img_size++;
                    }
                }
                DUER_LOGI("(%s)[duer_watch]:down type(%d) continue down next index(%d)", __FUNCTION__, type, continue_index);
                imageDownResourcePicture(s_real_skill_list[continue_index].img_url, type, continue_index);
            }
        }
        break;
        case IMG_DOWN_CENTER_BANNER:
        {
            //not need continue
        }
        break;
        case IMG_DOWN_RENDER_STANDARDCARD:
        {
            //not need continue
        }
        break;
        case IMG_DOWN_RENDER_LISTCARD:
        {
            if(s_render_card->listcard == NULL) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            
            if (imageIfDownOver(type) == TRUE) {
                DUER_LOGI("(%s)[duer_watch]:download over", __FUNCTION__);
                break;
            }
            
            if (s_render_card->listcard->image_src[s_render_card->listcard->index] == NULL) {
                DUER_LOGI("(%s)[duer_watch]:error", __FUNCTION__);
                break;
            }
            DUER_LOGI("(%s)[duer_watch]:down type(%d) continue down next index(%d)", __FUNCTION__, type, s_render_card->listcard->index);
            imageDownResourcePicture(s_render_card->listcard->image_src[s_render_card->listcard->index], type, s_render_card->listcard->index);
        }
        break;
        case IMG_DOWN_PARENTS_ASSISTS:
        {
            //not need continue
        }
        break;
        default:
        break;
    }
}

PUBLIC img_dsc_t *imageGetResource(int type, int index)
{
    img_dsc_t *result = NULL;

    switch (type) {
        case IMG_DOWN_DISCOVERY:
        {
            if (index >= s_discovery_img_size) {

            }
            else {
                result = &(s_discovery_img_item[index]);
            }
            return result;
        }
        break;
        case IMG_DOWN_HOME_BANNER:
        {
            // img_dsc_t *result = NULL;
            if (index >= s_home_banner_img_size) {

            }
            else {
                result = &(s_home_banner_img_item[index]);
            }
            return result;
        }
        break;
        case IMG_DOWN_SKILL_BANNER:
        {
            // img_dsc_t *result = NULL;
            if (index >= s_skill_banner_img_size) {

            }
            else {
                result = &(s_skill_banner_img_item[index]);
            }
            return result;
        }
        break;
        case IMG_DOWN_SKILL:
        {
            // img_dsc_t *result = NULL;
            if (index >= s_skill_img_size) {

            }
            else {
                result = &(s_skill_img_item[index]);
            }
            return result;
        }
        break;
        case IMG_DOWN_CENTER_BANNER:
        {
            result = &s_center_banner_img;
            return result;
        }
        break;
        case IMG_DOWN_RENDER_STANDARDCARD:
        {
            result = &s_rendercard_standardcard_img;
            return result;
        }
        break;
        case IMG_DOWN_RENDER_LISTCARD:
        {
            result = &(s_listcard_img_item[index]);
            return result;
        }
        break;
        case IMG_DOWN_PARENTS_ASSISTS:
        {
            result = &s_parents_assists_img;
            return result;
        }
        break;
        default:
        break;
    }
}

PUBLIC void DebugimageMMIDealDownCbMsg(MMI_HANDLE_T win_handle, void *param)
{
    img_down_param_t *p_msg_param = NULL;
    img_dsc_t *resource = NULL;
    MMI_CTRL_ID_T ctrl_id = (MMI_CTRL_ID_T)0;

    DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    p_msg_param = (img_down_param_t *)param;
    if (p_msg_param) {
        DUER_LOGI("(%s)[duer_watch]:anim type(%d),index(%d)", __FUNCTION__, p_msg_param->type, p_msg_param->index);

        switch (p_msg_param->type) {
            case IMG_DOWN_DISCOVERY:
            {
                resource = imageGetResource(p_msg_param->type, p_msg_param->index);
                if (resource) {
#ifdef DUERAPP_USE_WATCH_CONFIG
                    ctrl_id = GetCtrlIdFromDiscoveryIndex(p_msg_param->index);
                    MMIUpdateAnimImage(win_handle, ctrl_id, resource);
#endif // DUERAPP_USE_WATCH_CONFIG
                } else {
                    DUER_LOGE("%s resource get err", __FUNCTION__);
                }
            }
            break;
            case IMG_DOWN_HOME_BANNER:
            {
                resource = imageGetResource(p_msg_param->type, p_msg_param->index);
                if (resource) {
#ifdef DUERAPP_USE_WATCH_CONFIG
                    ctrl_id = GetCtrlIdFromBlockIndex(p_msg_param->index);
                    MMIUpdateAnimImage(win_handle, ctrl_id, resource);
#endif // DUERAPP_USE_WATCH_CONFIG
                } else {
                    DUER_LOGE("%s resource get err", __FUNCTION__);
                }

                // 下载完home banner后开始下载discovery
                if (imageIfDownOver(IMG_DOWN_HOME_BANNER) == TRUE) {
                    imageStartDownResource(IMG_DOWN_DISCOVERY);
                }
            }
            break;
            case IMG_DOWN_SKILL_BANNER:
            {
                //todo
            }
            break;
            case IMG_DOWN_SKILL:
            {
                //todo
            }
            break;
            case IMG_DOWN_CENTER_BANNER:
            {
                //not here

            }
            break;
            case IMG_DOWN_RENDER_STANDARDCARD:
            {
                //not here
            }
            break;
            case IMG_DOWN_RENDER_LISTCARD:
            {
                //not here
            }
            break;
            case IMG_DOWN_PARENTS_ASSISTS:
            {
                //not here
            }
            break;
            default:
            break;
        }

        imageContinueDownResource(p_msg_param->type);
        DUER_LOGI("(%s)[duer_watch]:", __FUNCTION__);
    }
}

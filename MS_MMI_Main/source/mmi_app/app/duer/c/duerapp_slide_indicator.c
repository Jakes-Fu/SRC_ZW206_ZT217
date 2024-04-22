// Copyright (2021) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_slide_indicator.c
 * Auth: guyahui (guyahui@baidu.com)
 * Desc: duerapp slide indicator.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 11/2021        guyahui           Create                                   *
******************************************************************************/

#include "duerapp_slide_indicator.h"
#include "duerapp_image.h"
#include "guicommon.h"
#include "guilcd.h"
#include "guires.h"
#include "lightduer_log.h"
#include "mmidisplay_color.h"



/**--------------------------------------------------------------------------*/
/**                         MACRO DEFINITION                                 */
/**--------------------------------------------------------------------------*/
#define DUERAPP_SLIDE_INDICATOR_LAY_X           0
#define DUERAPP_SLIDE_INDICATOR_LAY_Y           0
#define DUERAPP_SLIDE_INDICATOR_LAY_WIDTH       240
#define DUERAPP_SLIDE_INDICATOR_LAY_HEIGHT      MMI_MAINSCREEN_HEIGHT
#define DUERAPP_SLIDE_INDICATOR_INTERVAL        18

/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          CONSTANT ARRAY                                   */
/*---------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*/
/**                         EXTERNAL DECLARE                                 */
/**--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*/
/**                         STATIC DEFINITION                                */
/**--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
LOCAL void CreateIndicatorLayer(MMI_WIN_ID_T win_id, uint32 width, uint32 height, GUI_LCD_DEV_INFO *lcd_dev_ptr);
LOCAL void ReleaseIndicatorLayer(GUI_LCD_DEV_INFO *lcd_dev_ptr);
LOCAL void DisplayIndicator(MMI_WIN_ID_T win_id);

/**--------------------------------------------------------------------------*
**                         LOCAL VARIABLES                                   *
**--------------------------------------------------------------------------*/
LOCAL const int16 indicator_start_y[DUERAPP_SLIDE_IND_POS_TYPE_NUM] = {10, 210};

typedef struct {
    duerapp_silde_indicator_setting_t set;
    GUI_LCD_DEV_INFO indicator_lcd_dev;
    GUI_POINT_T indicator_start;
    BOOLEAN working;
} duerapp_slide_indicator_ctx_t;

// LOCAL duerapp_slide_indicator_ctx_t s_ctx = {
//     .working = FALSE,
// };
LOCAL duerapp_slide_indicator_ctx_t s_ctx;

PUBLIC int32 DUERAPP_SlideSettingInit(duerapp_silde_indicator_setting_t *set)
{
    duerapp_silde_indicator_position_t pos = DUERAPP_SLIDE_IND_POS_TYPE_TOP;
    uint8 page_cnt = 0;
    s_ctx.working = FALSE;
    if (s_ctx.working) {
        DUER_LOGE("%s->slide is working", __FUNCTION__);
        return -1;
    }
    if (PNULL == set) {
        DUER_LOGE("%s->parameter error", __FUNCTION__);
        return -1;
    }
    s_ctx.set = *set;
    page_cnt = set->page_cnt;
    pos = set->ind_pos;
    s_ctx.indicator_start.x = DUERAPP_SLIDE_INDICATOR_LAY_X + 
                            (DUERAPP_SLIDE_INDICATOR_LAY_WIDTH/2) - 
                            (page_cnt - 1)*(DUERAPP_SLIDE_INDICATOR_INTERVAL/2);
    if (pos < DUERAPP_SLIDE_IND_POS_TYPE_NUM) {
        s_ctx.indicator_start.y = indicator_start_y[pos];
    } else {
        s_ctx.indicator_start.y = indicator_start_y[DUERAPP_SLIDE_IND_POS_TYPE_BOTTOM];
    }
    s_ctx.working = TRUE;
}

PUBLIC MMI_RESULT_E DUERAPP_SlideHandleCb(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    //修改导航点的显示为固定图
    DUER_LOGI("slideHandleCb win_id = %x, msg_id = %x, param = 0x%08x", win_id, msg_id, param);
    switch (msg_id)
    {
        case MSG_SLIDEPAGE_OPENED:
        {
            DUER_LOGI("MSG_SLIDEPAGE_OPENED");
#if 0
            CreateIndicatorLayer(win_id, DUERAPP_SLIDE_INDICATOR_LAY_WIDTH, DUERAPP_SLIDE_INDICATOR_LAY_HEIGHT, &s_ctx.indicator_lcd_dev);
            if (!UILAYER_IsBltLayer(&s_ctx.indicator_lcd_dev)) {
                UILAYER_APPEND_BLT_T append;
                append.layer_level = UILAYER_LEVEL_NORMAL;
                append.lcd_dev_info = s_ctx.indicator_lcd_dev;
                UILAYER_AppendBltLayer(&append);
            }
#endif
            break;
        }
        case MSG_SLIDEPAGE_CLOSED:
        {
            DUER_LOGI("MSG_SLIDEPAGE_CLOSED");
#if 0
            ReleaseIndicatorLayer(&s_ctx.indicator_lcd_dev);
#endif
            break;
        }
        case MSG_SLIDEPAGE_GETFOCUS:
        {
            DUER_LOGI("MSG_SLIDEPAGE_GETFOCUS");
#if 0
            if (!UILAYER_IsBltLayer(&s_ctx.indicator_lcd_dev)) {
                UILAYER_APPEND_BLT_T append;
                append.layer_level = UILAYER_LEVEL_NORMAL;
                append.lcd_dev_info = s_ctx.indicator_lcd_dev;
                UILAYER_AppendBltLayer(&append);
            }
#endif
            break;
        }
        case MSG_SLIDEPAGE_LOSEFOCUS:
        {
            DUER_LOGI("MSG_SLIDEPAGE_LOSEFOCUS");
#if 0
            UILAYER_RemoveBltLayer(&s_ctx.indicator_lcd_dev);
            s_ctx.working = FALSE;
#endif
        } break;

        case MSG_SLIDEPAGE_PAGECHANGED:
        {
            DUER_LOGI("MSG_SLIDEPAGE_PAGECHANGED");
#if 0
            DisplayIndicator(win_id);
#endif
            break;
        }
        default:
            return MMI_RESULT_FALSE;
    }
    return recode;
}

LOCAL void CreateIndicatorLayer(MMI_WIN_ID_T win_id, uint32 width, uint32 height, GUI_LCD_DEV_INFO *lcd_dev_ptr)
{
#ifdef UI_MULTILAYER_SUPPORT
    UILAYER_CREATE_T create_info = {0};
    if (PNULL == lcd_dev_ptr){
        return;
    }

    lcd_dev_ptr->lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_ptr->block_id = UILAYER_NULL_HANDLE;

    create_info.lcd_id = GUI_MAIN_LCD_ID;
    create_info.owner_handle = win_id;
    create_info.offset_x = DUERAPP_SLIDE_INDICATOR_LAY_X;
    create_info.offset_y = DUERAPP_SLIDE_INDICATOR_LAY_Y;
    create_info.width = width;
    create_info.height = height;
    create_info.is_bg_layer = FALSE;
    create_info.is_static_layer = TRUE;

    UILAYER_CreateLayer(
        &create_info,
        lcd_dev_ptr
        );

    UILAYER_SetLayerColorKey(lcd_dev_ptr, TRUE, UILAYER_TRANSPARENT_COLOR);
    UILAYER_Clear(lcd_dev_ptr);
#endif
}

LOCAL void ReleaseIndicatorLayer(GUI_LCD_DEV_INFO *lcd_dev_ptr)
{
    DUER_LOGI("prepare release indicator layer");
    if(lcd_dev_ptr && lcd_dev_ptr->block_id != UILAYER_NULL_HANDLE) {
        DUER_LOGI("do release indicator layer");
        UILAYER_RELEASELAYER(lcd_dev_ptr);
    }
}

LOCAL void DisplayIndicator(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T title_rect = {0};
    tWatchSlidePage *entity = WatchSLIDEPAGE_GetSlideInfoByHandle(s_ctx.set.slide_handle);
    MMI_IMAGE_ID_T img_id = 0;
    GUI_LCD_DEV_INFO lcd_dev_info = s_ctx.indicator_lcd_dev;//{0, 0};
    uint8 i = 0;
    GUI_POINT_T start_point = {0};
    start_point.x = s_ctx.indicator_start.x;
    start_point.y = s_ctx.indicator_start.y;
    title_rect.left = DUERAPP_SLIDE_INDICATOR_LAY_X;
    title_rect.top = s_ctx.indicator_start.y;
    title_rect.right = title_rect.left + DUERAPP_SLIDE_INDICATOR_WIDTH;
    title_rect.bottom = title_rect.top + DUERAPP_SLIDE_INDICATOR_HEIGHT;
    
    GUI_FillRect(&lcd_dev_info, title_rect, MMI_BLACK_COLOR);
    if (entity != NULL) {
        uint8 cur_page_index = entity->cur_page_index;
        DUER_LOGI("%s->silde page num:%d cur:%d", __FUNCTION__, entity->page_cnt, cur_page_index);
        for (i = 0; i < entity->page_cnt; i++) {
            if (entity->pages[i].is_disabled == TRUE)
                continue;
            img_id = IMAGE_DUER_IC_POINT_OFF;
            if (i == cur_page_index) {
                img_id = IMAGE_DUER_IC_POINT_ON;
            }
            GUIRES_DisplayImg(&start_point,
                PNULL,
                PNULL,
                win_id,
                img_id,
                &lcd_dev_info);
            start_point.x += DUERAPP_SLIDE_INDICATOR_INTERVAL;
        }
    }
}

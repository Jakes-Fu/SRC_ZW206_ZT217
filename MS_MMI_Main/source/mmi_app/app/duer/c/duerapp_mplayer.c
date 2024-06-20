// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_mplayer.c
 * Auth: shichenyu01 (shichenyu01@baidu.com)
 * Desc: duerapp mplayer.
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

#include "dal_time.h"
#include "duerapp.h"
#include "mmicom_time.h"
#include "duerapp_common.h"
#include "duerapp_login.h"
#include "duerapp_qrwin.h"
#include "duerapp_http.h"
#include "duerapp_img_down.h"
#include "duerapp_operate.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_audio_play_utils.h"
#include "duerapp_center.h"
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
/**--------------------------------------------------------------------------*/
/**                         MACRO DEFINITION                                 */
/**--------------------------------------------------------------------------*/
#define DUERAPP_USE_LIST_BUTTON             0
#define DUERAPP_USE_COLLECT_BUTTON          0
#define DUERAPP_USE_VOL_SHOW                1
#define DUERAPP_USE_VIP_BUTTON              1

#define DUERAPP_FREE_LISTEN_STR             L"免费"
#define DUERAPP_VIP_LISTEN_STR              L"会员畅听"

#define DUERAPP_MPLAYER_LAY_X               0
#define DUERAPP_MPLAYER_LAY_Y               0
#define DUERAPP_MPLAYER_LAY_WIDTH           240
#define DUERAPP_MPLAYER_LAY_HEIGHT          240

#define DUERAPP_MUSIC_TITLE_RECT            {10, 6, 230, 39}
#define DUERAPP_MUSIC_SUBTITLE_RECT         {10, 44, 230, 66}
#define DUERAPP_MUSIC_PREVIOUS_RECT         {10, 98, 50, 138}
#define DUERAPP_MUSIC_PLAY_RECT             {94, 97, 146, 149}
#define DUERAPP_MUSIC_PLAY_ANIM_RECT        {65, 68, 174, 177}
#define DUERAPP_MUSIC_NEXT_RECT             {190, 98, 230, 138}
#define DUERAPP_MUSIC_VIP_RECT              {166, 65, 240, 92}
#define DUERAPP_MUSIC_VOLUME_RECT           {101, 193, 137, 221}  //vol show change
#define DUERAPP_MUSIC_LIST_RECT             {10, 183, 57, 230}
#define DUERAPP_MUSIC_COLLECTION_RECT       {68, 183, 115, 230}
#define DUERAPP_MUSIC_VOLUME_MINUS_RECT     {12, 189, 67, 225}      //{125, 183, 172, 230}
#define DUERAPP_MUSIC_VOLUME_PLUS_RECT      {173, 189, 228, 225}    //{183, 183, 230, 230}

#define DUERAPP_MPLAYER_LIST_TITLE_RECT     {10, 6, 230, 32}

#define DUERAPP_MUSIC_WIN_BG                RGB8882RGB565(0x202A7A) //MMI_BLACK_COLOR //RGB8882RGB565(0x202A7A)

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
extern RENDER_PLAYER_INFO       *s_renderPlayerInfo;
extern volatile BOOLEAN         s_user_novip;
extern int s_duration;

LOCAL MMIDUERAPP_MEDIA_STATE_E  s_play_state            = MEDIA_PLAY_STATE_FINISH;
LOCAL BOOLEAN                   s_collet_state          = FALSE;
LOCAL GUI_LCD_DEV_INFO          s_first_lcd_dev_info    = { 0, UILAYER_NULL_HANDLE };
LOCAL img_dsc_t                 *s_img_art              = NULL;
LOCAL int                       s_last_list_index       = 0;
LOCAL BOOLEAN                   s_list_anim_update      = FALSE;
LOCAL BOOLEAN                   gs_net_checked_flag     = FALSE;

/*---------------------------------------------------------------------------*/
/*                          LOCAL FUNCTION DECLARE                           */
/*---------------------------------------------------------------------------*/
LOCAL void OpenWaitingWindow(void);
LOCAL void MusicPreviousButtonCallBack(void);
LOCAL void MusicNextButtonCallBack(void);
LOCAL void MusicPlayButtonCallBack(void);
LOCAL void MusicPauseButtonCallBack(void);
LOCAL void MusicVipCallBack(void);
LOCAL void MusicListButtonCallBack(void);
LOCAL void MusicCollectionButtonCallBack(void);
LOCAL void MusicVolumeMinusCallBack(void);
LOCAL void MusicVolumePlusCallBack(void);
LOCAL void UpdateMediaButton(MMIDUERAPP_MEDIA_STATE_E state,BOOLEAN is_update);
LOCAL void UpdateCollectionButton(BOOLEAN is_collected, BOOLEAN is_update);
LOCAL void UpdatePlayListAnim(void);
LOCAL void UpdateMusicVolume(void);
LOCAL void UpdatePlayListTitle(BOOLEAN is_update);
LOCAL void CoverAnimCircleImg(void);
LOCAL void WatchRec_MusicwinDrawBG(void);
LOCAL void imageArtFree(void);
LOCAL void imageArtDownloadCb(img_dsc_t *img, void *down_param);
LOCAL void imageDownArtPicture(char *url);
LOCAL void MMIDUERAPP_CheckNetId(void);
/*****************************************************************************/
//  Description : PlayInfo窗口的回调函数
//  Global resource dependence : 
//  Author: shichenyu01
//  Note: 
/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePlayInfoWinMsg(
                                    MMI_WIN_ID_T        win_id,
                                    MMI_MESSAGE_ID_E    msg_id,
                                    DPARAM              param
                                    );
/*****************************************************************************/
//  Description : to handle Settings window message
//  Global resource dependence :
//  Author:bin.wang1
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandlePlayListWinMsg(
                                    MMI_WIN_ID_T        win_id,
                                    MMI_MESSAGE_ID_E    msg_id,
                                    DPARAM              param
                                    );
/*****************************************************************************/
//  Description : 音乐播放
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
LOCAL void MMIDUERAPP_MusicOpenWin(
                                    MMI_WIN_ID_T        win_id,
                                    RENDER_PLAYER_INFO  *playerInfo
                                    );

/**--------------------------------------------------------------------------*
**                         LOCAL VARIABLES                                   *
**--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*/
/**                         GLOBAL DEFINITION                                */
/**--------------------------------------------------------------------------*/
// V3改版音乐播放窗口
WINDOW_TABLE(MMI_DUERAPP_MPLAYER_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_PLAY_INFO_WIN_ID),
    WIN_FUNC((uint32)HandlePlayInfoWinMsg),
    WIN_STYLE(WS_DISABLE_COMMON_BG),

    /* 文字显示：主标题和副标题 */
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_MUSIC_TITLE_LABEL_CTRL_ID),
    CREATE_LABEL_CTRL(GUILABEL_ALIGN_MIDDLE, MMI_DUERAPP_MUSIC_AUTHOR_LABEL_CTRL_ID),

    /* 按键组一：上一首、下一首、播放暂停和开通会员 */
    CREATE_ANIM_CTRL(MMI_DUERAPP_MUSIC_PLAY_LIST_ANIM_CTRL_ID, MMI_DUERAPP_PLAY_INFO_WIN_ID),
    // CREATE_ANIM_CTRL(MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID, MMI_DUERAPP_PLAY_INFO_WIN_ID),
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_MUSIC_PREVIOUS, MMI_DUERAPP_MUSIC_PREVIOUS_BUTTON_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_MUSIC_NEXT, MMI_DUERAPP_MUSIC_NEXT_BUTTON_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID),
    #if (DUERAPP_USE_VIP_BUTTON == 1)
        CREATE_BUTTON_CTRL(IMAGE_DUERAPP_VIP_TAB, MMI_DUERAPP_MUSIC_VIP_BUTTON_CTRL_ID),
    #endif

    /* 按键组二：专辑列表、收藏、音量减、音量加 */
    //todo:use list&collection icon
    #if (DUERAPP_USE_LIST_BUTTON == 1)
        CREATE_BUTTON_CTRL(IMAGE_DUERAPP_MUSIC_LIST, MMI_DUERAPP_MUSIC_PLAY_LIST_BUTTON_CTRL_ID),
    #endif
    #if (DUERAPP_USE_COLLECT_BUTTON == 1)
        CREATE_BUTTON_CTRL(IMAGE_NULL, MMI_DUERAPP_MUSIC_COLLECTION_BUTTON_CTRL_ID),
    #endif
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_MUSIC_VOLUME_MINUS, MMI_DUERAPP_MUSIC_MINUS_BUTTON_CTRL_ID),
    CREATE_BUTTON_CTRL(IMAGE_DUERAPP_MUSIC_VOLUME_PLUS, MMI_DUERAPP_MUSIC_PLUS_BUTTON_CTRL_ID),

    END_WIN
};

// V3改版音乐播放列表/专辑列表
WINDOW_TABLE(MMI_DUERAPP_PLAY_LIST_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_PLAY_LIST_WIN_ID),
    WIN_FUNC((uint32)HandlePlayListWinMsg),

    /* 标题：播放列表/当前专辑 */
    // CREATE_LABEL_CTRL(GUILABEL_ALIGN_LEFT, MMI_DUERAPP_MPLAYER_LIST_LABEL_CTRL_ID),
    WIN_TITLE(TXT_NULL),

    /* 列表：播放列表/音乐歌单 */
    CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E, MMI_DUERAPP_MPLAYER_LIST_CTRL_ID),

    END_WIN
};

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
/***************************
 * MPLAYER
 * ***************************/
//CalendarCreateLayer
LOCAL void MplayerCreateLayer(GUI_LCD_DEV_INFO *dev_info_ptr, MMI_HANDLE_T handle)
{
    uint16 logic_lcd_width = 0;
    uint16 logic_lcd_height = 0;
    GUI_RECT_T client_rect = MMITHEME_GetClientRectEx(handle);

    if (UILAYER_IsMultiLayerEnable())
    {
        UILAYER_CREATE_T    create_info = {0};
        GUILCD_GetLogicWidthHeight(MAIN_LCD_ID, &logic_lcd_width, &logic_lcd_height);
        create_info.lcd_id = MAIN_LCD_ID;
        create_info.owner_handle =handle;
        create_info.offset_x = 0;
        create_info.offset_y = client_rect.bottom;
        create_info.width = logic_lcd_width;
        create_info.height = logic_lcd_height-client_rect.top-1;
        create_info.is_bg_layer = FALSE; //TRUE;
        create_info.is_static_layer = FALSE;
        UILAYER_CreateLayer(&create_info, dev_info_ptr);
    }
}
LOCAL void CreateMplayerLayer(MMI_WIN_ID_T win_id, uint32 width, uint32 height, GUI_LCD_DEV_INFO *lcd_dev_ptr)
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
    create_info.offset_x = DUERAPP_MPLAYER_LAY_X; //layerRect.left;
    create_info.offset_y = DUERAPP_MPLAYER_LAY_Y; //layerRect.top;
    create_info.width = width; //(layerRect.right - layerRect.left + 1);
    create_info.height = height; //(layerRect.bottom - layerRect.top + 1);
    create_info.is_bg_layer = FALSE;
    create_info.is_static_layer = FALSE; //TRUE;

    UILAYER_CreateLayer(
        &create_info,
        lcd_dev_ptr
        );

    UILAYER_SetLayerColorKey(lcd_dev_ptr, TRUE, UILAYER_TRANSPARENT_COLOR);
    UILAYER_Clear(lcd_dev_ptr);
#endif
}

PUBLIC MMI_HANDLE_T MMIDUERAPP_CreatePlayInfoWin(ADD_DATA data)
{
    return MMK_CreateWin((uint32*)MMI_DUERAPP_MPLAYER_WIN_TAB, data);
}

LOCAL void MMIDUERAPP_MusicOpenWin(MMI_WIN_ID_T win_id, RENDER_PLAYER_INFO *playerInfo) 
{
    char *title = NULL;
    char *subTitle = NULL;
    wchar *wch_txt_title = NULL;
    int utf8_len_title = 0;
    wchar *wch_txt_subtitle = NULL;
    int utf8_len_subtitle = 0;
    MMI_STRING_T title_text = {0};
    GUI_RECT_T titleRect = DUERAPP_MUSIC_TITLE_RECT;
    MMI_STRING_T subtitle_text = {0};
    GUI_RECT_T previousRect = DUERAPP_MUSIC_PREVIOUS_RECT;
    GUI_RECT_T nextRect = DUERAPP_MUSIC_NEXT_RECT;
    UILAYER_APPEND_BLT_T append;
    GUI_RECT_T playRect = DUERAPP_MUSIC_PLAY_RECT;
    GUI_RECT_T animRect = DUERAPP_MUSIC_PLAY_ANIM_RECT;
    // #if (DUERAPP_USE_VIP_BUTTON == 1)
    GUI_RECT_T vipRect = DUERAPP_MUSIC_VIP_RECT;
    // #endif // #if (DUERAPP_USE_VIP_BUTTON == 1)
    // #if (DUERAPP_USE_LIST_BUTTON == 1)
    GUI_RECT_T ListRect = DUERAPP_MUSIC_LIST_RECT;
    // #endif // #if (DUERAPP_USE_LIST_BUTTON == 1)
    // #if (DUERAPP_USE_COLLECT_BUTTON == 1)
    GUI_RECT_T CollectionRect = DUERAPP_MUSIC_COLLECTION_RECT;
    // #endif // #if (DUERAPP_USE_COLLECT_BUTTON == 1)
    GUI_RECT_T minusRect = DUERAPP_MUSIC_VOLUME_MINUS_RECT;
    GUI_BG_T button_press_fg = {0};
    GUI_RECT_T plusRect = DUERAPP_MUSIC_VOLUME_PLUS_RECT;
    GUI_BG_T button_press_fg_1 = {0};

    GUI_RECT_T subRect = DUERAPP_MUSIC_SUBTITLE_RECT;

    DUER_LOGI("%s", __FUNCTION__);

    if (playerInfo != PNULL)
    {
        title = playerInfo->title;
        subTitle = playerInfo->subtitle;

        
        utf8_len_title = strlen(title);
        wch_txt_title = DUER_CALLOC(1, (utf8_len_title + 1) * sizeof(wchar));
        GUI_UTF8ToWstr(wch_txt_title, utf8_len_title, title, utf8_len_title);

        utf8_len_subtitle = strlen(subTitle);
        wch_txt_subtitle = DUER_CALLOC(1, (utf8_len_subtitle + 1) * sizeof(wchar));
        GUI_UTF8ToWstr(wch_txt_subtitle, utf8_len_subtitle, subTitle, utf8_len_subtitle);

        title_text.wstr_len = MMIAPICOM_Wstrlen(wch_txt_title);
        title_text.wstr_ptr = wch_txt_title;

        GUILABEL_SetRect(MMI_DUERAPP_MUSIC_TITLE_LABEL_CTRL_ID,&titleRect,FALSE);
        GUILABEL_SetFont(MMI_DUERAPP_MUSIC_TITLE_LABEL_CTRL_ID, WATCH_DEFAULT_BIG_FONT, MMI_WHITE_COLOR);
        GUILABEL_SetAlign(MMI_DUERAPP_MUSIC_TITLE_LABEL_CTRL_ID, GUILABEL_ALIGN_MIDDLE);
        GUILABEL_SetText(MMI_DUERAPP_MUSIC_TITLE_LABEL_CTRL_ID ,&title_text, TRUE);
        MMK_SetAtvCtrl(win_id,MMI_DUERAPP_MUSIC_TITLE_LABEL_CTRL_ID);

        subtitle_text.wstr_len = MMIAPICOM_Wstrlen(wch_txt_subtitle);
        subtitle_text.wstr_ptr = wch_txt_subtitle;

        GUILABEL_SetRect(MMI_DUERAPP_MUSIC_AUTHOR_LABEL_CTRL_ID,&subRect,FALSE);
        GUILABEL_SetFont(MMI_DUERAPP_MUSIC_AUTHOR_LABEL_CTRL_ID, WATCH_DEFAULT_NORMAL_FONT, DUERAPP_QUERY_FONT_COLOR);
        GUILABEL_SetAlign(MMI_DUERAPP_MUSIC_AUTHOR_LABEL_CTRL_ID, GUILABEL_ALIGN_MIDDLE);
        GUILABEL_SetText(MMI_DUERAPP_MUSIC_AUTHOR_LABEL_CTRL_ID ,&subtitle_text, TRUE);

        DUER_FREE(wch_txt_title);
        DUER_FREE(wch_txt_subtitle);
    }

    {
        /* 上一首 */
        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_PREVIOUS_BUTTON_CTRL_ID, &previousRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_PREVIOUS_BUTTON_CTRL_ID, TRUE);
        //GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_PREVIOUS_BUTTON_CTRL_ID,FALSE);
        //GUIAPICTRL_SetState(MMI_DUERAPP_MUSIC_PREVIOUS_BUTTON_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_PREVIOUS_BUTTON_CTRL_ID, MusicPreviousButtonCallBack);
    }

    {
        /* 下一首 */
        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_NEXT_BUTTON_CTRL_ID, &nextRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_NEXT_BUTTON_CTRL_ID, TRUE);
        //GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_NEXT_BUTTON_CTRL_ID,FALSE);
        //GUIAPICTRL_SetState(MMI_DUERAPP_MUSIC_NEXT_BUTTON_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_NEXT_BUTTON_CTRL_ID, MusicNextButtonCallBack);
    }

    {
        /* 播放暂停 */

        {
            if(s_first_lcd_dev_info.block_id == UILAYER_NULL_HANDLE) {
                // MplayerCreateLayer(&s_first_lcd_dev_info, MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID);
                CreateMplayerLayer(MMI_DUERAPP_PLAY_INFO_WIN_ID, DUERAPP_MPLAYER_LAY_WIDTH, DUERAPP_MPLAYER_LAY_HEIGHT, &s_first_lcd_dev_info);
                if (!UILAYER_IsBltLayer(&s_first_lcd_dev_info)) {
                    append.layer_level = UILAYER_LEVEL_NORMAL;
                    append.lcd_dev_info = s_first_lcd_dev_info;
                    UILAYER_AppendBltLayer(&append);
                }
            }
        }

        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, &playRect);
        //GUIAPICTRL_SetState(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, FALSE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, MusicPlayButtonCallBack);
        MMK_SetCtrlLcdDevInfo(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, &s_first_lcd_dev_info);

        GUIAPICTRL_SetBothRect(MMI_DUERAPP_MUSIC_PLAY_LIST_ANIM_CTRL_ID, &animRect);
        GUIANIM_SetVisible(MMI_DUERAPP_MUSIC_PLAY_LIST_ANIM_CTRL_ID, FALSE, TRUE);

        {
#if 0
            GUI_RECT_T animCircleRect = DUERAPP_MUSIC_PLAY_ANIM_RECT;
            GUIAPICTRL_SetBothRect(MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID, &animCircleRect);
            // GUIANIM_SetVisible(MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID, FALSE, TRUE);
            // GUIANIM_SetBgImg(MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID, IMAGE_DUERAPP_MUSIC_ANIM_CIRCLE);
            MMK_SetCtrlLcdDevInfo(MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID, &s_first_lcd_dev_info);

            GUIANIM_CTRL_INFO_T     ctrl_info = {0};
            GUIANIM_DATA_INFO_T     data_info = {0};
            GUIANIM_DISPLAY_INFO_T  display_info = {0};
            MMI_HANDLE_T            anim_handle = NULL;

            anim_handle = MMK_GetCtrlHandleByWin(MMI_DUERAPP_PLAY_INFO_WIN_ID, MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID);
            ctrl_info.is_ctrl_id        = TRUE;
            ctrl_info.ctrl_id           = anim_handle;
            display_info.align_style    = GUIANIM_ALIGN_HVMIDDLE;
            display_info.bg.bg_type     = GUI_BG_NONE;
            display_info.is_update      = TRUE;

            if (s_img_art) {
                data_info.img_id = IMAGE_DUERAPP_MUSIC_ANIM_CIRCLE;

                display_info.is_zoom = TRUE;
                display_info.is_disp_one_frame = TRUE;
            }

            GUIANIM_SetParam(&ctrl_info, &data_info, PNULL, &display_info);

            GUIANIM_SetVisible(MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID, TRUE, TRUE);
            GUIANIM_PlayAnim(anim_handle);
#else

#endif
        }
    }

#if (DUERAPP_USE_VIP_BUTTON == 1)
    {
        /* 开通会员 */
        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_VIP_BUTTON_CTRL_ID, &vipRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_VIP_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_VIP_BUTTON_CTRL_ID,FALSE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_VIP_BUTTON_CTRL_ID, MusicVipCallBack);
        DUER_LOGI("s_user_novip -> %d", s_user_novip);
        GUIBUTTON_SetVisible(MMI_DUERAPP_MUSIC_VIP_BUTTON_CTRL_ID, s_user_novip ? TRUE : FALSE, TRUE);

        MMK_SetCtrlLcdDevInfo(MMI_DUERAPP_MUSIC_VIP_BUTTON_CTRL_ID, &s_first_lcd_dev_info);
    }
#endif

#if (DUERAPP_USE_LIST_BUTTON == 1)
    {
        /* 专辑列表 */
        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_PLAY_LIST_BUTTON_CTRL_ID, &ListRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_PLAY_LIST_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_PLAY_LIST_BUTTON_CTRL_ID, MusicListButtonCallBack);
    }
#endif

#if (DUERAPP_USE_COLLECT_BUTTON == 1)
    {
        /* 收藏 */
        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_COLLECTION_BUTTON_CTRL_ID, &CollectionRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_COLLECTION_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_COLLECTION_BUTTON_CTRL_ID, MusicCollectionButtonCallBack);
    }
#endif

    {
        /* 音量减 */
        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_MINUS_BUTTON_CTRL_ID, &minusRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_MINUS_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_MINUS_BUTTON_CTRL_ID,FALSE);

        button_press_fg.bg_type = GUI_BG_IMG;
        button_press_fg.img_id = IMAGE_DUERAPP_MUSIC_VOLUME_MINUS_PRESS;
        GUIBUTTON_SetPressedFg(MMI_DUERAPP_MUSIC_MINUS_BUTTON_CTRL_ID, &button_press_fg);
        //GUIAPICTRL_SetState(MMI_DUERAPP_MUSIC_NEXT_BUTTON_CTRL_ID, GUICTRL_STATE_TOPMOST, TRUE);

        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_MINUS_BUTTON_CTRL_ID, MusicVolumeMinusCallBack);
    }

    {
        /* 音量增 */
        GUIBUTTON_SetRect(MMI_DUERAPP_MUSIC_PLUS_BUTTON_CTRL_ID, &plusRect);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_PLUS_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_PLUS_BUTTON_CTRL_ID,FALSE);

        button_press_fg_1.bg_type = GUI_BG_IMG;
        button_press_fg_1.img_id = IMAGE_DUERAPP_MUSIC_VOLUME_PLUS_PRESS;
        GUIBUTTON_SetPressedFg(MMI_DUERAPP_MUSIC_PLUS_BUTTON_CTRL_ID, &button_press_fg_1);

        GUIBUTTON_SetCallBackFunc(MMI_DUERAPP_MUSIC_PLUS_BUTTON_CTRL_ID, MusicVolumePlusCallBack);
    }
   
    MMK_UpdateScreen();

    if (playerInfo != NULL && playerInfo->url) {
        MMIDUERAPP_CreateMiguWin(playerInfo->url);
    }

    if (playerInfo != NULL && playerInfo->art_src_url) {
        // V2版本调试：暂关闭播放器页面的背景图片加载功能
        // imageDownArtPicture(playerInfo->art_src_url);
    }
}

LOCAL void MusicPreviousButtonCallBack(void) 
{
    DUER_LOGI("%s", __FUNCTION__);
    OpenWaitingWindow();
    duer_audio_play_previous();
}

LOCAL void MusicNextButtonCallBack(void) 
{
    DUER_LOGI("%s", __FUNCTION__);
    OpenWaitingWindow();
    duer_audio_play_next();
}

LOCAL void MusicPlayButtonCallBack(void) 
{
    duer_audio_playinfo_t playinfo;
    DUER_LOGI("%s", __FUNCTION__);
    if (s_user_novip)
    {
        duer_audio_player_get_current_playinfo(&playinfo);
        if (playinfo.type == AUDIO_TYPE_DCS_SPEECH
            && playinfo.status == AUDIO_PLAY_STATUS_PLAY)
        {
            return;
        }
    }
    duer_audio_play_toggle();
    UpdateMediaButton(MEDIA_PLAY_STATE_STOP, FALSE);
}

LOCAL void MusicPauseButtonCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    duer_audio_play_toggle();
}

LOCAL void MusicVipCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
        MMK_CloseWin(MMI_DUERAPP_PLAY_INFO_WIN_ID);
    }

    //支付免登录
    duer_send_link_click_url(LINKCLICK_BUY_VIP);

}

LOCAL void MusicListButtonCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);

    MMIDUERAPP_CreatePlayListWin((ADD_DATA)s_last_list_index);
}

LOCAL void MusicCollectionButtonCallBack(void)
{
    wchar *wstr_ptr = L"收藏成功";
    DUER_LOGI("%s", __FUNCTION__);

#if 1 //test

    return; //第一版没有收藏功能

    //todo:collect action
    if (s_collet_state) {
        s_collet_state = FALSE;
    } else {
        s_collet_state = TRUE;
    }
    UpdateCollectionButton(s_collet_state, FALSE);
#endif
    duerapp_show_toast(wstr_ptr);
}

LOCAL int volume_len = 25;
LOCAL void MusicVolumeMinusCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    duer_audio_dac_adj_volume(-volume_len);
    UpdateMusicVolume();

#if (DUERAPP_USE_VOL_SHOW == 1)
    MMK_SendMsg(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_FULL_PAINT, PNULL); //vol show change
#endif
}
LOCAL void MusicVolumePlusCallBack(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    duer_audio_dac_adj_volume(volume_len);
    UpdateMusicVolume();

#if (DUERAPP_USE_VOL_SHOW == 1)
    MMK_SendMsg(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_FULL_PAINT, PNULL); //vol show change
#endif
}

LOCAL void UpdateMediaButton(MMIDUERAPP_MEDIA_STATE_E state, BOOLEAN is_update)
{
    duer_audio_playinfo_t playinfo;
    MMIDUERAPP_MEDIA_STATE_E play_state = MEDIA_PLAY_STATE_STOP;
    GUI_BG_T buttonFg = {0};

    DUER_LOGI("%s", __FUNCTION__);
    duerapp_close_waiting();
    if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID) && MMK_IsFocusWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
        DUER_LOGI("%s", __FUNCTION__);
        duer_audio_player_get_current_playinfo(&playinfo);
        if (playinfo.status == AUDIO_PLAY_STATUS_PLAY && playinfo.type == AUDIO_TYPE_DCS_AUDIO) 
        {
            play_state = MEDIA_PLAY_STATE_PLAYING;
        }
        if (playinfo.url) {
            DUER_LOGI("%s playinfo_url(%s)", __FUNCTION__, playinfo.url);
            DUER_FREE(playinfo.url);
            playinfo.url = NULL;
        }
        DUER_LOGI("%s", __FUNCTION__);
        // 首次进去刷新
        if (!is_update) {
            if (play_state == s_play_state) {
                return;
            }
        }
        s_play_state = play_state;
        DUER_LOGI("%s", __FUNCTION__);

        if(s_first_lcd_dev_info.block_id != UILAYER_NULL_HANDLE) {
            DUER_LOGI("%s", __FUNCTION__);
            UILAYER_Clear(&s_first_lcd_dev_info);
        }
        if (s_img_art) {
            CoverAnimCircleImg();
        }
        DUER_LOGI("%s", __FUNCTION__);
        buttonFg.bg_type = GUI_BG_IMG;
        if (play_state == MEDIA_PLAY_STATE_PLAYING) {
            buttonFg.img_id = IMAGE_DUERAPP_MUSIC_PAUSE;
        } else {
            buttonFg.img_id = IMAGE_DUERAPP_MUSIC_PLAY;
        }
        GUIBUTTON_SetFg(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, &buttonFg);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID, FALSE);
        MMK_SendMsg(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_FULL_PAINT, PNULL); //must

        DUER_LOGI("%s", __FUNCTION__);
    }
}

LOCAL void UpdateCollectionButton(BOOLEAN is_collected, BOOLEAN is_update)
{
    static BOOLEAN first_f = TRUE;
    static BOOLEAN last_is_collected = FALSE;
    GUI_BG_T buttonFg = {0};

#if (DUERAPP_USE_COLLECT_BUTTON == 0)
    return;
#endif
    DUER_LOGI("%s", __FUNCTION__);

    duerapp_close_waiting();
    if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID) &&
            MMK_IsFocusWin(MMI_DUERAPP_PLAY_INFO_WIN_ID))
    {
        // 首次进去刷新
        if (!first_f) {
            if (!is_update) {
                if (is_collected == last_is_collected) {
                    return;
                }
            }
        }
        first_f = FALSE;
        last_is_collected = is_collected;

        buttonFg.bg_type = GUI_BG_IMG;
        if (is_collected == TRUE) {
            buttonFg.img_id = IMAGE_DUERAPP_MUSIC_COLLECTED;
        } else {
            buttonFg.img_id = IMAGE_DUERAPP_MUSIC_COLLECT;
        }
        GUIBUTTON_SetFg(MMI_DUERAPP_MUSIC_COLLECTION_BUTTON_CTRL_ID, &buttonFg);
        GUIBUTTON_SetHandleLong(MMI_DUERAPP_MUSIC_COLLECTION_BUTTON_CTRL_ID, TRUE);
        GUIBUTTON_SetRunSheen(MMI_DUERAPP_MUSIC_COLLECTION_BUTTON_CTRL_ID, FALSE);
        MMK_SendMsg(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void UpdatePlayListAnim(void)
{
    GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    GUIANIM_DATA_INFO_T     data_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    MMI_HANDLE_T            anim_handle = NULL;

    DUER_LOGI("%s", __FUNCTION__);

    if (!MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID) ||
        !MMK_IsFocusWin(MMI_DUERAPP_PLAY_INFO_WIN_ID))
    {
        return;
    }

    {
        anim_handle = MMK_GetCtrlHandleByWin(MMI_DUERAPP_PLAY_INFO_WIN_ID, MMI_DUERAPP_MUSIC_PLAY_LIST_ANIM_CTRL_ID);
        ctrl_info.is_ctrl_id        = TRUE;
        ctrl_info.ctrl_id           = anim_handle;
        display_info.align_style    = GUIANIM_ALIGN_HVMIDDLE;
        display_info.bg.bg_type     = GUI_BG_COLOR;
        display_info.is_update      = TRUE;

        if (s_img_art) {
            // data_info.is_bitmap = FALSE;
            // data_info.is_free_bitmap = FALSE;
            // data_info.is_save_data = FALSE;
            data_info.data_ptr = s_img_art->data;
            data_info.data_size = s_img_art->data_size;

            display_info.is_zoom = TRUE;
            display_info.is_disp_one_frame = TRUE;
        }

        GUIANIM_SetParam(&ctrl_info, &data_info, PNULL, &display_info);
        GUIANIM_SetVisible(MMI_DUERAPP_MUSIC_PLAY_LIST_ANIM_CTRL_ID, TRUE, TRUE);
        GUIANIM_PlayAnim(anim_handle);
    }

    CoverAnimCircleImg();

    // {
    //     GUIANIM_CTRL_INFO_T     ctrl_info = {0};
    //     GUIANIM_DATA_INFO_T     data_info = {0};
    //     GUIANIM_DISPLAY_INFO_T  display_info = {0};
    //     MMI_HANDLE_T            anim_handle = NULL;

    //     anim_handle = MMK_GetCtrlHandleByWin(MMI_DUERAPP_PLAY_INFO_WIN_ID, MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID);
    //     ctrl_info.is_ctrl_id        = TRUE;
    //     ctrl_info.ctrl_id           = anim_handle;
    //     display_info.align_style    = GUIANIM_ALIGN_HVMIDDLE;
    //     display_info.bg.bg_type     = GUI_BG_COLOR;
    //     display_info.is_update      = TRUE;

    //     if (s_img_art) {
    //         data_info.img_id = IMAGE_DUERAPP_MUSIC_ANIM_CIRCLE;

    //         display_info.is_zoom = TRUE;
    //         display_info.is_disp_one_frame = TRUE;
    //     }

    //     GUIANIM_SetParam(&ctrl_info, &data_info, PNULL, &display_info);

    //     GUIANIM_SetVisible(MMI_DUERAPP_MUSIC_PLAY_CIRCLE_ANIM_CTRL_ID, TRUE, TRUE);
    //     GUIANIM_PlayAnim(anim_handle);
    // }

    s_list_anim_update = FALSE;
}

LOCAL void UpdateMusicVolume(void)
{
#if (DUERAPP_USE_VOL_SHOW == 1) //vol show change
    GUI_LCD_DEV_INFO    lcd_dev_info    = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T          image_rect      = DUERAPP_MUSIC_VOLUME_RECT;
    MMI_IMAGE_ID_T      image_id        = IMAGE_DUERAPP_MUSIC_VOLUME_0;
    MMI_WIN_ID_T        win_id          = MMI_DUERAPP_PLAY_INFO_WIN_ID;
    int vol = 0;
    GUI_POINT_T start_point = {0};

    DUER_LOGI("%s", __FUNCTION__);
    if (MMK_IsOpenWin(win_id))
    {
        duer_audio_dac_get_volume(&vol);
        DUER_LOGI("music vol: %d", vol);
        if (vol <= 0) {
            image_id = IMAGE_DUERAPP_MUSIC_VOLUME_0;
        } else if (vol > 0 && vol <= 25) {
            image_id = IMAGE_DUERAPP_MUSIC_VOLUME_1;
        } else if (vol > 25 && vol <= 50) {
            image_id = IMAGE_DUERAPP_MUSIC_VOLUME_2;
        } else if (vol > 50 && vol <= 75) {
            image_id = IMAGE_DUERAPP_MUSIC_VOLUME_3;
        } else if (vol > 75) {
            image_id = IMAGE_DUERAPP_MUSIC_VOLUME_4;
        }
        start_point.x = 101;
        start_point.y = 193;
        GUIRES_DisplayImg(&start_point,PNULL,PNULL
            ,win_id
            ,image_id
            ,&lcd_dev_info);
    }
#else
    int8                vol_level   = 0;
    MMI_WIN_ID_T        win_id      = MMI_DUERAPP_PLAY_INFO_WIN_ID;
    int vol = 0;
    wchar wstr_ptr[20] = {0};
    wchar *vol_txt = NULL;
    int utf8_len_title = 0;

    if (MMK_IsOpenWin(win_id))
    {
        duer_audio_dac_get_volume(&vol);
        DUER_LOGI("music vol: %d", vol);
        if (vol <= 0) {
            vol_level = 0;
        } else if (vol > 0 && vol <= 20) {
            vol_level = 1;
        } else if (vol > 20 && vol <= 40) {
            vol_level = 2;
        } else if (vol > 40 && vol <= 60) {
            vol_level = 3;
        } else if (vol > 60) {
            vol_level = 4;
        } else {
            vol_level = 4;
        }

        sprintf(wstr_ptr, "%s%d", "当前音量:", vol_level);

        utf8_len_title = strlen(wstr_ptr);
        vol_txt = DUER_CALLOC(1, (utf8_len_title + 1) * sizeof(wchar));
        GUI_UTF8ToWstr(vol_txt, utf8_len_title, wstr_ptr, utf8_len_title);

        duerapp_show_toast(vol_txt);
        // duerapp_show_waiting(vol_txt);
        DUER_FREE(vol_txt);
    }
#endif
}

LOCAL void UpdatePlayListTitle(BOOLEAN is_update)
{
    MMI_STRING_T title_text = {0};
    DUER_LOGI("%s", __FUNCTION__);
    if (!MMK_IsOpenWin(MMI_DUERAPP_PLAY_LIST_WIN_ID) || !MMK_IsFocusWin(MMI_DUERAPP_PLAY_LIST_WIN_ID))
    {
        return;
    }

    title_text.wstr_ptr = L"播放列表";
    title_text.wstr_len = MMIAPICOM_Wstrlen(title_text.wstr_ptr);

    GUIWIN_SetTitleText(MMI_DUERAPP_PLAY_LIST_WIN_ID, title_text.wstr_ptr, title_text.wstr_len, is_update);
    GUIWIN_SetTitleBackground(MMI_DUERAPP_PLAY_LIST_WIN_ID, IMAGE_NULL, MMI_BLACK_COLOR);
}

LOCAL void CoverAnimCircleImg(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T animRect = DUERAPP_MUSIC_PLAY_ANIM_RECT;
    GUI_POINT_T start_point = {0};
    start_point.x = animRect.left;
    start_point.y = animRect.top;
    GUIRES_DisplayImg(&start_point, PNULL, PNULL,
        MMI_DUERAPP_PLAY_INFO_WIN_ID,
        IMAGE_DUERAPP_MUSIC_ANIM_CIRCLE,
        &s_first_lcd_dev_info); //s_first_lcd_dev_info
}

LOCAL void OpenWaitingWindow(void)
{
    wchar *wstr_ptr = L"切换中，请稍等...";
    duerapp_show_waiting(wstr_ptr);
}

LOCAL void WatchRec_MusicwinDrawBG(void)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T rect = DUERAPP_FULL_SCREEN_RECT;
    LCD_FillRect(&lcd_dev_info, rect, DUERAPP_MUSIC_WIN_BG);
}

LOCAL void imageArtFree(void)
{
    DUER_LOGI("%s", __FUNCTION__);
    if (s_img_art) {
        DUER_LOGI("%s", __FUNCTION__);
        if (s_img_art->data) {
            DUER_LOGI("%s", __FUNCTION__);
            DUER_FREE(s_img_art->data);
            s_img_art->data = NULL;
        }

        DUER_FREE(s_img_art);
        s_img_art = NULL;
    }
}

LOCAL void imageArtDownloadCb(img_dsc_t *img, void *down_param)
{
    int *p_index = NULL;
    DUER_LOGI("%s", __FUNCTION__);

    if (!img) {
        return;
    }

    p_index = (int *)down_param;
    if (p_index) {
        DUER_LOGI("image down index=%d,p=%p", *p_index, p_index);
    }

    imageArtFree();

    s_img_art = img;

    s_list_anim_update = TRUE;

    #if 0 //更换接口，避免UI线程接口错误使用
    if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_DUERAPP_ANIM_PIC_UPDATE, PNULL, 0);
    }
    #else
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_DUERAPP_ANIM_PIC_UPDATE, PNULL, 0);
    #endif
}

LOCAL void imageDownArtPicture(char *url)
{
    int *p_index_b = NULL;
    DUER_LOGI("%s", __FUNCTION__);
    if (!url) {
        return;
    }

    imageArtFree();

    p_index_b = DUER_CALLOC(1, sizeof(int));
    *p_index_b = 88;
    DUER_LOGI("down index b=%d,p=%p", *p_index_b, p_index_b);
    duer_image_download(url, imageArtDownloadCb, p_index_b);
}

/*****************************************************************************/
LOCAL MMI_RESULT_E HandlePlayInfoWinMsg(
                                           MMI_WIN_ID_T     win_id, 
                                           MMI_MESSAGE_ID_E msg_id, 
                                           DPARAM           param
                                           )
{
    MMI_RESULT_E        recode = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_LCD_0, GUI_BLOCK_0};
    MMI_CTRL_ID_T       ctrl_id = MMI_DUERAPP_MUSIC_PLAY_BUTTON_CTRL_ID;
    GUI_POINT_T start_point = {0};
    RENDER_PLAYER_INFO *playerInfo = PNULL;
    wchar *wstr_ptr = L"播放出错了";
    LOCAL int volume;

    lcd_dev_info.lcd_id     = GUI_MAIN_LCD_ID;
    lcd_dev_info.block_id   = GUI_BLOCK_MAIN;

    DUER_LOGI("msg_id=0x%x", msg_id);

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        s_collet_state = FALSE;
        duer_audio_dac_get_volume(&volume);
        WatchRec_MusicwinDrawBG();
        //playerInfo = (RENDER_PLAYER_INFO*) MMK_GetWinAddDataPtr(win_id);
        MMIDUERAPP_MusicOpenWin(win_id, s_renderPlayerInfo);
        UpdateMediaButton(MEDIA_PLAY_STATE_PLAYING, TRUE);
        UpdateCollectionButton(s_collet_state, TRUE);

        MMIDUERAPP_CheckNetId();
        break;

    case MSG_GET_FOCUS:
        {
            duer_audio_dac_set_volume(volume);
            if (!UILAYER_IsBltLayer(&s_first_lcd_dev_info)) {
                UILAYER_APPEND_BLT_T append;
                append.layer_level = UILAYER_LEVEL_NORMAL;
                append.lcd_dev_info = s_first_lcd_dev_info;
                UILAYER_AppendBltLayer(&append);
            }
            if (s_list_anim_update) {
                UpdatePlayListAnim();
            }
            if (s_img_art) {
                CoverAnimCircleImg();
            }
            UpdateCollectionButton(s_collet_state, TRUE);
        }
        break;

    case MSG_LOSE_FOCUS:
        {
            // UILAYER_RemoveBltLayer(&s_first_lcd_dev_info);
            duerapp_close_waiting();
            duer_audio_dac_get_volume(&volume);
        }
        break;

    case MSG_FULL_PAINT:
        start_point.x = 0;
        start_point.y = 0;
        // GUIRES_DisplayImg(&start_point,PNULL,PNULL
        //     ,win_id
        //     ,IMAGE_DUERAPP_MUSIC_BG
        //     ,&lcd_dev_info);
        WatchRec_MusicwinDrawBG();

        #if (DUERAPP_USE_VOL_SHOW == 1)
            UpdateMusicVolume(); //vol show change
        #endif
        UpdateMediaButton(MEDIA_PLAY_STATE_PLAYING, FALSE);
        break;

    case MSG_CTL_OK:
    case MSG_CTL_CANCEL:
        duerapp_close_waiting();
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        {
            if(s_first_lcd_dev_info.block_id != UILAYER_NULL_HANDLE) {
                UILAYER_RELEASELAYER(&s_first_lcd_dev_info);   /*lint !e506 !e774*/
            }
            imageArtFree();
        }
        s_play_state = MEDIA_PLAY_STATE_FINISH;
        s_collet_state = FALSE;
        break;
    case MSG_DUERAPP_MUSIC_UPDATE:
        //playerInfo = (RENDER_PLAYER_INFO*)param;
        MMIDUERAPP_MusicOpenWin(win_id, s_renderPlayerInfo);
        MMIDUERAPP_CheckNetId();
        break;
    case MSG_DUERAPP_ANIM_PIC_UPDATE:
        UpdatePlayListAnim();
        if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID) && MMK_IsFocusWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
            MMK_SendMsg(MMI_DUERAPP_PLAY_INFO_WIN_ID, MSG_FULL_PAINT, PNULL);
        }
        break;
    case MSG_DUERAPP_MEDIA_BUTTON:
        UpdateMediaButton(MEDIA_PLAY_STATE_PLAYING, TRUE);
        break;
    case MSG_DUERAPP_MUSIC_PLAY_ERR:
        {
            if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_INFO_WIN_ID)) {
                MMK_CloseWin(MMI_DUERAPP_PLAY_INFO_WIN_ID);
            }
            //GUIBUTTON_SetVisible(MMI_DUERAPP_MUSIC_ENTER_BUTTON_CTRL_ID,FALSE,FALSE);
            CTRLFORM_SetChildDisplay(MMI_DUERAPP_HOME_FORM_CTRL_ID, MMI_DUERAPP_HOME_PLAYER_FORM_CTRL_ID, GUIFORM_CHILD_DISP_HIDE);

            duerapp_show_toast(wstr_ptr);
        }
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }

    return recode;
}

/***************************
 * MPLAYER LIST
 * ***************************/
PUBLIC MMI_HANDLE_T MMIDUERAPP_CreatePlayListWin(ADD_DATA data)
{
    DUER_LOGI("%s", __FUNCTION__);
    return MMK_CreateWin((uint32*)MMI_DUERAPP_PLAY_LIST_WIN_TAB, data);
}

// LOCAL void MMIDUERAPP_PlayListTitleCreate(MMI_WIN_ID_T win_id, char *title)
// {
//     DUER_LOGI("%s", __FUNCTION__);

//     wchar *wch_txt_title = NULL;
//     int utf8_len_title = strlen(title);
//     wch_txt_title = DUER_CALLOC(1, (utf8_len_title + 1) * sizeof(wchar));
//     GUI_UTF8ToWstr(wch_txt_title, utf8_len_title, title, utf8_len_title);

//     MMI_STRING_T title_text = {0};
//     title_text.wstr_len = MMIAPICOM_Wstrlen(wch_txt_title);
//     title_text.wstr_ptr = wch_txt_title;

//     GUI_RECT_T titleRect = DUERAPP_MPLAYER_LIST_TITLE_RECT;
//     GUILABEL_SetRect(MMI_DUERAPP_MPLAYER_LIST_LABEL_CTRL_ID, &titleRect, FALSE);
//     GUILABEL_SetFont(MMI_DUERAPP_MPLAYER_LIST_LABEL_CTRL_ID, WATCH_DEFAULT_BIG_FONT, MMI_WHITE_COLOR);
//     GUILABEL_SetAlign(MMI_DUERAPP_MPLAYER_LIST_LABEL_CTRL_ID, GUILABEL_ALIGN_LEFT);
//     GUILABEL_SetText(MMI_DUERAPP_MPLAYER_LIST_LABEL_CTRL_ID, &title_text, TRUE);
//     MMK_SetAtvCtrl(win_id, MMI_DUERAPP_MPLAYER_LIST_LABEL_CTRL_ID);

//     DUER_FREE(wch_txt_title);
// }

LOCAL void MMIDUERAPP_ListItem_Draw(
                                    MMI_CTRL_ID_T listCtrlId,
                                    MMI_STRING_T str1,
                                    MMI_STRING_T str2,
                                    MMI_STRING_T str3,
                                    MMI_STRING_T str4)
{
#if 0
    GUILIST_ITEM_T          item_t          = {0};
    GUILIST_ITEM_DATA_T     item_data       = {0};

    // item_t.item_style = GUIITEM_STYLE_TWO_LINE_NUMBER_TEXT_AND_TEXT;
    // item_t.item_style = GUIITEM_STYLE_TWO_LINE_TEXT_AND_TEXT_AND_TEXT_AND_TEXT;
    item_t.item_style = GUIITEM_STYLE_TWO_LINE_ICON_TWO_TEXT_AND_TEXT_SMALL;

    item_t.item_data_ptr = &item_data;
    // item_t.item_state |= GUIITEM_STATE_SPLIT;
    // item_t.item_state |= GUIITEM_STATE_SELFADAPT_RECT | GUIITEM_STATE_CONTENT_CHECK;

    // item_data.item_content[0].item_data_type            = GUIITEM_DATA_TEXT_BUFFER;
    // item_data.item_content[0].item_data.text_buffer     = str1;
    item_data.item_content[0].item_data_type            = GUIITEM_DATA_IMAGE_ID;
    item_data.item_content[0].item_data.image_id        = IMAGE_CHECK_SELECTED_ICON;

    item_data.item_content[1].item_data_type            = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[1].item_data.text_buffer     = str1;

    item_data.item_content[2].item_data_type            = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[2].item_data.text_buffer     = str2;

    item_data.item_content[3].item_data_type            = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[3].item_data.text_buffer     = str3;

    // Watch_SetAllListState(listCtrlId);
    // GUILIST_SetListState( listCtrlId, GUILIST_STATE_SPLIT_LINE, FALSE );
    // GUILIST_SetListState( listCtrlId, GUILIST_STATE_NEED_HIGHTBAR, FALSE );

    GUILIST_AppendItem(listCtrlId, &item_t);
#else
    GUILIST_ITEM_T          item_t          = {0};
    GUILIST_ITEM_DATA_T     item_data       = {0};

    #if 1 //watch list small
    item_t.item_style = GUIITEM_STYLE_3STR;
    #else
    item_t.item_style = GUIITEM_STYLE_TWO_LINE_NUMBER_TEXT_AND_TEXT;
    #endif
    DUER_LOGI("%s", __FUNCTION__);

    item_t.item_data_ptr = &item_data;

    item_data.item_content[0].item_data_type            = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[0].item_data.text_buffer     = str1;

    item_data.item_content[1].item_data_type            = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[1].item_data.text_buffer     = str2;

    item_data.item_content[2].item_data_type            = GUIITEM_DATA_TEXT_BUFFER;
    item_data.item_content[2].item_data.text_buffer     = str3;

    GUILIST_SetListState( listCtrlId, GUILIST_STATE_NEED_HIGHTBAR, FALSE );

    GUILIST_AppendItem(listCtrlId, &item_t);
#endif
}

LOCAL void MMIDUERAPP_PlayListAppendListItem(
                                    MMI_CTRL_ID_T listCtrlId,
                                    MMI_STRING_T str1,
                                    MMI_STRING_T str2,
                                    uint32 number)
{
    // MMI_STRING_T space_str = {0};
    // WatchCOM_ListItem_Draw_3Str_1Marker(listCtrlId, str1, str2, space_str, number);

    #define WATCHCOM_MARKER_NUM_STR_LEN_MIN           (2)
    #define WATCHCOM_MARKER_NUM_STR_LEN_MAX           (3)
    #define WATCHCOM_MARKER_NUM_DISP_MAX              (100)

    uint8           num_str[WATCHCOM_MARKER_NUM_STR_LEN_MAX]  = {0};
    wchar           temp_str[WATCHCOM_MARKER_NUM_STR_LEN_MAX] = {0};
    MMI_IMAGE_ID_T  image_id    = 0;
    MMI_STRING_T    numStr      = {0};
    MMI_STRING_T    spaceStr      = {0};

    DUER_LOGI("%s", __FUNCTION__);

    if(number != 0)
    {
        if (number <  WATCHCOM_MARKER_NUM_DISP_MAX)
        {
            image_id = common_list_disp_more;//common_list_disp_someinfo;
            MMIAPICOM_Int2Str(number, num_str, WATCHCOM_MARKER_NUM_STR_LEN_MIN);
            MMI_STRNTOWSTR(temp_str, WATCHCOM_MARKER_NUM_STR_LEN_MIN, num_str, WATCHCOM_MARKER_NUM_STR_LEN_MIN, WATCHCOM_MARKER_NUM_STR_LEN_MIN);
            numStr.wstr_ptr = temp_str;
            numStr.wstr_len = WATCHCOM_MARKER_NUM_STR_LEN_MIN;
        }
        else
        {
            image_id = common_list_disp_more;
            temp_str[0] = '9';
            temp_str[1] = '9';
            temp_str[2] = '+';
            numStr.wstr_ptr = temp_str;
            numStr.wstr_len = WATCHCOM_MARKER_NUM_STR_LEN_MAX;
        }
    }

    // WatchCOM_ListItem_Draw_3Str(listCtrlId, numStr, str1, str2);
    // WatchCOM_ListItem_Draw_2Str_1Icon_2Str(listCtrlId, numStr, str1, image_id, spaceStr, str2);
    MMIDUERAPP_ListItem_Draw(listCtrlId, numStr, str1, str2, spaceStr);
    // MMIDUERAPP_ListItem_Draw(listCtrlId, str1, numStr, str2, spaceStr);
}

extern DUEROS_RESOURCE_TYPE_E s_dueros_audio_type;
extern DUEROS_STORY_LIST_ITEM_ST *s_real_story_list;
extern int s_lesson_list_size;
extern int s_discovery_list_size;
extern DUEROS_DISCOVERY_LIST_ITEM_ST *s_lesson_list;
extern DUEROS_DISCOVERY_LIST_ITEM_ST *s_discovery_list;
extern char* s_p_story_url;
LOCAL void MMIDUERAPP_PlayListOpenWin(MMI_WIN_ID_T win_id,MMI_CTRL_ID_T ctrl_id,int index)
{
    DUEROS_DISCOVERY_LIST_ITEM_ST *list_data = NULL;
    int list_size = 0;
    int max_size = 0;
    int i = 0;
    MMI_STRING_T  name_str = {0};
    GUILIST_EMPTY_INFO_T empty_info = {0};
    MMI_STRING_T  vip_str = {0};

    DUER_LOGI("%s", __FUNCTION__);
    if (s_dueros_audio_type == DUER_AUDIO_SKILL) {
        // if (s_real_skill_list) {
        //     MMK_SendMsg(win_id,MSG_DUERAPP_SKILL_CALL_BACK,PNULL);
        // } else {
        //     wchar *text_buffer = L"加载中，请稍等...";
        //     empty_info.text_buffer.wstr_ptr = text_buffer;
        //     empty_info.text_buffer.wstr_len = MMIAPICOM_Wstrlen(text_buffer);
        //     GUILIST_SetEmptyInfo(ctrl_id, &empty_info);
        //     duerapp_get_watch_config();
        // }
    } else {
        DUER_LOGI("story_list index = %d", index);

        UpdatePlayListTitle(TRUE);

        GUILIST_RemoveAllItems(ctrl_id);
        DUER_LOGI("%s", __FUNCTION__);
        if (s_dueros_audio_type == DUER_AUDIO_LESSON) {
            DUER_LOGI("%s", __FUNCTION__);
            list_size = s_lesson_list_size;
            list_data = s_lesson_list;
        } else if (s_dueros_audio_type == DUER_AUDIO_UNICAST_STORY) {
            DUER_LOGI("%s", __FUNCTION__);
            list_size = s_discovery_list_size;
            list_data = s_discovery_list;
        }
        DUER_LOGI("list size=%d", list_size);
        if (index >= list_size) {
            MMK_CloseWin(win_id);
        } else {
            s_real_story_list = list_data[index].res_list_st;
            if (s_real_story_list) {
                DUER_LOGI("%s", __FUNCTION__);
                max_size = list_data[index].res_size;
                GUILIST_SetMaxItem(ctrl_id, max_size, FALSE);
                for (i = 0; i < max_size; i++) {


                    //V2版本未使用mplayer中的list
                    name_str.wstr_ptr = L"test"; //s_real_story_list[i].res_name;
                    name_str.wstr_len = MMIAPICOM_Wstrlen(name_str.wstr_ptr);
                    DUER_LOGI("name(len=%d)=%s", name_str.wstr_len, name_str.wstr_ptr);

                    vip_str.wstr_ptr = L""; //需要配置增加是否收费内容状态后才能显示 //DUERAPP_VIP_LISTEN_STR;
                    vip_str.wstr_len = MMIAPICOM_Wstrlen(vip_str.wstr_ptr);

                    // WatchCOM_ListItem_Draw_1Str(ctrl_id,item_str);
                    MMIDUERAPP_PlayListAppendListItem(ctrl_id, vip_str, name_str, i + 1);
                }
            } else {
                MMK_CloseWin(win_id);
            }
        }
    }
}

LOCAL void MMIDUERAPP_PlayListItem_Click(int index)
{
    char p_url[256] = {0};
    char *query_url = NULL;
    char *res_id = NULL;

    switch(s_dueros_audio_type) {
        case DUER_AUDIO_SKILL:
        {
            // duer_dcs_close_multi_dialog();
            // query_url = s_real_skill_list[index].query_url;
            // sprintf(p_url,s_p_skill_url,query_url);
        }
        break;
        case DUER_AUDIO_UNICAST_STORY:
        case DUER_AUDIO_LESSON:
        // V2版本未使用此mplayer中的list
        #if 0
        if (s_real_story_list[index].res_url) {
            strcpy(p_url,s_real_story_list[index].res_url);
        } else {
           res_id = s_real_story_list[index].res_id;
           sprintf(p_url, s_p_story_url, res_id);
        }
        #endif
        break;
        default:
        break;
    }

    duer_dcs_dialog_cancel();
    DUER_LOGI("dueros linkurl = %s", p_url);
    duer_send_link_click_url(p_url);

    // // 关闭两个窗口
    // if (MMK_IsOpenWin(MMI_DUERAPP_CENTER_LISTBOX_WIN_ID)) 
    // {
    //     MMK_CloseWin(MMI_DUERAPP_CENTER_LISTBOX_WIN_ID);
    // }
    // if (MMK_IsOpenWin(MMI_DUERAPP_ICONLIST_WIN_ID)) {
    //     MMK_CloseWin(MMI_DUERAPP_ICONLIST_WIN_ID);
    // }
    // if (MMK_IsOpenWin(MMI_DUERAPP_CENTER_INFO_WIN_ID)) 
    // {
    //     MMK_CloseWin(MMI_DUERAPP_CENTER_INFO_WIN_ID);
    // }

    if (MMK_IsOpenWin(MMI_DUERAPP_PLAY_LIST_WIN_ID)) {
        MMK_CloseWin(MMI_DUERAPP_PLAY_LIST_WIN_ID);
    }

    duerapp_show_waiting(NULL);
}

LOCAL MMI_RESULT_E  HandlePlayListWinMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_MPLAYER_LIST_CTRL_ID;
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
    int array_index = (int)MMK_GetWinAddDataPtr(win_id);
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            MMIDUERAPP_PlayListOpenWin(win_id, ctrl_id, array_index);
            GUILIST_SetBgColor(ctrl_id, MMI_BLACK_COLOR);
            MMK_SetAtvCtrl(win_id, ctrl_id);

            s_last_list_index = array_index;
            break;
        }

        case MSG_CTL_OK:
        case MSG_CTL_MIDSK:
        case MSG_CTL_PENOK:
        {
            int idx = GUILIST_GetCurItemIndex(ctrl_id);
            MMIDUERAPP_PlayListItem_Click(idx);
            break;
        }
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CLOSE_WINDOW:
            break;
        case MSG_DUERAPP_SKILL_CALL_BACK:
            // MMIDUERAPP_UpdateSkillList(win_id,ctrl_id);
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}

LOCAL void MMIDUERAPP_CheckNetId(void)
{
    wchar *net_tips = L"当前为非Wi-Fi环境，请注意流量消耗";
    
    if (gs_net_checked_flag) {
        return;
    }
    // 如果没有使用wifi，流量提醒
    if ( duer_get_net_id() >= 0/*MMIWIFI_STATUS_CONNECTED != MMIAPIWIFI_GetStatus()*/) 
    {
        gs_net_checked_flag = TRUE;
        duerapp_show_toast(net_tips);

        duer_audio_play_pause();
    }
}

PUBLIC void duerapp_mplayer_netchecked_refresh(void)
{
    gs_net_checked_flag = FALSE;
}


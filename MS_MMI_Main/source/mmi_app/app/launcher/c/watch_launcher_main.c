/*****************************************************************************
** File Name:      watch_launcher_main.c                                                                          *
** Author:                                                                                                                     *
** Date:             02/03/2020                                                                                          *
** Copyright:      2020 Spreadtrum, Incoporated. All Rights Reserved.                                   *
** Description:    Watch Launcher Win
******************************************************************************
**                         Important Edit History                                                                         *
** --------------------------------------------------------------------------*
** DATE            NAME             DESCRIPTION                                                                    *
** 02/03/2020  zhikun.lv        Create
******************************************************************************/

/**--------------------------------------------------------------------------*
**                         Include Files                                     *
**---------------------------------------------------------------------------*/
#include "mmk_type.h"
#include "mmk_timer.h"
#include "guistring.h"
#include "ui_layer.h"
#include "graphics_draw.h"

#include "watch_slidepage.h"
#include "watch_launcher_main.h"

#ifdef PDA_UI_DROPDOWN_WIN
#include "mmidropdownwin_export.h"
#endif

#include "watch_pb_view.h"
#include "watch_stopwatch.h"
#include "watch_gallery_export.h"
#include "watch_charge_win.h"
#ifdef MMI_RECORD_SUPPORT
#include "mmirecord_main.h"
#endif
#include "watch_launcher_editclockwin.h"
#include "watch_sms_main.h"
#include "mmicc_export.h" //for dialpad
#include "mmicom_trace.h"
#include "mmi_default.h"
#ifdef DROPDOWN_NOTIFY_SHORTCUT
#include "watch_notifycenter.h"
#endif
#ifdef ADULT_WATCH_SUPPORT
#include "watch_steps.h"
#include "watch_heartrate.h"
#include "watch_sms_message.h"
#include "mmieng_uitestwin.h"

#endif
#ifdef DYNAMIC_WATCHFACE_SUPPORT
#include "watchface_view.h"
#endif

#include "watch_extra_export.h"

#ifdef BAIDU_DRIVE_SUPPORT
#include "mmibaidu_export.h"
#endif

#ifdef TULING_AI_SUPPORT //图灵机器人
extern void MMIAPIMENU_EnterAiChat(void);
#endif

#ifdef ZDT_ZFB_SUPPORT
extern void MMIZFB_OpenMainWin(void);
#endif

#ifdef LEBAO_MUSIC_SUPPORT //乐宝音乐
#include "mmi_lebao.h"
#endif

#if defined(ZDT_VIDEOCHAT_SUPPORT) && defined(VIDEO_CALL_AGORA_SUPPORT) //声网视频通话
#include "video_call_text.h"
#include "video_call_export.h"
#endif

#include "Mmidc_export.h"
#include "mmicalc_export.h"
#include "watch_alarm_main.h"
#include "zdt_win_export.h"
#include "zdt_app.h"
#include "watch_set_alarmlist.h"
#ifdef SETUP_WIZARD_SUPPORT
#include "setup_wizard.h"
#endif

#include "mmieng_win.h"
#include "watch_common.h"

#include "guiiconlist.h"

#ifdef ZTE_MENU_SUPPORT
#include "zte_menu_win.h"
#include "zte_menu_id.h"
#endif


/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                  *
**---------------------------------------------------------------------------*/
#ifndef TRACE_APP_LAUNCHER
#define TRACE_APP_LAUNCHER
#endif

#define WATCH_LAUNCHER_CLOCK_UPDATE_INTERVAL 30*1000

#define WATCH_LAUNCHER_CLOCK_DIGITAL_UPDATE_INTERVAL (30 * 1000)// W307 notify update
#ifdef DROPDOWN_NOTIFY_SHORTCUT
#define WATCH_LAUNCHER_NOTIFY_TIMER_INTERVAL 1000
#endif
#define WATCH_LAUNCHER_INDICATOR_OFFSET DP2PX_VALUE(18)
#define WATCH_LAUNCHER_INDICATOR_START_POINT DP2PX_POINT(30,6)

//水平方向距离屏幕边距
#define LAUNCHER_HORIZONTAL_PADDING 15
//垂直方向距离屏幕边距
#define LAUNCHER_VERTICAL_PADDING 8
//菜单有多少列
#define LAUNCHER_MENU_COLUMNS 2
//菜单有多少行
#define LAUNCHER_MENU_ROWS 3
//一页有多少个菜单
#define LAUNCHER_MENUS_IN_PAGE 6

//菜单icon水平方向距离背景边距
#define MENU_ICON_HORIZONTAL_PADDING 48
//菜单icon垂直方向距离背景边距
#define MENU_ICON_VERTICAL_PADDING 38

//菜单Tilte水平方向距离背景边距
#define MENU_TEXT_HORIZONTAL_PADDING 10
//菜单Tilte垂直方向距离背景边距
#define MENU_TEXT_VERTICAL_PADDING 10

//菜单 textBook(同步课堂)Tilte水平方向距离背景边距
#define TEXTBOOK_MENU_TEXT_HORIZONTAL_PADDING 108
//菜单textBook(同步课堂)Tilte垂直方向距离背景边距
#define TEXTBOOK_MENU_TEXT_VERTICAL_PADDING 52
//状态栏高度
#define LAUNCHER_STATUSBAR_HEIGHT MMI_STATUSBAR_HEIGHT

/**--------------------------------------------------------------------------*
**                         LOCAL VARIABLES                                   *
**---------------------------------------------------------------------------*/
//handle of slidepage
LOCAL MMI_HANDLE_T s_handle = 0;

//type of clock page win

//clock update timer
LOCAL uint8 s_clock_update_timer = 0;
#ifdef DROPDOWN_NOTIFY_SHORTCUT
LOCAL uint8 s_clock_notify_timer = 0;
#endif
#ifdef ENG_SUPPORT
LOCAL uint8 s_eng_tp_down_count = 0;
#endif

LOCAL GUI_LCD_DEV_INFO s_indi_layer = {0};

LOCAL uint8 s_current_menu_style = 0;

typedef void(*STARTAPPHANDLE)(void);
typedef struct 
{
    uint8 app_index;
    uint8 check_sim;
    MMI_TEXT_ID_T   text_id;//the title of the item
    MMI_IMAGE_ID_T  img_id;//the image of the item
    MMI_IMAGE_ID_T  bg_img_id;//the backgroud image of the item
    STARTAPPHANDLE start_handle;
}APP_LIST_ITEM_T;

const APP_LIST_ITEM_T  g_app_list_info[] = 
{
    MENU_CLASS,
    MENU_PHONEBOOK,//通讯录
    MENU_GALLERY,//相册
#if defined(ZDT_VIDEOCHAT_SUPPORT) && defined(VIDEO_CALL_AGORA_SUPPORT) //声网视频通话
    #ifndef WIN32//视频通话
    MENU_VIDEO_CALL,
    #else
    MENU_VIDEO_INCOMING_TEST,
    #endif
#endif
    MENU_WECHART,//微聊
    MENU_CAMERA,//相机
    MENU_DIAL,//拨号
    MENU_FIND_FRIEND,// 交友
#if defined(ZDT_TOOLS_MENU_SUPPORT)
   MENU_TOOLS,
#endif
    MENU_CALLLOG,
    MENU_WEATHER, //天气
#ifdef ZDT_ZFB_SUPPORT
    MENU_ALIPAY, //支付宝
#endif
#ifdef TULING_AI_SUPPORT
    MENU_AI_CHAT,//图灵AI
#endif
#ifdef XIAODU_SUPPORT
    MENU_XIAODU,
#endif
#ifdef LEBAO_MUSIC_SUPPORT
    MENU_MUSIC,//咪咕音乐
#endif
#if defined(XYSDK_SUPPORT)|| defined(XYSDK_SRC_SUPPORT)
    MENU_XMLY,//喜马拉雅
#endif
#ifdef MAINMENU_STYLE_SUPPORT
    MENU_STYLE,//主菜单风格切换
#endif
#ifdef FOTA_SUPPORT // bao add. wuxx add 20231031
    //MENU_FOTA,
#endif

#ifdef POETRY_LISTEN_SUPPORT
    MENU_POETRY, 
#endif

#ifdef LISTENING_PRATICE_SUPPORT
    MENU_LISTEN, 
#endif

#ifdef WORD_CARD_SUPPORT
    MENU_LEARN_WORD, 
#endif

#ifdef MATH_COUNT_SUPPORT
    MENU_MNEMONICS, 
#endif

#ifdef HANZI_CARD_SUPPORT
    MENU_HANZI, 
#endif

#ifdef ZMT_GPT_SUPPORT
    MENU_AI,
#endif

#ifdef ZMT_PINYIN_SUPPORT
    MENU_PINYIN,
#endif

#ifdef ZMT_YINBIAO_SUPPORT
    MENU_YINBIAO,
#endif

    MENU_SETTINGS, //设置

};

LOCAL uint8 APP_MENU_SIZE = sizeof(g_app_list_info)/sizeof(APP_LIST_ITEM_T);

LOCAL uint16 app_menu_icon_img_width = 0; 
LOCAL uint16 app_menu_icon_img_height = 0; 

LOCAL uint16 app_menu_bg_img_width = 0; 
LOCAL uint16 app_menu_bg_img_height = 0; 

LOCAL BOOLEAN is_page_slide_end = FALSE;

/**--------------------------------------------------------------------------*
**                         LOCAL FUNCTION                                   *
**---------------------------------------------------------------------------*/
LOCAL BOOLEAN IsPressedStatusBarRect(GUI_POINT_T point);

//diplay background
LOCAL void DisplayWinPanelBg(MMI_WIN_ID_T win_id);

//draw foreground base
LOCAL void DisplayWinPanelFgBase(MMI_WIN_ID_T win_id,
                                               MMI_IMAGE_ID_T image_id,
                                               GUI_RECT_T client_rect,
                                               MMI_TEXT_ID_T title_id,
                                               GUI_RECT_T title_rect);

LOCAL void Launcher_App_Start(GUI_POINT_T click_point, MMI_WIN_ID_T win_id);

PUBLIC void MMIZDT_DropUp_EnterWin(eSlideWinStartUpMode launch_mode);

/*****************************************************************/
//judge if point is contained by status bar rect
LOCAL BOOLEAN IsPressedStatusBarRect(GUI_POINT_T point)
{
    GUI_RECT_T status_bar_rect = LAUNCHER_NOTIFY_AREA;
    if (GUI_PointIsInRect(point, status_bar_rect))
    {
        return TRUE;
    }
    return FALSE;
}

LOCAL BOOLEAN IsPressedSoftkeyBarRect(GUI_POINT_T point)
{
    GUI_RECT_T softkey_rect = LAUNCHER_SOFTKEY_AREA;
    if (GUI_PointIsInRect(point, softkey_rect))
    {
        return TRUE;
    }
    return FALSE;
}
//diplay background
LOCAL void DisplayWinPanelBg(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T clientRect = LAUNCHER_CLIENT_RECT;
    GUI_RECT_T win_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, clientRect);
    GUIRES_DisplayImg(PNULL, &win_rect, PNULL, win_id, IMAGE_APP_COMMON_BG, &lcd_dev_info);
}


LOCAL void DisplayWinPanelFgAddUnreadInfoNum(MMI_WIN_ID_T win_id,uint8 unread_num)
{
    GUI_RECT_T         disp_rect           = {0};
    GUI_RECT_T         max_rect            = LAUNCHER_NOTIFY_UNREAD_MESSAGE_MAX_RECT;
    GUI_RECT_T         normal_rect         = LAUNCHER_NOTIFY_UNREAD_MESSAGE_RECT;
    GUI_LCD_DEV_INFO   lcd_dev_info        = {0};
    char               str_unread_num[LAUNCHER_NOTIFICATION_NUM_MAX_LEN + 1] = {0};
    uint8              str_unread_num_len  = 0;
    wchar              temp[LAUNCHER_NOTIFICATION_NUM_MAX_LEN + 1]           = {0};
    MMI_STRING_T       dis_num_str         = {0};
    MMI_IMAGE_ID_T     image_id            = NULL;
    if(unread_num == 0)
    {
         return;
    }
    else if(unread_num > LAUNCHER_NOTIFICATION_MAX_NUM)
    {
        unread_num = LAUNCHER_NOTIFICATION_MAX_NUM;
        disp_rect  = max_rect;
        image_id   = res_launcher_notification_2;
        sprintf(str_unread_num, "%d+", unread_num);
    }
    else
    {
        disp_rect = normal_rect;
        image_id  = res_launcher_notification_1;
        sprintf(str_unread_num, "%d", unread_num);
    }
    str_unread_num_len = strlen(str_unread_num);
    MMI_STRNTOWSTR(temp, LAUNCHER_NOTIFICATION_NUM_MAX_LEN + 1, (const uint8 *)str_unread_num , str_unread_num_len,  str_unread_num_len);
    dis_num_str.wstr_ptr = temp;
    dis_num_str.wstr_len = str_unread_num_len;
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);

    //Draw unread  num txt
    do
    {
        GUISTR_STYLE_T      text_style = {0};
        GUISTR_STATE_T      text_state = GUISTR_STATE_ALIGN;
        text_style.align      = ALIGN_HVMIDDLE;
        text_style.font       = WATCH_DEFAULT_SMALL_FONT;
        text_style.font_color = MMI_WHITE_COLOR;

        //转换窗口坐标到显示坐标
        disp_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, disp_rect);
        GUIRES_DisplayImg(PNULL, &disp_rect, PNULL, win_id, image_id, &lcd_dev_info);
        GUISTR_DrawTextToLCDInRect(
                          (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                          &disp_rect,
                          &disp_rect,
                          &dis_num_str,
                          &text_style,
                          text_state,
                          GUISTR_TEXT_DIR_AUTO
                          );
    }while (0);
}

//draw foreground
LOCAL void DisplayWinPanelFgBase(MMI_WIN_ID_T win_id,
                                               MMI_IMAGE_ID_T image_id,
                                               GUI_RECT_T client_rect,
                                               MMI_TEXT_ID_T title_id,
                                               GUI_RECT_T title_rect)
{
    uint16 img_width = 0;
    uint16 img_height = 0;
    GUI_RECT_T img_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};

    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    GUIRES_GetImgWidthHeight(&img_width, &img_height,  image_id, win_id);

    img_rect = GUI_GetCenterRect(client_rect, img_width, img_height);
    GUIRES_DisplayImg(PNULL,
                       &img_rect,
                       PNULL,
                       win_id,
                       image_id,
                       &lcd_dev_info
                       );

    //Draw title txt
    do
    {
        MMI_STRING_T widgetStr = {0};
        GUI_RECT_T disp_rect = LAUNCHER_TITLE_RECT;
        GUISTR_STYLE_T      text_style = {0};
        GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
        text_style.align    = ALIGN_HVMIDDLE;
        text_style.font     = DP_FONT_20;
        text_style.font_color= MMI_WHITE_COLOR;

        //转换窗口坐标到显示坐标
        disp_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, disp_rect);
        MMI_GetLabelTextByLang(title_id, &widgetStr);
        GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &disp_rect,
        &disp_rect,
        &widgetStr,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
        );
    }while (0);
}

LOCAL void CreateIndicatorLayer(MMI_WIN_ID_T win_id, uint32 width, uint32 height, GUI_LCD_DEV_INFO *lcd_dev_ptr)
{
#ifdef UI_MULTILAYER_SUPPORT
    UILAYER_CREATE_T create_info = {0};
    if (PNULL == lcd_dev_ptr)
    {
        return;
    }

    lcd_dev_ptr->lcd_id = GUI_MAIN_LCD_ID;
    lcd_dev_ptr->block_id = UILAYER_NULL_HANDLE;

    // 创建层
    create_info.lcd_id = GUI_MAIN_LCD_ID;
    create_info.owner_handle = win_id;
    create_info.offset_x = 0;
    create_info.offset_y = 0;
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


//process the common action of launcher
PUBLIC MMI_RESULT_E WatchLAUNCHER_HandleCommonWinMsg(
                                      MMI_WIN_ID_T        win_id,        //IN:
                                      MMI_MESSAGE_ID_E    msg_id,        //IN:
                                      DPARAM            param        //IN:
                                      )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    LOCAL BOOLEAN b_tp_pressed = FALSE;
    LOCAL BOOLEAN b_tp_moved = FALSE;
    LOCAL GUI_POINT_T tp_point = {0};
    LOCAL BOOLEAN b_drop_from_top = FALSE;
    LOCAL BOOLEAN b_drop_from_bottom = FALSE;
    MMI_CheckAllocatedMemInfo();
    TRACE_APP_LAUNCHER("win_id = 0x%0x, msg_id = 0x%0x.", win_id, msg_id);
    switch (msg_id)
    {
        case MSG_KEYLONG_HASH:
        {
            TRACE_APP_LAUNCHER("MSG_KEYLONG_HASH");
#ifdef PDA_UI_DROPDOWN_WIN
            //MMINotifyWin_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
            MMIZDT_DropDown_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
#endif
            break;
        }

#ifdef TOUCH_PANEL_SUPPORT
        case MSG_TP_PRESS_DOWN:
        {
            tp_point.x = MMK_GET_TP_X(param);
            tp_point.y = MMK_GET_TP_Y(param);
#ifndef DROPDOWN_NOTIFY_SHORTCUT
#ifdef PDA_UI_DROPDOWN_WIN
            if (IsPressedStatusBarRect(tp_point))
            {
                //StartUp DropWin
                TRACE_APP_LAUNCHER("IsPressedStatusBarRect() = true.");

                b_drop_from_top = TRUE;
                //MMINotifyWin_EnterWin(SLIDEWIN_STARTUPMODE_TP);
                //MMIZDT_DropDown_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
            }
            else if(IsPressedSoftkeyBarRect(tp_point))
            {
                b_drop_from_bottom = TRUE;
            }
#endif
#else
            if (IsPressedStatusBarRect(tp_point))
            {
                //StartUp DropWin
                TRACE_APP_LAUNCHER("IsPressedStatusBarRect() = true.");
#ifdef DYNAMIC_WATCHFACE_SUPPORT
                if(MMK_IsFocusWin(WATCHFACE_WIN_ID))
#else
                if(MMK_IsFocusWin(WATCH_LAUNCHER_CLOCK_WIN_ID))
#endif
                {
                    NotifyCenter_EnterShortcutWin();
                }
            }
            else if(IsPressedSoftkeyBarRect(tp_point))
 			{
#ifdef DYNAMIC_WATCHFACE_SUPPORT

                if(MMK_IsFocusWin(WATCHFACE_WIN_ID))
#else
                if(MMK_IsFocusWin(WATCH_LAUNCHER_CLOCK_WIN_ID))
#endif
                {
					NotifyCenter_EnterListWin();
                }
			}
#endif
            b_tp_pressed = TRUE;
            break;
        }

        case MSG_TP_PRESS_MOVE:
        {
            TRACE_APP_LAUNCHER("MSG_TP_PRESS_MOVE");
            b_tp_moved = TRUE;
            if(abs(MMK_GET_TP_Y(param) - tp_point.y) > 10)
            {
                if(b_drop_from_top)
                {
                    MMIZDT_DropDown_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
                    b_drop_from_top = FALSE;
                }
                else if(b_drop_from_bottom)
                {
				//#ifdef ZTE_WATCH
                    //MMIZDT_DropUp_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
				//#endif
                    b_drop_from_bottom = FALSE;
                }
            }
            break;
        }

        case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T tp_up = {0};
            BOOLEAN bClick = FALSE;
            TRACE_APP_LAUNCHER("MSG_TP_PRESS_UP");
            tp_up.x = MMK_GET_TP_X(param);
            tp_up.y = MMK_GET_TP_Y(param);
            bClick = (abs(tp_up.x - tp_point.x) < 3) && (abs(tp_up.y - tp_point.y) < 3);
            if (b_tp_pressed && bClick)
            {
                //post tp click msg to win.
				MMK_PostMsg(win_id, MSG_APP_WEB, &tp_up, sizeof(GUI_POINT_T));
            }
            b_tp_pressed = FALSE;
            b_tp_moved = FALSE;
            b_drop_from_top = FALSE;
            b_drop_from_bottom = FALSE;
            break;
        }
#endif
        case MSG_KEYDOWN_0:
            if(!MMIENG_IsMonkeyTestMode())
            {
                MMIAPIMENU_CreatMainMenu();
            }
            break;
        case MSG_NOTIFY_OK:
        case MSG_APP_OK:
        {
            SCI_TRACE_LOW("WatchCC_DialpadWin_Enter");
            //WatchCC_DialpadWin_Enter();
            break;
        }

        case MSG_KEYUP_DOWN:
            break;

        case MSG_NOTIFY_CANCEL:
        case MSG_APP_CANCEL:
            if(!MMIENG_IsMonkeyTestMode())
            {
               // MMIAPIENG_StartEngineerMenu();
            }
            break;
        case MSG_KEYDOWN_8:
            // TODO:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            SCI_TRACE_LOW("WatchLAUNCHER_HandleCommonWinMsg() msg_id = %d.", msg_id);
            //主页按power键 如果是亮屏那就灭屏
            if(WATCH_LAUNCHER_APP_PAGE_START_WIN_ID + 1 == win_id)
            {
                if(TRUE == MMIDEFAULT_IsBacklightOn())
                {
                    MMIDEFAULT_TurnOffBackLight();
                    MMIDEFAULT_CloseAllLight_Watch();//close LCD
                }
                else
                {
                    MMIDEFAULT_TurnOnBackLight();
                }
            }
            else
            {
                WatchSLIDEAGE_SetCurrentPageIndex(0);
            }
            break;
        case MSG_KEYDOWN_UPSIDE:
        case MSG_KEYDOWN_VOL_UP:
            MMIAPISET_RingToneVolumeWin();
            break;

        case MSG_KEYDOWN_DOWNSIDE:
        case MSG_KEYDOWN_VOL_DOWN:
            MMIAPISET_RingToneVolumeWin();
            break;
        default:
            recode = MMI_RESULT_FALSE;
    }
    MMI_CheckAllocatedMemInfo();
    TRACE_APP_LAUNCHER("exit");;
    return recode;
}

PUBLIC MMI_RESULT_E WatchLAUNCHER_HandleCb(
                                      MMI_WIN_ID_T        win_id,        //IN:
                                      MMI_MESSAGE_ID_E    msg_id,        //IN:
                                      DPARAM            param        //IN:
                                      )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id)
    {
        case MSG_SLIDEPAGE_OPENED:
        {
#ifndef ADULT_WATCH_SUPPORT
#ifndef SCREEN_SHAPE_CIRCULAR
            GUI_RECT_T rect = LAUNCHER_CLIENT_RECT;
            CreateIndicatorLayer(win_id, 240, 240, &s_indi_layer);
            if (!UILAYER_IsBltLayer(&s_indi_layer))
            {
                UILAYER_APPEND_BLT_T append;
                append.layer_level = UILAYER_LEVEL_NORMAL;
                append.lcd_dev_info = s_indi_layer;
                UILAYER_AppendBltLayer(&append);
            }
#endif
#endif
            is_page_slide_end = FALSE;
            break;
        }

        case MSG_SLIDEPAGE_GETFOCUS:
        {
#ifndef ADULT_WATCH_SUPPORT
#ifndef SCREEN_SHAPE_CIRCULAR

            if (!UILAYER_IsBltLayer(&s_indi_layer))
            {
                UILAYER_APPEND_BLT_T append;
                append.layer_level = UILAYER_LEVEL_NORMAL;
                append.lcd_dev_info = s_indi_layer;
                UILAYER_AppendBltLayer(&append);
            }
#endif
#endif
            break;
        }

        case MSG_SLIDEPAGE_LOSEFOCUS:
        {
#ifndef ADULT_WATCH_SUPPORT
#ifndef SCREEN_SHAPE_CIRCULAR
            UILAYER_RemoveBltLayer(&s_indi_layer);
#endif
#endif
            break;
        }

        case MSG_SLIDEPAGE_PAGECHANGED:
        {
            break;
        }
        case MSG_SLIDEPAGE_END:
        {
            is_page_slide_end = TRUE;
            MMK_SendMsg(win_id, MSG_SLIDEPAGE_END, PNULL);
            break;
        }
        default:
            break;
    }
    return recode;
}

LOCAL void DisplayLauncherIndicator(MMI_WIN_ID_T win_id)
{
    MMI_IMAGE_ID_T img_id = 0;
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    uint8 i = 0;
    GUI_POINT_T start_point = WATCH_LAUNCHER_INDICATOR_START_POINT;
    //(APP_MENU_SIZE+1) 第一页只有LAUNCHER_MENUS_IN_PAGE-1项
    uint8 page_size = (APP_MENU_SIZE+1)%LAUNCHER_MENUS_IN_PAGE == 0?APP_MENU_SIZE/LAUNCHER_MENUS_IN_PAGE:(APP_MENU_SIZE/LAUNCHER_MENUS_IN_PAGE+1);
    uint8 page_index = win_id - WATCH_LAUNCHER_APP_PAGE_START_WIN_ID - 1;
    uint16 launcher_indicator_img_width = 0;
    uint16 launcher_indicator_img_height = 0;
    GUIRES_GetImgWidthHeight(&launcher_indicator_img_width, &launcher_indicator_img_height, res_launcher_ic_point_off, win_id);
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    start_point.x = (MMI_MAINSCREEN_WIDTH - launcher_indicator_img_width*page_size - WATCH_LAUNCHER_INDICATOR_OFFSET*(page_size-1))/2;
    for (i = 0; i < page_size; i++)
    {
        img_id = res_launcher_ic_point_off;
        if (i == page_index)
        {
            img_id = res_launcher_ic_point_on;
        }
        start_point.x += WATCH_LAUNCHER_INDICATOR_OFFSET;
        GUIRES_DisplayImg(&start_point,PNULL, PNULL, win_id, img_id, &lcd_dev_info);
    }

}

PUBLIC BOOLEAN isPageSlideEnd()
{
    return is_page_slide_end;
}

PUBLIC void getLauncherContentRect(MMI_HANDLE_T win_id, GUI_RECT_T* win_rect)
{
    MMK_GetWinRect(win_id, win_rect);
    win_rect->top += (LAUNCHER_STATUSBAR_HEIGHT + LAUNCHER_VERTICAL_PADDING);
    win_rect->left += LAUNCHER_HORIZONTAL_PADDING;
    win_rect->right -= LAUNCHER_HORIZONTAL_PADDING;
    win_rect->bottom -= LAUNCHER_VERTICAL_PADDING;
}

//获取菜单之间水平方向的间距
LOCAL uint8 getHorizontalSpace(MMI_WIN_ID_T win_id, GUI_RECT_T win_rect)
{
    if(app_menu_bg_img_width == 0 || app_menu_bg_img_height == 0)
    {
        GUIRES_GetImgWidthHeight(&app_menu_bg_img_width, &app_menu_bg_img_height, bright_green_bg, win_id);
    }   
    return ((win_rect.right - win_rect.left) - app_menu_bg_img_width*LAUNCHER_MENU_COLUMNS)/(LAUNCHER_MENU_COLUMNS*2);
}

//获取菜单之间垂直方向的间距
LOCAL uint8 getVerticalSpace(MMI_WIN_ID_T win_id, GUI_RECT_T win_rect)
{
    if(app_menu_bg_img_width == 0 || app_menu_bg_img_height == 0)
    {
        GUIRES_GetImgWidthHeight(&app_menu_bg_img_width, &app_menu_bg_img_height, bright_green_bg, win_id);
    }
    return ((win_rect.bottom - win_rect.top) - app_menu_bg_img_height*LAUNCHER_MENU_ROWS)/(LAUNCHER_MENU_ROWS*2);
}

LOCAL void Display_DateTime(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    SCI_TIME_T  time   = {0};
    MMI_STRING_T time_str = {0};
    uint8 date_str[12] = {0};
    GUISTR_STATE_T txt_state = GUISTR_STATE_ALIGN;
    GUISTR_STYLE_T txt_style = {0};
    wchar dateTime[15] = {0};
    GUI_RECT_T dateTime_rect = {0, 0, MMI_MAINSCREEN_WIDTH,LAUNCHER_STATUSBAR_HEIGHT};
    TM_GetSysTime(&time);
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    MMIAPISET_FormatTimeStrByTime(time.hour, time.min, date_str,12);
    MMI_STRNTOWSTR(dateTime,14,(const uint8*)date_str,(uint32)strlen((char*)date_str),(uint32)strlen((char*)date_str));
      
    txt_style.align = ALIGN_HVMIDDLE;
    txt_style.font_color = MMI_WHITE_COLOR;
    txt_style.font = DP_FONT_24;
    time_str.wstr_ptr = dateTime;
    time_str.wstr_len = MMIAPICOM_Wstrlen(dateTime);
    GUISTR_DrawTextToLCDInRect((const GUI_LCD_DEV_INFO *)&lcd_dev_info, &dateTime_rect, &dateTime_rect, &time_str, &txt_style, txt_state, GUISTR_TEXT_DIR_AUTO);
}

PUBLIC void DisplayLauncherStatusBar(MMI_WIN_ID_T win_id)
{
    //只在第一页显示
    if(WATCH_LAUNCHER_APP_PAGE1_WIN_ID == win_id)
    {
        ZDT_DisplaySingal(win_id);
        ZDT_DisplayBattery(win_id);
        Display_DateTime(win_id);
    }
}

LOCAL void startUpdateDateTimeTimer(MMI_WIN_ID_T win_id)
{
    if (0 == s_clock_update_timer && win_id == WATCH_LAUNCHER_APP_PAGE1_WIN_ID)
    {
        s_clock_update_timer = MMK_CreateWinTimer(WATCH_LAUNCHER_APP_PAGE1_WIN_ID,WATCH_LAUNCHER_CLOCK_UPDATE_INTERVAL,TRUE);
    }    
}

LOCAL void stopUpdateDateTimeTimer(MMI_WIN_ID_T win_id)
{
    if (s_clock_update_timer != 0)
    {
        MMK_StopTimer(s_clock_update_timer);
        s_clock_update_timer = 0;
    }
}

LOCAL void updateDateTimeTimerCallback(MMI_WIN_ID_T win_id)
{
    if(MMK_GetFocusWinId() != WATCH_LAUNCHER_APP_PAGE1_WIN_ID)
    {
        stopUpdateDateTimeTimer(win_id);            
    }
    else
    {
        MMK_SendMsg(win_id, MSG_FULL_PAINT,PNULL);
    }
}

/*
* menu_index 菜单索引 即g_app_list_info数组索引号
* page_index 菜单在page 里面索引 比如4宫格菜单从0-3 
*/
LOCAL void DisplayAppItem(MMI_WIN_ID_T win_id, uint8 menu_index, uint8 page_index, GUI_LCD_DEV_INFO lcd_dev_info)
{
    uint8 horizontal_space = 0;
    uint8 vertical_space = 0;
    GUI_POINT_T point = {0};
    GUI_RECT_T win_rect = {0};
    getLauncherContentRect(win_id, &win_rect);
    horizontal_space = getHorizontalSpace(win_id,win_rect);
    vertical_space = getVerticalSpace(win_id,win_rect); 
    point.x = win_rect.left + horizontal_space + (2*horizontal_space + app_menu_bg_img_width)*(page_index%LAUNCHER_MENU_COLUMNS);
    point.y = win_rect.top + vertical_space + (2*vertical_space + app_menu_bg_img_height)*(page_index/LAUNCHER_MENU_COLUMNS);
    //画背景
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,g_app_list_info[menu_index].bg_img_id,&lcd_dev_info);
    if(menu_index == 0 && page_index == 0)//同步课堂
    {
        //画title
        point.x = point.x + TEXTBOOK_MENU_TEXT_HORIZONTAL_PADDING;
        point.y = point.y + TEXTBOOK_MENU_TEXT_VERTICAL_PADDING;
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,g_app_list_info[menu_index].text_id,&lcd_dev_info);
    }
    else
    {
        //画小图标
        point.x += MENU_ICON_HORIZONTAL_PADDING;
        point.y += MENU_ICON_VERTICAL_PADDING;
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,g_app_list_info[menu_index].img_id,&lcd_dev_info);
        //画title
        point.x = point.x - MENU_ICON_HORIZONTAL_PADDING + MENU_TEXT_HORIZONTAL_PADDING;
        point.y = point.y - MENU_ICON_VERTICAL_PADDING + MENU_TEXT_VERTICAL_PADDING;
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,g_app_list_info[menu_index].text_id,&lcd_dev_info);
    }
}

LOCAL void DisplayLauncherPage(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    uint page_index = 0;
    uint8 menu_index = 0;
    uint8 horizontal_space = 0;
    uint8 vertical_space = 0;
    GUI_POINT_T point = {0};
    GUI_RECT_T win_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUISTR_STYLE_T text_style = {0};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_EFFECT|GUISTR_STATE_ELLIPSIS;
    GUI_RECT_T text_rect = {0};
    page_index = win_id - WATCH_LAUNCHER_APP_PAGE_START_WIN_ID - 1;
    if(page_index >= 0)
    {
        for(; i < LAUNCHER_MENUS_IN_PAGE; i++)
        {
            
            menu_index = (page_index*LAUNCHER_MENUS_IN_PAGE) + i;
            if(menu_index < APP_MENU_SIZE+1)
            {
                //第一页只有LAUNCHER_MENUS_IN_PAGE-1个菜单
                if(page_index == 0)
                {
                    if(menu_index == 0)
                    {
                        menu_index = menu_index + 1;
                    }
                    else if(menu_index == 1)
                    {
                        continue;
                    }
                }
                menu_index = menu_index - 1;
                DisplayAppItem(win_id, menu_index,i,lcd_dev_info);
            }
        }
    }
}

LOCAL MMI_RESULT_E HandleLauncherPageWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMI_CheckAllocatedMemInfo();
    switch (msg_id)
    {
        case MSG_GET_FOCUS:
           // MMK_PostMsg(win_id,MSG_FULL_PAINT,PNULL,PNULL);
            startUpdateDateTimeTimer(win_id);
            break;
        case MSG_FULL_PAINT:
        {
            DisplayWinPanelBg(win_id);
            DisplayLauncherPage(win_id);
            DisplayLauncherStatusBar(win_id);
            //DisplayLauncherIndicator(win_id);
            break;
        }

        case MSG_APP_WEB:
        {
            GUI_POINT_T *point = (GUI_POINT_T*)param;
            if(point != PNULL)
            {
                Launcher_App_Start(*point,win_id);
            }
            break;
        }
        case MSG_LOSE_FOCUS:
            stopUpdateDateTimeTimer(win_id);
            break;

        case MSG_TIMER:
            if (NULL != param)
            {           
                if (s_clock_update_timer == *(uint8*)param)
                {
                    updateDateTimeTimerCallback(win_id);
                }
            }
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    MMI_CheckAllocatedMemInfo();
    if (!recode)
    {
        recode = WatchLAUNCHER_HandleCommonWinMsg(win_id, msg_id, param);
    }
    return recode;
}

LOCAL void WatchLauncher_CreateWin(const MMI_WIN_ID_T win_id)
{
    uint32 win_table_create[10] = 
    {
        MMK_HIDE_STATUSBAR,
        MMK_WINFUNC, HandleLauncherPageWinMsg,
        MMK_WINID, 0,
        MMK_WINDOW_STYLE, (WS_DISPATCH_TO_CHILDWIN |WS_DISABLE_RETURN_WIN),
        MMK_WINDOW_ANIM_MOVE_SYTLE, (MOVE_FORBIDDEN),
        MMK_END_WIN,
    };
    win_table_create[4] = win_id;
    MMK_CreateWin(win_table_create, NULL);
}

PUBLIC MMI_RESULT_E WatchLAUNCHER_FourAppHandleCommonWinMsg(
                                      MMI_WIN_ID_T        win_id,        //IN:
                                      MMI_MESSAGE_ID_E    msg_id,        //IN:
                                      DPARAM            param        //IN:
                                      )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    LOCAL BOOLEAN b_tp_pressed = FALSE;
    LOCAL BOOLEAN b_tp_moved = FALSE;
    LOCAL GUI_POINT_T tp_point = {0};
    MMI_CheckAllocatedMemInfo();
    TRACE_APP_LAUNCHER("win_id = 0x%0x, msg_id = 0x%0x.", win_id, msg_id);
    switch (msg_id)
    {
        case MSG_KEYLONG_HASH:
        {
            TRACE_APP_LAUNCHER("MSG_KEYLONG_HASH");
#ifdef PDA_UI_DROPDOWN_WIN
            MMINotifyWin_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
            //MMIZDT_DropDown_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
#endif
            break;
        }

#ifdef TOUCH_PANEL_SUPPORT
        case MSG_TP_PRESS_DOWN:
        {
            tp_point.x = MMK_GET_TP_X(param);
            tp_point.y = MMK_GET_TP_Y(param);
#ifndef DROPDOWN_NOTIFY_SHORTCUT
#ifdef PDA_UI_DROPDOWN_WIN
            if (IsPressedStatusBarRect(tp_point))
            {
                //StartUp DropWin
                TRACE_APP_LAUNCHER("IsPressedStatusBarRect() = true.");

                MMINotifyWin_EnterWin(SLIDEWIN_STARTUPMODE_TP);
                //MMIZDT_DropDown_EnterWin(SLIDEWIN_STARTUPMODE_AUTO);
            }
#endif
#else
            if (IsPressedStatusBarRect(tp_point))
            {
                //StartUp DropWin
                TRACE_APP_LAUNCHER("IsPressedStatusBarRect() = true.");
#ifdef DYNAMIC_WATCHFACE_SUPPORT
                if(MMK_IsFocusWin(WATCHFACE_WIN_ID))
#else
                if(MMK_IsFocusWin(WATCH_LAUNCHER_CLOCK_WIN_ID))
#endif
                {
                    NotifyCenter_EnterShortcutWin();
                }
            }
            else if(IsPressedSoftkeyBarRect(tp_point))
 			{
#ifdef DYNAMIC_WATCHFACE_SUPPORT

                if(MMK_IsFocusWin(WATCHFACE_WIN_ID))
#else
                if(MMK_IsFocusWin(WATCH_LAUNCHER_CLOCK_WIN_ID))
#endif
                {
					NotifyCenter_EnterListWin();
                }
			}
#endif
            b_tp_pressed = TRUE;
            break;
        }

        case MSG_TP_PRESS_MOVE:
        {
            TRACE_APP_LAUNCHER("MSG_TP_PRESS_MOVE");
            b_tp_moved = TRUE;
            break;
        }

        case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T tp_up = {0};
            BOOLEAN bClick = FALSE;
            TRACE_APP_LAUNCHER("MSG_TP_PRESS_UP");
            tp_up.x = MMK_GET_TP_X(param);
            tp_up.y = MMK_GET_TP_Y(param);
            bClick = (abs(tp_up.x - tp_point.x) < 3) && (abs(tp_up.y - tp_point.y) < 3);
            if (b_tp_pressed && bClick)
            {
                //post tp click msg to win.
                MMK_PostMsg(win_id, MSG_APP_WEB, &tp_up, sizeof(GUI_POINT_T));
            }
            b_tp_pressed = FALSE;
            b_tp_moved = FALSE;
            break;
        }
#endif
        case MSG_KEYDOWN_0:
            if(!MMIENG_IsMonkeyTestMode())
            {
                MMIAPIMENU_CreatMainMenu();
            }
            break;
        case MSG_NOTIFY_OK:
        case MSG_APP_OK:
        {
            SCI_TRACE_LOW("WatchCC_DialpadWin_Enter");
            WatchCC_DialpadWin_Enter();
            break;
        }

        case MSG_NOTIFY_CANCEL:
        case MSG_APP_CANCEL:
            if(!MMIENG_IsMonkeyTestMode())
            {
                MMIAPIENG_StartEngineerMenu();
            }
            break;
        case MSG_KEYDOWN_8:
            // TODO:
#ifdef PRODUCT_CONFIG_UWS6131_XTC_I23
        case MSG_APP_RED:
            SCI_TRACE_LOW("WatchLAUNCHER_HandleCommonWinMsg() msg_id = %d.", msg_id);
#endif
            if(TRUE == MMIDEFAULT_IsBacklightOn())
            {
                MMIDEFAULT_TurnOffBackLight();
                MMIDEFAULT_CloseAllLight_Watch();//close LCD
            }else
            {
                MMIDEFAULT_TurnOnBackLight();
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
    }
    MMI_CheckAllocatedMemInfo();
    TRACE_APP_LAUNCHER("exit");
    return recode;
}

LOCAL void Launcher_App_Start(GUI_POINT_T click_point, MMI_WIN_ID_T win_id)
{
    GUI_RECT_T rect = {0};
    uint page_index = 0;
    uint8 i = 0;
    uint8 horizontal_space = 0;
    uint8 vertical_space = 0;
    GUI_RECT_T win_rect = {0};
    uint8 menu_index = 0;
    getLauncherContentRect(win_id, &win_rect);
    horizontal_space = getHorizontalSpace(win_id,win_rect);
    vertical_space = getVerticalSpace(win_id,win_rect); 
    page_index = win_id - WATCH_LAUNCHER_APP_PAGE_START_WIN_ID - 1;
    if(page_index >=0)
    {
        for(;i<LAUNCHER_MENUS_IN_PAGE;i++)
        {
            //第一页只有LAUNCHER_MENUS_IN_PAGE-1个菜单
            menu_index = page_index*LAUNCHER_MENUS_IN_PAGE + i - 1;
            rect.left = win_rect.left + horizontal_space + (2*horizontal_space + app_menu_bg_img_width)*(i%LAUNCHER_MENU_COLUMNS);
            rect.top = win_rect.top + vertical_space  + (2*vertical_space + app_menu_bg_img_height)*(i/LAUNCHER_MENU_COLUMNS);
            if(page_index == 0 && i == 0)
            {
                rect.right = win_rect.right - LAUNCHER_HORIZONTAL_PADDING;
                rect.bottom = rect.top + app_menu_bg_img_height;
                menu_index = i;
            }
            else
            {
                rect.right = rect.left + app_menu_bg_img_width;
                rect.bottom = rect.top + app_menu_bg_img_height;
            }
            if( menu_index >= APP_MENU_SIZE || g_app_list_info[menu_index].start_handle == NULL)
            {
                TRACE_APP_LAUNCHER("there is not app rect");
                return;
            }
            if(GUI_PointIsInRect(click_point,rect))
            {
                if(g_app_list_info[menu_index].check_sim == 1 && MMIAPIPHONE_GetSimStatus(MN_DUAL_SYS_1) != SIM_STATUS_OK)
                {
                    MMIZDT_OpenNoSimOrDataWin();
                }
                else
                {
                    g_app_list_info[menu_index].start_handle();
                }
                return;
            }	
        }
    }
}

PUBLIC MMI_RESULT_E WatchLAUNCHER_FourApp_HandleCb(
                                      MMI_WIN_ID_T        win_id,        //IN:
                                      MMI_MESSAGE_ID_E    msg_id,        //IN:
                                      DPARAM            param        //IN:
                                      )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    TRACE_APP_LAUNCHER("%s, win_id = 0x%0x, msg_id = 0x%0x.", __FUNCTION__,win_id, msg_id);
    switch (msg_id)
    {
        case MSG_SLIDEPAGE_OPENED:
        {
            break;
        }

        case MSG_SLIDEPAGE_GETFOCUS:
        {
            break;
        }

        case MSG_SLIDEPAGE_LOSEFOCUS:
        {
            break;
        }

        case MSG_SLIDEPAGE_PAGECHANGED:
        {
            break;
        }
        case MSG_SLIDEPAGE_END:
        {
            MMK_SendMsg(win_id, MSG_SLIDEPAGE_END, PNULL);
            break;
        }
        default:
            break;
    }
    return recode;
}

LOCAL void Launcher_Enter_page()
{
    tWatchSlidePageItem elem[LAUNCHER_ELEM_COUNT] = {0};
    MMI_HANDLE_T handle;
    uint8 i = 0;
    //(APP_MENU_SIZE+1) 第一页只有LAUNCHER_MENUS_IN_PAGE-1项
    uint8 page_size = (APP_MENU_SIZE+1)%LAUNCHER_MENUS_IN_PAGE == 0?(APP_MENU_SIZE+1)/LAUNCHER_MENUS_IN_PAGE:((APP_MENU_SIZE+1)/LAUNCHER_MENUS_IN_PAGE+1);
    if(s_handle != NULL)
    {
        WatchSLIDEPAGE_DestoryHandle(s_handle);
        s_handle = NULL;
    }
    handle = WatchSLIDEPAGE_CreateHandle();

    for(i;i<page_size;i++)
    {
        elem[i].fun_enter_win = WatchLauncher_CreateWin;
        if(WATCH_LAUNCHER_APP_PAGE_START_WIN_ID+i >= WATCH_LAUNCHER_APP_PAGE_MAX_WIN_ID)
        {
            break;
        }
        elem[i].win_id = WATCH_LAUNCHER_APP_PAGE_START_WIN_ID+i+1;
    }

    WatchSLIDEPAGE_Open(handle, elem, i, 0, TRUE, WatchLAUNCHER_FourApp_HandleCb);
    s_handle = handle;
}



/*****************************************************************************/
//  Discription: open watch launcher window
//  Global resource dependence:
//  Author: zhikun.lv
//  Note:
/*****************************************************************************/
PUBLIC void WatchLAUNCHER_Enter(void)
{
#ifndef RELEASE_INFO
    MMIENG_SetUSBLog(TRUE);  // wuxx for test
    SCI_SetArmLogFlag(TRUE); // wuxx for test
#endif
    if(SIM_STATUS_NO_SIM == MMIAPIPHONE_GetSimStatus(MN_DUAL_SYS_1))
    {
        MMIZDT_OpenInsertSimWin();
    }
#ifdef SETUP_WIZARD_SUPPORT
    else if(MMIZDT_Get_Setup_Wizard_Status() == 0)
    {
        Setup_Wizard_MainWin_Enter();
    }
#endif
    else
    {
        WatchOpen_IdleWin();
    }
    MMIZDT_CheckOpenClassModeWin();
}

PUBLIC void WatchOpen_IdleWin(void)
{
    SCI_TRACE_LOW("[%s]: enter", __FUNCTION__);
    Launcher_Enter_page();
#ifdef SETUP_WIZARD_SUPPORT
    //MMIZDT_NV_SetFirstPWON(0);
#endif
}

PUBLIC BOOLEAN WatchLAUNCHER_IsFoucsIdleWin(MMI_WIN_ID_T win_id)
{
    BOOLEAN is_focus = FALSE;
    uint8 i = 0;
    uint8 page_size = (APP_MENU_SIZE+1)%LAUNCHER_MENUS_IN_PAGE == 0?APP_MENU_SIZE/LAUNCHER_MENUS_IN_PAGE:(APP_MENU_SIZE/LAUNCHER_MENUS_IN_PAGE+1);
    for(i = 0; i < page_size;i++)
    {
        if(WATCH_LAUNCHER_APP_PAGE_START_WIN_ID + i + 1 == win_id)
        {
            is_focus = TRUE;
            break;
        }
    }
    return is_focus;
}

/*****************************************************************************/
//  Discription: set charge page visible
//  Global resource dependence:
//  Author: zhikun.lv
//  Note:
/*****************************************************************************/
PUBLIC void WatchLAUNCHER_SetChargePageVisible(BOOLEAN bVisible)
{
    if (bVisible)
    {
        WatchSLIDEAGE_SetPageEnable(WATCH_LAUNCHER_CHARGE_WIN_ID, TRUE);
    }
    else
    {
        WatchSLIDEAGE_SetPageEnable(WATCH_LAUNCHER_CHARGE_WIN_ID, FALSE);
    }
}

PUBLIC void MMIZDT_DropUp_EnterWin(eSlideWinStartUpMode launch_mode)
{
    GUI_RECT_T roiRect = MMIDROPUPWIN_ROIREGION;
    MMI_WIN_ID_T win_id = MMIZDT_VCHAT_GROUP_WIN_ID;
    MMI_WIN_ID_T old_id = MMK_GetFocusWinId();
    if (MMK_IsOpenWin(win_id) && MMK_GetFocusWinId() == win_id)
    {
        return;
    }
    MMIZDT_OpenChatContactListWin();
    do
    {
        uint32 handle = 0;
        tSlideWinInitData init = {0};
        init.win_id = win_id;
        init.win_before_id = old_id;
        init.direction = SLIDEWIN_DIRECTION_VERT;
        init.mode = launch_mode;
        init.style = SLIDEWIN_STYLE_ENABLE_ROI | SLIDEWIN_STYLE_ENABLE_RANGE| SLIDEWIN_STYLE_ENABLE_FREE_WHEN_WINCLOSE;
        init.init_pos = MMIDROPUPWIN_INIT_POS;
        init.stop_pos = MMIDROPUPWIN_STOP_POS;
        init.close_pos = MMIDROPUPWIN_END_POS;
        init.range_min = MMIDROPUPWIN_INIT_POS;
        init.range_max = 0;
        init.roi_region = roiRect;
        handle = WatchSLIDEWIN_Create(&init);

        WatchSLIDEWIN_Start(handle);
    }while (0);
}

PUBLIC void WatchLAUNCHER_ZTE_LowBatterSwitch()
{

}

PUBLIC void WatchLAUNCHER_ZTE_LowBatterToIdle()
{

}

#ifndef WATCH_PANEL
PUBLIC void WatchOpen_Panel_SelectWin()
{

}
#endif

PUBLIC void ZMTApp_CloseRecordAndPlayer(void)
{
#ifdef LISTENING_PRATICE_SUPPORT
    ZMTListening_CloseListeningPlayer();
#endif
#ifdef WORD_CARD_SUPPORT
    ZMTWord_CloseWordPlayer();
#endif
#ifdef HANZI_CARD_SUPPORT
    ZMTHanzi_CloseHanziPlayer();
#endif
#ifdef POETRY_LISTEN_SUPPORT
    ZMTPoetry_ClosePoetryPlayer();
#endif
#ifdef ZMT_GPT_SUPPORT
    ZMTGpt_CloseKouyuRecordAndPlayer();
    ZMTGpt_CloseZuoWenRecord();
#endif
#ifdef FORMULA_SUPPORT
    ZMTFormula_CloseFormulaPlayer();
#endif
#ifdef ZMT_CLASS_SUPPORT
    ZMTClass_CloseClassPlayer();
#endif
#ifdef ZMT_PINYIN_SUPPORT
    ZMTPinyin_ClosePlayerHandle();
#endif
#ifdef ZMT_YINBIAO_SUPPORT
    ZMTYinbiao_ClosePlayerHandle();
#endif
}


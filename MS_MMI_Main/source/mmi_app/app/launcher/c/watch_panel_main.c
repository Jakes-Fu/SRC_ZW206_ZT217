
//#include "mmiwatchidle.h"
#include "mmidisplay_data.h"

#include "Os_api.h"
#include "mmiset_display.h"
#include "launcher_id.h"
#include "mmiudisk_export.h"
#include "mmiset_export.h"
#include "watch_launcher_main.h"
#include "watch_slidepage.h"
#include "graphics_draw.h"
#include "ctrlanim_export.h"
#include "../../../mmi_ctrl/source/anim/h/ctrlanim.h"
#include "zdt_app.h"
#include "zdt_win_export.h"
#if ZMT_DIAL_STORE_SUPPORT
#include "zmt_dial.h"
#include "zmt_dial_image.h"
#include "zmt_dial_text.h"
#endif

#define MAXPAGENUM 1// xiongkai only two analog clock 3
#define doubThres 0.00001

#ifdef WIN32
#define Trace_Log_ZDT  SCI_TRACE_LOW
#else
#define Trace_Log_ZDT(_format,...) \
    do \
    { \
        SCI_TraceLow("{%s}[%s]<%d> "_format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }while(0)
#endif 

GUI_LCD_DEV_INFO s_layer[MAXPAGENUM] = {0};

#define m_abs(x) ((x) > 0 ? (x) :(-(x)))

#define ALGO_PEDOMETER_GET_CALORIE(step,height,weight) (4.5*((step)*(height)/ 800)*(weight)/ 1800)
#define ALGO_PEDOMETER_GET_DISTANCE(step,height) ((step)*(height)/300)

#include "zdt_yx_net.h"
#ifdef ZTE_WATCH
extern YX_WEATHER_INFO_T zte_wt_info[4];
extern BOOLEAN Is_ZTE_WeatherGetFromServer();
#else
extern YX_WEATHER_INFO_T g_yx_wt_info;
extern BOOLEAN Is_WeatherGetFromServer();
#endif

#ifndef ARGB_GET_A
#define ARGB_GET_A(_argb) (((_argb) >> 24) & 0xff)
#endif
#ifndef ARGB_GET_R
#define ARGB_GET_R(_argb) (((_argb) >> 16) & 0xff)
#endif
#ifndef ARGB_GET_G
#define ARGB_GET_G(_argb) (((_argb) >> 8) & 0xff)
#endif
#ifndef ARGB_GET_B
#define ARGB_GET_B(_argb) ((_argb) & 0xff)
#endif

#ifndef RGB888_TO_RGB565
#define RGB888_TO_RGB565(r, g, b)  (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | ((b >> 3) & 0x1f))
#endif

#ifndef BLEND_MASK
#define BLEND_MASK 0x7E0F81F
#endif

typedef struct 
{
    void   *data_ptr;
    uint32  data_type;  //0: RGB565, 1: ARGB888
    uint16  width;
    uint16  height;
    int16   start_x;
    int16   start_y;
}MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T; /*lint !e770*/

//the watch page number
LOCAL int s_clock_timer_id = 0;
BOOLEAN bScroll = FALSE;
static void * s_static_z_mem_ptr = PNULL;

#define CLOCK_UPDATE_PERIOD 1000

#define SCREEN_WIDTH 240//368
#define SCREEN_HEIGHT 240//448

LOCAL GUI_LCD_DEV_INFO  s_watch2_panel_layer_handle = {0,UILAYER_NULL_HANDLE};

LOCAL void Watch_Panel_Select_Enter();
LOCAL void updateClock(PANEL_INDEX_E index);
LOCAL void setDefaultPanel();
LOCAL MMI_RESULT_E HandlePanelSelectWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id,DPARAM param);
LOCAL  uint16 BlendRGB565(uint16 bk_color, uint16 fore_color, uint8 alpha);
LOCAL void TimeTeacherClock(MMI_WIN_ID_T  win_id,
                                GUI_RECT_T dispRect ,
                                SCI_TIME_T sys_time , 
                                MMI_IMAGE_ID_T clock_bg,
                                MMI_IMAGE_ID_T clock_hour,
                                MMI_IMAGE_ID_T clock_min,
                                MMI_IMAGE_ID_T clock_sec,
                                MMI_IMAGE_ID_T clock_center
                                );


#ifdef ZTE_WATCH
LOCAL void ZTE_Draw_Spaceman_Panel(MMI_WIN_ID_T win_id);
LOCAL void ZTE_Draw_Rocket_Panel(MMI_WIN_ID_T win_id);
LOCAL void ZTE_Draw_Cat_Panel(MMI_WIN_ID_T win_id);
LOCAL void Panel_TP_UP(DPARAM  param);
#endif

#ifdef WIN32
LOCAL void ZDT_InitWeather_for_win32()
{
    uint16 shenzhen_city[3] = {0x6DF1, 0x5733 , 0};//shenzhen
    //uint16 shenzhen_city[3] = {0x4178, 0x5958 ,0};//shenzhen
#ifdef ZTE_WATCH
    SCI_MEMCPY(zte_wt_info[0].city_name, shenzhen_city, 3*sizeof(uint16));
    zte_wt_info[0].min_degree = 20;
    zte_wt_info[0].max_degree = 30;
    zte_wt_info[0].cur_degree = -12;
    zte_wt_info[0].num = 4;
#else
    SCI_MEMCPY(g_yx_wt_info.city_name, shenzhen_city, 3*sizeof(uint16));
    g_yx_wt_info.min_degree = 20;
    g_yx_wt_info.max_degree = 30;
    g_yx_wt_info.cur_degree = -12;
    g_yx_wt_info.num = 4;  
#endif
}
#endif

LOCAL void CreateWatchPanelPaintLayer(
                              MMI_WIN_ID_T      win_id
                              )
{
    uint16   layer_width = 0;
    uint16   layer_height = 0;
    UILAYER_CREATE_T    create_info = {0};
    UILAYER_APPEND_BLT_T        append_layer = {0};

    if (UILAYER_HANDLE_MULTI != UILAYER_GetHandleType(&s_watch2_panel_layer_handle))    
    {
        //get tips layer width height
        GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&layer_width,&layer_height);    

        //creat layer
        create_info.lcd_id = GUI_MAIN_LCD_ID;
        create_info.owner_handle = win_id;
        create_info.offset_x = 0;
        create_info.offset_y = 0;
        create_info.width = layer_width;
        create_info.height = layer_height;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE; 
        UILAYER_CreateLayer(&create_info, &s_watch2_panel_layer_handle);
      
        append_layer.lcd_dev_info = s_watch2_panel_layer_handle;
        append_layer.layer_level = UILAYER_LEVEL_HIGH;
        UILAYER_AppendBltLayer(&append_layer);
    }

    if (UILAYER_IsLayerActive(&s_watch2_panel_layer_handle))
    {
        UILAYER_Clear(&s_watch2_panel_layer_handle);
    }

}

LOCAL void ReleaseWatchPanelPaintLayer()
{

    if (UILAYER_HANDLE_MULTI == UILAYER_GetHandleType(&s_watch2_panel_layer_handle))    
    {
        UILAYER_ReleaseLayer(&s_watch2_panel_layer_handle);
    }
    s_watch2_panel_layer_handle.block_id = 0;
    s_watch2_panel_layer_handle.lcd_id = UILAYER_NULL_HANDLE;
}

LOCAL void MMIWatchIdle_StartClockTimer(int usec)
{
    if (0 == s_clock_timer_id)
    {
        s_clock_timer_id = MMK_CreateWinTimer(WATCH_LAUNCHER_PANEL_WIN_ID,usec,TRUE);
    }    
}

LOCAL void MMIWatchIdle_StopClockTimer(void)
{
    BOOLEAN result = FALSE;

    if (0 < s_clock_timer_id)
    {
        MMK_StopTimer(s_clock_timer_id);
        s_clock_timer_id = 0;

        result = TRUE;
    }

    return;
}


BOOLEAN ProcessAlphaBlending(MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *bk_img_ptr, 
MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *fore_img_ptr)
{
    uint32 fore_width  = fore_img_ptr->width;
    uint32 fore_height = fore_img_ptr->height;
    uint32 bk_width = bk_img_ptr->width;
    uint32 bk_height = bk_img_ptr->height;
    int32 start_x = fore_img_ptr->start_x;
    int32 start_y = fore_img_ptr->start_y;
    uint32 blend_width = fore_width;
    uint32 blend_height = fore_height;
    uint32 *fore_ptr = (uint32 *)fore_img_ptr->data_ptr;
    GUI_RECT_T dst_rect = {0};
    GUI_RECT_T bk_rect = {0};
    GUI_RECT_T for_rect = {0};  
    
    bk_rect.left  = bk_img_ptr->start_x;
    bk_rect.top   = bk_img_ptr->start_y;
    bk_rect.right = bk_img_ptr->start_x + bk_img_ptr->width;
    bk_rect.bottom  = bk_img_ptr->start_y + bk_img_ptr->height;    

    for_rect.left  = fore_img_ptr->start_x;
    for_rect.top   = fore_img_ptr->start_y;
    for_rect.right = fore_img_ptr->start_x + fore_img_ptr->width;
    for_rect.bottom  = fore_img_ptr->start_y + fore_img_ptr->height;    

    if (!GUI_IntersectRect(&dst_rect, bk_rect, for_rect))
    {
        return FALSE;
    }

    if (start_y < 0)
    {
        start_y = -start_y;
        blend_height -= start_y;        /*lint !e737*/
        fore_ptr += (start_y * fore_width);     /*lint !e737*/
        start_y = 0;
    }

    if (start_x < 0)
    {
        start_x = -start_x;
        blend_width -= start_x;     /*lint !e737*/
        fore_ptr += start_x;
        start_x = 0;
    }

    blend_width = (start_x + blend_width) > bk_width        /*lint !e737*/
                    ? (bk_width - start_x) : blend_width;       /*lint !e737*/
    blend_height = (start_y + blend_height) > bk_height         /*lint !e737*/
                    ? (bk_height - start_y) : blend_height;     /*lint !e737*/

    if (/*IMGREF_FORMAT_RGB565*/DATA_TYPE_RGB565 == bk_img_ptr->data_type)     //output RGB565
    {        
        uint32 i = 0, j = 0;
        uint16 *bk_ptr = (uint16 *)bk_img_ptr->data_ptr;

        bk_ptr += (start_y * bk_width + start_x);       /*lint !e737*/

        for (i=0; i<blend_height; i++)
        {
            for (j=0; j<blend_width; j++)
            {
                uint32 alpha, r, g, b;
                uint32 fore_value = *(fore_ptr + j);    
                uint16 bk_value = *(bk_ptr + j);

                alpha = ARGB_GET_A(fore_value);
                r = ARGB_GET_R(fore_value);
                g = ARGB_GET_G(fore_value);
                b = ARGB_GET_B(fore_value);

                fore_value = RGB888_TO_RGB565(r, g, b);
                bk_value = BlendRGB565(bk_value, (uint16)fore_value, alpha);
                *(bk_ptr + j) = bk_value;
            }

            bk_ptr += bk_width;
            fore_ptr += fore_width;
        }
    } 
    else if (DATA_TYPE_ARGB888 == bk_img_ptr->data_type)
    {
        uint32 i = 0, j = 0;
        uint32 *bk_ptr = (uint32 *)bk_img_ptr->data_ptr;
        bk_ptr += (start_y * bk_width + start_x); 
        for (i=0; i<blend_height; i++)
        {
            memcpy(bk_ptr, fore_ptr, blend_width * 4);
            bk_ptr += bk_width;
            fore_ptr += blend_width;
        }
    }

    return TRUE;
}

//==================================宇宙飞船表盘==========================
#ifdef WATCH_PANEL_SPACE_SHIP_SUPPORT //宇宙飞船数字表盘
LOCAL void drawDigitalWatch0()
{
    MMI_IDLE_DISPLAY_T          idle_disp_style     = {0};
    GUI_POINT_T                 point               = {0};
    SCI_TIME_T                  time                = {0};
    SCI_DATE_T  date = {0};
    GUI_RECT_T display_rect = {0};
    uint16                      str_width           = 0;
    MMI_STRING_T                week_string            = {0};
    MMI_STRING_T            display_string   = {0};
    uint8                       datestr[30]         = {0};
    wchar                       date_wstr[30]        = {0};
    wchar                       wstr_buf[30]        = {0};
    wchar                       wstr_split[]        = {L", "};
    
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    MMISET_TIME_DISPLAY_TYPE_E  time_display_type   = MMIAPISET_GetTimeDisplayType();
    MMI_IMAGE_ID_T              am_pm_image         = IMAGE_IDLE_BIG_TIME_AM;
    //uint16                      mmi_main_time_y     = MMI_MAIN_TIME_Y;
    GUISTR_STYLE_T text_style = {0};/*lint !e64*/
    GUISTR_STATE_T          text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_EFFECT|GUISTR_STATE_ELLIPSIS|GUISTR_STATE_SINGLE_LINE;
    GUI_RECT_T              opn_rect = {0}; 
    MMI_STRING_T            ampm_str     = {0};
    
    const uint16 hour_img_width = 34;
    const uint16 hour_img_height = 64;
    const uint16 min_img_width = 24;
    const uint16 min_img_height = 43;

    
    uint16  hour_begin_x=0;
    uint16  hour_begin_y=0;
    uint16  min_begin_x=0;
    uint16  min_begin_y=0;

    uint16  date_begin_x=0;
    uint16  date_begin_y=0;
    uint16  week_begin_x=0;
    uint16  week_begin_y=0;    

        
    MMI_IMAGE_ID_T bg_img_id = IMG_DIGIT_CLOCK0_BG;
    //MMI_IMAGE_ID_T colon_img_id = IMG_DIGIT_CLOCK0_WHITE_COLON;
    MMI_IMAGE_ID_T hour_num_id_list[10] = 
    {
        IMG_DIGIT_CLOCK0_HOUR_NUM0,
        IMG_DIGIT_CLOCK0_HOUR_NUM1,
        IMG_DIGIT_CLOCK0_HOUR_NUM2,
        IMG_DIGIT_CLOCK0_HOUR_NUM3,
        IMG_DIGIT_CLOCK0_HOUR_NUM4,
        IMG_DIGIT_CLOCK0_HOUR_NUM5,
        IMG_DIGIT_CLOCK0_HOUR_NUM6,
        IMG_DIGIT_CLOCK0_HOUR_NUM7,
        IMG_DIGIT_CLOCK0_HOUR_NUM8,
        IMG_DIGIT_CLOCK0_HOUR_NUM9
    };
    MMI_IMAGE_ID_T min_num_id_list[10] = 
    {
        IMG_DIGIT_CLOCK0_MIN_NUM0,
        IMG_DIGIT_CLOCK0_MIN_NUM1,
        IMG_DIGIT_CLOCK0_MIN_NUM2,
        IMG_DIGIT_CLOCK0_MIN_NUM3,
        IMG_DIGIT_CLOCK0_MIN_NUM4,
        IMG_DIGIT_CLOCK0_MIN_NUM5,
        IMG_DIGIT_CLOCK0_MIN_NUM6,
        IMG_DIGIT_CLOCK0_MIN_NUM7,
        IMG_DIGIT_CLOCK0_MIN_NUM8,
        IMG_DIGIT_CLOCK0_MIN_NUM9
    };

    MMI_TEXT_ID_T week_table[] =
    {
        STXT_SUNDAY,//       TXT_SHORT_IDLE_SUNDAY,
        STXT_MONDAY,//       TXT_SHORT_IDLE_MONDAY,
        STXT_TUESDAY,//      TXT_SHORT_IDLE_TUESDAY,
        STXT_WEDNESDAY,//       TXT_SHORT_IDLE_WEDNESDAY,
        STXT_THURSDAY,//       TXT_SHORT_IDLE_THURSDAY,
        STXT_FRIDAY,//       TXT_SHORT_IDLE_FRIDAY,
        STXT_SATURDAY,//       TXT_SHORT_IDLE_SATURDAY
    };
        
    TM_GetSysTime(&time);

    //GUIRES_GetImgWidthHeight(&hour_img_width, &hour_img_height, IMG_DIGIT_CLOCK0_HOUR_NUM0, WATCH_LAUNCHER_PANEL_WIN_ID); 
    //GUIRES_GetImgWidthHeight(&min_img_width, &min_img_height, IMG_DIGIT_CLOCK0_MIN_NUM0, WATCH_LAUNCHER_PANEL_WIN_ID); 

    hour_begin_x=100;
    hour_begin_y=52;
    
    min_begin_x=136;
    min_begin_y=128;

    date_begin_x=5;
    date_begin_y=202;
    
    week_begin_x=121;
    week_begin_y=200;

    //12小时制
    //if(0) //if(time_display_type == MMISET_TIME_12HOURS)
    
    MMK_GetWinLcdDevInfo(WATCH_LAUNCHER_PANEL_WIN_ID,&lcd_dev_info);    
    idle_disp_style = MMITHEME_GetIdleDispStyle(MMI_MAIN_LCD_TIME);
    IMG_EnableTransparentColor(TRUE);
    
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, bg_img_id,&lcd_dev_info);

    //hour    
    point.x = hour_begin_x  ;
    point.y = hour_begin_y ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, hour_num_id_list[time.hour/10],&lcd_dev_info);
    point.x += (hour_img_width+2);
    point.y = hour_begin_y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, hour_num_id_list[time.hour%10],&lcd_dev_info);
    
    //min
    point.x = min_begin_x;
    point.y = min_begin_y ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, min_num_id_list[time.min/10],&lcd_dev_info);
    point.x += (min_img_width+2);
    point.y = min_begin_y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, min_num_id_list[time.min%10],&lcd_dev_info);
    IMG_EnableTransparentColor(FALSE);
    
     //date        
    text_style.font = SONG_FONT_30;
    text_style.font_color = MMI_WHITE_COLOR;//MMI_BACKGROUND_COLOR;//changed by James li 20200511
    text_style.region_num = 0;
    text_style.align = ALIGN_HVMIDDLE;//ALIGN_VMIDDLE;
    TM_GetSysDate(&date);

    //MMIAPISET_GetCurrentDateStr(datestr, '-', sizeof(datestr));
    MMIAPISET_GetCurrentDateMonDayStr(datestr, '-', sizeof(datestr));
    MMIAPICOM_StrToWstr(datestr, date_wstr);

    display_string.wstr_ptr = wstr_buf;

    MMIAPICOM_Wstrncpy(display_string.wstr_ptr, date_wstr, MMIAPICOM_Wstrlen(date_wstr));
    display_string.wstr_len = MMIAPICOM_Wstrlen(display_string.wstr_ptr);
    str_width = GUISTR_GetStringWidth(&text_style, &display_string, text_state);
    
    display_rect.top = MMI_MAINSCREEN_HEIGHT-WATCH_PANEL_SPACESHIP_DATE_MARGIN_BOTTOM;
    display_rect.left = WATCH_PANEL_SPACESHIP_DATE_MARGIN_LEFT;
    display_rect.right= display_rect.left+WATCH_PANEL_SPACESHIP_DATE_WIDTH;//150;//
    display_rect.bottom = display_rect.top + WATCH_PANEL_SPACESHIP_DATE_HEIGHT;   
    
    GUISTR_DrawTextToLCDInRect(
        &lcd_dev_info,
        (const GUI_RECT_T *)&display_rect,       //the fixed display area
        (const GUI_RECT_T *)&display_rect,       
        (const MMI_STRING_T *)&display_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO);

   //draw week 

    text_style.font = SONG_FONT_26;
    text_style.font_color = MMI_WHITE_COLOR;//MMI_BACKGROUND_COLOR;//changed by James li 20200511
    text_style.region_num = 0;
    text_style.align = ALIGN_HVMIDDLE;//ALIGN_VMIDDLE;
    
    display_rect.top = MMI_MAINSCREEN_HEIGHT-WATCH_PANEL_SPACESHIP_WEEK_MARGIN_BOTTOM;
    display_rect.left = WATCH_PANEL_SPACESHIP_WEEK_MARGIN_LEFT;//151;//
    display_rect.right = display_rect.left + WATCH_PANEL_SPACESHIP_WEEK_WIDTH;
    display_rect.bottom = display_rect.top + WATCH_PANEL_SPACESHIP_WEEK_HEIGHT;   
    
   SCI_MEMSET(display_string.wstr_ptr, 0, 30*sizeof(wchar));
   MMI_GetLabelTextByLang(week_table[date.wday], &week_string);       
   MMIAPICOM_Wstrncpy(display_string.wstr_ptr, week_string.wstr_ptr, week_string.wstr_len);
   display_string.wstr_len = MMIAPICOM_Wstrlen(display_string.wstr_ptr);

   GUISTR_DrawTextToLCDInRect(
        &lcd_dev_info,
        (const GUI_RECT_T *)&display_rect,       //the fixed display area
        (const GUI_RECT_T *)&display_rect,       
        (const MMI_STRING_T *)&display_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO);   
}
#endif
//==================================宇宙飞船表盘end==========================

//===========================海豚表盘===========================
#ifdef WATCH_PANEL_DOLPHIN_SUPPORT 
LOCAL void drawDigitalWatch1()
{
        MMI_IDLE_DISPLAY_T          idle_disp_style     = {0};
        GUI_POINT_T                 point               = {0};
        SCI_TIME_T                  time                = {0};
        SCI_DATE_T  date = {0};
        GUI_RECT_T display_rect = {0};
        uint16                      str_width           = 0;
        MMI_STRING_T                week_string            = {0};
        MMI_STRING_T            display_string   = {0};
        uint8                       datestr[15]         = {0};
        wchar                       date_wstr[30]        = {0};
        wchar                       wstr_buf[30]        = {0};
        wchar                       wstr_split[]        = {L", "};
        
        GUI_LCD_DEV_INFO lcd_dev_info = {0};
        MMISET_TIME_DISPLAY_TYPE_E  time_display_type   = MMIAPISET_GetTimeDisplayType();  
        MMI_IMAGE_ID_T              am_pm_image         = IMAGE_IDLE_BIG_TIME_AM;
        //uint16                      mmi_main_time_y     = MMI_MAIN_TIME_Y;
        GUISTR_STYLE_T text_style = {0};/*lint !e64*/
        GUISTR_STATE_T          text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_EFFECT|GUISTR_STATE_ELLIPSIS|GUISTR_STATE_SINGLE_LINE;
        GUI_RECT_T              opn_rect = {0}; 
        MMI_STRING_T            ampm_str     = {0};
        const uint16 img_width = 34;
        const uint16 img_height = 64;
        const uint16 blank_gap  = 1;
        const uint16 x_move = img_width + blank_gap;
        const uint16 start_x  = 20;
        uint16  mmi_main_time_y = 40;//(MMI_MAINSCREEN_HEIGHT - img_height)/2 -1;
        uint16  mmi_main_time_y2 = 106;
            
        MMI_IMAGE_ID_T bg_img_id = IMG_DIGIT_CLOCK1_BG;
        //MMI_IMAGE_ID_T colon_img_id = IMG_DIGIT_CLOCK1_WHITE_COLON;
        MMI_IMAGE_ID_T white_num_id_list[10] = 
        {
            IMG_DIGIT_CLOCK1_WHITE_NUM0,
            IMG_DIGIT_CLOCK1_WHITE_NUM1,
            IMG_DIGIT_CLOCK1_WHITE_NUM2,
            IMG_DIGIT_CLOCK1_WHITE_NUM3,
            IMG_DIGIT_CLOCK1_WHITE_NUM4,
            IMG_DIGIT_CLOCK1_WHITE_NUM5,
            IMG_DIGIT_CLOCK1_WHITE_NUM6,
            IMG_DIGIT_CLOCK1_WHITE_NUM7,
            IMG_DIGIT_CLOCK1_WHITE_NUM8,
            IMG_DIGIT_CLOCK1_WHITE_NUM9
        };

    
        MMI_TEXT_ID_T week_table[] =
        {
            STXT_SUNDAY,//       TXT_SHORT_IDLE_SUNDAY,
            STXT_MONDAY,//       TXT_SHORT_IDLE_MONDAY,
            STXT_TUESDAY,//      TXT_SHORT_IDLE_TUESDAY,
            STXT_WEDNESDAY,//       TXT_SHORT_IDLE_WEDNESDAY,
            STXT_THURSDAY,//       TXT_SHORT_IDLE_THURSDAY,
            STXT_FRIDAY,//       TXT_SHORT_IDLE_FRIDAY,
            STXT_SATURDAY,//       TXT_SHORT_IDLE_SATURDAY
        };
            
        if(MMITHEME_IsMainScreenLandscape())
        {
            mmi_main_time_y = MMI_MAIN_TIME_Y_H;
        }
        TM_GetSysTime(&time);
    
        //12С???
        if(0) //if(time_display_type == MMISET_TIME_12HOURS)
        {
            if ((MMISET_TIME_PM_HOUR > time.hour))//AM
            {
                if ((MMISET_TIME_1_HOUR > time.hour))//00:00-1:00
                {
                    time.hour = time.hour + MMISET_TIME_PM_HOUR;
                }
                
    
                 MMI_GetLabelTextByLang(STR_CLK_IDLE_AM, &ampm_str);
            }
            else//PM
            {
                if ((MMISET_TIME_PM_HOUR <= time.hour) && (MMISET_TIME_13_HOUR > time.hour))//12:00-13:00
                {
                }
                else
                {
                    time.hour = time.hour-MMISET_TIME_PM_HOUR;
                }
                 MMI_GetLabelTextByLang(STR_CLK_IDLE_PM, &ampm_str);
                am_pm_image = IMAGE_IDLE_BIG_TIME_PM;
            }   
        }
        MMK_GetWinLcdDevInfo(WATCH_LAUNCHER_PANEL_WIN_ID,&lcd_dev_info);    
        idle_disp_style = MMITHEME_GetIdleDispStyle(MMI_MAIN_LCD_TIME);
        IMG_EnableTransparentColor(TRUE);
        GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, bg_img_id,&lcd_dev_info);
    
        
        point.x = start_x  ;
        point.y = mmi_main_time_y ;
        GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, white_num_id_list[time.hour/10],&lcd_dev_info);
        point.x += x_move;
        point.y = mmi_main_time_y;
        GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, white_num_id_list[time.hour%10],&lcd_dev_info);
        point.x += x_move;
        point.y = mmi_main_time_y ;
        //GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, colon_img_id,&lcd_dev_info);
        point.x -= 2*x_move;
        point.y = mmi_main_time_y2 ;
        GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, white_num_id_list[time.min/10],&lcd_dev_info);
        point.x += x_move;
        point.y = mmi_main_time_y2;
        GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, white_num_id_list[time.min%10],&lcd_dev_info);
        IMG_EnableTransparentColor(FALSE);
    
            //draw date week
           
        text_style.font = SONG_FONT_26;
        text_style.font_color = MMI_WHITE_COLOR;//MMI_BACKGROUND_COLOR;//changed by James li 20200511
    
        text_style.region_num = 0;
    
        text_style.align = ALIGN_LEFT;//ALIGN_VMIDDLE;
    
        TM_GetSysDate(&date);
        MMI_GetLabelTextByLang(week_table[date.wday], &week_string);
        MMIAPISET_GetCurrentDateMonDayStr(datestr, '/', sizeof(datestr));
        MMIAPICOM_StrToWstr(datestr, date_wstr);
    
        display_string.wstr_ptr = wstr_buf;
    
        //MMIAPICOM_Wstrncpy(display_string.wstr_ptr, week_string.wstr_ptr, week_string.wstr_len);
        //MMIAPICOM_Wstrncat(display_string.wstr_ptr, wstr_split, MMIAPICOM_Wstrlen(wstr_split));
        //MMIAPICOM_Wstrncat(display_string.wstr_ptr, date_wstr, MMIAPICOM_Wstrlen(date_wstr));
        MMIAPICOM_Wstrncpy(display_string.wstr_ptr, date_wstr, MMIAPICOM_Wstrlen(date_wstr));
    
        display_string.wstr_len = MMIAPICOM_Wstrlen(display_string.wstr_ptr);
    
        str_width = GUISTR_GetStringWidth(&text_style, &display_string, text_state);
    
        //point = CalStartPoint(idle_disp_style, str_width);
        
       display_rect.top = 170;
       display_rect.left = 20;
       display_rect.bottom = 190;
       display_rect.right = 100;
        
        GUISTR_DrawTextToLCDInRect(
            &lcd_dev_info,
            (const GUI_RECT_T *)&display_rect,       //the fixed display area
            (const GUI_RECT_T *)&display_rect,       
            (const MMI_STRING_T *)&display_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO);
    
       display_rect.top = MMI_MAINSCREEN_HEIGHT-WATCH_PANEL_DOLPHIN_WEEK_MARGIN_BOTTOM;
       display_rect.left = WATCH_PANEL_DOLPHIN_WEEK_MARGIN_LEFT;
       display_rect.bottom = display_rect.top + WATCH_PANEL_DOLPHIN_WEEK_HEIGHT;
       display_rect.right = display_rect.left + WATCH_PANEL_DOLPHIN_WEEK_WIDTH;
       text_style.font = SONG_FONT_22;
       SCI_MEMSET(display_string.wstr_ptr, 0, 30*sizeof(wchar))
       MMIAPICOM_Wstrncpy(display_string.wstr_ptr, week_string.wstr_ptr, week_string.wstr_len);
       display_string.wstr_len = MMIAPICOM_Wstrlen(display_string.wstr_ptr);
    
       GUISTR_DrawTextToLCDInRect(
            &lcd_dev_info,
            (const GUI_RECT_T *)&display_rect,       //the fixed display area
            (const GUI_RECT_T *)&display_rect,       
            (const MMI_STRING_T *)&display_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO);
}
#endif
//===========================海豚表盘 end===========================

//======================太空人表盘==============================
#ifdef WATCH_PANEL_ASTRONAUT_SUPPORT //太空人
LOCAL void drawDigitalWatch2_Battery(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    uint8 p_percent = 0;
    GUI_POINT_T  point = {0};
    uint16 img_width = 0;
    uint16 img_height = 0;
    point.x = WATCH_PANEL_ASTRONAUT_POWER_START_X;
    point.y = WATCH_PANEL_ASTRONAUT_BATTERY_MARGIN_TOP;    
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_POWER,&lcd_dev_info);    

    //draw  percent icon
    point.x = MMI_MAINSCREEN_WIDTH-WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_MARGIN_RIGHT; 
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_PERCENT,&lcd_dev_info);    
    
    //draw  percent
    p_percent=ZDT_GetBatteryPercent();
    
    if(p_percent<0 || p_percent>100)
    {
        p_percent=1;
    }

    if(p_percent== 100)
    {                            
        point.x -= WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_IMG_WIDTH;   
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_NUM0,&lcd_dev_info);
        point.x -= WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_IMG_WIDTH; 
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_NUM0,&lcd_dev_info); 
        point.x -= WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_IMG_WIDTH;       
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_NUM1,&lcd_dev_info);
    }
    else if((p_percent >=10) && (p_percent <=99))    
    {               
        point.x -= WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_IMG_WIDTH;    
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_NUM0+p_percent%10,&lcd_dev_info); 
        point.x -= WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_IMG_WIDTH; 
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_NUM0+p_percent/10,&lcd_dev_info); 
    }
    else
    {
        point.x -= WATCH_PANEL_ASTRONAUT_BATTERY_PERCENT_IMG_WIDTH;      
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK2_OTHER_NUM0+p_percent,&lcd_dev_info);                
    }
}

LOCAL void drawDigitalWatch2_Time(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_POINT_T  point = {0};
    SCI_TIME_T  time   = {0};
    TM_GetSysTime(&time);
    point.x = WATCH_PANEL_ASTRONAUT_TIME_START_X ;
    point.y = WATCH_PANEL_ASTRONAUT_TIME_MARGIN_TOP;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_TIME_NUM0 +time.hour/10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_TIME_IMG_WIDTH+WATCH_PANEL_ASTRONAUT_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_TIME_NUM0+time.hour%10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_TIME_IMG_WIDTH+WATCH_PANEL_ASTRONAUT_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_TIME_COLON,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_COLON_IMG_WIDTH+WATCH_PANEL_ASTRONAUT_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_TIME_NUM0+time.min/10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_TIME_IMG_WIDTH+WATCH_PANEL_ASTRONAUT_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_TIME_NUM0+time.min%10,&lcd_dev_info);
    point.y = WATCH_PANEL_ASTRONAUT_TIME_SECOND_MARGIN_TOP;
    point.x = WATCH_PANEL_ASTRONAUT_TIME_SECOND_START_X;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_TIME_SECOND_NUM0+time.sec/10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_TIME_SECOND_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_TIME_SECOND_NUM0+time.sec%10,&lcd_dev_info);
}

LOCAL void drawDigitalWatch2_Year(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_POINT_T  point = {0};
    SCI_DATE_T  date = {0};
    TM_GetSysDate(&date);
    point.x = WATCH_PANEL_ASTRONAUT_YEAR_START_X;
    point.y = WATCH_PANEL_ASTRONAUT_DATE_Y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+date.year/1000,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+(date.year/100)%10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+(date.year%100)/10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+(date.year%100)%10,&lcd_dev_info); 
}

LOCAL void drawDigitalWatch2_Date(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_POINT_T  point = {0};
    SCI_DATE_T  date = {0};
    TM_GetSysDate(&date);
    point.x = WATCH_PANEL_ASTRONAUT_DATE_START_X ;
    point.y = WATCH_PANEL_ASTRONAUT_DATE_Y ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+date.mon/10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+date.mon%10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_DI,&lcd_dev_info);    
    point.x += WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+date.mday/10,&lcd_dev_info);
    point.x += WATCH_PANEL_ASTRONAUT_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK2_OTHER_NUM0+date.mday%10,&lcd_dev_info);
}

LOCAL void drawDigitalWatch2_Weather(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_POINT_T  point = {0};
    MMI_IMAGE_ID_T weather_img_id = IMG_DIGIT_CLOCK2_WEATHER_UNKNOWN;
    uint8 temp_str[32] = {0};
    int32 temp_degreet=0;
    BOOLEAN degree_is_below_0=FALSE;
    point.x = WATCH_PANEL_ASTRONAUT_WEATHER_MARGIN_LEFT;
    point.y = WATCH_PANEL_ASTRONAUT_WEATHER_ICON_MARGIN_TOP;
    switch(g_yx_wt_info.num)
    {
        case 1:
            weather_img_id = IMG_DIGIT_CLOCK2_WEATHER_SUNNY;
            break;
        case 2:
            weather_img_id = IMG_DIGIT_CLOCK2_WEATHER_CLOUDY;
            break;
        case 3:
            weather_img_id = IMG_DIGIT_CLOCK2_WEATHER_SHOWERS;
            break;
        case 4:
            weather_img_id = IMG_DIGIT_CLOCK2_WEATHER_SNOW;
            break;
        default:
            weather_img_id = IMG_DIGIT_CLOCK2_WEATHER_UNKNOWN;            
            break;
    }    
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,weather_img_id,&lcd_dev_info);    
    
    //draw temperature ICON
    point.x = WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_ICON_START_X;
    point.y = WATCH_PANEL_ASTRONAUT_WEATHER_MARGIN_TOP;    
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_TEMP,&lcd_dev_info);        

    temp_degreet=    g_yx_wt_info.cur_degree;
    if(g_yx_wt_info.cur_degree<0)
    {
        degree_is_below_0 =TRUE;
        temp_degreet=m_abs(g_yx_wt_info.cur_degree);
    }

    if(degree_is_below_0==TRUE)      //负温度
    {
        if(temp_degreet >99)
        {
            temp_degreet=0;
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0,&lcd_dev_info);
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_DI,&lcd_dev_info);
        }
        else if((temp_degreet >=10) && (temp_degreet <=99))    
        {                             
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0+temp_degreet%10,&lcd_dev_info);
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0+temp_degreet/10,&lcd_dev_info); 
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_DI,&lcd_dev_info);
        }
        else
        {
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;     
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0+temp_degreet,&lcd_dev_info);
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_DI,&lcd_dev_info);
        }
    }
    else
    {
        if(temp_degreet >99)
        {
            temp_degreet=0;
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0,&lcd_dev_info);                     
        }
        else if((temp_degreet >=10) && (temp_degreet <=99))    
        {                
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0+temp_degreet%10,&lcd_dev_info); 
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0+temp_degreet/10,&lcd_dev_info); 
        }
        else
        {
            point.x -= WATCH_PANEL_ASTRONAUT_WEATHER_TEMPERATURE_IMG_WIDTH;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK2_OTHER_NUM0+temp_degreet,&lcd_dev_info);                     
        }
    }    
        
}

LOCAL void drawDigitalWatch2()
{
    MMI_IDLE_DISPLAY_T          idle_disp_style     = {0};
    GUI_POINT_T                 point               = {0};
    SCI_TIME_T                  time                = {0};
    SCI_DATE_T  date = {0};
    GUI_RECT_T display_rect = {0};
    uint16                      str_width           = 0;
    MMI_STRING_T                week_string            = {0};
    MMI_STRING_T            display_string   = {0};
    uint8                       datestr[30]         = {0};
    wchar                       date_wstr[30]        = {0};
    wchar                       wstr_buf[30]        = {0};
    wchar                       wstr_split[]        = {L", "};
    
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    MMISET_TIME_DISPLAY_TYPE_E  time_display_type   = MMIAPISET_GetTimeDisplayType();  
    MMI_IMAGE_ID_T              am_pm_image         = IMAGE_IDLE_BIG_TIME_AM;
    //uint16                      mmi_main_time_y     = MMI_MAIN_TIME_Y;
    GUISTR_STYLE_T text_style = {0};/*lint !e64*/
    GUISTR_STATE_T          text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_EFFECT|GUISTR_STATE_ELLIPSIS|GUISTR_STATE_SINGLE_LINE;
    GUI_RECT_T              opn_rect = {0}; 
    MMI_STRING_T            ampm_str     = {0};
    const uint16 img_width = 0;
    const uint16 img_height = 0;
    GUIANIM_CTRL_INFO_T             ctrl_info = {0};
    GUIANIM_DATA_INFO_T             data_info = {0};
    GUIANIM_DISPLAY_INFO_T          display_info = {0};
    GUIANIM_INIT_DATA_T             anim_init = {0};
    MMI_CONTROL_CREATE_T            anim_ctrl = {0};
    GUIANIM_RESULT_E        anim_result = GUIANIM_RESULT_SUCC;
    GUI_BOTH_RECT_T     both_rect = MMITHEME_GetFullScreenBothRect();
     
    
    MMI_TEXT_ID_T week_img_id_list[7] =
    {
        IMG_DIGIT_CLOCK2_WEEK_7,//       TXT_SHORT_IDLE_SUNDAY,
        IMG_DIGIT_CLOCK2_WEEK_1,//       TXT_SHORT_IDLE_MONDAY,
        IMG_DIGIT_CLOCK2_WEEK_2,//      TXT_SHORT_IDLE_TUESDAY,
        IMG_DIGIT_CLOCK2_WEEK_3,//       TXT_SHORT_IDLE_WEDNESDAY,
        IMG_DIGIT_CLOCK2_WEEK_4,//       TXT_SHORT_IDLE_THURSDAY,
        IMG_DIGIT_CLOCK2_WEEK_5,//       TXT_SHORT_IDLE_FRIDAY,
        IMG_DIGIT_CLOCK2_WEEK_6,//       TXT_SHORT_IDLE_SATURDAY
    };  

#ifdef WIN32
    ZDT_InitWeather_for_win32();
#else
    if(!Is_WeatherGetFromServer())
    {
        YX_API_WT_Send();
    }
#endif

    TM_GetSysTime(&time);
    CreateWatchPanelPaintLayer(WATCH_LAUNCHER_PANEL_WIN_ID);
    lcd_dev_info = s_watch2_panel_layer_handle;
    idle_disp_style = MMITHEME_GetIdleDispStyle(MMI_MAIN_LCD_TIME);

    if(MMK_GetCtrlHandleByWin(WATCH_LAUNCHER_PANEL_WIN_ID,MMIIDLE_ANIM_CTRL_ID) == NULL)
    {
        //ctrl_info.is_wallpaper_ctrl = TRUE;
        ctrl_info.is_ctrl_id = TRUE;
        ctrl_info.ctrl_id =MMIIDLE_ANIM_CTRL_ID;
        anim_init.both_rect = MMITHEME_GetFullScreenBothRect(); 
        data_info.img_id = IMG_DIGIT_CLOCK2_BG;     
        //creat anim
        anim_ctrl.ctrl_id           = MMIIDLE_ANIM_CTRL_ID;
        anim_ctrl.guid              = SPRD_GUI_ANIM_ID;
        anim_ctrl.init_data_ptr     = &anim_init;
        anim_ctrl.parent_win_handle = MMK_ConvertIdToHandle(WATCH_LAUNCHER_PANEL_WIN_ID);
        MMK_CreateControl(&anim_ctrl);

        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_play_once = FALSE;
        display_info.is_bg_buf      = TRUE;
        display_info.bg.bg_type     = GUI_BG_COLOR;
        display_info.bg.color       = MMI_WINDOW_BACKGROUND_COLOR;
        display_info.is_update = TRUE;
        anim_result = GUIANIM_SetParam(&ctrl_info,&data_info,PNULL,&display_info);
    }
    IMG_EnableTransparentColor(TRUE);

    //draw weather 
   drawDigitalWatch2_Weather(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);
    //draw battery
   drawDigitalWatch2_Battery(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);
        
    //draw time
    drawDigitalWatch2_Time(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);
    IMG_EnableTransparentColor(FALSE);
    
    //draw week
    TM_GetSysDate(&date);
    point.x = WATCH_PANEL_ASTRONAUT_WEEK_X;
    point.y = WATCH_PANEL_ASTRONAUT_WEEK_Y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, week_img_id_list[date.wday],&lcd_dev_info);
    
    //draw year
    drawDigitalWatch2_Year(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);
    
    //draw date
    drawDigitalWatch2_Date(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);    

}
#endif
//======================太空人表盘 end==============================

//=====================太空站宇航员表盘 ============================
#if defined(WATCH_PANEL_SPACE_STATION_SUPPORT) && defined(MAINLCD_DEV_SIZE_240X284) //太空站宇航员
LOCAL void drawDigitalWatch3_Weather(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_POINT_T  point = {0};
    MMI_IMAGE_ID_T weather_img_id = IMG_DIGIT_CLOCK3_WEATHER_UNKNOWN;
    uint8 temp_str[32] = {0};
    int32 temp_degreet=0;
    BOOLEAN degree_is_below_0=FALSE;
#ifdef ZTE_WATCH
    YX_WEATHER_INFO_T wt_info = zte_wt_info[0];
#else
    YX_WEATHER_INFO_T wt_info = g_yx_wt_info;
#endif
    uint8 temperature_img_width = WATCH_PANEL_SPACE_STATION_WEATHER_TEMPERATURE_IMG_WIDTH;
    point.x = WATCH_PANEL_SPACE_STATION_WEATHER_MARGIN_LEFT;
    point.y = WATCH_PANEL_SPACE_STATION_WEATHER_ICON_MARGIN_TOP;
    switch(wt_info.num)
    {
        case 1:
            weather_img_id = IMG_DIGIT_CLOCK3_WEATHER_SUNNY;
            break;
        case 2:
            weather_img_id = IMG_DIGIT_CLOCK3_WEATHER_CLOUDY;
            break;
        case 3:
            weather_img_id = IMG_DIGIT_CLOCK3_WEATHER_SHOWERS;
            break;
        case 4:
            weather_img_id = IMG_DIGIT_CLOCK3_WEATHER_SNOW;
            break;
        default:
            weather_img_id = IMG_DIGIT_CLOCK3_WEATHER_UNKNOWN;            
            break;
    }    
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,weather_img_id,&lcd_dev_info);    
    
    //draw temperature ICON
    point.x = WATCH_PANEL_SPACE_STATION_WEATHER_TEMPERATURE_ICON_START_X;
    point.y = WATCH_PANEL_SPACE_STATION_WEATHER_TEMPERATURE_MARGIN_TOP;    
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_TEMP,&lcd_dev_info);        

    temp_degreet=    wt_info.cur_degree;
    if(wt_info.cur_degree<0)
    {
        degree_is_below_0 =TRUE;
        temp_degreet=m_abs(wt_info.cur_degree);
    }

    if(degree_is_below_0==TRUE)      //负温度
    {
        if(temp_degreet >99)
        {
            temp_degreet=0;
            point.x -= temperature_img_width;
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0,&lcd_dev_info);
            point.x -= temperature_img_width;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_HYPHEN,&lcd_dev_info);
        }
        else if((temp_degreet >=10) && (temp_degreet <=99))    
        {                             
            point.x -= temperature_img_width;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0+temp_degreet%10,&lcd_dev_info);
            point.x -= temperature_img_width;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0+temp_degreet/10,&lcd_dev_info); 
            point.x -= temperature_img_width;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_HYPHEN,&lcd_dev_info);
        }
        else
        {
            point.x -= temperature_img_width;     
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0+temp_degreet,&lcd_dev_info);   
            point.x -= temperature_img_width;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_HYPHEN,&lcd_dev_info);
        }
    }
    else
    {
        if(temp_degreet >99)
        {
            temp_degreet=0;
            point.x -= temperature_img_width;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0,&lcd_dev_info);                     
        }
        else if((temp_degreet >=10) && (temp_degreet <=99))    
        {                
            point.x -= temperature_img_width;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0+temp_degreet%10,&lcd_dev_info); 
            point.x -= temperature_img_width;   
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0+temp_degreet/10,&lcd_dev_info); 
        }
        else
        {
            point.x -= temperature_img_width;  
            GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID,IMG_DIGIT_CLOCK3_WHITE_NUM0+temp_degreet,&lcd_dev_info);                     
        }
    }    
        
}

LOCAL void drawDigitalWatch3_Battery(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    uint8 p_percent = 0;
    GUI_POINT_T  point = {0};
    uint16 img_width = 0;
    uint16 img_height = 0;
    point.x = WATCH_PANEL_SPACE_STATION_POWER_START_X;
    point.y = WATCH_PANEL_SPACE_STATION_BATTERY_MARGIN_TOP;    
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_POWER,&lcd_dev_info);    

    //draw  percent icon
    point.x = MMI_MAINSCREEN_WIDTH-WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_MARGIN_RIGHT; 
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_PERCENT,&lcd_dev_info);    
    
    //draw  percent
    p_percent=ZDT_GetBatteryPercent();
    
    if(p_percent<0 || p_percent>100)
    {
        p_percent=1;
    }

    if(p_percent== 100)
    {                            
        point.x -= WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_IMG_WIDTH;   
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_WHITE_NUM0,&lcd_dev_info);
        point.x -= WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_IMG_WIDTH; 
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_WHITE_NUM0,&lcd_dev_info); 
        point.x -= WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_IMG_WIDTH;       
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_WHITE_NUM1,&lcd_dev_info);
    }
    else if((p_percent >=10) && (p_percent <=99))    
    {               
        point.x -= WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_IMG_WIDTH;    
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_WHITE_NUM0+p_percent%10,&lcd_dev_info); 
        point.x -= WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_IMG_WIDTH; 
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_WHITE_NUM0+p_percent/10,&lcd_dev_info); 
    }
    else
    {
        point.x -= WATCH_PANEL_SPACE_STATION_BATTERY_PERCENT_IMG_WIDTH;      
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMG_DIGIT_CLOCK3_WHITE_NUM0+p_percent,&lcd_dev_info);                
    }
}

LOCAL void drawDigitalWatch3_Time(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_POINT_T  point = {0};
    SCI_TIME_T  time   = {0};
    TM_GetSysTime(&time);
    point.x = WATCH_PANEL_SPACE_STATION_TIME_START_X ;
    point.y = WATCH_PANEL_SPACE_STATION_TIME_MARGIN_TOP;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_DATE_NUM0 +time.hour/10,&lcd_dev_info);
    point.x += WATCH_PANEL_SPACE_STATION_TIME_IMG_WIDTH+WATCH_PANEL_SPACE_STATION_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_DATE_NUM0+time.hour%10,&lcd_dev_info);

    point.x += WATCH_PANEL_SPACE_STATION_TIME_IMG_WIDTH+WATCH_PANEL_SPACE_STATION_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_DATE_COLON,&lcd_dev_info);

    point.x += WATCH_PANEL_SPACE_STATION_COLON_IMG_WIDTH+WATCH_PANEL_SPACE_STATION_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_DATE_NUM0+time.min/10,&lcd_dev_info);
    point.x += WATCH_PANEL_SPACE_STATION_TIME_IMG_WIDTH+WATCH_PANEL_SPACE_STATION_TIME_X_OFFSET;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_DATE_NUM0+time.min%10,&lcd_dev_info);
}

LOCAL void drawDigitalWatch3_Date(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUI_POINT_T  point = {0};
    SCI_DATE_T  date = {0};
    TM_GetSysDate(&date);
    point.x = WATCH_PANEL_SPACE_STATION_DATE_START_X ;
    point.y = WATCH_PANEL_SPACE_STATION_DATE_Y ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_WHITE_NUM0+date.mon/10,&lcd_dev_info);
    point.x += WATCH_PANEL_SPACE_STATION_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_WHITE_NUM0+date.mon%10,&lcd_dev_info);
    point.x += WATCH_PANEL_SPACE_STATION_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_HYPHEN,&lcd_dev_info);    
    point.x += WATCH_PANEL_SPACE_STATION_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_WHITE_NUM0+date.mday/10,&lcd_dev_info);
    point.x += WATCH_PANEL_SPACE_STATION_DATE_IMG_WIDTH;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMG_DIGIT_CLOCK3_WHITE_NUM0+date.mday%10,&lcd_dev_info);
}

LOCAL void drawDigitalWatch3()
{
    MMI_IDLE_DISPLAY_T          idle_disp_style     = {0};
    GUI_POINT_T                 point               = {0};
    SCI_TIME_T                  time                = {0};
    SCI_DATE_T  date = {0};
    GUI_RECT_T display_rect = {0};
    uint16                      str_width           = 0;
    MMI_STRING_T                week_string            = {0};
    MMI_STRING_T            display_string   = {0};
    uint8                       datestr[30]         = {0};
    wchar                       date_wstr[30]        = {0};
    wchar                       wstr_buf[30]        = {0};
    wchar                       wstr_split[]        = {L", "};
    
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    MMISET_TIME_DISPLAY_TYPE_E  time_display_type   = MMIAPISET_GetTimeDisplayType();  
    MMI_IMAGE_ID_T              am_pm_image         = IMAGE_IDLE_BIG_TIME_AM;
    //uint16                      mmi_main_time_y     = MMI_MAIN_TIME_Y;
    GUISTR_STYLE_T text_style = {0};/*lint !e64*/
    GUISTR_STATE_T          text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_EFFECT|GUISTR_STATE_ELLIPSIS|GUISTR_STATE_SINGLE_LINE;
    GUI_RECT_T              opn_rect = {0}; 
    MMI_STRING_T            ampm_str     = {0};
    const uint16 img_width = 0;
    const uint16 img_height = 0;
    GUIANIM_CTRL_INFO_T             ctrl_info = {0};
    GUIANIM_DATA_INFO_T             data_info = {0};
    GUIANIM_DISPLAY_INFO_T          display_info = {0};
    GUIANIM_INIT_DATA_T             anim_init = {0};
    MMI_CONTROL_CREATE_T            anim_ctrl = {0};
    GUIANIM_RESULT_E        anim_result = GUIANIM_RESULT_SUCC;
    GUI_BOTH_RECT_T     both_rect = MMITHEME_GetFullScreenBothRect();
     
    
    MMI_TEXT_ID_T week_img_id_list[7] =
    {
        IMG_DIGIT_CLOCK3_WEEK7,//       TXT_SHORT_IDLE_SUNDAY,
        IMG_DIGIT_CLOCK3_WEEK1,//       TXT_SHORT_IDLE_MONDAY,
        IMG_DIGIT_CLOCK3_WEEK2,//      TXT_SHORT_IDLE_TUESDAY,
        IMG_DIGIT_CLOCK3_WEEK3,//       TXT_SHORT_IDLE_WEDNESDAY,
        IMG_DIGIT_CLOCK3_WEEK4,//       TXT_SHORT_IDLE_THURSDAY,
        IMG_DIGIT_CLOCK3_WEEK5,//       TXT_SHORT_IDLE_FRIDAY,
        IMG_DIGIT_CLOCK3_WEEK6,//       TXT_SHORT_IDLE_SATURDAY
    };  

#ifdef WIN32
    ZDT_InitWeather_for_win32();
#else
#ifdef ZTE_WATCH
    if(!Is_ZTE_WeatherGetFromServer())
#else
    if(!Is_WeatherGetFromServer())
#endif
    {
        YX_API_WT_Send();
    }
#endif

    TM_GetSysTime(&time);
    CreateWatchPanelPaintLayer(WATCH_LAUNCHER_PANEL_WIN_ID);
    lcd_dev_info = s_watch2_panel_layer_handle;
    idle_disp_style = MMITHEME_GetIdleDispStyle(MMI_MAIN_LCD_TIME);

    if(MMK_GetCtrlHandleByWin(WATCH_LAUNCHER_PANEL_WIN_ID,LAUNCHER_SPACE_STATION_ANIM_CTRL_ID) == NULL)
    {
        //ctrl_info.is_wallpaper_ctrl = TRUE;
        ctrl_info.is_ctrl_id = TRUE;
        ctrl_info.ctrl_id =LAUNCHER_SPACE_STATION_ANIM_CTRL_ID;
        anim_init.both_rect = MMITHEME_GetFullScreenBothRect(); 
        data_info.img_id = IMG_DIGIT_CLOCK3_BG;     
        //creat anim
        anim_ctrl.ctrl_id           = LAUNCHER_SPACE_STATION_ANIM_CTRL_ID;
        anim_ctrl.guid              = SPRD_GUI_ANIM_ID;
        anim_ctrl.init_data_ptr     = &anim_init;
        anim_ctrl.parent_win_handle = MMK_ConvertIdToHandle(WATCH_LAUNCHER_PANEL_WIN_ID);
        MMK_CreateControl(&anim_ctrl);

        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
        display_info.is_play_once = FALSE;
        display_info.is_bg_buf      = TRUE;
        display_info.bg.bg_type     = GUI_BG_COLOR;
        display_info.bg.color       = MMI_WINDOW_BACKGROUND_COLOR;
        display_info.is_update = TRUE;
        anim_result = GUIANIM_SetParam(&ctrl_info,&data_info,PNULL,&display_info);
    }
    IMG_EnableTransparentColor(TRUE);

    //draw weather 
   drawDigitalWatch3_Weather(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);
    //draw battery
   drawDigitalWatch3_Battery(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);
        
    //draw time
    drawDigitalWatch3_Time(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);
    IMG_EnableTransparentColor(FALSE);
    
    //draw week
    TM_GetSysDate(&date);
    point.x = WATCH_PANEL_SPACE_STATION_WEEK_X;
    point.y = WATCH_PANEL_SPACE_STATION_WEEK_Y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,WATCH_LAUNCHER_PANEL_WIN_ID, week_img_id_list[date.wday],&lcd_dev_info);
    
    //draw date
    drawDigitalWatch3_Date(WATCH_LAUNCHER_PANEL_WIN_ID, lcd_dev_info);    
}
#endif
//=====================太空站宇航员表盘end ============================
#ifdef ZMT_DIAL_STORE_SUPPORT
int32 zmt_watch_last_index = -1;
uint8 * zmt_watch_name = NULL;
LOCAL void ZMT_DisplayDial(MMI_WIN_ID_T win_id, PANEL_INDEX_E index)
{
    uint8 idx = 0;
    uint8 size = 0;
    ZMT_DIAL_LIST_INFO_T * dail_list = NULL;

    idx = index - ZMT_DIAL_INDEX_0;
    //SCI_TRACE_LOW("%s: zmt_watch_last_index = %d, index = %d", __FUNCTION__, zmt_watch_last_index, index);
    if(zmt_watch_last_index != index){
        zmt_watch_last_index = index;
        dail_list = ZmtWatch_GetPanelList();
        if(dail_list != NULL){
            if(zmt_watch_name != NULL){
                SCI_FREE(zmt_watch_name);
                zmt_watch_name = NULL;
            }
            size = strlen(dail_list->info[idx]->name);
            zmt_watch_name = SCI_ALLOC_APPZ(size + 1);
            memset(zmt_watch_name, 0, size + 1);
            SCI_MEMCPY(zmt_watch_name, dail_list->info[idx]->name, size);            
            ZmtDial_ReleaseDialList(dail_list);
        }
    }
    if(zmt_watch_name != NULL){
        ZMT_DialPanelShow(win_id, zmt_watch_name);
    }
}
#endif

LOCAL void draw_watch_panel(PANEL_INDEX_E watch_index)
{

    switch(watch_index)
    {
        case PANEL_SPACE_SHIP:
            #ifdef WATCH_PANEL_SPACE_SHIP_SUPPORT //宇宙飞船数字表盘
            drawDigitalWatch0();
            #endif
            break;
        case PANEL_DOLPHIN:
            #ifdef WATCH_PANEL_DOLPHIN_SUPPORT //海豚表盘
            drawDigitalWatch1();
            #endif
            break;            
        case PANEL_ASTRONAUT:
            #ifdef WATCH_PANEL_ASTRONAUT_SUPPORT //太空人
            drawDigitalWatch2();
            #endif
            break;
        case PANEL_SPACE_STATION:
            #if defined(WATCH_PANEL_SPACE_STATION_SUPPORT) && defined(MAINLCD_DEV_SIZE_240X284) //太空站宇航员
            drawDigitalWatch3();
            #endif
            break;
    #ifdef ZTE_WATCH
        case ZTE_PANEL_SPACEMAN://宇航员
            ZTE_Draw_Spaceman_Panel(WATCH_LAUNCHER_PANEL_WIN_ID);
            break;
        case ZTE_PANEL_ROCKET://火箭
            ZTE_Draw_Rocket_Panel(WATCH_LAUNCHER_PANEL_WIN_ID);
            break;
        case ZTE_PANEL_CAT://粉色猫猫
            ZTE_Draw_Cat_Panel(WATCH_LAUNCHER_PANEL_WIN_ID);
            break;
    #endif
#ifdef ZMT_DIAL_STORE_SUPPORT
        case ZMT_DIAL_INDEX_0:
        case ZMT_DIAL_INDEX_1:
        case ZMT_DIAL_INDEX_2:
        case ZMT_DIAL_INDEX_3:
        case ZMT_DIAL_INDEX_4:
        case ZMT_DIAL_INDEX_5:
        case ZMT_DIAL_INDEX_6:
        case ZMT_DIAL_INDEX_7:
        case ZMT_DIAL_INDEX_8:
        case ZMT_DIAL_INDEX_9:
            {
                ZMT_DisplayDial(WATCH_LAUNCHER_PANEL_WIN_ID, watch_index);
            }
            break;
#endif
        default:
            setDefaultPanel();
            break;

    }
}

///////////////////////////////////////////////////////////////////////////////

uint32 sMoveTimer = 0;
LOCAL void startMoveTimer()
{
    if (0 == sMoveTimer)
    {
        sMoveTimer = MMK_CreateWinTimer(WATCH_LAUNCHER_PANEL_WIN_ID, 60,TRUE);
    }    
}

LOCAL void stopMoveTimer()
{
    if (0 != sMoveTimer)
    {
        MMK_StopTimer(sMoveTimer);
    }    

    sMoveTimer = 0;
}

//WindProc
LOCAL MMI_RESULT_E MMIWatchPanel_HandleWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    LOCAL GUI_LCD_DEV_INFO lcd_dev_info = {0};
    const GUI_COLOR_T *pDstData = GetLcdInfoByLcdDev_Ex(&lcd_dev_info);
    //const GUI_COLOR_T *pDstData = GUILCD_GetMainLcdBufPtr();
    
    BOOLEAN ret = TRUE;
    
    SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg win_id = %0x, msg_id = %0x. bingd_statues:%d", win_id, msg_id,struct_yx_statues_data.bingd_statues);
#ifdef ZTE_WATCH
    if(Low_Battery_Model_Check())
    {
        if(MSG_LOSE_FOCUS == msg_id)
        {
            ReleaseWatchPanelPaintLayer();
            MMIWatchIdle_StopClockTimer();
        }
        return HandleZTE_LowBatteryWinMsg(win_id,msg_id,param);
    }
#endif
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg. MSG_OPEN_WINDOW.");
            break;
        }
        
        case MSG_GET_FOCUS:
        {
            SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg. MSG_GET_FOCUS.");
            //restart timer.
            MMK_SendMsg(win_id,MSG_FULL_PAINT,PNULL);
            MMIWatchIdle_StopClockTimer();
            MMIWatchIdle_StartClockTimer(CLOCK_UPDATE_PERIOD);
            break;
        }

        case MSG_CLOSE_WINDOW:
        {                 
            MMIWatchIdle_StopClockTimer();
            ReleaseWatchPanelPaintLayer();
            break;
        }
        case MSG_LOSE_FOCUS:
        {
            SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg. MSG_LOSE_FOCUS.");
            ReleaseWatchPanelPaintLayer();
            MMIWatchIdle_StopClockTimer();
            if(MMK_GetCtrlHandleByWin(WATCH_LAUNCHER_PANEL_WIN_ID,MMIIDLE_ANIM_CTRL_ID) != NULL)
            {
                MMK_DestroyControl(MMIIDLE_ANIM_CTRL_ID);
            }
            break;
        }

#ifdef TOUCH_PANEL_SUPPORT
        case MSG_APP_GREEN:
        {
            break;
        }
        case MSG_TP_PRESS_LONG:
        {
            if (FALSE == MMIAPIUDISK_UdiskIsRun())
            {
                //WATCHFACE_Edit_Slidepage_Enter();
                //MMIWatchIdle_CreateAnimWin(NULL);
                WATCH_PANEL_TYPE_T style = MMISET_GetWatchPanelStyle();
                Watch_Panel_Select_Enter(style.watch_index);
            }
            break;
        }
#endif

        case MSG_FULL_PAINT:
        {
            SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg. MSG_FULL_PAINT.");
            //if (MMK_GetFocusWinId() ==WATCH_LAUNCHER_PANEL_WIN_ID)
            {
                GUI_RECT_T disp_rect = MMITHEME_GetFullScreenRect();
                WATCH_PANEL_TYPE_T style = MMISET_GetWatchPanelStyle();
                LCD_FillRect(&lcd_dev_info, disp_rect, MMI_BLACK_COLOR);
            #ifdef ANALOG_CLK_SUPPORT  //yangyu add
                if(style.is_anolog_panel)
                {
                    SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg. draw clock begin index:%d", style.watch_index);
                    updateClock(style.watch_index);
                    //ZDT_DisplayBattery(win_id);
                    //ZDT_DisplaySingal(win_id);

                    SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg. draw clock end.");
                }
                else
                {
                    draw_watch_panel(style.watch_index);
                    if(style.watch_index == PANEL_SPACE_SHIP || style.watch_index == PANEL_DOLPHIN)
                    {
                        ZDT_DisplayBattery(win_id);
                        ZDT_DisplaySingal(win_id);
                    }
                }
            #else
                draw_watch_panel(style.watch_index);
            #endif

            }

            break;
        }

        case MSG_TIMER:
        {
            if (NULL != param)
            {
                if (s_clock_timer_id == *(uint8*)param)
                {
                    if(MMK_GetFocusWinId() !=WATCH_LAUNCHER_PANEL_WIN_ID)   //xiongkai
                    {
                        MMIWatchIdle_StopClockTimer();       
                        break;        
                    }
                    MMK_SendMsg(win_id,MSG_FULL_PAINT,PNULL);
                }
            }
            break;
        }

        case MSG_APP_LEFT:
        case MSG_APP_UP:
        {
            break;
        }

        case MSG_APP_RIGHT:
        case MSG_APP_DOWN:
        case MSG_APP_OK:
        case MSG_KEYDOWN_RED:
            break;

        case MSG_KEYUP_RED:
            if(MMK_IsFocusWin(win_id))
            {
                MMIDEFAULT_CloseAllLight_Watch();
            }
            break;
        case MSG_SLIDEPAGE_OPENED:
            break;
#ifdef ZTE_WATCH        
        case MSG_TP_PRESS_UP:
            Panel_TP_UP(param);
            ret = FALSE;//tp up 消息透传到WatchLAUNCHER_HandleCommonWinMsg 处理防止下拉上拉判断错误
            break;
#endif
        default:
            ret = FALSE;
            break;
    }
    SCI_TRACE_LOW("MMIWatchIdle_HandleWinMsg. end.");
    if(!ret)
    {
        ret = WatchLAUNCHER_HandleCommonWinMsg(win_id, msg_id, param);
    }
    return ret;
}

WINDOW_TABLE(WATCH_LAUNCHER_PANEL_WINTAB) =
{
    WIN_HIDE_STATUS,
    WIN_FUNC( (uint32)MMIWatchPanel_HandleWinMsg),
    WIN_ID(WATCH_LAUNCHER_PANEL_WIN_ID),
    WIN_STYLE(WS_DISPATCH_TO_CHILDWIN |WS_DISABLE_RETURN_WIN),
    WIN_MOVE_STYLE(MOVE_FORBIDDEN),
    END_WIN
};

//export apis
///////////////////////////////////////////////////////////////////////////////
PUBLIC void MMIWatchPanel_Open(void)
{
    if(MMK_IsOpenWin(WATCH_LAUNCHER_PANEL_WIN_ID) )
    {
       MMK_CloseWin(WATCH_LAUNCHER_PANEL_WIN_ID);
    }
    MMK_CreateWin(WATCH_LAUNCHER_PANEL_WINTAB, NULL);
    MMK_WinGrabFocus(WATCH_LAUNCHER_PANEL_WIN_ID);
    MMK_SetFocusWin(MMK_ConvertIdToHandle(WATCH_LAUNCHER_PANEL_WIN_ID));
    
}

//=======================熊猫表盘========================
#ifdef WATCH_PANEL_PANDA_SUPPORT //熊猫
LOCAL void DrawAClock()
{
    ///////////////////
    uint32 mem_width_ptr = 0;
    uint32 dst_widget_type = 0;
    void *lcd_buffer_ptr = NULL;
    MMI_WIN_ID_T win_id = WATCH_LAUNCHER_PANEL_WIN_ID;
	
    ////////////////////

//#ifdef UI_MULTILAYER_SUPPORT
    GUI_RECT_T        win_rect = {0};
    GUI_LCD_DEV_INFO  lcd_dev_info = {0, 0};
    SCI_TIME_T        sys_time        = {0};
    GUI_POINT_T       point = {0};  
    CLOCK_DRAW_IN_T   aclock_input_param  = {0};
    CLOCK_DRAW_OUT_T  aclock_output_param = {0};
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T bk_img = {0};
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T fore_img = {0};

//#undef UI_MULTILAYER_SUPPORT
#if (defined UI_MULTILAYER_SUPPORT) && (!defined IDLE_MAINMENU_LOW_MEMORY_SUPPORT)
    UILAYER_INFO_T    layer_info = {0};
#endif

    GUI_RECT_T fullScreenRect = MMITHEME_GetFullScreenRect();
    uint16            lcd_width  = 0;
    uint16            lcd_height = 0;
    uint16            width  = 0;
    uint16            height = 0;

    uint16 nScreenWidth =fullScreenRect.right - fullScreenRect.left - 1;
    uint16 nScreenHeight = fullScreenRect.bottom - fullScreenRect.top - 1;
    //lzk begin. 2018.04.12
    //add pure dark bg
    static int sec_times = 0;
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
#if 0
    //aclock dial.

    aclock_input_param.clock_dial.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.clock_dial.width, &aclock_input_param.clock_dial.height, IMAGE_WATCHIDLE_ACLOCK_PANEL1, win_id);
    aclock_input_param.clock_dial.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_PANEL1,win_id,&(aclock_input_param.clock_dial.data_size));
    aclock_input_param.clock_dial.center_x = aclock_input_param.clock_dial.width/2;
    aclock_input_param.clock_dial.center_y = aclock_input_param.clock_dial.height/2;   


    //aclock hour hand
    aclock_input_param.hour_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.hour_handle.width, &aclock_input_param.hour_handle.height, IMAGE_WATCHIDLE_ACLOCK_HOUR, win_id);    
    aclock_input_param.hour_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_HOUR,win_id,&(aclock_input_param.hour_handle.data_size));
	aclock_input_param.hour_handle.center_x = aclock_input_param.hour_handle.width/2;
    aclock_input_param.hour_handle.center_y = aclock_input_param.hour_handle.height/2;    

    //aclock min hand
    aclock_input_param.min_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.min_handle.width, &aclock_input_param.min_handle.height, IMAGE_WATCHIDLE_ACLOCK_MIN, win_id);        
    aclock_input_param.min_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_MIN,win_id,&(aclock_input_param.min_handle.data_size));
    aclock_input_param.min_handle.center_x = aclock_input_param.min_handle.width/2;
    aclock_input_param.min_handle.center_y = aclock_input_param.min_handle.height/2; 

    //aclock sec hand
    aclock_input_param.sec_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.sec_handle.width, &aclock_input_param.sec_handle.height, IMAGE_WATCHIDLE_ACLOCK_SEC, win_id);        
    aclock_input_param.sec_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_SEC,win_id,&(aclock_input_param.sec_handle.data_size));
    aclock_input_param.sec_handle.center_x = aclock_input_param.sec_handle.width/2;
    aclock_input_param.sec_handle.center_y = aclock_input_param.sec_handle.height/2; 

    aclock_input_param.target_buf_size = aclock_input_param.clock_dial.width * aclock_input_param.clock_dial.height * 4;
    Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
    aclock_input_param.target_buf_ptr = (void*)SCI_ALLOCA(aclock_input_param.target_buf_size);

    if (PNULL == aclock_input_param.target_buf_ptr)
    {
        //SCI_TRACE_LOW:"error: DrawAClock target_buf_ptr alloc failed,  return directly"
        //SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIKL_KEYLOCK_557_112_2_18_2_31_4_6,(uint8*)"");
        return;
    }

    //get system time
    TM_GetSysTime(&sys_time);  

#ifdef WIN32
    aclock_input_param.time.hour = sys_time.hour;
    aclock_input_param.time.minute  = sys_time.min;
    aclock_input_param.time.second = (sec_times++) % 60;//sys_time.sec;
#else
    aclock_input_param.time.hour = sys_time.hour;
    aclock_input_param.time.minute  = sys_time.min;
    aclock_input_param.time.second = sys_time.sec;
#endif

#if defined(ANALOG_CLK_SUPPORT)
    #if 1//def WIN32
    {
        GRAPH_DrawClock(&aclock_input_param, &aclock_output_param);//yangyu ,now only simulator can use
    }
    #endif
#endif
    Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    //display aclock
    MMK_GetWinRect(win_id, &win_rect);


#if (defined UI_MULTILAYER_SUPPORT) && (!defined IDLE_MAINMENU_LOW_MEMORY_SUPPORT)
    UILAYER_GetLayerInfo(&lcd_dev_info, &layer_info);
    
    bk_img.data_ptr = UILAYER_GetLayerBufferPtr(&lcd_dev_info);
    bk_img.data_type = layer_info.data_type; 
    //bk_img.data_type = IMGREF_FORMAT_RGB565;
    bk_img.width = layer_info.mem_width;
    bk_img.height = layer_info.layer_height;
#else
    GetLcdInfoByLcdDev_All(&lcd_dev_info, 
    &mem_width_ptr, 
    &bk_img.width, 
    &bk_img.height, 
    &dst_widget_type,
    &lcd_buffer_ptr);

    //bk_img.data_type = IMGREF_FORMAT_RGB565;
    bk_img.data_type = dst_widget_type;
    bk_img.data_ptr = lcd_buffer_ptr;
#endif
	
    if (PNULL == bk_img.data_ptr)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
	    aclock_input_param.target_buf_ptr = NULL;	
        SCI_TRACE_LOW("error: DrawAClock PNULL == bk_img.data_ptr");      
        return;
    }		
    Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    fore_img.data_ptr = aclock_output_param.output_ptr;
    fore_img.data_type = aclock_output_param.type;//IMGREF_FORMAT_ARGB888;
    fore_img.width = aclock_input_param.clock_dial.width;
    fore_img.height = aclock_input_param.clock_dial.height;

    if (PNULL == fore_img.data_ptr)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
	    aclock_input_param.target_buf_ptr = NULL;	
        SCI_TRACE_LOW("error: DrawAClock PNULL == fore_img.data_ptr");      
        return;
    }   
    Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);
    
    //fore_img.start_x = (lcd_width - aclock_input_param.clock_dial.width)/2;   
    //fore_img.start_y = top; 
    fore_img.start_x = MMI_MAINSCREEN_WIDTH - fore_img.width; // point.x; //(128 - aclock_input_param.clock_dial.width)/2;   
    fore_img.start_y = (MMI_MAINSCREEN_HEIGHT - fore_img.height)/2; // point.y; //top; 

	//memcpy(bk_img.data_ptr, fore_img.data_ptr, fore_img.width * fore_img.height * 3);

    if (!ProcessAlphaBlending(&bk_img, &fore_img))
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);   
	    aclock_input_param.target_buf_ptr = NULL;	
        return;
    }

    //in the end, display the center point.
    GUIRES_GetImgWidthHeight(&width, &height, IMAGE_KEYLOCK_ACLOCK_CENTER, win_id); 

    point.x = (lcd_width - width)/2;
    point.y = (lcd_height - height)/2; // top + (aclock_input_param.clock_dial.height - height)/2;

    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,IMAGE_KEYLOCK_ACLOCK_CENTER,&lcd_dev_info);

    MMITHEME_StoreUpdateRect(&lcd_dev_info,win_rect);
        
    if (aclock_input_param.target_buf_ptr != NULL)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
        aclock_input_param.target_buf_ptr = NULL;
    }
    SCI_TRACE_LOW("DrawAClock() exit.");
#else
{
    GUI_RECT_T dis_rect = MMITHEME_GetFullScreenRectEx(win_id);
    TM_GetSysTime(&sys_time);  
    TimeTeacherClock(win_id,
        dis_rect,
        sys_time,
        IMAGE_WATCHIDLE_ACLOCK_PANEL1,
        IMAGE_WATCHIDLE_ACLOCK_HOUR,
        IMAGE_WATCHIDLE_ACLOCK_MIN,
        IMAGE_WATCHIDLE_ACLOCK_SEC,
        IMAGE_KEYLOCK_ACLOCK_CENTER
    );
}
#endif 

    
}
#endif
//=======================熊猫表盘end========================

//==========================黑绿色模拟时钟表盘=============
#ifdef WATCH_PANEL_BLACK_GREEN_SUPPORT //黑绿色模拟时钟
LOCAL void DrawAClock1()
{
//#define TEST_ON 1
#ifdef TEST_ON

#else
    ///////////////////
    uint32 mem_width_ptr = 0;
    uint32 dst_widget_type = 0;
    void *lcd_buffer_ptr = NULL;
    MMI_WIN_ID_T win_id = WATCH_LAUNCHER_PANEL_WIN_ID;

     //xiongkai draw date
	uint8						datestr[16] 		= {0};
	wchar						datewstr[16]		= {0};
	SCI_DATE_T                  sys_date = {0};
	uint16						len 				= {0}; 
	GUI_FONT_T					font				= SONG_FONT_8;
	GUI_COLOR_T 				color				= 0;
	uint16						str_width			= 0;
	GUISTR_STYLE_T			text_style = {0};/*lint !e64*/
	GUISTR_STATE_T			text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_EFFECT|GUISTR_STATE_ELLIPSIS;
	MMI_STRING_T			text   = {0};

	SCI_TIME_T systime = {0};
		
    ////////////////////

//#ifdef UI_MULTILAYER_SUPPORT
    GUI_RECT_T        win_rect = {0};
    GUI_LCD_DEV_INFO  lcd_dev_info = {0, 0};
    SCI_TIME_T        sys_time        = {0};
    GUI_POINT_T       point = {0};  
    CLOCK_DRAW_IN_T   aclock_input_param  = {0};
    CLOCK_DRAW_OUT_T  aclock_output_param = {0};
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T bk_img = {0};
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T fore_img = {0};

//#undef UI_MULTILAYER_SUPPORT
#if (defined UI_MULTILAYER_SUPPORT) && (!defined IDLE_MAINMENU_LOW_MEMORY_SUPPORT)
    UILAYER_INFO_T    layer_info = {0};
#endif

    GUI_RECT_T fullScreenRect = MMITHEME_GetFullScreenRect();
    uint16            lcd_width  = 0;
    uint16            lcd_height = 0;
    uint16            width  = 0;
    uint16            height = 0;

    uint16 nScreenWidth =fullScreenRect.right - fullScreenRect.left - 1;
    uint16 nScreenHeight = fullScreenRect.bottom - fullScreenRect.top - 1;
    static int sec_times = 0;


    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);

    SCI_TRACE_LOW("DrawAClock() enter.");
#if 0
    //aclock dial.
    aclock_input_param.clock_dial.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.clock_dial.width, &aclock_input_param.clock_dial.height,IMAGE_WATCHIDLE_ACLOCK_PANEL2/* pData->mPanelImgId */, win_id);
    aclock_input_param.clock_dial.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_PANEL2 /*pData->mPanelImgId*/,win_id,&(aclock_input_param.clock_dial.data_size));
    aclock_input_param.clock_dial.center_x = aclock_input_param.clock_dial.width/2;
    aclock_input_param.clock_dial.center_y = aclock_input_param.clock_dial.height/2;   

    //aclock hour hand
    aclock_input_param.hour_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.hour_handle.width, &aclock_input_param.hour_handle.height, IMAGE_WATCHIDLE_ACLOCK_HOUR2, win_id);    
    aclock_input_param.hour_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_HOUR2,win_id,&(aclock_input_param.hour_handle.data_size));
	aclock_input_param.hour_handle.center_x = aclock_input_param.hour_handle.width/2;
    aclock_input_param.hour_handle.center_y = aclock_input_param.hour_handle.height/2;    

    //aclock min hand
    aclock_input_param.min_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.min_handle.width, &aclock_input_param.min_handle.height, IMAGE_WATCHIDLE_ACLOCK_MIN2, win_id);        
    aclock_input_param.min_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_MIN2,win_id,&(aclock_input_param.min_handle.data_size));
    aclock_input_param.min_handle.center_x = aclock_input_param.min_handle.width/2;
    aclock_input_param.min_handle.center_y = aclock_input_param.min_handle.height/2; 

    //aclock sec hand
    aclock_input_param.sec_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.sec_handle.width, &aclock_input_param.sec_handle.height, IMAGE_WATCHIDLE_ACLOCK_SEC2, win_id);        
    aclock_input_param.sec_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_SEC2,win_id,&(aclock_input_param.sec_handle.data_size));
    aclock_input_param.sec_handle.center_x = aclock_input_param.sec_handle.width/2;
    aclock_input_param.sec_handle.center_y = aclock_input_param.sec_handle.height/2; 

    aclock_input_param.target_buf_size = aclock_input_param.clock_dial.width * aclock_input_param.clock_dial.height * 4;
    aclock_input_param.target_buf_ptr = (void*)SCI_ALLOCA(aclock_input_param.target_buf_size);

    if (PNULL == aclock_input_param.target_buf_ptr)
    {
        //SCI_TRACE_LOW:"error: DrawAClock target_buf_ptr alloc failed,  return directly"
        //SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIKL_KEYLOCK_557_112_2_18_2_31_4_6,(uint8*)"");
        return;
    }

    //get system time
    TM_GetSysTime(&sys_time);  

#ifdef WIN32
    aclock_input_param.time.hour = sys_time.hour;
    aclock_input_param.time.minute  = sys_time.min;
    aclock_input_param.time.second = (sec_times++) % 60;//sys_time.sec;
#else
    aclock_input_param.time.hour = sys_time.hour;
    aclock_input_param.time.minute  = sys_time.min;
    aclock_input_param.time.second = sys_time.sec;
#endif

#if defined(ANALOG_CLK_SUPPORT)
    #if 1//def WIN32
    {
        GRAPH_DrawClock(&aclock_input_param, &aclock_output_param);//yangyu ,now only simulator can use
    }
    #endif
#endif

    MMK_GetWinRect(win_id, &win_rect);

#if (defined UI_MULTILAYER_SUPPORT) && (!defined IDLE_MAINMENU_LOW_MEMORY_SUPPORT)
    UILAYER_GetLayerInfo(&lcd_dev_info, &layer_info);
    
    bk_img.data_ptr = UILAYER_GetLayerBufferPtr(&lcd_dev_info);
    bk_img.data_type = layer_info.data_type; 
    bk_img.width = layer_info.mem_width;
    bk_img.height = layer_info.layer_height;
#else
    GetLcdInfoByLcdDev_All(&lcd_dev_info, 
    &mem_width_ptr, 
    &bk_img.width, 
    &bk_img.height, 
    &dst_widget_type,
    &lcd_buffer_ptr);

    //bk_img.data_type = IMGREF_FORMAT_RGB565;
    bk_img.data_type = dst_widget_type;
    bk_img.data_ptr = lcd_buffer_ptr;
#endif

    if (PNULL == bk_img.data_ptr)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
	    aclock_input_param.target_buf_ptr = NULL;	
        SCI_TRACE_LOW("error: DrawAClock PNULL == bk_img.data_ptr");      
        return;
    }		    

    fore_img.data_ptr = aclock_output_param.output_ptr;
    fore_img.data_type = aclock_output_param.type;//IMGREF_FORMAT_ARGB888;
    fore_img.width = aclock_input_param.clock_dial.width;
    fore_img.height = aclock_input_param.clock_dial.height;

    if (PNULL == fore_img.data_ptr)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
	    aclock_input_param.target_buf_ptr = NULL;	
        SCI_TRACE_LOW("error: DrawAClock PNULL == fore_img.data_ptr");      
        return;
    } 

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);
    
    fore_img.start_x = MMI_MAINSCREEN_WIDTH - fore_img.width; // point.x; //(128 - aclock_input_param.clock_dial.width)/2;   
    fore_img.start_y = (MMI_MAINSCREEN_HEIGHT - fore_img.height)/2; // point.y; //top; 

    if (!ProcessAlphaBlending(&bk_img, &fore_img))
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);   
	    aclock_input_param.target_buf_ptr = NULL;	
        return;
    }
#else
{
    GUI_RECT_T dis_rect = MMITHEME_GetFullScreenRectEx(win_id);
    TM_GetSysTime(&sys_time);  
    TimeTeacherClock(win_id,
        dis_rect,
        sys_time
        IMAGE_WATCHIDLE_ACLOCK_PANEL2,
        IMAGE_WATCHIDLE_ACLOCK_HOUR2,
        IMAGE_WATCHIDLE_ACLOCK_MIN2,
        IMAGE_WATCHIDLE_ACLOCK_SEC2,
        PNULL
    );
}
#endif 

#if 1
	{
        GUI_RECT_T weak_rect = {150,130,210,163};
        uint16                      str_width           = 0;
        MMI_STRING_T                week_string            = {0};
        MMI_TEXT_ID_T week_table[] =
        {
            STXT_SUNDAY,//       TXT_SHORT_IDLE_SUNDAY,
            STXT_MONDAY,//       TXT_SHORT_IDLE_MONDAY,
            STXT_TUESDAY,//      TXT_SHORT_IDLE_TUESDAY,
            STXT_WEDNESDAY,//       TXT_SHORT_IDLE_WEDNESDAY,
            STXT_THURSDAY,//       TXT_SHORT_IDLE_THURSDAY,
            STXT_FRIDAY,//       TXT_SHORT_IDLE_FRIDAY,
            STXT_SATURDAY,//       TXT_SHORT_IDLE_SATURDAY
        };
        TM_GetSysDate(&sys_date);
        font =SONG_FONT_24;
        color = 0xffff;
			  
        sprintf(datestr,"%02d-%02d",sys_date.mon,sys_date.mday);
		
        len = strlen((char *)datestr);
        MMI_STRNTOWSTR( datewstr, 16, datestr, 16, len );
        MMIIDLE_GetIdleTextStyle(&text_style, font, color);
		
        text.wstr_len = len;
        text.wstr_ptr = datewstr;
		
        str_width = GUISTR_GetStringWidth(&text_style,&text,text_state);
		   
        point.x=(240-str_width)>>1;
        point.y=175;
			
        //text_style.begin_alpha = text_style.end_alpha = 80;
        GUISTR_DrawTextToLCDSingleLine( 
	        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
	        (const GUI_POINT_T		*)&point,
	        (const MMI_STRING_T 	*)&text,
	        &text_style,
	        text_state
	        );

	    TM_GetSysTime(&systime);
        font =SONG_FONT_32;
        color = 0xffff;
			  
        sprintf(datestr,"%02d:%02d",systime.hour,systime.min);
		
        len = strlen((char *)datestr);
        MMI_STRNTOWSTR( datewstr, 16, datestr, 16, len );
        MMIIDLE_GetIdleTextStyle(&text_style, font, color);
		
        text.wstr_len = len;
        text.wstr_ptr = datewstr;
		
        str_width = GUISTR_GetStringWidth(&text_style,&text,text_state);
		   
        point.x=(240-str_width)>>1;
        point.y=65;

        //text_style.begin_alpha = text_style.end_alpha = 80;
        GUISTR_DrawTextToLCDSingleLine( 
	        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
	        (const GUI_POINT_T		*)&point,
	        (const MMI_STRING_T 	*)&text,
	        &text_style,
	        text_state
	        );	

        text_style.font = SONG_FONT_22;
        MMI_GetLabelTextByLang(week_table[sys_date.wday], &week_string);
        week_string.wstr_len = MMIAPICOM_Wstrlen(week_string.wstr_ptr);
    
        GUISTR_DrawTextToLCDInRect(
            &lcd_dev_info,
            (const GUI_RECT_T *)&weak_rect,       //the fixed display area
            (const GUI_RECT_T *)&weak_rect,       
            (const MMI_STRING_T *)&week_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO);
	}
#endif

    MMITHEME_StoreUpdateRect(&lcd_dev_info,win_rect);

    if (aclock_input_param.target_buf_ptr != NULL)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
        aclock_input_param.target_buf_ptr = NULL;
    }
    SCI_TRACE_LOW("DrawAClock() exit.");
#endif
}
#endif
//==========================黑绿色模拟时钟表盘end=============

#ifdef MAINLCD_DEV_SIZE_240X284
//==================小鹿表盘==========================
#ifdef WATCH_PANEL_DEER_SUPPORT  
LOCAL void DrawAClock2()
{
    ///////////////////
    uint32 mem_width_ptr = 0;
    uint32 dst_widget_type = 0;
    void *lcd_buffer_ptr = NULL;
    MMI_WIN_ID_T win_id = WATCH_LAUNCHER_PANEL_WIN_ID;
	
    ////////////////////

//#ifdef UI_MULTILAYER_SUPPORT
    GUI_RECT_T        win_rect = {0};
    GUI_LCD_DEV_INFO  lcd_dev_info = {0, 0};
    SCI_TIME_T        sys_time        = {0};
    GUI_POINT_T       point = {0};  
    CLOCK_DRAW_IN_T   aclock_input_param  = {0};
    CLOCK_DRAW_OUT_T  aclock_output_param = {0};
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T bk_img = {0};
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T fore_img = {0};

//#undef UI_MULTILAYER_SUPPORT
#if (defined UI_MULTILAYER_SUPPORT) && (!defined IDLE_MAINMENU_LOW_MEMORY_SUPPORT)
    UILAYER_INFO_T    layer_info = {0};
#endif

    GUI_RECT_T fullScreenRect = MMITHEME_GetFullScreenRect();
    uint16            lcd_width  = 0;
    uint16            lcd_height = 0;
    uint16            width  = 0;
    uint16            height = 0;

    uint16 nScreenWidth =fullScreenRect.right - fullScreenRect.left - 1;
    uint16 nScreenHeight = fullScreenRect.bottom - fullScreenRect.top - 1;
    //lzk begin. 2018.04.12
    //add pure dark bg
    static int sec_times = 0;
#if 0
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    
    //aclock dial.

    aclock_input_param.clock_dial.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.clock_dial.width, &aclock_input_param.clock_dial.height, IMAGE_WATCHIDLE_DEER, win_id);
    aclock_input_param.clock_dial.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_DEER,win_id,&(aclock_input_param.clock_dial.data_size));
    aclock_input_param.clock_dial.center_x = aclock_input_param.clock_dial.width/2;
    aclock_input_param.clock_dial.center_y = aclock_input_param.clock_dial.height/2;   


    //aclock hour hand
    aclock_input_param.hour_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.hour_handle.width, &aclock_input_param.hour_handle.height, IMAGE_WATCHIDLE_ACLOCK_HOUR3, win_id);    
    aclock_input_param.hour_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_HOUR3,win_id,&(aclock_input_param.hour_handle.data_size));
	aclock_input_param.hour_handle.center_x = aclock_input_param.hour_handle.width/2;
    aclock_input_param.hour_handle.center_y = aclock_input_param.hour_handle.height/2;    

    //aclock min hand
    aclock_input_param.min_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.min_handle.width, &aclock_input_param.min_handle.height, IMAGE_WATCHIDLE_ACLOCK_MIN3, win_id);        
    aclock_input_param.min_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_MIN3,win_id,&(aclock_input_param.min_handle.data_size));
    aclock_input_param.min_handle.center_x = aclock_input_param.min_handle.width/2;
    aclock_input_param.min_handle.center_y = aclock_input_param.min_handle.height/2; 

    //aclock sec hand
    aclock_input_param.sec_handle.data_type = IMGREF_FORMAT_COMPRESSED_ARGB;
    GUIRES_GetImgWidthHeight(&aclock_input_param.sec_handle.width, &aclock_input_param.sec_handle.height, IMAGE_WATCHIDLE_ACLOCK_SEC3, win_id);        
    aclock_input_param.sec_handle.data_ptr = (uint8 *)MMI_GetLabelImage(IMAGE_WATCHIDLE_ACLOCK_SEC3,win_id,&(aclock_input_param.sec_handle.data_size));
    aclock_input_param.sec_handle.center_x = aclock_input_param.sec_handle.width/2;
    aclock_input_param.sec_handle.center_y = aclock_input_param.sec_handle.height/2; 

    aclock_input_param.target_buf_size = aclock_input_param.clock_dial.width * aclock_input_param.clock_dial.height * 4;

    aclock_input_param.target_buf_ptr = s_static_z_mem_ptr;
    //aclock_input_param.target_buf_ptr = (void*)SCI_ALLOCA(aclock_input_param.target_buf_size);

    if (PNULL == aclock_input_param.target_buf_ptr)
    {
        return;
    }

    //get system time
    TM_GetSysTime(&sys_time);  

#ifdef WIN32
    aclock_input_param.time.hour = sys_time.hour;
    aclock_input_param.time.minute  = sys_time.min;
    aclock_input_param.time.second = (sec_times++) % 60;//sys_time.sec;
#else
    aclock_input_param.time.hour = sys_time.hour;
    aclock_input_param.time.minute  = sys_time.min;
    aclock_input_param.time.second = sys_time.sec;
#endif

#if defined(ANALOG_CLK_SUPPORT)
    #if 1//def WIN32
    {
        GRAPH_DrawClock(&aclock_input_param, &aclock_output_param);//yangyu ,now only simulator can use
    }
    #endif
#endif

    //display aclock
    MMK_GetWinRect(win_id, &win_rect);


#if (defined UI_MULTILAYER_SUPPORT) && (!defined IDLE_MAINMENU_LOW_MEMORY_SUPPORT)
    UILAYER_GetLayerInfo(&lcd_dev_info, &layer_info);
    
    bk_img.data_ptr = UILAYER_GetLayerBufferPtr(&lcd_dev_info);
    bk_img.data_type = layer_info.data_type; 
    bk_img.width = layer_info.mem_width;
    bk_img.height = layer_info.layer_height;
#else
    GetLcdInfoByLcdDev_All(&lcd_dev_info, 
    &mem_width_ptr, 
    &bk_img.width, 
    &bk_img.height, 
    &dst_widget_type,
    &lcd_buffer_ptr);

    //bk_img.data_type = IMGREF_FORMAT_RGB565;
    bk_img.data_type = dst_widget_type;
    bk_img.data_ptr = lcd_buffer_ptr;
#endif
	
    if (PNULL == bk_img.data_ptr)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
	    aclock_input_param.target_buf_ptr = NULL;	
        SCI_TRACE_LOW("error: DrawAClock PNULL == bk_img.data_ptr");      
        return;
    }		

    fore_img.data_ptr = aclock_output_param.output_ptr;
    fore_img.data_type = aclock_output_param.type;//IMGREF_FORMAT_ARGB888;
    fore_img.width = aclock_input_param.clock_dial.width;
    fore_img.height = aclock_input_param.clock_dial.height;

    if (PNULL == fore_img.data_ptr)
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);
	    aclock_input_param.target_buf_ptr = NULL;	
        SCI_TRACE_LOW("error: DrawAClock PNULL == fore_img.data_ptr");      
        return;
    }   

    GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID,&lcd_width,&lcd_height);
    
    //fore_img.start_x = (lcd_width - aclock_input_param.clock_dial.width)/2;   
    //fore_img.start_y = top; 
    fore_img.start_x = MMI_MAINSCREEN_WIDTH - fore_img.width; // point.x; //(128 - aclock_input_param.clock_dial.width)/2;   
    fore_img.start_y = (MMI_MAINSCREEN_HEIGHT - fore_img.height)/2; // point.y; //top; 

	//memcpy(bk_img.data_ptr, fore_img.data_ptr, fore_img.width * fore_img.height * 3);

    if (!ProcessAlphaBlending(&bk_img, &fore_img))
    {
        SCI_FREE(aclock_input_param.target_buf_ptr);   
	    aclock_input_param.target_buf_ptr = NULL;	
        return;
    }

    MMITHEME_StoreUpdateRect(&lcd_dev_info,win_rect);
#else
{
    GUI_RECT_T dis_rect = MMITHEME_GetFullScreenRectEx(win_id);
    TM_GetSysTime(&sys_time);  
    TimeTeacherClock(win_id,
        dis_rect,
        sys_time,
        IMAGE_WATCHIDLE_DEER,
        IMAGE_WATCHIDLE_ACLOCK_HOUR3,
        IMAGE_WATCHIDLE_ACLOCK_MIN3,
        IMAGE_WATCHIDLE_ACLOCK_SEC3,
        PNULL
    );

    
}
#endif 
    // if (aclock_input_param.target_buf_ptr != NULL)
    // {
    //     SCI_FREE(aclock_input_param.target_buf_ptr);
    //     aclock_input_param.target_buf_ptr = NULL;
    // }

    {
        GUI_RECT_T weak_rect = {22,30,75,47};
        GUI_RECT_T date_rect = {22,50,75,77};
        uint16                      str_width           = 0;
        MMI_STRING_T                week_string            = {0};
        uint8						datestr[16] 		= {0};
	    wchar						datewstr[16]		= {0};
	    SCI_DATE_T                  sys_date = {0};
	    uint16						len 				= {0}; 
	    GUI_FONT_T					font				= SONG_FONT_8;
	    GUI_COLOR_T 				color				= 0;
	    GUISTR_STYLE_T			text_style = {0};/*lint !e64*/
	    GUISTR_STATE_T			text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_EFFECT|GUISTR_STATE_ELLIPSIS;
	    MMI_STRING_T			text   = {0};
        MMI_TEXT_ID_T week_table[] =
        {
            STXT_SUNDAY,//       TXT_SHORT_IDLE_SUNDAY,
            STXT_MONDAY,//       TXT_SHORT_IDLE_MONDAY,
            STXT_TUESDAY,//      TXT_SHORT_IDLE_TUESDAY,
            STXT_WEDNESDAY,//       TXT_SHORT_IDLE_WEDNESDAY,
            STXT_THURSDAY,//       TXT_SHORT_IDLE_THURSDAY,
            STXT_FRIDAY,//       TXT_SHORT_IDLE_FRIDAY,
            STXT_SATURDAY,//       TXT_SHORT_IDLE_SATURDAY
        };
        TM_GetSysDate(&sys_date);
        text_style.font = SONG_FONT_22;
        text_style.font_color = MMI_WHITE_COLOR;
        text_style.align = ALIGN_LVMIDDLE;
			  
        sprintf(datestr,"%02d-%02d",sys_date.mon,sys_date.mday);
		
        len = strlen((char *)datestr);
        MMI_STRNTOWSTR( datewstr, 16, datestr, 16, len );		
        text.wstr_len = len;
        text.wstr_ptr = datewstr;
	

        GUISTR_DrawTextToLCDInRect(
            &lcd_dev_info,
            (const GUI_RECT_T *)&date_rect,       //the fixed display area
            (const GUI_RECT_T *)&date_rect,       
            (const MMI_STRING_T *)&text,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO);

        MMI_GetLabelTextByLang(week_table[sys_date.wday], &week_string);
        week_string.wstr_len = MMIAPICOM_Wstrlen(week_string.wstr_ptr);
        text_style.font = SONG_FONT_20;
    
        GUISTR_DrawTextToLCDInRect(
            &lcd_dev_info,
            (const GUI_RECT_T *)&weak_rect,       //the fixed display area
            (const GUI_RECT_T *)&weak_rect,       
            (const MMI_STRING_T *)&week_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO);
	}

    SCI_TRACE_LOW("DrawAClock2() exit.");
}
#endif
//=======================小鹿表盘end=============================
#endif

#ifdef ZTE_WATCH

#define LOW_BATTERY_LEVEL 20 //低电量

#ifdef MAINLCD_DEV_SIZE_240X284 //240*284 分辨率
#define SPACEMAN_IMG_START_POINT {3,20}
#define SPACEMAN_BATTERY_BG_IMG_START_Y 240
#define SPACEMAN_BATTERY_FILL_START_Y 249
#define SPACEMAN_DATE_IMG_START_Y 244
#define SPACEMAN_TIME_IMG_START_Y 195
#define SPACEMAN_DATE_SEPERATOR_IMG_START_Y 246
#define SPACEMAN_TIME_SEPERATOR_IMG_START_Y 204
#define SPACEMAN_WEEK_POINT {110,242}
#define ROCKET_IMG_START_POINT {11,69} 
#define ROCKET_BATTERY_BG_IMG_START_Y 58
#define ROCKET_BATTERY_FILL_START_Y 67
#define ROCKET_DATE_IMG_START_Y 64
#define ROCKET_TIME_IMG_START_Y 10
#define ROCKET_DATE_SEPERATOR_IMG_START_Y 64
#define ROCKET_TIME_SEPERATOR_IMG_START_Y 23
#define CAT_IMG_START_POINT {55,7}
#define CAT_BATTERY_BG_IMG_START_Y 244
#define CAT_BATTERY_FILL_START_Y 249
#define CAT_DATE_IMG_START_Y 244
#define CAT_TIME_IMG_START_Y 190
#define CAT_DATE_SEPERATOR_IMG_START_Y 244
#define CAT_TIME_SEPERATOR_IMG_START_Y 202
#define CAT_WEEK_POINT {120,244}
#define CAT_AM_PM_IMG_START_X 102
#define CAT_AM_PM_IMG_START_Y 121
#else //240*240 分辨率
#define SPACEMAN_IMG_START_POINT {24,15} 
#define SPACEMAN_BATTERY_BG_IMG_START_Y 205
#define SPACEMAN_BATTERY_FILL_START_Y 214
#define SPACEMAN_DATE_IMG_START_Y 209
#define SPACEMAN_TIME_IMG_START_Y 160
#define SPACEMAN_DATE_SEPERATOR_IMG_START_Y 211
#define SPACEMAN_TIME_SEPERATOR_IMG_START_Y 169
#define SPACEMAN_WEEK_POINT {110,207}
#define ROCKET_IMG_START_POINT {35,83}
#define ROCKET_BATTERY_BG_IMG_START_Y 58
#define ROCKET_BATTERY_FILL_START_Y 67
#define ROCKET_DATE_IMG_START_Y 64
#define ROCKET_TIME_IMG_START_Y 10
#define ROCKET_DATE_SEPERATOR_IMG_START_Y 64
#define ROCKET_TIME_SEPERATOR_IMG_START_Y 23
#define CAT_IMG_START_POINT {67,7} 
#define CAT_BATTERY_BG_IMG_START_Y 210
#define CAT_BATTERY_FILL_START_Y 215
#define CAT_DATE_IMG_START_Y 212
#define CAT_TIME_IMG_START_Y 156
#define CAT_DATE_SEPERATOR_IMG_START_Y 212
#define CAT_TIME_SEPERATOR_IMG_START_Y 169
#define CAT_WEEK_POINT {120,212}
#define CAT_AM_PM_IMG_START_X 102
#define CAT_AM_PM_IMG_START_Y 100
#endif
#define SPACEMAN_IMG_MESSAGE_POINT {108,2}
#define SPACEMAN_BATTERY_BG_IMG_START_X 180
#define SPACEMAN_BATTERY_FILL_START_X 188
#define SPACEMAN_BATTERY_FILL_HEIGHT 10
#define SPACEMAN_BATTERY_FILL_WIDTH 26
#define SPACEMAN_BATTERY_FILL_COLOR GUI_RGB2RGB565(128,195,255)
#define SPACEMAN_BATTERY_LOW_FILL_COLOR GUI_RGB2RGB565(255,0,0)

#define SPACEMAN_DATE_IMG_OFFSET 19
#define SPACEMAN_DATE_IMG_START_X 20
#define SPACEMAN_TIME_IMG_OFFSET 45
#define SPACEMAN_TIME_IMG_START_X 23

#define SPACEMAN_DATE_SEPERATOR_IMG_OFFSET 11
#define SPACEMAN_DATE_SEPERATOR_IMG_START_X 59
#define SPACEMAN_TIME_SEPERATOR_IMG_OFFSET 18
#define SPACEMAN_TIME_SEPERATOR_IMG_START_X 112
#define SPACEMAN_AM_PM_IMG_START_X 153
#define SPACEMAN_AM_PM_IMG_START_Y 91
//spaceman define end

//rocket define start
#define ROCKET_IMG_MESSAGE_POINT {108,2}
#define ROCKET_BATTERY_BG_IMG_START_X 167
#define ROCKET_BATTERY_FILL_START_X 173
#define ROCKET_BATTERY_FILL_HEIGHT 10
#define ROCKET_BATTERY_FILL_WIDTH 26
#define ROCKET_WEEK_POINT {110,64}
#define ROCKET_BATTERY_FILL_COLOR GUI_RGB2RGB565(0,227,53)
#define ROCKET_BATTERY_LOW_FILL_COLOR GUI_RGB2RGB565(255,0,0)

#define ROCKET_DATE_IMG_OFFSET 12
#define ROCKET_DATE_IMG_START_X 32
#define ROCKET_TIME_IMG_OFFSET 42
#define ROCKET_TIME_IMG_START_X 30

#define ROCKET_DATE_SEPERATOR_IMG_OFFSET 11
#define ROCKET_DATE_SEPERATOR_IMG_START_X 62
#define ROCKET_TIME_SEPERATOR_IMG_OFFSET 18
#define ROCKET_TIME_SEPERATOR_IMG_START_X 114
#define ROCKET_AM_PM_IMG_START_X 105
#define ROCKET_AM_PM_IMG_START_Y 33

#define ROCKET_BG_COLOR GUI_RGB2RGB565(28,12,37)
// rocket define end

//cat  define start 粉色猫表盘
#define CAT_IMG_MESSAGE_POINT {108,2}
#define CAT_BATTERY_BG_IMG_START_X 174
#define CAT_BATTERY_FILL_START_X 178
#define CAT_BATTERY_FILL_HEIGHT 10
#define CAT_BATTERY_FILL_WIDTH 26
#define CAT_BATTERY_FILL_COLOR GUI_RGB2RGB565(128,195,255)
#define CAT_BATTERY_LOW_FILL_COLOR GUI_RGB2RGB565(255,0,0)

#define CAT_DATE_IMG_OFFSET 19
#define CAT_DATE_IMG_START_X 40
#define CAT_TIME_IMG_OFFSET 39
#define CAT_TIME_IMG_START_X 38

#define CAT_DATE_SEPERATOR_IMG_OFFSET 11
#define CAT_DATE_SEPERATOR_IMG_START_X 72
#define CAT_TIME_SEPERATOR_IMG_OFFSET 18
#define CAT_TIME_SEPERATOR_IMG_START_X 116

#define CAT_BATTERY_FILL_COLOR GUI_RGB2RGB565(0xfe,0xa5,0xda)
#define CAT_BATTERY_LOW_FILL_COLOR GUI_RGB2RGB565(255,0,0)

//cat define end

#define MESSAGE_TIP_IMG_WIDTH 30
#define MESSAGE_TIP_IMG_HEIGHT 30

typedef struct
{
    GUI_POINT_T point; //图片显示起点
    MMI_IMAGE_ID_T am_img_id; //上午图片id
    MMI_IMAGE_ID_T pm_img_id; //下午图片id
}TWELVE_HOURS_TIME_T; //12小时

typedef struct
{
    uint8 offset; //图片宽度+空隙
    GUI_POINT_T point; //图片显示起点
    MMI_IMAGE_ID_T img_id; //图片id

}DRAW_DATE_IMAGE_T; //时间/日期之间的分割符

typedef struct
{
    uint8 battery_width;//填充电量宽
    uint8 battery_height;//填充电量高
    GUI_POINT_T bg_point; //电池图片显示起点
    GUI_POINT_T battery_point; //填充电量显示起点
    GUI_COLOR_T battery_color; //填充电量的颜色
    GUI_COLOR_T low_battery_color; //低电填充电量的颜色
    MMI_IMAGE_ID_T bg_img_id; //图片id
    MMI_IMAGE_ID_T bg_low_img_id; //低电图片id

}DRAW_BATTERY_IMAGE_T; //时间 日期 分割符

LOCAL void Message_Tip_Click(GUI_POINT_T point,GUI_POINT_T message_tip_point)
{
    GUI_RECT_T message_tip_rect;
    message_tip_rect.top = message_tip_point.y;
    message_tip_rect.left = message_tip_point.x;
    message_tip_rect.right = message_tip_rect.left + MESSAGE_TIP_IMG_WIDTH;
    message_tip_rect.bottom = message_tip_rect.top + MESSAGE_TIP_IMG_HEIGHT;
    if( GUI_PointIsInRect(point,message_tip_rect) && YX_VCHAT_GetAllGroupUnread() > 0)
    {
        MMIZDT_OpenChatGroupWin();
    }
}

LOCAL void Panel_TP_UP(DPARAM  param)
{
    WATCH_PANEL_TYPE_T style = MMISET_GetWatchPanelStyle();
    PANEL_INDEX_E index = style.watch_index;
    if(index == ZTE_PANEL_SPACEMAN || index == ZTE_PANEL_ROCKET || index == ZTE_PANEL_CAT)
    {
        GUI_POINT_T point;
        point.x = MMK_GET_TP_X(param);
        point.y = MMK_GET_TP_Y(param);
        if(index == ZTE_PANEL_SPACEMAN)
        {
            GUI_POINT_T message_tip_point = SPACEMAN_IMG_MESSAGE_POINT;
            Message_Tip_Click(point,message_tip_point);
        }
        else if(index == ZTE_PANEL_ROCKET)
        {
            GUI_POINT_T message_tip_point = ROCKET_IMG_MESSAGE_POINT;
            Message_Tip_Click(point,message_tip_point);
        }  
        else if(index == ZTE_PANEL_CAT)
        {
            GUI_POINT_T message_tip_point = CAT_IMG_MESSAGE_POINT;
            Message_Tip_Click(point,message_tip_point);
        } 
    }
}

LOCAL void Draw_messageImg(GUI_POINT_T point,MMI_IMAGE_ID_T message_img_id,MMI_WIN_ID_T win_id)
{
    if(YX_VCHAT_GetAllGroupUnread() > 0)
    {
        GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
        MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, message_img_id,&lcd_dev_info);
    }
}

LOCAL void Draw_WeekImg(GUI_POINT_T point,MMI_IMAGE_ID_T week_img_id[],MMI_WIN_ID_T win_id)
{
    //draw week
    SCI_DATE_T date = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    TM_GetSysDate(&date);
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, week_img_id[date.wday],&lcd_dev_info);
}

LOCAL void Draw_Battery(DRAW_BATTERY_IMAGE_T *draw_battery_t,MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    uint8 battery = 0;
    GUI_RECT_T battery_rect = {0};
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    battery = ZDT_GetBatteryPercent();
    battery_rect.left = draw_battery_t->battery_point.x;
    battery_rect.top = draw_battery_t->battery_point.y;
    battery_rect.bottom = battery_rect.top + draw_battery_t->battery_height;
    battery_rect.right = battery_rect.left + (draw_battery_t->battery_width*battery)/100;
    
    if(battery < LOW_BATTERY_LEVEL)
    {
        GUIRES_DisplayImg(&(draw_battery_t->bg_point),PNULL,PNULL,win_id, draw_battery_t->bg_low_img_id,&lcd_dev_info);
        LCD_FillRect(&lcd_dev_info, battery_rect, draw_battery_t->low_battery_color);
    }
    else
    {
        GUIRES_DisplayImg(&(draw_battery_t->bg_point),PNULL,PNULL,win_id, draw_battery_t->bg_img_id,&lcd_dev_info);
        LCD_FillRect(&lcd_dev_info, battery_rect, draw_battery_t->battery_color);
    }
}

LOCAL void Draw_TimeAmOrPmFlag(TWELVE_HOURS_TIME_T twelve_hours_time, MMI_WIN_ID_T win_id)
{
    if(MMIAPISET_GetTimeDisplayType() == MMISET_TIME_12HOURS)
    {
        SCI_TIME_T  time   = {0};
        MMI_IMAGE_ID_T img_id = twelve_hours_time.am_img_id;
        GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
        TM_GetSysTime(&time);
        MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
        if(time.hour > 11)
        {
            img_id = twelve_hours_time.pm_img_id;
        }
        GUIRES_DisplayImg(&twelve_hours_time.point,PNULL,PNULL,win_id, img_id, &lcd_dev_info);
    }
}

LOCAL void Draw_DateImg(DRAW_DATE_IMAGE_T *draw_date_t,DRAW_DATE_IMAGE_T *draw_time_t,DRAW_DATE_IMAGE_T date_seperator_t,DRAW_DATE_IMAGE_T time_seperator_t,MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    if(PNULL != draw_time_t)
    {
        SCI_TIME_T  time   = {0};
        MMI_IMAGE_ID_T zero_img_id = draw_time_t->img_id;
        TM_GetSysTime(&time);
        if(MMIAPISET_GetTimeDisplayType() == MMISET_TIME_12HOURS)
        {
            if(time.hour > 12)
            {
                time.hour = time.hour-12;
            }
        }
        GUIRES_DisplayImg(&draw_time_t->point,PNULL,PNULL,win_id, zero_img_id + time.hour/10,&lcd_dev_info);
        draw_time_t->point.x += draw_time_t->offset;
        GUIRES_DisplayImg(&draw_time_t->point,PNULL,PNULL,win_id, zero_img_id + time.hour%10,&lcd_dev_info);

        GUIRES_DisplayImg(&time_seperator_t.point,PNULL,PNULL,win_id, time_seperator_t.img_id,&lcd_dev_info);
        draw_time_t->point.x = time_seperator_t.point.x + time_seperator_t.offset;

        GUIRES_DisplayImg(&draw_time_t->point,PNULL,PNULL,win_id, zero_img_id + time.min/10,&lcd_dev_info);
        draw_time_t->point.x += draw_time_t->offset;
        GUIRES_DisplayImg(&draw_time_t->point,PNULL,PNULL,win_id, zero_img_id + time.min%10,&lcd_dev_info);
    }
    if(PNULL != draw_date_t)
    {
        SCI_DATE_T  date = {0};
        MMI_IMAGE_ID_T zero_img_id = draw_date_t->img_id;
        TM_GetSysDate(&date);

        GUIRES_DisplayImg(&draw_date_t->point,PNULL,PNULL,win_id, zero_img_id + date.mon/10,&lcd_dev_info);
        draw_date_t->point.x += draw_date_t->offset;
        GUIRES_DisplayImg(&draw_date_t->point,PNULL,PNULL,win_id, zero_img_id + date.mon%10,&lcd_dev_info);

        GUIRES_DisplayImg(&date_seperator_t.point,PNULL,PNULL,win_id, date_seperator_t.img_id,&lcd_dev_info);
        draw_date_t->point.x = date_seperator_t.point.x + date_seperator_t.offset;

        GUIRES_DisplayImg(&draw_date_t->point,PNULL,PNULL,win_id, zero_img_id + date.mday/10,&lcd_dev_info);
        draw_date_t->point.x += draw_date_t->offset;
        GUIRES_DisplayImg(&draw_date_t->point,PNULL,PNULL,win_id, zero_img_id + date.mday%10,&lcd_dev_info);
    }
}

// spaceman ZTE 宇航员
LOCAL void ZTE_Draw_Spaceman_Panel(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_POINT_T point = SPACEMAN_IMG_START_POINT;
    GUI_POINT_T message_point = SPACEMAN_IMG_MESSAGE_POINT;
    DRAW_BATTERY_IMAGE_T draw_battery_t = {SPACEMAN_BATTERY_FILL_WIDTH,SPACEMAN_BATTERY_FILL_HEIGHT
        ,SPACEMAN_BATTERY_BG_IMG_START_X,SPACEMAN_BATTERY_BG_IMG_START_Y
        ,SPACEMAN_BATTERY_FILL_START_X,SPACEMAN_BATTERY_FILL_START_Y,0,0,panel_spaceman_battery_bg, panel_spaceman_battery_low_bg};
    GUI_POINT_T week_point = SPACEMAN_WEEK_POINT;
    MMI_TEXT_ID_T week_img_id[7] =
    {
        panel_spaceman_sunday,
        panel_spaceman_monday,
        panel_spaceman_tuesday,
        panel_spaceman_wednesday,
        panel_spaceman_thursday,
        panel_spaceman_friday,
        panel_spaceman_saturday,
    };
    DRAW_DATE_IMAGE_T draw_date_t = {SPACEMAN_DATE_IMG_OFFSET, SPACEMAN_DATE_IMG_START_X,SPACEMAN_DATE_IMG_START_Y, panel_spaceman_num0};
    DRAW_DATE_IMAGE_T draw_time_t = {SPACEMAN_TIME_IMG_OFFSET, SPACEMAN_TIME_IMG_START_X,SPACEMAN_TIME_IMG_START_Y, panel_spaceman_num0_big};
    DRAW_DATE_IMAGE_T date_seperator_t = {SPACEMAN_DATE_SEPERATOR_IMG_OFFSET, SPACEMAN_DATE_SEPERATOR_IMG_START_X,SPACEMAN_DATE_SEPERATOR_IMG_START_Y, panel_spaceman_date_seperator};
    DRAW_DATE_IMAGE_T time_seperator_t = {SPACEMAN_TIME_SEPERATOR_IMG_OFFSET, SPACEMAN_TIME_SEPERATOR_IMG_START_X,SPACEMAN_TIME_SEPERATOR_IMG_START_Y, panel_spaceman_time_seperator};
    TWELVE_HOURS_TIME_T twelve_hours_time = {SPACEMAN_AM_PM_IMG_START_X, SPACEMAN_AM_PM_IMG_START_Y, panel_spaceman_am, panel_spaceman_pm};
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, panel_spaceman_spaceman,&lcd_dev_info);
    Draw_DateImg(&draw_date_t,&draw_time_t,date_seperator_t,time_seperator_t,win_id);
    Draw_WeekImg(week_point,week_img_id,win_id);
    draw_battery_t.battery_color = SPACEMAN_BATTERY_FILL_COLOR;
    draw_battery_t.low_battery_color = SPACEMAN_BATTERY_LOW_FILL_COLOR;
    Draw_Battery(&draw_battery_t,win_id);
    Draw_messageImg(message_point,panel_message_tip,win_id);
    Draw_TimeAmOrPmFlag(twelve_hours_time,win_id);
}
//  end astronaut ZTE 宇航员

// rocket ZTE 火箭
LOCAL void ZTE_Draw_Rocket_Panel(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T win_rect;
    GUI_POINT_T point = ROCKET_IMG_START_POINT;
    GUI_POINT_T message_point = ROCKET_IMG_MESSAGE_POINT;
    DRAW_BATTERY_IMAGE_T draw_battery_t = {ROCKET_BATTERY_FILL_WIDTH,ROCKET_BATTERY_FILL_HEIGHT
        ,ROCKET_BATTERY_BG_IMG_START_X,ROCKET_BATTERY_BG_IMG_START_Y,ROCKET_BATTERY_FILL_START_X
        ,ROCKET_BATTERY_FILL_START_Y,0,0,panel_rocket_battery_bg, panel_rocket_battery_low_bg};
    GUI_POINT_T week_point = ROCKET_WEEK_POINT;
    MMI_TEXT_ID_T week_img_id[7] =
    {
        panel_rocket_sunday,
        panel_rocket_monday,
        panel_rocket_tuesday,
        panel_rocket_wednesday,
        panel_rocket_thursday,
        panel_rocket_friday,
        panel_rocket_saturday,
    };
    DRAW_DATE_IMAGE_T draw_date_t = {ROCKET_DATE_IMG_OFFSET, ROCKET_DATE_IMG_START_X,ROCKET_DATE_IMG_START_Y, panel_rocket_num0};
    DRAW_DATE_IMAGE_T draw_time_t = {ROCKET_TIME_IMG_OFFSET, ROCKET_TIME_IMG_START_X,ROCKET_TIME_IMG_START_Y, panel_rocket_num0_big};
    DRAW_DATE_IMAGE_T date_seperator_t = {ROCKET_DATE_SEPERATOR_IMG_OFFSET, ROCKET_DATE_SEPERATOR_IMG_START_X,ROCKET_DATE_SEPERATOR_IMG_START_Y, panel_rocket_date_seperator};
    DRAW_DATE_IMAGE_T time_seperator_t = {ROCKET_TIME_SEPERATOR_IMG_OFFSET, ROCKET_TIME_SEPERATOR_IMG_START_X,ROCKET_TIME_SEPERATOR_IMG_START_Y, panel_rocket_time_seperator};
    TWELVE_HOURS_TIME_T twelve_hours_time = {ROCKET_AM_PM_IMG_START_X,ROCKET_AM_PM_IMG_START_Y, panel_rocket_am, panel_rocket_pm};
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    win_rect = MMITHEME_GetFullScreenRect();
    LCD_FillRect(&lcd_dev_info, win_rect, ROCKET_BG_COLOR);
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, panel_rocket_rocket,&lcd_dev_info);
    Draw_DateImg(&draw_date_t,&draw_time_t,date_seperator_t,time_seperator_t,win_id);
    Draw_WeekImg(week_point,week_img_id,win_id);
    draw_battery_t.battery_color = ROCKET_BATTERY_FILL_COLOR;
    draw_battery_t.low_battery_color = ROCKET_BATTERY_LOW_FILL_COLOR;
    Draw_Battery(&draw_battery_t,win_id);
    Draw_messageImg(message_point,panel_message_tip,win_id);
    Draw_TimeAmOrPmFlag(twelve_hours_time,win_id);
}
//  end rocket ZTE 火箭

LOCAL void ZTE_Draw_Cat_Panel(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T win_rect;
    GUI_POINT_T point = CAT_IMG_START_POINT;
    GUI_POINT_T message_point = CAT_IMG_MESSAGE_POINT;
    DRAW_BATTERY_IMAGE_T draw_battery_t = {CAT_BATTERY_FILL_WIDTH,CAT_BATTERY_FILL_HEIGHT
        ,CAT_BATTERY_BG_IMG_START_X,CAT_BATTERY_BG_IMG_START_Y,CAT_BATTERY_FILL_START_X
        ,CAT_BATTERY_FILL_START_Y,0,0,panel_cat_battery_bg, panel_cat_battery_low_bg};
    GUI_POINT_T week_point = CAT_WEEK_POINT;
    MMI_TEXT_ID_T week_img_id[7] =
    {
        panel_cat_sunday,
        panel_cat_monday,
        panel_cat_tuesday,
        panel_cat_wednesday,
        panel_cat_thursday,
        panel_cat_friday,
        panel_cat_saturday,
    };
    DRAW_DATE_IMAGE_T draw_date_t = {CAT_DATE_IMG_OFFSET, CAT_DATE_IMG_START_X,CAT_DATE_IMG_START_Y, panel_cat_num0};
    DRAW_DATE_IMAGE_T draw_time_t = {CAT_TIME_IMG_OFFSET, CAT_TIME_IMG_START_X,CAT_TIME_IMG_START_Y, panel_cat_num0_big};
    DRAW_DATE_IMAGE_T date_seperator_t = {CAT_DATE_SEPERATOR_IMG_OFFSET, CAT_DATE_SEPERATOR_IMG_START_X,CAT_DATE_SEPERATOR_IMG_START_Y, panel_cat_date_seperator};
    DRAW_DATE_IMAGE_T time_seperator_t = {CAT_TIME_SEPERATOR_IMG_OFFSET, CAT_TIME_SEPERATOR_IMG_START_X,CAT_TIME_SEPERATOR_IMG_START_Y, panel_cat_time_seperator};
    TWELVE_HOURS_TIME_T twelve_hours_time = {CAT_AM_PM_IMG_START_X,CAT_AM_PM_IMG_START_Y, panel_cat_am, panel_cat_pm};
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, panel_cat_cat,&lcd_dev_info);
    Draw_DateImg(&draw_date_t,&draw_time_t,date_seperator_t,time_seperator_t,win_id);
    Draw_WeekImg(week_point,week_img_id,win_id);
    draw_battery_t.battery_color = CAT_BATTERY_FILL_COLOR;
    draw_battery_t.low_battery_color = CAT_BATTERY_LOW_FILL_COLOR;
    Draw_Battery(&draw_battery_t,win_id);
    Draw_messageImg(message_point,panel_message_tip,win_id);
    Draw_TimeAmOrPmFlag(twelve_hours_time,win_id);
}

#endif //end ZTE WATCH

LOCAL void WatchPanel_CreateWin(const MMI_WIN_ID_T win_id)
{
    uint32 win_table_create[10] = 
    {
        MMK_HIDE_STATUSBAR,
        MMK_WINFUNC, HandlePanelSelectWinMsg,
        MMK_WINID, 0,
        MMK_WINDOW_STYLE, (WS_DISPATCH_TO_CHILDWIN |WS_DISABLE_RETURN_WIN),
        MMK_WINDOW_ANIM_MOVE_SYTLE, (MOVE_FORBIDDEN),
        MMK_END_WIN,
    };
    win_table_create[4] = win_id;
    MMK_CreateWin(win_table_create, NULL);
}

typedef void (*DRAWHANDLE)(MMI_WIN_ID_T win_id);
typedef void (*PANEL_PREVIEW_WIN)(void);

typedef struct
{
    PANEL_INDEX_E index;    //表盘序号
    BOOLEAN is_anolog_watch;//是否是模拟时钟表盘
    MMI_IMAGE_ID_T img_id; //preview图片ID
    MMI_WIN_ID_T   win_id; //preview窗口ID
    DRAWHANDLE draw_handle; //表盘绘制入口

}PANEL_SELECT_INFO;

typedef struct
{
    PANEL_INDEX_E index_id;
}PANEL_INDEX_ID_INFO;

LOCAL MMI_HANDLE_T s_panel_select_handle;

LOCAL PANEL_SELECT_INFO panel_select_page_list[] = 
{
#ifdef WATCH_PANEL_SPACE_STATION_SUPPORT //太空站宇航员
    #ifdef MAINLCD_DEV_SIZE_240X284
    {PANEL_SPACE_STATION, FALSE, PANEL_SAMPLE_7, WATCH_LAUNCHER_PANEL_SPACE_STATION_WIN_ID,drawDigitalWatch3},
    #endif
#endif
#ifdef WATCH_PANEL_SPACE_SHIP_SUPPORT //宇宙飞船数字表盘
    {PANEL_SPACE_SHIP, FALSE, PANEL_SAMPLE_1, WATCH_LAUNCHER_PANEL_SPACE_SHIP_WIN_ID,drawDigitalWatch0},
#endif
#ifdef WATCH_PANEL_DOLPHIN_SUPPORT //海豚
    {PANEL_DOLPHIN, FALSE, PANEL_SAMPLE_2, WATCH_LAUNCHER_PANEL_DOLPHIN_WIN_ID,drawDigitalWatch1},
#endif
#ifdef WATCH_PANEL_ASTRONAUT_SUPPORT //太空人
    {PANEL_ASTRONAUT, FALSE, PANEL_SAMPLE_3, WATCH_LAUNCHER_PANEL_ASTRONAUT_WIN_ID,drawDigitalWatch2},
#endif
#ifdef ANALOG_CLK_SUPPORT    //   模拟时钟
#ifdef WATCH_PANEL_PANDA_SUPPORT //熊猫
    {PANEL_PANDA, TRUE, PANEL_SAMPLE_4, WATCH_LAUNCHER_PANEL_PANDA_WIN_ID,DrawAClock},
#endif
#ifdef WATCH_PANEL_BLACK_GREEN_SUPPORT //黑绿色模拟时钟
    {PANEL_BLACK_GREEN, TRUE, PANEL_SAMPLE_5, WATCH_LAUNCHER_PANEL_5_WIN_ID,DrawAClock1},
#endif  
#ifdef WATCH_PANEL_DEER_SUPPORT    //小鹿
    #ifdef MAINLCD_DEV_SIZE_240X284
    {PANEL_DEER, TRUE, PANEL_SAMPLE_6, WATCH_LAUNCHER_PANEL_DEER_WIN_ID,DrawAClock2},
    #endif
#endif
#endif //   模拟时钟 end
#ifdef ZTE_WATCH
    {ZTE_PANEL_SPACEMAN, FALSE, panel_spaceman_preview, WATCH_LAUNCHER_PANEL_ZTE_SPACEMAN_WIN_ID, ZTE_Draw_Spaceman_Panel},
//#ifndef BAIDU_AI_SUPPORT  // 去掉大图片，节省空间
    {ZTE_PANEL_ROCKET, FALSE, panel_rocket_preview, WATCH_LAUNCHER_PANEL_ZTE_ROCKET_WIN_ID, ZTE_Draw_Rocket_Panel},
    {ZTE_PANEL_CAT, FALSE, panel_cat_preview, WATCH_LAUNCHER_PANEL_ZTE_CAT_WIN_ID, ZTE_Draw_Cat_Panel},
//#endif
#endif
};

LOCAL const uint PANEL_SIZE = sizeof(panel_select_page_list)/sizeof(PANEL_SELECT_INFO);

#define PANEL_SIZE_MAX 10

//第一次开机设置一下默认表盘
LOCAL void setDefaultPanel()
{
    WATCH_PANEL_TYPE_T style = {0};
    style.watch_index = panel_select_page_list[0].index;
    style.is_anolog_panel = panel_select_page_list[0].is_anolog_watch;
    panel_select_page_list[0].draw_handle(panel_select_page_list[0].win_id);
    MMISET_SetWatchPanelStyle(style);
}

LOCAL void updateClock(PANEL_INDEX_E index)
{
    switch(index)
    {
        case PANEL_PANDA:
            {
            #ifdef WATCH_PANEL_PANDA_SUPPORT //熊猫
                DrawAClock();
            #endif
            }
            break;
        case PANEL_BLACK_GREEN:
            {
            #ifdef WATCH_PANEL_BLACK_GREEN_SUPPORT //黑绿色模拟时钟
                DrawAClock1();
            #endif
            }
            break;
        case PANEL_DEER:
            {
        #ifdef WATCH_PANEL_DEER_SUPPORT    //小鹿
            #ifdef MAINLCD_DEV_SIZE_240X284
                DrawAClock2();
            #endif
        #endif
            }
            break;
#ifdef ZMT_DIAL_STORE_SUPPORT
        case ZMT_DIAL_INDEX_0:
        case ZMT_DIAL_INDEX_1:
        case ZMT_DIAL_INDEX_2:
        case ZMT_DIAL_INDEX_3:
        case ZMT_DIAL_INDEX_4:
        case ZMT_DIAL_INDEX_5:
        case ZMT_DIAL_INDEX_6:
        case ZMT_DIAL_INDEX_7:
        case ZMT_DIAL_INDEX_8:
        case ZMT_DIAL_INDEX_9:
            {
                ZMT_DisplayDial(WATCH_LAUNCHER_PANEL_WIN_ID, index);
            }
            break;
#endif
        default:
            {
                setDefaultPanel();
            }
            break;
    }
}

#ifdef ZMT_DIAL_STORE_SUPPORT
LOCAL void DisplayPanelZmtDial(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    GUI_RECT_T win_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    MMI_CTRL_ID_T ctrl_id = 0;
    GUI_RECT_T anim_rect = {0};
    GUI_RECT_T img_rect = {0};
    char img_str[128] = {0};
    wchar img_path[128] = {0};
    ZMT_DIAL_LIST_INFO_T * dail_list = ZmtWatch_GetPanelList();
    if(dail_list != NULL)
    {
        for(i = 0;i < dail_list->count && i < PANEL_SIZE_MAX - PANEL_SIZE;i++)
        {
            if(win_id == WATCH_LAUNCHER_PANEL_ZMT_0_WIN_ID + i)
            {
                MMK_GetWinRect(win_id, &win_rect);
                MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
                GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);
                
                if(dail_list->info[i] == NULL){
                    return;
                }
                ctrl_id = LAUNCHER_SELECET_PANEL_0_CTRL_ID + i;
                sprintf(img_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dail_list->info[i]->name, dail_list->info[i]->preview);
                //SCI_TRACE_LOW("%s: img_str = %s", __FUNCTION__, img_str);
                if(dsl_file_exist(img_str))
                {
                    GUIIMG_INFO_T img_info = {0};
                    MMIAPICOM_StrToWstr(img_str, img_path);
                    ZMT_GetImgInfoByPath(img_path, &img_info);
                    img_rect.left = (MMI_MAINSCREEN_WIDTH - img_info.image_width) / 2;
                    img_rect.top = (MMI_MAINSCREEN_HEIGHT - img_info.image_height) / 2;
                    img_rect.right = img_rect.left + img_info.image_width;
                    img_rect.bottom = img_rect.top + img_info.image_height;
                    {
                        GUIANIM_FILE_INFO_T file_info = {0};                           
                        GUIANIM_CTRL_INFO_T ctrl_info = {0};
                        GUIANIM_DISPLAY_INFO_T display_info = {0};
                        GUIANIM_CTRL_INFO_T control_info = {0};
                        GUIANIM_INIT_DATA_T anim_init = {0};
                        MMI_CONTROL_CREATE_T anim_create = {0};
                        CTRLANIM_OBJ_T* anim_ctrl_ptr = PNULL;

                        file_info.full_path_wstr_ptr = SCI_ALLOC_APPZ(sizeof(wchar)*(WATCH_IMAGE_FULL_PATH_MAX_LEN + 1));
                        if (PNULL == file_info.full_path_wstr_ptr){
                            SCI_TRACE_LOW("%s: full_path_wstr_ptr = pnull!", __FUNCTION__);
                            return;
                        }
                        SCI_MEMSET(file_info.full_path_wstr_ptr,0,(sizeof(wchar)*(WATCH_IMAGE_FULL_PATH_MAX_LEN + 1)));

                        file_info.full_path_wstr_len = MMIAPICOM_Wstrlen(img_path);
                        MMI_WSTRNCPY(file_info.full_path_wstr_ptr,sizeof(wchar)*(file_info.full_path_wstr_len+1),
                                img_path,file_info.full_path_wstr_len,file_info.full_path_wstr_len);

                        ctrl_info.is_ctrl_id = TRUE;
                        ctrl_info.ctrl_id = ctrl_id;
                        display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
                        display_info.is_auto_zoom_in = TRUE;
                        display_info.is_update = TRUE;
                        display_info.is_disp_one_frame = TRUE;

                        anim_init.both_rect.h_rect = anim_init.both_rect.v_rect = img_rect;
                        anim_create.guid = SPRD_GUI_ANIM_ID;
                        anim_create.ctrl_id = ctrl_id;
                        anim_create.init_data_ptr = &anim_init;
                        anim_create.parent_win_handle = win_id;
                        anim_ctrl_ptr = (CTRLANIM_OBJ_T*)MMK_CreateControl(&anim_create);

                        control_info.is_ctrl_id = TRUE;
                        control_info.ctrl_id = ctrl_id;
                        control_info.ctrl_ptr = anim_ctrl_ptr;
                            
                        GUIANIM_SetDefaultIcon(ctrl_id, PNULL, PNULL);
                        GUIANIM_SetDisplayRect(ctrl_id, &img_rect, FALSE);
                        GUIANIM_SetParam(&ctrl_info, PNULL, &file_info, &display_info);
                    }
                }
                break;
            }
        }
        ZmtDial_ReleaseDialList(dail_list);
    }
}
#endif

LOCAL void DisplayPanelPage(MMI_WIN_ID_T win_id)
{
    BOOLEAN is_get = FALSE;
    uint8 i = 0;
    GUI_RECT_T win_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUI_RECT_T img_rect = WATCH_PANEL_PREVIEW_IMG_RECT;
    GUI_POINT_T img_point;
    
    img_point.x = img_rect.left;
    img_point.y = img_rect.top;
    MMK_GetWinRect(win_id, &win_rect);
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);

    for(i=0;i<PANEL_SIZE; i++)
    {
        if(panel_select_page_list[i].win_id == win_id)
        {
            GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);
            GUIRES_DisplayImg(NULL,&img_rect,NULL,win_id,panel_select_page_list[i].img_id,&lcd_dev_info);
            is_get = TRUE;
            break;
        }
    }
    SCI_TRACE_LOW("%s: is_get = %d", __FUNCTION__, is_get);
    if(is_get){
        return;
    }

#ifdef ZMT_DIAL_STORE_SUPPORT
    DisplayPanelZmtDial(win_id);
#endif
}

#ifdef ZMT_DIAL_STORE_SUPPORT
LOCAL void ZMT_DisplayStorePanel(MMI_WIN_ID_T win_id)
{   
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUI_RECT_T win_rect = {0};
    GUI_RECT_T title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT/10};
    GUI_RECT_T img_rect = WATCH_PANEL_PREVIEW_IMG_RECT;
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};

    MMK_GetWinRect(win_id, &win_rect);
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);

    GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;
    MMIRES_GetText(ZMT_DIAL_STORE, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &title_rect,
        &title_rect,
        &text_string,
        &text_style,
        GUISTR_STATE_ALIGN,
        GUISTR_TEXT_DIR_AUTO
    );
    
    GUIRES_DisplayImg(NULL,&img_rect,NULL,win_id,IMG_ZMT_DIAL_STORE,&lcd_dev_info);
}
#endif

LOCAL void SetPanelIndex(MMI_WIN_ID_T win_id)
{
    BOOLEAN is_get = FALSE;
    uint8 i = 0;
    for(i=0;i<PANEL_SIZE; i++)
    {
        if(panel_select_page_list[i].win_id == win_id)
        {
            WATCH_PANEL_TYPE_T style = {0};
            style.watch_index= panel_select_page_list[i].index;
            style.is_anolog_panel = panel_select_page_list[i].is_anolog_watch;
            MMISET_SetWatchPanelStyle(style);
            //MMK_CloseWin(win_id);
            WatchSLIDEPAGE_DestoryHandle(s_panel_select_handle);
            s_panel_select_handle = 0;
            //MMIWatchPanel_Open();
            WatchOpen_IdleWin();
            WatchSLIDEAGE_SetCurrentPageIndex(1);
            ZMT_DialRelease();
            is_get = TRUE;
            break;
        }
    }
    SCI_TRACE_LOW("%s: is_get = %d", __FUNCTION__, is_get);
    if(is_get){
        return;
    }
    //SCI_TRACE_LOW("%s: PANEL_SIZE = %d, i = %d, win_id = %d", __FUNCTION__, PANEL_SIZE, i, win_id);
#ifdef ZMT_DIAL_STORE_SUPPORT
    {
        uint8 j = 0;
        MMI_CTRL_ID_T ctrl_id = 0;
        GUI_RECT_T anim_rect = {0};
        char img_str[80] = {0};
        wchar img_path[80] = {0};
        ZMT_DIAL_LIST_INFO_T * dail_list = ZmtWatch_GetPanelList();
        if(dail_list != NULL)
        {
            for(i = 0;i < dail_list->count && i < PANEL_SIZE_MAX - PANEL_SIZE;i++)
            {
                if(win_id == WATCH_LAUNCHER_PANEL_ZMT_0_WIN_ID + i)
                {
                    WATCH_PANEL_TYPE_T style = {0};
                    style.watch_index= ZMT_DIAL_INDEX_0 + i;
                    if(dail_list->info[i]->type == 2){
                        style.is_anolog_panel = TRUE;
                    }else{
                        style.is_anolog_panel = FALSE;
                    }
                    MMISET_SetWatchPanelStyle(style);
                    WatchSLIDEPAGE_DestoryHandle(s_panel_select_handle);
                    s_panel_select_handle = 0;
                    ZMT_DialRelease();
                    WatchOpen_IdleWin();
                    WatchSLIDEAGE_SetCurrentPageIndex(1);
                    break;
                }
            }
            ZmtDial_ReleaseDialList(dail_list);
        }
    }
#endif
}

PUBLIC MMI_RESULT_E Watch_Panel_Select_HandleCb(
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

//process the common action of launcher
PUBLIC MMI_RESULT_E Watch_Panel_Select_HandleCommonWinMsg(
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
    MMI_CheckAllocatedMemInfo();
    switch (msg_id)
    {

#ifdef TOUCH_PANEL_SUPPORT
        case MSG_TP_PRESS_DOWN:
        {
            tp_point.x = MMK_GET_TP_X(param);
            tp_point.y = MMK_GET_TP_Y(param);
            b_tp_pressed = TRUE;
            break;
        }

        case MSG_TP_PRESS_MOVE:
        {
            b_tp_moved = TRUE;
            break;
        }

        case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T tp_up = {0};
            BOOLEAN bClick = FALSE;
            tp_up.x = MMK_GET_TP_X(param);
            tp_up.y = MMK_GET_TP_Y(param);
            bClick = (abs(tp_up.x - tp_point.x) < 3) && (abs(tp_up.y - tp_point.y) < 3);
            if (b_tp_pressed && bClick)
            {
				MMK_PostMsg(win_id, MSG_APP_WEB, &tp_up, sizeof(GUI_POINT_T));
            }
            b_tp_pressed = FALSE;
            b_tp_moved = FALSE;
            b_drop_from_top = FALSE;
            break;
        }
#endif
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
            if(s_panel_select_handle != NULL)
            {
                WatchSLIDEPAGE_DestoryHandle(s_panel_select_handle);
                s_panel_select_handle = 0;
                WatchOpen_IdleWin();
                WatchSLIDEAGE_SetCurrentPageIndex(1);
            }
            break;
        case MSG_NOTIFY_CANCEL:
        case MSG_APP_CANCEL:
            break;
        default:
            recode = MMI_RESULT_FALSE;
    }
    MMI_CheckAllocatedMemInfo();
    return recode;
}

LOCAL MMI_RESULT_E HandlePanelSelectWinMsg(
                                      MMI_WIN_ID_T        win_id,        //IN:
                                      MMI_MESSAGE_ID_E    msg_id,        //IN:
                                      DPARAM            param        //IN:
                                      )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMI_CheckAllocatedMemInfo();
    switch (msg_id)
    {
        case MSG_GET_FOCUS:
            MMK_PostMsg(win_id,MSG_FULL_PAINT,PNULL,PNULL);
            break;
        case MSG_FULL_PAINT:
        {
            SCI_TRACE_LOW("%s: win_id = %d", __FUNCTION__, win_id);
        #ifdef ZMT_DIAL_STORE_SUPPORT
            if(win_id == WATCH_LAUNCHER_PANEL_ZMT_STORE_WIN_ID)
            {
                ZMT_DisplayStorePanel(win_id);
                break;
            }
        #endif
            DisplayPanelPage(win_id);
            break;
        }
        case MSG_APP_WEB:
        {
        #ifdef ZMT_DIAL_STORE_SUPPORT
            if(win_id == WATCH_LAUNCHER_PANEL_ZMT_STORE_WIN_ID)
            {
                MMI_CreateZmtDialStoreWin();
                WatchSLIDEAGE_SetCurrentPageIndex(1);
                break;
            }
        #endif
            SetPanelIndex(win_id);
            break;
        }
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    MMI_CheckAllocatedMemInfo();
    if (!recode)
    {
        recode = Watch_Panel_Select_HandleCommonWinMsg(win_id, msg_id, param);
    }
    return recode;
}

LOCAL void Watch_Panel_Select_Enter(uint8 index)
{
    tWatchSlidePageItem elem[PANEL_SIZE_MAX] = {0};
    MMI_HANDLE_T handle = 0;
    uint8 i = 0;
    uint8 focus_index = 0;
    if(s_panel_select_handle)
    {
       WatchSLIDEPAGE_DestoryHandle(s_panel_select_handle);
       s_panel_select_handle = 0;
    }
    handle = WatchSLIDEPAGE_CreateHandle();
    for(i;i<PANEL_SIZE;i++)
    {
        elem[i].fun_enter_win = WatchPanel_CreateWin;//panel_select_page_list[i].preview_win;
        elem[i].win_id = panel_select_page_list[i].win_id;
        if(index == panel_select_page_list[i].index)//因为表盘是可以在MK里面配置删减所以不是顺序的
        {
            focus_index = i;
        }
        //SCI_TRACE_LOW("%s: elem[%d].win_id = %d", __FUNCTION__, i, elem[i].win_id);
    }
    SCI_TRACE_LOW("%s: PANEL_SIZE = %d, index = %d, i = %d", __FUNCTION__, PANEL_SIZE, index, i);
#ifdef ZMT_DIAL_STORE_SUPPORT
    {
        uint8 j = 0;
        ZMT_DIAL_LIST_INFO_T * dail_list = NULL;    
        dail_list = ZmtWatch_GetPanelList();
        if(dail_list != NULL){
            for(;i < PANEL_SIZE + dail_list->count;i++)
            {
                elem[i].fun_enter_win = WatchPanel_CreateWin;
                elem[i].win_id = WATCH_LAUNCHER_PANEL_ZMT_0_WIN_ID + j;
                if(index == ZMT_DIAL_INDEX_0 - PANEL_SIZE + i)
                {
                    focus_index = i;
                }
                j++;
                //SCI_TRACE_LOW("%s: elem[%d].win_id = %d", __FUNCTION__, i, elem[i].win_id);
            }
            ZmtDial_ReleaseDialList(dail_list);
        }
    }
    
    elem[i].fun_enter_win = WatchPanel_CreateWin;
    elem[i].win_id = WATCH_LAUNCHER_PANEL_ZMT_STORE_WIN_ID;
    i++;
#endif
    //SCI_TRACE_LOW("%s: focus_index = %d", __FUNCTION__, focus_index);
    WatchSLIDEPAGE_Open(handle, elem, i, focus_index, TRUE, Watch_Panel_Select_HandleCb);
    s_panel_select_handle = handle;
}

PUBLIC void WatchOpen_Panel_SelectWin()
{
    WATCH_PANEL_TYPE_T style = MMISET_GetWatchPanelStyle();
    Watch_Panel_Select_Enter(style.watch_index);
}

#define ARGB_GET_A(_argb) (((_argb) >> 24) & 0xff)
#define ARGB_GET_R(_argb) (((_argb) >> 16) & 0xff)
#define ARGB_GET_G(_argb) (((_argb) >> 8) & 0xff)
#define ARGB_GET_B(_argb) ((_argb) & 0xff)

#define ARGB_SET_A(_argb, _alpha)   ((_argb) | (((_alpha) << 24) & 0xff000000))
#define ARGB_SET_R(_argb, _r)       ((_argb) | (((_r) << 16) & 0xff0000))
#define ARGB_SET_G(_argb, _g)       ((_argb) | (((_g) << 8) & 0xff00))
#define ARGB_SET_B(_argb, _b)       ((_argb) | (((_b)) & 0xff))

#define RGB565_GET_R(_color) (((_color) >> 8) & 0xf8)
#define RGB565_GET_G(_color) (((_color) >> 3) & 0xfc)
#define RGB565_GET_B(_color) (((_color) << 3) & 0xf8)

#define RGB565_SET_R(_color, _r) ((_color) | (((_r) << 8) & 0xf800))
#define RGB565_SET_G(_color, _g) ((_color) | (((_g) << 3) & 0x07e0))
#define RGB565_SET_B(_color, _b) ((_color) | (((_b) >> 3) & 0x001f))

#define RGB888_TO_RGB565(r, g, b)  (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | ((b >> 3) & 0x1f))

#define BLEND_MASK 0x7E0F81F   //快速 Alpha混合


LOCAL BOOLEAN ProcessAlphaBlending_EX(MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *bk_img_ptr, MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *fore_img_ptr);
LOCAL void calculatePointerAngles(int hour, int minute, int second, uint16* hourAngle, uint16* minuteAngle, uint16* secondAngle);


LOCAL void calculatePointerAngles(int hour, int minute, int second, uint16* hourAngle, uint16* minuteAngle, uint16* secondAngle) 
{
    // 计算时针角度
if(hourAngle != PNULL)
    *hourAngle =  ((hour % 12) * 30);
    
    // 计算分针角度
if(minuteAngle != PNULL)
    *minuteAngle = 6 * minute;
    
    // 计算秒针角度
if(secondAngle != PNULL)
    *secondAngle = 6 * second;
}

/**
 * @brief Create a Cache Layer object
 * 
 * @param out 
 * @param win_id 
 * @param width 
 * @param height 
 * @return LOCAL 
 */
LOCAL void CreateCacheLayer(GUI_LCD_DEV_INFO *out, MMI_WIN_ID_T win_id, uint16 width, uint16 height)
{
#ifdef UI_MULTILAYER_SUPPORT
    UILAYER_CREATE_T create_info = {0};
    GUI_LCD_DEV_INFO *lcd_dev_ptr = out;
    if (PNULL == lcd_dev_ptr)
    {
        return;
    }

    if (lcd_dev_ptr->block_id != UILAYER_NULL_HANDLE)
    {
        UILAYER_RELEASELAYER(lcd_dev_ptr);
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
    create_info.is_bg_layer = TRUE;
    create_info.is_static_layer = TRUE;

    UILAYER_CreateLayer(
        &create_info,
        lcd_dev_ptr
        );

    UILAYER_SetLayerColorKey(lcd_dev_ptr, TRUE, MMI_BLACK_COLOR);
    UILAYER_Clear(lcd_dev_ptr);

    if (lcd_dev_ptr->block_id == UILAYER_NULL_HANDLE)
    {
        //SCI_ASSERT(0);
        Trace_Log_ZDT("lcd_dev_ptr->block_id == UILAYER_NULL_HANDLE");
    }
#endif
}


LOCAL BOOLEAN DisplayImageToBufferWithAngle(uint8 *dst_buf,
    uint16 dst_width,
    uint16 dst_height,
    MMI_WIN_ID_T win_id,
    MMI_IMAGE_ID_T image_id, 
    uint32 angle)
{
    uint32 image_width = 0;
    uint32 image_height = 0;
    uint8 *image_decompress_ptr = NULL;
    uint32 image_decompress_size = 0;

    IMG_RES_SRC_T img_src = {0};
    IMG_RES_INFO_T img_info = {0};
    IMG_RES_DST_T img_dst = {0};
    IMG_RES_DST_CTL_T dstctl = {0};
    IMG_RES_ERROR_E error = IMG_RES_SUCCESS;
    uint32 image_stream_size = 0;
    uint8 *image_stream_ptr = NULL;
    Trace_Log_ZDT("enter.");

    if (dst_buf == NULL)
    {
        Trace_Log_ZDT("dst_buf == NULL.");
        return FALSE;
    }

    //step1. get image base info
    image_stream_ptr = (uint8 *)MMI_GetLabelImage(image_id, win_id, &image_stream_size);
    img_src.imgstream_ptr = image_stream_ptr;
    img_src.imgstreamsize = image_stream_size;

    error = ALPHA_RES_GetImageInfo(&img_src, &img_info);

    if (error != IMG_RES_SUCCESS)
    {
        Trace_Log_ZDT("ALPHA_RES_GetImageInfo() error. error = %d.", error);
        return FALSE;
    }
    image_width = img_info.width;
    image_height = img_info.height;
    Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    //step 2. decode
    image_decompress_size = image_width * image_height * sizeof(uint32);
    image_decompress_ptr = (uint8 *)SCI_ALLOC_APPZ(image_decompress_size);
    Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    Trace_Log_ZDT("malloc image_decompress_ptr, addr = 0x%0x.", image_decompress_ptr);
    if (image_decompress_ptr == NULL) /*lint !e774*/
    {
        Trace_Log_ZDT("malloc image_decompress_ptr = error.");
        return FALSE;
    }

    img_src.imgrect.left = 0;
    img_src.imgrect.right = image_width - 1;
    img_src.imgrect.top = 0;
    img_src.imgrect.bottom = image_height - 1;

    img_dst.dstctl = &dstctl;
    img_dst.dstctl->bg_type = IMG_BG_NORMAL;
    img_dst.dstctl->format = IMGREF_FORMAT_ARGB888;
    img_dst.dstctl->dstmem = image_decompress_ptr;
    img_dst.dstctl->width = img_info.width;
    img_dst.dstctl->height = img_info.height;
    img_dst.dstrect.top = 0;
    img_dst.dstrect.left = 0;
    img_dst.dstrect.right = img_info.width - 1;
    img_dst.dstrect.bottom = img_info.height - 1;
    Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    error = ALPHA_RES_Display_Img(&img_src, &img_dst);
    Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    if (error != IMG_RES_SUCCESS)
    {
        Trace_Log_ZDT("ALPHA_RES_Display_Img() error. error = %d.", error);
        return FALSE;
    }

    //step3 .rotate image && draw it to dstbuf
#if 1
    do
    {
        ROTATE_ARBITRARY_IN_T	rot_in = {0};
        ROTATE_ARBITRARY_OUT_T	rot_out = {0};
        rot_in.angle = angle;

        rot_in.src_format = IMGREF_FORMAT_ARGB888;
        rot_in.src_size.w = image_width;
        rot_in.src_size.h = image_height;
        rot_in.src_center.x = image_width >> 1;
        rot_in.src_center.y = image_height >> 1;
        rot_in.src_chn.chn0.ptr = image_decompress_ptr;
        rot_in.src_chn.chn0.size = image_decompress_size;

        rot_in.target_format = IMGREF_FORMAT_ARGB888;
        rot_in.target_size.w = dst_width;
        rot_in.target_size.h = dst_height;
        rot_in.target_chn.chn0.ptr = (void *)dst_buf;
        rot_in.target_chn.chn0.size = dst_width * dst_height * sizeof(uint32);
        rot_in.target_center.x = dst_width / 2;
        rot_in.target_center.y = dst_height / 2;

        rot_in.mode = ROTATE_ARBITRARY_AUTO_CROP;
        Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

        if (SCI_SUCCESS != GRAPH_RotateArbitrary(&rot_in, &rot_out))
        {
            Trace_Log_ZDT(" error. end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
            SCI_FREE(image_decompress_ptr);
            SCI_TRACE_LOW("invoke GRAPH_RotateArbitrary() error.");
            return FALSE;
        }
        Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
    } while (0);
#else

    do
    {
        uint16 h = 0;
        uint16 start_x = 24;
        uint16 start_y = 24;
        uint32 *pdst = (uint32 *)dst_buf;
        uint32 *psrc = (uint32 *)image_decompress_ptr;
        pdst += start_x * dst_width + start_x;
        for (h = 0; h < image_height; h++)
        {
            SCI_MEMCPY(pdst, psrc, sizeof(uint32) * image_width);
            pdst += dst_width;
            psrc += image_width;
        }
    } while (0);
#endif
    Trace_Log_ZDT("free image_decompress_ptr, addr = 0x%0x.", image_decompress_ptr);
    if (image_decompress_ptr != NULL)
    {
        SCI_FREE(image_decompress_ptr);
    }

    Trace_Log_ZDT("exit.");
}
    
/**
 * @brief 
 * 
 * @param bk_color 
 * @param fore_color 
 * @param alpha 
 * @return LOCAL 
 */
LOCAL  uint16 BlendRGB565(uint16 bk_color, uint16 fore_color, uint8 alpha)
{
    uint32 blend_color = 0;
    uint32 temp_back_color = 0;
    uint32 temp_fore_color = 0;

    alpha = (alpha + 1) >> 3;

    temp_fore_color = ( fore_color | (fore_color << 16)) & BLEND_MASK;
    temp_back_color = ( bk_color | (bk_color << 16)) & BLEND_MASK;
    blend_color = ((((temp_fore_color - temp_back_color) * alpha ) >> 5 ) + temp_back_color) & BLEND_MASK;
    blend_color = ((blend_color & 0xFFFF) | (blend_color >> 16));

    return (uint16)blend_color;
}

LOCAL BOOLEAN ProcessAlphaBlending_EX(MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *bk_img_ptr, MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *fore_img_ptr)
{
    uint32 fore_width  = fore_img_ptr->width;
    uint32 fore_height = fore_img_ptr->height;
    uint32 bk_width = bk_img_ptr->width;
    uint32 bk_height = bk_img_ptr->height;
    int32 start_x = 0;//fore_img_ptr->start_x;
    int32 start_y = 0;//fore_img_ptr->start_y;
    uint32 blend_width = fore_width;
    uint32 blend_height = fore_height;
    uint32 *fore_ptr = (uint32 *)fore_img_ptr->data_ptr;
    GUI_RECT_T dst_rect = {0};
    GUI_RECT_T bk_rect = {0};
    GUI_RECT_T for_rect = {0};
    bk_rect.left  = bk_img_ptr->start_x;
    bk_rect.top   = bk_img_ptr->start_y;
    bk_rect.right = bk_img_ptr->start_x + bk_img_ptr->width;
    bk_rect.bottom  = bk_img_ptr->start_y + bk_img_ptr->height;

    for_rect.left  = fore_img_ptr->start_x;
    for_rect.top   = fore_img_ptr->start_y;
    for_rect.right = fore_img_ptr->start_x + fore_img_ptr->width;
    for_rect.bottom  = fore_img_ptr->start_y + fore_img_ptr->height;

    if (!GUI_IntersectRect(&dst_rect, bk_rect, for_rect))
    {
        return FALSE;
    }

    if (fore_img_ptr->start_y < 0)
    {
        start_y = -fore_img_ptr->start_y;
        blend_height -= start_y;        /*lint !e737*/
        fore_ptr += (start_y * fore_width);     /*lint !e737*/
        start_y = 0;
    }

    if (fore_img_ptr->start_x < 0)
    {
        start_x = -fore_img_ptr->start_x;
        blend_width -= start_x;     /*lint !e737*/
        fore_ptr += start_x;
        start_x = 0;
    }

    //SCI_PASSERT((int32)bk_width >= start_x, ("bk_width=%d, start_x=%d", bk_width, start_x));
    //SCI_PASSERT((int32)bk_height >= start_y, ("bk_height=%d, start_y=%d", bk_height, start_y));

    if ((int32)bk_width < start_x || (int32)bk_height < start_y)
    {
        //SCI_TRACE_LOW:"aclock ProcessAlphaBlending (int32)bk_width < start_x || (int32)bk_height < start_y"
        //SCI_TRACE_ID(TRACE_TOOL_CONVERT,MMIWIDGET_ACLOCK_387_112_2_18_3_7_49_166,(uint8*)"");
        return FALSE;
    }

    blend_width = (start_x + blend_width) > bk_width        /*lint !e737*/
                    ? (bk_width - start_x) : blend_width;       /*lint !e737*/
    blend_height = (start_y + blend_height) > bk_height         /*lint !e737*/
                    ? (bk_height - start_y) : blend_height;     /*lint !e737*/

    if (IMGREF_FORMAT_RGB565 == bk_img_ptr->data_type)     //output RGB565
    {
        uint32 i = 0, j = 0;
        uint16 *bk_ptr = (uint16 *)bk_img_ptr->data_ptr;

        bk_ptr += (start_y * bk_width + start_x);       /*lint !e737*/

        for (i=0; i<blend_height; i++)
        {
            for (j=0; j<blend_width; j++)
            {
                uint32 alpha, r, g, b;
                uint32 fore_value = *(fore_ptr + j);
                uint16 bk_value = *(bk_ptr + j);

                alpha = ARGB_GET_A(fore_value);
                r = ARGB_GET_R(fore_value);
                g = ARGB_GET_G(fore_value);
                b = ARGB_GET_B(fore_value);

                fore_value = RGB888_TO_RGB565(r, g, b);
                bk_value = BlendRGB565(bk_value, (uint16)fore_value, alpha);
                *(bk_ptr + j) = bk_value;
            }

            bk_ptr += bk_width;
            fore_ptr += fore_width;
        }
    }
    else                                //output ARGB888
    {
        uint32 i = 0, j = 0;
        uint32 *bk_ptr = (uint32 *)bk_img_ptr->data_ptr;

        bk_ptr += (start_y * bk_width + start_x);       /*lint !e737*/

        for (i=0; i<blend_height; i++)
        {
            for (j=0; j<blend_width; j++)
            {
                int32 alpha, bk_color, fore_color;
                uint32 fore_value = *(fore_ptr + j);
                uint32 bk_value = *(bk_ptr + j);
                uint32 result_value = (bk_value & 0xff000000);

#if 1
                alpha = ARGB_GET_A(bk_value);

                if (0 == alpha) 
                {
                    *(bk_ptr + j) = *(fore_ptr + j);
                    continue;
                }

                alpha = ARGB_GET_A(fore_value);

                //r
                bk_color = ARGB_GET_R(bk_value);
                fore_color = ARGB_GET_R(fore_value);
                bk_color = bk_color + (((fore_color - bk_color) * alpha) >> 8);
                result_value = ARGB_SET_R(result_value, bk_color);

                //g
                bk_color = ARGB_GET_G(bk_value);
                fore_color = ARGB_GET_G(fore_value); 
                bk_color = bk_color + (((fore_color - bk_color) * alpha) >> 8);
                result_value = ARGB_SET_G(result_value, bk_color);

                //b
                bk_color = ARGB_GET_B(bk_value);
                fore_color = ARGB_GET_B(fore_value); 
                bk_color = bk_color + (((fore_color - bk_color) * alpha) >> 8);
                result_value = ARGB_SET_B(result_value, bk_color);
#else
                result_value = GUI_CompoundARGB888(bk_value, fore_value);
#endif
                *(bk_ptr + j) = result_value;
            }

            bk_ptr += bk_width;
            fore_ptr += fore_width;
        }
    }

    return TRUE;
}

LOCAL void TimeTeacherClock(MMI_WIN_ID_T  win_id,
                                GUI_RECT_T dispRect ,
                                SCI_TIME_T sys_time , 
                                MMI_IMAGE_ID_T clock_bg,
                                MMI_IMAGE_ID_T clock_hour,
                                MMI_IMAGE_ID_T clock_min,
                                MMI_IMAGE_ID_T clock_sec,
                                MMI_IMAGE_ID_T clock_center
                                )
{
#ifdef UI_MULTILAYER_SUPPORT
    uint8 i = 0;
    uint32 rotate_buf_size = 0;
    uint16 angle_arr[3] = {0};
   // SCI_TIME_T sys_time= {0};
    uint16 image_width = 0;
    uint16 image_height = 0;
    GUI_POINT_T clock_bg_point = {0, 0};
    
    GUI_POINT_T clockpanel_pos = {0, 0};
    GUI_POINT_T clockcenter_pos = {0, 0};
    GUI_RECT_T display_bg_rect = {0};
    
    GUI_RECT_T rect = {0};
    
    GUI_LCD_DEV_INFO lcd = {0};
    GUI_LCD_DEV_INFO lcd_cache_info = {0, UILAYER_NULL_HANDLE};
    uint8 *rotate_buf_ptr = NULL;
    MMI_IMAGE_ID_T image_arr[3] = {0};               
                                                     
                                                     
                                                     
    image_arr[0] = clock_hour;
    image_arr[1] = clock_min;
    image_arr[2] = clock_sec;

    //SCI_DATE_T s_sys_date = {0};
    //TM_GetSysDate(&s_sys_date);
    rect.right = dispRect.right - dispRect.left;
    rect.bottom =  dispRect.bottom - dispRect.top;
    Trace_Log_ZDT("enter.");
    rotate_buf_size = (rect.right) * (rect.bottom) * sizeof(uint32);

    //get system time
    //sys_time = *time_ptr;
    Trace_Log_ZDT("sys_time = %d: %d: %d.", sys_time.hour, sys_time.min, sys_time.sec);
#ifdef WIN32
    do
    {
        LOCAL uint8 localsec = 0;
        localsec++;
        localsec %= 60;
        sys_time.sec= localsec;
    } while (0);
#endif

    calculatePointerAngles(sys_time.hour, sys_time.min, sys_time.sec, &angle_arr[0], &angle_arr[1], &angle_arr[2]);

    Trace_Log_ZDT("hour=%d,min=%d,angle_hour=%d,angle_min=%d",sys_time.hour,sys_time.min,angle_arr[0],angle_arr[1]);
    
    Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
    CreateCacheLayer(&lcd_cache_info, win_id, (rect.right), (rect.bottom));
    Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

    if (lcd_cache_info.block_id == UILAYER_NULL_HANDLE) 
    {
        Trace_Log_ZDT("CreateCacheLayer [error].");
        return;
    }
    
    GUIRES_GetImgWidthHeight(&image_width, &image_height, clock_bg, win_id);
    
    clock_bg_point.x = rect.left;
    clock_bg_point.y = rect.top;
    //clockpanel_pos.x = (rect.right - image_width) / 2 ;
    //clockpanel_pos.y = (rect.bottom - image_height) / 2 ;
    display_bg_rect.top = 0;
    display_bg_rect.left = 0;
    display_bg_rect.right = image_width;
    display_bg_rect.bottom = image_height;
    if(clock_bg != PNULL)
    {
    GUIRES_DisplayImg(&clock_bg_point,
                      PNULL,  //&display_bg_rect,
                      PNULL,  //&dispRect,              //
                      win_id,
                      clock_bg,
                      &lcd_cache_info);             //clock bg
    }
    //step2. draw hour, min, second image to dstbuf
        Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
        rotate_buf_ptr = SCI_ALLOC_APPZ(rotate_buf_size);
        Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
        Trace_Log_ZDT("malloc rotate_buf_ptr, addr = 0x%0x.", rotate_buf_ptr);
    
        if (rotate_buf_ptr == NULL)
        {
            Trace_Log_ZDT("malloc rotate_buf_ptr [error].");
            return;
        }
    
        //
        for (i = 0; i < 3; i++)
        {
            MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T bk = {0};
            MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T fg = {0};
            SCI_MEMSET(rotate_buf_ptr, 0, rotate_buf_size);
            Trace_Log_ZDT(" start %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
            DisplayImageToBufferWithAngle(rotate_buf_ptr,
                                                                (rect.right), 
                                                                (rect.bottom),
                                                                win_id,
                                                                image_arr[i],
                                                                angle_arr[i]);
            Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);

            bk.data_ptr = UILAYER_GetLayerBufferPtr(&lcd_cache_info);
            bk.data_type = 0;
            bk.start_x = 0;
            bk.start_y = 0;
            bk.width = rect.right ; 
            bk.height = rect.bottom  ;
    
            fg.data_ptr = rotate_buf_ptr;
            fg.data_type = 1;
            fg.start_x = rect.left ;
            fg.start_y = rect.top ;
            fg.width = rect.right;
            fg.height = rect.bottom;
            ProcessAlphaBlending_EX(&bk, &fg);
        }
    
        if (rotate_buf_ptr != NULL)
        {
            Trace_Log_ZDT("free rotate_buf_ptr, addr = 0x%0x.", rotate_buf_ptr);
            SCI_FREE(rotate_buf_ptr);
        }
        Trace_Log_ZDT(" end %d KB-- %d kB",SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
        
        GUIRES_GetImgWidthHeight(&image_width, &image_height, clock_center, win_id);
        clockcenter_pos.x = (rect.right - image_width) / 2;
        clockcenter_pos.y = (rect.bottom- image_height) / 2;
    
        //step3, draw clock red center
        if(clock_center != PNULL)
        {
                GUIRES_DisplayImg(&clockcenter_pos,
                                  PNULL,  //&display_bg_rect,
                                  PNULL,  //&dispRect,              //
                                  win_id,
                                  clock_center,
                                  &lcd_cache_info);             //clock bg
        }

    
        UILAYER_SetLayerPosition(&lcd_cache_info, dispRect.left, dispRect.top);
        UILAYER_BltLayerToLayer(&lcd, &lcd_cache_info, &dispRect, FALSE);
    
        UILAYER_RELEASELAYER(&lcd_cache_info);



        
#endif

}

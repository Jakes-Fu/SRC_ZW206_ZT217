/*****************************************************************************
** File Name:         watch_common.c                                         *
** Author:           fangfang.yao                                            *
** Date:             02/20/2020                                              *
** Copyright:                                                                *
** Description:    This file is used to define common part                   *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE                 NAME                  DESCRIPTION                    *
** 02/07/2020           fangfang.yao          Create                         *
******************************************************************************/

/**--------------------------------------------------------------------------*
**                         Include Files                                     *
**---------------------------------------------------------------------------*/
#include "os_api.h"
#include "watch_common.h"
#include "guilcd.h"
#include "mmi_theme.h"
#include "mmidisplay_data.h"
#include "mmi_string.h"
#include "mmi_common.h"
#include "guires.h"

#include "zdt_image.h"
#include "zdt_text.h"
#include "version.h"

/**-------------------------------------------------------------------------*
**                         MACRO DEFINITION                                 *
**--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                               *
**---------------------------------------------------------------------------*/


/*****************************************************************************/
//  Description : draw window background
//  Parameter: [In] win_id: the list control ID
//             [Out] None
//             [Return] None
//  Author: fangfang.yao
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_DisplayBackground( MMI_WIN_ID_T win_id )
{
    // TODO : win_id for different window requirement
    GUI_RECT_T         	disp_rect   		= MMITHEME_GetFullScreenRect();
    GUI_LCD_DEV_INFO  lcd_dev_info 	= {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};

    LCD_FillRect(&lcd_dev_info, disp_rect, MMI_BLACK_COLOR);
}

/*****************************************************************************/
//  Description : draw window background and background PIC
//  Parameter: [In] win_id: the list control ID
//             [Out] None
//             [Return] None
//  Author: fangfang.yao
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_DisplayBackgroundEx( MMI_WIN_ID_T win_id ,MMI_IMAGE_ID_T  img_id)
{
    // TODO : win_id for different window requirement
    GUI_RECT_T         	disp_rect   		= MMITHEME_GetFullScreenRect();
    GUI_LCD_DEV_INFO  lcd_dev_info 	= {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};

    LCD_FillRect(&lcd_dev_info, disp_rect, MMI_BLACK_COLOR);
    GUIRES_DisplayImg(PNULL,
            &disp_rect,
            PNULL,
            win_id,
            img_id, 
            &lcd_dev_info); 
}

/*****************************************************************************/
//  Description : draw title on top
//  Parameter: [In] win_id: 
//             [in] title_id:title res id
//             [Return] None
//  Author: deng
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_DisplayTitle( MMI_WIN_ID_T win_id, MMI_TEXT_ID_T title_id)
{
    // TODO : win_id for different window requirement
    GUI_RECT_T         	disp_rect   		= WINDOW_TITLE_RECT;
    GUI_LCD_DEV_INFO  lcd_dev_info 	= {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_STRING_T   title_str     = {0};
    GUISTR_STYLE_T text_style = {0};
    LCD_FillRect(&lcd_dev_info, disp_rect, MMI_BLACK_COLOR);
    text_style.font = SONG_FONT_26;
    text_style.font_color = MMI_WHITE_COLOR;//MMI_BACKGROUND_COLOR;//changed by James li 20200511
    text_style.align = ALIGN_HVMIDDLE;//ALIGN_VMIDDLE;
    MMIRES_GetText(title_id, win_id, &title_str);
    GUISTR_DrawTextToLCDInRect(
        &lcd_dev_info,
        (const GUI_RECT_T *)&disp_rect,       //the fixed display area
        (const GUI_RECT_T *)&disp_rect,       
        (const MMI_STRING_T *)&title_str,
        &text_style,
        GUISTR_STATE_SINGLE_LINE,
        GUISTR_TEXT_DIR_AUTO);
}

/*****************************************************************************/
//  Description : draw title on top
//  Parameter: [In] win_id: 
//             [in] title_id:title res id
//             [in] title_height:title height
//             [in] font_color:font color
//             [in] text_align:对齐方式
//             [Return] None
//  Author: deng
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_DisplayTitleEx( MMI_WIN_ID_T win_id, MMI_TEXT_ID_T title_id, int16 title_height, GUI_COLOR_T font_color,GUI_ALIGN_E text_align )
{
    // TODO : win_id for different window requirement
    #ifdef ZTE_WATCH
    GUI_RECT_T         	disp_rect   		= WINDOW_TITLE_RECT;
    #else
    GUI_RECT_T         	disp_rect = {42,0,240,MMI_SPECIAL_TITLE_HEIGHT};
    #endif
    GUI_LCD_DEV_INFO  lcd_dev_info 	= {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_STRING_T   title_str     = {0};
    GUISTR_STYLE_T text_style = {0};
    LCD_FillRect(&lcd_dev_info, disp_rect, MMI_BLACK_COLOR);
    text_style.font = SONG_FONT_26;
    text_style.font_color = font_color;//MMI_BACKGROUND_COLOR;//changed by James li 20200511
    text_style.align = text_align;//ALIGN_VMIDDLE;
    disp_rect.bottom = disp_rect.top + title_height;
    MMIRES_GetText(title_id, win_id, &title_str);
    GUISTR_DrawTextToLCDInRect(
        &lcd_dev_info,
        (const GUI_RECT_T *)&disp_rect,       //the fixed display area
        (const GUI_RECT_T *)&disp_rect,       
        (const MMI_STRING_T *)&title_str,
        &text_style,
        GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
        GUISTR_TEXT_DIR_AUTO);
}

PUBLIC void WATCHCOM_DisplayTips( MMI_WIN_ID_T win_id, MMI_TEXT_ID_T title_id)
{
    GUI_RECT_T         	disp_rect   		= MMITHEME_GetFullScreenRect();
    GUI_LCD_DEV_INFO  lcd_dev_info 	= {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_STRING_T   title_str     = {0};
    GUISTR_STYLE_T text_style = {0};
    text_style.font = SONG_FONT_26;
    text_style.font_color = MMI_WHITE_COLOR;//MMI_BACKGROUND_COLOR;//changed by James li 20200511
    text_style.align = ALIGN_HVMIDDLE;//ALIGN_VMIDDLE;
    MMIRES_GetText(title_id, win_id, &title_str);
    GUISTR_DrawTextToLCDInRect(
        &lcd_dev_info,
        (const GUI_RECT_T *)&disp_rect,       //the fixed display area
        (const GUI_RECT_T *)&disp_rect,       
        (const MMI_STRING_T *)&title_str,
        &text_style,
        GUISTR_STATE_SINGLE_LINE,
        GUISTR_TEXT_DIR_AUTO);
}

/*****************************************************************************/
//  Description : fill rounded rect 填充一个圆角矩形
//  Parameter: [In] win_id: 
//             [in] rect:矩形区域
//             [in] radius:圆角半径
//             [in] color:填充颜色
//             [Return] None
//  Author: deng
//  Note:
/*****************************************************************************/

PUBLIC void WATCHCOM_FillRoundedRect(MMI_WIN_ID_T win_id, GUI_RECT_T rect, uint16 radius, GUI_COLOR_T color)
{
    uint16 x = 0;
    uint16 y = 0;
    GUI_RECT_T rectangle = {0};
    GUI_RECT_T         	disp_rect   		= MMITHEME_GetFullScreenRect();
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    x = rect.left + radius;
    y = rect.top + radius;
    LCD_FillCircle(&lcd_dev_info, &disp_rect, x, y, radius,color);
    y = rect.bottom - radius;
    LCD_FillCircle(&lcd_dev_info, &disp_rect, x, y, radius,color);

    x = rect.right - radius;
    y = rect.top + radius;
    LCD_FillCircle(&lcd_dev_info, &disp_rect, x, y, radius,color);
    y = rect.bottom - radius;
    LCD_FillCircle(&lcd_dev_info, &disp_rect, x, y, radius,color);

    rectangle.top = rect.top;
    rectangle.left = rect.left + radius;
    rectangle.bottom = rect.bottom;
    rectangle.right = rect.right - radius;

    LCD_FillRect(&lcd_dev_info, rectangle, color);

    rectangle.top = rect.top + radius;
    rectangle.left = rect.left;
    rectangle.bottom = rect.bottom - radius;
    rectangle.right = rect.left + radius;

    LCD_FillRect(&lcd_dev_info, rectangle, color);

    rectangle.top = rect.top + radius;
    rectangle.left = rect.right - radius;
    rectangle.bottom = rect.bottom - radius;
    rectangle.right = rect.right;

    LCD_FillRect(&lcd_dev_info, rectangle, color);
}

/*****************************************************************************/
//  Description : 画个进度条
//  Parameter: [In] win_id: 
//             [in] rect:进度条区域
//             [in] percent:百分比
//             [in] bg_img_id:背景图片
//             [in] fg_img_id:前景图片
//             [in] hand_img_id:滑块图片 可以为空
//             [Return] None
//  Author: deng
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_ProgressBar(MMI_WIN_ID_T win_id, GUI_RECT_T rect, uint8 percent, MMI_IMAGE_ID_T bg_img_id, MMI_IMAGE_ID_T fg_img_id, MMI_IMAGE_ID_T hand_img_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUI_RECT_T progress_rect = {0};
    GUI_POINT_T point = {0};
    uint16 hand_img_width = 0;
    uint16 hand_img_height = 0;
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    if(hand_img_id > 0)
    {
        GUIRES_GetImgWidthHeight(&hand_img_width, &hand_img_height, hand_img_id, win_id);
        if(hand_img_height > (rect.bottom - rect.top)) //滑块大于进度条的话需要填充背景刷除
        {
            GUI_RECT_T hand_rect;
            hand_rect.left = rect.left - hand_img_width/2;
            hand_rect.top = rect.top - (hand_img_height - (rect.bottom - rect.top))/2;
            hand_rect.right = rect.right + hand_img_width/2;
            hand_rect.bottom = rect.bottom + (hand_img_height - (rect.bottom - rect.top))/2;
            LCD_FillRect(&lcd_dev_info, hand_rect, MMI_BLACK_COLOR);
        }
    }
    GUIRES_DisplayImg(PNULL,&rect,PNULL,win_id, bg_img_id,&lcd_dev_info);
    progress_rect.left = rect.left;
    progress_rect.top = rect.top;
    progress_rect.bottom = rect.bottom;
    progress_rect.right = rect.left + (rect.right-rect.left)*percent/100;
    GUIRES_DisplayImg(PNULL,&progress_rect,PNULL,win_id, fg_img_id,&lcd_dev_info);
    if(hand_img_width > 0 && hand_img_height > 0)
    {
        /*if(progress_rect.right < progress_rect.left+hand_img_width)
        point.x = 0;
        else*/ 
        if(progress_rect.right+hand_img_width/2 > rect.right)
            point.x = rect.right - hand_img_width+hand_img_width/3; 
        else
            point.x = progress_rect.right - hand_img_width/3;

        point.y = progress_rect.top - (hand_img_height - (progress_rect.bottom - progress_rect.top))/2;
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, hand_img_id,&lcd_dev_info);
    }
}
/*****************************************************************************/
//  Description : 画个进度条
//  Parameter: [In] win_id: 
//             [in] rect:进度条区域
//             [in] cur_index:当前进度级数
//             [in] total_item_num:进度条总级数
//             [in] bg_img_id:背景图片
//             [in] fg_img_id:前景图片
//             [in] hand_img_id:滑块图片 可以为空
//             [Return] None
//  Author: deng
//  Note:
/*****************************************************************************/
PUBLIC void WATCHCOM_ProgressBarByIndex(MMI_WIN_ID_T win_id, GUI_RECT_T rect, uint32 cur_index, uint32 total_item_num,MMI_IMAGE_ID_T bg_img_id, MMI_IMAGE_ID_T fg_img_id, MMI_IMAGE_ID_T hand_img_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUI_RECT_T progress_rect = {0};
    GUI_POINT_T point = {0};
    uint16 hand_img_width = 0;
    uint16 hand_img_height = 0;
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    if(hand_img_id > 0)
    {
        GUIRES_GetImgWidthHeight(&hand_img_width, &hand_img_height, hand_img_id, win_id);
        if(hand_img_height > (rect.bottom - rect.top)) //滑块大于进度条的话需要填充背景刷除
        {
            GUI_RECT_T hand_rect;
            hand_rect.left = rect.left - hand_img_width/2;
            hand_rect.top = rect.top - (hand_img_height - (rect.bottom - rect.top))/2;
            hand_rect.right = rect.right + hand_img_width/2;
            hand_rect.bottom = rect.bottom + (hand_img_height - (rect.bottom - rect.top))/2;
            LCD_FillRect(&lcd_dev_info, hand_rect, MMI_BLACK_COLOR);
        }
    }else
    	LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
    GUIRES_DisplayImg(PNULL,&rect,PNULL,win_id, bg_img_id,&lcd_dev_info);
    progress_rect.left = rect.left;
    progress_rect.top = rect.top;
    progress_rect.bottom = rect.bottom;
    progress_rect.right = rect.right;
    //from left to right
    if ( (cur_index + 1) != total_item_num)
    {
        progress_rect.right = (int16) (progress_rect.left + (progress_rect.right - progress_rect.left) * cur_index /total_item_num);    /*lint !e737*/
    }
    // progress_rect.right = rect.left + (rect.right-rect.left)*percent/100;
    if(progress_rect.right !=progress_rect.left)
        GUIRES_DisplayImg(PNULL,&progress_rect,PNULL,win_id, fg_img_id,&lcd_dev_info);
    if(hand_img_width > 0 && hand_img_height > 0)
    {
        /*if(progress_rect.right < progress_rect.left+hand_img_width)
        point.x = 0;
        else*/ 
        if(progress_rect.right+hand_img_width/2 > rect.right)
            point.x = rect.right - hand_img_width+hand_img_width/3; 
        else
            point.x = progress_rect.right - hand_img_width/3;

        point.y = progress_rect.top - (hand_img_height - (progress_rect.bottom - progress_rect.top))/2;
        GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, hand_img_id,&lcd_dev_info);
    }
}

typedef struct
{   MMI_TEXT_ID_T name_text_id;
    MMI_IMAGE_ID_T  avatar_image_id;
}AVATAR_ITEM_T;

AVATAR_ITEM_T avater_list[] = 
{
    {TXT_FATHER,IMAGE_ZDT_PB_FATHER},
    {TXT_MOTHER,IMAGE_ZDT_PB_MOTHER},
    {TXT_FATHER_EX,IMAGE_ZDT_PB_FATHER},
    {TXT_MOTHER_EX,IMAGE_ZDT_PB_MOTHER},
    {TXT_GRANDPA,IMAGE_ZDT_PB_GRANDPA},
    {TXT_GRANDMA,IMAGE_ZDT_PB_GRANDMA},
#ifdef ZTE_WATCH
    {TXT_SISTER,IMAGE_ZDT_PB_SISTER},
    {TXT_BROTHER,IMAGE_ZDT_PB_BROTHER},
    {TXT_YOUNGER_SISTER,IMAGE_ZDT_PB_SISTER},
    {TXT_YOUNGER_BROTHER,IMAGE_ZDT_PB_BROTHER},
    {TXT_GROUP,IMAGE_ZDT_PB_GROUP},
    {TXT_TEACHER,IMAGE_ZDT_PB_TEACHER},
#else
    {TXT_UNCLE,IMAGE_ZDT_PB_UNCLE},
    {TXT_AUNT,IMAGE_ZDT_PB_AUNT},
    {TXT_WAIGONG,IMAGE_ZDT_PB_WAIGONG},
    {TXT_WAIPO,IMAGE_ZDT_PB_WAIPO}, 
#endif
};

PUBLIC MMI_IMAGE_ID_T WATCHCOM_GetAvaterImageId(wchar *name)
{
    MMI_STRING_T   avater_str     = {0};
    uint8 size = sizeof(avater_list)/sizeof(AVATAR_ITEM_T);
    uint i=0;
    for(i;i<size;i++)
    {
        MMI_GetLabelTextByLang(avater_list[i].name_text_id, &avater_str);
        if(MMIAPICOM_Wstrncmp(avater_str.wstr_ptr, name, avater_str.wstr_len)== 0)
        {
            return avater_list[i].avatar_image_id;
        }
    }
#ifdef ZTE_WATCH
    return IMAGE_ZDT_PB_OTHER;
#else
    return IMAGE_ZDT_PB_OTHERMAN;
#endif
    
}

PUBLIC void WATCHCOM_Backlight(BOOLEAN is_alway_on)
{
    if(is_alway_on)
    {
        MMIDEFAULT_SetAlwaysHalfOnBackLight(FALSE);
        MMIDEFAULT_AllowTurnOffBackLight(FALSE);
        MMIDEFAULT_TurnOnBackLight();
    } else {
        MMIDEFAULT_AllowTurnOffBackLight(TRUE);
        MMIDEFAULT_SetAlwaysHalfOnBackLight(FALSE);
    }
}

//头像大图用于显示拨打\接听电话
#ifdef ZTE_WATCH
AVATAR_ITEM_T avater_big_list[] = 
{
    {TXT_FATHER,IMAGE_ZDT_PB_FATHER_BIG},
    {TXT_MOTHER,IMAGE_ZDT_PB_MOTHER_BIG},
    {TXT_FATHER_EX,IMAGE_ZDT_PB_FATHER_BIG},
    {TXT_MOTHER_EX,IMAGE_ZDT_PB_MOTHER_BIG},
    {TXT_GRANDPA,IMAGE_ZDT_PB_GRANDPA_BIG},
    {TXT_GRANDMA,IMAGE_ZDT_PB_GRANDMA_BIG},
    {TXT_SISTER,IMAGE_ZDT_PB_SISTER_BIG},
    {TXT_BROTHER,IMAGE_ZDT_PB_BROTHER_BIG},
    {TXT_YOUNGER_SISTER,IMAGE_ZDT_PB_SISTER_BIG},
    {TXT_YOUNGER_BROTHER,IMAGE_ZDT_PB_BROTHER_BIG},
    {TXT_TEACHER,IMAGE_ZDT_PB_TEACHER_BIG},
};

PUBLIC MMI_IMAGE_ID_T WATCHCOM_GetAvaterBigImageId(wchar *name)
{
    MMI_STRING_T   avater_str     = {0};
    uint8 size = sizeof(avater_big_list)/sizeof(AVATAR_ITEM_T);
    uint i=0;
    for(i;i<size;i++)
    {
        MMI_GetLabelTextByLang(avater_big_list[i].name_text_id, &avater_str);
        if(MMIAPICOM_Wstrncmp(avater_str.wstr_ptr, name, avater_str.wstr_len)== 0)
        {
            return avater_big_list[i].avatar_image_id;
        }
    }
    return IMAGE_ZDT_PB_OTHER_BIG;  
}

PUBLIC uint8* WATCHCOM_GetDeviceModel()
{
    #ifdef ZTE_WATCH
    return ZDT_SFR_TYPE;
    #else
    return "";
    #endif
}

PUBLIC uint8* WATCHCOM_GetSoftwareVersion()
{
    #ifdef ZTE_WATCH
    return ZDT_SFR_SW_VER;
    #else
    return "";
    #endif
}

#endif



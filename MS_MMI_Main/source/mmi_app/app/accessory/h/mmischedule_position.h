/*****************************************************************************
** File Name:      mmischedule_position.h                                               
** Author:           zhaohui  
** Date:            02/10/2007
** Copyright:      2003 Spreadtrum, Incoporated. All Rights Reserved.         *
** Description:    This file is used to describe gui common data struct      *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 02/10/2007       zhaohui          Create                                   *
******************************************************************************/

#ifndef _MMISCHEDULE_POSITION_H
#define _MMISCHEDULE_POSITION_H
#ifdef CALENDAR_SUPPORT
/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif
/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                 *
**--------------------------------------------------------------------------*/
#if defined (MAINLCD_SIZE_176X220) || defined (MAINLCD_SIZE_128X160)
	#define MMISCH_FOCUS_HEIGHT             18//24
	#define MMISCH_SPACE_TWO_RECT          	5//9
	#define MMISCH_SPACE_RECT             	(22 + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_WEEK_ICON_WIDTH          24//33
	#define MMISCH_WEEK_ICON_HEIGH          24//42

	//name
	#define MMISCH_NAME_TITLE_LEFT          10
	#define MMISCH_NAME_TITLE_TOP           (MMI_TITLE_HEIGHT + 3)

	#define MMISCH_NAME_TITLE_RIGHT         (MMISCH_NAME_TITLE_LEFT + 52)

	#define MMISCH_NAME_TITLE_BOTTOM        (MMISCH_NAME_TITLE_TOP + 18)//(MMISCH_NAME_TITLE_TOP + 22)
	#define MMISCH_NAME_LEFT                (MMISCH_NAME_TITLE_RIGHT + 2)
	#define MMISCH_NAME_TOP                 MMISCH_NAME_TITLE_TOP
	#define MMISCH_NAME_RIGHT               (MMI_MAINSCREEN_RIGHT_MAX_PIXEL - 13)
	#define MMISCH_NAME_BOTTOM              MMISCH_NAME_TITLE_BOTTOM
	#define MMISCH_NAME_RECT				{MMISCH_NAME_LEFT, MMISCH_NAME_TOP, MMISCH_NAME_RIGHT, MMISCH_NAME_BOTTOM}

	//date
	#define MMISCH_DATE_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_DATE_TITLE_TOP           (MMISCH_NAME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_DATE_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_DATE_TITLE_BOTTOM        (MMISCH_DATE_TITLE_TOP + 18)//(MMISCH_DATE_TITLE_TOP + 22)
	#define MMISCH_DATE_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_DATE_TOP                 MMISCH_DATE_TITLE_TOP
	#define MMISCH_DATE_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_DATE_BOTTOM              MMISCH_DATE_TITLE_BOTTOM
	#define MMISCH_DATE_RECT				{MMISCH_DATE_LEFT, MMISCH_DATE_TOP, MMISCH_DATE_RIGHT, MMISCH_DATE_BOTTOM}
	//time
	#define MMISCH_TIME_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_TIME_TITLE_TOP           (MMISCH_DATE_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_TIME_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_TIME_TITLE_BOTTOM        (MMISCH_TIME_TITLE_TOP + 18)//(MMISCH_TIME_TITLE_TOP + 22)
	#define MMISCH_TIME_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_TIME_TOP                 MMISCH_TIME_TITLE_TOP
	#define MMISCH_TIME_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_TIME_BOTTOM              MMISCH_TIME_TITLE_BOTTOM
	#define MMISCH_TIME_RECT				{MMISCH_TIME_LEFT, MMISCH_TIME_TOP, MMISCH_TIME_RIGHT, MMISCH_TIME_BOTTOM}

	//ring
	#define MMISCH_RING_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT    
	#define MMISCH_RING_TITLE_TOP           (MMISCH_TIME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_RING_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_RING_TITLE_BOTTOM        (MMISCH_RING_TITLE_TOP + 18)//(MMISCH_RING_TITLE_TOP + 22)
	#define MMISCH_RING_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_RING_TOP                 MMISCH_RING_TITLE_TOP
	#define MMISCH_RING_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_RING_BOTTOM              MMISCH_RING_TITLE_BOTTOM
	#define MMISCH_RING_RECT				{MMISCH_RING_LEFT, MMISCH_RING_TOP, MMISCH_RING_RIGHT, MMISCH_RING_BOTTOM}

	//freq
	#define MMISCH_FREQ_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT
	#define MMISCH_FREQ_TITLE_TOP           (MMISCH_RING_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_FREQ_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_FREQ_TITLE_BOTTOM        (MMISCH_FREQ_TITLE_TOP + 18)//(MMISCH_FREQ_TITLE_TOP + 22)
	#define MMISCH_FREQ_LEFT                MMISCH_NAME_LEFT//(MMISCH_FREQ_TITLE_RIGHT + 1)
	#define MMISCH_FREQ_TOP                 MMISCH_FREQ_TITLE_TOP 
	#define MMISCH_FREQ_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_FREQ_BOTTOM              MMISCH_FREQ_TITLE_BOTTOM + 2
	#define MMISCH_FREQ_RECT				{MMISCH_FREQ_LEFT, MMISCH_FREQ_TOP, MMISCH_FREQ_RIGHT, MMISCH_FREQ_BOTTOM}

	//freq week
	#define MMISCH_FREQ_WEEK_LEFT      		4
	#define MMISCH_FREQ_WEEK_TOP     		(MMISCH_FREQ_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT +4)
	#define MMISCH_FREQ_WEEK_RIGHT 			175//239
	#define MMISCH_FREQ_WEEK_BOTTOM        	(MMISCH_FREQ_WEEK_TOP + MMISCH_WEEK_ICON_HEIGH)
	#define MMISCH_FREQ_WEEK_RECT			{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM}
	#define MMISCH_FREQ_WEEK_FOCUS_RECT		{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM - 1}

	//deadline
	#define MMISCH_DEADLINE_TITLE_LEFT		MMISCH_TIME_TITLE_LEFT
	#define MMISCH_DEADLINE_TITLE_TOP		(MMISCH_FREQ_WEEK_BOTTOM + 4)
	#define MMISCH_DEADLINE_TITLE_RIGHT		(MMISCH_DEADLINE_TITLE_LEFT + 80)//(MMISCH_TIME_TITLE_RIGHT + 30)
	#define MMISCH_DEADLINE_TITLE_BOTTOM	(MMISCH_DEADLINE_TITLE_TOP + 18)
	#define MMISCH_DEADLINE_LEFT   			(MMISCH_DEADLINE_TITLE_RIGHT + 5)//(MMISCH_DEADLINE_TITLE_RIGHT + 1)
	#define MMISCH_DEADLINE_TOP  			MMISCH_DEADLINE_TITLE_TOP
	#define MMISCH_DEADLINE_RIGHT    		(MMI_MAINSCREEN_RIGHT_MAX_PIXEL - 8)//MMISCH_TIME_RIGHT
	#define MMISCH_DEADLINE_BOTTOM    		MMISCH_DEADLINE_TITLE_BOTTOM
	#define MMISCH_DEADLINE_RECT			{MMISCH_DEADLINE_LEFT, MMISCH_DEADLINE_TOP, MMISCH_DEADLINE_RIGHT, MMISCH_DEADLINE_BOTTOM}
	#define MMISCH_FOCUS_RECT				{(MMISCH_NAME_LEFT - 1), (MMISCH_NAME_TOP - 1), (MMISCH_NAME_RIGHT + 5), (MMISCH_NAME_BOTTOM + 1)}              

	// button控件的信息
	#define SCH_EDIT_BUTTON_HIGHT           24//42
	#define SCH_EDIT_BUTTON_WIDTH           24//34
	#define SCH_DROP_PAGE_ITEM_NUM  		2
	#define MMISCH_ICON_LINE_ITEM_MAX       6
	#define MMISCH_ICON_ROW_ITEM_MAX       	6

	#define MMISCH_VIEWLIST_BUTTON_TOP	6//暂时用大屏的数据，
	#define MMISCH_VIEWLIST_BUTTON_LF_LEFT	65
	#define MMISCH_VIEWLIST_BUTTON_HIGHT		11
	#define MMISCH_VIEWLIST_BUTTON_WIDTH		14
	#define MMISCH_VIEWLIST_BUTTON_RH_LEFT	65
#elif defined MAINLCD_SIZE_240X320
	#define MMISCH_FOCUS_HEIGHT             24
	#define MMISCH_SPACE_TWO_RECT          	9
	#define MMISCH_SPACE_RECT             	(22 + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_WEEK_ICON_WIDTH          33
	#define MMISCH_WEEK_ICON_HEIGH          42

	//name
	#define MMISCH_NAME_TITLE_LEFT          10
	#define MMISCH_NAME_TITLE_TOP           (MMI_TITLE_HEIGHT + 3)

	#define MMISCH_NAME_TITLE_RIGHT         (MMISCH_NAME_TITLE_LEFT + 60)

	#define MMISCH_NAME_TITLE_BOTTOM        (MMISCH_NAME_TITLE_TOP + 22)
	#define MMISCH_NAME_LEFT                (MMISCH_NAME_TITLE_RIGHT + 2)
	#define MMISCH_NAME_TOP                 MMISCH_NAME_TITLE_TOP
	#define MMISCH_NAME_RIGHT               (MMI_MAINSCREEN_RIGHT_MAX_PIXEL - 15)
	#define MMISCH_NAME_BOTTOM              MMISCH_NAME_TITLE_BOTTOM
	#define MMISCH_NAME_RECT				{MMISCH_NAME_LEFT, MMISCH_NAME_TOP, MMISCH_NAME_RIGHT, MMISCH_NAME_BOTTOM}

	//date
	#define MMISCH_DATE_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_DATE_TITLE_TOP           (MMISCH_NAME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_DATE_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_DATE_TITLE_BOTTOM        (MMISCH_DATE_TITLE_TOP + 22)
	#define MMISCH_DATE_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_DATE_TOP                 MMISCH_DATE_TITLE_TOP
	#define MMISCH_DATE_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_DATE_BOTTOM              MMISCH_DATE_TITLE_BOTTOM
	#define MMISCH_DATE_RECT				{MMISCH_DATE_LEFT, MMISCH_DATE_TOP, MMISCH_DATE_RIGHT, MMISCH_DATE_BOTTOM}
	//time
	#define MMISCH_TIME_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_TIME_TITLE_TOP           (MMISCH_DATE_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_TIME_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_TIME_TITLE_BOTTOM        (MMISCH_TIME_TITLE_TOP + 22)
	#define MMISCH_TIME_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_TIME_TOP                 MMISCH_TIME_TITLE_TOP
	#define MMISCH_TIME_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_TIME_BOTTOM              MMISCH_TIME_TITLE_BOTTOM
	#define MMISCH_TIME_RECT				{MMISCH_TIME_LEFT, MMISCH_TIME_TOP, MMISCH_TIME_RIGHT, MMISCH_TIME_BOTTOM}

	//ring
	#define MMISCH_RING_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT    
	#define MMISCH_RING_TITLE_TOP           (MMISCH_TIME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_RING_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_RING_TITLE_BOTTOM        (MMISCH_RING_TITLE_TOP + 22)
	#define MMISCH_RING_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_RING_TOP                 MMISCH_RING_TITLE_TOP
	#define MMISCH_RING_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_RING_BOTTOM              MMISCH_RING_TITLE_BOTTOM
	#define MMISCH_RING_RECT				{MMISCH_RING_LEFT, MMISCH_RING_TOP, MMISCH_RING_RIGHT, MMISCH_RING_BOTTOM}

	//freq
	#define MMISCH_FREQ_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT
	#define MMISCH_FREQ_TITLE_TOP           (MMISCH_RING_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_FREQ_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_FREQ_TITLE_BOTTOM        (MMISCH_FREQ_TITLE_TOP + 22)
	#define MMISCH_FREQ_LEFT                MMISCH_NAME_LEFT//(MMISCH_FREQ_TITLE_RIGHT + 1)
	#define MMISCH_FREQ_TOP                 MMISCH_FREQ_TITLE_TOP
	#define MMISCH_FREQ_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_FREQ_BOTTOM              MMISCH_FREQ_TITLE_BOTTOM +2//@zhaohui,cr107777
	#define MMISCH_FREQ_RECT				{MMISCH_FREQ_LEFT, MMISCH_FREQ_TOP, MMISCH_FREQ_RIGHT, MMISCH_FREQ_BOTTOM}

	//freq week
	#define MMISCH_FREQ_WEEK_LEFT      		4
	#define MMISCH_FREQ_WEEK_TOP     		(MMISCH_FREQ_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_FREQ_WEEK_RIGHT 			239
	#define MMISCH_FREQ_WEEK_BOTTOM        	(MMISCH_FREQ_WEEK_TOP + MMISCH_WEEK_ICON_HEIGH)
	#define MMISCH_FREQ_WEEK_RECT			{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM}
	#define MMISCH_FREQ_WEEK_FOCUS_RECT		{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM - 1}

	//deadline
	#define MMISCH_DEADLINE_TITLE_LEFT		MMISCH_TIME_TITLE_LEFT
	#define MMISCH_DEADLINE_TITLE_TOP		(MMISCH_FREQ_WEEK_BOTTOM + MMISCH_SPACE_TWO_RECT)
	//@cr112607 start,解决多国语言版本中截止时间显示与下拉框重叠的问题

	#define MMISCH_DEADLINE_TITLE_RIGHT		(MMISCH_TIME_TITLE_RIGHT + 40)//@zhaohui,cr107929

	//@cr112607 end
	#define MMISCH_DEADLINE_TITLE_BOTTOM	(MMISCH_DEADLINE_TITLE_TOP + 22)
	#define MMISCH_DEADLINE_LEFT   			(MMISCH_DEADLINE_TITLE_RIGHT + 1)
	#define MMISCH_DEADLINE_TOP  			MMISCH_DEADLINE_TITLE_TOP
	#define MMISCH_DEADLINE_RIGHT    		MMISCH_TIME_RIGHT
	#define MMISCH_DEADLINE_BOTTOM    		MMISCH_DEADLINE_TITLE_BOTTOM
	#define MMISCH_DEADLINE_RECT			{MMISCH_DEADLINE_LEFT, MMISCH_DEADLINE_TOP, MMISCH_DEADLINE_RIGHT, MMISCH_DEADLINE_BOTTOM}
	#define MMISCH_FOCUS_RECT				{(MMISCH_NAME_LEFT - 1), (MMISCH_NAME_TOP - 1), (MMISCH_NAME_RIGHT + 5), (MMISCH_NAME_BOTTOM + 1)}              

	// button控件的信息
	#define SCH_EDIT_BUTTON_HIGHT           42
	#define SCH_EDIT_BUTTON_WIDTH           34
	#define SCH_DROP_PAGE_ITEM_NUM  		3 
	#define MMISCH_ICON_LINE_ITEM_MAX       8
	#define MMISCH_ICON_ROW_ITEM_MAX       	8

	#define MMISCH_VIEWLIST_BUTTON_TOP	6
	#define MMISCH_VIEWLIST_BUTTON_LF_LEFT	65
	#define MMISCH_VIEWLIST_BUTTON_HIGHT		11
	#define MMISCH_VIEWLIST_BUTTON_WIDTH		14
	#define MMISCH_VIEWLIST_BUTTON_RH_LEFT	165
#elif defined MAINLCD_SIZE_128X64
	#define MMISCH_FOCUS_HEIGHT             24
	#define MMISCH_SPACE_TWO_RECT          	9
	#define MMISCH_SPACE_RECT             	(22 + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_WEEK_ICON_WIDTH          33
	#define MMISCH_WEEK_ICON_HEIGH          42

	//name
	#define MMISCH_NAME_TITLE_LEFT          10
	#define MMISCH_NAME_TITLE_TOP           (MMI_TITLE_HEIGHT + 3)

	#define MMISCH_NAME_TITLE_RIGHT         (MMISCH_NAME_TITLE_LEFT + 60)

	#define MMISCH_NAME_TITLE_BOTTOM        (MMISCH_NAME_TITLE_TOP + 22)
	#define MMISCH_NAME_LEFT                (MMISCH_NAME_TITLE_RIGHT + 2)
	#define MMISCH_NAME_TOP                 MMISCH_NAME_TITLE_TOP
	#define MMISCH_NAME_RIGHT               (MMI_MAINSCREEN_RIGHT_MAX_PIXEL - 15)
	#define MMISCH_NAME_BOTTOM              MMISCH_NAME_TITLE_BOTTOM
	#define MMISCH_NAME_RECT				{MMISCH_NAME_LEFT, MMISCH_NAME_TOP, MMISCH_NAME_RIGHT, MMISCH_NAME_BOTTOM}

	//date
	#define MMISCH_DATE_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_DATE_TITLE_TOP           (MMISCH_NAME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_DATE_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_DATE_TITLE_BOTTOM        (MMISCH_DATE_TITLE_TOP + 22)
	#define MMISCH_DATE_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_DATE_TOP                 MMISCH_DATE_TITLE_TOP
	#define MMISCH_DATE_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_DATE_BOTTOM              MMISCH_DATE_TITLE_BOTTOM
	#define MMISCH_DATE_RECT				{MMISCH_DATE_LEFT, MMISCH_DATE_TOP, MMISCH_DATE_RIGHT, MMISCH_DATE_BOTTOM}
	//time
	#define MMISCH_TIME_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_TIME_TITLE_TOP           (MMISCH_DATE_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_TIME_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_TIME_TITLE_BOTTOM        (MMISCH_TIME_TITLE_TOP + 22)
	#define MMISCH_TIME_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_TIME_TOP                 MMISCH_TIME_TITLE_TOP
	#define MMISCH_TIME_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_TIME_BOTTOM              MMISCH_TIME_TITLE_BOTTOM
	#define MMISCH_TIME_RECT				{MMISCH_TIME_LEFT, MMISCH_TIME_TOP, MMISCH_TIME_RIGHT, MMISCH_TIME_BOTTOM}

	//ring
	#define MMISCH_RING_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT    
	#define MMISCH_RING_TITLE_TOP           (MMISCH_TIME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_RING_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_RING_TITLE_BOTTOM        (MMISCH_RING_TITLE_TOP + 22)
	#define MMISCH_RING_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_RING_TOP                 MMISCH_RING_TITLE_TOP
	#define MMISCH_RING_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_RING_BOTTOM              MMISCH_RING_TITLE_BOTTOM
	#define MMISCH_RING_RECT				{MMISCH_RING_LEFT, MMISCH_RING_TOP, MMISCH_RING_RIGHT, MMISCH_RING_BOTTOM}

	//freq
	#define MMISCH_FREQ_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT
	#define MMISCH_FREQ_TITLE_TOP           (MMISCH_RING_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_FREQ_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_FREQ_TITLE_BOTTOM        (MMISCH_FREQ_TITLE_TOP + 22)
	#define MMISCH_FREQ_LEFT                MMISCH_NAME_LEFT//(MMISCH_FREQ_TITLE_RIGHT + 1)
	#define MMISCH_FREQ_TOP                 MMISCH_FREQ_TITLE_TOP
	#define MMISCH_FREQ_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_FREQ_BOTTOM              MMISCH_FREQ_TITLE_BOTTOM +2//@zhaohui,cr107777
	#define MMISCH_FREQ_RECT				{MMISCH_FREQ_LEFT, MMISCH_FREQ_TOP, MMISCH_FREQ_RIGHT, MMISCH_FREQ_BOTTOM}

	//freq week
	#define MMISCH_FREQ_WEEK_LEFT      		4
	#define MMISCH_FREQ_WEEK_TOP     		(MMISCH_FREQ_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_FREQ_WEEK_RIGHT 			239
	#define MMISCH_FREQ_WEEK_BOTTOM        	(MMISCH_FREQ_WEEK_TOP + MMISCH_WEEK_ICON_HEIGH)
	#define MMISCH_FREQ_WEEK_RECT			{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM}
	#define MMISCH_FREQ_WEEK_FOCUS_RECT		{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM - 1}

	//deadline
	#define MMISCH_DEADLINE_TITLE_LEFT		MMISCH_TIME_TITLE_LEFT
	#define MMISCH_DEADLINE_TITLE_TOP		(MMISCH_FREQ_WEEK_BOTTOM + MMISCH_SPACE_TWO_RECT)
	//@cr112607 start,解决多国语言版本中截止时间显示与下拉框重叠的问题

	#define MMISCH_DEADLINE_TITLE_RIGHT		(MMISCH_TIME_TITLE_RIGHT + 40)//@zhaohui,cr107929

	//@cr112607 end
	#define MMISCH_DEADLINE_TITLE_BOTTOM	(MMISCH_DEADLINE_TITLE_TOP + 22)
	#define MMISCH_DEADLINE_LEFT   			(MMISCH_DEADLINE_TITLE_RIGHT + 1)
	#define MMISCH_DEADLINE_TOP  			MMISCH_DEADLINE_TITLE_TOP
	#define MMISCH_DEADLINE_RIGHT    		MMISCH_TIME_RIGHT
	#define MMISCH_DEADLINE_BOTTOM    		MMISCH_DEADLINE_TITLE_BOTTOM
	#define MMISCH_DEADLINE_RECT			{MMISCH_DEADLINE_LEFT, MMISCH_DEADLINE_TOP, MMISCH_DEADLINE_RIGHT, MMISCH_DEADLINE_BOTTOM}
	#define MMISCH_FOCUS_RECT				{(MMISCH_NAME_LEFT - 1), (MMISCH_NAME_TOP - 1), (MMISCH_NAME_RIGHT + 5), (MMISCH_NAME_BOTTOM + 1)}              

	// button控件的信息
	#define SCH_EDIT_BUTTON_HIGHT           42
	#define SCH_EDIT_BUTTON_WIDTH           34
	#define SCH_DROP_PAGE_ITEM_NUM  		3 
	#define MMISCH_ICON_LINE_ITEM_MAX       8
	#define MMISCH_ICON_ROW_ITEM_MAX       	8

	#define MMISCH_VIEWLIST_BUTTON_TOP	6
	#define MMISCH_VIEWLIST_BUTTON_LF_LEFT	65
	#define MMISCH_VIEWLIST_BUTTON_HIGHT		11
	#define MMISCH_VIEWLIST_BUTTON_WIDTH		14
	#define MMISCH_VIEWLIST_BUTTON_RH_LEFT	165	
#elif defined MAINLCD_SIZE_240X400
	#define MMISCH_FOCUS_HEIGHT             24
	#define MMISCH_SPACE_TWO_RECT          	18
	#define MMISCH_SPACE_RECT             	(22 + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_WEEK_ICON_WIDTH          33
	#define MMISCH_WEEK_ICON_HEIGH          42

	//name
	#define MMISCH_NAME_TITLE_LEFT          10
	#define MMISCH_NAME_TITLE_TOP           (MMI_TITLE_HEIGHT + 10)

	#define MMISCH_NAME_TITLE_RIGHT         (MMISCH_NAME_TITLE_LEFT + 60)

	#define MMISCH_NAME_TITLE_BOTTOM        (MMISCH_NAME_TITLE_TOP + 22)
	#define MMISCH_NAME_LEFT                (MMISCH_NAME_TITLE_RIGHT + 2)
	#define MMISCH_NAME_TOP                 MMISCH_NAME_TITLE_TOP
	#define MMISCH_NAME_RIGHT               (MMI_MAINSCREEN_RIGHT_MAX_PIXEL - 15)
	#define MMISCH_NAME_BOTTOM              MMISCH_NAME_TITLE_BOTTOM
	#define MMISCH_NAME_RECT				{MMISCH_NAME_LEFT, MMISCH_NAME_TOP, MMISCH_NAME_RIGHT, MMISCH_NAME_BOTTOM}

	//date
	#define MMISCH_DATE_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_DATE_TITLE_TOP           (MMISCH_NAME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_DATE_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_DATE_TITLE_BOTTOM        (MMISCH_DATE_TITLE_TOP + 22)
	#define MMISCH_DATE_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_DATE_TOP                 MMISCH_DATE_TITLE_TOP
	#define MMISCH_DATE_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_DATE_BOTTOM              MMISCH_DATE_TITLE_BOTTOM
	#define MMISCH_DATE_RECT				{MMISCH_DATE_LEFT, MMISCH_DATE_TOP, MMISCH_DATE_RIGHT, MMISCH_DATE_BOTTOM}
	//time
	#define MMISCH_TIME_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_TIME_TITLE_TOP           (MMISCH_DATE_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_TIME_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_TIME_TITLE_BOTTOM        (MMISCH_TIME_TITLE_TOP + 22)
	#define MMISCH_TIME_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_TIME_TOP                 MMISCH_TIME_TITLE_TOP
	#define MMISCH_TIME_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_TIME_BOTTOM              MMISCH_TIME_TITLE_BOTTOM
	#define MMISCH_TIME_RECT				{MMISCH_TIME_LEFT, MMISCH_TIME_TOP, MMISCH_TIME_RIGHT, MMISCH_TIME_BOTTOM}

	//ring
	#define MMISCH_RING_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT    
	#define MMISCH_RING_TITLE_TOP           (MMISCH_TIME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_RING_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_RING_TITLE_BOTTOM        (MMISCH_RING_TITLE_TOP + 22)
	#define MMISCH_RING_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_RING_TOP                 MMISCH_RING_TITLE_TOP
	#define MMISCH_RING_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_RING_BOTTOM              MMISCH_RING_TITLE_BOTTOM
	#define MMISCH_RING_RECT				{MMISCH_RING_LEFT, MMISCH_RING_TOP, MMISCH_RING_RIGHT, MMISCH_RING_BOTTOM}

	//freq
	#define MMISCH_FREQ_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT
	#define MMISCH_FREQ_TITLE_TOP           (MMISCH_RING_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_FREQ_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_FREQ_TITLE_BOTTOM        (MMISCH_FREQ_TITLE_TOP + 22)
	#define MMISCH_FREQ_LEFT                MMISCH_NAME_LEFT//(MMISCH_FREQ_TITLE_RIGHT + 1)
	#define MMISCH_FREQ_TOP                 MMISCH_FREQ_TITLE_TOP
	#define MMISCH_FREQ_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_FREQ_BOTTOM              MMISCH_FREQ_TITLE_BOTTOM + 2//@zhaohui,cr107777
	#define MMISCH_FREQ_RECT				{MMISCH_FREQ_LEFT, MMISCH_FREQ_TOP, MMISCH_FREQ_RIGHT, MMISCH_FREQ_BOTTOM}

	//freq week
	#define MMISCH_FREQ_WEEK_LEFT      		4
	#define MMISCH_FREQ_WEEK_TOP     		(MMISCH_FREQ_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT*2)
	#define MMISCH_FREQ_WEEK_RIGHT 			239
	#define MMISCH_FREQ_WEEK_BOTTOM        	(MMISCH_FREQ_WEEK_TOP + MMISCH_WEEK_ICON_HEIGH)
	#define MMISCH_FREQ_WEEK_RECT			{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM}
	#define MMISCH_FREQ_WEEK_FOCUS_RECT		{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM - 1}

	//deadline
	#define MMISCH_DEADLINE_TITLE_LEFT		MMISCH_TIME_TITLE_LEFT
	#define MMISCH_DEADLINE_TITLE_TOP		(MMISCH_FREQ_WEEK_BOTTOM + MMISCH_SPACE_TWO_RECT)
	//@cr112607 start,解决多国语言版本中截止时间显示与下拉框重叠的问题

	#define MMISCH_DEADLINE_TITLE_RIGHT		(MMISCH_TIME_TITLE_RIGHT + 40)//@zhaohui,cr107929

	//@cr112607 end
	#define MMISCH_DEADLINE_TITLE_BOTTOM	(MMISCH_DEADLINE_TITLE_TOP + 22)
	#define MMISCH_DEADLINE_LEFT   			(MMISCH_DEADLINE_TITLE_RIGHT + 1)
	#define MMISCH_DEADLINE_TOP  			MMISCH_DEADLINE_TITLE_TOP
	#define MMISCH_DEADLINE_RIGHT    		MMISCH_TIME_RIGHT
	#define MMISCH_DEADLINE_BOTTOM    		MMISCH_DEADLINE_TITLE_BOTTOM
	#define MMISCH_DEADLINE_RECT			{MMISCH_DEADLINE_LEFT, MMISCH_DEADLINE_TOP, MMISCH_DEADLINE_RIGHT, MMISCH_DEADLINE_BOTTOM}
	#define MMISCH_FOCUS_RECT				{(MMISCH_NAME_LEFT - 1), (MMISCH_NAME_TOP - 1), (MMISCH_NAME_RIGHT + 5), (MMISCH_NAME_BOTTOM + 1)}              

	// button控件的信息
	#define SCH_EDIT_BUTTON_HIGHT           42
	#define SCH_EDIT_BUTTON_WIDTH           34
	#define SCH_DROP_PAGE_ITEM_NUM  		3 
	#define MMISCH_ICON_LINE_ITEM_MAX       8
	#define MMISCH_ICON_ROW_ITEM_MAX       	8

	#define MMISCH_VIEWLIST_BUTTON_TOP	6
	#define MMISCH_VIEWLIST_BUTTON_LF_LEFT	65
	#define MMISCH_VIEWLIST_BUTTON_HIGHT		11
	#define MMISCH_VIEWLIST_BUTTON_WIDTH		14
	#define MMISCH_VIEWLIST_BUTTON_RH_LEFT	165

#elif defined MAINLCD_SIZE_320X480
	#define MMISCH_FOCUS_HEIGHT             24
	#define MMISCH_SPACE_TWO_RECT          	18
	#define MMISCH_SPACE_RECT             	(22 + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_WEEK_ICON_WIDTH          33
	#define MMISCH_WEEK_ICON_HEIGH          42

	//name
	#define MMISCH_NAME_TITLE_LEFT          10
	#define MMISCH_NAME_TITLE_TOP           (MMI_TITLE_HEIGHT + 10)

	#define MMISCH_NAME_TITLE_RIGHT         (MMISCH_NAME_TITLE_LEFT + 60)

	#define MMISCH_NAME_TITLE_BOTTOM        (MMISCH_NAME_TITLE_TOP + 22)
	#define MMISCH_NAME_LEFT                (MMISCH_NAME_TITLE_RIGHT + 2)
	#define MMISCH_NAME_TOP                 MMISCH_NAME_TITLE_TOP
	#define MMISCH_NAME_RIGHT               (MMI_MAINSCREEN_RIGHT_MAX_PIXEL - 15)
	#define MMISCH_NAME_BOTTOM              MMISCH_NAME_TITLE_BOTTOM
	#define MMISCH_NAME_RECT				{MMISCH_NAME_LEFT, MMISCH_NAME_TOP, MMISCH_NAME_RIGHT, MMISCH_NAME_BOTTOM}

	//date
	#define MMISCH_DATE_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_DATE_TITLE_TOP           (MMISCH_NAME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_DATE_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_DATE_TITLE_BOTTOM        (MMISCH_DATE_TITLE_TOP + 22)
	#define MMISCH_DATE_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_DATE_TOP                 MMISCH_DATE_TITLE_TOP
	#define MMISCH_DATE_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_DATE_BOTTOM              MMISCH_DATE_TITLE_BOTTOM
	#define MMISCH_DATE_RECT				{MMISCH_DATE_LEFT, MMISCH_DATE_TOP, MMISCH_DATE_RIGHT, MMISCH_DATE_BOTTOM}
	//time
	#define MMISCH_TIME_TITLE_LEFT          MMISCH_NAME_TITLE_LEFT
	#define MMISCH_TIME_TITLE_TOP           (MMISCH_DATE_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_TIME_TITLE_RIGHT         MMISCH_NAME_TITLE_RIGHT
	#define MMISCH_TIME_TITLE_BOTTOM        (MMISCH_TIME_TITLE_TOP + 22)
	#define MMISCH_TIME_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_TIME_TOP                 MMISCH_TIME_TITLE_TOP
	#define MMISCH_TIME_RIGHT               MMISCH_NAME_RIGHT
	#define MMISCH_TIME_BOTTOM              MMISCH_TIME_TITLE_BOTTOM
	#define MMISCH_TIME_RECT				{MMISCH_TIME_LEFT, MMISCH_TIME_TOP, MMISCH_TIME_RIGHT, MMISCH_TIME_BOTTOM}

	//ring
	#define MMISCH_RING_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT    
	#define MMISCH_RING_TITLE_TOP           (MMISCH_TIME_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_RING_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_RING_TITLE_BOTTOM        (MMISCH_RING_TITLE_TOP + 22)
	#define MMISCH_RING_LEFT                MMISCH_NAME_LEFT
	#define MMISCH_RING_TOP                 MMISCH_RING_TITLE_TOP
	#define MMISCH_RING_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_RING_BOTTOM              MMISCH_RING_TITLE_BOTTOM
	#define MMISCH_RING_RECT				{MMISCH_RING_LEFT, MMISCH_RING_TOP, MMISCH_RING_RIGHT, MMISCH_RING_BOTTOM}

	//freq
	#define MMISCH_FREQ_TITLE_LEFT          MMISCH_TIME_TITLE_LEFT
	#define MMISCH_FREQ_TITLE_TOP           (MMISCH_RING_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT)
	#define MMISCH_FREQ_TITLE_RIGHT         MMISCH_TIME_TITLE_RIGHT
	#define MMISCH_FREQ_TITLE_BOTTOM        (MMISCH_FREQ_TITLE_TOP + 22)
	#define MMISCH_FREQ_LEFT                MMISCH_NAME_LEFT//(MMISCH_FREQ_TITLE_RIGHT + 1)
	#define MMISCH_FREQ_TOP                 MMISCH_FREQ_TITLE_TOP
	#define MMISCH_FREQ_RIGHT               MMISCH_TIME_RIGHT
	#define MMISCH_FREQ_BOTTOM              MMISCH_FREQ_TITLE_BOTTOM + 2//@zhaohui,cr107777
	#define MMISCH_FREQ_RECT				{MMISCH_FREQ_LEFT, MMISCH_FREQ_TOP, MMISCH_FREQ_RIGHT, MMISCH_FREQ_BOTTOM}

	//freq week
	#define MMISCH_FREQ_WEEK_LEFT      		4
	#define MMISCH_FREQ_WEEK_TOP     		(MMISCH_FREQ_TITLE_BOTTOM + MMISCH_SPACE_TWO_RECT*2)
	#define MMISCH_FREQ_WEEK_RIGHT 			239
	#define MMISCH_FREQ_WEEK_BOTTOM        	(MMISCH_FREQ_WEEK_TOP + MMISCH_WEEK_ICON_HEIGH)
	#define MMISCH_FREQ_WEEK_RECT			{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM}
	#define MMISCH_FREQ_WEEK_FOCUS_RECT		{MMISCH_FREQ_WEEK_LEFT, MMISCH_FREQ_WEEK_TOP,MMISCH_FREQ_WEEK_RIGHT, MMISCH_FREQ_WEEK_BOTTOM - 1}

	//deadline
	#define MMISCH_DEADLINE_TITLE_LEFT		MMISCH_TIME_TITLE_LEFT
	#define MMISCH_DEADLINE_TITLE_TOP		(MMISCH_FREQ_WEEK_BOTTOM + MMISCH_SPACE_TWO_RECT)
	//@cr112607 start,解决多国语言版本中截止时间显示与下拉框重叠的问题

	#define MMISCH_DEADLINE_TITLE_RIGHT		(MMISCH_TIME_TITLE_RIGHT + 40)//@zhaohui,cr107929

	//@cr112607 end
	#define MMISCH_DEADLINE_TITLE_BOTTOM	(MMISCH_DEADLINE_TITLE_TOP + 22)
	#define MMISCH_DEADLINE_LEFT   			(MMISCH_DEADLINE_TITLE_RIGHT + 1)
	#define MMISCH_DEADLINE_TOP  			MMISCH_DEADLINE_TITLE_TOP
	#define MMISCH_DEADLINE_RIGHT    		MMISCH_TIME_RIGHT
	#define MMISCH_DEADLINE_BOTTOM    		MMISCH_DEADLINE_TITLE_BOTTOM
	#define MMISCH_DEADLINE_RECT			{MMISCH_DEADLINE_LEFT, MMISCH_DEADLINE_TOP, MMISCH_DEADLINE_RIGHT, MMISCH_DEADLINE_BOTTOM}
	#define MMISCH_FOCUS_RECT				{(MMISCH_NAME_LEFT - 1), (MMISCH_NAME_TOP - 1), (MMISCH_NAME_RIGHT + 5), (MMISCH_NAME_BOTTOM + 1)}              

	// button控件的信息
	#define SCH_EDIT_BUTTON_HIGHT           42
	#define SCH_EDIT_BUTTON_WIDTH           34
	#define SCH_DROP_PAGE_ITEM_NUM  		3 
	#define MMISCH_ICON_LINE_ITEM_MAX       8
	#define MMISCH_ICON_ROW_ITEM_MAX       	8

	#define MMISCH_VIEWLIST_BUTTON_TOP	6
	#define MMISCH_VIEWLIST_BUTTON_LF_LEFT	65
	#define MMISCH_VIEWLIST_BUTTON_HIGHT		11
	#define MMISCH_VIEWLIST_BUTTON_WIDTH		14
	#define MMISCH_VIEWLIST_BUTTON_RH_LEFT	165

#else
    #error	
#endif

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif
#endif
#endif //_MMISCHEDULE_POSITION_H

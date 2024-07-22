/*****************************************************************************
** File Name:      watch_alarm_main.c      //alarm settings                  *
** Author:         qi.liu1                                                   *
** Date:           26/2/2021                                                 *
** Copyright:      All Rights Reserved.                                      *
** Description:  alarm list window                                           *
******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 03/2021      qi.liu1              Creat                                   *
******************************************************************************/

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "guilistbox.h"
#include "watch_common_list.h"
#include "os_api.h"
#include "sci_types.h"
#include "window_parse.h"
#include "mmk_type.h"
#include "guilabel.h"
#include "guibutton.h"
#include "mmidisplay_data.h"

#include "mmicom_trace.h"
#include "mmiset_image.h"
#include "mmialarm_image.h"
#include "mmiset_text.h"
#include "mmialarm_id.h"
#include "mmiset_export.h"
#include "mmiacc_event.h"
#include "mmialarm_export.h"

#include "watch_commonwin_export.h"
#include "watch_set_alarmedit.h"
#include "watch_set_position.h"
#include "watch_alarm_main.h"
#include "watch_alarm_edit.h"
#ifdef ADULT_WATCH_SUPPORT
#include "mmiphone_export.h"
#endif
#include "watch_common.h"
#include "ctrllabel_export.h"
#include "mmialarm_text.h"



/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/
#define WATCH_ALARM_LIST_ITEM_MAX               (3)
#define ALARM_DATE_DISPLAY_SUNDAY               (0)
#define ALARM_DATE_DISPLAY_MONDAY               (1)
#define ALARM_DATE_DISPLAY_TUESDAY              (2)
#define ALARM_DATE_DISPLAY_WEDNESDAY            (3)
#define ALARM_DATE_DISPLAY_THURSDAY             (4)
#define ALARM_DATE_DISPLAY_FIRDAY               (5)
#define ALARM_DATE_DISPLAY_SATURDAY             (6)
#define ALARM_ITEM_CONTENT_SELECT_INDEX         (2)

/**--------------------------------------------------------------------------*
 **                         STATIC DEFINITION                                *
 **--------------------------------------------------------------------------*/
LOCAL uint16 s_alarm_del_index = 0;
LOCAL uint8 s_adult_alarm_week_mask[ ALM_MODE_WEEK_NUM ] = { MASK_B0, MASK_B1, MASK_B2, MASK_B3, MASK_B4, MASK_B5, MASK_B6 };
LOCAL THEMELIST_ITEM_STYLE_T   s_item_style = {0};
LOCAL int32 s_offset_y = 0;  //bug:1590776 
PUBLIC ALARM_EDITINFO_TABLE_T       pEditInfoTable = {0};
LOCAL int8 tp_down_item_index = -1;

extern PUBLIC void WatchAlarm_EditListWin_Enter( uint32 event_id );

LOCAL BOOLEAN AlarmConver24HourTo12Hour( uint8 *hour_ptr )
{
    BOOLEAN result = FALSE;

    if( PNULL == hour_ptr )
    {
        TRACE_APP_ALARM("hour_ptr is PNULL!");
    }

    if( (12 > *hour_ptr) ) //AM
    {
        result = TRUE;

        if( (1 > *hour_ptr) ) //00:00-00:59
        {
            *hour_ptr = (uint8)(*hour_ptr+12);
        }
    }
    else //PM
    {
        if( 12 == *hour_ptr ) //12:00-12:59
        {
        }
        else
        {
            *hour_ptr = (uint8)(*hour_ptr-12);
        }
    }

    return result;
}

/*****************************************************************************/
//  Description : AlarmMainListGetAlarmNum
//  Parameter: [In] None
//             [Out] None
//  Author: qi.liu1
//  Note:获取当前闹钟个数
/*****************************************************************************/
LOCAL uint16 AlarmMainListGetAlarmNum( void )
{
    uint16      index = 0;
    uint16      alarmNum = 0;

    for( index = 0; index < ALM_ALARM_NUM; index++ )
    {
        const MMIACC_SMART_EVENT_FAST_TABLE_T* pEventTable = PNULL;

        pEventTable = MMIALM_GeEvent( (uint16)(EVENT_ALARM_0+index) );
        if( PNULL == pEventTable )
        {
            TRACE_APP_ALARM("pEventTable is PNULL");
            return alarmNum;
        }
        if( TRUE == pEventTable->is_valid )
        {
            alarmNum++;
        }
    }
    return alarmNum;
}

/*****************************************************************************/
//  Description : AlarmListUpdeteButton
//  Parameter: [In] event_num
//             [Out] None
//  Author: qi.liu1
//  Note:根据闹钟个数，更新button图片
/*****************************************************************************/
LOCAL void AlarmListUpdeteButton( uint16 alarm_num )
{
    BOOLEAN         reCode = TRUE;
    MMI_CTRL_ID_T   addBtn = MMIALM_ADD_ALARM_BTN_CTRL_ID;
    GUI_BG_T    bg_info ={0};

    if( alarm_num >= ALM_ALARM_NUM )
    {
        reCode = GUIBUTTON_SetVisible( addBtn, ALARM_IS_ADD_BUTTON_VISIBLE, FALSE );
        bg_info.bg_type = ALARM_ADD_BUTTON_TYPE;
        bg_info.img_id = ALARM_ADD_BUTTON_IMAGE;
        CTRLBUTTON_SetBg(addBtn,&bg_info);
    }
    else
    {
        reCode = GUIBUTTON_SetVisible( addBtn, TRUE, FALSE );
        bg_info.bg_type = GUI_BG_IMG;
        bg_info.img_id = res_aw_alarm_ic_done;
        CTRLBUTTON_SetBg(addBtn,&bg_info);
    }

}

/*****************************************************************************/
//  Description : AlarmIsWorkDay
//  Parameter: [In] event_id
//             [Out]
//  Author: qi.liu1
//  Note:当前日期是否为工作日
/*****************************************************************************/
LOCAL BOOLEAN AlarmIsWorkDay(uint16 event_id)
{
    const MMIACC_SMART_EVENT_FAST_TABLE_T*        pEventTable = PNULL;

    pEventTable = MMIALM_GeEvent( event_id );

    if(PNULL == pEventTable)
    {
        TRACE_APP_ALARM("pEventTable = PNULL!");
    }

    TRACE_APP_ALARM("event_id = %d",event_id);
    if( ALM_MODE_EVERYWEEK == pEventTable->fre_mode )
    {
        //MMI_STRING_T item_str={0};
        if((s_adult_alarm_week_mask[1] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[2] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[3] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[4] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[5] & pEventTable->fre)
        &&!(s_adult_alarm_week_mask[0] & pEventTable->fre)
        &&!(s_adult_alarm_week_mask[6] & pEventTable->fre))
        {
           TRACE_APP_ALARM("Alarm is Work Day");
           return TRUE;
        }
        else
        {
           TRACE_APP_ALARM("Alarm is not Work Day");
           return FALSE;
        }
    }
    else
    {
        TRACE_APP_ALARM("fre_mode error!!");
        return FALSE;
    }

}

/*****************************************************************************/
//  Description : AlarmIsEveryDay
//  Parameter: [In] event_id
//             [Out]
//  Author: qi.liu1
//  Note:当前日期是否为每天
/*****************************************************************************/
LOCAL BOOLEAN AlarmIsEveryDay(uint16 event_id)
{
    const MMIACC_SMART_EVENT_FAST_TABLE_T*        pEventTable = PNULL;

    pEventTable = MMIALM_GeEvent( event_id );

    if(PNULL == pEventTable)
    {
        TRACE_APP_ALARM("pEventTable = PNULL!");
    }

    TRACE_APP_ALARM("event_id = %d",event_id);
    if( ALM_MODE_EVERYWEEK == pEventTable->fre_mode )
    {
        //MMI_STRING_T item_str={0};
        if((s_adult_alarm_week_mask[0] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[1] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[2] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[3] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[4] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[5] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[6] & pEventTable->fre))
        {
           TRACE_APP_ALARM("Alarm is Every Day");
           return TRUE;
        }
        else
        {
           TRACE_APP_ALARM("Alarm is not Every Day");
           return FALSE;
        }
    }
    else
    {
        TRACE_APP_ALARM("fre_mode error!!");
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : AlarmListGetRepeatStr
//  Parameter: [In]event_id
//             [In]repeat_str_ptr
//             [Out]
//  Author: qi.liu1
//  Note:获取显示重复日期的字串 如 一 二 .../工作日/每天
/*****************************************************************************/
LOCAL BOOLEAN AlarmListGetRepeatStr(uint16 event_id, MMI_STRING_T* repeat_str_ptr)
{
    const MMIACC_SMART_EVENT_FAST_TABLE_T*        pEventTable = PNULL;
    MMI_STRING_T repeat_wstr = {0};

    pEventTable = MMIALM_GeEvent( event_id );

    if(PNULL == pEventTable)
    {
        TRACE_APP_ALARM("pEventTable = PNULL!");
        return FALSE;
    }

    if(PNULL == repeat_str_ptr)
    {
        TRACE_APP_ALARM("repeat_str_ptr = PNULL!");
        return FALSE;
    }

    if( ALM_MODE_EVERYWEEK == pEventTable->fre_mode )
    {
        uint16 index = 0;

        if(TRUE == AlarmIsEveryDay(event_id))
        {
            //display Every day
            MMI_GetLabelTextByLang(TXT_ALARM_EVERYDAY, repeat_str_ptr);
        }
        else if(TRUE == AlarmIsWorkDay(event_id))
        {
            //display Work day
            MMI_GetLabelTextByLang(TXT_ALARM_WORKDAY, repeat_str_ptr);
        }
        else
        {
            //display 一二... e.g.
            for( index = 0; index < ALM_MODE_WEEK_NUM; index++ )
            {
                if( s_adult_alarm_week_mask[index] & pEventTable->fre )
                {
                    switch(index)
                    {
                        case ALARM_DATE_DISPLAY_SUNDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_SUNDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_MONDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_MONDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_TUESDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_TUESDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_WEDNESDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_WEDNESDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_THURSDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_THURSDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_FIRDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_FRIDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_SATURDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_SATURDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            //temp debug	
            SCI_TRACE_MID("[MMI] MMI_CheckAllocatedMemInfo file:%s, line:%d", _D_FILE_NAME, _D_FILE_LINE);
            MMI_CheckAllocatedMemInfo();
        }
    }
    else
    {
        //display No repeat
        if( ALM_MODE_ONCE == pEventTable->fre_mode )
        {
            MMI_GetLabelTextByLang( TXT_ALARM_NOREPEAT, repeat_str_ptr );
        }
        else
        {
            TRACE_APP_ALARM("fre_mode=%d error!!",pEventTable->fre_mode);
        }
    }
    return TRUE;
}

/*****************************************************************************/
//  Description : AlarmIsWorkDay
//  Parameter: [In] event_id
//             [Out]
//  Author: qi.liu1
//  Note:当前日期是否为工作日
/*****************************************************************************/
LOCAL BOOLEAN AlarmIsWorkDayEx(ALARM_EDITINFO_TABLE_T pAlarmInfoTable)
{
    ALARM_EDITINFO_TABLE_T*        pEventTable = &pAlarmInfoTable;

    //pEventTable = MMIALM_GeEvent( event_id );

    if(PNULL == pEventTable)
    {
        TRACE_APP_ALARM("pEventTable = PNULL!");
    }

    //TRACE_APP_ALARM("event_id = %d",event_id);
    if( ALM_MODE_EVERYWEEK == pEventTable->fre_mode )
    {
        //MMI_STRING_T item_str={0};
        if((s_adult_alarm_week_mask[1] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[2] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[3] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[4] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[5] & pEventTable->fre)
        &&!(s_adult_alarm_week_mask[0] & pEventTable->fre)
        &&!(s_adult_alarm_week_mask[6] & pEventTable->fre))
        {
           TRACE_APP_ALARM("Alarm is Work Day");
           return TRUE;
        }
        else
        {
           TRACE_APP_ALARM("Alarm is not Work Day");
           return FALSE;
        }
    }
    else
    {
        TRACE_APP_ALARM("fre_mode error!!");
        return FALSE;
    }

}

/*****************************************************************************/
//  Description : AlarmIsEveryDay
//  Parameter: [In] event_id
//             [Out]
//  Author: qi.liu1
//  Note:当前日期是否为每天
/*****************************************************************************/
LOCAL BOOLEAN AlarmIsEveryDayEx(ALARM_EDITINFO_TABLE_T pAlarmInfoTable)
{
   ALARM_EDITINFO_TABLE_T*        pEventTable = &pAlarmInfoTable;

    //pEventTable = MMIALM_GeEvent( event_id );

    if(PNULL == pEventTable)
    {
        TRACE_APP_ALARM("pEventTable = PNULL!");
    }

    //TRACE_APP_ALARM("event_id = %d",event_id);
    if( ALM_MODE_EVERYWEEK == pEventTable->fre_mode )
    {
        //MMI_STRING_T item_str={0};
        if((s_adult_alarm_week_mask[0] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[1] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[2] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[3] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[4] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[5] & pEventTable->fre)
        &&(s_adult_alarm_week_mask[6] & pEventTable->fre))
        {
           TRACE_APP_ALARM("Alarm is Every Day");
           return TRUE;
        }
        else
        {
           TRACE_APP_ALARM("Alarm is not Every Day");
           return FALSE;
        }
    }
    else
    {
        TRACE_APP_ALARM("fre_mode error!!");
        return FALSE;
    }

    return TRUE;
}

/*****************************************************************************/
//  Description : AlarmListGetRepeatStr
//  Parameter: [In]event_id
//             [In]repeat_str_ptr
//             [Out]
//  Author: qi.liu1
//  Note:获取显示重复日期的字串 如 一 二 .../工作日/每天
/*****************************************************************************/
LOCAL BOOLEAN AlarmListGetRepeatStrEx(ALARM_EDITINFO_TABLE_T pAlarmInfoTable, MMI_STRING_T* repeat_str_ptr)
{
    ALARM_EDITINFO_TABLE_T*        pEventTable = &pAlarmInfoTable;
    MMI_STRING_T repeat_wstr = {0};

    //pEventTable = MMIALM_GeEvent( event_id );

    if(PNULL == pEventTable)
    {
        TRACE_APP_ALARM("pEventTable = PNULL!");
        return FALSE;
    }

    if(PNULL == repeat_str_ptr)
    {
        TRACE_APP_ALARM("repeat_str_ptr = PNULL!");
        return FALSE;
    }

    if( ALM_MODE_EVERYWEEK == pEventTable->fre_mode )
    {
        uint16 index = 0;

        if(TRUE == AlarmIsEveryDayEx(pAlarmInfoTable))
        {
            //display Every day
            MMI_GetLabelTextByLang(TXT_ALARM_EVERYDAY, repeat_str_ptr);
        }
        else if(TRUE == AlarmIsWorkDayEx(pAlarmInfoTable))
        {
            //display Work day
            MMI_GetLabelTextByLang(TXT_ALARM_WORKDAY, repeat_str_ptr);
        }
        else
        {
            //display 一二... e.g.
            for( index = 0; index < ALM_MODE_WEEK_NUM; index++ )
            {
                if( s_adult_alarm_week_mask[index] & pEventTable->fre )
                {
                    switch(index)
                    {
                        case ALARM_DATE_DISPLAY_SUNDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_SUNDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_MONDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_MONDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_TUESDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_TUESDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_WEDNESDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_WEDNESDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_THURSDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_THURSDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_FIRDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_FRIDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        case ALARM_DATE_DISPLAY_SATURDAY:
                        {
                            MMI_GetLabelTextByLang(TXT_SHORT_IDLE_SATURDAY, &repeat_wstr);
                            MMIAPICOM_Wstrncpy((repeat_str_ptr->wstr_ptr+repeat_str_ptr->wstr_len),repeat_wstr.wstr_ptr,repeat_wstr.wstr_len);
                            repeat_str_ptr->wstr_len += repeat_wstr.wstr_len;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            //temp debug	
            SCI_TRACE_MID("[MMI] MMI_CheckAllocatedMemInfo file:%s, line:%d", _D_FILE_NAME, _D_FILE_LINE);
            MMI_CheckAllocatedMemInfo();
        }
    }
    else
    {
        //display No repeat
        if( ALM_MODE_ONCE == pEventTable->fre_mode )
        {
            MMI_GetLabelTextByLang( TXT_ALARM_NOREPEAT, repeat_str_ptr );
        }
        else
        {
            TRACE_APP_ALARM("fre_mode=%d error!!",pEventTable->fre_mode);
        }
    }
    return TRUE;
}

/*****************************************************************************/
//  Description : AlarmListGetTimeStr
//  Parameter: [In] sys_time_ptr
//             [Out] time_str_ptr
//  Author: qi.liu1
//  Note:获取闹钟显示时间
/*****************************************************************************/
LOCAL BOOLEAN AlarmListGetTimeStr( SCI_TIME_T* sys_time_ptr, MMI_STRING_T* time_str_ptr )
{
    BOOLEAN     isAM = FALSE;
    uint8       timeStr[WATCH_ALARM_TEMP_STR_LEN] = { 0 };

    if( PNULL == time_str_ptr || PNULL == sys_time_ptr )
    {
        TRACE_APP_ALARM("input is PNULL!");
        return FALSE;
    }

    if( MMISET_TIME_12HOURS == MMIAPISET_GetTimeDisplayType() )
    {
        isAM = AlarmConver24HourTo12Hour( &(sys_time_ptr->hour) );
        if( TRUE == isAM )
        {
            sprintf( (char*)timeStr,"%02d:%02d AM", sys_time_ptr->hour, sys_time_ptr->min );
        }
        else
        {
            sprintf( (char*)timeStr,"%02d:%02d PM", sys_time_ptr->hour, sys_time_ptr->min );
        }
    }
    else
    {
        sprintf( (char*)timeStr,"%02d:%02d", sys_time_ptr->hour, sys_time_ptr->min );
    }
    MMIAPICOM_StrToWstr( timeStr, time_str_ptr->wstr_ptr );
    time_str_ptr->wstr_len = MMIAPICOM_Wstrlen( time_str_ptr->wstr_ptr );
    return TRUE;
}

/*****************************************************************************/
//  Description : AlarmListDrawList
//  Parameter: [In] win_id
//             [In] list_ctr_id
//             [In] event_num
//             [Out] None
//  Author: qi.liu1
//  Note:绘制闹钟list
/*****************************************************************************/
LOCAL void AlarmListDrawList( MMI_WIN_ID_T win_id, MMI_CTRL_ID_T list_ctr_id, uint16 event_num )
{
    MMI_IMAGE_ID_T onoff_image = 0;
    THEMELIST_ITEM_STYLE_T* itemstyle_ptr = THEMELIST_GetStyle(ALARM_CHECK_LIST_STYLE);

    if( 0 == event_num )
    {
        //hide list
        GUI_RECT_T  emptyListRect = { 0, 0, 0, 0 };

        GUILIST_SetRect( list_ctr_id, &emptyListRect );
        GUILIST_RemoveAllItems( list_ctr_id );
    }
    else
    {
        uint16 i =0;
        uint16      index = 0;
        uint16      itemIndex = 0;
        uint16      curSelection = 0;
        GUI_RECT_T  listRect = { WATCH_ALARM_LIST_X, WATCH_ALARM_LIST_Y, WATCH_ALARM_LIST_X+WATCH_ALARM_LIST_W, WATCH_ALARM_LIST_Y+WATCH_ALARM_LIST_H-4 };

        curSelection = GUILIST_GetCurItemIndex( list_ctr_id );

        GUILIST_RemoveAllItems( list_ctr_id );
        GUILIST_SetMaxItem(list_ctr_id, ALM_ALARM_NUM, FALSE);
        GUILIST_SetRect( list_ctr_id, &listRect );

        //Append alarm list
        for ( index = 0; index < ALM_ALARM_NUM; index++ )
        {
            SCI_TIME_T                              alarmTime = { 0 };
            MMI_STRING_T                            timeStr = { 0 };
            MMI_STRING_T                            repeatStr = { 0 };
            wchar                                   wTimeStr[WATCH_ALARM_TEMP_STR_LEN] = { 0 };
            wchar                                   wRepeatStr[WATCH_ALARM_TEMP_STR_LEN] = { 0 };
            uint16                                  eventId = EVENT_ALARM_0 + index;
            uint32                                  userData = 0;
            const MMIACC_SMART_EVENT_FAST_TABLE_T*  pEventTable = PNULL;

            pEventTable = MMIALM_GeEvent( eventId );
            if( PNULL == pEventTable )
            {
                TRACE_APP_ALARM("pEventTable is PNULL");
                return;
            }
            if( FALSE == pEventTable->is_valid )
            {
                continue;
            }

            //time string
            timeStr.wstr_ptr    = wTimeStr;
            alarmTime.hour      = pEventTable->hour;
            alarmTime.min       = pEventTable->minute;
            AlarmListGetTimeStr( &alarmTime, &timeStr );

            //repeat string
            repeatStr.wstr_ptr = wRepeatStr;
            AlarmListGetRepeatStr(eventId, &repeatStr);
            //change selete image
            if(TRUE == pEventTable->is_on)
            {
                onoff_image = IMAGE_SWITCH_ON_SELECTED;
            }
            else
            {
                onoff_image = IMAGE_SWITCH_OFF_SELECTED;
            }

            WATCHCOM_LISTITEM_DRAW_2STR_1ICON_2LINE(list_ctr_id, timeStr, repeatStr,onoff_image);

            //temp:check
            CTRLLIST_SetItemStateById(list_ctr_id,itemIndex,GUIITEM_STATE_CONTENT_CHECK,TRUE);

            userData = (uint32)eventId;
            GUILIST_SetItemUserData( list_ctr_id, itemIndex, &userData );
            itemIndex++;
        }

        GUILIST_SetMaxSelectedItem(list_ctr_id, ALM_ALARM_NUM);

        if(PNULL == itemstyle_ptr)
        {
            TRACE_APP_ALARM("itemstyle_ptr = PNULL!");
            return;
        }
        //将某一item中的子内容，设置成GUIITEM_CONTENT_STATE_CHECK模式，
        //用ctrl内部发出MSG_CTL_LIST_CHECK_CONTENT消息
        SCI_MEMSET(&s_item_style, 0, sizeof(THEMELIST_ITEM_STYLE_T));
        SCI_MEMCPY(&s_item_style, itemstyle_ptr, sizeof(THEMELIST_ITEM_STYLE_T));
        s_item_style.content[0].state = GUIITEM_CONTENT_STATE_CHECK;
        s_item_style.content[1].state = GUIITEM_CONTENT_STATE_CHECK;
        s_item_style.content[2].state = GUIITEM_CONTENT_STATE_CHECK;
        for(i=0; i<itemIndex; i++)
        {
            GUILIST_SetItemStyleEx(list_ctr_id, i, &s_item_style);
        }
    }
}

/*****************************************************************************/
//  Description : AlarmListSetIconLabel
//  Parameter: [In] event_num
//             [Out] None
//  Author: qi.liu1
//  Note:根据Alarm个数，判断无Alarm是否显示
/*****************************************************************************/
LOCAL void AlarmListSetIconLabel( uint16 event_num )
{
    MMI_CTRL_ID_T   ctrlIcon = MMIALM_NO_ALARM_ICON_LABEL_CTRL_ID;

    if( 0 == event_num )
    {
        GUILABEL_SetVisible( ctrlIcon, TRUE, FALSE );
    }
    else
    {
        GUILABEL_SetVisible( ctrlIcon, FALSE, FALSE );
    }
}

/*****************************************************************************/
//  Description : Alarm_MainList_FULL_PAINT
//  Parameter: [In] win_id
//             [Out] None
//  Author: qi.liu1
//  Note:
/*****************************************************************************/
LOCAL void Alarm_MainList_FULL_PAINT( MMI_WIN_ID_T win_id )
{
    uint16 eventNum = AlarmMainListGetAlarmNum();

    //bg color
    WATCHCOM_DisplayBackground( win_id );
    if(eventNum == 0)
    {
        GUI_RECT_T rect = {0,0,0,0};
        GUI_FONT_T font = SONG_FONT_20;
        GUI_RECT_T label_rect = WATCH_ALARM_NO_ALRRM_RECT;
        MMI_CTRL_ID_T   ctrlIcon = MMIALM_NO_ALARM_ICON_LABEL_CTRL_ID;
        GUILIST_SetRect(MMIALM_MAIN_LIST_CTRL_ID,&rect);
        GUILABEL_SetRect( ctrlIcon, &label_rect, FALSE );
        CTRLLABEL_SetTextPosition(ctrlIcon,TEXT_DIRECTION_BOTTOM);
        GUILABEL_SetFont(ctrlIcon, font, MMI_WHITE_COLOR);
        GUILABEL_SetIcon( ctrlIcon, res_aw_alarm_ic_alarm );
        CTRLLABEL_SetTextById(ctrlIcon,TXT_NO_ALARM_TIP,FALSE);
    }
    else
    {
            //icon & label
        //AlarmListSetIconLabel( eventNum );
        //list
        AlarmListDrawList( win_id, MMIALM_MAIN_LIST_CTRL_ID, eventNum );
        //button
        //AlarmListUpdeteButton( eventNum );
	    //set offset
        //CTRLLIST_SetTopItemOffset(MMIALM_MAIN_LIST_CTRL_ID,s_offset_y);

        //WATCHCOM_DisplayTitle(win_id, TXT_ALARM_CLOCK);
    }
    //icon & label
    AlarmListSetIconLabel( eventNum );
}

/*****************************************************************************/
//  Description : Alarm_MainList_OPEN_WINDOW
//  Parameter: [In] win_id
//             [Out] None
//  Author: qi.liu1
//  Note:
/*****************************************************************************/
LOCAL void Alarm_MainList_OPEN_WINDOW( MMI_WIN_ID_T win_id )
{
    //GUI_RECT_T      iconRect = { WATCH_ALARM_NO_ALRRM_ICON_X, WATCH_ALARM_NO_ALRRM_ICON_Y, WATCH_ALARM_NO_ALRRM_ICON_X+WATCH_ALARM_NO_ALRRM_ICON_W, WATCH_ALARM_NO_ALRRM_ICON_Y+WATCH_ALARM_NO_ALRRM_ICON_H };
    //MMI_CTRL_ID_T   ctrlIcon = MMIALM_NO_ALARM_ICON_LABEL_CTRL_ID;

    MMI_CTRL_ID_T   listCtrlId = MMIALM_MAIN_LIST_CTRL_ID;
/*#ifdef LOCAL_ALARM_CLOCK_SUPPORT 
    MMI_CTRL_ID_T   addBtn = MMIALM_ADD_ALARM_BTN_CTRL_ID;
    GUI_RECT_T      btnRect = WATCH_ALARM_BTN_RECT;
    GUI_FONT_ALL_T      font = {SONG_FONT_22,MMI_WHITE_COLOR};
        //set button rect
    GUIBUTTON_SetRect(addBtn, &btnRect);
	GUIBUTTON_SetFont(addBtn,&font);
    GUIBUTTON_SetTextId(addBtn,TXT_ADD_ALARM);
#endif*///edit by fys

    //set title text color
    //{
    //    GUI_COLOR_T      font_color = MMI_RED_COLOR;
    //    GUIWIN_SetTitleFontColor(win_id,font_color);
    //}

    s_offset_y = 0;

    //no alarm icon
    //GUILABEL_SetRect( ctrlIcon, &iconRect, FALSE );
    //GUILABEL_SetIcon( ctrlIcon, res_aw_alarm_ic_alarm );

    //alarm list
    GUILIST_SetMaxItem( listCtrlId, WATCH_ALARM_LIST_ITEM_MAX, FALSE );
    MMK_SetAtvCtrl( win_id, listCtrlId );
}

/*****************************************************************************/
//  Description : _Callback_DelAlarmConfirmWin
//  Parameter: [In] win_id
//             [Out] None
//  Author: qi.liu1
//  Note:确认删除Alerm的callback
/*****************************************************************************/
LOCAL MMI_RESULT_E _Callback_DelAlarmConfirmWin( MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;

    switch(msg_id)
    {
#ifdef ADULT_WATCH_SUPPORT
        case MSG_NOTIFY_OK:
#ifdef TOUCH_PANEL_SUPPORT
        case MSG_CTL_PENOK:
#endif
#else
        case MSG_NOTIFY_CANCEL:
#endif
        {
            uint16 event_id = s_alarm_del_index;
            MMI_STRING_T    text_str = {0};

            //delete alarm
            MMIALARM_DeleteOneAlarm( event_id );
            //tip note win
            MMI_GetLabelTextByLang( TXT_ALARM_DELETE, &text_str );
#ifdef ADULT_WATCH_SUPPORT
            Adult_WatchCOM_NoteWin_1Line_Toast_Enter( WATCHALM_DEL_ALARM_NOTE_WIN_ID,&text_str,PNULL,PNULL);
#endif
            MMK_CloseWin(win_id);
            break;
        }
#ifdef ADULT_WATCH_SUPPORT
        case MSG_NOTIFY_CANCEL:
#else
        case MSG_NOTIFY_OK:
#ifdef TOUCH_PANEL_SUPPORT
        case MSG_CTL_PENOK:
#endif
#endif
        {
            MMK_CloseWin( win_id );
            break;
        }
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}

/*****************************************************************************/
//  Description : AlarmRepeat_List_Select
//  Parameter: [In] winId
//             [Out] None
//  Author: qi.liu1
//  Note:在alarm repeat界面，选中一个repeat日期
/*****************************************************************************/
LOCAL void AlarmList_Select_Status(MMI_WIN_ID_T winId ,uint32 event_id)
{
    uint16                  curIndex = 0;
    MMI_CTRL_ID_T           listCtrlId = MMIALM_MAIN_LIST_CTRL_ID;
    const MMIACC_SMART_EVENT_FAST_TABLE_T*  pEventTable = PNULL;

    pEventTable = MMIALM_GeEvent( event_id );

    if(PNULL == pEventTable)
    {
        TRACE_APP_ALARM("pEventTable is PNULL");
        return;
    }

    curIndex = GUILIST_GetCurItemIndex( listCtrlId );

    // change current item state
    TRACE_APP_ALARM("GUILIST_SetSelectedItem Alarm is_on = %d",pEventTable->is_on);
    if( FALSE == pEventTable->is_on )
    {
        MMIALARM_OpenOneAlarm(event_id);
    }
    else
    {
        MMIALARM_CloseOneAlarm(event_id);
    }

    MMK_SendMsg( winId, MSG_FULL_PAINT, PNULL );
}

/*****************************************************************************/
//  Description : HandleAlarmMainListWindow
//  Parameter: [In] win_id
//             [Out] None
//  Author: qi.liu1
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleAlarmMainListWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E reCode =  MMI_RESULT_TRUE;
    MMI_CTRL_ID_T   listCtrlId = MMIALM_MAIN_LIST_CTRL_ID;

    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            Alarm_MainList_OPEN_WINDOW( win_id );
            break;
        }

        case MSG_FULL_PAINT:
        {
            Alarm_MainList_FULL_PAINT( win_id );
            break;
        }

        case MSG_CTL_MIDSK:
        case MSG_APP_WEB:
        {
            #ifdef LOCAL_ALARM_CLOCK_SUPPORT
                //进入修改闹钟流程
                uint16 index = 0;
                uint32 event_id = 0;
                index = GUILIST_GetCurItemIndex( listCtrlId );
                reCode = GUILIST_GetItemData( listCtrlId, index, &event_id );
                TRACE_APP_ALARM("Event_id %d", event_id);
                //WatchAlarm_EditWin_Enter(event_id);
                //WatchAlarm_EditListWin_Enter(event_id);//edit by fys
            #endif
            break;
        }

#ifdef TOUCH_PANEL_SUPPORT
        case MSG_CTL_PENOK:
        {
            #ifdef LOCAL_ALARM_CLOCK_SUPPORT
            //Add Alarm
            MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T*)param)->src_id;
            if(MMIALM_ADD_ALARM_BTN_CTRL_ID == ctrl_id)
            {
                uint16 AlarmNum = AlarmMainListGetAlarmNum();

                if( AlarmNum < ALM_ALARM_NUM )
                {
                    //enter Alarm add win
                    //WatchAlarm_AddWin_Enter();//edit by fys
                }
                else
                {
                    TRACE_APP_ALARM("AlarmNum %d", AlarmNum);
                }
            }
            else
            {
                //进入修改闹钟流程
                uint16 index = 0;
                uint32 event_id = 0;
                index = GUILIST_GetCurItemIndex( listCtrlId );
                reCode = GUILIST_GetItemData( listCtrlId, index, &event_id );
                TRACE_APP_ALARM("Event_id %d", event_id);
                //WatchAlarm_EditWin_Enter(event_id);
                //WatchAlarm_EditListWin_Enter(event_id);//edit by fys
            }
            #endif
            break;
        }
#endif

        case MSG_CTL_LIST_CHECK_CONTENT:
        {
        #ifdef LOCAL_ALARM_CLOCK_SUPPORT
            GUILIST_NEED_ITEM_CONTENT_T * item_content_ptr = (GUILIST_NEED_ITEM_CONTENT_T *)param;

            if(ALARM_ITEM_CONTENT_SELECT_INDEX ==item_content_ptr->item_content_index)
            {
                //Open and Close Alarm
                uint16 index = GUILIST_GetCurItemIndex( listCtrlId );
                uint32 event_id = 0;
                reCode = GUILIST_GetItemData( listCtrlId, index, &event_id );

                CTRLLIST_GetTopItemOffset(listCtrlId, &s_offset_y);
                AlarmList_Select_Status(win_id,event_id);
            }
            else
            {
                //进入修改闹钟流程
                uint16 index = 0;
                uint32 event_id = 0;
                index = GUILIST_GetCurItemIndex( listCtrlId );
                reCode = GUILIST_GetItemData( listCtrlId, index, &event_id );
                TRACE_APP_ALARM("Event_id %d", event_id);
                 //WatchAlarm_EditWin_Enter(event_id);
                //WatchAlarm_EditListWin_Enter(event_id);//edit by fys
            }
        #endif
            break;
        }

        case MSG_CTL_OK:
        case MSG_APP_OK:
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYDOWN_CANCEL:
            break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
        {
            MMK_CloseWin( win_id );
            break;
        }

        default:
            reCode = MMI_RESULT_FALSE;
            break;
    }

    return reCode;
}
/*****************************************************************************/
//  Description : AlarmEditList_OPEN_WINDOW
//  Parameter: [In] win_id
//             [Out] None
//  Author: qi.liu1
//  Note:
/*****************************************************************************/

LOCAL void AlarmEditList_OPEN_WINDOW( MMI_WIN_ID_T win_id )
{
    //MMI_CTRL_ID_T   listCtrlId = MMIALM_EDIT_LIST_CTRL_ID;
    MMI_CTRL_ID_T   DelBtn = MMIALM_EDIT_LIST_DEL_BTN_CTRL_ID;
    MMI_CTRL_ID_T   SaveBtn = MMIALM_EDIT_LIST_SAVE_BTN_CTRL_ID;
    GUI_RECT_T      DelbtnRect = WATCH_ALARM_DELBTN_RECT;
    GUI_RECT_T      SavebtnRect = WATCH_ALARM_SAVEBTN_RECT;
    GUI_FONT_ALL_T      font = {SONG_FONT_22,MMI_WHITE_COLOR};
    SCI_TIME_T                              alarmTime = { 0 };
    MMI_STRING_T                            timeStr = { 0 };
    MMI_STRING_T                            repeatStr = { 0 };
    wchar                                   wTimeStr[WATCH_ALARM_TEMP_STR_LEN] = { 0 };
    wchar                                   wRepeatStr[WATCH_ALARM_TEMP_STR_LEN] = { 0 };
    const MMIACC_SMART_EVENT_FAST_TABLE_T*  pEventTable = PNULL;
    
    ALARM_EDIT_WIN_T*      pWinData = PNULL;
    //GUI_RECT_T  listRect = { WATCH_ALARM_LIST_X, WATCH_ALARM_LIST_Y, WATCH_ALARM_LIST_X+WATCH_ALARM_LIST_W, WATCH_ALARM_LIST_Y+WATCH_ALARM_LIST_H-4 };
    SCI_Memset(&pEditInfoTable,0,sizeof(ALARM_EDITINFO_TABLE_T));
    //GUILIST_RemoveAllItems( listCtrlId );
    //GUILIST_SetRect( listCtrlId, &listRect );

    pWinData = (ALARM_EDIT_WIN_T*)MMK_GetWinAddDataPtr( win_id );
    if( PNULL == pWinData )
    {
        TRACE_APP_ALARM("pWinData is PNULL!!");
        return;
    }        

    //set button rect
    GUIBUTTON_SetRect(DelBtn, &DelbtnRect);
    GUIBUTTON_SetRect(SaveBtn, &SavebtnRect);

    //alarm list
    //GUILIST_SetMaxItem( listCtrlId, 2, FALSE );
    //MMK_SetAtvCtrl( win_id, listCtrlId );
    
    pEventTable = MMIALM_GeEvent( pWinData->eventId );
    if( PNULL == pEventTable )
    {
        TRACE_APP_ALARM("pEventTable is PNULL");
        return;
    }
     //time string
    timeStr.wstr_ptr    = wTimeStr;
    alarmTime.hour      = pEventTable->hour;
    alarmTime.min       = pEventTable->minute;

    pEditInfoTable.hour = pEventTable->hour; 
    pEditInfoTable.minute = pEventTable->minute;
    pEditInfoTable.fre_mode = pEventTable->fre_mode; 
    pEditInfoTable.fre = pEventTable->fre;
    /*AlarmListGetTimeStr( &alarmTime, &timeStr );

    MMIAPICOM_StrcatFromTextIDToUCS2(wRepeatStr,&repeatStr.wstr_len,TXT_TIME);

    
    //repeat string
    repeatStr.wstr_ptr = wRepeatStr;
    WatchCOM_ListItem_Draw_2Str(listCtrlId,repeatStr,timeStr);
    //temp:check
    CTRLLIST_SetItemStateById(listCtrlId,0,GUIITEM_STATE_CONTENT_CHECK,TRUE);
    
    AlarmListGetRepeatStr(pWinData->eventId , &repeatStr);
    SCI_Memset(wTimeStr, 0, WATCH_ALARM_TEMP_STR_LEN*2);
    timeStr.wstr_len = 0;
    MMIAPICOM_StrcatFromTextIDToUCS2(wTimeStr,&timeStr.wstr_len,TXT_SET_ALARM_REPEAT);
     //repeat string
    timeStr.wstr_ptr = wTimeStr;
    WatchCOM_ListItem_Draw_2Str(listCtrlId,timeStr,repeatStr);
    //temp:check
    CTRLLIST_SetItemStateById(listCtrlId,1,GUIITEM_STATE_CONTENT_CHECK,TRUE);*/

}

LOCAL void AlarmEditList_Paint( MMI_WIN_ID_T win_id )
{
    GUI_RECT_T         timetxt_rect = {25,26,67,70};
    GUI_RECT_T         timer_rect = {68,26,215,70};
     GUI_RECT_T         repeattxt_rect = {25,82,67,126};
     GUI_RECT_T         repeat_rect = {68,82,215,126};
     GUI_RECT_T         timeitem_rect = WATCH_ALARM_EDITLIST_TIMEITEM_RECT;//{12,24,228,72};
     GUI_RECT_T         repeatitem_rect = WATCH_ALARM_EDITLIST_REPEATITEM_RECT;//{12,80,228,128};
    
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STYLE_T text_style = {0};

    SCI_TIME_T                              alarmTime = { 0 };
    MMI_STRING_T                            timeStr = { 0 };
    MMI_STRING_T                            repeatStr = { 0 };
    wchar                                   wTimeStr[WATCH_ALARM_TEMP_STR_LEN] = { 0 };
    wchar                                   wRepeatStr[WATCH_ALARM_TEMP_STR_LEN] = { 0 };
    
    ALARM_EDIT_WIN_T*      pWinData = PNULL;
    pWinData = (ALARM_EDIT_WIN_T*)MMK_GetWinAddDataPtr( win_id );
    if( PNULL == pWinData )
    {
        TRACE_APP_ALARM("pWinData is PNULL!!");
        return;
    }

    //alarm list
    //GUILIST_SetMaxItem( listCtrlId, 2, FALSE );
   // MMK_SetAtvCtrl( win_id, listCtrlId );
    
     //time string
    timeStr.wstr_ptr    = wTimeStr;
    alarmTime.hour      = pEditInfoTable.hour;
    alarmTime.min       = pEditInfoTable.minute;

    /*pEditInfoTable.hour = pEventTable->hour; 
    pEditInfoTable.minute = pEventTable->minute;
    pEditInfoTable.fre_mode = pEventTable->fre_mode; 
    pEditInfoTable.fre = pEventTable->fre;*/

    GUIRES_DisplayImg(PNULL,
                &timeitem_rect,
                PNULL,
                win_id,
                image_watch_list_bg,
                &lcd_dev_info);
    GUIRES_DisplayImg(PNULL,
                &repeatitem_rect,
                PNULL,
                win_id,
                image_watch_list_bg,
                &lcd_dev_info);
    

    MMIAPICOM_StrcatFromTextIDToUCS2(wRepeatStr,&repeatStr.wstr_len,TXT_TIME);

    
    //repeat string
    repeatStr.wstr_ptr = wRepeatStr;

    text_style.font = SONG_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
    text_style.align = ALIGN_HVMIDDLE;
     GUISTR_DrawTextToLCDInRect(
                &lcd_dev_info,
                (const GUI_RECT_T *)&timetxt_rect,       //the fixed display area
                (const GUI_RECT_T *)&timetxt_rect,       
                (const MMI_STRING_T *)&repeatStr,
                &text_style,
                GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
                GUISTR_TEXT_DIR_AUTO);
      alarmTime.hour      = pEditInfoTable.hour;
    alarmTime.min       = pEditInfoTable.minute;          
     AlarmListGetTimeStr( &alarmTime, &timeStr );
     text_style.font = SONG_FONT_24;
    text_style.font_color = MMI_CUSTOM_BLUE_COLOR;
    text_style.align = ALIGN_RVMIDDLE;
     GUISTR_DrawTextToLCDInRect(
                &lcd_dev_info,
                (const GUI_RECT_T *)&timer_rect,       //the fixed display area
                (const GUI_RECT_T *)&timer_rect,       
                (const MMI_STRING_T *)&timeStr,
                &text_style,
                GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
                GUISTR_TEXT_DIR_AUTO);

    SCI_Memset(wTimeStr, 0, WATCH_ALARM_TEMP_STR_LEN*2);
    timeStr.wstr_len = 0;
    MMIAPICOM_StrcatFromTextIDToUCS2(wTimeStr,&timeStr.wstr_len,TXT_SET_ALARM_REPEAT);
    timeStr.wstr_ptr = wTimeStr;
     text_style.font = SONG_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
    text_style.align = ALIGN_HVMIDDLE;
     GUISTR_DrawTextToLCDInRect(
                &lcd_dev_info,
                (const GUI_RECT_T *)&repeattxt_rect,       //the fixed display area
                (const GUI_RECT_T *)&repeattxt_rect,       
                (const MMI_STRING_T *)&timeStr,
                &text_style,
                GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
                GUISTR_TEXT_DIR_AUTO);

     SCI_Memset(wTimeStr, 0, WATCH_ALARM_TEMP_STR_LEN*2);
     repeatStr.wstr_len = 0;
    AlarmListGetRepeatStrEx(pEditInfoTable, &repeatStr);
    text_style.font = SONG_FONT_24;
    text_style.font_color = MMI_CUSTOM_BLUE_COLOR;
    text_style.align = ALIGN_RVMIDDLE;
     GUISTR_DrawTextToLCDInRect(
                &lcd_dev_info,
                (const GUI_RECT_T *)&repeat_rect,       //the fixed display area
                (const GUI_RECT_T *)&repeat_rect,       
                (const MMI_STRING_T *)&repeatStr,
                &text_style,
                GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
                GUISTR_TEXT_DIR_AUTO);

}

/*****************************************************************************/
//  Description : HandleAlarmEditListWindow
//  Parameter: [In] winId
//             [Out] None
//  Author: qi.liu1
//  Note:alarm edit界面处理函数
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleAlarmEditListWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E reCode =  MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            AlarmEditList_OPEN_WINDOW(win_id);
            break;
        }

        case MSG_FULL_PAINT:
        {
            //bg color
            WATCHCOM_DisplayBackground( win_id );
            AlarmEditList_Paint(win_id);
            break;
        }
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            GUI_RECT_T         timeitem_rect = WATCH_ALARM_EDITLIST_TIMEITEM_RECT;//{12,24,228,72};
            GUI_RECT_T         repeatitem_rect = WATCH_ALARM_EDITLIST_REPEATITEM_RECT;//{12,80,228,128};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            SCI_TRACE_LOW("lichao point.x = %d,point.y", point.x,point.y);
            if (GUI_PointIsInRect(point, timeitem_rect))
            {
                tp_down_item_index = 0;
            }
            else if(GUI_PointIsInRect(point,repeatitem_rect))
            {
                tp_down_item_index = 1;
            }
            else
            {
                tp_down_item_index = -1;
            }
        }
        break;
    
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            GUI_RECT_T         timeitem_rect = WATCH_ALARM_EDITLIST_TIMEITEM_RECT;//{12,24,228,72};
            GUI_RECT_T         repeatitem_rect = WATCH_ALARM_EDITLIST_REPEATITEM_RECT;//{12,80,228,128};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            SCI_TRACE_LOW("lichao point.x = %d,point.y", point.x,point.y);
            if(tp_down_item_index == 1 || tp_down_item_index == 0)
            {
                ALARM_EDIT_WIN_T*      pWinData = PNULL;
                pWinData = (ALARM_EDIT_WIN_T*)MMK_GetWinAddDataPtr( win_id );
                if( PNULL == pWinData )
                {
                    tp_down_item_index = -1;
                    TRACE_APP_ALARM("pWinData is PNULL!!");
                    break;
                }
                if (GUI_PointIsInRect(point, timeitem_rect) && tp_down_item_index == 0)
                {
                    WatchAlarm_EditWin_Enter(pWinData->eventId );
                }
                else if(GUI_PointIsInRect(point,repeatitem_rect) && tp_down_item_index == 1)
                {
                    ALARM_REPEAT_WIN_T     repeatWin = { 0 };

                    repeatWin.eventId   = pWinData->eventId;
                    repeatWin.type      = pWinData->type;
                    repeatWin.time.hour = pEditInfoTable.hour;
                    repeatWin.time.min  = pEditInfoTable.minute;
                    WatchAlarm_RepeatWin_Enter(&repeatWin);
                }
            }
            tp_down_item_index = -1;
            break;
        }
    case MSG_CTL_MIDSK:
    case MSG_KEYDOWN_OK:
    case MSG_APP_WEB:
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_CTL_PENOK:
    case MSG_CTL_OK:
        {
            //Add Alarm
            MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T*)param)->src_id;
            if(MMIALM_EDIT_LIST_DEL_BTN_CTRL_ID == ctrl_id)
            {
                //Press Next btn:(->)
                //HandleAlarmPressNextBtn();
#ifdef LOCAL_ALARM_CLOCK_SUPPORT
                //Delete alarm
                MMI_STRING_T            queryTipStr = { 0 };
                MMI_CTRL_ID_T   listCtrlId = MMIALM_MAIN_LIST_CTRL_ID;
                uint16 alarm_num = AlarmMainListGetAlarmNum();
                uint16 index = 0;
                ALARM_EDIT_WIN_T*      pWinData = PNULL;

                if(0 != alarm_num)
                {
                    //index = GUILIST_GetCurItemIndex( listCtrlId );
                   // reCode = GUILIST_GetItemData( listCtrlId, index, (uint32*)&s_alarm_del_index );
                    //delete alarm
                    pWinData = (ALARM_EDIT_WIN_T*)MMK_GetWinAddDataPtr( win_id );
                    if( PNULL == pWinData )
                    {
                        TRACE_APP_ALARM("pWinData is PNULL!!");
                        return;
                    }
                    MMIALARM_DeleteOneAlarm( pWinData->eventId );
                    
                }
                MMK_CloseWin(win_id);
#endif
            }
            else  if(MMIALM_EDIT_LIST_SAVE_BTN_CTRL_ID == ctrl_id)
            {
                SCI_DATE_T                              dateValue = { 0 };
                MMIACC_SMART_EVENT_FAST_TABLE_T*        pEventTable = PNULL;
                ALARM_EDIT_WIN_T*      pWinData = PNULL;

                pEventTable = (MMIACC_SMART_EVENT_FAST_TABLE_T*)SCI_ALLOC_APPZ( sizeof(MMIACC_SMART_EVENT_FAST_TABLE_T) );
                if( PNULL == pEventTable )
                {
                    return MMI_RESULT_FALSE;
                }
                // date && time
                TM_GetSysDate( &dateValue );

                pEventTable->start_year  = dateValue.year;
                pEventTable->start_mon   = dateValue.mon;
                pEventTable->start_day   = dateValue.mday;

                pEventTable->year        = dateValue.year;
                pEventTable->mon         = dateValue.mon;
                pEventTable->day         = dateValue.mday;
                pEventTable->hour        = pEditInfoTable.hour;
                pEventTable->minute      = pEditInfoTable.minute;
                pEventTable->fre_mode        = pEditInfoTable.fre_mode;
                pEventTable->fre        = pEditInfoTable.fre;

                pEventTable->is_on       = TRUE;
                pEventTable->is_valid    = TRUE;

                pWinData = (ALARM_EDIT_WIN_T*)MMK_GetWinAddDataPtr( win_id );
                if( PNULL == pWinData )
                {
                    TRACE_APP_ALARM("pWinData is PNULL!!");
                    return;
                }

                MMIALARM_ModifyOneAlarm( pWinData->eventId, pEventTable );

                SCI_FREE( pEventTable );
                MMK_CloseWin(win_id);
            }
            break;
        }
#endif
        /*case MSG_ALARM_TIME_DATA:
        {
            SCI_DATE_T                              dateValue = { 0 };
            ALARM_REPEAT_WIN_T     *repeatWin = (ALARM_REPEAT_WIN_T*)param;
            pEditInfoTable.hour  = repeatWin->time.hour;
            pEditInfoTable.minute  = repeatWin->time.min;
            break;
        }
         case MSG_ALARM_REPEAT_DATA:
        {
            SCI_DATE_T                              dateValue = { 0 };
            ALARM_EDITINFO_TABLE_T     *temp_repeatWin = (ALARM_EDITINFO_TABLE_T*)param;
            pEditInfoTable.fre = temp_repeatWin->fre;
            pEditInfoTable.fre_mode= temp_repeatWin->fre_mode;
            break;
         }*/
        case MSG_KEYDOWN_RED:
            break;
        case MSG_KEYUP_RED:
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin( win_id );
            break;
        }
        case MSG_GET_FOCUS:
        MMK_PostMsg(win_id,MSG_FULL_PAINT,PNULL,PNULL);
        break;

        case MSG_CLOSE_WINDOW:
        {
            MMK_FreeWinAddData( win_id );
            break;
        }

        default:
            reCode = MMI_RESULT_FALSE;
            break;
    }

    return reCode;
}

//idle -> alarm list
WINDOW_TABLE( MMIALARM_MAINLIST_WIN_TAB ) =
{
    WIN_FUNC( (uint32)HandleAlarmMainListWindow ),
    WIN_ID( WATCHALM_MAIN_WIN_ID ),
    WIN_HIDE_STATUS,
#ifdef ADULT_WATCH_SUPPORT
    WIN_TITLE( TXT_ALARM_CLOCK ),
#endif
    CREATE_LABEL_CTRL( GUILABEL_ALIGN_MIDDLE, MMIALM_NO_ALARM_ICON_LABEL_CTRL_ID ),
    CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMIALM_MAIN_LIST_CTRL_ID ),
#ifdef LOCAL_ALARM_CLOCK_SUPPORT
    //CREATE_BUTTON_CTRL(IMAGE_COMMON_ONEBTN_KBBG_IMAG, MMIALM_ADD_ALARM_BTN_CTRL_ID ),
#endif
#ifndef TOUCH_PANEL_SUPPORT
    WIN_SOFTKEY( STXT_OK, TXT_NULL, STXT_RETURN ),
#endif
    END_WIN
};
WINDOW_TABLE( MMIALARM_EDITLIST_WIN_TAB ) =
{
    WIN_FUNC( (uint32)HandleAlarmEditListWindow ),
    WIN_ID( WATCHALM_EDITLIST_WIN_ID ),
    WIN_HIDE_STATUS,
    //CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMIALM_EDIT_LIST_CTRL_ID ),
    CREATE_BUTTON_CTRL(alarm_edit_list_del_btn, MMIALM_EDIT_LIST_DEL_BTN_CTRL_ID ),
    CREATE_BUTTON_CTRL(res_common_set_confirm, MMIALM_EDIT_LIST_SAVE_BTN_CTRL_ID ),
    END_WIN
};
/*****************************************************************************/
//  Description : WatchAlarm_EditListWin_Enter
//  Parameter: [In] event_id
//             [Out] None
//  Author: qi.liu1
//  Note:alarm edit win的入口函数
/*****************************************************************************/
PUBLIC void WatchAlarm_EditListWin_Enter( uint32 event_id )
{
    ALARM_EDIT_WIN_T* pWinData = PNULL;

     if(MMK_IsOpenWin(WATCHALM_EDITLIST_WIN_ID))
    {
        MMK_CloseWin(WATCHALM_EDITLIST_WIN_ID);
    }

    pWinData = (ALARM_EDIT_WIN_T*)SCI_ALLOC_APPZ( sizeof(ALARM_EDIT_WIN_T) );
    if( PNULL == pWinData )
    {
        TRACE_APP_ALARM("pWinData is PNULL!!");
        return;
    }

    pWinData->type      = WATCH_ALARM_EDIT;
    pWinData->eventId   = event_id;

    MMK_CreateWin( (uint32 *)MMIALARM_EDITLIST_WIN_TAB, (ADD_DATA)pWinData );
}

/*****************************************************************************/
//  Description : WatchAlarm_MainWin_Enter
//  Parameter: [In] None
//             [Out] None
//  Author: qi.liu1
//  Note:
/*****************************************************************************/
PUBLIC void WatchAlarm_MainWin_Enter( void )
{
    if(MMK_IsOpenWin(WATCHALM_MAIN_WIN_ID))
    {
        MMK_CloseWin(WATCHALM_MAIN_WIN_ID);
    }
    MMK_CreateWin( (uint32 *)MMIALARM_MAINLIST_WIN_TAB, PNULL );
}

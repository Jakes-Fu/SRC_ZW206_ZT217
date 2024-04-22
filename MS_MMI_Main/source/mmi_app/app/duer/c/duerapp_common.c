// Copyright (2020) Baidu Inc. All rights reserveed.
/**
 * File: duerap_login.c
 * Auth: Liuwenshuai (liuwenshuai@baidu.com)
 * Desc: QR login.
 */
/**--------------------------------------------------------------------------*/
#include "baidu_json.h"
#include "mmicom_time.h"
#include "lightduer_log.h"
#include "lightduer_types.h"
#include "duerapp_id.h"
#include "mmi_string.h"
#include "lightduer_memory.h"
#include "lightduer_timestamp.h"
#include "watch_commonwin_export.h"
#include "mmipub.h"
#include "mmi_common.h"
#include "duerapp_common.h"
#include "guistring.h"
#include "guigraph.h"
#include "mmidisplay_color.h"
#include "duerapp_login.h"
#include "mmi_position.h"
/**--------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------*/
#define     DUERAPP_DATE_MAX_YEAR                2099   //时间设置最大年份
#define     DUERAPP_DATE_MIN_YEAR                1970   //时间设置最小年份
#define     DUERAPP_SEC_PER_DAY                  86400
#define     DUERAPP_SEC_PER_HOUR                 3600
#define     DUERAPP_SEC_PER_MIN                  60
#define     DUERAPP_DAY_PER_YEAR                 365
#define     DUERAPP_MONTH_START                  1
#define     DEFAULT_WAITING_TIME 5000
#define     PRINTF_STR_MAX_LEN 50

const uint16 duerapp_dayfromjanone[13] =
{0,31,59,90,120,151,181,212,243,273,304,334,365};

const uint16 duerapp_dayfromjanonerun[13] =
{0,31,60,91,121,152,182,213,244,274,305,335,366};
/**--------------------------------------------------------------------------*/
LOCAL uint8 s_waiting_timer_id = 0;
/**--------------------------------------------------------------------------*/

uint32 duerapp_Tm2Second(
                      uint32 tm_sec, 
                      uint32 tm_min, 
                      uint32 tm_hour,
                      uint32 tm_mday,
                      uint32 tm_mon, 
                      uint32 tm_year    
                      )
{
    uint16 i = 0;
    uint32 no_of_days = 0;
    uint32 utc_time = 0;

    //check if the date value range is valid
    if ( (tm_mday > 31) || (tm_mon > 12) 
        ||(tm_year < DUERAPP_DATE_MIN_YEAR)
        ||(tm_mon < DUERAPP_MONTH_START)
        )
    {
        return 0;
    }
    
    /* year */
    for (i = DUERAPP_DATE_MIN_YEAR; i < tm_year; i++)
    {
        no_of_days += (MMICOM_DAY_PER_YEAR + MMIAPICOM_IsLeapYear(i));
    }

    if(MMIAPICOM_IsLeapYear(tm_year))
    {
        no_of_days += 
            duerapp_dayfromjanonerun[tm_mon-DUERAPP_MONTH_START]
            +tm_mday - 1;
    }
    else
    {
        no_of_days += 
            duerapp_dayfromjanone[tm_mon-DUERAPP_MONTH_START]
            +tm_mday - 1;
    }

    /* sec */
    utc_time =
         no_of_days *DUERAPP_SEC_PER_DAY +  
         tm_hour * DUERAPP_SEC_PER_HOUR +
         tm_min * DUERAPP_SEC_PER_MIN + tm_sec;

    return utc_time;
}


// 返回1970年至今时间戳，单位：ms
int duerapp_timestamp() {
    // SCI_DATE_T sys_date = {0};
    // SCI_TIME_T sys_time = {0};
    // uint32      second = 0;
    
    // TM_GetSysDate(&sys_date);
    // TM_GetSysTime(&sys_time);
    // second = duerapp_Tm2Second( sys_time.sec, 
    //     sys_time.min, 
    //     sys_time.hour,
    //     sys_date.mday,
    //     sys_date.mon, 
    //     sys_date.year );   
    // return ( second * 1000);
    return duer_timestamp();
    //return MMIAPICOM_GetCurTime();
}


// 返回随机16位随机数，需要free
char *duerapp_get_nonce()
{
    char *random_range = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int size = strlen(random_range);
    char *rand_ptr = NULL;
    int i = 0, index = -1;
    srand(duerapp_timestamp());
    // 取16位随机数
    rand_ptr = DUER_CALLOC(1,17);
    for (i = 0;i < 16; i++) {
        index = rand() % size;
        rand_ptr[i] = *(random_range + index);
    }
    DUER_LOGI("random->%s",rand_ptr);
    return rand_ptr;
}
/**--------------------------------------------------------------------------*/

void duerapp_show_toast(wchar *toast_ptr) {
    WATCH_SOFTKEY_TEXT_ID_T softkey={COMMON_TXT_NULL, COMMON_TXT_NULL, COMMON_TXT_NULL};
    MMI_STRING_T tipSting = {0};
    tipSting.wstr_ptr = toast_ptr;
    tipSting.wstr_len = MMIAPICOM_Wstrlen(toast_ptr);
    WatchCOM_NoteWin_2Line_Enter(MMI_DUERAPP_QUERY_WIN_ID,
                                     &tipSting,
                                     0,
                                     softkey,
                                     PNULL);
}
/**--------------------------------------------------------------------------*/

LOCAL void StopWaitingTimer(void)
{
    if (0 != s_waiting_timer_id)
    {
        MMK_StopTimer(s_waiting_timer_id);
        s_waiting_timer_id = 0;
    }
}

LOCAL void HandleWaitingTimer(uint8 timer_id, uint32 param)
{
    DUER_LOGI("HandleWaitingTimer...");
    duerapp_close_waiting();
}

LOCAL void StartWaitingTimer(void)
{
    DUER_LOGI("StartWaitingTimer...");
    if (0 != s_waiting_timer_id)
    {
        MMK_StopTimer(s_waiting_timer_id);
        s_waiting_timer_id = 0;
    }
    s_waiting_timer_id = MMK_CreateTimerCallback(DEFAULT_WAITING_TIME, HandleWaitingTimer, NULL, FALSE);
}

LOCAL MMI_RESULT_E HandleDuerWaitingWinMsg(
                                       MMI_WIN_ID_T       win_id,     // 窗口的ID
                                       MMI_MESSAGE_ID_E   msg_id,     // 窗口的内部消息ID
                                       DPARAM             param       // 相应消息的参数
                                       )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    switch (msg_id)
    {
        case MSG_OPEN_WINDOW:
            StartWaitingTimer();
            break;
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_APP_CANCEL:
        case MSG_LOSE_FOCUS:
            MMK_CloseWin(win_id);
            break;
        case MSG_CLOSE_WINDOW:
            StopWaitingTimer();
            break;
        default:
            result = MMIPUB_HandleWaitWinMsg(win_id, msg_id, param);
            break;
    }

    return (result);
}

/*****************************************************************************/

void duerapp_show_waiting(wchar *wstr_ptr)
{
    MMI_STRING_T waiting_text = {0};
    waiting_text.wstr_ptr = wstr_ptr ? wstr_ptr : L"思考中，请稍等...";
    waiting_text.wstr_len = MMIAPICOM_Wstrlen(waiting_text.wstr_ptr);
    MMIPUB_OpenWaitWin(1, &waiting_text, PNULL, PNULL, MMI_DUERAPP_WAITING_WIN_ID, IMAGE_NULL,
         ANIM_PUBWIN_WAIT, WIN_ONE_LEVEL, MMIPUB_SOFTKEY_NONE, HandleDuerWaitingWinMsg);
}

void duerapp_close_waiting()
{
    if (MMK_IsOpenWin(MMI_DUERAPP_WAITING_WIN_ID)) {
        MMK_CloseWin(MMI_DUERAPP_WAITING_WIN_ID);
    }
}

/*****************************************************************************/
unsigned int duerapp_parse_color(char *color_string)
{
    unsigned int ch1;
    unsigned int ch2;
    unsigned int ch3;
    unsigned int color;
    sscanf(color_string, "#%02X%02X%02X", &ch1, &ch2, &ch3);
    printf("%02X %02X %02X\n", ch1, ch2, ch3);
    color = (ch1<<16) + (ch2<<8) + ch3;
    printf("0x%06X\n", color);    
    return color;
}
/*****************************************************************************/
PUBLIC uint16 duerapp_DisplayLineTextInRect(
                            GUI_LCD_DEV_INFO *lcd_dev_ptr,
                            GUI_RECT_T      *rect_ptr,
                            GUI_FONT_T       font,
                            GUI_COLOR_T      font_color,
                            wchar            *wstr_ptr,
                            uint16           wstr_len
                            )
{
    GUISTR_STYLE_T  text_style = {0};/*lint !e64*/
    MMI_STRING_T    text = {0};
    GUISTR_STATE_T  text_state = GUISTR_STATE_ALIGN|GUISTR_STATE_ELLIPSIS|GUISTR_STATE_SINGLE_LINE;
    GUI_RECT_T      disp_rect = *rect_ptr;
    uint16          lcd_width = 0;
    uint16          lcd_height = 0;
    uint16          str_pixel_width = 0;

    GUILCD_GetLogicWidthHeight(lcd_dev_ptr->lcd_id, &lcd_width, &lcd_height);

    MMIIDLE_GetIdleTextStyle(&text_style, font, font_color);
    text_style.align = ALIGN_HVMIDDLE;

    text.wstr_len = wstr_len;
    text.wstr_ptr = wstr_ptr;
    //display date
    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)lcd_dev_ptr,
        (const GUI_RECT_T      *)&disp_rect,
        (const GUI_RECT_T      *)&disp_rect,
        (const MMI_STRING_T     *)&text,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
        );

    str_pixel_width = GUISTR_GetStringWidth(&text_style,&text,text_state);
    return (str_pixel_width);
}
/*****************************************************************************/
PUBLIC void duerapp_show_waiting_text() {
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    wchar *vip_tile = L"加载中，请稍等...";
    GUI_RECT_T title_rect = {0,0,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT};
    LCD_FillRect(&lcd_dev_info, title_rect, MMI_BLACK_COLOR);
    duerapp_DisplayLineTextInRect(&lcd_dev_info,&title_rect,
        WATCH_DEFAULT_BIG_FONT,
        MMI_WHITE_COLOR,
        vip_tile,
        MMIAPICOM_Wstrlen(vip_tile));
}

// 文字query
PUBLIC void duerapp_send_query(char *query_text)
{
    char p_url[256] = {0};
    char* s_p_skill_url = "dueros://server.dueros.ai/query?q=%s";
    int result = -1;

    snprintf(p_url, 256, s_p_skill_url, query_text);
    result = duer_send_link_click_url(p_url);
    
    DUER_LOGI("(%s)[duer_watch]:result(%s), text(%s)", __func__, result == 0 ? "success":"failed", query_text);
}
/*****************************************************************************/

PUBLIC char *duerapp_TimeStampGet(void)
{
    static char RtcBuf[64] = {0};
    SCI_TM_T TM_Attr = {0};

    memset(RtcBuf, 0, sizeof(RtcBuf)/sizeof(RtcBuf[0]));
    TM_GetTime(&TM_Attr);
    sprintf(RtcBuf, "%d-%d %d:%d:%d", TM_Attr.tm_mon, TM_Attr.tm_mday, TM_Attr.tm_hour, TM_Attr.tm_min, TM_Attr.tm_sec);

    return RtcBuf;
}

PUBLIC void duer_print_long_str(char* str)
{
    int tmp_len = strlen(str);
    char *tmp_p = str;
    char tmp = 0;
    int i = 0;

    if ((NULL == str) || (strlen(str) <= 0)) {
        return;
    }
    
    tmp_len /= PRINTF_STR_MAX_LEN;//太长打印丢失
    
    for (i = 0; i < tmp_len; i++) {
        tmp = tmp_p[PRINTF_STR_MAX_LEN];
        tmp_p[PRINTF_STR_MAX_LEN] = '\0';
        DUER_LOGI("[duer_watch]:(%s)->", tmp_p);
        tmp_p[PRINTF_STR_MAX_LEN] = tmp;
        tmp_p += PRINTF_STR_MAX_LEN;
    }
    DUER_LOGI("[duer_watch]:(%s)END", tmp_p);
}

#if 0 //T_P_APP线程使用消息调用GUI接口方式，传递函数指针与函数参数到UI线程执行
PUBLIC void MMK_duer_call_func(void *param)
{
    MMK_duer_call_func_struct *info = (MMK_duer_call_func_struct *)param;
    if(info)
    {
        if(info->func)
        {
            info->func(info->userData);
        }
        else
        {
            SCI_TRACE_HIGH("MMK_duer_call_func---param->func is NULL");
        }
    }
}

PUBLIC void duer_sub_send_message(BLOCK_ID src, BLOCK_ID dst, uint16 msgid, void *param_ptr, uint32 size)
{
    xSignalHeader sig = (xSignalHeader)param_ptr;

    if (!param_ptr)
    {
        sig = (xSignalHeader)SCI_ALLOCAZ(sizeof(xSignalHeaderRec));
        SCI_ASSERT(sig != NULL);
    }

    sig->SignalCode = msgid;
    if (param_ptr)
    {
        sig->SignalSize = size;
    }
    else
    {
        sig->SignalSize = sizeof(xSignalHeaderRec);
    }
    sig->Sender = src;

    SCI_SendSignal((xSignalHeader)sig, dst);
}

LOCAL void duer_gui_msg_deal_example(void *param)
{

}

PUBLIC void duer_send_gui_msg_example(void)
{
    unsigned int cur_id = SCI_IdentifyThread();
    MMK_duer_call_func_struct *pParam = SCI_NULL;
    pParam = (MMK_duer_call_func_struct *)DUER_CALLOC(1, sizeof(MMK_duer_call_func_struct));
    if (pParam)
    {
        pParam->func = duer_gui_msg_deal_example;
        pParam->userData = SCI_NULL;

        duer_sub_send_message(cur_id, P_APP, APP_DUER_CALL_FUNC, (MMK_duer_call_func_struct *)pParam, sizeof(MMK_duer_call_func_struct));
    }
}
#else //V2改版：复用MMIAPICOM_OtherTaskToMMI结构，最小改动解决非UI线程调用UI接口
#include "mmi_osbridge.h"
PUBLIC void MMK_duer_call_func(void *param)
{
    MmiOtherTaskMsgS* data_ptr = (MmiOtherTaskMsgS*)param;
    
    SCI_ASSERT( PNULL != data_ptr ); /*assert verified*/
    
    if (MMK_IsOpenWin(data_ptr->handle)) 
    {
        MMK_PostMsg( data_ptr->handle, data_ptr->msg_id, data_ptr->param_ptr, data_ptr->size_of_param );
    }
    else
    {
        // SCI_TRACE_HIGH("MMK_duer_call_func---win id is not open:%d", data_ptr->handle);
        DUER_LOGE("MMK_duer_call_func---win id is not open:%d", data_ptr->handle);
    }
    
    //释放额外的内存
    if( PNULL != data_ptr->param_ptr )
    {
        SCI_FREE( data_ptr->param_ptr );
    }
}

PUBLIC void MMK_duer_other_task_to_MMI( 
                                     MMI_HANDLE_T     handle,
                                     MMI_MESSAGE_ID_E msg_id,
                                     DPARAM           param_ptr,
                                     uint32           size_of_param
                                     )
{
    MmiOtherTaskMsgS *sendSignal = PNULL;

    MmiCreateSignal(APP_DUER_CALL_FUNC, sizeof(MmiOtherTaskMsgS), (MmiSignalS**)&sendSignal);

    sendSignal->Sender = P_APP;

    sendSignal->handle         = handle;
    sendSignal->msg_id         = msg_id;
    sendSignal->size_of_param  = size_of_param;

    if( PNULL != param_ptr )
    {
        sendSignal->param_ptr = SCI_ALLOC_APP(size_of_param);
        
        MMI_MEMCPY( 
            sendSignal->param_ptr,
            size_of_param, 
            param_ptr,
            size_of_param,
            size_of_param);
    }

    MmiSendSignal(P_APP, (MmiSignalS*)sendSignal);
}

#endif

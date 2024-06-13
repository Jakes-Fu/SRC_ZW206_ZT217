#include "mmk_type.h"
#include "graphics_draw.h"
#include "mmk_app.h"
#include "mmidisplay_color.h"
#include "window_parse.h"
#include "formula_export.h"
#include "formula_image.h"
#include "os_api.h"
#include "dal_time.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmi_appmsg.h"
#include "guibutton.h"
#include "guifont.h"
#include "mmi_theme.h"
#include "mmidisplay_data.h"
#include "mmi_timer_export.h"
#include "os_api.h"
#include "mmk_type.h"
#include "formula_id.h"
#include "mmi_default.h"
#include "mmiacc_id.h"
#include "mmiacc_position.h"
#include "mmialarm_text.h"
#include "mmialarm_image.h"
#include "mmiset_export.h"
#include "mmiacc_nv.h"
#include "guistring.h"
#include "mmimp3_export.h"
#define FMUL_MAX   9
#define RING_MAXNUM 5
uint32 fl_1=1;
uint32 fl_2=1;
typedef struct  
{
    uint32                      max_size;       //�����������ֵ
    MMI_WIN_ID_T                win_id;         //������Ϣ��Ŀ�Ĵ���
    wchar                       ticked_file[MMIFILE_FULL_PATH_MAX_LEN+1];  //file need to be ticked on
    uint16                      ticked_file_len; //file name len
    int32                       ring_vol;
#ifdef DRM_SUPPORT
    MMIFMM_DRM_LIMIT_TYPE_T     drm_limit;      //���������Ƿ�������޻����Ƿ�ת���������޵�
#endif
}MMIFMM_SELECT_WIN_PARAM_T;
typedef enum
{
  FORMULA_STATUS_STOP,
  FORMULA_STATUS_PAUSE,
  FORMULA_STATUS_RUNNING,
    FORMULA_STATUS_TEST
} FORMULA_STATUS_E;

typedef struct  //�����Լ���ʱ������
{
    uint8   dsec;       //one-tenth of the second-[0,9]
    uint8   sec;        // secondsafter the minute - [0,59]
    uint8   min;        // minutesafter the hour - [0,59]
    uint32  hour;       // hours since midnight - [0-...]
}FORMULA_TIME_T;
uint32	ring_id_f = RING_FORMULA_R1,ring_vol = 0;

/**--------------------------------------------------------------------------*
**                         MACRO DEFINITION                                  *
**---------------------------------------------------------------------------*/
#define FORMULA_TIME_STRLENGTH        (32)  // X...X:XX:XX.X(Сʱ���7λ)


#define FORMULA_TIME_DISP_RECT        DP2PX_RECT(42,84,197,130)
#define FORMULA_TIME_DISP_BG_RECT     DP2PX_RECT(42,84,197,130)
#define FORMULA_TIME_DISP_FONT_BIG    DP2PX_FONT(SONG_FONT_48)
#define FORMULA_TIME_DISP_FONT_SMALL  DP2PX_FONT(SONG_FONT_36)
#define FORMULA_BTN_SINGLE_RECT       DP2PX_RECT(91,172,149,230)
#define FORMULA_BTN_TWO_LEFT_RECT     DP2PX_RECT(51,172,100,210)
#define FORMULA_BTN_TWO_RIGHT_RECT    DP2PX_RECT(120,172,168,210)
#define FORMULA_BTN_FORMULA_TEST_RECT    DP2PX_RECT(188,172,218,210)
#define FORMULA_TIMER_INTERVAL        (15)
/**--------------------------------------------------------------------------*
**                         LOCAL VARIABLES                                   *
**---------------------------------------------------------------------------*/
LOCAL uint64 s_FORMULA_start_time   = 0;    //ÿ�ΰ���start��ʱ�� (��dsecΪ��λ)
LOCAL uint64 s_FORMULA_stop_time    = 0;    //ÿ�ΰ���stopʱ��time2�Ѿ���ʱ������ʱ�� (��dsecΪ��λ)
LOCAL uint64 s_FORMULA_current_time = 0;    //�ܼ�ʱʱ��,��ĿǰΪֹ������ʱ��(��dsecΪ��λ)
LOCAL uint8  s_FORMULA_timeId       = 0;    //�����timerID, ��ʱʱ��Ϊ0
LOCAL FORMULA_STATUS_E s_FORMULA_status = FORMULA_STATUS_STOP;

/*---------------------------------------------------------------------------*
**                          LOCAL FUNCTION                                   *
**---------------------------------------------------------------------------*/
LOCAL void Formula_TimeBackgroundDisplay( MMI_WIN_ID_T win_id )
{
	GUI_RECT_T          dispRect    = {0};
    GUI_LCD_DEV_INFO    lcdDevInfo  = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUIRES_DisplayImg(PNULL, &dispRect, PNULL, win_id, IMAGE_BG,&lcdDevInfo);
}

LOCAL void Formula_SetBtnVisibleStatus( FORMULA_STATUS_E status)
{
    if (FORMULA_STATUS_RUNNING == status)
    {
        GUIBUTTON_SetVisible(FORMULA_CTRL_STOP, TRUE,  FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_START,  FALSE, FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_RESET, TRUE,  FALSE);
		 GUIBUTTON_SetVisible(FORMULA_CTRL_TEST, TRUE,  FALSE);
    }
    else if(FORMULA_STATUS_PAUSE == status)
    {
        GUIBUTTON_SetVisible(FORMULA_CTRL_STOP, FALSE, FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_START,  TRUE,  FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_RESET, TRUE,  FALSE);
		GUIBUTTON_SetVisible(FORMULA_CTRL_TEST, TRUE,  FALSE);
    }
    else if(FORMULA_STATUS_STOP == status)
    {
        GUIBUTTON_SetVisible(FORMULA_CTRL_STOP, FALSE, FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_START,  TRUE,  FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_RESET, FALSE, FALSE);
		GUIBUTTON_SetVisible(FORMULA_CTRL_TEST, FALSE,  TRUE);
    }
	 else if(FORMULA_STATUS_TEST== status)
    {
		GUIBUTTON_SetVisible(FORMULA_CTRL_STOP, TRUE,  FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_START, FALSE, FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_RESET,  FALSE,  FALSE);
        GUIBUTTON_SetVisible(FORMULA_CTRL_TEST, TRUE, FALSE);
    }
    else
    {
        SCI_TRACE_LOW("status = %d",status);
    }
}


LOCAL void Formula_GetCurTimeAndFont( uint8 *str_ptr, const uint64 curent_time, GUI_FONT_T *font_ptr )
{
    FORMULA_TIME_T    curTime    = {0};
    uint32              timeTemp   = (uint32)(curent_time / 10);   //bug1999095
    uint32              timeSec    = 0;
    if (PNULL == str_ptr)
    {
        SCI_TRACE_LOW("str_ptr is PNULL!");
        return;
    }
    curTime.dsec    = (uint32)timeTemp % 100;
    timeSec         = (uint32)timeTemp / 100;//��secΪ��λ   //bug1999095
    curTime.sec     = timeSec % 60;
    timeSec         = timeSec / 60; //��minΪ��λ
    curTime.min     = timeSec % 60;
    timeSec         = timeSec / 60; //��hourΪ��λ
    curTime.hour    = timeSec;
}

LOCAL void Formula_ResetTime( MMI_WIN_ID_T win_id )
{
    GUI_RECT_T  btnRect = FORMULA_BTN_SINGLE_RECT;

    if(FORMULA_STATUS_STOP == s_FORMULA_status)
    {
        SCI_TRACE_LOW("s_FORMULA_status = %d",s_FORMULA_status);
        return;
    }

    if( s_FORMULA_timeId != 0)
    {
        MMK_StopTimer(s_FORMULA_timeId);
        s_FORMULA_timeId   = 0;
    }
    s_FORMULA_current_time  = 0;
    s_FORMULA_start_time    = 0;
    s_FORMULA_stop_time     = 0;
    s_FORMULA_status        = FORMULA_STATUS_STOP;
	fl_1=1;
	fl_2=1;
	ring_id_f=RING_FORMULA_R1;
    GUIBUTTON_SetRect(FORMULA_CTRL_START, &btnRect);
    Formula_SetBtnVisibleStatus(s_FORMULA_status);
    MMK_SendMsg(win_id, MSG_FULL_PAINT, NULL);
}
LOCAL void Formula_DisplayCurTime(
                                            MMI_WIN_ID_T    win_id,
                                            GUI_RECT_T      time_rect,
                                            GUISTR_STYLE_T  *style_ptr,
                                            uint64          current_time
                                            )
{
    uint8               timeStr[FORMULA_TIME_STRLENGTH+1]     = {0};
    wchar               timeTemp[FORMULA_TIME_STRLENGTH+1]    = {0};
    GUISTR_STATE_T      state       = GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE;
    GUI_LCD_DEV_INFO    lcdDevInfo  = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_STRING_T        curTime     = {0};
    size_t              strLen      = 0;
    GUI_FONT_T          font        = 0;    
	GUI_LCD_DEV_INFO lcd_dev_info = {0};
			GUI_RECT_T rect={0};
			uint16 prod=0;
			 GUI_RECT_T win_rect = {0};
    Formula_GetCurTimeAndFont(timeStr, current_time, &font);// BUG- 1327280
    strLen = strlen((char *)timeStr);
    style_ptr->font = font;// BUG- 1327280
	if(current_time>1000){
		rect.top=75;
		rect.left=15;
		 GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMAGE_S_BG,&lcd_dev_info);
		rect.top=80;
		rect.left=50;						 
		  GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMG_DIGIT_FORMULA_NUM0+(fl_2),&lcd_dev_info);						  
		 rect.left+=28;
		 GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMAGE_MUL,&lcd_dev_info);						
		  rect.left+=28;
		GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMG_DIGIT_FORMULA_NUM0+(fl_1),&lcd_dev_info);						
		rect.left+=28;
		GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMAGE_DY,&lcd_dev_info);					
		rect.left+=28;
		prod=(fl_1)*(fl_2);
		if(prod>9){
		GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMG_DIGIT_FORMULA_NUM0+prod/10,&lcd_dev_info);							
		 rect.left+=20;
		GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMG_DIGIT_FORMULA_NUM0+prod%10,&lcd_dev_info);
							}
		  else{
		 GUIRES_DisplayImg(&rect, PNULL, PNULL,win_id,IMG_DIGIT_FORMULA_NUM0+prod,&lcd_dev_info);
		  }     
		ring_vol = MMIAPIENVSET_GetCurModeRingVol( MMISRVAUD_RING_FMT_MIDI);
		MMIAPISET_PlayRingByIdEx(TRUE,ring_id_f,ring_vol, 1,   MMISRVAUD_RING_FMT_MIDI, PNULL);
						SCI_Sleep(1500);		
		SCI_TRACE_LOW("[formula]:%d",ring_id_f);
		 	 if(fl_1>fl_2){fl_2++;}
		  else  {
			  fl_1++;
			  fl_2=1;
				 }
		  ring_id_f++;
		current_time=0;
		if(10==fl_1||10==fl_2){ring_id_f=RING_FORMULA_R1;Formula_ResetTime(win_id);
		    }							
	}
    GUISTR_DrawTextToLCDInRect(
                                (const GUI_LCD_DEV_INFO *)&lcdDevInfo,
                                (const GUI_RECT_T       *)&rect,       //the fixed display area
                                (const GUI_RECT_T       *)&rect,       //�û�Ҫ���е�ʵ������
                                (const MMI_STRING_T     *)&curTime,
                                style_ptr,
                                state,
                                GUISTR_TEXT_DIR_AUTO
                                );
}

LOCAL void Formula_TimeUpdate_CallBack( uint8 timer_id )
{
    SCI_TICK_TIME_T tickTime    = {0};//��ǰʱ�̣�ticktime
    uint32          currentTime = 0;  //��ticktimeת��Ϊ��dsecΪ��λ������
    SCI_TRACE_LOW("timer_id = %d , s_FORMULA_timeId = %d",timer_id,s_FORMULA_timeId);
    if (timer_id == s_FORMULA_timeId)
    {
        if(FORMULA_STATUS_RUNNING == s_FORMULA_status)
        {
            SCI_GetTickTime(&tickTime);
            currentTime = tickTime.second * 1000 + tickTime.milliseconds;
            s_FORMULA_current_time  = s_FORMULA_stop_time + currentTime - s_FORMULA_start_time;//�ܼ�ʱʱ��
            if (FORMULA_WIN_ID == MMK_GetFocusWinId()||FORMULA_WIN_ID == MMK_GetFocusChildWinId())
            {
                if ( FORMULA_STATUS_RUNNING == s_FORMULA_status )//ֻ�м�ʱ����ʱ��ˢ��
                {
                    MMK_SendMsg(FORMULA_WIN_ID, MSG_ACC_UPDATE_FORMULA, PNULL);
                }
                else
                {
                    SCI_TRACE_LOW("s_FORMULA_status = %d",s_FORMULA_status);
                }
            }
        }
        else
        {
            if( s_FORMULA_timeId != 0)
            {
                MMK_StopTimer(s_FORMULA_timeId);
                s_FORMULA_timeId   = 0;
            }
        }
    }
}

LOCAL void Formula_StartOrPauseTime( MMI_WIN_ID_T win_id )
{
    SCI_TICK_TIME_T tickTime    = {0};
    uint64          currentTime = 0; 
    GUI_RECT_T      leftBtnRect = FORMULA_BTN_TWO_LEFT_RECT;
    GUI_RECT_T      rightBtnRect= FORMULA_BTN_TWO_RIGHT_RECT;
	 GUI_RECT_T      test_rect= FORMULA_BTN_FORMULA_TEST_RECT;
	
    if(FORMULA_STATUS_RUNNING == s_FORMULA_status)
    {
        if( s_FORMULA_timeId != 0)
        {
            MMK_StopTimer(s_FORMULA_timeId);
            s_FORMULA_timeId   = 0;
        }
        s_FORMULA_stop_time   = s_FORMULA_current_time;
        s_FORMULA_status      = FORMULA_STATUS_PAUSE;
        GUIBUTTON_SetRect(FORMULA_CTRL_RESET, &leftBtnRect);
        GUIBUTTON_SetRect(FORMULA_CTRL_START, &rightBtnRect);

        GUIWIN_SetSoftkeyTextId(win_id, TXT_RESET,TXT_COMMON_PLAY,STXT_RETURN,FALSE);
    }
    else //û��running������£����µ�Ϊstart(������stop���Ǵ�δ��ʼ�����ƴ���)
    {
        SCI_GetTickTime(&tickTime);
        currentTime             = ((uint64)(tickTime.second) * 1000 + (uint64)(tickTime.milliseconds));
        s_FORMULA_start_time  = currentTime;
        s_FORMULA_status      = FORMULA_STATUS_RUNNING;
        s_FORMULA_timeId      = MMK_CreateTimerCallback(FORMULA_TIMER_INTERVAL,Formula_TimeUpdate_CallBack,NULL,TRUE);
        GUIBUTTON_SetRect(FORMULA_CTRL_RESET, &leftBtnRect);
        GUIBUTTON_SetRect(FORMULA_CTRL_STOP, &rightBtnRect);
		  GUIBUTTON_SetRect(FORMULA_CTRL_TEST, &test_rect);
        GUIWIN_SetSoftkeyTextId(win_id, TXT_RESET,TXT_PAUSE,STXT_RETURN,FALSE);
	

    }
    Formula_SetBtnVisibleStatus(s_FORMULA_status);

    MMK_SendMsg(win_id, MSG_FULL_PAINT, NULL);

}

LOCAL void WATCHCOM_DisplayStopwatchBackground( MMI_WIN_ID_T win_id )
{
    GUI_RECT_T         disp_rect    = MMITHEME_GetFullScreenRect();
    GUI_LCD_DEV_INFO   lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
 MMI_STRING_T text_str = {0};
	 	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_string = {0};
	GUI_RECT_T title_rect = {40, 78, 240, 118};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    GUIRES_DisplayImg(PNULL, &disp_rect, PNULL, win_id, IMAGE_BG,&lcd_dev_info);	
	text_style.align = ALIGN_LVMIDDLE;
	text_style.font = SONG_FONT_20;
	text_style.font_color = MMI_WHITE_COLOR;
	MMIRES_GetText(TXT_FORMULA_ACTION,win_id,&text_string);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&title_rect,
		&title_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

}
extern void MMIZMT_CreateMathCountWin(void);
LOCAL MMI_RESULT_E HandleMsgFormulaWindow( MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param )
{
    MMI_RESULT_E    recode      = MMI_RESULT_TRUE;
	GUI_RECT_T      rect    = {0};  
    GUISTR_STYLE_T  timeStyle   = {0};
	GUI_RECT_T      timeRect    = {0};

    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            //����button����ʾ�ַ�
            if (FORMULA_STATUS_RUNNING == s_FORMULA_status)
            {
                GUI_RECT_T leftBtnRect = FORMULA_BTN_TWO_LEFT_RECT;
                GUI_RECT_T rightBtnRect = FORMULA_BTN_TWO_RIGHT_RECT;
                GUIBUTTON_SetRect(FORMULA_CTRL_RESET, &leftBtnRect);
                GUIBUTTON_SetRect(FORMULA_CTRL_STOP, &rightBtnRect);

                GUIWIN_SetSoftkeyTextId(win_id, TXT_RESET,TXT_PAUSE,STXT_RETURN,FALSE);
            }
            else if(FORMULA_STATUS_PAUSE == s_FORMULA_status)
            {
                GUI_RECT_T leftBtnRect = FORMULA_BTN_TWO_LEFT_RECT;
                GUI_RECT_T rightBtnRect = FORMULA_BTN_TWO_RIGHT_RECT;
                GUIBUTTON_SetRect(FORMULA_CTRL_RESET, &leftBtnRect);
                GUIBUTTON_SetRect(FORMULA_CTRL_START, &rightBtnRect);

                GUIWIN_SetSoftkeyTextId(win_id, TXT_RESET,TXT_COMMON_PLAY,STXT_RETURN,FALSE);
            }
            else if(FORMULA_STATUS_STOP == s_FORMULA_status)
            {
                GUI_RECT_T  btnRect = FORMULA_BTN_SINGLE_RECT;
                GUIBUTTON_SetRect(FORMULA_CTRL_START, &btnRect);
            }
			  else if(FORMULA_STATUS_TEST == s_FORMULA_status)
			  {
              GUI_RECT_T  rest_rect = FORMULA_BTN_FORMULA_TEST_RECT;
                GUIBUTTON_SetRect(FORMULA_CTRL_TEST, &rest_rect);
				  GUIWIN_SetSoftkeyTextId(win_id, TXT_RESET,TXT_PAUSE,STXT_RETURN,FALSE);
            }
            else
            {
                SCI_TRACE_LOW("s_FORMULA_status = %d",s_FORMULA_status);
            }
            Formula_SetBtnVisibleStatus(s_FORMULA_status);
        }
        break;
 case MSG_FULL_PAINT:
        {

            WATCHCOM_DisplayStopwatchBackground(win_id);
        }
        break;

        case MSG_CTL_OK:
        case MSG_APP_OK:
        {
            Formula_ResetTime(win_id);
        }
        break;

        case MSG_CTL_MIDSK:
        case MSG_APP_WEB:
        {
            Formula_StartOrPauseTime(win_id);
        }
        break;

        case MSG_KEYDOWN_CANCEL:
        case MSG_CTL_CANCEL:
        {  
			MMK_StopTimer(s_FORMULA_timeId);
                s_FORMULA_timeId   = 0;
            MMK_CloseWin(win_id);
			
        }
        break;

        case MSG_ACC_UPDATE_FORMULA:
        {
			
            
          Formula_TimeBackgroundDisplay(win_id);
            Formula_DisplayCurTime(win_id, timeRect, &timeStyle, s_FORMULA_current_time);
        }        
        break;

#ifdef TOUCH_PANEL_SUPPORT
        case MSG_CTL_PENOK:
        {
            if(FORMULA_CTRL_RESET == ((MMI_NOTIFY_T*)param)->src_id)//reset button��ѡ��
            {
                Formula_ResetTime(win_id);
            }
            else if(FORMULA_CTRL_START == ((MMI_NOTIFY_T*)param)->src_id)//play button��ѡ��
            {
                Formula_StartOrPauseTime(win_id);
            }
            else if(FORMULA_CTRL_STOP == ((MMI_NOTIFY_T*)param)->src_id)//pause button��ѡ��
            {
				MMK_StopTimer(s_FORMULA_timeId);
                s_FORMULA_timeId   = 0;
                Formula_StartOrPauseTime(win_id);
            }
			else if(FORMULA_CTRL_TEST==((MMI_NOTIFY_T*)param)->src_id)
			{
				   MMK_StopTimer(s_FORMULA_timeId);
                s_FORMULA_timeId   = 0;
				MMIZMT_CreateMathCountWin();
			}
        }
        break;
#endif //TOUCH_PANEL_SUPPORT

        case MSG_KEYDOWN_RED:
            break;

        case MSG_KEYUP_RED:
        {
            SCI_TRACE_LOW("[HandleMsgFormulaWindow]:receive MSG_KEYDOWN_RED");
            if(FORMULA_STATUS_RUNNING == s_FORMULA_status)
            {
                Formula_StartOrPauseTime(win_id);
            }
            MMK_CloseWin(win_id);
            break;
        }

        case MSG_CLOSE_WINDOW:
        {
            if (FORMULA_STATUS_STOP == s_FORMULA_status)
            {
                if( s_FORMULA_timeId != 0)
                {
                    MMK_StopTimer(s_FORMULA_timeId);
                    s_FORMULA_timeId   = 0;
                }
                s_FORMULA_current_time = 0;
                s_FORMULA_start_time   = 0;
                s_FORMULA_stop_time    = 0;
                s_FORMULA_status       = FORMULA_STATUS_STOP;
            }
        }
        break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}
WINDOW_TABLE(FORMULA_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(FORMULA_WIN_ID),
    WIN_FUNC((uint32)HandleMsgFormulaWindow),
    CREATE_BUTTON_CTRL(FORMULA_RESET, FORMULA_CTRL_RESET),
    CREATE_BUTTON_CTRL(FORMULA_START,  FORMULA_CTRL_START),
    CREATE_BUTTON_CTRL(FORMULA_STOP, FORMULA_CTRL_STOP),
	  CREATE_BUTTON_CTRL(FORMULA_TEST, FORMULA_CTRL_TEST),
    END_WIN
};
PUBLIC void Create_Formula_Win()
{
    MMK_CreateWin(FORMULA_WIN_TAB, NULL);
}
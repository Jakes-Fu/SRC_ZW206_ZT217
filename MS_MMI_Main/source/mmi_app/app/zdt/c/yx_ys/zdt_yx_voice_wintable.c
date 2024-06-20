/****************************************************************************
** File Name:      mmiphsapp_wintab.c                                           *
** Author:          jianshengqi                                                       *
** Date:           03/22/2006                                              *
** Copyright:      2006 TLT, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the PHS                   *
/****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2006       Jianshq         Create
** 
****************************************************************************/
#ifndef _MMIZDT_YX_VOICE_WINTAB_C_
#define _MMIZDT_YX_VOICE_WINTAB_C_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmicc_export.h"
#include "guitext.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "guilistbox.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "mmi_common.h"

#include "zdt_app.h"
#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "img_dec_interface.h"
#include "guiownerdraw.h"
#include "graphics_draw.h"
#include "mmiparse_export.h"
#include "mmicl_export.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef ENG_SUPPORT
#include "mmieng_export.h"
#endif
#include "mmiset_export.h"
#include "mmiset_id.h"

#include "zdt_win_export.h"
#include "mmicc_internal.h"
#include "watch_commonwin_export.h"
#include "guiiconlist.h"


#define SCALE  1

extern void GT_Qcode_show(const unsigned char *QR_TEST_STR,int x0,int y0,int w0,int h0);

#define z_abs(x)  ((x) >= 0 ? (x) : (-(x)))

LOCAL THEMELIST_ITEM_STYLE_T new_style = {0};

LOCAL GUI_LCD_DEV_INFO  s_record_panel_layer_handle = { 0,UILAYER_NULL_HANDLE};

PUBLIC void MMIAPI_Zdt_Alert_Win(MMI_TEXT_ID_T              text_id)
{
    MMIPUB_OpenAlertWarningWin(text_id);
}

#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
#include "mmisrvrecord_export.h"
#include "zdt_yx_voice.h"
#define ZDT_TITLE_HEIGHT 45
#define TINYCHAT_PLAY_ANIM_DELAY 500
#define TINY_CHAT_LIST_MAX_SIZE   MAX_YX_VOC_SAVE_SIZE
#define TINYCHAT_RECORD_ANIM_DELAY 500
#define TINYCHAT_MAX_RECORD_TIME_MS 10000
#define TINYCHAT_BOTTOM_HEIGHT  63    //52  //xiongkai 58//120
#define TINYCHAT_LIS_BOX_BOTTOM (MMI_MAINSCREEN_HEIGHT -1 - TINYCHAT_BOTTOM_HEIGHT)//(240-83)//180//160

#define  TINYCHAT_RECORD_ANIMAL_TOTAL_FRAME_NUM  4

#define MAX_REMAIN_SIZE  (50*1024) //10K
extern YX_GROUP_INFO_DATA_T m_vchat_all_group_info_arr[YX_DB_APPUSER_MAX_SUM+YX_DB_FRIEND_MAX_SUM+1];
extern uint16 m_vchat_all_group_info_index[YX_DB_APPUSER_MAX_SUM+YX_DB_FRIEND_MAX_SUM+1];
extern uint16 m_vchat_all_group_sum;

PUBLIC void MMIZDT_OpenChatGroupWin(void);
LOCAL void MMIZDT_ShowPlayAudioIcon(uint16 list_index,BOOLEAN isLeft);
PUBLIC void MMIZDT_CleanPlayAudioIcon(void);
void MMIZDT_TinyChatRecordStop(void);
void MMIZDT_TinyChatRecordStart(void);

LOCAL MMI_RESULT_E  HandleZDT_TinyChatWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );

LOCAL RECORD_SRV_HANDLE tiny_chat_record_handle = PNULL;
static uint8 tiny_chat_record_anim_timer_id = 0;
LOCAL uint32 tiny_chat_record_update_anim_index =0;
LOCAL uint32 tiny_chat_record_timer_index =0;
LOCAL uint8 tiny_chat_is_recording =0;
LOCAL uint8 tiny_chat_play_anim_timer_id = 0;

static uint8 tiny_chat_max_record_timer_id = 0;

LOCAL uint8 s_playing_anim_index = 0;
LOCAL int16 s_playing_index = -1;
LOCAL BOOLEAN s_playing_is_left = FALSE;

static int s_chat_tp_x;
static int s_chat_tp_y;
static BOOLEAN s_is_chat_tp_down = FALSE;
static BOOLEAN s_is_chat_tp_long = FALSE;
static uint16 s_cur_last_list_index = 0;
static uint16 s_cur_last_list_top_index = 0;

MMISRV_HANDLE_T      tinychat_playmedia_handle = 0;
static BOOLEAN tinychat_is_play_stream = FALSE;

uint8 g_friendPP_timer_id = 0;
uint8 g_friendPP_shakecount = 0;

int16 gsensor_x,gsensor_y,gsensor_z;

static void ZdtTalk_BackLight(BOOLEAN is_alway_on)
{
      if(is_alway_on)
      {
		MMIDEFAULT_SetAlwaysHalfOnBackLight(FALSE);
		MMIDEFAULT_AllowTurnOffBackLight(FALSE);
		MMIDEFAULT_TurnOnBackLight();
      }
      else
      {
		MMIDEFAULT_AllowTurnOffBackLight(TRUE);
		MMIDEFAULT_SetAlwaysHalfOnBackLight(FALSE);
      }
}

#if 1
LOCAL uint8 s_vchat_send_wait_win_status = 0;
LOCAL uint8 s_vchat_send_wait_timer_id = 0;

void MMIZDT_VchatSendWait_HandleOpenTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
   if(MMK_IsOpenWin(MMIZDT_VCHAT_SEND_WAIT_WIN_ID))
   {
        s_vchat_send_wait_win_status = 2;
        MMK_PostMsg(MMIZDT_VCHAT_SEND_WAIT_WIN_ID, MSG_FULL_PAINT, PNULL,PNULL);
   }
}

void MMIZDT_VchatSendWait_HandleCloseTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
    if(MMK_IsOpenWin(MMIZDT_VCHAT_SEND_WAIT_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_VCHAT_SEND_WAIT_WIN_ID);
    }
}

PUBLIC void MMIZDT_VchatSendWait_TimerStop()
{
    if(s_vchat_send_wait_timer_id != 0)
    {
        MMK_StopTimer(s_vchat_send_wait_timer_id);
    }
}

PUBLIC void MMIZDT_VchatSendWait_TimerStart(uint32 time_out, MMI_TIMER_FUNC func)
{
    if(s_vchat_send_wait_timer_id != 0)
    {
        MMK_StopTimer(s_vchat_send_wait_timer_id);
    }
    s_vchat_send_wait_timer_id = MMK_CreateTimerCallback(time_out, func, 0, FALSE);
}

void MMIZDT_VchatSendWait_Win_Show(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T	cur_str_t	= {0};
    GUI_RECT_T	 cur_rect		   = {0};
    GUI_RECT_T	 rect		 = {0}; 
    GUISTR_STYLE_T		text_style		= {0};
    GUISTR_STATE_T		state = 		GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    GUI_FONT_T f_big =SONG_FONT_30;
    GUI_RECT_T	 title_rect		   = {0};
    uint16 wstr_txt[GUILIST_STRING_MAX_NUM+1] = {0};
    uint16 wstr_waiting[10] = {0x53D1,0x9001,0x4E2D,0x002E,0x002E,0x002E,0x0 }; //语音发送中...
    uint16 wstr_ok[10] = {0x53D1,0x9001,0x6210,0x529F,0x0021,0x0 }; //发送成功!
    uint16 wstr_fail[10] = {0x53D1,0x9001,0x5931,0x8D25,0x0021,0x0 }; //发送失败!
    uint16 wstr_fail_recode[10] = {0x5F55,0x97F3,0x5931,0x8D25,0x0021,0x0 }; //录音失败!
    rect.left	= 0;
    rect.top	= 0;
    rect.right	= MMI_MAINSCREEN_WIDTH-1;
    rect.bottom = MMI_MAINSCREEN_HEIGHT-1;
    cur_rect = rect;
    title_rect = rect;
    
    GUI_FillRect(&lcd_dev_info,rect,MMI_BLACK_COLOR);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = f_big;
    text_style.font_color = MMI_WHITE_COLOR;

    if(s_vchat_send_wait_win_status == 0)
    {
        cur_str_t.wstr_ptr = wstr_waiting;
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
        GUISTR_DrawTextToLCDInRect( 
                                                      (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                      (const GUI_RECT_T      *)&cur_rect,       
                                                      (const GUI_RECT_T      *)&cur_rect,     
                                                      (const MMI_STRING_T    *)&cur_str_t,
                                                      &text_style,
                                                      state,
                                                      GUISTR_TEXT_DIR_AUTO
                                                      ); 
    }
    else if(s_vchat_send_wait_win_status == 1)
    {
        cur_str_t.wstr_ptr = wstr_ok;
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
        GUISTR_DrawTextToLCDInRect( 
                                                      (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                      (const GUI_RECT_T      *)&cur_rect,       
                                                      (const GUI_RECT_T      *)&cur_rect,     
                                                      (const MMI_STRING_T    *)&cur_str_t,
                                                      &text_style,
                                                      state,
                                                      GUISTR_TEXT_DIR_AUTO
                                                      ); 
    }
    else if(s_vchat_send_wait_win_status == 2)
    {
        cur_str_t.wstr_ptr = wstr_fail_recode;
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
        GUISTR_DrawTextToLCDInRect( 
                                                      (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                      (const GUI_RECT_T      *)&cur_rect,       
                                                      (const GUI_RECT_T      *)&cur_rect,     
                                                      (const MMI_STRING_T    *)&cur_str_t,
                                                      &text_style,
                                                      state,
                                                      GUISTR_TEXT_DIR_AUTO
                                                      ); 
    }
    else if(s_vchat_send_wait_win_status == 3)
    {
        cur_str_t.wstr_ptr = wstr_fail;
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
        GUISTR_DrawTextToLCDInRect( 
                                                      (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                      (const GUI_RECT_T      *)&cur_rect,       
                                                      (const GUI_RECT_T      *)&cur_rect,     
                                                      (const MMI_STRING_T    *)&cur_str_t,
                                                      &text_style,
                                                      state,
                                                      GUISTR_TEXT_DIR_AUTO
                                                      ); 
    }
    return;
}

LOCAL MMI_RESULT_E  HandleZDT_VchatSendWaitWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
	MMI_RESULT_E				recode			=	MMI_RESULT_TRUE;
	GUI_RECT_T		bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
	GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
	GUISTR_STYLE_T		text_style = {0};
	GUISTR_STATE_T		state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
	GUI_RECT_T rect = {0,0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_HEIGHT -1)};
	MMI_STRING_T cur_str_t	 = {0};
	GUI_RECT_T	  cur_rect			= {0};
	GUI_FONT_T f_big =SONG_FONT_42;
       text_style.align = ALIGN_HVMIDDLE;
       text_style.font = f_big;
       text_style.font_color = MMI_BLACK_COLOR;
	switch(msg_id) {
		case MSG_OPEN_WINDOW:
                    ZdtTalk_BackLight(TRUE);
                    s_vchat_send_wait_win_status = 0;
			 GUI_FillRect(&lcd_dev_info,rect,MMI_BLACK_COLOR);	
                    MMIZDT_VchatSendWait_TimerStart(50000,MMIZDT_VchatSendWait_HandleOpenTimer);
			break;
            
           case MSG_FULL_PAINT:
                    MMIZDT_VchatSendWait_Win_Show(win_id);
                    if(s_vchat_send_wait_win_status != 0)
                    {
                        MMIZDT_VchatSendWait_TimerStart(1000,MMIZDT_VchatSendWait_HandleCloseTimer);
                    }
           	     break;
		
#ifdef TOUCH_PANEL_SUPPORT
	case MSG_TP_PRESS_DOWN:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
		
	case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
            #ifdef WIN32
                    if((point.x > 0 && point.x <50)&&(point.y >= 0 &&  point.y < ZDT_TITLE_HEIGHT))
                    {
                        //YX_Net_Receive_PPR(&g_yx_app,m_pCurSendInfo->send_id,SCI_STRLEN(m_pCurSendInfo->send_id));
                    }
            #endif
		}
		break;
		
	case MSG_TP_PRESS_MOVE:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
	case MSG_TP_PRESS_LONG:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
#endif

		case MSG_CTL_MIDSK:
            break;
            
		case MSG_LOSE_FOCUS:
            break;
                
		case MSG_CTL_CANCEL:
		case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
			break;

		case MSG_CLOSE_WINDOW:
            ZdtTalk_BackLight(FALSE);
			break;
			
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;

}

WINDOW_TABLE( MMIZDT_VCHAT_SEND_WAIT_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_VchatSendWaitWinMsg),
    WIN_ID(MMIZDT_VCHAT_SEND_WAIT_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

BOOLEAN MMIZDT_OpenVchatSendWait_Win(void)
{
    if(TRUE == MMK_IsFocusWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        if(FALSE == MMK_IsOpenWin(MMIZDT_VCHAT_SEND_WAIT_WIN_ID))
        {
            MMK_CreateWin((uint32*)MMIZDT_VCHAT_SEND_WAIT_WIN_TAB,PNULL);
        }
    }
    return TRUE;
}

PUBLIC BOOLEAN MMIZDT_UpdateVchatSendWait_Win(uint8 result)
{
    BOOLEAN ret = FALSE;
    ZDT_LOG("MMIZDT_UpdateVchatSendWait_Win result=%d",result);
    MMIZDT_VchatSendWait_TimerStop();
    if(MMK_IsFocusWin(MMIZDT_VCHAT_SEND_WAIT_WIN_ID))
    {
        if(result == 0)
        {
            s_vchat_send_wait_win_status = 1;
            MMK_PostMsg(MMIZDT_VCHAT_SEND_WAIT_WIN_ID, MSG_FULL_PAINT, PNULL,0);
        }
        else if(result == 1)
        {
            s_vchat_send_wait_win_status = 2;
            MMK_PostMsg(MMIZDT_VCHAT_SEND_WAIT_WIN_ID, MSG_FULL_PAINT, PNULL,0);
        }
        else
        {
            s_vchat_send_wait_win_status = 3;
            MMK_PostMsg(MMIZDT_VCHAT_SEND_WAIT_WIN_ID, MSG_FULL_PAINT, PNULL,0);
        }
    }
    return ret;
}
#endif

WINDOW_TABLE( MMIZDT_TINY_CHAT_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_TinyChatWinMsg),    
    WIN_ID( MMIZDT_TINY_CHAT_WIN_ID),
    WIN_HIDE_STATUS,
    //WIN_TITLE(TXT_BH_NET_CHAT),
    END_WIN
};

BOOLEAN MMIZDT_OpenTinyChatWin(void)
{
    if(FALSE == MMK_IsOpenWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        m_pCurGroupInfo = &m_vchat_all_group_info_arr[0];
        MMK_CreateWin((uint32*)MMIZDT_TINY_CHAT_WIN_TAB,PNULL);
    }
    return TRUE;
}

BOOLEAN MMIZDT_OpenTinyChatWinByGroupID(uint32 pos_user_data)
{
    if(FALSE == MMK_IsOpenWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        m_pCurGroupInfo = &m_vchat_all_group_info_arr[pos_user_data];
        MMK_CreateWin((uint32*)MMIZDT_TINY_CHAT_WIN_TAB,PNULL);
    }
    return TRUE;
}


BOOLEAN MMIZDT_CloseTinyChatWin(void)
{
    if(MMK_IsOpenWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_TINY_CHAT_WIN_ID);
    }
    return TRUE;
}

PUBLIC BOOLEAN MMIZDT_IsInTinyChatWin()
{
    
    if(MMK_IsOpenWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        return TRUE;
    }
    return FALSE;
}



PUBLIC void MMIAPIMENU_EnterTinyChat(void)
{
      MMIZDT_OpenChatGroupWin();
}

void MMIZDT_TinyShowBottom(MMI_WIN_ID_T  win_id)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_STRING_T    cur_str_t   = {0};
    GUI_RECT_T   rect        = {0}; 
    GUISTR_STYLE_T      text_style      = {0};  
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    //uint16 tittle[10] = {0x957F, 0x6309, 0x5E95 ,0x90E8, 0x8BB2, 0x8BDD, 0}; //长按此处讲话957F 6309 6B64 5904 8BB2 8BDD 
    uint16 tittle[10] = {0x6309,0x4F4F,0x8BB2,0x8BDD, 0}; ///按住讲话
    GUI_FONT_T f_big = SONG_FONT_26;//SONG_FONT_22;//SONG_FONT_19;//SONG_FONT_34;
    GUI_POINT_T      point = {0};
    GUI_RECT_T   record_text_rect = WECHART_RECORD_TEXT_RECT;
    
    rect.left = 0;
    rect.top= TINYCHAT_LIS_BOX_BOTTOM;
    rect.right= (MMI_MAINSCREEN_WIDTH -1);
    rect.bottom = (MMI_MAINSCREEN_HEIGHT-1); 
  
    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

    point.x = WECHART_RECORD_IMG_X;
    point.y = WECHART_RECORD_IMG_Y;

    GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, IMAGE_CHAT_NO_RECORD_BG, &lcd_dev_info); 

#ifdef WECHAT_SEND_EMOJI //表情
    point.x = WECHART_EMOJI_IMG_X;
    point.y = WECHART_EMOJI_IMG_Y;
    GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, tinychat_face_icon, &lcd_dev_info); 
    record_text_rect.left += WECHART_EMOJI_IMG_X + 10;
#endif

    text_style.align = ALIGN_LVMIDDLE; // ALIGN_HVMIDDLE;
    text_style.font = f_big;
    text_style.font_color = MMI_WHITE_COLOR;

    cur_str_t.wstr_len = MMIAPICOM_Wstrlen(tittle);
    cur_str_t.wstr_ptr = tittle;

    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        (const GUI_RECT_T      *)&record_text_rect,       
        (const GUI_RECT_T      *)&record_text_rect,     
        (const MMI_STRING_T    *)&cur_str_t,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
        );
    return;
}

LOCAL void MMIZDT_StartRecordAnimTimer()
{
    if(tiny_chat_record_anim_timer_id != 0)
    {
        MMK_StopTimer(tiny_chat_record_anim_timer_id);
    }
    tiny_chat_record_anim_timer_id =  MMK_CreateWinTimer(MMIZDT_TINY_CHAT_WIN_ID, TINYCHAT_RECORD_ANIM_DELAY,TRUE);

}

LOCAL void MMIZDT_StopRecordAnimTimer()
{
    if(tiny_chat_record_anim_timer_id != 0)
    {
        MMK_StopTimer(tiny_chat_record_anim_timer_id);
    }
    tiny_chat_record_anim_timer_id = 0;

}

LOCAL void MMIZDT_StartRecordMaxTimeTimer()
{
    if(tiny_chat_max_record_timer_id != 0)
    {
        MMK_StopTimer(tiny_chat_max_record_timer_id);
    }
    tiny_chat_max_record_timer_id =  MMK_CreateWinTimer(MMIZDT_TINY_CHAT_WIN_ID, TINYCHAT_MAX_RECORD_TIME_MS, FALSE);

}

LOCAL void MMIZDT_StopRecordMaxTimeTimer()
{
    if(tiny_chat_max_record_timer_id != 0)
    {
        MMK_StopTimer(tiny_chat_max_record_timer_id);
    }
    tiny_chat_max_record_timer_id = 0;

}



LOCAL void MMIZDT_ClearTinyChatRecordAnim(MMI_WIN_ID_T win_id)
{
    MMIZDT_TinyShowBottom(win_id);
}

LOCAL MMI_IMAGE_ID_T MMIZDT_GetTinyChatRecordAnimalFrame(uint32 cur_frame_index)
{
    #if 0
    MMI_IMAGE_ID_T  anim_frame[TINYCHAT_RECORD_ANIMAL_TOTAL_FRAME_NUM] = {IMAGE_TINYCHAT_TALK_ANIM_01, IMAGE_TINYCHAT_TALK_ANIM_02, 
                                                                        IMAGE_TINYCHAT_TALK_ANIM_03, IMAGE_TINYCHAT_TALK_ANIM_04};
    if(cur_frame_index < TINYCHAT_RECORD_ANIMAL_TOTAL_FRAME_NUM)
    {
        {
            return anim_frame[cur_frame_index];

        }
    }
    #endif
    return 0;
}

LOCAL void CreateRecordSecondsPanelPaintLayer(MMI_WIN_ID_T   win_id)
{
    uint16   layer_width = 0;
    uint16   layer_height = 0;
    UILAYER_CREATE_T    create_info = { 0 };
    UILAYER_APPEND_BLT_T        append_layer = { 0 };

    if (UILAYER_NULL_HANDLE == s_record_panel_layer_handle.block_id)
    {
        //get tips layer width height
        GUILCD_GetLogicWidthHeight(GUI_MAIN_LCD_ID, &layer_width, &layer_height);

        //creat layer
        create_info.lcd_id = GUI_MAIN_LCD_ID;
        create_info.owner_handle = win_id;
        create_info.offset_x = 0;
        create_info.offset_y = 0;
        create_info.width = layer_width;
        create_info.height = layer_height;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE;
        UILAYER_CreateLayer(&create_info, &s_record_panel_layer_handle);

        append_layer.lcd_dev_info = s_record_panel_layer_handle;
        append_layer.layer_level = UILAYER_LEVEL_HIGH;//添加的图层优先级高
        UILAYER_AppendBltLayer(&append_layer);
    }

    if (UILAYER_IsLayerActive(&s_record_panel_layer_handle))
    {
        UILAYER_Clear(&s_record_panel_layer_handle);
    }

}

LOCAL void ReleaseRecordPanelPaintLayer()
{

    if (UILAYER_NULL_HANDLE != s_record_panel_layer_handle.block_id)
    {
        UILAYER_ReleaseLayer(&s_record_panel_layer_handle);
    }
    s_record_panel_layer_handle.block_id = UILAYER_NULL_HANDLE;
    s_record_panel_layer_handle.lcd_id = 0;
}

#define EMOJI_SIZE 10
#ifdef WECHAT_SEND_EMOJI

LOCAL void Create_Emoji_IconList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    GUI_BORDER_T   select_border = {0};
    GUI_RECT_T list_rect = APP_LIST_RECT;
    GUI_BG_T bg = {0};
    GUIICONLIST_MARGINSPACE_INFO_T margin_space = WECHART_EMOJI_LIST_ITEM_MARGIN_SPACE;
    select_border.type = GUI_BORDER_NONE;
    select_border.width = 0;
    GUIICONLIST_SetHideSlideBar(ctrl_id);
    GUIICONLIST_SetItemBorderStyle(ctrl_id, FALSE, &select_border);//不需要边框
    GUIICONLIST_SetItemBorderStyle(ctrl_id, TRUE, &select_border);//不需要选中边框
    GUIICONLIST_SetTotalIcon(ctrl_id,EMOJI_SIZE);
    GUIICONLIST_SetRect(ctrl_id,&list_rect);
    GUIAPICTRL_SetBothRect(ctrl_id, &list_rect);
    GUIICONLIST_SetIconItemSpace(ctrl_id, margin_space);
    GUIICONLIST_SetStyle(ctrl_id, GUIICONLIST_STYLE_ICON_UIDT);
    CTRLICONLIST_SetLayoutStyle(ctrl_id,GUIICONLIST_LAYOUT_V);
    GUIICONLIST_SetCurIconIndex(0, ctrl_id);
    GUIICONLIST_SetIconWidthHeight(ctrl_id, WECHART_EMOJI_ICON_WIDTH, WECHART_EMOJI_ICON_HEIGHT); 
    GUIICONLIST_SetLoadType(ctrl_id, GUIICONLIST_LOAD_ALL);
    bg.bg_type = GUI_BG_COLOR;
    bg.color = MMI_BLACK_COLOR;
    GUIICONLIST_SetBg(ctrl_id, &bg); 
    MMK_SetAtvCtrl(win_id,ctrl_id);
}

LOCAL void Append_Emoji_Icon(MMI_CTRL_ID_T ctrl_id, DPARAM param)
{
    uint16 icon_index = *((uint16 *)param);
    GUIANIM_DATA_INFO_T  data_info = {0};
    data_info.img_id = tinychat_face_0+icon_index;
    GUIICONLIST_AppendIcon(icon_index,ctrl_id,&data_info,PNULL);
}

LOCAL MMI_RESULT_E  HandleZDT_TinyChatEmojiWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode	=	MMI_RESULT_TRUE;
	GUI_RECT_T	bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
	GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_CTRL_ID_T  ctrl_id = MMIZDT_TINY_CHAT_EMOJI_LIST_CTRL_ID;
    switch(msg_id) 
    {
		case MSG_OPEN_WINDOW:
            GUI_FillRect(&lcd_dev_info,bg_rect,MMI_BLACK_COLOR);
            Create_Emoji_IconList(win_id, ctrl_id);
            break;
            
        case MSG_FULL_PAINT:
            break;
		case MSG_CTL_ICONLIST_APPEND_ICON:
            Append_Emoji_Icon(ctrl_id, param);
            break;
	    case MSG_TP_PRESS_DOWN:
		    break;
		
	    case MSG_TP_PRESS_UP:
		    break;	

        case MSG_CTL_PENOK:
            {
                uint16 cur_index = GUIICONLIST_GetCurIconIndex(ctrl_id);
                YX_API_Emoji_Send((uint8)cur_index);
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_KEYDOWN_RED:    
		case MSG_LOSE_FOCUS:
            break;
        case MSG_KEYUP_RED:        
		case MSG_CTL_CANCEL:
		case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
			break;

		case MSG_CLOSE_WINDOW:
			break;
			
		default:
			recode = MMI_RESULT_FALSE;
			break;
	    }
	return recode;
}

WINDOW_TABLE( MMIZDT_CHAT_EMOJI_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_TinyChatEmojiWinMsg),    
    WIN_ID( MMI_WATCH_WECHAT_EMOJI_WIN_ID),
    CREATE_ICONLIST_CTRL(MMIZDT_TINY_CHAT_EMOJI_LIST_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

BOOLEAN MMIZDT_OpenTinyChatEmojiWin(void)
{
    if(MMK_IsOpenWin(MMI_WATCH_WECHAT_EMOJI_WIN_ID))
    {
        MMK_CloseWin(MMI_WATCH_WECHAT_EMOJI_WIN_ID);    
    }
    MMK_CreateWin((uint32*)MMIZDT_CHAT_EMOJI_WIN_TAB,PNULL);
    return TRUE;
}

#endif

LOCAL void MMIZDT_DisplayTinyChatRecordAnim(MMI_WIN_ID_T win_id)
{
    MMI_IMAGE_ID_T cur_img_id = 0;
    GUI_POINT_T      point = {50, 240-40};
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    char disp_str[100] = {0};
    wchar disp_wstr[100] = {0};
    MMI_STRING_T    cur_str_t   = {0};
    GUI_RECT_T   rect        = {0}; 
    GUISTR_STYLE_T      text_style      = {0};  
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    GUI_FONT_T f_big = SONG_FONT_30;//SONG_FONT_28;//SONG_FONT_34;
    uint16 tittle[10] = {0x6b63, 0x5728, 0x5f55, 0x97f3,0}; //正在录音
    uint16 tip[25]    = {0x677e, 0x5f00, 0x53d1, 0x9001,0xff0c,0x4e0a,0x5212,0x53d6,0x6d88,0}; //松开发送，上划取消
    GUI_RECT_T   record_text_rect = WECHART_RECORD_TEXT_RECT;
    GUI_RECT_T   record_tip_rect = WECHART_RECORD_TIP_RECT;
    GUI_RECT_T   countdown_rect = WECHART_RECORD_COUNTDOWN_TEXT_RECT;
    GUI_RECT_T win_rect = { 0 };

    MMK_GetWinRect(win_id, &win_rect);
    CreateRecordSecondsPanelPaintLayer(win_id);

    GUI_FillRect(&s_record_panel_layer_handle, win_rect, MMI_BLACK_COLOR);
    UILAYER_WeakLayerAlpha(&s_record_panel_layer_handle, 0x80);

    rect.left = 0;
    rect.top= TINYCHAT_LIS_BOX_BOTTOM;
    rect.right= MMI_MAINSCREEN_WIDTH-1;
    rect.bottom = MMI_MAINSCREEN_HEIGHT-1; 
    
    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

    point.x = WECHART_RECORD_IMG_X;
    point.y = WECHART_RECORD_IMG_Y;

    GUIRES_DisplayImg(&point,
                PNULL,
                PNULL,
                win_id,
                IMAGE_CHAT_RECORDING_BG, 
                &lcd_dev_info);

    point.x = 62;
    point.y = 30;
   
    GUIRES_DisplayImg(&point,
                PNULL,
                PNULL,
                win_id,
                IMAGE_CHAT_RECORDING_SECONDS,
                &s_record_panel_layer_handle);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = f_big;
    text_style.font_color = MMI_WHITE_COLOR;

    cur_str_t.wstr_len = sprintf((char*)disp_str, (char*) "%dS",((TINYCHAT_MAX_RECORD_TIME_MS/1000)-(tiny_chat_record_timer_index/2)));
    cur_str_t.wstr_ptr = disp_wstr;
    MMI_STRNTOWSTR(cur_str_t.wstr_ptr, cur_str_t.wstr_len, (uint8*)disp_str, cur_str_t.wstr_len, cur_str_t.wstr_len);
    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)&s_record_panel_layer_handle,
        (const GUI_RECT_T      *)&countdown_rect,       
        (const GUI_RECT_T      *)&countdown_rect,     
        (const MMI_STRING_T    *)&cur_str_t,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
        );
  
#ifdef WECHAT_SEND_EMOJI //表情
    point.x = WECHART_EMOJI_IMG_X;
    point.y = WECHART_EMOJI_IMG_Y;
    GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, tinychat_face_icon, &lcd_dev_info); 
    record_text_rect.left += WECHART_EMOJI_IMG_X + 10;
#endif

    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = SONG_FONT_26;//SONG_FONT_22;
    text_style.font_color = MMI_WHITE_COLOR;

    cur_str_t.wstr_len = MMIAPICOM_Wstrlen(tittle);
    cur_str_t.wstr_ptr = tittle;

    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        (const GUI_RECT_T      *)&record_text_rect,       
        (const GUI_RECT_T      *)&record_text_rect,     
        (const MMI_STRING_T    *)&cur_str_t,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
        ); 

    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = SONG_FONT_18;//SONG_FONT_22;
    text_style.font_color = MMI_WHITE_COLOR;

    cur_str_t.wstr_len = MMIAPICOM_Wstrlen(tip);
    cur_str_t.wstr_ptr = tip;

    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO*)&s_record_panel_layer_handle,
        (const GUI_RECT_T*)&record_tip_rect,
        (const GUI_RECT_T*)&record_tip_rect,
        (const MMI_STRING_T*)&cur_str_t,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
    ); 
}

#ifndef ZYB_APP_SUPPORT
LOCAL void MMIZDT_TinyChatAudioPlayNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
    MMISRVAUD_REPORT_T *report_ptr = PNULL;
    //AI_APP_T *   pMe = (AI_APP_T *)&g_ai_app;
    BOOLEAN       result = TRUE;
    
    if(param != PNULL && handle > 0)
    {
        report_ptr = (MMISRVAUD_REPORT_T *)param->data;
        if(report_ptr != PNULL && handle == tinychat_playmedia_handle)
        {
            switch(report_ptr->report)
            {
                case MMISRVAUD_REPORT_END:  
                    ZDT_LOG("MMIZDT_TinyChatAudioPlayNotify MMISRVAUD_REPORT_END data1=%d" ,report_ptr->data1);
                   
                    MMISRVAUD_Stop(handle);
                    MMISRVMGR_Free(handle);
                    tinychat_playmedia_handle = 0;
                    MMIZDT_CleanPlayAudioIcon();

                    if (MMISRVAUD_REPORT_RESULT_STOP != report_ptr->data1)
                    {
                        if (MMISRVAUD_REPORT_RESULT_SUCESS != report_ptr->data1)
                        {
                            result = FALSE;
                        }
                    }

                    break;

                default:
                    break;
            }
        }
    }
}
#endif

LOCAL BOOLEAN MMIZDT_TinyChatAudioPlayRequestHandle( 
                        MMISRV_HANDLE_T *audio_handle,
                        MMISRVAUD_ROUTE_T route,
                        MMISRVAUD_TYPE_U *audio_data,
                        MMISRVMGR_NOTIFY_FUNC notify,
						MMISRVAUD_VOLUME_T volume
                        )
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    
    req.notify = notify;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.duation = 0;
    audio_srv.eq_mode = 0;
    audio_srv.is_mixing_enable = FALSE;
    audio_srv.play_times = 1;
    audio_srv.all_support_route = route;
    audio_srv.volume =  volume;

    switch(audio_data->type)
    {
    case MMISRVAUD_TYPE_RING_FILE:
        audio_srv.info.ring_file.type = audio_data->type;
        audio_srv.info.ring_file.fmt  = audio_data->ring_file.fmt;
        audio_srv.info.ring_file.name = audio_data->ring_file.name;
        audio_srv.info.ring_file.name_len = audio_data->ring_file.name_len;

        if(audio_srv.volume > MMISRVAUD_VOLUME_LEVEL_MAX)
        {
            audio_srv.volume = MMISRVAUD_VOLUME_LEVEL_MAX;
        }
        break;

    case MMISRVAUD_TYPE_RECORD_FILE:
        audio_srv.info.record_file.type = audio_data->type;        
        audio_srv.info.record_file.fmt  = audio_data->record_file.fmt;
        audio_srv.info.record_file.name = audio_data->record_file.name;
        audio_srv.info.record_file.name_len = audio_data->record_file.name_len;    
        audio_srv.info.record_file.source   = audio_data->record_file.source;
        audio_srv.info.record_file.frame_len= audio_data->record_file.frame_len;

        audio_srv.volume = AUD_MAX_SPEAKER_VOLUME;
        break;

    case MMISRVAUD_TYPE_IQ://测试模式
        audio_srv.info.iq_file.type = audio_data->type;
        audio_srv.info.iq_file.name_ptr = audio_data->iq_file.name_ptr;
        audio_srv.info.iq_file.param    = audio_data->iq_file.param;

        audio_srv.volume = AUD_MAX_SPEAKER_VOLUME;
        break;
        
    default:
        break;
    }

    *audio_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
    ZDT_LOG("MMIZDT_TinyChatAudioPlayRequestHandle = %d", *audio_handle);    
    if(*audio_handle > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

LOCAL void MMIZDT_StartPlayAnimTimer()
{
    if(tiny_chat_play_anim_timer_id != 0)
    {
        MMK_StopTimer(tiny_chat_play_anim_timer_id);
    }
    tiny_chat_play_anim_timer_id =  MMK_CreateWinTimer(MMIZDT_TINY_CHAT_WIN_ID, TINYCHAT_PLAY_ANIM_DELAY,TRUE);

}

LOCAL void MMIZDT_StopPlayAnimTimer()
{
    if(tiny_chat_play_anim_timer_id != 0)
    {
        MMK_StopTimer(tiny_chat_play_anim_timer_id);
    }
    tiny_chat_play_anim_timer_id = 0;

}

LOCAL void MMIZDT_SetPlayAudioIcon(uint8 anim_idx)
{
    CTRLLIST_ITEM_T *pre_item_ptr = PNULL;
    GUILIST_ITEM_DATA_T *pre_item_data_ptr = NULL;
    if(s_playing_index >= 0)
    {
        pre_item_ptr = CTRLLIST_GetItem(MMIZDT_TINY_CHAT_LIST_CTRL_ID, s_playing_index);
        pre_item_data_ptr = pre_item_ptr->data_ptr;
        pre_item_data_ptr->item_content[6].item_data_type = GUIITEM_DATA_IMAGE_ID;
        if(anim_idx > 2)
        {
            anim_idx = 2;
        }
        if(s_playing_is_left)
        {
            pre_item_data_ptr->item_content[6].item_data.image_id = IMAGE_TINYCHAT_TALK_ANIM_01+anim_idx;
        }
        else
        {
            pre_item_data_ptr->item_content[6].item_data.image_id = IMAGE_TINYCHAT_MY_TALK_ANIM_01+anim_idx;
        }
        MMK_SendMsg(MMK_ConvertIdToHandle(MMIZDT_TINY_CHAT_LIST_CTRL_ID), MSG_CTL_PAINT, PNULL);
    }
    return;
}

PUBLIC void MMIZDT_CleanPlayAudioIcon(void)
{
    CTRLLIST_ITEM_T *pre_item_ptr = PNULL;
    GUILIST_ITEM_DATA_T *pre_item_data_ptr = NULL;
    if(s_playing_index >= 0)
    {
        pre_item_ptr = CTRLLIST_GetItem(MMIZDT_TINY_CHAT_LIST_CTRL_ID, s_playing_index);
        pre_item_data_ptr = pre_item_ptr->data_ptr;
        pre_item_data_ptr->item_content[6].item_data_type = GUIITEM_DATA_NONE;      
        pre_item_data_ptr->item_content[6].item_data.image_id = 0;//IMAGE_NULL;
        MMK_SendMsg(MMK_ConvertIdToHandle(MMIZDT_TINY_CHAT_LIST_CTRL_ID), MSG_CTL_PAINT, PNULL);
        MMIZDT_StopPlayAnimTimer();
        s_playing_index = -1;
    }
    return;
}

LOCAL void MMIZDT_ShowPlayAudioIcon(uint16 list_index,BOOLEAN isLeft)
{
    CTRLLIST_ITEM_T *item_ptr = CTRLLIST_GetItem(MMIZDT_TINY_CHAT_LIST_CTRL_ID, list_index);
    GUILIST_ITEM_DATA_T *item_data_ptr = NULL;
    
    if(item_ptr == NULL)
    {
        return;
    }
    ZDT_LOG("yyu MMIZDT_ShowPlayAudioIcon list_index=%d, isLeft=%d", list_index, isLeft);
    item_data_ptr =  item_ptr->data_ptr;
    s_playing_anim_index = 0;
    if(s_playing_index >= 0)
    {
        CTRLLIST_ITEM_T *pre_item_ptr = CTRLLIST_GetItem(MMIZDT_TINY_CHAT_LIST_CTRL_ID, s_playing_index);
        GUILIST_ITEM_DATA_T *pre_item_data_ptr = NULL;
        pre_item_data_ptr = pre_item_ptr->data_ptr;
        pre_item_data_ptr->item_content[2].item_data_type = GUIITEM_DATA_NONE;      
        pre_item_data_ptr->item_content[2].item_data.image_id = 0;//IMAGE_NULL;
    }
    s_playing_index = list_index;
    s_playing_is_left = isLeft;
    item_data_ptr->item_content[2].item_data_type = GUIITEM_DATA_IMAGE_ID;      
    if(isLeft)
    {
        item_data_ptr->item_content[2].item_data.image_id = IMAGE_TINYCHAT_TALK_ANIM_01;
    }
    else
    {
        item_data_ptr->item_content[2].item_data.image_id = IMAGE_TINYCHAT_MY_TALK_ANIM_01;
    }
    MMIZDT_StartPlayAnimTimer();
    MMK_SendMsg(MMK_ConvertIdToHandle(MMIZDT_TINY_CHAT_LIST_CTRL_ID), MSG_CTL_PAINT, PNULL);//ok ,list will update view
}

#ifdef ZYB_APP_SUPPORT
LOCAL int MMIZDT_TinyChatStartPlayAudio(uint16 index, uint8 * url)
{
    int res = -1;
    YX_APP_T * pMe = &g_yx_app;
    BOOLEAN isLeft = FALSE;
    char audio_url[512] = {0};
    char file_name[MAX_YX_VOC_GROUP_FULL_PATH_SIZE+1] = {0};
    ZDT_LOG("MMIZDT_TinyChatStartPlayAudio  url=%s",url);
    sscanf(url,"%[^,]",&audio_url);//努比亚增加了消息标识
    ZDT_LOG("MMIZDT_TinyChatStartPlayAudio  file_name=%s",file_name);
    SCI_MEMCPY(file_name, m_pCurGroupInfo->file_arr[index].fullname, MAX_YX_VOC_GROUP_FULL_PATH_SIZE);
    isLeft = YX_VOC_IsRcvFile(m_pCurGroupInfo->file_arr[index].fullname);
    if(tinychat_is_play_stream)
    {   
        tinychat_is_play_stream = FALSE;
        MMIZDT_CleanPlayAudioIcon();
    }
    YX_Voice_HandleStop(pMe);
    res = ZYB_StreamPlayer_Start(audio_url);
    #ifdef WIN32
        res = 0;
    #endif
    if(res == 0)
    {
        tinychat_is_play_stream = TRUE;
        MMIZDT_ShowPlayAudioIcon(index,isLeft);
    }
    return 1;
}
#else
LOCAL int MMIZDT_TinyChatStartPlayAudio(uint16 index, uint16 list_index)
{
    MMISRVAUD_TYPE_U    audio_data  = {0};
    MMISRV_HANDLE_T audio_handle = PNULL;
    uint16      full_path[MAX_YX_VOC_GROUP_FULL_PATH_SIZE] = {0};
    uint16      full_path_len = 0;
    int Ret = 0;
    char file_name[MAX_YX_VOC_GROUP_FULL_PATH_SIZE+1] = {0};
    BOOLEAN isLeft = FALSE;

    YX_APP_T * pMe = &g_yx_app;
    SCI_MEMCPY(file_name, m_pCurGroupInfo->file_arr[index].fullname, MAX_YX_VOC_GROUP_FULL_PATH_SIZE);
    
    ZDT_LOG("MMIZDT_TinyChatStartPlayAudio 1 index=%d, file_name=%s",index, file_name);
    if(file_name == PNULL || !ZDT_File_Exsit(m_pCurGroupInfo->file_arr[index].fullname))
    {
        return 1;
    }
    isLeft = YX_VOC_IsRcvFile(m_pCurGroupInfo->file_arr[index].fullname);

    YX_Voice_HandleStop(pMe); //stop rcv play and stop record
    
    if(tinychat_playmedia_handle)
    {   
        MMISRVAUD_Stop(tinychat_playmedia_handle);
        MMISRVMGR_Free(tinychat_playmedia_handle);
        tinychat_playmedia_handle = NULL;
        MMIZDT_CleanPlayAudioIcon();
    }
    
    full_path_len = GUI_GBToWstr(full_path, (const uint8*)file_name, SCI_STRLEN(file_name));
    audio_data.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
    audio_data.ring_file.name = full_path;
    audio_data.ring_file.name_len = full_path_len;
    audio_data.ring_file.fmt  = (MMISRVAUD_RING_FMT_E)MMIAPICOM_GetMusicType(audio_data.ring_file.name, audio_data.ring_file.name_len);
    
    if(MMIZDT_TinyChatAudioPlayRequestHandle(&audio_handle, MMISRVAUD_ROUTE_NONE, &audio_data, MMIZDT_TinyChatAudioPlayNotify, MMISRVAUD_VOLUME_LEVEL_MAX))
    {
        tinychat_playmedia_handle = audio_handle;
        if(!MMISRVAUD_Play(audio_handle, 0))
        {     
            ZDT_LOG("MMIZDT_TinyChatStartPlayAudio play failed");
            MMISRVMGR_Free(tinychat_playmedia_handle);
            tinychat_playmedia_handle = 0;       
            Ret = 2;
        }
        else
        {
            MMIZDT_ShowPlayAudioIcon(list_index, isLeft);
        }
    }
    else
    {        
        Ret = 3;
    }
      
    if (Ret != 0)
    {
        tinychat_playmedia_handle = NULL;
        //m_ai_eFilePlayrStatus = AI_FILE_PLAYSTOP;
        ZDT_LOG("MMIZDT_TinyChatStartPlayAudio Err Play Ret=%d",Ret);
        return Ret;
    }
    else
    {
        //ZDT_LOG("MMIZDT_TinyChatStartPlayAudio OK vol=%d",s_ai_voc_play_vol);
        //m_ai_eFilePlayrStatus = AI_FILE_PLAYING;        
    }
    return Ret;
}
#endif

LOCAL void MMIZDT_TinyChatStopPlayAudio()
{
#ifdef ZYB_APP_SUPPORT
    if(tinychat_is_play_stream)
    {
        ZYB_StreamPlayer_Stop();
        MMIZDT_CleanPlayAudioIcon();
        tinychat_is_play_stream = FALSE;
    }
#else
    if(tinychat_playmedia_handle)
    {
        MMIZDT_CleanPlayAudioIcon();
        MMISRVAUD_Stop(tinychat_playmedia_handle);
        MMISRVMGR_Free(tinychat_playmedia_handle);
        tinychat_playmedia_handle = NULL;
    }
#endif
}

PUBLIC void MMIZDT_TinyChatRefreshWin()
{
    ZDT_LOG("MMIZDT_TinyChatRefreshWin ");
    if(!MMK_IsFocusWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        return;
    }
    s_cur_last_list_index = 0xFFFF;
    s_cur_last_list_top_index = 0xFFFF;
    MMK_SendMsg(MMIZDT_TINY_CHAT_WIN_ID, MSG_FULL_PAINT, NULL);
}

PUBLIC void MMIZDT_TinyChatRecordCallBack(BOOLEAN is_ok,YXVocRecordErrStatus err_code)
{
    ZDT_LOG("MMIZDT_TinyChatRefreshWin ");
    if(!MMK_IsOpenWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        return;
    }
    if(is_ok)
    {
    }
    else
    {
        MMIZDT_TinyChatRecordStop();
        if(err_code == YX_VOCRECORD_ERR_SPACE)
        {
            MMIPUB_OpenAlertWinByTextId(PNULL,TXT_NO_SPACE,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        }
        else
        {
            MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_RECORD_FAIL,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        }
    }
}

PUBLIC MMI_IMAGE_ID_T MMIZDT_Tiny_GetHeadImageId(uint8 * group_num)
{
    MMI_IMAGE_ID_T image_id = IMAGE_NULL;
    uint16 idx = 0;

    if(strlen(group_num) > 0)
    {
        idx = atoi(group_num);
        if(idx > 0)
        {
            idx--;
        }
        if(idx > 8)
        {
            image_id = IMAGE_ZDT_TINY_OTHERMAN;
        }
        else
        {
            image_id = IMAGE_ZDT_TINY_FATHER+idx;
        }
        
    }
    else
    {
        image_id = IMAGE_ZDT_TINY_OTHERMAN;	 
    }
	return image_id;
}

LOCAL void MMIZDT_TinyChatUpdateList()
{
    BOOLEAN ret = FALSE;
    uint16 i = 0, j=0;
    uint16 position = 0;
    uint16 line_num = m_pCurGroupInfo->file_num;
    int user_index = 0;
    BOOLEAN is_group_chat = FALSE;
    GUILIST_RemoveAllItems(MMIZDT_TINY_CHAT_LIST_CTRL_ID);
    if(strcmp(m_pCurGroupInfo->group_id,YX_VCHAT_DEFAULT_GROUP_ID) == 0)
    {
        is_group_chat = TRUE;
    }
    for(i = 0 ; i < line_num ; i++)
    {        
        GUILIST_ITEM_T       		item_t    =  {0};
        GUILIST_ITEM_DATA_T  		item_data = {0};
        wchar buff[GUILIST_STRING_MAX_NUM + 1] = {0};
        uint16 total_item_num = 0;
        BOOLEAN isLeft = FALSE;
        wchar   full_path[MAX_YX_VOC_GROUP_FULL_PATH_SIZE] = {0}; 
        uint16  full_path_len = 0;

        MMISRVAUD_CONTENT_INFO_T  mp3_file_info = {0};
        char 						temp_str[202] = {0};
        wchar                       time_wstr[101] = {0};
        if(!ZDT_File_Exsit(m_pCurGroupInfo->file_arr[i].fullname))
        {
            continue;
        }
        
        isLeft = YX_VOC_IsRcvFile(m_pCurGroupInfo->file_arr[i].fullname);
        ZDT_LOG("MMIZDT_TinyChatUpdateList i=%d isLeft=%d, %s" , i, isLeft , m_pCurGroupInfo->file_arr[i].fullname);
        item_t.item_data_ptr = &item_data; 

        if(isLeft)
        {
            if(is_group_chat)
            {
                item_t.item_style = GUIITEM_STYLE_LF_ONE_LINE_BGICON_WITH_TOP_TEXT_MS;
            }
            else
            {
                item_t.item_style = GUIITEM_STYLE_LF_ONE_LINE_BGICON_TEXT_MS;
            }
        }
        else
        {
            item_t.item_style =  GUIITEM_STYLE_LF_ONE_LINE_BGICON_R_TEXT_MS;
        }
        
        full_path_len = GUI_GBToWstr(full_path, (const uint8*)m_pCurGroupInfo->file_arr[i].fullname, SCI_STRLEN(m_pCurGroupInfo->file_arr[i].fullname));

        if(m_pCurGroupInfo->status_arr[i].msg_type == 1) //语音
        {
            item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
            item_data.item_content[0].item_data.image_id = isLeft ? IMAGE_TINYCHAT_VOICE_LEFT_BG : IMAGE_TINYCHAT_VOICE_RIGHT_BG;

            if(m_pCurGroupInfo->status_arr[i].druation > 0)
            {
                SCI_MEMSET (temp_str, 0, sizeof (temp_str));
                SCI_MEMSET (time_wstr, 0, sizeof (time_wstr));
                sprintf ( (char*) temp_str, "  %d\"", m_pCurGroupInfo->status_arr[i].druation);
                MMIAPICOM_StrToWstr (temp_str, time_wstr);
            }
            else
            {
                MMISRVAUD_GetFileContentInfo (full_path,full_path_len,&mp3_file_info);

                SCI_MEMSET (temp_str, 0, sizeof (temp_str));
                SCI_MEMSET (time_wstr, 0, sizeof (time_wstr));
                if(mp3_file_info.total_time == 0) 
                {
                    mp3_file_info.total_time = 1;
                }
                sprintf ( (char*) temp_str, "  %d\"", mp3_file_info.total_time);
                MMIAPICOM_StrToWstr (temp_str, time_wstr);
            }
            item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
            item_data.item_content[1].item_data.text_buffer.wstr_ptr = time_wstr;
            item_data.item_content[1].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (time_wstr);
            if(isLeft)
            {
                item_data.item_content[1].is_custom_font_color = TRUE;
                item_data.item_content[1].custom_font_color = MMI_BLACK_COLOR;
                item_data.item_content[1].custom_font_color_focus = MMI_BLACK_COLOR;
            }
            if( m_pCurGroupInfo->status_arr[i].is_read == 0)
            {
                item_data.item_content[3].item_data_type = GUIITEM_DATA_IMAGE_ID;
                item_data.item_content[3].item_data.image_id = IMAGE_TINYCHAT_UNREAD;
            }
        }
        else if(m_pCurGroupInfo->status_arr[i].msg_type == 0) //文本
        {
            uint32 read_len = 0;
            uint8 text[256] = {0};
            ZDT_File_Read((const uint8*)m_pCurGroupInfo->file_arr[i].fullname , text, 200, &read_len);
            sscanf(text,"%[^,]",&temp_str);//努比亚增加了消息标识
            ZDT_UCS_Str16_to_uint16((uint8*)temp_str, SCI_STRLEN(temp_str) ,time_wstr, 100);
            item_data.item_content[4].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
            item_data.item_content[4].item_data.text_buffer.wstr_ptr = time_wstr;
            item_data.item_content[4].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (time_wstr);
            if( m_pCurGroupInfo->status_arr[i].is_read == 0) //文字进来要就标记已读，否则用户看到有未读消息但不知道是哪个消息未读
            {
                m_pCurGroupInfo->status_arr[i].is_read = 1;
            }
        }
        else if(m_pCurGroupInfo->status_arr[i].msg_type == 7)
        {
            uint32 read_len = 0;
            uint16 id = 0;
            ZDT_File_Read((const uint8*)m_pCurGroupInfo->file_arr[i].fullname , temp_str, 100, &read_len);
            //id = atoi(temp_str);
            sscanf(temp_str,"%d,",&id);//努比亚表情增加了消息标识
            if(id > 11)
            {
                id = 0;
            }
            item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
            item_data.item_content[0].item_data.image_id = tinychat_face_0 + id;
            if(m_pCurGroupInfo->status_arr[i].is_read == 0) //表情不需要点击，进来要就标记已读，否则用户看到有未读消息但不知道是哪个消息未读
            {
                m_pCurGroupInfo->status_arr[i].is_read = 1;
            }
        }
        
        //item_data.item_content[2].item_data_type = GUIITEM_DATA_IMAGE_ID;
        user_index = YX_DB_APPUSER_IsValid_ID(m_pCurGroupInfo->status_arr[i].friend_id);
        if(isLeft && user_index > 0 && is_group_chat) //群聊显示名称
        {
            item_data.item_content[7].font_color_id=MMITHEME_COLOR_9;
            item_data.item_content[7].is_default =TRUE; 
            SCI_MEMSET (temp_str, 0, sizeof (temp_str));
            ZDT_UCS_Str16_to_uint16((uint8*)yx_DB_AppUser_Reclist[user_index-1].appuser_name, SCI_STRLEN(yx_DB_AppUser_Reclist[user_index-1].appuser_name) ,temp_str, GUILIST_STRING_MAX_NUM);
            item_data.item_content[7].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
            item_data.item_content[7].item_data.text_buffer.wstr_ptr = temp_str;
            item_data.item_content[7].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (temp_str);
        }

        ret = GUILIST_AppendItem (MMIZDT_TINY_CHAT_LIST_CTRL_ID, &item_t);
        ZDT_LOG("MMIZDT_TinyChatUpdateList ret = %d", ret);
        if(ret)
        {
            CTRLLIST_SetItemUserData(MMIZDT_TINY_CHAT_LIST_CTRL_ID, position, &i);
            position++;
        }
    }
}

LOCAL MMI_RESULT_E MMIZDT_ChatVoice_QueryDelete(
                                                MMI_WIN_ID_T win_id, 
                                                MMI_MESSAGE_ID_E msg_id, 
                                                DPARAM param
                                                )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
    case MSG_APP_RED:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
        {
            YX_VocFileRemoveOneItem(s_cur_last_list_index);
            MMK_CloseWin(win_id);
        }
        break;

    default:
        result = MMIPUB_HandleQueryWinMsg(win_id, msg_id, param);
        break;
    }
    return result;
}

PUBLIC MMI_RESULT_E MMIZDT_TinyChatShowText_Alert(
                                    MMI_WIN_ID_T        win_id,        //IN:
                                    MMI_MESSAGE_ID_E    msg_id,        //IN:
                                    DPARAM                param        //IN:
                                    )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        break;

    case MSG_FULL_PAINT:
            recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;

    case MSG_TIMER:
            recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;

    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        MMK_CloseWin( win_id );
        break;

    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin( win_id );
        break;

    case MSG_APP_RED:
        recode = MMI_RESULT_FALSE;
        break;

    case MSG_CLOSE_WINDOW:

        recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;

    default:
        recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (recode);
}

LOCAL void MMIZDT_TinyChatShowText(uint16 * disp_wstr, uint16 disp_len)
{
    MMI_WIN_PRIORITY_E win_priority = WIN_LOWEST_LEVEL;
    MMI_STRING_T prompt_str = {0};
    uint32 time_out = 0; //5000;
    MMI_WIN_ID_T alert_win_id = MMIZDT_VCHAT_TEXT_WIN_ID;
    
    prompt_str.wstr_len = 0;
    prompt_str.wstr_ptr = PNULL;

    if (disp_wstr == PNULL || disp_len == 0)
    {
        return;
    }

    prompt_str.wstr_ptr = disp_wstr;
    prompt_str.wstr_len = disp_len;
    
    if (MMK_IsFocusWin(MMIZDT_VCHAT_TEXT_WIN_ID))
    {
        MMIPUB_SetAlertWinTextByPtr(alert_win_id,&prompt_str,PNULL,TRUE);
    }
    else
    {
        MMK_CloseWin(MMIZDT_VCHAT_TEXT_WIN_ID);

        // 窗口: 提示用户新短消息
        if(MMISMS_IsForbitNewMsgWin(TRUE))
        {
            win_priority = WIN_LOWEST_LEVEL;
        }
        else
        {
            win_priority = WIN_ONE_LEVEL;
        }
        MMIPUB_OpenAlertWinByTextPtr(&time_out,&prompt_str,PNULL,IMAGE_PUBWIN_NEWMSG,&alert_win_id,&win_priority,MMIPUB_SOFTKEY_ONE,MMIZDT_TinyChatShowText_Alert);
    }
}

void MMIZDT_TinyChatRecordStop(void)
{
    MMIZDT_StopRecordAnimTimer();
    MMIZDT_ClearTinyChatRecordAnim(MMIZDT_TINY_CHAT_WIN_ID);
    MMK_UpdateScreen();
    MMIZDT_StopRecordMaxTimeTimer();
    YX_API_Record_Stop();
    tiny_chat_is_recording = 0;
    return;
}

void MMIZDT_TinyChatRecordCancel(void)
{
    MMIZDT_StopRecordAnimTimer();
    MMIZDT_ClearTinyChatRecordAnim(MMIZDT_TINY_CHAT_WIN_ID);
    MMK_UpdateScreen();
    MMIZDT_StopRecordMaxTimeTimer();
    YX_API_Record_Cancel();
    tiny_chat_is_recording = 0;
    MMIZDT_TinyShowBottom(MMIZDT_TINY_CHAT_WIN_ID);
    return;
}

void MMIZDT_TinyChatRecordStart(void)
{
    if(tiny_chat_is_recording != 1)
    {
        MMIZDT_TinyChatStopPlayAudio();
        MMIZDT_StartRecordAnimTimer();
        MMIZDT_DisplayTinyChatRecordAnim(MMIZDT_TINY_CHAT_WIN_ID);
        MMIZDT_StartRecordMaxTimeTimer();
        YX_API_Record_Start();
        tiny_chat_is_recording = 1;
    }
    return;
}

LOCAL BOOLEAN Send_Message_Check()
{
#ifdef ZTE_WATCH
    if(!MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_1))
    {
        MMIZDT_OpenNoSimOrDataWin();
        return FALSE;
    }
#else
    if(ZDT_SIM_Exsit() == FALSE)
    {
        MMIPUB_OpenAlertWinByTextId(PNULL,STR_SIM_NOT_SIM_EXT01,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);//TXT_SIM_REJECTED
        return FALSE;
    }
    if (MMIAPICONNECTION_isGPRSSwitchedOff())
    {
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_NEED_NET,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return FALSE;
    }
#endif
    if(yx_DB_Set_Rec.net_open == 0)
    {
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_NEED_OPEN,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return FALSE;
    }
    if(ZDT_File_MemFull(FALSE))
    {
        ZDT_LOG("win ZDT_IsMemNearFull");
        MMIAPI_Zdt_Alert_Win(STR_NO_SPACE_EXT01);
        return FALSE;
    }
    if(FALSE == YX_Net_Is_Land())
    {
        ZDT_LOG("win TINY_CHAT NET Reset");
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_NEED_NET,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        if(MMIZDT_Net_IsInit())
        {
            MMIZDT_Net_Reset(FALSE);
        }
        return FALSE;
    }
    return TRUE;
}

LOCAL MMI_RESULT_E  HandleZDT_TinyChatWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUI_BG_T bg_ptr = {0};
    GUI_BG_DISPLAY_T bg_display = {0};
    const GUI_LCD_DEV_INFO *lcd_info = MMITHEME_GetDefaultLcdDev();
    MMIRECORD_SRV_RESULT_E record_result = MMIRECORD_SRV_RESULT_SUCCESS;
    MMI_CTRL_ID_T           ctrl_id = MMIZDT_TINY_CHAT_LIST_CTRL_ID;
    //MMIFMM_VIEW_WIN_DATA_T* view_win_d = (MMIFMM_VIEW_WIN_DATA_T*) MMK_GetWinUserData (win_id);
    GUILIST_INIT_DATA_T list_init = {0};    
    uint16                      cur_selection   =   0;
    static uint8 s_is_key_green_down = 0;
    uint32 pos_user_data = 0; // position user data
    uint16 index; //click item index
    GUI_RECT_T          rect         = MMITHEME_GetFullScreenRectEx(win_id);	
    GUISTR_STYLE_T      text_style      = {0};/*lint !e64*/
    GUI_RECT_T          text_rect={0,0,239,ZDT_TITLE_HEIGHT}; 
    GUI_RECT_T content_rect = {0, ZDT_TITLE_HEIGHT, MMI_MAINSCREEN_WIDTH - 1 , MMI_MAINSCREEN_HEIGHT - 1}; 
    BOOLEAN ret = FALSE;
    GUISTR_STATE_T		state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    uint32 cur_group_index = 0; 
    MMI_STRING_T        title_string = {0};	
    wchar title_wstr[GUILIST_STRING_MAX_NUM+1] = {0};
    wchar first_wstr[5] = {0x5BB6, 0x5EAD,0x7FA4, 0x804A, 0x0};//家庭群聊
    uint32 * p_index = PNULL;
    YX_APP_T * pMe = &g_yx_app;
    static uint8 s_cur_long_pen_handle = 0;
    
    p_index = (uint32 *) MMK_GetWinAddDataPtr(win_id);
    //ZDT_LOG("HandleZDT_TinyChatWinMsg msg_id = 0x%x", msg_id);
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            MMISRVAUD_ReqVirtualHandle("TINY CHAT", MMISRVAUD_PRI_NORMAL);

            //view_win_d = (MMIFMM_VIEW_WIN_DATA_T*) MMK_GetWinUserData (win_id);     
            
            list_init.both_rect.v_rect.left = 0;//0;//mic icon width is 52
            list_init.both_rect.v_rect.right = (MMI_MAINSCREEN_WIDTH -1);
            list_init.both_rect.v_rect.top = ZDT_TITLE_HEIGHT;
            list_init.both_rect.v_rect.bottom = TINYCHAT_LIS_BOX_BOTTOM;//185;//240;//180;

            list_init.both_rect.h_rect.left = 0;
            list_init.both_rect.h_rect.right = (MMI_MAINSCREEN_WIDTH -1);//185;//240;//180;
            list_init.both_rect.h_rect.top = ZDT_TITLE_HEIGHT;
            list_init.both_rect.h_rect.bottom = TINYCHAT_LIS_BOX_BOTTOM; //240;
            
            list_init.type = GUILIST_TEXTLIST_E;
                        
            GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);            
            MMK_SetAtvCtrl(win_id,ctrl_id);

            //不需要分割线
            GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE );
            //不画高亮条
            GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE );
            GUILIST_SetListState( ctrl_id, GUILIST_STATE_TEXTSCROLL_ENABLE | GUILIST_STATE_AUTO_SCROLL, TRUE );//长文本滚动
            CTRLLIST_SetTextFont(ctrl_id, SONG_FONT_24, MMI_CYAN_COLOR);
            GUILIST_SetListState(ctrl_id, GUILIST_STATE_EFFECT_STR,TRUE);//item 自定义文本颜色
            ret = GUILIST_SetMaxItem(ctrl_id, TINY_CHAT_LIST_MAX_SIZE, FALSE);
            s_cur_last_list_index = 0xFFFF;
            s_cur_last_list_top_index = 0xFFFF;
            s_cur_long_pen_handle = 0;
        }
        break;
        
    case MSG_LOSE_FOCUS:
            if(tiny_chat_is_recording)
            {
                MMIZDT_TinyChatRecordStop();

                if(MMICC_IsExistIncommingCall()) 
                    MMK_CloseWin(win_id);	
			   
            }
            ReleaseRecordPanelPaintLayer();
        break;
        
    case MSG_GET_FOCUS:
                s_cur_long_pen_handle = 0;
            break;
            
    case MSG_FULL_PAINT:
#if 1
      GUI_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);

	///////draw title
	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_24;
	text_style.font_color = MMI_WHITE_COLOR;
	text_style.char_space = 0;
      
        if(SCI_STRLEN(m_pCurGroupInfo->group_name) > 0)
        {
            ZDT_UCS_Str16_to_uint16((uint8*)m_pCurGroupInfo->group_name, SCI_STRLEN(m_pCurGroupInfo->group_name) ,title_wstr, GUILIST_STRING_MAX_NUM);
            title_string.wstr_ptr = title_wstr;
        }
        else if(SCI_STRLEN(m_pCurGroupInfo->group_num) > 0)
        {
            GUI_UTF8ToWstr(title_wstr, GUILIST_STRING_MAX_NUM, m_pCurGroupInfo->group_num, SCI_STRLEN(m_pCurGroupInfo->group_num));
            title_string.wstr_ptr = title_wstr;
        }
        else
        {
            title_string.wstr_ptr = first_wstr;
        }
      title_string.wstr_len = MMIAPICOM_Wstrlen(title_string.wstr_ptr);
	GUISTR_DrawTextToLCDInRect( 
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		(const GUI_RECT_T	   *)&text_rect,	   //the fixed display area
		(const GUI_RECT_T	   *)&text_rect,	   //用户要剪切的实际区域
		(const MMI_STRING_T    *)&title_string,
		&text_style,
		state,
		GUISTR_TEXT_DIR_AUTO
		);
#endif			
        if(tiny_chat_is_recording == 0)
        {
            int cur_top_index = 0;
            uint16 total_item_num = 0;
            MMIZDT_TinyShowBottom(win_id);
            MMIZDT_TinyChatUpdateList();
            total_item_num = CTRLLIST_GetTotalItemNum(MMIZDT_TINY_CHAT_LIST_CTRL_ID);
            if(s_cur_last_list_index >= total_item_num)
            {
                if(total_item_num > 0)
                {
                    s_cur_last_list_index = total_item_num-1;
                }
                else
                {
                    s_cur_last_list_index = 0;
                }
            }
            
            if(total_item_num > 0)
            {
                if(s_cur_last_list_top_index >= total_item_num || s_cur_last_list_index == (total_item_num-1))
                {
                    uint16 page_item_num = CTRLLIST_GetLastPageTopIndex(MMIZDT_TINY_CHAT_LIST_CTRL_ID);
                    s_cur_last_list_top_index = total_item_num-page_item_num;
                    GUILIST_SetTopItemIndex(MMIZDT_TINY_CHAT_LIST_CTRL_ID,s_cur_last_list_top_index);
                }
                else
                {
                    GUILIST_SetTopItemIndex(MMIZDT_TINY_CHAT_LIST_CTRL_ID,total_item_num-1);
                }
            }
            else
            {
                s_cur_last_list_top_index = 0;
            }
        }
        break;
        
    
    case MSG_CTL_LIST_NEED_ITEM_DATA:
        break;
 
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            s_chat_tp_x = point.x;
            s_chat_tp_y = point.y;
            s_is_chat_tp_down = TRUE;
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
        #ifdef WECHAT_SEND_EMOJI
            GUI_RECT_T emoji_rect = WECHART_EMOJI_IMG_RECT;
        #endif
            ReleaseRecordPanelPaintLayer();
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if((point.x > 0 && point.x <50)&&(point.y >= 0 &&  point.y < ZDT_TITLE_HEIGHT))
            {
                MMK_CloseWin(win_id);
                return;	 
            }
            if(s_is_chat_tp_long && tiny_chat_is_recording)
            {
                if(abs(point.y-s_chat_tp_y) > 20)//上滑取消
                {
                    MMIZDT_TinyChatRecordCancel();
                }
                else
                {
                    MMIZDT_TinyChatRecordStop();
                }
            }
            
            if(s_is_chat_tp_down && s_is_chat_tp_long == FALSE)
            {
                offset_y =  point.y - s_chat_tp_y;
                offset_x =  point.x - s_chat_tp_x;
                if(offset_x <= -(20))	
                {
                    MMK_CloseWin(win_id);
                }	
                else if(offset_x >= (20))
                {
                    MMK_CloseWin(win_id);
                }
            }
        #ifdef WECHAT_SEND_EMOJI
            if(GUI_PointIsInRect(point,emoji_rect))
            {
                MMIZDT_TinyChatStopPlayAudio();
                YX_Voice_HandleStop(pMe);
                if(Send_Message_Check())
                {
                    MMIZDT_OpenTinyChatEmojiWin();
                }
            }
        #endif
            s_is_chat_tp_long = FALSE;
            s_is_chat_tp_down = FALSE;
        }
        break;

    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(s_is_chat_tp_down && s_is_chat_tp_long == FALSE)
            {
                if(point.y > TINYCHAT_LIS_BOX_BOTTOM)
                {
                    if(Send_Message_Check())
                    {
                        s_is_key_green_down = 1;
                        ZDT_LOG("win TINY_CHAT_Record_Start");
                        tiny_chat_record_update_anim_index = 0;
                        tiny_chat_record_timer_index = 0;
                        s_is_chat_tp_long = TRUE;
                        MMIZDT_TinyChatRecordStart();
                    }
                    
                }
            }
            
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527
        
    case MSG_KEYLONG_WEB:    
    {
        ZDT_LOG("HandleZDT_TinyChatWinMsg MSG_KEYLONG_WEB");
        if(Send_Message_Check())
        {
            s_is_key_green_down = 1;
            tiny_chat_record_update_anim_index = 0;
            tiny_chat_record_timer_index = 0;
            s_is_chat_tp_long = TRUE;
            MMIZDT_TinyChatRecordStart();
        }
    }
    break;
    
    case MSG_KEYPRESSUP_WEB:
    {
            ZDT_LOG("MSG_KEYPRESSUP_WEB");
            if(s_is_key_green_down)
            {
                MMIZDT_TinyChatRecordStop();
                s_is_chat_tp_long = FALSE;
                s_is_key_green_down = 0;
            }
            else
            {
                MMK_CloseWin(win_id);
            }
            
    }        
    break;
    case MSG_CTL_LIST_LONGOK:
            {
                uint32 pos_user_data = 0;
                MMI_WIN_ID_T	query_win_id = MMIZDT_QUERY_WIN_ID;
                if(tiny_chat_is_recording)
                {
                    MMIZDT_TinyChatRecordStop();
                }
                MMIZDT_TinyChatStopPlayAudio();
                s_cur_long_pen_handle = 1;
                index = GUILIST_GetCurItemIndex(ctrl_id );
                s_cur_last_list_index = index;
                s_cur_last_list_top_index = GUILIST_GetTopItemIndex(ctrl_id);
                //GUILIST_GetItemData(ctrl_id, index, &pos_user_data);
                MMIPUB_OpenQueryWinByTextId(TXT_DELETE_QUERY,IMAGE_PUBWIN_QUERY,&query_win_id,MMIZDT_ChatVoice_QueryDelete);                
            }
        break;
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    //case MSG_KEYDOWN_WEB:
        if(s_cur_long_pen_handle == 0)
        {
            index = GUILIST_GetCurItemIndex(ctrl_id);
            s_cur_last_list_index = index;
            s_cur_last_list_top_index = GUILIST_GetTopItemIndex(ctrl_id);
            //GUILIST_GetItemData(ctrl_id, index, &pos_user_data);// user_data stand position
            if(m_pCurGroupInfo->status_arr[index].is_read == 0)
            {
                m_pCurGroupInfo->status_arr[index].is_read = 1;
            }
            //YX_VocFileStatusWrite(m_vchat_cur_group_info.status_arr);
            if(m_pCurGroupInfo->status_arr[index].msg_type == 0) //文本
            {
                char 						temp_str[512] = {0};
                wchar                       temp_wstr[254] = {0};
                uint8 text[512] = {0};
                uint32 read_len = 0;
                ZDT_File_Read((const uint8*)m_pCurGroupInfo->file_arr[index].fullname , temp_str, 500, &read_len);
                sscanf(temp_str,"%[^,]",&text);//努比亚增加了消息标识
                ZDT_UCS_Str16_to_uint16((uint8*)text, SCI_STRLEN(text) ,temp_wstr, 254);
                MMIZDT_TinyChatStopPlayAudio();//停止音频和播放动画，否则弹框被覆盖
                MMIZDT_TinyChatShowText(temp_wstr, MMIAPICOM_Wstrlen(temp_wstr));
            }
            else if(m_pCurGroupInfo->status_arr[index].msg_type == 1)
            {
                #ifdef ZYB_APP_SUPPORT
                    char 						temp_str[512] = {0};
                    uint32 read_len = 0;
                    ZDT_File_Read((const uint8*)m_pCurGroupInfo->file_arr[index].fullname , temp_str, 500, &read_len);
                    MMIZDT_TinyChatStartPlayAudio(index, temp_str);
                #else
                    MMIZDT_TinyChatStartPlayAudio(index, index);
                #endif
            }
            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
        break;
                
	case MSG_TIMER:
		if (*(uint8*)param == tiny_chat_record_anim_timer_id)
		{
		    if(tiny_chat_record_update_anim_index < TINYCHAT_RECORD_ANIMAL_TOTAL_FRAME_NUM)
		    {
		        tiny_chat_record_update_anim_index++;
		    }
            else
            {
                tiny_chat_record_update_anim_index = 0;
            }
            tiny_chat_record_timer_index++;
		    MMIZDT_DisplayTinyChatRecordAnim(win_id);
		}
        else if (*(uint8*)param == tiny_chat_max_record_timer_id)
        {
            MMIZDT_TinyChatRecordStop();
            s_is_chat_tp_long = FALSE;
        }
        else if (*(uint8*)param == tiny_chat_play_anim_timer_id)
        {
            if(s_playing_anim_index >= 3)
            {
                s_playing_anim_index = 0;
            }
            MMIZDT_SetPlayAudioIcon(s_playing_anim_index);
            s_playing_anim_index++;
        }
        else
        {
            recode = MMI_RESULT_FALSE;
	     ZDT_LOG("HandleZDT_TinyChatWinMsg MSG_TIMER   otherwise");					
        }
        break;

    case MSG_KEYUP_RED:
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
        break;
    case MSG_KEYDOWN_RED:
        break;

    case MSG_CLOSE_WINDOW:
        ReleaseRecordPanelPaintLayer();
        MMISRVAUD_FreeVirtualHandle("TINY CHAT"); 
        MMIZDT_StopRecordAnimTimer();
        MMIZDT_TinyChatStopPlayAudio();
        YX_Voice_HandleStop(pMe);
        tiny_chat_is_recording = 0;
        YX_VocFileStatusWrite(m_pCurGroupInfo->status_name.fullname,m_pCurGroupInfo->status_arr);
        ZdtTalk_BackLight(FALSE);
        if(p_index != PNULL)
        {
            SCI_FREE(p_index);
        }
        break;

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

PUBLIC void ZDT_Close_Tiny_Chat_Win()
{
	 if(tiny_chat_is_recording)
	 {
		 MMIZDT_TinyChatRecordStop();
	 }
	 
	 if(MMK_IsOpenWin(MMIZDT_TINY_CHAT_WIN_ID))
	 {
		 MMK_CloseWin(MMIZDT_TINY_CHAT_WIN_ID);
	 }
}

PUBLIC void ZDT_Delete_CheckClose_Tiny_Chat_Win(uint8 * friend_id)
{
    if(MMK_IsFocusWin(MMIZDT_TINY_CHAT_WIN_ID))
    {
        if(strcmp(friend_id,m_pCurGroupInfo->group_id) == 0)
        {
            MMIZDT_TinyChatStopPlayAudio();
            if(tiny_chat_is_recording)
            {
                 MMIZDT_TinyChatRecordStop();
            }
            MMK_CloseWin(MMIZDT_TINY_CHAT_WIN_ID);
        }
    }
}

PUBLIC void MMIZDT_OpenChatWinByRcv(void)
{
    m_pCurGroupInfo = m_pCurRcvGroupInfo;
    MMK_CreateWin((uint32*)MMIZDT_TINY_CHAT_WIN_TAB,PNULL);
   return;
}

#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND

LOCAL uint8 s_del_friend_win_wait_status = 0;
LOCAL uint8 s_del_friend_ui_timer_id = 0;
LOCAL uint16 s_cur_gproup_select_index = 0;
static uint16 s_cur_gproup_long_key_handle = 0;

#if 1
void MMIZDT_VChatGroup_HandleOpenTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
   if(MMK_IsOpenWin(MMIZDT_FRIEND_WAIT_WIN_ID))
   {
        s_del_friend_win_wait_status = 2;
        MMK_PostMsg(MMIZDT_FRIEND_WAIT_WIN_ID, MSG_FULL_PAINT, PNULL,PNULL);
   }
}

void MMIZDT_VChatGroup_HandleCloseTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
    if(MMK_IsOpenWin(MMIZDT_FRIEND_WAIT_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_FRIEND_WAIT_WIN_ID);
    }
}

PUBLIC void MMIZDT_VChatGroup_TimerStop()
{
    if(s_del_friend_ui_timer_id != 0)
    {
        MMK_StopTimer(s_del_friend_ui_timer_id);
    }
}

PUBLIC void MMIZDT_VChatGroup_TimerStart(uint32 time_out, MMI_TIMER_FUNC func)
{
    if(s_del_friend_ui_timer_id != 0)
    {
        MMK_StopTimer(s_del_friend_ui_timer_id);
    }
    s_del_friend_ui_timer_id = MMK_CreateTimerCallback(time_out, func, 0, FALSE);
}

void MMIZDT_ChatGroup_Win_Wating(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T	cur_str_t	= {0};
    GUI_RECT_T	 cur_rect		   = {0};
    GUI_RECT_T	 rect		 = {0}; 
    GUISTR_STYLE_T		text_style		= {0};
    GUISTR_STATE_T		state = 		GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    GUI_FONT_T f_big =SONG_FONT_30;
    GUI_RECT_T	 title_rect		   = {0};
    uint16 wstr_txt[GUILIST_STRING_MAX_NUM+1] = {0};
    uint16 wstr_waiting[10] = {0x597D,0x53CB,0x5220,0x9664,0x4E2D,0x002E,0x002E,0x002E,0x0 }; //好友删除中...
    uint16 wstr_ok[10] = {0x5220,0x9664,0x6210,0x529F,0x0021,0x0 }; //删除成功
    uint16 wstr_fail[10] = {0x5220,0x9664,0x5931,0x8D25,0x0021,0x0 }; //删除失败!
    
    rect.left	= 0;
    rect.top	= 0;
    rect.right	= MMI_MAINSCREEN_WIDTH-1;
    rect.bottom = MMI_MAINSCREEN_HEIGHT-1;
    cur_rect = rect;
    title_rect = rect;
    
    GUI_FillRect(&lcd_dev_info,rect,MMI_BLACK_COLOR);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = f_big;
    text_style.font_color = MMI_WHITE_COLOR;

    if(s_del_friend_win_wait_status == 0)
    {
        title_rect.top = 20;
        title_rect.bottom = 50;
        cur_rect.top = 60;
        cur_rect.bottom = 180;
        if(SCI_STRLEN(m_pCurGroupInfo->group_name) > 0)
        {
            ZDT_UCS_Str16_to_uint16((uint8*)m_pCurGroupInfo->group_name, SCI_STRLEN(m_pCurGroupInfo->group_name) ,wstr_txt, GUILIST_STRING_MAX_NUM);
            cur_str_t.wstr_ptr = wstr_txt;
            cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
            GUISTR_DrawTextToLCDInRect( 
                                                          (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                          (const GUI_RECT_T      *)&title_rect,       
                                                          (const GUI_RECT_T      *)&title_rect,     
                                                          (const MMI_STRING_T    *)&cur_str_t,
                                                          &text_style,
                                                          state,
                                                          GUISTR_TEXT_DIR_AUTO
                                                          ); 
       }
       else if(SCI_STRLEN(m_pCurGroupInfo->group_num) > 0)
       {
            GUI_UTF8ToWstr(wstr_txt, GUILIST_STRING_MAX_NUM, m_pCurGroupInfo->group_num, SCI_STRLEN(m_pCurGroupInfo->group_num));
            cur_str_t.wstr_ptr = wstr_txt;
            cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
            GUISTR_DrawTextToLCDInRect( 
                                                          (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                          (const GUI_RECT_T      *)&title_rect,       
                                                          (const GUI_RECT_T      *)&title_rect,     
                                                          (const MMI_STRING_T    *)&cur_str_t,
                                                          &text_style,
                                                          state,
                                                          GUISTR_TEXT_DIR_AUTO
                                                          ); 
       }
       
        cur_rect = rect;
        cur_str_t.wstr_ptr = wstr_waiting;
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
        GUISTR_DrawTextToLCDInRect( 
                                                      (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                      (const GUI_RECT_T      *)&cur_rect,       
                                                      (const GUI_RECT_T      *)&cur_rect,     
                                                      (const MMI_STRING_T    *)&cur_str_t,
                                                      &text_style,
                                                      state,
                                                      GUISTR_TEXT_DIR_AUTO
                                                      ); 
    }
    else if(s_del_friend_win_wait_status == 1)
    {
        cur_str_t.wstr_ptr = wstr_ok;
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
        GUISTR_DrawTextToLCDInRect( 
                                                      (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                      (const GUI_RECT_T      *)&cur_rect,       
                                                      (const GUI_RECT_T      *)&cur_rect,     
                                                      (const MMI_STRING_T    *)&cur_str_t,
                                                      &text_style,
                                                      state,
                                                      GUISTR_TEXT_DIR_AUTO
                                                      ); 
    }
    else if(s_del_friend_win_wait_status == 2)
    {
        cur_str_t.wstr_ptr = wstr_fail;
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(cur_str_t.wstr_ptr);
        GUISTR_DrawTextToLCDInRect( 
                                                      (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                                      (const GUI_RECT_T      *)&cur_rect,       
                                                      (const GUI_RECT_T      *)&cur_rect,     
                                                      (const MMI_STRING_T    *)&cur_str_t,
                                                      &text_style,
                                                      state,
                                                      GUISTR_TEXT_DIR_AUTO
                                                      ); 
    }
    return;
}

LOCAL MMI_RESULT_E  HandleZDT_ChatGroupWaitWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
	MMI_RESULT_E				recode			=	MMI_RESULT_TRUE;
	GUI_RECT_T		bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
	GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
	GUISTR_STYLE_T		text_style = {0};
	GUISTR_STATE_T		state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
	GUI_RECT_T rect = {0,0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_HEIGHT -1)};
	MMI_STRING_T cur_str_t	 = {0};
	GUI_RECT_T	  cur_rect			= {0};
	GUI_FONT_T f_big =SONG_FONT_42;
       text_style.align = ALIGN_HVMIDDLE;
       text_style.font = f_big;
       text_style.font_color = MMI_BLACK_COLOR;
	switch(msg_id) {
		case MSG_OPEN_WINDOW:
                    ZdtTalk_BackLight(TRUE);
                    s_del_friend_win_wait_status = 0;
			GUI_FillRect(&lcd_dev_info,rect,MMI_BLACK_COLOR);	
                    MMIZDT_VChatGroup_TimerStart(20000,MMIZDT_VChatGroup_HandleOpenTimer);
			break;
            
           case MSG_FULL_PAINT:
                    MMIZDT_ChatGroup_Win_Wating(win_id);
                    if(s_del_friend_win_wait_status != 0)
                    {
                        MMIZDT_VChatGroup_TimerStart(2000,MMIZDT_VChatGroup_HandleCloseTimer);
                    }
           	     break;
		
#ifdef TOUCH_PANEL_SUPPORT
	case MSG_TP_PRESS_DOWN:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
		
	case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
            #ifdef WIN32
            if((point.x > 0 && point.x <50)&&(point.y >= 0 &&  point.y < ZDT_TITLE_HEIGHT))
            {
                YX_Net_Receive_PPR(&g_yx_app,m_pCurGroupInfo->group_id,SCI_STRLEN(m_pCurGroupInfo->group_id));
            }
            #endif
		}
		break;
		
	case MSG_TP_PRESS_MOVE:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
	case MSG_TP_PRESS_LONG:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
#endif

		case MSG_CTL_MIDSK:
                    break;
            
		case MSG_LOSE_FOCUS:
                    break;
                
		case MSG_CTL_CANCEL:
		case MSG_APP_CANCEL:
                        MMK_CloseWin(win_id);
			break;

		case MSG_CLOSE_WINDOW:
                        ZdtTalk_BackLight(FALSE);
			break;
			
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;

}

WINDOW_TABLE( MMIZDT_FRIEND_WAIT_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_ChatGroupWaitWinMsg),    
    WIN_ID(MMIZDT_FRIEND_WAIT_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

BOOLEAN MMIZDT_OpenChatGroupWait_Win(void)
{
    MMK_CreateWin((uint32*)MMIZDT_FRIEND_WAIT_WIN_TAB,PNULL);
    return TRUE;
}
#endif

PUBLIC MMI_RESULT_E  HandleZDT_ChatGroupWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );


WINDOW_TABLE( MMIZDT_VCHAT_GROUP_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_ChatGroupWinMsg),    
    WIN_ID( MMIZDT_VCHAT_GROUP_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZDT_OpenChatGroupWin(void)
{
#ifdef ZTE_WATCH
    if(!MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_1))
    {
        MMIZDT_OpenNoSimOrDataWin();
        return;
    }
#else
    if(ZDT_SIM_Exsit() == FALSE)
    {
    	 MMIAPI_Zdt_Alert_Win(STR_SIM_NOT_SIM_EXT01);
        //MMIPUB_OpenAlertWinByTextId(PNULL,STR_SIM_NOT_SIM_EXT01,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);//TXT_SIM_REJECTED
        return;
    }
    if (MMIAPICONNECTION_isGPRSSwitchedOff())
    {
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_NEED_NET,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return;
    }
#endif
    if(yx_DB_Set_Rec.net_open == 0)
    {
        MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_NEED_OPEN,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return;
    }

   if(FALSE == MMK_IsOpenWin(MMIZDT_VCHAT_GROUP_WIN_ID))
   {
        MMK_CreateWin((uint32*)MMIZDT_VCHAT_GROUP_WIN_TAB,PNULL);
   }
   else
   {
        MMK_PostMsg(MMIZDT_VCHAT_GROUP_WIN_ID, MSG_FULL_PAINT, PNULL,PNULL);
   }
   return;
}

PUBLIC void MMIZDT_OpenChatContactListWin(void)
{
    if(FALSE == MMK_IsOpenWin(MMIZDT_VCHAT_GROUP_WIN_ID))
   {
        MMK_CreateWin((uint32*)MMIZDT_VCHAT_GROUP_WIN_TAB,PNULL);
   }
   else
   {
        MMK_PostMsg(MMIZDT_VCHAT_GROUP_WIN_ID, MSG_FULL_PAINT, PNULL,PNULL);
   }
}


PUBLIC void MMIZDT_OpenChatGroupWinByPP(uint8 * friend_id)
{
   if(FALSE == MMK_IsOpenWin(MMIZDT_VCHAT_GROUP_WIN_ID))
   {
        MMK_CreateWin((uint32*)MMIZDT_VCHAT_GROUP_WIN_TAB,(ADD_DATA)friend_id);
   }
   return;
}

PUBLIC void MMIZDT_CloseChatGroupWin()
{
    if(MMK_IsOpenWin(MMIZDT_VCHAT_GROUP_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_VCHAT_GROUP_WIN_ID);
    }
}

PUBLIC void MMIZDT_UpdateChatGroupWin(BOOLEAN is_ok)
{
    ZDT_LOG("MMIZDT_UpdateChatGroupWin is_ok = %d",is_ok);
   if(MMK_IsFocusWin(MMIZDT_VCHAT_GROUP_WIN_ID))
   {
        MMK_PostMsg(MMIZDT_VCHAT_GROUP_WIN_ID, MSG_FULL_PAINT, PNULL,PNULL);
   }
   if(MMK_IsFocusWin(MMIZDT_FRIEND_WAIT_WIN_ID))
   {
        if(is_ok)
        {
            s_del_friend_win_wait_status = 1;
        }
        else
        {
            s_del_friend_win_wait_status = 2;
        }
        MMK_PostMsg(MMIZDT_FRIEND_WAIT_WIN_ID, MSG_FULL_PAINT, PNULL,PNULL);
   }
}

LOCAL MMI_RESULT_E MMIZDT_ChatGroup_QueryDelete(
                                                MMI_WIN_ID_T win_id, 
                                                MMI_MESSAGE_ID_E msg_id, 
                                                DPARAM param
                                                )
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;

    switch(msg_id)
    {
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
    case MSG_APP_RED:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
        {
            #ifdef WIN32
                MMIZDT_OpenChatGroupWait_Win();
                MMK_CloseWin(win_id);
           #else
            if(YX_Net_Send_PPR(&g_yx_app,m_pCurGroupInfo->group_id))
            {
                MMIZDT_OpenChatGroupWait_Win();
                MMK_CloseWin(win_id);
            }
            #endif
        }
        break;

    default:
        result = MMIPUB_HandleQueryWinMsg(win_id, msg_id, param);
        break;
    }
    return result;
}

void  MMIZDT_ChatGroup_ShowList(MMI_WIN_ID_T win_id,uint8 * p_friend_id)
{
    uint16 i;
    uint32 user_data = 0;
    uint8				copy_len = 0;
    uint16 max_list_num = YX_DB_APPUSER_MAX_SUM+YX_DB_FRIEND_MAX_SUM + 1;
    MMI_CTRL_ID_T            ctrl_id =  MMIZDT_YX_CHAT_CTRL_ID;
    GUILIST_ITEM_T      first_item_t = {0};
    //GUILIST_ITEM_DATA_T first_item_data = {0};
    //uint16 first_wstr[5] = {0x5BB6, 0x5EAD,0x7FA4, 0x804A, 0x0};//家庭群聊
    uint32 unread_num = 0;
    GUILIST_RemoveAllItems(ctrl_id);
    
    GUILIST_SetMaxItem(ctrl_id, max_list_num,FALSE);

    for ( i = 0; i < m_vchat_all_group_sum; i++ )
    {
        GUILIST_ITEM_T      item_t = {0};
        GUILIST_ITEM_DATA_T item_data = {0};
        wchar name_buff[GUILIST_STRING_MAX_NUM + 1] = {0};
        wchar number_buff[GUILIST_STRING_MAX_NUM + 1] = {0};
        YX_GROUP_INFO_DATA_T * pGroupInfo = &m_vchat_all_group_info_arr[i];
        unread_num = YX_VCHAT_GetGroupUnread(pGroupInfo);
    #ifdef ZTE_WATCH
        item_t.item_style    = GUIITEM_STYLE_1ICON_1STR_1LINE_WITH_UNREAD_NUM;
    #else
        item_t.item_style    = GUIITEM_STYLE_1ICON_1STR_1LINE_LAYOUT1 ;//GUIITEM_STYLE_TWO_LINE_ANIM_TEXT_AND_TEXT_ANIM;
    #endif
        item_t.item_data_ptr = &item_data;
        
        if(SCI_STRLEN(pGroupInfo->group_name) > 0)
        {
            ZDT_UCS_Str16_to_uint16((uint8*)pGroupInfo->group_name, SCI_STRLEN(pGroupInfo->group_name) ,name_buff, GUILIST_STRING_MAX_NUM);
            item_data.item_content[1].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
            item_data.item_content[1].item_data.text_buffer.wstr_ptr = name_buff;
            item_data.item_content[1].item_data.text_buffer.wstr_len = MIN( GUILIST_STRING_MAX_NUM, MMIAPICOM_Wstrlen(item_data.item_content[1].item_data.text_buffer.wstr_ptr));
        }

        item_data.item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = WATCHCOM_GetAvaterImageId(name_buff);

        if(unread_num > 0)
        {
        #ifdef ZTE_WATCH
            char num[4] = {0};
            if(unread_num > 9)
            {
                sprintf(num,"%s", "...");
            }
            else
            {
                sprintf(num,"%d", unread_num);
            }           
            MMIAPICOM_StrToWstr(num,number_buff);
            item_data.item_content[3].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
            item_data.item_content[3].item_data.text_buffer.wstr_ptr = number_buff;
            item_data.item_content[3].item_data.text_buffer.wstr_len = MMIAPICOM_Wstrlen(item_data.item_content[3].item_data.text_buffer.wstr_ptr);
        #endif
            item_data.item_content[2].item_data_type     = GUIITEM_DATA_IMAGE_ID;
            item_data.item_content[2].item_data.image_id = IMAGE_TINYCHAT_UNREAD;
        }
     #ifndef ZTE_WATCH
        copy_len = SCI_STRLEN(pGroupInfo->group_num);
        if(copy_len > 0)
        {
            GUI_UTF8ToWstr(number_buff, GUILIST_STRING_MAX_NUM, pGroupInfo->group_num, copy_len);
        }
        else
        {
            //GUI_UTF8ToWstr(number_buff, GUILIST_STRING_MAX_NUM, pGroupInfo->group_id, SCI_STRLEN(pGroupInfo->group_id));
        }
        item_data.item_content[3].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[3].item_data.text_buffer.wstr_ptr = number_buff;
        item_data.item_content[3].item_data.text_buffer.wstr_len = MIN( GUILIST_STRING_MAX_NUM, MMIAPICOM_Wstrlen(item_data.item_content[3].item_data.text_buffer.wstr_ptr));
    #endif   
        GUILIST_AppendItem(ctrl_id, &item_t);
        user_data = i;
        if(p_friend_id != PNULL && SCI_STRLEN(p_friend_id) > 0 && strcmp(p_friend_id,pGroupInfo->group_id) == 0)
        {
            s_cur_gproup_select_index = i;
        }
        CTRLLIST_SetItemUserData(ctrl_id, i, &user_data);
    }
}

LOCAL void Create_Chat_Contact_List_Listbox(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, GUI_RECT_T rect)
{
    GUILIST_INIT_DATA_T list_init = {0};           
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);
    GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE );
    //不画高亮条
    GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE );
    GUILIST_SetRect(ctrl_id, &rect);
    MMK_SetAtvCtrl(win_id,ctrl_id);
}

LOCAL void Chat_Contact_List_Draw_Title(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO	lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUISTR_STYLE_T      text_style      = {0};/*lint !e64*/
    GUI_RECT_T          text_rect={42,0,239,MMI_SPECIAL_TITLE_HEIGHT}; 
    GUI_RECT_T content_rect = {0, MMI_SPECIAL_TITLE_HEIGHT, MMI_MAINSCREEN_WIDTH - 1 , MMI_MAINSCREEN_HEIGHT - 1}; 
    GUISTR_STATE_T		state = GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    GUI_POINT_T point = {0};
    GUI_RECT_T  rect = MMITHEME_GetFullScreenRectEx(win_id);
    MMI_STRING_T        string = {0};
    //draw TITLE bg 	
    GUIRES_DisplayImg(PNULL,&rect,PNULL,win_id,IMAGE_THEME_BLACK_BG_SPEC, &lcd_dev_info); 

    ///////draw title
    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = MMI_DEFAULT_BIG_FONT;
    text_style.font_color = MMI_WHITE_COLOR;
    text_style.char_space = 0;
        	
    MMI_GetLabelTextByLang(TXT_BH_NET_CHAT, &string);
    GUISTR_DrawTextToLCDInRect( 
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        (const GUI_RECT_T	   *)&text_rect,	   //the fixed display area
        (const GUI_RECT_T	   *)&text_rect,	   //用户要剪切的实际区域
        (const MMI_STRING_T    *)&string,
        &text_style,
        state,
        GUISTR_TEXT_DIR_AUTO
        );
}

LOCAL void Chat_Contact_List_Title_Back_Click(MMI_WIN_ID_T win_id, DPARAM param)
{
    GUI_POINT_T   point = {0};
    point.x = MMK_GET_TP_X(param);
    point.y = MMK_GET_TP_Y(param);

    if((point.x > 0 && point.x <50)&&(point.y >= 0 &&  point.y < MMI_SPECIAL_TITLE_HEIGHT))
    {
        MMK_CloseWin(win_id);
    }
    if((point.x > 190 && point.x < 240)&&(point.y >= 0 &&  point.y < MMI_SPECIAL_TITLE_HEIGHT))
    {
        #if 0
        if(strcmp(m_pCurGroupInfo->group_id,YX_VCHAT_DEFAULT_GROUP_ID) != 0)
        {
            MMI_WIN_ID_T	query_win_id = MMIZDT_QUERY_WIN_ID;
            MMIPUB_OpenQueryWinByTextId(TXT_DELETE_QUERY,IMAGE_PUBWIN_QUERY,&query_win_id,MMIZDT_ChatGroup_QueryDelete);                
        }
        #endif
    }
}

LOCAL void Chat_Contact_List_Item_LongClick(MMI_CTRL_ID_T ctrl_id)
{
    uint16 current_index = GUILIST_GetCurItemIndex(ctrl_id);
    if(strcmp(m_vchat_all_group_info_arr[current_index].group_id,YX_VCHAT_DEFAULT_GROUP_ID) != 0)
    {
        uint32 pos_user_data = 0;
        MMI_WIN_ID_T	query_win_id = MMIZDT_QUERY_WIN_ID;
        if(m_vchat_all_group_info_arr[current_index].user_type == YX_GROUP_USER_BIND 
            || m_vchat_all_group_info_arr[current_index].user_type == YX_GROUP_USER_COMMON) //绑定用户或者群聊不允许删除
        {
            ZDT_LOG("Chat_Contact_List_Item_LongClick is bind user can not delete");
        }
        else //好友
        {
            //s_cur_gproup_long_key_handle = 1;
            s_cur_gproup_select_index = current_index;
            m_pCurGroupInfo = &m_vchat_all_group_info_arr[current_index];
            MMIPUB_OpenQueryWinByTextId(TXT_DELETE_QUERY,IMAGE_PUBWIN_QUERY,&query_win_id,MMIZDT_ChatGroup_QueryDelete); 
        }               
    }
}

LOCAL void Chat_Contact_List_Item_Click(MMI_CTRL_ID_T ctrl_id)
{
    uint32 pos_user_data = 0;
    uint16 current_index;
    if(s_cur_gproup_long_key_handle == 0)
    {
        current_index = GUILIST_GetCurItemIndex(ctrl_id );
        s_cur_gproup_select_index = current_index;
        GUILIST_GetItemData(ctrl_id, current_index, &pos_user_data);
        MMIZDT_OpenTinyChatWinByGroupID(pos_user_data);
    }
}

LOCAL void Chat_Contact_Show_Tip(MMI_WIN_ID_T win_id,MMI_CTRL_ID_T ctrl_id)
{
    GUI_RECT_T rect = {0};
    GUILIST_SetRect(ctrl_id, &rect); //隐藏listbox
    WATCHCOM_DisplayTips(win_id,TXT_BIND_DEVICE_TIP);

}

PUBLIC MMI_RESULT_E  HandleZDT_ChatGroupWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_CTRL_ID_T            ctrl_id =  MMIZDT_YX_CHAT_CTRL_ID;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    MMI_WIN_ID_T                wait_win_id             =   0;
    uint16                    node_id = 0;
    uint32                      time_out                =   0;
    MMI_STRING_T        string = {0};	
    GUI_LCD_DEV_INFO	lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};	 
    GUISTR_STATE_T		state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
    GUI_RECT_T          rect         = MMITHEME_GetFullScreenRectEx(win_id);
    uint8 * p_friend_id = PNULL;
    GUISTR_STYLE_T      text_style      = {0};/*lint !e64*/
    GUI_RECT_T          text_rect={42,0,239,ZDT_TITLE_HEIGHT}; 
    GUI_RECT_T content_rect = {0, ZDT_TITLE_HEIGHT, MMI_MAINSCREEN_WIDTH - 1 , MMI_MAINSCREEN_HEIGHT - 1}; 
    GUILIST_INIT_DATA_T list_init = {0};

    p_friend_id = (uint8 *) MMK_GetWinAddDataPtr(win_id);

    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            {
                GUI_RECT_T list_rect = {0, MMI_SPECIAL_TITLE_HEIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
                Create_Chat_Contact_List_Listbox(win_id, ctrl_id,list_rect);
                GUI_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
                s_cur_gproup_select_index = 0;
                s_cur_gproup_long_key_handle = 0;
                if(m_vchat_all_group_sum == 0)
                {
                    YX_VCHAT_GetAllGroupInfo();
                }
            }
            break;
        
        case MSG_GET_FOCUS:
                s_cur_gproup_long_key_handle = 0;
            break;
                
        case MSG_FULL_PAINT:
            {
                if(struct_yx_statues_data.bingd_statues != 0) //没有绑定
                {
                    GUI_RECT_T list_rect = {0, MMI_SPECIAL_TITLE_HEIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
                    GUILIST_SetRect(ctrl_id, &rect);
                    Chat_Contact_List_Draw_Title(win_id);
                    MMIZDT_ChatGroup_ShowList(win_id,p_friend_id);
                }
                else
                {
                    GUI_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
                    Chat_Contact_Show_Tip(win_id,ctrl_id);
                }
            }
            break;
            
        case MSG_TP_PRESS_UP:
            Chat_Contact_List_Title_Back_Click(win_id,param);
            break;
        
        case MSG_CTL_LIST_LONGOK:
            Chat_Contact_List_Item_LongClick(ctrl_id);
            break;
        
        case MSG_CTL_PENOK:
        case MSG_CTL_MIDSK:
        case MSG_APP_WEB:
        case MSG_CTL_OK:
        case MSG_APP_OK:
            Chat_Contact_List_Item_Click(ctrl_id);
            break;
            
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
        case MSG_KEYDOWN_RED:
            break;

        case MSG_KEYUP_RED:
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}

PUBLIC MMI_RESULT_E HandleLauncher_ChatContactListWinMsg( MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_CTRL_ID_T            ctrl_id = MMIZDT_YX_CHAT_CTRL_ID;
    MMI_RESULT_E             recode  = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO	lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};	    
    GUI_RECT_T          rect         = MMITHEME_GetFullScreenRectEx(win_id);
    uint8 * p_friend_id = PNULL;
    p_friend_id = (uint8 *) MMK_GetWinAddDataPtr(win_id);

    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            {
                GUI_RECT_T list_rect = LAUNCHER_WECHAT_CONTACT_LIST_RECT;
                Create_Chat_Contact_List_Listbox(win_id, ctrl_id,list_rect);
                s_cur_gproup_select_index = 0;
                s_cur_gproup_long_key_handle = 0;
                if(m_vchat_all_group_sum == 0)
                {
                    YX_VCHAT_GetAllGroupInfo();
                }
                GUI_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
            }
            break;
        
        case MSG_GET_FOCUS:
                s_cur_gproup_long_key_handle = 0;
            break;
                
        case MSG_FULL_PAINT:
            {
                 ZDT_LOG("status:%d",YX_MMI_Get_Bingd_Statues());
                 if(struct_yx_statues_data.bingd_statues != 0) //没有绑定
                {
                    GUI_RECT_T list_rect = LAUNCHER_WECHAT_CONTACT_LIST_RECT;
                    GUILIST_SetRect(ctrl_id, &rect);
                    GUI_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
                    MMIZDT_ChatGroup_ShowList(win_id,p_friend_id);
                }
                else
                {
                    GUI_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
                    Chat_Contact_Show_Tip(win_id,ctrl_id);
                }
            }
            break;    
        case MSG_CTL_LIST_LONGOK:
            Chat_Contact_List_Item_LongClick(ctrl_id);
            break;
        
        case MSG_CTL_PENOK:
        case MSG_CTL_MIDSK:
        case MSG_APP_WEB:
        case MSG_CTL_OK:
        case MSG_APP_OK:
            Chat_Contact_List_Item_Click(ctrl_id);
            break;
                  
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
        break;

        case MSG_KEYDOWN_RED:
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

#endif

#endif //end chat


static uint8 s_find_friend_timer_id = 0;

LOCAL void Draw_FindFriend_Search_Text(MMI_WIN_ID_T win_id)
{
    GUISTR_STYLE_T      text_style = {0};
    MMI_STRING_T        search_str = {0};
    MMI_STRING_T        search_tip_str = {0};
    GUI_RECT_T search_str_rect = SEARCH_ING_STR_RECT;
    GUI_RECT_T search_tip_rect = SEARCH_ING_TIP_STR_RECT;
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    MMIRES_GetText(TXT_FIND_FIREND_SEARCH, win_id, &search_str);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font_color = MMI_WHITE_COLOR;
    text_style.font = SONG_FONT_22;
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,
                (const GUI_RECT_T      *)&search_str_rect,
                (const GUI_RECT_T      *)&search_str_rect,      
                (const MMI_STRING_T    *)&search_str,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );
    state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS;
    MMIRES_GetText(TXT_FIND_FIREND_SEARCH_TIP, win_id, &search_tip_str);
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,
                (const GUI_RECT_T      *)&search_tip_rect,
                (const GUI_RECT_T      *)&search_tip_rect,      
                (const MMI_STRING_T    *)&search_tip_str,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );
}

LOCAL void Draw_FindFriend_Search_Bg(MMI_WIN_ID_T win_id,const GUI_LCD_DEV_INFO *dev_info_ptr)
{
    GUI_POINT_T point = {0};
    point.x = FIND_FRIEND_PING_X;
    point.y = FIND_FRIEND_PING_Y;
    GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, find_friend_ping, dev_info_ptr);
    point.x = FIND_FRIEND_PONG_X;
    point.y = FIND_FRIEND_PONG_Y;
    GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, find_friend_waiting1, dev_info_ptr);
    point.x = FIND_FRIEND_WAITING_X;
    point.y = FIND_FRIEND_WAITING_Y;
    GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, find_friend_pong, dev_info_ptr);
}

LOCAL void FindFriend_TimeOut_Callback()
{
    MMK_CloseWin(MMIZDT_FIND_FRIEND_WIN_ID);
}

LOCAL MMI_RESULT_E  HandleZDT_FindFriendWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    GUI_RECT_T full_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUISTR_STYLE_T      text_style = {0};
    MMI_STRING_T        str_data = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUI_RECT_T tip_str_rect = {0};
    GUI_POINT_T point = {0};
    switch(msg_id) 
    {
        case MSG_OPEN_WINDOW:
            LCD_FillRect(&lcd_dev_info, full_rect, MMI_BLACK_COLOR);
            
            point.x = FIND_FRIEND_PING_X;
            point.y = FIND_FRIEND_PING_Y;
            GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, find_friend_ping, &lcd_dev_info);
            point.x = FIND_FRIEND_PONG_X;
            point.y = FIND_FRIEND_PONG_Y;
            GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, find_friend_waiting1, &lcd_dev_info);
            point.x = FIND_FRIEND_WAITING_X;
            point.y = FIND_FRIEND_WAITING_Y;
            GUIRES_DisplayImg(&point, PNULL, PNULL, win_id, find_friend_pong, &lcd_dev_info);
            Draw_FindFriend_Search_Text(win_id);
            YX_Net_Friend_Start(&g_yx_app);
            if(s_find_friend_timer_id == 0)
            {
                MMK_StopTimer(s_find_friend_timer_id);
                s_find_friend_timer_id = 0;
            }
            s_find_friend_timer_id = MMK_CreateTimerCallback(FIND_FRIEND_TIME_OUT,FindFriend_TimeOut_Callback,(uint32)win_id, FALSE);
            break;
        case MSG_FULL_PAINT: 

            break;
        case MSG_KEYUP_RED:       
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);

            break;
        case MSG_CLOSE_WINDOW:
            if(s_find_friend_timer_id == 0)
            {
                MMK_StopTimer(s_find_friend_timer_id);
                s_find_friend_timer_id = 0;
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

WINDOW_TABLE( MMIZDT_FIND_FRIEND_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_FindFriendWinMsg),    
    WIN_ID( MMIZDT_FIND_FRIEND_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};


PUBLIC void MMIZDT_OpenFindFriendWin()
{
   if(MMK_GetFocusWinId() != MMIZDT_FIND_FRIEND_WIN_ID)
   {
        MMK_CreateWin((uint32*)MMIZDT_FIND_FRIEND_WIN_TAB,PNULL);
   }
}

PUBLIC void ZdtApp_YX_Friend_DrawResult(YX_APP_T *pMe,YXFriendDispStatus status)
{
    if(status == YX_FRIEND_CONFIRM_OK)
    {
        
    }
}

PUBLIC BOOLEAN ZdtApp_YX_Friend_DrawSearchRcv(YX_APP_T *pMe, uint8 index, uint8* pContent, uint16 Content_len)
{
    ZDT_LOG("ZdtApp_YX_Friend_DrawSearchRcv content:%s",pContent);

    return FALSE;
}

#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
///-----------insert sim  tips win start-----------
LOCAL uint8 s_friend_pp_status = 3;//0;
uint8 g_pp_friend_id[YX_DB_FRIEND_MAX_ID_SIZE+1] = {0};
uint16 g_pp_wstr_pp_name[41] = {0};
uint16 g_pp_wstr_pp_num[41] = {0};
LOCAL uint8 s_pp_disp_ui_timer_id = 0;

LOCAL void Draw_FindFriend_Shake(MMI_WIN_ID_T win_id,const GUI_LCD_DEV_INFO *dev_info_ptr)
{

}

void MMIZDT_PPDisplay_HandleSuccessTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
   if(MMK_IsOpenWin(MMIZDT_FRIEND_PP_WIN_ID))
   {
        MMIZDT_OpenChatGroupWinByPP(g_pp_friend_id);
        MMK_CloseWin(MMIZDT_FRIEND_PP_WIN_ID);
   }
}

void MMIZDT_PPDisplay_HandleFailTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
    if(MMK_IsOpenWin(MMIZDT_FRIEND_PP_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_FRIEND_PP_WIN_ID);
    }
}

PUBLIC void MMIZDT_PPDisplay_TimerStop()
{
    if(s_pp_disp_ui_timer_id != 0)
    {
        MMK_StopTimer(s_pp_disp_ui_timer_id);
    }
}

PUBLIC void MMIZDT_PPDisplay_TimerStart(uint32 time_out, MMI_TIMER_FUNC func)
{
    if(s_pp_disp_ui_timer_id != 0)
    {
        MMK_StopTimer(s_pp_disp_ui_timer_id);
    }
    s_pp_disp_ui_timer_id = MMK_CreateTimerCallback(time_out, func, 0, FALSE);
}

LOCAL MMI_RESULT_E  HandleZDT_FriendPPWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );



WINDOW_TABLE( MMIZDT_FRIEND_PP_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_FriendPPWinMsg),    
    WIN_ID( MMIZDT_FRIEND_PP_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

void HandleZDT_FriendPP_ShowTxt(uint16 * w_str1, uint16 * w_str2,uint16 * w_str3)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    uint16          uint16_str[100 + 50] = {0};
    uint16          uint16_str_len = 0;
    MMI_STRING_T    cur_str_t   = {0};
    GUI_RECT_T   cur_rect          = {0};
    GUI_RECT_T   rect        = {0}; 
    GUISTR_STYLE_T      text_style      = {0};  
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK; 
    GUI_FONT_T f_font = SONG_FONT_26;
    char *deviceVersion=NULL;

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = MMI_MAINSCREEN_WIDTH-1;
    rect.bottom = MMI_MAINSCREEN_HEIGHT-1;
    cur_rect = rect;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = f_font;
    text_style.font_color = MMI_WHITE_COLOR;
    
    if(w_str1 != NULL)
    {
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(w_str1);
        cur_str_t.wstr_ptr = w_str1;
        cur_rect.top    = 0; 
        cur_rect.bottom = 46;
        LCD_FillRect(&lcd_dev_info, cur_rect, MMI_BLACK_COLOR);

        GUISTR_DrawTextToLCDInRect( 
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            (const GUI_RECT_T      *)&cur_rect,       
            (const GUI_RECT_T      *)&cur_rect,     
            (const MMI_STRING_T    *)&cur_str_t,
            &text_style,
            state,
            GUISTR_TEXT_DIR_AUTO
            );
    }
        
    if(w_str2 != NULL)
    {
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(w_str2);
        cur_str_t.wstr_ptr = w_str2;
        cur_rect.top    = 170; 
        cur_rect.bottom = 200;
        LCD_FillRect(&lcd_dev_info, cur_rect, MMI_BLACK_COLOR);
        GUISTR_DrawTextToLCDInRect( 
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            (const GUI_RECT_T      *)&cur_rect,       
            (const GUI_RECT_T      *)&cur_rect,
            (const MMI_STRING_T    *)&cur_str_t,
            &text_style,
            state,
            GUISTR_TEXT_DIR_AUTO
            );
    }
    if(w_str3 != NULL)
    {
        cur_str_t.wstr_len = MMIAPICOM_Wstrlen(w_str3);
        cur_str_t.wstr_ptr = w_str3;
        cur_rect.top    = 201; 
        cur_rect.bottom = 239;
        LCD_FillRect(&lcd_dev_info, cur_rect, MMI_BLACK_COLOR);
        GUISTR_DrawTextToLCDInRect( 
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            (const GUI_RECT_T      *)&cur_rect,       
            (const GUI_RECT_T      *)&cur_rect,
            (const MMI_STRING_T    *)&cur_str_t,
            &text_style,
            state,
            GUISTR_TEXT_DIR_AUTO
            );
    }
    return;
}

PUBLIC BOOLEAN MMIZDT_IsFriendPPWin(void)
{
    MMI_WIN_ID_T win_id = MMK_GetFocusWinId() ;

    return win_id == MMIZDT_FRIEND_PP_WIN_ID;
}


PUBLIC BOOLEAN MMIZDT_IsFriendPPWinOpen(void)
{
    BOOLEAN ret = FALSE;
    if(MMK_IsOpenWin(MMIZDT_FRIEND_PP_WIN_ID))
    {
        ret = TRUE;
    }
    return ret;

}

void MMIZDT_FriendPPWin_TimeStop(void)
{
    if(0 != g_friendPP_timer_id)
    {
        MMK_StopTimer(g_friendPP_timer_id);
        g_friendPP_timer_id = 0;
    }
}

#ifdef ZDT_GSENSOR_SUPPORT

void MMIZDT_FriendPPWin_GSENSOR_Data(void)
{
    ZDT_GSENSOR_IC_TYPE_E gsenser_type = ZDT_GSensor_GetType();
    int16 x,y,z,tempxyz;

#ifdef WIN32
    x=300;
    y=200;
    z=300;
#else
    QQMA6981_ReadXYZ(&x,&y,&z);
#endif
    SCI_TRACE_LOW("MMIZDT_FriendPPWin_GSENSOR_Data:x=%d,y=%d,z=%d",x,y,z);
    tempxyz = abs(x -gsensor_x)+abs(y-gsensor_y)+abs(z-gsensor_z);
    SCI_TRACE_LOW("MMIZDT_FriendPPWin_GSENSOR_Data:tempxyz=%d",tempxyz);
   // if(tempxyz - xyz > 1000)
    if(tempxyz > 600)
    {
        g_friendPP_shakecount++;
    }
    else
    {
        g_friendPP_shakecount = 0;
    }
#ifndef WIN32
    gsensor_x = x;
    gsensor_y = y;
    gsensor_z = z;
#endif
    if(g_friendPP_shakecount >= 4)
    {
        gsensor_x = 0;
        gsensor_y = 0;
        gsensor_z = 0;
        s_friend_pp_status = 0;
        SCI_MEMSET(g_pp_wstr_pp_name,0,sizeof(g_pp_wstr_pp_name));
        SCI_MEMSET(g_pp_wstr_pp_num,0,sizeof(g_pp_wstr_pp_num));
        YX_Net_Friend_Start(&g_yx_app);
        ZdtTalk_BackLight(TRUE);
        MMIZDT_FriendPPWin_TimeStop();
    }
}

void MMIZDT_FriendPPWin_TimeStart(uint32 time_ms,MMI_TIMER_FUNC  func)
{
    if(0 != g_friendPP_timer_id)
    {
        MMK_StopTimer(g_friendPP_timer_id);
        g_friendPP_timer_id = 0;
    }
    
    g_friendPP_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        func, 
                                                                        (uint32)PNULL, 
                                                                        FALSE);
}



void MMIZDT_FriendPPWin_GSENSOR_HandleTimer(
                                                                        uint8 timer_id,
                                                                        uint32 param
                                                                        )
{
    if(s_friend_pp_status == 3 || s_friend_pp_status == 2)//等待摇晃
    {
        MMIZDT_FriendPPWin_GSENSOR_Data();
        MMIZDT_FriendPPWin_TimeStart(200, MMIZDT_FriendPPWin_GSENSOR_HandleTimer);
        MMK_SendMsg(MMIZDT_FRIEND_PP_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

void MMIZDT_FriendPPWin_GSENSOR_Exit(void)
{
    //ZDT_LOG("ZdtWatch_Factory_GSENSOR_Exit");
    MMIZDT_FriendPPWin_TimeStop();
    return;
}

void MMIZDT_FriendPPWin_GSENSOR_Start(void)
{
    //ZDT_LOG("ZdtWatch_Factory_GSENSOR_Entry");
    if(s_friend_pp_status == 3 || s_friend_pp_status == 2)
    {
        g_friendPP_shakecount = 0;
        //ZDT_GSensor_Open();
        MMIZDT_FriendPPWin_GSENSOR_Data();
        MMIZDT_FriendPPWin_TimeStart(200, MMIZDT_FriendPPWin_GSENSOR_HandleTimer);
    }
    return;
}

#endif

PUBLIC void MMIZDT_OpenFriendPPWin(void)
{
    if(ZDT_SIM_Exsit() == FALSE)
    {
    	 MMIAPI_Zdt_Alert_Win(STR_SIM_NOT_SIM_EXT01);
        //MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SETTINGS_ADUPS_NO_SIM,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return;
    }
    if (MMIAPICONNECTION_isGPRSSwitchedOff())
    {
    	 MMIAPI_Zdt_Alert_Win(TXT_SETTINGS_ADUPS_NO_NET);
        //MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SETTINGS_ADUPS_NO_NET,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return;
    }
    if(FALSE == YX_Net_Is_Land())
    {
        MMIZDT_Net_Reset(TRUE);
	 MMIAPI_Zdt_Alert_Win(TXT_SETTINGS_ADUPS_NO_NET);
        //MMIPUB_OpenAlertWinByTextId(PNULL,TXT_SETTINGS_ADUPS_NO_NET,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
        return;
    }

    if(FALSE == MMK_IsOpenWin(MMIZDT_FRIEND_PP_WIN_ID))
   {
        MMK_CreateWin((uint32*)MMIZDT_FRIEND_PP_WIN_TAB,PNULL);
   }
}

PUBLIC void MMIZDT_CloseFriendPPWin()
{
    if(MMK_IsOpenWin(MMIZDT_FRIEND_PP_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_FRIEND_PP_WIN_ID);
    }
}

PUBLIC BOOLEAN MMIZDT_UpdateFriendPPWinOpen(BOOLEAN is_ok)
{
    BOOLEAN ret;
    if(is_ok)
    {
        s_friend_pp_status = 1;
    }
    else
    {
        s_friend_pp_status = 2;
    }
    if(MMK_IsFocusWin(MMIZDT_FRIEND_PP_WIN_ID))
    {
        MMK_SendMsg(MMIZDT_FRIEND_PP_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
    return ret;
}

LOCAL MMI_RESULT_E  HandleZDT_FriendPPWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T 		group_id		=	0;
    MMI_MENU_ID_T				menu_id 		=	0;
    MMI_RESULT_E				recode			=	MMI_RESULT_TRUE;
    GUI_RECT_T		bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T		str_data = {0};
    GUI_RECT_T	text_display_rect={0};
    GUISTR_STYLE_T		text_style = {0};
    GUISTR_STATE_T		state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    uint16 wstr_search_1[10] = {0x641C,0x7D22,0x4E2D,0x002E,0x002E,0x002E,0x0};//搜索中...
    uint16 wstr_search_2[20] = {0x4E24,0x4E2A,0x624B,0x8868,0x540C,0x65F6,0x0}; //两个手表同时
    uint16 wstr_search_3[30] = {0x8FDB,0x5165,0x4EA4,0x53CB,0x754C,0x9762,0x0 };//进入交友界面
    uint16 wstr_pp_ok[30] = {0x4EA4,0x53CB,0x6210,0x529F,0x0 };//交友成功
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
#ifdef ZDT_GSENSOR_SUPPORT
                s_friend_pp_status = 3;
                g_friendPP_shakecount = 0;
               /* s_friend_pp_status = 0;
                SCI_MEMSET(g_pp_wstr_pp_name,0,sizeof(g_pp_wstr_pp_name));
                SCI_MEMSET(g_pp_wstr_pp_num,0,sizeof(g_pp_wstr_pp_num));
                YX_Net_Friend_Start(&g_yx_app);
                ZdtTalk_BackLight(TRUE);*/
                ZdtTalk_BackLight(FALSE);
                gsensor_x =0;
                gsensor_y = 0;
                gsensor_z = 0;
#ifdef WIN32
                gsensor_x=8;
                gsensor_y=6;
                gsensor_z=7;
#else
                ZDT_GSensor_Open();
                QQMA6981_ReadXYZ(&gsensor_x,&gsensor_y,&gsensor_z);
                SCI_TRACE_LOW("HandleZDT_FriendPPWinMsg:gsensor_x=%d,gsensor_y=%d,gsensor_z=%d",gsensor_x,gsensor_y,gsensor_z);
#endif
                MMIZDT_FriendPPWin_TimeStart(200, MMIZDT_FriendPPWin_GSENSOR_HandleTimer);
                //MMIZDT_FriendPPWin_GSENSOR_Start();
#else
            s_friend_pp_status = 0;
            ZdtTalk_BackLight(FALSE);
#endif

            break;
            
        case MSG_GET_FOCUS:
            break;
            
        case MSG_FULL_PAINT:
        {
            GUI_FillRect(&lcd_dev_info, bg_rect, MMI_BLACK_COLOR);
            if(s_friend_pp_status == 0)
            {
                //搜索中
            #ifdef ZTE_WATCH
                GUI_POINT_T point = {FIND_FRIEND_SHAKE_FINDING_IMG_X, FIND_FRIEND_SHAKE_FINDING_IMG_Y};
                GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,friend_finding,&lcd_dev_info);              
                HandleZDT_FriendPP_ShowTxt(wstr_search_1,NULL,NULL);
            #else
            	GUIRES_DisplayImg(PNULL,&bg_rect,PNULL,win_id,IMAGE_PP_BG,&lcd_dev_info);              
                HandleZDT_FriendPP_ShowTxt(wstr_search_1,wstr_search_2,wstr_search_3);
            #endif
                ZdtTalk_BackLight(TRUE);
            }
            else if(s_friend_pp_status == 1)
            {
                //交友成功
            	GUIRES_DisplayImg(PNULL,&bg_rect,PNULL,win_id,IMAGE_PP_SUCCESS,&lcd_dev_info);
                HandleZDT_FriendPP_ShowTxt(wstr_pp_ok,g_pp_wstr_pp_name,g_pp_wstr_pp_num);
                ZdtTalk_BackLight(FALSE);
                MMIZDT_PPDisplay_TimerStart(3000,MMIZDT_PPDisplay_HandleSuccessTimer);
            }
            else if(s_friend_pp_status == 2)
            {
                //交友失败
                GUIRES_DisplayImg(PNULL,&bg_rect,PNULL,win_id,IMAGE_PP_FAIL,&lcd_dev_info);
                ZdtTalk_BackLight(FALSE);
                MMIZDT_PPDisplay_TimerStart(5000,MMIZDT_PPDisplay_HandleFailTimer);
            }
            else  if(s_friend_pp_status == 3)
            {
                GUI_POINT_T point = {FIND_FRIEND_SHAKE_IMG_X, FIND_FRIEND_SHAKE_IMG_Y};
                GUI_RECT_T         	disp_rect   		= {0,0,MMI_MAINSCREEN_WIDTH,80};
                 //摇一摇
                GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,shake_icon,&lcd_dev_info);
                ZdtTalk_BackLight(FALSE);
                MMI_GetLabelTextByLang(TXT_SHAKE_FRIEND,&str_data);
                text_style.align = ALIGN_HVMIDDLE;
                text_style.font = SONG_FONT_22;
                text_style.font_color = MMI_WHITE_COLOR;
                GUISTR_DrawTextToLCDInRect(
                &lcd_dev_info,
                (const GUI_RECT_T *)&disp_rect,       //the fixed display area
                (const GUI_RECT_T *)&disp_rect,       
                (const MMI_STRING_T *)&str_data,
                &text_style,
                GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE,
                GUISTR_TEXT_DIR_AUTO);
            }
            else
            {
            #ifdef ZTE_WATCH
                GUI_POINT_T point = {FIND_FRIEND_SHAKE_FINDING_IMG_X, FIND_FRIEND_SHAKE_FINDING_IMG_Y};
                GUIRES_DisplayImg(&point,PNULL,PNULL,win_id,friend_finding,&lcd_dev_info);              
                HandleZDT_FriendPP_ShowTxt(wstr_search_1,NULL,NULL);
            #else
                GUIRES_DisplayImg(PNULL,&bg_rect,PNULL,win_id,IMAGE_PP_BG,&lcd_dev_info);
            #endif
                ZdtTalk_BackLight(FALSE);
                MMIZDT_PPDisplay_TimerStart(5000,MMIZDT_PPDisplay_HandleFailTimer);

            }
        }
        break;
		
#if 1//def TOUCH_PANEL_SUPPORT
	case MSG_TP_PRESS_DOWN:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			
		}
		break;
		
	case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T   point = {0};
			int offset_y =	0;//up to down
			int offset_x =	0;//up to down
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
            if(s_friend_pp_status == 2 && (point.x > 5 && point.x < 235)&&(point.y >= 160 &&  point.y < 235))
            {
                /* s_friend_pp_status = 0;
                YX_Net_Friend_Start(&g_yx_app);
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);*/
                s_friend_pp_status = 3;
#ifdef ZDT_GSENSOR_SUPPORT
                MMIZDT_FriendPPWin_GSENSOR_Start();
#endif
                MMIZDT_PPDisplay_TimerStop();
                ZdtTalk_BackLight(FALSE);
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
		}
		break;
		
	case MSG_TP_PRESS_MOVE:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
	case MSG_TP_PRESS_LONG:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
		}
		break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

	case MSG_CTL_MIDSK:
		break;

	case MSG_BACKLIGHT_TURN_ON:
	case MSG_BACKLIGHT_TURN_OFF:

		break;
	case MSG_LOSE_FOCUS:
	case MSG_CLOSE_WINDOW:
#ifdef ZDT_GSENSOR_SUPPORT
	    MMIZDT_FriendPPWin_GSENSOR_Exit();
#endif
        MMIZDT_PPDisplay_TimerStop();
        ZdtTalk_BackLight(FALSE);
    	break;


	case MSG_CTL_CANCEL:
	case MSG_APP_CANCEL:
		MMK_CloseWin(win_id);
		break;

	default:
		recode = MMI_RESULT_FALSE;
		break;
}
    return recode;

}

///---------FriendPP win end----------
#endif

#endif

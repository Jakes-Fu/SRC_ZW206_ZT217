
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
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "mmi_common.h"

#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "mmi_resource.h"
#include "mmiset_id.h"
#include "version.h"
#include "mmicc_internal.h"
#include "mmisrvmgr.h"
#include "mmisrvrecord_export.h"
#include "mmirecord_export.h"
#include "mmiphone_export.h"
#include "zmt_listening_export.h"
#include "zmt_listening_id.h"
#include "zmt_listening_image.h"
#include "zmt_listening_text.h"
#include "zmt_listening_nv.h"

extern LISTENING_ALBUM_INFO * album_info;
extern uint8 listening_downloading_index;
uint32 listening_play_times = 0;
uint8 listening_play_timer_id = 0;
MMISRV_HANDLE_T listening_player_handle=PNULL;
char * listening_player_lrc_buf = NULL;

uint8 player_bottom_show_times = 0;
LISTENING_PALYER_PLAY_INFO player_play_info = {0};

extern GUI_RECT_T listen_win_rect;
extern GUI_RECT_T listen_title_rect;
LOCAL GUI_RECT_T listening_pre_rect = {LISTEN_LINE_HIGHT, 1.8*LISTEN_LINE_HIGHT, 2*LISTEN_LINE_WIDTH, 3.2*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_play_rect = {2*LISTEN_LINE_WIDTH, LISTEN_LINE_HIGHT, 4*LISTEN_LINE_WIDTH, 4*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_next_rect = {4*LISTEN_LINE_WIDTH ,1.8*LISTEN_LINE_HIGHT, 5*LISTEN_LINE_WIDTH, 3.2*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_play_time_rect = {0, 4*LISTEN_LINE_HIGHT, LISTEN_LINE_WIDTH+5, 5*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_dis_play_rect = {LISTEN_LINE_WIDTH+5, 4*LISTEN_LINE_HIGHT, 5*LISTEN_LINE_WIDTH-5, 5*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_dis_total_rect = {LISTEN_LINE_WIDTH+5, 4*LISTEN_LINE_HIGHT, 5*LISTEN_LINE_WIDTH-5, 5*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_total_rect = {5*LISTEN_LINE_WIDTH-5, 4*LISTEN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 5*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_lrc_rect = {2*LISTEN_LINE_WIDTH, 6*LISTEN_LINE_HIGHT, 4*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_empty_bottom = {0, 6*LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T listening_play_style_rect = {LISTEN_LINE_WIDTH, 6*LISTEN_LINE_HIGHT+5, 2*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT+5};
LOCAL GUI_RECT_T listening_rect_style_loop = {0.8*LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 1.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_style_random = {2.8*LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 3.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_style_single = {4.8*LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 5.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_volume_rect = {4.5*LISTEN_LINE_WIDTH, 6*LISTEN_LINE_HIGHT+5, 6*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT+5};
LOCAL GUI_RECT_T listening_rect_empty = {0, 7*LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T listening_rect_decrese = {10, 8*LISTEN_LINE_HIGHT+5, 2*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT+5};
LOCAL GUI_RECT_T listening_rect_volume = {LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 1.5*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_increse = {5*LISTEN_LINE_WIDTH+5, 8*LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH, 9*LISTEN_LINE_HIGHT+5};

LOCAL void ListeningPlayer_InitPlayerInfo(void);
LOCAL void ListeningPlayer_ButtonPreCallback(void);
LOCAL void ListeningPlayer_ButtonNextCallback(void);
LOCAL void ListeningPlayerTimerCallback(uint8 timer_id, uint32 * param);

LOCAL void Listening_PlayMp3Notify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = PNULL;
	BOOLEAN result = TRUE;
	if(param != PNULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		if(report_ptr != PNULL && handle == listening_player_handle)
		{
			SCI_TRACE_LOW("%s: report_ptr->report = %d", __FUNCTION__, report_ptr->report);
			switch(report_ptr->report)
			{
				case MMISRVAUD_REPORT_END:
				{ 
                    if (MMISRVAUD_REPORT_RESULT_STOP != report_ptr->data1)
                    {
                        if (MMISRVAUD_REPORT_RESULT_SUCESS != report_ptr->data1)
                        {
                            result = FALSE;
                        }
                    }
					if(result)
					{
						Listening_StopPlayMp3();
						ListeningPlayer_ButtonNextCallback();
					}
            	}
				break;
               	default:
				break;
            }
        }
    }
	SCI_TRACE_LOW("%s: result = %d", __FUNCTION__, result);
}

LOCAL BOOLEAN Listening_Play_RequestHandle( 
                        MMISRV_HANDLE_T *audio_handle,
                        MMISRVAUD_ROUTE_T route,
                        MMISRVAUD_TYPE_U *audio_data,
                        MMISRVMGR_NOTIFY_FUNC notify
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
    audio_srv.volume = 1;//MMIAPISET_GetMultimVolume();

	audio_srv.info.record_file.type = audio_data->type;        
	audio_srv.info.record_file.fmt  = audio_data->record_file.fmt;
	audio_srv.info.record_file.name = audio_data->record_file.name;
	audio_srv.info.record_file.name_len = audio_data->record_file.name_len;    
	audio_srv.info.record_file.source   = audio_data->record_file.source;
	audio_srv.info.record_file.frame_len= audio_data->record_file.frame_len;
	audio_srv.volume = AUD_MAX_SPEAKER_VOLUME;

    *audio_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);

    if(*audio_handle > 0)
    {
       	return TRUE;
    }
    else
    {
       	return FALSE;
    }
}

PUBLIC void Listening_StopPlayMp3(void)
{
	SCI_TRACE_LOW("%s: handle = %d", __FUNCTION__, listening_player_handle);
	if (0 != listening_player_handle)
	{
		MMISRVAUD_Stop(listening_player_handle);
		MMISRVMGR_Free(listening_player_handle);
		listening_player_handle = PNULL;
	}
	listening_play_times = 0;
}

PUBLIC void Listening_PausePlayMp3(void)
{
	if (0 != listening_player_handle)
	{
		MMISRVAUD_Pause(listening_player_handle);
		
	}
	if(0 != listening_play_timer_id)
	{
		MMK_StopTimer(listening_play_timer_id);
		listening_play_timer_id = 0;
	}
}

PUBLIC void Listening_ResumePlayMp3(void)
{
	if (0 != listening_player_handle)
	{
		MMISRVAUD_Resume(listening_player_handle);
		
	}
	if(0 != listening_play_timer_id)
	{
		MMK_StopTimer(listening_play_timer_id);
		listening_play_timer_id = 0;
	}
	listening_play_timer_id = MMK_CreateTimerCallback(1000, ListeningPlayerTimerCallback, PNULL, TRUE);
}

PUBLIC uint8 Listening_StartPlayMp3(char* file_name)
{
    uint8 ret = 0;
   	MMIRECORD_SRV_RESULT_E srv_result = MMIRECORD_SRV_RESULT_SUCCESS;
   	MMISRV_HANDLE_T audio_handle = PNULL;
   	uint16      full_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
   	uint16      full_path_len = 0;
    MMISRVAUD_TYPE_U    audio_data  = {0};

    if(listening_player_handle)
    {
        MMISRVAUD_Stop(listening_player_handle);
       	MMISRVMGR_Free(listening_player_handle);
       	listening_player_handle = PNULL;
   	}

    full_path_len = GUI_GBToWstr(full_path, (const uint8*)file_name, SCI_STRLEN(file_name));
    audio_data.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
    audio_data.ring_file.name = full_path;
    audio_data.ring_file.name_len = full_path_len;
    audio_data.ring_file.fmt  = (MMISRVAUD_RING_FMT_E)MMIAPICOM_GetMusicType(audio_data.ring_file.name, audio_data.ring_file.name_len);

   	if(Listening_Play_RequestHandle(&audio_handle, MMISRVAUD_ROUTE_NONE, &audio_data, Listening_PlayMp3Notify))
    {
    	ListeningPlayer_InitPlayerInfo();
        listening_player_handle = audio_handle;
		MMISRVAUD_SetVolume(audio_handle, player_play_info.volume);
        if(!MMISRVAUD_Play(audio_handle, 0))
        {     
            SCI_TRACE_LOW("%s: MMISRVAUD_Play failed", __FUNCTION__);
            MMISRVMGR_Free(listening_player_handle);
            listening_player_handle = 0;
           	ret = 1;
       }
    }
    else
    {        
       	ret = 2;
    }
	SCI_TRACE_LOW("%s: ret = %d", __FUNCTION__, ret);
    if (ret != 0)
    {
		listening_player_handle = NULL;
        return ret;
    }
    return ret;
}

PUBLIC BOOLEAN Listening_PlayMp3(int module_id, int album_id, int audio_id)
{
	char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
	Listening_GetFileName(file_path, module_id, album_id, audio_id);
	if(zmt_file_exist(file_path))
	{
		if(player_play_info.play_status == 1)
		{
			player_play_info.play_status = 0;
			Listening_ResumePlayMp3();
		}
		Listening_StopPlayMp3();
		if(Listening_StartPlayMp3(file_path) == 0)
		{
			if(listening_play_timer_id == 0)
			{
				listening_play_timer_id = MMK_CreateTimerCallback(1000, ListeningPlayerTimerCallback, PNULL, TRUE);
			}
			return TRUE;
		}
	}
	return FALSE;
}


LOCAL void ListeningPlayerWin_DisplayPlayerButton(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
	MMI_CTRL_ID_T pre_id = LISTENING_BUTTON_PREV_CTRL_ID;
	MMI_CTRL_ID_T play_id = LISTENING_BUTTON_PLAY_CTRL_ID;
	MMI_CTRL_ID_T next_id = LISTENING_BUTTON_NEXT_CTRL_ID;
	GUI_BG_T play_bg = {0};

	GUIBUTTON_SetRect(pre_id, &listening_pre_rect);
	GUIBUTTON_SetRect(play_id, &listening_play_rect);
	GUIBUTTON_SetRect(next_id, &listening_next_rect);
	play_bg.bg_type = GUI_BG_IMG;
	if(player_play_info.play_status == 0)
	{
		play_bg.img_id = ZMT_LISTEN_PAUSE;
	}
	else if(player_play_info.play_status == 1)
	{
		play_bg.img_id = ZMT_LISTEN_PLAY;
	}
	else
	{
		play_bg.img_id = ZMT_LISTEN_DOWNLOADING;
	}
	GUIBUTTON_SetBg(play_id, &play_bg);
}

LOCAL void ListeningPlayerWin_DisplayPlayDuration(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info, LISTENING_PLAYER_INFO * player_info)
{
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_string = {0};
	wchar time_wchar[10] = {0};
	char time_str[10] = {0};
	uint32 duration = 0;
	uint8 min = 0;
	uint8 second = 0;
	MMISRVAUD_PLAY_INFO_T playing_info = { 0 };
	
	if(MMISRVAUD_GetPlayingInfo(listening_player_handle, &playing_info) == TRUE)
	{
		duration = playing_info.total_time;
	}
	else
	{
		duration = 0;
	}

	SCI_TRACE_LOW("%s: duration = %d", __FUNCTION__, duration);
	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_16;
	text_style.font_color = MMI_BLACK_COLOR;

	min = listening_play_times / 60;
	second = listening_play_times % 60;
	memset(time_str, 0, 10);
	memset(time_wchar, 0, 10);

	sprintf(time_str, "%d:%02d", min, second);
	MMIAPICOM_StrToWstr(time_str, time_wchar);
	text_string.wstr_ptr = time_wchar;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&listening_play_time_rect,
		&listening_play_time_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);

	LCD_DrawRect(&lcd_dev_info, listening_dis_total_rect, MMI_BLACK_COLOR);
	listening_dis_play_rect.right = 38 + ((float)((float)listening_play_times / (float)duration) * 150);
	if(listening_dis_play_rect.right > 90)
	{
		listening_dis_play_rect.right = 90;
	}
	LCD_FillRect(&lcd_dev_info, listening_dis_play_rect, MMI_BLACK_COLOR);

	min = duration / 60;
	second = duration % 60;
	memset(time_str, 0, 10);
	memset(time_wchar, 0, 10);
	sprintf(time_str, "%d:%02d", min, second);
	MMIAPICOM_StrToWstr(time_str, time_wchar);
	text_string.wstr_ptr = time_wchar;
	text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&listening_total_rect,
		&listening_total_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);
}

LOCAL void ListeningPlayerWin_DisplayPlayChoose(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_string = {0};
	
	GUI_FillRect(&lcd_dev_info, listening_rect_empty_bottom, MMI_WHITE_COLOR);

	if(player_play_info.play_style == PALYER_PLAY_STYLE_LOOP)
	{
		GUIRES_DisplayImg(PNULL, &listening_play_style_rect, PNULL, win_id, ZMT_LISTEN_LOOP, &lcd_dev_info);
	}
	else if(player_play_info.play_style == PALYER_PLAY_STYLE_RANDOM)
	{
		GUIRES_DisplayImg(PNULL, &listening_play_style_rect, PNULL, win_id, ZMT_LISTEN_RANDOM, &lcd_dev_info);
	}
	else if(player_play_info.play_style == PALYER_PLAY_STYLE_SINGLE)
	{
		GUIRES_DisplayImg(PNULL, &listening_play_style_rect, PNULL, win_id, ZMT_LISTEN_SINGLE, &lcd_dev_info);
	}
	GUIRES_DisplayImg(PNULL, &listening_volume_rect, PNULL, win_id, ZMT_LISTEN_VOLUME, &lcd_dev_info);

	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_16;
	text_style.font_color = MMI_BLACK_COLOR;
	MMIRES_GetText(ZMT_TXT_LYRICS, win_id, &text_string);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&listening_lrc_rect,
		&listening_lrc_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);
	LCD_DrawRect(&lcd_dev_info, listening_lrc_rect, MMI_BLACK_COLOR);
}

LOCAL void ListeningPlayerWin_DisplayPlayBottom(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
	GUI_FillRect(&lcd_dev_info, listening_rect_empty, MMI_WHITE_COLOR);
	if(player_play_info.bottom_type == 1)
	{
		uint8 i = 0;
		GUI_RECT_T rect_style = {0.8*LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 1.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
		GUI_RECT_T rect_text = {0.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT, 1.8*LISTEN_LINE_WIDTH, 10*LISTEN_LINE_HIGHT};
		GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
		GUISTR_STYLE_T text_style = {0};
		MMI_STRING_T text_string = {0};
		
		MMI_IMAGE_ID_T img_id[3] = {ZMT_LISTEN_LOOP, ZMT_LISTEN_RANDOM, ZMT_LISTEN_SINGLE};
		MMI_TEXT_ID_T text_id[3] = {ZMT_TXT_LOOP_PLAY, ZMT_TXT_RANDOM_PLAY, ZMT_TXT_SINGLE_PLAY};
		
		text_style.align = ALIGN_LVMIDDLE;
		text_style.font = SONG_FONT_16;
		text_style.font_color = MMI_BLACK_COLOR;
		for(; i < 3; i++)
		{
			GUIRES_DisplayImg(PNULL, &rect_style, PNULL, win_id, img_id[i], &lcd_dev_info);
			rect_style.left += 2*LISTEN_LINE_WIDTH;
			rect_style.right = rect_style.left + LISTEN_LINE_WIDTH;

			MMIRES_GetText(text_id[i], win_id, &text_string);
			GUISTR_DrawTextToLCDInRect(
				(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
				&rect_text,
				&rect_text,
				&text_string,
				&text_style,
				text_state,
				GUISTR_TEXT_DIR_AUTO
				);
			rect_text.left += 2*LISTEN_LINE_WIDTH-5;
			rect_text.right = rect_style.left + LISTEN_LINE_WIDTH;
		}
	}
	else if(player_play_info.bottom_type == 2)
	{
		uint count = 0;
		uint8 i = 0;
		uint8 volume_item = 0;
		GUI_RECT_T rect_volume = {0};
		GUIRES_DisplayImg(PNULL, &listening_rect_decrese, PNULL, win_id, ZMT_LISTEN_DECRESE, &lcd_dev_info);
		GUIRES_DisplayImg(PNULL, &listening_rect_increse, PNULL, win_id, ZMT_LISTEN_INCRESE, &lcd_dev_info);
		count = player_play_info.volume / 2 + player_play_info.volume % 2;
		SCI_TRACE_LOW("%s: listening_player_volume = %d, count = %d", __FUNCTION__, player_play_info.volume, count);
		rect_volume = listening_rect_volume;
		for(i = 0; i < count && i < 5; i++)
		{
			LCD_FillRect(&lcd_dev_info, rect_volume, MMI_BLACK_COLOR);
			rect_volume.left = rect_volume.right+ 13;
			rect_volume.right = rect_volume.left + 0.5*LISTEN_LINE_WIDTH;
		}
	}
}

LOCAL void ListeningPlayer_DecreseVolume(void)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	LISTEING_PLAYER_INFO player_info = {0};
	if(player_play_info.volume >= 2)
	{
		MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
		player_play_info.volume -= 2;
		if(player_play_info.play_status != 0)
		{
			ListeningPlayerWin_DisplayPlayBottom(LISTENING_PLAYER_WIN_ID, lcd_dev_info);
		}
		if(0 != listening_player_handle)
		{
			MMISRVAUD_SetVolume(listening_player_handle, player_play_info.volume);
		}

		MMIAPISET_SetMultimVolume(player_play_info.volume);
     		MMIAPISET_SetCallVolume(player_play_info.volume);
		
		player_info.volume = player_play_info.volume;
		MMI_WriteNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
	}
}

LOCAL void ListeningPlayer_IncreseVolume(void)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	LISTEING_PLAYER_INFO player_info = {0};
	if(player_play_info.volume <= 8)
	{
		MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
		player_play_info.volume += 2;
		if(player_play_info.play_status != 0)
		{
			ListeningPlayerWin_DisplayPlayBottom(LISTENING_PLAYER_WIN_ID, lcd_dev_info);
		}
		if(0 != listening_player_handle)
		{
			MMISRVAUD_SetVolume(listening_player_handle, player_play_info.volume);
		}
		
		MMIAPISET_SetMultimVolume(player_play_info.volume);
     		MMIAPISET_SetCallVolume(player_play_info.volume);
			
		player_info.volume = player_play_info.volume;
		MMI_WriteNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
	}
}

LOCAL void ListeningPlayer_ButtonPlayCallback(void)
{
	if(player_play_info.play_status == 0)
	{
		player_play_info.play_status = 1;
		Listening_PausePlayMp3();
	}
	else if(player_play_info.play_status == 1)
	{
		player_play_info.play_status = 0;
		if(listening_player_handle != NULL)
		{
			Listening_ResumePlayMp3();
		}
		else
		{
			int module_id = 0;
			int album_id = 0;
			int audio_id = 0;
			LISTENING_PLAYER_INFO * player_info = NULL;
			LISTEING_LOCAL_INFO * local_info = NULL;
			player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(LISTENING_PLAYER_WIN_ID);
			if(player_info->is_local_play)
			{
				local_info = Listening_GetLocalDataInfo();
				module_id = local_info->module_info[player_info->moudle_index].module_id;
				album_id = local_info->module_info[player_info->moudle_index].album_info[0].album_id;
				audio_id = local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id;
			}
			else
			{
				module_id = album_info->module_id;
				album_id = album_info->album_id;
				audio_id = album_info->item_info[player_info->audio_index].audio_id;
			}
			Listening_PlayMp3(module_id, album_id, audio_id);
		}
	}
	else
	{
		return;
	}
	MMK_SendMsg(LISTENING_PLAYER_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void ListeningPlayer_ButtonPreCallback(void)
{
	LISTENING_PLAYER_INFO * player_info = NULL;
	LISTENING_PLAYER_INFO * player_infos = NULL;
	LISTEING_LOCAL_INFO * local_info = NULL;
	int module_id = 0;
	int album_id = 0;
	int audio_id = 0;
	
	player_infos = (LISTENING_PLAYER_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
	player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(LISTENING_PLAYER_WIN_ID);

	SCI_TRACE_LOW("%s: player_info->is_local_play = %d", __FUNCTION__, player_info->is_local_play);
	if(player_info->is_local_play)
	{
		local_info = Listening_GetLocalDataInfo();
		SCI_TRACE_LOW("%s: player_info->audio_index = %d", __FUNCTION__, player_info->audio_index);
		if(player_info->audio_index == 0) 
		{
			player_info->audio_index = local_info->module_info[player_info->moudle_index].album_info[0].audio_count - 1;
		}
		else
		{
			player_info->audio_index--;
		}

		module_id = local_info->module_info[player_info->moudle_index].module_id;
		album_id = local_info->module_info[player_info->moudle_index].album_info[0].album_id;
		audio_id = local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id;
		
		player_infos->is_local_play = player_info->is_local_play;
		player_infos->moudle_index = player_info->moudle_index;
		player_infos->audio_index = player_info->audio_index;
		SCI_TRACE_LOW("%s: audio_index = %d, module_id = %d, album_id = %d, audio_id = %d",
				__FUNCTION__, player_info->audio_index, module_id, album_id, audio_id);
		if(Listening_PlayMp3(module_id, album_id, audio_id))
		{
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
			MMK_PostMsg(LISTENING_PLAYER_WIN_ID, MSG_FULL_PAINT, player_infos, sizeof(LISTENING_PLAYER_INFO));
		}
		else
		{
			ListeningPlayer_ButtonPreCallback();
			SCI_FREE(player_infos);
		}
	}
	else
	{
		if(player_info->audio_index == 0) 
		{
			player_play_info.play_status = 1;
			if(0 != listening_play_timer_id)
			{
				MMK_StopTimer(listening_play_timer_id);
				listening_play_timer_id = 0;
			}
			Listening_StopPlayMp3();
			MMI_CreateListeningTipWin(PALYER_PLAY_NO_PRE_TIP);
			SCI_FREE(player_infos);
			return;
		}
		player_info->audio_index--;
		listening_downloading_index--;

		module_id = album_info->module_id;
		album_id = album_info->album_id;
		audio_id = album_info->item_info[player_info->audio_index].audio_id;
		
		player_infos->is_local_play = player_info->is_local_play;
		player_infos->moudle_index = player_info->moudle_index;
		player_infos->audio_index = player_info->audio_index;
		SCI_TRACE_LOW("%s: audio_index = %d, module_id = %d, album_id = %d, audio_id = %d",
				__FUNCTION__, player_info->audio_index, module_id, album_id, audio_id);
		if(Listening_PlayMp3(module_id, album_id, audio_id))
		{
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
			MMK_PostMsg(LISTENING_PLAYER_WIN_ID, MSG_FULL_PAINT, player_infos, sizeof(LISTENING_PLAYER_INFO));
		}
		else
		{
			player_info->audio_index++;
			listening_downloading_index++;
			player_play_info.play_status = 1;
			if(0 != listening_play_timer_id)
			{
				MMK_StopTimer(listening_play_timer_id);
				listening_play_timer_id = 0;
			}
			Listening_StopPlayMp3();
			MMI_CreateListeningTipWin(PALYER_PLAY_NO_PRE_TIP);
			SCI_FREE(player_infos);
		}
	}
}

LOCAL void ListeningPlayer_ButtonNextCallback(void)
{
	LISTENING_PLAYER_INFO * player_info = NULL;
	LISTENING_PLAYER_INFO * player_infos = NULL;
	LISTEING_LOCAL_INFO * local_info = NULL;
	int module_id = 0;
	int album_id = 0;
	int audio_id = 0;
	
	player_infos = (LISTENING_PLAYER_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
	player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(LISTENING_PLAYER_WIN_ID);

	SCI_TRACE_LOW("%s: player_info->is_local_play = %d", __FUNCTION__, player_info->is_local_play);
	if(player_info->is_local_play)
	{
		local_info = Listening_GetLocalDataInfo();
		SCI_TRACE_LOW("%s: player_info->audio_index = %d", __FUNCTION__, player_info->audio_index);
		SCI_TRACE_LOW("%s: audio_count = %d", __FUNCTION__, local_info->module_info[player_info->moudle_index].album_info[0].audio_count);
		if(player_play_info.play_style == PALYER_PLAY_STYLE_LOOP)
		{
			if(player_info->audio_index + 1 >= local_info->module_info[player_info->moudle_index].album_info[0].audio_count)
			{
				player_info->audio_index = 0;
			}
			else
			{
				player_info->audio_index++;
			}
		}
		else if(player_play_info.play_style == PALYER_PLAY_STYLE_RANDOM)
		{
			uint8 random = abs(rand());
			player_info->audio_index = random % local_info->module_info[player_info->moudle_index].album_info[0].audio_count;
			listening_downloading_index = player_info->audio_index;
		}
		else
		{
			//single no to do anything
		}
		
		module_id = local_info->module_info[player_info->moudle_index].module_id;
		album_id = local_info->module_info[player_info->moudle_index].album_info[0].album_id;
		audio_id = local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id;
		
		player_infos->is_local_play = player_info->is_local_play;
		player_infos->moudle_index = player_info->moudle_index;
		player_infos->audio_index = player_info->audio_index;
		SCI_TRACE_LOW("%s: audio_index = %d, module_id = %d, album_id = %d, audio_id = %d",
				__FUNCTION__, player_info->audio_index, module_id, album_id, audio_id);
		if(Listening_PlayMp3(module_id, album_id, audio_id))
		{
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
			MMK_PostMsg(LISTENING_PLAYER_WIN_ID, MSG_FULL_PAINT, player_infos, sizeof(LISTENING_PLAYER_INFO));
		}else
		{
			ListeningPlayer_ButtonNextCallback();
			SCI_FREE(player_infos);
		}
	}
	else
	{
		if(player_play_info.play_style == PALYER_PLAY_STYLE_LOOP)
		{
			player_info->audio_index++;
			listening_downloading_index++;
		}
		else if(player_play_info.play_style == PALYER_PLAY_STYLE_RANDOM)
		{
			uint8 random = abs(rand());
			player_info->audio_index = random % 50;
			listening_downloading_index = player_info->audio_index;
		}
		else
		{
			//single no to do anything
		}
		module_id = album_info->module_id;
		album_id = album_info->album_id;
		audio_id = album_info->item_info[player_info->audio_index].audio_id;
		
		player_infos->is_local_play = player_info->is_local_play;
		player_infos->moudle_index = player_info->moudle_index;
		player_infos->audio_index = player_info->audio_index;
		if(Listening_PlayMp3(module_id, album_id, audio_id))
		{
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
			MMK_PostMsg(LISTENING_PLAYER_WIN_ID, MSG_FULL_PAINT, player_infos, sizeof(LISTENING_PLAYER_INFO));
		}else
		{
			if(player_play_info.play_style == PALYER_PLAY_STYLE_LOOP)
			{
				player_info->audio_index--;
				listening_downloading_index--;
				player_play_info.play_status = 1;
				if(0 != listening_play_timer_id)
				{
					MMK_StopTimer(listening_play_timer_id);
					listening_play_timer_id = 0;
				}
				Listening_StopPlayMp3();
				MMI_CreateListeningTipWin(PALYER_PLAY_NO_NEXT_TIP);
			}
			else
			{
				ListeningPlayer_ButtonNextCallback();
			}
			SCI_FREE(player_infos);
		}
	}
}

LOCAL void ListeningPlayerWin_HandleTpPressUp(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	SCI_TRACE_LOW("%s: player_play_info.bottom_type = %d", __FUNCTION__, player_play_info.bottom_type);
	//bottom type
	if(GUI_PointIsInRect(point, listening_play_style_rect))
	{
		if(player_play_info.bottom_type == 1)
		{
			player_play_info.bottom_type = 0;
			player_bottom_show_times = 0;
		}else
		{
			player_play_info.bottom_type = 1;
			player_bottom_show_times = 9;
		}
	}
	else if(GUI_PointIsInRect(point, listening_volume_rect))
	{
		if(player_play_info.bottom_type == 2)
		{
			player_play_info.bottom_type = 0;
			player_bottom_show_times = 0;
		}else
		{
			player_play_info.bottom_type = 2;
			player_bottom_show_times = 9;
		}
	}
	//play style
	else if(GUI_PointIsInRect(point, listening_rect_style_loop))
	{
		LISTEING_PLAYER_INFO player_info = {0};
		if(player_play_info.bottom_type == 1)
		{
			MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
			player_play_info.play_style = PALYER_PLAY_STYLE_LOOP;
			player_play_info.bottom_type = 0;
			player_info.style = player_play_info.play_style;
			MMI_WriteNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
		}
	}
	else if(GUI_PointIsInRect(point, listening_rect_style_random))
	{
		LISTEING_PLAYER_INFO player_info = {0};
		if(player_play_info.bottom_type == 1)
		{
			MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
			player_play_info.play_style = PALYER_PLAY_STYLE_RANDOM;
			player_play_info.bottom_type = 0;
			player_info.style = player_play_info.play_style;
			MMI_WriteNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
		}
	}
	else if(GUI_PointIsInRect(point, listening_rect_style_single))
	{
		LISTEING_PLAYER_INFO player_info = {0};
		if(player_play_info.bottom_type == 1)
		{
			MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
			player_play_info.play_style = PALYER_PLAY_STYLE_SINGLE;
			player_play_info.bottom_type = 0;
			player_info.style = player_play_info.play_style;
			MMI_WriteNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
		}
	}
	//volume 
	else if(GUI_PointIsInRect(point, listening_rect_decrese))
	{
		if(player_play_info.bottom_type == 2)
		{
			ListeningPlayer_DecreseVolume();
			player_bottom_show_times = 9;
		}
	}
       else if(GUI_PointIsInRect(point, listening_rect_increse))
	{
		if(player_play_info.bottom_type == 2)
		{
			ListeningPlayer_IncreseVolume();
			player_bottom_show_times = 9;
		}
	}
	ListeningPlayerWin_DisplayPlayChoose(win_id, lcd_dev_info);
	ListeningPlayerWin_DisplayPlayBottom(win_id, lcd_dev_info);
}

LOCAL MMI_RESULT_E HandleListeningPlayerWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	GUI_POINT_T point = {0};
	uint8 id_index = 0;
	LISTENING_PLAYER_INFO * player_info = NULL;
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_NEXT_CTRL_ID, ListeningPlayer_ButtonNextCallback);
				GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PLAY_CTRL_ID, ListeningPlayer_ButtonPlayCallback);
				GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PREV_CTRL_ID, ListeningPlayer_ButtonPreCallback);
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar title_wchar[50] = {0};
				uint8 length = 0;
				
				GUI_FillRect(&lcd_dev_info, listen_win_rect, MMI_WHITE_COLOR);

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = SONG_FONT_16;
				text_style.font_color = MMI_BLACK_COLOR;
				player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(win_id);
				if(player_info->is_local_play)
				{
					LISTEING_LOCAL_INFO * local_info = NULL;
					local_info = Listening_GetLocalDataInfo();
					length = strlen(local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name);
					#ifdef WIN32
					GUI_GBToWstr(title_wchar, local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name, length);
					#else
					GUI_UTF8ToWstr(title_wchar, 50, local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name, length);
					#endif
				}else
				{
					length = strlen(album_info->item_info[player_info->audio_index].audio_name) + 1;
					GUI_UTF8ToWstr(title_wchar, 50, album_info->item_info[player_info->audio_index].audio_name, length);
				}
				text_string.wstr_ptr = title_wchar;
				text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&listen_title_rect,
					&listen_title_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				ListeningPlayerWin_DisplayPlayerButton(win_id, lcd_dev_info);
				ListeningPlayerWin_DisplayPlayDuration(win_id, lcd_dev_info, player_info);
				ListeningPlayerWin_DisplayPlayChoose(win_id, lcd_dev_info);
				ListeningPlayerWin_DisplayPlayBottom(win_id, lcd_dev_info);
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
				if(GUI_PointIsInRect(point, listening_lrc_rect))
				{
					LISTENING_PLAYER_INFO * player_infos = NULL;
					player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(win_id);
					SCI_TRACE_LOW("%s: is_local_play = %d", __FUNCTION__, player_info->is_local_play);
					{
						char file_str[50] = {0};
						if(player_info->is_local_play)
						{
							LISTEING_LOCAL_INFO * local_info = Listening_GetLocalDataInfo();
							Listening_GetLrcFileName(file_str,
									local_info->module_info[player_info->moudle_index].module_id,
									local_info->module_info[player_info->moudle_index].album_info[0].album_id,
									local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id);
							if(zmt_file_exist(file_str)){
								player_infos = (LISTENING_PLAYER_INFO*) SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
								player_infos->is_local_play = player_info->is_local_play;
								player_infos->moudle_index = player_info->moudle_index;
								player_infos->audio_index = player_info->audio_index;
								player_play_info.lrc_ready = 1;
								MMI_CreateListeningPlayerLrcWin(player_infos);
							}else{
								Listening_RequestDownloadAudioLrc(local_info->module_info[player_info->moudle_index].module_id,
									local_info->module_info[player_info->moudle_index].album_info[0].album_id,
									local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id,
									local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name);
							}
						}
						else
						{
							
							Listening_GetLrcFileName(file_str,
								album_info->module_id, 
								album_info->album_id,
								album_info->item_info[player_info->audio_index].audio_id
								);
							if(zmt_file_exist(file_str))
							{
								player_infos = (LISTENING_PLAYER_INFO*) SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
								player_infos->is_local_play = player_info->is_local_play;
								player_infos->moudle_index = player_info->moudle_index;
								player_infos->audio_index = player_info->audio_index;
								player_play_info.lrc_ready = 1;
								MMI_CreateListeningPlayerLrcWin(player_infos);
							}
							else
							{
								Listening_RequestDownloadAudioLrc(
									album_info->module_id, 
									album_info->album_id,
									album_info->item_info[player_info->audio_index].audio_id,
									album_info->item_info[player_info->audio_index].audio_name
									);
							}
						}
					}
				}
				else
				{
					ListeningPlayerWin_HandleTpPressUp(win_id, point);
				}
			}
			break;
		case MSG_TIMER:
			{
				listening_play_times++;
				if(MMK_IsFocusWin(win_id))
				{
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}
				SCI_TRACE_LOW("%s: listening_play_times = %d", __FUNCTION__, listening_play_times);
			}
			break;
		case MSG_KEYUP_CANCEL:
			{
				MMK_CloseWin(win_id);	
			}
			break;
		case MSG_CLOSE_WINDOW:
			{
				if(player_play_info.play_status == 1)
				{
					player_play_info.play_status = 0;
					Listening_ResumePlayMp3();
				}
				Listening_StopPlayMp3();
				if(0 != listening_play_timer_id)
				{
					MMK_StopTimer(listening_play_timer_id);
					listening_play_timer_id = 0;
				}
				if(listening_player_lrc_buf)
				{
					SCI_FREE(listening_player_lrc_buf);
				}
				player_play_info.lrc_ready = 0;
				player_play_info.bottom_type = 0;
				player_bottom_show_times = 0;
				MMK_FreeWinAddData(win_id);
			}
			break;
		default:
                    recode = MMI_RESULT_FALSE;
			break;
		}
}

WINDOW_TABLE(MMI_LIST_PLAYER_WIN_TAB) = 
{
	WIN_ID(LISTENING_PLAYER_WIN_ID),
	WIN_FUNC((uint32)HandleListeningPlayerWinMsg),
	CREATE_BUTTON_CTRL(ZMT_LISTEN_NEXT, LISTENING_BUTTON_NEXT_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_PLAY_CTRL_ID),
	CREATE_BUTTON_CTRL(ZMT_LISTEN_PRE, LISTENING_BUTTON_PREV_CTRL_ID),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMI_CreateListeningPlayerWin(LISTENING_PLAYER_INFO * player_info)
{
	MMI_WIN_ID_T win_id = LISTENING_PLAYER_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_LIST_PLAYER_WIN_TAB, (ADD_DATA)player_info);
	MMK_SetWinRect(win_handle, &rect);
	if(0 != listening_play_timer_id)
	{
		MMK_StopTimer(listening_play_timer_id);
		listening_play_timer_id = 0;
	}
	listening_play_timer_id = MMK_CreateTimerCallback(1000, ListeningPlayerTimerCallback, PNULL, TRUE);
}

LOCAL void ListeningPlayerTimerCallback(uint8 timer_id, uint32 * param)
{
	listening_play_times++;
	//SCI_TRACE_LOW("%s: listening_play_times = %d", __FUNCTION__, listening_play_times);
	if(MMK_IsFocusWin(LISTENING_PLAYER_WIN_ID))
	{
		MMK_SendMsg(LISTENING_PLAYER_WIN_ID, MSG_FULL_PAINT, PNULL);
	}
	if(player_bottom_show_times > 0)
	{
		player_bottom_show_times--;
	}
	if(player_bottom_show_times == 0)
	{
		player_play_info.bottom_type = 0;
	}
}

LOCAL void ListeningPlayer_InitPlayerInfo(void)
{
	LISTEING_PLAYER_INFO player_info = {0};
	MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
	if(player_info.volume == 0)
	{
		player_info.volume = MMIAPISET_GetMultimVolume();
	}
	player_play_info.play_style = player_info.style;
	player_play_info.volume = player_info.volume;
}

/////////////////////////////////////////////////////////////////////////////////
LOCAL void display_lrc(MMI_CTRL_ID_T ctrl_id,char *path )
{
	MMI_STRING_T content_text = {0};
	uint16 * content_wstr =PNULL;
	char buf[1024];
	int curline = 0,starline=0, i = 0,start=0;
	int time[1024];
	char *lrc[1024];
	char *file_buf=PNULL;
	uint16 * wtmp=L"\n";
	char tmp[20]={0};
	int data_size=0;
	GUI_WstrToUTF8(tmp,20,wtmp,MMIAPICOM_Wstrlen(wtmp));
	file_buf=zmt_file_data_read(path,&data_size);
	//Listening_Decryption(file_buf, data_size);
	if(data_size>0)
	{
		while(getLine(buf, file_buf,&start) != 0){
			if(praseLRC(buf, &time[curline])){
				starline=curline;
				curline++;
				while(praseLRC(buf, &time[curline]))
				{
					curline++;
				}
				while (starline<curline)
				{
					lrc[starline]=SCI_ALLOC(strlen(buf)+1);
					SCI_MEMSET(lrc[starline],0,strlen(buf)+1);
					strcpy(lrc[starline],buf);
					starline++;
				}
			}
			SCI_MEMSET(buf,0,1024);
		} 
		sort(time,lrc,curline);
		SCI_MEMSET(file_buf,0,data_size);
		content_wstr=SCI_ALLOC_APPZ(data_size*2);
		SCI_MEMSET(content_wstr,0,data_size*2);
		//SCI_TRACE_LOW("%s: curline = %d", __FUNCTION__, curline);
		for(i=0;i<curline;i++)
		{
			if(i % 2 == 1) continue;
			if(lrc[i]!=PNULL && strlen(lrc[i]) > 2)
			{
				if(lrc[i]!=""||lrc[i]!="\n"||lrc[i]!=" "||lrc[i]!=" \n")
				{
						strcat(file_buf,lrc[i]);
				}
				SCI_FREE(lrc[i]);
				lrc[i]=PNULL;
			}
		}
		GUI_UTF8ToWstr(content_wstr,data_size*2, file_buf,strlen(file_buf)+1);
		content_text.wstr_len = MMIAPICOM_Wstrlen(content_wstr)+1;
		content_text.wstr_ptr = content_wstr;
		GUITEXT_SetString(ctrl_id,content_text.wstr_ptr, content_text.wstr_len, FALSE);
		SCI_FREE(file_buf);
		SCI_FREE(content_wstr);
	}
}


LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;
LOCAL int listening_player_lrc_page = 0;

LOCAL MMI_RESULT_E HandleListeningPlayerLrcWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	LISTENING_PLAYER_INFO * player_info = NULL;
	MMI_CTRL_ID_T ctrl_id = LISTENING_TEXTBOX_PLAYER_CTRL_ID;
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				GUI_FONT_ALL_T font = {0};
				GUI_RECT_T rect = {10,70,230,359};
				GUI_FONT_T text_font = SONG_FONT_20;
				GUI_COLOR_T text_color = MMI_BLACK_COLOR;
				font.font = SONG_FONT_16;
				font.color = MMI_BLACK_COLOR;
                rect = listen_win_rect;
                rect.left += 10;
                rect.right -= 10;
				GUITEXT_SetRect(ctrl_id, &rect);
				GUITEXT_SetFont(ctrl_id, &text_font, &text_color);
				GUITEXT_IsDisplayPrg(FALSE, ctrl_id);
				GUITEXT_SetHandleTpMsg(FALSE, ctrl_id);
				GUITEXT_SetClipboardEnabled(ctrl_id,FALSE);
				GUITEXT_SetAlign(ctrl_id, ALIGN_HVMIDDLE);
				GUITEXT_IsSlide(ctrl_id,FALSE);
			}
			break;
		case MSG_FULL_PAINT:
			{
				LISTEING_LOCAL_INFO * local_info = NULL;
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar title_wchar[50] = {0};
				uint8 length = 0;
				char lrc_file[50] = {0};
				
				GUI_FillRect(&lcd_dev_info, listen_win_rect, MMI_WHITE_COLOR);

				text_style.align = ALIGN_LVMIDDLE;
				text_style.font = SONG_FONT_16;
				text_style.font_color = MMI_BLACK_COLOR;
				player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(win_id);
				if(player_info->is_local_play)
				{
					local_info = Listening_GetLocalDataInfo();
					length = strlen(local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name);
					#ifdef WIN32
					GUI_GBToWstr(title_wchar, local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name, length);
					#else
					GUI_UTF8ToWstr(title_wchar, 50, local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name, length);
					#endif
				}else
				{
					length = strlen(album_info->item_info[player_info->audio_index].audio_name) + 1;
					GUI_UTF8ToWstr(title_wchar, 50, album_info->item_info[player_info->audio_index].audio_name, length);
				}
				text_string.wstr_ptr = title_wchar;
				text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&listen_title_rect,
					&listen_title_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
				//GUIRES_DisplayImg(PNULL, &back_rect, PNULL, win_id, IMG_BACK, &lcd_dev_info);

				SCI_TRACE_LOW("%s: player_play_info.lrc_ready = %d", __FUNCTION__, player_play_info.lrc_ready);
				if(player_play_info.lrc_ready == 0)
				{
					text_style.align = ALIGN_HVMIDDLE;
					MMIRES_GetText(ZMT_TXT_LOADING, win_id, &text_string);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&listen_win_rect,
						&listen_win_rect,
						&text_string,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);
					break;
				}
				else if(player_play_info.lrc_ready == 2)
				{
					text_style.align = ALIGN_HVMIDDLE;
					MMIRES_GetText(ZMT_TXT_LOADING_FAIL, win_id, &text_string);
					GUISTR_DrawTextToLCDInRect(
						(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
						&listen_win_rect,
						&listen_win_rect,
						&text_string,
						&text_style,
						text_state,
						GUISTR_TEXT_DIR_AUTO
						);
					break;	
				}

				SCI_TRACE_LOW("%s: player_info->is_local_play = %d", __FUNCTION__, player_info->is_local_play);
				if(player_info->is_local_play)
				{
					Listening_GetLrcFileName(lrc_file,
						local_info->module_info[player_info->moudle_index].module_id,
						local_info->module_info[player_info->moudle_index].album_info[0].album_id,
						local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id
						);
				}
				else
				{
					Listening_GetLrcFileName(lrc_file, 
						album_info->module_id,
						album_info->album_id,
						album_info->item_info[player_info->audio_index].audio_id);
				}
				if(zmt_file_exist(lrc_file))
				{
					display_lrc(ctrl_id,lrc_file);
				}
				
				//dispaly page dir
				
			}
			break;
		case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T point = {0};
			int16 tp_offset_x;
			int16 tp_offset_y;
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			tp_offset_x = point.x - main_tp_down_x;
			tp_offset_y = point.y - main_tp_down_y;
			if(ABS(tp_offset_x) <= ABS(tp_offset_y))
			{
				if(tp_offset_y > 40)
				{
					MMK_PostMsg(win_id, MSG_KEYREPEAT_UP, PNULL, 0);
					//listening_player_lrc_page++;
				}
				else if(tp_offset_y < -40)
				{
					MMK_PostMsg(win_id, MSG_KEYREPEAT_DOWN, PNULL, 0);
					//listening_player_lrc_page++;
				}
				if(listening_player_lrc_page > 4)
				{
				#ifndef WIN32
					//MMIREADBOY_RefreshWindow();
				#endif
					listening_player_lrc_page = 0;
				}
				break;
			}
		}
		break;
		case MSG_TP_PRESS_DOWN:
		       {
			       main_tp_down_x = MMK_GET_TP_X(param);
			       main_tp_down_y = MMK_GET_TP_Y(param);
		       }
		       break;
		case MSG_KEYUP_CANCEL:
			{
				MMK_CloseWin(win_id);	
			}
			break;
		case MSG_CLOSE_WINDOW:
			{
				MMK_FreeWinAddData(win_id);
				listening_player_lrc_page = 0;
			}
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;
}

WINDOW_TABLE(MMI_LIST_PLAYER_LRC_WIN_TAB) = 
{
	WIN_ID(LISTENING_PLAYER_LRC_WIN_ID),
	WIN_FUNC((uint32)HandleListeningPlayerLrcWinMsg),
	CREATE_TEXT_CTRL(LISTENING_TEXTBOX_PLAYER_CTRL_ID),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMI_CreateListeningPlayerLrcWin(LISTENING_PLAYER_INFO * player_info)
{
	MMI_WIN_ID_T win_id = LISTENING_PLAYER_LRC_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_LIST_PLAYER_LRC_WIN_TAB, (ADD_DATA)player_info);
	MMK_SetWinRect(win_handle, &rect);
}


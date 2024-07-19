
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
#ifdef MATH_COUNT_SUPPORT
#include "math_count_image.h"
#endif
#ifdef LYRIC_PARSER_SUPPORT
#include "lyric_parser.h"
#endif

extern LISTENING_ALBUM_INFO * album_info;
extern uint8 listening_downloading_index;
uint32 listening_play_times = 0;
uint8 listening_play_timer_id = 0;
MMISRV_HANDLE_T listening_player_handle=PNULL;
char * listening_player_lrc_buf = NULL;
LISTENING_PALYER_PLAY_INFO player_play_info = {0};
LOCAL uint8 player_bottom_show_times = 0;
#ifdef LYRIC_PARSER_SUPPORT
lyric_t *listening_lrc_p = NULL;
char listening_intput_lrc[50] = {0};
int g_lrc_sel_index = -1;
int g_lrc_all_index = -1;
#endif
LOCAL GUI_LCD_DEV_INFO listening_play_volume_layer={0};
LOCAL uint16 listening_lrc_timers = 0;
LOCAL uint8 listening_lrc_timer_id=0;
LOCAL uint8 listening_all_lrc_timer_id=0;
LOCAL uint16 listening_all_lrc_timers = 0;
BOOLEAN listening_all_lrc_download = FALSE;

extern GUI_RECT_T listen_win_rect;
extern GUI_RECT_T listen_title_rect;
LOCAL GUI_RECT_T listening_pre_rect = {0.8*LISTEN_LINE_WIDTH, 2.3*LISTEN_LINE_HIGHT, 1.8*LISTEN_LINE_WIDTH, 3.7*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_play_rect = {2*LISTEN_LINE_WIDTH, 1.5*LISTEN_LINE_HIGHT, 4*LISTEN_LINE_WIDTH, 4.5*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_next_rect = {4.2*LISTEN_LINE_WIDTH ,2.3*LISTEN_LINE_HIGHT, 5.2*LISTEN_LINE_WIDTH, 3.7*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_play_time_rect = {0.5*LISTEN_LINE_WIDTH, 4*LISTEN_LINE_HIGHT, 2*LISTEN_LINE_WIDTH, 5*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_dis_play_rect = {0.5*LISTEN_LINE_WIDTH, 4.9*LISTEN_LINE_HIGHT, 5.5*LISTEN_LINE_WIDTH, 5.3*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_dis_total_rect = {0.5*LISTEN_LINE_WIDTH, 4.9*LISTEN_LINE_HIGHT, 5.5*LISTEN_LINE_WIDTH, 5.3*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_total_rect = {5*LISTEN_LINE_WIDTH, 4*LISTEN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 5*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_lrc_rect = {5, 5.3*LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH - 5, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T listening_rect_empty_bottom = {0, 6*LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T listening_play_style_rect = {0.5*LISTEN_LINE_WIDTH, 6*LISTEN_LINE_HIGHT+7, 1.5*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT+7};
LOCAL GUI_RECT_T listening_rect_style_loop = {0.8*LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 1.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_style_random = {2.8*LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 3.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_style_single = {4.8*LISTEN_LINE_WIDTH, 8*LISTEN_LINE_HIGHT, 5.8*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_volume_rect = {4.8*LISTEN_LINE_WIDTH, 6*LISTEN_LINE_HIGHT+5, 5.8*LISTEN_LINE_WIDTH, 7*LISTEN_LINE_HIGHT+5};
LOCAL GUI_RECT_T listening_rect_empty = {0, 7*LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T listening_rect_decrese = {7, 8.2*LISTEN_LINE_HIGHT, 2*LISTEN_LINE_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_volume = {LISTEN_LINE_WIDTH-2, 8.2*LISTEN_LINE_HIGHT, 5*LISTEN_LINE_WIDTH+2, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_rect_increse = {5*LISTEN_LINE_WIDTH+10, 8.2*LISTEN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, 9*LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listening_lrc_txt_rect = {5, LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH-5, MMI_MAINSCREEN_HEIGHT};

LOCAL void ListeningPlayer_InitPlayerInfo(void);
LOCAL void ListeningPlayer_ButtonPreCallback(void);
LOCAL void ListeningPlayer_ButtonNextCallback(void);
LOCAL void ListeningPlayerTimerCallback(uint8 timer_id, uint32 * param);
LOCAL void ListeningPlayerWin_LrcShowText(BOOLEAN is_full_paint);
PUBLIC void ListeningPlayerWin_LrcParser(MMI_WIN_ID_T win_id, char * lrc_path);

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
    audio_srv.volume = player_play_info.volume;

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

    ListeningPlayer_InitPlayerInfo();

    full_path_len = GUI_GBToWstr(full_path, (const uint8*)file_name, SCI_STRLEN(file_name));
    audio_data.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
    audio_data.ring_file.name = full_path;
    audio_data.ring_file.name_len = full_path_len;
    audio_data.ring_file.fmt  = (MMISRVAUD_RING_FMT_E)MMIAPICOM_GetMusicType(audio_data.ring_file.name, audio_data.ring_file.name_len);

    if(Listening_Play_RequestHandle(&audio_handle, MMISRVAUD_ROUTE_NONE, &audio_data, Listening_PlayMp3Notify))
    {
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
#ifdef WIN32
	duration = 243;
#endif

	SCI_TRACE_LOW("%s: duration = %d", __FUNCTION__, duration);
	text_style.align = ALIGN_LVMIDDLE;
	text_style.font = DP_FONT_16;
	text_style.font_color = MMI_WHITE_COLOR;

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

	LCD_FillRoundedRect(&lcd_dev_info, listening_dis_total_rect, listening_dis_total_rect, GUI_RGB2RGB565(43,121,208));
	listening_dis_play_rect.right = (0.5*LISTEN_LINE_WIDTH) + ((float)((float)listening_play_times / (float)duration) * (5*LISTEN_LINE_WIDTH));
	if(listening_dis_play_rect.right > 5.5*LISTEN_LINE_WIDTH)
	{
		listening_dis_play_rect.right = 5.5*LISTEN_LINE_WIDTH;
	}
	LCD_FillRoundedRect(&lcd_dev_info, listening_dis_play_rect, listening_dis_play_rect, GUI_RGB2RGB565(71,235,255));

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
	
	GUI_FillRect(&lcd_dev_info, listening_rect_empty_bottom, GUI_RGB2RGB565(80, 162, 254));

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
}

LOCAL void ListeningPlayerWin_DisplayPlayBottom(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
#if LISTENING_PLAYER_USE_STYLE_VOLUME != 1
	UILAYER_APPEND_BLT_T append_layer = {0};
	UILAYER_Clear(&lcd_dev_info);
	append_layer.lcd_dev_info = listening_play_volume_layer;
	append_layer.layer_level = UILAYER_LEVEL_HIGH;
	UILAYER_AppendBltLayer(&append_layer);
	lcd_dev_info = listening_play_volume_layer;
	listening_rect_empty.top = listening_rect_volume.top;
	listening_rect_empty.bottom = listening_rect_volume.bottom;
#endif
	GUI_FillRect(&lcd_dev_info, listening_rect_empty, GUI_RGB2RGB565(80, 162, 254));
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
		text_style.font = DP_FONT_16;
		text_style.font_color = MMI_WHITE_COLOR;
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
		uint8 img_width = 7;
		uint8 img_space = 5;
		uint8 count = 2*player_play_info.volume;
		uint8 i = 0;
        #if LISTENING_PLAYER_USE_STYLE_VOLUME != 0
		GUI_RECT_T rect_volume = {LISTEN_LINE_WIDTH, 8.3*LISTEN_LINE_HIGHT, LISTEN_LINE_WIDTH+7, 9*LISTEN_LINE_HIGHT};
		GUIRES_DisplayImg(PNULL, &listening_rect_decrese, PNULL, win_id, MATH_COUNT_REDUCE_IMG, &lcd_dev_info);
		GUIRES_DisplayImg(PNULL, &listening_rect_increse, PNULL, win_id, MATH_COUNT_ADD_IMG, &lcd_dev_info);
        #else
		GUI_RECT_T rect_volume = {15, 8.3*LISTEN_LINE_HIGHT, 22, 9*LISTEN_LINE_HIGHT};
		listening_rect_volume.left = 10;
		listening_rect_volume.right = MMI_MAINSCREEN_WIDTH - 10;
        #endif
        
		LCD_FillRoundedRect(&lcd_dev_info, listening_rect_volume, listening_rect_volume, GUI_RGB2RGB565(43, 121, 208));
		LCD_DrawRoundedRect(&lcd_dev_info, listening_rect_volume, listening_rect_volume, GUI_RGB2RGB565(43, 121, 208));
		for(i = 0; i < count && i < 2*MMISRVAUD_VOLUME_LEVEL_MAX; i++)
		{
		    GUIRES_DisplayImg(PNULL,&rect_volume, PNULL,win_id, MATH_COUNT_PROCESS_IMG, &lcd_dev_info);
		    rect_volume.left = rect_volume.right + img_space;
		    rect_volume.right += img_width + img_space;
		}
	}
#if LISTENING_PLAYER_USE_STYLE_VOLUME != 1
	else
	{
	    UILAYER_RemoveBltLayer(&listening_play_volume_layer);
	}
#endif
}

LOCAL void ListeningPlayer_DecreseVolume(void)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	LISTEING_PLAYER_INFO player_info = {0};
	if(player_play_info.volume > 1)
	{
		MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
		player_play_info.volume--;
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
	if(player_play_info.volume < 9)
	{
		MMI_ReadNVItem(MMINV_LISTENING_PLAYER_INFO, &player_info);
		player_play_info.volume++;
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
			char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
			Listening_GetLrcFileName(file_path, module_id, album_id, audio_id);
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
            #ifdef LYRIC_PARSER_SUPPORT
			ListeningPlayerWin_LrcParser(LISTENING_PLAYER_WIN_ID, file_path);
            #endif
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
			char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
			Listening_GetLrcFileName(file_path, module_id, album_id, audio_id);
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
            #ifdef LYRIC_PARSER_SUPPORT
			ListeningPlayerWin_LrcParser(LISTENING_PLAYER_WIN_ID, file_path);
            #endif
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
			char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
			Listening_GetLrcFileName(file_path, module_id, album_id, audio_id);
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
            #ifdef LYRIC_PARSER_SUPPORT
			ListeningPlayerWin_LrcParser(LISTENING_PLAYER_WIN_ID, file_path);
            #endif
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
			char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
			Listening_GetLrcFileName(file_path, module_id, album_id, audio_id);
			player_play_info.lrc_ready = 0;
			player_play_info.bottom_type = 0;
			if(MMK_IsOpenWin(LISTENING_PLAYER_LRC_WIN_ID))
			{
				MMK_CloseWin(LISTENING_PLAYER_LRC_WIN_ID);
			}
            #ifdef LYRIC_PARSER_SUPPORT
			ListeningPlayerWin_LrcParser(LISTENING_PLAYER_WIN_ID, file_path);
            #endif
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
	if(GUI_PointIsInRect(point, listening_lrc_rect))
	{
            LISTENING_PLAYER_INFO * player_infos = NULL;
            LISTENING_PLAYER_INFO * player_info = NULL;
            player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(win_id);
            player_infos = (LISTENING_PLAYER_INFO*) SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
            player_infos->is_local_play = player_info->is_local_play;
            player_infos->moudle_index = player_info->moudle_index;
            player_infos->audio_index = player_info->audio_index;
            player_play_info.lrc_ready = 1;
            MMI_CreateListeningPlayerLrcWin(player_infos);
	}
#if LISTENING_PLAYER_USE_STYLE_VOLUME != 0
	//bottom type
	else if(GUI_PointIsInRect(point, listening_play_style_rect))
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
	ListeningPlayerWin_LrcShowText(TRUE);
#endif
}

#ifdef LYRIC_PARSER_SUPPORT
void listening_roller_lrc_free(void)
{
    if(listening_lrc_p != NULL){
        //SCI_TRACE_LOW("%s: free", __FUNCTION__);
        lyric_uninit(listening_lrc_p);
        listening_lrc_p = NULL;
    }
    memset(&listening_intput_lrc, 0, 50);
    if(listening_lrc_timer_id){
        MMK_StopTimer(listening_lrc_timer_id);
        listening_lrc_timer_id = 0;
    }
    g_lrc_sel_index = -1;
    g_lrc_all_index = -1;
}

LOCAL void ListeningPlayerWin_LrcShowText(BOOLEAN is_full_paint)
{
    if (listening_lrc_p != NULL) 
    {
        int i = 0;
        int lyric_sel_index = 0;
        uint32 curtimsMs = 0;
        MMISRVAUD_PLAY_INFO_T play_info = {0};
    #ifndef WIN32
        if(listening_player_handle == PNULL){
            SCI_TRACE_LOW("%s: listening_player_handle not ready!", __FUNCTION__);
            return;
        }
        MMISRVAUD_GetPlayingInfo(listening_player_handle, &play_info);
        //SCI_TRACE_LOW("%s: play_info.cur_time = %d", __FUNCTION__, play_info.cur_time);
        curtimsMs = play_info.cur_time * 1000;
    #else
        curtimsMs = listening_lrc_timers*1000;
    #endif  
        lyric_sel_index = lyric_get_node_index(listening_lrc_p, (long) curtimsMs);
        //SCI_TRACE_LOW("%s: lyric_sel_index = %d", __FUNCTION__, lyric_sel_index);
        if (lyric_sel_index >= 0 && (lyric_sel_index != g_lrc_sel_index || is_full_paint == TRUE))
        {
            MMI_STRING_T text_string = {0};
            wchar * text_str = NULL;
            lyric_content_node * node = lyric_get_node_by_index(listening_lrc_p, lyric_sel_index);
            //SCI_TRACE_LOW("%s: node->content = %s", __FUNCTION__, node->content);
            text_str = SCI_ALLOC_APPZ(3*strlen(node->content));
            memset(text_str, 0, 3*strlen(node->content));
            GUI_UTF8ToWstr(text_str, 3*strlen(node->content), node->content, strlen(node->content));
            text_string.wstr_ptr = text_str;
            text_string.wstr_len = MMIAPICOM_Wstrlen(text_str);
            GUITEXT_SetString(LISTENING_TEXTBOX_PLAYER_CTRL_ID, text_string.wstr_ptr, text_string.wstr_len, TRUE);
            GUITEXT_SetResetTopDisplay(LISTENING_TEXTBOX_PLAYER_CTRL_ID, 0);
            SCI_FREE(text_str);
            g_lrc_sel_index = lyric_sel_index;
        }
    }
}

PUBLIC void ListeningPlayerWin_LrcParser(MMI_WIN_ID_T win_id, char * lrc_path)
{
    if(!zmt_file_exist(lrc_path)){
        return;
    }

    listening_roller_lrc_free();
 
    strcpy(listening_intput_lrc, lrc_path);
    
    listening_lrc_p = lyric_init(listening_intput_lrc);
    
    listening_lrc_timer_id = MMK_CreateWinTimer(win_id, 300, TRUE);
    MMK_StartWinTimer(win_id, listening_lrc_timer_id, 300, TRUE);
    
    ListeningPlayerWin_LrcShowText(FALSE);
}

#endif

LOCAL void ListeningPlayerWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T ctrl_id = LISTENING_TEXTBOX_PLAYER_CTRL_ID;
    GUI_FONT_T text_font = DP_FONT_18;
    GUI_COLOR_T text_color = MMI_WHITE_COLOR;
    GUI_BG_T bg = {0};
    LISTENING_PLAYER_INFO * player_infos = NULL;
    LISTENING_PLAYER_INFO * player_info = NULL;
    char file_str[50] = {0};
    int module_id = 0;
    int album_id = 0;
    int audio_id = 0;
    
    GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_NEXT_CTRL_ID, ListeningPlayer_ButtonNextCallback);
    GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PLAY_CTRL_ID, ListeningPlayer_ButtonPlayCallback);
    GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PREV_CTRL_ID, ListeningPlayer_ButtonPreCallback);

    GUITEXT_SetRect(ctrl_id, &listening_lrc_rect);
    GUITEXT_SetFont(ctrl_id, &text_font, &text_color);
    GUITEXT_IsDisplayPrg(FALSE, ctrl_id);
    GUITEXT_SetHandleTpMsg(FALSE, ctrl_id);
    GUITEXT_SetClipboardEnabled(ctrl_id,FALSE);
    GUITEXT_SetAlign(ctrl_id, ALIGN_HMIDDLE);
    GUITEXT_IsSlide(ctrl_id,FALSE);
    bg.bg_type = GUI_BG_COLOR;
    bg.color = GUI_RGB2RGB565(80, 162, 254);
    GUITEXT_SetBg(ctrl_id, &bg);

    player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(win_id);
    SCI_TRACE_LOW("%s: is_local_play = %d", __FUNCTION__, player_info->is_local_play);
    {
        if(player_info->is_local_play)
        {
            LISTEING_LOCAL_INFO * local_info = Listening_GetLocalDataInfo();
            module_id = local_info->module_info[player_info->moudle_index].module_id;
            album_id = local_info->module_info[player_info->moudle_index].album_info[0].album_id;
            audio_id = local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id;
            Listening_GetLrcFileName(file_str, module_id, album_id, audio_id);
            if(zmt_file_exist(file_str)){
                player_infos = (LISTENING_PLAYER_INFO*) SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
                player_infos->is_local_play = player_info->is_local_play;
                player_infos->moudle_index = player_info->moudle_index;
                player_infos->audio_index = player_info->audio_index;
                player_play_info.lrc_ready = 1;
                //MMI_CreateListeningPlayerLrcWin(player_infos);
                #ifdef LYRIC_PARSER_SUPPORT
                ListeningPlayerWin_LrcParser(win_id, file_str);
                #endif
            }else{
                Listening_RequestDownloadAudioLrc(module_id,album_id,audio_id,
                    local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name
                );
            }
        }
        else
        {
            module_id = album_info->module_id;
            album_id = album_info->album_id;
            audio_id = album_info->item_info[player_info->audio_index].audio_id;
            Listening_GetLrcFileName(file_str, module_id, album_id, audio_id);
            if(zmt_file_exist(file_str))
            {
                player_infos = (LISTENING_PLAYER_INFO*) SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
                player_infos->is_local_play = player_info->is_local_play;
                player_infos->moudle_index = player_info->moudle_index;
                player_infos->audio_index = player_info->audio_index;
                player_play_info.lrc_ready = 1;
                //MMI_CreateListeningPlayerLrcWin(player_infos);
                #ifdef LYRIC_PARSER_SUPPORT
                ListeningPlayerWin_LrcParser(win_id, file_str);
                #endif
            }
            else
            {
                Listening_RequestDownloadAudioLrc(module_id, album_id, audio_id,
                    album_info->item_info[player_info->audio_index].audio_name
                );
            }
        }
    }
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
				ListeningPlayerWin_OPEN_WINDOW(win_id);
                #if LISTENING_PLAYER_USE_STYLE_VOLUME != 1
				if (UILAYER_IsMultiLayerEnable())
				{              
				    UILAYER_CREATE_T create_info = {0};
				    create_info.lcd_id = MAIN_LCD_ID;
				    create_info.owner_handle = win_id;
				    create_info.offset_x = listening_rect_empty.left;
				    create_info.offset_y = listening_rect_empty.top;
				    create_info.width = listening_rect_empty.right - listening_rect_empty.left;
				    create_info.height = listening_rect_empty.bottom - listening_rect_empty.top;
				    create_info.is_bg_layer = FALSE;
				    create_info.is_static_layer = FALSE;
				    UILAYER_CreateLayer(&create_info, &listening_play_volume_layer);
				}
                #endif
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar title_wchar[50] = {0};
				uint8 length = 0;
				uint8 line_num = 0;
				
				GUI_FillRect(&lcd_dev_info, listen_win_rect, GUI_RGB2RGB565(80, 162, 254));
				GUI_FillRect(&lcd_dev_info, listen_title_rect, GUI_RGB2RGB565(108, 181, 255));
                
				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_20;
				text_style.font_color = MMI_WHITE_COLOR;
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
				line_num = GUI_CalculateStringLinesByPixelNum(MMI_MAINSCREEN_WIDTH,text_string.wstr_ptr,text_string.wstr_len,DP_FONT_20,0,TRUE);
				if(line_num > 1){
				    text_style.font = DP_FONT_16;
				}
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
                #if LISTENING_PLAYER_USE_STYLE_VOLUME != 0
				ListeningPlayerWin_DisplayPlayChoose(win_id, lcd_dev_info);
                #endif
				ListeningPlayerWin_DisplayPlayBottom(win_id, lcd_dev_info);
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
				if(point.y > listening_dis_total_rect.bottom)
				{
					ListeningPlayerWin_HandleTpPressUp(win_id, point);
				}
			}
			break;
		case MSG_APP_LEFT:
			{
			    ListeningPlayer_ButtonPreCallback();
			}
			break;
		case MSG_APP_OK:
		case MSG_APP_WEB:
		case MSG_CTL_MIDSK:
		case MSG_CTL_OK:
		case MSG_CTL_PENOK:
			{
			    ListeningPlayer_ButtonPlayCallback();
			}
			break;
		case MSG_APP_RIGHT:
			{
			    ListeningPlayer_ButtonNextCallback();
			}
			break;
		case MSG_TIMER:
			{
				if(MMK_IsFocusWin(win_id)){
				    listening_lrc_timers++;
				    ListeningPlayerWin_LrcShowText(FALSE);
				}
			}
			break;
#if LISTENING_PLAYER_USE_STYLE_VOLUME != 1
		case MSG_KEYDOWN_UPSIDE:
		case MSG_KEYDOWN_VOL_UP:
			{
			    player_play_info.bottom_type = 2;
			    ListeningPlayer_IncreseVolume();
			    player_bottom_show_times = 9;
			}
			break;
		case MSG_KEYDOWN_DOWNSIDE:
		case MSG_KEYDOWN_VOL_DOWN:
			{
			    player_play_info.bottom_type = 2;
			    ListeningPlayer_DecreseVolume();
			    player_bottom_show_times = 9;
			}
			break;
#endif
		case MSG_KEYDOWN_CANCEL:
                    break;
		case MSG_KEYUP_RED:
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
				listening_roller_lrc_free();
				listening_lrc_timers = 0;
				WATCHCOM_Backlight(FALSE);
			}
			break;
		default:
                    recode = MMI_RESULT_FALSE;
			break;
		}
    return recode;
}

WINDOW_TABLE(MMI_LIST_PLAYER_WIN_TAB) = 
{
	WIN_ID(LISTENING_PLAYER_WIN_ID),
	WIN_FUNC((uint32)HandleListeningPlayerWinMsg),
	CREATE_BUTTON_CTRL(ZMT_LISTEN_NEXT, LISTENING_BUTTON_NEXT_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_PLAY_CTRL_ID),
	CREATE_BUTTON_CTRL(ZMT_LISTEN_PRE, LISTENING_BUTTON_PREV_CTRL_ID),
	CREATE_TEXT_CTRL(LISTENING_TEXTBOX_PLAYER_CTRL_ID),
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
	WATCHCOM_Backlight(TRUE);
}

LOCAL void ListeningPlayerTimerCallback(uint8 timer_id, uint32 * param)
{
	listening_play_times++;
	//SCI_TRACE_LOW("%s: listening_play_times = %d", __FUNCTION__, listening_play_times);
	if(MMK_IsFocusWin(LISTENING_PLAYER_WIN_ID))
	{
		MMK_SendMsg(LISTENING_PLAYER_WIN_ID, MSG_FULL_PAINT, PNULL);
		ListeningPlayerWin_LrcShowText(TRUE);
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
LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;
LOCAL int listening_player_lrc_page = 0;

#ifdef LYRIC_PARSER_SUPPORT
LOCAL void ListeningPlayerLrcWin_ShowLrcText(MMI_WIN_ID_T win_id, BOOLEAN is_full_paint)
{
    int offset = 0;
    int i = 0;
    MMISRVAUD_PLAY_INFO_T play_info = {0};
    uint32 curtimsMs = 0;
    int lyric_sel_index = 0;
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    wchar * text_str = NULL;
    GUI_RECT_T text_rect = listening_lrc_txt_rect;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;
    text_style.line_color = MMI_WHITE_COLOR;

#ifndef WIN32
    if(listening_player_handle == PNULL){
        SCI_TRACE_LOW("%s: listening_player_handle not ready!", __FUNCTION__);
        return;
    }
    MMISRVAUD_GetPlayingInfo(listening_player_handle, &play_info);
    //SCI_TRACE_LOW("%s: play_info.cur_time = %d", __FUNCTION__, play_info.cur_time);
    curtimsMs = play_info.cur_time * 1000;
#else
    curtimsMs = listening_all_lrc_timers*1000;
#endif  
    lyric_sel_index = lyric_get_node_index(listening_lrc_p, (long) curtimsMs);
    if (lyric_sel_index > 0 && (lyric_sel_index != g_lrc_all_index || is_full_paint == TRUE))
    {
        GUI_FillRect(&lcd_dev_info, listening_lrc_txt_rect, GUI_RGB2RGB565(80, 162, 254));
        offset = lyric_sel_index - LISTEN_LRC_LABEL_NUM / 2 + 3;
        for (i = 0; i < LISTEN_LRC_LABEL_NUM; i++) {
            lyric_content_node * last_node = NULL;
            lyric_content_node * node = NULL;
            int lrc_index = offset + i;
            if (lrc_index < 0) {
                continue;
            }
            //SCI_TRACE_LOW("%s: lrc_index = %d", __FUNCTION__, lrc_index);
            last_node = lyric_get_node_by_index(listening_lrc_p, lrc_index - 1);
            node = lyric_get_node_by_index(listening_lrc_p, lrc_index);
            if (node != NULL && last_node != NULL){
                if(0 != strcmp(last_node->content, node->content) || lrc_index < listening_lrc_p->lyric_num - LISTEN_LRC_LABEL_NUM)
                {
                    uint8 line_num = 0;
                    text_str = SCI_ALLOC_APPZ(3*strlen(node->content));
                    memset(text_str, 0, 3*strlen(node->content));
                    GUI_UTF8ToWstr(text_str, 3*strlen(node->content), node->content, strlen(node->content));
                    text_string.wstr_ptr = text_str;
                    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                    line_num = GUI_CalculateStringLinesByPixelNum(text_rect.right-text_rect.left,text_string.wstr_ptr,text_string.wstr_len,DP_FONT_22,0,TRUE);
                    text_rect.bottom = text_rect.top + 0.8*line_num*LISTEN_LINE_HIGHT;
                    if(lyric_sel_index == lrc_index){
                        text_style.font = DP_FONT_22;
                        text_style.font_color = MMI_WHITE_COLOR;
                    }else{
                        text_style.font = DP_FONT_20;
                        text_style.font_color = GUI_RGB2RGB565(230, 230, 230);
                    }
                    GUISTR_DrawTextToLCDInRect(
                        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                        &text_rect,
                        &text_rect,
                        &text_string,
                        &text_style,
                        text_state,
                        GUISTR_TEXT_DIR_AUTO
                    );
                    text_rect.top = text_rect.bottom;
                    SCI_FREE(text_str);
                }
            }
        }
        g_lrc_all_index = lyric_sel_index;
    }
}

PUBLIC void ListeningPlayerLrcWin_LrcParser(MMI_WIN_ID_T win_id, char * lrc_path)
{
    if(!zmt_file_exist(lrc_path)){
        return;
    }
    if(listening_intput_lrc != NULL){
        return;
    }
    listening_roller_lrc_free();
 
    strcpy(listening_intput_lrc, lrc_path);
    
    listening_lrc_p = lyric_init(listening_intput_lrc);
    
    ListeningPlayerLrcWin_ShowLrcText(win_id, FALSE);
}
#endif

LOCAL MMI_RESULT_E HandleListeningPlayerLrcWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T ctrl_id = LISTENING_TEXTBOX_PLAYER_LRC_CTRL_ID;
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
			#ifdef LYRIC_PARSER_SUPPORT
			    if(listening_all_lrc_timer_id != 0){
			        MMK_StopTimer(listening_all_lrc_timer_id);
			        listening_all_lrc_timer_id = 0;
			    }
                        listening_all_lrc_timer_id = MMK_CreateWinTimer(win_id, 300, TRUE);
                        MMK_StartTimer(listening_all_lrc_timer_id, 300, TRUE);
			#endif
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_FROM_HEAD;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar texe_str[50] = {0};
				LISTEING_LOCAL_INFO * local_info = NULL;
				LISTENING_PLAYER_INFO *player_info = NULL;
				wchar title_wchar[50] = {0};
				uint8 length = 0;
				uint8 line_num = 0;

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_20;
				text_style.font_color = MMI_WHITE_COLOR;
				
				GUI_FillRect(&lcd_dev_info, listen_win_rect, GUI_RGB2RGB565(80, 162, 254));
				GUI_FillRect(&lcd_dev_info, listen_title_rect, GUI_RGB2RGB565(108, 181, 255));
				
				player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(win_id);
				if(player_info->is_local_play)
				{
					local_info = Listening_GetLocalDataInfo();
					length = strlen(local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name);
					GUI_UTF8ToWstr(title_wchar, 50, local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name, length);
				}else
				{
					length = strlen(album_info->item_info[player_info->audio_index].audio_name) + 1;
					GUI_UTF8ToWstr(title_wchar, 50, album_info->item_info[player_info->audio_index].audio_name, length);
				}
				text_string.wstr_ptr = title_wchar;
				text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
				line_num = GUI_CalculateStringLinesByPixelNum(MMI_MAINSCREEN_WIDTH,text_string.wstr_ptr,text_string.wstr_len,DP_FONT_20,0,TRUE);
				if(line_num > 1){
				    text_style.font = DP_FONT_16;
				}
				GUISTR_DrawTextToLCDInRect(
				    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
				    &listen_title_rect,
				    &listen_title_rect,
				    &text_string,
				    &text_style,
				    text_state,
				    GUISTR_TEXT_DIR_AUTO
				);
				#ifdef LYRIC_PARSER_SUPPORT
				if(listening_lrc_p != NULL)
				{
				    ListeningPlayerLrcWin_ShowLrcText(win_id, TRUE);
				}
				else
				{
				    char file_str[LIST_ITEM_PATH_SIZE_MAX] = {0};
				    int module_id = 0;
				    int album_id = 0;
				    int audio_id = 0;
				    if(player_info->is_local_play)
				    {
				        LISTEING_LOCAL_INFO * local_info = Listening_GetLocalDataInfo();			        
				        module_id = local_info->module_info[player_info->moudle_index].module_id;
				        album_id = local_info->module_info[player_info->moudle_index].album_info[0].album_id;
				        audio_id = local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_id;
				        Listening_GetLrcFileName(file_str, module_id, album_id, audio_id);
				        if(zmt_file_exist(file_str)){
				            ListeningPlayerLrcWin_LrcParser(win_id, file_str);
				        }else
				        {
				            listening_all_lrc_download = TRUE;
    				            Listening_RequestDownloadAudioLrc(module_id,album_id,audio_id,
    				                local_info->module_info[player_info->moudle_index].album_info[0].audio_info[player_info->audio_index].audio_name
    				            );
				        }
				    }
				    else
				    {
				        module_id = album_info->module_id;
				        album_id = album_info->album_id;
				        audio_id = album_info->item_info[player_info->audio_index].audio_id;
				        Listening_GetLrcFileName(file_str, module_id, album_id, audio_id);
				        if(zmt_file_exist(file_str))
				        {
				            ListeningPlayerLrcWin_LrcParser(win_id, file_str);
				        }
				        else
				        {
				            listening_all_lrc_download = TRUE;
				            Listening_RequestDownloadAudioLrc(module_id,album_id,audio_id,
    				                album_info->item_info[player_info->audio_index].audio_name
    				            );
				        }
				    }
				}
				#endif
			}
			break;
            #ifdef LYRIC_PARSER_SUPPORT
		case MSG_TIMER:
			{
			    if(MMK_IsFocusWin(win_id)){
			        if(listening_lrc_p != NULL)
			        {
			            listening_all_lrc_timers++;
			            ListeningPlayerLrcWin_ShowLrcText(win_id, FALSE);
			        }
			    }
			}
			break;
            #endif
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
				}
				else if(tp_offset_y < -40)
				{
					MMK_PostMsg(win_id, MSG_KEYREPEAT_DOWN, PNULL, 0);
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
		case MSG_KEYDOWN_CANCEL:
                    break;
		case MSG_KEYUP_RED:
		case MSG_KEYUP_CANCEL:
			{
				MMK_CloseWin(win_id);
			}
			break;
		case MSG_CLOSE_WINDOW:
			{
				listening_player_lrc_page = 0;
				g_lrc_all_index = 0;
				MMK_FreeWinAddData(win_id);
				if(listening_all_lrc_timer_id != 0){
				    MMK_StopTimer(listening_all_lrc_timer_id);
				    listening_all_lrc_timer_id = 0;
				}
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

PUBLIC void MMIZMT_CloseListeningPlayer(void)
{
    Listening_StopPlayMp3();
}


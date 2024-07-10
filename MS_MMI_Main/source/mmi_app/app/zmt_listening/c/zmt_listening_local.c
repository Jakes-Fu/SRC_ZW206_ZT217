
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
#include "mmisrvrecord_export.h"
#include "mmirecord_export.h"
#include "mmiphone_export.h"
#include "cjson.h"
#include "zmt_listening_export.h"
#include "zmt_listening_id.h"
#include "zmt_listening_image.h"
#include "zmt_listening_text.h"
#include "zmt_listening_nv.h"


GUI_RECT_T listen_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
GUI_RECT_T listen_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, LISTEN_LINE_HIGHT};
GUI_RECT_T listen_bottom_rect = {0, MMI_MAINSCREEN_HEIGHT-LISTEN_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
GUI_RECT_T listen_dir_rect = {4*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-LISTEN_LINE_HIGHT-10, 5*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT};
GUI_RECT_T listen_list_rect = {0, 2*LISTEN_LINE_HIGHT+5, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT - LISTEN_LINE_HIGHT};
LOCAL GUI_RECT_T listen_del_all_rect = {0.5*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-0.8*LISTEN_LINE_HIGHT, 1.5*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T listen_del_rect = {2*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-0.8*LISTEN_LINE_HIGHT, 3*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T listen_del_back_rect = {3.5*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT-0.8*LISTEN_LINE_HIGHT, 5*LISTEN_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT};

extern char * listening_downloading_audio_path;
extern LISTENING_LIST_INFO * listening_info;

LISTEING_LOCAL_DELETE_INFO delete_info = {0};

typedef struct
{
	char name[100];
}LISTEING_LOCAL_AUDIO_NAME;

LOCAL void ListeningLocal_DownloadDataInit(void)
{
	uint8 i = 0;
	uint8 j = 0;
	uint8 k = 0;
	char file_path[50] = {0};
	char * out;
	cJSON * root;
	cJSON * module_list;
	cJSON * module_count;

	LISTEING_LOCAL_AUDIO_NAME local_album_name[2] = 
	{
		"词汇", "语文_直击中考",
	};

	LISTEING_LOCAL_AUDIO_NAME local_audio_name[2][4] = 
	{
		{"dog", "lamp", "deer", "panda"},
		{"1、字音题专项突破", "2、字形题专项突破",
			"3、近义词的辨析和应用", "4、成语的理解和运用题型精讲"}
	};

	int local_module_id[2] = {6, 16};
	int local_album_id[2] = {11, 19};
	int local_audio_id[2][4] = {{395, 396, 397, 398},{868, 869, 870, 871}};

	strcpy(file_path, LISTENING_FILE_INFO_PATH);
	/*if(zmt_file_exist(file_path))
	{
		return;
	}*/
	
	root = cJSON_CreateObject();
	module_list = cJSON_CreateArray();

	module_count = cJSON_CreateNumber(2);
	cJSON_AddItemToObject(root, "count", module_count);
	cJSON_AddItemToObject(root, "module", module_list);
	for(i = 0; i < 2; i++)
	{
		cJSON * module_root;
		cJSON * mudule_id;
		cJSON * album_count;
		cJSON * album_list;
				
		module_root = cJSON_CreateObject();
		cJSON_AddItemToArray(module_list, module_root);

		mudule_id = cJSON_CreateNumber(local_module_id[i]);
		cJSON_AddItemToObject(module_root, "module_id", mudule_id);
		SCI_TRACE_LOW("%s: module_id = %d", __FUNCTION__, mudule_id->valueint);
	
		album_count = cJSON_CreateNumber(1);
		cJSON_AddItemToObject(module_root, "album_count", album_count);

		album_list = cJSON_CreateArray();
		cJSON_AddItemToObject(module_root, "album_list", album_list);
		for(j = 0; j < 1; j++)
		{
			cJSON * album_root;
			cJSON * album_id;
			cJSON * album_name;
			cJSON * audio_count;
			cJSON * audio_list;

			album_root = cJSON_CreateObject();
			cJSON_AddItemToArray(album_list, album_root);
					
			album_id = cJSON_CreateNumber(local_album_id[i]);
			cJSON_AddItemToObject(album_root, "album_id", album_id);

			album_name = cJSON_CreateString(local_album_name[i].name);
			cJSON_AddItemToObject(album_root, "album_name", album_name);

			audio_count = cJSON_CreateNumber(4);
			cJSON_AddItemToObject(album_root, "audio_count", audio_count);
			
			audio_list = cJSON_CreateArray();
			cJSON_AddItemToObject(album_root, "audio_list", audio_list);
			for(k = 0; k < 4; k++)
			{
				cJSON * audio_root;
				cJSON * audio_id;
				cJSON * audio_name;

				audio_root = cJSON_CreateObject();
				cJSON_AddItemToArray(audio_list, audio_root);
						
				audio_id = cJSON_CreateNumber(local_audio_id[i][k]);
				cJSON_AddItemToObject(audio_root, "audio_id", audio_id);
						
				audio_name = cJSON_CreateString(local_audio_name[i][k].name);
				cJSON_AddItemToObject(audio_root, "audio_name", audio_name);
			}
		}
	}
	out = cJSON_Print(root);

	if(zmt_file_exist(file_path))
	{
		zmt_file_delete(file_path);
	}
	zmt_file_data_write(out, strlen(out), file_path);
	SCI_FREE(out);
	cJSON_Delete(root);
	//Listening_FreeLocalDataInfo();
}

LOCAL void Listening_InitButton(MMI_CTRL_ID_T ctrl_id, GUI_RECT_T rect, MMI_TEXT_ID_T text_id, GUI_ALIGN_E text_align, BOOLEAN visable, GUIBUTTON_CALLBACK_FUNC func)
{
    GUIBUTTON_SetRect(ctrl_id, &rect);
    GUIBUTTON_SetTextAlign(ctrl_id, text_align);
    GUIBUTTON_SetVisible(ctrl_id, visable, visable);
    if(func != NULL){
        GUIBUTTON_SetCallBackFunc(ctrl_id, func);
    }
    if(text_id != NULL){
        GUIBUTTON_SetTextId(ctrl_id, text_id);
    }
}


///////////////////////////////////////////////////////////////////////////////////////
LOCAL void ListeningLocalAudioWin_ButtonDeleteCallback(void)
{
    if(!delete_info.is_select_delete)
    {
        delete_info.is_select_delete = TRUE;
        MMK_SendMsg(LISTENING_LOCAL_AUDIO_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
    else
    {
        uint8 i = 0;
        BOOLEAN delete_module = FALSE;
        uint8 delete_count = 0;
        LISTEING_LOCAL_INFO * local_info = NULL;
        uint8 id_index = 0;
        local_info = Listening_GetLocalDataInfo();
        id_index = (uint8) MMK_GetWinAddDataPtr(LISTENING_LOCAL_AUDIO_WIN_ID);
        for(i = 0;i < local_info->module_info[id_index].album_info[0].audio_count;i++)
        {
            if(delete_info.select_info[i].is_select)
            {
                delete_count++;
            }
        }
        SCI_TRACE_LOW("%s: delete_count = %d", __FUNCTION__, delete_count);
        if(delete_count == 0){
            return;
        }
        if(local_info->module_info[id_index].album_info[0].audio_count == delete_count)
        {
            delete_module = TRUE;
        }
        SCI_TRACE_LOW("%s: delete_module = %d", __FUNCTION__, delete_module);
        if(delete_module)
        {
            Listening_DeleteOneAlbum(local_info->module_info[id_index].module_id);
            memset(&delete_info, 0, sizeof(LISTEING_LOCAL_DELETE_INFO));
            MMK_CloseWin(LISTENING_LOCAL_AUDIO_WIN_ID);
        }
        else
        {
            for(i = 0;i < local_info->module_info[id_index].album_info[0].audio_count;i++)
            {
                if(delete_info.select_info[i].is_select){
                    Listening_DeleteOneAudio(local_info->module_info[id_index].module_id, local_info->module_info[id_index].album_info[0].audio_info[i].audio_id);
                }
            }
            memset(&delete_info, 0, sizeof(LISTEING_LOCAL_DELETE_INFO));
            MMK_SendMsg(LISTENING_LOCAL_AUDIO_WIN_ID, MSG_FULL_PAINT, PNULL);
        }
    }
}

LOCAL void ListeningLocalAudioWin_ButtonAllCallback(void)
{
    uint8 index = 0;
    if(!delete_info.is_select_delete){
        return;
    }
    for(index = 0;index < listening_info->local_audio_total;index++)
    {
        if(!delete_info.select_info[index].is_select)
        {
            delete_info.is_select_all = FALSE;
            break;
        }
        else
        {
            delete_info.is_select_all = TRUE;
        }
    }
    if(delete_info.is_select_all)
    {
        delete_info.is_select_all = FALSE;
        for(index = 0;index < listening_info->local_audio_total;index++)
        {
            delete_info.select_info[index].is_select = FALSE;
        }
    }
    else
    {
        delete_info.is_select_all = TRUE;
        for(index = 0;index < listening_info->local_audio_total;index++)
        {
            delete_info.select_info[index].is_select = TRUE;
        }
    }
    MMK_SendMsg(LISTENING_LOCAL_AUDIO_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void ListeningLocalAudioWin_ButtonBackCallback(void)
{
    uint8 index = 0;
    if(!delete_info.is_select_delete){
        return;
    }
    delete_info.is_select_delete = FALSE;
    for(index = 0;index < listening_info->local_audio_total;index++)
    {
        delete_info.select_info[index].is_select = FALSE;
    }
    MMK_SendMsg(LISTENING_LOCAL_AUDIO_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void ListeningLocalAudioWin_InitBottom(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T button_rect = listen_list_rect;
    GUI_BG_T bg = {0};
    bg.bg_type = GUI_BG_IMG;

    button_rect.top = button_rect.bottom;
    button_rect.bottom = MMI_MAINSCREEN_HEIGHT;
    button_rect.left = 0;
    button_rect.right = 2*LISTEN_LINE_WIDTH;
    Listening_InitButton(LISTENING_LOCAL_AUDIO_BUTTON_DELETE_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, TRUE, ListeningLocalAudioWin_ButtonDeleteCallback);
    bg.img_id = ZMT_LISTEN_DELECT;
    GUIBUTTON_SetBg(LISTENING_LOCAL_AUDIO_BUTTON_DELETE_CTRL_ID, &bg);
    button_rect.left = button_rect.right;
    button_rect.right += 2*LISTEN_LINE_WIDTH;
    Listening_InitButton(LISTENING_LOCAL_AUDIO_BUTTON_ALL_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, FALSE, ListeningLocalAudioWin_ButtonAllCallback);
    bg.img_id = ZMT_LISTEN_SECLET_ALL;
    GUIBUTTON_SetBg(LISTENING_LOCAL_AUDIO_BUTTON_ALL_CTRL_ID, &bg);
    button_rect.left = button_rect.right;
    button_rect.right += 2*LISTEN_LINE_WIDTH;
    Listening_InitButton(LISTENING_LOCAL_AUDIO_BUTTON_BACK_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, FALSE, ListeningLocalAudioWin_ButtonBackCallback);
    bg.img_id = ZMT_LISTEN_BACK;
    GUIBUTTON_SetBg(LISTENING_LOCAL_AUDIO_BUTTON_BACK_CTRL_ID, &bg);
}

LOCAL void ListeningLocalAudioWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    LISTEING_LOCAL_INFO * local_info = NULL;
    uint8 id_index = 0;
    
    memset(&delete_info, 0, sizeof(LISTEING_LOCAL_DELETE_INFO));
    local_info = Listening_GetLocalDataInfo();
    id_index = (uint8) MMK_GetWinAddDataPtr(win_id);
    listening_info->local_audio_total = local_info->module_info[id_index].album_info[0].audio_count;
    SCI_TRACE_LOW("%s: local_audio_total = %d", __FUNCTION__, listening_info->local_audio_total);

    ListeningLocalAudioWin_InitBottom(win_id);
}

LOCAL void ListeningLocalAudioWin_DisplayLocalAudioList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, uint8 id_index)
{
	uint16 index = 0;
	LISTEING_LOCAL_INFO * local_info = NULL;
	GUILIST_INIT_DATA_T list_init = {0};
	GUILIST_ITEM_T item_t = {0};
	GUIITEM_STATE_T item_state = {0};
	GUILIST_ITEM_DATA_T item_data = {0};
	GUI_COLOR_T list_color = {0};
	MMI_STRING_T text_str = {0};
	wchar name_wchar[50] = {0};
	char name_str[50] = {0};
	uint8 length = 0;

	local_info = Listening_GetLocalDataInfo();
	SCI_TRACE_LOW("%s: audio_count = %d", __FUNCTION__, local_info->module_info[id_index].album_info[0].audio_count);
	if(local_info->module_info[id_index].album_info[0].audio_count <= 0){
		return;
	}

	list_init.both_rect.v_rect = listen_list_rect;
	list_init.both_rect.v_rect.top -= LISTEN_LINE_HIGHT;
	list_init.type = GUILIST_TEXTLIST_E;
	GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

	MMK_SetAtvCtrl(win_id, ctrl_id);
	GUILIST_RemoveAllItems(ctrl_id);
	GUILIST_SetMaxItem(ctrl_id, local_info->module_info[id_index].album_info[0].audio_count, FALSE);

	for(index = 0; index < local_info->module_info[id_index].album_info[0].audio_count; index++)
	{
		char file_str[LIST_ITEM_PATH_SIZE_MAX] = {0};
		length = strlen(local_info->module_info[id_index].album_info[0].audio_info[index].audio_name);
		//SCI_TRACE_LOW("%s: length = %d, audio_name = %s",
		//	__FUNCTION__, length, local_info->module_info[id_index].album_info[0].audio_info[index].audio_name);
		if(length == 0) continue;
		Listening_GetFileName(file_str, local_info->module_info[id_index].module_id, 
		local_info->module_info[id_index].album_info[0].album_id, 
		local_info->module_info[id_index].album_info[0].audio_info[index].audio_id);
		//if(!zmt_file_exist(file_str))continue;
		length += 1;
		item_t.item_style = GUIITEM_STYLE_ONE_ICON_AND_ONE_TEXT_MS;
		item_t.item_data_ptr = &item_data;
		item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
		
		memset(name_wchar, 0, 50);
		memset(name_str, 0, 50);

		//name item
        #ifdef WIN32
		GUI_GBToWstr(name_str, local_info->module_info[id_index].album_info[0].audio_info[index].audio_name, length);
        #else
		GUI_UTF8ToWstr(name_str, length, local_info->module_info[id_index].album_info[0].audio_info[index].audio_name, length);
        #endif
		text_str.wstr_ptr = name_str;
		text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
		item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
		item_data.item_content[0].item_data.text_buffer = text_str;

		//icon arrow
		item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
		if(!delete_info.is_select_delete)
		{
			//SCI_TRACE_LOW("%s: listening_downloading_audio_path = %s", __FUNCTION__, listening_downloading_audio_path);
			if(listening_downloading_audio_path != NULL &&
				0 == strcmp(listening_downloading_audio_path, file_str))
			{
				item_data.item_content[1].item_data.image_id = ZMT_LISTEN_DOWNLOADING;
			}else
			{
				item_data.item_content[1].item_data.image_id = NULL;
			}
		}
		else
		{
			if(delete_info.select_info[index].is_select == 0)
			{
				item_data.item_content[1].item_data.image_id = ZMT_LISTEN_UNSELECT;
			}
			else
			{
				item_data.item_content[1].item_data.image_id = ZMT_LISTEN_SELECT;
			}
		}

		//不画分割线
		GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
		//不画高亮条
		GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

		GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

		GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));
		GUILIST_SetTextFont(ctrl_id, DP_FONT_22, MMI_WHITE_COLOR);

		GUILIST_AppendItem(ctrl_id, &item_t);
	}
}

LOCAL void ListeningLocalAudioWin_SelectOpenAudio(uint8 moudle_index, uint8 audio_idx)
{
	LISTEING_LOCAL_INFO * local_info = NULL;
	char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
	int module_id = 0;
	int album_id = 0;
	int audio_id = 0;
	
	SCI_TRACE_LOW("%s: moudle_index = %d, audio_idx = %d", __FUNCTION__, moudle_index, audio_idx);

	local_info = Listening_GetLocalDataInfo();
	module_id = local_info->module_info[moudle_index].module_id;
	SCI_TRACE_LOW("%s: module_id = %d", __FUNCTION__, module_id);
	album_id = local_info->module_info[moudle_index].album_info[0].album_id;
	SCI_TRACE_LOW("%s: album_id = %d", __FUNCTION__, album_id);
	audio_id = local_info->module_info[moudle_index].album_info[0].audio_info[audio_idx].audio_id;
	SCI_TRACE_LOW("%s: audio_id = %d", __FUNCTION__, audio_id);
	//SCI_TRACE_LOW("%s: module_id = %d, album_id = %d, audio_id = %d", module_id, album_id, audio_id);
	if(Listening_PlayMp3(module_id, album_id, audio_id))
	{
		LISTENING_PLAYER_INFO * player_info = NULL;
		player_info = (LISTENING_PLAYER_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
		player_info->is_local_play = TRUE;
		player_info->moudle_index = moudle_index;
		player_info->audio_index = audio_idx;
		MMI_CreateListeningPlayerWin(player_info);
	}
}

LOCAL void ListeningLocalAudioWin_DisplayListAndDir(MMI_WIN_ID_T win_id, int id_index)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T ctrl_id = LISTENING_LISTBOX_LOCAL_AUDIO_CTRL_ID;

	ListeningLocalAudioWin_DisplayLocalAudioList(win_id, ctrl_id, id_index);

	SCI_TRACE_LOW("%s: is_select_delete = %d", __FUNCTION__, delete_info.is_select_delete);
	if(delete_info.is_select_delete)
	{
		GUIBUTTON_SetVisible(LISTENING_LOCAL_AUDIO_BUTTON_DELETE_CTRL_ID, TRUE, TRUE);
		GUIBUTTON_SetVisible(LISTENING_LOCAL_AUDIO_BUTTON_ALL_CTRL_ID, TRUE, TRUE);
		GUIBUTTON_SetVisible(LISTENING_LOCAL_AUDIO_BUTTON_BACK_CTRL_ID, TRUE, TRUE);
	}
	else
	{
		GUIBUTTON_SetVisible(LISTENING_LOCAL_AUDIO_BUTTON_DELETE_CTRL_ID, TRUE, TRUE);
		GUIBUTTON_SetVisible(LISTENING_LOCAL_AUDIO_BUTTON_ALL_CTRL_ID, FALSE, FALSE);
		GUIBUTTON_SetVisible(LISTENING_LOCAL_AUDIO_BUTTON_BACK_CTRL_ID, FALSE, FALSE);
	}
}

LOCAL MMI_RESULT_E HandleListeningLocalAudioWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T ctrl_id = LISTENING_LISTBOX_LOCAL_AUDIO_CTRL_ID;
	GUI_POINT_T point = {0};
	uint8 id_index = 0;
	LISTEING_LOCAL_INFO * local_info = NULL;
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
				ListeningLocalAudioWin_OPEN_WINDOW(win_id);
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar title_wchar[50] = {0};
				uint8 length = 0;
				uint8 num = 0;
				char num_buf[10] = {0};
				wchar num_str[10] = {0};
				uint8 total_page = 0;

				GUI_FillRect(&lcd_dev_info, listen_win_rect, GUI_RGB2RGB565(80, 162, 254));
				GUI_FillRect(&lcd_dev_info, listen_title_rect, GUI_RGB2RGB565(108, 181, 255));
				GUI_FillRect(&lcd_dev_info, listen_bottom_rect, GUI_RGB2RGB565(255, 255, 255));				

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_24;
				text_style.font_color = MMI_WHITE_COLOR;

				local_info = Listening_GetLocalDataInfo();
				id_index = (uint8) MMK_GetWinAddDataPtr(win_id);
				SCI_TRACE_LOW("MSG_FULL_PAINT: id_index = %d", id_index);
				length = strlen(local_info->module_info[id_index].album_info[0].album_name) + 1;
				#ifdef WIN32
				GUI_GBToWstr(title_wchar, local_info->module_info[id_index].album_info[0].album_name, length);
				#else
				GUI_UTF8ToWstr(title_wchar, length, local_info->module_info[id_index].album_info[0].album_name, length);
				#endif
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
				
				if(listening_info->local_audio_total > 0)
				{
					ListeningLocalAudioWin_DisplayListAndDir(win_id, id_index);
				}
			}
			break;
		case MSG_APP_OK:
		case MSG_APP_WEB:
		case MSG_CTL_MIDSK:
		case MSG_CTL_OK:
		case MSG_CTL_PENOK:
			{
				uint16 index = GUILIST_GetCurItemIndex(ctrl_id);
				id_index = (uint8) MMK_GetWinAddDataPtr(win_id);
				SCI_TRACE_LOW("%s: delete_info.is_select_delete = %d", __FUNCTION__, delete_info.is_select_delete);
				if(!delete_info.is_select_delete)
				{
					listening_info->local_audio_cur = 0;
					#ifdef WIN32
						MMI_TestToOpenPlayerWin();
					#else
						ListeningLocalAudioWin_SelectOpenAudio(id_index, index);
					#endif
				}
				else
				{
					if(delete_info.select_info[index].is_select)
					{
						delete_info.select_info[index].is_select = FALSE;
						delete_info.is_select_all = FALSE;
					}
					else
					{
						delete_info.select_info[index].is_select = TRUE;
					}
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}
			}
			break;
		case MSG_LIST_PRE_PAGE:
			{
				if(listening_info->local_audio_cur != 0)
				{
					listening_info->local_audio_cur--;
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}
			}
			break;
		case MSG_LIST_NEXT_PAGE:
			{	
				if(listening_info->local_audio_cur < listening_info->local_audio_total / (AUDIO_LIST_SHOW_ITEM_MAX + 1))
				{
					listening_info->local_audio_cur++;
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
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
				memset(&delete_info, 0, sizeof(LISTEING_LOCAL_DELETE_INFO));
			}
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
		}
	return recode;
}

WINDOW_TABLE(MMI_LOCAL_AUDIO_WIN_TAB) = 
{
	WIN_ID(LISTENING_LOCAL_AUDIO_WIN_ID),
	WIN_FUNC((uint32)HandleListeningLocalAudioWinMsg),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_LOCAL_AUDIO_BUTTON_DELETE_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_LOCAL_AUDIO_BUTTON_ALL_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_LOCAL_AUDIO_BUTTON_BACK_CTRL_ID),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMI_CreateListeningLocalAudioWin(uint8 index)
{
	MMI_WIN_ID_T win_id = LISTENING_LOCAL_AUDIO_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}

	win_handle = MMK_CreateWin((uint32*)MMI_LOCAL_AUDIO_WIN_TAB, (ADD_DATA)index);
	MMK_SetWinRect(win_handle, &rect);
}

/////////////////////////////////////////////////////////////////////////////////////////
LOCAL void ListeningLocalWin_ButtonDeleteCallback(void)
{
    uint8 i = 0;
    uint8 delete_count = 0;
    int module_id[50] = {0};
    LISTEING_LOCAL_INFO * local_info = NULL;
    if(!delete_info.is_select_delete)
    {
        delete_info.is_select_delete = TRUE;
    }
    else
    {
        local_info = Listening_GetLocalDataInfo();
        for(i = 0;i < listening_info->local_album_total;i++)
        {
            if(delete_info.select_info[i].is_select)
            {
                module_id[delete_count] = local_info->module_info[i].module_id;
                delete_count++;
            }
        }
        SCI_TRACE_LOW("%s: delete_count = %d", __FUNCTION__, delete_count);
        if(delete_count == 0) {
            return;
        }
        for(i = 0;i < delete_count;i++)
        {
            Listening_DeleteOneAlbum(module_id[i]);
        }
        memset(&delete_info, 0, sizeof(LISTEING_LOCAL_DELETE_INFO));
    }
    MMK_SendMsg(LISTENING_LOCAL_ALBUM_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void ListeningLocalWin_ButtonAllCallback(void)
{
    uint8 index = 0;
    if(!delete_info.is_select_delete){
        return;
    }
    if(listening_info == NULL){
        return;
    }
    for(index = 0;index < listening_info->local_album_total;index++)
    {
        if(!delete_info.select_info[index].is_select)
        {
            delete_info.is_select_all = FALSE;
            break;
        }
        else
        {
            delete_info.is_select_all = TRUE;
        }
    }
    if(delete_info.is_select_all)
    {
        delete_info.is_select_all = FALSE;
        for(index = 0;index < listening_info->local_album_total;index++)
        {
            delete_info.select_info[index].is_select = FALSE;
        }
    }
    else
    {
        delete_info.is_select_all = TRUE;
        for(index = 0;index < listening_info->local_album_total;index++)
        {
            delete_info.select_info[index].is_select = TRUE;
        }
    }
    MMK_SendMsg(LISTENING_LOCAL_ALBUM_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void ListeningLocalWin_ButtonBackCallback(void)
{
    uint8 index = 0;
    if(!delete_info.is_select_delete){
        return;
    }
    delete_info.is_select_delete = FALSE;
    for(index = 0;index < listening_info->local_album_total;index++)
    {
        delete_info.select_info[index].is_select = FALSE;
    }
    MMK_SendMsg(LISTENING_LOCAL_ALBUM_WIN_ID, MSG_FULL_PAINT, PNULL);
}

LOCAL void ListeningLocalWin_InitBottom(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T button_rect = listen_list_rect;
    GUI_BG_T bg = {0};
    bg.bg_type = GUI_BG_IMG;

    button_rect.top = button_rect.bottom;
    button_rect.bottom = MMI_MAINSCREEN_HEIGHT;
    button_rect.left = 0;
    button_rect.right = 2*LISTEN_LINE_WIDTH;
    Listening_InitButton(LISTENING_LOCAL_BUTTON_DELETE_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, TRUE, ListeningLocalWin_ButtonDeleteCallback);
    bg.img_id = ZMT_LISTEN_DELECT;
    GUIBUTTON_SetBg(LISTENING_LOCAL_BUTTON_DELETE_CTRL_ID, &bg);
    button_rect.left = button_rect.right;
    button_rect.right += 2*LISTEN_LINE_WIDTH;
    Listening_InitButton(LISTENING_LOCAL_BUTTON_ALL_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, FALSE, ListeningLocalWin_ButtonAllCallback);
    bg.img_id = ZMT_LISTEN_SECLET_ALL;
    GUIBUTTON_SetBg(LISTENING_LOCAL_BUTTON_ALL_CTRL_ID, &bg);
    button_rect.left = button_rect.right;
    button_rect.right += 2*LISTEN_LINE_WIDTH;
    Listening_InitButton(LISTENING_LOCAL_BUTTON_BACK_CTRL_ID, button_rect, NULL, ALIGN_HVMIDDLE, FALSE, ListeningLocalWin_ButtonBackCallback);
    bg.img_id = ZMT_LISTEN_BACK;
    GUIBUTTON_SetBg(LISTENING_LOCAL_BUTTON_BACK_CTRL_ID, &bg);
}

LOCAL void ListeningLocalWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    LISTEING_LOCAL_INFO * local_info = NULL;
    if(listening_info != NULL)
    {
        SCI_FREE(listening_info);
        listening_info = NULL;
    }
    listening_info = (LISTENING_LIST_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_LIST_INFO));
    SCI_MEMSET(listening_info, 0, sizeof(LISTENING_LIST_INFO));
    local_info = Listening_GetLocalDataInfo();
    listening_info->local_album_total = local_info->module_count;
    listening_info->select_cur_class = SELECT_MODULE_LOCAL;
    SCI_TRACE_LOW("%s: local_album_total = %d", __FUNCTION__, listening_info->local_album_total);
				
    GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PRE_CTRL_ID, ButtonPreClass_CallbackFunc);
    GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_PRI_CTRL_ID, ButtonPriClass_CallbackFunc);
    GUIBUTTON_SetCallBackFunc(LISTENING_BUTTON_JUR_CTRL_ID, ButtonJurClass_CallbackFunc);

    ListeningLocalWin_InitBottom(win_id);
}

PUBLIC void ListeningLocalWin_DisplayLocalAlbumList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
	uint16 index = 0;
	LISTEING_LOCAL_INFO * local_info = NULL;
	GUILIST_INIT_DATA_T list_init = {0};
	GUILIST_ITEM_T item_t = {0};
	GUIITEM_STATE_T item_state = {0};
	GUILIST_ITEM_DATA_T item_data = {0};
	GUI_COLOR_T list_color = {0};
	MMI_STRING_T text_str = {0};
	wchar name_wchar[50] = {0};
	char name_str[50] = {0};
	uint8 length = 0;
	MMI_STRING_T text_string = {0};

	list_init.both_rect.v_rect = listen_list_rect;
	list_init.type = GUILIST_TEXTLIST_E;
	GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

	MMK_SetAtvCtrl(win_id, ctrl_id);
	GUILIST_RemoveAllItems(ctrl_id);
	GUILIST_SetMaxItem(ctrl_id, listening_info->local_album_total, FALSE);

	local_info = Listening_GetLocalDataInfo();
	for(index = 0; index < listening_info->local_album_total; index++)
	{
		length = strlen(local_info->module_info[index].album_info[0].album_name);
		SCI_TRACE_LOW("%s: length = %d, album_name = %s",
			__FUNCTION__, length, local_info->module_info[index].album_info[0].album_name);
		if(length == 0) continue; 
		length += 1;
		
		item_t.item_style = GUIITEM_STYLE_TWO_ICON_AND_ONE_TEXT_MS;
		item_t.item_data_ptr = &item_data;
		item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
		
		memset(name_wchar, 0, 50);
		memset(name_str, 0, 50);

		//icon album
		item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
		item_data.item_content[0].item_data.image_id = ZMT_LISTEN_ALBUM;

		//name album
		#ifdef WIN32
		GUI_GBToWstr(name_wchar, local_info->module_info[index].album_info[0].album_name, length);
		#else
		GUI_UTF8ToWstr(name_wchar, length, local_info->module_info[index].album_info[0].album_name, length);
		#endif
		text_str.wstr_ptr = name_wchar;
		text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
		item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
		item_data.item_content[1].item_data.text_buffer = text_str;

		//icon arrow
		item_data.item_content[2].item_data_type = GUIITEM_DATA_IMAGE_ID;
		if(!delete_info.is_select_delete)
		{
			item_data.item_content[2].item_data.image_id = NULL;
		}
		else
		{
			if(delete_info.select_info[index].is_select == 0)
			{
				item_data.item_content[2].item_data.image_id = ZMT_LISTEN_UNSELECT;
			}
			else
			{
				item_data.item_content[2].item_data.image_id = ZMT_LISTEN_SELECT;
			}
		}
		
		//不画分割线
		GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
		//不画高亮条
		GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

		GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

		GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));
		GUILIST_SetTextFont(ctrl_id, DP_FONT_22, MMI_WHITE_COLOR);

		GUILIST_AppendItem(ctrl_id, &item_t);
	}
}

PUBLIC void ListeningLocalWin_DisplayListAndDir(MMI_WIN_ID_T win_id)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T ctrl_id = LISTENING_LISTBOX_LOCAL_ALBUM_CTRL_ID;
	GUI_RECT_T win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_string = {0};
	wchar title_wchar[50] = {0};
	uint8 length = 0;
	uint8 num = 0;
	char num_buf[10] = {0};
	wchar num_str[10] = {0};
	uint8 total_page = 0;
	LISTEING_LOCAL_INFO * local_info = NULL;
	
	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = DP_FONT_24;
	text_style.font_color = MMI_WHITE_COLOR;

    if(listening_info == NULL)
    {
        SCI_TRACE_LOW("%s: listening_info empty!!", __FUNCTION__);
        win_rect.top += 40;
        MMIRES_GetText(ZMT_TXT_NO_LOCAL, win_id, &text_string);
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &win_rect,
            &win_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
        return;
    }
	
	SCI_TRACE_LOW("%s: listening_info->local_album_total = %d", __FUNCTION__, listening_info->local_album_total);
	if(listening_info->local_album_total == 0 || listening_info->local_album_total > 12)
	{
		win_rect.top += 40;
		MMIRES_GetText(ZMT_TXT_NO_LOCAL, win_id, &text_string);
		GUISTR_DrawTextToLCDInRect(
			(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
			&win_rect,
			&win_rect,
			&text_string,
			&text_style,
			text_state,
			GUISTR_TEXT_DIR_AUTO
			);
            MMK_DestroyControl(ctrl_id);
	}
	else
	{
		ListeningLocalWin_DisplayLocalAlbumList(win_id, ctrl_id);

		GUI_FillRect(&lcd_dev_info, listen_bottom_rect, GUI_RGB2RGB565(255, 255, 255));

		SCI_TRACE_LOW("%s: is_select_delete = %d", __FUNCTION__, delete_info.is_select_delete);
		if(delete_info.is_select_delete)
		{
			GUIBUTTON_SetVisible(LISTENING_LOCAL_BUTTON_DELETE_CTRL_ID, TRUE, TRUE);
			GUIBUTTON_SetVisible(LISTENING_LOCAL_BUTTON_ALL_CTRL_ID, TRUE, TRUE);
			GUIBUTTON_SetVisible(LISTENING_LOCAL_BUTTON_BACK_CTRL_ID, TRUE, TRUE);
		}
		else
		{
			GUIBUTTON_SetVisible(LISTENING_LOCAL_BUTTON_DELETE_CTRL_ID, TRUE, TRUE);
			GUIBUTTON_SetVisible(LISTENING_LOCAL_BUTTON_ALL_CTRL_ID, FALSE, FALSE);
			GUIBUTTON_SetVisible(LISTENING_LOCAL_BUTTON_BACK_CTRL_ID, FALSE, FALSE);
		}
	}
}

LOCAL MMI_RESULT_E HandleListeningLocalWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
	MMI_CTRL_ID_T ctrl_id = LISTENING_LISTBOX_LOCAL_ALBUM_CTRL_ID;
	LISTEING_LOCAL_INFO * local_info = NULL;
	GUI_POINT_T point = {0};
	uint8 id_index = 0;
	MMI_CheckAllocatedMemInfo();
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
			{
              		ListeningLocalWin_OPEN_WINDOW(win_id);
			}
			break;
		case MSG_FULL_PAINT:
			{
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				wchar title_wchar[50] = {0};
				uint8 length = 0;
				uint8 num = 0;
				char num_buf[10] = {0};
				wchar num_str[10] = {0};
				uint8 total_page = 0;

				GUI_FillRect(&lcd_dev_info, listen_win_rect, GUI_RGB2RGB565(80, 162, 254));
				GUI_FillRect(&lcd_dev_info, listen_title_rect, GUI_RGB2RGB565(108, 181, 255));

				text_style.align = ALIGN_HVMIDDLE;
				text_style.font = DP_FONT_24;
				text_style.font_color = MMI_WHITE_COLOR;
				MMIRES_GetText(ZMT_TXT_LISTENING, win_id, &text_string);
				GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
					&listen_title_rect,
					&listen_title_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);

				ListeningWin_DisplaySecletedClass(win_id, lcd_dev_info);

				ListeningLocalWin_DisplayListAndDir(win_id);
			}
			break;
		case MSG_APP_OK:
		case MSG_APP_WEB:
		case MSG_CTL_MIDSK:
		case MSG_CTL_OK:
		case MSG_CTL_PENOK:
			{
				uint16 index = GUILIST_GetCurItemIndex(ctrl_id);
				SCI_TRACE_LOW("%s: index = %d", __FUNCTION__, index);
				if(!delete_info.is_select_delete)
				{
					listening_info->local_audio_cur = 0;
					MMI_CreateListeningLocalAudioWin(index);
				}
				else
				{
					if(delete_info.select_info[index].is_select)
					{
						delete_info.select_info[index].is_select = FALSE;
						delete_info.is_select_all = FALSE;
					}
					else
					{
						delete_info.select_info[index].is_select = TRUE;
					}
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
				}
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				point.x = MMK_GET_TP_X(param);
				point.y = MMK_GET_TP_Y(param);
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
				if(listening_info != NULL)
				{
					SCI_FREE(listening_info);
					listening_info = NULL;
				}
				Listening_StopPlayMp3();
				Listening_FreeLocalDataInfo();
				memset(&delete_info, 0 , sizeof(LISTEING_LOCAL_DELETE_INFO));
			}
			break;
		default:
                    recode = MMI_RESULT_FALSE;
			break;
		}
    return recode;
}

WINDOW_TABLE(MMI_LIST_LOCAL_WIN_TAB) = 
{
	WIN_ID(LISTENING_LOCAL_ALBUM_WIN_ID),
	WIN_FUNC((uint32)HandleListeningLocalWinMsg),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_PRE_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_PRI_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_BUTTON_JUR_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_LOCAL_BUTTON_DELETE_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_LOCAL_BUTTON_ALL_CTRL_ID),
	CREATE_BUTTON_CTRL(PNULL, LISTENING_LOCAL_BUTTON_BACK_CTRL_ID),
	WIN_HIDE_STATUS,
	END_WIN
};

PUBLIC void MMI_CreateListeningLocalWin(void)
{
	MMI_WIN_ID_T win_id = LISTENING_LOCAL_ALBUM_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}
    
#ifdef WIN32
	ListeningLocal_DownloadDataInit();
#else
	if(!zmt_tfcard_exist()){
            MMI_CreateListeningTipWin(PALYER_PLAY_NO_TFCARD_TIP);
            return;
	}
#endif
	Listening_InitLocalDataInfo();

	win_handle = MMK_CreateWin((uint32*)MMI_LIST_LOCAL_WIN_TAB, PNULL);
	MMK_SetWinRect(win_handle, &rect);
}


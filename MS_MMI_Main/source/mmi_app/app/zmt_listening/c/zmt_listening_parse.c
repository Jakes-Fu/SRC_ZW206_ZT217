
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
#include "http_api.h"
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

LISTEING_LOCAL_INFO * local_data_info = NULL;

extern BOOLEAN listening_download_fail;
extern BOOLEAN listening_download_audio;
extern uint32 request_http_listening_idx;
extern int listening_load_win;
extern LISTENING_LIST_INFO * listening_info;
extern LISTENING_MODULE_INFO * module_info;
extern LISTENING_ALBUM_INFO * album_info;
extern char * listening_player_lrc_buf;
extern LISTENING_PALYER_PLAY_INFO player_play_info;
extern char * listening_downloading_audio_path;
extern uint8 listening_downloading_index;
extern BOOLEAN listening_all_lrc_download;

PUBLIC void Listening_InsertOneAudioInfoToLocal(LISTENING_ALBUM_INFO * album_info, uint8 index)
{
	LISTEING_LOCAL_INFO * local_info = NULL;
	char * out;
	char file_path[50] = {0};

	if(zmt_tfcard_exist())
	{
		cJSON * root;
		uint8 i = 0;
		uint8 j = 0;
		uint8 m = 0;
		uint32 length = 0;
		BOOLEAN new_module = TRUE;
		
		local_info = Listening_GetLocalDataInfo();
		if(local_info == NULL)
		{
			SCI_TRACE_LOW("%s: local_info == NULL !!", __FUNCTION__);
			new_module = TRUE;
		}else{	
       		for(i = 0; i < local_info->module_count; i++)
        		{
        			//SCI_TRACE_LOW("%s: module_id = %d", __FUNCTION__, album_info->module_id);
        			//SCI_TRACE_LOW("%s: module_info[%d].module_id = %d", __FUNCTION__, i, local_info->module_info[i].module_id);

        			if(album_info->module_id == local_info->module_info[i].module_id)
        			{
        				new_module = FALSE;
        				break;
        			}
        		}
		}

		SCI_TRACE_LOW("%s: new_module = %d", __FUNCTION__, new_module);
		if(local_info != NULL)
		{
			cJSON * module_list;
			cJSON * module_count;
			SCI_TRACE_LOW("%s: local start add", __FUNCTION__);
			root = cJSON_CreateObject();
			module_list = cJSON_CreateArray();
			if(!new_module)
			{
				module_count = cJSON_CreateNumber(local_info->module_count);
			}
			else
			{
				module_count = cJSON_CreateNumber(local_info->module_count + 1);
			}

			cJSON_AddItemToObject(root, "count", module_count);
			cJSON_AddItemToObject(root, "module", module_list);
			SCI_TRACE_LOW("%s: module_count = %d", __FUNCTION__, local_info->module_count);
			for(i = 0; i < local_info->module_count; i++)
			{
				cJSON * module_root;
				cJSON * mudule_id;
				cJSON * album_count;
				cJSON * album_list;
				
				module_root = cJSON_CreateObject();
				cJSON_AddItemToArray(module_list, module_root);

				mudule_id = cJSON_CreateNumber(local_info->module_info[i].module_id);
				cJSON_AddItemToObject(module_root, "module_id", mudule_id);
				SCI_TRACE_LOW("%s: module_id = %d", __FUNCTION__, mudule_id->valueint);
	
				album_count = cJSON_CreateNumber(local_info->module_info[i].album_count);
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
					
					album_id = cJSON_CreateNumber(local_info->module_info[i].album_info[j].album_id);
					cJSON_AddItemToObject(album_root, "album_id", album_id);

					if(local_info->module_info[i].album_info[j].album_name)
					{
						album_name = cJSON_CreateString(local_info->module_info[i].album_info[j].album_name);
						cJSON_AddItemToObject(album_root, "album_name", album_name);
					}

					if(album_info->module_id == local_info->module_info[i].module_id)
					{
						audio_count = cJSON_CreateNumber(local_info->module_info[i].album_info[j].audio_count + 1);
						cJSON_AddItemToObject(album_root, "audio_count", audio_count);
					}
					else
					{
						audio_count = cJSON_CreateNumber(local_info->module_info[i].album_info[j].audio_count);
						cJSON_AddItemToObject(album_root, "audio_count", audio_count);
					}

					audio_list = cJSON_CreateArray();
					cJSON_AddItemToObject(album_root, "audio_list", audio_list);
					SCI_TRACE_LOW("%s: audio_count = %d", __FUNCTION__, local_info->module_info[i].album_info[j].audio_count);
					for(m = 0; m < local_info->module_info[i].album_info[j].audio_count; m++)
					{
						cJSON * audio_root;
						cJSON * audio_id;
						cJSON * audio_name;
						cJSON * audio_lrc;

						audio_root = cJSON_CreateObject();
						cJSON_AddItemToArray(audio_list, audio_root);
						
						audio_id = cJSON_CreateNumber(local_info->module_info[i].album_info[j].audio_info[m].audio_id);
						cJSON_AddItemToObject(audio_root, "audio_id", audio_id);

						if(local_info->module_info[i].album_info[j].audio_info[m].audio_name)
						{
							audio_name = cJSON_CreateString(local_info->module_info[i].album_info[j].audio_info[m].audio_name);
							cJSON_AddItemToObject(audio_root, "audio_name", audio_name);
						}

					}
					if(album_info->module_id == local_info->module_info[i].module_id)
					{
						cJSON * audio_root;
						cJSON * audio_id;
						cJSON * audio_name;
						cJSON * audio_lrc;

						audio_root = cJSON_CreateObject();
						cJSON_AddItemToArray(audio_list, audio_root);
						
						audio_id = cJSON_CreateNumber(album_info->item_info[index].audio_id);
						cJSON_AddItemToObject(audio_root, "audio_id", audio_id);

						if(album_info->item_info[index].audio_name)
						{
							audio_name = cJSON_CreateString(album_info->item_info[index].audio_name);
							cJSON_AddItemToObject(audio_root, "audio_name", audio_name);
						}
					}
				}
			}
			//module list
			if(new_module)
			{
				cJSON * module_root;
				cJSON * mudule_id;
				cJSON * album_count;
				cJSON * album_list;
				
				SCI_TRACE_LOW("%s: new_module add", __FUNCTION__);
				
				i++;
				module_root = cJSON_CreateObject();
				cJSON_AddItemToArray(module_list, module_root);
				
				mudule_id = cJSON_CreateNumber(album_info->module_id);
				cJSON_AddItemToObject(module_root, "module_id", mudule_id);
				SCI_TRACE_LOW("%s: module_id = %d", __FUNCTION__, mudule_id->valueint);
	
				album_count = cJSON_CreateNumber(1);
				cJSON_AddItemToObject(module_root, "album_count", album_count);

				album_list = cJSON_CreateArray();
				cJSON_AddItemToObject(module_root, "album_list", album_list);

				//album list
				{
					cJSON * album_root;
					cJSON * album_id;
					cJSON * album_name;
					cJSON * audio_count;
					cJSON * audio_list;

					album_root = cJSON_CreateObject();
					cJSON_AddItemToArray(album_list, album_root);
					
					album_id = cJSON_CreateNumber(album_info->album_id);
					cJSON_AddItemToObject(album_root, "album_id", album_id);

					if(album_info->album_name)
					{
						album_name = cJSON_CreateString(album_info->album_name);
						cJSON_AddItemToObject(album_root, "album_name", album_name);
					}
					
					audio_count = cJSON_CreateNumber(1);
					cJSON_AddItemToObject(album_root, "audio_count", audio_count);
					
					audio_list = cJSON_CreateArray();
					cJSON_AddItemToObject(album_root, "audio_list", audio_list);

					//audio list
					{
						cJSON * audio_root;
						cJSON * audio_id;
						cJSON * audio_name;
						cJSON * audio_lrc;

						audio_root = cJSON_CreateObject();
						cJSON_AddItemToArray(audio_list, audio_root);
						
						audio_id = cJSON_CreateNumber(album_info->item_info[index].audio_id);
						cJSON_AddItemToObject(audio_root, "audio_id", audio_id);

						if(album_info->item_info[index].audio_name)
						{
							audio_name = cJSON_CreateString(album_info->item_info[index].audio_name);
							cJSON_AddItemToObject(audio_root, "audio_name", audio_name);
						}

					}
				}
			}	
		}
		
		out = cJSON_Print(root);
		
		strcpy(file_path, LISTENING_FILE_INFO_PATH);
		if(zmt_file_exist(file_path))
		{
			zmt_file_delete(file_path);
		}
		zmt_file_data_write(out, strlen(out), file_path);
		
		SCI_FREE(out);
		cJSON_Delete(root);
		Listening_FreeLocalDataInfo();
		Listening_InitLocalDataInfo();
	}
	else
	{
		SCI_TRACE_LOW("%s: dsl tf crad not exist !!", __FUNCTION__);
	}
}

PUBLIC void Listening_DeleteOneAudio(int del_module_id, int del_audio_id)
{
    uint8 i = 0;
    uint8 j = 0;
    uint8 m = 0;
    uint8 k = 0;
    int del_album_id = 0;
    char * out;
    cJSON * root;
    cJSON * module_list;
    cJSON * module_count;
    char file_path[50] = {0};
    LISTEING_LOCAL_INFO * local_info;

    SCI_TRACE_LOW("%s: del_module_id = %d, del_audio_id = %d", __FUNCTION__, del_module_id, del_audio_id);
    local_info = Listening_GetLocalDataInfo();

    root = cJSON_CreateObject();
    module_list = cJSON_CreateArray();
    module_count = cJSON_CreateNumber(local_info->module_count);
    
    cJSON_AddItemToObject(root, "count", module_count);
    cJSON_AddItemToObject(root, "module", module_list);

    for(i = 0;i < local_info->module_count;i++)
    {
        cJSON * module_root;
        cJSON * mudule_id;
        cJSON * album_count;
        cJSON * album_list;

        module_root = cJSON_CreateObject();
        cJSON_AddItemToArray(module_list, module_root);

        mudule_id = cJSON_CreateNumber(local_info->module_info[i].module_id);
        cJSON_AddItemToObject(module_root, "module_id", mudule_id);
        SCI_TRACE_LOW("%s: mudule_id->valueint = %d", __FUNCTION__, mudule_id->valueint);
		
        album_count = cJSON_CreateNumber(local_info->module_info[i].album_count);
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
						
            album_id = cJSON_CreateNumber(local_info->module_info[i].album_info[j].album_id);
            cJSON_AddItemToObject(album_root, "album_id", album_id);

            if(local_info->module_info[i].album_info[j].album_name)
            {
                album_name = cJSON_CreateString(local_info->module_info[i].album_info[j].album_name);
                cJSON_AddItemToObject(album_root, "album_name", album_name);
            }

            audio_count = cJSON_CreateNumber(local_info->module_info[i].album_info[j].audio_count - 1);
            cJSON_AddItemToObject(album_root, "audio_count", audio_count);

            audio_list = cJSON_CreateArray();
            cJSON_AddItemToObject(album_root, "audio_list", audio_list);
            SCI_TRACE_LOW("%s: audio_count = %d", __FUNCTION__, local_info->module_info[i].album_info[j].audio_count);
            for(m = 0; m < local_info->module_info[i].album_info[j].audio_count; m++)
            {
                cJSON * audio_root;
                cJSON * aodio_id;
                cJSON * audio_name;
                cJSON * audio_lrc;

                if(del_module_id == local_info->module_info[i].module_id && 
                    del_audio_id == local_info->module_info[i].album_info[j].audio_info[m].audio_id){
                    del_album_id = local_info->module_info[i].album_info[j].album_id;
                    continue;
                }

                audio_root = cJSON_CreateObject();
                cJSON_AddItemToArray(audio_list, audio_root);
							
                aodio_id = cJSON_CreateNumber(local_info->module_info[i].album_info[j].audio_info[m].audio_id);
                cJSON_AddItemToObject(audio_root, "audio_id", aodio_id);

                if(local_info->module_info[i].album_info[j].audio_info[m].audio_name)
                {
                    audio_name = cJSON_CreateString(local_info->module_info[i].album_info[j].audio_info[m].audio_name);
                    cJSON_AddItemToObject(audio_root, "audio_name", audio_name);
                }
            }
        }
    }

    Listening_GetFileName(file_path, del_module_id, del_album_id, del_audio_id);
    if(zmt_file_exist(file_path)){
        zmt_file_delete(file_path);
    }

    memset(file_path, 0, 50);
    strcpy(file_path, LISTENING_FILE_INFO_PATH);
    if(zmt_file_exist(file_path)){
        zmt_file_delete(file_path);
    }
    out = cJSON_Print(root);
    zmt_file_data_write(out, strlen(out), file_path);
    SCI_FREE(out);
    cJSON_Delete(root);
    Listening_FreeLocalDataInfo();
    Listening_InitLocalDataInfo();
}

PUBLIC void Listening_DeleteOneAlbum(int del_module_id)
{
	uint8 i = 0;
	uint8 j = 0;
	uint8 m = 0;
	uint8 k = 0;
	char * out;
	cJSON * root;
	cJSON * module_list;
	cJSON * module_count;
	char file_path[50] = {0};
	LISTEING_LOCAL_INFO * local_info;

	SCI_TRACE_LOW("%s: del_module_id = %d", __FUNCTION__, del_module_id);
	local_info = Listening_GetLocalDataInfo();
	{	
		root = cJSON_CreateObject();
		module_list = cJSON_CreateArray();
		module_count = cJSON_CreateNumber(local_info->module_count - 1);

		cJSON_AddItemToObject(root, "count", module_count);
		cJSON_AddItemToObject(root, "module", module_list);
		SCI_TRACE_LOW("%s: module_count = %d", __FUNCTION__, local_info->module_count);
		for(i = 0;i < local_info->module_count;i++)
		{
			cJSON * module_root;
			cJSON * mudule_id;
			cJSON * album_count;
			cJSON * album_list;
			
			if(local_info->module_info[i].module_id == del_module_id)
			{
				SCI_TRACE_LOW("%s: del_module_id = %d delete, not write to .json", __FUNCTION__, del_module_id);
				continue;
			}
			
			module_root = cJSON_CreateObject();
			cJSON_AddItemToArray(module_list, module_root);

			mudule_id = cJSON_CreateNumber(local_info->module_info[i].module_id);
			cJSON_AddItemToObject(module_root, "module_id", mudule_id);
			SCI_TRACE_LOW("%s: mudule_id->valueint = %d", __FUNCTION__, mudule_id->valueint);
	
			album_count = cJSON_CreateNumber(local_info->module_info[i].album_count);
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
					
				album_id = cJSON_CreateNumber(local_info->module_info[i].album_info[j].album_id);
				cJSON_AddItemToObject(album_root, "album_id", album_id);

				if(local_info->module_info[i].album_info[j].album_name)
				{
					album_name = cJSON_CreateString(local_info->module_info[i].album_info[j].album_name);
					cJSON_AddItemToObject(album_root, "album_name", album_name);
				}

				audio_count = cJSON_CreateNumber(local_info->module_info[i].album_info[j].audio_count);
				cJSON_AddItemToObject(album_root, "audio_count", audio_count);

				audio_list = cJSON_CreateArray();
				cJSON_AddItemToObject(album_root, "audio_list", audio_list);
				SCI_TRACE_LOW("%s: audio_count = %d", __FUNCTION__, local_info->module_info[i].album_info[j].audio_count);
				for(m = 0; m < local_info->module_info[i].album_info[j].audio_count; m++)
				{
					cJSON * audio_root;
					cJSON * audio_id;
					cJSON * audio_name;
					cJSON * audio_lrc;

					audio_root = cJSON_CreateObject();
					cJSON_AddItemToArray(audio_list, audio_root);
						
					audio_id = cJSON_CreateNumber(local_info->module_info[i].album_info[j].audio_info[m].audio_id);
					cJSON_AddItemToObject(audio_root, "audio_id", audio_id);

					if(local_info->module_info[i].album_info[j].audio_info[m].audio_name)
					{
						audio_name = cJSON_CreateString(local_info->module_info[i].album_info[j].audio_info[m].audio_name);
						cJSON_AddItemToObject(audio_root, "audio_name", audio_name);
					}
				}
			}
		}
	}

	for(i = 0;i < local_info->module_count;i++)
	{
		if(local_info->module_info[i].module_id == del_module_id)
		{
			SCI_TRACE_LOW("%s: del_module_id = %d delete mp3 && lrc", __FUNCTION__, del_module_id);
			for(j = 0;j < local_info->module_info[i].album_count;j++)
			{
				SCI_TRACE_LOW("%s: audio_count = %d", __FUNCTION__, local_info->module_info[i].album_info[j].audio_count);
				for(m = 0;m < local_info->module_info[i].album_info[j].audio_count;m++)
				{
					memset(file_path, 0, 50);
					Listening_GetFileName(file_path,
						local_info->module_info[i].module_id,
						local_info->module_info[i].album_info[j].album_id, 
						local_info->module_info[i].album_info[j].audio_info[m].audio_id);
					zmt_file_delete(file_path);
					
					memset(file_path, 0, 50);
					Listening_GetLrcFileName(file_path,
						local_info->module_info[i].module_id,
						local_info->module_info[i].album_info[j].album_id, 
						local_info->module_info[i].album_info[j].audio_info[m].audio_id);
					zmt_file_delete(file_path);
				}
				memset(file_path, 0, 50);
				sprintf(file_path, LISTENING_DIRECTORY_BASE_PATH, 
					local_info->module_info[i].module_id,
					local_info->module_info[i].album_info[j].album_id);
				zmt_directory_delete(file_path);
			}
		}
	}

    memset(file_path, 0, 50);
    strcpy(file_path, LISTENING_FILE_INFO_PATH);
    if(zmt_file_exist(file_path)){
        zmt_file_delete(file_path);
    }
    if(module_count->valueint != 0){
        out = cJSON_PrintUnformatted(root);
        zmt_file_data_write(out, strlen(out), file_path);
		SCI_FREE(out);
    }
    cJSON_Delete(root);
    Listening_FreeLocalDataInfo();
    Listening_InitLocalDataInfo();
}

PUBLIC void Listening_ParseAudioDownload(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);	
    request_http_listening_idx = 0;
    if(is_ok)
    {
        if((!MMK_IsOpenWin(LISTENING_AUDIO_LIST_WIN_ID) &&
                !MMK_IsOpenWin(LISTENING_LOCAL_AUDIO_WIN_ID)) || 
            MMK_IsOpenWin(LISTENING_PLAYER_WIN_ID)){
            SCI_TRACE_LOW("%s: window LISTENING_AUDIO_LIST_WIN_ID no open", __FUNCTION__);
            if(listening_downloading_audio_path != NULL){
                SCI_FREE(listening_downloading_audio_path);
                listening_downloading_audio_path = NULL;
            }
            listening_download_audio = FALSE;
            return;
        }
        album_info->item_info[listening_downloading_index].aduio_ready = 2;
        listening_download_fail = FALSE;
        SCI_TRACE_LOW("%s: module_id = %d, album_id = %d, audio_id = %d", 
                __FUNCTION__,
                album_info->module_id,
                album_info->album_id,
                album_info->item_info[listening_downloading_index].audio_id);
        Listening_InsertOneAudioInfoToLocal(album_info, listening_downloading_index);
        if(Listening_PlayMp3(
                album_info->module_id,
                album_info->album_id,
                album_info->item_info[listening_downloading_index].audio_id)
        )
        {
            LISTENING_PLAYER_INFO * player_info = (LISTENING_PLAYER_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
            player_info->is_local_play = FALSE;
            player_info->moudle_index = 0;//not use
            player_info->audio_index = listening_downloading_index;
            MMI_CreateListeningPlayerWin(player_info);
        }
    }
    else
    {
        if(1)
        {
            char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
            SCI_TRACE_LOW("%s: module_id = %d, album_id = %d, audio_id = %d", 
                __FUNCTION__,
                album_info->module_id,
                album_info->album_id,
                album_info->item_info[listening_downloading_index].audio_id);
            Listening_GetFileName(
                file_path, 
                album_info->module_id,
                album_info->album_id,
                album_info->item_info[listening_downloading_index].audio_id);
            if(zmt_file_exist(file_path))
            {
                zmt_file_delete(file_path);
            }
        }
        album_info->item_info[listening_downloading_index].aduio_ready = 0;
        if(err_id == HTTP_ERROR_FILE_NO_SPACE){
            MMI_CreateListeningTipWin(PALYER_PLAY_NO_SPACE_TIP);
        }else{
            MMI_CreateListeningTipWin(PALYER_PLAY_DOWNLOAD_FAIL_TIP);
        }
    }
    if(listening_downloading_audio_path != NULL){
        SCI_FREE(listening_downloading_audio_path);
        listening_downloading_audio_path = NULL;
    }
    listening_download_audio = FALSE;
    if(MMK_IsOpenWin(LISTENING_AUDIO_LIST_WIN_ID)){
        MMK_SendMsg(LISTENING_AUDIO_LIST_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
    if(MMK_IsOpenWin(LISTENING_LOCAL_AUDIO_WIN_ID)){
        MMK_SendMsg(LISTENING_LOCAL_AUDIO_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

PUBLIC LISTEING_LOCAL_INFO * Listening_GetLocalDataInfo(void)
{
	return local_data_info;
}

PUBLIC void Listening_FreeLocalDataInfo(void)
{
	if(local_data_info != NULL)
	{
		SCI_FREE(local_data_info);
	}
}

PUBLIC void Listening_InitLocalDataInfo(void)
{
	char file_path[50] = {0};
	char * data_buf;
	uint32 file_len;
	uint32 length = 0;
		
	strcpy(file_path, LISTENING_FILE_INFO_PATH);
	if(zmt_file_exist(file_path))
	{
		data_buf = zmt_file_data_read(file_path, &file_len);
	}
	else
	{
		SCI_TRACE_LOW("%s: file_path = %s not exist !!", __FUNCTION__, file_path);
		return;
	}
	SCI_TRACE_LOW("%s: file_len = %d", __FUNCTION__, file_len);
	if(data_buf && file_len > 10)
	{
		uint8 i = 0;
		uint8 j = 0;
		uint8 m = 0;
		cJSON *root = cJSON_Parse(data_buf);
		cJSON *mudule_count = cJSON_GetObjectItem(root, "count");
		cJSON *mudule_list = cJSON_GetObjectItem(root, "module");
		length = sizeof(LISTEING_LOCAL_INFO);
		if(local_data_info == NULL)
		{
			local_data_info = (LISTEING_LOCAL_INFO *)SCI_ALLOC_APPZ(length);
		}
		memset(local_data_info, 0, length);
		local_data_info->module_count = mudule_count->valueint;
		SCI_TRACE_LOW("%s: module_count = %d", __FUNCTION__, mudule_count->valueint);
		if(mudule_list != NULL && mudule_list->type != cJSON_NULL)
		{
			for(i = 0; i < cJSON_GetArraySize(mudule_list) && i < MODULE_LIST_ITEM_MAX ; i++)
			{
				cJSON *item = cJSON_GetArrayItem(mudule_list, i);
				cJSON *mudule_id = cJSON_GetObjectItem(item, "module_id");
				cJSON *album_count = cJSON_GetObjectItem(item, "album_count");
				cJSON *album_list = cJSON_GetObjectItem(item, "album_list");
				local_data_info->module_info[i].module_id = mudule_id->valueint;
				local_data_info->module_info[i].album_count = album_count->valueint;
				SCI_TRACE_LOW("%s: module_id = %d", __FUNCTION__, mudule_id->valueint);
				SCI_TRACE_LOW("%s: album_count = %d", __FUNCTION__, album_count->valueint);
				for(j = 0; j < cJSON_GetArraySize(album_list) && j < 1 ; j++)
				{
					cJSON *item = cJSON_GetArrayItem(album_list, j);
					cJSON *album_id = cJSON_GetObjectItem(item, "album_id");
					cJSON *album_name = cJSON_GetObjectItem(item, "album_name");
					cJSON *audio_count = cJSON_GetObjectItem(item, "audio_count");
					cJSON *audio_list = cJSON_GetObjectItem(item, "audio_list");
					local_data_info->module_info[i].album_info[j].album_id = album_id->valueint;
					local_data_info->module_info[i].album_info[j].audio_count = audio_count->valueint;
					/*if(local_data_info->module_info[i].album_info[j].album_name == NULL)
					{
						local_data_info->module_info[i].album_info[j].album_name = (char*)SCI_ALLOC_APPZ(strlen(album_name->valuestring));
					}
					memset(local_data_info->module_info[i].album_info[j].album_name, 0, strlen(album_name->valuestring));*/
					strcpy(local_data_info->module_info[i].album_info[j].album_name, album_name->valuestring);
					SCI_TRACE_LOW("%s: album_id = %d", __FUNCTION__, album_id->valueint);
					SCI_TRACE_LOW("%s: audio_count = %d", __FUNCTION__, audio_count->valueint);
					SCI_TRACE_LOW("%s: album_name = %s", __FUNCTION__, album_name->valuestring);
					for(m = 0; m < cJSON_GetArraySize(audio_list) && m < AUDIO_LIST_ITEM_MAX ; m++)
					{
						cJSON *item = cJSON_GetArrayItem(audio_list, m);
						cJSON *audio_id = cJSON_GetObjectItem(item, "audio_id");
						cJSON *audio_name = cJSON_GetObjectItem(item, "audio_name");
						cJSON *audio_lrc = cJSON_GetObjectItem(item, "audio_lrc");
						
						local_data_info->module_info[i].album_info[j].audio_info[m].audio_id = audio_id->valueint;
						
						/*if(local_data_info->module_info[i].album_info[j].audio_info[m].audio_name == NULL)
						{
							local_data_info->module_info[i].album_info[j].audio_info[m].audio_name = (char*)SCI_ALLOC_APPZ(strlen(audio_name->valuestring));
						}
						memset(local_data_info->module_info[i].album_info[j].audio_info[m].audio_name, 0, strlen(audio_name->valuestring));*/
						strcpy(local_data_info->module_info[i].album_info[j].audio_info[m].audio_name, audio_name->valuestring);
						
						//SCI_TRACE_LOW("%s: audio_id = %d", __FUNCTION__, audio_id->valueint);
						//SCI_TRACE_LOW("%s: audio_name = %s", __FUNCTION__, audio_name->valuestring);
					}
				}
			}
		}
		cJSON_Delete(root);
		SCI_FREE(data_buf);
	}
	else
	{
		length = sizeof(LISTEING_LOCAL_INFO);
		if(local_data_info == NULL)
		{
			local_data_info = (LISTEING_LOCAL_INFO *)SCI_ALLOC_APPZ(length);
			memset(local_data_info, 0, length);
		}
		local_data_info->module_count = 0;
		if(listening_info != NULL){
			memset(listening_info, 0, sizeof(LISTENING_LIST_INFO));
			listening_info->select_cur_class = SELECT_MODULE_LOCAL;
		}
	}
}

PUBLIC void Listening_ParseAudioLrcResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if(is_ok)
    {
        char file_str[50] = {0};
        uint8 lenght = 0;
        uint32 * data_size;
        char * data_buf;

        request_http_listening_idx = 0;
        Listening_GetLrcFileName(
            file_str, 
            album_info->module_id,
            album_info->album_id, 
            album_info->item_info[listening_downloading_index].audio_id);
        //if(zmt_file_exist(file_str))
        {
            LISTENING_PLAYER_INFO * player_infos = NULL;
            LISTENING_PLAYER_INFO * player_info = NULL;
            player_info = (LISTENING_PLAYER_INFO *) MMK_GetWinAddDataPtr(LISTENING_PLAYER_WIN_ID);
            player_infos = (LISTENING_PLAYER_INFO*) SCI_ALLOC_APPZ(sizeof(LISTENING_PLAYER_INFO));
            player_infos->is_local_play = FALSE;
            player_infos->moudle_index = player_info->moudle_index;
            player_infos->audio_index = player_info->audio_index;
            player_play_info.lrc_ready = 1;//success
            SCI_TRACE_LOW("%s: player_infos->is_local_play = %d", __FUNCTION__, player_infos->is_local_play);
            if(MMK_IsFocusWin(LISTENING_PLAYER_WIN_ID)){
                ListeningPlayerWin_LrcParser(LISTENING_PLAYER_WIN_ID, file_str);
            }else if(MMK_IsFocusWin(LISTENING_PLAYER_LRC_WIN_ID)){
                listening_all_lrc_download = FALSE;
                ListeningPlayerLrcWin_LrcParser(LISTENING_PLAYER_LRC_WIN_ID, file_str);
            }
        }
    }
    else
    {
        player_play_info.lrc_ready = 2;//fail
        if(err_id == HTTP_ERROR_FILE_NO_SPACE){
            MMI_CreateListeningTipWin(PALYER_PLAY_NO_SPACE_TIP);
        }else{
            MMI_CreateListeningTipWin(PALYER_PLAY_DOWNLOAD_FAIL_TIP);
        }
    }
}

PUBLIC void Listening_GetAudioLrcData(int module_id, int album_id, int audio_id)
{
	char file_str[50] = {0};
	uint8 length = 0;
	uint32 * data_size;
	char * data_buf;

	Listening_GetLrcFileName(file_str, module_id,album_id, audio_id);
	if(zmt_file_exist(file_str))
	{
		data_buf = zmt_file_data_read(file_str, data_size);
	}
			
	length = strlen(data_buf);
	if(listening_player_lrc_buf == NULL)
	{
		listening_player_lrc_buf = (char *)SCI_ALLOC_APPZ(length);
	}
	else
	{
		length = strlen(listening_player_lrc_buf);
	}
	memset(listening_player_lrc_buf, 0, length);
	strcpy(listening_player_lrc_buf, data_buf);

	SCI_TRACE_LOW("%s: lenght = %d", __FUNCTION__, length);
	SCI_TRACE_LOW("%s: listening_player_lrc_buf = %s", __FUNCTION__, listening_player_lrc_buf);

	player_play_info.lrc_ready = 1;//success

	//MMK_SendMsg(LISTENING_PLAYER_LRC_WIN_ID, MSG_FULL_PAINT, PNULL);
}

PUBLIC void Listening_ParseAlbumResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
     if(pRcv != NULL && Rcv_len > 2)
    {
        uint8 i = 0;
        uint8 count = 0;
        cJSON *root = cJSON_Parse(pRcv);
        cJSON *data = cJSON_GetObjectItem(root, "data");
        cJSON *chList = cJSON_GetObjectItem(data, "module_list");
        SCI_TRACE_LOW("%s: Rcv_len = %d", __FUNCTION__, Rcv_len);
        if(chList != NULL && chList->type != cJSON_NULL)
        {
            if(module_info != NULL)
            {
                SCI_FREE(module_info);
                module_info = NULL;
            }
            module_info = (LISTENING_MODULE_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_MODULE_INFO));
            memset(module_info, 0, sizeof(LISTENING_MODULE_INFO));
            for(i = 0; i < cJSON_GetArraySize(chList) && i < ALBUM_LIST_ITEM_MAX ; i++)
            {
                cJSON *item = cJSON_GetArrayItem(chList, i);
                cJSON *name = cJSON_GetObjectItem(item, "module_name");
                cJSON *album_id = cJSON_GetObjectItem(item, "id");
                cJSON *module_id = cJSON_GetObjectItem(item, "stage");
                if(album_id->valueint == 14) continue;
                module_info->module_id = module_id->valueint;
                //SCI_TRACE_LOW("%s: album_id->valueint = %d", __FUNCTION__, album_id->valueint);
                module_info->item_info[count].album_id = album_id->valueint;
                strcpy(module_info->item_info[count].album_name, name->valuestring);
                count++;
            }
            if(listening_info != NULL){
                listening_info->album_total_num = count;
            }
            SCI_TRACE_LOW("%s: count = %d", __FUNCTION__, count);
        }	
        cJSON_Delete(root);
        listening_load_win = 1;
    }
    else
    {
        listening_load_win = -1;
    }
     if(MMK_IsOpenWin(LISTENING_ALBUM_LIST_WIN_ID))
    {	
        MMK_SendMsg(LISTENING_ALBUM_LIST_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

PUBLIC void Listening_ParseAudioResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    if(pRcv != NULL && Rcv_len > 2)
    {
        uint8 i = 0;
        uint8 index = 0;
        uint8 count = 0;
        cJSON *root = cJSON_Parse(pRcv);
        cJSON *data = cJSON_GetObjectItem(root, "data");
        cJSON *chList = cJSON_GetObjectItem(data, "album_list");
        SCI_TRACE_LOW("%s: Rcv_len = %d", __FUNCTION__, Rcv_len);
        if(chList != NULL && chList->type != cJSON_NULL)
        {
            if(album_info != NULL)
            {
                SCI_FREE(album_info);
                album_info = NULL;
            }
            album_info = (LISTENING_ALBUM_INFO *)SCI_ALLOC_APPZ(sizeof(LISTENING_ALBUM_INFO));
            memset(album_info, 0, sizeof(LISTENING_ALBUM_INFO));
            for(index = 0; index < cJSON_GetArraySize(chList) && index < AUDIO_LIST_ITEM_MAX ; index++)
            {
                cJSON *album_list = cJSON_GetArrayItem(chList, index);
                cJSON *module_id = cJSON_GetObjectItem(album_list, "module_id");
                cJSON *album_id = cJSON_GetObjectItem(album_list, "id");
                cJSON *album_name = cJSON_GetObjectItem(album_list, "module_name");
                cJSON *audList = cJSON_GetObjectItem(album_list, "audio_list");
                album_info->module_id = module_id->valueint;
                album_info->album_id = album_id->valueint;
                strcpy(album_info->album_name, album_name->valuestring);
                for(count = 0; count < cJSON_GetArraySize(audList) && count < AUDIO_LIST_ITEM_MAX; count++)
                {
                    cJSON *audio_list = cJSON_GetArrayItem(audList, count);
                    cJSON *audio_id = cJSON_GetObjectItem(audio_list, "id");
                    cJSON *audio_duration = cJSON_GetObjectItem(audio_list, "duration");
                    cJSON *audio_name = cJSON_GetObjectItem(audio_list, "audio_name");
                    //cJSON *audio_lrc = cJSON_GetObjectItem(audio_list, "lrc_file");
                    cJSON *audio_file = cJSON_GetObjectItem(audio_list, "audio_file");
                    album_info->item_info[count].audio_id = audio_id->valueint;
                    album_info->item_info[count].audio_duration = audio_duration->valueint;
                    album_info->item_info[count].aduio_ready = 0;

                    SCI_MEMCPY(album_info->item_info[count].audio_name, audio_name->valuestring, strlen(audio_name->valuestring) + 1);
                    //SCI_MEMCPY(album_info->item_info[count].audio_lrc, audio_lrc->valuestring, strlen(audio_lrc->valuestring) + 1);
                    SCI_MEMCPY(album_info->item_info[count].audio_path, audio_file->valuestring, strlen(audio_file->valuestring) + 1);
                    /*
                    SCI_TRACE_LOW("%s: audio_path_len = %d", __FUNCTION__, strlen(album_info->item_info[count].audio_path));
                    SCI_TRACE_LOW("%s: audio_lrc_len = %d", __FUNCTION__, strlen(album_info->item_info[count].audio_lrc));
                    SCI_TRACE_LOW("%s: audio_name_len = %d", __FUNCTION__, strlen(album_info->item_info[count].audio_name));
                    SCI_TRACE_LOW("%s: strlen(audio_name->valuestring) = %d", __FUNCTION__, strlen(audio_name->valuestring));
                    */
                }
            }
            if(listening_info != NULL){
                listening_info->item_total_num = count;
            }
            SCI_TRACE_LOW("%s: count = %d", __FUNCTION__, count);
        }
        cJSON_Delete(root);
        if(count > 0){
            listening_load_win = 1;
        }else{
            listening_load_win = -2;
        }
    }
    else
    {
        listening_load_win = -1;
    }
    if(MMK_IsOpenWin(LISTENING_AUDIO_LIST_WIN_ID))
    {
        MMK_SendMsg(LISTENING_AUDIO_LIST_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}


#include<time.h>
#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmk_msg.h"
#include "mmicc_export.h"
#include "guitext.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "guistring.h"
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
#include "zmt_poetry_id.h"
#include "zmt_poetry_image.h"
#include "zmt_poetry_text.h"
#include "zmt_poetry_main.h"
#include "cafcontrol.h"
#include "os_api.h"
#include "cjson.h"
#include "types.h"
#include "mmidisplay_data.h"
#include "mmisrvrecord_export.h"
#include "mmisrvaud_api.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "zmt_listening_export.h"
#endif
#ifdef WORD_CARD_SUPPORT
#include "zmt_word_text.h"
#include "zmt_word_image.h"
#endif

LOCAL GUI_RECT_T poetry_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T poetry_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, 1.2*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_back_rect = {0.5*POETRY_LINE_WIDTH, 0, 2.5*POETRY_LINE_WIDTH, POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_list_rect = {0, 1.2*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T poetry_play_rect = {2.6*POETRY_LINE_WIDTH, 10, 4.6*POETRY_LINE_WIDTH, POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_play_text_rect = {3*POETRY_LINE_WIDTH, 0, 4*POETRY_LINE_WIDTH, 1.2*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_favorite_rect = {5*POETRY_LINE_WIDTH, 10, 6*POETRY_LINE_WIDTH, POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_tip_rect = {POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT/2-POETRY_LINE_WIDTH,MMI_MAINSCREEN_WIDTH-POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT/2+POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_page_rect = {5*POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT-0.8*POETRY_LINE_HIGHT,6*POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT-2};
LOCAL GUI_RECT_T poetry_grade_line_rect = {10,1.2*POETRY_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-10,2.4*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_grade_line_rect_array[POETRY_GRADE_LIST_SHOW_ITEM] = {0};
LOCAL GUI_RECT_T poetry_grade_icon_rect = {0.6*POETRY_LINE_WIDTH,1.5*POETRY_LINE_HIGHT,1.5*POETRY_LINE_WIDTH,2.4*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_grade_text_rect = {1.6*POETRY_LINE_WIDTH,1.2*POETRY_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-10,2.4*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_item_line_rect = {10,1.2*POETRY_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-10,2.7*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_item_line_rect_array[POETRY_ITEM_LIST_SHOW_ITEM] = {0};
LOCAL GUI_RECT_T poetry_content_title_rect = {0.2*POETRY_LINE_WIDTH, 1.8*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.2*POETRY_LINE_WIDTH, 3.3*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_content_rect = {0.2*POETRY_LINE_WIDTH, 3.5*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.2*POETRY_LINE_WIDTH, 9*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_content_other_rect = {0.2*POETRY_LINE_WIDTH, 1.5*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.2*POETRY_LINE_WIDTH, 9*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_bottom_option_rect = {0,9*POETRY_LINE_HIGHT+1,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T poetry_bottom_option_rect_array[4] = {0};

LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;
LOCAL GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
LOCAL ZMT_POETRY_SHELECT_INFO poetery_info = {0};
LOCAL ZMT_POETRY_ALL_GRADE_LIST_T grade_all_list = {0};
LOCAL uint16 poetry_cur_post_grade_id = 0;
LOCAL uint16 poetry_cur_post_poem_id = 0;
LOCAL uint16 total_poetrynum = 0;
LOCAL uint16 total_favorite_poetrynum = 0;
LOCAL uint8 grade_get_status= 0;//0是初始化，1是成功，2是list里没有东西，3是失败了,4是自动重试也失败了需要手动刷新
LOCAL uint8 favorite_get_status= 0;
LOCAL uint16 detail_get_status = 0;
LOCAL uint16 auto_play_open_close_tip = 0;//0无提示，1是开启，2是关闭
LOCAL uint16 auto_play = 1;
LOCAL uint8 autoplay_tip_timer=0;
LOCAL GUI_LCD_DEV_INFO poetry_auto_tip_layer={0};
LOCAL GUI_LCD_DEV_INFO poetry_item_auto_tip_layer={0};
LOCAL GUI_LCD_DEV_INFO poetry_detail_auto_tip_layer={0};
LOCAL int where_open_favorite = 0;//0年级列表进入,1诗词列表进入
LOCAL BOOLEAN is_open_favorite = FALSE;
LOCAL ZMT_POETRY_LIST_T favorite_infos ={0};
LOCAL ZMT_POETRY_DETAIL_T * poetry_detail_infos = NULL;
LOCAL BOOLEAN poetry_is_favorite = FALSE;
LOCAL MMISRV_HANDLE_T poetry_player_handle=PNULL;
LOCAL BOOLEAN audio_play_now = FALSE;
LOCAL BOOLEAN audio_download_now = FALSE;
LOCAL uint8 audio_play_progress = 0;
LOCAL uint8 audio_download_progress = 0;
LOCAL uint8 audio_download_retry_num = 0;
LOCAL int poetry_player_volume = 0;
LOCAL int poetry_click_btn = 2;//0 auto,1 favorite,2 list
LOCAL int poetry_detail_click_btn = 0;

LOCAL void MMI_CreatePoetryItemWin(void);
LOCAL void MMI_CreatePoetryDetailWin(void);

LOCAL void Poetry_getGradeList(uint16 gradeid) ;
LOCAL void Poetry_parseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void Poetry_GetFavoritePoetry(void);
LOCAL void Poetry_ParsePoetryFavoriteList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void Poetry_GetPoetryDetail(uint16 poem_id);
LOCAL void Poetry_ParsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void Poetry_StartPlayAudio(void);
LOCAL void Poetry_StartDownloadAudio(void);

LOCAL void Read_list_data_from_tf_and_parse(uint16 gradeid, BOOLEAN online)
{
    char file_path[80] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    sprintf(file_path,"E:/Poetry/Poetry_offline/grade_%d/list.bin",gradeid);
    poetry_cur_post_grade_id = gradeid;
    SCI_TRACE_LOW("%s: poetry_cur_post_grade_id = %d", __FUNCTION__, poetry_cur_post_grade_id);
#ifdef WIN32
    if(1)
#else
    if(!online)
#endif
    {
        if(zmt_file_exist(file_path)){
            data_buf = zmt_file_data_read(file_path, &file_len);
            if(data_buf != PNULL && file_len > 2){
                Poetry_parseGradeList(1, data_buf , file_len, gradeid);
                SCI_FREE(data_buf);
            }else{
                Poetry_getGradeList(gradeid);
            }
        }else{
            Poetry_getGradeList(gradeid);
        }
    }else{
        Poetry_getGradeList(gradeid);
    }
}

LOCAL void Read_detail_data_from_tf_and_parse(void)
{
    char file_path[80] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;

    sprintf(file_path,"E:/Poetry/Poetry_offline/%d/%d.bin",poetry_cur_post_poem_id,poetry_cur_post_poem_id);
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        if(data_buf != PNULL && file_len > 0){
            Poetry_ParsePoetryDetail(1, data_buf, file_len, poetry_cur_post_poem_id);
            SCI_FREE(data_buf);
        }else{
            Poetry_GetPoetryDetail(poetry_cur_post_poem_id);
        }
    }else{
        Poetry_GetPoetryDetail(poetry_cur_post_poem_id);
    }        
}

LOCAL void Poetry_AutoPlayTipTimeout(uint8 timer_id,uint32 param)
{
    MMI_WIN_ID_T win_id = (MMI_WIN_ID_T)param;
    if(0 != autoplay_tip_timer)
    {
        MMK_StopTimer(autoplay_tip_timer);
        autoplay_tip_timer = 0;
    }
    auto_play_open_close_tip=0;
    UILAYER_RemoveBltLayer(&poetry_auto_tip_layer);
    UILAYER_RemoveBltLayer(&poetry_item_auto_tip_layer);
    UILAYER_RemoveBltLayer(&poetry_detail_auto_tip_layer);
}

LOCAL void Poetry_AutoPlayTipTimer(MMI_WIN_ID_T win_id)
{
    if(0 != autoplay_tip_timer)
    {
        MMK_StopTimer(autoplay_tip_timer);
        autoplay_tip_timer = 0;
    }
    autoplay_tip_timer = MMK_CreateTimerCallback(2000, Poetry_AutoPlayTipTimeout,(MMI_WIN_ID_T)win_id, FALSE);
    MMK_StartTimerCallback(autoplay_tip_timer, 2000, Poetry_AutoPlayTipTimeout, (MMI_WIN_ID_T)win_id, FALSE);
}

LOCAL BOOLEAN Poetry_Play_RequestHandle( 
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
    audio_srv.volume = poetry_player_volume;

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

LOCAL void Poetry_PlayMp3Notify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = PNULL;
	BOOLEAN result = TRUE;
	if(param != PNULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		if(report_ptr != PNULL && handle == poetry_player_handle)
		{
			switch(report_ptr->report)
			{
			    case MMISRVAUD_REPORT_END:
				{			    
				    SCI_TRACE_LOW("%s: audio_play_progress = %d", __FUNCTION__, audio_play_progress);
					if(audio_play_progress < poetry_detail_infos->sentences_num){
						audio_play_progress ++;
						audio_play_now = FALSE;
						Poetry_StartPlayAudio();
					}else{
						audio_play_progress = 0;
						audio_download_progress = 0;
						audio_play_now = FALSE;
						audio_download_now = FALSE;
						SCI_TRACE_LOW("%s: play end = %d", __FUNCTION__);
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


LOCAL void Poetry_StopPlayMp3(void)
{
    if (0 != poetry_player_handle)
    {
        MMISRVAUD_Stop(poetry_player_handle);
        MMISRVMGR_Free(poetry_player_handle);
        poetry_player_handle = PNULL;
    }
}

LOCAL uint8 Poetry_StartPlayMp3(char* file_name)
{
    uint8 ret = 0;
    MMIRECORD_SRV_RESULT_E srv_result = MMIRECORD_SRV_RESULT_SUCCESS;
    MMISRV_HANDLE_T audio_handle = PNULL;
    uint16      full_path[100] = {0};
    uint16      full_path_len = 0;
    MMISRVAUD_TYPE_U    audio_data  = {0};
    uint32 poetry_volume;

    if(poetry_player_handle)
    {
        MMISRVAUD_Stop(poetry_player_handle);
        MMISRVMGR_Free(poetry_player_handle);
        poetry_player_handle = PNULL;
    }

    full_path_len = GUI_GBToWstr(full_path, (const uint8*)file_name, SCI_STRLEN(file_name));
    audio_data.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
    audio_data.ring_file.name = full_path;
    audio_data.ring_file.name_len = full_path_len;
    audio_data.ring_file.fmt  = (MMISRVAUD_RING_FMT_E)MMIAPICOM_GetMusicType(audio_data.ring_file.name, audio_data.ring_file.name_len);

    if(Poetry_Play_RequestHandle(&audio_handle, MMISRVAUD_ROUTE_NONE, &audio_data, Poetry_PlayMp3Notify))
    {
        audio_play_now = TRUE;
        poetry_player_handle = audio_handle;
        poetry_volume = MMIAPISET_GetMultimVolume();
        MMISRVAUD_SetVolume(audio_handle, poetry_volume);
        if(!MMISRVAUD_Play(audio_handle, 0))
        {     
           	MMISRVMGR_Free(poetry_player_handle);
           	poetry_player_handle = 0;
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
        poetry_player_handle = NULL;
        return ret;
    }
    return ret;
}

LOCAL void Poetry_ParsePlayAudio(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        char file_path[100] = {0};
        char poem_id[10] = {0};
        if(poetry_detail_infos->id != NULL){
            strcpy(poem_id, poetry_detail_infos->id);
            sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d.mp3",poem_id,poem_id,audio_download_progress);
            if(!zmt_tfcard_exist())
            {
			#ifdef LISTENING_PRATICE_SUPPORT
                MMI_CreateListeningTipWin(PALYER_PLAY_NO_TFCARD_TIP);
			#endif
                return;
            }
            else if(zmt_tfcard_get_free_kb() < 100 * 1024)
            {
			#ifdef LISTENING_PRATICE_SUPPORT
                MMI_CreateListeningTipWin(PALYER_PLAY_NO_SPACE_TIP);
			#endif
                return;
            }
            else
            {
                zmt_file_data_write(pRcv, Rcv_len, file_path);
            }
            audio_play_progress++;
            audio_download_retry_num = 0;
        }
        else
        {
            audio_download_retry_num++;
        }
        if(audio_download_retry_num > 3){
            audio_play_progress++;
        }
        Poetry_StartPlayAudio();
    }
}

LOCAL void Poetry_StartPlayAudio(void)
{
    char url[100] = {0};
    char file_path[100] = {0};
    char poem_id[10] = {0};

    if(!MMI_IsPoetryDetailWinOpen()){
        SCI_TRACE_LOW("%s: DetailWin no exist", __FUNCTION__);
        return;
    }
    
    if(poetry_detail_infos != NULL && poetry_detail_infos->id != NULL){
        strcpy(poem_id, poetry_detail_infos->id);
    }else{
        SCI_TRACE_LOW("%s: poem_id is null", __FUNCTION__);
        return;
    }
    sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d.mp3",poem_id,poem_id,audio_play_progress);
    if(zmt_file_exist(file_path))
    {
        Poetry_StartPlayMp3(file_path);
        //audio_download_now = FALSE;
    }
    else
    {
        if(poetry_detail_infos != NULL){
            if(audio_play_progress == 0){
                if(poetry_detail_infos->titleAudio != NULL){
                    sprintf(url,"http://%s",poetry_detail_infos->titleAudio);
                }
            }else if(audio_play_progress < poetry_detail_infos->sentences_num + 1){
                if(poetry_detail_infos->sen_audio[audio_download_progress-1] != NULL){
                    sprintf(url,"http://%s",poetry_detail_infos->sen_audio[audio_play_progress-1]->audio);
                }
            }else{
                return;
            }
            audio_download_now = TRUE;
            MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, Poetry_ParsePlayAudio);
        }
    }
}

LOCAL void Poetry_parseDownloadAudio(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        char file_path[100] = {0};
        char poem_id[10] = {0};
        if(poetry_detail_infos != NULL && poetry_detail_infos->id != NULL){
            strcpy(poem_id, poetry_detail_infos->id);
            sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d.mp3",poem_id,poem_id,audio_download_progress);
            if(!zmt_tfcard_exist())
            {
			#ifdef LISTENING_PRATICE_SUPPORT
                MMI_CreateListeningTipWin(PALYER_PLAY_NO_TFCARD_TIP);
			#endif
                return;
            }
            else if(zmt_tfcard_get_free_kb() < 100 * 1024)
            {
			#ifdef LISTENING_PRATICE_SUPPORT
                MMI_CreateListeningTipWin(PALYER_PLAY_NO_SPACE_TIP);
			#endif
                return;
            }
            else
            {
                zmt_file_data_write(pRcv, Rcv_len, file_path);
                if(audio_download_progress == 0 || !audio_play_now){
                    SCI_TRACE_LOW("%s: action play", __FUNCTION__);
                    Poetry_StartPlayAudio();
                }
            }
            audio_download_progress++;
            audio_download_retry_num = 0;
        }
    }
    else
    {
        audio_download_retry_num++;
    }
    if(audio_download_retry_num > 3){
        audio_download_progress++;
        audio_download_retry_num = 0;
    }
    Poetry_StartDownloadAudio();
}

LOCAL void Poetry_StartDownloadAudio(void)
{
    char url[100] = {0};
    char file_path[100] = {0};
    char poem_id[10] = {0};
    
    if(!MMI_IsPoetryDetailWinOpen()){
        SCI_TRACE_LOW("%s: DetailWin no exist", __FUNCTION__);
        return;
    }
    //SCI_TRACE_LOW("%s: sentences_num = %d", __FUNCTION__, poetry_detail_infos->sentences_num);
    if(poetry_detail_infos != NULL && poetry_detail_infos->id != NULL){
        strcpy(poem_id, poetry_detail_infos->id);
    }else{
        SCI_TRACE_LOW("%s: poem_id is null", __FUNCTION__);
        return;
    }
    sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d.mp3",poem_id,poem_id,audio_download_progress);
    if(zmt_file_exist(file_path))
    {        
        if(audio_download_progress < poetry_detail_infos->sentences_num + 1)
        {
            if(audio_download_progress == 0){
                audio_play_progress = 0;
                Poetry_StartPlayAudio();       
            }
            audio_download_progress++;
            audio_download_now = TRUE;
            Poetry_StartDownloadAudio();
        }
        else
        {
            audio_download_now = FALSE;
            SCI_TRACE_LOW("%s: offline download audio end", __FUNCTION__);
        }
    }
    else
    {
        if(poetry_detail_infos != NULL){
            if(audio_download_progress == 0){
                if(poetry_detail_infos->titleAudio != NULL){
                    sprintf(url,"https://%s",poetry_detail_infos->titleAudio);
                }
            }else if(audio_download_progress < poetry_detail_infos->sentences_num + 1){
                if(poetry_detail_infos->sen_audio[audio_download_progress-1] != NULL){
                    sprintf(url,"https://%s",poetry_detail_infos->sen_audio[audio_download_progress-1]->audio);
                }
            }else{
                //audio_play_progress = 0;
                //startPlayAudio();
                audio_download_now = FALSE;
                SCI_TRACE_LOW("%s: online download audio end", __FUNCTION__);
                return;
            }
            //SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
            audio_download_now = TRUE;
            MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, Poetry_parseDownloadAudio);
        }
    }
}

LOCAL void Poetry_parseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    int i = 0;
    if(!MMI_IsPoetryItemWinOpen()){
        SCI_TRACE_LOW("PoetryItemWin no open", __FUNCTION__);
        return;
    }
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        cJSON *root = cJSON_Parse(pRcv);
        cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");
        if(responseNo->valueint==10000)
        {
            cJSON *poetry_arr = cJSON_GetObjectItem(root, "data");
            if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL)
            {
                memset(&grade_all_list.all_grade[poetry_cur_post_grade_id-1], 0, sizeof(ZMT_POETRY_GRADE_LIST_T));
                for (i = 0; i < cJSON_GetArraySize(poetry_arr); i++)
                {
                    cJSON *item = cJSON_GetArrayItem(poetry_arr, i);
                    cJSON *id = cJSON_GetObjectItem(item, "PoemId");
                    cJSON *title = cJSON_GetObjectItem(item, "Title");
                    grade_all_list.all_grade[poetry_cur_post_grade_id-1].grade_info[i].id = id->valueint;
                    strncpy(grade_all_list.all_grade[poetry_cur_post_grade_id-1].grade_info[i].title, title->valuestring,strlen(title->valuestring));
                }
                if(cJSON_GetArraySize(poetry_arr) == 0){
                    grade_get_status = 2;
                    total_poetrynum = 0;
                }else{
                    grade_get_status = 1;
                    total_poetrynum = cJSON_GetArraySize(poetry_arr);
                    /*if(zmt_tfcard_exist()&& zmt_tfcard_get_free_kb() > 100 * 1024){
                        char file_path[80] = {0};
                        uint32 file_len = 0;
                        char * out = NULL;
                        sprintf(file_path,"E:/Poetry/Poetry_offline/grade_%d/list.bin",poetry_cur_post_grade_id);
                        if(zmt_file_exist(file_path)){
                            zmt_file_delete(file_path);
                        }
                        out = cJSON_PrintUnformatted(root);
                        zmt_file_data_write(out, strlen(out), file_path);
                        SCI_FREE(out);  
                    }*/
                }
            }
            else
            {
                grade_get_status = 3;
            }
        }
        else
        {        
            grade_get_status = 3;
        }
        cJSON_Delete(root);
    } 
    else 
    {  
        grade_get_status = 3;
    }
    SCI_TRACE_LOW("%s: grade_get_status = %d, total_poetrynum = %d", __FUNCTION__, grade_get_status, total_poetrynum);
    if(MMK_IsFocusWin(POETRY_ITEM_WIN_ID))
    {
        MMK_PostMsg(POETRY_ITEM_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
}

LOCAL void Poetry_getGradeList(uint16 gradeid)
{
    if(ZMTApp_GetSimIsExist()){
        char *url = SCI_ALLOCA(350 * sizeof(char));
        memset(url, 0, 350);
        poetry_cur_post_grade_id = gradeid;
        sprintf(url,"v1/card/poem?F_grade_id=%d&F_limit=30",gradeid);
        MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, Poetry_parseGradeList);
        SCI_FREE(url);
    }else{
    #ifdef LISTENING_PRATICE_SUPPORT
        MMI_CreateListeningTipWin(PALYER_PLAY_NO_SIM_TIP);
    #endif
    }
}

LOCAL void Poetry_ParsePoetryFavoriteList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        cJSON *root = cJSON_Parse(pRcv);
        cJSON *responseNo= cJSON_GetObjectItem(root, "F_responseNo");
        if(responseNo->valueint == 10000)
        {
            cJSON *poetry_arr = cJSON_GetObjectItem(root, "F_poem_list");
            if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL)
            {
                SCI_MEMSET(&favorite_infos, 0, sizeof(ZMT_POETRY_LIST_T));
                for (i = 0; i < cJSON_GetArraySize(poetry_arr)  && i < MAX_POETRY_LIST_SIZE; i++)
                {
                    cJSON *item = cJSON_GetArrayItem(poetry_arr, i);
                    cJSON *id = cJSON_GetObjectItem(item, "F_poem_id");
                    cJSON *title = cJSON_GetObjectItem(item, "F_title");
                    strncpy(favorite_infos.poetry[i].id, id->valuestring,strlen(id->valuestring));
                    strncpy(favorite_infos.poetry[i].title, title->valuestring,strlen(title->valuestring));					
                }
                if(cJSON_GetArraySize(poetry_arr) == 0)
                {
                    favorite_get_status = 5;
                    total_favorite_poetrynum = 0;
                }
                else
                {
                    favorite_get_status = 1;
                    total_favorite_poetrynum = cJSON_GetArraySize(poetry_arr);
                }
            }
            else
            {   
                favorite_get_status = 5;
                total_favorite_poetrynum = 0;
            }
        }
        else
        {
            favorite_get_status = 5;
            total_favorite_poetrynum = 0;
        }
        cJSON_Delete(root);
    } 
    else 
    {  
        favorite_get_status = 4;
        total_favorite_poetrynum = 0;
    }
    SCI_TRACE_LOW("%s: grade_get_status = %d, total_favorite_poetrynum = %d", __FUNCTION__, grade_get_status, total_favorite_poetrynum);
    if(MMK_IsFocusWin(POETRY_ITEM_WIN_ID))
    {
        MMK_SendMsg(POETRY_ITEM_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void Poetry_GetFavoritePoetry(void)
{
    char url[80] = {0};
    char file_path[80] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;

    strcpy(file_path,"E:/Poetry/Poetry_offline/favorite_list.bin");
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            Poetry_ParsePoetryFavoriteList(1, data_buf, file_len,0);
        }
    }
    else
    {
        favorite_get_status = 5;
        if(MMK_IsFocusWin(POETRY_ITEM_WIN_ID))
        {
            MMK_SendMsg(POETRY_ITEM_WIN_ID, MSG_FULL_PAINT, PNULL);
        }
        //sprintf(url, "%s", "v1/card/poems/collect");
        //MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, parsePoetryFavoriteList);
    }
}

LOCAL BOOLEAN Poetry_GetPoemIsFavorite(char * poem_id)
{
    BOOLEAN is_favorite = FALSE;
    uint16 i = 0;
    if(total_favorite_poetrynum != 0)
    {
        for(i = 0;i < total_favorite_poetrynum;i++)
        {
            if(strcmp(favorite_infos.poetry[i].id, poem_id) == 0)
            {
                is_favorite = TRUE;
                break;
            }
        }
    }
    return is_favorite;
}

LOCAL void Poetry_ChangeFavoritePoem(char * poetry_id,BOOLEAN is_add)
{
    uint16 i = 0;
    char *out = PNULL;
    cJSON *root;
    cJSON *F_poem_list;
    cJSON *F_poem_item;
    cJSON *F_poem_id;
    cJSON *F_title;
    char file_path[80] = {0};

    SCI_TRACE_LOW("%s: poetry_id = %s, is_add = %d", __FUNCTION__, poetry_id, is_add);
    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "F_card_id", BASE_DEVICE_IMEI);
    cJSON_AddNumberToObject(root, "F_responseNo", 10000);
    F_poem_list = cJSON_CreateArray();
    for(i = 0;i < total_favorite_poetrynum;i++)
    {
        if(!is_add && 0 == strcmp(poetry_id, favorite_infos.poetry[i].id))
        {
            continue;
        }
        F_poem_item = cJSON_CreateObject();
        
        F_poem_id = cJSON_CreateString(favorite_infos.poetry[i].id);
        cJSON_AddItemToObject(F_poem_item, "F_poem_id", F_poem_id);
        
        F_title = cJSON_CreateString(favorite_infos.poetry[i].title);
        cJSON_AddItemToObject(F_poem_item, "F_title", F_title);

        cJSON_AddItemToArray(F_poem_list, F_poem_item);
    }
    if(is_add)
    {
        F_poem_item = cJSON_CreateObject();
        
        F_poem_id = cJSON_CreateString(poetry_detail_infos->id);
        cJSON_AddItemToObject(F_poem_item, "F_poem_id", F_poem_id);
        
        F_title = cJSON_CreateString(poetry_detail_infos->title);
        cJSON_AddItemToObject(F_poem_item, "F_title", F_title);

        cJSON_AddItemToArray(F_poem_list, F_poem_item);
    }
    cJSON_AddItemToObject(root, "F_poem_list", F_poem_list);
    out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    SCI_TRACE_LOW("%s: out = %s", __FUNCTION__, out);
    strcpy(file_path,"E:/Poetry/Poetry_offline/favorite_list.bin");
    if(zmt_tfcard_exist()){
        if(zmt_file_exist(file_path)){
            zmt_file_delete(file_path);
        }
        zmt_file_data_write(out, strlen(out), file_path);
        Poetry_ParsePoetryFavoriteList(1, out, strlen(out), 0);
    }
    SCI_FREE(out);
}

LOCAL void Poetry_ReleasePoetryDetail(void)
{
    uint8 i = 0;
    if(poetry_detail_infos != NULL){
        if(poetry_detail_infos->id != NULL){
            SCI_FREE(poetry_detail_infos->id);
            poetry_detail_infos->id = NULL;
            //SCI_TRACE_LOW("%s: free id", __FUNCTION__);
        }
        if(poetry_detail_infos->title != NULL){
            SCI_FREE(poetry_detail_infos->title);
            poetry_detail_infos->title = NULL;
            //SCI_TRACE_LOW("%s: free title", __FUNCTION__);
        }
        if(poetry_detail_infos->title_content != NULL){
            SCI_FREE(poetry_detail_infos->title_content);
            poetry_detail_infos->title_content = NULL;
            //SCI_TRACE_LOW("%s: free title_content", __FUNCTION__);
        }      
        if(poetry_detail_infos->titleAudio != NULL){
            SCI_FREE(poetry_detail_infos->titleAudio);
            poetry_detail_infos->titleAudio = NULL;
            //SCI_TRACE_LOW("%s: free titleAudio", __FUNCTION__);
        }
        if(poetry_detail_infos->type != NULL){
            SCI_FREE(poetry_detail_infos->type);
            poetry_detail_infos->type = NULL;
            //SCI_TRACE_LOW("%s: free type", __FUNCTION__);
        }
        if(poetry_detail_infos->content != NULL){
            SCI_FREE(poetry_detail_infos->content);
            poetry_detail_infos->content = NULL;
            //SCI_TRACE_LOW("%s: free content", __FUNCTION__);
        }
        if(poetry_detail_infos->comment != NULL){
            SCI_FREE(poetry_detail_infos->comment);
            poetry_detail_infos->comment = NULL;
            //SCI_TRACE_LOW("%s: free comment", __FUNCTION__);
        }
        if(poetry_detail_infos->translation != NULL){
            SCI_FREE(poetry_detail_infos->translation);
            poetry_detail_infos->translation = NULL;
            //SCI_TRACE_LOW("%s: free translation", __FUNCTION__);
        }
        if(poetry_detail_infos->appreciation != NULL){
            SCI_FREE(poetry_detail_infos->appreciation);
            poetry_detail_infos->appreciation = NULL;
            //SCI_TRACE_LOW("%s: free appreciation", __FUNCTION__);
        }
        for(i = 0; i < MAX_POETRY_SENTENCES_SIZE;i++){
            if(poetry_detail_infos->sen_audio[i] != NULL){
                SCI_FREE(poetry_detail_infos->sen_audio[i]);
                poetry_detail_infos->sen_audio[i] = NULL;
               // SCI_TRACE_LOW("%s: free sen_audio", __FUNCTION__);
            }
        }
        poetry_detail_infos->sentences_num = 0;
        SCI_FREE(poetry_detail_infos);
        poetry_detail_infos = NULL;
        //SCI_TRACE_LOW("%s: free poetry_detail_infos", __FUNCTION__);
    }
}

LOCAL void Poetry_ParsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    uint8 i = 0;
    if(!MMK_IsOpenWin(POETRY_DETAIL_WIN_ID))
    {
        return;
    }
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        cJSON *root = cJSON_Parse(pRcv);
        cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");
        if(responseNo->valueint == 10000)
        {
            uint32 content_size = 0;
            uint32 appreciation_size = 0;
            cJSON *data = cJSON_GetObjectItem(root, "data");
            if (data != NULL && data->type != cJSON_NULL)
            {
                cJSON *item = cJSON_GetArrayItem(data, 0);
                cJSON *title = cJSON_GetObjectItem(item, "title");
                cJSON *type = cJSON_GetObjectItem(item, "type");
                cJSON *titleAudio = cJSON_GetObjectItem(item, "titleAudio");
                cJSON *appreciation = cJSON_GetObjectItem(item, "appreciation");
                cJSON *comment = cJSON_GetObjectItem(item, "comment");
                cJSON *authorDetail = cJSON_GetObjectItem(item, "authorDetail");
                cJSON *author_name = cJSON_GetObjectItem(authorDetail, "name");
                cJSON *dynasty = cJSON_GetObjectItem(item, "dynasty");
                cJSON *Realname = cJSON_GetObjectItem(dynasty, "Realname");
                cJSON *sentences_arr = cJSON_GetObjectItem(item, "sentences");

                poetry_detail_infos = SCI_ALLOC_APPZ(sizeof(ZMT_POETRY_DETAIL_T));
                memset(poetry_detail_infos, 0, sizeof(ZMT_POETRY_DETAIL_T));

                poetry_detail_infos->id = SCI_ALLOC_APPZ(10);
                memset(poetry_detail_infos->id, 0, 10);
                sprintf(poetry_detail_infos->id, "%d", poetry_cur_post_poem_id);
               
                poetry_detail_infos->type = SCI_ALLOC_APPZ(strlen(type->valuestring)+1);
                memset(poetry_detail_infos->type, 0, strlen(type->valuestring)+1);
                SCI_MEMCPY(poetry_detail_infos->type, type->valuestring, strlen(type->valuestring));

                poetry_detail_infos->title = SCI_ALLOC_APPZ(strlen(title->valuestring)+1);
                memset(poetry_detail_infos->title, 0, strlen(title->valuestring)+1);
                SCI_MEMCPY(poetry_detail_infos->title, title->valuestring, strlen(title->valuestring));

                if(titleAudio != NULL && strlen(titleAudio->valuestring) > 0){
                    poetry_detail_infos->titleAudio = SCI_ALLOC_APPZ(strlen(titleAudio->valuestring)+1);
                    memset(poetry_detail_infos->titleAudio, 0, strlen(titleAudio->valuestring)+1);
                    SCI_MEMCPY(poetry_detail_infos->titleAudio, titleAudio->valuestring, strlen(titleAudio->valuestring));
                }else{
                    poetry_detail_infos->titleAudio = NULL;
                }

                if(appreciation != NULL && strlen(appreciation->valuestring) > 0){
                    poetry_detail_infos->appreciation = SCI_ALLOC_APPZ(strlen(appreciation->valuestring)+1);
                    memset(poetry_detail_infos->appreciation, 0, strlen(appreciation->valuestring)+1);
                    SCI_MEMCPY(poetry_detail_infos->appreciation, appreciation->valuestring, strlen(appreciation->valuestring));
                }else{
                    poetry_detail_infos->appreciation = NULL;
                }

                if(comment != NULL && strlen(comment->valuestring) > 0){
                    poetry_detail_infos->comment = SCI_ALLOC_APPZ(strlen(comment->valuestring)+1);
                    memset(poetry_detail_infos->comment, 0, strlen(comment->valuestring)+1);
                    SCI_MEMCPY(poetry_detail_infos->comment, comment->valuestring, strlen(comment->valuestring));
                }else{
                    poetry_detail_infos->comment = NULL;
                }

                poetry_detail_infos->title_content = SCI_ALLOC_APPZ(strlen(title->valuestring)+strlen(Realname->valuestring)+strlen(author_name->valuestring)+10);
                memset(poetry_detail_infos->title_content, 0, strlen(title->valuestring)+strlen(Realname->valuestring)+strlen(author_name->valuestring)+10);
                strcpy(poetry_detail_infos->title_content, title->valuestring);
                strcat(poetry_detail_infos->title_content, " \n");
                strcat(poetry_detail_infos->title_content, Realname->valuestring);
                strcat(poetry_detail_infos->title_content, "  ");
                strcat(poetry_detail_infos->title_content, author_name->valuestring);
                
                poetry_detail_infos->sentences_num = cJSON_GetArraySize(sentences_arr);
                for(i = 0; i < poetry_detail_infos->sentences_num; i++)
                {
                    cJSON *contentitem = cJSON_GetArrayItem(sentences_arr, i);
                    cJSON *content = cJSON_GetObjectItem(contentitem, "content");
                    cJSON *audio = cJSON_GetObjectItem(contentitem, "audio");
                    cJSON *trans = cJSON_GetObjectItem(contentitem, "translation");

                    if(poetry_detail_infos->sen_audio[i] != NULL){
                        SCI_FREE(poetry_detail_infos->sen_audio[i]);
                        poetry_detail_infos->sen_audio[i] = NULL;
                    }
                    poetry_detail_infos->sen_audio[i] = SCI_ALLOC_APPZ(sizeof(ZMT_POETRY_DETAIL_AUDIO_T));
                    memset(poetry_detail_infos->sen_audio[i], 0, sizeof(ZMT_POETRY_DETAIL_AUDIO_T));
                    SCI_MEMCPY(poetry_detail_infos->sen_audio[i]->audio, audio->valuestring, strlen(audio->valuestring));

                    content_size += strlen(content->valuestring);
                    if(trans != NULL && strlen(trans->valuestring) > 0){
                        appreciation_size += strlen(trans->valuestring);
                    }
                }
                if(content_size > 0){
                    poetry_detail_infos->content = SCI_ALLOC_APPZ(content_size+100);
                    memset(poetry_detail_infos->content, 0, content_size+100);
                }else{
                    poetry_detail_infos->content = NULL;
                }
                if(appreciation_size > 0){
                    poetry_detail_infos->translation = SCI_ALLOC_APPZ(appreciation_size+100);
                    memset(poetry_detail_infos->translation, 0, appreciation_size+100);
                }else{
                    poetry_detail_infos->translation = NULL;
                }           
                for(i = 0; i < poetry_detail_infos->sentences_num; i++)
                {
                    cJSON *contentitem = cJSON_GetArrayItem(sentences_arr, i);
                    cJSON *content = cJSON_GetObjectItem(contentitem, "content");
                    cJSON *trans = cJSON_GetObjectItem(contentitem, "translation");

                    strcat(poetry_detail_infos->content, content->valuestring);
                    strcat(poetry_detail_infos->translation, trans->valuestring);
                }
                
                detail_get_status = 1;
                if(zmt_tfcard_exist() && zmt_tfcard_get_free_kb() > 100 * 1024)
                {
                    char file_path[80] = {0};
                    uint32 file_len = 0;
                    char *out = PNULL;
                    sprintf(file_path,"E:/Poetry/Poetry_offline/%d/%d.bin",atoi(poetry_detail_infos->id),atoi(poetry_detail_infos->id));
                    if(zmt_file_exist(file_path))
                    {
                        zmt_file_delete(file_path);
                    }
                    out = cJSON_PrintUnformatted(root);
                    zmt_file_data_write(out, strlen(out), file_path);
                    SCI_FREE(out);
                }
            }
            else
            {
                detail_get_status = 2;
            }
        }
        else
        {		
            detail_get_status = 2;
        }
        cJSON_Delete(root);
    } 
    else 
    {
        detail_get_status = 2;
    }
    SCI_TRACE_LOW("%s: detail_get_status = %d", __FUNCTION__, detail_get_status);
    if(MMK_IsOpenWin(POETRY_DETAIL_WIN_ID))
    {
        if(poetry_detail_infos != NULL && poetry_detail_infos->id != NULL){
            poetry_is_favorite = Poetry_GetPoemIsFavorite(poetry_detail_infos->id);
        }else{
            SCI_TRACE_LOW("%s: poem_id is null", __FUNCTION__);
            return;
        }
        MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        if(detail_get_status == 1){
            if(auto_play){
                audio_download_progress = 0;
                audio_play_progress = 0;
                audio_play_now = FALSE;
                audio_download_now = FALSE;
                Poetry_StartDownloadAudio();
            }
        }
    }
}

LOCAL void Poetry_GetPoetryDetail(uint16 poem_id)
{
    if(ZMTApp_GetSimIsExist()){
        uint16 id = 0;
        char fav_id[15] = {0};
        char url [100] = {0};
        sprintf(url,"v1/card/poemDetail?F_poem_id=%d", poem_id);
        SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
        MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, Poetry_ParsePoetryDetail);
    }else{
    #ifdef LISTENING_PRATICE_SUPPORT
        MMI_CreateListeningTipWin(PALYER_PLAY_NO_SIM_TIP);
    #endif
    }
}

LOCAL void PoetryWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    if (UILAYER_IsMultiLayerEnable())
    {              
        UILAYER_CREATE_T create_info = {0};
        create_info.lcd_id = MAIN_LCD_ID;
        create_info.owner_handle = win_id;
        create_info.offset_x = poetry_tip_rect.left;
        create_info.offset_y = poetry_tip_rect.top;
        create_info.width = poetry_tip_rect.right - poetry_tip_rect.left;
        create_info.height = poetry_tip_rect.bottom - poetry_tip_rect.top;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE;
        UILAYER_CreateLayer(&create_info, &poetry_auto_tip_layer);
    }
}

LOCAL void PoetryWin_ShowAutoPlayTip(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info)
{
    if(auto_play_open_close_tip != 0)
    {
        UILAYER_APPEND_BLT_T append_layer = {0};
        GUI_RECT_T tip_rect_line = {0};
        GUISTR_STYLE_T text_style = {0};
        MMI_STRING_T text_string = {0};
        wchar text_str[35] = {0};
        char count_str[35] = {0};

        UILAYER_Clear(&lcd_dev_info);
        append_layer.lcd_dev_info = lcd_dev_info;
        append_layer.layer_level = UILAYER_LEVEL_HIGH;
        UILAYER_AppendBltLayer(&append_layer);

        LCD_FillRoundedRect(&lcd_dev_info, poetry_tip_rect, poetry_tip_rect, MMI_WHITE_COLOR);

        text_style.align = ALIGN_HVMIDDLE;
        text_style.font = DP_FONT_16;
        text_style.font_color = GUI_RGB2RGB565(80, 162, 254);

        if(auto_play_open_close_tip==1){
            sprintf(count_str,"已开启自动朗读");
        }else if(auto_play_open_close_tip==2){
            sprintf(count_str,"已关闭自动朗读");
        }else if(auto_play_open_close_tip==3){
            sprintf(count_str,"正在播放音频");
        }else if(auto_play_open_close_tip==4){
            sprintf(count_str,"正在下载音频");
        }
        GUI_GBToWstr(text_str, count_str, strlen(count_str));
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &poetry_tip_rect,
            &poetry_tip_rect,
            &text_string,
            &text_style,
            GUISTR_STATE_ALIGN,
            GUISTR_TEXT_DIR_AUTO
        );
    }
}

LOCAL void  PoetryWin_DrawGradeList(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T ctrl_id = MMI_ZMT_POETRY_GRADE_LIST_CTRL_ID;
    uint8 i = 0;
    char text_char[20] = {0};
    wchar text_str[20] = {0};
    MMI_STRING_T text_string = {0};
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    ZMT_POETRY_GRADE_NAME grade_str[POETRY_GRADE_TOTAL_NUM] = {
            "一年级上册","一年级下册","二年级上册","二年级下册",
            "三年级上册","三年级下册","四年级上册","四年级下册","五年级上册",
            "五年级下册","六年级上册", "六年级下册","七年级上册","七年级下册",
            "八年级上册","八年级下册","九年级上册","九年级下册"
    };
    GUI_RECT_T list_rect = {0, 32, 240, 320};

    list_init.both_rect.v_rect = poetry_list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, POETRY_GRADE_TOTAL_NUM, FALSE);

    for(i = 0;i < POETRY_GRADE_TOTAL_NUM; i++)
    {
        memset(text_char, 0, 20);
        memset(text_str, 0, 20);
        
        item_t.item_style = GUIITEM_STYLE_POETRY_GRADE_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = IMG_POETRY_FILE;

        strcpy(text_char, grade_str[i].name);
        GUI_GBToWstr(text_str, text_char, strlen(text_char));
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_str);
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[1].item_data.text_buffer = text_string;

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    //不画分割线
    GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
    //不画高亮条
    GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
    //GUILIST_SetListState( ctrl_id, GUILIST_STATE_ENABLE_MARK, FALSE);
    GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

    GUILIST_SetBgColor(ctrl_id,GUI_RGB2RGB565(80, 162, 254));
    GUILIST_SetTextFont(ctrl_id, DP_FONT_20, MMI_WHITE_COLOR);
    GUILIST_SetCurItemIndex(ctrl_id, poetery_info.grade_idx);
}

LOCAL void PoetryWin_DrawTitle(MMI_WIN_ID_T win_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T border_rect = {0};
    uint8 num = 0;
    char num_buf[10] = {0};
    wchar num_str[10] = {0};
				
    GUI_FillRect(&lcd_dev_info, poetry_win_rect, GUI_RGB2RGB565(80, 162, 254));
    GUI_FillRect(&lcd_dev_info, poetry_title_rect, GUI_RGB2RGB565(108, 181, 255));

    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
    MMIRES_GetText(ZMT_TXT_POETRY, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &poetry_title_rect,
        &poetry_title_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
    text_style.align = ALIGN_HVMIDDLE;
    if(auto_play == 1){
        GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_AUTO_PLAY, &lcd_dev_info);//自动播放的喇叭
    }else{
        GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_DISAUTO_PLAY, &lcd_dev_info);//不播放的喇叭
    }
    GUIRES_DisplayImg(PNULL, &poetry_favorite_rect, PNULL, win_id, IMG_POETRY_FAVORITELIST, &lcd_dev_info);//收藏页图标

    if(poetry_click_btn == 0){
        border_rect = poetry_play_rect;
        border_rect.bottom = border_rect.top + 18 + 5;
        border_rect.top -= 5;
        border_rect.right = border_rect.left + 50 + 5;
        border_rect.left -= 5;
        LCD_DrawRoundedRect(&lcd_dev_info, border_rect, border_rect, MMI_WHITE_COLOR);
    }else if(poetry_click_btn == 1){
        border_rect = poetry_favorite_rect;
        border_rect.bottom = border_rect.top + 23;
        border_rect.top -= 5;
        border_rect.right = border_rect.left + 23 + 5;
        border_rect.left -= 5;
        LCD_DrawRoundedRect(&lcd_dev_info, border_rect, border_rect, MMI_WHITE_COLOR);
    }
}

LOCAL void PoetryWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    PoetryWin_DrawTitle(win_id);

    PoetryWin_DrawGradeList(win_id);
    PoetryWin_ShowAutoPlayTip(win_id, poetry_auto_tip_layer);
}

LOCAL void PoetryWin_HanldeTpUp(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
    if (GUI_PointIsInRect(point, poetry_play_rect))//自动播放按钮
    {
        if(auto_play == 0){
            auto_play = 1;
            auto_play_open_close_tip = 1;
            Poetry_AutoPlayTipTimer(win_id);
        }else{
            auto_play = 0;
            auto_play_open_close_tip = 2;
            Poetry_AutoPlayTipTimer(win_id);
        }
        MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
    }
    else if (GUI_PointIsInRect(point, poetry_favorite_rect))//收藏夹
    {
        is_open_favorite = TRUE;
        where_open_favorite = 0;
        Poetry_GetFavoritePoetry();
        MMI_CreatePoetryItemWin();
    }
}

LOCAL void PoetryWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    GUI_POINT_T point = {0};
    if(poetry_click_btn == 0)
    {
        point.x = poetry_play_rect.left + 10;
        point.y = poetry_play_rect.top + 10;
        PoetryWin_HanldeTpUp(win_id, point);
    }
    else if(poetry_click_btn == 1)
    {
        point.x = poetry_favorite_rect.left + 10;
        point.y = poetry_favorite_rect.top + 10;
        PoetryWin_HanldeTpUp(win_id, point);
    }
    else if(poetry_click_btn == 2)
    {
        uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_POETRY_GRADE_LIST_CTRL_ID);
        poetery_info.grade_idx = cur_idx;
        is_open_favorite = FALSE;
        where_open_favorite = 1;
        MMI_CreatePoetryItemWin();
        Read_list_data_from_tf_and_parse(cur_idx+1, ZMTApp_GetSimIsExist());
        Poetry_GetFavoritePoetry();
    }
}
LOCAL void PoetryWin_KeyUpLeftDownRight(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id)
{
    int direction = msg_id - MSG_KEYUP_UP;
    SCI_TRACE_LOW("%s: direction = %d", __FUNCTION__, direction);
    switch(direction)
    {
        case 0:
        case 1:
            poetry_click_btn = 2;
            break;
        case 2:
            {
                if(poetry_click_btn == 0 || poetry_click_btn == 2){
                    poetry_click_btn = 1;
                }else{
                    poetry_click_btn--;
                }
            }
            break;
        case 3:
            {
                if(poetry_click_btn == 1 || poetry_click_btn == 2){
                    poetry_click_btn = 0;
                }else{
                    poetry_click_btn++;
                }
            }
            break;
        default:
            break;
    }
    SCI_TRACE_LOW("%s: poetry_click_btn = %d", __FUNCTION__, poetry_click_btn);
    PoetryWin_DrawTitle(win_id);
}
LOCAL void PoetryWin_CLOSE_WINDOW(MMI_WIN_ID_T win_id)
{
    MMIZDT_HTTP_Close();
    Poetry_ReleasePoetryDetail();
    memset(&poetery_info, 0, sizeof(ZMT_POETRY_SHELECT_INFO));
    poetry_click_btn = 2;
}

LOCAL MMI_RESULT_E HandlePoetryWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
                {
                    PoetryWin_OPEN_WINDOW(win_id);
                }
                break;
		case MSG_FULL_PAINT:
                {
                    PoetryWin_FULL_PAINT(win_id);
                }
                break;
             case MSG_KEYUP_LEFT:
             case MSG_KEYUP_RIGHT:
             case MSG_KEYUP_UP:
             case MSG_KEYUP_DOWN:
                {
                    PoetryWin_KeyUpLeftDownRight(win_id, msg_id);
                }
                break;
             case MSG_APP_WEB:
             case MSG_APP_OK:
             case MSG_CTL_PENOK:
                {
                    PoetryWin_CTL_PENOK(win_id);
                }
                break;
		case MSG_TP_PRESS_UP:
                {
                    GUI_POINT_T point = {0};
                    point.x =MMK_GET_TP_X(param);
                    point.y =MMK_GET_TP_Y(param);
                    if(point.y < 1.2*POETRY_LINE_HIGHT)
                    {			
                        PoetryWin_HanldeTpUp(win_id, point);                       
                    }
                }
                break;
        case MSG_KEYDOWN_CANCEL:
		case MSG_KEYDOWN_RED:
			break;
		case MSG_KEYUP_RED:
		case MSG_KEYUP_CANCEL:
                {
                    MMK_CloseWin(win_id);
                }
                break;
		case MSG_CLOSE_WINDOW:
                {
                    PoetryWin_CLOSE_WINDOW(win_id);
                }
                break;
		default:
                recode = MMI_RESULT_FALSE;
                break;
	}
	return recode;
}

LOCAL void PoetryItemWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    if (UILAYER_IsMultiLayerEnable())
    {              
        UILAYER_CREATE_T create_info = {0};
        create_info.lcd_id = MAIN_LCD_ID;
        create_info.owner_handle = win_id;
        create_info.offset_x = poetry_tip_rect.left;
        create_info.offset_y = poetry_tip_rect.top;
        create_info.width = poetry_tip_rect.right - poetry_tip_rect.left;
        create_info.height = poetry_tip_rect.bottom - poetry_tip_rect.top;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE;
        UILAYER_CreateLayer(&create_info, &poetry_item_auto_tip_layer);
    }
}

LOCAL void DrawPoetryItemList(MMI_WIN_ID_T win_id, uint16 study_favorite)
{
    MMI_CTRL_ID_T ctrl_id = MMI_ZMT_POETRY_ITEM_LIST_CTRL_ID;
    uint8 i = 0;
    char text_char[50] = {0};
    wchar text_str[50] = {0};
    MMI_STRING_T text_string = {0};
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    uint8 size = 0;

    list_init.both_rect.v_rect = poetry_list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    if(study_favorite){
        size = total_poetrynum;
    }else{
        size = total_favorite_poetrynum;
    }
    GUILIST_SetMaxItem(ctrl_id, size, FALSE);
    for(i = 0;i < size; i++)
    {
        memset(text_char, 0, 50);
        memset(text_str, 0, 50);
        
        item_t.item_style = GUIITEM_STYLE_POETRY_ITEM_LIST_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = IMG_POETRY_ITEM_BG;

        if(study_favorite){
            strcpy(text_char, grade_all_list.all_grade[poetry_cur_post_grade_id-1].grade_info[i].title);
        }else{
            strcpy(text_char, favorite_infos.poetry[i].title);
        }
        //SCI_TRACE_LOW("%s: text_char = %s", __FUNCTION__, text_char);
        GUI_UTF8ToWstr(text_str, 50, text_char, strlen(text_char));
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_str);
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[1].item_data.text_buffer = text_string;

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    //不画分割线
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
    //不画高亮条
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_AUTO_SCROLL, TRUE);
    GUILIST_SetCycScroll(ctrl_id, TRUE);
    GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);
    GUILIST_SetBgImage(ctrl_id, IMG_POETRY_ITEM_BG, FALSE);
    GUILIST_SetBgColor(ctrl_id, GUI_RGB2RGB565(80, 162, 254));
    GUILIST_SetTextFont(ctrl_id, DP_FONT_20, GUI_RGB2RGB565(80, 162, 254));
    GUILIST_SetRect(ctrl_id,&poetry_list_rect);
    GUILIST_SetCurItemIndex(ctrl_id, poetery_info.poetry_idx);
}

LOCAL void PoetryItemWin_DrawTitle(MMI_WIN_ID_T win_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T border_rect = {0};
    uint8 status = 0;
				
    GUI_FillRect(&lcd_dev_info, poetry_win_rect, GUI_RGB2RGB565(80, 162, 254));
    GUI_FillRect(&lcd_dev_info, poetry_title_rect, GUI_RGB2RGB565(108, 181, 255));

    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
    MMIRES_GetText(ZMT_TXT_POETRY, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &poetry_title_rect,
        &poetry_title_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );

    text_style.align = ALIGN_HVMIDDLE;
    if(auto_play == 1)
    {
        GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_AUTO_PLAY, &lcd_dev_info);//自动播放的喇叭
    }
    else
    {
        GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_DISAUTO_PLAY, &lcd_dev_info);//不播放的喇叭
    }
    if(!is_open_favorite){
        status = grade_get_status;
        GUIRES_DisplayImg(PNULL, &poetry_favorite_rect, PNULL, win_id, IMG_POETRY_FAVORITELIST, &lcd_dev_info);//收藏页图标
    }else{
        status = favorite_get_status;
    }
    if(poetry_click_btn == 0){
        border_rect = poetry_play_rect;
        border_rect.bottom = border_rect.top + 18 + 5;
        border_rect.top -= 5;
        border_rect.right = border_rect.left + 50 + 5;
        border_rect.left -= 5;
        LCD_DrawRoundedRect(&lcd_dev_info, border_rect, border_rect, MMI_WHITE_COLOR);
    }else if(poetry_click_btn == 1 && !is_open_favorite){
        border_rect = poetry_favorite_rect;
        border_rect.bottom = border_rect.top + 23;
        border_rect.top -= 5;
        border_rect.right = border_rect.left + 23 + 5;
        border_rect.left -= 5;
        LCD_DrawRoundedRect(&lcd_dev_info, border_rect, border_rect, MMI_WHITE_COLOR);
    }
    if(status == 3){//加载失败 
        MMIRES_GetText(ZMT_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
    }else if(status == 4){//自动重试也失败了 展示手动刷新按钮
        MMIRES_GetText(ZMT_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
    }else if(status == 2){//暂无古诗词
        MMIRES_GetText(ZMT_TXT_POETRY_NO_INFO, win_id, &text_string);
    }else if(status == 0){//正在加载
        MMIRES_GetText(ZMT_TXT_POETRY_LOADING, win_id, &text_string);
    }else if(status == 5){//暂无收藏
        MMIRES_GetText(ZMT_TXT_POETRY_NO_FAVORITE, win_id, &text_string);
    }else{
        return;
    }
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &poetry_win_rect,
        &poetry_win_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
}

LOCAL void PoetryItemWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    uint8 status = 0;
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
    PoetryItemWin_DrawTitle(win_id);
    if(!is_open_favorite){
        status = grade_get_status;
    }else{
        status = favorite_get_status;
    }
    if(MMK_IsActiveCtrl(MMI_ZMT_POETRY_ITEM_LIST_CTRL_ID))
    {
        GUI_RECT_T rect = {0};
        GUILIST_SetRect(MMI_ZMT_POETRY_ITEM_LIST_CTRL_ID, &rect);
    }
    PoetryWin_ShowAutoPlayTip(win_id, poetry_item_auto_tip_layer);
    if(status == 1){
        if(!is_open_favorite){
            if(total_poetrynum > 0){
                DrawPoetryItemList(win_id, 1);
            }
        }else{
            if(total_favorite_poetrynum > 0){
                DrawPoetryItemList(win_id, 0);
            }
        }
    }
}
LOCAL void PoetryItemWin_HanldeTpUp(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
    if(GUI_PointIsInRect(point, poetry_play_rect))//自动播放按钮
    {
        if(auto_play == 0){
            auto_play = 1;
            auto_play_open_close_tip = 1;
            Poetry_AutoPlayTipTimer(win_id);
        }else{
            auto_play = 0;
            auto_play_open_close_tip = 2;
            Poetry_AutoPlayTipTimer(win_id);
        }
        MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
    }
    else if(GUI_PointIsInRect(point, poetry_favorite_rect) && !is_open_favorite)//收藏夹
    {
        is_open_favorite = TRUE;
        poetry_is_favorite = TRUE;
        where_open_favorite = 1;
        Poetry_GetFavoritePoetry();
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void PoetryItemWin_KeyUpLeftDownRight(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id)
{
    int direction = msg_id - MSG_KEYUP_UP;
    uint8 status = 0;
    if(!is_open_favorite){
        status = grade_get_status;
    }else{
        status = favorite_get_status;
    }
    if(status != 1){
        return;
    }
    SCI_TRACE_LOW("%s: direction = %d", __FUNCTION__, direction);
    switch(direction)
    {
        case 0:
        case 1:
            poetry_click_btn = 2;
            break;
        case 2:
            {
                if(!is_open_favorite){
                    if(poetry_click_btn == 0 || poetry_click_btn == 2){
                        poetry_click_btn = 1;
                    }else{
                        poetry_click_btn--;
                    }
                }else{
                    poetry_click_btn = 0;
                }
            }
            break;
        case 3:
            {
                if(!is_open_favorite){
                    if(poetry_click_btn == 1 || poetry_click_btn == 2){
                        poetry_click_btn = 0;
                    }else{
                        poetry_click_btn++;
                    }
                }else{
                    poetry_click_btn = 0;
                }
            }
            break;
        default:
            break;
    }
    SCI_TRACE_LOW("%s: poetry_click_btn = %d", __FUNCTION__, poetry_click_btn);
    PoetryItemWin_DrawTitle(win_id);
}
LOCAL void PoetryItemWin_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    GUI_POINT_T point = {0};
    uint8 status = 0;
    if(!is_open_favorite){
        status = grade_get_status;
    }else{
        status = favorite_get_status;
    }
    if(status != 1){
        return;
    }
    if(poetry_click_btn == 0)
    {
        point.x = poetry_play_rect.left + 10;
        point.y = poetry_play_rect.top + 10;
        PoetryItemWin_HanldeTpUp(win_id, point);
    }
    else if(poetry_click_btn == 1 && !is_open_favorite)
    {
        point.x = poetry_favorite_rect.left + 10;
        point.y = poetry_favorite_rect.top + 10;
        PoetryItemWin_HanldeTpUp(win_id, point);
    }
    else if(poetry_click_btn == 2)
    {
        uint16 cur_idx = GUILIST_GetCurItemIndex(MMI_ZMT_POETRY_ITEM_LIST_CTRL_ID);
        poetery_info.poetry_idx = cur_idx;
        if(!is_open_favorite){
            poetry_cur_post_poem_id = grade_all_list.all_grade[(poetry_cur_post_grade_id-1)].grade_info[cur_idx].id;
        }else{
            if(favorite_get_status = 1){
                poetry_cur_post_poem_id = atoi(favorite_infos.poetry[cur_idx].id);
            }else{
                return;
            }
        }
        SCI_TRACE_LOW("%s: poetry_cur_post_poem_id = %d", __FUNCTION__, poetry_cur_post_poem_id);
        MMI_CreatePoetryDetailWin();  
    }
}

LOCAL void PoetryItemWin_CLOSE_WINDOW(MMI_WIN_ID_T win_id)
{
    is_open_favorite = FALSE;
    grade_get_status = 0;
    where_open_favorite = 0;
    poetry_click_btn = 2;
    MMIZDT_HTTP_Close();
}

LOCAL MMI_RESULT_E HandlePoetryItemWinMsg(
	MMI_WIN_ID_T win_id, 
	MMI_MESSAGE_ID_E msg_id,
	DPARAM param
	)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id)
    {
        case MSG_OPEN_WINDOW:
            {
                PoetryItemWin_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_GET_FOCUS:
            {
                //releasePoetryDetail();
            }
            break;
        case MSG_FULL_PAINT:
            {
                PoetryItemWin_FULL_PAINT(win_id);
            }
            break;
        case MSG_APP_WEB:
        case MSG_APP_OK:
        case MSG_CTL_PENOK:
            {              
                PoetryItemWin_CTL_PENOK(win_id);
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T point = {0};
                point.x =MMK_GET_TP_X(param);
                point.y =MMK_GET_TP_Y(param);
                if(point.y < 1.2*POETRY_LINE_HIGHT)
                {			
                    PoetryItemWin_HanldeTpUp(win_id, point);                       
                }
            }
            break;
        case MSG_KEYUP_LEFT:
        case MSG_KEYUP_RIGHT:
        case MSG_KEYUP_UP:
        case MSG_KEYUP_DOWN:
            {
                PoetryItemWin_KeyUpLeftDownRight(win_id, msg_id);
            }
            break;
        case MSG_KEYDOWN_CANCEL:
		case MSG_KEYDOWN_RED:
			break;
        case MSG_KEYUP_RED:
        case MSG_KEYUP_CANCEL:
            {
                if(!is_open_favorite){
                    MMK_CloseWin(win_id);
                }else{
                    if(!where_open_favorite){
                        MMK_CloseWin(win_id);
                    }else{
                        is_open_favorite = FALSE;
                        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
                    }
                }
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                PoetryItemWin_CLOSE_WINDOW(win_id);
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

LOCAL void PoetryDetailWin_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    MMI_CTRL_ID_T ctrl_id = MMI_ZMT_POETRY_APPRE_TEXT_ID;
    GUI_FONT_T text_font = DP_FONT_22;
    GUI_COLOR_T text_color = MMI_WHITE_COLOR;
    GUI_BG_T bg = {0};
    
    Poetry_ReleasePoetryDetail();
    if (UILAYER_IsMultiLayerEnable())
    {              
        UILAYER_CREATE_T create_info = {0};
        create_info.lcd_id = MAIN_LCD_ID;
        create_info.owner_handle = win_id;
        create_info.offset_x = poetry_tip_rect.left;
        create_info.offset_y = poetry_tip_rect.top;
        create_info.width = poetry_tip_rect.right - poetry_tip_rect.left;
        create_info.height = poetry_tip_rect.bottom - poetry_tip_rect.top;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE;
        UILAYER_CreateLayer(&create_info, &poetry_detail_auto_tip_layer);
    }

    bg.bg_type = GUI_BG_COLOR;
    bg.color = GUI_RGB2RGB565(80, 162, 254);
    GUITEXT_SetBg(ctrl_id, &bg);
    GUITEXT_SetRect(ctrl_id, &poetry_content_other_rect);
    GUITEXT_SetFont(ctrl_id, &text_font,&text_color);
    GUITEXT_IsDisplayPrg(FALSE, ctrl_id);
    GUITEXT_SetHandleTpMsg(FALSE, ctrl_id);
    GUITEXT_SetClipboardEnabled(ctrl_id,FALSE);
    GUITEXT_IsSlide(ctrl_id,FALSE);

    SCI_TRACE_LOW("%s: start %d KB-- %d kB", __FUNCTION__, SCI_GetSystemSpaceTotalAvalidMem()/1024, SCI_GetHeapTotalSpace(DYNAMIC_HEAP_APP_MEMORY)/1024);
    
    audio_download_progress = 0;
    audio_play_progress = 0;
    audio_play_now = FALSE;
    audio_download_now = FALSE;
    detail_get_status = 0;
    poetry_player_volume = MMIAPISET_GetMultimVolume();
    Read_detail_data_from_tf_and_parse();
}

LOCAL void PoetryDetailWin_DrawOption(MMI_WIN_ID_T win_id, uint16 current_option)
{
    uint8 i = 0;
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T optin_rect = {0};
    GUI_RECT_T optin_hor_line_rect = {0};
    GUI_RECT_T optin_option_line_rect = {0};
    MMI_TEXT_ID_T text_id[4] = {ZMT_TXT_POETRY_CONTENT_OPTIONS, ZMT_TXT_POETRY_EXPRESSION_OPTIONS,
        ZMT_TXT_POETRY_TRANSLATE_OPTIONS, ZMT_TXT_POETRY_APPRECIATION_OPTIONS
    };

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_20;
    text_style.font_color = GUI_RGB2RGB565(151, 166, 180);

    GUI_FillRect(&lcd_dev_info, poetry_bottom_option_rect, MMI_WHITE_COLOR);
  
    optin_rect = poetry_bottom_option_rect;
    optin_rect.left = 0.3*POETRY_LINE_WIDTH;
    optin_rect.right = 1.3*POETRY_LINE_WIDTH;
    for(i = 0; i < 4;i++){
        if(i == current_option){
            text_style.font_color = GUI_RGB2RGB565(80, 162, 254);
        }else{
            text_style.font_color = GUI_RGB2RGB565(151, 166, 180);
        }
        poetry_bottom_option_rect_array[i] = optin_rect;
        MMIRES_GetText(text_id[i], win_id, &text_string);
        GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&optin_rect,
		&optin_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
		);
        optin_rect.left += 1.5*POETRY_LINE_WIDTH;
        optin_rect.right = optin_rect.left + POETRY_LINE_WIDTH;
    }
    optin_option_line_rect = poetry_bottom_option_rect_array[current_option];
    optin_option_line_rect.bottom -= 2;
    optin_option_line_rect.top = optin_option_line_rect.bottom - 1;
    LCD_FillRect(&lcd_dev_info, optin_option_line_rect, GUI_RGB2RGB565(80, 162, 254));
}

LOCAL void PoetryDetailWin_ShowText(MMI_WIN_ID_T win_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    MMI_STRING_T text_comment = {0};
    MMI_STRING_T text_appre = {0};
    char * text_char = NULL;
    wchar * text_str = NULL;
    GUI_RECT_T rect = {0};
    GUI_RECT_T content_title_rect = {0};

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_22;
    text_style.font_color = MMI_WHITE_COLOR;
    switch(poetery_info.detail_idx)
    {
        case 0:
            {
                uint8 line_num = 0;
                text_char = SCI_ALLOC_APPZ(strlen(poetry_detail_infos->title_content)+1);
                memset(text_char, 0, strlen(poetry_detail_infos->title_content)+1);
                strcpy(text_char, poetry_detail_infos->title_content);

                text_str = SCI_ALLOC_APPZ(3*strlen(text_char));
                memset(text_str, 0, 3*strlen(text_char));
                
                GUI_UTF8ToWstr(text_str, 3*strlen(text_char), text_char, strlen(text_char));
                text_string.wstr_ptr = text_str;
                text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                line_num = GUI_CalculateStringLinesByPixelNum(poetry_content_title_rect.right-poetry_content_title_rect.left,text_string.wstr_ptr,text_string.wstr_len,DP_FONT_22,0,TRUE);
                content_title_rect = poetry_content_title_rect;
                if(line_num > 2){
                    content_title_rect.top = 1.2*POETRY_LINE_HIGHT;
                }
                GUISTR_DrawTextToLCDInRect(
                    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                    &content_title_rect,
                    &content_title_rect,
                    &text_string,
                    &text_style,
                    text_state,
                    GUISTR_TEXT_DIR_AUTO
                );

                SCI_FREE(text_char);
                text_char = NULL;
                SCI_FREE(text_str);
                text_str = NULL;

                text_char = SCI_ALLOC_APPZ(strlen(poetry_detail_infos->content)+1);
                memset(text_char, 0, strlen(poetry_detail_infos->content)+1);
                strcpy(text_char, poetry_detail_infos->content);

                text_style.align = ALIGN_HMIDDLE;
                rect = poetry_content_rect;
            }
            break;
        case 1:
            {
                if(poetry_detail_infos->comment == NULL){
                    text_char = NULL;
                    MMIRES_GetText(ZMT_TXT_POETRY_NO_COMMENT, win_id, &text_comment);
                    text_string.wstr_ptr = text_comment.wstr_ptr;
                    text_string.wstr_len = text_comment.wstr_len;
                }else{
                    uint32 size = strlen(poetry_detail_infos->comment);
                    if(size > 1000){
                        size = 1000;
                    }
                    text_char = SCI_ALLOC_APPZ(size+1);
                    memset(text_char, 0, size+1);
                    SCI_MEMCPY(text_char, poetry_detail_infos->comment, size);
                }

                text_style.align = ALIGN_HVMIDDLE;
                rect = poetry_content_other_rect; 
            }
            break;
        case 2:
            {
                text_char = SCI_ALLOC_APPZ(strlen(poetry_detail_infos->translation)+1);
                memset(text_char, 0, strlen(poetry_detail_infos->translation)+1);
                strcpy(text_char, poetry_detail_infos->translation);
                
                text_style.align = ALIGN_HVMIDDLE;
                rect = poetry_content_other_rect;
            }
            break;
        case 3:
            {
                if(poetry_detail_infos->appreciation == NULL){
                    text_char = NULL;
                    MMIRES_GetText(ZMT_TXT_POETRY_NO_APPRECIATION, win_id, &text_appre);
                    text_string.wstr_ptr = text_appre.wstr_ptr;
                    text_string.wstr_len = text_appre.wstr_len;
                }else{
                    uint32 size = strlen(poetry_detail_infos->appreciation);
                    text_char = SCI_ALLOC_APPZ(strlen(poetry_detail_infos->appreciation)+1);
                    memset(text_char, 0, strlen(poetry_detail_infos->appreciation)+1);
                    strcpy(text_char, poetry_detail_infos->appreciation);
                }

                text_style.align = ALIGN_HVMIDDLE;
                rect = poetry_content_other_rect;
            }
            break;
    }
    if(text_char != NULL){
        text_str = SCI_ALLOC_APPZ(3*strlen(text_char));
        memset(text_str, 0, 3*strlen(text_char));
        GUI_UTF8ToWstr(text_str, 3*strlen(text_char), text_char, strlen(text_char));
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        GUITEXT_SetRect(MMI_ZMT_POETRY_APPRE_TEXT_ID, &rect);
        GUITEXT_SetAlign(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_style.align);
        GUITEXT_SetString(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_string.wstr_ptr, text_string.wstr_len, TRUE);
        GUITEXT_SetResetTopDisplay(MMI_ZMT_POETRY_APPRE_TEXT_ID, 0);
        
        SCI_FREE(text_char);
        text_char = NULL;
        SCI_FREE(text_str);
        text_str = NULL;
    }else{
        GUITEXT_SetRect(MMI_ZMT_POETRY_APPRE_TEXT_ID, &rect);
        GUITEXT_SetAlign(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_style.align);
        GUITEXT_SetString(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_string.wstr_ptr, text_string.wstr_len, TRUE);
        GUITEXT_SetResetTopDisplay(MMI_ZMT_POETRY_APPRE_TEXT_ID, 0);
    }
}

LOCAL void PoetryDetailWin_DrawBorder(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T border_rect = {0};
    if(poetry_detail_click_btn == 4 && poetery_info.detail_idx == 0){
        border_rect = poetry_play_rect;
        border_rect.bottom = border_rect.top + 22 + 5;
        border_rect.top -= 5;
        border_rect.right = border_rect.left + 22 + 5;
        border_rect.left -= 5;
        LCD_DrawRoundedRect(&lcd_dev_info, border_rect, border_rect, MMI_WHITE_COLOR);
    }else if(poetry_detail_click_btn == 5){
        border_rect = poetry_favorite_rect;
        border_rect.bottom = border_rect.top + 23 + 5;
        border_rect.top -= 5;
        border_rect.right = border_rect.left + 23 + 5;
        border_rect.left -= 5;
        LCD_DrawRoundedRect(&lcd_dev_info, border_rect, border_rect, MMI_WHITE_COLOR);
    }
}
LOCAL void PoetryDetailWin_DrawTitle(MMI_WIN_ID_T win_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_SINGLE_LINE | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};

    GUI_FillRect(&lcd_dev_info, poetry_title_rect, GUI_RGB2RGB565(108, 181, 255));

    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
    MMIRES_GetText(ZMT_TXT_POETRY, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &poetry_title_rect,
        &poetry_title_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );

    text_style.align = ALIGN_HVMIDDLE;
    if(poetery_info.detail_idx == 0){//只有原文有音频
        GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_POETRY_PLAY, &lcd_dev_info);
    }
    if(poetry_is_favorite){
        GUIRES_DisplayImg(PNULL, &poetry_favorite_rect, PNULL, win_id, IMG_ZMT_FAVORITE, &lcd_dev_info);
    }else{
        GUIRES_DisplayImg(PNULL, &poetry_favorite_rect, PNULL, win_id, IMG_ZMT_NOTFAVORITE, &lcd_dev_info);
    }
    PoetryDetailWin_DrawBorder(win_id);
}

LOCAL void PoetryDetailWin_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_SINGLE_LINE | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_FillRect(&lcd_dev_info, poetry_win_rect, GUI_RGB2RGB565(80, 162, 254));
    PoetryDetailWin_DrawTitle(win_id);
    PoetryDetailWin_DrawOption(win_id, poetery_info.detail_idx);
    PoetryWin_ShowAutoPlayTip(win_id, poetry_detail_auto_tip_layer);
    text_style.align = ALIGN_HVMIDDLE;
    if(detail_get_status == 0)
    {
        MMIRES_GetText(ZMT_TXT_POETRY_LOADING, win_id, &text_string);
        GUITEXT_SetRect(MMI_ZMT_POETRY_APPRE_TEXT_ID, &poetry_content_other_rect);
        GUITEXT_SetAlign(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_style.align);
        GUITEXT_SetString(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_string.wstr_ptr, text_string.wstr_len, FALSE);
    }
    else if(detail_get_status == 2)
    {
        MMIRES_GetText(ZMT_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
        GUITEXT_SetRect(MMI_ZMT_POETRY_APPRE_TEXT_ID, &poetry_content_other_rect);
        GUITEXT_SetAlign(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_style.align);
        GUITEXT_SetString(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_string.wstr_ptr, text_string.wstr_len, FALSE);
    }
    else if(detail_get_status == 3)
    {
        MMIRES_GetText(ZMT_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
        GUITEXT_SetRect(MMI_ZMT_POETRY_APPRE_TEXT_ID, &poetry_content_other_rect);
        GUITEXT_SetAlign(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_style.align);
        GUITEXT_SetString(MMI_ZMT_POETRY_APPRE_TEXT_ID, text_string.wstr_ptr, text_string.wstr_len, FALSE);
    }
    else
    {
        PoetryDetailWin_ShowText(win_id);
    }
}

LOCAL void PoetryDetailWin_HanldeTpUp(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
    if(GUI_PointIsInRect(point, poetry_play_rect) && poetery_info.detail_idx == 0)
    {
        SCI_TRACE_LOW("%s: audio_play_now = %d, audio_download_now = %d", __FUNCTION__, audio_play_now, audio_download_now);
        if(!audio_play_now && !audio_download_now){
            audio_download_progress = 0;
            audio_play_progress = 0;
            audio_download_now = FALSE;
            Poetry_StartPlayAudio();
        }else if(audio_download_now){
            auto_play_open_close_tip = 4;
            Poetry_AutoPlayTipTimer(win_id);
        }else{
            auto_play_open_close_tip = 3;
            Poetry_AutoPlayTipTimer(win_id);
        }
        MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
    }
    else if(GUI_PointIsInRect(point, poetry_favorite_rect))
    {
        if(poetry_is_favorite){
            poetry_is_favorite = FALSE;
            MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
        }else{
            poetry_is_favorite = TRUE;
            MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[0]))
    {
        if (poetery_info.detail_idx != 0){
            poetery_info.detail_idx = 0;
            MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[1]))
    {
        if (poetery_info.detail_idx != 1){
            poetery_info.detail_idx = 1;
            MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[2]))
    {
        if (poetery_info.detail_idx != 2){
            poetery_info.detail_idx = 2;
            MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[3]))
    {
        if (poetery_info.detail_idx != 3){
            poetery_info.detail_idx = 3;
            MMK_PostMsg(win_id, MSG_FULL_PAINT, PNULL, 0);
        }
    }
}

LOCAL void PoetryDetailWin_KeyOk(MMI_WIN_ID_T win_id)
{
    GUI_POINT_T point = {0};
    if(detail_get_status != 1){
        return;
    }
    if(poetry_detail_click_btn == 4 && poetery_info.detail_idx == 0){
        point.x = poetry_play_rect.left + 10;
        point.y = poetry_play_rect.top + 10;
        PoetryDetailWin_HanldeTpUp(win_id, point);
    }else if(poetry_detail_click_btn == 5){
        point.x = poetry_favorite_rect.left + 10;
        point.y = poetry_favorite_rect.top + 10;
        PoetryDetailWin_HanldeTpUp(win_id, point);
    }else{
        return;
    }
}
LOCAL void PoetryDetailWin_KeyUpLeftDownRight(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id)
{
    int direction = msg_id - MSG_KEYUP_UP;
    if(detail_get_status != 1){
        return;
    }
    SCI_TRACE_LOW("%s: direction = %d", __FUNCTION__, direction);
    if(direction < 2){
        return;
    }
    switch(direction)
    {
        case 2:
            {
                if(poetry_detail_click_btn == 0){
                    poetry_detail_click_btn = 5;
                }else{
                    if(poetery_info.detail_idx != 0 && poetry_detail_click_btn == 5){
                        poetry_detail_click_btn--;
                    }
                    poetry_detail_click_btn--;
                }
            }
            break;
        case 3:
            {
                if(poetry_detail_click_btn == 5){
                    poetry_detail_click_btn = 0;
                }else{
                    if(poetery_info.detail_idx != 0 && poetry_detail_click_btn == 3){
                        poetry_detail_click_btn++;
                    }
                    poetry_detail_click_btn++;
                }
            }
            break;
        default:
            break;
    }
    SCI_TRACE_LOW("%s: poetry_detail_click_btn = %d", __FUNCTION__, poetry_detail_click_btn);
    if(poetry_detail_click_btn < 4){
        poetery_info.detail_idx = poetry_detail_click_btn;
    }
    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
}
LOCAL void PoetryDetailWin_CLOSE_WINDOW(MMI_WIN_ID_T win_id)
{
    BOOLEAN is_favorite = FALSE;
    if(poetry_detail_infos == NULL || poetry_detail_infos->id == NULL){
        return;
    }
    is_favorite = Poetry_GetPoemIsFavorite(poetry_detail_infos->id);
    SCI_TRACE_LOW("%s: is_favorite = %d, poetry_is_favorite = %d", __FUNCTION__, is_favorite, poetry_is_favorite);
    if(is_favorite){
        if(!poetry_is_favorite){
            Poetry_ChangeFavoritePoem(poetry_detail_infos->id, FALSE);
        }
    }else{
        if(poetry_is_favorite){
            Poetry_ChangeFavoritePoem(poetry_detail_infos->id, TRUE);
        }
    }
    poetery_info.detail_idx = 0;
    audio_download_progress = 0;
    audio_play_progress = 0;
    audio_play_now = FALSE;
    audio_download_now = FALSE;
    detail_get_status = 0;
    poetry_detail_click_btn = 0;
    MMIZDT_HTTP_Close();
    Poetry_StopPlayMp3();  
}

LOCAL MMI_RESULT_E HandlePoetryDetailWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id,DPARAM param) 
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id)
    {
        case MSG_OPEN_WINDOW: 
            {
                PoetryDetailWin_OPEN_WINDOW(win_id);               
            }
            break;
        case MSG_FULL_PAINT:
            {
                PoetryDetailWin_FULL_PAINT(win_id);
            }
            break;
	case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T point = {0};
                int16 tp_offset_x = 0;
                int16 tp_offset_y = 0;
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                tp_offset_x = point.x - main_tp_down_x;
                tp_offset_y = point.y - main_tp_down_y;
                if(point.y < POETRY_LINE_HIGHT || point.y > MMI_MAINSCREEN_HEIGHT - POETRY_LINE_HIGHT)
                {
                    PoetryDetailWin_HanldeTpUp(win_id, point);
                }
                else
                {
                    if(ABS(tp_offset_x) <= ABS(tp_offset_y))
                    {
                        if(tp_offset_y > 40)
                        {
                            MMK_PostMsg(MMI_ZMT_POETRY_APPRE_TEXT_ID, MSG_KEYREPEAT_UP, PNULL, 0);
                        }
                        else if(tp_offset_y < -40)
                        {
                            MMK_PostMsg(MMI_ZMT_POETRY_APPRE_TEXT_ID, MSG_KEYREPEAT_DOWN, PNULL, 0);
                        }
                    }
                }
            }
            break;
        case MSG_KEYUP_WEB:
        case MSG_APP_WEB:
        case MSG_APP_OK:
        case MSG_CTL_PENOK:
            {
                PoetryDetailWin_KeyOk(win_id);
            }
            break;
        case MSG_KEYUP_LEFT:
        case MSG_KEYUP_RIGHT:
        case MSG_KEYUP_UP:
        case MSG_KEYUP_DOWN:
            {
                PoetryDetailWin_KeyUpLeftDownRight(win_id, msg_id);
            }
            break;
       case MSG_TP_PRESS_DOWN:
            {
                main_tp_down_x = MMK_GET_TP_X(param);
                main_tp_down_y = MMK_GET_TP_Y(param);
            }
            break;
        case MSG_KEYDOWN_UPSIDE:
        case MSG_KEYDOWN_VOL_UP:
            {
                poetry_player_volume = ZmtApp_VolumeChange(poetry_player_handle, TRUE, poetry_player_volume);
            }
            break;
        case MSG_KEYDOWN_DOWNSIDE:
        case MSG_KEYDOWN_VOL_DOWN:
            {
                poetry_player_volume = ZmtApp_VolumeChange(poetry_player_handle, FALSE, poetry_player_volume);
            }
            break;
        case MSG_KEYDOWN_CANCEL:
		case MSG_KEYDOWN_RED:
			break;
	case MSG_KEYUP_RED:
	case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
	case MSG_CLOSE_WINDOW:
            {
                PoetryDetailWin_CLOSE_WINDOW(win_id);
            }
            break;
	default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_POETRY_WIN_TAB) = 
{
	WIN_ID(POETRY_MAIN_WIN_ID),
	WIN_FUNC((uint32)HandlePoetryWinMsg),
	WIN_HIDE_STATUS,
	END_WIN
};

WINDOW_TABLE(POETRY_ITEM_WIN_TAB) = 
{
	WIN_ID(POETRY_ITEM_WIN_ID),
	WIN_FUNC((uint32)HandlePoetryItemWinMsg),
	WIN_HIDE_STATUS,
	END_WIN
};

WINDOW_TABLE(POETRY_DETAIL_WIN_TAB) = {
    WIN_ID(POETRY_DETAIL_WIN_ID),
    WIN_FUNC((uint32)HandlePoetryDetailWinMsg),
    CREATE_TEXT_CTRL(MMI_ZMT_POETRY_APPRE_TEXT_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreatePoetryWin(void)
{
    MMI_WIN_ID_T win_id = POETRY_MAIN_WIN_ID;
    if (MMK_IsOpenWin(win_id))
    {
        MMK_CloseWin(win_id);
    }
    MMK_CreateWin((uint32 *)MMI_POETRY_WIN_TAB, PNULL);
}

LOCAL void MMI_CreatePoetryItemWin(void)
{
    MMI_WIN_ID_T win_id = POETRY_ITEM_WIN_ID;
    if (MMK_IsOpenWin(win_id))
    {
        MMK_CloseWin(win_id);
    }
    MMK_CreateWin((uint32 *)POETRY_ITEM_WIN_TAB, PNULL);
}

PUBLIC BOOLEAN MMI_IsPoetryItemWinOpen(void)
{
    if (MMK_IsOpenWin(POETRY_ITEM_WIN_ID))
    {
        return TRUE;
    }
    return FALSE;
}

LOCAL void MMI_CreatePoetryDetailWin(void)
{
    MMI_WIN_ID_T win_id = POETRY_DETAIL_WIN_ID;
    if (MMK_IsOpenWin(win_id))
    {
        MMK_CloseWin(win_id);
    }
    MMK_CreateWin((uint32 *)POETRY_DETAIL_WIN_TAB, PNULL);
}

PUBLIC BOOLEAN MMI_IsPoetryDetailWinOpen(void)
{
    if (MMK_IsOpenWin(POETRY_DETAIL_WIN_ID))
    {
        return TRUE;
    }
    return FALSE;
}

PUBLIC void ZMTPoetry_ClosePoetryPlayer(void)
{
    Poetry_StopPlayMp3();
}


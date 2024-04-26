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
#include "dsl_poetry_export.h"
#include "dsl_poetry_id.h"
#include "dsl_poetry_image.h"
#include "dsl_poetry_text.h"
#include "dsl_poetry_main.h"
#include "cafcontrol.h"
#include "os_api.h"
#include "cjson.h"
#include "types.h"
#include "mmidisplay_data.h"
#include "types.h"
#include "mmisrvrecord_export.h"
#include "mmisrvaud_api.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "dsl_listening_export.h"
#endif
#ifdef WORD_CARD_SUPPORT
#include "dsl_word_image.h"
#endif
//水调歌头的赏析太长了超出数组范围了，赏析里的于江岸玩月前缺字，注释里的深长前面缺字，赏析里的仰天叹息前面缺字，注释 只希望两人年年平安
//全局变量、静态变量在全局变量区，函数内部变量在栈区，alloc在堆区
//4键退出记住学到哪一首诗 下一次直接进
//grade第一条改成家长推送（确定有家长推送了才出现这一条）
//RB 关mp3下载http的函数有问题,好像暂时解决了 
//todo 收藏夹里为空就不显示收藏夹？  不行啊，我每次点进去收藏夹才会拉取list，逻辑冲突了
//	if(MMK_IsFocusWin(POETRY_MAIN_WIN_ID)){所有发送刷新的地方都检查一下是否focus？严谨？
		// MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	// }
//列表页的get收藏列表、get年级列表，退出时也需要关闭http，不过不太严重
//todo 需要兼容没有version的旧版数据
//todo 需要考虑退出时终止update流程的问题
//会先读标题然后才展现详情页面
//快速进入、返回详情页   会在返回的一瞬间死机
//本地有最新的为什么还要去建temp.bin
LOCAL uint8 is_can_click=0;
LOCAL int16 main_poetry_status = -3;
LOCAL int16 main_favorite_status ;
LOCAL uint16 total_poetrynum[POETRY_GRADE_TOTAL_NUM] = {0};
LOCAL uint16 total_favorite_poetrynum = 0;
LOCAL GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
LOCAL int16 main_tp_down_x = 0;
LOCAL int16 main_tp_down_y = 0;
LOCAL uint16 pagenum = 1;
LOCAL uint16 favorite_pagenum = 1;
LOCAL uint16 currentpage = 1;
LOCAL uint16 current_favorite_page = 1;
LOCAL uint8 main_option = 1;///1:主页2:收藏夹
LOCAL uint16 detail_selected_index = 0;
LOCAL uint16 detail_get_status = 0;
LOCAL uint16 detail_win_current_option = 1;
LOCAL uint16 detail_current_poetry_favorite_status ;
LOCAL uint16 detail_current_poetry_favorite_status_has_changed ;
LOCAL uint16 favorite_page_need_refresh =0;
LOCAL uint16 detail_just_closed = 0;
LOCAL uint16 audio_is_playing = 0;
LOCAL uint16 detail_content_line_num = 0;
LOCAL uint16 grade_win;///1:选年级0.2:选诗歌
LOCAL uint16 grade_current_page;
LOCAL uint16 auto_play;
LOCAL uint16 abs_http_running_need_close_when_close_win;
LOCAL uint16 mp3_download_http_running_need_close_when_close_win;
LOCAL uint16 parse_detail_success_now_can_click_audio;//进入详情页，加载详情信息成功后才响应喇叭的点击
LOCAL uint16 getting_mp3_now_cant_back;//自动或手动点击播放音频后，在结果(成功或失败)出来之前都禁止点击返回
// LOCAL DSL_POETRY_LIST_T infos ={0};//todo 这样不好  在parse里根据实际情况申请大小  退出时释放  如果按4键没释放，下次会重新申请？
// LOCAL DSL_POETRY_GRADE_LIST_T grade_infos ={0};
LOCAL DSL_POETRY_ALL_GRADE_LIST_T grade_all_list = {0};
LOCAL uint16 grade_get_status[18]= {0};//0是初始化，1是成功，2是list里没有东西，3是失败了,4是自动重试也失败了需要手动刷新
LOCAL uint16 grade_display_status;
LOCAL DSL_POETRY_LIST_T favorite_infos ={0};
LOCAL DSL_POETRY_DETAIL_T poetry_detail_infos = {0};
LOCAL DSL_POETRY_UPDATE_DETAIL_T update_detail_infos = {0};
LOCAL MMISRV_HANDLE_T mp3_player_handle=PNULL;
LOCAL DSL_POETRY_AUDIO_DATA_T *audio_data_infos;
LOCAL MMISRV_HANDLE_T poetry_player_handle=PNULL;
LOCAL DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T * temp_favorite_list = PNULL;
LOCAL uint16 audio_playing_status ;
LOCAL uint16 audio_play_progress;
LOCAL uint16 audio_download_progress;
LOCAL uint16 audio_break_status;
LOCAL uint16 audio_download_complete;
LOCAL uint16 detailwin_is_open_closed;
LOCAL MMI_STRING_T text_title;
LOCAL MMI_STRING_T text_origin;
LOCAL MMI_STRING_T text_note ;
LOCAL MMI_STRING_T text_trans ;
LOCAL MMI_STRING_T text_appr ;
LOCAL uint16 *wstr_title = PNULL;
LOCAL uint16 *wstr_origin = PNULL;
LOCAL uint16 *wstr_note = PNULL;
LOCAL uint16 *wstr_trans = PNULL;
LOCAL uint16 *wstr_appr = PNULL;
LOCAL uint16 auto_play_finished;
LOCAL uint16 auto_play_open_close_tip;//0无提示，1是开启，2是关闭
LOCAL GUI_LCD_DEV_INFO poetry_auto_tip_layer={0};
LOCAL uint8 autoplay_tip_timer=0;
LOCAL uint8 autoplay_cant_back_timer=0;
// uint16 audio_is_playing_cant_back_in_2s;
//LOCAL MMIDSLMAIN_HTTP_CONTEXT_T * close_win_abs_http_context_ptr = PNULL;
LOCAL uint16 re_download_ten_times_limits;
LOCAL uint16 get_grade_list_fail_retry_times_limit;
LOCAL uint16 get_detail_fail_retry_times_limit;
LOCAL uint16 get_grade_list_idx;
LOCAL uint16 update_re_download_limits = 0;
LOCAL uint16 local_file_load_failed = 0;

LOCAL uint16 poetry_cur_post_grade_id = 0;
LOCAL uint16 poetry_cur_post_poem_id = 0;
LOCAL GUI_RECT_T poetry_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T poetry_title_rect = {0.5*POETRY_LINE_WIDTH, 0, 5.5*POETRY_LINE_WIDTH, POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_back_rect = {0.5*POETRY_LINE_WIDTH, 0, 2.5*POETRY_LINE_WIDTH, POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_play_rect = {2.6*POETRY_LINE_WIDTH, 6, 3.6*POETRY_LINE_WIDTH, POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_favorite_rect = {5*POETRY_LINE_WIDTH, 6, 6*POETRY_LINE_WIDTH, POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_tip_rect = {POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT/2-POETRY_LINE_WIDTH,MMI_MAINSCREEN_WIDTH-POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT/2+POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_page_rect = {5*POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT-POETRY_LINE_HIGHT,6*POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T poetry_grade_line_rect = {10,1.2*POETRY_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-10,2.4*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_grade_line_rect_array[POETRY_GRADE_LIST_SHOW_ITEM] = {0};
LOCAL GUI_RECT_T poetry_grade_icon_rect = {0.6*POETRY_LINE_WIDTH,1.5*POETRY_LINE_HIGHT,1.5*POETRY_LINE_WIDTH,2.4*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_grade_text_rect = {1.6*POETRY_LINE_WIDTH,1.2*POETRY_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-10,2.4*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_item_line_rect = {10,1.2*POETRY_LINE_HIGHT,MMI_MAINSCREEN_WIDTH-10,2.7*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_item_line_rect_array[POETRY_ITEM_LIST_SHOW_ITEM] = {0};
LOCAL GUI_RECT_T poetry_content_title_rect = {0.5*POETRY_LINE_WIDTH, 1.5*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.5*POETRY_LINE_HIGHT, 2.5*POETRY_LINE_HIGHT};
LOCAL GUI_RECT_T poetry_content_rect = {0.5*POETRY_LINE_WIDTH, 2.5*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.5*POETRY_LINE_HIGHT, 9*POETRY_LINE_HIGHT-5};
LOCAL GUI_RECT_T poetry_appre_rect = {0.5*POETRY_LINE_WIDTH, 1.5*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.5*POETRY_LINE_HIGHT, 9*POETRY_LINE_HIGHT-5};
LOCAL GUI_RECT_T poetry_trans_rect = {0.5*POETRY_LINE_WIDTH, 1.5*POETRY_LINE_HIGHT, MMI_MAINSCREEN_WIDTH-0.5*POETRY_LINE_HIGHT, 9*POETRY_LINE_HIGHT-5};
LOCAL GUI_RECT_T poetry_bottom_option_rect = {0.3*POETRY_LINE_WIDTH,9*POETRY_LINE_HIGHT,1.3*POETRY_LINE_WIDTH,MMI_MAINSCREEN_HEIGHT};
LOCAL GUI_RECT_T poetry_bottom_option_rect_array[4] = {0};

LOCAL void ClickTimeOut(uint8 timer_id, uint32 param);
LOCAL uint8 GetCanntClick(uint is_click); 
LOCAL void setFavorite(uint16 poetry_id,uint8 add_or_delete);
LOCAL uint8 getFavoriteStatus(uint16 id);
LOCAL void parsePoetryFavoriteList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void localParsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void parsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void updateParsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void getPoetry(void);
LOCAL void getFavoritePoetry(void);
LOCAL void getPoetryDetail(void);
LOCAL void PoetryDetail_DrawOption(MMI_WIN_ID_T win_id, uint16 current_option);
LOCAL void Draw_PageNum(uint8 study_favorite);
LOCAL void DrawPoetryList(uint16 study_favorite);
LOCAL void Main_LoadPrePage(void);
LOCAL void Main_LoadNextPage(void);
LOCAL void Detail_LoadPrePage(void);
LOCAL void Detail_LoadNextPage(void);
LOCAL void Detail_If_Favorite_Changed_Than_Post(void);
LOCAL void PoetryWin_PenOkCall(void);
LOCAL MMI_RESULT_E HandlePoetryWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id,DPARAM param);
LOCAL MMI_RESULT_E HandlePoetryDetailWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id,DPARAM param);
LOCAL void MMI_CreatePoetryDetailWin(void);
LOCAL void favorite_post_handle(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void startDownloadAudio(void);
LOCAL void Poetry_ParseAudioDownload( BOOLEAN success, uint32 idx);
LOCAL void Poetry_ParseAudioUpdate( BOOLEAN success, uint32 idx);
LOCAL void Poetry_PlayMp3(void);
LOCAL uint8 Poetry_StartPlayMp3(char* file_name);
LOCAL void Poetry_StopPlayMp3(void);
LOCAL void Poetry_PlayMp3Notify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param);
LOCAL BOOLEAN Poetry_Play_RequestHandle(MMISRV_HANDLE_T *audio_handle, MMISRVAUD_ROUTE_T route, MMISRVAUD_TYPE_U *audio_data, MMISRVMGR_NOTIFY_FUNC notify);
LOCAL void Draw_Grade(MMI_WIN_ID_T win_id);
LOCAL void Draw_Grade_Page_Num(void);
LOCAL void Grade_LoadPrePage(void);
LOCAL void Grade_LoadNextPage(void);
LOCAL void getGradeList(uint16 gradeid);
LOCAL void updateGetGradeList(uint16 gradeid);
LOCAL void localParseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void parseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void updateParseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void updateLocalDetail(uint16 idx);
LOCAL void auto_play_tip_timeout(uint8 timer_id,uint32 param);
LOCAL void auto_play_tip_timer(void);
LOCAL void faildReDownloadAudio(uint32 idx);
LOCAL int StrReplace(char strRes[],char from[], char to[]);
#if OFFLINE_DATA
LOCAL void Read_detail_data_from_tf_and_parse(void);
LOCAL void Read_list_data_from_tf_and_parse(uint16 gradeid);
LOCAL void Offline_favorite_list_add(void);
LOCAL void Offline_favorite_list_delete(void);
#endif
LOCAL void poetry_maingradeRefresh(void);
LOCAL void LayoutItem(MMI_CTRL_ID_T ctrl_id, int16 left, int16 right, int16 top,int16 bottom);
LOCAL void InitButton(MMI_CTRL_ID_T ctrl_id);
LOCAL void poetry_detailRefresh(void);
LOCAL void startUpdateMp3(uint16 idx);


LOCAL void poetry_detailRefresh(void){
	detail_get_status = 0;
	MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	getPoetryDetail();
}

LOCAL void InitButton(MMI_CTRL_ID_T ctrl_id) {
  GUI_FONT_ALL_T font = {0};
  GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
  font.font = SONG_FONT_16;
  font.color = MMI_BLACK_COLOR;
  GUIBUTTON_SetBorder(ctrl_id, &btn_border, FALSE);
  GUIBUTTON_SetFont(ctrl_id, &font);
}

LOCAL void LayoutItem(MMI_CTRL_ID_T ctrl_id, int16 left, int16 right, int16 top,int16 bottom) 
{
	GUI_BOTH_RECT_T both_rect = {0};
	both_rect.h_rect.right = right;
	both_rect.h_rect.left = left;
	both_rect.h_rect.top = top;
	both_rect.h_rect.bottom = bottom;

	both_rect.v_rect.right = right;
	both_rect.v_rect.left = left;
	both_rect.v_rect.top = top;
	both_rect.v_rect.bottom = bottom;
	GUIAPICTRL_SetBothRect(ctrl_id, &both_rect);
}

LOCAL void poetry_maingradeRefresh(void){
	if(get_grade_list_idx!=0){
		grade_get_status[(grade_display_status-1)] = 0;//加载中
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);//显示加载中
		getGradeList(get_grade_list_idx);
	}
}
//收藏夹加载失败时怎么办，没有做自动、手动刷新功能啊
#if OFFLINE_DATA//todo grade list也有可能加载失败啊  加上自动重试功能！！！
LOCAL void Offline_favorite_list_add(void)
{//todo 离线收藏夹功能不太正常，怎么一直显示加载中啊  还有，添加的时候要检查是不是已经有了！成功之后要回调啊！
	if(dsl_tfcard_exist())
	{
		char file_path[80] = {0};
		char * data_buf = PNULL;
		uint32 file_len = 0;
		char * out = PNULL;
		char *temp_file_path = "E:/Poetry/Poetry_offline/favorite_list.bin";

		strcpy(file_path,temp_file_path);
		if(dsl_file_exist(file_path)){
			data_buf = dsl_file_data_read(file_path, &file_len);
		}else{
			SCI_TRACE_LOW("guooffline %s: file_path = %s not exist !!", __FUNCTION__, file_path);
			return;
		}
		SCI_TRACE_LOW("guooffline %s: file_len = %d", __FUNCTION__, file_len);
		if((data_buf!=PNULL) && (file_len > 0))
		{
			cJSON *id;
			cJSON *title;
			cJSON *root = cJSON_Parse(data_buf);//记得FREE掉！！！
			cJSON *poetry_arr = cJSON_GetObjectItem(root, "F_poem_list");
			cJSON *poetry_item = cJSON_CreateObject();
			char tempstr[25]={0};
			char tempstr_title[100]={0};

			if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL && (cJSON_GetArraySize(poetry_arr)<349)) {//todo 检查一下需要add的诗是否已经在里面
				uint16 add_id_already_exist = 0;
				uint16 i;

				sprintf(tempstr, "%d", grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id);

				for(i=0;i < cJSON_GetArraySize(poetry_arr);i++){
						cJSON *poetry_item = cJSON_GetArrayItem(poetry_arr, i);
						cJSON *target_id = cJSON_GetObjectItem(poetry_item, "F_poem_id");
						if(strcmp(&tempstr,&target_id->valuestring)==0){
							add_id_already_exist = 1;
							break;
						}
				}
				if(add_id_already_exist){

				}else{
					cJSON_AddItemToArray(poetry_arr, poetry_item);
					id = cJSON_CreateString(tempstr);
					title = cJSON_CreateString(grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].title);
					cJSON_AddItemToObject(poetry_item, "F_poem_id", id);
					cJSON_AddItemToObject(poetry_item, "F_title", title);

					out = cJSON_Print(root);
				
					if(dsl_file_exist(file_path))
					{
						SCI_TRACE_LOW("guo930 delete 1 file_path=%s",file_path);
						dsl_file_delete(file_path);
					}
					dsl_file_data_write(out, strlen(out), file_path);
					favorite_post_handle(1, PNULL,0,0);//成功之后回调
					if(out != PNULL){
						SCI_FREE(out);
						out = PNULL;
					}
				}
			}
			cJSON_Delete(root);
		}
		if(data_buf != PNULL){
			SCI_FREE(data_buf);
			data_buf = PNULL;
		}
	}
	else
	{
		SCI_TRACE_LOW("%s: dsl tf crad not exist !!", __FUNCTION__);
	}
}

LOCAL void Offline_favorite_list_delete(void)
{
	if(dsl_tfcard_exist())
	{
		char file_path[80] = {0};
		char * data_buf = PNULL;
		uint32 file_len = 0;
		char * out = PNULL;
		char *temp_file_path = "E:/Poetry/Poetry_offline/favorite_list.bin";

		strcpy(file_path,temp_file_path);
		if(dsl_file_exist(file_path)){
			data_buf = dsl_file_data_read(file_path, &file_len);
		}else{
			return;
		}
		if((data_buf!=PNULL) && (file_len > 0))
		{
			uint16 i;
			cJSON *root = cJSON_Parse(data_buf);
			cJSON *poetry_arr = cJSON_GetObjectItem(root, "F_poem_list");
			char tempstr[25]={0};
			if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL) {
				if(main_option == 1)
				{
					for(i=0;i < cJSON_GetArraySize(poetry_arr);i++){
						cJSON *poetry_item = cJSON_GetArrayItem(poetry_arr, i);
						cJSON *target_id = cJSON_GetObjectItem(poetry_item, "F_poem_id");
						if(atoi(target_id->valuestring)==grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id){
							cJSON_DeleteItemFromArray(poetry_arr,i);
							out = cJSON_Print(root);					
							if(dsl_file_exist(file_path))
							{
								SCI_TRACE_LOW("guo930 delete 2 file_path=%s",file_path);
								dsl_file_delete(file_path);
							}
							dsl_file_data_write(out, strlen(out), file_path);
							favorite_post_handle(1, PNULL,0,0);//成功之后回调
							break;
						}
					}
				}
				else if (main_option == 2)
				{
					for(i=0;i < cJSON_GetArraySize(poetry_arr);i++){
						cJSON *poetry_item = cJSON_GetArrayItem(poetry_arr, i);
						cJSON *target_id = cJSON_GetObjectItem(poetry_item, "F_poem_id");
						if(atoi(target_id->valuestring)==atoi(favorite_infos.poetry[(((current_favorite_page-1)*4)+detail_selected_index-1)].id)){
							cJSON_DeleteItemFromArray(poetry_arr,i);
							out = cJSON_Print(root);
							if(dsl_file_exist(file_path))
							{
								SCI_TRACE_LOW("guo930 delete 3 file_path=%s",file_path);
								dsl_file_delete(file_path);
							}
							dsl_file_data_write(out, strlen(out), file_path);
							favorite_post_handle(1, PNULL,0,0);//成功之后回调
							break;
						}
					}
				}
				if(out!=PNULL){
					SCI_FREE(out);
					out = PNULL;
				}
				cJSON_Delete(root);
			}
		}
		if(data_buf != PNULL){
			SCI_FREE(data_buf);
			data_buf = PNULL;
		}
	}
}

LOCAL void Read_list_data_from_tf_and_parse(uint16 gradeid)
{
    SCI_TRACE_LOW("guooffline %s: gradeid = %d", __FUNCTION__, gradeid);
    if(dsl_tfcard_exist())
    {
        char file_path[80] = {0};
        char * data_buf = PNULL;
        uint32 file_len = 0;
        char *temp_file_path = PNULL;
        temp_file_path = SCI_ALLOCA(80 * sizeof(char));
        memset(temp_file_path, 0, 80);
        sprintf(temp_file_path,"E:/Poetry/Poetry_offline/grade_%d/list.bin",gradeid);
        strcpy(file_path,temp_file_path);
        SCI_FREE(temp_file_path);
        if(dsl_file_exist(file_path))
        {
            data_buf = dsl_file_data_read(file_path, &file_len);
        }else{
            SCI_TRACE_LOW("guooffline %s: file_path = %s not exist !!", __FUNCTION__, file_path);
            getGradeList(gradeid);
            return;
        }
        SCI_TRACE_LOW("guooffline %s: file_len = %d", __FUNCTION__, file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            localParseGradeList(1, data_buf , file_len, gradeid);
            //updateGetGradeList(gradeid);//更新一下TF卡的年级列表
        }else{
            getGradeList(gradeid);
            if(data_buf != PNULL){
                SCI_FREE(data_buf);
                data_buf = PNULL;
            }
        }
    }
    else
    {	
        getGradeList(gradeid);
    }
}

LOCAL void Read_detail_data_from_tf_and_parse(void){
    if(dsl_tfcard_exist())
    {
		char file_path[80] = {0};
		char * data_buf = PNULL;
		uint32 file_len = 0;
		char *temp_file_path =PNULL;
		uint16 idx;
        
		temp_file_path= SCI_ALLOCA(80 * sizeof(char));
		memset(temp_file_path, 0, 80);
		if(main_option == 1){
			idx = grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id;
		}else if(main_option == 2){
			idx = atoi(favorite_infos.poetry[(((current_favorite_page-1)*4)+detail_selected_index-1)].id);
		}
		sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d.bin",idx,idx);
		strcpy(file_path,temp_file_path);
		SCI_FREE(temp_file_path);
		if(dsl_file_exist(file_path))
		{
			data_buf = dsl_file_data_read(file_path, &file_len);
		}else{
			getPoetryDetail();
			return;
		}
		if(data_buf != PNULL && file_len > 0){
			localParsePoetryDetail(1, data_buf, file_len, idx);
		}else{
			getPoetryDetail();
			if(data_buf != PNULL){
				SCI_FREE(data_buf);
				data_buf = PNULL;
			}
		}
    }
    else
    {	
        getPoetryDetail();
    }
}
#endif

LOCAL void auto_play_cant_back_timeout(uint8 timer_id,uint32 param)
{
	// if(0 != autoplay_cant_back_timer)
	// {
	// 	MMK_StopTimer(autoplay_cant_back_timer);
	// 	autoplay_cant_back_timer = 0;
	// }
	// audio_is_playing_cant_back_in_2s = 0;
}

LOCAL void auto_play_cant_back_timer(void)
{
	// if(0 != autoplay_cant_back_timer)
	// {
	// 	MMK_StopTimer(autoplay_cant_back_timer);
	// 	autoplay_cant_back_timer = 0;
	// }
	// autoplay_cant_back_timer = MMK_CreateTimerCallback(500, auto_play_cant_back_timeout,(uint32)0, FALSE);
	// MMK_StartTimerCallback(autoplay_cant_back_timer, 500, auto_play_cant_back_timeout, (uint32)0, FALSE);
}

LOCAL void auto_play_tip_timeout(uint8 timer_id,uint32 param)
{
	if(0 != autoplay_tip_timer)
	{
		MMK_StopTimer(autoplay_tip_timer);
		autoplay_tip_timer = 0;
	}
	auto_play_open_close_tip=0;
	if(MMK_IsFocusWin(POETRY_MAIN_WIN_ID)){
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
}

LOCAL void auto_play_tip_timer(void)
{
	if(0 != autoplay_tip_timer)
	{
		MMK_StopTimer(autoplay_tip_timer);
		autoplay_tip_timer = 0;
	}
	autoplay_tip_timer = MMK_CreateTimerCallback(2000, auto_play_tip_timeout,(uint32)0, FALSE);
	MMK_StartTimerCallback(autoplay_tip_timer, 2000, auto_play_tip_timeout, (uint32)0, FALSE);
}

LOCAL void updateParseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	int i = 0;
	if (is_ok && pRcv != PNULL && Rcv_len> 2){
		cJSON *root = cJSON_Parse(pRcv);
		cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");
		char *out = cJSON_PrintUnformatted(root);
		if(responseNo->valueint==10000){
			cJSON *poetry_arr = cJSON_GetObjectItem(root, "data");
			if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL) {
				if(cJSON_GetArraySize(poetry_arr) != 0){
					if(dsl_tfcard_exist()&& dsl_tfcard_get_free_kb() > 100 * 1024){
						char file_path[80] = {0};
						char * data_buf = PNULL;
						uint32 file_len = 0;
						char *temp_file_path =PNULL;

						temp_file_path= SCI_ALLOCA(80 * sizeof(char));
						memset(temp_file_path, 0, 80);
						sprintf(temp_file_path,"E:/Poetry/Poetry_offline/grade_%d/list.bin",poetry_cur_post_grade_id);
						strcpy(file_path,temp_file_path);
						SCI_FREE(temp_file_path);
						if(dsl_file_exist(file_path))
						{
							data_buf = dsl_file_data_read(file_path, &file_len);
							if(strcmp(data_buf,out)!=0){
								SCI_TRACE_LOW("guo930 delete 4 file_path=%s",file_path);
								dsl_file_delete(file_path);
								dsl_file_data_write(out, strlen(out), file_path);
							}
							if(data_buf != PNULL){
								SCI_FREE(data_buf);
								data_buf = PNULL;
							}
						}else{
							dsl_file_data_write(out, strlen(out), file_path);
						}
					}
				}
			}
		}
		SCI_FREE(out);
		cJSON_Delete(root);
	}
}

LOCAL void localParseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	int i = 0;
	if (is_ok && pRcv != PNULL && Rcv_len> 2){
		cJSON *root = cJSON_Parse(pRcv);
		cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");
		if(responseNo->valueint==10000){
			cJSON *poetry_arr = cJSON_GetObjectItem(root, "data");
			if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL) {
				for (i = 0; i < cJSON_GetArraySize(poetry_arr); i++) {
					cJSON *item = cJSON_GetArrayItem(poetry_arr, i);
					cJSON *id = cJSON_GetObjectItem(item, "PoemId");
					cJSON *title = cJSON_GetObjectItem(item, "Title");
					SCI_TRACE_LOW("%s: title = %s", __FUNCTION__, title->valuestring);
					grade_all_list.all_grade[(err_id-1)].grade_info[i].id = id->valueint;
					strncpy((char *)grade_all_list.all_grade[(err_id-1)].grade_info[i].title, title->valuestring,strlen(title->valuestring));
				}
				if(cJSON_GetArraySize(poetry_arr) == 0){
					main_poetry_status = -2;
					grade_get_status[(err_id-1)] = 2;
					total_poetrynum[(grade_display_status-1)] = 0;
					MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
				}else{
					main_poetry_status = -4;
					grade_get_status[(err_id-1)] = 1;
					total_poetrynum[(grade_display_status-1)] = cJSON_GetArraySize(poetry_arr);
					MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
				}
			}else{
				main_poetry_status = -1;
				grade_get_status[(err_id-1)] = 3;
				MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
			}
		}else{
			main_poetry_status = -1;
			grade_get_status[(err_id-1)] = 3;
			MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}
		cJSON_Delete(root);
	} else {
		main_poetry_status = -1;
		grade_get_status[(err_id-1)] = 3;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
	if((get_grade_list_fail_retry_times_limit>4)&&(grade_get_status[(err_id-1)]==3)){
		get_grade_list_fail_retry_times_limit --;
		getGradeList(err_id);
	}else if((get_grade_list_fail_retry_times_limit<=4)&&(grade_get_status[(err_id-1)]==3)){
		main_poetry_status = -1;
		grade_get_status[(err_id-1)] = 4;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
#if OFFLINE_DATA
	{
		if(pRcv !=PNULL){
			SCI_FREE(pRcv);
			pRcv = PNULL;
		}
	}
#endif
}

LOCAL void parseGradeList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	int i = 0;
	if (is_ok && pRcv != PNULL && Rcv_len> 2){
		cJSON *root = cJSON_Parse(pRcv);
		cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");

		if(responseNo->valueint==10000){
			cJSON *poetry_arr = cJSON_GetObjectItem(root, "data");
			if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL) {
				for (i = 0; i < cJSON_GetArraySize(poetry_arr); i++) {
					cJSON *item = cJSON_GetArrayItem(poetry_arr, i);
					cJSON *id = cJSON_GetObjectItem(item, "PoemId");
					cJSON *title = cJSON_GetObjectItem(item, "Title");
					grade_all_list.all_grade[(poetry_cur_post_grade_id-1)].grade_info[i].id = id->valueint;
					strncpy((char *)grade_all_list.all_grade[(poetry_cur_post_grade_id-1)].grade_info[i].title, title->valuestring,strlen(title->valuestring));
				}
				if(cJSON_GetArraySize(poetry_arr) == 0){
					main_poetry_status = -2;
					grade_get_status[(poetry_cur_post_grade_id-1)] = 2;
					total_poetrynum[(grade_display_status-1)] = 0;
					MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
				}else{
					main_poetry_status = -4;
					grade_get_status[(poetry_cur_post_grade_id-1)] = 1;
					total_poetrynum[(grade_display_status-1)] = cJSON_GetArraySize(poetry_arr);
					MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
					if(dsl_tfcard_exist()&& dsl_tfcard_get_free_kb() > 100 * 1024){
						char file_path[80] = {0};
						uint32 file_len = 0;
						char *temp_file_path =PNULL;
						char * out = cJSON_PrintUnformatted(root);
						temp_file_path= SCI_ALLOCA(80 * sizeof(char));
						memset(temp_file_path, 0, 80);
						sprintf(temp_file_path,"E:/Poetry/Poetry_offline/grade_%d/list.bin",poetry_cur_post_grade_id);
						strcpy(file_path,temp_file_path);
						SCI_FREE(temp_file_path);
						if(dsl_file_exist(file_path)){
							SCI_TRACE_LOW("guo930 delete 5 file_path=%s",file_path);
							dsl_file_delete(file_path);
						}
						dsl_file_data_write(out, strlen(out), file_path);
						SCI_FREE(out);
					}
				}
			}else{
				main_poetry_status = -1;
				grade_get_status[(poetry_cur_post_grade_id-1)] = 3;
				MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
			}
		}else{
			main_poetry_status = -1;
			grade_get_status[(poetry_cur_post_grade_id-1)] = 3;
			MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}
		cJSON_Delete(root);
	} else {
		main_poetry_status = -1;
		grade_get_status[(poetry_cur_post_grade_id-1)] = 3;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
	if((get_grade_list_fail_retry_times_limit>4)&&(grade_get_status[(poetry_cur_post_grade_id-1)]==3))
	{
		get_grade_list_fail_retry_times_limit --;
		getGradeList(poetry_cur_post_grade_id);
	}else if((get_grade_list_fail_retry_times_limit<=4)&&(grade_get_status[(poetry_cur_post_grade_id-1)]==3))
	{
		main_poetry_status = -1;
		grade_get_status[(poetry_cur_post_grade_id-1)] = 4;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
}

LOCAL void updateGetGradeList(uint16 gradeid)
{
	char *url = SCI_ALLOCA(350 * sizeof(char));
	memset(url, 0, 350);
	sprintf(url,"v1/card/poem?F_grade_id=%d&F_limit=30",gradeid);
	poetry_cur_post_grade_id = gradeid;
	MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, updateParseGradeList);
  	//StartAbslutoUrlHttpRequest(url, 3000, gradeid, updateParseGradeList);
	SCI_FREE(url);
}

LOCAL void getGradeList(uint16 gradeid) 
{
	char *url = SCI_ALLOCA(350 * sizeof(char));
	memset(url, 0, 350);
	poetry_cur_post_grade_id = gradeid;
	sprintf(url,"v1/card/poem?F_grade_id=%d&F_limit=30",gradeid);
	MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, parseGradeList);
  	//StartAbslutoUrlHttpRequest(url, 3000, gradeid, parseGradeList);
	SCI_FREE(url);
}
LOCAL void Grade_LoadNextPage(void){
	if(grade_current_page != 4){
		grade_current_page ++;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
}
LOCAL void Grade_LoadPrePage(void){
	if(grade_current_page != 1){
		grade_current_page --;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
}

LOCAL void Draw_Grade_Page_Num(void){
	uint16 wtmp[5] = {0};
	uint16 tmp[5] = {0};
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_str = {0};

	SCI_MEMSET(wtmp,0,5);
	SCI_MEMSET(tmp,0,5);
	sprintf(tmp,"%d/4",grade_current_page);
	GUI_GBToWstr(wtmp,tmp,strlen(tmp)+1);
	text_str.wstr_ptr=wtmp;
	text_str.wstr_len= MMIAPICOM_Wstrlen(wtmp);
	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_16;
	text_style.font_color = MMI_BLACK_COLOR;
	GUISTR_DrawTextToLCDInRect(
		&lcd_dev_info,
		&poetry_page_rect,
		&poetry_page_rect,
		&text_str,
		&text_style,
		GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO
	);
}

LOCAL void Draw_Grade(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    uint8 index = 0;
    uint8 num = 0;
    MMI_STRING_T grade_string = {0};
    wchar grade_wchar[20] = {0};
    GUISTR_STYLE_T text_style = {0};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    DSL_POETRY_GRADE_NAME grade_str[POETRY_GRADE_TOTAL_NUM] = {"一年级上册","一年级下册","二年级上册","二年级下册",
            "三年级上册","三年级下册","四年级上册","四年级下册","五年级上册",
            "五年级下册","六年级上册", "六年级下册","七年级上册","七年级下册",
            "八年级上册","八年级下册","九年级上册","九年级下册"
    };
    GUI_RECT_T lind_rect= {0};
    GUI_RECT_T icon_rect= {0};
    GUI_RECT_T grade_rect= {0};

    index = POETRY_GRADE_LIST_SHOW_ITEM * (grade_current_page - 1);
    num = POETRY_GRADE_LIST_SHOW_ITEM * grade_current_page;
    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = SONG_FONT_16;
    text_style.font_color = MMI_BLACK_COLOR;

    memset(&poetry_grade_line_rect_array, 0, sizeof(poetry_grade_line_rect_array));
    lind_rect = poetry_grade_line_rect;
    icon_rect = poetry_grade_icon_rect;
    grade_rect = poetry_grade_text_rect;
    for( ;index < num && index < POETRY_GRADE_TOTAL_NUM;index++)
    {
        poetry_grade_line_rect_array[i] = lind_rect;
        LCD_DrawRect(&lcd_dev_info,lind_rect,MMI_BLACK_COLOR);
        GUIRES_DisplayImg(PNULL, &icon_rect, PNULL, win_id, IMG_POETRY_FILE, &lcd_dev_info);
        GUI_GBToWstr(grade_wchar, &grade_str[index], strlen(&grade_str[index]));
        grade_string.wstr_ptr = grade_wchar;
        grade_string.wstr_len = MMIAPICOM_Wstrlen(grade_wchar);
        GUISTR_DrawTextToLCDInRect(
            &lcd_dev_info,
            &grade_rect,
            &grade_rect,
            &grade_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
        i++;
        lind_rect.top += 1.5*POETRY_LINE_HIGHT;
        lind_rect.bottom = lind_rect.top + 1.2*POETRY_LINE_HIGHT;
        icon_rect.top += 1.5*POETRY_LINE_HIGHT;
        icon_rect.bottom = icon_rect.top + 1.2*POETRY_LINE_HIGHT;
        grade_rect.top += 1.5*POETRY_LINE_HIGHT;
        grade_rect.bottom = grade_rect.top + 1.2*POETRY_LINE_HIGHT;
    }
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

LOCAL void Poetry_PlayMp3Notify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
	MMISRVAUD_REPORT_T *report_ptr = PNULL;
	BOOLEAN result = TRUE;
	if(param != PNULL && handle > 0)
	{
		report_ptr = (MMISRVAUD_REPORT_T *)param->data;
		if(report_ptr != PNULL && handle == poetry_player_handle)
		{
			SCI_TRACE_LOW("guochushibiao  Poetry_PlayMp3Notify  559 running !");
			switch(report_ptr->report)
			{
            	case MMISRVAUD_REPORT_END:
				{
					if(audio_play_progress < (detail_content_line_num+1)){
						SCI_TRACE_LOW("guochushibiao  Poetry_PlayMp3Notify  564 got end msg and not ending!");
						audio_play_progress ++;
						Poetry_PlayMp3();
					}else{
						SCI_TRACE_LOW("guochushibiao  Poetry_PlayMp3Notify  568 got end msg and ending!");
						audio_play_progress = 0;
						audio_playing_status = 0;
						auto_play_finished = 1;
						MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
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
	//SCI_TRACE_LOW("%s: handle = %d", __FUNCTION__, listening_player_handle);
	/*if (0 != listening_player_handle)
	{
		MMISRVAUD_Stop(listening_player_handle);
		MMISRVMGR_Free(listening_player_handle);
		listening_player_handle = PNULL;
	}
	listening_play_times = 0;*/
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
		SCI_TRACE_LOW("guochushibiao  Poetry_StartPlayMp3  621 !");
       	MMISRVAUD_Stop(poetry_player_handle);
       	MMISRVMGR_Free(poetry_player_handle);
       	poetry_player_handle = PNULL;
    }

	SCI_TRACE_LOW("guochushibiao  Poetry_StartPlayMp3  627 !");

    full_path_len = GUI_GBToWstr(full_path, (const uint8*)file_name, SCI_STRLEN(file_name));
    audio_data.ring_file.type = MMISRVAUD_TYPE_RING_FILE;
    audio_data.ring_file.name = full_path;
    audio_data.ring_file.name_len = full_path_len;
    audio_data.ring_file.fmt  = (MMISRVAUD_RING_FMT_E)MMIAPICOM_GetMusicType(audio_data.ring_file.name, audio_data.ring_file.name_len);

   	if(Poetry_Play_RequestHandle(&audio_handle, MMISRVAUD_ROUTE_NONE, &audio_data, Poetry_PlayMp3Notify))
    {
		SCI_TRACE_LOW("guochushibiao  Poetry_StartPlayMp3  637 success !");
        poetry_player_handle = audio_handle;
		poetry_volume = MMIAPISET_GetMultimVolume();
		MMISRVAUD_SetVolume(audio_handle, poetry_volume);
        if(!MMISRVAUD_Play(audio_handle, 0))
        {     
            SCI_TRACE_LOW("%s: MMISRVAUD_Play failed", __FUNCTION__);
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


LOCAL void Poetry_PlayMp3(void)
{
	char file_path[100] = {0};
	SCI_TRACE_LOW("guo929  Poetry_PlayMp3  662 running!");
#if OFFLINE_DATA
	{
		uint16 idx;
		if(main_option == 1){
			idx = grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id;
		}else if(main_option == 2){
			idx = atoi(favorite_infos.poetry[(((current_favorite_page-1)*4)+detail_selected_index-1)].id);
		}
		if(poetry_detail_infos.version==-1){//旧版没有version的离线数据
			sprintf(file_path,"E:/Poetry/Poetry_offline/%d/%d_%d.mp3",idx,idx,(audio_play_progress-1));
		}else{
			sprintf(file_path,"E:/Poetry/Poetry_offline/%d/%d_%d_v%d.mp3",idx,idx,(audio_play_progress-1),poetry_detail_infos.version);
		}
		// sprintf(file_path,"E:/Poetry/Poetry_offline/%d/%d_%d.mp3",idx,idx,(audio_play_progress-1));
		SCI_TRACE_LOW("guo929  Poetry_PlayMp3  file_path=%s",file_path);
	}
#else
	// {
	// 	sprintf(file_path,"E:/Poetry/%s/%d.mp3",poetry_detail_infos.id,audio_play_progress);
	// }
#endif
	if(dsl_file_exist(file_path)&&(audio_play_progress<=audio_download_complete))//确保播放的是下载完全的
	{
		SCI_TRACE_LOW("guo929  Poetry_PlayMp3  668 !");
		Poetry_StopPlayMp3();
		if(Poetry_StartPlayMp3(file_path) == 0){
		}else{//失败了的话就清零
		SCI_TRACE_LOW("guo929  Poetry_PlayMp3  673 failed !");
			audio_playing_status = 0;
			audio_play_progress = 0;
		}
	}else{//没有文件或者有文件但不是audio_download_complete确认过的完整文件，说明还没下载完成
		SCI_TRACE_LOW("guo929  Poetry_PlayMp3  678 failed !");
		audio_break_status = 1;
	}
}

LOCAL void Poetry_ParseAudioDownload( BOOLEAN success, uint32 idx) {
	mp3_download_http_running_need_close_when_close_win = 0;
	if(success&&detailwin_is_open_closed){//下载完回调的时候，需要确认详情页没被关闭
		SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload 681 running audio_download_complete=%d ready to++!",audio_download_complete);
		audio_download_complete ++;
		if((audio_play_progress==1)&&(idx == 1)){
			SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  684 audio_play_progress=%d!",audio_play_progress);
			Poetry_PlayMp3();
		}
		if((audio_break_status == 1)&&(audio_play_progress == idx)){//因为没下载或者只下载到一半导致中断的情况，在这里续上播放 
			SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  688 audio_play_progress=%d!",audio_play_progress);
			SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  689 audio_break_status=%d!",audio_break_status);
			audio_break_status = 0;
			Poetry_PlayMp3();
		}
		if(idx == (detail_content_line_num+1)){//意思是最后一条也下载完了 回调到这里了,如果用的是本地数据，那最后一条读完不需要任何操作啊
			SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  694 idx=%d!",idx);
			SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  695 detail_content_line_num=%d!",detail_content_line_num);
			audio_download_progress = 0;
			if(local_file_load_failed){
				if(dsl_tfcard_exist() && dsl_tfcard_get_free_kb() > 100 * 1024){
					char file_path[80] = {0};
					char * data_buf = PNULL;
					uint32 file_len = 0;
					char *temp_file_path =PNULL;

					temp_file_path= SCI_ALLOCA(80 * sizeof(char));
					memset(temp_file_path, 0, 80);

					sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d_temp.bin",atoi(poetry_detail_infos.id),atoi(poetry_detail_infos.id));
					strcpy(file_path,temp_file_path);
					SCI_FREE(temp_file_path);
					if(dsl_file_exist(file_path)){
						data_buf = dsl_file_data_read(file_path, &file_len);
						if((data_buf!=PNULL) && (file_len > 0)){
							char file_path[80] = {0};
							uint32 file_len = 0;
							char *temp_file_path =PNULL;

							temp_file_path= SCI_ALLOCA(80 * sizeof(char));
							memset(temp_file_path, 0, 80);

							sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d.bin",atoi(poetry_detail_infos.id),atoi(poetry_detail_infos.id));
							strcpy(file_path,temp_file_path);
							SCI_FREE(temp_file_path);
							SCI_TRACE_LOW("guo930 delete 6 file_path=%s",file_path);
							dsl_file_delete(file_path);
							{
								cJSON *root = cJSON_Parse(data_buf);
								char *out = cJSON_PrintUnformatted(root);
							
								dsl_file_data_write(out, strlen(out), file_path);
								local_file_load_failed = 0;
								SCI_FREE(out);
								cJSON_Delete(root);
							}
						}
						if(data_buf != PNULL){
							SCI_FREE(data_buf);
							data_buf = PNULL;
						}
						SCI_TRACE_LOW("guo930 delete 7 file_path=%s",file_path);
						dsl_file_delete(file_path);
					}
				}
			}
		}else{
			SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  698 audio_download_progress=%d!",audio_download_progress);
			audio_download_progress ++;
			startDownloadAudio();
		}
	}else if(detailwin_is_open_closed){
		SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  712  call back failed success = %ddetailwin_is_open_closed=%d!!!",success);
		SCI_TRACE_LOW("guo929  Poetry_ParseAudioDownload  712  call back failed detailwin_is_open_closed=%d!!!",detailwin_is_open_closed);
		if(re_download_ten_times_limits > 2){//加个次数限制一下，防止没网时死循环死机
			re_download_ten_times_limits --;
			if(MMK_IsOpenWin(POETRY_DETAIL_WIN_ID)){
				faildReDownloadAudio(idx);
			}
		}
	}
}

LOCAL void faildReDownloadAudio(uint32 idx) {//总是莫名其妙下载失败,只能这样暴力无限重试
	char *url = SCI_ALLOCA(255 * sizeof(char));
	char file_path[100] = {0};
	// audio_is_playing_cant_back_in_2s = 1;
	// auto_play_cant_back_timer();
	memset(url, 0, 255);
	if(dsl_tfcard_exist() && dsl_tfcard_get_free_kb() > 100 * 1024){
		if(idx == 1){
			sprintf(url,"http://%s",poetry_detail_infos.titleAudio);
			// sprintf(file_path,"E:/Poetry/%s/%d.mp3",poetry_detail_infos.id,idx);
		}else{
			sprintf(url,"http://%s",poetry_detail_infos.audio[idx-2].audio);
			// sprintf(file_path,"E:/Poetry/%s/%d.mp3",poetry_detail_infos.id,idx);
		}
		sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d_v%d.mp3",poetry_detail_infos.id,poetry_detail_infos.id,(idx-1),poetry_detail_infos.version);
		if(dsl_file_exist(file_path))
		{
			SCI_TRACE_LOW("guo930 delete 8 file_path=%s",file_path);
			dsl_file_delete(file_path);//todo 这样没必要吧，本地有就直接播放就好了啊，后面再优化
		}
		mp3_download_http_running_need_close_when_close_win = 1;
		//RB_StartHttpRequest(url, file_path,3 * 20 * 1000, idx,Poetry_ParseAudioDownload);//出师表太长，下载慢，60秒
	}else{
		MMIREADBOY_CreateListeningTipWin(PALYER_PLAY_NO_SPACE_TIP);
	}
	SCI_FREE(url);
}

LOCAL void Poetry_ParseAudioUpdate( BOOLEAN success, uint32 idx) {
	mp3_download_http_running_need_close_when_close_win = 0;
	if(success&&detailwin_is_open_closed){
		if(idx == update_detail_infos.mp3num){//意思是最后一条也下载完了 回调到这里了
			{
				char file_path[80] = {0};
				char * data_buf = PNULL;
				uint32 file_len = 0;
				char *temp_file_path =PNULL;

				temp_file_path= SCI_ALLOCA(80 * sizeof(char));
				memset(temp_file_path, 0, 80);

				sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d_temp.bin",atoi(update_detail_infos.id),atoi(update_detail_infos.id));
				strcpy(file_path,temp_file_path);
				SCI_FREE(temp_file_path);

				data_buf = dsl_file_data_read(file_path, &file_len);
				{
					char file_path[80] = {0};
					uint32 file_len = 0;
					char *temp_file_path =PNULL;

					temp_file_path= SCI_ALLOCA(80 * sizeof(char));
					memset(temp_file_path, 0, 80);

					sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d.bin",atoi(update_detail_infos.id),atoi(update_detail_infos.id));
					strcpy(file_path,temp_file_path);
					SCI_FREE(temp_file_path);
					SCI_TRACE_LOW("guo930 delete 9 file_path=%s",file_path);
					dsl_file_delete(file_path);
					{
						cJSON *root = cJSON_Parse(data_buf);
						char *out = cJSON_PrintUnformatted(root);
					
						dsl_file_data_write(out, strlen(out), file_path);
						SCI_FREE(out);
						cJSON_Delete(root);
					}
				}
				if(data_buf != PNULL){
					SCI_FREE(data_buf);
					data_buf = PNULL;
				}
				SCI_TRACE_LOW("guo930 delete 10 file_path=%s",file_path);
				dsl_file_delete(file_path);
			}
		}else{
			startUpdateMp3((idx+1));
		}
	}else if(detailwin_is_open_closed){
		if(update_re_download_limits > 2){
			update_re_download_limits --;
			startUpdateMp3(idx);
		}
	}
}

LOCAL void startUpdateMp3(uint16 idx){
	char *url = SCI_ALLOCA(255 * sizeof(char));
	char file_path[100] = {0};
	memset(url, 0, 255);
	if(dsl_tfcard_exist() && dsl_tfcard_get_free_kb() > 100 * 1024){
		if(idx == 0){//标题
			sprintf(url,"http://%s",update_detail_infos.titleAudio);//"E:/Poetry/Poetry_offline/%d/%d_%d_v%d.mp3"
			// sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d_v%d.mp3",update_detail_infos.id,update_detail_infos.id,idx,update_detail_infos.version);
		}else{
			sprintf(url,"http://%s",update_detail_infos.audio[idx-1].audio);//"E:/Poetry/Poetry_offline/%d/%d_%d_v%d.mp3"
			// sprintf(file_path,"E:/Poetry/%s/%d_v%d.mp3",update_detail_infos.id,idx,update_detail_infos.version);
		}
		sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d_v%d.mp3",update_detail_infos.id,update_detail_infos.id,idx,update_detail_infos.version);
		if(dsl_file_exist(file_path)){
			SCI_TRACE_LOW("guo930 delete 11 file_path=%s",file_path);
			dsl_file_delete(file_path);
		}
		if(MMK_IsOpenWin(POETRY_DETAIL_WIN_ID)){
			mp3_download_http_running_need_close_when_close_win = 1;
			//RB_StartHttpRequest(url, file_path,30 * 1000, idx,Poetry_ParseAudioUpdate);
		}
	}
	SCI_FREE(url);
}

LOCAL void startDownloadAudio(void)
{
	char *url = SCI_ALLOCA(255 * sizeof(char));
	char file_path[100] = {0};
	re_download_ten_times_limits = 4;
	memset(url, 0, 255);
	if(dsl_tfcard_exist() && dsl_tfcard_get_free_kb() > 100 * 1024){
		if(local_file_load_failed){
			if(audio_download_progress == 1){
				sprintf(url,"http://%s",poetry_detail_infos.titleAudio);
			}else{
				sprintf(url,"http://%s",poetry_detail_infos.audio[audio_download_progress-2].audio);
			}
			sprintf(file_path,"E:/Poetry/Poetry_offline/%s/%s_%d_v%d.mp3",poetry_detail_infos.id,poetry_detail_infos.id,(audio_download_progress-1),poetry_detail_infos.version);
			if(dsl_file_exist(file_path))
			{
				SCI_TRACE_LOW("guo930 delete 13 file_path=%s",file_path);
				dsl_file_delete(file_path);//todo 这样没必要吧，本地有就直接播放就好了啊，后面再优化,不行，本地的可能是残缺的
			}
			{
				mp3_download_http_running_need_close_when_close_win = 1;
				//RB_StartHttpRequest(url, file_path,2 * 20 * 1000, audio_download_progress,Poetry_ParseAudioDownload);//出师表太长，下载慢，60秒
			}
		}else{
			{
				char file_path[80] = {0};
				char *temp_file_path =PNULL;
				uint16 idx;

				if(main_option == 1){
					idx = grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id;
				}else if(main_option == 2){
					idx = atoi(favorite_infos.poetry[(((current_favorite_page-1)*4)+detail_selected_index-1)].id);
				}
				temp_file_path= SCI_ALLOCA(80 * sizeof(char));
				memset(temp_file_path, 0, 80);
				if(poetry_detail_infos.version==-1){//旧版没有version的离线数据
					sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d_%d.mp3",idx,idx,(audio_download_progress-1));
				}else{
					sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d_%d_v%d.mp3",idx,idx,(audio_download_progress-1),poetry_detail_infos.version);
				}
				strcpy(file_path,temp_file_path);
				SCI_TRACE_LOW("guo9292 startDownloadAudio file_path=%s",file_path);
				SCI_FREE(temp_file_path);
				if(dsl_file_exist(file_path))
				{
					SCI_TRACE_LOW("guo9292 startDownloadAudio file exit calling Poetry_ParseAudioDownload");
					SCI_TRACE_LOW("guo9292 startDownloadAudio audio_download_progress=%d",audio_download_progress);
					Poetry_ParseAudioDownload(1,audio_download_progress);
				}
			}
		}
	}else{
		MMIREADBOY_CreateListeningTipWin(PALYER_PLAY_NO_SPACE_TIP);
	}
	SCI_FREE(url);
}



LOCAL void ClickTimeOut(uint8 timer_id, uint32 param) {
	if (0 != is_can_click) {
		MMK_StopTimer(is_can_click);
		is_can_click= 0;
	}
}

LOCAL uint8 GetCanntClick(uint is_click) 
{
	if(0 == is_can_click)
	{
		if(is_click)//传1是设置timer的，传0是查询timer的
		{
			is_can_click = MMK_CreateTimerCallback(300, ClickTimeOut, 0, FALSE);
			MMK_StartTimerCallback(is_can_click, 300,ClickTimeOut,0,FALSE);
		}
		return 0;//0就可以点，只要is_can_click是0就能点
	}
	return 1;//返回1就不能点
}

LOCAL void setFavorite(uint16 poetry_id,uint8 add_or_delete){
	cJSON *root;
    char *out=PNULL;
	char json[200] = {0};

	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "F_card_id", BASE_DEVICE_IMEI);
	cJSON_AddNumberToObject(root, "F_poems_id", poetry_id);
	out = cJSON_PrintUnformatted(root);
	strncpy(json, out, strlen(out));
	if(add_or_delete){
		StartDSLHttpPostRequest("v1/card/poems/collect", json, 3000, 0,PNULL);
	}else{
		StartDSLHttpPostRequest("v1/card/poems/collect/delete", json, 3000, 0,PNULL);
	}
	
    cJSON_Delete(root);
	if (out != PNULL) {
		SCI_FREE(out);
		out = PNULL;
    }
}

LOCAL uint8 getFavoriteStatus(uint16 id){
	//todo  遍历 
	uint8 result = 0;
	return result;
}

LOCAL void parsePoetryFavoriteList(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	int i = 0;
	if (is_ok && pRcv != PNULL && Rcv_len> 2){
		cJSON *root = cJSON_Parse(pRcv);
		cJSON *responseNo= cJSON_GetObjectItem(root, "F_responseNo");

		if(responseNo->valueint==10000){
			cJSON *poetry_arr = cJSON_GetObjectItem(root, "F_poem_list");
			SCI_TRACE_LOW("guofavoritedebug parsePoetryFavoriteList ");

			if (poetry_arr != NULL && poetry_arr->type != cJSON_NULL) {
				uint16 arrysize_overflow_check = cJSON_GetArraySize(poetry_arr);
				if(arrysize_overflow_check>349){
					arrysize_overflow_check = 349;
				}
				SCI_MEMSET(&favorite_infos, 0, sizeof(DSL_POETRY_LIST_T));
				SCI_MEMSET(temp_favorite_list,0,sizeof(DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T));//set0,准备与最新拉取的收藏list完全同步
				for (i = 0; i < arrysize_overflow_check; i++) {
					cJSON *item = cJSON_GetArrayItem(poetry_arr, i);
					cJSON *id = cJSON_GetObjectItem(item, "F_poem_id");
					cJSON *title = cJSON_GetObjectItem(item, "F_title");

					strncpy((char *)favorite_infos.poetry[i].id, id->valuestring,strlen(id->valuestring));
					
					SCI_TRACE_LOW("guokuaibiao  id->valuestring is %d !",(atoi(id->valuestring)));
					temp_favorite_list->favorite[(atoi(id->valuestring))]=1;//填一下快表，因为收藏夹list拉取更新并不频繁，需要本地快表
					SCI_TRACE_LOW("guokuaibiao  temp_favorite_list->favorite[(atoi(id->valuestring))] is %d !",temp_favorite_list->favorite[(atoi(id->valuestring))]);
					strncpy((char *)favorite_infos.poetry[i].title, title->valuestring,strlen(title->valuestring));					
				}
				if(arrysize_overflow_check == 0){
					main_favorite_status = -2;//-2暂无收藏
					total_favorite_poetrynum = 0;
					favorite_page_need_refresh = 0;
					if(main_option == 2){//不用检查是否focus，用户没有东西点，点不进详情页的
						MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
					}
				}else{
					main_favorite_status = -4;//-4一切正常  -3正在加载？
					total_favorite_poetrynum = arrysize_overflow_check;
					favorite_page_need_refresh = 0;
					if(main_option == 2){//不用检查是否focus，用户没有东西点，点不进详情页的
						MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
					}
					SCI_TRACE_LOW("guofavoritedebug total_favorite_poetrynum is %d !",total_favorite_poetrynum);
				}
			}else{
				main_favorite_status = -1;//-1加载失败,  是不是该加个重试的代码？
				total_favorite_poetrynum = 0;
			}
  		}else{
			main_favorite_status = -1;
			total_favorite_poetrynum = 0;
		}
		cJSON_Delete(root);
	} else {
		main_favorite_status = -1;
		total_favorite_poetrynum = 0;
	}
#if OFFLINE_DATA
    if(pRcv != PNULL){
    	SCI_FREE(pRcv);
    	pRcv = PNULL;
    }
#endif
}

LOCAL void localParsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	int i = 0;
	if (is_ok && pRcv != PNULL && Rcv_len> 2){
   		cJSON *root = cJSON_Parse(pRcv);
		cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");
		if(responseNo->valueint==10000){
			cJSON *data = cJSON_GetObjectItem(root, "data");
			if (data != NULL && data->type != cJSON_NULL) {
				char *content_add = PNULL;
				char textspace[] = "  ";
				char textfourspace[] = "    ";
				char textreturn[] = " \n";
				cJSON *item = cJSON_GetArrayItem(data, 0);
				cJSON *id = cJSON_GetObjectItem(item, "Id");
				cJSON *title = cJSON_GetObjectItem(item, "title");
				cJSON *wenyanwen = cJSON_GetObjectItem(item, "type");
				cJSON *titleAudio = cJSON_GetObjectItem(item, "titleAudio");
				cJSON *yearsitem = cJSON_GetObjectItem(item, "dynasty");
				cJSON *years = cJSON_GetObjectItem(yearsitem, "Realname");
				cJSON *authoritem = cJSON_GetObjectItem(item, "authorDetail");
				cJSON *author = cJSON_GetObjectItem(authoritem, "name");
				cJSON *appre = cJSON_GetObjectItem(item, "appreciation");
				cJSON *note = cJSON_GetObjectItem(item, "comment");
				cJSON *content_arr = cJSON_GetObjectItem(item, "sentences");
				cJSON *version = NULL;
				version = cJSON_GetObjectItem(item, "Version");
				local_file_load_failed = 0;

				content_add = (char *)SCI_ALLOC(7000*sizeof(char));
				
				if (content_arr == NULL || content_arr->type == cJSON_NULL){
				}

				SCI_MEMSET(&poetry_detail_infos, 0, sizeof(DSL_POETRY_DETAIL_T));

				for(i = 0; i < cJSON_GetArraySize(content_arr); i++){
					cJSON *contentitem = cJSON_GetArrayItem(content_arr, i);
					cJSON *content = cJSON_GetObjectItem(contentitem, "content");
					cJSON *audio = cJSON_GetObjectItem(contentitem, "audio");
					cJSON *trans = cJSON_GetObjectItem(contentitem, "translation");

					strncpy((char *)poetry_detail_infos.content[i].content, content->valuestring, strlen(content->valuestring));
 					strncpy((char *)poetry_detail_infos.audio[i].audio, audio->valuestring, strlen(audio->valuestring));
					strncpy((char *)poetry_detail_infos.trans[i].trans, trans->valuestring, strlen(trans->valuestring));
				}
				detail_content_line_num = cJSON_GetArraySize(content_arr);

				if(version != NULL){
					poetry_detail_infos.version=version->valueint;
				}else{
					poetry_detail_infos.version = -1;//表示离线数据里的version不存在
				}
				strncpy((char *)poetry_detail_infos.id, id->valuestring, strlen(id->valuestring));
				strncpy((char *)poetry_detail_infos.title, title->valuestring, strlen(title->valuestring));
				strncpy((char *)poetry_detail_infos.titleAudio, titleAudio->valuestring, strlen(titleAudio->valuestring));
				strncpy((char *)poetry_detail_infos.years, years->valuestring,  strlen(years->valuestring));
				strncpy((char *)poetry_detail_infos.author, author->valuestring, strlen(author->valuestring));
 				strncpy((char *)poetry_detail_infos.appre, appre->valuestring, strlen(appre->valuestring)); 
				strncpy((char *)poetry_detail_infos.note, note->valuestring, strlen(note->valuestring)); 
				
				parse_detail_success_now_can_click_audio = 1;//拿到音频了，现在可以响应喇叭的点击

				wstr_title = PNULL;
				wstr_origin = PNULL;
				wstr_note = PNULL;
				wstr_trans = PNULL;
				wstr_appr = PNULL;

				wstr_title = (uint16 *)SCI_ALLOC(100*sizeof(uint16));//uint16 是两个字节
				SCI_MEMSET(wstr_title,0,200);
				GUI_UTF8ToWstr(wstr_title, 200, poetry_detail_infos.title, strlen(poetry_detail_infos.title)+1);
				text_title.wstr_len = MMIAPICOM_Wstrlen(wstr_title);
				text_title.wstr_ptr = wstr_title;


				SCI_MEMSET(content_add,0,7000);
				wstr_origin = (uint16 *)SCI_ALLOC(3500*sizeof(uint16));
				if(1){
					uint16 i ;
					char poetry[]="诗";
					uint16 this_is_poetry[10]={0};
					uint16 check_if_poetry[10]={0};
					GUI_UTF8ToWstr(&check_if_poetry, 10, wenyanwen->valuestring, strlen(wenyanwen->valuestring));
					GUI_GBToWstr(&this_is_poetry, poetry, strlen(poetry));
					for(i=0;i<10;i++){
						if(this_is_poetry[i]!=check_if_poetry[i]){
							break;
						}
					}
					if(i>8){
						poetry_detail_infos.is_wenyanwen = 0;//诗
					}else{
						poetry_detail_infos.is_wenyanwen = 1;//词或者文言文
						strcat(content_add,textspace);
						strcat(content_add,textspace);
					}
				}
				strcat(content_add,poetry_detail_infos.years);//朝代 名字 加上去
				strcat(content_add,textspace);
				strcat(content_add,poetry_detail_infos.author);
				strcat(content_add,textreturn);
				for(i=0;i<detail_content_line_num;i++){//这里是原文
					if(poetry_detail_infos.is_wenyanwen == 1){
						strcat(content_add,textfourspace);
					}
					strcat(content_add,poetry_detail_infos.content[i].content);
				}
				SCI_MEMSET(wstr_origin,0,7000);
				GUI_UTF8ToWstr(wstr_origin, 7000, content_add, strlen(content_add)+1);
				text_origin.wstr_len = MMIAPICOM_Wstrlen(wstr_origin);
				text_origin.wstr_ptr = wstr_origin;

				//注释
				wstr_note = (uint16 *)SCI_ALLOC(3500*sizeof(uint16));
				SCI_MEMSET(wstr_note,0,7000);
				GUI_UTF8ToWstr(wstr_note, 7000, poetry_detail_infos.note, strlen(poetry_detail_infos.note)+1);
				text_note.wstr_len = MMIAPICOM_Wstrlen(wstr_note);
				text_note.wstr_ptr = wstr_note;

				SCI_MEMSET(content_add,0,7000);
				wstr_trans = (uint16 *)SCI_ALLOC(3500*sizeof(uint16));
				SCI_MEMSET(wstr_trans,0,7000);
				for(i=0;i<detail_content_line_num;i++){//翻译
					strcat(content_add,textfourspace);
					strcat(content_add,poetry_detail_infos.trans[i].trans);
				}
				SCI_MEMSET(wstr_trans,0,7000);
				GUI_UTF8ToWstr(wstr_trans, 7000, content_add, strlen(content_add)+1);
				text_trans.wstr_len = MMIAPICOM_Wstrlen(wstr_trans);
				text_trans.wstr_ptr = wstr_trans;

				//赏析
				wstr_appr = (uint16 *)SCI_ALLOC(9000*sizeof(uint16));
				SCI_MEMSET(wstr_appr,0,18000);
				GUI_UTF8ToWstr(wstr_appr, 20000, poetry_detail_infos.appre, strlen(poetry_detail_infos.appre)+1);
				text_appr.wstr_len = MMIAPICOM_Wstrlen(wstr_appr);
				text_appr.wstr_ptr = wstr_appr;

				if(content_add != PNULL){
					SCI_FREE(content_add);
					content_add = PNULL;
				}
				detail_get_status = 1;
				if(MMK_IsOpenWin(POETRY_DETAIL_WIN_ID)){
					MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
				
					if(auto_play == 1){
						audio_playing_status = 1;
						audio_play_progress = 1;
						audio_download_progress = 1;
						audio_download_complete = 0;
						audio_break_status = 0;
						startDownloadAudio();
					}
					updateLocalDetail(err_id);
				}
			}else{
				local_file_load_failed = 1;
				getPoetryDetail();
			}
  		}else{
			local_file_load_failed = 1;
			getPoetryDetail();
		}
		cJSON_Delete(root);
	} else {
		local_file_load_failed = 1;
		getPoetryDetail();
	}

#if OFFLINE_DATA
	if(pRcv != PNULL){
		SCI_FREE(pRcv);
		pRcv = PNULL;
	}
#endif
}

LOCAL void parsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	int i = 0;
	if (is_ok && pRcv != PNULL && Rcv_len> 2){
   		cJSON *root = cJSON_Parse(pRcv);
		cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");
		if(responseNo->valueint==10000){
			cJSON *data = cJSON_GetObjectItem(root, "data");
			SCI_TRACE_LOW("guojiaqi  parse detail running !");

			if (data != NULL && data->type != cJSON_NULL) {
				char *content_add = PNULL;
				char textspace[] = "  ";
				char textfourspace[] = "    ";
				char textreturn[] = " \n";
				cJSON *item = cJSON_GetArrayItem(data, 0);//一次只请求一首诗
				cJSON *id = cJSON_GetObjectItem(item, "Id");
				cJSON *title = cJSON_GetObjectItem(item, "title");
				cJSON *wenyanwen = cJSON_GetObjectItem(item, "type");
				cJSON *titleAudio = cJSON_GetObjectItem(item, "titleAudio");
				cJSON *yearsitem = cJSON_GetObjectItem(item, "dynasty");
				cJSON *years = cJSON_GetObjectItem(yearsitem, "Realname");
				cJSON *authoritem = cJSON_GetObjectItem(item, "authorDetail");
				cJSON *author = cJSON_GetObjectItem(authoritem, "name");
				cJSON *appre = cJSON_GetObjectItem(item, "appreciation");
				cJSON *note = cJSON_GetObjectItem(item, "comment");
				cJSON *content_arr = cJSON_GetObjectItem(item, "sentences");
				cJSON *version = cJSON_GetObjectItem(item, "Version");

				content_add = (char *)SCI_ALLOC(7000*sizeof(char));
				
				if (content_arr == NULL || content_arr->type == cJSON_NULL){
					SCI_TRACE_LOW("guojiaqi  parse 275 !");
				}
				SCI_TRACE_LOW("guojiaqi  parse 273  %d !",cJSON_GetArraySize(content_arr));

				SCI_MEMSET(&poetry_detail_infos, 0, sizeof(DSL_POETRY_DETAIL_T));

				for(i = 0; i < cJSON_GetArraySize(content_arr); i++){
					cJSON *contentitem = cJSON_GetArrayItem(content_arr, i);
					cJSON *content = cJSON_GetObjectItem(contentitem, "content");
					cJSON *audio = cJSON_GetObjectItem(contentitem, "audio");
					cJSON *trans = cJSON_GetObjectItem(contentitem, "translation");

					SCI_TRACE_LOW("guojiaqi  parse detail i=%d !",i);

					strncpy((char *)poetry_detail_infos.content[i].content, content->valuestring, strlen(content->valuestring));
					strncpy((char *)poetry_detail_infos.audio[i].audio, audio->valuestring, strlen(audio->valuestring));
					strncpy((char *)poetry_detail_infos.trans[i].trans, trans->valuestring, strlen(trans->valuestring));
				}
				detail_content_line_num = cJSON_GetArraySize(content_arr);

				poetry_detail_infos.version=version->valueint;
				strncpy((char *)poetry_detail_infos.id, id->valuestring, strlen(id->valuestring));
				strncpy((char *)poetry_detail_infos.title, title->valuestring, strlen(title->valuestring));
				strncpy((char *)poetry_detail_infos.titleAudio, titleAudio->valuestring, strlen(titleAudio->valuestring));
				strncpy((char *)poetry_detail_infos.years, years->valuestring, strlen(years->valuestring));
				strncpy((char *)poetry_detail_infos.author, author->valuestring, strlen(author->valuestring));
 				strncpy((char *)poetry_detail_infos.appre, appre->valuestring, strlen(appre->valuestring)); 
				strncpy((char *)poetry_detail_infos.note, note->valuestring, strlen(note->valuestring)); 
				
				parse_detail_success_now_can_click_audio = 1;//拿到音频了，现在可以响应喇叭的点击

				wstr_title = PNULL;
				wstr_origin = PNULL;
				wstr_note = PNULL;
				wstr_trans = PNULL;
				wstr_appr = PNULL;

				wstr_title = (uint16 *)SCI_ALLOC(100*sizeof(uint16));//uint16 是两个字节
				SCI_MEMSET(wstr_title,0,200);
				GUI_UTF8ToWstr(wstr_title, 200, poetry_detail_infos.title, strlen(poetry_detail_infos.title)+1);
				text_title.wstr_len = MMIAPICOM_Wstrlen(wstr_title);
				text_title.wstr_ptr = wstr_title;

				SCI_MEMSET(content_add,0,7000);
				wstr_origin = (uint16 *)SCI_ALLOC(3500*sizeof(uint16));
				if(1){
					uint16 i ;
					char poetry[]="诗";
					uint16 this_is_poetry[10]={0};
					uint16 check_if_poetry[10]={0};
					GUI_UTF8ToWstr(&check_if_poetry, 10, wenyanwen->valuestring, strlen(wenyanwen->valuestring));
					GUI_GBToWstr(&this_is_poetry, poetry, strlen(poetry));
					for(i=0;i<10;i++){
						if(this_is_poetry[i]!=check_if_poetry[i]){
							break;
						}
					}
					if(i>8){
						poetry_detail_infos.is_wenyanwen = 0;//诗
					}else{
						poetry_detail_infos.is_wenyanwen = 1;//词或者文言文
						strcat(content_add,textspace);
						strcat(content_add,textspace);
					}
				}
				strcat(content_add,poetry_detail_infos.years);//朝代 名字 加上去
				strcat(content_add,textspace);
				strcat(content_add,poetry_detail_infos.author);
				strcat(content_add,textreturn);
				for(i=0;i<detail_content_line_num;i++){//这里是原文
					if(poetry_detail_infos.is_wenyanwen == 1){
						strcat(content_add,textfourspace);
					}
					strcat(content_add,poetry_detail_infos.content[i].content);
				}
				SCI_MEMSET(wstr_origin,0,7000);
				GUI_UTF8ToWstr(wstr_origin, 7000, content_add, strlen(content_add)+1);
				text_origin.wstr_len = MMIAPICOM_Wstrlen(wstr_origin);
				text_origin.wstr_ptr = wstr_origin;

				//注释
				wstr_note = (uint16 *)SCI_ALLOC(3500*sizeof(uint16));
				SCI_MEMSET(wstr_note,0,7000);
				GUI_UTF8ToWstr(wstr_note, 7000, poetry_detail_infos.note, strlen(poetry_detail_infos.note)+1);
				text_note.wstr_len = MMIAPICOM_Wstrlen(wstr_note);
				text_note.wstr_ptr = wstr_note;

				SCI_MEMSET(content_add,0,7000);
				wstr_trans = (uint16 *)SCI_ALLOC(3500*sizeof(uint16));
				SCI_MEMSET(wstr_trans,0,7000);
				for(i=0;i<detail_content_line_num;i++){//翻译
					strcat(content_add,textfourspace);
					strcat(content_add,poetry_detail_infos.trans[i].trans);
				}
				SCI_MEMSET(wstr_trans,0,7000);//todo 这些wstr、text、content_add、GUI_UTF8ToWstr的大小还需要更多测试，记得把宏里的大小也改了
				GUI_UTF8ToWstr(wstr_trans, 7000, content_add, strlen(content_add)+1);
				text_trans.wstr_len = MMIAPICOM_Wstrlen(wstr_trans);
				text_trans.wstr_ptr = wstr_trans;

				//赏析
				wstr_appr = (uint16 *)SCI_ALLOC(9000*sizeof(uint16));
				SCI_MEMSET(wstr_appr,0,18000);
				GUI_UTF8ToWstr(wstr_appr, 20000, poetry_detail_infos.appre, strlen(poetry_detail_infos.appre)+1);
				text_appr.wstr_len = MMIAPICOM_Wstrlen(wstr_appr);
				text_appr.wstr_ptr = wstr_appr;

				if(content_add != PNULL){
					SCI_FREE(content_add);
					content_add = PNULL;
				}
				detail_get_status = 1;
				MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
				//todo检查一下本地的，新建
				if(dsl_tfcard_exist() && dsl_tfcard_get_free_kb() > 100 * 1024){
					char file_path[80] = {0};
					uint32 file_len = 0;
					char *temp_file_path =PNULL;
					char *out = PNULL;

					temp_file_path= SCI_ALLOCA(80 * sizeof(char));
					memset(temp_file_path, 0, 80);

					sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d_temp.bin",atoi(poetry_detail_infos.id),atoi(poetry_detail_infos.id));
					strcpy(file_path,temp_file_path);
					SCI_FREE(temp_file_path);

					if(dsl_file_exist(file_path))
					{
						SCI_TRACE_LOW("guo930 delete 16 file_path=%s",file_path);
						dsl_file_delete(file_path);
					}
					out = cJSON_PrintUnformatted(root);
					dsl_file_data_write(out, strlen(out), file_path);
					SCI_FREE(out);
				}
				if(auto_play == 1){
					audio_playing_status = 1;
					audio_play_progress = 1;
					audio_download_progress = 1;
					audio_download_complete = 0;
					audio_break_status = 0;
					startDownloadAudio();
				}
			}else{
				detail_get_status = 2;//2加载失败
				MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
			}
  		}else{
			detail_get_status = 2;//2加载失败
			MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}
		cJSON_Delete(root);
	} else {
		detail_get_status = 2;//2加载失败
		MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
	}
	abs_http_running_need_close_when_close_win = 0;
	if((get_detail_fail_retry_times_limit>2)&&(detail_get_status == 2)){
		get_detail_fail_retry_times_limit --;
		getPoetryDetail();
	}else if((get_detail_fail_retry_times_limit<=2)&&(detail_get_status == 2)){
		detail_get_status = 3;//加载失败，并且自动重试也失败
	}
}

LOCAL void getPoetry(void) {//获取家长添加的古诗词列表
    char *POETRY_LIST_URL = "v1/card/poems";
	//char *url = getUrl(POETRY_LIST_URL);
    //StartDSLHttpRequest(url, 3000, 0, parsePoetryList);
	//SCI_FREE(url);
}

LOCAL void getFavoritePoetry(void)
{
	char file_path[80] = {0};
	char * data_buf = PNULL;
	uint32 file_len = 0;
	char *temp_file_path = "E:/Poetry/Poetry_offline/favorite_list.bin";

	strcpy(file_path,temp_file_path);
	if(dsl_file_exist(file_path)){
		data_buf = dsl_file_data_read(file_path, &file_len);
	}else{
		SCI_TRACE_LOW("guooffline %s: file_path = %s not exist !!", __FUNCTION__, file_path);
		return;
	}
	SCI_TRACE_LOW("guooffline %s: file_len = %d", __FUNCTION__, file_len);
	if(data_buf != PNULL && file_len > 0)
	{
		parsePoetryFavoriteList(1, data_buf, file_len,0);
	}
}

LOCAL int StrReplace(char strRes[],char from[], char to[]) 
{
    int i,flag = 0;
    char *p,*q,*ts;
    for(i = 0; strRes[i]; ++i) 
    {
        if(strRes[i] == from[0]) 
        {
            p = strRes + i;
            q = from;
            while(*q && (*p++ == *q++));
            if(*q == '\0') 
            {
                ts = (char *)malloc(strlen(strRes) + 1);
                strcpy(ts,p);
                strRes[i] = '\0';
                strcat(strRes,to);
                strcat(strRes,ts);
                free(ts);
                flag = 1;
            }
        }
    }
    return flag;
}

//这个是在后台运行的，只起更新作用
LOCAL void updateParsePoetryDetail(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{//phonedata直接保存的话，在最后总有一块脏数据
	int i = 0;
	abs_http_running_need_close_when_close_win = 0;
	if (is_ok && pRcv != PNULL && Rcv_len> 2){
   		cJSON *root = cJSON_Parse(pRcv);
		cJSON *responseNo= cJSON_GetObjectItem(root, "response_code");

		if(responseNo->valueint==10000){
			cJSON *data = cJSON_GetObjectItem(root, "data");

			if (data != NULL && data->type != cJSON_NULL) {
				cJSON *item = cJSON_GetArrayItem(data, 0);
				cJSON *content_arr = cJSON_GetObjectItem(item, "sentences");
				cJSON *version = cJSON_GetObjectItem(item, "Version");
				cJSON *id = cJSON_GetObjectItem(item, "Id");
				cJSON *titleAudio = cJSON_GetObjectItem(item, "titleAudio");
				char *out = cJSON_PrintUnformatted(root);
				SCI_MEMSET(&update_detail_infos, 0, sizeof(DSL_POETRY_UPDATE_DETAIL_T));
				for(i = 0; i < cJSON_GetArraySize(content_arr); i++){
					cJSON *contentitem = cJSON_GetArrayItem(content_arr, i);
					cJSON *audio = cJSON_GetObjectItem(contentitem, "audio");


					strncpy((char *)update_detail_infos.audio[i].audio, audio->valuestring,
						strlen(audio->valuestring));
				}
				update_detail_infos.version=version->valueint;
				strncpy((char *)update_detail_infos.id, id->valuestring,
						strlen(id->valuestring));
				strncpy((char *)update_detail_infos.titleAudio, titleAudio->valuestring,
						strlen(titleAudio->valuestring));
				update_detail_infos.mp3num = cJSON_GetArraySize(content_arr);

				if(dsl_tfcard_exist()){
					char file_path[80] = {0};
					char * data_buf = PNULL;
					uint32 file_len = 0;

					char *temp_file_path =PNULL;

					temp_file_path= SCI_ALLOCA(80 * sizeof(char));
					memset(temp_file_path, 0, 80);

					sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d.bin",poetry_cur_post_poem_id,poetry_cur_post_poem_id);
					strcpy(file_path,temp_file_path);
					SCI_FREE(temp_file_path);
					SCI_TRACE_LOW("guo9302 updateParsePoetryDetail file_path=%s",file_path);
					if(dsl_file_exist(file_path)){
						data_buf = dsl_file_data_read(file_path, &file_len);
						SCI_TRACE_LOW("guo9302 updateParsePoetryDetail strcmp=%d",strcmp(data_buf,out));
						SCI_TRACE_LOW("guo9302 updateParsePoetryDetail data_buf=%s",data_buf);
						SCI_TRACE_LOW("guo9302 updateParsePoetryDetail file_len=%d",file_len);
						SCI_TRACE_LOW("guo9302 updateParsePoetryDetail out=%s",out);
						if((data_buf!=PNULL) && (file_len > 0)){
							cJSON *root2 = cJSON_Parse(data_buf);
							cJSON *responseNo2= cJSON_GetObjectItem(root2, "response_code");

							if(responseNo2->valueint==10000){
								cJSON *data2 = cJSON_GetObjectItem(root2, "data");

								if (data2 != NULL && data2->type != cJSON_NULL) {
									cJSON *item2 = cJSON_GetArrayItem(data2, 0);
									cJSON *version2 = NULL;
									version2 = cJSON_GetObjectItem(item2, "Version");
									if(version2==NULL){
										if(strcmp(data_buf,out)!=0){//有更新，重新保存一份，下载一遍音频//相当不靠谱！还是用比较版本号的办法吧！,先判断version是否为空
											char file_path[80] = {0};
											uint32 file_len = 0;
											char *temp_file_path =PNULL;

											temp_file_path= SCI_ALLOCA(80 * sizeof(char));
											memset(temp_file_path, 0, 80);

											sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d_temp.bin",poetry_cur_post_poem_id,poetry_cur_post_poem_id);
											strcpy(file_path,temp_file_path);
											SCI_FREE(temp_file_path);

											if(dsl_file_exist(file_path))
											{
												dsl_file_delete(file_path);
											}
											SCI_TRACE_LOW("guo9302 2 temp.bin");
											dsl_file_data_write(out, strlen(out), file_path);
											update_re_download_limits = 4;
											if(MMK_IsOpenWin(POETRY_DETAIL_WIN_ID)){
												startUpdateMp3(0);
											}
										}
									}else if(version->valueint != version2->valueint){
										if(strcmp(data_buf,out)!=0){//有更新，重新保存一份，下载一遍音频//相当不靠谱！还是用比较版本号的办法吧！,先判断version是否为空
											char file_path[80] = {0};
											uint32 file_len = 0;
											char *temp_file_path =PNULL;

											temp_file_path= SCI_ALLOCA(80 * sizeof(char));
											memset(temp_file_path, 0, 80);

											sprintf(temp_file_path,"E:/Poetry/Poetry_offline/%d/%d_temp.bin",poetry_cur_post_poem_id,poetry_cur_post_poem_id);
											strcpy(file_path,temp_file_path);
											SCI_FREE(temp_file_path);

											if(dsl_file_exist(file_path))
											{
												dsl_file_delete(file_path);
											}
											SCI_TRACE_LOW("guo9302 2 temp.bin");
											dsl_file_data_write(out, strlen(out), file_path);
											update_re_download_limits = 4;
											if(MMK_IsOpenWin(POETRY_DETAIL_WIN_ID)){
												startUpdateMp3(0);
											}
										}
									}
								}
							}
						cJSON_Delete(root2);
						}
						if(data_buf != PNULL){
							SCI_FREE(data_buf);
							data_buf = PNULL;
						}
					}
				}
				SCI_FREE(out);
			}
  		}
		cJSON_Delete(root);
	}
}

LOCAL void updateLocalDetail(uint16 idx)
{
	char *url = SCI_ALLOCA(255 * sizeof(char));
	memset(url, 0, 255);
#if TEST_API
	sprintf(url,"v1/card/poemDetail?F_poem_id=%d",idx);
#else
	sprintf(url,"v1/card/poemDetail?F_poem_id=%d",idx);
#endif
	abs_http_running_need_close_when_close_win = 1;
	poetry_cur_post_poem_id = idx;
	MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, updateParsePoetryDetail);
	SCI_FREE(url);
}

LOCAL void getPoetryDetail(void)
{
	uint16 id = 0;
    char fav_id[15] = {0};
	char *url = SCI_ALLOCA(255 * sizeof(char));
	memset(url, 0, 255);
	local_file_load_failed = 1;//既然优先使用本地数据，那么需要拉取在线数据就说明本地数据损坏/不存在
	if(main_option == 1)
	{
		id = grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id;
	#if TEST_API
		sprintf(url,"v1/card/poemDetail?F_poem_id=%d", id);// 改成测试版的测试一下版本号
	#else
		sprintf(url,"v1/card/poemDetail?F_poem_id=%d", id);
	#endif
	}
	else if(main_option == 2)
	{
		strcpy(fav_id, favorite_infos.poetry[(((current_favorite_page-1)*4)+detail_selected_index-1)].id);
	#if TEST_API
		sprintf(url,"v1/card/poemDetail?F_poem_id=%s", fav_id);
	#else
		sprintf(url,"v1/card/poemDetail?F_poem_id=%s", fav_id);
	#endif
	}
	abs_http_running_need_close_when_close_win = 1;
#if TEST_API
	MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_TEST_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, parsePoetryDetail);
#else
	MMIZDT_HTTP_AppSend(TRUE, BASE_POETRY_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, parsePoetryDetail);
#endif
  	//close_win_abs_http_context_ptr = StartAbslutoUrlHttpRequest(url, 3000, 0, parsePoetryDetail);
	SCI_FREE(url);
}


LOCAL void PoetryDetail_DrawOption(MMI_WIN_ID_T win_id, uint16 current_option)
{
    uint8 i = 0;
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T optin_rect = {0};
    GUI_RECT_T optin_hor_line_rect = {0};
    GUI_RECT_T optin_option_line_rect = {0};
    MMI_TEXT_ID_T text_id[4] = {READBOY_TXT_POETRY_CONTENT_OPTIONS, READBOY_TXT_POETRY_EXPRESSION_OPTIONS,
        READBOY_TXT_POETRY_TRANSLATE_OPTIONS, READBOY_TXT_POETRY_APPRECIATION_OPTIONS
    };

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
  
    optin_rect = poetry_bottom_option_rect;
    for(i = 0; i < 4;i++){
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
    optin_option_line_rect = poetry_bottom_option_rect_array[current_option-1];
    optin_option_line_rect.bottom -= 2;
    optin_option_line_rect.top = optin_option_line_rect.bottom - 1;
    LCD_FillRect(&lcd_dev_info, optin_option_line_rect, MMI_BLACK_COLOR);

    optin_hor_line_rect = poetry_bottom_option_rect;
    optin_hor_line_rect.right = poetry_bottom_option_rect_array[3].right;
    optin_hor_line_rect.top -= 3;
    optin_hor_line_rect.bottom = optin_hor_line_rect.top + 1;
    LCD_DrawHLine(&lcd_dev_info, optin_hor_line_rect.left, optin_hor_line_rect.bottom, optin_hor_line_rect.right, MMI_BLACK_COLOR);
}
LOCAL void Draw_PageNum(uint8 study_favorite){
	uint16 wtmp[5] = {0};
	uint16 tmp[5] = {0};	
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_str = {0};

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_16;
    text_style.font_color = MMI_BLACK_COLOR;
     if(study_favorite)//学习
     {
		if(total_poetrynum[(grade_display_status-1)] % 4){
			pagenum = ((total_poetrynum[(grade_display_status-1)]/4)+1);
		}else{
			pagenum = (total_poetrynum[(grade_display_status-1)]/4);
		}
		if(currentpage > pagenum){//如果因为取消收藏、取消添加诗词导致当前页码大于最大页码了，就改一下
			currentpage = pagenum;
		}
		SCI_MEMSET(wtmp,0,5);
		SCI_MEMSET(tmp,0,5);
		sprintf(tmp,"%d/%d",currentpage, pagenum);
		GUI_GBToWstr(wtmp,tmp,strlen(tmp)+1);
		text_str.wstr_ptr=wtmp;
		text_str.wstr_len= MMIAPICOM_Wstrlen(wtmp);
	}
       else//收藏夹
	{
		if(total_favorite_poetrynum % 4){
			favorite_pagenum= ((total_favorite_poetrynum/4)+1);
		}else{
			favorite_pagenum= (total_favorite_poetrynum/4);
		}
		if(current_favorite_page>favorite_pagenum){
			current_favorite_page = favorite_pagenum;
		}
		SCI_MEMSET(wtmp,0,5);
		SCI_MEMSET(tmp,0,5);
		sprintf(tmp,"%d/%d",current_favorite_page, favorite_pagenum);
		GUI_GBToWstr(wtmp,tmp,strlen(tmp)+1);
		text_str.wstr_ptr=wtmp;
		text_str.wstr_len= MMIAPICOM_Wstrlen(wtmp);	
	}
	GUISTR_DrawTextToLCDInRect(
            &lcd_dev_info,
            &poetry_page_rect,
            &poetry_page_rect,
            &text_str,
            &text_style,
            GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO
        );
}

LOCAL void DrawPoetryList(uint16 study_favorite)
{
	uint8 i = 0;
	uint8 j = 0;
	uint16 current_page_num;
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
	GUISTR_STYLE_T text_style = {0};
	GUI_RECT_T rect_line = {0};

	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_16;
	text_style.font_color = MMI_BLACK_COLOR;
    
	rect_line = poetry_item_line_rect;	
	{
		uint16 wstr[40] = {0};
		MMI_STRING_T text = {0};
		if(study_favorite == 1)//主页
		{
			if(total_poetrynum[(grade_display_status-1)] < 5){
				current_page_num = total_poetrynum[(grade_display_status-1)];
			}else if (currentpage < pagenum){
				current_page_num = 4;
			}else{
				current_page_num = (total_poetrynum[(grade_display_status-1)] - (pagenum*4 - 4)); 
			}
		}else//收藏页
		{
			if(total_favorite_poetrynum < 5){
				current_page_num = total_favorite_poetrynum;//判断一下当前页有几条诗需要画
			}else if (current_favorite_page < favorite_pagenum){
				current_page_num = 4;
			}else{
				current_page_num = (total_favorite_poetrynum - (favorite_pagenum*4 - 4)); 
			}
		}
		for(i = 4;i > 0;i--)
		{
			if(current_page_num <= 0){
				break;
			}
			poetry_item_line_rect_array[j] = rect_line;
			LCD_DrawRect(&lcd_dev_info,rect_line,MMI_BLACK_COLOR);
			SCI_MEMSET(wstr,0,40);
			if(study_favorite == 1){
				GUI_UTF8ToWstr(wstr, 40, grade_all_list.all_grade[grade_display_status-1].grade_info[((currentpage*4)-i)].title, strlen(grade_all_list.all_grade[grade_display_status-1].grade_info[((currentpage*4)-i)].title)+1);
			}else{
				GUI_UTF8ToWstr(wstr, 40, favorite_infos.poetry[((current_favorite_page*4)-i)].title, strlen(favorite_infos.poetry[((current_favorite_page*4)-i)].title)+1);
			}
			text.wstr_len = MMIAPICOM_Wstrlen(wstr);
			text.wstr_ptr = wstr;
			GUISTR_DrawTextToLCDInRect(
			(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
				&rect_line,
				&rect_line,
				&text,
				&text_style,
				text_state,
				GUISTR_TEXT_DIR_AUTO
			);
			current_page_num--;
			j++;
			rect_line.top += 2*POETRY_LINE_HIGHT;
			rect_line.bottom = rect_line.top + 1.5*POETRY_LINE_HIGHT;
		}
	}
}

LOCAL void Main_LoadPrePage(void){
	if(main_option == 1){
		do{
		if(currentpage == 1){break;}
		currentpage --;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}while(0);
	}else if(main_option == 2){
		do{
		if(current_favorite_page == 1){break;}
		current_favorite_page --;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}while(0);
	}
	
}
LOCAL void Main_LoadNextPage(void){
	if(main_option == 1){
		do{
		if(currentpage == pagenum){break;}
		currentpage ++;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}while(0);
	}else if(main_option == 2){
		do{
		if(current_favorite_page == favorite_pagenum){break;}
		current_favorite_page ++;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
		}while(0);
	}
}
LOCAL void Detail_LoadPrePage(void){
	
}
LOCAL void Detail_LoadNextPage(void){
	
}

LOCAL void Detail_If_Favorite_Changed_Than_Post(void){
    if(detail_current_poetry_favorite_status != detail_current_poetry_favorite_status_has_changed)
    {
		cJSON *root;
		char *out = PNULL;
		char json[300] = {0};
		char url[200] = {0};

		favorite_page_need_refresh = 1;
		MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);

		root = cJSON_CreateObject();
		cJSON_AddStringToObject(root, "F_card_id", BASE_DEVICE_IMEI);
		if(main_option == 1){
			char tempstr[300]={0};
			sprintf(tempstr, "%d", grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id);
			cJSON_AddStringToObject(root, "F_poems_id", tempstr);
		}else if (main_option == 2){
			cJSON_AddStringToObject(root, "F_poems_id", favorite_infos.poetry[(((current_favorite_page-1)*4)+detail_selected_index-1)].id);

		}
		out = cJSON_PrintUnformatted(root);
		strncpy(json, out, strlen(out));
		if(detail_current_poetry_favorite_status_has_changed){//原来是收藏那就删除
		#if OFFLINE_DATA  //todo 判断mainoption啊 可能是在收藏夹里删除的
			Offline_favorite_list_delete();
		#else
			sprintf(url, "%s%s", BASE_URL_PATH, "v1/card/poems/collect/delete");
			MMIZDT_HTTP_AppSend(FALSE, url, json, strlen(json), 1000, 0, 0, 0, 0, 0, favorite_post_handle);
			// StartDSLHttpPostRequest("v1/card/poems/collect/delete", json, 3000, 0,favorite_post_handle);
		#endif
		}else{//原来没收藏那就添加
		#if OFFLINE_DATA
			Offline_favorite_list_add();
		#else
			sprintf(url, "%s%s", BASE_URL_PATH, "v1/card/poems/collect");
			MMIZDT_HTTP_AppSend(FALSE, url, json, strlen(json), 1000, 0, 0, 0, 0, 0, favorite_post_handle);
			// StartDSLHttpPostRequest("v1/card/poems/collect", json, 3000, 0,favorite_post_handle);
		#endif
		}
		
		cJSON_Delete(root);
		if (out != PNULL) {
			SCI_FREE(out);
			out = PNULL;
		}
	}
}

LOCAL void favorite_post_handle(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
	if(is_ok){
		getFavoritePoetry();
	}
}


LOCAL void PoetryGrade_HanldeTpUp(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
    if(GUI_PointIsInRect(point, poetry_grade_line_rect_array[0]))//点击了第1个框
    {
        grade_win = 0;
        grade_display_status = (grade_current_page-1)*5+1;
        if(grade_get_status[(grade_display_status-1)] != 1){//等于1说明本地有就不用再去网上要了
            get_grade_list_fail_retry_times_limit = 6;
            get_grade_list_idx = (grade_current_page-1)*5+1;
            Read_list_data_from_tf_and_parse((grade_current_page-1)*5+1);
        }
        currentpage = 1;
        MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
    else if(GUI_PointIsInRect(point, poetry_grade_line_rect_array[1]))//点击了第2个框
    {
        grade_win = 0;
        grade_display_status = (grade_current_page-1)*5+2;
        if(grade_get_status[(grade_display_status-1)] != 1){//等于1说明本地有就不用再去网上要了
            get_grade_list_fail_retry_times_limit = 6;
            get_grade_list_idx = (grade_current_page-1)*5+2;
            Read_list_data_from_tf_and_parse((grade_current_page-1)*5+2);
        }
        currentpage = 1;
        MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
    else if(GUI_PointIsInRect(point, poetry_grade_line_rect_array[2]))//点击了第3个框
    {//3
        grade_win = 0;
        grade_display_status = (grade_current_page-1)*5+3;
        if(grade_get_status[(grade_display_status-1)] != 1){//等于1说明本地有就不用再去网上要了
            get_grade_list_fail_retry_times_limit = 6;
            get_grade_list_idx = (grade_current_page-1)*5+3;
            Read_list_data_from_tf_and_parse((grade_current_page-1)*5+3);
        }
        currentpage = 1;
        MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
    }
    else if(GUI_PointIsInRect(point, poetry_grade_line_rect_array[3]))//点击了第4个框
    {//4
        if(grade_current_page != 4){
            grade_win = 0;
            grade_display_status = (grade_current_page-1)*5+4;
            if(grade_get_status[(grade_display_status-1)] != 1){//等于1说明本地有就不用再去网上要了
                get_grade_list_fail_retry_times_limit = 6;
                get_grade_list_idx = (grade_current_page-1)*5+4;
                Read_list_data_from_tf_and_parse((grade_current_page-1)*5+4);
            }
            currentpage = 1;
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_grade_line_rect_array[4]))//点击了第5个框
    {
        if(grade_current_page != 4){
            grade_win = 0;
            grade_display_status = (grade_current_page-1)*5+5;
            if(grade_get_status[(grade_display_status-1)] != 1){//等于1说明本地有就不用再去网上要了
                get_grade_list_fail_retry_times_limit = 6;
                get_grade_list_idx = (grade_current_page-1)*5+5;
                Read_list_data_from_tf_and_parse(((grade_current_page-1)*5)+5);
            }
            currentpage = 1;
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_back_rect))//返回
    {
        if(GetCanntClick(0))
        {
            return;
        }
        if(main_option == 2){
            main_option = 1;
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }else{
            MMK_CloseWin(win_id);
        }
    }
    else if (GUI_PointIsInRect(point, poetry_play_rect))//自动播放按钮
    {
        if(main_option != 2)
        {
            if(auto_play == 0){
                auto_play = 1;
                auto_play_open_close_tip = 1;
                auto_play_tip_timer();
                MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
            }else{
                auto_play = 0;
                auto_play_open_close_tip = 2;
                auto_play_tip_timer();
                MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
            }
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if (GUI_PointIsInRect(point, poetry_favorite_rect))//收藏夹
    {
        if(main_option == 1){
            main_option = 2;
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
}

LOCAL void PoetryItem_HanldeTpUp(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
    if(GUI_PointIsInRect(point, poetry_item_line_rect_array[0]))//点击了第1个框
    {
        if((main_option == 1) && (main_poetry_status == -4))
        {
            detail_selected_index = 1;
            MMI_CreatePoetryDetailWin();
        }
        else if((main_option == 2)&&(main_favorite_status == -4))
        {
            detail_selected_index = 1;
            MMI_CreatePoetryDetailWin();
        }
    }
    else if(GUI_PointIsInRect(point, poetry_item_line_rect_array[1]))//点击了第2个框
    {
        if(main_option == 1 && main_poetry_status == -4){
            if(currentpage < pagenum || (total_poetrynum[grade_display_status-1] % 4 >= 2) ||
                total_poetrynum[grade_display_status-1] % 4== 0)
            {
                detail_selected_index = 2;
                MMI_CreatePoetryDetailWin();
            }
        }
        else if(main_option == 2 && main_favorite_status == -4)
        {
            if(current_favorite_page < favorite_pagenum ||total_favorite_poetrynum % 4 >= 2 ||
                total_favorite_poetrynum % 4 == 0)
            {
                detail_selected_index = 2;
                MMI_CreatePoetryDetailWin();
            }
        }
    }
    else if(GUI_PointIsInRect(point, poetry_item_line_rect_array[2]))//点击了第3个框
    {
        if(main_option == 1 && main_poetry_status == -4){
            if(currentpage < pagenum || total_poetrynum[grade_display_status-1] % 4 >= 3 ||
                total_poetrynum[grade_display_status-1] % 4 == 0)
            {
                detail_selected_index = 3;
                MMI_CreatePoetryDetailWin();
            }
        }
        else if(main_option == 2 && main_favorite_status == -4)
        {
            if(current_favorite_page < favorite_pagenum || total_favorite_poetrynum % 4 >= 3 ||
                total_favorite_poetrynum % 4 == 0)
            {
                detail_selected_index = 3;
                MMI_CreatePoetryDetailWin();
            }
        }
    }
    else if(GUI_PointIsInRect(point, poetry_item_line_rect_array[3]))//点击了第4个框
    {
        if(main_option == 1 && main_poetry_status == -4)
        {
            if(currentpage < pagenum || total_poetrynum[grade_display_status-1] % 4 == 0)
            {
                detail_selected_index = 4;
                MMI_CreatePoetryDetailWin();
            }
        }
        else if(main_option == 2 && main_favorite_status == -4)
        {
            if(current_favorite_page < favorite_pagenum||total_favorite_poetrynum % 4 == 0)
            {
                detail_selected_index = 4;
                MMI_CreatePoetryDetailWin();
            }
        }
    }
    else if(GUI_PointIsInRect(point, poetry_back_rect))//返回
    {
        if(GetCanntClick(0)){
            return;
        }
        if(main_option == 2){
            main_option = 1;
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }else{
            grade_win = 1;
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_play_rect))//自动播放按钮
    {
        if(main_option != 2){
            if(auto_play == 0){
                auto_play = 1;
                auto_play_open_close_tip = 1;
                auto_play_tip_timer();
                MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
            }else{
                auto_play = 0;
                auto_play_open_close_tip = 2;
                auto_play_tip_timer();
                MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
            }
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_favorite_rect))//收藏夹
    {
        if(main_option == 1){
            main_option = 2;
            MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
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
				uint16 i;
				pagenum = 1;
				auto_play_open_close_tip = 0;
				favorite_pagenum = 1;
				currentpage = 1;
				current_favorite_page = 1;
				main_option = 1;
				main_poetry_status = -3;
				total_favorite_poetrynum = 0;
				detail_selected_index = 0;
				main_favorite_status = -3;
				if(temp_favorite_list == PNULL){
					temp_favorite_list = SCI_ALLOCA(sizeof(DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T));
					SCI_MEMSET(temp_favorite_list,0,sizeof(DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T));
				}else{//这样是不是严谨一点？
					SCI_FREE(temp_favorite_list);
					temp_favorite_list = PNULL;
					temp_favorite_list = SCI_ALLOCA(sizeof(DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T));
					SCI_MEMSET(temp_favorite_list,0,sizeof(DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T));
				}
				favorite_page_need_refresh = 0;
				detail_just_closed = 0;
				grade_win = 1;
				grade_current_page = 1;
				auto_play = 1;
				get_grade_list_idx = 0;
				for(i=0;i<18;i++){
					grade_get_status[i]=0;
					total_poetrynum[i] = 0;
				}
				grade_display_status = 0;
				SCI_MEMSET(&grade_all_list, 0, sizeof(DSL_POETRY_ALL_GRADE_LIST_T));
				LayoutItem(MMI_DSL_POETRY_BUTTON_REFRESH, 72, 168, 317, 346);
				InitButton(MMI_DSL_POETRY_BUTTON_REFRESH);
				GUIBUTTON_SetTextId(MMI_DSL_POETRY_BUTTON_REFRESH, READBOY_TXT_POETRY_REFRESH);
				GUIBUTTON_SetCallBackFunc(MMI_DSL_POETRY_BUTTON_REFRESH,poetry_maingradeRefresh);
				getFavoritePoetry();
			}
			break;
		case MSG_GET_FOCUS:
			auto_play_open_close_tip = 0;
			if(detail_just_closed){
				detail_just_closed = 0;
				SCI_TRACE_LOW("guobackdebug  main win got detail just closed! !");
				Detail_If_Favorite_Changed_Than_Post();
			}
			break;
		case MSG_FULL_PAINT:
			{				
				GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
				GUISTR_STYLE_T text_style = {0};
				MMI_STRING_T text_string = {0};
				uint8 num = 0;
				char num_buf[10] = {0};
				wchar num_str[10] = {0};
				
				GUI_FillRect(&lcd_dev_info, poetry_win_rect, MMI_WHITE_COLOR);
				SCI_TRACE_LOW("%s: main_option = %d, grade_win = %d", __FUNCTION__, main_option, grade_win);
				text_style.align = ALIGN_LVMIDDLE;
				text_style.font = SONG_FONT_20;
				text_style.font_color = MMI_BLACK_COLOR;
				GUIBUTTON_SetVisible(MMI_DSL_POETRY_BUTTON_REFRESH,FALSE,FALSE);
				if(main_option == 2){//收藏夹
					MMIRES_GetText(READBOY_TXT_POETRY_FAVORITE, win_id, &text_string);
				}else{
					MMIRES_GetText(READBOY_TXT_POETRY, win_id, &text_string);//"古诗词"
				}
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
				if(main_option != 2){//进入收藏页后就不再显示收藏图标了,也不再显示自动发音按钮了
					if(auto_play == 1){
						GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_POETRY_AUTOPLAY, &lcd_dev_info);//自动播放的喇叭
					}else{
						GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_AUTO_PLAY, &lcd_dev_info);//不播放的喇叭
					}
					GUIRES_DisplayImg(PNULL, &poetry_favorite_rect, PNULL, win_id, IMG_POETRY_FAVORITELIST, &lcd_dev_info);//收藏页图标
				}
				if(main_option == 1)
				{
					if(grade_win == 1){
						Draw_Grade(win_id);
						Draw_Grade_Page_Num();
					}else{
						//TODO 诗词学习和收藏夹的逻辑分开 可能有诗词但无收藏
						if(grade_get_status[(grade_display_status-1)] == 3){//加载失败 
							MMIRES_GetText(READBOY_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
						}else if(grade_get_status[(grade_display_status-1)] == 4)
						{//自动重试也失败了 展示手动刷新按钮
							MMIRES_GetText(READBOY_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
							GUIBUTTON_SetVisible(MMI_DSL_POETRY_BUTTON_REFRESH,TRUE,TRUE);
						}else if(grade_get_status[(grade_display_status-1)] == 2)
						{//暂无古诗词
							MMIRES_GetText(READBOY_TXT_POETRY_NO_INFO, win_id, &text_string);
						}else if(grade_get_status[(grade_display_status-1)] == 0)
						{//正在加载
							MMIRES_GetText(READBOY_TXT_POETRY_LOADING, win_id, &text_string);
						}else 
						{
							if(total_poetrynum[(grade_display_status-1)] != 0){//如果诗数量不等于0
								Draw_PageNum(1);
								DrawPoetryList(1);
							}
							break;
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
				}
				else//收藏夹	
				{			
					if(favorite_page_need_refresh == 0)
					{
						if(total_favorite_poetrynum == 0)
						{
							MMIRES_GetText(READBOY_TXT_POETRY_NO_FAVORITE, win_id, &text_string);                                							
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
						else
						{
							if(total_favorite_poetrynum)//如果收藏的诗数量不等于0
							{
								Draw_PageNum(0);
								DrawPoetryList(0);
							}
						}
					}
					else//favorite_page_need_refresh == 1 收藏状态更改了 需要刷新		
					{			
						MMIRES_GetText(READBOY_TXT_POETRY_LOADING, win_id, &text_string);
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
				}
				if(auto_play_open_close_tip != 0)
				{
					UILAYER_APPEND_BLT_T append_layer = {0};
					GUI_RECT_T tip_rect_line = {0};
					GUISTR_STYLE_T text_style = {0};
					MMI_STRING_T text_string = {0};
					wchar text_str[35] = {0};
					char count_str[35] = {0};

					append_layer.lcd_dev_info = poetry_auto_tip_layer;
					append_layer.layer_level = UILAYER_LEVEL_HIGH;
					UILAYER_AppendBltLayer(&append_layer);

					GUI_FillRect(&poetry_auto_tip_layer,poetry_tip_rect,0xffff);

					tip_rect_line = poetry_tip_rect;
					tip_rect_line.left += 10;
					tip_rect_line.left -= 10;
					tip_rect_line.top +=10;
					tip_rect_line.bottom -= 10;
					LCD_DrawRect(&poetry_auto_tip_layer, tip_rect_line, MMI_BLACK_COLOR);

					text_style.align = ALIGN_HVMIDDLE;
					text_style.font = SONG_FONT_16;
					text_style.font_color = MMI_BLACK_COLOR;

					if(auto_play_open_close_tip==1)
					{
						sprintf(count_str,"已开启自动朗读");
					}else if(auto_play_open_close_tip==2)
					{
						sprintf(count_str,"已关闭自动朗读");
					}
					GUI_GBToWstr(text_str, count_str, strlen(count_str));
					text_string.wstr_ptr = text_str;
					text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
					GUISTR_DrawTextToLCDInRect(
					(const GUI_LCD_DEV_INFO *)&poetry_auto_tip_layer,
						&poetry_tip_rect,
						&poetry_tip_rect,
						&text_string,
						&text_style,
						GUISTR_STATE_ALIGN,
						GUISTR_TEXT_DIR_AUTO
					);
				}
				else
				{
					UILAYER_RemoveBltLayer(&poetry_auto_tip_layer);
				}
			}
			break;
		case MSG_TP_PRESS_UP:
			{
				GUI_POINT_T point = {0};
				int16 tp_offset_x = 0;
				int16 tp_offset_y = 0;
				point.x =MMK_GET_TP_X(param);
				point.y =MMK_GET_TP_Y(param);
				tp_offset_x = point.x - main_tp_down_x;
				tp_offset_y = point.y - main_tp_down_y;
				if(ABS(tp_offset_y) <= ABS(tp_offset_x))//动态点击
				{
					if(grade_win == 1 && main_option == 1){
						if(tp_offset_x > 50){
							Grade_LoadPrePage();
							break;
						}else if(tp_offset_x < -50){
							Grade_LoadNextPage();
							break;
						}
					}else{
						if(tp_offset_x > 50){
							Main_LoadPrePage();
							break;
						}else if(tp_offset_x < -50){
							Main_LoadNextPage();
							break;
						}
					}
				}
				if(grade_win == 1 && main_option != 2)//选年级的页面
				{
					if(ABS(tp_offset_x) < 20 && ABS(tp_offset_y) < 20){//静态点击			
						PoetryGrade_HanldeTpUp(win_id, point);                       
					}
				}
				else//选诗歌的页面
				{				
					if(ABS(tp_offset_x) < 20 && ABS(tp_offset_y) < 20){//静态点击
						PoetryItem_HanldeTpUp(win_id, point);
					}
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
             case MSG_CTL_CANCEL:
                    break;
		case MSG_KEYUP_CANCEL:
			{
				if(GetCanntClick(0)){
					SCI_TRACE_LOW("%s can not click", __FUNCTION__);
					break;
				}
				if(main_option == 2){
					main_option = 1;
					MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
				}else{
					if(grade_win == 0){
						grade_win = 1;
						MMK_PostMsg(POETRY_MAIN_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
					}else{
						MMK_CloseWin(win_id);
					}
				}	
			}
			break;
		case MSG_CLOSE_WINDOW:
			{
				if(temp_favorite_list != PNULL){
					SCI_FREE(temp_favorite_list);
					temp_favorite_list = PNULL;
				}
				MMIZDT_HTTP_Close();
			}
			break;
		default:
			recode = MMI_RESULT_FALSE;
			break;
	}
	return recode;
}

LOCAL void PoetryDetail_HanldeTpUp(MMI_WIN_ID_T win_id, GUI_POINT_T point)
{
     if(GUI_PointIsInRect(point, poetry_back_rect))
     {
        if(!GetCanntClick(0))//限制一下详情页返回的速度，防止获取详情信息的http造成死机
        {
            detail_just_closed = 1;
            MMK_CloseWin(win_id);
        }
     }
    else if(GUI_PointIsInRect(point, poetry_play_rect))
    {
        if(detail_win_current_option == 1 && parse_detail_success_now_can_click_audio == 1)
        {
            if(mp3_download_http_running_need_close_when_close_win == 1){
                mp3_download_http_running_need_close_when_close_win = 0;
            }
            if (0 != poetry_player_handle)
            {
                MMISRVAUD_Stop(poetry_player_handle);
                MMISRVMGR_Free(poetry_player_handle);
                poetry_player_handle = PNULL;
            }
            audio_playing_status = 1;
            audio_play_progress = 1;
            audio_download_progress = 1;
            audio_download_complete = 0;
            audio_break_status = 0;
            startDownloadAudio();
        }
    }
    else if(GUI_PointIsInRect(point, poetry_favorite_rect))
    {
        if(detail_current_poetry_favorite_status){
            detail_current_poetry_favorite_status = 0;
            MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }else{
            detail_current_poetry_favorite_status = 1;
            MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[0]))
    {
        if (detail_win_current_option != 1){
            detail_win_current_option = 1;
            MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[1]))
    {
        if (detail_win_current_option != 2){
            detail_win_current_option = 2;
            MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[2]))
    {
        if (detail_win_current_option != 3){
            detail_win_current_option = 3;
            MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
    else if(GUI_PointIsInRect(point, poetry_bottom_option_rect_array[3]))
    {
        if (detail_win_current_option != 4){
            detail_win_current_option = 4;
            MMK_PostMsg(POETRY_DETAIL_WIN_ID, MSG_FULL_PAINT, PNULL, 0);
        }
    }
}

LOCAL MMI_RESULT_E HandlePoetryDetailWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id,DPARAM param) 
{
	MMI_RESULT_E recode = MMI_RESULT_TRUE;
	GUI_RECT_T appre_openwindow_rect = {0, 0, 1, 1};
	GUI_FONT_T text_font = SONG_FONT_16;
	GUI_COLOR_T text_color = MMI_BLACK_COLOR;
	uint16 i;

	switch (msg_id) {
	case MSG_OPEN_WINDOW: 
	{
		detail_get_status = 0;//从服务器获取诗详情的状态
		detail_win_current_option = 1;//下面的四个选项
		audio_is_playing = 0;
		audio_playing_status = 0;
		detailwin_is_open_closed = 1;
		auto_play_finished = 0;
		detail_just_closed = 0;
		get_detail_fail_retry_times_limit = 4;
		GUITEXT_SetRect(MMI_DSL_POETRY_APPRE_TEXT_ID, &appre_openwindow_rect);
		GUITEXT_SetFont(MMI_DSL_POETRY_APPRE_TEXT_ID, &text_font,&text_color);
		GUITEXT_IsDisplayPrg(FALSE, MMI_DSL_POETRY_APPRE_TEXT_ID);
		GUITEXT_SetHandleTpMsg(FALSE, MMI_DSL_POETRY_APPRE_TEXT_ID);
		GUITEXT_SetClipboardEnabled(MMI_DSL_POETRY_APPRE_TEXT_ID,FALSE);
		GUITEXT_IsSlide(MMI_DSL_POETRY_APPRE_TEXT_ID,FALSE);
		detail_content_line_num = 0;
		parse_detail_success_now_can_click_audio = 0;
		abs_http_running_need_close_when_close_win = 0;
		mp3_download_http_running_need_close_when_close_win = 0;
		LayoutItem(MMI_DSL_POETRY_BUTTON_DETAIL_REFRESH, 72, 168, 271, 300);
		InitButton(MMI_DSL_POETRY_BUTTON_DETAIL_REFRESH);
		GUIBUTTON_SetTextId(MMI_DSL_POETRY_BUTTON_DETAIL_REFRESH, READBOY_TXT_POETRY_REFRESH);
		GUIBUTTON_SetCallBackFunc(MMI_DSL_POETRY_BUTTON_DETAIL_REFRESH,poetry_detailRefresh);
		local_file_load_failed = 0;

		Read_detail_data_from_tf_and_parse();

		if(main_option == 1){//todo 判断收藏状态这里的代码要更新！！！！
			if(temp_favorite_list->favorite[grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id]==0){
				detail_current_poetry_favorite_status = 0;
				detail_current_poetry_favorite_status_has_changed = 0;
			}else if(temp_favorite_list->favorite[grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id]==1){
				detail_current_poetry_favorite_status = 1;
				detail_current_poetry_favorite_status_has_changed = 1;
			}
		}else if (main_option == 2){
			detail_current_poetry_favorite_status = 1; //从收藏夹里打开的肯定是已收藏的
			detail_current_poetry_favorite_status_has_changed = 1;
		}
		if(detail_get_status == 0){
			MMI_STRING_T text_string = {0};
			GUI_RECT_T win_rect = {0, 30, 239, 359};
			GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN;
			GUISTR_STYLE_T text_style = {0};
			text_style.align = ALIGN_HVMIDDLE;
			text_style.font = SONG_FONT_16;
			text_style.font_color = MMI_BLACK_COLOR;
			MMIRES_GetText(READBOY_TXT_POETRY_LOADING, win_id, &text_string);
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
	}
		break;
	case MSG_FULL_PAINT:
	{
		GUI_FONT_T text_font = SONG_FONT_20;
		GUI_COLOR_T text_color = MMI_BLACK_COLOR;
		GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_SINGLE_LINE | GUISTR_STATE_ELLIPSIS_EX;
		GUISTR_STYLE_T text_style = {0};
		MMI_STRING_T text_string = {0};
		uint8 num = 0;
		char num_buf[10] = {0};
		wchar num_str[10] = {0};
		uint16 i;

		GUI_FillRect(&lcd_dev_info, poetry_win_rect, MMI_WHITE_COLOR);
		GUIBUTTON_SetVisible(MMI_DSL_POETRY_BUTTON_DETAIL_REFRESH,FALSE,FALSE);

		text_style.align = ALIGN_LVMIDDLE;
		text_style.font = SONG_FONT_20;
		text_style.font_color = MMI_BLACK_COLOR;
		MMIRES_GetText(READBOY_TXT_POETRY, win_id, &text_string);//"古诗词"
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
		text_style.font = SONG_FONT_16;
		GUITEXT_SetRect(MMI_DSL_POETRY_APPRE_TEXT_ID, &appre_openwindow_rect);
		GUITEXT_SetFont(MMI_DSL_POETRY_APPRE_TEXT_ID, &text_font, &text_color);
		if(detail_get_status == 1){//成功从服务器拿到数据
			if(detail_win_current_option == 1){//原文 
			    text_style.font = SONG_FONT_20;
			    GUISTR_DrawTextToLCDInRect(
        				(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        				&poetry_content_title_rect,
        				&poetry_content_title_rect,
        				&text_title,
        				&text_style,
        				text_state,
        				GUISTR_TEXT_DIR_AUTO
				);
				GUITEXT_SetAlign(MMI_DSL_POETRY_APPRE_TEXT_ID, ALIGN_HMIDDLE);
				GUITEXT_SetRect(MMI_DSL_POETRY_APPRE_TEXT_ID, &poetry_content_rect);
				GUITEXT_SetString(MMI_DSL_POETRY_APPRE_TEXT_ID, text_origin.wstr_ptr,text_origin.wstr_len, FALSE);                    
			}else if(detail_win_current_option == 2){//注释
				GUITEXT_SetAlign(MMI_DSL_POETRY_APPRE_TEXT_ID, ALIGN_HVMIDDLE);
				GUITEXT_SetRect(MMI_DSL_POETRY_APPRE_TEXT_ID, &poetry_appre_rect);
				GUITEXT_SetString(MMI_DSL_POETRY_APPRE_TEXT_ID, text_note.wstr_ptr,text_note.wstr_len, FALSE);
			}else if (detail_win_current_option == 3 ){//翻译
				GUITEXT_SetAlign(MMI_DSL_POETRY_APPRE_TEXT_ID, ALIGN_HVMIDDLE);
				GUITEXT_SetRect(MMI_DSL_POETRY_APPRE_TEXT_ID, &poetry_trans_rect);
				GUITEXT_SetString(MMI_DSL_POETRY_APPRE_TEXT_ID, text_trans.wstr_ptr,text_trans.wstr_len, FALSE);
			}else if(detail_win_current_option == 4){//赏析
				GUITEXT_SetAlign(MMI_DSL_POETRY_APPRE_TEXT_ID, ALIGN_HVMIDDLE);
				GUITEXT_SetRect(MMI_DSL_POETRY_APPRE_TEXT_ID, &poetry_appre_rect);
				GUITEXT_SetString(MMI_DSL_POETRY_APPRE_TEXT_ID, text_appr.wstr_ptr,text_appr.wstr_len, FALSE);
			}
		}else if(detail_get_status == 0){//加载中
			MMIRES_GetText(READBOY_TXT_POETRY_LOADING, win_id, &text_string);
			GUISTR_DrawTextToLCDInRect(
					&lcd_dev_info,
					&poetry_win_rect,
					&poetry_win_rect,
					&text_string,
					&text_style,
					text_state,
					GUISTR_TEXT_DIR_AUTO
					);
		}else if(detail_get_status == 2){//加载失败
			MMIRES_GetText(READBOY_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
			GUISTR_DrawTextToLCDInRect(
				(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
				&poetry_win_rect,
				&poetry_win_rect,
				&text_string,
				&text_style,
				text_state,
				GUISTR_TEXT_DIR_AUTO
			);
		}else if(detail_get_status == 3){//加载失败，且自动重试也失败，需要手动刷新
			GUIBUTTON_SetVisible(MMI_DSL_POETRY_BUTTON_DETAIL_REFRESH,TRUE,TRUE);
			MMIRES_GetText(READBOY_TXT_POETRY_LOAD_FAILED, win_id, &text_string);
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

		if(detail_win_current_option == 1){//只有原文有音频
			GUIRES_DisplayImg(PNULL, &poetry_play_rect, PNULL, win_id, IMG_DIS_AUTO_PLAY, &lcd_dev_info);
		}
		if(detail_current_poetry_favorite_status){
			GUIRES_DisplayImg(PNULL, &poetry_favorite_rect, PNULL, win_id, IMG_DSL_FAVORITE, &lcd_dev_info);//todo 换favorite图标
		}else{
			GUIRES_DisplayImg(PNULL, &poetry_favorite_rect, PNULL, win_id, IMG_DSL_NOTFAVORITE, &lcd_dev_info);//todo 换not favorite图标
		}
		PoetryDetail_DrawOption(win_id, detail_win_current_option);
	}
	break;

	case MSG_TP_PRESS_UP:
	{
		GUI_POINT_T point = {0};
		int16 tp_offset_x =  0;
		int16 tp_offset_y = 0;
		point.x = MMK_GET_TP_X(param);
		point.y = MMK_GET_TP_Y(param);
		tp_offset_x = point.x - main_tp_down_x;
		tp_offset_y = point.y - main_tp_down_y;
		if(ABS(tp_offset_y) <= ABS(tp_offset_x))
		{
			if(tp_offset_x > 40)
			{
				Detail_LoadPrePage();
				break;
			}
			else if(tp_offset_x < -40)
			{
				Detail_LoadNextPage();
				break;
			}
		}else {
			if(tp_offset_y > 40)
			{
				MMK_PostMsg(MMI_DSL_POETRY_APPRE_TEXT_ID, MSG_KEYREPEAT_UP, PNULL, 0);
			}
			else if(tp_offset_y < -40)
			{
				MMK_PostMsg(MMI_DSL_POETRY_APPRE_TEXT_ID, MSG_KEYREPEAT_DOWN, PNULL, 0);
			}
		}

		if(ABS(tp_offset_x) <20 && ABS(tp_offset_y) < 20)
		{
			PoetryDetail_HanldeTpUp(win_id, point);
		}
	}
	break;
	case MSG_TP_PRESS_DOWN:
	{
		main_tp_down_x = MMK_GET_TP_X(param);
		main_tp_down_y = MMK_GET_TP_Y(param);
	}
	break;
	case MSG_KEYUP_OK:
		break;
	case MSG_CLOSE_WINDOW:
			temp_favorite_list->favorite[grade_all_list.all_grade[grade_display_status-1].grade_info[(((currentpage-1)*4)+detail_selected_index-1)].id]=detail_current_poetry_favorite_status;
			detail_just_closed = 1;
			detail_get_status = 0;//好像有点问题，试试开关窗口时都给它赋值
			//下面两个关http的可能有问题，再加点精确判断条件，确定有正在进行中的http才关
			if (0 != poetry_player_handle)
			{
				MMISRVAUD_Stop(poetry_player_handle);
				MMISRVMGR_Free(poetry_player_handle);
				poetry_player_handle = PNULL;
			}
			detailwin_is_open_closed = 0;
			if(wstr_title != PNULL){
				SCI_FREE(wstr_title);
				wstr_title = PNULL;
			}
			if(wstr_origin != PNULL){
				SCI_FREE(wstr_origin);
				wstr_origin = PNULL;
			}
			if(wstr_note != PNULL){
				SCI_FREE(wstr_note);
				wstr_note = PNULL;
			}
			if(wstr_trans != PNULL){
				SCI_FREE(wstr_trans);
				wstr_trans = PNULL;
			}
			if(wstr_appr != PNULL){
				SCI_FREE(wstr_appr);
				wstr_appr = PNULL;
			}
			MMIZDT_HTTP_Close();
		break;
	case MSG_KEYUP_CANCEL:
		{
			if(!GetCanntClick(0)){
				detail_just_closed = 1;
				MMK_CloseWin(win_id);
			}  
		}
		break;
	default:
		break;
	}
	return recode;
}

WINDOW_TABLE(MMI_POETRY_WIN_TAB) = 
{
	WIN_ID(POETRY_MAIN_WIN_ID),
	WIN_FUNC((uint32)HandlePoetryWinMsg),
	CREATE_BUTTON_CTRL(PNULL, MMI_DSL_POETRY_BUTTON_REFRESH),
	WIN_HIDE_STATUS,
	END_WIN
};

WINDOW_TABLE(POETRY_DETAIL_WIN_TAB) = {
    WIN_ID(POETRY_DETAIL_WIN_ID),
    WIN_FUNC((uint32)HandlePoetryDetailWinMsg),
	CREATE_BUTTON_CTRL(PNULL, MMI_DSL_POETRY_BUTTON_DETAIL_REFRESH),
	CREATE_TEXT_CTRL(MMI_DSL_POETRY_APPRE_TEXT_ID),
	WIN_HIDE_STATUS,
    END_WIN
};



PUBLIC void MMIREADBOY_CreatePoetryWin(void)
{
	MMI_WIN_ID_T win_id = POETRY_MAIN_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}
	if(GetCanntClick(1))
	{
	SCI_TRACE_LOW("%s can not click", __FUNCTION__);
	return;
	}
	win_handle = MMK_CreateWin((uint32*)MMI_POETRY_WIN_TAB, PNULL);
	MMK_SetWinRect(win_handle, &rect);
}

LOCAL void MMI_CreatePoetryDetailWin(void) {
	MMI_WIN_ID_T win_id = POETRY_DETAIL_WIN_ID;
	MMI_HANDLE_T win_handle = 0;
	GUI_RECT_T rect = {0, 30, 239, 359};

	if (MMK_IsOpenWin(win_id))
	{
		MMK_CloseWin(win_id);
	}
	if(GetCanntClick(1))
	{
	SCI_TRACE_LOW("%s can not click", __FUNCTION__);
	return;
	}
    win_handle = MMK_CreateWin((uint32 *)POETRY_DETAIL_WIN_TAB, PNULL);
	MMK_SetWinRect(win_handle, &rect);
}

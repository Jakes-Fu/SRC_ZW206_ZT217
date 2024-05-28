/*****************************************************************************
** File Name:      zmt_gpt_zuowen.c                                           *
** Author:           fys                                                        *
** Date:           2024/05/16                                                 *
** Copyright:       *
** Description:                       *
******************************************************************************/
#include "std_header.h"
#include <stdlib.h>
#include "cjson.h"
#include "dal_time.h"
#include "zmt_gpt.h"
#include "zmt_gpt_id.h"
#include "zmt_gpt_text.h"
#include "zmt_gpt_image.h"
#include "ctrlanim_export.h"
#include "../../../mmi_ctrl/source/anim/h/ctrlanim.h"
#include "gps_drv.h"
#include "gps_interface.h"
#include "guibutton.h"
#include "guifont.h"
#include "guilcd.h"
#include "guistring.h"
#include "guitext.h"
#include "guires.h"
#include "mmi_textfun.h"
#include "mmiacc_text.h"
#include "mmicc_export.h"
#include "mmidisplay_data.h"
#include "mmipub.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmisrvrecord_export.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "dsl_main_file.h"
#endif
#include "graphics_draw.h"
#include "img_dec_interface.h"

extern GUI_RECT_T zmt_gpt_win_rect;//窗口
extern GUI_RECT_T zmt_gpt_title_rect;//顶部
extern GUI_RECT_T zmt_gpt_list_rect;
extern GUI_RECT_T zmt_gpt_record_rect;
extern GUI_RECT_T zmt_gpt_record_left_rect;
extern GUI_RECT_T zmt_gpt_record_center_rect;
extern GUI_RECT_T zmt_gpt_record_right_rect;

LOCAL uint8 zmt_gpt_zuowen_status = 0;
LOCAL uint8 gpt_zuowen_talk_size = 0;
LOCAL gpt_talk_info_t * gpt_zuowen_talk_info[GPT_ZUOWEN_TALK_MAX_SIZE];
LOCAL ZMT_GPT_RECORD_TYPE_E gpt_zuowen_record_type = GPT_RECORD_TYPE_NONE;
LOCAL GUI_LCD_DEV_INFO gpt_zuowen_record_layer = {0};
LOCAL uint8 gpt_zuowen_record_timer_id = 0;
LOCAL uint32 gpt_zuowen_record_times = 0;
LOCAL uint8 gpt_zuowen_record_identify_timer_id = 0;
LOCAL uint8 * gpt_zuowen_record_text = NULL;
LOCAL RECORD_SRV_HANDLE gpt_zuowen_record_handle = PNULL;
LOCAL BOOLEAN gpt_zuowen_load_text = FALSE;

LOCAL void  ZmtGptZuoWen_DispalyRecord(MMI_WIN_ID_T win_id, int record_type);
LOCAL void ZmtGptZuoWen_ShowFormList(MMI_WIN_ID_T win_id);

LOCAL void ZmtGptZuoWen_ReleaseTalkInfo(void)
{
    uint8 i = 0;
    for(i = 0;i < GPT_KOUYU_TALK_MAX_SIZE && i < gpt_zuowen_talk_size;i++)
    {
        if(gpt_zuowen_talk_info[i] != NULL){
            if(gpt_zuowen_talk_info[i]->str != NULL){
                SCI_FREE(gpt_zuowen_talk_info[i]->str);
                gpt_zuowen_talk_info[i]->str = NULL;
            }
            SCI_FREE(gpt_zuowen_talk_info[i]);
            gpt_zuowen_talk_info[i] = NULL;
        }
    }
}

LOCAL void ZmtGptZuoWen_DeleteFrontTwoMsg(void)
{
    uint8 index = 0;
    uint8 i = 0;
    uint16 size = 0;
    gpt_talk_info_t * talk_info[GPT_ZUOWEN_TALK_MAX_SIZE];

    for(i = 0;i < GPT_ZUOWEN_TALK_MAX_SIZE;i++)
    {
        if(i != 1 && i != 2){
            if(gpt_zuowen_talk_info[i] != NULL){
                talk_info[index] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
                memset(talk_info[index], 0, sizeof(gpt_talk_info_t));
                talk_info[index]->is_user = gpt_zuowen_talk_info[i]->is_user;
                size = strlen(gpt_zuowen_talk_info[i]->str);
                talk_info[index]->str = SCI_ALLOC_APPZ(size + 1);
                memset(talk_info[index]->str, 0, size + 1);
                SCI_MEMCPY(talk_info[index]->str, gpt_zuowen_talk_info[i]->str, size);
                index++;
            }
        }
    }
    SCI_TRACE_LOW("%s: index = %d", __FUNCTION__, index);
    
    ZmtGptZuoWen_ReleaseTalkInfo();
    for(i = 0;i < GPT_ZUOWEN_TALK_MAX_SIZE - 2;i++)
    {
        if(talk_info[i] != NULL){
            gpt_zuowen_talk_info[i] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
            memset(gpt_zuowen_talk_info[i], 0, sizeof(gpt_talk_info_t));
            gpt_zuowen_talk_info[i]->is_user = talk_info[i]->is_user;
            size = strlen(talk_info[i]->str);
            gpt_zuowen_talk_info[i]->str = SCI_ALLOC_APPZ(size + 1);
            memset(gpt_zuowen_talk_info[i]->str, 0, size + 1);
            SCI_MEMCPY(gpt_zuowen_talk_info[i]->str, talk_info[i]->str, size);
        }
    }
	
    for(i = 0;i < GPT_ZUOWEN_TALK_MAX_SIZE && i < index;i++)
    {
        if(talk_info[i] != NULL){
            if(talk_info[i]->str != NULL){
                SCI_FREE(talk_info[i]->str);
                talk_info[i]->str = NULL;
            }
            SCI_FREE(talk_info[i]);
            talk_info[i] = NULL;
        }
    }
}

PUBLIC void ZmtGptZuoWen_RecAiTextResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        cJSON * root = cJSON_Parse(pRcv);
        cJSON * result = cJSON_GetObjectItem(root, "result");
        if(result != NULL && result->type != cJSON_NULL && strlen(result->valuestring) > 0)
        {
            SCI_TRACE_LOW("%s: Size(result) = %d", __FUNCTION__, cJSON_GetArraySize(result));
            for(i = 0;i < cJSON_GetArraySize(result) && i < 1;i++){
                cJSON * item = cJSON_GetArrayItem(result, 0);
                //SCI_TRACE_LOW("%s: item_string = %s", __FUNCTION__, item->valuestring);
                SCI_TRACE_LOW("%s: item_len = %d", __FUNCTION__, strlen(item->valuestring));
                if(item != NULL && item->type != cJSON_NULL && strlen(item->valuestring) > 0)
                {
                    if(gpt_zuowen_record_text){
                        SCI_FREE(gpt_zuowen_record_text);
                        gpt_zuowen_record_text = NULL;
                    }
                    gpt_zuowen_record_text = SCI_ALLOC_APPZ(strlen(item->valuestring)+1);
                    memset(gpt_zuowen_record_text, 0, strlen(item->valuestring)+1);
                    strcpy(gpt_zuowen_record_text, item->valuestring);
                    gpt_zuowen_record_type = GPT_RECORD_TYPE_SUCCESS;
                }
                else
                {
                    SCI_TRACE_LOW("%s: item is NULL!!", __FUNCTION__);
                    if(gpt_zuowen_record_text){
                        SCI_FREE(gpt_zuowen_record_text);
                        gpt_zuowen_record_text = NULL;
                    }
                    gpt_zuowen_record_type = GPT_RECORD_TYPE_FAIL;
                }
            }       
        }
        else
        {
            SCI_TRACE_LOW("%s: result is empty!!", __FUNCTION__);
            if(gpt_zuowen_record_text){
                SCI_FREE(gpt_zuowen_record_text);
                gpt_zuowen_record_text = NULL;
            }
            gpt_zuowen_record_type = GPT_RECORD_TYPE_FAIL;
        }
    }
    else
    {
        SCI_TRACE_LOW("%s: request error!!", __FUNCTION__);
        if(gpt_zuowen_record_text){
            SCI_FREE(gpt_zuowen_record_text);
            gpt_zuowen_record_text = NULL;
        }
        gpt_zuowen_record_type = GPT_RECORD_TYPE_ERROR;
    }
    if(MMK_IsFocusWin(ZMT_GPT_ZUOWEN_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_ZUOWEN_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

PUBLIC void ZmtGptZuoWen_RecvSelfResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
#if ZMT_GPT_USE_FOR_TEST != 0
    uint8 * buf = NULL;
    uint32 len = 0;
    {
        if(dsl_file_exist(ZMT_GPT_SELF_ZUOWEN_TALK_PATH)){
            buf = dsl_file_data_read(ZMT_GPT_SELF_ZUOWEN_TALK_PATH, &len);
            is_ok = TRUE;
        }
    }
    pRcv = buf;
    Rcv_len = len;
#endif
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    SCI_TRACE_LOW("%s: pRcv = %s", __FUNCTION__, pRcv);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        cJSON * root = cJSON_Parse(pRcv);
        cJSON * data = cJSON_GetObjectItem(root, "data");
        cJSON * code = cJSON_GetObjectItem(root, "code");
        SCI_TRACE_LOW("%s: code = %d", __FUNCTION__, code->valueint);
        if(data != NULL && data->type != cJSON_NULL && strlen(data->valuestring) > 0)
        {
            SCI_TRACE_LOW("%s: gpt_zuowen_talk_size = %d", __FUNCTION__, gpt_zuowen_talk_size);
            if(gpt_zuowen_talk_info[gpt_zuowen_talk_size] == NULL){
                gpt_zuowen_talk_info[gpt_zuowen_talk_size] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
            }
            memset(gpt_zuowen_talk_info[gpt_zuowen_talk_size], 0, sizeof(gpt_talk_info_t));
            gpt_zuowen_talk_info[gpt_zuowen_talk_size]->is_user = FALSE;

            gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str = SCI_ALLOC_APPZ(strlen(data->valuestring)+1);
            memset(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, 0, strlen(data->valuestring)+1);
            strcpy(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, data->valuestring);

            gpt_zuowen_talk_size++;
            gpt_zuowen_talk_info[gpt_zuowen_talk_size] = NULL;

            zmt_gpt_zuowen_status = 3;
            gpt_zuowen_record_type = GPT_RECORD_TYPE_NONE;
        }
        else
        {
            gpt_zuowen_record_type = GPT_RECORD_TYPE_FAIL;
        }
    }
    else
    {
        gpt_zuowen_record_type = GPT_RECORD_TYPE_ERROR;
    }
    SCI_TRACE_LOW("%s: gpt_zuowen_record_type = %d", __FUNCTION__, gpt_zuowen_record_type);    
    gpt_zuowen_load_text = FALSE;
    if(MMK_IsFocusWin(ZMT_GPT_ZUOWEN_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_ZUOWEN_WIN_ID, MSG_FULL_PAINT, PNULL);
        ZmtGptZuoWen_ShowFormList(ZMT_GPT_ZUOWEN_WIN_ID);
    }
#if ZMT_GPT_USE_FOR_TEST != 0
    if(pRcv != NULL){
        SCI_FREE(pRcv);
    }
#endif
}

PUBLIC void ZmtGptZuoWen_RecvResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
#if ZMT_GPT_USE_FOR_TEST != 0
    uint8 * buf = NULL;
    uint32 len = 0;
    {
        if(dsl_file_exist(ZMT_GPT_ZUOWEN_TALK_PATH)){
            buf = dsl_file_data_read(ZMT_GPT_ZUOWEN_TALK_PATH, &len);
            is_ok = TRUE;
        }
    }
    pRcv = buf;
    Rcv_len = len;
#endif
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        cJSON * root = cJSON_Parse(pRcv);
        cJSON * data = cJSON_GetObjectItem(root, "data");
        if(data != NULL && data->type != cJSON_NULL)
        {
            cJSON * choices = cJSON_GetObjectItem(data, "choices");
            cJSON * item = cJSON_GetArrayItem(choices, 0);
            cJSON * message = cJSON_GetObjectItem(item, "message");
            cJSON * content = cJSON_GetObjectItem(message, "content");
            if(content != NULL && content->type != cJSON_NULL)
            {
                SCI_TRACE_LOW("%s: gpt_zuowen_talk_size = %d", __FUNCTION__, gpt_zuowen_talk_size);
                if(gpt_zuowen_talk_info[gpt_zuowen_talk_size] == NULL){
                    gpt_zuowen_talk_info[gpt_zuowen_talk_size] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
                }
                memset(gpt_zuowen_talk_info[gpt_zuowen_talk_size], 0, sizeof(gpt_talk_info_t));
                gpt_zuowen_talk_info[gpt_zuowen_talk_size]->is_user = FALSE;
				
                gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str = SCI_ALLOC_APPZ(strlen(content->valuestring)+1);
                memset(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, 0, strlen(content->valuestring)+1);
                strcpy(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, content->valuestring);

                gpt_zuowen_talk_size++;
                gpt_zuowen_talk_info[gpt_zuowen_talk_size] = NULL;

                zmt_gpt_zuowen_status = 3;
            }
        }
        else
        {
            zmt_gpt_zuowen_status = 1;
        }
    }
    else
    {
        zmt_gpt_zuowen_status = 2;
    }
    gpt_zuowen_load_text = FALSE;
    gpt_zuowen_record_type = GPT_RECORD_TYPE_NONE;
    if(MMK_IsFocusWin(ZMT_GPT_ZUOWEN_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_ZUOWEN_WIN_ID, MSG_FULL_PAINT, PNULL);
        ZmtGptZuoWen_ShowFormList(ZMT_GPT_ZUOWEN_WIN_ID);
    }
#if ZMT_GPT_USE_FOR_TEST != 0
    if(pRcv != NULL){
        SCI_FREE(pRcv);
    }
#endif
}

LOCAL void ZmtGptZuoWen_NotifyRecordCallback(RECORD_SRV_HANDLE record_srv_handle, 
    MMIRECORD_SRV_SUPPORT_TYPE_E support_type, 
    MMIRECORD_SRV_RESULT_E result
    )
{
    SCI_TRACE_LOW("%s: support_type = %d", __FUNCTION__, support_type);
    if (record_srv_handle == gpt_zuowen_record_handle &&
        MMIRECORD_SRV_SUPPORT_RECORD == support_type)
    {
        SCI_TRACE_LOW("%s: result = %d", __FUNCTION__, result);
    }
}

LOCAL void ZmtGptZuoWen_StopRecord(MMI_WIN_ID_T win_id, BOOLEAN is_send)
{
    if (PNULL != gpt_zuowen_record_handle)
    {
        MMIRECORDSRV_StopRecord(gpt_zuowen_record_handle);
        MMIRECORDSRV_FreeRecordHandle(gpt_zuowen_record_handle);
        gpt_zuowen_record_handle = 0;
    }
    if(gpt_zuowen_record_timer_id != 0)
    {
        MMK_StopTimer(gpt_zuowen_record_timer_id);
        gpt_zuowen_record_timer_id = 0;
    }
    gpt_zuowen_record_times = 0;
    
    if(is_send){
        uint8 * data_buf = NULL;
        uint32 data_size = 0;
        uint32 size;
        data_buf = dsl_file_data_read(ZMT_GPT_RECORD_FILE_C, &data_size);
        SCI_TRACE_LOW("%s: data_size = %d", __FUNCTION__, data_size);
        if(data_buf != NULL && data_size > 2){
            ZmtGpt_SendRecord(1537, data_buf, data_size);
        }
        dsl_file_delete(ZMT_GPT_RECORD_FILE_C);
    #if ZMT_GPT_USE_FOR_TEST != 0
        gpt_zuowen_record_type = GPT_RECORD_TYPE_SUCCESS;
    #else
        gpt_zuowen_record_type = GPT_RECORD_TYPE_VOICE_LOADING;
    #endif
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    }else{
        dsl_file_delete(ZMT_GPT_RECORD_FILE_C);
        gpt_zuowen_record_type = GPT_RECORD_TYPE_NONE; 
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);       
    }
}

LOCAL void ZmtGptZuoWen_StartRecord(MMI_WIN_ID_T win_id)
{
    MMI_STRING_T  call_name_str = {0};
    MMIRECORD_SRV_RECORD_PARAM_T record_param = {0};
    MMIFILE_DEVICE_E record_dev = MMIAPIRECORD_GetRecordStorage();
    MMIRECORD_SRV_RESULT_E  record_result = MMIRECORD_SRV_RESULT_SUCCESS;

    if(gpt_zuowen_load_text){
        SCI_TRACE_LOW("%s: load text xxing", __FUNCTION__);
        return;
    }

    if (PNULL != gpt_zuowen_record_handle)
    {
        MMIRECORDSRV_StopRecord(gpt_zuowen_record_handle);
        MMIRECORDSRV_FreeRecordHandle(gpt_zuowen_record_handle);
        gpt_zuowen_record_handle = 0;
    }

    gpt_zuowen_record_handle = MMIRECORDSRV_RequestRecordHandle(ZmtGptZuoWen_NotifyRecordCallback);
    if (PNULL == gpt_zuowen_record_handle)
    {
        ZmtGptZuoWen_StopRecord(win_id, FALSE);
        return;
    }

    if(gpt_zuowen_record_timer_id != 0){
        return;
    }
    gpt_zuowen_record_timer_id = MMK_CreateWinTimer(win_id, 200, TRUE);
    MMK_StartWinTimer(win_id, gpt_zuowen_record_timer_id, 200, TRUE);
    gpt_zuowen_record_times = 0;
    gpt_zuowen_record_type = GPT_RECORD_TYPE_RECORDING;
    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
	
    call_name_str.wstr_ptr = ZMT_GPT_RECORD_FILE_L;
    call_name_str.wstr_len = MMIAPICOM_Wstrlen(ZMT_GPT_RECORD_FILE_L);

    record_param.fmt = MMISRVAUD_RECORD_FMT_AMR;
    record_param.prefix_ptr = PNULL;
    record_param.record_dev_ptr = &record_dev;
    record_param.record_file_id_ptr = PNULL;
    record_param.record_name_ptr = &call_name_str;
    record_param.source = MMISRVAUD_RECORD_SOURCE_NORMAL;
    record_param.record_param_ptr = PNULL;
    record_param.volume = MMIAPISET_GetMultimVolume();
    record_param.is_create_file_name = TRUE;

    record_result= MMIRECORDSRV_StartRecordFullFile(gpt_zuowen_record_handle, record_param);
}

LOCAL void ZmtGptZuoWen_LeftIndentifyClick(MMI_WIN_ID_T win_id)
{
    if(gpt_zuowen_record_text != NULL){
        SCI_FREE(gpt_zuowen_record_text);
        gpt_zuowen_record_text = NULL;
    }
    gpt_zuowen_record_type = GPT_RECORD_TYPE_NONE;
    gpt_zuowen_load_text = FALSE;
    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
}

LOCAL void ZmtGptZuoWen_RecordIndentifyTimerCallback(uint8 timer_id, uint32 param)
{
    if(timer_id == gpt_zuowen_record_identify_timer_id)
    {
        MMK_StopTimer(gpt_zuowen_record_identify_timer_id);
        gpt_zuowen_record_identify_timer_id = 0;
        gpt_zuowen_record_type = GPT_RECORD_TYPE_NONE;
        if(MMK_IsFocusWin(ZMT_GPT_ZUOWEN_WIN_ID)){
            ZmtGptZuoWen_DispalyRecord(ZMT_GPT_ZUOWEN_WIN_ID, gpt_zuowen_record_type);
        }
    }
}

LOCAL void ZmtGptZuoWen_RightIndentifyClick(MMI_WIN_ID_T win_id)
{
    if(gpt_zuowen_record_text == NULL || strlen(gpt_zuowen_record_text) < 2)
    {
        SCI_TRACE_LOW("%s:  gpt_zuowen_record_text empty!!", __FUNCTION__);
        return;
    }
    if(gpt_zuowen_talk_info[gpt_zuowen_talk_size] == NULL){
        gpt_zuowen_talk_info[gpt_zuowen_talk_size] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
    }
    gpt_zuowen_talk_info[gpt_zuowen_talk_size]->is_user = TRUE;
    gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str = SCI_ALLOC_APPZ(strlen(gpt_zuowen_record_text)+1);
    memset(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, 0, strlen(gpt_zuowen_record_text)+1);
    SCI_MEMCPY(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, gpt_zuowen_record_text, strlen(gpt_zuowen_record_text));
    gpt_zuowen_talk_size++;
    gpt_zuowen_record_type = GPT_RECORD_TYPE_TEXT_LOADING;
    if(gpt_zuowen_talk_size >= GPT_ZUOWEN_TALK_MAX_SIZE){
        ZmtGptZuoWen_DeleteFrontTwoMsg();
        gpt_zuowen_talk_size -= 2;
        //SCI_TRACE_LOW("%s: 01gpt_zuowen_talk_size = %d", __FUNCTION__, gpt_zuowen_talk_size);
    }
    gpt_zuowen_load_text = TRUE;
 #if ZMT_GPT_USE_SELF_API != 0
    if(gpt_zuowen_talk_size <= 2){
        ZmtGpt_SendSelfTxt(1, gpt_zuowen_record_text, TRUE);
    }else{
        ZmtGpt_SendSelfTxt(1, gpt_zuowen_record_text, FALSE);
    }
#else
    ZmtGpt_SendTxt(88, gpt_zuowen_record_text, NULL, NULL, 1);
#endif
    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    if(gpt_zuowen_record_text){
        SCI_FREE(gpt_zuowen_record_text);
        gpt_zuowen_record_text = NULL;
    }
}

LOCAL void ZmtGptZuoWen_DisplayList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    uint8 num = 0;
    GUI_RECT_T list_rect = {0};
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_string = {0};
    wchar text_str[2048] = {0};
    uint8 line_num = 0;

    list_rect = zmt_gpt_list_rect;
    list_rect.bottom = MMI_MAINSCREEN_HEIGHT - ZMT_GPT_LINE_HIGHT;
    list_init.both_rect.v_rect = list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, gpt_zuowen_talk_size+1, FALSE);

    SCI_TRACE_LOW("%s: gpt_kouyu_talk_size = %d", __FUNCTION__, gpt_zuowen_talk_size);
    for(i = 0;i < gpt_zuowen_talk_size; i++)
    {
        memset(text_str, 0, 2048);

        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT;
        if(i == 0)
        {
            GUI_GBToWstr(text_str, gpt_zuowen_talk_info[i]->str, strlen(gpt_zuowen_talk_info[i]->str));
        }
        else
        {
        #ifndef WIN32
            GUI_UTF8ToWstr(text_str, 2048, gpt_zuowen_talk_info[i]->str, strlen(gpt_zuowen_talk_info[i]->str));
        #else
            GUI_GBToWstr(text_str, gpt_zuowen_talk_info[i]->str, strlen(gpt_zuowen_talk_info[i]->str));
        #endif
        }
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        line_num = GUI_CalculateStringLinesByPixelNum(160,text_string.wstr_ptr,text_string.wstr_len,SONG_FONT_16,0,TRUE);
        SCI_TRACE_LOW("%s: i = %d, line_num = %d", __FUNCTION__, i, line_num);
        if(!gpt_zuowen_talk_info[i]->is_user)
        {
            switch(line_num)
            {
                case 1:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_TEXT_MS;
                    break;
                case 2:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_2LINE_TEXT_MS;
                    break;
                case 3:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_3LINE_TEXT_MS;
                    break;
                case 4:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_4LINE_TEXT_MS;
                    break;
                case 5:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_5LINE_TEXT_MS;
                    break;
                case 6:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_6LINE_TEXT_MS;
                    break;
                case 7:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_7LINE_TEXT_MS;
                    break;
                default:
                    item_t.item_style = GUIITEM_STYLE_GPT_AI_7LINE_TEXT_MS;
                    break;
            }           
            item_data.item_content[0].item_data.text_buffer = text_string;
            item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;

            item_data.item_content[1].item_data.image_id = PNULL;
            item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;
        }
        else
        {
            switch(line_num)
            {
                case 1:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_TEXT_MS;
                    break;
                case 2:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_2LINE_TEXT_MS;
                    break;
                case 3:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_3LINE_TEXT_MS;
                    break;
                case 4:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_4LINE_TEXT_MS;
                    break;
                case 5:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_5LINE_TEXT_MS;
                    break;
                case 6:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_6LINE_TEXT_MS;
                    break;
                case 7:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_7LINE_TEXT_MS;
                    break;
                default:
                    item_t.item_style = GUIITEM_STYLE_GPT_MINE_7LINE_TEXT_MS;
                    break;
            } 
            item_data.item_content[0].item_data.image_id = PNULL;
            item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
            
            item_data.item_content[1].item_data.text_buffer = text_string;
            item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        }
        //SCI_TRACE_LOW("%s: [%d]->str = %s", __FUNCTION__, i, gpt_zuowen_talk_info[i]->str);
        //不画分割线
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
        //不画高亮条
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);
        
        GUILIST_SetBgColor(ctrl_id,MMI_BLACK_COLOR);
        GUILIST_SetTextFont(ctrl_id, SONG_FONT_16, MMI_WHITE_COLOR);

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    GUILIST_SetCurItemIndex(ctrl_id, gpt_zuowen_talk_size - 1);
    SCI_TRACE_LOW("%s: gpt_zuowen_load_text = %d", __FUNCTION__, gpt_zuowen_load_text);
    if(gpt_zuowen_load_text)
    {
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT;
        item_t.item_style = GUIITEM_STYLE_GPT_AI_TEXT_MS;

        item_data.item_content[0].item_data.text_id= ZMT_CHAT_GPT_LOADING;
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_ID;

        item_data.item_content[1].item_data.image_id = PNULL;
        item_data.item_content[1].item_data_type = GUIITEM_DATA_IMAGE_ID;

         //不画分割线
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
        //不画高亮条
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);
        
        GUILIST_SetBgColor(ctrl_id,MMI_BLACK_COLOR);
        GUILIST_SetTextFont(ctrl_id, SONG_FONT_16, MMI_WHITE_COLOR);

        GUILIST_AppendItem(ctrl_id, &item_t);
        GUILIST_SetCurItemIndex(ctrl_id, gpt_zuowen_talk_size);
    }
}

LOCAL void ZmtGptZuoWen_ShowFormList(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    MMI_CTRL_ID_T form_ctrl_id = ZMT_GPT_FORM_CTRL_ID;
    MMI_CTRL_ID_T anim_ctrl_id = ZMT_GPT_FORM_ANIM_CTRL_ID;
    MMI_CTRL_ID_T text_ctrl_id = 0;
    MMI_HANDLE_T ctrl_handle = 0;
    GUI_RECT_T form_rect = {0};
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
    GUI_BG_T text_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_BLACK_COLOR, FALSE};
    GUIFORM_CHILD_WIDTH_T child_width = {0};
    GUI_COLOR_T font_color = MMI_WHITE_COLOR;
    GUI_FONT_T font_size = SONG_FONT_16;
    MMI_STRING_T text_string = {0};
    wchar text_str[2048] = {0};
    uint8 line_num = 0;
    uint16 width = 0;
    BOOLEAN result = FALSE;

    //ZmtGptKouYuTalk_DestoryCtrl(win_id, gpt_zuowen_talk_size);
    result = GUIFORM_CreatDynaCtrl(win_id, form_ctrl_id, GUIFORM_LAYOUT_ORDER);
    SCI_TRACE_LOW("%s: gpt_zuowen_talk_size = %d", __FUNCTION__, gpt_zuowen_talk_size);
    for(i = 0;i < gpt_zuowen_talk_size;i++)
    {
        GUITEXT_INIT_DATA_T text_init_data = {0};
        GUIFORM_DYNA_CHILD_T text_form_child_ctrl = {0};
        text_form_child_ctrl.child_handle = ZMT_GPT_FORM_TEXT_1_CTRL_ID + i;
        text_form_child_ctrl.init_data_ptr = &text_init_data;
        text_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        result = GUIFORM_CreatDynaChildCtrl(win_id, form_ctrl_id, &text_form_child_ctrl);
    }
    ctrl_handle = MMK_GetCtrlHandleByWin(win_id, form_ctrl_id);
    form_rect = zmt_gpt_list_rect;
    form_rect.bottom -= ZMT_GPT_LINE_HIGHT;
    GUIFORM_SetBg(ctrl_handle, &form_bg);
    GUIFORM_SetRect(ctrl_handle, &form_rect);

    for(i = 0;i < gpt_zuowen_talk_size;i++)
    {
        memset(text_str, 0, 2048);
        if(i == 0){
            GUI_GBToWstr(text_str, gpt_zuowen_talk_info[i]->str, strlen(gpt_zuowen_talk_info[i]->str));
        }else{
            GUI_UTF8ToWstr(text_str, 2048, gpt_zuowen_talk_info[i]->str, strlen(gpt_zuowen_talk_info[i]->str));
        }
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        line_num = GUI_CalculateStringLinesByPixelNum(160,text_string.wstr_ptr,text_string.wstr_len,font_size,0,TRUE);    
        if(line_num == 1){
            width = GUI_CalculateStringPiexlNum(text_string.wstr_ptr, text_string.wstr_len, font_size, 0) + 20;
        }else{
            width = zmt_gpt_list_rect.right - ZMT_GPT_LINE_WIDTH;
        }
        //SCI_TRACE_LOW("%s: width = %d", __FUNCTION__, width);
        text_ctrl_id = ZMT_GPT_FORM_TEXT_1_CTRL_ID + i;
        child_width.type = GUIFORM_CHILD_WIDTH_FIXED;
        child_width.add_data = width;
        GUIFORM_SetChildWidth(ctrl_handle, text_ctrl_id, &child_width);
        if(!gpt_zuowen_talk_info[i]->is_user){
            text_bg.color = MMI_WHITE_COLOR;
            font_color = MMI_BLACK_COLOR;
            GUIFORM_SetChildAlign(ctrl_handle, text_ctrl_id, GUIFORM_CHILD_ALIGN_LEFT);
        }else{
            text_bg.color = GUI_RGB2RGB565(0, 255, 0);
            font_color = MMI_WHITE_COLOR;
            GUIFORM_SetChildAlign(ctrl_handle, text_ctrl_id, GUIFORM_CHILD_ALIGN_RIGHT);            
        }
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LVMIDDLE);
        GUITEXT_SetBg(text_ctrl_id, &text_bg);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);        
        GUITEXT_SetString(text_ctrl_id, text_string.wstr_ptr, text_string.wstr_len, TRUE);
        GUITEXT_SetHandleTpMsg(TRUE, text_ctrl_id);
    }
    MMK_SetAtvCtrl(win_id, form_ctrl_id);
}

LOCAL void  ZmtGptZuoWen_DispalyRecord(MMI_WIN_ID_T win_id, int record_type)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    UILAYER_APPEND_BLT_T append_layer = {0};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T record_rect = {0};
    char text_str[1024] = {0};
    wchar text[1024] = {0};

    UILAYER_Clear(&gpt_zuowen_record_layer);
    append_layer.lcd_dev_info = gpt_zuowen_record_layer;
    append_layer.layer_level = UILAYER_LEVEL_HIGH;
    UILAYER_AppendBltLayer(&append_layer);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_16;
    text_style.font_color = MMI_BLACK_COLOR;
    switch(record_type)
    {
        case GPT_RECORD_TYPE_NONE:
        {
            zmt_gpt_record_rect = zmt_gpt_list_rect;
            zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - ZMT_GPT_LINE_HIGHT;
            zmt_gpt_record_rect.left += 1.5*ZMT_GPT_LINE_WIDTH;
            zmt_gpt_record_rect.right -= 1.5*ZMT_GPT_LINE_WIDTH;
            LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
            LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);

            MMIRES_GetText(ZMT_CHAT_GPT_RECORD_ACTION, win_id, &text_string);
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                &zmt_gpt_record_rect,
                &zmt_gpt_record_rect,
                &text_string,
                &text_style,
                text_state,
                GUISTR_TEXT_DIR_AUTO
            );
        }
        break;
        case GPT_RECORD_TYPE_RECORDING:
        {
            GUI_RECT_T tip_rect = {0};
            GUI_RECT_T text_rect = {0};
            GUI_RECT_T img_rect = {0};
            MMI_IMAGE_ID_T img_id[] = {
                IMG_ZMT_GPT_RECORD_1, IMG_ZMT_GPT_RECORD_2, IMG_ZMT_GPT_RECORD_3, IMG_ZMT_GPT_RECORD_4,
                IMG_ZMT_GPT_RECORD_5, IMG_ZMT_GPT_RECORD_6, IMG_ZMT_GPT_RECORD_7, IMG_ZMT_GPT_RECORD_8,
                IMG_ZMT_GPT_RECORD_9, IMG_ZMT_GPT_RECORD_1
            };
            uint8 index = 0;
            
            zmt_gpt_record_rect = zmt_gpt_list_rect;
            zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - 2*ZMT_GPT_LINE_HIGHT;
            zmt_gpt_record_rect.left += ZMT_GPT_LINE_WIDTH;
            zmt_gpt_record_rect.right -= ZMT_GPT_LINE_WIDTH;
            LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
            LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);

            text_style.align = ALIGN_HVMIDDLE;
            tip_rect = zmt_gpt_record_rect;
            tip_rect.bottom -= ZMT_GPT_LINE_HIGHT;
            MMIRES_GetText(ZMT_CHAT_GPT_RECORD_SLIDE_CANCEL, win_id, &text_string);
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                &tip_rect,
                &tip_rect,
                &text_string,
                &text_style,
                text_state,
                GUISTR_TEXT_DIR_AUTO
            );
            
            text_style.align = ALIGN_LVMIDDLE;
            sprintf(text_str, "%dS", 10 - (gpt_zuowen_record_times/1000));
            GUI_GBToWstr(text, text_str, strlen(text_str));
            text_string.wstr_ptr = text;
            text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
            text_rect = zmt_gpt_record_rect;
            text_rect.top = tip_rect.bottom;
            text_rect.bottom = text_rect.top + ZMT_GPT_LINE_HIGHT;
            text_rect.left += 5;
            text_rect.right = text_rect.left + 30;
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                &text_rect,
                &text_rect,
                &text_string,
                &text_style,
                text_state,
                GUISTR_TEXT_DIR_AUTO
            );
            
            index = (gpt_zuowen_record_times/200) % 10;
            //SCI_TRACE_LOW("%s: index = %d", __FUNCTION__, index);
            img_rect = zmt_gpt_record_rect;
            img_rect.top = tip_rect.bottom;
            img_rect.bottom = img_rect.top + ZMT_GPT_LINE_HIGHT;
            img_rect.left = text_rect.right + 5;
            GUIRES_DisplayImg(PNULL, &img_rect, PNULL, win_id, img_id[index], &gpt_zuowen_record_layer);
        }
        break;
        case GPT_RECORD_TYPE_VOICE_LOADING:
        {
            zmt_gpt_record_rect = zmt_gpt_list_rect;
            zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - ZMT_GPT_LINE_HIGHT;
            zmt_gpt_record_rect.left += ZMT_GPT_LINE_WIDTH;
            zmt_gpt_record_rect.right -= ZMT_GPT_LINE_WIDTH;

            LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
            LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);

            MMIRES_GetText(ZMT_CHAT_GPT_RECORD_INDENTIFY_NOW, win_id, &text_string);
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                &zmt_gpt_record_rect,
                &zmt_gpt_record_rect,
                &text_string,
                &text_style,
                text_state,
                GUISTR_TEXT_DIR_AUTO
            );
        }
        break;
        case GPT_RECORD_TYPE_TEXT_LOADING:
        {
            zmt_gpt_record_rect = zmt_gpt_list_rect;
            zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - ZMT_GPT_LINE_HIGHT;
            zmt_gpt_record_rect.left += ZMT_GPT_LINE_WIDTH;
            zmt_gpt_record_rect.right -= ZMT_GPT_LINE_WIDTH;

            LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
            LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);

            MMIRES_GetText(ZMT_CHAT_GPT_TEXT_LOADING, win_id, &text_string);
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                &zmt_gpt_record_rect,
                &zmt_gpt_record_rect,
                &text_string,
                &text_style,
                text_state,
                GUISTR_TEXT_DIR_AUTO
            );
        }
        break;
        case GPT_RECORD_TYPE_SUCCESS:
        {
            GUI_RECT_T text_rect = {0};
            GUI_RECT_T left_rect = {0};
            GUI_RECT_T right_rect = {0};
            uint8 line_num = 0;
            uint16 pixel_num = 0;
            
            zmt_gpt_record_rect = zmt_gpt_list_rect;
            zmt_gpt_record_rect.left += ZMT_GPT_LINE_WIDTH;
            zmt_gpt_record_rect.right -= ZMT_GPT_LINE_WIDTH;
            
        #if ZMT_GPT_USE_FOR_TEST != 0
            if(gpt_zuowen_record_text == NULL){
                gpt_zuowen_record_text = SCI_ALLOC_APPZ(200);
                memset(gpt_zuowen_record_text, 0, 200);
                strcpy(gpt_zuowen_record_text, "她俩向着光明和快乐飞走了，飞到那没有寒冷的地方");
            }
         #endif
            if(gpt_zuowen_record_text != NULL && strlen(gpt_zuowen_record_text) > 2)
            {
                sprintf(text_str, "%s", gpt_zuowen_record_text);
             #ifndef WIN32
                GUI_UTF8ToWstr(text, 1024, text_str, text_str);
             #else
                GUI_GBToWstr(text, text_str, strlen(text_str));
             #endif
                zmt_gpt_record_rect.left -= 0.5*ZMT_GPT_LINE_WIDTH;
                zmt_gpt_record_rect.right += 0.5*ZMT_GPT_LINE_WIDTH;
                text_string.wstr_ptr = text;
                text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                pixel_num = zmt_gpt_record_rect.right - zmt_gpt_record_rect.left;
                line_num = GUI_CalculateStringLinesByPixelNum(pixel_num,text_string.wstr_ptr,text_string.wstr_len,SONG_FONT_16,0,TRUE);
            }
            if(line_num > 0)
            {               
                zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - (line_num+1)*0.9*ZMT_GPT_LINE_HIGHT;
                LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
                LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
                text_style.align = ALIGN_HMIDDLE;
                text_rect = zmt_gpt_record_rect;
                text_rect.top += 5;
                GUISTR_DrawTextToLCDInRect(
                    (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                    &text_rect,
                    &text_rect,
                    &text_string,
                    &text_style,
                    text_state,
                    GUISTR_TEXT_DIR_AUTO
                );
                left_rect = zmt_gpt_record_left_rect;
                right_rect = zmt_gpt_record_right_rect;
                right_rect.left += 12;
                right_rect.right += 10;
            }
            else
            {
                zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - 2*ZMT_GPT_LINE_HIGHT;
                LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
                LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
                text_style.align = ALIGN_HMIDDLE;
                text_rect = zmt_gpt_record_rect;
                text_rect.top += 5;
                MMIRES_GetText(ZMT_CHAT_GPT_RECORD_INDENTIFY_FAIL, win_id, &text_string);
                GUISTR_DrawTextToLCDInRect(
                    (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                    &text_rect,
                    &text_rect,
                    &text_string,
                    &text_style,
                    text_state,
                    GUISTR_TEXT_DIR_AUTO
                );
                left_rect = zmt_gpt_record_center_rect;
            }
            GUIRES_DisplayImg(PNULL, &left_rect, PNULL, win_id, IMG_ZMT_GPT_CANCEL, &gpt_zuowen_record_layer);       
            GUIRES_DisplayImg(PNULL, &right_rect, PNULL, win_id, IMG_ZMT_GPT_OK, &gpt_zuowen_record_layer);
        }
        break;
        case GPT_RECORD_TYPE_FAIL:
        {
            zmt_gpt_record_rect = zmt_gpt_list_rect;
            zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - ZMT_GPT_LINE_HIGHT;
            zmt_gpt_record_rect.left += 1.5*ZMT_GPT_LINE_WIDTH;
            zmt_gpt_record_rect.right -= 1.5*ZMT_GPT_LINE_WIDTH;
            
            LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
            LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);

            MMIRES_GetText(ZMT_CHAT_GPT_RECORD_IDENTIFY_FAIL, win_id, &text_string);
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                &zmt_gpt_record_rect,
                &zmt_gpt_record_rect,
                &text_string,
                &text_style,
                text_state,
                GUISTR_TEXT_DIR_AUTO
            );

            if(gpt_zuowen_record_identify_timer_id){
                MMK_StopTimer(gpt_zuowen_record_identify_timer_id);
                gpt_zuowen_record_identify_timer_id = 0;
            }
            gpt_zuowen_record_identify_timer_id = MMK_CreateTimerCallback(2000, ZmtGptZuoWen_RecordIndentifyTimerCallback, PNULL, FALSE);
        }
        break;
        case GPT_RECORD_TYPE_ERROR:
        {
            zmt_gpt_record_rect = zmt_gpt_list_rect;
            zmt_gpt_record_rect.top = zmt_gpt_record_rect.bottom - ZMT_GPT_LINE_HIGHT;
            zmt_gpt_record_rect.left += 1.5*ZMT_GPT_LINE_WIDTH;
            zmt_gpt_record_rect.right -= 1.5*ZMT_GPT_LINE_WIDTH;
            
            LCD_DrawRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
            LCD_FillRoundedRect(&gpt_zuowen_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);
            
            MMIRES_GetText(ZMT_CHAT_GPT_RECORD_REQUESET_ERROR, win_id, &text_string);
            GUISTR_DrawTextToLCDInRect(
                (const GUI_LCD_DEV_INFO *)&gpt_zuowen_record_layer,
                &zmt_gpt_record_rect,
                &zmt_gpt_record_rect,
                &text_string,
                &text_style,
                text_state,
                GUISTR_TEXT_DIR_AUTO
            );

            if(gpt_zuowen_record_identify_timer_id){
                MMK_StopTimer(gpt_zuowen_record_identify_timer_id);
                gpt_zuowen_record_identify_timer_id = 0;
            }
            gpt_zuowen_record_identify_timer_id = MMK_CreateTimerCallback(2000, ZmtGptZuoWen_RecordIndentifyTimerCallback, PNULL, FALSE);
        }
        break;
    }
}


LOCAL void ZmtGptZuoWen_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};

    GUI_FillRect(&lcd_dev_info, zmt_gpt_win_rect, MMI_BLACK_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;
    MMIRES_GetText(ZMT_CHAT_GPT_ZUOWEN, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &zmt_gpt_title_rect,
        &zmt_gpt_title_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );

    if(zmt_gpt_zuowen_status < 3)
    {
        MMI_IMAGE_ID_T img_id = 0;
        GUI_RECT_T img_rect = {0, 2*ZMT_GPT_LINE_HIGHT, 0, 3*ZMT_GPT_LINE_HIGHT};
        GUI_RECT_T txt_rect = {0, 0, MMI_MAINSCREEN_WIDTH, 0};
        switch(zmt_gpt_zuowen_status)
       {
            case 0:
                {
                    img_id = IMG_ZMT_GPT_LOAD_1;
                    MMIRES_GetText(ZMT_CHAT_GPT_LOADING, win_id, &text_string);
                    img_rect.bottom = img_rect.top + 15;
                    img_rect.left = (MMI_MAINSCREEN_WIDTH - 32)/2;
                    img_rect.right = img_rect.left + 32;
                }
                break;
            case 1:
                {
                    img_id = IMG_ZMT_GPT_REQUEST_FAIL;
                    MMIRES_GetText(ZMT_CHAT_GPT_LOADING_FAIL, win_id, &text_string);
                    img_rect.bottom = img_rect.top + 67;
                    img_rect.left = (MMI_MAINSCREEN_WIDTH - 67)/2;
                    img_rect.right = img_rect.left + 67;
                }
                break;
            case 2:
                {
                    img_id = IMG_ZMT_GPT_NET_ERROR;
                    MMIRES_GetText(ZMT_CHAT_GPT_REQUSET_FAIL, win_id, &text_string);
                    img_rect.bottom = img_rect.top + 57;
                    img_rect.left = (MMI_MAINSCREEN_WIDTH - 70)/2;
                    img_rect.right = img_rect.left + 70;
                }
                break;
        }
        txt_rect.top = img_rect.bottom + 5;
        txt_rect.bottom = txt_rect.top + 20;
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &txt_rect,
            &txt_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
        GUIRES_DisplayImg(PNULL, &img_rect, PNULL, win_id, img_id, &lcd_dev_info);
        return;
    }

    ZmtGptZuoWen_ShowFormList(win_id);

    ZmtGptZuoWen_DispalyRecord(win_id, gpt_zuowen_record_type); 
}

LOCAL void ZmtGptZuoWen_CTL_PENOK(MMI_WIN_ID_T win_id, DPARAM param)
{
    //uint16 cur_idx = GUILIST_GetCurItemIndex(ZMT_GPT_ZUOWEN_LIST_CTRL_ID);
    uint8 cur_idx = 0;
    MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T *)param)->src_id;
    cur_idx = ctrl_id - ZMT_GPT_FORM_TEXT_1_CTRL_ID;
    SCI_TRACE_LOW("%s: cur_idx = %d", __FUNCTION__, cur_idx);
}

LOCAL void ZmtGptZuoWen_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    MMI_STRING_T text_string = {0};
    char text_str[200] = {0};
    if (UILAYER_IsMultiLayerEnable())
    {              
        UILAYER_CREATE_T create_info = {0};
        create_info.lcd_id = MAIN_LCD_ID;
        create_info.owner_handle = win_id;
        create_info.offset_x = zmt_gpt_list_rect.left;
        create_info.offset_y = zmt_gpt_list_rect.top;
        create_info.width = zmt_gpt_list_rect.right - zmt_gpt_list_rect.left;
        create_info.height = zmt_gpt_list_rect.bottom - zmt_gpt_list_rect.top;
        create_info.is_bg_layer = FALSE;
        create_info.is_static_layer = FALSE;
        UILAYER_CreateLayer(&create_info, &gpt_zuowen_record_layer);
    }
    gpt_zuowen_talk_size = 0;
    if(gpt_zuowen_talk_info[gpt_zuowen_talk_size] == NULL){
        gpt_zuowen_talk_info[gpt_zuowen_talk_size] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
    }           
    memset(gpt_zuowen_talk_info[gpt_zuowen_talk_size], 0, sizeof(gpt_talk_info_t));
    gpt_zuowen_talk_info[gpt_zuowen_talk_size]->is_user = FALSE;

    strcpy(text_str, "输入一段30字以内的开头，我帮你续写一篇作文");
    gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str = SCI_ALLOC_APPZ(strlen(text_str)+1);
    memset(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, 0, strlen(text_str)+1);
    strcpy(gpt_zuowen_talk_info[gpt_zuowen_talk_size]->str, text_str);

    gpt_zuowen_talk_size++;
    gpt_zuowen_talk_info[gpt_zuowen_talk_size] = NULL;
    zmt_gpt_zuowen_status = 3;
    if(MMK_IsFocusWin(win_id)){
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void ZmtGptZuoWen_CLOSE_WINDOW(void)
{
    ZmtGptZuoWen_ReleaseTalkInfo();
    gpt_zuowen_talk_size = 0;
    if (PNULL != gpt_zuowen_record_handle)
    {
        MMIRECORDSRV_StopRecord(gpt_zuowen_record_handle);
        MMIRECORDSRV_FreeRecordHandle(gpt_zuowen_record_handle);
        gpt_zuowen_record_handle = 0;
    }
    if(gpt_zuowen_record_timer_id != 0)
    {
        MMK_StopTimer(gpt_zuowen_record_timer_id);
        gpt_zuowen_record_timer_id = 0;
    }
    if(gpt_zuowen_record_identify_timer_id){
        MMK_StopTimer(gpt_zuowen_record_identify_timer_id);
        gpt_zuowen_record_identify_timer_id = 0;
    }
    zmt_gpt_zuowen_status = 0;
    gpt_zuowen_record_type = 0;
    gpt_zuowen_record_times = 0;
    gpt_zuowen_load_text = FALSE;
    if(gpt_zuowen_record_text){
        SCI_FREE(gpt_zuowen_record_text);
        gpt_zuowen_record_text = NULL;
    }
    MMIZDT_HTTP_Close();
}

LOCAL MMI_RESULT_E HandleZmtGptZuoWenWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ZmtGptZuoWen_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {             
                ZmtGptZuoWen_FULL_PAINT(win_id);               
            }
            break;
        case MSG_CTL_PENOK:
            {
                ZmtGptZuoWen_CTL_PENOK(win_id, param);
            }
            break;
        case MSG_TP_PRESS_LONG:
            {
                GUI_POINT_T point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(GUI_PointIsInRect(point, zmt_gpt_record_rect) && 
                    gpt_zuowen_record_type == GPT_RECORD_TYPE_NONE)
                {                    
                    ZmtGptZuoWen_StartRecord(win_id);
                }
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(GUI_PointIsInRect(point, zmt_gpt_record_rect) && 
                    gpt_zuowen_record_type == GPT_RECORD_TYPE_RECORDING)
                {
                    ZmtGptZuoWen_StopRecord(win_id, TRUE);
                }
                else if (!GUI_PointIsInRect(point, zmt_gpt_record_rect) &&
                    gpt_zuowen_record_type == GPT_RECORD_TYPE_RECORDING)
                {
                    ZmtGptZuoWen_StopRecord(win_id, FALSE);
                }
                else if(GUI_PointIsInRect(point, zmt_gpt_record_left_rect) && 
                    gpt_zuowen_record_type == GPT_RECORD_TYPE_SUCCESS)
                {
                    ZmtGptZuoWen_LeftIndentifyClick(win_id);
                }
                else if(GUI_PointIsInRect(point, zmt_gpt_record_right_rect) && 
                    gpt_zuowen_record_type == GPT_RECORD_TYPE_SUCCESS)
                {
                    ZmtGptZuoWen_RightIndentifyClick(win_id);
                }
                
            }
            break;
        case MSG_TIMER:
            {
                gpt_zuowen_record_times += 200;
                ZmtGptZuoWen_DispalyRecord(win_id, 1);
                if(gpt_zuowen_record_times >= 10 * 1000){
                    ZmtGptZuoWen_StopRecord(win_id, TRUE);
                }
            }
            break;  
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                ZmtGptZuoWen_CLOSE_WINDOW();
            }
            break;
        default:
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_ZMT_GPT_ZUOWEN_WIN_TAB) = {
    WIN_ID(ZMT_GPT_ZUOWEN_WIN_ID),
    WIN_FUNC((uint32)HandleZmtGptZuoWenWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZMT_CreateZmtGptZuoWenWin(void)
{
    if(MMK_IsOpenWin(ZMT_GPT_ZUOWEN_WIN_ID))
    {
        MMK_CloseWin(ZMT_GPT_ZUOWEN_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_ZMT_GPT_ZUOWEN_WIN_TAB, PNULL);
}


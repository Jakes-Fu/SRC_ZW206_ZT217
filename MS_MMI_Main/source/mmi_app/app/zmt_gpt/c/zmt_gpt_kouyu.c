/*****************************************************************************
** File Name:      zmt_gpt_store.c                                           *
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
#include "zmt_main_file.h"
#endif
#include "graphics_draw.h"
#include "img_dec_interface.h"

extern GUI_RECT_T zmt_gpt_win_rect;//窗口
extern GUI_RECT_T zmt_gpt_title_rect;//顶部
extern GUI_RECT_T zmt_gpt_list_rect;
GUI_RECT_T zmt_gpt_record_rect = {ZMT_GPT_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT - 2.2*ZMT_GPT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH - ZMT_GPT_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT - 0.2*ZMT_GPT_LINE_HIGHT};
GUI_RECT_T zmt_gpt_judge_record_rect = {0.5*ZMT_GPT_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT - 2.2*ZMT_GPT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH - 0.5*ZMT_GPT_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT - 0.2*ZMT_GPT_LINE_HIGHT};
GUI_RECT_T zmt_gpt_record_left_rect = {0.5*ZMT_GPT_LINE_WIDTH + 10, MMI_MAINSCREEN_HEIGHT - 1.2*ZMT_GPT_LINE_HIGHT - 5, 1.5*ZMT_GPT_LINE_WIDTH + 10, MMI_MAINSCREEN_HEIGHT - 0.2*ZMT_GPT_LINE_HIGHT - 5};
GUI_RECT_T zmt_gpt_record_right_rect = {MMI_MAINSCREEN_WIDTH - 1.5*ZMT_GPT_LINE_WIDTH - 10, MMI_MAINSCREEN_HEIGHT - 1.2*ZMT_GPT_LINE_HIGHT - 5, MMI_MAINSCREEN_WIDTH - 0.5*ZMT_GPT_LINE_WIDTH - 10, MMI_MAINSCREEN_HEIGHT - 0.2*ZMT_GPT_LINE_HIGHT - 5};

LOCAL uint8 zmt_gpt_topic_status = 0;
LOCAL uint8 gpt_kouyu_topic_index = 0;
LOCAL gpt_kouyu_info_t gpt_kouyu_info = {0};
LOCAL uint8 zmt_gpt_talk_status = 0;
LOCAL uint8 gpt_kouyu_talk_size = 0;
LOCAL gpt_talk_info_t * gpt_kouyu_talk_info[GPT_KOUYU_TALK_MAX_SIZE];
LOCAL GUI_LCD_DEV_INFO gpt_kouyu_record_layer = {0};
LOCAL uint8 gpt_kouyu_record_timer_id = 0;
LOCAL uint32 gpt_kouyu_record_times = 0;
LOCAL uint8 gpt_kouyu_record_identify_timer_id = 0;
LOCAL ZMT_GPT_RECORD_TYPE_E gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
LOCAL uint8 * gpt_kouyu_record_text = NULL;
LOCAL RECORD_SRV_HANDLE gpt_kouyu_record_handle = PNULL;
LOCAL MMISRV_HANDLE_T gpt_kouyu_player_handle=PNULL;
LOCAL BOOLEAN gpt_kouyu_load_text = FALSE;
LOCAL uint8 gpt_kouyu_cur_idx = 0;

LOCAL void ZmtGptKouYuTalk_RecordSuccess(MMI_WIN_ID_T win_id);
LOCAL void ZmtGptKouYuTalk_DispalyRecord(MMI_WIN_ID_T win_id, int record_type);
LOCAL void ZmtGptKouYuTalk_ShowFormList(MMI_WIN_ID_T win_id);

LOCAL void ZmtGptKouYuTalk_ReleaseTalkInfo(void)
{
    uint8 i = 0;
    for(i = 0;i < GPT_KOUYU_TALK_MAX_SIZE && i < gpt_kouyu_talk_size;i++)
    {
        if(gpt_kouyu_talk_info[i] != NULL){
            if(gpt_kouyu_talk_info[i]->str != NULL){
                SCI_FREE(gpt_kouyu_talk_info[i]->str);
                gpt_kouyu_talk_info[i]->str = NULL;
            }
            SCI_FREE(gpt_kouyu_talk_info[i]);
            gpt_kouyu_talk_info[i] = NULL;
        }
    }
}

LOCAL void ZmtGptKouYuTalk_DeleteFrontTwoMsg(void)
{
    uint8 index = 0;
    uint8 i = 0;
    uint16 size = 0;
    gpt_talk_info_t * talk_info[GPT_KOUYU_TALK_MAX_SIZE];

    for(i = 2;i < GPT_KOUYU_TALK_MAX_SIZE;i++)
    {
        if(gpt_kouyu_talk_info[i] != NULL){
            talk_info[index] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
            memset(talk_info[index], 0, sizeof(gpt_talk_info_t));
            talk_info[index]->is_user = gpt_kouyu_talk_info[i]->is_user;
            size = strlen(gpt_kouyu_talk_info[i]->str);
            talk_info[index]->str = SCI_ALLOC_APPZ(size + 1);
            memset(talk_info[index]->str, 0, size + 1);
            SCI_MEMCPY(talk_info[index]->str, gpt_kouyu_talk_info[i]->str, size);
            index++;
        }
    }
    SCI_TRACE_LOW("%s: index = %d", __FUNCTION__, index);
	
    ZmtGptKouYuTalk_ReleaseTalkInfo();
    for(i = 0;i < GPT_KOUYU_TALK_MAX_SIZE - 2;i++)
    {
        if(talk_info[i] != NULL){
            gpt_kouyu_talk_info[i] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
            memset(gpt_kouyu_talk_info[i], 0, sizeof(gpt_talk_info_t));
            gpt_kouyu_talk_info[i]->is_user = talk_info[i]->is_user;
            size = strlen(talk_info[i]->str);
            gpt_kouyu_talk_info[i]->str = SCI_ALLOC_APPZ(size + 1);
            memset(gpt_kouyu_talk_info[i]->str, 0, size + 1);
            SCI_MEMCPY(gpt_kouyu_talk_info[i]->str, talk_info[i]->str, size);
        }
    }
	
    for(i = 0;i < GPT_KOUYU_TALK_MAX_SIZE && i < index;i++)
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

LOCAL BOOLEAN ZmtGptKouYuTalk_PlayAmrDataNotify(MMISRV_HANDLE_T handle, MMISRVMGR_NOTIFY_PARAM_T *param)
{
    MMISRVAUD_REPORT_T *report_ptr = PNULL;
    BOOLEAN ret = FALSE;
    if(param != PNULL && handle > 0)
    {
        report_ptr = (MMISRVAUD_REPORT_T *)param->data;
        if(report_ptr != PNULL && handle == gpt_kouyu_player_handle)
        {
            switch(report_ptr->report)
            {
                case MMISRVAUD_REPORT_END:  
                    {
                        SCI_TRACE_LOW("%s player end", __FUNCTION__);
                        MMISRVAUD_Stop(handle);
                        MMISRVMGR_Free(handle);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return TRUE;
}

LOCAL void ZmtGptKouYuTalk_PlayAmrData(uint8 *data,uint32 data_len)
{
    MMISRVMGR_SERVICE_REQ_T req = {0};
    MMISRVAUD_TYPE_T audio_srv = {0};
    BOOLEAN result = FALSE;

    if(gpt_kouyu_player_handle)
    {
        MMISRVAUD_Stop(gpt_kouyu_player_handle);
        MMISRVMGR_Free(gpt_kouyu_player_handle);
        gpt_kouyu_player_handle = NULL;
    }

    req.is_auto_free = FALSE;
    req.notify = ZmtGptKouYuTalk_PlayAmrDataNotify;
    req.pri = MMISRVAUD_PRI_NORMAL;

    audio_srv.info.type = MMISRVAUD_TYPE_RING_BUF;
#if ZMT_GPT_USE_SELF_API != 0
    audio_srv.info.ring_buf.fmt = MMISRVAUD_RING_FMT_WAVE;
#else
    audio_srv.info.ring_buf.fmt = MMISRVAUD_RING_FMT_AMR;
#endif
    audio_srv.info.ring_buf.data = data;
    audio_srv.info.ring_buf.data_len = data_len;
    audio_srv.volume=9;
    audio_srv.all_support_route = MMISRVAUD_ROUTE_SPEAKER | MMISRVAUD_ROUTE_EARPHONE;

    gpt_kouyu_player_handle = MMISRVMGR_Request(STR_SRV_AUD_NAME, &req, &audio_srv);
    if(gpt_kouyu_player_handle > 0)
    {
        SCI_TRACE_LOW("%s chat_player_handle > 0", __FUNCTION__);
        result = MMISRVAUD_Play(gpt_kouyu_player_handle, 0);
        if(!result)
        {
            SCI_TRACE_LOW("%s chat_player error", __FUNCTION__);
            MMISRVMGR_Free(gpt_kouyu_player_handle);
            gpt_kouyu_player_handle = 0;
        }
        if(result == MMISRVAUD_RET_OK)
        {
            SCI_TRACE_LOW("%s chat_player success", __FUNCTION__);
        }
    }
    else
    {
        SCI_TRACE_LOW("%s chat_player_handle <=> 0", __FUNCTION__);
    }
}

LOCAL void ZmtGptKouYuTalk_StopAmrData(void)
{
    if(gpt_kouyu_player_handle)
    {
        MMISRVAUD_Stop(gpt_kouyu_player_handle);
        MMISRVMGR_Free(gpt_kouyu_player_handle);
        gpt_kouyu_player_handle = NULL;
    }
}

PUBLIC void ZmtGptKouYuTalk_RecTxtToVoiceResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        if(MMK_IsOpenWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
        {
            ZmtGptKouYuTalk_PlayAmrData(pRcv, Rcv_len);
        }
    }
    else
    {
        gpt_kouyu_record_type = GPT_RECORD_TYPE_VOICE_ERROR;
        SCI_TRACE_LOW("%s: 03voice load error!!", __FUNCTION__);
        if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID)){
            MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
        }
    }
}

PUBLIC void ZmtGptKouYuTalk_RecAiVoiceResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
    #if ZMT_GPT_USE_SELF_API != 0
        cJSON * root = cJSON_Parse(pRcv);
        if(root != NULL && root->type != cJSON_NULL)
        {
            cJSON * code = cJSON_GetObjectItem(root, "code");
            if(code->valueint == 200){
                cJSON *msg = cJSON_GetObjectItem(root, "msg");
                //SCI_TRACE_LOW("%s: msg = %s", __FUNCTION__, msg->valuestring);
                MMIZDT_HTTP_AppSend(TRUE, msg->valuestring, PNULL, 0, 1000, 0, 0, 10000, 0, 0, ZmtGptKouYuTalk_RecTxtToVoiceResultCb);
            }else{
                gpt_kouyu_record_type = GPT_RECORD_TYPE_VOICE_ERROR;
                SCI_TRACE_LOW("%s: 01voice load error, code = %d", __FUNCTION__, code->valueint);
                if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID)){
                    MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
                }
            }
        }
    #else
        if(MMK_IsOpenWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
        {
            ZmtGptKouYuTalk_PlayAmrData(pRcv, Rcv_len);
        }
    #endif
    }
    else
    {
        gpt_kouyu_record_type = GPT_RECORD_TYPE_VOICE_ERROR;
        SCI_TRACE_LOW("%s: 02voice load error!!", __FUNCTION__);
        if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID)){
            MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
        }
    }
}

PUBLIC void ZmtGptKouYuTalk_SelfRecordRecCallback(void *pUser,uint8 * pRcv,uint32 Rcv_len)
{
    uint16 flag= 0;
    int i = 0;
    int j = 0;
    uint16 size = 0;
    char json_str[4096]={0};
    cJSON *rcvJsonPtr;
    char * result;

    result=SCI_ALLOCA(Rcv_len+1);
    SCI_MEMSET(result,0,Rcv_len+1);
    SCI_MEMCPY(result,pRcv,Rcv_len);
	
    for (i = 0 ,j = 0;i < Rcv_len ; i++)
    {
        if(flag!=0)
        {
            if(result[i]!='\n'&&result[i]!='\r'&&result[i]!='\0')
            {
                json_str[j++]=result[i];
            }
        }
        if(result[i] == '{'&&flag==0)
        {
            flag = i;
            json_str[j++]=result[i];
        }else if(result[i] == '}')
        {
            json_str[j++]='\0';
            break;
        }
    }
    SCI_TRACE_LOW("%s: json_str: %s", __FUNCTION__, json_str);
     if(pRcv != NULL && Rcv_len > 0)
     {
        cJSON * root = cJSON_Parse(json_str);
        cJSON * data = cJSON_GetObjectItem(root, "data");
        cJSON * code = cJSON_GetObjectItem(root, "code");
        if(root != NULL && root->type != cJSON_NULL)
        {
            if(code->valueint == 200 && data != NULL && data->valuestring != NULL)
            {
                if(gpt_kouyu_record_text){
                    SCI_FREE(gpt_kouyu_record_text);
                    gpt_kouyu_record_text = NULL;
                }
                size = strlen(data->valuestring);
                if(size > 0){
                    gpt_kouyu_record_text = SCI_ALLOC_APPZ(size + 1);
                    memset(gpt_kouyu_record_text, 0, size + 1);
                    strcpy(gpt_kouyu_record_text, data->valuestring);
                    gpt_kouyu_record_type = GPT_RECORD_TYPE_SUCCESS;
                }else{
                    SCI_TRACE_LOW("%s: success data empty!!", __FUNCTION__);
                    gpt_kouyu_record_type = GPT_RECORD_TYPE_SUCCESS_EMPTY;
                }
            }
            else
            {
                SCI_TRACE_LOW("%s: code error!!, code = %d", __FUNCTION__, code->valueint);
                if(gpt_kouyu_record_text){
                    SCI_FREE(gpt_kouyu_record_text);
                    gpt_kouyu_record_text = NULL;
                }
                gpt_kouyu_record_type = GPT_RECORD_TYPE_VOICE_ERROR;
            }
            cJSON_Delete(root);
        }
        else
        {
            SCI_TRACE_LOW("%s: data error!!", __FUNCTION__);
            if(gpt_kouyu_record_text){
                SCI_FREE(gpt_kouyu_record_text);
                gpt_kouyu_record_text = NULL;
            }
            gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_FAIL;
        }
     }
     else
    {
        SCI_TRACE_LOW("%s: requset error!!", __FUNCTION__);
        if(gpt_kouyu_record_text){
            SCI_FREE(gpt_kouyu_record_text);
            gpt_kouyu_record_text = NULL;
        }
        gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_ERROR;
    }
    SCI_FREE(result);
    if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID)){
        if(gpt_kouyu_record_type == GPT_RECORD_TYPE_SUCCESS){
            ZmtGptKouYuTalk_RecordSuccess(ZMT_GPT_KOUYU_TALK_WIN_ID);
        }else{
            ZmtGptKouYuTalk_DispalyRecord(ZMT_GPT_KOUYU_TALK_WIN_ID, gpt_kouyu_record_type);
        }
    }
    ZMT_TCP_SuccessSendingStop();
    //这里不能直接走app msg处理，需要添加个图层来重绘full paint
    /*if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
    }*/
}

PUBLIC void ZmtGptKouYuTalk_RecAiSelfTextResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        cJSON * root = cJSON_Parse(pRcv);
        if(root != NULL && root->type != cJSON_NULL)
        {
            cJSON * code = cJSON_GetObjectItem(root, "code");
            cJSON * data = cJSON_GetObjectItem(root, "data");
            if(code->valueint == 200)
            {
                if(gpt_kouyu_record_text){
                    SCI_FREE(gpt_kouyu_record_text);
                    gpt_kouyu_record_text = NULL;
                }
                gpt_kouyu_record_text = SCI_ALLOC_APPZ(strlen(data->valuestring)+1);
                memset(gpt_kouyu_record_text, 0, strlen(data->valuestring)+1);
                strcpy(gpt_kouyu_record_text, data->valuestring);
                gpt_kouyu_record_type = GPT_RECORD_TYPE_SUCCESS;
            }
            else
            {
                SCI_TRACE_LOW("%s: code error!!, code = %d", __FUNCTION__, code->valueint);
                if(gpt_kouyu_record_text){
                    SCI_FREE(gpt_kouyu_record_text);
                    gpt_kouyu_record_text = NULL;
                }
                gpt_kouyu_record_type = GPT_RECORD_TYPE_VOICE_ERROR;
            }
            cJSON_Delete(root);
        }
        else
        {
            SCI_TRACE_LOW("%s: data error!!", __FUNCTION__);
            if(gpt_kouyu_record_text){
                SCI_FREE(gpt_kouyu_record_text);
                gpt_kouyu_record_text = NULL;
            }
            gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_FAIL;
        }
    }
    else
    {
        SCI_TRACE_LOW("%s: requset error!!", __FUNCTION__);
        if(gpt_kouyu_record_text){
            SCI_FREE(gpt_kouyu_record_text);
            gpt_kouyu_record_text = NULL;
        }
        gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_ERROR;
    }
    if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

PUBLIC void ZmtGptKouYuTalk_RecAiTextResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
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
                SCI_TRACE_LOW("%s: item_string = %s", __FUNCTION__, item->valuestring);
                SCI_TRACE_LOW("%s: item_len = %d", __FUNCTION__, strlen(item->valuestring));
                if(item != NULL && item->type != cJSON_NULL && strlen(item->valuestring) > 0)
                {
                    if(gpt_kouyu_record_text){
                        SCI_FREE(gpt_kouyu_record_text);
                        gpt_kouyu_record_text = NULL;
                    }
                    gpt_kouyu_record_text = SCI_ALLOC_APPZ(strlen(item->valuestring)+1);
                    memset(gpt_kouyu_record_text, 0, strlen(item->valuestring)+1);
                    strcpy(gpt_kouyu_record_text, item->valuestring);
                    gpt_kouyu_record_type = GPT_RECORD_TYPE_SUCCESS;
                }
                else
                {
                    SCI_TRACE_LOW("%s: item is NULL!!", __FUNCTION__);
                    if(gpt_kouyu_record_text){
                        SCI_FREE(gpt_kouyu_record_text);
                        gpt_kouyu_record_text = NULL;
                    }
                    gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_FAIL;
                }
            }
            
        }
        else
        {
            SCI_TRACE_LOW("%s: result is empty!!", __FUNCTION__);
            if(gpt_kouyu_record_text){
                SCI_FREE(gpt_kouyu_record_text);
                gpt_kouyu_record_text = NULL;
            }
            gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_FAIL;
        }
        cJSON_Delete(root);
    }
    else
    {
        SCI_TRACE_LOW("%s: result is empty!!", __FUNCTION__);
        if(gpt_kouyu_record_text){
            SCI_FREE(gpt_kouyu_record_text);
            gpt_kouyu_record_text = NULL;
        }
        gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_ERROR;
    }
    if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

PUBLIC void ZmtGptKouYuTalk_RecvSelfResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
#if ZMT_GPT_USE_FOR_TEST != 0
    uint8 * buf = NULL;
    uint32 len = 0;
    {
        if(zmt_file_exist(ZMT_GPT_SELF_KOUYU_TALK_PATH)){
            buf = zmt_file_data_read(ZMT_GPT_SELF_KOUYU_TALK_PATH, &len);
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
             cJSON * reply = cJSON_GetObjectItem(data, "reply");
             if(reply != NULL && reply->type != cJSON_NULL && strlen(reply->valuestring) > 0)
             {
                SCI_TRACE_LOW("%s: gpt_kouyu_talk_size = %d", __FUNCTION__, gpt_kouyu_talk_size);
                if(gpt_kouyu_talk_info[gpt_kouyu_talk_size] == NULL){
                    gpt_kouyu_talk_info[gpt_kouyu_talk_size] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
                }           
                memset(gpt_kouyu_talk_info[gpt_kouyu_talk_size], 0, sizeof(gpt_talk_info_t));
                gpt_kouyu_talk_info[gpt_kouyu_talk_size]->is_user = FALSE;
				
                gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str = SCI_ALLOC_APPZ(strlen(reply->valuestring)+1);
                memset(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, 0, strlen(reply->valuestring)+1);
                //strcpy(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, "Hello! What's your");
                strcpy(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, reply->valuestring);

                gpt_kouyu_talk_size++;
                gpt_kouyu_talk_info[gpt_kouyu_talk_size] = NULL;

                zmt_gpt_talk_status = 3;
                gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
             }
             else
             {
                gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_FAIL;
             }
        }
        else
        {
            gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_FAIL;
        }
        cJSON_Delete(root);
    }
    else
    {
        gpt_kouyu_record_type = GPT_RECORD_TYPE_TXT_ERROR;
    }
    SCI_TRACE_LOW("%s: gpt_kouyu_record_type = %d", __FUNCTION__, gpt_kouyu_record_type);  
    gpt_kouyu_load_text = FALSE;
    gpt_kouyu_cur_idx = gpt_kouyu_talk_size - 1;
    if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
#if ZMT_GPT_USE_FOR_TEST != 0
    if(pRcv != NULL){
        SCI_FREE(pRcv);
    }
#endif
}

PUBLIC void ZmtGptKouYuTalk_RecvResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
#if ZMT_GPT_USE_FOR_TEST != 0
    uint8 * buf = NULL;
    uint32 len = 0;
    {
        if(zmt_file_exist(ZMT_GPT_KOUYU_TALK_PATH)){
            buf = zmt_file_data_read(ZMT_GPT_KOUYU_TALK_PATH, &len);
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
                SCI_TRACE_LOW("%s: gpt_kouyu_talk_size = %d", __FUNCTION__, gpt_kouyu_talk_size);
                if(gpt_kouyu_talk_info[gpt_kouyu_talk_size] == NULL){
                    gpt_kouyu_talk_info[gpt_kouyu_talk_size] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
                }           
                memset(gpt_kouyu_talk_info[gpt_kouyu_talk_size], 0, sizeof(gpt_talk_info_t));
                gpt_kouyu_talk_info[gpt_kouyu_talk_size]->is_user = FALSE;
				
                gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str = SCI_ALLOC_APPZ(strlen(content->valuestring)+1);
                memset(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, 0, strlen(content->valuestring)+1);
                //strcpy(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, "Hello! What's your");
                strcpy(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, content->valuestring);

                gpt_kouyu_talk_size++;
                gpt_kouyu_talk_info[gpt_kouyu_talk_size] = NULL;

                zmt_gpt_talk_status = 3;
            }
        }
        else
        {
            zmt_gpt_talk_status = 1;
        }
        cJSON_Delete(root);
    }
    else
    {
        zmt_gpt_talk_status = 2;
    }
    gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
    gpt_kouyu_load_text = FALSE;
    if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_KOUYU_TALK_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
#if ZMT_GPT_USE_FOR_TEST != 0
    if(pRcv != NULL){
        SCI_FREE(pRcv);
    }
#endif
}

LOCAL void ZmtGptKouYuTalk_NotifyRecordCallback(RECORD_SRV_HANDLE record_srv_handle, 
    MMIRECORD_SRV_SUPPORT_TYPE_E support_type, 
    MMIRECORD_SRV_RESULT_E result
    )
{
    SCI_TRACE_LOW("%s: support_type = %d", __FUNCTION__, support_type);
    if (record_srv_handle == gpt_kouyu_record_handle &&
        MMIRECORD_SRV_SUPPORT_RECORD == support_type)
    {
        SCI_TRACE_LOW("%s: result = %d", __FUNCTION__, result);
    }
}

LOCAL void ZmtGptKouYuTalk_StartRecord(MMI_WIN_ID_T win_id)
{
    MMI_STRING_T  call_name_str = {0};
    MMIRECORD_SRV_RECORD_PARAM_T record_param = {0};
    MMIFILE_DEVICE_E record_dev = MMIAPIRECORD_GetRecordStorage();
    MMIRECORD_SRV_RESULT_E  record_result = MMIRECORD_SRV_RESULT_SUCCESS;

    if(gpt_kouyu_load_text){
        SCI_TRACE_LOW("%s: load text xxing", __FUNCTION__);
        return;
    }

    if (PNULL != gpt_kouyu_record_handle)
    {
        MMIRECORDSRV_StopRecord(gpt_kouyu_record_handle);
        MMIRECORDSRV_FreeRecordHandle(gpt_kouyu_record_handle);
        gpt_kouyu_record_handle = 0;
    }

    ZmtGptKouYuTalk_StopAmrData();
    gpt_kouyu_record_handle = MMIRECORDSRV_RequestRecordHandle(ZmtGptKouYuTalk_NotifyRecordCallback);
    if (PNULL == gpt_kouyu_record_handle)
    {
        return;
    }

    if(gpt_kouyu_record_timer_id != 0){
        return;
    }
    gpt_kouyu_record_timer_id = MMK_CreateWinTimer(win_id, 200, TRUE);
    MMK_StartWinTimer(win_id, gpt_kouyu_record_timer_id, 200, TRUE);
    gpt_kouyu_record_times = 0;
    gpt_kouyu_record_type = GPT_RECORD_TYPE_RECORDING;
    if(MMK_IsFocusWin(win_id)){
        ZmtGptKouYuTalk_DispalyRecord(win_id, gpt_kouyu_record_type);
    }
	
#if ZMT_GPT_USE_SELF_API != 0  
    call_name_str.wstr_ptr = ZMT_GPT_RECORD_SELF_FILE_L;
    call_name_str.wstr_len = MMIAPICOM_Wstrlen(ZMT_GPT_RECORD_SELF_FILE_L);
    record_param.fmt = MMISRVAUD_RECORD_FMT_AMR;
#else
    call_name_str.wstr_ptr = ZMT_GPT_RECORD_FILE_L;
    call_name_str.wstr_len = MMIAPICOM_Wstrlen(ZMT_GPT_RECORD_FILE_L);
    record_param.fmt = MMISRVAUD_RECORD_FMT_PCM;
#endif

    record_param.prefix_ptr = PNULL;
    record_param.record_dev_ptr = &record_dev;
    record_param.record_file_id_ptr = PNULL;
    record_param.record_name_ptr = &call_name_str;
    record_param.source = MMISRVAUD_RECORD_SOURCE_NORMAL;
    record_param.record_param_ptr = PNULL;
    record_param.volume = MMIAPISET_GetMultimVolume();
    record_param.is_create_file_name = TRUE;

    record_result = MMIRECORDSRV_StartRecordFullFile(gpt_kouyu_record_handle, record_param);
}

LOCAL void ZmtGptKouYuTalk_StopRecord(MMI_WIN_ID_T win_id, BOOLEAN is_send)
{
    if (PNULL != gpt_kouyu_record_handle)
    {
        MMIRECORDSRV_StopRecord(gpt_kouyu_record_handle);
        MMIRECORDSRV_FreeRecordHandle(gpt_kouyu_record_handle);
        gpt_kouyu_record_handle = 0;
    }
    if(gpt_kouyu_record_timer_id != 0)
    {
        MMK_StopTimer(gpt_kouyu_record_timer_id);
        gpt_kouyu_record_timer_id = 0;
    }
    
    if(is_send){
        uint8 * data_buf = NULL;
        uint32 data_size = 0;
        uint32 size;
    #if ZMT_GPT_USE_SELF_API != 0
        data_buf = zmt_file_data_read(ZMT_GPT_RECORD_SELF_FILE_C, &data_size);
        SCI_TRACE_LOW("%s: data_size = %d", __FUNCTION__, data_size);
        if(data_buf != NULL && data_size > 2){
            ZmtGpt_SendSelfRecord(1, data_buf, data_size, ZmtGptKouYuTalk_SelfRecordRecCallback);
            SCI_FREE(data_buf);
        }
        zmt_file_delete(ZMT_GPT_RECORD_SELF_FILE_C);
    #else
        data_buf = zmt_file_data_read(ZMT_GPT_RECORD_FILE_C, &data_size);
        SCI_TRACE_LOW("%s: data_size = %d", __FUNCTION__, data_size);
        if(data_buf != NULL && data_size > 2){
            ZmtGpt_SendRecord(1737, data_buf, data_size);
            SCI_FREE(data_buf);
        }
        zmt_file_delete(ZMT_GPT_RECORD_FILE_C);
    #endif
    #ifdef WIN32
        gpt_kouyu_record_type = GPT_RECORD_TYPE_SUCCESS;
    #else
        gpt_kouyu_record_type = GPT_RECORD_TYPE_VOICE_LOADING;
    #endif
    }
    else
    {
    #if ZMT_GPT_USE_SELF_API != 0
        zmt_file_delete(ZMT_GPT_RECORD_SELF_FILE_C);
    #else
        zmt_file_delete(ZMT_GPT_RECORD_FILE_C);
    #endif
        gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
    }
    if(MMK_IsFocusWin(win_id)){
        ZmtGptKouYuTalk_DispalyRecord(win_id, gpt_kouyu_record_type);
    }
}

LOCAL void ZmtGptKouYuTalk_LeftIndentifyClick(MMI_WIN_ID_T win_id)
{
    if(gpt_kouyu_record_text != NULL){
        SCI_FREE(gpt_kouyu_record_text);
        gpt_kouyu_record_text = NULL;
    }
    gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
    gpt_kouyu_load_text = FALSE;
    if(MMK_IsFocusWin(win_id)){
        ZmtGptKouYuTalk_DispalyRecord(win_id, gpt_kouyu_record_type);
    }
}

LOCAL void ZmtGptKouYuTalk_RightIndentifyClick(MMI_WIN_ID_T win_id)
{
    if(gpt_kouyu_record_text == NULL || strlen(gpt_kouyu_record_text) < 2)
    {
        SCI_TRACE_LOW("%s:  gpt_kouyu_record_text empty!!", __FUNCTION__);
        return;
    }
    if(gpt_kouyu_talk_info[gpt_kouyu_talk_size] == NULL){
        gpt_kouyu_talk_info[gpt_kouyu_talk_size] = SCI_ALLOC_APPZ(sizeof(gpt_talk_info_t));
    }
    gpt_kouyu_talk_info[gpt_kouyu_talk_size]->is_user = TRUE;
    gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str = SCI_ALLOC_APPZ(strlen(gpt_kouyu_record_text)+1);
    memset(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, 0, strlen(gpt_kouyu_record_text)+1);
    strcpy(gpt_kouyu_talk_info[gpt_kouyu_talk_size]->str, gpt_kouyu_record_text);
    gpt_kouyu_talk_size++;
    gpt_kouyu_record_type = GPT_RECORD_TYPE_TEXT_LOADING;
    if(gpt_kouyu_talk_size >= GPT_KOUYU_TALK_MAX_SIZE){
        ZmtGptKouYuTalk_DeleteFrontTwoMsg();
        gpt_kouyu_talk_size -= 2;
        SCI_TRACE_LOW("%s: 01gpt_kouyu_talk_size = %d", __FUNCTION__, gpt_kouyu_talk_size);
    }
    gpt_kouyu_load_text = TRUE;
#if ZMT_GPT_USE_SELF_API != 0
    if(gpt_kouyu_talk_size <= 2){
        ZmtGpt_SendSelfTxt(0, gpt_kouyu_record_text, TRUE);
    }else{
        ZmtGpt_SendSelfTxt(0, gpt_kouyu_record_text, FALSE);
    }
#else
    ZmtGpt_SendTxt(89, gpt_kouyu_record_text, gpt_kouyu_info.talk_list[gpt_kouyu_topic_index].talk, gpt_kouyu_info.field, 0);   
#endif
    gpt_kouyu_cur_idx = gpt_kouyu_talk_size - 1;
    if(MMK_IsFocusWin(win_id)){
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    }
    if(gpt_kouyu_record_text){
        SCI_FREE(gpt_kouyu_record_text);
        gpt_kouyu_record_text = NULL;
    }
}

LOCAL void ZmtGptKouYuTalk_RecordIndentifyTimerCallback(uint8 timer_id, uint32 param)
{
    if(timer_id == gpt_kouyu_record_identify_timer_id)
    {
        MMK_StopTimer(gpt_kouyu_record_identify_timer_id);
        gpt_kouyu_record_identify_timer_id = 0;
        gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
        if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TALK_WIN_ID)){
            ZmtGptKouYuTalk_DispalyRecord(ZMT_GPT_KOUYU_TALK_WIN_ID, gpt_kouyu_record_type);
        }
    }
}

LOCAL void ZmtGptKouYuTalk_StartRecordIndentifyTimer(void)
{
    if(gpt_kouyu_record_identify_timer_id){
        MMK_StopTimer(gpt_kouyu_record_identify_timer_id);
        gpt_kouyu_record_identify_timer_id = 0;
    }
    gpt_kouyu_record_identify_timer_id = MMK_CreateTimerCallback(2000, ZmtGptKouYuTalk_RecordIndentifyTimerCallback, PNULL, FALSE);
}

LOCAL void  ZmtGptKouYuTalk_RecordStatus(MMI_WIN_ID_T win_id, MMI_TEXT_ID_T text_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T text_rect = {0};

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_20;
    text_style.font_color = GPT_WIN_BG_COLOR;
    
    LCD_DrawRoundedRect(&gpt_kouyu_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, GPT_WIN_BG_COLOR);
    LCD_FillRoundedRect(&gpt_kouyu_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);

    text_rect = zmt_gpt_record_rect;
    text_rect.left += 5;
    text_rect.right -= 5;
    MMIRES_GetText(text_id, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&gpt_kouyu_record_layer,
        &text_rect,
        &text_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
}

LOCAL void  ZmtGptKouYuTalk_Recording(MMI_WIN_ID_T win_id)
{
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T record_rect = {0};
    char text_str[1024] = {0};
    wchar text[1024] = {0};
    GUI_RECT_T tip_rect = {0};
    GUI_RECT_T text_rect = {0};
    GUI_RECT_T img_rect = {0};
    MMI_IMAGE_ID_T img_id[] = {
        IMG_ZMT_GPT_RECORD_1, IMG_ZMT_GPT_RECORD_2, IMG_ZMT_GPT_RECORD_3, IMG_ZMT_GPT_RECORD_4,
        IMG_ZMT_GPT_RECORD_5, IMG_ZMT_GPT_RECORD_6, IMG_ZMT_GPT_RECORD_7, IMG_ZMT_GPT_RECORD_8,
        IMG_ZMT_GPT_RECORD_9, IMG_ZMT_GPT_RECORD_1
    };
    uint8 index = 0;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_20;
    text_style.font_color = GPT_WIN_BG_COLOR;
    
    LCD_DrawRoundedRect(&gpt_kouyu_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, GPT_WIN_BG_COLOR);
    LCD_FillRoundedRect(&gpt_kouyu_record_layer, zmt_gpt_record_rect, zmt_gpt_record_rect, MMI_WHITE_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    tip_rect = zmt_gpt_record_rect;
    tip_rect.bottom -= ZMT_GPT_LINE_HIGHT;
    MMIRES_GetText(ZMT_CHAT_GPT_RECORD_SLIDE_CANCEL, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&gpt_kouyu_record_layer,
        &tip_rect,
        &tip_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );

    memset(&text_str, 0, 1024);
    memset(&text, 0, 1024);
    text_style.align = ALIGN_LVMIDDLE;
    sprintf(text_str, "%dS", 10 - (gpt_kouyu_record_times/1000));
    GUI_GBToWstr(text, text_str, strlen(text_str));
    text_string.wstr_ptr = text;
    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
    text_rect = zmt_gpt_record_rect;
    text_rect.top = tip_rect.bottom;
    text_rect.bottom = text_rect.top + ZMT_GPT_LINE_HIGHT;
    text_rect.left += 5;
    text_rect.right = text_rect.left + 30;
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&gpt_kouyu_record_layer,
        &text_rect,
        &text_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
            
    index = (gpt_kouyu_record_times/200) % 10;
    //SCI_TRACE_LOW("%s: index = %d", __FUNCTION__, index);
    img_rect = zmt_gpt_record_rect;
    img_rect.top = tip_rect.bottom;
    img_rect.bottom = img_rect.top + ZMT_GPT_LINE_HIGHT;
    img_rect.left = text_rect.right + 5;
    GUIRES_DisplayImg(PNULL, &img_rect, PNULL, win_id, img_id[index], &gpt_kouyu_record_layer);
}

LOCAL void  ZmtGptKouYuTalk_RecordSuccess(MMI_WIN_ID_T win_id)
{
    UILAYER_APPEND_BLT_T append_layer = {0};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T record_rect = {0};
    char text_str[1024] = {0};
    wchar text[1024] = {0};
    GUI_RECT_T text_rect = {0};
    GUI_RECT_T left_rect = {0};
    GUI_RECT_T right_rect = {0};
    uint8 line_num = 0;
    uint16 pixel_num = 0;

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_18;
    text_style.font_color = GPT_WIN_BG_COLOR;

    UILAYER_Clear(&gpt_kouyu_record_layer);
    append_layer.lcd_dev_info = gpt_kouyu_record_layer;
    append_layer.layer_level = UILAYER_LEVEL_HIGH;
    UILAYER_AppendBltLayer(&append_layer);

#if ZMT_GPT_USE_FOR_TEST != 0
    if(gpt_kouyu_record_text == NULL){
        gpt_kouyu_record_text = SCI_ALLOC_APPZ(200);
        memset(gpt_kouyu_record_text, 0, 200);
        strcpy(gpt_kouyu_record_text, "piazzaaaa");
    }
#endif
    if(gpt_kouyu_record_text != NULL && strlen(gpt_kouyu_record_text) > 2)
    {
        memset(&text_str, 0, 1024);
        memset(&text, 0, 1024);
        strcpy(text_str, gpt_kouyu_record_text);
    #ifndef WIN32
        GUI_UTF8ToWstr(text, 1024, text_str, strlen(text_str));
    #else
        GUI_GBToWstr(text, text_str, strlen(text_str));
    #endif
        text_string.wstr_ptr = text;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        pixel_num = zmt_gpt_record_rect.right - zmt_gpt_record_rect.left;
        line_num = GUI_CalculateStringLinesByPixelNum(pixel_num,text_string.wstr_ptr,text_string.wstr_len,DP_FONT_18,0,TRUE);
    }
    if(line_num > 0)
    {
        zmt_gpt_judge_record_rect.top = zmt_gpt_judge_record_rect.bottom - (line_num+1)*0.9*ZMT_GPT_LINE_HIGHT - 1.5*ZMT_GPT_LINE_HIGHT;
        LCD_DrawRoundedRect(&gpt_kouyu_record_layer, zmt_gpt_judge_record_rect, zmt_gpt_judge_record_rect, GPT_WIN_BG_COLOR);
        LCD_FillRoundedRect(&gpt_kouyu_record_layer, zmt_gpt_judge_record_rect, zmt_gpt_judge_record_rect, MMI_WHITE_COLOR);

        text_rect = zmt_gpt_judge_record_rect;
        text_rect.left += 5;
        text_rect.right -= 5;
        text_rect.top += 5;
        text_rect.bottom = zmt_gpt_record_left_rect.top;
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&gpt_kouyu_record_layer,
            &text_rect,
            &text_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
    }
    GUIRES_DisplayImg(PNULL, &zmt_gpt_record_left_rect, PNULL, win_id, IMG_ZMT_GPT_CANCEL, &gpt_kouyu_record_layer);       
    GUIRES_DisplayImg(PNULL, &zmt_gpt_record_right_rect, PNULL, win_id, IMG_ZMT_GPT_OK, &gpt_kouyu_record_layer);
}

LOCAL void  ZmtGptKouYuTalk_DispalyRecord(MMI_WIN_ID_T win_id, int record_type)
{
    UILAYER_APPEND_BLT_T append_layer = {0};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_RECT_T record_rect = {0};
    char text_str[1024] = {0};
    wchar text[1024] = {0};

    UILAYER_Clear(&gpt_kouyu_record_layer);
    append_layer.lcd_dev_info = gpt_kouyu_record_layer;
    append_layer.layer_level = UILAYER_LEVEL_HIGH;
    UILAYER_AppendBltLayer(&append_layer);

    switch(record_type)
    {
        case GPT_RECORD_TYPE_NONE:
            {
                ZmtGptKouYuTalk_RecordStatus(win_id, ZMT_CHAT_GPT_RECORD_ACTION);
            }
            break;
        case GPT_RECORD_TYPE_RECORDING:
            {
                ZmtGptKouYuTalk_Recording(win_id);
            }
            break;
        case GPT_RECORD_TYPE_VOICE_LOADING:
            {
                ZmtGptKouYuTalk_RecordStatus(win_id, ZMT_CHAT_GPT_RECORD_INDENTIFY_NOW);
            }
            break;
        case GPT_RECORD_TYPE_TEXT_LOADING:
            {
                ZmtGptKouYuTalk_RecordStatus(win_id, ZMT_CHAT_GPT_TEXT_LOADING);
            }
            break;
        case GPT_RECORD_TYPE_SUCCESS:
            {
                ZmtGptKouYuTalk_RecordSuccess(win_id);
            }
            break;
            
        case GPT_RECORD_TYPE_SUCCESS_EMPTY:
            {
                ZmtGptKouYuTalk_RecordStatus(win_id, ZMT_CHAT_GPT_RECORD_INDENTIFY_FAIL);
                ZmtGptKouYuTalk_StartRecordIndentifyTimer();
            }
            break;
        case GPT_RECORD_TYPE_TXT_FAIL:
            {
                ZmtGptKouYuTalk_RecordStatus(win_id, ZMT_CHAT_GPT_TXT_IDENTIFY_FAIL);
                ZmtGptKouYuTalk_StartRecordIndentifyTimer();
            }
            break;
        case GPT_RECORD_TYPE_TXT_ERROR:
            {
                ZmtGptKouYuTalk_RecordStatus(win_id, ZMT_CHAT_GPT_TXT_REQUESET_ERROR);
                ZmtGptKouYuTalk_StartRecordIndentifyTimer();
            }
            break;
        case GPT_RECORD_TYPE_VOICE_ERROR:
            {
                ZmtGptKouYuTalk_RecordStatus(win_id, ZMT_CHAT_GPT_VOICE_REQUESET_ERROR);
                ZmtGptKouYuTalk_StartRecordIndentifyTimer();
            }
            break;
    }
}

LOCAL BOOLEAN ZmtGptKouYuTalk_DestoryDynaCtrl(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    if(MMK_GetCtrlHandleByWin(win_id, ZMT_GPT_FORM_CTRL_ID))
    {
        for(i = 0; i < gpt_kouyu_talk_size;i++)
        {
            if(MMK_GetCtrlHandleByWin(win_id, ZMT_GPT_FORM_TEXT_1_CTRL_ID+i))
            {
                GUIFORM_DestroyDynaChildCtrl(ZMT_GPT_FORM_CTRL_ID, ZMT_GPT_FORM_TEXT_1_CTRL_ID+i);
            }
        }
        return TRUE;
    }
    return FALSE;
}

LOCAL void ZmtGptKouYuTalk_ShowFormList(MMI_WIN_ID_T win_id)
{
    uint8 i = 0;
    MMI_CTRL_ID_T form_ctrl_id = ZMT_GPT_FORM_CTRL_ID;
    MMI_CTRL_ID_T anim_ctrl_id = ZMT_GPT_FORM_ANIM_CTRL_ID;
    MMI_CTRL_ID_T text_ctrl_id = 0;
    MMI_HANDLE_T ctrl_handle = 0;
    GUI_RECT_T form_rect = {0};
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, GPT_WIN_BG_COLOR, FALSE};
    GUI_BG_T text_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    GUIFORM_CHILD_WIDTH_T child_width = {0};
    GUI_BORDER_T border = {1, MMI_WHITE_COLOR, GUI_BORDER_ROUNDED};
    GUI_COLOR_T font_color = GPT_WIN_BG_COLOR;
    GUI_FONT_T font_size = DP_FONT_18;
    MMI_STRING_T text_string = {0};
    wchar text_str[2048] = {0};
    uint8 line_num = 0;
    uint16 width = 0;

    ZmtGptKouYuTalk_DestoryDynaCtrl(win_id);
    SCI_TRACE_LOW("%s: gpt_kouyu_talk_size = %d", __FUNCTION__, gpt_kouyu_talk_size);
    for(i = 0;i < gpt_kouyu_talk_size;i++)
    {
        GUITEXT_INIT_DATA_T text_init_data = {0};
        GUIFORM_DYNA_CHILD_T text_form_child_ctrl = {0};
        text_form_child_ctrl.child_handle = ZMT_GPT_FORM_TEXT_1_CTRL_ID + i;
        text_form_child_ctrl.init_data_ptr = &text_init_data;
        text_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        GUIFORM_CreatDynaChildCtrl(win_id, form_ctrl_id, &text_form_child_ctrl);
    }
    ctrl_handle = MMK_GetCtrlHandleByWin(win_id, form_ctrl_id);
    for(i = 0;i < gpt_kouyu_talk_size;i++)
    {
        memset(text_str, 0, 2048);
    #ifndef WIN32
        GUI_UTF8ToWstr(text_str, 2048, gpt_kouyu_talk_info[i]->str, strlen(gpt_kouyu_talk_info[i]->str));
    #else
        GUI_GBToWstr(text_str, gpt_kouyu_talk_info[i]->str, strlen(gpt_kouyu_talk_info[i]->str));
    #endif
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        line_num = GUI_CalculateStringLinesByPixelNum(200,text_string.wstr_ptr,text_string.wstr_len,font_size,0,TRUE);    
        if(line_num == 1){
            width = GUI_CalculateStringPiexlNum(text_string.wstr_ptr, text_string.wstr_len, font_size, 0) + 30;
        }else{
            width = zmt_gpt_list_rect.right - ZMT_GPT_LINE_WIDTH;
        }
        //SCI_TRACE_LOW("%s: width = %d", __FUNCTION__, width);
        text_ctrl_id = ZMT_GPT_FORM_TEXT_1_CTRL_ID + i;
        child_width.type = GUIFORM_CHILD_WIDTH_FIXED;
        child_width.add_data = width;
        GUIFORM_SetChildWidth(ctrl_handle, text_ctrl_id, &child_width);
        if(!gpt_kouyu_talk_info[i]->is_user){
            text_bg.color = GPT_TITLE_BG_COLOR;
            font_color = MMI_WHITE_COLOR;
            GUIFORM_SetChildAlign(ctrl_handle, text_ctrl_id, GUIFORM_CHILD_ALIGN_LEFT);
        }else{
            text_bg.color = GPT_SELF_TEXT_BG_COLOR;
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
    GUITEXT_SetBorder(&border, ZMT_GPT_FORM_TEXT_1_CTRL_ID + gpt_kouyu_cur_idx);
    GUIFORM_SetActiveChild(ctrl_handle, ZMT_GPT_FORM_TEXT_1_CTRL_ID + gpt_kouyu_cur_idx);
}

LOCAL void ZmtGptKouYuTalk_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    wchar text_str[GPT_KOUYU_TALK_STR_MAX_LEN] = {0};

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;
    
    GUI_UTF8ToWstr(&text_str, GPT_KOUYU_TALK_STR_MAX_LEN, &gpt_kouyu_info.talk_list[gpt_kouyu_topic_index].talk_chn, strlen(gpt_kouyu_info.talk_list[gpt_kouyu_topic_index].talk_chn));
    text_string.wstr_ptr = text_str;
    text_string.wstr_len = MMIAPICOM_Wstrlen(text_str);
    ZmtGpt_DisplayTitle(win_id, text_string, zmt_gpt_title_rect, DP_FONT_24);

    if(zmt_gpt_talk_status < 3)
    {
        MMI_IMAGE_ID_T img_id = 0;
        GUI_RECT_T img_rect = {0, 3*ZMT_GPT_LINE_HIGHT, 0, 5*ZMT_GPT_LINE_HIGHT};
        GUI_RECT_T txt_rect = {0, 0, MMI_MAINSCREEN_WIDTH, 0};
        uint16 img_size = 64;
        switch(zmt_gpt_talk_status)
        {
            case 0:
                {
                    img_id = IMG_ZMT_GPT_LOAD;
                    MMIRES_GetText(ZMT_CHAT_GPT_LOADING, win_id, &text_string);
                }
                break;
            case 1:
                {
                    img_id = IMG_ZMT_GPT_REQUEST_FAIL;
                    MMIRES_GetText(ZMT_CHAT_GPT_LOADING_FAIL, win_id, &text_string);
                }
                break;
            case 2:
                {
                    img_id = IMG_ZMT_GPT_NET_ERROR;
                    MMIRES_GetText(ZMT_CHAT_GPT_REQUSET_FAIL, win_id, &text_string);
                }
                break;
        }
        img_rect.bottom = img_rect.top + img_size;
        img_rect.left = (MMI_MAINSCREEN_WIDTH - img_size)/2;
        img_rect.right = img_rect.left + img_size;
        GUIRES_DisplayImg(PNULL, &img_rect, PNULL, win_id, img_id, &lcd_dev_info);
        
        txt_rect.top = img_rect.bottom + 5;
        txt_rect.bottom = txt_rect.top + 25;
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &txt_rect,
            &txt_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
        return;
    }
    if(gpt_kouyu_talk_size > 0){
        ZmtGptKouYuTalk_ShowFormList(win_id);
    }
    ZmtGptKouYuTalk_DispalyRecord(win_id, gpt_kouyu_record_type);
}

LOCAL void ZmtGptKouYuTalk_CTL_PENOK(MMI_WIN_ID_T win_id, DPARAM param)
{
    int8 cur_idx = 0;
    MMI_CTRL_ID_T ctrl_id = ((MMI_NOTIFY_T *)param)->src_id;
    cur_idx = ctrl_id - ZMT_GPT_FORM_TEXT_1_CTRL_ID;
    if(cur_idx < 0){
        cur_idx = gpt_kouyu_cur_idx;
    }
    SCI_TRACE_LOW("%s: cur_idx = %d", __FUNCTION__, cur_idx);
    if(gpt_kouyu_load_text){
        SCI_TRACE_LOW("%s: load text xxing", __FUNCTION__);
        return;
    }
    if(gpt_kouyu_cur_idx == cur_idx){
        if(gpt_kouyu_player_handle){
            return;
        }
        if( gpt_kouyu_talk_info[cur_idx] != NULL && gpt_kouyu_talk_info[cur_idx]->str != NULL)
        {
            ZmtGpt_SendString(NULL, gpt_kouyu_talk_info[cur_idx]->str);
        }
    }else{
        gpt_kouyu_cur_idx = cur_idx;
        if(MMK_IsFocusWin(win_id)){
            MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
        }
    }
}

LOCAL void ZmtGptKouYuTalk_KEYDOWN_UP_DOWN(MMI_WIN_ID_T win_id, BOOLEAN is_up)
{
    if(is_up){
        if(gpt_kouyu_cur_idx > 0){
            gpt_kouyu_cur_idx--;
            if(MMK_IsFocusWin(win_id)){
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
        }
    }else{
        if(gpt_kouyu_cur_idx + 1< gpt_kouyu_talk_size){
            gpt_kouyu_cur_idx++;
            if(MMK_IsFocusWin(win_id)){
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
        }
    }
}

LOCAL void ZmtGptKouYuTalk_InitFormContorl(MMI_WIN_ID_T win_id)
{
    GUI_RECT_T form_rect = zmt_gpt_list_rect;
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, GPT_WIN_BG_COLOR, FALSE};
    MMI_CTRL_ID_T form_ctrl_id = ZMT_GPT_FORM_CTRL_ID;
    
    GUIFORM_CreatDynaCtrl(win_id, form_ctrl_id, GUIFORM_LAYOUT_ORDER);
    GUIFORM_SetBg(form_ctrl_id, &form_bg);
    GUIFORM_SetRect(form_ctrl_id, &form_rect);
    GUIFORM_SetDisplayScrollBar(form_ctrl_id, FALSE);
    GUIFORM_PermitChildBorder(form_ctrl_id, FALSE);
    GUIFORM_PermitChildBg(form_ctrl_id, FALSE);
    GUIFORM_PermitChildFont(form_ctrl_id, FALSE);
    GUIFORM_PermitChildFontColor(form_ctrl_id, FALSE);
    MMK_SetActiveCtrl(form_ctrl_id, FALSE);
}

LOCAL void ZmtGptKouYuTalk_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    gpt_kouyu_cur_idx = 0;
    ZmtGptKouYuTalk_InitFormContorl(win_id);
    
    gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
#if ZMT_GPT_USE_SELF_API != 0
    zmt_gpt_talk_status = 3;
#else
    ZmtGpt_SendTxt(89, "Hi", gpt_kouyu_info.talk_list[gpt_kouyu_topic_index].talk, gpt_kouyu_info.field, 0);
#endif

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
        UILAYER_CreateLayer(&create_info, &gpt_kouyu_record_layer);
    }
}

LOCAL void ZmtGptKouYuTalk_CLOSE_WINDOW(void)
{
    ZmtGptKouYuTalk_ReleaseTalkInfo();
    gpt_kouyu_talk_size = 0;
    ZmtGptKouYuTalk_StopAmrData();
    if (PNULL != gpt_kouyu_record_handle)
    {
        MMIRECORDSRV_StopRecord(gpt_kouyu_record_handle);
        MMIRECORDSRV_FreeRecordHandle(gpt_kouyu_record_handle);
        gpt_kouyu_record_handle = 0;
    }
    if(gpt_kouyu_player_handle)
    {
        MMISRVAUD_Stop(gpt_kouyu_player_handle);
        MMISRVMGR_Free(gpt_kouyu_player_handle);
        gpt_kouyu_player_handle = NULL;
    }
    if(gpt_kouyu_record_timer_id != 0)
    {
        MMK_StopTimer(gpt_kouyu_record_timer_id);
        gpt_kouyu_record_timer_id = 0;
    }
    if(gpt_kouyu_record_identify_timer_id){
        MMK_StopTimer(gpt_kouyu_record_identify_timer_id);
        gpt_kouyu_record_identify_timer_id = 0;
    }
    zmt_gpt_talk_status = 0;
    gpt_kouyu_record_type = GPT_RECORD_TYPE_NONE;
    gpt_kouyu_record_times = 0;
    gpt_kouyu_load_text = FALSE;
    if(gpt_kouyu_record_text){
        SCI_FREE(gpt_kouyu_record_text);
        gpt_kouyu_record_text = NULL;
    }
    MMIZDT_HTTP_Close();
}

LOCAL MMI_RESULT_E HandleZmtGptKouYuTalkWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {              
                ZmtGptKouYuTalk_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {             
                ZmtGptKouYuTalk_FULL_PAINT(win_id);               
            }
            break;
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
            {
                ZmtGptKouYuTalk_CTL_PENOK(win_id, param);
            }
            break;
        case MSG_KEYDOWN_UP:
            {
                ZmtGptKouYuTalk_KEYDOWN_UP_DOWN(win_id, TRUE);
            }
            break;
        case MSG_KEYDOWN_DOWN:
            {
                ZmtGptKouYuTalk_KEYDOWN_UP_DOWN(win_id, FALSE);
            }
            break;
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_TP_PRESS_LONG:
            {
                GUI_POINT_T point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(GUI_PointIsInRect(point, zmt_gpt_record_rect) && 
                    gpt_kouyu_record_type == GPT_RECORD_TYPE_NONE)
                {                    
                    ZmtGptKouYuTalk_StartRecord(win_id);
                }
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(GUI_PointIsInRect(point, zmt_gpt_record_rect) && 
                    gpt_kouyu_record_type == GPT_RECORD_TYPE_RECORDING)
                {
                    ZmtGptKouYuTalk_StopRecord(win_id, TRUE);
                }
                else if (!GUI_PointIsInRect(point, zmt_gpt_record_rect) && 
                    gpt_kouyu_record_type == GPT_RECORD_TYPE_RECORDING)
                {
                    ZmtGptKouYuTalk_StopRecord(win_id, FALSE);
                }
                else if(GUI_PointIsInRect(point, zmt_gpt_record_left_rect) && 
                    gpt_kouyu_record_type == GPT_RECORD_TYPE_SUCCESS &&
                    (gpt_kouyu_record_text != NULL && strlen(gpt_kouyu_record_text) > 2))
                {
                    ZmtGptKouYuTalk_LeftIndentifyClick(win_id);
                }
                else if(GUI_PointIsInRect(point, zmt_gpt_record_right_rect) && 
                    gpt_kouyu_record_type == GPT_RECORD_TYPE_SUCCESS)
                {
                    ZmtGptKouYuTalk_RightIndentifyClick(win_id);
                }
            }
            break;
        case MSG_TIMER:
            {
                gpt_kouyu_record_times += 200;
                ZmtGptKouYuTalk_DispalyRecord(win_id, 1);
                if(gpt_kouyu_record_times >= 10 * 1000){
                    ZmtGptKouYuTalk_StopRecord(win_id, TRUE);
                }
            }
            break;            
        case MSG_CLOSE_WINDOW:
            {
                ZmtGptKouYuTalk_CLOSE_WINDOW();
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_ZMT_GPT_KOUYU_TALK_WIN_TAB) = {
    WIN_ID(ZMT_GPT_KOUYU_TALK_WIN_ID),
    WIN_FUNC((uint32)HandleZmtGptKouYuTalkWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZMT_CreateZmtGptKouYuWin(void)
{
    if(MMK_IsOpenWin(ZMT_GPT_KOUYU_TALK_WIN_ID))
    {
        MMK_CloseWin(ZMT_GPT_KOUYU_TALK_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_ZMT_GPT_KOUYU_TALK_WIN_TAB, PNULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
LOCAL void ZmtGptTopic_SelfListResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        cJSON * root = cJSON_Parse(pRcv);
        cJSON * code = cJSON_GetObjectItem(root, "code");
        cJSON * data = cJSON_GetObjectItem(root, "data");
        if(code != NULL && code->type != cJSON_NULL && code->valueint == 200)
        {
            gpt_kouyu_info.size = cJSON_GetArraySize(data);
            memset(&gpt_kouyu_info.field, 0, GPT_KOUYU_TOPIC_MAX_SIZE);
            strcpy(gpt_kouyu_info.field, "english");
            for(i = 0;i < cJSON_GetArraySize(data);i++)
            {
                cJSON * data_item = cJSON_GetArrayItem(data, i);
                cJSON * topic = cJSON_GetObjectItem(data_item, "topic");
                memset(gpt_kouyu_info.talk_list[i].talk, 0, GPT_KOUYU_TALK_STR_MAX_LEN);
                strcpy(gpt_kouyu_info.talk_list[i].talk, topic->valuestring);
                strcpy(gpt_kouyu_info.talk_list[i].talk_chn, topic->valuestring);
            }
            zmt_gpt_topic_status = 3;
        }
        else
        {
            SCI_TRACE_LOW("%s: code->valueint = %d", __FUNCTION__, code->valueint);
            zmt_gpt_topic_status = 1;
        }
        cJSON_Delete(root);
    }
    else
    {
        zmt_gpt_topic_status = 2;
    }
    if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TOPIC_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_KOUYU_TOPIC_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
#if ZMT_GPT_USE_FOR_TEST != 0
    if(pRcv != NULL){
        SCI_FREE(pRcv);
    }
#endif
}

LOCAL void ZmtGptTopic_ListResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
#if ZMT_GPT_USE_FOR_TEST != 0
    uint8 * buf = NULL;
    uint32 len = 0;
    {
        if(zmt_file_exist(ZMT_GPT_TOPIC_LIST_PATH)){
            buf = zmt_file_data_read(ZMT_GPT_TOPIC_LIST_PATH, &len);
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
        if(data != NULL && data->type != cJSON_NULL){
            cJSON * list = cJSON_GetObjectItem(data, "list");
            cJSON * kouyu = cJSON_GetArrayItem(list, 1);
            cJSON * sys_params = cJSON_GetObjectItem(kouyu, "sys_params");
            cJSON * talk_data = cJSON_GetObjectItem(sys_params, "data");
            cJSON * params = cJSON_GetObjectItem(sys_params, "params");
            cJSON * item = cJSON_GetArrayItem(params, 0);
            cJSON * field = cJSON_GetObjectItem(item, "field");
            memset(&gpt_kouyu_info.field, 0, GPT_KOUYU_TOPIC_MAX_SIZE);
            strcpy(gpt_kouyu_info.field, field->valuestring);
            //SCI_TRACE_LOW("%s: 01gpt_kouyu_info.field = %s", __FUNCTION__, gpt_kouyu_info.field);

            {
                uint8 * talk[GPT_KOUYU_TOPIC_MAX_SIZE];
                char * p = talk_data->valuestring;
                p = strtok(p, "\n");
                while((p = strtok(NULL, "\n")) != NULL && gpt_kouyu_info.size < GPT_KOUYU_TOPIC_MAX_SIZE)
                {
                    talk[gpt_kouyu_info.size] = SCI_ALLOC_APPZ(strlen(p)+1);
                    memset(talk[gpt_kouyu_info.size], 0, strlen(p)+1);
                    strcpy(talk[gpt_kouyu_info.size], p);
                    gpt_kouyu_info.size++;
                }
                SCI_TRACE_LOW("%s: gpt_kouyu_info.size = %d", __FUNCTION__, gpt_kouyu_info.size);
                {
                    uint8 i = 0;
                    char * str = NULL;
                    char string[GPT_KOUYU_TALK_STR_MAX_LEN] = {0};
                    for(i = 0;i < gpt_kouyu_info.size && i < GPT_KOUYU_TOPIC_MAX_SIZE;i++)
                    {
                        if(talk[i]){
                            memset(string, 0, 100);
                            strcpy(string, talk[i]);
                            str = strtok(talk[i], ",");
                            //SCI_TRACE_LOW("%s: str = %s", __FUNCTION__, str);
                            if(str){
                                memset(gpt_kouyu_info.talk_list[i].talk, 0, GPT_KOUYU_TALK_STR_MAX_LEN);
                                strcpy(gpt_kouyu_info.talk_list[i].talk, str);
                                strcpy(gpt_kouyu_info.talk_list[i].talk_chn, string+strlen(str)+1);
                            }
                            SCI_FREE(talk[i]);
                        }
                    }
                }
            }          
            zmt_gpt_topic_status = 3;
            //SCI_TRACE_LOW("%s: 02gpt_kouyu_info.field = %s", __FUNCTION__, gpt_kouyu_info.field);
        }
        else
        {
            zmt_gpt_topic_status = 1;
        }
        cJSON_Delete(root);
    }
    else
    {
        zmt_gpt_topic_status = 2;
    }
    if(MMK_IsFocusWin(ZMT_GPT_KOUYU_TOPIC_WIN_ID))
    {
        MMK_SendMsg(ZMT_GPT_KOUYU_TOPIC_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
#if ZMT_GPT_USE_FOR_TEST != 0
    if(pRcv != NULL){
        SCI_FREE(pRcv);
    }
#endif
}

LOCAL void ZmtGptTopic_RequsetTopicList(void)
{
    char url[1024] = {0};
    uint8 * buf = NULL;
    uint32 len = 0;
    memset(&gpt_kouyu_info, 0, sizeof(gpt_kouyu_info_t));
#ifdef WIN32
    if(zmt_file_exist(ZMT_GPT_TOPIC_LIST_PATH)){
        buf = zmt_file_data_read(ZMT_GPT_TOPIC_LIST_PATH, &len);
    }
#endif
#if ZMT_GPT_USE_SELF_API != 0
    #ifdef WIN32
    ZmtGptTopic_SelfListResultCb(1, buf, len, 0);
    #else
    sprintf(url, "%s%s?type=english", GPT_HTTP_SELF_API_BASE_PATH, GPT_HTTP_SELF_API_KOUYU_TOPIC);
    MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, ZmtGptTopic_SelfListResultCb);
    #endif
#else
    #ifdef WIN32
    ZmtGptTopic_ListResultCb(1, buf, len, 0);
    #else
    ZMT_makeGptTokenUrl(0, &url, 1, 10);
    MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, ZmtGptTopic_ListResultCb);
    #endif
#endif
}

LOCAL void ZmtGptKouYuTopic_DisplayList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 i = 0;
    uint8 num = 0;
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_string = {0};
    wchar text_str[100] = {0};

    ZmtGpt_InitListbox(win_id, ctrl_id, zmt_gpt_list_rect, gpt_kouyu_info.size);
    for(i = 0;i < gpt_kouyu_info.size; i++)
    {
        memset(text_str, 0, 100);
        item_t.item_style = GUIITEM_SYTLE_ZMT_HANZI_TEXT_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        GUI_UTF8ToWstr(text_str, 100, gpt_kouyu_info.talk_list[i].talk_chn, strlen(gpt_kouyu_info.talk_list[i].talk_chn));
        text_string.wstr_ptr = text_str;
        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
        item_data.item_content[0].item_data.text_buffer = text_string;

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
    GUILIST_SetTextFont(ctrl_id, DP_FONT_24, MMI_WHITE_COLOR);
    GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, TRUE);
    GUILIST_SetCurItemIndex(ctrl_id, gpt_kouyu_topic_index);
}

LOCAL void ZmtGptKouYuTopic_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};

    MMIRES_GetText(ZMT_CHAT_GPT_TOPIC, win_id, &text_string);
    ZmtGpt_DisplayTitle(win_id, text_string, zmt_gpt_title_rect, DP_FONT_24);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = DP_FONT_24;
    text_style.font_color = GPT_WIN_BG_COLOR;
    if(zmt_gpt_topic_status < 3)
    {
        MMI_IMAGE_ID_T img_id = 0;
        GUI_RECT_T img_rect = {0, 3*ZMT_GPT_LINE_HIGHT, 0, 5*ZMT_GPT_LINE_HIGHT};
        GUI_RECT_T txt_rect = {0, 0, MMI_MAINSCREEN_WIDTH, 0};
        uint16 img_size = 64;
        switch(zmt_gpt_topic_status)
       {
            case 0:
                {
                    img_id = IMG_ZMT_GPT_LOAD;
                    MMIRES_GetText(ZMT_CHAT_GPT_LOADING, win_id, &text_string);
                }
                break;
            case 1:
                {
                    img_id = IMG_ZMT_GPT_REQUEST_FAIL;
                    MMIRES_GetText(ZMT_CHAT_GPT_LOADING_FAIL, win_id, &text_string);
                }
                break;
            case 2:
                {
                    img_id = IMG_ZMT_GPT_NET_ERROR;
                    MMIRES_GetText(ZMT_CHAT_GPT_REQUSET_FAIL, win_id, &text_string);
                }
                break;
        }
        img_rect.left = (MMI_MAINSCREEN_WIDTH - img_size)/2;
        img_rect.right = img_rect.left + img_size;
        GUIRES_DisplayImg(PNULL, &img_rect, PNULL, win_id, img_id, &lcd_dev_info);
        
        txt_rect.top = img_rect.bottom + 5;
        txt_rect.bottom = txt_rect.top + 25;
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &txt_rect,
            &txt_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
        return;
    }
    
    ZmtGptKouYuTopic_DisplayList(win_id, ZMT_GPT_KOUYU_TOPIC_LIST_CTRL_ID);
}

LOCAL void ZmtGptKouYuTopic_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUILIST_GetCurItemIndex(ZMT_GPT_KOUYU_TOPIC_LIST_CTRL_ID);
    gpt_kouyu_topic_index = cur_idx;
    SCI_TRACE_LOW("%s: talk_list[%d].talk = %s", __FUNCTION__, cur_idx, gpt_kouyu_info.talk_list[cur_idx].talk);
    MMIZMT_CreateZmtGptKouYuWin();
}

LOCAL MMI_RESULT_E HandleZmtGptKouYuTopicWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ZmtGptTopic_RequsetTopicList();
            }
            break;
        case MSG_FULL_PAINT:
            {             
                ZmtGptKouYuTopic_FULL_PAINT(win_id);               
            }
            break;
        case MSG_CTL_PENOK:
            {
                ZmtGptKouYuTopic_CTL_PENOK(win_id);
            }
            break;
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                memset(&gpt_kouyu_info, 0, sizeof(gpt_kouyu_info_t));
                gpt_kouyu_topic_index = 0;
                zmt_gpt_topic_status = 0;
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_ZMT_GPT_KOUYU_TOPIC_WIN_TAB) = {
    WIN_ID(ZMT_GPT_KOUYU_TOPIC_WIN_ID),
    WIN_FUNC((uint32)HandleZmtGptKouYuTopicWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZMT_CreateZmtGptKouYuTopicWin(void)
{
    if(MMK_IsOpenWin(ZMT_GPT_KOUYU_TOPIC_WIN_ID))
    {
        MMK_CloseWin(ZMT_GPT_KOUYU_TOPIC_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_ZMT_GPT_KOUYU_TOPIC_WIN_TAB, PNULL);
}



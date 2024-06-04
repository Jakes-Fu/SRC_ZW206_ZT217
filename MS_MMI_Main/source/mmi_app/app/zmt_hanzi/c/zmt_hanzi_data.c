/*************************************************************************
 ** File Name:      mmiacc_id.c                                          *
 ** Author:         haiyang.hu                                           *
 ** Date:           2006/09/18                                           *
 ** Copyright:      2006 Spreadtrum, Incorporated. All Rights Reserved.  *
 ** Description:     This file defines the function about nv             *

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "std_header.h"
#include "sci_types.h"
#include "mmi_module.h"
#include "cjson.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "zmt_hanzi_main.h"
#include "zmt_hanzi_id.h"
#include "zmt_main_file.h"
#endif


HANZI_CHAPTER_INFO_T * hanzi_chapter_info[20];
int16 hanzi_chapter_count = 0;
int16 hanzi_chapter_children_count[20] = {0};
HANZI_BOOK_HANZI_INFO * hanzi_detail_info[100];
int16 hanzi_detail_count = 0;
int16 hanzi_detail_cur_idx = 0;
extern HANZI_BOOK_INFO_T hanzi_book_info;

ZMT_HANZI_BOOK_NAME hanzi_book_name_str[HANZI_BOOK_TOTAL] = {
    {"一年级上册人教部编版", 0},{"一年级上册人教版", 1},{"一年级下册人教部编版", 2},{"一年级下册人教版", 3},
    {"二年级上册人教部编版", 4},{"二年级上册人教版", 5},{"二年级下册人教部编版", 6},{"二年级下册人教版", 7},
    {"三年级上册人教部编版", 8},{"三年级上册人教版", 9},{"三年级上册人教部编版", 10},{"三年级下册人教版", 11},
    {"四年级上册人教部编版", 12},{"四年级上册人教版", 13},{"四年级上册人教部编版", 14},{"四年级下册人教版", 15},
    {"五年级上册人教部编版", 16},{"五年级上册人教版", 17},{"五年级上册人教部编版", 18},{"五年级下册人教版", 19},
    {"六年级上册人教部编版", 20},{"六年级上册人教版", 21},{"六年级上册人教部编版", 22},{"六年级下册人教版", 23},
    {"七年级上册人教部编版", 24},{"七年级上册人教版", 25},{"七年级上册人教部编版", 26},{"七年级下册人教版", 27},
    {"八年级上册人教部编版", 28},{"八年级上册人教版", 29},{"八年级上册人教部编版", 30},{"八年级下册人教版", 31},
    {"九年级上册人教部编版", 32},{"九年级上册人教版", 33},{"九年级下册人教部编版", 34},{"九年级下册人教版", 35}
};

PUBLIC void Hanzi_ReleaseChapterInfo(void)
{
    uint8 i,j = 0;
    for(i = 0;i < 20;i++)
    {
        //SCI_TRACE_LOW("%s: i = %d", __FUNCTION__, i);
        if(hanzi_chapter_info[i] != NULL){
            if(hanzi_chapter_info[i]->chapter_name != NULL){
                SCI_FREE(hanzi_chapter_info[i]->chapter_name);
                hanzi_chapter_info[i]->chapter_name = NULL;
            }
            for(j = 0;j < 30;j++){
                //SCI_TRACE_LOW("%s: j = %d", __FUNCTION__, j);
                if(hanzi_chapter_info[i]->children[j].chapter_name != NULL){
                    SCI_FREE(hanzi_chapter_info[i]->children[j].chapter_name);
                    hanzi_chapter_info[i]->children[j].chapter_name = NULL;
                }
                memset(&hanzi_chapter_info[i]->children[j], 0, sizeof(HANZI_CHAPTER_CHILDREN_INFO_T));
            }
            SCI_FREE(hanzi_chapter_info[i]);
            hanzi_chapter_info[i] = NULL;
        }
    }
}

PUBLIC void Hanzi_ParseChapterInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        uint8 m = 0;
        uint16 size = 0;
        cJSON *root = cJSON_Parse(pRcv);
        cJSON *F_list = cJSON_GetObjectItem(root, "F_list");
        if(F_list != NULL && F_list->type != cJSON_NULL)
        {
            for(i = 0;i < cJSON_GetArraySize(F_list) && i < 20;i++)
            {              
                cJSON *list_item =  cJSON_GetArrayItem(F_list, i);
                cJSON *F_chapter_children = cJSON_GetObjectItem(list_item, "F_chapter_children");
                if(hanzi_chapter_info[i] != NULL){
                    SCI_FREE(hanzi_chapter_info[i]);
                    hanzi_chapter_info[i] = NULL;
                }
                hanzi_chapter_info[i] = (HANZI_CHAPTER_INFO_T *)SCI_ALLOC_APP(sizeof(HANZI_CHAPTER_INFO_T));
                memset(hanzi_chapter_info[i], 0, sizeof(HANZI_CHAPTER_INFO_T));
                if(F_chapter_children != NULL && F_chapter_children->type != cJSON_NULL && cJSON_GetArraySize(F_chapter_children) > 0)
                {
                    cJSON *F_chapter_name = cJSON_GetObjectItem(list_item, "F_chapter_name");
                    if(hanzi_chapter_info[i]->chapter_name != NULL){
                        SCI_FREE(hanzi_chapter_info[i]->chapter_name);
                        hanzi_chapter_info[i]->chapter_name = NULL;
                    }
                    size = strlen(F_chapter_name->valuestring);
                    hanzi_chapter_info[i]->chapter_name = SCI_ALLOC_APP(size+1);
                    memset(hanzi_chapter_info[i]->chapter_name, 0, size+1);
                    strncpy(hanzi_chapter_info[i]->chapter_name, F_chapter_name->valuestring, size);
                    SCI_TRACE_LOW("%s: info[%d].chapter_name = %s", __FUNCTION__, i, hanzi_chapter_info[i]->chapter_name);
                    for(m = 0;m < cJSON_GetArraySize(F_chapter_children) && m < 20;m++)
                    {
                        cJSON *children_item =  cJSON_GetArrayItem(F_chapter_children, m);
                        cJSON *F_chapter_id = cJSON_GetObjectItem(children_item, "F_chapter_id");
                        cJSON *chapter_name = cJSON_GetObjectItem(children_item, "F_chapter_name");

                        memset(&hanzi_chapter_info[i]->children[m], 0, sizeof(HANZI_CHAPTER_CHILDREN_INFO_T));
                        if(hanzi_chapter_info[i]->children[m].chapter_name != NULL){
                            SCI_FREE(hanzi_chapter_info[i]->children[m].chapter_name);
                            hanzi_chapter_info[i]->children[m].chapter_name = NULL;
                        }
                        size = strlen(chapter_name->valuestring);
                        hanzi_chapter_info[i]->children[m].chapter_name = SCI_ALLOC_APP(size+1);
                        memset(hanzi_chapter_info[i]->children[m].chapter_name, 0, size+1);
                        strncpy(hanzi_chapter_info[i]->children[m].chapter_name, chapter_name->valuestring, size);                       
                        SCI_TRACE_LOW("%s: info[%d].children[%d].chapter_name = %s", __FUNCTION__, i, m, hanzi_chapter_info[i]->children[m].chapter_name);
                        hanzi_chapter_info[i]->children[m].chapter_id= F_chapter_id->valueint;
                    }
                    hanzi_chapter_children_count[i] = m;
                }       
            }
            hanzi_chapter_count = i;
        }
        else
        {
            hanzi_chapter_count = -2;
        }
        cJSON_Delete(root);    
    }
    else
    {
        hanzi_chapter_count = -1;
    }
#if HANZI_CARD_USE_OFF_DATA
    if(pRcv != PNULL)
    {
    	SCI_FREE(pRcv);
    	pRcv = PNULL;
    }
#endif    
    MMK_SendMsg(MMI_HANZI_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
}

PUBLIC void HanziChapter_requestChapterInfo(uint16 book_id)
{
    char file_path[80] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    char *temp_file_path = HANZI_CARD_CHAPTER_PATH;
    strcpy(file_path,temp_file_path);
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        SCI_TRACE_LOW("%s: file_len = %d", __FUNCTION__, file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            Hanzi_ParseChapterInfo(1, data_buf, file_len,0);
        }
    }
    else
    {
        char url[100] = {0};
        sprintf(url, "v1/card/book/hanzi/chapters?F_book_id=%d", book_id);
        SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
        MMIZDT_HTTP_AppSend(TRUE, HANZI_BOOK_HEADER_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, Hanzi_ParseChapterInfo);
    }
}

PUBLIC void HanziDetail_InitDetailInfo(void)
{
	uint16 i = 0;
	for(i = 0;i < 100;i++)
	{
		hanzi_detail_info[i] = NULL;
	}
}

PUBLIC void HanziDetail_ReleaseDetailInfo(void)
{
    uint16 i = 0;
    for(i = 0;i < 100;i++)
    {
        if(hanzi_detail_info[i] != NULL){
            if(hanzi_detail_info[i]->word != NULL){
                SCI_FREE(hanzi_detail_info[i]->word);
                hanzi_detail_info[i]->word = NULL;
            }
            if(hanzi_detail_info[i]->pingy != NULL){
                SCI_FREE(hanzi_detail_info[i]->pingy);
                hanzi_detail_info[i]->pingy = NULL;
            }
            if(hanzi_detail_info[i]->audio_uri != NULL){
                SCI_FREE(hanzi_detail_info[i]->audio_uri);
                hanzi_detail_info[i]->audio_uri = NULL;
            }
            if(hanzi_detail_info[i]->audio_data != NULL){
                SCI_FREE(hanzi_detail_info[i]->audio_data);
                hanzi_detail_info[i]->audio_data = NULL;
            }
            if(hanzi_detail_info[i]->remark != NULL){
                SCI_FREE(hanzi_detail_info[i]->remark);
                hanzi_detail_info[i]->remark = NULL;
            }
            SCI_FREE(hanzi_detail_info[i]);
            hanzi_detail_info[i] = NULL;
        }
    }
}

PUBLIC void HanziDetail_ReleaseDetailItemInfo(uint16 idx)
{
    if(hanzi_detail_info[idx]->word != NULL){
        SCI_FREE(hanzi_detail_info[idx]->word);
        hanzi_detail_info[idx]->word = NULL;
    }
    if(hanzi_detail_info[idx]->pingy != NULL){
        SCI_FREE(hanzi_detail_info[idx]->pingy);
        hanzi_detail_info[idx]->pingy = NULL;
    }
    if(hanzi_detail_info[idx]->audio_uri != NULL){
        SCI_FREE(hanzi_detail_info[idx]->audio_uri);
        hanzi_detail_info[idx]->audio_uri = NULL;
    }
    if(hanzi_detail_info[idx]->audio_data != NULL){
        SCI_FREE(hanzi_detail_info[idx]->audio_data);
        hanzi_detail_info[idx]->audio_data = NULL;
    }
    if(hanzi_detail_info[idx]->remark != NULL){
        SCI_FREE(hanzi_detail_info[idx]->remark);
        hanzi_detail_info[idx]->remark = NULL;
    }   
}

PUBLIC void Hanzi_DetailMakeTermRemark(char* remark, char * explain, char * example)
{
	strcat(remark, "释义:");
	strcat(remark, explain);
	strcat(remark, "\n");
	strcat(remark, "例句:");
	strcat(remark, example);
}

PUBLIC void Hanzi_ParseDetailInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint16 i = 0;
        uint16 j = 0;
        uint32 size = 0;
        cJSON *root = cJSON_Parse(pRcv);
        cJSON *F_list = cJSON_GetObjectItem(root, "F_list");
        if(F_list != NULL && F_list->type != cJSON_NULL)
        {
            HanziDetail_InitDetailInfo();
            for(i = 0;i < cJSON_GetArraySize(F_list) && i < 100;i++)
            {
                cJSON *list_item =  cJSON_GetArrayItem(F_list, i);
                cJSON *F_word = cJSON_GetObjectItem(list_item, "F_word");
                cJSON *F_pingy = cJSON_GetObjectItem(list_item, "F_pingy");
                cJSON *F_audio_uri = cJSON_GetObjectItem(list_item, "F_audio_uri");

                if(hanzi_detail_info[j] == NULL){
                    hanzi_detail_info[j] = (HANZI_BOOK_HANZI_INFO *)SCI_ALLOC_APP(sizeof(HANZI_BOOK_HANZI_INFO));
                    hanzi_detail_info[j]->word = NULL;
                    hanzi_detail_info[j]->pingy = NULL;
                    hanzi_detail_info[j]->audio_uri = NULL;
                    hanzi_detail_info[j]->remark = NULL;
                }
                memset(hanzi_detail_info[j], 0, sizeof(HANZI_BOOK_HANZI_INFO));

                HanziDetail_ReleaseDetailItemInfo(j);

                if(F_word != NULL && F_word->type != cJSON_NULL){
                    size = strlen(F_word->valuestring);
                    if(size > 0){
                        hanzi_detail_info[j]->word = (char *)SCI_ALLOC_APP(size + 1);
                        memset(hanzi_detail_info[j]->word, 0, size + 1);
                        strncpy(hanzi_detail_info[j]->word, F_word->valuestring, size);
                    }
                }

                if(F_pingy != NULL && F_pingy->type != cJSON_NULL){
                    size = strlen(F_pingy->valuestring);
                    if(size > 0){
                        hanzi_detail_info[j]->pingy = (char *)SCI_ALLOC_APP(size + 1);
                        memset(hanzi_detail_info[j]->pingy, 0, size + 1);
                        strncpy(hanzi_detail_info[j]->pingy, F_pingy->valuestring, size);
                    }
                }

                if(F_audio_uri != NULL && F_audio_uri->type != cJSON_NULL){
                    size = strlen(F_audio_uri->valuestring);
                    if(size > 0)
                    {
                        hanzi_detail_info[j]->audio_uri = (char *)SCI_ALLOC_APP(size + 1);
                        memset(hanzi_detail_info[j]->audio_uri, 0, size + 1);
                        strncpy(hanzi_detail_info[j]->audio_uri, F_audio_uri->valuestring, size);
                        hanzi_detail_info[j]->audio_len = 0;
                    }
                    else
                    {
                        hanzi_detail_info[j]->audio_len = -1;
                    }                    
                }

                if(F_word != NULL && F_word->type != cJSON_NULL){
                    if(strlen(F_word->valuestring) > 4){
                        cJSON *F_explain = cJSON_GetObjectItem(list_item, "F_explain");
                        cJSON *F_example = cJSON_GetObjectItem(list_item, "F_example");
                        if(F_explain != NULL && F_explain->type != cJSON_NULL &&
                            F_example != NULL && F_example->type != cJSON_NULL){
                            if(strlen(F_explain->valuestring) > 0 && strlen(F_example->valuestring) > 0){
                                size = strlen(F_explain->valuestring) + strlen(F_example->valuestring) + 30;
                                hanzi_detail_info[j]->remark = (char *)SCI_ALLOC_APP(size);
                                memset(hanzi_detail_info[j]->remark, 0, size);
                                Hanzi_DetailMakeTermRemark(hanzi_detail_info[j]->remark, F_explain->valuestring, F_example->valuestring);
                            }
                        }
                        else
                        {
                            cJSON *F_remark = cJSON_GetObjectItem(list_item, "F_remark");
                            if(F_remark != NULL && F_remark->type != cJSON_NULL){
                                size = strlen(F_remark->valuestring);
                                if(size > 0){
                                    hanzi_detail_info[j]->remark = (char *)SCI_ALLOC_APP(size + 1);
                                    memset(hanzi_detail_info[j]->remark, 0, size + 1);
                                    strncpy(hanzi_detail_info[j]->remark, F_remark->valuestring, size);
                                }
                            }
                        }
                    }else{
                        cJSON *F_remark = cJSON_GetObjectItem(list_item, "F_remark");
                        if(F_remark != NULL && F_remark->type != cJSON_NULL){
                            size = strlen(F_remark->valuestring);
                            if(size > 0){
                                hanzi_detail_info[j]->remark = (char *)SCI_ALLOC_APP(size + 1);
                                memset(hanzi_detail_info[j]->remark, 0, size + 1);
                                strncpy(hanzi_detail_info[j]->remark, F_remark->valuestring, size);
                            }
                        }
                    }
                    j++;
                }                
            }    
            hanzi_detail_count = j;
        }
        else
        {
            hanzi_detail_count = -2;
        }
        cJSON_Delete(root);
    }
    else
    {
        hanzi_detail_count = -1;
    }
#if HANZI_CARD_USE_OFF_DATA
    if(pRcv != PNULL)
    {
    	SCI_FREE(pRcv);
    	pRcv = PNULL;
    }
#endif
    HanziDetail_TipTimeout(0, 0);
}

PUBLIC void HanziDetail_requestDetailInfo(uint16 section_id)
{
//这里需要根据传过来的section_id获取单元汉字信息，这里先用固定的HANZI_CARD_CHAPTER_WORD_PATH
    char file_path[80] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    char *temp_file_path = HANZI_CARD_CHAPTER_WORD_PATH;
    strcpy(file_path,temp_file_path);
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        SCI_TRACE_LOW("%s: file_len = %d", __FUNCTION__, file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            Hanzi_ParseDetailInfo(1, data_buf, file_len,0);
        }
    }
    else
    {
        char url[130] = {0};
        sprintf(url, "v1/card/chapters/hanzi?F_cardId=%s&&F_section_id=%d", HANZI_BASE_DEVICE_IMEI, section_id);
        SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
        MMIZDT_HTTP_AppSend(TRUE, HANZI_BOOK_HEADER_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, Hanzi_ParseDetailInfo);
    }
}

PUBLIC void HanziDetail_RequestNewWord(void)
{
    char file_path[80] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    char *temp_file_path = HANZI_CARD_NEW_WORD_PATH;
    strcpy(file_path,temp_file_path);
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        SCI_TRACE_LOW("%s: file_len = %d", __FUNCTION__, file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            Hanzi_ParseDetailInfo(1, data_buf, file_len,0);
        }
    }
    else
    {
        char url[200] = {0};
        Hanzi_ParseDetailInfo(0, data_buf, file_len,0);
        //sprintf(url, "v1/card/chapters/hanzi?F_cardId=%s&&F_section_id=%d", HANZI_BASE_DEVICE_IMEI, section_id);
        //MMIZDT_HTTP_AppSend(TRUE, HANZI_BOOK_HEADER_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, Hanzi_ParseDetailInfo);
    }
}

PUBLIC void HanziDetail_DeleteNewWordItem(uint16 cur_idx)
{
    char file_path[80] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    char *temp_file_path = HANZI_CARD_NEW_WORD_PATH;
    strcpy(file_path,temp_file_path);
    if(hanzi_detail_count == 1 && cur_idx == 0){
        if(zmt_file_exist(file_path)){
            zmt_file_delete(file_path);
        }
        MMI_CloseHanziDetailWin();
        return;
    }
    {
        uint16 i = 0;
        char * out = NULL;
        cJSON * root ;
        cJSON * F_list;
        cJSON * list_root;
        cJSON * F_word;
        cJSON * F_pingy;
        cJSON * F_audio_uri;
        cJSON * F_remark;

        root = cJSON_CreateObject();
        F_list = cJSON_CreateArray();
        for(i = 0;i < hanzi_detail_count && i < 100;i++)
        {
            if(i == cur_idx){
                continue;
            }
            list_root = cJSON_CreateObject();
            cJSON_AddItemToArray(F_list, list_root);

            F_word = cJSON_CreateString(hanzi_detail_info[i]->word);
            cJSON_AddItemToObject(list_root, "F_word", F_word);

            F_pingy = cJSON_CreateString(hanzi_detail_info[i]->pingy);
            cJSON_AddItemToObject(list_root, "F_pingy", F_pingy);

            F_audio_uri = cJSON_CreateString(hanzi_detail_info[i]->audio_uri);
            cJSON_AddItemToObject(list_root, "F_audio_uri", F_audio_uri);

            F_remark = cJSON_CreateString(hanzi_detail_info[i]->remark);
            cJSON_AddItemToObject(list_root, "F_remark", F_remark);
        }
        cJSON_AddItemToObject(root, "F_list", F_list);

        out = cJSON_PrintUnformatted(root);
        
        if(zmt_file_exist(file_path)){
            zmt_file_delete(file_path);
        }
        zmt_file_data_write(out, strlen(out), file_path);
        
        SCI_FREE(out);
        cJSON_Delete(root);
        
        HanziDetail_ReleaseDetailInfo();
        data_buf = zmt_file_data_read(file_path, &file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            Hanzi_ParseDetailInfo(1, data_buf, file_len,0);
        }
        if(hanzi_detail_cur_idx >= hanzi_detail_count){
            hanzi_detail_cur_idx--;
        }
    }
}

PUBLIC void Hanzi_ParseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        hanzi_detail_info[hanzi_detail_cur_idx]->audio_len = Rcv_len;
        if(hanzi_detail_info[hanzi_detail_cur_idx]->audio_data != PNULL)
        {
            SCI_FREE(hanzi_detail_info[hanzi_detail_cur_idx]->audio_data);
            hanzi_detail_info[hanzi_detail_cur_idx]->audio_data = NULL;
        }
        hanzi_detail_info[hanzi_detail_cur_idx]->audio_data = SCI_ALLOCA(Rcv_len);
        SCI_MEMSET(hanzi_detail_info[hanzi_detail_cur_idx]->audio_data, 0, Rcv_len);
        SCI_MEMCPY(hanzi_detail_info[hanzi_detail_cur_idx]->audio_data, pRcv, Rcv_len);

        HanziDetail_PlayPinyinAudio();
    }
    else
    {
        hanzi_detail_info[hanzi_detail_cur_idx]->audio_len = -2;
    }
}


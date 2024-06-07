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
#include "zmt_hanzi_main.h"
#include "zmt_hanzi_id.h"
#include "zmt_hanzi_text.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "zmt_main_file.h"
#endif

extern HANZI_BOOK_INFO_T hanzi_book_info;
extern BOOLEAN is_open_auto_play;
extern int cur_chapter_unmaster_idx[HANZI_CHAPTER_WORD_MAX];
extern uint8 cur_new_word_page_idx;

HANZI_CONTENT_INFO_T * hanzi_content_info[HANZI_CONTENT_CHAPTER_MAX];
int16 hanzi_chapter_count = 0;
int16 hanzi_chapter_children_count[HANZI_CONTENT_CHAPTER_MAX] = {0};
HANZI_BOOK_HANZI_INFO * hanzi_detail_info[HANZI_CHAPTER_WORD_MAX];
int16 hanzi_detail_count = 0;
int16 hanzi_detail_cur_idx = 0;
BOOLEAN hanzi_is_load_local = FALSE;

PUBLIC void Hanzi_WriteExistUnmasterHanzi(cJSON * hanzis, uint8 write_count)
{
    uint16 i = 0;
    uint16 j = 0;
    uint16 count = 0;
    char file_path[30] = {0};
    char * pRcv = NULL;
    uint32 data_size = 0;
    cJSON * roots;
    cJSON * hanziss;
    cJSON * hanzis_item;
    cJSON * hanzis_items;
    cJSON * texts;
    cJSON * audios;
    cJSON * pinyins;
    cJSON * similar_words;
    cJSON * annotations;

    strcpy(file_path,HANZI_CARD_NEW_WORD_PATH);
    if(zmt_tfcard_exist() && zmt_file_exist(file_path)){
        pRcv = zmt_file_data_read(file_path, &data_size);
    }else{
        return;
    }
    if(pRcv != NULL && data_size > 2)
    {
        roots = cJSON_Parse(pRcv);
        if(roots != NULL && roots->type != cJSON_NULL)
        {
            hanziss = cJSON_GetObjectItem(roots, "hanzis");
            count = cJSON_GetArraySize(hanziss);
            for(i = 0; i < cJSON_GetArraySize(hanziss);i++)
            {
                BOOLEAN is_exist = FALSE;
                hanzis_item = cJSON_GetArrayItem(hanziss, i);
                texts = cJSON_GetObjectItem(hanzis_item, "text");
                audios = cJSON_GetObjectItem(hanzis_item, "audio");
                pinyins = cJSON_GetObjectItem(hanzis_item, "pinyin");
                similar_words = cJSON_GetObjectItem(hanzis_item, "similar_word");
                annotations = cJSON_GetObjectItem(hanzis_item, "annotation");

                //SCI_TRACE_LOW("%s: texts->valuestring = %s", __FUNCTION__, texts->valuestring);
                //SCI_TRACE_LOW("%s: pinyins->valuestring = %s", __FUNCTION__, pinyins->valuestring);
                for(j = 0;j < write_count;j++)
                {
                    uint8 idx = cur_chapter_unmaster_idx[j] - 1;
                    //SCI_TRACE_LOW("%s: hanzi_detail_info[%d]->word = %s", __FUNCTION__, idx, hanzi_detail_info[idx]->word);
                    //SCI_TRACE_LOW("%s: hanzi_detail_info[%d]->pingy = %s", __FUNCTION__, idx, hanzi_detail_info[idx]->pingy);
                    if(texts->valuestring != NULL && pinyins->valuestring != NULL)
                    {
                        if(0 == strcmp(texts->valuestring, hanzi_detail_info[idx]->word) && 
                            0 == strcmp(pinyins->valuestring, hanzi_detail_info[idx]->pingy))
                        {
                            is_exist = TRUE;
                            break;
                        }
                    }
                }
                SCI_TRACE_LOW("%s: is_exist = %d", __FUNCTION__, is_exist);
                if(is_exist){
                    continue;
                }

                hanzis_items = cJSON_CreateObject();
                if(texts->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "text", texts->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "text", "");
                }
                if(audios->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "audio", audios->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "audio", "");
                }
                if(pinyins->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "pinyin", pinyins->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "pinyin", "");
                }
                if(similar_words->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "similar_word", similar_words->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "similar_word", "");
                }
                if(annotations->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "annotation", annotations->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "annotation", "");
                }
                cJSON_AddItemToArray(hanzis, hanzis_items);
            }
            cJSON_Delete(roots);
        }
        SCI_FREE(pRcv);
    }
}

PUBLIC void Hanzi_WriteUnmasterHanzi(uint8 write_count)
{
    uint16 i = 0;
    char * out = NULL;
    char file_path[30] = {0};
    char * pRcv = NULL;
    uint32 data_size = 0;
    cJSON * root;
    cJSON * hanzis;
    cJSON * hanzis_item;
    cJSON * text;
    cJSON * audio;
    cJSON * pinyin;
    cJSON * similar_word;
    cJSON * annotation;

    if(write_count <= 0){
        SCI_TRACE_LOW("%s: UnmasterHanzi no to write", __FUNCTION__);
        return;
    }
    SCI_TRACE_LOW("%s: write_count = %d", __FUNCTION__, write_count);

    root = cJSON_CreateObject();
    hanzis = cJSON_CreateArray();

    strcpy(file_path,HANZI_CARD_NEW_WORD_PATH);
    Hanzi_WriteExistUnmasterHanzi(hanzis, write_count);
    for(i = 0;i < HANZI_CHAPTER_WORD_MAX;i++)
    {       
        if(cur_chapter_unmaster_idx[i] != 0)
        {
            hanzis_item = cJSON_CreateObject();
            if(hanzi_detail_info[i]->word != NULL){
                text = cJSON_CreateString(hanzi_detail_info[i]->word);
                cJSON_AddItemToObject(hanzis_item, "text", text);
            }else{
                cJSON_AddStringToObject(hanzis_item, "text", "");
            }
            
            if(hanzi_detail_info[i]->audio_uri != NULL){
                audio = cJSON_CreateString(hanzi_detail_info[i]->audio_uri);
                cJSON_AddItemToObject(hanzis_item, "audio", audio);
            }else{
                cJSON_AddStringToObject(hanzis_item, "audio", "");
            }
            
            if(hanzi_detail_info[i]->pingy != NULL){
                pinyin = cJSON_CreateString(hanzi_detail_info[i]->pingy);
                cJSON_AddItemToObject(hanzis_item, "pinyin", pinyin);
            }else{
                cJSON_AddStringToObject(hanzis_item, "pinyin", "");
            }
            
            if(hanzi_detail_info[i]->similar_word != NULL){
                similar_word = cJSON_CreateString(hanzi_detail_info[i]->similar_word);
                cJSON_AddItemToObject(hanzis_item, "similar_word", similar_word);
            }else{
                cJSON_AddStringToObject(hanzis_item, "similar_word", "");
            }
            
            if(hanzi_detail_info[i]->annotation != NULL){
                annotation = cJSON_CreateString(hanzi_detail_info[i]->annotation);
                cJSON_AddItemToObject(hanzis_item, "annotation", annotation);
            }else{
                cJSON_AddStringToObject(hanzis_item, "annotation", "");
            }

            cJSON_AddItemToArray(hanzis, hanzis_item);
        }
    }
    cJSON_AddItemToObject(root, "hanzis", hanzis);
    out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    //SCI_TRACE_LOW("%s: out = %s", __FUNCTION__, out);

    if(zmt_tfcard_exist() && zmt_file_exist(file_path)){
        zmt_file_delete(file_path);
    }
    zmt_file_data_write(out, strlen(out), file_path);
    SCI_FREE(out);
}

PUBLIC void Hanzi_ReleaseChapterInfo(void)
{
    uint8 i,j = 0;
    for(i = 0;i < HANZI_CONTENT_CHAPTER_MAX;i++)
    {
        if(hanzi_content_info[i] != NULL){
            if(hanzi_content_info[i]->content_name != NULL){
                SCI_FREE(hanzi_content_info[i]->content_name);
                hanzi_content_info[i]->content_name = NULL;
            }
            for(j = 0;j < HANZI_CONTENT_CHAPTER_MAX;j++){
                if(hanzi_content_info[i]->chapter[j] != NULL){
                    if(hanzi_content_info[i]->chapter[j]->chapter_name != NULL){
                        SCI_FREE(hanzi_content_info[i]->chapter[j]->chapter_name);
                        hanzi_content_info[i]->chapter[j]->chapter_name = NULL;
                    }
                    SCI_FREE(hanzi_content_info[i]->chapter[j]);
                    hanzi_content_info[i]->chapter[j]= NULL;
                }
            }
            SCI_FREE(hanzi_content_info[i]);
            hanzi_content_info[i] = NULL;
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
        if(root != NULL && root->type != cJSON_NULL)
        {
            for(i = 0;i < cJSON_GetArraySize(root) && i < HANZI_CONTENT_CHAPTER_MAX;i++)
            {              
                cJSON *list_item =  cJSON_GetArrayItem(root, i);
                cJSON *contentId = cJSON_GetObjectItem(list_item, "id");
                cJSON *contentName = cJSON_GetObjectItem(list_item, "contentName");
                cJSON *chapters = cJSON_GetObjectItem(list_item, "chapters");
                if(hanzi_content_info[i] != NULL){
                    SCI_FREE(hanzi_content_info[i]);
                    hanzi_content_info[i] = NULL;
                }
                hanzi_content_info[i] = (HANZI_CONTENT_INFO_T *)SCI_ALLOC_APP(sizeof(HANZI_CONTENT_INFO_T));
                memset(hanzi_content_info[i], 0, sizeof(HANZI_CONTENT_INFO_T));
                if(contentName != NULL && contentName->type != cJSON_NULL)
                {
                    if(hanzi_content_info[i]->content_name != NULL){
                        SCI_FREE(hanzi_content_info[i]->content_name);
                        hanzi_content_info[i]->content_name = NULL;
                    }
                    
                    size = strlen(contentName->valuestring);
                    hanzi_content_info[i]->content_name = SCI_ALLOC_APP(size+1);
                    memset(hanzi_content_info[i]->content_name, 0, size+1);
                    strncpy(hanzi_content_info[i]->content_name, contentName->valuestring, size);
                    SCI_TRACE_LOW("%s: info[%d].content_name = %s", __FUNCTION__, i, hanzi_content_info[i]->content_name);

                    hanzi_content_info[i]->content_id = contentId->valueint;
                    SCI_TRACE_LOW("%s: info[%d].content_id = %d", __FUNCTION__, i, hanzi_content_info[i]->content_id);            
                }
                for(m = 0;m < cJSON_GetArraySize(chapters) && m < HANZI_CONTENT_CHAPTER_MAX;m++)
                {
                    cJSON *chapters_item =  cJSON_GetArrayItem(chapters, m);
                    cJSON *chaptersId = cJSON_GetObjectItem(chapters_item, "id");
                    cJSON *chaptersName = cJSON_GetObjectItem(chapters_item, "chapterName");

                    if(hanzi_content_info[i]->chapter[m] != NULL){
                        SCI_FREE(hanzi_content_info[i]->chapter[m]);
                        hanzi_content_info[i]->chapter[m] = NULL;
                    }
                    hanzi_content_info[i]->chapter[m] = SCI_ALLOC_APP(sizeof(HANZI_CONTENT_CHAPTER_INFO_T));
                    memset(hanzi_content_info[i]->chapter[m], 0, sizeof(HANZI_CONTENT_CHAPTER_INFO_T));
                    
                    if(hanzi_content_info[i]->chapter[m]->chapter_name != NULL){
                        SCI_FREE(hanzi_content_info[i]->chapter[m]->chapter_name);
                        hanzi_content_info[i]->chapter[m]->chapter_name = NULL;
                    }
                    size = strlen(chaptersName->valuestring);
                    hanzi_content_info[i]->chapter[m]->chapter_name = SCI_ALLOC_APP(size+1);
                    memset(hanzi_content_info[i]->chapter[m]->chapter_name, 0, size+1);
                    strncpy(hanzi_content_info[i]->chapter[m]->chapter_name, chaptersName->valuestring, size);
                    SCI_TRACE_LOW("%s: info[%d].chapter[%d]->chapter_name = %s", __FUNCTION__, i, m, chaptersName->valuestring);

                    hanzi_content_info[i]->chapter[m]->chapter_id = chaptersId->valueint;
                }
                hanzi_chapter_children_count[i] = m;
            }
            hanzi_chapter_count = i;
            if(!hanzi_is_load_local)
            {
                char * out = NULL;
                char file_path[30] = {0};
                sprintf(file_path, HANZI_CARD_GRADE_CHAPTER_PATH, hanzi_book_info.cur_book_idx+1, hanzi_book_info.cur_book_idx+1);
                if(zmt_file_exist(file_path)){
                    zmt_file_delete(file_path);
                }
                out = cJSON_PrintUnformatted(root);
                zmt_file_data_write(out, strlen(out), file_path);
                SCI_FREE(out);
            }
            
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
    if(hanzi_is_load_local){
        if(pRcv != PNULL)
        {
        	SCI_FREE(pRcv);
        	pRcv = PNULL;
        }
    }
    hanzi_is_load_local = FALSE;
    MMK_SendMsg(MMI_HANZI_CHAPTER_WIN_ID, MSG_FULL_PAINT, PNULL);
}

PUBLIC void Hanzi_requestChapterInfo(uint8 grade_id)
{
    char file_path[30] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    sprintf(file_path, HANZI_CARD_GRADE_CHAPTER_PATH, grade_id, grade_id);
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        SCI_TRACE_LOW("%s: file_len = %d", __FUNCTION__, file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            hanzi_is_load_local = TRUE;
            Hanzi_ParseChapterInfo(1, data_buf, file_len,0);
        }
    }
    else
    {
        char url[40] = {0};
        sprintf(url,HANZI_BOOK_GRADE_PATH,grade_id);
        SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
        hanzi_is_load_local = FALSE;
        MMIZDT_HTTP_AppSend(TRUE, HANZI_BOOK_HEADER_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, Hanzi_ParseChapterInfo);
    }
}

PUBLIC void Hanzi_ReleaseDetailInfo(void)
{
    uint16 i = 0;
    for(i = 0;i < HANZI_CHAPTER_WORD_MAX;i++)
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
            if(hanzi_detail_info[i]->similar_word != NULL){
                SCI_FREE(hanzi_detail_info[i]->similar_word);
                hanzi_detail_info[i]->similar_word = NULL;
            }
            if(hanzi_detail_info[i]->annotation != NULL){
                SCI_FREE(hanzi_detail_info[i]->annotation);
                hanzi_detail_info[i]->annotation = NULL;
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

LOCAL void Hanzi_DetailMakeTermRemark(char* remark, char * similar_word, char * annotation)
{
    if(remark != NULL)
    {
        if(similar_word != NULL && strlen(similar_word) > 0){
            char word[20] = {0};
            MMI_STRING_T text_string = {0};
            MMIRES_GetText(HANZI_SIMILAR_WORD, MMI_HANZI_DETAIL_WIN_ID, &text_string);
            GUI_WstrToUTF8(word, 20, text_string.wstr_ptr, text_string.wstr_len);
            strcat(remark, word);
            strcat(remark, similar_word);
            strcat(remark, "\n");
        }
        if(annotation != NULL && strlen(annotation) > 0){
            strcat(remark, annotation);
        }
    }
}

LOCAL void Hanzi_AddDetailInfo(uint8 idx, cJSON* text, cJSON * audio, cJSON * pinyin, cJSON * similar_word, cJSON *annotation)
{
    uint32 size = 0;
    uint32 similar_word_size = 0;
    uint32 annotation_size = 0;
    hanzi_detail_info[idx] = (HANZI_BOOK_HANZI_INFO *)SCI_ALLOC_APP(sizeof(HANZI_BOOK_HANZI_INFO));
    memset(hanzi_detail_info[idx], 0, sizeof(HANZI_BOOK_HANZI_INFO));

    if(text->valuestring != NULL){
        size = strlen(text->valuestring);
        if(size > 0){
            hanzi_detail_info[idx]->word = (char *)SCI_ALLOC_APP(size + 1);
            memset(hanzi_detail_info[idx]->word, 0, size + 1);
            SCI_MEMCPY(hanzi_detail_info[idx]->word, text->valuestring, size);
        }else{
            hanzi_detail_info[idx]->word = NULL;
        }
    }else{
        hanzi_detail_info[idx]->word = NULL;
    }

    if(pinyin->valuestring != NULL){
        size = strlen(pinyin->valuestring);
        if(size > 0){
            hanzi_detail_info[idx]->pingy = (char *)SCI_ALLOC_APP(size + 1);
            memset(hanzi_detail_info[idx]->pingy, 0, size + 1);
            SCI_MEMCPY(hanzi_detail_info[idx]->pingy, pinyin->valuestring, size);
        }else{
            hanzi_detail_info[idx]->pingy = NULL;
        }
    }else{
        hanzi_detail_info[idx]->pingy = NULL;
    }

    if(audio->valuestring != NULL){
        size = strlen(audio->valuestring);
        if(size > 0){
            hanzi_detail_info[idx]->audio_uri = (char *)SCI_ALLOC_APP(size + 1);
            memset(hanzi_detail_info[idx]->audio_uri, 0, size + 1);
            SCI_MEMCPY(hanzi_detail_info[idx]->audio_uri, audio->valuestring, size);
            hanzi_detail_info[idx]->audio_len = 0;
        }else{
            hanzi_detail_info[idx]->audio_uri = NULL;
            hanzi_detail_info[idx]->audio_len = -1;
        }                    
    }else{
        hanzi_detail_info[idx]->audio_uri = NULL;
        hanzi_detail_info[idx]->audio_len = -1;
    }
                        
    if(similar_word->valuestring != NULL){
        similar_word_size = strlen(similar_word->valuestring);
        if(similar_word_size > 0){
            hanzi_detail_info[idx]->similar_word = (char *)SCI_ALLOC_APP(similar_word_size + 1);
            memset(hanzi_detail_info[idx]->similar_word, 0, similar_word_size + 1);
            SCI_MEMCPY(hanzi_detail_info[idx]->similar_word, similar_word->valuestring, similar_word_size);
        }else{
            hanzi_detail_info[idx]->similar_word = NULL;
        }
    }else{
        hanzi_detail_info[idx]->similar_word = NULL;
    }
                        
    if(annotation->valuestring != NULL){
        annotation_size = strlen(annotation->valuestring);
        if(annotation_size > 0){
            hanzi_detail_info[idx]->annotation = (char *)SCI_ALLOC_APP(annotation_size + 1);
            memset(hanzi_detail_info[idx]->annotation, 0, annotation_size + 1);
            SCI_MEMCPY(hanzi_detail_info[idx]->annotation, annotation->valuestring, annotation_size);
        }else{
            hanzi_detail_info[idx]->annotation = NULL;
        }
    }else{
        hanzi_detail_info[idx]->annotation = NULL;
    }

    size = similar_word_size + annotation_size + 30;
    hanzi_detail_info[idx]->remark = (char *)SCI_ALLOC_APP(size + 1);
    memset(hanzi_detail_info[idx]->remark, 0, size + 1);
    Hanzi_DetailMakeTermRemark(hanzi_detail_info[idx]->remark, similar_word->valuestring, annotation->valuestring);          
}

PUBLIC void Hanzi_ParseDetailInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        uint8 j = 0;
        uint16 req_chapter_id = 0;
        uint32 size = 0;
        cJSON *root = cJSON_Parse(pRcv);
        if(root != NULL && root->type != cJSON_NULL)
        {
            SCI_TRACE_LOW("%s: cur_chapter_idx = %d, cur_section_children_idx = %d", __FUNCTION__, hanzi_book_info.cur_chapter_idx, hanzi_book_info.cur_section_children_idx);
            if(cJSON_GetArraySize(root) > hanzi_book_info.cur_section_children_idx)
            {
                cJSON *list_item =  cJSON_GetArrayItem(root, hanzi_book_info.cur_section_children_idx);
                cJSON *id = cJSON_GetObjectItem(list_item, "id");
                cJSON *hanzis = cJSON_GetObjectItem(list_item, "hanzis");
                req_chapter_id = hanzi_content_info[hanzi_book_info.cur_section_idx]->chapter[hanzi_book_info.cur_section_children_idx]->chapter_id;
                if(id->valueint == req_chapter_id)
                {
                    hanzi_detail_count = cJSON_GetArraySize(hanzis);
                    SCI_TRACE_LOW("%s: hanzi_detail_count = %d", __FUNCTION__, hanzi_detail_count);
                    if(hanzi_detail_count > HANZI_CHAPTER_WORD_MAX){
                        hanzi_detail_count = HANZI_CHAPTER_WORD_MAX;
                    }
                    for(j = 0;j < cJSON_GetArraySize(hanzis) && j < HANZI_CHAPTER_WORD_MAX;j++)
                    {
                        cJSON *hanzis_item =  cJSON_GetArrayItem(hanzis, j);
                        cJSON *text = cJSON_GetObjectItem(hanzis_item, "text");
                        cJSON *audio = cJSON_GetObjectItem(hanzis_item, "audio");
                        cJSON *pinyin = cJSON_GetObjectItem(hanzis_item, "pinyin");
                        cJSON * similar_word = cJSON_GetObjectItem(hanzis_item, "similar_word");
                        cJSON *annotation = cJSON_GetObjectItem(hanzis_item, "annotation");
                        
                        Hanzi_AddDetailInfo(j, text, audio, pinyin, similar_word, annotation);
                    }
                    if(!hanzi_is_load_local)
                    {
                        char * out = NULL;
                        char file_path[40] = {0};
                        sprintf(file_path, HANZI_CARD_CHAPTER_WORD_PATH, 
                            hanzi_book_info.cur_book_idx+1, 
                            hanzi_content_info[hanzi_book_info.cur_section_idx]->content_id
                        );
                        if(zmt_file_exist(file_path)){
                            zmt_file_delete(file_path);
                        }
                        out = cJSON_PrintUnformatted(root);
                        zmt_file_data_write(out, strlen(out), file_path);
                        SCI_FREE(out);
                    }
                    if(is_open_auto_play){
                        HanziDetail_PlayPinyinAudio();
                    }
                }
                else
                {
                    hanzi_detail_count = -2;
                }
            }
            else
            {
                hanzi_detail_count = -2;
            }
            cJSON_Delete(root);
        }
        else
        {
            hanzi_detail_count = -2;
        }
    }
    else
    {
        hanzi_detail_count = -1;
    }
    if(hanzi_is_load_local){
        if(pRcv != PNULL)
        {
        	SCI_FREE(pRcv);
        	pRcv = PNULL;
        }
    }
    hanzi_is_load_local = FALSE;
    HanziDetail_TipTimeout(0, 0);
}

PUBLIC void Hanzi_requestDetailInfo(uint8 grade_id, uint16 req_id)
{
    char file_path[40] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    SCI_TRACE_LOW("%s: grade_id = %d, req_id = %d", __FUNCTION__,grade_id, req_id);
    sprintf(file_path, HANZI_CARD_CHAPTER_WORD_PATH, grade_id, req_id);
    Hanzi_ReleaseDetailInfo();
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        SCI_TRACE_LOW("%s: file_len = %d", __FUNCTION__, file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            hanzi_is_load_local = TRUE;
            Hanzi_ParseDetailInfo(1, data_buf, file_len,0);
        }
    }
    else
    {
        char url[130] = {0};
        sprintf(url, HANZI_BOOK_CHAPTER_PATH, req_id);
        SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
        hanzi_is_load_local = FALSE;
        MMIZDT_HTTP_AppSend(TRUE, HANZI_BOOK_HEADER_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, Hanzi_ParseDetailInfo);
    }
}

PUBLIC void Hanzi_ParseUnmasterDetailInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint16 i = 0;
        uint16 j = 0;
        uint16 read_idx = 0;
        uint8 read_count = 0;
        cJSON * root;
        cJSON * hanzis;
        cJSON * hanzis_item;
        cJSON * text;
        cJSON * audio;
        cJSON * pinyin;
        cJSON * similar_word;
        cJSON * annotation;

        root = cJSON_Parse(pRcv);
        if(root != NULL && root->type != cJSON_NULL)
        {
            hanzis = cJSON_GetObjectItem(root, "hanzis");
            hanzi_detail_count = cJSON_GetArraySize(hanzis);
            read_idx = cur_new_word_page_idx * HANZI_CHAPTER_WORD_MAX;
            read_count = (cur_new_word_page_idx + 1) * HANZI_CHAPTER_WORD_MAX;
            for(i = read_idx; i < cJSON_GetArraySize(hanzis) && i < read_count;i++)
            {
                hanzis_item = cJSON_GetArrayItem(hanzis, i);
                text = cJSON_GetObjectItem(hanzis_item, "text");
                audio = cJSON_GetObjectItem(hanzis_item, "audio");
                pinyin = cJSON_GetObjectItem(hanzis_item, "pinyin");
                similar_word = cJSON_GetObjectItem(hanzis_item, "similar_word");
                annotation = cJSON_GetObjectItem(hanzis_item, "annotation");

                Hanzi_AddDetailInfo(j, text, audio, pinyin, similar_word, annotation);
                j++;
            }
            cJSON_Delete(root);
        }
        else
        {
            hanzi_detail_count = -3;
        }
    }
    else
    {
        hanzi_detail_count = -3;
    }
    if(hanzi_is_load_local){
        if(pRcv != PNULL)
        {
        	SCI_FREE(pRcv);
        	pRcv = PNULL;
        }
    }
    hanzi_is_load_local = FALSE;
    HanziDetail_TipTimeout(0, 0);
}

PUBLIC void Hanzi_RequestNewWord(void)
{
    char file_path[30] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    char *temp_file_path = HANZI_CARD_NEW_WORD_PATH;
    strcpy(file_path,temp_file_path);
    Hanzi_ReleaseDetailInfo();
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        SCI_TRACE_LOW("%s: file_len = %d", __FUNCTION__, file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            hanzi_is_load_local = TRUE;
            Hanzi_ParseUnmasterDetailInfo(1, data_buf, file_len,0);
        }
    }
    else
    {
        //char url[200] = {0};
        hanzi_is_load_local = FALSE;
        Hanzi_ParseUnmasterDetailInfo(0, data_buf, file_len,0);
        //sprintf(url, "v1/card/chapters/hanzi?F_cardId=%s&&F_section_id=%d", HANZI_BASE_DEVICE_IMEI, section_id);
        //MMIZDT_HTTP_AppSend(TRUE, HANZI_BOOK_HEADER_PATH, url, strlen(url), 1000, 0, 0, 0, 0, 0, Hanzi_ParseUnmasterDetailInfo);
    }
}

PUBLIC void Hanzi_DeleteNewWordItem(uint16 cur_idx)
{
    char file_path[30] = {0};
    char * data_buf = PNULL;
    uint32 file_len = 0;
    cJSON * root;
    
    strcpy(file_path,HANZI_CARD_NEW_WORD_PATH);
    SCI_TRACE_LOW("%s: cur_idx = %d, hanzi_detail_count = %d", __FUNCTION__, cur_idx, hanzi_detail_count);
    if(hanzi_detail_count == 1 && cur_idx == 0){
        if(zmt_file_exist(file_path)){
            zmt_file_delete(file_path);
        }
        MMI_CloseHanziDetailWin();
        return;
    }
    
    if(zmt_file_exist(file_path)){
        data_buf = zmt_file_data_read(file_path, &file_len);
        if(data_buf != PNULL && file_len > 0){
            root = cJSON_Parse(data_buf);
        }else{
            return;
        }
    }else{
        return;
    }
    {
        uint16 i = 0;
        char * out = NULL;
        cJSON * roots;
        cJSON * hanzis;
        cJSON * hanzis_item;
        cJSON * text;
        cJSON * audio;
        cJSON * pinyin;
        cJSON * similar_word;
        cJSON * annotation;

        if(root != NULL && root->type != cJSON_NULL)
        {      
            cJSON * hanziss;
            cJSON * hanzis_item;
            cJSON * hanzis_items;
            cJSON * texts;
            cJSON * audios;
            cJSON * pinyins;
            cJSON * similar_words;
            cJSON * annotations;
            
            roots = cJSON_CreateObject();
            hanziss = cJSON_CreateArray();
            hanzis = cJSON_GetObjectItem(root, "hanzis");
            for(i = 0;i < hanzi_detail_count;i++)
            {
                if(i == cur_idx){
                    continue;
                }
                hanzis_item = cJSON_GetArrayItem(hanzis, i);
                text = cJSON_GetObjectItem(hanzis_item, "text");
                audio = cJSON_GetObjectItem(hanzis_item, "audio");
                pinyin = cJSON_GetObjectItem(hanzis_item, "pinyin");
                similar_word = cJSON_GetObjectItem(hanzis_item, "similar_word");
                annotation = cJSON_GetObjectItem(hanzis_item, "annotation");

                hanzis_items = cJSON_CreateObject();
                if(text->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "text", text->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "text", "");
                }
                if(audio->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "audio", audio->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "audio", "");
                }
                if(pinyin->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "pinyin", pinyin->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "pinyin", "");
                }
                if(similar_word->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "similar_word", similar_word->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "similar_word", "");
                }
                if(annotation->valuestring != NULL){
                    cJSON_AddStringToObject(hanzis_items, "annotation", annotation->valuestring);
                }else{
                    cJSON_AddStringToObject(hanzis_items, "annotation", "");
                }
                cJSON_AddItemToArray(hanziss, hanzis_items);
            }
            cJSON_AddItemToObject(roots, "hanzis", hanziss);
        }
        out = cJSON_PrintUnformatted(roots);  
        if(zmt_file_exist(file_path)){
            zmt_file_delete(file_path);
        }
        zmt_file_data_write(out, strlen(out), file_path);
        SCI_FREE(out);
        cJSON_Delete(root);
        cJSON_Delete(roots);

        SCI_TRACE_LOW("%s: before, hanzi_detail_cur_idx = %d, cur_new_word_page_idx = %d", __FUNCTION__, hanzi_detail_cur_idx, cur_new_word_page_idx);
        if(hanzi_detail_cur_idx == 0 && cur_new_word_page_idx > 0){
            hanzi_detail_cur_idx = HANZI_CHAPTER_WORD_MAX - 1;
            cur_new_word_page_idx--;
        }else if(hanzi_detail_cur_idx == 0 && cur_new_word_page_idx == 0)
        {
            //not to do
        }else if(hanzi_detail_cur_idx > 0 && hanzi_detail_cur_idx < HANZI_CHAPTER_WORD_MAX){
            hanzi_detail_cur_idx--;
        }
        SCI_TRACE_LOW("%s: after, hanzi_detail_cur_idx = %d, cur_new_word_page_idx = %d", __FUNCTION__, hanzi_detail_cur_idx, cur_new_word_page_idx);
        Hanzi_ReleaseDetailInfo();
        data_buf = zmt_file_data_read(file_path, &file_len);
        if(data_buf != PNULL && file_len > 0)
        {
            hanzi_is_load_local = TRUE;
            Hanzi_ParseUnmasterDetailInfo(1, data_buf, file_len,0);
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


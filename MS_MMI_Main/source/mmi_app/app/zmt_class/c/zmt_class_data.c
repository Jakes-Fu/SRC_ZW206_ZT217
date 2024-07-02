/*****************************************************************************
** File Name:      zmt_class_data.c                                               *
** Author:         fys                                               *
** Date:           2024/06/27                                                *
******************************************************************************/
#include "std_header.h"
#include "sci_types.h"
#include "mmi_module.h"
#include "cjson.h"
#include "dal_time.h"
#include "zmt_class_main.h"
#include "zmt_class_id.h"
#include "zmt_class_text.h"
#include "zmt_class_image.h"
#include "mbedtls/md5.h"

extern CLASS_SYNC_INFO_T class_sync_info;
extern CLASS_BOOK_INFO_T * class_book_info[CLASS_SYN_BOOK_NUM_MAX];
extern int8 class_book_count;
extern CLASS_SECTION_INFO_T * class_section_info[CLASS_SYN_SECTION_NUM_MAX];
extern int8 class_section_count;
extern CLASS_READ_INFO_T * class_read_info[CLASS_SYN_READ_NUM_MAX];
extern int8 class_read_count;
extern CLASS_READ_CURRENT_INFO_T class_cur_info;
uint8 class_cur_down_idx = 0;
BOOLEAN class_download_next_now = FALSE;
BOOLEAN have_new_quest = FALSE;

LOCAL void Class_ParseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
LOCAL void Class_ParseMp3FileDownload(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);

LOCAL uint8 * Class_MakeMd5Str(char * sign)
{
    mbedtls_md5_context md5_ctx = {0};
    char digest[16] = {0};
    uint8 i = 0;
    uint8 *md5 = (uint8 *)SCI_ALLOC_APPZ(33);
#ifndef WIN32
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, sign, strlen(sign));
    mbedtls_md5_finish(&md5_ctx, digest);
    mbedtls_md5_free(&md5_ctx);
#endif
    for(i = 0; i < 16; i++)
    {
        sprintf(md5+(i*2),"%02x",digest[i]);
    }
    return md5;
}

LOCAL uint8 * Class_MakeShowCoursesToken(uint8 subject_id, uint8 grade_id, uint8 page)
{
    uint32 times = 0;
    char sign[100] = {0};
    uint8 * md5_str = NULL;
    uint8 * token = NULL;

    times = MMIAPICOM_GetCurTime() + CLASS_SYN_TIMES_DIFF_FROM_1978_TO_1980;
    
    //appid,course_type,grade_id,mediatype,page,subject_id,take_number,times,CLASS_SYN_SECRET
    sprintf(sign, "%s%d%d%d%d%d%d%d%s", 
        CLASS_SYN_APP_ID, CLASS_SYN_COURSE_TYPE, grade_id, CLASS_SYN_MEDIATYPE, 
        page, subject_id, CLASS_SYN_TAKE_NUMBER, times, CLASS_SYN_SECRET
    );
    SCI_TRACE_LOW("%s: sign = %s", __FUNCTION__, sign);
    
    md5_str = Class_MakeMd5Str(sign);
    SCI_TRACE_LOW("%s: md5_str = %s", __FUNCTION__, md5_str);

    token = SCI_ALLOC_APPZ(100);
    memset(token, 0, 100);
    sprintf(token, CLASS_SYN_BASE_TOKEN, CLASS_SYN_APP_ID, times, md5_str);

    SCI_FREE(md5_str);
    return token;
}

PUBLIC void Class_ReleaseBookInfo(void)
{
    uint8 i = 0;
    for(i = 0;i < CLASS_SYN_BOOK_NUM_MAX;i++)
    {
        if(class_book_info[i] != NULL){
            if(class_book_info[i]->cate_id != NULL){
                SCI_FREE(class_book_info[i]->cate_id);
                class_book_info[i]->cate_id = NULL;
            }
            if(class_book_info[i]->course_id != NULL){
                SCI_FREE(class_book_info[i]->course_id);
                class_book_info[i]->course_id = NULL;
            }
            if(class_book_info[i]->name != NULL){
                SCI_FREE(class_book_info[i]->name);
                class_book_info[i]->name = NULL;
            }
            SCI_FREE(class_book_info[i]);
            class_book_info[i] = NULL;
        }
    }
}

LOCAL void Class_ParseBookInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        uint8 m = 0;
        uint16 size = 0;
        cJSON *root = cJSON_Parse(pRcv);
        if(root != NULL && root->type != cJSON_NULL)
        {
            cJSON * code = cJSON_GetObjectItem(root, "code");
            if(code->valueint == 10000)
            {
                cJSON * data = cJSON_GetObjectItem(root, "data");
                cJSON * courses = cJSON_GetObjectItem(data, "courses");
                for(i = 0;i < cJSON_GetArraySize(courses) && i < CLASS_SYN_BOOK_NUM_MAX;i++)
                {
                    cJSON * courses_item = cJSON_GetArrayItem(courses, i);
                    cJSON * cateId = cJSON_GetObjectItem(courses_item, "cateId");
                    cJSON * cateName = cJSON_GetObjectItem(courses_item, "cateName");
                    cJSON * id = cJSON_GetObjectItem(courses_item, "id");
                    cJSON * name = cJSON_GetObjectItem(courses_item, "name");
                    //if(0 == strcmp(cateName->valuestring, "课文朗读"))
                    {
                        if(class_book_info[m] != NULL){
                            SCI_FREE(class_book_info[m]);
                            class_book_info[m] = NULL;
                        }
                        class_book_info[m] = (CLASS_BOOK_INFO_T *)SCI_ALLOC_APP(sizeof(CLASS_BOOK_INFO_T));
                        memset(class_book_info[m], 0, sizeof(CLASS_BOOK_INFO_T));

                        //cate_id
                        if(class_book_info[m]->cate_id != NULL){
                            SCI_FREE(class_book_info[m]->cate_id);
                            class_book_info[m]->cate_id = NULL;
                        }
                        size = strlen(cateId->valuestring);
                        class_book_info[m]->cate_id = SCI_ALLOC_APPZ(size + 1);
                        memset(class_book_info[m]->cate_id, 0, size + 1);
                        SCI_MEMCPY(class_book_info[m]->cate_id, cateId->valuestring, size + 1);

                        //course_id
                        if(class_book_info[m]->course_id != NULL){
                            SCI_FREE(class_book_info[m]->course_id);
                            class_book_info[m]->course_id = NULL;
                        }
                        size = strlen(id->valuestring);
                        class_book_info[m]->course_id = SCI_ALLOC_APPZ(size + 1);
                        memset(class_book_info[m]->course_id, 0, size + 1);
                        SCI_MEMCPY(class_book_info[m]->course_id, id->valuestring, size + 1);

                        //name
                        if(class_book_info[m]->name != NULL){
                            SCI_FREE(class_book_info[m]->name);
                            class_book_info[m]->name = NULL;
                        }
                        size = strlen(name->valuestring);
                        class_book_info[m]->name = SCI_ALLOC_APPZ(size + 1);
                        memset(class_book_info[m]->name, 0, size + 1);
                        SCI_MEMCPY(class_book_info[m]->name, name->valuestring, size + 1);
                        
                        m++;
                    }
                }
                class_book_count = m;
            }
            else
            {
                class_book_count = -1;
                SCI_TRACE_LOW("%s: code->valueint = %d", __FUNCTION__, code->valueint);
            }
            cJSON_Delete(root);
        }
    }
    else
    {
        class_book_count = -2;
    }
    SCI_TRACE_LOW("%s: class_book_count = %d", __FUNCTION__, class_book_count);
    if(MMK_IsFocusWin(ZMT_CLASS_BOOK_WIN_ID))
    {
        MMK_SendMsg(ZMT_CLASS_BOOK_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

PUBLIC void Class_ReuestBookInfo(uint8 subject_id, uint8 grade_id, uint8 page)
{
    char url[200] = {0};
    uint8 * token = NULL;
#ifdef WIN32
    uint32 data_size;
    char * data_buf = zmt_file_data_read(CLASS_SYN_BOOK_FILE_PATH, &data_size);
    if(data_buf != NULL && data_size > 0)
    {
        Class_ParseBookInfo(1, data_buf, data_size, 0);
        SCI_FREE(data_buf);
    }
#else
    token= Class_MakeShowCoursesToken(subject_id, grade_id, page);
    SCI_TRACE_LOW("%s: token = %s", __FUNCTION__,token);
    
    sprintf(url, CLASS_SYN_BASE_SHOWCOURSES_API, subject_id, grade_id, 
        CLASS_SYN_COURSE_TYPE, CLASS_SYN_MEDIATYPE, CLASS_SYN_TAKE_NUMBER, page, token);
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__,url);
    SCI_FREE(token);

    MMIZDT_HTTP_AppSend(TRUE, CLASS_SYN_BASE_API, url, strlen(url), 1000, 0, 0, 0, 0, 0, Class_ParseBookInfo);
#endif
}

LOCAL uint8 * Class_MakeCourseDetailToken(char * course_id)
{
    uint32 times = 0;
    char sign[100] = {0};
    uint8 * md5_str = NULL;
    uint8 * token = NULL;

    times = MMIAPICOM_GetCurTime() + CLASS_SYN_TIMES_DIFF_FROM_1978_TO_1980;
    
    sprintf(sign, "%s%s%d%s", CLASS_SYN_APP_ID, course_id, times, CLASS_SYN_SECRET);
    SCI_TRACE_LOW("%s: sign = %s", __FUNCTION__, sign);
    md5_str = Class_MakeMd5Str(sign);
    SCI_TRACE_LOW("%s: md5_str = %s", __FUNCTION__, md5_str);

    token = SCI_ALLOC_APPZ(100);
    memset(token, 0, 100);
    sprintf(token, CLASS_SYN_BASE_TOKEN, CLASS_SYN_APP_ID, times, md5_str);

    SCI_FREE(md5_str);
    return token;
}

PUBLIC void Class_ReleaseSectionInfo(void)
{
    uint8 i = 0;
    for(i = 0; i < CLASS_SYN_SECTION_NUM_MAX;i++)
    {
        if(class_section_info[i] != NULL){
            if(class_section_info[i]->type_id != NULL){
                SCI_FREE(class_section_info[i]->type_id);
                class_section_info[i]->type_id = NULL;
            }
            if(class_section_info[i]->section_name != NULL){
                SCI_FREE(class_section_info[i]->section_name);
                class_section_info[i]->section_name = NULL;
            }
            SCI_FREE(class_section_info[i]);
            class_section_info[i] = NULL;
        }
    }
}

LOCAL void Class_ParseSectionInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        uint8 j = 0;
        uint8 m = 0;
        uint16 size = 0;
        cJSON *root = cJSON_Parse(pRcv);
        if(root != NULL && root->type != cJSON_NULL)
        {
            cJSON * code = cJSON_GetObjectItem(root, "code");
            if(code->valueint == 10000)
            {
                cJSON * data = cJSON_GetObjectItem(root, "data");
                cJSON * lessons = cJSON_GetObjectItem(data, "lessons");
                for(i = 0; i < cJSON_GetArraySize(lessons);i++)
                {
                    cJSON * lessons_item = cJSON_GetArrayItem(lessons, i);
                    cJSON * section = cJSON_GetObjectItem(lessons_item, "section");
                    for(j = 0; j < cJSON_GetArraySize(section);j++)
                    {
                        cJSON * section_item = cJSON_GetArrayItem(section, j);
                        cJSON * id = cJSON_GetObjectItem(section_item, "id");
                        cJSON * name = cJSON_GetObjectItem(section_item, "name");

                        if(m >= CLASS_SYN_SECTION_NUM_MAX){
                            break;
                        }

                        if(class_section_info[m] != NULL){
                            SCI_FREE(class_section_info[m]);
                            class_section_info[m] = NULL;
                        }
                        class_section_info[m] = SCI_ALLOC_APPZ(sizeof(CLASS_SECTION_INFO_T));
                        memset(class_section_info[m], 0, sizeof(CLASS_SECTION_INFO_T));
                        
                        if(class_section_info[m]->type_id != NULL){
                            SCI_FREE(class_section_info[m]->type_id);
                            class_section_info[m]->type_id = NULL;
                        }
                        size = strlen(id->valuestring);
                        class_section_info[m]->type_id = SCI_ALLOC_APPZ(size + 1);
                        memset(class_section_info[m]->type_id, 0, size + 1);
                        SCI_MEMCPY(class_section_info[m]->type_id, id->valuestring, size);

                        if(class_section_info[m]->section_name != NULL){
                            SCI_FREE(class_section_info[m]->section_name);
                            class_section_info[m]->section_name = NULL;
                        }
                        size = strlen(name->valuestring);
                        class_section_info[m]->section_name = SCI_ALLOC_APPZ(size + 1);
                        memset(class_section_info[m]->section_name, 0, size + 1);
                        SCI_MEMCPY(class_section_info[m]->section_name, name->valuestring, size);
                        
                        m++;
                    }
                }
                class_section_count = m;
            }
            else
            {
                SCI_TRACE_LOW("%s: code->valueint = %d", __FUNCTION__, code->valueint);
                class_section_count = -1;
            }
            cJSON_Delete(root);
        }
    }
    else
    {
        class_section_count = -2;
    }
    SCI_TRACE_LOW("%s: class_section_count = %d", __FUNCTION__, class_section_count);
    if(MMK_IsFocusWin(ZMT_CLASS_SECTION_WIN_ID))
    {
        MMK_SendMsg(ZMT_CLASS_SECTION_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

PUBLIC void Class_ReuestSectionInfo(char * course_id)
{
    char url[200] = {0};
    uint8 * token = NULL;
#ifdef WIN32
    uint32 data_size;
    char * data_buf = zmt_file_data_read(CLASS_SYN_COURSE_FILE_PATH, &data_size);
    if(data_buf != NULL && data_size > 0)
    {
        Class_ParseSectionInfo(1, data_buf, data_size, 0);
        SCI_FREE(data_buf);
    }
#else
    token = Class_MakeCourseDetailToken(course_id);
    SCI_TRACE_LOW("%s: token = %s", __FUNCTION__,token);
    sprintf(url, CLASS_SYN_BASE_GETCOURSEDETAIL_API, course_id, token);
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__,url);
    SCI_FREE(token);
    MMIZDT_HTTP_AppSend(TRUE, CLASS_SYN_BASE_API, url, strlen(url), 1000, 0, 0, 0, 0, 0, Class_ParseSectionInfo);
#endif
}

LOCAL uint8 * Class_MakeApiProxyToken(char * section_id, char * api_param)
{
    uint32 times = 0;
    char sign[150] = {0};
    uint8 * md5_str = NULL;
    uint8 * token = NULL;

    times = MMIAPICOM_GetCurTime() + CLASS_SYN_TIMES_DIFF_FROM_1978_TO_1980;
    
    sprintf(sign, "%s%s%s%s%d%d%s%s", 
        CLASS_SYN_API_APP, CLASS_SYN_API_METHOD, api_param,  CLASS_SYN_APP_ID,
        CLASS_SYN_IS_TEACHER, times, CLASS_SYN_UID, CLASS_SYN_SECRET
    );
    SCI_TRACE_LOW("%s: sign = %s", __FUNCTION__, sign);
    
    md5_str = Class_MakeMd5Str(sign);
    SCI_TRACE_LOW("%s: md5_str = %s", __FUNCTION__, md5_str);

    token = SCI_ALLOC_APPZ(100);
    memset(token, 0, 100);
    sprintf(token, CLASS_SYN_BASE_TOKEN, CLASS_SYN_APP_ID, times, md5_str);

    SCI_FREE(md5_str);
    return token;
}

PUBLIC void Class_RealeaseReadInfo(void)
{
    uint8 i = 0;
    for(i = 0;i < CLASS_SYN_READ_NUM_MAX;i++)
    {
        if(class_read_info[i] != NULL){
            if(class_read_info[i]->text != NULL){
                SCI_FREE(class_read_info[i]->text);
                class_read_info[i]->text = NULL;
            }
            if(class_read_info[i]->audio_url != NULL){
                SCI_FREE(class_read_info[i]->audio_url);
                class_read_info[i]->audio_url = NULL;
            }
            if(class_read_info[i]->audio_data != NULL){
                SCI_FREE(class_read_info[i]->audio_data);
                class_read_info[i]->audio_data = NULL;
            }
            SCI_FREE(class_read_info[i]);
            class_read_info[i] = NULL;
        }
    }
}

PUBLIC void Class_DeleteReadAudioFile(void)
{
    uint8 i = 0;
    char file_path[30] = {0};
    SCI_TRACE_LOW("%s: class_read_count = %d", __FUNCTION__, class_read_count);
    for(i = 0;i < class_read_count && i < CLASS_SYN_READ_NUM_MAX;i++)
    {
        memset(&file_path, 0, 30);
        sprintf(file_path, CLASS_SYN_SENTECT_AUDIO_PATH, i);
        zmt_file_delete(file_path);
    }
}

LOCAL void Class_AddReadInfo(CLASS_READ_INFO_T * read_info, cJSON* text, cJSON * audio, cJSON * duration)
{
    uint16 size = 0;
    if(text != NULL && text->type != cJSON_NULL && text->valuestring != NULL){
        size = strlen(text->valuestring);
        if(size > 0){
            read_info->text = (char *)SCI_ALLOC_APP(size + 1);
            memset(read_info->text, 0, size + 1);
            SCI_MEMCPY(read_info->text, text->valuestring, size);
        }else{
            read_info->text = NULL;
        }
    }else{
        read_info->text = NULL;
    }

    if(audio != NULL && audio->type != cJSON_NULL && audio->valuestring != NULL){
        size = strlen(audio->valuestring);
        if(size > 0){
            read_info->audio_url = (char *)SCI_ALLOC_APP(size + 1);
            memset(read_info->audio_url, 0, size + 1);
            SCI_MEMCPY(read_info->audio_url, audio->valuestring, size);
            read_info->audio_len = 0;
        }else{
            read_info->audio_url = NULL;
            read_info->audio_len = -1;
        }
    }else{
        read_info->audio_url = NULL;
        read_info->audio_len = -1;
    }
    read_info->audio_data = NULL;

    if(duration != NULL && duration->type != cJSON_NULL){
       read_info->audio_duration = cJSON_GetObjectDouble(duration);
       //SCI_TRACE_LOW("%s: audio_duration = %f", __FUNCTION__, read_info->audio_duration);
    }
}

LOCAL void Class_ParseReadInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        uint16 size = 0;
        cJSON *root = cJSON_Parse(pRcv);
        if(root != NULL && root->type != cJSON_NULL)
        {
            cJSON * code = cJSON_GetObjectItem(root, "code");
            if(code->valueint == 10000)
            {
                cJSON * data = cJSON_GetObjectItem(root, "data");
                cJSON * gateresult = cJSON_GetObjectItem(data, "gateresult");
                cJSON * result = cJSON_GetObjectItem(gateresult, "result");
                cJSON * result_data = cJSON_GetObjectItem(result, "data");
                cJSON * paragraphs = cJSON_GetObjectItem(result_data, "paragraphs");
                for(i = 0;i < cJSON_GetArraySize(paragraphs) && i < CLASS_SYN_READ_NUM_MAX;i++)
                {
                    cJSON * paragraphs_item = cJSON_GetArrayItem(paragraphs, i);
                    cJSON * text = cJSON_GetObjectItem(paragraphs_item, "text");
                    cJSON * audioUrl = cJSON_GetObjectItem(paragraphs_item, "audioUrl");
                    cJSON * audioDuration = cJSON_GetObjectItem(paragraphs_item, "audioDuration");

                    if(class_read_info[i] != NULL){
                        SCI_FREE(class_read_info[i]);
                        class_read_info[i] = NULL;
                    }
                    class_read_info[i] = SCI_ALLOC_APPZ(sizeof(CLASS_READ_INFO_T));
                    memset(class_read_info[i], 0, sizeof(CLASS_READ_INFO_T));

                    Class_AddReadInfo(class_read_info[i], text, audioUrl, audioDuration);
                }
                class_read_count = i;
            }
            else
            {
                SCI_TRACE_LOW("%s: code->valueint = %d", __FUNCTION__, code->valueint);
                class_read_count = -1;
            }
            cJSON_Delete(root);
        }
    }
    else
    {
        class_read_count = -2;
    }
    SCI_TRACE_LOW("%s: class_read_count = %d", __FUNCTION__, class_read_count);
    if(MMK_IsFocusWin(ZMT_CLASS_READ_WIN_ID))
    {
        #ifndef WIN32
        MMK_SendMsg(ZMT_CLASS_READ_WIN_ID, MSG_FULL_PAINT, PNULL);
        #endif
    }
}

PUBLIC void Class_ReuestReadInfo(char * section_id)
{
    char url[300] = {0};
    char api_param[60] = {0};
    uint8 * token = NULL;
#ifdef WIN32
    char file_path[30] = {0};
    uint32 data_size;
    char * data_buf = NULL;
    sprintf(file_path, CLASS_SYN_SECTION_FILE_PATH, class_sync_info.subject_type);
    data_buf = zmt_file_data_read(file_path, &data_size);
    if(data_buf != NULL && data_size > 0)
    {
        Class_ParseReadInfo(1, data_buf, data_size, 0);
        SCI_FREE(data_buf);
    }
#else
    sprintf(api_param, CLASS_SYN_BASE_API_PARAM, section_id);
    token = Class_MakeApiProxyToken(section_id, api_param);
    SCI_TRACE_LOW("%s: token = %s", __FUNCTION__,token);
    sprintf(url, CLASS_SYN_BASE_APIPROXY_API, 
        CLASS_SYN_UID, CLASS_SYN_IS_TEACHER, CLASS_SYN_API_APP, CLASS_SYN_API_METHOD,
        api_param, token
    );
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__,url);
    SCI_FREE(token);
    MMIZDT_HTTP_AppSend(TRUE, CLASS_SYN_BASE_API, url, strlen(url), 1000, 0, 0, 0, 0, 0, Class_ParseReadInfo);
#endif
}

LOCAL void Class_DownloadNextMp3(BOOLEAN is_download_file)
{
    class_cur_down_idx++;
    if(class_cur_down_idx >= class_read_count){
        SCI_TRACE_LOW("%s: cur_idx = %d, read_count = %d ,download complete", __FUNCTION__, class_cur_down_idx, class_read_count);
        class_download_next_now = FALSE;
        have_new_quest = FALSE;
        class_cur_down_idx = 0;
        return;
    }
    if(class_read_info[class_cur_down_idx] == NULL){
        SCI_TRACE_LOW("%s: class_read_info empty!!, cur_idx = %d", __FUNCTION__, class_cur_down_idx);
        class_download_next_now = FALSE;
        return;
    }
    if(class_read_info[class_cur_down_idx]->audio_len > 0){
        SCI_TRACE_LOW("%s: cur_idx = %d ,data exist go next", __FUNCTION__,class_cur_down_idx);
        Class_DownloadNextMp3(is_download_file);
        return;
    }
    SCI_TRACE_LOW("%s: download action, cur_idx = %d", __FUNCTION__, class_cur_down_idx);
    class_download_next_now = TRUE;
    if(is_download_file){
        char file_path[30] = {0};
        sprintf(file_path, CLASS_SYN_SENTECT_AUDIO_PATH, class_cur_down_idx);
        MMIZDT_HTTP_AppSend(TRUE,class_read_info[class_cur_down_idx]->audio_url,PNULL,0,101,0,1,20000,file_path,strlen(file_path),Class_ParseMp3FileDownload);
    }else{
        MMIZDT_HTTP_AppSend(TRUE, class_read_info[class_cur_down_idx]->audio_url, PNULL, 0, 1000, 0, 0, 20000, 0, 0, Class_ParseMp3Response);
    }
}

LOCAL void Class_ParseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len = %d", __FUNCTION__, is_ok, Rcv_len);
    if(have_new_quest){
        have_new_quest = FALSE;
        class_cur_down_idx--;
        Class_DownloadNextMp3(FALSE);
        SCI_TRACE_LOW("%s: have_new_quest, reback cur_idx = %d", __FUNCTION__, class_cur_down_idx);
        return;
    }
    if(class_read_info[class_cur_down_idx] == NULL)
    {
        SCI_TRACE_LOW("%s: class_read_info empty!!, cur_idx = %d", __FUNCTION__, class_cur_down_idx);
        return;
    }
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        SCI_TRACE_LOW("%s: cur_idx = %d", __FUNCTION__, class_cur_down_idx);

        class_read_info[class_cur_down_idx]->audio_len = Rcv_len;
        if(class_read_info[class_cur_down_idx]->audio_data != NULL){
            SCI_FREE(class_read_info[class_cur_down_idx]->audio_data);
            class_read_info[class_cur_down_idx]->audio_data = NULL;
        }
        class_read_info[class_cur_down_idx]->audio_data = SCI_ALLOC_APPZ(Rcv_len);
        SCI_MEMSET(class_read_info[class_cur_down_idx]->audio_data, 0, Rcv_len);
        SCI_MEMCPY(class_read_info[class_cur_down_idx]->audio_data, pRcv, Rcv_len);

        SCI_TRACE_LOW("%s: class_download_next_now = %d", __FUNCTION__, class_download_next_now);
        if(!class_download_next_now){
            Class_PlayAudioMp3();
        }
    }
    else
    {
        class_read_info[class_cur_down_idx]->audio_len = -2;
    }
    Class_DownloadNextMp3(FALSE);
}

LOCAL void Class_ParseMp3FileDownload(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d", __FUNCTION__, is_ok);
    if(have_new_quest){
        have_new_quest = FALSE;
        class_cur_down_idx--;//为了Class_DownloadNextMp3里的++能正好相抵消
        Class_DownloadNextMp3(TRUE);
        SCI_TRACE_LOW("%s: have_new_quest, reback cur_idx = %d", __FUNCTION__, class_cur_down_idx);
        return;
    }
    if(class_read_info[class_cur_down_idx] == NULL)
    {
        SCI_TRACE_LOW("%s: class_read_info empty!!, cur_idx = %d", __FUNCTION__, class_cur_down_idx);
        return;
    }
    if(is_ok)
    {
        if(!MMK_IsOpenWin(ZMT_CLASS_READ_WIN_ID))
        {
            char file_path[30] = {0};
            sprintf(file_path, CLASS_SYN_SENTECT_AUDIO_PATH, class_cur_down_idx);
            if(zmt_file_exist(file_path)){
                zmt_file_delete(file_path);
            }
            SCI_TRACE_LOW("%s: CLASS_READ_WIN is not exist!!", __FUNCTION__);
            return;
        }
        class_read_info[class_cur_down_idx]->audio_len = 2;
        
        SCI_TRACE_LOW("%s: class_download_next_now = %d", __FUNCTION__, class_download_next_now);
        if(!class_download_next_now){
            Class_PlayAudioMp3();
        }
    }
    else
    {
        char file_path[30] = {0};
        sprintf(file_path, CLASS_SYN_SENTECT_AUDIO_PATH, class_cur_down_idx);
        if(zmt_file_exist(file_path)){
            zmt_file_delete(file_path);
        }
        SCI_TRACE_LOW("%s: err_id = %d", __FUNCTION__, err_id);//HTTP_ERROR_FILE_NO_SPACE
        class_read_info[class_cur_down_idx]->audio_len = -2;
    }
    Class_DownloadNextMp3(TRUE);
}

PUBLIC void Class_RequestMp3Data(char * url, uint8 idx, BOOLEAN is_download_file)
{
    if(url != NULL)
    {
        SCI_TRACE_LOW("%s: idx = %d, class_download_next_now = %d", __FUNCTION__, idx, class_download_next_now);
        if(class_download_next_now){
            have_new_quest = TRUE;
        }
        class_cur_down_idx = idx;
        if(is_download_file){
            char file_path[30] = {0};
            sprintf(file_path, CLASS_SYN_SENTECT_AUDIO_PATH, class_cur_down_idx);
            MMIZDT_HTTP_AppSend(TRUE,url,PNULL,0,101,0,1,20000,file_path,strlen(file_path),Class_ParseMp3FileDownload);
        }else{
            MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 20000, 0, 0, Class_ParseMp3Response);
        }
    }
    else
    {
        SCI_TRACE_LOW("%s: idx = %d, url empty!!", __FUNCTION__, idx);
    }
}


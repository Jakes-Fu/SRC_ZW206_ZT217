/*****************************************************************************
** File Name:      zmt_gpt.h                                          *
** Author:           fys                                                        *
** Date:           2024/05/16                                                 *
** Copyright:       *
** Description:                       *
******************************************************************************/

#include "sci_types.h"
#include "mmk_type.h"
#include "os_api.h"

#ifdef WIN32
#define ZMT_GPT_USE_FOR_TEST 1
#else
#define ZMT_GPT_USE_FOR_TEST 0
#endif

#define ZMT_GPT_USE_SELF_API 1

#define ZMT_GPT_TOPIC_LIST_PATH "D:\\topic_list.json"
#define ZMT_GPT_KOUYU_TALK_PATH "D:\\kouyu_talk.json"
#define ZMT_GPT_SELF_KOUYU_TALK_PATH "D:\\self_kouyu_talk.json"
#define ZMT_GPT_ZUOWEN_TALK_PATH "D:\\zuowen_talk.json"
#define ZMT_GPT_SELF_ZUOWEN_TALK_PATH "D:\\self_zuowen_talk.json"
#define ZMT_GPT_RECORD_FILE_C "D:\\zmt_gpt_record.pcm"
#define ZMT_GPT_RECORD_FILE_L L"D:\\zmt_gpt_record.pcm"

#define ZMT_GPT_LINE_WIDTH MMI_MAINSCREEN_WIDTH/6
#define ZMT_GPT_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/10
#define ZMT_GPT_LIST_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/30

#define ZMT_GPT_TIMES_DIFF_FROM_1978_TO_1980  ((365*10+2)*24*3600) - (8 * 60 * 60)

#define GPT_PCM_HEAD_SIZE 6
#define GPT_MAX_RECORD_DURATION 10
#define GPT_MAX_RECORD_SIZE GPT_MAX_RECORD_DURATION * 16000 + GPT_PCM_HEAD_SIZE

#define GPT_HTTP_SELF_API_BASE_PATH "http://106.13.7.202:8080"
#define GPT_HTTP_SELF_API_KOUYU_PATH "/aichat/enchat"
#define GPT_HTTP_SELF_API_ZUOWEN_PATH "/aichat/artchat"

#define GPT_HTTP_BAIDU_TXT_PARSE "http://tsn.baidu.com/text2audio"
#define GPT_HTTP_BAIDU_VOICE_PARSE "http://vop.baidu.com/server_api"
#define GPT_HTTP_BAIDU_ACCESS_TOKEN_PATH "https://aip.baidubce.com/oauth/2.0/token"
#define GPT_HTTP_BAIDU_APPID "74270737"//"44742782"
#define GPT_HTTP_BAIDU_GRANT_TYPE "client_credentials"
#define GPT_HTTP_BAIDU_API_KEY "aSqIhCmjhxojptJqvtm9Mo44"//"3T6V5B5CFBGS49AiVz1mttRg"
#define GPT_HTTP_BAIDU_SECRET_KEY "8oIimiIorgXNCOJEIZi28CYiQ0iYNZCP"//"7G5piArt8Uj1hFb1ekMCv0mAkpVNz0Up"
#define GPT_HTTP_BAIDU_TXT_HTML_PATH "tex=%s&lan=zh&cuid=%s&ctp=1&aue=3&tok=%s"

#define GPT_HTTP_URL_KOUYU "https://api-gpt.readboy.com"//"https://api-gpt-test.readboy.com"
#define GPT_HTTP_URL_KOUYU_MODEL_LIST "/v1/pen/list"
#define GPT_HTTP_URL_KOUYU_SEND_CHAT "/v1/pen/chat"
#define GPT_HTTP_URL_KOUYU_UA "//A7/gpt-smallos//"
#define GPT_HTTP_URL_KOUYU_PLATFORM "smallos"
#define GPT_HTTP_URL_KOUYU_APPID "gpt-smallos"
#define GPT_HTTP_URL_KOUYU_APPSEC "6890c5fa3f7750121006c917979b506d"

#define GPT_ZUOWEN_DOWNLOAD_PATH "D:/gpt_audio.mp3"
#define GPT_KOUYU_DOWNLOAD_PATH "D:/gpt_audio_%d.mp3"

#define GPT_KOUYU_TALK_STR_MAX_LEN 100
#define GPT_KOUYU_TOPIC_MAX_SIZE 20
#define GPT_KOUYU_TALK_MAX_SIZE 20
#define GPT_ZUOWEN_TALK_MAX_SIZE 10

extern char * gpt_baidu_access_token;

typedef struct
{
    char talk[GPT_KOUYU_TALK_STR_MAX_LEN];
    char talk_chn[GPT_KOUYU_TALK_STR_MAX_LEN];
}gpt_kouyu_talk_t;

typedef struct
{
    uint8 size;
    char field[GPT_KOUYU_TOPIC_MAX_SIZE];
    gpt_kouyu_talk_t talk_list[GPT_KOUYU_TALK_MAX_SIZE];
}gpt_kouyu_info_t;

typedef struct
{
    BOOLEAN is_user;
    char * str;
}gpt_talk_info_t;

typedef enum {
    GPT_RECORD_TYPE_NONE = 0,
    GPT_RECORD_TYPE_RECORDING,
    GPT_RECORD_TYPE_VOICE_LOADING,
    GPT_RECORD_TYPE_TEXT_LOADING,
    GPT_RECORD_TYPE_SUCCESS,
    GPT_RECORD_TYPE_FAIL,
    GPT_RECORD_TYPE_ERROR,
}ZMT_GPT_RECORD_TYPE_E;

//PUBLIC int corepush_base64_encode( unsigned char *dst, unsigned int *dlen,const unsigned char *src, unsigned int slen );
PUBLIC void gpt_get_baidu_access_token(void);
PUBLIC void ZmtGpt_SendString(char * save_path, char * string);
PUBLIC void ZmtGpt_SendRecord(uint32 lan_type, char * record_buf, uint32 record_size);
PUBLIC void ZmtGpt_SendTxt(uint32 app_type, char * send_txt, char * sys_param, char * field, int post_type);
PUBLIC void ZmtGptKouYuTalk_RecvSelfResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void ZmtGptKouYuTalk_RecvResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void ZmtGptKouYuTalk_RecAiTextResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void ZmtGptKouYuTalk_RecAiVoiceResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void ZmtGptZuoWen_RecvSelfResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void ZmtGptZuoWen_RecvResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void ZmtGptZuoWen_RecAiTextResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);

PUBLIC void MMIZMT_CreateZmtGptWin(void);
PUBLIC void MMIZMT_CreateZmtGptKouYuTopicWin(void);
PUBLIC void MMIZMT_CreateZmtGptZuoWenWin(void);

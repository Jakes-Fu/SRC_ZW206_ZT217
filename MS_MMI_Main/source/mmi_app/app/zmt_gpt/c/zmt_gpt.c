/*****************************************************************************
** File Name:      zmt_gpt.c                                           *
** Author:           fys                                                        *
** Date:           2024/05/16                                                *
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
#include "mbedtls/md5.h"

#define ZMT_GPT_MAX_ITEM 2

#define BAIDU_ACCESS_TOKEN_TIMES_MAX 29*24*60*60

GUI_RECT_T zmt_gpt_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};//´°¿Ú
GUI_RECT_T zmt_gpt_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, ZMT_GPT_LINE_HIGHT};//¶¥²¿
GUI_RECT_T zmt_gpt_list_rect = {0, ZMT_GPT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH , MMI_MAINSCREEN_HEIGHT};

char * gpt_baidu_access_token = NULL;
uint32 baidu_access_token_times = 0;
char gpt_record_encry_buf[GPT_MAX_RECORD_SIZE] = {0};

LOCAL uint8 * GPT_MakeMd5Str(char * sign)
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

PUBLIC void GPT_makeGptTokenUrl(uint8 type, char * url, int page, int size)
{
    char imei[20] = {0};
    char ua[200] = {0};
    char * sn = NULL;
    long t = 0; 
    char sn_str[200] = {0};

    //app_adp_get_imei_req(&imei);
    sprintf(imei, "%s", "869937060002261");
    sprintf(ua, "%s%s", GPT_HTTP_URL_KOUYU_UA, imei);
    t = MMIAPICOM_GetCurTime() + ZMT_GPT_TIMES_DIFF_FROM_1978_TO_1980;
    sprintf(sn_str, "%s%s%ld", ua, GPT_HTTP_URL_KOUYU_APPSEC, t);
    sn = GPT_MakeMd5Str(sn_str);
    if(type == 0){
        sprintf(url, "%s%s?platform=%s&page=%d&size=%d&ua=%s&t=%ld&sn=%s", 
            GPT_HTTP_URL_KOUYU, GPT_HTTP_URL_KOUYU_MODEL_LIST, GPT_HTTP_URL_KOUYU_PLATFORM, page, size, ua, t, sn);
    }else{
        sprintf(url, "%s%s?&ua=%s&t=%ld&sn=%s",
            GPT_HTTP_URL_KOUYU, GPT_HTTP_URL_KOUYU_SEND_CHAT, ua, t, sn);
    }
    SCI_FREE(sn);
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
}

PUBLIC void ZmtGpt_SendString(char * save_path, char * string)
{
    char data[2048] = {0};
    char imei[20] = {0};
    char url[50] = {0};
#if ZMT_GPT_USE_SELF_API != 0
    {
        char * buf;
        cJSON * root;
        cJSON * text;
        cJSON * sid;
        cJSON * speed;
        cJSON * isEn;

        root = cJSON_CreateObject();

        text = cJSON_CreateString(string);
        cJSON_AddItemToObject(root, "text", text);

        sid = cJSON_CreateNumber(66);
        cJSON_AddItemToObject(root, "sid", sid);

        speed = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(root, "speed", speed);

        isEn = cJSON_CreateTrue();
        cJSON_AddItemToObject(root, "isEn", isEn);

        buf = cJSON_PrintUnformatted(root);
        SCI_MEMCPY(data, buf, strlen(buf));
        SCI_FREE(buf);
        
        sprintf(url, "%s%s", GPT_HTTP_SELF_API_BASE_PATH, GPT_HTTP_SELF_API_TXT2VOICE_PATH);
    }
#else
    sprintf(imei, "%s", "869937060002261");
    if(gpt_baidu_access_token){
        sprintf(data, GPT_HTTP_BAIDU_TXT_HTML_PATH, string, imei, gpt_baidu_access_token);
    }else{
        SCI_TRACE_LOW("%s: token error", __FUNCTION__);
        gpt_get_baidu_access_token();
        return;
    }
    sprintf(url, "%s", GPT_HTTP_BAIDU_TXT_PARSE);
#endif
    SCI_TRACE_LOW("%s: data = %s", __FUNCTION__, data);
    MMIZDT_HTTP_AppSend(FALSE, url, data, strlen(data), 30*1000, 0, 0, 0, 0, 0, ZmtGptKouYuTalk_RecAiVoiceResultCb);
}

PUBLIC void ZmtGpt_SendSelfRecord(uint8 req_type, char * record_buf, uint32 record_size, ZMTTCPRCVHANDLER rec_handle)
{
#if ZMT_GPT_USE_TCP_POST_VOICE == 0
    char url[50] = {0};
    char * out;
    cJSON * root;
    cJSON * type;
    cJSON * audio;
    uint32 record_rel_len = 0;
    uint32 record_encry_len = GPT_MAX_RECORD_SIZE;

    root = cJSON_CreateObject();
    type = cJSON_CreateString("amr");
    cJSON_AddItemToObject(root, "type", type);
    memset(&gpt_record_encry_buf, 0, record_encry_len);
 #ifndef WIN32
    mbedtls_base64_encode(&gpt_record_encry_buf, record_encry_len, &record_rel_len, record_buf, record_size);
 #endif
    gpt_record_encry_buf[record_rel_len] = 0;
    audio = cJSON_CreateString(gpt_record_encry_buf);
    cJSON_AddItemToObject(root, "audio", audio);
    out = cJSON_PrintUnformatted(root);
    SCI_TRACE_LOW("%s: data_buf = %s", __FUNCTION__, out);
    sprintf(url, "%s%s", GPT_HTTP_SELF_API_BASE_PATH, GPT_HTTP_SELF_API_VOICE2TXT_JSON_PATH);
    if(req_type == 0){//req_type = 0 zuowen,req_type = 1 kouyu
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out),10000, 0, 0, 0, 0, 0, ZmtGptZuoWen_RecAiSelfTextResultCb);
    }else{
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out), 10000, 0, 0, 0, 0, 0, ZmtGptKouYuTalk_RecAiSelfTextResultCb);
    }
    cJSON_Delete(root);
    SCI_FREE(out);
#else
    #if ZMT_GPT_USE_FOR_TEST != 0
    
    #else
    BOOLEAN result = TRUE;
    ZMT_DATA_CONTENT_T * chatdata = NULL;
    char * data_buf = NULL;
    uint32 data_size = 0;
    chatdata=(ZMT_DATA_CONTENT_T*)SCI_ALLOC_APPZ(sizeof(ZMT_DATA_CONTENT_T));
    SCI_MEMSET(chatdata, 0, sizeof(ZMT_DATA_CONTENT_T));
    chatdata->data_type = ZMT_DATA_AUDIO;
    chatdata->data = record_buf;
    chatdata->data_len = record_size;
    chatdata->rec_handle = rec_handle;
    chatdata->time_stamp = MMIAPICOM_GetCurTime();
    result = ZMT_Net_TCPSendFile(chatdata);
    SCI_FREE(chatdata);
    SCI_TRACE_LOW("%s: result = %d", __FUNCTION__, result);
    #endif
#endif
}

PUBLIC void ZmtGpt_SendRecord(uint32 lan_type, char * record_buf, uint32 record_size)
{
    char imei[20] = {0};
    cJSON * root;
    cJSON * format;
    cJSON * rate;
    cJSON * dev_pid;
    cJSON * channel;
    cJSON * cuid;
    cJSON * len;
    char * out;
    char url[50] = {0};

    root = cJSON_CreateObject();
    format = cJSON_CreateString("pcm");
    cJSON_AddItemToObject(root, "format", format);
    rate = cJSON_CreateNumber(8000);
    cJSON_AddItemToObject(root, "rate", rate);
    dev_pid = cJSON_CreateNumber(lan_type);//1537 CHN,1737 ENG
    cJSON_AddItemToObject(root, "dev_pid", dev_pid);
    channel = cJSON_CreateNumber(1);
    cJSON_AddItemToObject(root, "channel", channel);
    if(gpt_baidu_access_token){
        cJSON * token = cJSON_CreateString(gpt_baidu_access_token);
        cJSON_AddItemToObject(root, "token", token);
    }else{
        cJSON_Delete(root);
        SCI_TRACE_LOW("%s: token error", __FUNCTION__);
        gpt_get_baidu_access_token();
        return;
    }
    
    //app_adp_get_imei_req(&imei);
    sprintf(imei, "%s", "869937060002261");
    cuid = cJSON_CreateString(imei);
    cJSON_AddItemToObject(root, "cuid", cuid);
    len = cJSON_CreateNumber(record_size);
    cJSON_AddItemToObject(root, "len", len);
    SCI_TRACE_LOW("%s: strlen(record_buf) = %d", __FUNCTION__, strlen(record_buf));
    if(record_buf)
    {
        cJSON * speech;
        uint32 record_rel_len = 0;
        uint32 record_encry_len = GPT_MAX_RECORD_SIZE;
        memset(&gpt_record_encry_buf, 0, record_encry_len);
 #ifndef WIN32
        mbedtls_base64_encode(&gpt_record_encry_buf, record_encry_len, &record_rel_len, record_buf, record_size);
 #endif
        gpt_record_encry_buf[record_rel_len] = 0;
        speech = cJSON_CreateString(gpt_record_encry_buf);
        cJSON_AddItemToObject(root, "speech", speech);
    }
    else
    {
        cJSON_Delete(root);
        return;
    }
    out = cJSON_PrintUnformatted(root);

    sprintf(url, "%s", GPT_HTTP_BAIDU_VOICE_PARSE);
    if(lan_type == 1537){
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out), 6000, 0, 0, 0, 0, 0, ZmtGptZuoWen_RecAiTextResultCb);
    }else{
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out), 6000, 0, 0, 0, 0, 0, ZmtGptKouYuTalk_RecAiTextResultCb);
    }

    free(out);
    cJSON_Delete(root);
}

PUBLIC void ZmtGpt_SendSelfTxt(uint8 lan_type, char * send_txt, BOOLEAN is_new_talk)
{
    cJSON * root;
    cJSON * uid;
    cJSON * question;
    cJSON * proceed;
    char * out = NULL;
    char imei[20] = {0};
    char url[1024] = {0};

    root = cJSON_CreateObject();
    sprintf(imei, "%s", "869937060002261");
    uid = cJSON_CreateString(imei);
    cJSON_AddItemToObject(root, "imei", uid);
    question = cJSON_CreateString(send_txt);
    cJSON_AddItemToObject(root, "question", question);
    if(is_new_talk){
        proceed = cJSON_CreateFalse();
    }else{
        proceed = cJSON_CreateTrue();
    }
    cJSON_AddItemToObject(root, "proceed", proceed);
    out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    SCI_TRACE_LOW("%s: out = %s", __FUNCTION__, out);

    if(lan_type == 0)
    {
        sprintf(url, "%s%s", GPT_HTTP_SELF_API_BASE_PATH, GPT_HTTP_SELF_API_KOUYU_PATH);
#if ZMT_GPT_USE_FOR_TEST != 0
        {
            uint8 * buf = PNULL;
            uint32 len = 0;
            ZmtGptKouYuTalk_RecvSelfResultCb(0, buf, len, 0); 
        }
 #else
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out), 10*1000, 0, 0, 0, 0, 0, ZmtGptKouYuTalk_RecvSelfResultCb);
 #endif        
    }
    else
    {
        sprintf(url, "%s%s", GPT_HTTP_SELF_API_BASE_PATH, GPT_HTTP_SELF_API_ZUOWEN_PATH);
 #if ZMT_GPT_USE_FOR_TEST != 0
        {
            uint8 * buf = PNULL;
            uint32 len = 0;
            ZmtGptZuoWen_RecvSelfResultCb(0, buf, len, 0); 
        }
 #else
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out), 10*1000, 0, 0, 0, 0, 0, ZmtGptZuoWen_RecvSelfResultCb);
 #endif         
    }
    SCI_FREE(out);
}

PUBLIC void ZmtGpt_SendTxt(uint32 app_type, char * send_txt, char * sys_param, char * field, int post_type)
{
    cJSON * root;
    cJSON * uid;
    cJSON * module_id;
    cJSON * stream;
    char * out = NULL;
    char url[1024] = {0};
    
    root = cJSON_CreateObject();
    uid = cJSON_CreateNumber(0);
    cJSON_AddItemToObject(root, "uid", uid);
    module_id = cJSON_CreateNumber(app_type);//146 zuowen;147 kouyu
    cJSON_AddItemToObject(root, "module_id", module_id);
    if(send_txt){
        cJSON * query = cJSON_CreateString(send_txt);
        cJSON_AddItemToObject(root, "query", query);
    }else{
        SCI_TRACE_LOW("%s: gpt_record_txt error!", __FUNCTION__);
        cJSON_Delete(root);
        return;
    }
    if(sys_param){
        cJSON * sys_params = cJSON_CreateObject();
        cJSON * topic = cJSON_CreateString(sys_param);
        cJSON_AddItemToObject(sys_params, field, topic);
        cJSON_AddItemToObject(root, "sys_params", sys_params);
    }
    stream = cJSON_CreateFalse();
    cJSON_AddItemToObject(root, "stream", stream);

    out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    SCI_TRACE_LOW("%s: strlen(out) = %d", __FUNCTION__, strlen(out));
    SCI_TRACE_LOW("%s: out = %s", __FUNCTION__, out);
	
    GPT_makeGptTokenUrl(1, &url, 1, 10);
    if(post_type == 0)
    {
#if ZMT_GPT_USE_FOR_TEST != 0
        {
            uint8 * buf = PNULL;
            uint32 len = 0;
            ZmtGptKouYuTalk_RecvResultCb(0, buf, len, 0); 
        }
 #else
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out), 1000, 0, 0, 0, 0, 0, ZmtGptKouYuTalk_RecvResultCb);
 #endif
    }
    else
    {
#if ZMT_GPT_USE_FOR_TEST != 0
        {
            uint8 * buf = PNULL;
            uint32 len = 0;
            ZmtGptZuoWen_RecvResultCb(0, buf, len, 0); 
        }
 #else 
        MMIZDT_HTTP_AppSend(FALSE, url, out, strlen(out), 1000, 0, 0, 0, 0, 0, ZmtGptZuoWen_RecvResultCb);
 #endif
    }
    SCI_FREE(out);
}

//////////////////////////////////////////////////////////////////////////////
PUBLIC void ZmtGpt_DisplayTitle(MMI_WIN_ID_T win_id, MMI_STRING_T text_string, GUI_RECT_T title_rect, GUI_FONT_T font)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_WORDBREAK;
    GUISTR_STYLE_T text_style = {0};
    GUI_RECT_T win_rect = zmt_gpt_win_rect;
    GUI_RECT_T text_rect = title_rect;

    GUI_FillRect(&lcd_dev_info, win_rect, GPT_WIN_BG_COLOR);
    GUI_FillRect(&lcd_dev_info, title_rect, GPT_TITLE_BG_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = font;
    text_style.font_color = MMI_WHITE_COLOR;

    text_rect.left += 10;
    text_rect.right -= 10;
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &text_rect,
        &text_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
}

PUBLIC void ZmtGpt_InitListbox(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, GUI_RECT_T list_rect, uint16 max_item)
{
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};

    if(!MMK_GetCtrlHandleByWin(win_id, ctrl_id)){
        list_init.both_rect.v_rect = list_rect;
        list_init.type = GUILIST_TEXTLIST_E;
        GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);
        MMK_SetAtvCtrl(win_id, ctrl_id);
        GUILIST_SetMaxItem(ctrl_id, max_item, FALSE);

        GUILIST_SetListState(ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_AUTO_SCROLL, FALSE);
        GUILIST_SetListState(ctrl_id, GUILIST_STATE_EFFECT_STR,TRUE);
        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);
        GUILIST_SetBgColor(ctrl_id, GPT_WIN_BG_COLOR);
    }else{
        GUILIST_RemoveAllItems(ctrl_id);
    }
}

LOCAL void ZmtGpt_DisplayList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 index = 0;
    uint8 num = 0;
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_IMAGE_ID_T img_id[ZMT_GPT_MAX_ITEM] = {IMG_ZMT_GPT_KOUYU, IMG_ZMT_GPT_ZUOWEN};
    MMI_TEXT_ID_T text_id[ZMT_GPT_MAX_ITEM] = {ZMT_CHAT_GPT_KOUYU, ZMT_CHAT_GPT_ZUOWEN};

    for(index = 0;index < ZMT_GPT_MAX_ITEM; index++)
    {
        item_t.item_style = GUIITEM_STYLE_GPT_ONE_ICON_AND_ONE_TEXT_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = img_id[index];

        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_ID;
        item_data.item_content[1].item_data.text_id = text_id[index];

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
}

LOCAL void ZmtGpt_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
#if ZMT_GPT_USE_SELF_API == 0
    gpt_get_baidu_access_token();
#endif
#if ZMT_GPT_USE_TCP_POST_VOICE != 0
    MMIZmt_AppInit();
    MMIZMT_Net_Init();				
    MMIZMT_Net_Open();
#endif

    ZmtGpt_InitListbox(win_id, ZMT_GPT_MAIN_LIST_CTRL_ID, zmt_gpt_list_rect, ZMT_GPT_MAX_ITEM);
    GUILIST_SetSlideState(ZMT_GPT_MAIN_LIST_CTRL_ID, FALSE);
    GUILIST_SetTextFont(ZMT_GPT_MAIN_LIST_CTRL_ID, DP_FONT_24, MMI_WHITE_COLOR);
    ZmtGpt_DisplayList(win_id, ZMT_GPT_MAIN_LIST_CTRL_ID);
}

LOCAL void ZmtGpt_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    
    MMIRES_GetText(ZMT_CHAT_GPT, win_id, &text_string);
    ZmtGpt_DisplayTitle(win_id, text_string, zmt_gpt_title_rect, DP_FONT_24);
}

LOCAL void ZmtGpt_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUILIST_GetCurItemIndex(ZMT_GPT_MAIN_LIST_CTRL_ID);
    if(cur_idx == 0){
        MMIZMT_CreateZmtGptKouYuTopicWin();
    }else{
        MMIZMT_CreateZmtGptZuoWenWin();
    }
}

LOCAL MMI_RESULT_E HandleZmtGptWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ZmtGpt_OPEN_WINDOW(win_id);
            }
            break;
        case MSG_FULL_PAINT:
            {             
                ZmtGpt_FULL_PAINT(win_id);               
            }
            break;
        case MSG_CTL_PENOK:
            {
                ZmtGpt_CTL_PENOK(win_id);
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
                ZMT_Net_TCP_Close();
            }
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_ZMT_GPT_WIN_TAB) = {
    WIN_ID(ZMT_GPT_MAIN_WIN_ID),
    WIN_FUNC((uint32)HandleZmtGptWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZMT_CreateZmtGptWin(void)
{
    if(MMK_IsOpenWin(ZMT_GPT_MAIN_WIN_ID))
    {
        MMK_CloseWin(ZMT_GPT_MAIN_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_ZMT_GPT_WIN_TAB, PNULL);
}

LOCAL void gpt_baidu_access_token_result_cb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        SCI_TIME_T sys_time= {0}; 
        SCI_DATE_T sys_date = {0};
        cJSON * root = cJSON_Parse(pRcv);
        cJSON * access_token = cJSON_GetObjectItem(root, "access_token");
        if(gpt_baidu_access_token){
            SCI_FREE(gpt_baidu_access_token);
            gpt_baidu_access_token = NULL;
        }
        gpt_baidu_access_token = SCI_ALLOC_APPZ(strlen(access_token->valuestring)+1);
        memset(gpt_baidu_access_token, 0, strlen(access_token->valuestring)+1);
        strcpy(gpt_baidu_access_token, access_token->valuestring);
        SCI_TRACE_LOW("%s: token = %s", __FUNCTION__, gpt_baidu_access_token);
        cJSON_Delete(root);
        TM_GetSysTime(&sys_time);
        TM_GetSysDate(&sys_date);
        baidu_access_token_times = MMIAPICOM_GetCurTime() + ZMT_GPT_TIMES_DIFF_FROM_1978_TO_1980;
        SCI_TRACE_LOW("%s: baidu_access_token_times = %ld", __FUNCTION__, baidu_access_token_times);
    }
}

PUBLIC void gpt_get_baidu_access_token(void)
{
    char url[200] = {0};
    uint32 cur_times = MMIAPICOM_GetCurTime() + ZMT_GPT_TIMES_DIFF_FROM_1978_TO_1980;
    SCI_TRACE_LOW("%s: cur_times = %d, baidu_access_token_tims = %d", __FUNCTION__, cur_times, baidu_access_token_times);
    /*if(gpt_baidu_access_token == NULL || 
        cur_times - baidu_access_token_times < BAIDU_ACCESS_TOKEN_TIMES_MAX)
    {
        return;
    }*/
    sprintf(url, "%s?grant_type=%s&client_id=%s&client_secret=%s", 
		GPT_HTTP_BAIDU_ACCESS_TOKEN_PATH, 
		GPT_HTTP_BAIDU_GRANT_TYPE, 
		GPT_HTTP_BAIDU_API_KEY,
		GPT_HTTP_BAIDU_SECRET_KEY);
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
    
    MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, gpt_baidu_access_token_result_cb);
}


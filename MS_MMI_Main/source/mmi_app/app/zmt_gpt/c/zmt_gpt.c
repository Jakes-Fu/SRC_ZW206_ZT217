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
#include "dsl_main_file.h"
#endif
#include "graphics_draw.h"
#include "img_dec_interface.h"
#ifndef WIN32
#include "mbedtls/base64.h"
#endif

#define ZMT_GPT_MAX_ITEM 2

#define BAIDU_ACCESS_TOKEN_TIMES_MAX 29*24*60*60

GUI_RECT_T zmt_gpt_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};//窗口
GUI_RECT_T zmt_gpt_title_rect = {0, 0, MMI_MAINSCREEN_WIDTH, ZMT_GPT_LINE_HIGHT};//顶部
GUI_RECT_T zmt_gpt_list_rect = {0, ZMT_GPT_LINE_HIGHT, MMI_MAINSCREEN_WIDTH , MMI_MAINSCREEN_HEIGHT};

char * gpt_baidu_access_token = NULL;
uint32 baidu_access_token_times = 0;
char gpt_record_encry_buf[GPT_MAX_RECORD_SIZE] = {0};

PUBLIC void ZMT_makeGptTokenUrl(uint8 type, char * url, int page, int size)
{
    char imei[20] = {0};
    char ua[200] = {0};
    char sn[200] = {0};
    long t = 0; 
    char sn_str[200] = {0};

    //app_adp_get_imei_req(&imei);
    sprintf(imei, "%s", "869937060002261");
    sprintf(ua, "%s%s", GPT_HTTP_URL_KOUYU_UA, imei);
    t = MMIAPICOM_GetCurTime() + ZMT_GPT_TIMES_DIFF_FROM_1978_TO_1980;
    sprintf(sn_str, "%s%s%ld", ua, GPT_HTTP_URL_KOUYU_APPSEC, t);
    corepush_md5_str(&sn_str, &sn);
    if(type == 0){
        sprintf(url, "%s%s?platform=%s&page=%d&size=%d&ua=%s&t=%ld&sn=%s", 
            GPT_HTTP_URL_KOUYU, GPT_HTTP_URL_KOUYU_MODEL_LIST, GPT_HTTP_URL_KOUYU_PLATFORM, page, size, ua, t, sn);
    }else{
        sprintf(url, "%s%s?&ua=%s&t=%ld&sn=%s",
            GPT_HTTP_URL_KOUYU, GPT_HTTP_URL_KOUYU_SEND_CHAT, ua, t, sn);
    }
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
}

PUBLIC void ZmtGpt_SendString(char * save_path, char * string)
{
    char data[2048] = {0};
    char imei[20] = {0};
    char url[50] = {0};
    //app_adp_get_imei_req(&imei);
    sprintf(imei, "%s", "869937060002261");
    if(gpt_baidu_access_token){
        sprintf(data, GPT_HTTP_BAIDU_TXT_HTML_PATH, string, imei, gpt_baidu_access_token);
    }else{
        SCI_TRACE_LOW("%s: token error", __FUNCTION__);
        gpt_get_baidu_access_token();
        return;
    }
    SCI_TRACE_LOW("%s: data = %s", __FUNCTION__, data);
    
    sprintf(url, "%s", GPT_HTTP_BAIDU_TXT_PARSE);
    //SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
    MMIZDT_HTTP_AppSend(FALSE, url, data, strlen(data), 10*1000, 0, 0, 0, 0, 0, ZmtGptKouYuTalk_RecAiVoiceResultCb);
    //MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 30*1000, 0, 0, ZmtGptKouYuTalk_RecAiVoiceResultCb);
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
    format = cJSON_CreateString("amr");
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
	
    ZMT_makeGptTokenUrl(1, &url, 1, 10);
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

    list_init.both_rect.v_rect = zmt_gpt_list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, ZMT_GPT_MAX_ITEM, FALSE);

    for(index = 0;index < ZMT_GPT_MAX_ITEM; index++)
    {
        item_t.item_style = GUIITEM_STYLE_GPT_ONE_ICON_AND_ONE_TEXT_MS;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;

        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
        item_data.item_content[0].item_data.image_id = img_id[index];

        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_ID;
        item_data.item_content[1].item_data.text_id = text_id[index];

        //不画分割线
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
        //不画高亮条
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

        GUILIST_SetBgColor(ctrl_id,MMI_BLACK_COLOR);
        GUILIST_SetTextFont(ctrl_id, SONG_FONT_20, MMI_WHITE_COLOR);

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
}

LOCAL void ZmtGpt_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};

    GUI_FillRect(&lcd_dev_info, zmt_gpt_win_rect, MMI_BLACK_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;
    MMIRES_GetText(ZMT_CHAT_GPT, win_id, &text_string);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &zmt_gpt_title_rect,
        &zmt_gpt_title_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
    
    ZmtGpt_DisplayList(win_id, ZMT_GPT_MAIN_LIST_CTRL_ID);
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
                gpt_get_baidu_access_token();
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
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                
            }
            break;
        default:
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
        cur_times - baidu_access_token_times > BAIDU_ACCESS_TOKEN_TIMES_MAX)
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


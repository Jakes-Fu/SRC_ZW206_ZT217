/*****************************************************************************
** File Name:      zmt_dial_store.c                                           *
** Author:           fys                                                        *
** Date:           2024/05/08                                                 *
** Copyright:       *
** Description:                       *
******************************************************************************/
#include "std_header.h"
#include <stdlib.h>
#include "cjson.h"
#include "dal_time.h"
#include "zmt_dial.h"
#include "zmt_dial_id.h"
#include "zmt_dial_text.h"
#include "zmt_dial_image.h"
#include "gps_drv.h"
#include "gps_interface.h"
#include "guibutton.h"
#include "guifont.h"
#include "guilcd.h"
#include "guistring.h"
#include "guitext.h"
#include "mmi_textfun.h"
#include "mmiacc_text.h"
#include "mmicc_export.h"
#include "mmidisplay_data.h"
#include "mmipub.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "watch_slidepage.h"
#ifdef LISTENING_PRATICE_SUPPORT
#include "dsl_main_file.h"
#endif

LOCAL GUI_RECT_T zmt_dial_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};//窗口
LOCAL GUI_RECT_T zmt_dial_title_rect = {10, 0, MMI_MAINSCREEN_WIDTH -10, ZMT_DIAL_LINE_HIGHT};//顶部
LOCAL GUI_RECT_T zmt_dial_list_rect = {0, ZMT_DIAL_LINE_HIGHT, MMI_MAINSCREEN_WIDTH , MMI_MAINSCREEN_HEIGHT - ZMT_DIAL_LINE_HIGHT};
LOCAL GUI_RECT_T zmt_dial_button_rect = {ZMT_DIAL_LINE_WIDTH, MMI_MAINSCREEN_WIDTH - 1.2*ZMT_DIAL_LINE_HIGHT + 5, 5*ZMT_DIAL_LINE_WIDTH, MMI_MAINSCREEN_WIDTH - 5};

typedef struct
{
    char ch_name[80];
    char name[80];
    int type;
}ZMT_DIAL_NAME;

ZMT_DIAL_NAME zmt_dial_name_str[2] = {
    {"表盘0", "dial_0", 2},{"表盘1","dial_1", 1}
};

ZMT_DIAL_LIST_INFO_T * dial_list_info;
MMI_HANDLE_T zmt_dial_select_handle;
ZMT_DIAL_NAME * zmt_cur_dial_info = NULL;
BOOLEAN zmt_is_need_pay = TRUE;

PUBLIC ZMT_DIAL_LIST_INFO_T * ZmtWatch_GetPanelList(void)
{
    char * data_buf = PNULL;
    uint32 data_len = 0;
    char file_path[50] = {0};
    ZMT_DIAL_LIST_INFO_T * dial_list = NULL;
    
    sprintf(file_path, "%s", ZMT_DIAL_FILE_DB_PATH);
    if(dsl_file_exist(file_path))
    {
        data_buf = dsl_file_data_read(file_path, &data_len);
        if(data_buf && data_len > 2){
            dial_list = (ZMT_DIAL_LIST_INFO_T *)SCI_ALLOC_APPZ(sizeof(ZMT_DIAL_LIST_INFO_T));
            memset(dial_list, 0, sizeof(ZMT_DIAL_LIST_INFO_T));
            ZmtDial_ParseDialDB(data_buf, dial_list);
            SCI_FREE(data_buf);
        }
    }
    return dial_list;
}

PUBLIC BOOLEAN ZmtDial_IsExistPanel(char * dial_name)
{
    uint16 i = 0;
    BOOLEAN is_exist = FALSE;
    ZMT_DIAL_LIST_INFO_T * dial_list = ZmtWatch_GetPanelList();

    if(dial_list != NULL){
        for(i = 0;i < dial_list->count;i++)
        {
            if(0 == strncmp(dial_name, dial_list->info[i]->name, strlen(dial_list->info[i]->name)))
            {
                is_exist = TRUE;
                break;
            }
        }
        SCI_FREE(dial_list);
    }
    SCI_TRACE_LOW("%s: is_exist = %d", __FUNCTION__, is_exist);
    return is_exist;
}

PUBLIC void ZmtDialPreview_RealseDialInfo(void)
{
    if(zmt_cur_dial_info != NULL){
        SCI_FREE(zmt_cur_dial_info);
        zmt_cur_dial_info = NULL;
    }
}

PUBLIC void ZmtDial_ReleaseDialList(ZMT_DIAL_LIST_INFO_T * dail_list)
{
    uint16 i = 0;
    if(dail_list != NULL){
        if(dail_list->info[i] != NULL){
            SCI_FREE(dail_list->info[i]);
            dail_list->info[i] = NULL;
        }
        SCI_FREE(dail_list);
        dail_list = NULL;
    }
}

PUBLIC void ZmtDial_ReleaseDialListInfo(void)
{
    uint16 i = 0;
    if(dial_list_info != NULL){
        if(dial_list_info->info[i] != NULL){
            SCI_FREE(dial_list_info->info[i]);
            dial_list_info->info[i] = NULL;
        }
        SCI_FREE(dial_list_info);
        dial_list_info = NULL;
    }
}

PUBLIC void ZmtDial_ParseDialDB(char * buf, ZMT_DIAL_LIST_INFO_T * dail_list)
{
    int i = 0;
    cJSON * json_root = NULL;
    cJSON * json_item_ll = NULL;
    cJSON * json_item = NULL;
    char * valuestring = NULL;
    if(NULL == dail_list){
        return;
    }

    json_root = cJSON_Parse(buf);
    if(NULL == json_root){
        SCI_TRACE_LOW("%s: json parse error\n", __FUNCTION__);
        return;
    }

    dail_list->count = cJSON_GetObjectItem(json_root, "dial_cnt")->valueint;
    //SCI_TRACE_LOW("%s: dail_list->count = %d", __FUNCTION__, dail_list->count);
    json_item_ll = cJSON_GetObjectItem(json_root, "dial_ll");
    cJSON_ArrayForEach(json_item, json_item_ll)
    {
        dail_list->info[i] = (ZMT_DIAL_INFO_T *)SCI_ALLOC_APPZ(sizeof(ZMT_DIAL_INFO_T));
        memset(dail_list->info[i], 0, sizeof(ZMT_DIAL_INFO_T));
        valuestring = cJSON_GetObjectItem(json_item, "dial_name")->valuestring;
        if(valuestring && strlen(valuestring) > 2){
            SCI_MEMCPY(dail_list->info[i]->name, valuestring, strlen(valuestring));
            //SCI_TRACE_LOW("%s: dail_list->info[%d]->name = %s", __FUNCTION__, i, dail_list->info[i]->name);
        }
        valuestring = cJSON_GetObjectItem(json_item, "dial_preview")->valuestring;
        if(valuestring && strlen(valuestring) > 2){
            SCI_MEMCPY(dail_list->info[i]->preview, valuestring, strlen(valuestring));
            //SCI_TRACE_LOW("%s: dail_list->info[%d]->preview = %s", __FUNCTION__, i, dail_list->info[i]->preview);
        }
        dail_list->info[i]->type = cJSON_GetObjectItem(json_item, "dial_type")->valueint;
        //SCI_TRACE_LOW("%s: dail_list->info[%d]->type = %d", __FUNCTION__, i, dail_list->info[i]->type);
        i++;
    }
    cJSON_Delete(json_root);
}

PUBLIC void ZmtDial_AddDialToWatchDB(char * name, int type)
{
    int i = 0;
    BOOLEAN is_add = TRUE;
    char * data_buf = PNULL;
    uint32 file_len = 0;
    char file_path[50] = {0};
    uint16 count = 0;
    
    sprintf(file_path, "%s", ZMT_DIAL_FILE_DB_PATH);
    if(dsl_file_exist(file_path))
    {
        data_buf = dsl_file_data_read(file_path, &file_len);
        if(dial_list_info == NULL){
            dial_list_info = (ZMT_DIAL_LIST_INFO_T *)SCI_ALLOC_APPZ(sizeof(ZMT_DIAL_LIST_INFO_T));
            memset(dial_list_info, 0, sizeof(ZMT_DIAL_LIST_INFO_T));
            if(data_buf && file_len > 2){
                ZmtDial_ParseDialDB(data_buf, dial_list_info);
                SCI_FREE(data_buf);
            }
        }     
        for(i = 0;i < dial_list_info->count;i++)
        {
            if(0 == strncmp(dial_list_info->info[i]->name, name, strlen(dial_list_info->info[i]->name))){
                is_add = FALSE;
                break;
            }
        }
        count = dial_list_info->count;
    }
   
    SCI_TRACE_LOW("%s: is_add = %d", __FUNCTION__, is_add);
    if(is_add)
    {
        cJSON * json_root = NULL;
        cJSON * json_count = NULL;
        cJSON * json_item_ll = NULL;
        cJSON * json_item = NULL;
        cJSON * json_name = NULL;
        cJSON * json_preview = NULL;
        cJSON * json_type = NULL;
        cJSON * json_index = NULL;
        char * out = NULL;
        char preview[80] ={0};

        json_root = cJSON_CreateObject();
        json_count = cJSON_CreateNumber(count+1);
        json_item_ll = cJSON_CreateArray();
        
        cJSON_AddItemToObject(json_root, "dial_cnt", json_count);
        cJSON_AddItemToObject(json_root, "dial_ll", json_item_ll);
        
        SCI_TRACE_LOW("%s: count = %d", __FUNCTION__, count);
        for(i = 0;i < count;i++)
        {
            json_item = cJSON_CreateObject();
            json_name = cJSON_CreateString(dial_list_info->info[i]->name);
            cJSON_AddItemToObject(json_item, "dial_name", json_name);
            json_preview = cJSON_CreateString(dial_list_info->info[i]->preview);
            cJSON_AddItemToObject(json_item, "dial_preview", json_preview);
            json_type = cJSON_CreateNumber(dial_list_info->info[i]->type);
            cJSON_AddItemToObject(json_item, "dial_type", json_type);

            cJSON_AddItemToArray(json_item_ll, json_item);
            
            SCI_FREE(dial_list_info->info[i]);
            dial_list_info->info[i] = NULL;
        }

        json_item = cJSON_CreateObject();
        json_name = cJSON_CreateString(name);
        cJSON_AddItemToObject(json_item, "dial_name", json_name);
        sprintf(preview, "%s_preview.png", name);
        json_preview = cJSON_CreateString(preview);
        cJSON_AddItemToObject(json_item, "dial_preview", json_preview);
        json_type = cJSON_CreateNumber(type);
        cJSON_AddItemToObject(json_item, "dial_type", json_type);

        cJSON_AddItemToArray(json_item_ll, json_item);

        out = cJSON_Print(json_root);
        cJSON_Delete(json_root);
        if(dsl_file_exist(file_path)){
            dsl_file_delete(file_path);
        }
        dsl_file_data_write(out, strlen(out), file_path);

        if(dial_list_info == NULL){
            dial_list_info = (ZMT_DIAL_LIST_INFO_T *)SCI_ALLOC_APPZ(sizeof(ZMT_DIAL_LIST_INFO_T));
        }
        memset(dial_list_info, 0, sizeof(ZMT_DIAL_LIST_INFO_T));
        ZmtDial_ParseDialDB(out, dial_list_info);
        SCI_FREE(out);
    }
}

LOCAL MMI_RESULT_E ZmtDialStorePreview_ButtonClickUse(MMI_WIN_ID_T win_id)
{
    WatchOpen_Panel_SelectWin();
    if(zmt_dial_select_handle != NULL)
    {
        WatchSLIDEPAGE_DestoryHandle(zmt_dial_select_handle);
        zmt_dial_select_handle = 0;
    }
    if(MMK_IsOpenWin(ZMT_DIAL_STORE_WIN_ID))
    {
        MMK_CloseWin(ZMT_DIAL_STORE_WIN_ID);
    }
}

LOCAL MMI_RESULT_E ZmtDialStorePreview_ButtonClickDownload(MMI_WIN_ID_T win_id)
{
    if(zmt_cur_dial_info != NULL){
        ZmtDial_AddDialToWatchDB(zmt_cur_dial_info->name, zmt_cur_dial_info->type);
        MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void ZmtDialStorePreview_ShowPreview(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info, int index)
{
    GUIIMG_INFO_T img_info = {0};
    GUI_RECT_T img_rect = {0};
    MMI_CTRL_ID_T ctrl_id = 0;
    char img_str[80] = {0};
    wchar img_path[80] = {0};
    
    ctrl_id = ZMT_DIAL_STORE_PREVIEW_IMG_1_CTRL_ID + index;
    sprintf(img_str, "%s\\%s\\%s_preview.png", ZMT_DIAL_DIR_BASE_PATH, zmt_cur_dial_info->name, zmt_cur_dial_info->name);
    if(dsl_file_exist(img_str)){
        //SCI_TRACE_LOW("%s: img_str = %d", __FUNCTION__, img_str);
        MMIAPICOM_StrToWstr(&img_str, &img_path);
        ZMT_GetImgInfoByPath(&img_path, &img_info);
        memset(&img_rect, 0, sizeof(GUI_RECT_T));
        img_rect.left = (MMI_MAINSCREEN_WIDTH - img_info.image_width) / 2;
        img_rect.top = ZMT_DIAL_LINE_HIGHT;
        img_rect.right = img_info.image_width + img_rect.left;
        img_rect.bottom = img_info.image_height + img_rect.top;
        ZMT_CreateAnimImg(win_id, ctrl_id, &img_rect, img_path, 1, &lcd_dev_info);
    }
}

LOCAL void ZmtDialStorePreview_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
    wchar dial_name[80] = {0};

    GUI_FillRect(&lcd_dev_info, zmt_dial_win_rect, MMI_WHITE_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
    text_style.font_color = MMI_BLACK_COLOR;
//#ifdef WIN32
//    GUI_UTF8ToWstr(&dial_name, 80, zmt_cur_dial_info->ch_name, strlen(zmt_cur_dial_info->ch_name));
//#else
    GUI_GBToWstr(&dial_name, zmt_cur_dial_info->ch_name, strlen(zmt_cur_dial_info->ch_name));
//#endif
    text_string.wstr_ptr = dial_name;
    text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &zmt_dial_title_rect,
        &zmt_dial_title_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
    text_style.font = SONG_FONT_16;
    if(ZmtDial_IsExistPanel(zmt_cur_dial_info->name))
    {
        zmt_is_need_pay = FALSE;
        MMIRES_GetText(ZMT_DIAL_PREVIEW_ALREADY_DOWNLOAD, win_id, &text_string);
    }
    else
    {
        zmt_is_need_pay = TRUE;
        MMIRES_GetText(ZMT_DIAL_PREVIEW_DOWNLOAD_DIAL, win_id, &text_string);
    }
    GUISTR_DrawTextToLCDInRect(
        (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
        &zmt_dial_button_rect,
        &zmt_dial_button_rect,
        &text_string,
        &text_style,
        text_state,
        GUISTR_TEXT_DIR_AUTO
    );
    GUI_DisplayBorder(zmt_dial_button_rect, zmt_dial_button_rect, &btn_border, &lcd_dev_info);

    if(win_id == ZMT_DIAL_STORE_PREVIEW_1_WIN_ID)
    {
        ZmtDialStorePreview_ShowPreview(win_id, lcd_dev_info, 0);
    }
    else
    {
        ZmtDialStorePreview_ShowPreview(win_id, lcd_dev_info, 1);
    }
}

LOCAL MMI_RESULT_E HandleZmtDialStorePreviewWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                
            }
            break;
        case MSG_FULL_PAINT:
            {             
                ZmtDialStorePreview_FULL_PAINT(win_id);               
            }
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(GUI_PointIsInRect(point, zmt_dial_button_rect))
                {
                    if(zmt_is_need_pay){
                        ZmtDialStorePreview_ButtonClickDownload(win_id);
                    }else{
                        ZmtDialStorePreview_ButtonClickUse(win_id);
                    }
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
                ZmtDialPreview_RealseDialInfo();
                if(zmt_dial_select_handle != NULL)
                {
                    WatchSLIDEPAGE_DestoryHandle(zmt_dial_select_handle);
                    zmt_dial_select_handle = 0;
                }
            }
            break;
        default:
            break;
    }
}

LOCAL void MMI_CreateZmtDialStorePreviewWin(const MMI_WIN_ID_T win_id)
{
    uint32 win_table_create[12] = 
    {
        MMK_HIDE_STATUSBAR,
        MMK_WINFUNC, HandleZmtDialStorePreviewWinMsg,
        MMK_WINID, 0,
        MMK_WINDOW_STYLE, (WS_DISPATCH_TO_CHILDWIN |WS_DISABLE_RETURN_WIN),
        MMK_WINDOW_ANIM_MOVE_SYTLE, (MOVE_FORBIDDEN),
        //MMK_CREATE_BUTTON, (PNULL, ZMT_DIAL_STORE_PREVIEW_BUTTON_CTRL_ID),//该创建窗口方式不支持创建button控件
        MMK_END_WIN,
    };
    win_table_create[4] = win_id;
    MMK_CreateWin(win_table_create, NULL);
}

PUBLIC MMI_RESULT_E ZmtDialStorePreview_Select_HandleCb(
                                      MMI_WIN_ID_T        win_id,        //IN:
                                      MMI_MESSAGE_ID_E    msg_id,        //IN:
                                      DPARAM            param        //IN:
                                      )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id)
    {
        case MSG_SLIDEPAGE_OPENED:
        {
            break;
        }

        case MSG_SLIDEPAGE_GETFOCUS:
        {
            break;
        }

        case MSG_SLIDEPAGE_LOSEFOCUS:
        {
            break;
        }

        case MSG_SLIDEPAGE_PAGECHANGED:
        {
            break;
        }
        case MSG_SLIDEPAGE_END:
        {
            MMK_SendMsg(win_id, MSG_SLIDEPAGE_END, PNULL);
            break;
        }
        default:
            break;
    }
    return recode;
}

PUBLIC void ZmtDialStorePreview_Enter(void)
{
    tWatchSlidePageItem elem[5] = {0};
    MMI_HANDLE_T handle = 0;
    uint8 i = 0;
    uint8 focus_index = 0;
    if(zmt_dial_select_handle)
    {
       WatchSLIDEPAGE_DestoryHandle(zmt_dial_select_handle);
       zmt_dial_select_handle = 0;
    }
    handle = WatchSLIDEPAGE_CreateHandle();

    elem[i].fun_enter_win = MMI_CreateZmtDialStorePreviewWin;
    elem[i].win_id = ZMT_DIAL_STORE_PREVIEW_1_WIN_ID;
    i++;

    elem[i].fun_enter_win = MMI_CreateZmtDialStorePreviewWin;
    elem[i].win_id = ZMT_DIAL_STORE_PREVIEW_2_WIN_ID;
    i++;

    WatchSLIDEPAGE_Open(handle, elem, i, 0, FALSE, ZmtDialStorePreview_Select_HandleCb);
    zmt_dial_select_handle = handle;
}

///////////////////////////////////////////////////////////////////////////

LOCAL void ZmtDialStore_DisplayDialList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    uint8 index = 0;
    uint8 num = 0;
    GUILIST_INIT_DATA_T list_init = {0};
    GUILIST_ITEM_T item_t = {0};
    GUIITEM_STATE_T item_state = {0};
    GUILIST_ITEM_DATA_T item_data = {0};
    MMI_STRING_T text_str = {0};
    wchar name_wchar[50] = {0};
    char name_str[50] = {0};
    uint8 length = 0;
    MMI_STRING_T text_string = {0};

    list_init.both_rect.v_rect = zmt_dial_list_rect;
    list_init.type = GUILIST_TEXTLIST_E;
    GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);

    MMK_SetAtvCtrl(win_id, ctrl_id);
    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, 2, FALSE);

    for(index = 0;index < 2; index++)
    {
        length = strlen(zmt_dial_name_str[index].ch_name);
		
        item_t.item_style = GUIITEM_DSL_HANZI_BOOK;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
		
        memset(name_wchar, 0, 50);
        memset(name_str, 0, 50);

        //book name
        //#ifdef WIN32
        GUI_GBToWstr(name_wchar, zmt_dial_name_str[index].ch_name, length);
        //#else
        //GUI_UTF8ToWstr(name_wchar, length, zmt_dial_name_str[index].ch_name, length);
        //#endif
        text_str.wstr_ptr = name_wchar;
        text_str.wstr_len = MMIAPICOM_Wstrlen(text_str.wstr_ptr);
        item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
        item_data.item_content[0].item_data.text_buffer = text_str;

        //不画分割线
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE);
        //不画高亮条
        GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);

        GUILIST_SetNeedPrgbarBlock(ctrl_id,FALSE);

        GUILIST_SetBgColor(ctrl_id,MMI_WHITE_COLOR);
        GUILIST_SetTextFont(ctrl_id, SONG_FONT_16, MMI_BLACK_COLOR);

        GUILIST_AppendItem(ctrl_id, &item_t);
    }
}

LOCAL void ZmtDialStore_FULL_PAINT(MMI_WIN_ID_T win_id)
{
	GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
	GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
	GUISTR_STYLE_T text_style = {0};
	MMI_STRING_T text_string = {0};

	GUI_FillRect(&lcd_dev_info, zmt_dial_win_rect, MMI_WHITE_COLOR);

	text_style.align = ALIGN_HVMIDDLE;
	text_style.font = SONG_FONT_20;
	text_style.font_color = MMI_BLACK_COLOR;
	MMIRES_GetText(ZMT_DIAL_STORE, win_id, &text_string);
	GUISTR_DrawTextToLCDInRect(
		(const GUI_LCD_DEV_INFO *)&lcd_dev_info,
		&zmt_dial_title_rect,
		&zmt_dial_title_rect,
		&text_string,
		&text_style,
		text_state,
		GUISTR_TEXT_DIR_AUTO
	);
	ZmtDialStore_DisplayDialList(win_id, ZMT_DIAL_STORE_MAIN_WIN_LIST_CTRL_ID);
}

LOCAL void ZmtDialStore_CTL_PENOK(MMI_WIN_ID_T win_id)
{
    uint16 cur_idx = GUILIST_GetCurItemIndex(ZMT_DIAL_STORE_MAIN_WIN_LIST_CTRL_ID);
    
    ZmtDialPreview_RealseDialInfo();
    zmt_cur_dial_info = (ZMT_DIAL_NAME *)SCI_ALLOC_APPZ(sizeof(ZMT_DIAL_NAME));
    memset(zmt_cur_dial_info, 0, sizeof(ZMT_DIAL_NAME));
    SCI_MEMCPY(zmt_cur_dial_info->ch_name, zmt_dial_name_str[cur_idx].ch_name, strlen(zmt_dial_name_str[cur_idx].ch_name));
    SCI_MEMCPY(zmt_cur_dial_info->name, zmt_dial_name_str[cur_idx].name, strlen(zmt_dial_name_str[cur_idx].name));
    zmt_cur_dial_info->type = zmt_dial_name_str[cur_idx].type;
    
    ZmtDialStorePreview_Enter();
}

LOCAL MMI_RESULT_E HandleZmtDialStoreWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {

            }
            break;
        case MSG_FULL_PAINT:
            {             
                ZmtDialStore_FULL_PAINT(win_id);               
            }
            break;
        case MSG_CTL_PENOK:
            {
                ZmtDialStore_CTL_PENOK(win_id);
            }
            break;
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                ZmtDial_ReleaseDialListInfo();
            }
            break;
        default:
            break;
    }
    return recode;
}

WINDOW_TABLE(MMI_DIAL_STORE_WIN_TAB) = {
    WIN_ID(ZMT_DIAL_STORE_WIN_ID),
    WIN_FUNC((uint32)HandleZmtDialStoreWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMI_CreateZmtDialStoreWin(void)
{
    if(MMK_IsOpenWin(ZMT_DIAL_STORE_WIN_ID))
    {
        MMK_CloseWin(ZMT_DIAL_STORE_WIN_ID);
    }
    MMK_CreateWin((uint32 *)MMI_DIAL_STORE_WIN_TAB, PNULL);
}

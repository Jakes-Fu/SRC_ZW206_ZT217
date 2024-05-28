/*****************************************************************************
** File Name:      zmt_dial_store.c                                           *
** Author:           fys                                                        *
** Date:           2024/05/08                                                 *
** Copyright:       *
** Description:                       *
******************************************************************************/
#include "std_header.h"
#include <stdio.h> 
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
#include "guiiconlist.h"
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
#include "http_api.h"

LOCAL GUI_RECT_T zmt_dial_win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};//窗口
LOCAL GUI_RECT_T zmt_dial_title_rect = {10, 0, MMI_MAINSCREEN_WIDTH -10, ZMT_DIAL_LINE_HIGHT};//顶部
LOCAL GUI_RECT_T zmt_dial_list_rect = {0, ZMT_DIAL_LINE_HIGHT, MMI_MAINSCREEN_WIDTH , MMI_MAINSCREEN_HEIGHT - ZMT_DIAL_LINE_HIGHT};
LOCAL GUI_RECT_T zmt_dial_button_rect = {ZMT_DIAL_LINE_WIDTH, MMI_MAINSCREEN_WIDTH - ZMT_DIAL_LINE_HIGHT + 5, 5*ZMT_DIAL_LINE_WIDTH, MMI_MAINSCREEN_HEIGHT - 5};

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
BOOLEAN zmt_is_need_pay = TRUE;
uint8 dial_store_status = 0;
uint8 dial_store_count = 0;
ZMT_DIAL_STORE_INFO_T * dial_store_list[WATCH_MAX_DIAL_LIST_COUNT];
uint8 dial_store_download_idx = 0;
uint8 dial_store_cur_idx = 0;
uint8 dial_store_preview_status = 0;
uint8 * dial_store_preview_buf = NULL;

LOCAL void ZmtDialStore_RequestPreviewIcon(uint8 idx);

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

PUBLIC BOOLEAN ZmtDial_IsExistPanel(char * dial_name)
{
    uint16 i = 0;
    BOOLEAN is_exist = FALSE;
    ZMT_DIAL_LIST_INFO_T * dail_list = ZmtWatch_GetPanelList();

    if(dail_list != NULL){
        for(i = 0;i < dail_list->count;i++)
        {
            if(0 == strncmp(dial_name, dail_list->info[i]->name, strlen(dail_list->info[i]->name)))
            {
                is_exist = TRUE;
                break;
            }
        }
        ZmtDial_ReleaseDialList(dail_list);
    }
    SCI_TRACE_LOW("%s: is_exist = %d", __FUNCTION__, is_exist);
    return is_exist;
}

PUBLIC void ZmtDial_ReleaseDialListInfo(void)
{
    uint8 i = 0;
    for(i = 0;i < dial_store_count && i < WATCH_MAX_DIAL_LIST_COUNT;i++)
    {
        if(dial_store_list[i] != NULL)
        {         
            SCI_FREE(dial_store_list[i]);
            dial_store_list[i] = NULL;
        }
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

LOCAL void ZmtDialStore_ParseDialBuf(char * file_path)
{
    uint8 i = 0;
    MMIFILE_HANDLE file_handle = 0;
    wchar file_str[100] = {0};
    char * buf = NULL;
    uint32 size = 0;
    int byte_read = 0;
    char buffer[256];
    char filename[256];
    int fileCount, bytesRead, versionNumber;
    //|版本号 int 4byte|文件长度 int 4byte| [ 文件名|文件内容]

    /*FILE *handler = fopen(file_path,"rb");
    if (handler)  
    {
        SCI_TRACE_LOW("%s: ok", __FUNCTION__);
    }*/
     buf = dsl_file_data_read(file_path, &size);
     if (buf != PNULL && size > 2)
     {
        for(i = 0;i < 8;i++)
        {
            SCI_TRACE_LOW("%s: buf[%d] = %c", __FUNCTION__, i, buf[i]);
        }
        /*SCI_TRACE_LOW("%s: sizeof(int) = %d", __FUNCTION__, sizeof(int));
        byte_read = 8;
        buf = SCI_ALLOC_APPZ(8);
        memset(buf, 0, 8);
        dsl_file_read(file_path, &buf, 4, &byte_read);*/
        
    }
   /*MMIAPICOM_StrToWstr(file_path, file_str);
    file_handle = MMIFILE_CreateFile(file_str, SFS_MODE_OPEN_EXISTING |SFS_MODE_READ, NULL, NULL);
    if(SFS_INVALID_HANDLE != file_handle){
        MMIAPIFMM_ReadFile(file_handle, buf, byte_read, &size, NULL);
        SCI_TRACE_LOW("%s: buf = %s", __FUNCTION__, buf);
    }*/
}

PUBLIC void ZmtDialStore_RecDownlaodResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        char file[100] = {0};
        sprintf(file, ZMT_DIAL_FILE_BASE_PATH, dial_store_list[dial_store_cur_idx]->name, dial_store_list[dial_store_cur_idx]->name);
        SCI_TRACE_LOW("%s: file = %s", __FUNCTION__, file);
        if(!dsl_file_exist(file)){
            dsl_file_data_write(pRcv, Rcv_len, file);
         }   
        //ZmtDialStore_ParseDialBuf(file);
        
    }
    else
    {
        
    }
}

LOCAL MMI_RESULT_E ZmtDialStorePreview_ButtonClickDownload(MMI_WIN_ID_T win_id)
{
    if(dial_store_list[dial_store_cur_idx] != NULL)
    {
        char url[100] = {0};
        sprintf(url, "%s%s", ZMT_HTTP_API_BASE_PATH, dial_store_list[dial_store_cur_idx]->file);
        MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, ZmtDialStore_RecDownlaodResultCb);
        //ZmtDial_AddDialToWatchDB(dial_store_list[dial_store_cur_idx]->name, dial_store_list[dial_store_cur_idx]->type);
        //MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
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
    if(MMK_IsOpenWin(ZMT_DIAL_STORE_PREVIEW_2_WIN_ID))
    {
        MMK_CloseWin(ZMT_DIAL_STORE_PREVIEW_2_WIN_ID);
    }
    if(MMK_IsOpenWin(ZMT_DIAL_STORE_WIN_ID))
    {
        MMK_CloseWin(ZMT_DIAL_STORE_WIN_ID);
    }
}

LOCAL void ZmtDialStorePreview_ShowPreview(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info, int index)
{
    MMI_CTRL_ID_T form_ctrl_id = ZMT_DIAL_STORE_PREVIEW_FORM_CTRL_ID;
    MMI_CTRL_ID_T anim_ctrl_id = ZMT_DIAL_STORE_PREVIEW_IMG_1_CTRL_ID;
    GUIANIM_INIT_DATA_T anim_init_data = {0};
    GUIFORM_DYNA_CHILD_T anim_form_child_ctrl = {0};
    MMI_HANDLE_T ctrl_handle = 0;
    GUIIMG_INFO_T img_info = {0};
    GUI_RECT_T form_rect = {0};
    GUI_RECT_T img_rect = {MMI_MAINSCREEN_WIDTH/4, MMI_MAINSCREEN_HEIGHT/4, 3*MMI_MAINSCREEN_WIDTH/4, 3*MMI_MAINSCREEN_HEIGHT/4};
    MMI_CTRL_ID_T ctrl_id = 0;
    char img_str[128] = {0};
    wchar img_path[128] = {0};
    uint8 i = 0;

    sprintf(img_str, ZMT_DIAL_PREVIEW_FILE_BASE_PATH, dial_store_list[dial_store_cur_idx]->name, dial_store_list[dial_store_cur_idx]->name);
    if(dsl_file_exist(img_str))
    {
        MMIAPICOM_StrToWstr(img_str, img_path);
        ZMT_GetImgInfoByPath(img_path, &img_info);
        img_rect.left = (MMI_MAINSCREEN_WIDTH - img_info.image_width) / 2;
        img_rect.top = 1.5*ZMT_DIAL_LINE_HIGHT;
        img_rect.right = img_info.image_width + img_rect.left;
        img_rect.bottom = img_info.image_height + img_rect.top;

        GUIFORM_CreatDynaCtrl(win_id, form_ctrl_id, GUIFORM_LAYOUT_ORDER);
        {
            anim_form_child_ctrl.child_handle = anim_ctrl_id+i;
            anim_form_child_ctrl.init_data_ptr = &anim_init_data;
            anim_form_child_ctrl.guid = SPRD_GUI_ANIM_ID;
            GUIFORM_CreatDynaChildCtrl(win_id, form_ctrl_id, &anim_form_child_ctrl);
        }
        ctrl_handle = MMK_GetCtrlHandleByWin(win_id, form_ctrl_id);
        GUIFORM_SetRect(ctrl_handle, &img_rect);
        {
            GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
            GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
            GUIANIM_FILE_INFO_T file_info = {0};
            GUIANIM_DATA_INFO_T data_info = {0};
            GUIANIM_CTRL_INFO_T ctrl_info = {0};
            GUIANIM_DISPLAY_INFO_T display_info = {0};
            BOOLEAN is_visible = FALSE;
            BOOLEAN is_update = FALSE;

            file_info.full_path_wstr_ptr = SCI_ALLOC_APPZ(sizeof(wchar)*(WATCH_IMAGE_FULL_PATH_MAX_LEN + 1));
            if (PNULL == file_info.full_path_wstr_ptr){
                SCI_TRACE_LOW("%s: full_path_wstr_ptr = pnull!", __FUNCTION__);
                return;
            }
            SCI_MEMSET(file_info.full_path_wstr_ptr,0,(sizeof(wchar)*(WATCH_IMAGE_FULL_PATH_MAX_LEN + 1)));

            file_info.full_path_wstr_len = MMIAPICOM_Wstrlen(img_path);
            MMI_WSTRNCPY(file_info.full_path_wstr_ptr,sizeof(wchar)*(file_info.full_path_wstr_len+1),
                img_path,file_info.full_path_wstr_len,file_info.full_path_wstr_len);

            //data_info.data_ptr = dial_store_preview_buf;
            //data_info.data_size = strlen(dial_store_preview_buf);
            ctrl_info.is_ctrl_id = TRUE;
            ctrl_info.ctrl_id = anim_ctrl_id+i;
            display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
            display_info.is_auto_zoom_in = TRUE;
            display_info.is_update = TRUE;
            display_info.is_disp_one_frame = TRUE;

            width.add_data = img_info.image_width;
            height.add_data = img_info.image_height;
            GUIFORM_SetChildAlign(ctrl_handle, anim_ctrl_id+i, GUIFORM_CHILD_ALIGN_HMIDDLE);
            GUIFORM_SetChildWidth(ctrl_handle, anim_ctrl_id+i, &width);
            GUIFORM_SetChildHeight(ctrl_handle, anim_ctrl_id+i, &height);
            //GUIANIM_SetDefaultIcon(anim_ctrl_id, PNULL, PNULL);
            GUIANIM_SetParam(&ctrl_info, PNULL, &file_info, &display_info);
            //GUIANIM_SetParam(&ctrl_info, &data_info, PNULL, &display_info);
        }
    }
}

LOCAL void ZmtDialStorePreview_FULL_PAINT(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID,GUI_BLOCK_MAIN};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_string = {0};
    GUI_BORDER_T btn_border = {1, MMI_BLACK_COLOR, GUI_BORDER_SOLID};
    wchar dial_name[50] = {0};

    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    GUI_FillRect(&lcd_dev_info, zmt_dial_win_rect, MMI_WHITE_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
    text_style.font_color = MMI_BLACK_COLOR;
#ifndef WIN32
    GUI_UTF8ToWstr(&dial_name, 50, dial_store_list[dial_store_cur_idx]->name_chn, strlen(dial_store_list[dial_store_cur_idx]->name_chn));
#else
    GUI_GBToWstr(&dial_name, dial_store_list[dial_store_cur_idx]->name_chn, strlen(dial_store_list[dial_store_cur_idx]->name_chn));
#endif
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

    if(dial_store_preview_status < 3)
    {
        switch(dial_store_preview_status)
        {
            case 0:
                {
                    MMIRES_GetText(ZMT_DIAL_STORE_PREVIEW_REQUESET, win_id, &text_string);
                }
                break;
            case 1:
                {
                    MMIRES_GetText(ZMT_DIAL_STORE_REQUEST_FAIL, win_id, &text_string);
                }
                break;
            case 2:
                {
                    MMIRES_GetText(ZMT_DIAL_STORE_REQUEST_ERROR, win_id, &text_string);
                }
                break;
            default:
                break;
        }
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &zmt_dial_win_rect,
            &zmt_dial_win_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
        return;
    }
    
    text_style.font = SONG_FONT_16;
    if(ZmtDial_IsExistPanel(dial_store_list[dial_store_cur_idx]->name))
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

PUBLIC void ZmtDialStorePreview_RecResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        if(dial_store_preview_buf != NULL)
        {
            SCI_FREE(dial_store_preview_buf);
            dial_store_preview_buf = NULL;
        }
        dial_store_preview_buf = SCI_ALLOC_APPZ(Rcv_len + 1);
        memset(dial_store_preview_buf, 0, Rcv_len + 1);
        strcpy(dial_store_preview_buf, pRcv);
        dial_store_preview_status = 3;
    }
    else
    {
        dial_store_preview_status = 1;
    }
    if(MMK_IsFocusWin(ZMT_DIAL_STORE_PREVIEW_1_WIN_ID))
    {
        MMK_SendMsg(ZMT_DIAL_STORE_PREVIEW_1_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void ZmtDialStorePreview_OPEN_WINDOW(MMI_WIN_ID_T win_id)
{
    char url[100] = {0};
    sprintf(url, "%s%s", ZMT_HTTP_API_BASE_PATH, dial_store_list[dial_store_cur_idx]->preview);
    SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
    MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, ZmtDialStorePreview_RecResultCb);
}

LOCAL MMI_RESULT_E HandleZmtDialStorePreviewWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                //ZmtDialStorePreview_OPEN_WINDOW(win_id);
                dial_store_preview_status = 3;
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
                dial_store_cur_idx = 0;
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

WINDOW_TABLE(MMI_ZMT_STORE_PREVIEW_WIN_TAB) = {
    WIN_ID(ZMT_DIAL_STORE_PREVIEW_2_WIN_ID),
    WIN_FUNC((uint32)HandleZmtDialStorePreviewWinMsg),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL void MMIZMT_CreateZmtDialStorePreviewWin(void)
{
    MMK_CreateWin((uint32 *)MMI_ZMT_STORE_PREVIEW_WIN_TAB, PNULL);
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

    /*elem[i].fun_enter_win = MMI_CreateZmtDialStorePreviewWin;
    elem[i].win_id = ZMT_DIAL_STORE_PREVIEW_2_WIN_ID;
    i++;*/

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
    GUILIST_SetMaxItem(ctrl_id, dial_store_count, FALSE);

    for(index = 0;index < dial_store_count; index++)
    {
        length = strlen(dial_store_list[index]->name_chn);
		
        item_t.item_style = GUIITEM_DSL_HANZI_BOOK;
        item_t.item_data_ptr = &item_data;
        item_t.item_state = GUIITEM_STATE_SELFADAPT_RECT|GUIITEM_STATE_CONTENT_CHECK;
		
        memset(name_wchar, 0, 50);
        memset(name_str, 0, 50);

        //book name
        #ifdef WIN32
        GUI_GBToWstr(name_wchar, dial_store_list[index]->name_chn, length);
        #else
        GUI_UTF8ToWstr(name_wchar, length, dial_store_list[index]->name_chn, length);
        #endif
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

    GUI_FillRect(&lcd_dev_info, zmt_dial_win_rect, MMI_BLACK_COLOR);

    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_20;
    text_style.font_color = MMI_WHITE_COLOR;
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
    if(dial_store_status < 3)
    {
        switch(dial_store_status)
        {
            case 0:
                {
                    MMIRES_GetText(ZMT_DIAL_STORE_LIST_REQUEST, win_id, &text_string);
                }
                break;
            case 1:
                {
                    MMIRES_GetText(ZMT_DIAL_STORE_REQUEST_FAIL, win_id, &text_string);
                }
                break;
            case 2:
                {
                    MMIRES_GetText(ZMT_DIAL_STORE_REQUEST_ERROR, win_id, &text_string);
                }
                break;
            default:
                break;
        }
        GUISTR_DrawTextToLCDInRect(
            (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
            &zmt_dial_win_rect,
            &zmt_dial_win_rect,
            &text_string,
            &text_style,
            text_state,
            GUISTR_TEXT_DIR_AUTO
        );
        return;
    }
    //ZmtDialStore_DisplayDialList(win_id, ZMT_DIAL_STORE_MAIN_WIN_LIST_CTRL_ID);
}

LOCAL void ZmtDialStore_CTL_PENOK(MMI_WIN_ID_T win_id, uint16 cur_idx)
{
    dial_store_cur_idx = cur_idx;
    ZmtDialStorePreview_Enter();
    //MMIZMT_CreateZmtDialStorePreviewWin();
}

LOCAL void ZmtDialStore_UpdateIconlist(uint8 idx)
{
    GUI_RECT_T rect = zmt_dial_list_rect;
    MMI_CTRL_ID_T ctrl_id = ZMT_DIAL_STORE_PREVIEW_ICONLIST_CTRL_ID;
    
    GUIICONLIST_SetTotalIcon(ZMT_DIAL_STORE_PREVIEW_ICONLIST_CTRL_ID, idx+1);
    GUIICONLIST_SetRect(ZMT_DIAL_STORE_PREVIEW_ICONLIST_CTRL_ID,&rect);
    {
        GUIICONLIST_DISP_T icon_list = {0};
        uint8 length = 0;
        char file_str[100] = {0};
        wchar name[100] = {0};
        GUIANIM_FILE_INFO_T file_info = {0};
        uint16 icon_index = idx;
        SCI_TRACE_LOW("%s: icon_index = %d", __FUNCTION__, icon_index); 

        sprintf(file_str, ZMT_DIAL_PREVIEW_FILE_BASE_PATH, dial_store_list[icon_index]->name, dial_store_list[icon_index]->name);
        MMIAPICOM_StrToWstr(file_str, name);
        file_info.full_path_wstr_ptr = name;
        file_info.full_path_wstr_len = MMIAPICOM_Wstrlen(name);
        GUIICONLIST_AppendIcon(icon_index, ctrl_id, PNULL, &file_info);

        length = strlen(dial_store_list[icon_index]->name_chn);
    #ifdef WIN32
        GUI_GBToWstr(name, dial_store_list[icon_index]->name_chn, length);
    #else
        GUI_UTF8ToWstr(name, length, dial_store_list[icon_index]->name_chn, length);
    #endif
        icon_list.name_str.wstr_ptr = name;
        icon_list.name_str.wstr_len= MMIAPICOM_Wstrlen(name);
        GUIICONLIST_AppendText(icon_index, ctrl_id, &icon_list);
    }
}

PUBLIC void ZmtDialStore_RecPreviewIconResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        char file[100] = {0};
        sprintf(file, ZMT_DIAL_PREVIEW_FILE_BASE_PATH, dial_store_list[dial_store_download_idx]->name, dial_store_list[dial_store_download_idx]->name);
        SCI_TRACE_LOW("%s: file = %s", __FUNCTION__, file);
        dsl_file_data_write(pRcv, Rcv_len, file);
        ZmtDialStore_UpdateIconlist(dial_store_download_idx);
        SCI_TRACE_LOW("%s: dial_store_download_idx = %d", __FUNCTION__, dial_store_download_idx);
        if(dial_store_download_idx < dial_store_count){
            dial_store_download_idx++;
            ZmtDialStore_RequestPreviewIcon(dial_store_download_idx);
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_FILE_NO_SPACE){
            SCI_TRACE_LOW("%s: NO_SPACE", __FUNCTION__);
        }else{
            SCI_TRACE_LOW("%s: err_id = %d", __FUNCTION__, err_id);
            ZmtDialStore_RequestPreviewIcon(dial_store_download_idx);
        }
    }
}

LOCAL void ZmtDialStore_RequestPreviewIcon(uint8 idx)
{ 
    char url[100] = {0};
    char file_str[100] = {0};
    if(idx < dial_store_count)
    {
        dial_store_download_idx = idx;
        sprintf(url, "%s%s", ZMT_HTTP_API_BASE_PATH, dial_store_list[idx]->preview);
        SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
        sprintf(file_str, ZMT_DIAL_PREVIEW_FILE_BASE_PATH, dial_store_list[idx]->name, dial_store_list[idx]->name);
        if(dsl_file_exist(file_str)){
            ZmtDialStore_UpdateIconlist(dial_store_download_idx);
            dial_store_download_idx++;
            ZmtDialStore_RequestPreviewIcon(dial_store_download_idx);
        }else{
            MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, ZmtDialStore_RecPreviewIconResultCb);
        }
    }
    else
    {
        SCI_TRACE_LOW("%s: downlaod compelete", __FUNCTION__);
    }
}

PUBLIC void ZmtDialStore_RecResultCb(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id)
{
    SCI_TRACE_LOW("%s: is_ok = %d, Rcv_len  = %d", __FUNCTION__, is_ok, Rcv_len);
    if (is_ok && pRcv != PNULL && Rcv_len> 2)
    {
        uint8 i = 0;
        cJSON * root = cJSON_Parse(pRcv);
        cJSON * code = cJSON_GetObjectItem(root, "code");
        cJSON * data = cJSON_GetObjectItem(root, "data");
        if(code->valueint == 200 && data != NULL && data->type != cJSON_NULL)
        {
            SCI_TRACE_LOW("%s: Size(data) = %d", __FUNCTION__, cJSON_GetArraySize(data));
            dial_store_count = cJSON_GetArraySize(data);
            for(i = 0;i < cJSON_GetArraySize(data) && i < WATCH_MAX_DIAL_LIST_COUNT;i++)
            {
                cJSON * item = cJSON_GetArrayItem(data, i);
                cJSON * name = cJSON_GetObjectItem(item, "name");
                cJSON * nameChn = cJSON_GetObjectItem(item, "nameChn");
                cJSON * previewImg = cJSON_GetObjectItem(item, "previewImg");
                cJSON * bgImg = cJSON_GetObjectItem(item, "bgImg");
                cJSON * dialType = cJSON_GetObjectItem(item, "dialType");
                cJSON * file = cJSON_GetObjectItem(item, "file");

                if(dial_store_list[i] == NULL){
                    dial_store_list[i] = (ZMT_DIAL_STORE_INFO_T *)SCI_ALLOC_APPZ(sizeof(ZMT_DIAL_STORE_INFO_T));
                }
                memset(dial_store_list[i], 0, sizeof(ZMT_DIAL_STORE_INFO_T));
                SCI_MEMCPY(dial_store_list[i]->name, name->valuestring, strlen(name->valuestring));
                SCI_MEMCPY(dial_store_list[i]->name_chn, nameChn->valuestring, strlen(nameChn->valuestring));
                SCI_MEMCPY(dial_store_list[i]->preview, previewImg->valuestring, strlen(previewImg->valuestring));
                SCI_MEMCPY(dial_store_list[i]->bg, bgImg->valuestring, strlen(bgImg->valuestring));
                SCI_MEMCPY(dial_store_list[i]->file, file->valuestring, strlen(file->valuestring));
                dial_store_list[i]->type = atoi(dialType->valuestring);
            }
            dial_store_status = 3;
            ZmtDialStore_RequestPreviewIcon(0);
        }
        else
        {
            dial_store_status = 1;
        }
    }
    else
    {
        dial_store_status = 2;
    }
    if(MMK_IsFocusWin(ZMT_DIAL_STORE_WIN_ID))
    {
        MMK_SendMsg(ZMT_DIAL_STORE_WIN_ID, MSG_FULL_PAINT, PNULL);
    }
}

LOCAL void ZmtDialStore_RequestDialList(void)
{
    char url[50] = {0};
    sprintf(url, "%s%s", ZMT_HTTP_API_BASE_PATH, ZMT_HTTP_DIAL_URL_PATH);
    MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 3000, 0, 0, ZmtDialStore_RecResultCb);   
}

LOCAL void ZmtDialStore_OPEN_WINDOW(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id)
{
    GUI_RECT_T rect = {0};
    GUI_BORDER_T border={0};
    GUI_BG_T ctrl_bg = {0};
    GUI_FONT_ALL_T font_all = {0};
    GUIICONLIST_MARGINSPACE_INFO_T margin_space = {20,10,0,0};

    GUIICONLIST_SetTotalIcon(ctrl_id, dial_store_count);
    GUIICONLIST_SetCurIconIndex(0,ctrl_id);
    GUIICONLIST_SetStyle(ctrl_id,GUIICONLIST_STYLE_ICON_UIDT);
    GUIICONLIST_SetIconWidthHeight(ctrl_id,90,100);
    border.type = GUI_BORDER_NONE;
    GUIICONLIST_SetItemBorderStyle(ctrl_id,TRUE,&border);
    GUIICONLIST_SetItemBorderStyle(ctrl_id,FALSE,&border);
    GUIICONLIST_SetLoadType(ctrl_id,GUIICONLIST_LOAD_ALL);
    ctrl_bg.bg_type  = GUI_BG_COLOR;
    ctrl_bg.color   = MMI_BLACK_COLOR;
    GUIICONLIST_SetBg(ctrl_id, &ctrl_bg);
    font_all.color = MMI_WHITE_COLOR;
    font_all.font = SONG_FONT_16;
    GUIICONLIST_SetIconListTextInfo(ctrl_id, font_all);
    GUIICONLIST_SetIconItemSpace(ctrl_id,margin_space);
    GUIICONLIST_SetRect(ctrl_id,&rect);
    MMK_SetAtvCtrl(win_id, ctrl_id);
}

LOCAL MMI_RESULT_E HandleZmtDialStoreWinMsg(MMI_WIN_ID_T win_id,MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T ctrl_id = ZMT_DIAL_STORE_PREVIEW_ICONLIST_CTRL_ID;
    switch (msg_id) 
    {
        case MSG_OPEN_WINDOW:
            {
                ZmtDialStore_RequestDialList();
                ZmtDialStore_OPEN_WINDOW(win_id, ctrl_id);
            }
            break;
        case MSG_FULL_PAINT:
            {             
                ZmtDialStore_FULL_PAINT(win_id);               
            }
            break;
        case MSG_CTL_ICONLIST_APPEND_TEXT:
            {
                GUIICONLIST_DISP_T icon_list = {0};
                uint16 icon_index = *((uint16 *)param);
                wchar name[20] = {0};
                //SCI_TRACE_LOW("%s: APPEND_TEXT, icon_index = %d, dial_store_count = %d", __FUNCTION__, icon_index, dial_store_count);
                if(dial_store_count > 0)
                {
                    uint8 length = strlen(dial_store_list[icon_index]->name_chn);
                #ifdef WIN32
                    GUI_GBToWstr(name, dial_store_list[icon_index]->name_chn, length);
                #else
                    GUI_UTF8ToWstr(name, length, dial_store_list[icon_index]->name_chn, length);
                #endif                
                    icon_list.name_str.wstr_ptr = name;
                    icon_list.name_str.wstr_len= MMIAPICOM_Wstrlen(name);
                    GUIICONLIST_AppendText(icon_index, ctrl_id, &icon_list);
                }
            }
            break;
        case MSG_CTL_ICONLIST_APPEND_ICON:
            {
                GUIICONLIST_DISP_T icon_list = {0};
                uint16 icon_index = *((uint16 *)param);
                char file_str[100] = {0};
                wchar name[100] = {0};
                GUIANIM_FILE_INFO_T file_info = {0};
                //SCI_TRACE_LOW("%s: APPEND_ICON, icon_index = %d, dial_store_count = %d", __FUNCTION__, icon_index, dial_store_count);
                if(dial_store_count > 0)
                {
                    sprintf(file_str, ZMT_DIAL_PREVIEW_FILE_BASE_PATH, dial_store_list[icon_index]->name, dial_store_list[icon_index]->name);
                    MMIAPICOM_StrToWstr(file_str, name);
                    file_info.full_path_wstr_ptr = name;
                    file_info.full_path_wstr_len = MMIAPICOM_Wstrlen(name);
                    GUIICONLIST_AppendIcon(icon_index, ctrl_id, PNULL, &file_info);
                }
            }
            break;
        case MSG_CTL_OK:
        case MSG_CTL_PENOK:
        case MSG_APP_WEB:
            {
                uint16 icon_index = GUIICONLIST_GetCurIconIndex(ctrl_id);
                SCI_TRACE_LOW("%s: icon_index = %d", __FUNCTION__, icon_index);
                ZmtDialStore_CTL_PENOK(win_id, icon_index);
            }
            break;
        case MSG_KEYUP_CANCEL:
            {
                MMK_CloseWin(win_id);
            }
            break;
        case MSG_CLOSE_WINDOW:
            {
                dial_store_download_idx = 0;
                dial_store_count = 0;
                dial_store_status = 0;
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
    CREATE_ICONLIST_CTRL(ZMT_DIAL_STORE_PREVIEW_ICONLIST_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

/*
#include <zlib.h>
#define     MAX_CHUNK_LEN              (128*1024)

int UnZipcompressfile(SFS_HANDLE hSrcFile, SFS_HANDLE hDstFile)
{
    int ret;
    unsigned have;
    z_stream strm;
    SFS_ERROR_E err;
    uint32      dwRWNums;
    unsigned char *in;
    unsigned char *out;
    
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    strm.zalloc  =  wre_Zcalloc;
    strm.zfree   =  wre_Zcfree;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    in  = SCI_ALLOC_APP(MAX_CHUNK_LEN);
    out = SCI_ALLOC_APP(MAX_CHUNK_LEN);

    do {
        err = SFS_ReadFile(hSrcFile, in, MAX_CHUNK_LEN, (uint32 *)&strm.avail_in, PNULL);
        if (err != SFS_NO_ERROR) {
            (void)inflateEnd(&strm);
            SCI_FREE(in);
            SCI_FREE(out);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        do {
            strm.avail_out = MAX_CHUNK_LEN;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);

            switch (ret) {
            case Z_NEED_DICT:
                //ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                if(ret == Z_NEED_DICT)
                {
                    ret = Z_DATA_ERROR;
                }
                (void)inflateEnd(&strm);
                SCI_FREE(in);
                SCI_FREE(out);
                return ret;
            default:
                break;
            }
            have = MAX_CHUNK_LEN - strm.avail_out;

            err = SFS_WriteFile(hDstFile, (void *)out, have, &dwRWNums, PNULL);
            if((err != SFS_NO_ERROR)|| (dwRWNums != have)) {
                (void)inflateEnd(&strm);
                SCI_FREE(in);
                SCI_FREE(out);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        
    } while (ret != Z_STREAM_END);
    
    (void)inflateEnd(&strm);

    SCI_FREE(in);
    SCI_FREE(out);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int zmt_unzip(const uint16 *srcfile, uint16 *dstfile)
{
    SFS_HANDLE  fpr;
    SFS_HANDLE  fpw;

    fpr = SFS_CreateFile(srcfile, SFS_MODE_READ|SFS_MODE_OPEN_EXISTING, PNULL, PNULL);
    if(PNULL == fpr)
    {
        SCI_TRACE_LOW("unzip CreateFile srcfile failure");
        return 1;
    }
    fpw = SFS_CreateFile(dstfile, SFS_MODE_WRITE|SFS_MODE_CREATE_ALWAYS, PNULL, PNULL);
    if(PNULL == fpw)
    {
        SCI_TRACE_LOW("unzip CreateFile dstfile failure");
        SFS_CloseFile(fpr);
        return 1;
    }

    if(UnZipcompressfile(fpr, fpw) != Z_OK)
    {
        SFS_CloseFile(fpr);
        SFS_CloseFile(fpw);
        SCI_TRACE_LOW("unzip UnZipcompressfile failure");
        return 2;
    }

    SFS_CloseFile(fpr);
    SFS_CloseFile(fpw);
    return 0;
}
*/

PUBLIC void MMI_CreateZmtDialStoreWin(void)
{
    if(MMK_IsOpenWin(ZMT_DIAL_STORE_WIN_ID))
    {
        MMK_CloseWin(ZMT_DIAL_STORE_WIN_ID);
    }
    //zmt_unzip("D:\\zmt_dial\\dial_1\\dial_1_watch.zip", "D:\\zmt_dial\\dial_1");
    MMK_CreateWin((uint32 *)MMI_DIAL_STORE_WIN_TAB, PNULL);
}

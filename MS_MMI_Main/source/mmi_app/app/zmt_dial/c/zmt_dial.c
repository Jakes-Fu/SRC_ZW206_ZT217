/*****************************************************************************
** File Name:      zmt_dial.c                                           *
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

#define ZMT_ARGB_GET_A(_argb) (((_argb) >> 24) & 0xff)
#define ZMT_ARGB_GET_R(_argb) (((_argb) >> 16) & 0xff)
#define ZMT_ARGB_GET_G(_argb) (((_argb) >> 8) & 0xff)
#define ZMT_ARGB_GET_B(_argb) ((_argb) & 0xff)

#define ZMT_ARGB_SET_A(_argb, _alpha)   ((_argb) | (((_alpha) << 24) & 0xff000000))
#define ZMT_ARGB_SET_R(_argb, _r)       ((_argb) | (((_r) << 16) & 0xff0000))
#define ZMT_ARGB_SET_G(_argb, _g)       ((_argb) | (((_g) << 8) & 0xff00))
#define ZMT_ARGB_SET_B(_argb, _b)       ((_argb) | (((_b)) & 0xff))

#define ZMT_RGB565_GET_R(_color) (((_color) >> 8) & 0xf8)
#define ZMT_RGB565_GET_G(_color) (((_color) >> 3) & 0xfc)
#define ZMT_RGB565_GET_B(_color) (((_color) << 3) & 0xf8)

#define ZMT_RGB565_SET_R(_color, _r) ((_color) | (((_r) << 8) & 0xf800))
#define ZMT_RGB565_SET_G(_color, _g) ((_color) | (((_g) << 3) & 0x07e0))
#define ZMT_RGB565_SET_B(_color, _b) ((_color) | (((_b) >> 3) & 0x001f))

#define ZMT_RGB888_TO_RGB565(r, g, b)  (((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | ((b >> 3) & 0x1f))
#define ZMT_RGB8882RGB565(rgb888)   ((((rgb888)&0xf80000)>>8)|(((rgb888)&0xfc00)>>5)|(((rgb888)&0xf8)>>3))

#define ZMT_RGB5652ARGB8888(rgb565,alpha) ( ((alpha)<<24) |((((rgb565) >> 8) & 0xf8)<<16)|((((rgb565) >> 3) & 0xfc)<<8)|(((rgb565) << 3)  & 0xf8))

#define ZMT_BLEND_MASK 0x7E0F81F

typedef struct 
{
    void   *data_ptr;
    uint32  data_type;  //0: RGB565, 1: ARGB888
    uint16  width;
    uint16  height;
    int16   start_x;
    int16   start_y;
}MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T; 

watch_dial_t * zmt_watch_dial = NULL;

PUBLIC void ZMT_DialRelease(void)
{
    if(zmt_watch_dial != NULL){
        SCI_FREE(zmt_watch_dial);
        zmt_watch_dial = NULL;
    }
}

LOCAL void ZMT_DialParse(char * buf, watch_dial_t * dial)
{
    cJSON * json_root = NULL;
    cJSON * json_item = NULL;
    char * valuestring = NULL;

    if(NULL == dial) {
        return;
    }

    json_root = cJSON_Parse(buf);
    if(NULL == json_root) {
        SCI_TRACE_LOW("%s: json parse error\n", __FUNCTION__);
        return;
    }

    valuestring = cJSON_GetStringValue(cJSON_GetObjectItem(json_root, "name"));
    if(valuestring){
        SCI_MEMCPY(dial->name, valuestring, strlen(valuestring));
    }
    valuestring = cJSON_GetStringValue(cJSON_GetObjectItem(json_root, "preview_img"));
    if(valuestring){
        SCI_MEMCPY(dial->preview_img, valuestring, strlen(valuestring));
    }
    valuestring = cJSON_GetStringValue(cJSON_GetObjectItem(json_root, "bg_img"));
    if(valuestring){
        SCI_MEMCPY(dial->bg_img, valuestring, strlen(valuestring));
    }
    dial->dial_type = cJSON_GetObjectItem(json_root, "dial_type")->valueint;
    {
        //element ll
        uint8 i = 0;
        cJSON * json_element_ll = cJSON_GetObjectItem(json_root, "element_ll");
        cJSON * json_element = NULL;
        dial->element_cnt = cJSON_GetArraySize(json_element_ll);
        SCI_TRACE_LOW("%s: dial->element_cnt = %d", __FUNCTION__, dial->element_cnt);
        cJSON_ArrayForEach(json_element, json_element_ll)
        {
            dial->element_ll[i].element_type = cJSON_GetObjectItem(json_element, "element_type")->valueint;    
            dial->element_ll[i].img_cnt = cJSON_GetObjectItem(json_element, "img_cnt")->valueint;
            SCI_TRACE_LOW("%s: [%d]img_cnt = %d", __FUNCTION__, i, dial->element_ll[i].img_cnt);
            {
                //img ll
                uint8 k = 0;
                cJSON * json_img_ll = cJSON_GetObjectItem(json_element, "img_ll");
                cJSON * json_img = NULL;
                cJSON_ArrayForEach(json_img, json_img_ll)
                {
                    dial->element_ll[i].img_ll[k].img_type = cJSON_GetObjectItem(json_img, "img_type")->valueint;
                    SCI_TRACE_LOW("%s: [%d]img_ll[%d].img_type= %d", __FUNCTION__, i, k, dial->element_ll[i].img_ll[k].img_type);
                    
                    json_item = cJSON_GetObjectItem(json_img, "img_pos");
                    dial->element_ll[i].img_ll[k].img_pos.x = cJSON_GetObjectItem(json_item, "x")->valueint;
                    dial->element_ll[i].img_ll[k].img_pos.y = cJSON_GetObjectItem(json_item, "y")->valueint;
                    
                    json_item = cJSON_GetObjectItem(json_img, "img_size");
                    dial->element_ll[i].img_ll[k].img_width = cJSON_GetObjectItem(json_item, "width")->valueint;
                    dial->element_ll[i].img_ll[k].img_height = cJSON_GetObjectItem(json_item, "height")->valueint;
                    
                    dial->element_ll[i].img_ll[k].sub_img_cnt = cJSON_GetObjectItem(json_img, "sub_img_cnt")->valueint;
                    {
                        //sub img ll
                        uint8 m = 0;
                        cJSON * json_sub_img_ll = cJSON_GetObjectItem(json_img, "sub_img_ll");
                        cJSON * json_sub_img = NULL;
                        cJSON_ArrayForEach(json_sub_img, json_sub_img_ll)
                        {
                            valuestring = cJSON_GetStringValue(json_sub_img);
                            if(valuestring && strlen(valuestring) > 0){
                                SCI_TRACE_LOW("%s: strlen(valuestring) = %d", __FUNCTION__, strlen(valuestring));
                                SCI_MEMCPY(dial->element_ll[i].img_ll[k].sub_img_ll[m].sub_img, valuestring, strlen(valuestring));
                                m++;
                            }
                        }
                    }
                    k++;
                }
                dial->element_ll[i].label_cnt = cJSON_GetObjectItem(json_element, "label_cnt")->valueint;
                SCI_TRACE_LOW("%s: [%d]label_cnt = %d", __FUNCTION__, i, dial->element_ll[i].label_cnt);
                {
                    //label ll
                    uint8 k = 0;
                    cJSON * json_label_ll = cJSON_GetObjectItem(json_element, "label_ll");
                    cJSON * json_label = NULL;
                    cJSON_ArrayForEach(json_label, json_label_ll)
                    {
                        dial->element_ll[i].label_ll[k].label_type = cJSON_GetObjectItem(json_label, "label_type")->valueint;
                        SCI_TRACE_LOW("%s: [%d]label_ll[%d].label_type= %d", __FUNCTION__, i, k, dial->element_ll[i].label_ll[k].label_type);

                        valuestring = cJSON_GetStringValue(cJSON_GetObjectItem(json_label, "text"));
                        if(valuestring && strlen(valuestring) > 0){
                            SCI_MEMCPY(dial->element_ll[i].label_ll[k].text, valuestring, strlen(valuestring));
                        }
                        
                        json_item = cJSON_GetObjectItem(json_label, "text_pos");
                        dial->element_ll[i].label_ll[k].text_pos.x = cJSON_GetObjectItem(json_item, "x")->valueint;
                        dial->element_ll[i].label_ll[k].text_pos.y = cJSON_GetObjectItem(json_item, "y")->valueint;
                        
                        dial->element_ll[i].label_ll[k].font_size = cJSON_GetObjectItem(json_label, "font_size")->valueint;
                        k++;
                    }
                }
            }
        i++;
        }
    }
    cJSON_Delete(json_root);
}

PUBLIC MMI_HANDLE_T ZMT_CreateAnimImg(MMI_WIN_ID_T         win_id,
                                                           MMI_CTRL_ID_T        ctrl_id,
                                                           GUI_RECT_T*          disp_rect,
                                                           wchar*               p_image_name,
                                                           float                scale,
                                                           GUI_LCD_DEV_INFO*    p_lcd_info
                                                           )
{
    MMI_HANDLE_T            handle = 0;
    GUIANIM_CTRL_INFO_T     control_info = {0};
    GUIANIM_INIT_DATA_T    anim_init = {0};
    MMI_CONTROL_CREATE_T    anim_create = {0};
    CTRLANIM_OBJ_T*         anim_ctrl_ptr = PNULL;
    GUIANIM_RESULT_E        anim_res = GUIANIM_RESULT_MAX;
    GUIANIM_FILE_INFO_T     file_info = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    GUIANIM_DATA_INFO_T     data_info = {0};
    GUI_RECT_T win_rect = {0, 0, 0, 0};
    uint16 w, h = 0;

    if(PNULL == p_image_name)
    {
        SCI_TRACE_LOW("%s: image_name = pnull!", __FUNCTION__);
        return handle;
    }

    if(PNULL == p_lcd_info)
    {
        SCI_TRACE_LOW("%s: p_lcd_info = pnull!", __FUNCTION__);
        return handle;
    }

     if(PNULL == p_image_name)
    {
        SCI_TRACE_LOW("%s: image_name = pnull!", __FUNCTION__);
        return handle;
    }

     //set file info
    file_info.full_path_wstr_ptr = SCI_ALLOC_APPZ(sizeof(wchar)*(WATCH_IMAGE_FULL_PATH_MAX_LEN + 1));
    if (PNULL == file_info.full_path_wstr_ptr)
    {
        SCI_TRACE_LOW("%s: full_path_wstr_ptr = pnull!", __FUNCTION__);
        return GUIANIM_RESULT_EMPTY;
    }
    SCI_MEMSET(file_info.full_path_wstr_ptr,0,(sizeof(wchar)*(WATCH_IMAGE_FULL_PATH_MAX_LEN + 1)));

    file_info.full_path_wstr_len = MMIAPICOM_Wstrlen(p_image_name);
    MMI_WSTRNCPY(file_info.full_path_wstr_ptr,sizeof(wchar)*(file_info.full_path_wstr_len+1),
        p_image_name,file_info.full_path_wstr_len,
        file_info.full_path_wstr_len);

    //create anim
    if(!MMK_GetCtrlHandleByWin(win_id, ctrl_id))
    {
        MMK_DestroyControl(ctrl_id);

        anim_init.both_rect.h_rect = anim_init.both_rect.v_rect = *disp_rect;
              
        anim_create.guid = SPRD_GUI_ANIM_ID;
        anim_create.ctrl_id = ctrl_id;
        anim_create.init_data_ptr = &anim_init;
        anim_create.parent_win_handle = win_id;
        anim_ctrl_ptr = (CTRLANIM_OBJ_T*)MMK_CreateControl(&anim_create);

        control_info.is_ctrl_id = TRUE;
        control_info.ctrl_id = ctrl_id;
        control_info.ctrl_ptr = anim_ctrl_ptr;
    }
    else
    {
        control_info.is_ctrl_id = TRUE;
        control_info.ctrl_id = ctrl_id;
    }

    display_info.is_update = TRUE;
    display_info.align_style = GUIANIM_ALIGN_HVMIDDLE;
    display_info.is_disp_one_frame = TRUE;
    display_info.is_crop_rect = TRUE;
    display_info.is_syn_decode = TRUE;
    display_info.is_handle_transparent = TRUE;
    display_info.is_auto_zoom_in = TRUE;
    display_info.is_zoom = (1 == scale) ? FALSE : TRUE;
    
    //set anim lcd
    GUIAPICTRL_SetLcdDevInfo(ctrl_id, p_lcd_info);

    //set anim rect
    GUIAPICTRL_SetRect(ctrl_id, disp_rect);
    
    //set default icon
    GUIANIM_SetDefaultIcon(ctrl_id, PNULL, PNULL);
    
    //set bg anim
    anim_res = GUIANIM_SetParam(&control_info, PNULL, &file_info, &display_info);
    
    if (PNULL != file_info.full_path_wstr_ptr)
    {
        SCI_FREE(file_info.full_path_wstr_ptr);
    }
    SCI_TRACE_LOW("%s: end", __FUNCTION__);
}

LOCAL void ZMT_CalculatePointerAngles(int hour, 
                                                                        int minute, 
                                                                        int second, 
                                                                        uint16* hourAngle, 
                                                                        uint16* minuteAngle, 
                                                                        uint16* secondAngle
                                                                        ) 
{
    // 计算时针角度
if(hourAngle != PNULL)
    *hourAngle =  ((hour % 12) * 30);
    
    // 计算分针角度
if(minuteAngle != PNULL)
    *minuteAngle = 6 * minute;
    
    // 计算秒针角度
if(secondAngle != PNULL)
    *secondAngle = 6 * second;
}

PUBLIC BOOLEAN ZMT_GetImgInfoByPath(wchar* p_full_path_wstr,
                                                                 GUIIMG_INFO_T* p_img_info
                                                                 )
{
    BOOLEAN             result = TRUE;
    IMG_DEC_RET_E       get_result = IMG_DEC_RET_SUCCESS;
    IMG_DEC_SRC_T       dec_src = {0};
    IMG_DEC_INFO_T      dec_info = {0};

    if(PNULL == p_img_info || PNULL == p_full_path_wstr)
    {
        SCI_TRACE_LOW("%s: full_path_wstr_ptr = pnull or img_info_ptr = pnull!", __FUNCTION__);
        return FALSE;
    }

    //set decode src info
    dec_src.file_name_ptr = p_full_path_wstr;

    //get image info
    get_result = IMG_DEC_GetInfo(&dec_src, &dec_info);
    SCI_TRACE_LOW("%s:get_result = %d, img_type = %d", __FUNCTION__, get_result, dec_info.img_type);

    if (IMG_DEC_RET_SUCCESS == get_result)
    {
        //set image info
        switch (dec_info.img_type)
        {
            case IMG_DEC_TYPE_BMP:
            {
                p_img_info->image_width  = dec_info.img_detail_info.bmp_info.img_width;
                p_img_info->image_height = dec_info.img_detail_info.bmp_info.img_height;
                break;
            }
            case IMG_DEC_TYPE_WBMP:
            {
                p_img_info->image_width  = dec_info.img_detail_info.wbmp_info.img_width;
                p_img_info->image_height = dec_info.img_detail_info.wbmp_info.img_height;
                break;
            }
            case IMG_DEC_TYPE_JPEG:
            {
                p_img_info->image_width  = dec_info.img_detail_info.jpeg_info.img_width;
                p_img_info->image_height = dec_info.img_detail_info.jpeg_info.img_heigth;
                break;
            }
            case IMG_DEC_TYPE_PNG:
            {
                p_img_info->image_width  = dec_info.img_detail_info.png_info.img_width;
                p_img_info->image_height = dec_info.img_detail_info.png_info.img_height;
                break;
            }
            case IMG_DEC_TYPE_GIF:
            {
                p_img_info->image_width  = dec_info.img_detail_info.gif_info.img_width;
                p_img_info->image_height = dec_info.img_detail_info.gif_info.img_height;
                break;
            }
            case IMG_DEC_TYPE_UNKNOWN:
                break;

            default:
                result = FALSE;
                break;
        }
    }
    else
    {
        result = FALSE;
    }
    return result;
}

LOCAL __inline uint16 ZMT_BlendRGB565(uint16 bk_color, uint16 fore_color, uint8 alpha)
{
    uint32 blend_color = 0;
    uint32 temp_back_color = 0;
    uint32 temp_fore_color = 0;

    alpha = (alpha + 1) >> 3;

    temp_fore_color = ( fore_color | (fore_color << 16)) & ZMT_BLEND_MASK;
    temp_back_color = ( bk_color | (bk_color << 16)) & ZMT_BLEND_MASK;
    blend_color = ((((temp_fore_color - temp_back_color) * alpha ) >> 5 ) + temp_back_color) & ZMT_BLEND_MASK;
    blend_color = ((blend_color & 0xFFFF) | (blend_color >> 16));

    return (uint16)blend_color;
}

LOCAL BOOLEAN ZMT_ProcessAlphaBlending_EX(MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *p_bk_img, MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T *p_fore_img)
{
    uint32      fore_width  = 0;
    uint32      fore_height = 0;
    uint32      bk_width = 0;
    uint32      bk_height = 0;
    int32       start_x = 0;
    int32       start_y = 0;
    uint32      blend_width = 0;
    uint32      blend_height = 0;
    uint32*     fore_ptr = PNULL;
    GUI_RECT_T  dst_rect = {0};
    GUI_RECT_T  bk_rect = {0};
    GUI_RECT_T  for_rect = {0};

    if ((PNULL == p_bk_img) || (PNULL == p_fore_img))
    {
        SCI_TRACE_LOW("%s: p_bk_img = pnull or p_fore_img = pnull!", __FUNCTION__);
        return FALSE;
    }

    fore_width  = p_fore_img->width;
    fore_height = p_fore_img->height;
    bk_width    = p_bk_img->width;
    bk_height   = p_bk_img->height;
    start_x     = p_fore_img->start_x;
    start_y     = p_fore_img->start_y;
    blend_width = fore_width;
    blend_height = fore_height;
    fore_ptr    = (uint32 *)p_fore_img->data_ptr;

    bk_rect.left  = p_bk_img->start_x;
    bk_rect.top   = p_bk_img->start_y;
    bk_rect.right = p_bk_img->start_x + p_bk_img->width;
    bk_rect.bottom = p_bk_img->start_y + p_bk_img->height;

    for_rect.left  = p_fore_img->start_x;
    for_rect.top   = p_fore_img->start_y;
    for_rect.right = p_fore_img->start_x + p_fore_img->width;
    for_rect.bottom = p_fore_img->start_y + p_fore_img->height;

    if (FALSE == GUI_IntersectRect(&dst_rect, bk_rect, for_rect))
    {
        SCI_TRACE_LOW("%s: rect error.", __FUNCTION__);
        return FALSE;
    }

    if (start_y < 0)
    {
        start_y = -start_y;
        blend_height -= start_y;
        fore_ptr += (start_y * fore_width);
        start_y = 0;
    }

    if (start_x < 0)
    {
        start_x = -start_x;
        blend_width -= start_x;
        fore_ptr += start_x;
        start_x = 0;
    }

    if ((int32)bk_width < start_x || (int32)bk_height < start_y)
    {
        SCI_TRACE_LOW("%s: (bk_width < start_x) or (bk_height < start_y)", __FUNCTION__);
        return FALSE;
    }

    blend_width = (start_x + blend_width) > bk_width ? (bk_width - start_x) : blend_width;
    blend_height = (start_y + blend_height) > bk_height ? (bk_height - start_y) : blend_height;

    if (IMGREF_FORMAT_RGB565 == p_bk_img->data_type) //output RGB565
    {
        uint32  i = 0, j = 0;
        uint16* bk_ptr = (uint16 *)p_bk_img->data_ptr;

        bk_ptr += (start_y * bk_width + start_x);

        for (i = 0; i < blend_height; i++)
        {
            for (j = 0; j < blend_width; j++)
            {
                uint32 alpha, r, g, b;
                uint32 fore_value = *(fore_ptr + j);
                uint16 bk_value = *(bk_ptr + j);

                alpha = ZMT_ARGB_GET_A(fore_value);
                r = ZMT_ARGB_GET_R(fore_value);
                g = ZMT_ARGB_GET_G(fore_value);
                b = ZMT_ARGB_GET_B(fore_value);

                fore_value = ZMT_RGB888_TO_RGB565(r, g, b);
                bk_value = ZMT_BlendRGB565(bk_value, (uint16)fore_value, alpha);
                *(bk_ptr + j) = bk_value;
            }

            bk_ptr += bk_width;
            fore_ptr += fore_width;
        }
    }
    else  //output ARGB888
    {
        uint32 i = 0, j = 0;
        uint32 *bk_ptr = (uint32 *)p_bk_img->data_ptr;

        bk_ptr += (start_y * bk_width + start_x);

        for (i = 0; i < blend_height; i++)
        {
            for (j = 0; j < blend_width; j++)
            {
                uint32 back_pix = *(bk_ptr+j);
                uint32 fore_pix = *(fore_ptr + j);
                uint32 fa = ZMT_ARGB_GET_A(fore_pix);//alpha of foreground image
                uint32 fr; //R of fore img
                uint32 fg; //G of fore img
                uint32 fb; //B of fore img
                uint32 ba = ZMT_ARGB_GET_A(back_pix);//alpha of background image.
                uint32 br; //R of bg img
                uint32 bg; //G of bg img
                uint32 bb; //B of bg img
                uint32 desta;
                uint32 destr;
                uint32 destg;
                uint32 destb;
                if (0 == ba)
                {
                    *(bk_ptr + j) = *(fore_ptr + j);
                    continue;
                }
                else if(0 == fa)
                {
                    continue;
                }
                else if(fa == 255)
                {
                    *(bk_ptr + j) = *(fore_ptr + j);
                    continue;
                }
                else
                {
                    fr = ZMT_ARGB_GET_R(fore_pix);
                    fg = ZMT_ARGB_GET_G(fore_pix);
                    fb = ZMT_ARGB_GET_B(fore_pix);
                    br = ZMT_ARGB_GET_R(back_pix);
                    bg = ZMT_ARGB_GET_G(back_pix);
                    bb = ZMT_ARGB_GET_B(back_pix);
                    desta = fa + ba - (fa*ba/255); //alpha = alpha1 + alpha2 - alpha1*alpha2
                    destr = (fr*fa+br*ba-(br*fa*ba/255))/desta; //c12 = (c1a1(1-a2)+c2a2)/alpha  1:bg, 2:fg
                    destg = (fg*fa+bg*ba-(bg*fa*ba/255))/desta;
                    destb = (fb*fa+bb*ba-(bb*fa*ba/255))/desta;
                    *(bk_ptr+j) = (desta<<24) | (destr<<16) | (destg<<8) | destb;
                }
            }
            bk_ptr += p_bk_img->width;
            fore_ptr += p_fore_img->width;
        }
    }
    return TRUE;
}

LOCAL BOOLEAN ZMT_ImageDecodeByPath(IMG_PROCESS_DATA_T decode_data, wchar* p_img_path)
{
    BOOLEAN             result          = FALSE;
    GUIIMG_SRC_T        src_info        = {0};
    GUIIMG_DISPLAY_T    img_display     = {0};
    GUIIMG_DEC_OUT_T    img_output      = {0};

    if (PNULL == decode_data.data_ptr)
    {
        SCI_TRACE_LOW("%s: decode_data.data_ptr == PNULL.", __FUNCTION__);
        return result;
    }

    if (PNULL == p_img_path)
    {
        SCI_TRACE_LOW("%s: p_img_path == PNULL.", __FUNCTION__);
        return result;
    }

    //set src info
    src_info.is_file = TRUE;
    src_info.full_path_ptr = p_img_path;
    //set display info
    img_display.is_handle_transparent = TRUE;
    img_display.dest_width  = decode_data.width;
    img_display.dest_height = decode_data.height;
    //set output data
    img_output.actual_width = decode_data.width;
    img_output.actual_height = decode_data.height;
    img_output.decode_data_ptr  = decode_data.data_ptr;
    img_output.decode_data_size = decode_data.data_size;

    result = GUIIMG_Decode(&src_info, &img_display, &img_output);
    return result;
}

LOCAL BOOLEAN ZMT_ImageExtendToSquare(IMG_PROCESS_DATA_T src_data, IMG_PROCESS_DATA_T dst_data)
{
    BOOLEAN                 result = FALSE;
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T   bk = {0};
    MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T   fg = {0};

    if (PNULL == src_data.data_ptr)
    {
        SCI_TRACE_LOW("%s: src_data.data_ptr == PNULL.", __FUNCTION__);
        return result;
    }

    if (PNULL == dst_data.data_ptr)
    {
        SCI_TRACE_LOW("%s: dst_data.data_ptr == PNULL.", __FUNCTION__);
        return result;
    }

    fg.data_ptr = src_data.data_ptr;
    fg.data_type = 1;
    fg.start_x = (dst_data.width / 2) - (src_data.width / 2);
    fg.start_y = 0;
    fg.width = src_data.width;
    fg.height = src_data.height;

    bk.data_ptr = dst_data.data_ptr;
    bk.data_type = 1;
    bk.start_x = 0;
    bk.start_y = 0;
    bk.width = dst_data.width;
    bk.height = dst_data.height;

    result = ZMT_ProcessAlphaBlending_EX(&bk, &fg);
    return result;
}

LOCAL BOOLEAN ZMT_ImageScaleToDstSize(IMG_PROCESS_DATA_T src_data, IMG_PROCESS_DATA_T dst_data)
{
    SCALE_IMAGE_IN_T    scale_in = {0};
    SCALE_IMAGE_OUT_T   scale_out = {0};

    if (PNULL == src_data.data_ptr)
    {
        SCI_TRACE_LOW("%s: src_data.data_ptr == PNULL.", __FUNCTION__);
        return FALSE;
    }

    if (PNULL == dst_data.data_ptr)
    {
        SCI_TRACE_LOW("%s: dst_data.data_ptr == PNULL.", __FUNCTION__);
        return FALSE;
    }

    //set src width and height
    scale_in.src_size.w = src_data.width;
    scale_in.src_size.h = src_data.height;
    //trim rect of src image
    scale_in.src_trim_rect.x = 0;
    scale_in.src_trim_rect.y = 0;
    scale_in.src_trim_rect.w = src_data.width;
    scale_in.src_trim_rect.h = src_data.height;
    //set src format
    scale_in.src_format = IMGREF_FORMAT_ARGB888;
    //set src buffer
    scale_in.src_chn.chn0.ptr = src_data.data_ptr;
    scale_in.src_chn.chn0.size = src_data.data_size;
    //set target width and height
    scale_in.target_size.w = dst_data.width;
    scale_in.target_size.h = dst_data.height;
    //set target format
    scale_in.target_format = IMGREF_FORMAT_ARGB888;
    //set target buffer
    scale_in.target_buf.ptr = dst_data.data_ptr;
    scale_in.target_buf.size = dst_data.data_size;
    scale_in.scale_mode = SCALE_MODE_NO_DISTORT;

    if (SCI_SUCCESS != GRAPH_ScaleImage(&scale_in, &scale_out))
    {
        SCI_TRACE_LOW("%s: GRAPH_ScaleImage failed.", __FUNCTION__);
        return FALSE;
    }
    return TRUE;
}

LOCAL BOOLEAN ZMT_ImageRotateWithAngle(IMG_PROCESS_DATA_T src_data, IMG_PROCESS_DATA_T dst_data, uint32 angle)
{
    ROTATE_ARBITRARY_IN_T   rot_in = {0};
    ROTATE_ARBITRARY_OUT_T  rot_out = {0};

    if (PNULL == src_data.data_ptr)
    {
        SCI_TRACE_LOW("%s: src_data.data_ptr == PNULL.", __FUNCTION__);
        return FALSE;
    }

    if (PNULL == dst_data.data_ptr)
    {
        SCI_TRACE_LOW("%s: dst_data.data_ptr == PNULL.", __FUNCTION__);
        return FALSE;
    }

    //set rot angle
    rot_in.angle = angle;
    //set src info
    rot_in.src_format = IMGREF_FORMAT_ARGB888;
    rot_in.src_size.w = src_data.width;
    rot_in.src_size.h = src_data.height;
    rot_in.src_center.x = src_data.width >> 1;
    rot_in.src_center.y = src_data.height >> 1;
    rot_in.src_chn.chn0.ptr = src_data.data_ptr;
    rot_in.src_chn.chn0.size = src_data.data_size;
    //set target info
    rot_in.target_format = IMGREF_FORMAT_ARGB888;
    rot_in.target_size.w = dst_data.width;
    rot_in.target_size.h = dst_data.height;
    rot_in.target_chn.chn0.ptr = dst_data.data_ptr;
    rot_in.target_chn.chn0.size = dst_data.data_size;
    rot_in.target_center.x = dst_data.width >> 1;
    rot_in.target_center.y = dst_data.height >> 1;
    //set mode
    rot_in.mode = ROTATE_ARBITRARY_AUTO_CROP;

    if (SCI_SUCCESS != GRAPH_RotateArbitrary(&rot_in, &rot_out))
    {
        SCI_TRACE_LOW("%s: GRAPH_RotateArbitrary fail.", __FUNCTION__);
        return FALSE;
    }
    return TRUE;
}

LOCAL BOOLEAN ZMT_DecodeImageToBuffer(uint8*      dst_buf,
                                                        uint16      dst_width,
                                                        uint16      dst_height,
                                                        wchar*      p_img_hand
                                                        )
{
    GUIIMG_INFO_T       img_info = {0};
    uint16              img_width = 0;
    uint16              img_height = 0;
    IMG_PROCESS_DATA_T  decode_data = {0};
    IMG_PROCESS_DATA_T  extend_data = {0};
    IMG_PROCESS_DATA_T  scale_data = {0};
    IMG_PROCESS_DATA_T  rotate_data = {0};
    uint8*              p_decode_buf = PNULL;
    uint32              decode_buf_size = 0;
    uint32              extend_buf_size = 0;
    uint16              extend_buf_width = 0;

    if (PNULL == dst_buf)
    {
        SCI_TRACE_LOW("%s: dst_buf == PNULL.", __FUNCTION__);
        return FALSE;
    }

    if (PNULL == p_img_hand)
    {
        SCI_TRACE_LOW("%s: p_img_hand == PNULL.", __FUNCTION__);
        return FALSE;
    }

    //step1. get image base info
    img_width = dst_width;
    img_height = dst_height;
    SCI_TRACE_LOW("%s: img_width = %d, img_height = %d", __FUNCTION__, img_width, img_height);
    //step2. decode
    decode_buf_size = img_width * img_height * sizeof(uint32);
    p_decode_buf = (uint8 *)SCI_ALLOC_APPZ(decode_buf_size);
    if (PNULL == p_decode_buf)
    {
        SCI_TRACE_LOW("%s: malloc p_decode_buf error.", __FUNCTION__);
        return FALSE;
    }

    decode_data.width = img_width;
    decode_data.height = img_height;
    decode_data.data_ptr = p_decode_buf;
    decode_data.data_size = decode_buf_size;
    if(FALSE == ZMT_ImageDecodeByPath(decode_data, p_img_hand))
    {
        SCI_TRACE_LOW("%s: Image decode fail.", __FUNCTION__);
        SCI_FREE(p_decode_buf);
        return FALSE;
    }

    //step3 .rotate image && draw it to dstbuf
    {
        BOOLEAN rotate_result = FALSE;

        rotate_data.width = dst_width;
        rotate_data.height = dst_height;
        rotate_data.data_ptr = dst_buf;
        rotate_data.data_size = dst_width * dst_height * sizeof(uint32);
        
        //scale decode_data
        rotate_result = ZMT_ImageRotateWithAngle(decode_data, rotate_data, 0);

        if(FALSE == rotate_result)
        {
            SCI_TRACE_LOW("%s: Image rotate fail.", __FUNCTION__);
            SCI_FREE(p_decode_buf);
            return FALSE;
        }
    }
    SCI_FREE(p_decode_buf);
    return TRUE;
}

LOCAL BOOLEAN ZMT_DecodeImageToBufferWithAngle(uint8*      dst_buf,
                                                        uint16      dst_width,
                                                        uint16      dst_height,
                                                        wchar*      p_img_hand,
                                                        uint32      angle
                                                        )
{
    GUIIMG_INFO_T       img_info = {0};
    uint16              img_width = 0;
    uint16              img_height = 0;
    BOOLEAN             is_need_extend = FALSE;
    BOOLEAN             is_need_scale = FALSE;
    IMG_PROCESS_DATA_T  decode_data = {0};
    IMG_PROCESS_DATA_T  extend_data = {0};
    IMG_PROCESS_DATA_T  scale_data = {0};
    IMG_PROCESS_DATA_T  rotate_data = {0};
    uint8*              p_decode_buf = PNULL;
    uint32              decode_buf_size = 0;
    uint8*              p_extend_buf = PNULL;
    uint32              extend_buf_size = 0;
    uint16              extend_buf_width = 0;
    uint8*              p_scale_buf = PNULL;
    uint32              scale_buf_size = 0;

    if (PNULL == dst_buf)
    {
        SCI_TRACE_LOW("%s: dst_buf == PNULL.", __FUNCTION__);
        return FALSE;
    }

    if (PNULL == p_img_hand)
    {
        SCI_TRACE_LOW("%s: p_img_hand == PNULL.", __FUNCTION__);
        return FALSE;
    }

    //step1. get image base info
    ZMT_GetImgInfoByPath(p_img_hand, &img_info);
    img_width = img_info.image_width;
    img_height = img_info.image_height;

    if(img_width != img_height)
    {
        SCI_TRACE_LOW("%s: img need extend, img_width = %d, img_height = %d", __FUNCTION__, img_width, img_height);
        is_need_extend = TRUE;
    }

    if ((dst_width != img_width) || (dst_height != img_height))
    {
        SCI_TRACE_LOW("%s: img need scale, dst_height = %d, img_height = %d", __FUNCTION__, dst_height, img_height);
        is_need_scale = TRUE;
    }

    //step2. decode
    decode_buf_size = img_width * img_height * sizeof(uint32);
    p_decode_buf = (uint8 *)SCI_ALLOC_APPZ(decode_buf_size);
    if (PNULL == p_decode_buf)
    {
        SCI_TRACE_LOW("%s: malloc p_decode_buf error.", __FUNCTION__);
        return FALSE;
    }

    decode_data.width = img_width;
    decode_data.height = img_height;
    decode_data.data_ptr = p_decode_buf;
    decode_data.data_size = decode_buf_size;
    if(FALSE == ZMT_ImageDecodeByPath(decode_data, p_img_hand))
    {
        SCI_TRACE_LOW("%s: Image decode fail.", __FUNCTION__);
        SCI_FREE(p_decode_buf);
        return FALSE;
    }

    //step3. extend
    if (TRUE == is_need_extend)
    {
        extend_buf_width = (img_width > img_height) ? img_width : img_height;
        extend_buf_size = extend_buf_width * extend_buf_width * sizeof(uint32);
        p_extend_buf = (uint8 *)SCI_ALLOC_APPZ(extend_buf_size);
        if (PNULL == p_extend_buf)
        {
            SCI_TRACE_LOW("%s: malloc p_extend_buf error.", __FUNCTION__);
            SCI_FREE(p_decode_buf);
            return FALSE;
        }

        extend_data.width = extend_buf_width;
        extend_data.height = extend_buf_width;
        extend_data.data_ptr = p_extend_buf;
        extend_data.data_size = extend_buf_size;
        if(FALSE == ZMT_ImageExtendToSquare(decode_data, extend_data))
        {
            SCI_TRACE_LOW("%s: Image extend fail.", __FUNCTION__);
            SCI_FREE(p_decode_buf);
            SCI_FREE(p_extend_buf);
            return FALSE;
        }
    }

    //step4 .scale image
    if (TRUE == is_need_scale)
    {
        BOOLEAN scale_result = FALSE;

        scale_buf_size = dst_width * dst_height * sizeof(uint32);
        p_scale_buf = (uint8 *)SCI_ALLOC_APPZ(scale_buf_size);
        if (PNULL == p_scale_buf)
        {
            SCI_TRACE_LOW("%s: malloc p_scale_buf error.", __FUNCTION__);
            SCI_FREE(p_decode_buf);
            SCI_FREE(p_extend_buf);
            return FALSE;
        }

        scale_data.width = dst_width;
        scale_data.height = dst_height;
        scale_data.data_ptr = p_scale_buf;
        scale_data.data_size = scale_buf_size;

        if (TRUE == is_need_extend)
        {
            //scale extend_data
            scale_result = ZMT_ImageScaleToDstSize(extend_data, scale_data);
        }
        else
        {
            //scale decode_data
            scale_result = ZMT_ImageScaleToDstSize(decode_data, scale_data);
        }

        if(FALSE == scale_result)
        {
            SCI_TRACE_LOW("%s: Image scale fail.", __FUNCTION__);
            SCI_FREE(p_decode_buf);
            SCI_FREE(p_extend_buf);
            SCI_FREE(p_scale_buf);
            return FALSE;
        }
    }

    //step5 .rotate image && draw it to dstbuf
    {
        BOOLEAN rotate_result = FALSE;

        rotate_data.width = dst_width;
        rotate_data.height = dst_height;
        rotate_data.data_ptr = dst_buf;
        rotate_data.data_size = dst_width * dst_height * sizeof(uint32);

        if (TRUE == is_need_scale)
        {
            //rotate scale_data
            rotate_result = ZMT_ImageRotateWithAngle(scale_data, rotate_data, angle);
        }
        else if (TRUE == is_need_extend)
        {
            //scale extend_data
            rotate_result = ZMT_ImageRotateWithAngle(extend_data, rotate_data, angle);
        }
        else
        {
            //scale decode_data
            rotate_result = ZMT_ImageRotateWithAngle(decode_data, rotate_data, angle);
        }

        if(FALSE == rotate_result)
        {
            SCI_TRACE_LOW("%s: Image rotate fail.", __FUNCTION__);
            SCI_FREE(p_decode_buf);
            SCI_FREE(p_extend_buf);
            SCI_FREE(p_scale_buf);
            return FALSE;
        }
    }
    SCI_FREE(p_decode_buf);
    SCI_FREE(p_extend_buf);
    SCI_FREE(p_scale_buf);
    return TRUE;
}

LOCAL void ZMT_CreateAnalogTime(GUI_LCD_DEV_INFO * p_lcd_info,
                                                GUI_RECT_T * disp_rect,
                                                wchar* p_image_hour,
                                                wchar* p_image_minute,
                                                wchar* p_image_second
                                                )
{
#ifdef UI_MULTILAYER_SUPPORT
    uint8               i = 0;
    SCI_TIME_T          sys_time= {0};
    uint16              angle_arr[3] = {0};
    uint8*              p_rotate_buf = PNULL;
    uint32              rotate_buf_size = 0;
    uint16              rect_width = 0;
    uint16              rect_height = 0;
    wchar *            p_hand[3];

    rect_width = disp_rect->right - disp_rect->left;
    rect_height = disp_rect->bottom - disp_rect->top;
    SCI_TRACE_LOW("%s: draw analog clock", __FUNCTION__);
    //get system time
    TM_GetSysTime(&sys_time);
#if defined(UI_MULTILAYER_SUPPORT) && !defined(WF_USE_SINGLELAYER) 
    //clear layer
    UILAYER_Clear(p_lcd_info);
#endif
#ifdef WIN32
    do
    {
        LOCAL uint8 localsec = 0;
        localsec++;
        localsec %= 60;
        sys_time.sec= localsec;
    } while (0);
#endif
	SCI_TRACE_LOW("%s: sys_time.hour = %d, sys_time.min = %d, sys_time.sec = %d", __FUNCTION__, sys_time.hour, sys_time.min, sys_time.sec);
    //get angle by time
    ZMT_CalculatePointerAngles(sys_time.hour, sys_time.min, sys_time.sec, &angle_arr[0], &angle_arr[1], &angle_arr[2]);
    SCI_TRACE_LOW("%s: angle_arr[0] = %d, angle_arr[1] = %d, angle_arr[2] = %d", __FUNCTION__, angle_arr[0], angle_arr[1], angle_arr[2]);
    //malloc rotate buf
    rotate_buf_size = rect_width * rect_height * sizeof(uint32);
    if (PNULL == p_rotate_buf)
    {
        p_rotate_buf = SCI_ALLOC_APPZ(rotate_buf_size);
        SCI_TRACE_LOW("%s: malloc p_rotate_buf, addr = 0x%0x.", __FUNCTION__, (uint32)p_rotate_buf);
        if (PNULL == p_rotate_buf)
        {
            SCI_TRACE_LOW("%s: malloc p_rotate_buf error.", __FUNCTION__);
            return;
        }
    }
    
    //set image path
    p_hand[0] = p_image_hour;
    p_hand[1] = p_image_minute;
    p_hand[2] = p_image_second;
    
    //draw analog clock
    for (i = 0; i < 3; i++)
    {
        MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T   bk = {0};
        MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T   fg = {0};
        UILAYER_INFO_T  layer_info;
        SCI_MEMSET(p_rotate_buf, 0, rotate_buf_size);
        ZMT_DecodeImageToBufferWithAngle(p_rotate_buf, rect_width, rect_height, p_hand[i], angle_arr[i]);
        UILAYER_GetLayerInfo(p_lcd_info, &layer_info);
        if(layer_info.data_type!=DATA_TYPE_ARGB888 && layer_info.data_type!=DATA_TYPE_RGB565)
        {
            break;
        }
        bk.data_ptr = UILAYER_GetLayerBufferPtr(p_lcd_info);
        bk.data_type = (layer_info.data_type==DATA_TYPE_ARGB888) ? IMGREF_FORMAT_ARGB888 : IMGREF_FORMAT_RGB565;//only support argb888& rgb565
        bk.start_x = 0;
        bk.start_y = 0;
        bk.width = rect_width;
        bk.height = rect_height;

        fg.data_ptr = p_rotate_buf;
        fg.data_type = 1;
        fg.start_x = 0;
        fg.start_y = 0;
        fg.width = rect_width;
        fg.height = rect_height;
        ZMT_ProcessAlphaBlending_EX(&bk, &fg);
    }

    //free rotate buf
    if (PNULL != p_rotate_buf)
    {
        SCI_TRACE_LOW("%s: free p_rotate_buf, addr = 0x%0x.", __FUNCTION__, (uint32)p_rotate_buf);
        SCI_FREE(p_rotate_buf);
    }
#endif
}

PUBLIC void ZMT_CreateElementImg(GUI_LCD_DEV_INFO * p_lcd_info,
                                                GUI_RECT_T*          disp_rect,
                                                wchar*               p_image_path
                                                )
{
#ifdef UI_MULTILAYER_SUPPORT
    uint8               i = 0;
    SCI_TIME_T          sys_time= {0};
    uint16              angle_arr[3] = {0};
    uint8*              p_rotate_buf = PNULL;
    uint32              rotate_buf_size = 0;
    uint16              rect_width = 0;
    uint16              rect_height = 0;
    GUIIMG_INFO_T       img_info = {0};

    //SCI_TRACE_LOW("%s: p_image_path = %ls", __FUNCTION__, p_image_path);
    ZMT_GetImgInfoByPath(p_image_path, &img_info);
    rect_width = img_info.image_width;
    rect_height = img_info.image_height;
    SCI_TRACE_LOW("%s: rect_width = %d, rect_height = %d", __FUNCTION__, rect_width, rect_height);
#if defined(UI_MULTILAYER_SUPPORT) && !defined(WF_USE_SINGLELAYER) 
    //clear layer
    UILAYER_Clear(p_lcd_info);
#endif
    rotate_buf_size = rect_width * rect_height * sizeof(uint32);
    if (PNULL == p_rotate_buf)
    {
        p_rotate_buf = SCI_ALLOC_APPZ(rotate_buf_size);
        SCI_TRACE_LOW("%s: malloc p_rotate_buf, addr = 0x%0x.", __FUNCTION__, (uint32)p_rotate_buf);
        if (PNULL == p_rotate_buf)
        {
            SCI_TRACE_LOW("%s: malloc p_rotate_buf error.", __FUNCTION__);
            return;
        }
    }
    {
        MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T   bk = {0};
        MMI_WATCHIDLE_CLOCK_ALPHA_IMG_T   fg = {0};
        UILAYER_INFO_T  layer_info;
        SCI_MEMSET(p_rotate_buf, 0, rotate_buf_size);
        ZMT_DecodeImageToBuffer(p_rotate_buf, rect_width, rect_height, p_image_path);
        UILAYER_GetLayerInfo(p_lcd_info, &layer_info);
        if(layer_info.data_type!=DATA_TYPE_ARGB888 && layer_info.data_type!=DATA_TYPE_RGB565)
        {
            return;
        }
        bk.data_ptr = UILAYER_GetLayerBufferPtr(p_lcd_info);
        bk.data_type = (layer_info.data_type==DATA_TYPE_ARGB888) ? IMGREF_FORMAT_ARGB888 : IMGREF_FORMAT_RGB565;//only support argb888& rgb565
        bk.start_x = disp_rect->left;
        bk.start_y = disp_rect->top;
        bk.width = rect_width;
        bk.height = rect_height;

        fg.data_ptr = p_rotate_buf;
        fg.data_type = 1;
        fg.start_x = disp_rect->left;
        fg.start_y = disp_rect->top;
        fg.width = rect_width;
        fg.height = rect_height;
        ZMT_ProcessAlphaBlending_EX(&bk, &fg);
    }

    //free rotate buf
    if (PNULL != p_rotate_buf)
    {
        SCI_TRACE_LOW("%s: free p_rotate_buf, addr = 0x%0x.", __FUNCTION__, (uint32)p_rotate_buf);
        SCI_FREE(p_rotate_buf);
    }
#endif
}

PUBLIC void ZMT_DialPanelShow(MMI_WIN_ID_T win_id, char * watch_name)
{
    GUI_LCD_DEV_INFO lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_CTRL_ID_T ctrl_id = 0;
    GUI_RECT_T win_rect = {0, 0, MMI_MAINSCREEN_WIDTH, MMI_MAINSCREEN_HEIGHT};
    GUI_RECT_T anim_rect = {0};
    MMI_STRING_T text_string = {0};
    GUISTR_STATE_T text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_ELLIPSIS_EX;
    GUISTR_STYLE_T text_style = {0};
    GUI_POINT_T point = {0};
    wchar image_name[WATCH_IMAGE_FULL_PATH_MAX_LEN] ={0};
    char image_str[WATCH_IMAGE_FULL_PATH_MAX_LEN] = {0};
    wchar text_name[WATCH_IMAGE_FULL_PATH_MAX_LEN] ={0};
    char text_str[WATCH_IMAGE_FULL_PATH_MAX_LEN] = {0};
    char str_name[WATCH_IMAGE_FULL_PATH_MAX_LEN] = {0};
    char dir_str[20] = {0};
    uint8 img_cnt = 0;
    uint8 label_cnt = 0;
    uint8 img_width = 0;
    uint8 img_hight = 0;
    uint8 label_width = 0;
    uint8 label_hight = 0;
    SCI_TIME_T sys_time= {0};
    SCI_DATE_T sys_date = {0};
    uint8 i,j,k,m = 0;
    char * data_buf = PNULL;
    uint32 file_len = 0;
    char file_path[100] = {0};
    watch_dial_t * watch_dial = NULL;
#if ZMT_DIAL_USE_FOR_TEST
    sprintf(file_path, "%s", ZMT_DIAL_FILE_BASE_PATH);
#else
    sprintf(file_path, ZMT_DIAL_FILE_BASE_PATH, watch_name, watch_name);
#endif
    //SCI_TRACE_LOW("%s: file_path = %s", __FUNCTION__, file_path);
    if(zmt_watch_dial == NULL){
        if(dsl_file_exist(file_path)){
            data_buf = dsl_file_data_read(file_path, &file_len);
        }else{
            SCI_TRACE_LOW("%s: file_path = %s not exist !!", __FUNCTION__, file_path);
            return;
        }
        zmt_watch_dial = (watch_dial_t*)SCI_ALLOC_APPZ(sizeof(watch_dial_t));
        memset(zmt_watch_dial, 0, sizeof(watch_dial_t));
        if(data_buf && file_len > 2){
            ZMT_DialParse(data_buf, zmt_watch_dial);
            SCI_FREE(data_buf);
        }else{
            return;
        }
    }
    watch_dial = zmt_watch_dial;
    
    text_style.align = ALIGN_LVMIDDLE;
    text_style.font = SONG_FONT_16;
    text_style.font_color = MMI_WHITE_COLOR;

    TM_GetSysTime(&sys_time);
    TM_GetSysDate(&sys_date);
    //SCI_TRACE_LOW("%s: sys_time.hour = %d, sys_time.min = %d, sys_time.sec = %d", __FUNCTION__, sys_time.hour, sys_time.min, sys_time.sec);
    //SCI_TRACE_LOW("%s: mon = %d, mday = %d, wday = %d", __FUNCTION__, sys_date.mon, sys_date.mday, sys_date.wday);
    
    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);

    if(watch_dial == NULL){
        SCI_TRACE_LOW("%s: watch_dial is empty!!", __FUNCTION__);
        return;
    }
    
    win_rect.left = 0;
    win_rect.right = 240;
    win_rect.top = 0;
    win_rect.bottom = 240;
    sprintf(dir_str, "%s", watch_dial->name);
    memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
    sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->bg_img);
    MMIAPICOM_StrToWstr(image_str, image_name);
    ZMT_CreateElementImg(&lcd_dev_info, &win_rect, &image_name);
    for(i = 0;i < watch_dial->element_cnt && i < WATCH_MAX_ARRAY_COUNT;i++)
    {
        switch(watch_dial->element_ll[i].element_type)
        {
            case ELEMENT_TYPE_TIME_HAND:
            {
                wchar image_hour[WATCH_IMAGE_FULL_PATH_MAX_LEN] ={0};
                wchar image_min[WATCH_IMAGE_FULL_PATH_MAX_LEN] ={0};
                wchar image_sec[WATCH_IMAGE_FULL_PATH_MAX_LEN] ={0};
                wchar image_center[WATCH_IMAGE_FULL_PATH_MAX_LEN] ={0};
                int hour = -1;
                int min = -1;
                int sec = -1;
                int center = -1;
                img_cnt = watch_dial->element_ll[i].img_cnt;
                for(j = 0;j < img_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                {
                    switch(watch_dial->element_ll[i].img_ll[j].img_type)
                    {
                        case IMG_TYPE_TIME_HOUR_HAND:
                            hour = j;
                            break;
                        case IMG_TYPE_TIME_MIN_HAND:
                            min = j;
                            break;
                        case IMG_TYPE_TIME_SEC_HAND:
                            sec = j;
                            break;
                        case IMG_TYPE_TIME_CENTER_DOT:
                            center = j;
                            break;
                    }
                }
                if(hour >= 0 && min >= 0 && sec >= 0){
                    memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                    sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[hour].sub_img_ll[0].sub_img);          
                    MMIAPICOM_StrToWstr(image_str, image_hour);
                    memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                    sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[min].sub_img_ll[0].sub_img); 
                    MMIAPICOM_StrToWstr(image_str, image_min);
                    memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                    sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[sec].sub_img_ll[0].sub_img);
                    MMIAPICOM_StrToWstr(image_str, image_sec);
                    ZMT_CreateAnalogTime(&lcd_dev_info, &win_rect, &image_hour, &image_min, &image_sec);
                }
                if(center >= 0){
                    memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                    sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[center].sub_img_ll[0].sub_img);
                    MMIAPICOM_StrToWstr(image_str, image_center); 
                    ZMT_CreateElementImg(&lcd_dev_info, &win_rect, &image_center);
                }
            }
            break;
            case ELEMENT_TYPE_TIME_DIGIT:
            {
                uint8 ten = 0;
                uint8 unit = 0;
                if(watch_dial->element_ll[i].img_cnt > 0)
                {                    
                    img_cnt = watch_dial->element_ll[i].img_cnt;
                    img_width = 30;
                    img_hight = img_width;
                    for(j = 0;j < img_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        anim_rect.left = watch_dial->element_ll[i].img_ll[j].img_pos.x;
                        anim_rect.top = watch_dial->element_ll[i].img_ll[j].img_pos.y;
                        anim_rect.right = anim_rect.left + watch_dial->element_ll[i].img_ll[j].img_width;
                        anim_rect.bottom = anim_rect.top + watch_dial->element_ll[i].img_ll[j].img_height;
                        switch(watch_dial->element_ll[i].img_ll[j].img_type)
                        {
                            case IMG_TYPE_TIME_HOUR_TENS:
                            {
                                if(sys_time.hour >= 10 && sys_time.hour < 20){
                                    ten = 1;
                                }else if(sys_time.hour >= 20){
                                    ten = 2;
                                }else{
                                    ten = 0;
                                }
                                //SCI_TRACE_LOW("%s: ten = %d", __FUNCTION__, ten);
                                ctrl_id = ZMT_DIAL_WIN_HOUR_TEN_CTRL_ID;
                                memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                                sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[j].sub_img_ll[ten].sub_img);
                                MMIAPICOM_StrToWstr(image_str, image_name);
                            }
                            break;
                            case IMG_TYPE_TIME_HOUR_UNIT:
                            {
                                if(sys_time.hour >= 10 && sys_time.hour < 20){
                                    unit = sys_time.hour % 10;
                                }else  if(sys_time.hour >= 20){
                                    unit = sys_time.hour % 20;
                                }else{
                                    unit = sys_time.hour;
                                }
                                //SCI_TRACE_LOW("%s: unit = %d", __FUNCTION__, unit);
                                ctrl_id = ZMT_DIAL_WIN_HOUR_UNIT_CTRL_ID;
                                memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                                sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[j].sub_img_ll[unit].sub_img);
                                MMIAPICOM_StrToWstr(image_str, image_name);
                            }
                            break;
                            case IMG_TYPE_TIME_MIN_TENS:
                            {
                                if(sys_time.min >= 10 && sys_time.min < 60){
                                    ten = sys_time.min /10;
                                }else{
                                    ten = 0;
                                }
                                //SCI_TRACE_LOW("%s: ten = %d", __FUNCTION__, ten);
                                ctrl_id = ZMT_DIAL_WIN_MIN_TEN_CTRL_ID;
                                memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                                sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[j].sub_img_ll[ten].sub_img);
                                MMIAPICOM_StrToWstr(image_str, image_name);
                            }
                            break;
                            case IMG_TYPE_TIME_MIN_UNIT:
                            {
                                unit = sys_time.min % 10;
                                //SCI_TRACE_LOW("%s: unit = %d", __FUNCTION__, unit);
                                ctrl_id = ZMT_DIAL_WIN_MIN_UNIT_CTRL_ID;
                                memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                                sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[j].sub_img_ll[unit].sub_img);
                                MMIAPICOM_StrToWstr(image_str, image_name);
                            }
                            break;
                            default:
                            break;
                        }
                        ZMT_CreateAnimImg(win_id, ctrl_id, &anim_rect, image_name, 1, &lcd_dev_info);
                    }
                }
                else
                {
                    label_cnt = watch_dial->element_ll[i].label_cnt;
                    for(j = 0; j < label_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        point.x = watch_dial->element_ll[i].label_ll[j].text_pos.x;
                        point.y = watch_dial->element_ll[i].label_ll[j].text_pos.y;
                        memset(&text_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                        sprintf(text_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].label_ll[j].text);
                        MMIAPICOM_StrToWstr(text_str, text_name);                       
                        text_string.wstr_ptr = text_name;
                        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                        GUISTR_DrawTextToLCDSingleLine(
                            &lcd_dev_info, 
                            &point, 
                            &text_string, 
                            &text_style,
                            text_state
                        );
                    }
                }
            }
            break;            
            case ELEMENT_TYPE_WEEK:
            {
                uint8 week = sys_date.wday;
                if(watch_dial->element_ll[i].img_cnt > 0)
                {
                    img_cnt = watch_dial->element_ll[i].img_cnt;
                    for(j = 0;j < img_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        switch(watch_dial->element_ll[i].img_ll[j].img_type)
                        {
                            case IMG_TYPE_WEEK_HAND:
                            case IMG_TYPE_WEEK_DIGIT:
                            {
                                anim_rect.left = watch_dial->element_ll[i].img_ll[j].img_pos.x;
                                anim_rect.right = anim_rect.left + watch_dial->element_ll[i].img_ll[j].img_width;
                                anim_rect.top = watch_dial->element_ll[i].img_ll[j].img_pos.y;
                                anim_rect.bottom = anim_rect.top + watch_dial->element_ll[i].img_ll[j].img_height;
                                memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                                sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[j].sub_img_ll[week].sub_img);
                                MMIAPICOM_StrToWstr(image_str, image_name);
                                //ZMT_CreateElementImg(&lcd_dev_info, &anim_rect, image_name);
                                ZMT_CreateAnimImg(win_id, ZMT_DIAL_WIN_WEEK_DIGIT_CTRL_ID, &anim_rect, image_name, 1, &lcd_dev_info);
                                //ZMT_ShowElementImg(win_id, &lcd_dev_info, &anim_rect, &image_name);
                            }
                            break;
                            default:
                            break;
                        }
                    }
                }
                else
                {
                    label_cnt = watch_dial->element_ll[i].label_cnt;
                    for(j = 0;j < label_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        switch(watch_dial->element_ll[i].label_ll[j].label_type)
                        {
                            case LABEL_TYPE_WEEK:
                            {
                                MMI_TEXT_ID_T text_id[] = {
                                    ZMT_DIAL_WEEK_CH_SUNDAY,ZMT_DIAL_WEEK_CH_MONDAY,ZMT_DIAL_WEEK_CH_TUESDAY,
                                    ZMT_DIAL_WEEK_CH_WEDNESDAY,ZMT_DIAL_WEEK_CH_THURSDAY,ZMT_DIAL_WEEK_CH_FRIDAY,ZMT_DIAL_WEEK_CH_SATURDAY    
                                };
                                point.x = watch_dial->element_ll[i].label_ll[j].text_pos.x;
                                point.y = watch_dial->element_ll[i].label_ll[j].text_pos.y;
                                MMIRES_GetText(text_id[week], win_id, &text_string);
                                GUISTR_DrawTextToLCDSingleLine(
                                    &lcd_dev_info, 
                                    &point, 
                                    &text_string, 
                                    &text_style,
                                    text_state
                                );
                            }
                            break;
                            case LABEL_TYPE_WEEK_EN:
                            {
                                MMI_TEXT_ID_T text_id[] = {
                                    ZMT_DIAL_WEEK_EN_SUNDAY,ZMT_DIAL_WEEK_EN_MONDAY,ZMT_DIAL_WEEK_EN_TUESDAY,
                                    ZMT_DIAL_WEEK_EN_WEDNESDAY,ZMT_DIAL_WEEK_EN_THURSDAY,ZMT_DIAL_WEEK_EN_FRIDAY,ZMT_DIAL_WEEK_EN_SATURDAY
                                };
                                point.x = watch_dial->element_ll[i].label_ll[j].text_pos.x;
                                point.y = watch_dial->element_ll[i].label_ll[j].text_pos.y;
                                //SCI_TRACE_LOW("%s: WEEK,point.x = %d, point.y = %d", __FUNCTION__, point.x, point.y);
                                MMIRES_GetText(text_id[week], win_id, &text_string);
                                GUISTR_DrawTextToLCDSingleLine(
                                    &lcd_dev_info, 
                                    &point, 
                                    &text_string, 
                                    &text_style,
                                    text_state
                                );                                
                            }
                            break;
                            default:
                            break;
                        }
                    }
                }
            }
            break;
            case ELEMENT_TYPE_DATE:
            {
                uint8 year = sys_date.year;
                uint8 month = sys_date.mon;
                uint8 day = sys_date.mday;
                if(watch_dial->element_ll[i].img_cnt > 0)
                {
                    img_cnt = watch_dial->element_ll[i].img_cnt;
                    for(j = 0;j < img_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        switch(watch_dial->element_ll[i].img_ll[j].img_type)
                        {
                            case IMG_TYPE_MON_TENS:
                            {

                            }
                            break;
                            case IMG_TYPE_MON_UNIT:
                            {

                            }
                            break;
                            case IMG_TYPE_DAY_TENS:
                            {

                            }
                            break;
                            case IMG_TYPE_DAY_UNIT:
                            {

                            }
                            break;
                            case IMG_TYPE_MON:
                            {

                            }
                            break;
                        }
                    }
                }
                else
                {
                    label_cnt = watch_dial->element_ll[i].label_cnt;
                    for(j = 0;j < label_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        switch(watch_dial->element_ll[i].label_ll[j].label_type)
                        {
                            case LABEL_TYPE_DATE_YEAR:
                            {                               
                                sprintf(str_name, "%d", year);
                            }
                            break;
                            case LABEL_TYPE_DATE_MONTH:
                            {
                                sprintf(str_name, "%.2d", month);
                            }
                            break;
                            case LABEL_TYPE_DATE_DAY:
                            {
                                sprintf(str_name, "%.2d", day);
                            }
                            break;
                            case LABEL_TYPE_DATE_CONNECTOR:
                            {
                                sprintf(str_name, "%s", watch_dial->element_ll[i].label_ll[j].text);
                            }
                            break;
                        }
                        point.x = watch_dial->element_ll[i].label_ll[j].text_pos.x;
                        point.y = watch_dial->element_ll[i].label_ll[j].text_pos.y;
                        //SCI_TRACE_LOW("%s: DATE,point.x = %d, point.y = %d", __FUNCTION__, point.x, point.y);
                        MMIAPICOM_StrToWstr(&str_name, text_name);
                        text_string.wstr_ptr = text_name;
                        text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                        GUISTR_DrawTextToLCDSingleLine(
                            &lcd_dev_info, 
                            &point, 
                            &text_string, 
                            &text_style,
                            text_state
                        );
                    }
                }
            }
            break;
            case ELEMENT_TYPE_BATTERY:
            {
                uint8 battery = ZDT_GetBatteryPercent();
                uint8 bat_img_cnt = 0;
                uint8 percent = 0;
                //SCI_TRACE_LOW("%s: battery = %d", __FUNCTION__, battery);
                if(watch_dial->element_ll[i].img_cnt > 0)
                {
                    img_cnt = watch_dial->element_ll[i].img_cnt;
                    for(j = 0;j < img_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        bat_img_cnt = watch_dial->element_ll[i].img_ll[j].sub_img_cnt;
                        switch(watch_dial->element_ll[i].img_ll[j].img_type)
                        {
                            case IMG_TYPE_BATTERY_SIGN:
                            {
                                anim_rect.left = watch_dial->element_ll[i].img_ll[j].img_pos.x;
                                anim_rect.right = anim_rect.left + watch_dial->element_ll[i].img_ll[j].img_width;
                                anim_rect.top = watch_dial->element_ll[i].img_ll[j].img_pos.y;
                                anim_rect.bottom = anim_rect.top + watch_dial->element_ll[i].img_ll[j].img_height;
                                if(battery == 100){
                                    percent = bat_img_cnt - 1;
                                }else{
                                    percent = battery / (100 / bat_img_cnt);
                                }
                                //SCI_TRACE_LOW("%s: percent = %d", __FUNCTION__, percent);
                                memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                                sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[j].sub_img_ll[percent].sub_img);
                                MMIAPICOM_StrToWstr(image_str, image_name);
                                //ZMT_CreateElementImg(&lcd_dev_info, &anim_rect, image_name);
                                ZMT_CreateAnimImg(win_id, ZMT_DIAL_WIN_BATTERY_SIGN_CTRL_ID, &anim_rect, image_name, 1, &lcd_dev_info);
                                //ZMT_ShowElementImg(win_id, &lcd_dev_info, &anim_rect, &image_name);
                            }
                            break;
                        }
                    }
                }
                else
                {
                    label_cnt = watch_dial->element_ll[i].label_cnt;
                    for(j = 0;j < img_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                    {
                        switch(watch_dial->element_ll[i].label_ll[j].label_type)
                        {
                            case LABEL_TYPE_BATTERY:
                            {
                                point.x = watch_dial->element_ll[i].label_ll[j].text_pos.x;
                                point.y = watch_dial->element_ll[i].label_ll[j].text_pos.y;
                                sprintf(str_name, "%d", battery);
                                MMIAPICOM_StrToWstr(&str_name, text_name);
                                text_string.wstr_ptr = text_name;
                                text_string.wstr_len = MMIAPICOM_Wstrlen(text_string.wstr_ptr);
                                GUISTR_DrawTextToLCDSingleLine(
                                    &lcd_dev_info, 
                                    &point, 
                                    &text_string, 
                                    &text_style,
                                    text_state
                                );
                            }
                            break;
                        }
                    }
                }
            }
            break;
            case ELEMENT_TYPE_SIGNAL:
            {
                img_cnt = watch_dial->element_ll[i].img_cnt;
                for(j = 0;j < img_cnt && j < WATCH_MAX_ARRAY_COUNT;j++)
                {
                    switch(watch_dial->element_ll[i].img_ll[j].img_type)
                    {
                        case IMG_TYPE_SIGNAL_SIGN:
                        {
                            MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
                            int level = 0;
                            if(MMIAPIPHONE_GetSimExistedStatus(dual_sys))
                            {                   		
                                PHONE_SERVICE_STATUS_T* service_status = MMIAPIPHONE_GetServiceStatus();
                                MMI_GMMREG_RAT_E network_status_rat = MMI_GMMREG_RAT_GPRS;
                                network_status_rat = MMIAPIPHONE_GetTDOrGsm(dual_sys);
                                SCI_TRACE_LOW("%s: service_status->rssiLevel = %d", __FUNCTION__, service_status->rssiLevel);
                                if(service_status->rssiLevel >= 40)
                                {
                                    level = 6;
                                }else if(service_status->rssiLevel >= 30 && service_status->rssiLevel < 40)
                                {
                                    level = 5;
                                }else if(service_status->rssiLevel >= 20 && service_status->rssiLevel < 30)
                                {
                                    level = 4;
                                }else if(service_status->rssiLevel >= 10 && service_status->rssiLevel < 20)
                                {
                                    level = 3;
                                }else if(service_status->rssiLevel >= 1 && service_status->rssiLevel < 10)
                                {
                                    level = 2;
                                }else
                                {
                                    level = 1;
                                }
                            }
                            else
                            {
                                level = 0;
                            }
                            anim_rect.left = watch_dial->element_ll[i].img_ll[j].img_pos.x;
                            anim_rect.right = anim_rect.left + watch_dial->element_ll[i].img_ll[j].img_width;
                            anim_rect.top = watch_dial->element_ll[i].img_ll[j].img_pos.y;
                            anim_rect.bottom = anim_rect.top + watch_dial->element_ll[i].img_ll[j].img_height;
                            memset(&image_str, 0, WATCH_IMAGE_FULL_PATH_MAX_LEN);
                            sprintf(image_str, "%s\\%s\\%s", ZMT_DIAL_DIR_BASE_PATH, dir_str, watch_dial->element_ll[i].img_ll[j].sub_img_ll[level].sub_img);
                            MMIAPICOM_StrToWstr(image_str, image_name);
                            //ZMT_CreateElementImg(&lcd_dev_info, &anim_rect, image_name);
                            ZMT_CreateAnimImg(win_id, ZMT_DIAL_WIN_SIGNAL_SIGN_CTRL_ID, &anim_rect, image_name, 1, &lcd_dev_info);
                            //ZMT_ShowElementImg(win_id, &lcd_dev_info, &anim_rect, &image_name);
                        }
                        break;
                    }
                }
            }
            break;
            default:
            break;
        }
    }
}



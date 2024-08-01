/****************************************************************************
** File Name:      mmiphsapp_wintab.c                                           *
** Author:          jianshengqi                                                       *
** Date:           03/22/2006                                              *
** Copyright:      2006 TLT, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the PHS                   *
/****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2006       Jianshq         Create
** 
****************************************************************************/
#ifndef _MMIZDT_YX_WINTAB_C_
#define _MMIZDT_YX_WINTAB_C_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmicc_export.h"
#include "guitext.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "guilistbox.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "mmi_common.h"

#include "zdt_app.h"
#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "img_dec_interface.h"
#include "guiownerdraw.h"
#include "graphics_draw.h"
#include "mmiparse_export.h"
#include "mmicl_export.h"
#ifdef BROWSER_SUPPORT
#include "mmibrowser_export.h"
#endif
#ifdef ENG_SUPPORT
#include "mmieng_export.h"
#endif
#include "mmiset_export.h"
#include "mmiset_id.h"

#include "zdt_win_export.h"
#include "mmicc_internal.h"
#include "watch_commonwin_export.h"
#ifdef XYSDK_SUPPORT
#include "libxmly_api.h"
#endif 
#include "watch_common.h"


#define SCALE  1

#ifndef WIN32
extern void GT_Qcode_show(const unsigned char *QR_TEST_STR,int x0,int y0,int w0,int h0);
#else
void GT_Qcode_show(const unsigned char *QR_TEST_STR,int x0,int y0,int w0,int h0){};
#endif

#define z_abs(x)  ((x) >= 0 ? (x) : (-(x)))

LOCAL THEMELIST_ITEM_STYLE_T new_style = {0};

#if 1 //查找手表
LOCAL MMI_RESULT_E  HandleZDT_FindWatchWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );



WINDOW_TABLE( MMIZDT_FIND_WATCH_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_FindWatchWinMsg),    
    WIN_ID( MMIZDT_FIND_WATCH_WIN_ID),
    CREATE_ANIM_CTRL(MMIZDT_FINDWATCH_ANIM_CTRL_ID, MMIZDT_FIND_WATCH_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC BOOLEAN MMIZDT_FindWatchWin(void)
{
    if(FALSE == MMK_IsOpenWin(MMIZDT_FIND_WATCH_WIN_ID))
    {
#ifndef WIN32    
#ifdef LEBAO_MUSIC_SUPPORT
        lebao_exit();
#endif
#ifdef XYSDK_SUPPORT
        LIBXMLYAPI_AppExit();
#endif
#endif
        ZMTApp_CloseRecordAndPlayer();
        MMK_CreateWin((uint32*)MMIZDT_FIND_WATCH_WIN_TAB,PNULL);
    }
    return TRUE;
}


LOCAL GUIANIM_RESULT_E SetFindWatchAnimParam(
    BOOLEAN          is_update,  //是否立即刷新
    MMI_CTRL_ID_T    ctrl_id
)
{
    GUIANIM_RESULT_E        anim_result = GUIANIM_RESULT_SUCC;
    FILEARRAY_DATA_T        file_info = {0};
    GUIANIM_CTRL_INFO_T     control_info = {0};
    GUIANIM_FILE_INFO_T     anim_info = {0};
    GUIANIM_CALL_BACK_T     call_back = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    GUIANIM_DATA_INFO_T data_info={0};

    //set anim min period
    GUIANIM_SetPeriod(200, ctrl_id);

    //set anim param
    control_info.is_ctrl_id = TRUE;
    control_info.ctrl_id    = ctrl_id;

    

    display_info.is_zoom        = FALSE;


    

    if(MMIZDT_FINDWATCH_ANIM_CTRL_ID == ctrl_id)
    {
        display_info.is_disp_one_frame = FALSE;
    }
    else
    {
        display_info.is_disp_one_frame = TRUE;
    }
    display_info.is_play_once = FALSE;
    display_info.align_style    = GUIANIM_ALIGN_HVMIDDLE;
    display_info.is_update      = is_update;
    display_info.bg.bg_type     = GUI_BG_COLOR;

    //背景色设为黑色
    display_info.bg.color       = MMI_BLACK_COLOR;//MMI_WINDOW_BACKGROUND_COLOR;

    
    data_info.img_id  = IMAGE_FIND_WATCH;
    anim_result = GUIANIM_SetParam(&control_info, &data_info, PNULL, &display_info);


    return (anim_result);
}



LOCAL MMI_RESULT_E  HandleZDT_FindWatchWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUI_BG_T bg_ptr = {0};
    GUI_BG_DISPLAY_T bg_display = {0};
    MMI_CTRL_ID_T anim_ctrl_id = MMIZDT_FINDWATCH_ANIM_CTRL_ID;
    const GUI_LCD_DEV_INFO *lcd_info = MMITHEME_GetDefaultLcdDev();
    //GUI_POINT_T      point = {94,185};
    GUI_BOTH_RECT_T     both_rect = MMITHEME_GetFullScreenBothRect();
   
    
    BOOLEAN ret = FALSE;

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        GUIAPICTRL_SetBothRect(anim_ctrl_id, &both_rect);
        SetFindWatchAnimParam(TRUE, anim_ctrl_id);  
        break;
        
    case MSG_FULL_PAINT:
        break;

    #if 1   //xiongkai add   for bug: 设备充电灭屏时-APP查找设备-设备被查找界面短按HOME键-界面消失，其铃声不会立即停止-拔掉充电线，设备仍显示查找界面；        
    case MSG_LOSE_FOCUS:
        MMK_CloseWin(win_id);
     YX_API_CZSB_Stop();    
        break;        
    #endif
    
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL: 
        MMK_CloseWin(win_id);
        break;
    case MSG_CLOSE_WINDOW:
            YX_API_CZSB_Stop();
        break;
    case MSG_KEYDOWN_RED:
        break;
        
    case MSG_KEYUP_RED:
		MMK_CloseWin(win_id);
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

#endif


#ifdef ZDT_PLAT_YX_SUPPORT
LOCAL MMI_RESULT_E HandleZDT_BHMainMenuWinMsg (
                                      MMI_WIN_ID_T   win_id,     // 窗口的ID
                                      MMI_MESSAGE_ID_E     msg_id,     // 窗口的内部消息ID
                                      DPARAM            param        // 相应消息的参数
                                      );
LOCAL void ZDTSET_CommonTextId(                                    
                                                MMI_TEXT_ID_T        text_id,
                                                MMI_TEXT_ID_T        left_softkey_id,
                                                MMI_TEXT_ID_T        middle_softkey_id,
                                                MMI_TEXT_ID_T        right_softkey_id,
                                                MMI_CTRL_ID_T        ctrl_id,
                                                GUIITEM_STYLE_E      item_style
                                                )
{
    GUILIST_ITEM_T      item_t      =   {0};/*lint !e64*/
    GUILIST_ITEM_DATA_T item_data   =   {0};/*lint !e64*/
    
    item_t.item_style    = item_style;
    item_t.item_data_ptr = &item_data;
    
    item_data.item_content[0].item_data_type    = GUIITEM_DATA_TEXT_ID;
    item_data.item_content[0].item_data.text_id = text_id;
    
    item_data.softkey_id[0] = left_softkey_id;
    item_data.softkey_id[1] = middle_softkey_id;
    item_data.softkey_id[2] = right_softkey_id;
    
    GUILIST_AppendItem( ctrl_id, &item_t );      
}

LOCAL void  SetBHOnOffListCode( MMI_CTRL_ID_T ctrl_id)
{
    GUILIST_ITEM_T              item_t              = {0};/*lint !e64*/
    GUILIST_ITEM_DATA_T         item_data           = {0};/*lint !e64*/
    MMIENVSET_SETTING_T         mode_setting_ptr    = {0};
    uint16 cur_selection =0;
    GUILIST_SetMaxItem( ctrl_id, 2, FALSE );//max item 5
    GUILIST_SetMaxSelectedItem(ctrl_id,2);
    //append item
    ZDTSET_CommonTextId(TXT_COMM_OPEN,TXT_COMMON_OK,TXT_NULL,STXT_RETURN,ctrl_id,GUIITEM_STYLE_ONE_LINE_RADIO);
    ZDTSET_CommonTextId(STXT_EXIT,TXT_COMMON_OK,TXT_NULL,STXT_RETURN,ctrl_id,GUIITEM_STYLE_ONE_LINE_RADIO);
    if(yx_DB_Set_Rec.net_open > 0)
    {
        cur_selection = 0;
    }
    else
    {
        cur_selection = 1;
    }
    GUILIST_SetCurItemIndex(ctrl_id, cur_selection);
    GUILIST_SetSelectedItem(ctrl_id,cur_selection,TRUE);

}

LOCAL void SetBHOnOffType(void)
{
    uint16 cur_select = 0;
    cur_select =(uint16)GUILIST_GetCurItemIndex(MMIZDT_YX_ONOFF_CTRL_ID);   ///GUISETLIST_GetCurIndex(MMISET_TORCH_SETLIST_CTRL_ID);
    if(cur_select == 0)
    {
        YX_API_SeNetOpen();
    }
    else
    {
        YX_API_SeNetClose();
    }
}
LOCAL MMI_RESULT_E HandleZDT_BHOnOffWinMsg(
        MMI_WIN_ID_T       win_id,
        MMI_MESSAGE_ID_E   msg_id,
        DPARAM             param
        )
{
   
    MMI_RESULT_E     recode = MMI_RESULT_TRUE;
    static uint16          cur_selection = 0;
    MMI_CTRL_ID_T   ctrl_id  = MMIZDT_YX_ONOFF_CTRL_ID;
    uint16                     max_item = 2; 

    switch (msg_id)
    {
        case MSG_OPEN_WINDOW:
              SetBHOnOffListCode(ctrl_id);

              MMK_SetAtvCtrl(win_id, MMIZDT_YX_ONOFF_CTRL_ID);
        break;
            
        case MSG_CTL_MIDSK:
        case MSG_CTL_OK:
        case MSG_APP_OK:
        case MSG_APP_WEB:
        SetBHOnOffType();
        MMK_CloseWin(win_id);
        break;          
    case MSG_APP_CANCEL:
    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
        
    case MSG_APP_RED:
        recode = MMI_RESULT_FALSE;
        break;
    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
   
}


WINDOW_TABLE(MMIZDT_BH_ONOFF_WIN_TAB ) = 
{
        //窗口处理函数
        WIN_FUNC((uint32)HandleZDT_BHOnOffWinMsg),
        //win_id
        WIN_ID(MMIZDT_BH_ONOFF_WIN_ID),
        //窗口的标题
        WIN_TITLE(TXT_BH_NET_ONOFF),
        //创建  控件
       CREATE_LISTBOX_CTRL(GUILIST_RADIOLIST_E,MMIZDT_YX_ONOFF_CTRL_ID),
        WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
        //   WIN_TIPS,
     END_WIN

};
PUBLIC void MMIZDT_OpenOnOffWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_BH_ONOFF_WIN_TAB, PNULL);
}


#if 1
#define QRCODE_FILE_NAME  L"D:\\Others\\ewm.bmp"
#define QRCODE_DOWNLOAD_FILE_NAME  L"D:\\Others\\ewm_download.bmp"
#define QRCODE_BIND_FILE_NAME  L"D:\\Others\\ewm_bind.bmp"
#define QRCODE_MANUAL_FILE_NAME  L"D:\\Others\\ewm_manual.bmp"



#define QRCODE_DISP_X    40
#define QRCODE_DISP_Y    40
#define QRCODE_DISP_WIDTH    160
#define QRCODE_DISP_HEIGHT    160

#define ZOOM_ALIGN(x,align)     (x/align * align)

//zoom in param
typedef struct 
{
    uint8           *src_buf_ptr;       //src buffer
    uint32          src_buf_size;       //src buffer size
    uint8           *dest_buf_ptr;      //dest buffer
    uint32          dest_buf_size;      //dest buffer size

    uint16          src_img_width;      //src image width
    uint16          src_img_height;     //src image height
    uint16          dest_img_width;     //dest image width
    uint16          dest_img_height;    //dest image height
    //GUI_RECT_T      clip_rect;          //clip rect
} ZOOM_INPUT_T;

//scale out param
typedef struct 
{
    uint16      img_width;      //image width
    uint16      img_height;     //image height
    uint32      dest_buf_offset;//dest buffer必须256对齐
} ZOOM_OUTPUT_T;

typedef enum
{
    ZOOM_RESULT_SUCC,                    /*!<成功*/
    ZOOM_RESULT_FAIL,                    /*!<失败*/
    ZOOM_RESULT_NO_SUPPORT,              /*!<不支持*/
    ZOOM_RESULT_MAX                      /*!<保留位*/
} ZOOM_RESULT_E;

static int s_2vm_tp_x;
static int s_2vm_tp_y;
static BOOLEAN s_is_2vm_tp_down = FALSE;

LOCAL ZOOM_RESULT_E ZoomImage(
                      ZOOM_INPUT_T  *zoom_in_ptr,   //in:
                      ZOOM_OUTPUT_T *zoom_out_ptr   //in/out:
                      )
{
    uint32              scale_result = 0;
    ZOOM_RESULT_E    result = ZOOM_RESULT_FAIL;
    SCALE_IMAGE_IN_T    scale_in = {0};
    SCALE_IMAGE_OUT_T   scale_out = {0};

    if(PNULL == zoom_in_ptr ||PNULL == zoom_out_ptr )
    {
        return result;
    }
    //set src width and height
    scale_in.src_size.w = zoom_in_ptr->src_img_width;
    scale_in.src_size.h = zoom_in_ptr->src_img_height;

    //set target width and height
    scale_in.target_size.w = zoom_in_ptr->dest_img_width;
    scale_in.target_size.h = zoom_in_ptr->dest_img_height;

    //w 4对齐，h 2对齐
    scale_in.target_size.w = (uint16)ZOOM_ALIGN(scale_in.target_size.w,4);
    scale_in.target_size.h = (uint16)ZOOM_ALIGN(scale_in.target_size.h,2);

    //原图裁剪
    scale_in.src_trim_rect.x = 0;
    scale_in.src_trim_rect.y = 0;
    scale_in.src_trim_rect.w = zoom_in_ptr->src_img_width;
    scale_in.src_trim_rect.h = zoom_in_ptr->src_img_height;

    //set src format
    scale_in.src_format = IMGREF_FORMAT_RGB565;

    //set src buffer
    scale_in.src_chn.chn0.ptr = zoom_in_ptr->src_buf_ptr;
    scale_in.src_chn.chn0.size = zoom_in_ptr->src_buf_size;

    //set target format
    scale_in.target_format = IMGREF_FORMAT_RGB565;

    //set target buffer
    scale_in.target_buf.ptr  = zoom_in_ptr->dest_buf_ptr;
    scale_in.target_buf.size = zoom_in_ptr->dest_buf_size;

    //scale
    scale_result = GRAPH_ScaleImage(&scale_in,&scale_out);
    //BRWDebug_Printf:"[BRW]BrwZoomImage:zoom result is %d!"
    switch (scale_result)
    {
    case SCI_SUCCESS:
        //set image width and height
        zoom_out_ptr->img_width  = scale_out.output_size.w;
        zoom_out_ptr->img_height = scale_out.output_size.h;

        //set target buffer offset
        zoom_out_ptr->dest_buf_offset = (uint32)(scale_out.output_chn.chn0.ptr) - (uint32)(scale_in.target_buf.ptr);
        result = ZOOM_RESULT_SUCC;
        break;
    
    case GRAPH_ERR_SIZE_UNSUPPORT:
        result = ZOOM_RESULT_NO_SUPPORT;
        break;

    default:
        result = ZOOM_RESULT_FAIL;
        break;
    }

    return (result);
}

PUBLIC BOOLEAN MMIZDT_CheckImeiChange(void)
{
    if(strncmp( (char *)yx_DB_Set_Rec.imei, g_zdt_phone_imei_1, 15 ) == 0)
    {
        return FALSE;
    }
    return TRUE;
}

PUBLIC BOOLEAN MMIZDT_SaveImei(void)
{
    SCI_MEMSET(yx_DB_Set_Rec.imei,0,16);
    SCI_MEMCPY(yx_DB_Set_Rec.imei,g_zdt_phone_imei_1,15);
    YX_DB_SET_ListModify();
    return TRUE;
}

BOOLEAN  MMIZDT_Check2VMFile()
{
    //http://4g.watch.ecellsz.com:8080/app/newindex.html?imei=
    char ewm_str[200] = {0};
    if(MMIZDT_CheckImeiChange())
    {
        //UserFile_DeleteU("Others","ewm.bmp");
        UserFile_DeleteU("Others","ewm_bind.bmp");
    }
    #if 0 //yangyu delete ,
    //if(!MMIFILE_IsFileExist(QRCODE_FILE_NAME,MMIAPICOM_Wstrlen(QRCODE_FILE_NAME)))
    if(!MMIFILE_IsFileExist(QRCODE_DOWNLOAD_FILE_NAME,MMIAPICOM_Wstrlen(QRCODE_BIND_FILE_NAME)))
    {
        //sprintf(ewm_str,"http://%s:8080/app/newindex.html?imei=%s",yx_DB_Set_Rec.app_domain,g_zdt_phone_imei_1);
        sprintf(ewm_str,"http://%s:8080/app/newIndex.html",yx_DB_Set_Rec.app_domain);
#ifndef WIN32
        if(0 == Save_QEImage(ewm_str , 0))
        {
            //MMIZDT_SaveImei();
        }
#endif
    }    
    #endif

    if(!MMIFILE_IsFileExist(QRCODE_BIND_FILE_NAME,MMIAPICOM_Wstrlen(QRCODE_BIND_FILE_NAME)))
    {
        SCI_MEMSET(ewm_str, 0, 200*sizeof(char));
        sprintf(ewm_str,"http://%s:8080/app/newIndex.html?imei=%s",yx_DB_Set_Rec.app_domain,g_zdt_phone_imei_1);
#ifndef WIN32
        //if(0 == Save_QEImage(ewm_str, 1))//yangyu delete
        {
            MMIZDT_SaveImei();
        }
#endif
    }    
    #if 0 //yangyu delete , manual use new win
     if(!MMIFILE_IsFileExist(QRCODE_MANUAL_FILE_NAME, MMIAPICOM_Wstrlen(QRCODE_MANUAL_FILE_NAME)))
    {
        SCI_MEMSET(ewm_str, 0, 200*sizeof(char));
        sprintf(ewm_str,"https://share.weiyun.com/vQRtnw4K");
#ifndef WIN32
        if(0 == Save_QEImage(ewm_str, 2))
        {
            //MMIZDT_SaveImei();
        }
#endif
    }    
    #endif

    return TRUE;
}
LOCAL MMI_RESULT_E  HandleZDT_BH2VMWinMsg(
    MMI_WIN_ID_T    win_id,
    MMI_MESSAGE_ID_E   msg_id,
    DPARAM             param
) 
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetClientRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    BOOLEAN                 result = FALSE;
    GUIANIM_RESULT_E        anim_result = GUIANIM_RESULT_SUCC;
    GUIANIM_CTRL_INFO_T     control_info = {0};
    GUIANIM_FILE_INFO_T     anim_info = {0};
    GUIANIM_CALL_BACK_T     call_back = {0};
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    IMG_DEC_SRC_T       dec_src = {0};
    IMG_DEC_INFO_T      dec_info = {0}; /*lint !e64*/
    int w,h;
    uint8*   pszRGBData=NULL;
    uint32  img_handle = 0;
    IMG_DEC_FRAME_IN_PARAM_T    dec_in = {0};
    IMG_DEC_FRAME_OUT_PARAM_T   dec_out = {0};
    GUIIMG_BITMAP_T  bitmap_handle = {0};
    GUI_POINT_T                 dis_point = {0};
    ZOOM_INPUT_T    zoom_in_data = {0};
    ZOOM_OUTPUT_T   zoom_out_data = {0};
    BOOLEAN is_zoom_success = FALSE;    
    uint8 sn_str[24 + 1] = {0};
    uint16 sn_len = 0;
    MMI_STRING_T   sn_s = {0};
    uint16 uint16_str_len = 0;
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUISTR_INFO_T       text_info = {0};
    MMI_STRING_T sn_string = {0};
    char ewm_str[100 + 1] = {0};
    
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            GUI_FillRect(&lcd_dev_info, bg_rect, MMI_WHITE_COLOR);
            MMIZDT_Check2VMFile();
            dec_src.file_name_ptr = QRCODE_FILE_NAME;
            if(IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info))
            {    
                Trace_Log_Print("IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info)");            
                return FALSE;
            }        
            w=dec_info.img_detail_info.bmp_info.img_width;
            h=dec_info.img_detail_info.bmp_info.img_height;
            Trace_Log_Print("HandleQrcodeWindow w=%d h=%d",w, h);
        if(w==0 && h ==0)
        {
        dis_point.x=QRCODE_DISP_X;
            dis_point.y=QRCODE_DISP_Y; 
            bg_rect.left =QRCODE_DISP_X;
            bg_rect.right=QRCODE_DISP_X + QRCODE_DISP_WIDTH;
            bg_rect.top=QRCODE_DISP_Y;
            bg_rect.bottom=QRCODE_DISP_Y+QRCODE_DISP_HEIGHT;
            #if 0
            GUIRES_DisplayImg(&dis_point,
                &bg_rect,
                PNULL,
                win_id,
                IMAGE_DEFAULT_EWM,
                &lcd_dev_info);
            #endif
        }
        pszRGBData=(uint8*)SCI_ALLOC_APP(w*h*2);
        if(NULL == pszRGBData)
        {
            Trace_Log_Print("NULL == pszRGBData");
            return FALSE;
        }

        if (IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle))
        {
            Trace_Log_Print("IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle)");
            SCI_FREE(pszRGBData);
            return FALSE;
        }        

        dec_in.handle = img_handle;
        //set decode target
        dec_in.is_dec_thumbnail = 0;
        dec_in.target_ptr       = pszRGBData;
        dec_in.target_buf_size  = w*h*2;
        dec_in.target_width     = w;
        dec_in.target_height    = h;
        dec_in.img_rect.left    = 0;
        dec_in.img_rect.top     = 0;
        dec_in.img_rect.right   = w-1;
        dec_in.img_rect.bottom  = h-1;
        dec_in.data_format = IMG_DEC_RGB565;

        //图形解码
        if(IMG_DEC_RET_SUCCESS != IMG_DEC_GetFrame(&dec_in,&dec_out))
        {
            IMG_DEC_Destroy(img_handle);
            SCI_FREE(pszRGBData);
            return FALSE;
        }
            zoom_in_data.src_buf_ptr = dec_in.target_ptr;
            zoom_in_data.src_buf_size = dec_in.target_buf_size;
            zoom_in_data.src_img_width = dec_out.real_width;
            zoom_in_data.src_img_height= dec_out.real_height;
            zoom_in_data.dest_img_width = QRCODE_DISP_WIDTH;
            zoom_in_data.dest_img_height = QRCODE_DISP_HEIGHT;
            zoom_in_data.dest_buf_size = (zoom_in_data.dest_img_width * zoom_in_data.dest_img_height * 2 + 255);
            zoom_in_data.dest_buf_ptr = SCI_ALLOCA(zoom_in_data.dest_buf_size);
            
            if (PNULL != zoom_in_data.dest_buf_ptr)
            {
                if (ZOOM_RESULT_SUCC == ZoomImage(&zoom_in_data, &zoom_out_data))
                {
                    Trace_Log_Print("is_zoom_success = TRUE;");
                    is_zoom_success = TRUE;
                }
                else
                {
                    Trace_Log_Print("is_zoom_success = FALSE;");
                    is_zoom_success = FALSE;
                }
            }
            else
            {
               Trace_Log_Print("is_zoom_success = FALSE");
                is_zoom_success = FALSE;
            }
            
        IMG_DEC_Destroy(img_handle);

       
        bitmap_handle.bit_ptr =  (zoom_in_data.dest_buf_ptr + zoom_out_data.dest_buf_offset);;
        bitmap_handle.img_width  = QRCODE_DISP_WIDTH; //dec_out.real_width;
        bitmap_handle.img_height = QRCODE_DISP_HEIGHT; //dec_out.real_height;
        
        dis_point.x=QRCODE_DISP_X; //lpRC->left;
        dis_point.y=QRCODE_DISP_Y; //lpRC->top;        

        bg_rect.left =QRCODE_DISP_X;
        bg_rect.right=QRCODE_DISP_X + QRCODE_DISP_WIDTH;
        bg_rect.top=QRCODE_DISP_Y;
        bg_rect.bottom=QRCODE_DISP_Y+QRCODE_DISP_HEIGHT;    
        
        GUIIMG_DisplayBmp(FALSE, &bg_rect, &dis_point, &bitmap_handle,&lcd_dev_info);

            if (PNULL != zoom_in_data.dest_buf_ptr)
            {
                SCI_FREE(zoom_in_data.dest_buf_ptr);
                zoom_in_data.dest_buf_ptr = PNULL;
            }
            
        SCI_FREE(pszRGBData);                        
            break;
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527
            
    case MSG_FULL_PAINT:
            MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_BH_NET_2VM_INFO,&sn_string);
            text_style.align = ALIGN_HVMIDDLE; // 这里设置为Left,因为Label的位置自有自己调整区域
            text_style.font_color = MMI_BLACK_COLOR;
            text_style.angle = ANGLE_0;
            text_style.char_space = 0;
            text_style.effect = FONT_EFFECT_CUSTOM;
            text_style.edge_color = 0;

            text_style.font = SONG_FONT_24;
            text_style.line_space = 0;
            text_style.region_num = 0;
            text_style.region_ptr = PNULL;        
            text_display_rect.left = 0;
            text_display_rect.top = 40;
            text_display_rect.right=(MMI_MAINSCREEN_WIDTH -1);
            text_display_rect.bottom=80;    
            
            GUISTR_GetStringInfo(&text_style, &sn_string, state, &text_info);
        GUISTR_DrawTextToLCDInRect( 
                MMITHEME_GetDefaultLcdDev(),
                (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
                (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
                (const MMI_STRING_T    *)&sn_string,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );        
        break;
        
#ifdef TOUCH_PANEL_SUPPORT //IGNORE9527
        case MSG_CTL_PENOK:
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527
        //case MSG_CTL_OK:
        //case MSG_APP_OK:
        //case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            break;

        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}


//Qrcode 二维码
WINDOW_TABLE( MMIZDT_BH_2VM_WIN_TAB ) = {
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ),
    WIN_FUNC( (uint32)HandleZDT_BH2VMWinMsg ),
    WIN_ID(MMIZDT_BH_2VM_WIN_ID),
    WIN_TITLE(TXT_BH_NET_2VM),
    WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),    
    //CREATE_ANIM_CTRL (MMISET_QRCODE_CTRL_ID, MMISET_SET_QRCODE_WIN_ID),
    END_WIN
};

PUBLIC void MMIZDT_BH_Open2VMWin(void)
{
    MMK_CreateWin((uint32 *)MMIZDT_BH_2VM_WIN_TAB,PNULL);
}

LOCAL uint8 qrcode_page_index = 0;

LOCAL uint16 unbind_click_count = 0;

PUBLIC MMI_RESULT_E MMIZDT_Device_Unbind(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    switch(msg_id)
    {
        case MSG_APP_CANCEL:
        case MSG_CTL_CANCEL:
        case MSG_APP_RED:
            MMK_CloseWin(win_id);
            break;
        
        case MSG_APP_WEB:
        case MSG_CTL_OK:
        case MSG_APP_OK:
        case MSG_CTL_PENOK:
        case MSG_CTL_MIDSK:
            {
                MMK_CloseWin(win_id);
                YX_API_DLT_Send();
            }
            break;

        default:
            result = MMIPUB_HandleQueryWinMsg(win_id, msg_id, param);
            break;
    }
    return result;
}


LOCAL MMI_RESULT_E  HandleZDT_2VMWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetClientRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    BOOLEAN                 result = FALSE;
    GUIANIM_DISPLAY_INFO_T  display_info = {0};
    IMG_DEC_SRC_T       dec_src = {0};
    IMG_DEC_INFO_T      dec_info = {0}; /*lint !e64*/
    int w,h;
    uint8*   pszRGBData=NULL;
    uint32  img_handle = 0;
    IMG_DEC_FRAME_IN_PARAM_T    dec_in = {0};
    IMG_DEC_FRAME_OUT_PARAM_T   dec_out = {0};
    GUIIMG_BITMAP_T  bitmap_handle = {0};
    GUI_POINT_T                 dis_point = {0};
    ZOOM_INPUT_T    zoom_in_data = {0};
    ZOOM_OUTPUT_T   zoom_out_data = {0};
    BOOLEAN is_zoom_success = FALSE;    
    uint16 uint16_str[22] = {0};
    uint16 uint16_str_len = 0;
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUISTR_INFO_T       text_info = {0};
    MMI_STRING_T sn_string = {0};
    MMI_STRING_T imei_string = {0};
    char*           temp_ptr = PNULL;
    uint16 temp_len = 0;
    MMI_STRING_T    cr_s = {0};
    wchar           unicode_cr = 0x0d;
    cr_s.wstr_ptr = &unicode_cr;
    cr_s.wstr_len = 1;

    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            
            GUI_FillRect(&lcd_dev_info, bg_rect, MMI_WHITE_COLOR);
            MMIZDT_Check2VMFile();
            unbind_click_count = 0;
            //dec_src.file_name_ptr = QRCODE_FILE_NAME;
            if(qrcode_page_index == 0)
            {
                uint16 img_w = 180;
                dec_src.file_name_ptr = QRCODE_DOWNLOAD_FILE_NAME;
                //yangyu add, not generate
                dis_point.x= (MMI_MAINSCREEN_WIDTH - img_w)/2;
                dis_point.y= (MMI_MAINSCREEN_HEIGHT- img_w)/2; 
                bg_rect.left = dis_point.x;
                bg_rect.right= bg_rect.left + img_w;
                bg_rect.top = dis_point.y;
                bg_rect.bottom = bg_rect.top + img_w;
                GUIRES_DisplayImg(&dis_point,
                                &bg_rect,
                                PNULL,
                                win_id,
                                IMAGE_ZDT_YX_2VM,
                                &lcd_dev_info);
                break;

                //yangyu end

            }
            else if(qrcode_page_index == 1)
            {
                char ewm_str[200] = {0};
                uint16 large = 20;
                SCI_MEMSET(ewm_str, 0, 200*sizeof(char));
            #ifdef ZTE_WATCH
                //sprintf(ewm_str,APP_DOWNLOAD_URL,WATCHCOM_GetDeviceModel(),g_zdt_phone_imei_1);
				sprintf(ewm_str,APP_DOWNLOAD_URL,yx_DB_Set_Rec.app_domain,g_zdt_phone_imei_1);
            #else
                sprintf(ewm_str,APP_DOWNLOAD_URL,yx_DB_Set_Rec.app_domain,g_zdt_phone_imei_1);
            #endif
                dec_src.file_name_ptr = QRCODE_BIND_FILE_NAME;
                
                //yangyu add later 
                GT_Qcode_show(ewm_str, QRCODE_DISP_X-large/2, QRCODE_DISP_Y-large/2-6, QRCODE_DISP_WIDTH+large, QRCODE_DISP_HEIGHT+large);
                break;
                //yangyu end

            }
            else if(qrcode_page_index == 2)
            {
                dec_src.file_name_ptr = QRCODE_MANUAL_FILE_NAME;
                //yangyu add later, for manual qrcode ,just use res image,not generate
                //GUIRES_DisplayImg(&dis_point,
                //&bg_rect,
                //PNULL,
                //win_id,
                //IMAGE_QRCODE_MANUAL,
                //&lcd_dev_info);
                //break;
                //yangyu end

            }
            if(IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info))
            {    
                Trace_Log_Print("IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info)");            
                return FALSE;
            }        
            w=dec_info.img_detail_info.bmp_info.img_width;
            h=dec_info.img_detail_info.bmp_info.img_height;
            Trace_Log_Print("HandleQrcodeWindow w=%d h=%d",w, h);
        if(w==0 && h ==0)
        {
        dis_point.x=QRCODE_DISP_X;
            dis_point.y=QRCODE_DISP_Y; 
            bg_rect.left =QRCODE_DISP_X;
            bg_rect.right=QRCODE_DISP_X + QRCODE_DISP_WIDTH;
            bg_rect.top=QRCODE_DISP_Y;
            bg_rect.bottom=QRCODE_DISP_Y+QRCODE_DISP_HEIGHT;
            GUIRES_DisplayImg(&dis_point,
                &bg_rect,
                PNULL,
                win_id,
                IMAGE_ZDT_YX_2VM,
                &lcd_dev_info);
        }
        pszRGBData=(uint8*)SCI_ALLOC_APP(w*h*2);
        if(NULL == pszRGBData)
        {
            Trace_Log_Print("NULL == pszRGBData");
            return FALSE;
        }

        if (IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle))
        {
            Trace_Log_Print("IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle)");
            SCI_FREE(pszRGBData);
            return FALSE;
        }        

        dec_in.handle = img_handle;
        //set decode target
        dec_in.is_dec_thumbnail = 0;
        dec_in.target_ptr       = pszRGBData;
        dec_in.target_buf_size  = w*h*2;
        dec_in.target_width     = w;
        dec_in.target_height    = h;
        dec_in.img_rect.left    = 0;
        dec_in.img_rect.top     = 0;
        dec_in.img_rect.right   = w-1;
        dec_in.img_rect.bottom  = h-1;
        dec_in.data_format = IMG_DEC_RGB565;

        //图形解码
        if(IMG_DEC_RET_SUCCESS != IMG_DEC_GetFrame(&dec_in,&dec_out))
        {
            IMG_DEC_Destroy(img_handle);
            SCI_FREE(pszRGBData);
            return FALSE;
        }
            zoom_in_data.src_buf_ptr = dec_in.target_ptr;
            zoom_in_data.src_buf_size = dec_in.target_buf_size;
            zoom_in_data.src_img_width = dec_out.real_width;
            zoom_in_data.src_img_height= dec_out.real_height;
            zoom_in_data.dest_img_width = QRCODE_DISP_WIDTH;
            zoom_in_data.dest_img_height = QRCODE_DISP_HEIGHT;
            zoom_in_data.dest_buf_size = (zoom_in_data.dest_img_width * zoom_in_data.dest_img_height * 2 + 255);
            zoom_in_data.dest_buf_ptr = SCI_ALLOCA(zoom_in_data.dest_buf_size);
            
            if (PNULL != zoom_in_data.dest_buf_ptr)
            {
                if (ZOOM_RESULT_SUCC == ZoomImage(&zoom_in_data, &zoom_out_data))
                {
                    Trace_Log_Print("is_zoom_success = TRUE;");
                    is_zoom_success = TRUE;
                }
                else
                {
                    Trace_Log_Print("is_zoom_success = FALSE;");
                    is_zoom_success = FALSE;
                }
            }
            else
            {
               Trace_Log_Print("is_zoom_success = FALSE");
                is_zoom_success = FALSE;
            }
            
        IMG_DEC_Destroy(img_handle);

       
        bitmap_handle.bit_ptr =  (zoom_in_data.dest_buf_ptr + zoom_out_data.dest_buf_offset);;
        bitmap_handle.img_width  = QRCODE_DISP_WIDTH; //dec_out.real_width;
        bitmap_handle.img_height = QRCODE_DISP_HEIGHT; //dec_out.real_height;
        
        dis_point.x=QRCODE_DISP_X; //lpRC->left;
        dis_point.y=QRCODE_DISP_Y; //lpRC->top;        

        bg_rect.left =QRCODE_DISP_X;
        bg_rect.right=QRCODE_DISP_X + QRCODE_DISP_WIDTH;
        bg_rect.top=QRCODE_DISP_Y;
        bg_rect.bottom=QRCODE_DISP_Y+QRCODE_DISP_HEIGHT;    
        
        GUIIMG_DisplayBmp(FALSE, &bg_rect, &dis_point, &bitmap_handle,&lcd_dev_info);

            if (PNULL != zoom_in_data.dest_buf_ptr)
            {
                SCI_FREE(zoom_in_data.dest_buf_ptr);
                zoom_in_data.dest_buf_ptr = PNULL;
            }
            
        SCI_FREE(pszRGBData);    
        
            break;
    case MSG_GET_FOCUS:
        GUI_FillRect(&lcd_dev_info, bg_rect, MMI_WHITE_COLOR);
                    MMIZDT_Check2VMFile();
                    //dec_src.file_name_ptr = QRCODE_FILE_NAME;
                    if(qrcode_page_index == 0)
                    {
                        uint16 img_w = 180*SCALE;
                        dec_src.file_name_ptr = QRCODE_DOWNLOAD_FILE_NAME;
                        //yangyu add, not generate
                        dis_point.x= (MMI_MAINSCREEN_WIDTH - img_w)/2;
                        dis_point.y= (MMI_MAINSCREEN_HEIGHT- img_w)/2; 
                        bg_rect.left = dis_point.x;
                        bg_rect.right= bg_rect.left + img_w;
                        bg_rect.top = dis_point.y;
                        bg_rect.bottom = bg_rect.top + img_w;
                        GUIRES_DisplayImg(&dis_point,
                                        &bg_rect,
                                        PNULL,
                                        win_id,
                                        IMAGE_ZDT_YX_2VM,
                                        &lcd_dev_info);
                        break;
        
                        //yangyu end
        
                    }
                    else if(qrcode_page_index == 1)
                    {
                        char ewm_str[200] = {0};
                        uint16 large = 20;
                        SCI_MEMSET(ewm_str, 0, 200*sizeof(char));
                        sprintf(ewm_str,"http://%s:8080/app/newIndex.html?imei=%s",yx_DB_Set_Rec.app_domain,g_zdt_phone_imei_1);
                        dec_src.file_name_ptr = QRCODE_BIND_FILE_NAME;
                        
                        //yangyu add later 
                        GT_Qcode_show(ewm_str, QRCODE_DISP_X-large/2, QRCODE_DISP_Y-large/2-6, QRCODE_DISP_WIDTH+large, QRCODE_DISP_HEIGHT+large);
                        break;
                        //yangyu end
        
                    }
                    else if(qrcode_page_index == 2)
                    {
                        dec_src.file_name_ptr = QRCODE_MANUAL_FILE_NAME;
                        //yangyu add later, for manual qrcode ,just use res image,not generate
                        //GUIRES_DisplayImg(&dis_point,
                        //&bg_rect,
                        //PNULL,
                        //win_id,
                        //IMAGE_QRCODE_MANUAL,
                        //&lcd_dev_info);
                        //break;
                        //yangyu end
        
                    }
                    if(IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info))
                    {   
                        Trace_Log_Print("IMG_DEC_RET_SUCCESS != IMG_DEC_GetInfo(&dec_src,&dec_info)");          
                        return FALSE;
                    }       
                    w=dec_info.img_detail_info.bmp_info.img_width;
                    h=dec_info.img_detail_info.bmp_info.img_height;
                    Trace_Log_Print("HandleQrcodeWindow w=%d h=%d",w, h);
                if(w==0 && h ==0)
                {
                dis_point.x=QRCODE_DISP_X;
                    dis_point.y=QRCODE_DISP_Y; 
                    bg_rect.left =QRCODE_DISP_X;
                    bg_rect.right=QRCODE_DISP_X + QRCODE_DISP_WIDTH;
                    bg_rect.top=QRCODE_DISP_Y;
                    bg_rect.bottom=QRCODE_DISP_Y+QRCODE_DISP_HEIGHT;
                    GUIRES_DisplayImg(&dis_point,
                        &bg_rect,
                        PNULL,
                        win_id,
                        IMAGE_ZDT_YX_2VM,
                        &lcd_dev_info);
                }
                pszRGBData=(uint8*)SCI_ALLOC_APP(w*h*2);
                if(NULL == pszRGBData)
                {
                    Trace_Log_Print("NULL == pszRGBData");
                    return FALSE;
                }
        
                if (IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle))
                {
                    Trace_Log_Print("IMG_DEC_RET_SUCCESS != IMG_DEC_Create(&dec_src,&img_handle)");
                    SCI_FREE(pszRGBData);
                    return FALSE;
                }       
        
                dec_in.handle = img_handle;
                //set decode target
                dec_in.is_dec_thumbnail = 0;
                dec_in.target_ptr       = pszRGBData;
                dec_in.target_buf_size  = w*h*2;
                dec_in.target_width     = w;
                dec_in.target_height    = h;
                dec_in.img_rect.left    = 0;
                dec_in.img_rect.top     = 0;
                dec_in.img_rect.right   = w-1;
                dec_in.img_rect.bottom  = h-1;
                dec_in.data_format = IMG_DEC_RGB565;
        
                //图形解码
                if(IMG_DEC_RET_SUCCESS != IMG_DEC_GetFrame(&dec_in,&dec_out))
                {
                    IMG_DEC_Destroy(img_handle);
                    SCI_FREE(pszRGBData);
                    return FALSE;
                }
                    zoom_in_data.src_buf_ptr = dec_in.target_ptr;
                    zoom_in_data.src_buf_size = dec_in.target_buf_size;
                    zoom_in_data.src_img_width = dec_out.real_width;
                    zoom_in_data.src_img_height= dec_out.real_height;
                    zoom_in_data.dest_img_width = QRCODE_DISP_WIDTH;
                    zoom_in_data.dest_img_height = QRCODE_DISP_HEIGHT;
                    zoom_in_data.dest_buf_size = (zoom_in_data.dest_img_width * zoom_in_data.dest_img_height * 2 + 255);
                    zoom_in_data.dest_buf_ptr = SCI_ALLOCA(zoom_in_data.dest_buf_size);
                    
                    if (PNULL != zoom_in_data.dest_buf_ptr)
                    {
                        if (ZOOM_RESULT_SUCC == ZoomImage(&zoom_in_data, &zoom_out_data))
                        {
                            Trace_Log_Print("is_zoom_success = TRUE;");
                            is_zoom_success = TRUE;
                        }
                        else
                        {
                            Trace_Log_Print("is_zoom_success = FALSE;");
                            is_zoom_success = FALSE;
                        }
                    }
                    else
                    {
                       Trace_Log_Print("is_zoom_success = FALSE");
                        is_zoom_success = FALSE;
                    }
                    
                IMG_DEC_Destroy(img_handle);
        
               
                bitmap_handle.bit_ptr =  (zoom_in_data.dest_buf_ptr + zoom_out_data.dest_buf_offset);;
                bitmap_handle.img_width  = QRCODE_DISP_WIDTH; //dec_out.real_width;
                bitmap_handle.img_height = QRCODE_DISP_HEIGHT; //dec_out.real_height;
                
                dis_point.x=QRCODE_DISP_X; //lpRC->left;
                dis_point.y=QRCODE_DISP_Y; //lpRC->top;     
        
                bg_rect.left =QRCODE_DISP_X;
                bg_rect.right=QRCODE_DISP_X + QRCODE_DISP_WIDTH;
                bg_rect.top=QRCODE_DISP_Y;
                bg_rect.bottom=QRCODE_DISP_Y+QRCODE_DISP_HEIGHT;    
                
                GUIIMG_DisplayBmp(FALSE, &bg_rect, &dis_point, &bitmap_handle,&lcd_dev_info);
        
                    if (PNULL != zoom_in_data.dest_buf_ptr)
                    {
                        SCI_FREE(zoom_in_data.dest_buf_ptr);
                        zoom_in_data.dest_buf_ptr = PNULL;
                    }
                    
                SCI_FREE(pszRGBData);

        break;
            
    case MSG_FULL_PAINT:
            //MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_BH_NET_2VM_INFO,&sn_string);
             if(qrcode_page_index == 0)
            {
                MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_NET_2VM_DOWNLOAD,&sn_string);

            }
            else if(qrcode_page_index == 1)
            {
                MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_NET_2VM_BIND,&sn_string);

            }
            text_style.align = ALIGN_HVMIDDLE; // 这里设置为Left,因为Label的位置自有自己调整区域
            text_style.font_color = MMI_BLACK_COLOR;
            text_style.angle = ANGLE_0;
            text_style.char_space = 0;
            text_style.effect = FONT_EFFECT_CUSTOM;
            text_style.edge_color = 0;

            text_style.font = SONG_FONT_24;
            text_style.line_space = 0;
            text_style.region_num = 0;
            text_style.region_ptr = PNULL;        
            text_display_rect.left = 0;
            text_display_rect.top = 0;
            text_display_rect.right=(MMI_MAINSCREEN_WIDTH -1);
            text_display_rect.bottom=39;    
            
            GUISTR_GetStringInfo(&text_style, &sn_string, state, &text_info);
            if(qrcode_page_index != 2) 
              {  
              #if 0 //yangyu delete ,
            GUISTR_DrawTextToLCDInRect( 
                    MMITHEME_GetDefaultLcdDev(),
                    (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
                    (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
                    (const MMI_STRING_T    *)&sn_string,
                    &text_style,
                    state,
                    GUISTR_TEXT_DIR_AUTO
                    );
            #endif
            }
        
            text_style.font = SONG_FONT_28;
            text_style.line_space = 0;
            text_style.region_num = 0;
            text_style.region_ptr = PNULL;        
            text_display_rect.left = 0;
            text_display_rect.top = QRCODE_DISP_Y+QRCODE_DISP_HEIGHT+1;
            text_display_rect.right=(MMI_MAINSCREEN_WIDTH -1);
            text_display_rect.bottom=(MMI_MAINSCREEN_WIDTH -1);
                temp_ptr = (char*)g_zdt_phone_imei_1;
                temp_len = SCI_STRLEN((char*)temp_ptr);
                MMIAPICOM_StrcatFromStrToUCS2(uint16_str, &uint16_str_len, (uint8*)temp_ptr, temp_len);
                MMIAPICOM_StrcatFromSTRINGToUCS2(uint16_str, &uint16_str_len, &cr_s);//换行
                imei_string.wstr_ptr = uint16_str;
                imei_string.wstr_len = uint16_str_len;

              if(qrcode_page_index == 1) 
              {
              
                  GUISTR_DrawTextToLCDInRect( 
                      MMITHEME_GetDefaultLcdDev(),
                      (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
                      (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
                      (const MMI_STRING_T    *)&imei_string,
                      &text_style,
                      state,
                      GUISTR_TEXT_DIR_AUTO
                      );
            
            }
        break;
        
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            s_2vm_tp_x = point.x;
            s_2vm_tp_y = point.y;
            s_is_2vm_tp_down = TRUE;
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            MMI_WIN_ID_T	query_win_id = MMIZDT_QUERY_WIN_ID;
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(s_is_2vm_tp_down)
            {
                offset_y =  point.y - s_2vm_tp_y;
                offset_x =  point.x - s_2vm_tp_x;  
                if(offset_x >= (20))
                {
                    MMK_CloseWin(win_id);
                }
                //yangyu add for unbind
                if(point.x > MMI_MAINSCREEN_WIDTH/3 && point.y > MMI_MAINSCREEN_HEIGHT/3)
                {
                    unbind_click_count++;
                    if(unbind_click_count >= 18)
                    {
                        unbind_click_count = 0;
                        MMIPUB_OpenQueryWinByTextId(TXT_YX_UNBIND,IMAGE_PUBWIN_QUERY,&query_win_id,MMIZDT_Device_Unbind); 
                    }
                }
                else
                {
                    unbind_click_count = 0;
                }
                //yangyu end

            }
            s_is_2vm_tp_down = FALSE;
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527
            
        case MSG_APP_WEB:
                //ZDT_SetIdleWifiIcon(TRUE);
                //MAIN_SetIdleRoamingState(MN_DUAL_SYS_1,TRUE);
            break;
            
        case MSG_APP_OK:
                //ZDT_SetIdleWifiIcon(FALSE);
                //MAIN_SetIdleRoamingState(MN_DUAL_SYS_1,FALSE);
            break;
            
        case MSG_KEYDOWN_CANCEL:
            break;
        case MSG_KEYUP_CANCEL:
            unbind_click_count = 0;
            MMK_CloseWin(win_id);         
            break;

        case MSG_KEYDOWN_RED:
            break;

        case MSG_KEYUP_RED:
            MMK_CloseWin(win_id);
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}


LOCAL MMI_RESULT_E  HandleZDT_Watch2VMSelectWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );


WINDOW_TABLE( MMIZDT_WATCH_2VM_SELECT_WIN_TAB ) = {
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ),
    WIN_FUNC( (uint32)HandleZDT_Watch2VMSelectWinMsg),
    WIN_ID(MMIZDT_WATCH_2VM_SELECT_WIN_ID),
    //WIN_TITLE(TXT_BH_NET_2VM),
    //WIN_SOFTKEY(TXT_NULL, TXT_NULL, STXT_RETURN),    
    //CREATE_ANIM_CTRL (MMISET_QRCODE_CTRL_ID, MMISET_SET_QRCODE_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL uint16 w_down_x = 0;


LOCAL MMI_RESULT_E  HandleZDT_Watch2VMSelectWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T            ctrl_id = MMIZDT_TEST_MENU_CTRL_ID;
    uint16                    node_id = 0;
    static BOOLEAN            s_need_update   = FALSE;
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
  //  MMISET_VALIDATE_PRIVACY_RETURN_T    result_info = {MMISET_PROTECT_CL, FALSE};    
#if defined(DISABLE_STATUSBAR_DISPLAY) 
    GUI_RECT_T                          title_rect =   {0, 0, (MMI_MAINSCREEN_WIDTH -1), 23};
#else
    GUI_RECT_T                          title_rect = {0, 18, (MMI_MAINSCREEN_WIDTH -1), 18 + 23};
#endif
    GUI_RECT_T                          full_rect = {0, 0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_HEIGHT -1)};

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
    {
      qrcode_page_index = 0;
    }

        break;
    case MSG_FULL_PAINT:
    {
    #ifndef ZDT_NEW_BIND_STYLE   
        GUI_FillRect(&lcd_dev_info, full_rect, MMI_BLACK_COLOR);
        GUIRES_DisplayImg(PNULL,
                &full_rect,
                PNULL,
                win_id,
                IMAGE_QRCODE_SELECT_BG,
                &lcd_dev_info);
    #endif
    }

        break;        
     case MSG_TP_PRESS_DOWN:
    {
        GUI_POINT_T   point = {0};
        point.x = MMK_GET_TP_X(param);
        point.y = MMK_GET_TP_Y(param);
       
        w_down_x = point.x;

    }          
        break;        
    case MSG_TP_PRESS_MOVE:
    {
       GUI_POINT_T   point = {0};
       point.x = MMK_GET_TP_X(param);
       point.y = MMK_GET_TP_Y(param);
    

    }          
        break;
    case MSG_TP_PRESS_UP:
    {
       GUI_POINT_T   point = {0};
       point.x = MMK_GET_TP_X(param);
       point.y = MMK_GET_TP_Y(param);
       if(point.y < MMI_MAINSCREEN_HEIGHT/2)
       {
            qrcode_page_index = 0;
       }
       else
       {
           qrcode_page_index = 1;
       }
       if(w_down_x == point.x && point.y < MMI_MAINSCREEN_HEIGHT-1) 
       {
            MMIZDT_Open2VMWin();
       }
    }          
        break;        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:         
        break;

    case MSG_KEYDOWN_RED:
        break;

    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;
       
    case MSG_KEYDOWN_CANCEL:
        break;
    case MSG_KEYUP_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}


PUBLIC void MMIZDT_OpenManualWin()
{
    qrcode_page_index = 2;
    MMIZDT_Open2VMWin();
}


//zhou 秒表

PUBLIC void MMI_OpenStopwWin()
{
#ifdef STOPWATCH_SUPPORT
    MMIAPIST_OpenStopWatchWin();
#endif
}

PUBLIC void MMIZDT_Open2VMSelectWin(void)
{
    MMK_CreateWin((uint32 *)MMIZDT_WATCH_2VM_SELECT_WIN_TAB,PNULL);
}

WINDOW_TABLE( MMIZDT_WATCH_2VM_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_2VMWinMsg),    
    WIN_ID( MMIZDT_WATCH_2VM_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

PUBLIC void MMIZDT_Open2VMWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_WATCH_2VM_WIN_TAB,PNULL);
}

BOOLEAN MMIZDT_Close2VMWin(void)
{
    if(MMK_IsOpenWin(MMIZDT_WATCH_2VM_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WATCH_2VM_WIN_ID);
    }
    return TRUE;
}
#endif

LOCAL MMI_RESULT_E HandleZDT_BHHelpWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E            result = MMI_RESULT_TRUE;
    MMI_STRING_T    text_s = {0};
#if defined(ZDT_MMI_USE_SMALL_FONT)       
    GUI_FONT_T font = SONG_FONT_22 ;
#else
    GUI_FONT_T font = SONG_FONT_28 ;
#endif
    GUI_COLOR_T color = MMI_WHITE_COLOR ;//MMI_WHITE_COLOR;
    switch (msg_id) 
    {
    case MSG_OPEN_WINDOW:
       
      MMK_SetAtvCtrl(win_id, MMIZDT_YX_HELP_CTRL_ID);

    MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_BH_NET_HELP_INFO,&text_s);
    GUITEXT_SetFont(MMIZDT_YX_HELP_CTRL_ID, &font, &color );
    GUITEXT_SetString(MMIZDT_YX_HELP_CTRL_ID, text_s.wstr_ptr,text_s.wstr_len, TRUE);

        break;

    case MSG_FULL_PAINT:
     break;
        
    case MSG_APP_WEB:       
    case MSG_CTL_MIDSK:
    case MSG_KEYDOWN_OK:
    case MSG_CTL_OK:
    case MSG_KEYDOWN_CANCEL:
    case MSG_CTL_CANCEL:         
   
        MMK_CloseWin(win_id);
        break;

         
    default:
        result = MMI_RESULT_FALSE;
        break;
    }
    return (result);
}


WINDOW_TABLE(MMIZDT_BH_HELP_WIN_TAB) = 
{
    //CLEAR_LCD,
    //WIN_PRIO( WIN_ONE_LEVEL ),
    WIN_FUNC((uint32)HandleZDT_BHHelpWinMsg),
    WIN_ID(MMIZDT_BH_HELP_WIN_ID),
    WIN_TITLE(TXT_BH_NET_HELP),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),    
        CREATE_TEXT_CTRL(MMIZDT_YX_HELP_CTRL_ID),
    END_WIN
};

PUBLIC void MMIZDT_OpenHelpWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_BH_HELP_WIN_TAB, PNULL);
}

WINDOW_TABLE( MMIZDT_BH_MENU_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_BHMainMenuWinMsg),    
    WIN_ID( MMIZDT_BH_MENU_WIN_ID ),
    WIN_TITLE(TXT_BH_NET_LOC),
    CREATE_MENU_CTRL(MENU_ZDT_YX_NET, MMIZDT_YX_MENU_CTRL_ID),
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    END_WIN
};

BOOLEAN MMIZDT_OpenMainMenuWin(void)
{
    MMK_CreateWin((uint32*)MMIZDT_BH_MENU_WIN_TAB,PNULL);
    
    return TRUE;
}
LOCAL MMI_RESULT_E HandleZDT_BHMainMenuWinMsg (
                                      MMI_WIN_ID_T   win_id,     // 窗口的ID
                                      MMI_MESSAGE_ID_E     msg_id,     // 窗口的内部消息ID
                                      DPARAM            param        // 相应消息的参数
                                      )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;

    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
            GUIMENU_GetId(MMIZDT_YX_MENU_CTRL_ID,&group_id,&menu_id);
            MMK_SetAtvCtrl(win_id, MMIZDT_YX_MENU_CTRL_ID);
            break;

        case MSG_CTL_OK:
        case MSG_APP_OK:
        case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            GUIMENU_GetId(MMIZDT_YX_MENU_CTRL_ID,&group_id,&menu_id);
            switch (menu_id)
            {

            case ID_ZDT_MENU_BH_ONOFF:
                        MMIZDT_OpenOnOffWin();
                break;
                
            case ID_ZDT_MENU_BH_CHAT:
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
                        MMIZDT_OpenTinyChatWin();
#endif
                break;
                
            case ID_ZDT_MENU_BH_2VM:
                        MMIZDT_Open2VMWin();
                break;
                
            case ID_ZDT_MENU_BH_HELP:
                        MMIZDT_OpenHelpWin();
                break;
                
                default:
                    break;
            }
            break;

        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}

#if 1
MMI_HANDLE_T g_s_voice_prompt_win_handle = 0;
PUBLIC MMI_RESULT_E MMIVoice_HandleNewMsgWin(
                                    MMI_WIN_ID_T        win_id,        //IN:
                                    MMI_MESSAGE_ID_E    msg_id,        //IN:
                                    DPARAM                param        //IN:
                                    )
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    MN_DUAL_SYS_E dual_sys = MN_DUAL_SYS_1;
    
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        //如果屏保被点亮中，则关闭屏保窗口
        if (MMK_IsOpenWin(MMIMAIN_SHOWTIME_WIN_ID))
        {
            MMK_CloseWin(MMIMAIN_SHOWTIME_WIN_ID);
        }
        break;

    case MSG_FULL_PAINT:
        
            MMIPUB_SetWinSoftkey(win_id, TXT_READ, STXT_EXIT, FALSE);

            recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;

    case MSG_TIMER:
            recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;

    case MSG_APP_WEB:
    case MSG_APP_OK:
    case MSG_CTL_OK:
        
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
#ifdef ZDT_PLAT_YX_SUPPORT_FRIEND
    MMIZDT_OpenChatWinByRcv();
#else       //ZDT_PLAT_YX_SUPPORT_FRIEND
    MMIZDT_OpenTinyChatWin();
#endif      //ZDT_PLAT_YX_SUPPORT_FRIEND
#endif

            MMK_CloseWin( win_id );
        break;

    case MSG_KEYUP_CANCEL:
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin( win_id );
        break;

    case MSG_KEYDOWN_RED:
        break;

    case MSG_KEYUP_RED:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:

        recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;

    default:
        recode = MMIPUB_HandleAlertWinMsg(win_id,msg_id,param);
        break;
    }
    
    return (recode);
}

PUBLIC void CloseVoiceNewMsgDialog()
{
    //MMIPUB_CloseAlertWin();
    MMK_CloseWin(MMIVOICE_NEWMSG_WIN_ID);

}


LOCAL void ShowVoiceNewMsgPrompt(void)
{
    uint8 disp_str_len = 0;
    uint8 disp_num_len = 0;
    uint8 new_msg_num[6] = {0};
    wchar disp_num[10] = {0};
    uint16 to_be_read_num = 1;
    MMI_WIN_PRIORITY_E win_priority = WIN_LOWEST_LEVEL;
    MMI_STRING_T prompt_str = {0};
    uint32 time_out = 0;
    MMI_WIN_ID_T alert_win_id = MMIVOICE_NEWMSG_WIN_ID;
    
    prompt_str.wstr_len = 0;
    prompt_str.wstr_ptr = PNULL;

    if (0 == to_be_read_num)
    {
        return;
    }
        
    MMI_GetLabelTextByLang(TXT_YX_NEW_VOICE_MSG, &prompt_str);
    
    if (MMK_IsFocusWin(MMIVOICE_NEWMSG_WIN_ID))
    {
        // set to window
        MMIPUB_SetAlertWinTextByPtr(alert_win_id,&prompt_str,PNULL,TRUE);
    }
    else
    {
        MMK_CloseWin(MMIVOICE_NEWMSG_WIN_ID);

        // 窗口: 提示用户新短消息
#ifdef SCREENSAVER_SUPPORT
        if (MMK_IsFocusWin(MMIIDLE_SCREENSAVER_WIN_ID))
        {
            MMIAPIIDLESS_CloseScreenSaver();
        }
#endif

        if(MMISMS_IsForbitNewMsgWin(FALSE))
        {
            win_priority = WIN_LOWEST_LEVEL;
        }
        else
        {
            win_priority = WIN_ONE_LEVEL;
        }
        MMIPUB_OpenAlertWinByTextPtr(&time_out,&prompt_str,PNULL,IMAGE_PUBWIN_NEWMSG,&alert_win_id,&win_priority,MMIPUB_SOFTKEY_CUSTOMER,MMIVoice_HandleNewMsgWin);
        g_s_voice_prompt_win_handle = (MMI_HANDLE_T)alert_win_id; 
        MMIPUB_SetWinTitleTextId(MMIVOICE_NEWMSG_WIN_ID, TXT_COMMON_PROMPT, FALSE);
    }
}

LOCAL void YX_PlayRcvVoiceMsgTone()
{
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    if(!MMIZDT_IsInTinyChatWin())
#endif
    {
        //MMIAPISET_PlayRing(MN_DUAL_SYS_1, FALSE, 0, 1, MMISET_RING_TYPE_MSG, PNULL);
        MMIAPISET_PlayCallRingByVol(MMIAPISET_GetCallRingVolume(), 1,1, MMISET_RING_TYPE_MSG, PNULL);//yangyu modify
    }
}
#ifdef BAIRUI_VIDEOCHAT_SUPPORT
LOCAL BOOLEAN isVideoChatOverDialog = FALSE;

PUBLIC void DelayShowNewMsgPrompt()
{
    if(isVideoChatOverDialog)
    {
        isVideoChatOverDialog = FALSE;
        ShowVoiceNewMsgPrompt();
    }
}
#endif
PUBLIC void MMIVoice_ShowNewMsgPrompt(
                             BOOLEAN    is_play_ring
                             )
{
    MMIDEFAULT_TurnOnBackLight();
    if (MMIZDT_IsInClassModeWin()
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
    || MMIZDT_IsInTinyChatWin()
#endif
    )
    {
        return;
    }

#ifdef BAIRUI_VIDEOCHAT_SUPPORT
    if(VideoChat_IsInCall())
    {
        isVideoChatOverDialog = TRUE;
        return;
    }

#endif
#ifndef WIN32    
#ifdef LEBAO_MUSIC_SUPPORT
    lebao_exit();
#endif  
#ifdef XYSDK_SUPPORT
        LIBXMLYAPI_AppExit();
#endif
#endif
    ZMTApp_CloseRecordAndPlayer();
    if(is_play_ring)
    {
        YX_PlayRcvVoiceMsgTone();
    }
#ifdef BAIRUI_VIDEOCHAT_SUPPORT

    if(!VideoChat_IsLogin()) //yangyu add
#endif
    {
#ifdef ZDT_PLAT_YX_SUPPORT_VOICE
        //MMIZDT_OpenTinyChatWin();
#endif
        if(!MMIZDT_ShouldStayInChargingWin())//充电中只提醒铃声不弹框
        {
            ShowVoiceNewMsgPrompt();
        }
    }

}
#endif
#endif //ZDT_PLAT_YX_SUPPORT

#if defined(ZDT_NEW_BIND_STYLE)
LOCAL uint16 w_press_down_x = 0;
LOCAL uint8 w_press_button_rec[4] ={183,13,233,173};


/////////////////////////////////////////////SETP 2///////////////////////////////////////

LOCAL MMI_RESULT_E  HandleZDT_WatchBindStep2WinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );

WINDOW_TABLE( MMIZDT_WATCH_BIND_STEP2_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)HandleZDT_WatchBindStep2WinMsg),
    WIN_ID(MMIZDT_WATCH_BIND_STEP2_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E  HandleZDT_WatchBindStep2WinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUI_RECT_T                          full_rect = {0, 0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_WIDTH -1)};
    char ewm_str[200] = {0};
    uint16 large = 20;
    uint16 uint16_str[22] = {0};
    uint16 uint16_str_len = 0;
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    MMI_STRING_T imei_string = {0};
    char*           temp_ptr = PNULL;
    uint16 temp_len = 0;

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        break;
        
    case MSG_FULL_PAINT:
        GUIRES_DisplayImg(PNULL,
                &full_rect,
                PNULL,
                win_id,
                IMAGE_WATCH_BIND_STEP_2,
                &lcd_dev_info);

            MMIZDT_Check2VMFile();
        SCI_MEMSET(ewm_str, 0, 200*sizeof(char));
        sprintf(ewm_str,"http://%s:8080/app/newIndex.html?imei=%s",yx_DB_Set_Rec.app_domain,g_zdt_phone_imei_1);
        //dec_src.file_name_ptr = QRCODE_BIND_FILE_NAME;
        GT_Qcode_show(ewm_str, 4, 0, 174, 174); 

        text_style.font = SONG_FONT_25;
        text_style.align= ALIGN_HVMIDDLE;
        text_style.line_space = 0;
        text_style.region_num = 0;
        text_style.region_ptr = PNULL;        
        text_display_rect.left = 0; // 30;
        text_display_rect.top = 208;
        text_display_rect.right=(MMI_MAINSCREEN_WIDTH -1);
        text_display_rect.bottom=240;
        temp_ptr = (char*)g_zdt_phone_imei_1;
        temp_len = SCI_STRLEN((char*)temp_ptr);
        MMIAPICOM_StrcatFromStrToUCS2(uint16_str, &uint16_str_len, (uint8*)temp_ptr, temp_len);
        imei_string.wstr_ptr = uint16_str;
        imei_string.wstr_len = uint16_str_len;

        GUISTR_DrawTextToLCDInRect( 
                      MMITHEME_GetDefaultLcdDev(),
                      (const GUI_RECT_T      *)&text_display_rect,       //the fixed display area
                      (const GUI_RECT_T      *)&text_display_rect,       //用户要剪切的实际区域
                      (const MMI_STRING_T    *)&imei_string,
                      &text_style,
                      state,
                      GUISTR_TEXT_DIR_AUTO
                      );
        break;
        
    case MSG_TP_PRESS_DOWN:
    {
        GUI_POINT_T   point = {0};
        point.x = MMK_GET_TP_X(param);
        point.y = MMK_GET_TP_Y(param);
       
        w_press_down_x = point.x;

    }          
        break;  
        
    case MSG_TP_PRESS_MOVE:
        break;
        
    case MSG_TP_PRESS_UP:
    {
       GUI_POINT_T   point = {0};
       point.x = MMK_GET_TP_X(param);
       point.y = MMK_GET_TP_Y(param);
       
       if(w_press_down_x == point.x &&    point.x >= w_press_button_rec[0]   &&    point.x <= w_press_button_rec[2]
           &&  point.y >= w_press_button_rec[1]   &&    point.y <= w_press_button_rec[3]) 
       {
                   MMK_CloseWin(win_id);
       }
    }          
        break;
        
    case MSG_APP_UP:
    case MSG_APP_DOWN:
    case MSG_APP_GREEN:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {

    }          
        break;
       
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

PUBLIC void MMIZDT_OpenBindStep2Win(void)
{
    if(MMK_IsOpenWin(MMIZDT_WATCH_BIND_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WATCH_BIND_WIN_ID);
    }

    MMK_CreateWin((uint32 *)MMIZDT_WATCH_BIND_STEP2_WIN_TAB,PNULL);
}



/////////////////////////////////////////////SETP 1///////////////////////////////////////
LOCAL MMI_RESULT_E  HandleZDT_WatchBindWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );

WINDOW_TABLE( MMIZDT_WATCH_BIND_WIN_TAB ) = 
{
    WIN_FUNC( (uint32)HandleZDT_WatchBindWinMsg),
    WIN_ID(MMIZDT_WATCH_BIND_WIN_ID),
    WIN_HIDE_STATUS,
    END_WIN
};

LOCAL MMI_RESULT_E  HandleZDT_WatchBindWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUI_RECT_T                          full_rect = {0, 0, (MMI_MAINSCREEN_WIDTH -1), (MMI_MAINSCREEN_WIDTH -1)};

    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        unbind_click_count = 0;        
        break;
        
    case MSG_FULL_PAINT:
        GUIRES_DisplayImg(PNULL,
                &full_rect,
                PNULL,
                win_id,
                IMAGE_WATCH_BIND_STEP_1,
                &lcd_dev_info);
        break;      
        
    case MSG_TP_PRESS_DOWN:
    {
        GUI_POINT_T   point = {0};
        point.x = MMK_GET_TP_X(param);
        point.y = MMK_GET_TP_Y(param);
       
        w_press_down_x = point.x;

        s_2vm_tp_x = point.x;
        s_2vm_tp_y = point.y;
        s_is_2vm_tp_down = TRUE;
    }         
        break;  
        
    case MSG_TP_PRESS_MOVE:
        break;
        
    case MSG_TP_PRESS_UP:
    {
       GUI_POINT_T   point = {0};
       int offset_y =  0;//up to down
       int offset_x =  0;//up to down    
       
       point.x = MMK_GET_TP_X(param);
       point.y = MMK_GET_TP_Y(param);
       
       if(w_press_down_x == point.x &&    point.x >= w_press_button_rec[0]   &&    point.x <= w_press_button_rec[2]
           &&  point.y >= w_press_button_rec[1]   &&    point.y <= w_press_button_rec[3]) 
       {
            MMIZDT_OpenBindStep2Win();
       }

            if(s_is_2vm_tp_down)
            {
                offset_y =  point.y - s_2vm_tp_y;
                offset_x =  point.x - s_2vm_tp_x;
                if(offset_x <= -(20))    
                {
                    MMK_CloseWin(win_id);
                }    
                else if(offset_x >= (20))
                {
                    MMK_CloseWin(win_id);
                }
                if(point.x < MMI_MAINSCREEN_WIDTH/3 && point.y > MMI_MAINSCREEN_HEIGHT/3)
                {
                    unbind_click_count++;
                    if(unbind_click_count >= 12)
                    {
                        unbind_click_count = 0;
                        YX_API_DLT_Send();
                    }
                }
                else
                {
                    unbind_click_count = 0;
                }
            }
            s_is_2vm_tp_down = FALSE;
            
    }          
        break;
        
    case MSG_APP_UP:
    case MSG_APP_DOWN:
    case MSG_APP_GREEN:
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {
            MMIZDT_OpenBindStep2Win();
    }          
        break;
       
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        unbind_click_count = 0;    
        MMK_CloseWin(win_id);
        break;

    case MSG_CLOSE_WINDOW:
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

PUBLIC void MMIZDT_OpenBindWin(void)
{
    MMK_CreateWin((uint32 *)MMIZDT_WATCH_BIND_WIN_TAB,PNULL);
}

#endif

#if 1  //联系人窗口
#define ZDT_PB_ICON_LEFT_START        0
#define ZDT_PB_ICON_TOP_START        10
#define ZDT_PB_ICON_WIDTH        120
#define ZDT_PB_ICON_HEIGHT        75
#define ZDT_PB_TEXT_HEIGHT        30

uint16 m_yx_cur_whitelist_idx = 0;
static uint16 m_yx_whitelist_totalnum=0;
static uint16 m_yx_whitelist_pagenum=0;
static uint16 m_yx_whitelist_curpage=0;
static uint16 m_yx_whitelist_curindex=0;
static int s_pb_tp_x;
static int s_pb_tp_y;
static BOOLEAN s_is_pb_tp_down = FALSE;

PUBLIC void ZDT_PB_GetContactNameByNumber(uint8 *number, uint16 *dst_name)
{
    uint8 i = 0;
    if(number == NULL || dst_name == NULL)
    {
        return;
    }
    for(i = 0; i < YX_DB_WHITE_MAX_SUM; i++)
    {
        
        if(strcmp(yx_DB_White_Reclist[i].white_num,  number) == 0)
        {
            ZDT_UCS_Str16_to_uint16((uint8*)yx_DB_White_Reclist[i].white_name, SCI_STRLEN(yx_DB_White_Reclist[i].white_name) ,dst_name, 100);
            return;
        }
    }  

}

PUBLIC MMI_IMAGE_ID_T ZDT_PB_GetDispInfoContent(uint8 *tele_num, uint8 tele_len, wchar *in_str, BOOLEAN is_mo_call)
{
    MMI_STRING_T        nameStr = {0};
    MMI_IMAGE_ID_T               imageId = 0;
    BOOLEAN             isEmergencyCall = FALSE;

    if((tele_num == PNULL) || (in_str == PNULL))
    {
        return NULL;
    }
    isEmergencyCall = MMIAPICC_IsEmergencyNum(tele_num, tele_len, MMICC_GetWantCallDualSys(), FALSE);
    //get info for display
    if(isEmergencyCall && is_mo_call )
    {
#ifdef MODEM_SUPPORT_RDA
        if(0 != tele_len)
        {
            MMI_STRNTOWSTR(in_str,MMIPB_NUMBER_MAX_STRING_LEN,tele_num,tele_len,tele_len);
        }
#else
        MMI_GetLabelTextByLang(TXT_PB_EMERGENCY, &nameStr);
        MMI_WSTRNCPY(in_str,MMIPB_NAME_MAX_LEN,(const wchar*)nameStr.wstr_ptr, nameStr.wstr_len,nameStr.wstr_len);
#endif
        imageId = IMAGE_ZDT_PB_FATHER;//IAMGE_CALL_PORTRAIT_EMERGENCY;
    }
    else
    {
        if (0 != tele_len)
        {
            uint8 i = 0;
            for(i = 0; i < YX_DB_WHITE_MAX_SUM; i++)
            {
                //这个地方有问题比如+86135xxxxxx 就匹配不了
                if(strcmp(yx_DB_White_Reclist[i].white_num, tele_num) == 0)
                {
                    ZDT_UCS_Str16_to_uint16((uint8*)yx_DB_White_Reclist[i].white_name, SCI_STRLEN(yx_DB_White_Reclist[i].white_name) ,in_str, 100);                  
                #ifdef ZTE_WATCH
                    imageId = WATCHCOM_GetAvaterBigImageId(in_str);
                #else
                    imageId = WATCHCOM_GetAvaterImageId(in_str);
                #endif
                    break;
                }
            }  
        }
    }
    //no name in pb and is not emergency
    if (imageId == 0)
    {
    #ifdef ZTE_WATCH
        imageId = IMAGE_ZDT_PB_OTHER_BIG;
    #else
        imageId = IMAGE_ZDT_PB_OTHERMAN;
    #endif
        if(0 != tele_len)
        {
            MMI_STRNTOWSTR(in_str,MMIPB_NUMBER_MAX_STRING_LEN,tele_num,tele_len,tele_len);
        }
        else
        {
            MMI_STRING_T  dispStr = {0};
            MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_UNKNOW_NUM, &dispStr);
            MMI_WSTRNCPY(in_str, MMIPB_NAME_MAX_LEN, dispStr.wstr_ptr, dispStr.wstr_len, dispStr.wstr_len);
        }
    }
    return imageId;
}

PUBLIC BOOLEAN ZDT_Number_Is_In_Contact(uint8 *number)
{
    uint8 i = 0;
    if(number == NULL)
    {
        return FALSE;
    }
    ZDT_LOG("ZDT_Number_Is_In_Contact number:%s", number);
    for(i = 0; i < YX_DB_WHITE_MAX_SUM; i++)
    {
        
        if(strcmp(yx_DB_White_Reclist[i].white_num,  number) == 0)
        {
            return TRUE;
        }
    }  
    return FALSE;

}

PUBLIC BOOLEAN MMIZDT_Check_Auto_Answer_Call()
{
    uint8   telNum[MMICC_PHONE_NUM_MAX_LEN + 2] = {0};
    uint8   telNumLen = 0;
    MMICC_GetCallNumStrByIndex(telNum, &telNumLen, MMICC_GetCurrentCallIndex());
    if(MMIZDT_Get_Auto_Answer_Call_Status() == 1 && ZDT_Number_Is_In_Contact(telNum) == TRUE)
    {
        return TRUE;
    }
    return FALSE;
}

PUBLIC BOOLEAN ZDT_Reject_UnknownCall()
{
    uint8 res = ZDT_NV_GetRejectUnknownCall();
    ZDT_LOG("zdt__ ZDT_Reject_UnknownCal res=%d",res);
    return res;
}

PUBLIC BOOLEAN ZDT_Reject_Call(uint8 *number)
{
    ZDT_LOG("zdt__ ZDT_Reject_Call, number=%s", number);
    return  !ZDT_Number_Is_In_Contact(number);
}

PUBLIC BOOLEAN ZDT_Device_Disable( GUI_POINT_T tp_point)
{
    BOOLEAN isLogOn = FALSE;
    MMIENG_GetUSBLog(&isLogOn);
    return (MMIZDT_ShouldStayInChargingWin() || MMIZDT_ShouldStayInClassModeWin())/* && !isLogOn*/;
}

extern uint16  MMIZDT_PB_GetNext(uint16 white_idx)
{
    uint16 i = 0;
    uint16 next_idx = 0;
    
    for(i = white_idx; i < YX_DB_WHITE_MAX_SUM; i++)
    {
        if(yx_DB_White_Reclist[i].db_id != 0 && SCI_STRLEN(yx_DB_White_Reclist[i].white_num) > 0)
        {
            next_idx = i+1;
            break;
        }
    }

    if(next_idx == 0)
    {
        for(i = 0; i < YX_DB_WHITE_MAX_SUM; i++)
        {
            if(yx_DB_White_Reclist[i].db_id != 0 && SCI_STRLEN(yx_DB_White_Reclist[i].white_num) > 0)
            {
                next_idx = i+1;
                break;
            }
        }
    }
    return next_idx;
}

BOOLEAN MMIZDT_PB_Call_Start(uint16 white_idx)
{
    if(MMIZDT_CheckSimStatus())
    {
        uint8 i = 0;
        uint16 tempStr[101] ={0};
        if(white_idx == 0 || white_idx > YX_DB_WHITE_MAX_SUM || SCI_STRLEN(yx_DB_White_Reclist[white_idx-1].white_num) == 0)
        {
            MMIPUB_OpenAlertWarningWin(TXT_COMMON_EMPTY_NUM);
            return FALSE;
        }
        SCI_MEMSET(tempStr,0,(101)*sizeof(uint16));
        ZDT_UCS_Str16_to_uint16((uint8*)yx_DB_White_Reclist[white_idx-1].white_name, SCI_STRLEN(yx_DB_White_Reclist[white_idx-1].white_name) ,tempStr, 100);            
        
        MMIZDT_MakeAll(yx_DB_White_Reclist[white_idx-1].white_num,SCI_STRLEN(yx_DB_White_Reclist[white_idx-1].white_num),tempStr);
        return TRUE;
    }
    return FALSE;
}

void  MMIZDT_PB_DrawBg(void)
{
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    GUI_RECT_T   rect        = {0}; 
    rect.left   = 0;
    rect.top    = 0; 
    rect.right  = MMI_MAINSCREEN_WIDTH-1;
    rect.bottom = MMI_MAINSCREEN_HEIGHT-1;

    LCD_FillRect(&lcd_dev_info, rect, MMI_BLACK_COLOR);
    return;
}

void  MMIZDT_PB_DrawCur(MMI_WIN_ID_T win_id,uint16 index)
{
    uint16 pb_sum = 0;
    uint16 i = 0;
    uint32 image_id = IMAGE_ZDT_PB_FATHER;
    GUI_LCD_DEV_INFO    lcd_dev_info = {GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN};
    MMI_STRING_T    cur_str_t   = {0};
    GUI_RECT_T   cur_rect          = {0};
    GUI_RECT_T   rect        = {0}; 
    GUISTR_STYLE_T      text_style      = {0};
    GUI_POINT_T                 dis_point = {0,0};
    GUISTR_STATE_T      state =         GUISTR_STATE_ALIGN|GUISTR_STATE_WORDBREAK|GUISTR_STATE_SINGLE_LINE; 
#if defined(ZDT_MMI_USE_SMALL_FONT)      
    GUI_FONT_T f_big =SONG_FONT_22;
#else
    GUI_FONT_T f_big =SONG_FONT_28;
#endif
    uint16 TempStr[101] ={0};
    uint16 ba_wStr[2] = {0x7238,0};
    uint16 ma_wStr[2] = {0x5988,0};
    uint16 ye_wStr[2] = {0x7237,0};
    uint16 nai_wStr[2] = {0x5976,0};
    #if 1 // xiongkai    通讯录界面图标不一致
    uint16 fu_wStr[2] = {0x7236,0};//fu
    uint16 mu_wStr[2] = {0x6BCD,0};//mu qin
    uint16 shu_wStr[2] = {0x53D4,0};//shushu
    uint16 yi_wStr[2] = {0x59E8,0};//yi
    uint16 waigong_wStr[3] = {0x5916, 0x516C,0};//wai gong
    uint16 waipo_wStr[3] = {0x5916, 0x5A46,0};//wai po    
    #endif
    if(index > 0)
    {
        index--;
    }
     //if(index != m_yx_whitelist_pagenum)
     {
        MMIZDT_PB_DrawBg();
        
         for(i=(index)*4;i<(index)*4+4;i++)
         {
            if(yx_DB_White_Reclist[i].db_id == 0)
            {
                break;
            }
            pb_sum++;
            SCI_MEMSET(TempStr,0,(101)*sizeof(uint16));     
            ZDT_UCS_Str16_to_uint16((uint8*)yx_DB_White_Reclist[i].white_name, SCI_STRLEN(yx_DB_White_Reclist[i].white_name) ,TempStr, 100);
           
            if(MMIAPICOM_Wstrstr(TempStr,ba_wStr)||MMIAPICOM_Wstrstr(TempStr,fu_wStr))
            {
                image_id = IMAGE_ZDT_PB_FATHER;
            }
            else if(MMIAPICOM_Wstrstr(TempStr,ma_wStr)||MMIAPICOM_Wstrstr(TempStr,mu_wStr))
            {
                image_id = IMAGE_ZDT_PB_MOTHER;
            } 
            else if(MMIAPICOM_Wstrstr(TempStr,ye_wStr))
            {
                image_id = IMAGE_ZDT_PB_GRANDPA;
            }
            else if(MMIAPICOM_Wstrstr(TempStr,nai_wStr))
            {
                image_id = IMAGE_ZDT_PB_GRANDMA;
            }
        #ifndef ZTE_WATCH
            else if(MMIAPICOM_Wstrstr(TempStr,waigong_wStr))
            {
                image_id = IMAGE_ZDT_PB_WAIGONG;
            }
            else if(MMIAPICOM_Wstrstr(TempStr,waipo_wStr))
            {
                image_id = IMAGE_ZDT_PB_WAIPO;
            }         
            else if(MMIAPICOM_Wstrstr(TempStr,shu_wStr))
            {
                image_id = IMAGE_ZDT_PB_UNCLE;
            }
            else if(MMIAPICOM_Wstrstr(TempStr,yi_wStr))
            {
                image_id = IMAGE_ZDT_PB_AUNT;
            }      
            else
            {
                image_id = IMAGE_ZDT_PB_OTHERMAN;            
            }
        #else
            else
            {
                image_id = IMAGE_ZDT_PB_OTHER;            
            }
        #endif
            cur_rect.left = ZDT_PB_ICON_LEFT_START+ ((i%2)*ZDT_PB_ICON_WIDTH);
            cur_rect.right = cur_rect.left+ZDT_PB_ICON_WIDTH-1;
            if(i/2%2)
            {
                cur_rect.top = ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT+ZDT_PB_ICON_TOP_START;
            }
            else
            {
                cur_rect.top = ZDT_PB_ICON_TOP_START;
            }
            cur_rect.bottom = cur_rect.top+ZDT_PB_ICON_HEIGHT-1;
            dis_point.x = cur_rect.left;
            dis_point.y = cur_rect.top;
            GUIRES_DisplayImg(&dis_point,
                    &rect,
                    PNULL,
                    win_id,
                    image_id,
                    &lcd_dev_info);
            
            cur_rect.top = cur_rect.bottom+1;
            cur_rect.bottom = cur_rect.top + ZDT_PB_TEXT_HEIGHT-1;
            
            text_style.align = ALIGN_HVMIDDLE;
            text_style.font = f_big;
            text_style.font_color = MMI_WHITE_COLOR;
            #ifdef ZDT_PCBA_Z605_SUPPORT
            if(m_yx_whitelist_curindex == (i%4))
            {
                text_style.font_color = MMI_DARK_BLUE_COLOR;
            }
            #endif
            cur_str_t.wstr_ptr = TempStr;
            cur_str_t.wstr_len = MMIAPICOM_Wstrlen(TempStr);

            GUISTR_DrawTextToLCDInRect( 
                                    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                    (const GUI_RECT_T      *)&cur_rect,       
                                    (const GUI_RECT_T      *)&cur_rect,     
                                    (const MMI_STRING_T    *)&cur_str_t,
                                    &text_style,
                                    state,
                                    GUISTR_TEXT_DIR_AUTO
                                    ); 
     }
         
        if(pb_sum == 0)
        {
            cur_rect.left   = 0;
            cur_rect.top    = 0; 
            cur_rect.right  = MMI_MAINSCREEN_WIDTH-1;
            cur_rect.bottom = MMI_MAINSCREEN_HEIGHT-1;
            //空7A7A
            TempStr[0] = 0x7A7A;
            TempStr[1] = 0;
            
            text_style.align = ALIGN_HVMIDDLE;
            text_style.font = f_big;
            text_style.font_color = MMI_WHITE_COLOR;

            cur_str_t.wstr_ptr = TempStr;
            cur_str_t.wstr_len = MMIAPICOM_Wstrlen(TempStr);

            GUISTR_DrawTextToLCDInRect( 
                                    (const GUI_LCD_DEV_INFO *)&lcd_dev_info,
                                    (const GUI_RECT_T      *)&cur_rect,       
                                    (const GUI_RECT_T      *)&cur_rect,     
                                    (const MMI_STRING_T    *)&cur_str_t,
                                    &text_style,
                                    state,
                                    GUISTR_TEXT_DIR_AUTO
                                    ); 
        }
     }
     MMITHEME_UpdateRect();
    return;
}

void  MMIZDT_PB_ShowInit(void)
{
    uint16 i = 0;
    m_yx_whitelist_curindex = 0;
    m_yx_whitelist_totalnum=0;
    for(i=0;i<YX_DB_WHITE_MAX_SUM;i++)
    {
        if(yx_DB_White_Reclist[i].db_id)
        {
            m_yx_whitelist_totalnum ++;
        }
    }
    if(m_yx_whitelist_totalnum%4)
    {
        m_yx_whitelist_pagenum = m_yx_whitelist_totalnum/4 + 1;
    }
    else
    {
        m_yx_whitelist_pagenum = m_yx_whitelist_totalnum/4;
    }
    m_yx_whitelist_curpage= 1;
    return;
}

void  MMIZDT_PB_Show(MMI_WIN_ID_T win_id)
{
    MMIZDT_PB_DrawCur(win_id,m_yx_whitelist_curpage);
}

#ifdef BAIRUI_VIDEOCHAT_SUPPORT
extern VIDEO_USER_INFO video_users[MAX_VIDEO_USERS_COUNT];
PUBLIC void GetNameAndIndexByTalkUserId(int talk_user_id, uint16 *name_wstr, uint8 *index)
{
    int i = 0;
    for(i = 0 ; i < MAX_VIDEO_USERS_COUNT; i++)
    {
        if(talk_user_id == atoi(video_users[i].phone_user_id))
        {
            ZDT_PB_GetContactNameByNumber(video_users[i].phone_num, name_wstr);
            *index = i;
            break;
        }

    }
}
//yangyu end

PUBLIC MMI_IMAGE_ID_T MMIZDT_GetHeadImageId(int appUserId)
{
    uint16 ba_wStr[2] = {0x7238,0};//ba
    uint16 fu_wStr[2] = {0x7236,0};//fu
    uint16 ma_wStr[2] = {0x5988,0};//ma
    uint16 mu_wStr[2] = {0x6BCD,0};//mu qin
    uint16 ye_wStr[2] = {0x7237,0};//ye
    uint16 nai_wStr[2] = {0x5976,0};//nai
    uint16 shu_wStr[2] = {0x53D4,0};//shushu
    uint16 yi_wStr[2] = {0x59E8,0};//yi
    uint16 waigong_wStr[3] = {0x5916, 0x516C,0};//wai gong
    uint16 waipo_wStr[3] = {0x5916, 0x5A46,0};//wai po
    uint16 tempStr[101] ={0};
    MMI_IMAGE_ID_T image_id = IMAGE_NULL;
    uint8 i = 0;

    if(appUserId > 0)
    {
        //GetNameAndIndexByTalkUserId(appUserId, tempStr, &i);
        //cur_str_t.wstr_ptr = tempStr;
        //cur_str_t.wstr_len = (uint16) MMIAPICOM_Wstrlen (tempStr);
         //int i = 0;
        for(i = 0 ; i < MAX_VIDEO_USERS_COUNT; i++)
        {
            if(appUserId == atoi(video_users[i].phone_user_id))
            {
                //ZDT_PB_GetContactNameByNumber(video_users[i].phone_num, name_wstr);
                SCI_MEMCPY(tempStr, video_users[i].family_relative, 16*2);
                break;
            }

        }
        SCI_TRACE_LOW("MMIZDT_GetHeadImageId have name");
    }
    else
    {
        return IMAGE_NULL;
    }
    if(MMIAPICOM_Wstrstr(tempStr,ba_wStr)||MMIAPICOM_Wstrstr(tempStr,fu_wStr))
     {
            image_id = IMAGE_ZDT_PB_FATHER;
     }
    else if(MMIAPICOM_Wstrstr(tempStr,ma_wStr)||MMIAPICOM_Wstrstr(tempStr,mu_wStr))
     {
            image_id = IMAGE_ZDT_PB_MOTHER;
     } 
    else if(MMIAPICOM_Wstrstr(tempStr,ye_wStr))
     {
            image_id = IMAGE_ZDT_PB_GRANDPA;
     }
    else if(MMIAPICOM_Wstrstr(tempStr,nai_wStr))
     {
            image_id = IMAGE_ZDT_PB_GRANDMA;
     }
    else if(MMIAPICOM_Wstrstr(tempStr,waigong_wStr))
     {
            image_id = IMAGE_ZDT_PB_WAIGONG;
     }
    else if(MMIAPICOM_Wstrstr(tempStr,waipo_wStr))
     {
            image_id = IMAGE_ZDT_PB_WAIPO;
     }         
     else if(MMIAPICOM_Wstrstr(tempStr,shu_wStr))
     {
            image_id = IMAGE_ZDT_PB_UNCLE;
     }
     else if(MMIAPICOM_Wstrstr(tempStr,yi_wStr))
     {
            image_id = IMAGE_ZDT_PB_AUNT;
     }
    else
    {
        #if 1 // xiongkai      亲人号码前面图标固定用一个。
                  image_id = IMAGE_ZDT_PB_OTHERMAN;     
        #else
        if(i%2)
        {
                  image_id = IMAGE_ZDT_PB_OTHERWOMAN;
        }
        else
        {
                  image_id = IMAGE_ZDT_PB_OTHERMAN;
        }
        #endif
     }
    return image_id;

}
#endif

PUBLIC BOOLEAN MMIZDT_IsPhoneBookEmpty()
{
    BOOLEAN ret = TRUE;
    uint16 count = 0;
    int i = 0;
    for(i=0;i<YX_DB_WHITE_MAX_SUM;i++)
    {
       if(yx_DB_White_Reclist[i].db_id)
       {
           count++;
       }
    }
    if(count > 0)
    {
        ret = FALSE; 
    }
    return ret;

}

void  MMIZDT_PB_ShowList(MMI_WIN_ID_T win_id)
{
    BOOLEAN ret = FALSE;
    MMI_STRING_T    cur_str_t   = {0};
    uint16 tempStr[101] ={0};
    uint16 phoneNumberStr[30] ={0};
    uint16 i = 0, j = 0;
    MMI_IMAGE_ID_T image_id = IMAGE_NULL;
    uint16 position = 0;

    GUILIST_ITEM_T               item_t    =  {0};
    GUILIST_ITEM_DATA_T          item_data = {0};
    wchar buff[GUILIST_STRING_MAX_NUM + 1] = {0};
    //add by James li begin
    uint16 total_item_num = 0;
    MMI_STRING_T    empty_str = {0};
    GUILIST_EMPTY_INFO_T empty_info = {0};
    //add by James li end
    MMI_TEXT_ID_T tip_text_id;
    
#ifdef ZTE_WATCH
    item_t.item_style = GUIITEM_STYLE_1ICON_2STR_1LINE_LAYOUT;//GUIITEM_STYLE_1ICON_1STR_1LINE_LAYOUT1;
#else
    item_t.item_style = GUIITEM_STYLE_1ICON_2STR_1ICON;
#endif

    item_t.item_data_ptr = &item_data; 

    for(i=0;i<YX_DB_WHITE_MAX_SUM;i++)
    {
       if(yx_DB_White_Reclist[i].db_id)
       {
           m_yx_whitelist_totalnum++;
       }
    }
    GUILIST_RemoveAllItems(MMIZDT_PB_LIST_CTRL_ID);

    for(i = 0 ;i < m_yx_whitelist_totalnum ; i++)
    {
        if(yx_DB_White_Reclist[i].db_id == 0)
        {
            break;
        }
        SCI_MEMSET(tempStr,0,(101)*sizeof(uint16));     
        ZDT_UCS_Str16_to_uint16((uint8*)yx_DB_White_Reclist[i].white_name, SCI_STRLEN(yx_DB_White_Reclist[i].white_name) ,tempStr, 100);
        image_id = WATCHCOM_GetAvaterImageId(tempStr);      
        item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;        
        item_data.item_content[0].item_data.image_id = image_id;
        
        
        item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;      
        item_data.item_content[1].item_data.text_buffer.wstr_ptr = tempStr;
        item_data.item_content[1].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (tempStr);

        SCI_MEMSET(phoneNumberStr,0,sizeof(phoneNumberStr));
        GUI_UTF8ToWstr(phoneNumberStr,100,yx_DB_White_Reclist[i].white_num,strlen(yx_DB_White_Reclist[i].white_num));
        item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;    
        item_data.item_content[2].item_data.text_buffer.wstr_ptr = phoneNumberStr;
        item_data.item_content[2].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (phoneNumberStr);

        item_data.item_content[3].item_data_type = GUIITEM_DATA_IMAGE_ID;        
        item_data.item_content[3].item_data.image_id = IMAGE_PHONE_ICON;
        //MMIAPICOM_StrToWstr(content , item_data.item_content[0].item_data.text_buffer.wstr_ptr);// luan ma,Garbled
        //GUI_GBToWstr(item_data.item_content[0].item_data.text_buffer.wstr_ptr, (const uint8*)content, SCI_STRLEN(content));//luan ma,has chinese

        //GUI_GB2UCS(item_data.item_content[0].item_data.text_buffer.wstr_ptr, (const uint8*)content, SCI_STRLEN(content));//luan ma,has chinese
        //GUI_GBKToWstr(item_data.item_content[0].item_data.text_buffer.wstr_ptr, (const uint8*)content, SCI_STRLEN(content));//luan ma,has chinese
        //GUI_UTF8ToWstr(item_data.item_content[1].item_data.text_buffer.wstr_ptr, GUILIST_STRING_MAX_NUM + 1, content, SCI_STRLEN(content));

        
        ret = GUILIST_AppendItem (MMIZDT_PB_LIST_CTRL_ID, &item_t);
        if(ret)
        {
            CTRLLIST_SetItemUserData(MMIZDT_PB_LIST_CTRL_ID, position, &i); 
            #if 1//def NEW_LEFT_RIGHT_STYLE 
            //GUILIST_SetItemStyleEx(MMIZDT_PB_LIST_CTRL_ID, position, &new_style );
            #endif
            position++;   
        }
        
    }

    total_item_num = CTRLLIST_GetTotalItemNum(MMIZDT_PB_LIST_CTRL_ID);

    if (0 == total_item_num)
    {
        if(struct_yx_statues_data.bingd_statues != 0) //已经绑定但是服务器没有下发就请求一下
        {
            tip_text_id = TXT_LOADING_AND_RETRY; //加载中请稍后重试
            YX_Net_Send_PHLQ(&g_yx_app);
        }
        else
        {
            tip_text_id = STR_CONTA_NOTE_NO_CONTA_TEXT;
        }
        MMI_GetLabelTextByLang(STR_CONTA_NOTE_NO_CONTA_TEXT, &empty_str);
        empty_info.text_buffer = empty_str;
        CTRLLIST_SetEmptyInfo(MMIZDT_PB_LIST_CTRL_ID, &empty_info);
    }
    
}


void  MMIZDT_PB_TP_Move(MMI_WIN_ID_T win_id,uint8 dir)
{
    if(dir) 
    {
            if(m_yx_whitelist_curpage>1)
                m_yx_whitelist_curpage--;
    }
    else
    {
            if(m_yx_whitelist_curpage<m_yx_whitelist_pagenum)
                m_yx_whitelist_curpage++;
    }
    MMIZDT_PB_DrawCur(win_id,m_yx_whitelist_curpage);
}

LOCAL MMI_RESULT_E  HandleZDT_PBWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    uint8 * p_index = PNULL;
    p_index = (uint8 *) MMK_GetWinAddDataPtr(win_id);
    
    switch (msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            MMIZDT_PB_ShowInit();
        }
        break;
        
        case MSG_FULL_PAINT:
                MMIZDT_PB_Show(win_id);
            break;
            
        case MSG_GET_FOCUS:
                MMIZDT_PB_Show(win_id);
            break;
            
        case MSG_LOSE_FOCUS:
            break;
            
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            s_pb_tp_x = point.x;
            s_pb_tp_y = point.y;
            s_is_pb_tp_down = TRUE;
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            int offset_y =  0;//up to down
            int offset_x =  0;//up to down
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(s_is_pb_tp_down)
            {
                offset_y =  point.y - s_pb_tp_y;
                offset_x =  point.x - s_pb_tp_x;
                if(offset_x <= -(20))    
                {
                    MMIZDT_PB_TP_Move(win_id,0);
                }    
                else if(offset_x >= (20))
                {
                    MMIZDT_PB_TP_Move(win_id,1);
                }
                else
                {
                  if(point.y > ZDT_PB_ICON_TOP_START && point.y<(ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT))
                  {
                      if(point.x<120)
                      {
                          MMIZDT_PB_Call_Start(1+(m_yx_whitelist_curpage-1)*4);
                      }
                    else
                    {
                        MMIZDT_PB_Call_Start(2+(m_yx_whitelist_curpage-1)*4);
                    }
                  }
                  else if(point.y> (ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT+ZDT_PB_ICON_TOP_START) 
                      && point.y< (ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT+ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT))
                  {
                      if(point.x<120)
                      {
                          MMIZDT_PB_Call_Start(3+(m_yx_whitelist_curpage-1)*4);
                      }
                    else
                    {
                        MMIZDT_PB_Call_Start(4+(m_yx_whitelist_curpage-1)*4);
                    }          
                  }
                }
            }
            s_is_pb_tp_down = FALSE;
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(point.y > ZDT_PB_ICON_TOP_START && point.y<(ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT))
            {
                if(point.x<120)
                {
                    MMIZDT_PB_Call_Start(1+(m_yx_whitelist_curpage-1)*4);
                }
                else
                {
                    MMIZDT_PB_Call_Start(2+(m_yx_whitelist_curpage-1)*4);
                }
            }
            else if(point.y> (ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT+ZDT_PB_ICON_TOP_START) 
              && point.y< (ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT+ZDT_PB_ICON_TOP_START+ZDT_PB_ICON_HEIGHT+ZDT_PB_TEXT_HEIGHT))
            {
                if(point.x<120)
                {
                    MMIZDT_PB_Call_Start(3+(m_yx_whitelist_curpage-1)*4);
                }
                else
                {
                    MMIZDT_PB_Call_Start(4+(m_yx_whitelist_curpage-1)*4);
                }          
            }
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

  
		case MSG_KEYDOWN_RED:
			break;
        
    case MSG_KEYUP_RED:
            MMK_CloseWin(win_id);
        break;
        
    case MSG_KEYDOWN_WEB:
            MMIZDT_PB_Call_Start(4+(m_yx_whitelist_curpage-1)*4);
        break;
        
   case MSG_KEYDOWN_CANCEL:
            MMK_CloseWin(win_id);
        break;
        
    case MSG_KEYDOWN_DOWN:
            m_yx_whitelist_curindex = 0;
            MMIZDT_PB_TP_Move(win_id,1);
        break;
        
    case MSG_KEYDOWN_UP:
            m_yx_whitelist_curindex = 0;
            MMIZDT_PB_TP_Move(win_id,0);
        break;

    case MSG_KEYDOWN_LEFT:
            if(m_yx_whitelist_curindex > 0)
            {
                m_yx_whitelist_curindex--;
                MMIZDT_PB_DrawCur(win_id,m_yx_whitelist_curpage);
            }
            else
            {
                MMIZDT_PB_TP_Move(win_id,1);
            }
        break;

    case MSG_KEYDOWN_RIGHT:
            m_yx_whitelist_curindex++;
            if(m_yx_whitelist_curindex > 3)
            {
                m_yx_whitelist_curindex = 0;
                MMIZDT_PB_TP_Move(win_id,0);
            }
            else
            {
                MMIZDT_PB_DrawCur(win_id,m_yx_whitelist_curpage);
            }
        break;
        
    case MSG_CLOSE_WINDOW:
            if(p_index != PNULL)
            {
                SCI_FREE(p_index);
            }
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}

LOCAL MMI_RESULT_E  HandleZDT_NewPbWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T ctrl_id = MMIZDT_PB_LIST_CTRL_ID;
    BOOLEAN ret = FALSE;
    uint32 pos_user_data = 0; // position user data
    uint8 * p_index = PNULL;
    GUILIST_INIT_DATA_T list_init = {0}; 
    p_index = (uint8 *) MMK_GetWinAddDataPtr(win_id);
    
    
    switch (msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            GUI_RECT_T rect = {0,0,(MMI_MAINSCREEN_WIDTH -1),(MMI_MAINSCREEN_WIDTH -1)};
            list_init.both_rect.v_rect.left = 0;
            list_init.both_rect.v_rect.right = (MMI_MAINSCREEN_WIDTH -1);
            list_init.both_rect.v_rect.top = 0;
            list_init.both_rect.v_rect.bottom = (MMI_MAINSCREEN_HEIGHT-1);//240;//180;

            list_init.both_rect.h_rect.left = 0;
            list_init.both_rect.h_rect.right = (MMI_MAINSCREEN_HEIGHT -1);//240;//180;
            list_init.both_rect.h_rect.top = 0;
            list_init.both_rect.h_rect.bottom = (MMI_MAINSCREEN_WIDTH -1);
            
            list_init.type = GUILIST_TEXTLIST_E;
                        
            GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init); 
            //不画分割线
            GUILIST_SetListState( ctrl_id, GUILIST_STATE_SPLIT_LINE, FALSE );
            //不画高亮条
            GUILIST_SetListState( ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);
            GUILIST_SetListHighlightImage(ctrl_id, image_watch_list_highlight_bg);
            ret = GUILIST_SetMaxItem(ctrl_id, YX_DB_WHITE_MAX_SUM, FALSE);
            //GUILIST_SetRect(MMK_ConvertIdToHandle(ctrl_id), &rect);
            MMK_SetAtvCtrl(win_id,ctrl_id);
            //xiongkai del  这里导致进电话本会卡。MMIAPICC_DecMissedCallNumber(MMIAPICC_GetMissedCallNumber());//remove miss call status bar icon
            //MMIAPICOM_StatusAreaSetMissedCallState(FALSE); //remove miss call status bar icon
        }
        break;
        
        case MSG_FULL_PAINT:
                //MMIZDT_PB_Show(win_id);
                MMIZDT_PB_ShowList(win_id);
            break;
            
        case MSG_GET_FOCUS:
                //MMIZDT_PB_Show(win_id);
            break;
            
        case MSG_LOSE_FOCUS:
            break;
            
     case MSG_KEYUP_OK:
     case MSG_KEYUP_GREEN:
         {
            uint16 num_index = GUILIST_GetCurItemIndex(ctrl_id);
            GUILIST_GetItemData(ctrl_id, num_index, &pos_user_data);// user_data stand position
            MMIZDT_PB_Call_Start(pos_user_data+1);
         }
         break;
        case MSG_CTL_PENOK:
        case MSG_CTL_MIDSK:
        case MSG_NOTIFY_LIST_ITEM_CONTENT_CLICK:
        {
        //case MSG_KEYDOWN_WEB:
            uint16 index = GUILIST_GetCurItemIndex(ctrl_id);
            GUILIST_GetItemData(ctrl_id, index, &pos_user_data);// user_data stand position
            MMIZDT_PB_Call_Start(pos_user_data+1);
            //ZDT_LOG("HandleZDT_TinyChatWinMsg MSG_CTL_MIDSK  index=%d, pos_user_data=%d" , index, pos_user_data);
        }

        break;
            
#ifdef TOUCH_PANEL_SUPPORT
    case MSG_TP_PRESS_DOWN:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            s_pb_tp_x = point.x;
            s_pb_tp_y = point.y;
            s_is_pb_tp_down = TRUE;
            
        }
        break;
        
    case MSG_TP_PRESS_UP:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            if(point.x - s_pb_tp_x > 120 && z_abs(point.y - s_pb_tp_y) < 100)
            {
                MMK_SendMsg(win_id, MSG_APP_CANCEL, PNULL);
            }
        
        }
        break;
        
    case MSG_TP_PRESS_MOVE:
        {
            GUI_POINT_T   point = {0};
            point.x = MMK_GET_TP_X(param);
            point.y = MMK_GET_TP_Y(param);
            
        }
        break;
    case MSG_TP_PRESS_LONG:
        {
                
        }
        break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

    case MSG_KEYDOWN_RED:
        break;
        
    case MSG_KEYUP_RED:
            MMK_CloseWin(win_id);
        break;
        
    case MSG_KEYDOWN_WEB:
            //MMIZDT_PB_Call_Start(4+(m_yx_whitelist_curpage-1)*4);
        break;
        

        
    case MSG_KEYDOWN_DOWN:
            //m_yx_whitelist_curindex = 0;
            //MMIZDT_PB_TP_Move(win_id,1);
        break;
        
    case MSG_KEYDOWN_UP:
            //m_yx_whitelist_curindex = 0;
            //MMIZDT_PB_TP_Move(win_id,0);
        break;

    case MSG_KEYDOWN_LEFT:

        break;

    case MSG_KEYDOWN_RIGHT:

        break;
    case MSG_CTL_CANCEL:
    case MSG_APP_CANCEL:
        MMK_CloseWin(win_id);
        break;
    
    case MSG_CLOSE_WINDOW:
        if(p_index != PNULL)
        {
            SCI_FREE(p_index);
        }
        break;      

    default:
        recode = MMI_RESULT_FALSE;
        break;
    }
    
    return recode;
}


#if 0
WINDOW_TABLE( MMIZDT_WATCH_PB_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_PBWinMsg),    
    WIN_ID( MMIZDT_WATCH_PB_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};
#else
WINDOW_TABLE( MMIZDT_WATCH_PB_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_NewPbWinMsg),    
    WIN_ID( MMIZDT_WATCH_PB_WIN_ID ),
    //CREATE_LISTBOX_CTRL(GUILIST_TEXTLIST_E,MMIZDT_PB_LIST_CTRL_ID),
    WIN_HIDE_STATUS,
    END_WIN
};
#endif

PUBLIC void MMIZDT_OpenPBWin(void)
{
    if(MMIZDT_IsPhoneBookEmpty())
    {
        WatchCOM_NoteWin_1Line_Enter_Timeout(MMIZDT_WATCH_PB_NOTE_WIN_ID,STR_CONTA_NOTE_NO_CONTA_TEXT,PNULL,0);
    }
    else
    {
        MMK_CreateWin((uint32*)MMIZDT_WATCH_PB_WIN_TAB,PNULL);
    }
}

BOOLEAN MMIZDT_ClosePBWin(void)
{
    if(MMK_IsOpenWin(MMIZDT_WATCH_PB_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WATCH_PB_WIN_ID);
    }
    return TRUE;
}


#endif

#if 1  //天气窗口
extern YX_WEATHER_INFO_T g_yx_wt_info;

LOCAL MMI_RESULT_E  HandleZDT_WeatherWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            );



WINDOW_TABLE( MMIZDT_WEATHER_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_WeatherWinMsg),    
    WIN_ID( MMIZDT_WEATHER_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};


#define WEATHER_EXPIRE_MINUTES  30//120


LOCAL uint8 weather_timer_id = 0;
LOCAL uint16 weather_click_count = 0;
LOCAL uint16 wt_tp_down_x = 0;
LOCAL uint16 wt_tp_down_y = 0;
LOCAL SCI_TIME_T last_wt_time = {0};

LOCAL void MMIZDT_StartWeatherTimer()
{
    if(weather_timer_id != 0)
    {
        MMK_StopTimer(weather_timer_id);
    }
    weather_timer_id =  MMK_CreateWinTimer(MMIZDT_WEATHER_WIN_ID, 60*1000,TRUE);

}

LOCAL void MMIZDT_StopWeatherTimer()
{
    if(weather_timer_id != 0)
    {
        MMK_StopTimer(weather_timer_id);
    }
}



PUBLIC BOOLEAN MMIZDT_IsInWeatherWin()
{
    MMI_WIN_ID_T win_id = MMK_GetFocusWinId() ;

    return win_id == MMIZDT_WEATHER_WIN_ID;
}

PUBLIC BOOLEAN MMIZDT_IsInWeatherWinHandle(MMI_HANDLE_T win_handle)
{
    return MMK_GetWinId( win_handle) == MMIZDT_WEATHER_WIN_ID;
}


PUBLIC void MMIZDT_OpenWeatherWin()
{
   if(MMK_GetFocusWinId() != MMIZDT_WEATHER_WIN_ID)
   {
        MMK_CreateWin((uint32*)MMIZDT_WEATHER_WIN_TAB,PNULL);
   }
}

PUBLIC void MMK_CloseWeatherWin()
{
    if(MMK_IsOpenWin(MMIZDT_WEATHER_WIN_ID))
    {
        MMK_CloseWin(MMIZDT_WEATHER_WIN_ID);
    }
}

#ifdef WIN32
LOCAL void ZDT_InitWeather()
{
    uint16 shenzhen_city[3] = {0x6DF1, 0x5733 , 0};//shenzhen
    //uint16 shenzhen_city[3] = {0x4178, 0x5958 ,0};//shenzhen
    SCI_MEMCPY(g_yx_wt_info.city_name, shenzhen_city, 3*sizeof(uint16));
    g_yx_wt_info.min_degree = 20;
    g_yx_wt_info.max_degree = 30;
    g_yx_wt_info.cur_degree = 26;
    g_yx_wt_info.num = 1;
}
#endif

LOCAL BOOLEAN Is_WeatherExpired()
{
    BOOLEAN ret = TRUE;    
    uint8 year_str[5] = {0};
    uint8 mon_str[3] = {0};
    uint8 day_str[3] = {0};
    
    SCI_TIME_T time = {0};
    SCI_DATE_T date = {0};
    TM_GetSysTime(&time);
    TM_GetSysDate(&date);
    #if 0
    SCI_MEMCPY(year_str, g_yx_wt_info.date_str+0, 4);
    SCI_MEMCPY(mon_str, g_yx_wt_info.date_str+5, 2);
    SCI_MEMCPY(day_str, g_yx_wt_info.date_str+8, 2);
    
    if(atoi(year_str) == date.year 
        && atoi(mon_str) == date.mon
        && atoi(day_str) == date.mday
        )
    {
        ret = FALSE;
    }
    #endif
    if(time.hour < last_wt_time.hour //next day
        || (time.hour*60 + time.min) - (last_wt_time.hour*60 + last_wt_time.min) >= WEATHER_EXPIRE_MINUTES //2hour

    )
    {
        ret = TRUE;
        last_wt_time = time;

    }
    else
    {
        ret = FALSE;

    }
    ZDT_LOG("zdt__ Is_WeatherExpired, ret=%d", ret);
    return ret;
}


/*LOCAL*/ BOOLEAN Is_WeatherGetFromServer()
{
    BOOLEAN ret = TRUE;
    if((g_yx_wt_info.min_degree == 0 &&  g_yx_wt_info.max_degree == 0) || Is_WeatherExpired())
    {
        ret = FALSE;
    }
    ZDT_LOG("zdt__ Is_WeatherGetFromServer, ret=%d", ret);
    return ret;
}


LOCAL void DisplayWeather(MMI_WIN_ID_T win_id)
{
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    SCI_TIME_T time = {0};
    GUI_POINT_T point = {0};
    //add by James li begin
    MMI_STRING_T    display_string    = {0};
    wchar    wstr_buf[MMIPHONE_MAX_OPER_NAME_LEN]    = {0};
    //add by James li end
   
    MMI_IMAGE_ID_T weather_img_id = IMAGE_WEATHER_UNKNOWN;

    uint16 qing_wstr[2] = {0x6674, 0};//qing
    uint16 yin_wstr[2] = {0x9634, 0};//yin
    uint16 yu_wstr[2] = {0x96E8, 0};//yu
    uint16 xue_wstr[2] = {0x96EA, 0};//xue


    GUI_RECT_T    wt_rect=WEATHER_TEXT_RECT;
    GUI_RECT_T    temperature_rect=WEATHER_TEMPERATURE_RECT;
    GUI_RECT_T    update_time_rect=WEATHER_UPDATE_TIME_RECT;    
    GUI_RECT_T    city_rect=WEATHER_CITY_RECT;
    GUI_RECT_T    sim_rect=WEATHER_SIM_RECT;
    GUI_RECT_T    bg_rect = MMITHEME_GetFullScreenRect();


    GUI_RECT_T    text_rect={0};
    MMI_STRING_T tmp_string = {0};
    MMI_STRING_T        content_str = {0};
    MMI_STRING_T        update_time_str = {0};    
    wchar               content_text[100 + 1] = {0};
    wchar               content_text_1[100 + 1] = {0};    
    uint16 uint16_str_len = 0;
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    MMI_TEXT_ID_T weather_string_id;
    char*           temp_ptr = PNULL;
    uint16 temp_len = 0;

    uint8 temp_str[32] = {0};
    uint8 temp_wstr[32] = {0};
    
    TM_GetSysTime(&time);
    ZDT_LOG("zdt__ DisplayWeather, ");

    GUI_FillRect(&lcd_dev_info,bg_rect, MMI_BLACK_COLOR);
    
    LCD_DrawThickLine(&lcd_dev_info,0,40,MMI_MAINSCREEN_WIDTH,40,MMI_DARK_GRAY2_COLOR,1);
    LCD_DrawThickLine(&lcd_dev_info,0,160,MMI_MAINSCREEN_WIDTH,160,MMI_DARK_GRAY2_COLOR,1);

    point.x = 17;
    point.y = 8 ;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, IMAGE_WEATHER_LOCATION,&lcd_dev_info);

    //city
    tmp_string.wstr_ptr = g_yx_wt_info.city_name;
    tmp_string.wstr_len = (uint16) MMIAPICOM_Wstrlen (tmp_string.wstr_ptr);
    text_style.align = ALIGN_LEFT;
    text_style.font = SONG_FONT_24;
    text_style.font_color = MMI_WHITE_COLOR;
    GUISTR_DrawTextToLCDInRect( 
                MMITHEME_GetDefaultLcdDev(),
                (const GUI_RECT_T      *)&city_rect,       //the fixed display area
                (const GUI_RECT_T      *)&city_rect,       //用户要剪切的实际区域
                (const MMI_STRING_T    *)&tmp_string,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );    

    //weather icon  
    switch(g_yx_wt_info.num)
    {
        case 1:
            weather_img_id = IMAGE_BIG_WEATHER_SUNNY;
            weather_string_id=TXT_WEATHER_SUNNY;
            break;
        case 2:
            weather_img_id = IMAGE_BIG_WEATHER_RAIN;
            weather_string_id=TXT_WEATHER_RAIN;            
            break;
        case 3:
            weather_img_id = IMAGE_BIG_WEATHER_CLOUD;
            weather_string_id=TXT_WEATHER_CLOUD;                    
            break;
        case 4:
            weather_img_id = IMAGE_BIG_WEATHER_SNOW;
            weather_string_id=TXT_WEATHER_SNOW;                    
            break;
        default:
            weather_img_id = IMAGE_BIG_WEATHER_UNKNOW;
            weather_string_id=TXT_WEATHER_UNKNOW;                    
            break;                
    }
 
    point.x = WEATHER_ICON_X;
    point.y = WEATHER_ICON_Y;
    GUIRES_DisplayImg(&point,PNULL,PNULL,win_id, weather_img_id,&lcd_dev_info); 
    

    //weather text
    MMI_GetLabelTextByLang((MMI_TEXT_ID_T)weather_string_id,&tmp_string);
    text_style.align = ALIGN_LEFT;
    text_style.font_color = MMI_WHITE_COLOR;
    text_style.font = SONG_FONT_32;
    GUISTR_DrawTextToLCDInRect( 
                MMITHEME_GetDefaultLcdDev(),
                (const GUI_RECT_T      *)&wt_rect,       //the fixed display area
                (const GUI_RECT_T      *)&wt_rect,       //用户要剪切的实际区域
                (const MMI_STRING_T    *)&tmp_string,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );    
    
    //temperature
    memset(temp_str,0,sizeof(temp_str));
    sprintf((char*) temp_str, "%d", g_yx_wt_info.cur_degree);
    MMIAPICOM_StrcatFromStrToUCS2(content_text, &uint16_str_len, temp_str, strlen(temp_str));
    MMI_GetLabelTextByLang((MMI_TEXT_ID_T)TXT_WEATHER_TEMPERATURE_DU,&tmp_string);
    MMIAPICOM_StrcatFromSTRINGToUCS2(content_text, &uint16_str_len, &tmp_string);
    text_style.font = SONG_FONT_40;    
    text_style.align = ALIGN_LEFT;
    text_style.font_color = MMI_YELLOW_COLOR;
    content_str.wstr_ptr = content_text;
    content_str.wstr_len = MMIAPICOM_Wstrlen(content_text);
    GUISTR_DrawTextToLCDInRect( 
                MMITHEME_GetDefaultLcdDev(),
                (const GUI_RECT_T      *)&temperature_rect,       //the fixed display area
                (const GUI_RECT_T      *)&temperature_rect,       //用户要剪切的实际区域
                (const MMI_STRING_T    *)&content_str,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );    

    //update time
    text_style.font = SONG_FONT_28;
    text_style.font_color = MMI_WHITE_COLOR;    
    text_style.align=ALIGN_LEFT;         
    text_style.line_space = 0;
    text_style.region_num = 0;
    text_style.region_ptr = PNULL;        
        
    memset(temp_str,0,sizeof(temp_str));
    sprintf((char*) temp_str, "%02d:%02d", time.hour,time.min);

    temp_ptr = (char*)temp_str;
    temp_len = SCI_STRLEN((char*)temp_ptr);
    MMIAPICOM_StrcatFromStrToUCS2(content_text_1, &uint16_str_len, (uint8*)temp_ptr, temp_len);
    update_time_str.wstr_ptr = content_text_1;
    update_time_str.wstr_len = uint16_str_len;

    GUISTR_DrawTextToLCDInRect( 
                    MMITHEME_GetDefaultLcdDev(),
                    (const GUI_RECT_T      *)&update_time_rect,       //the fixed display area
                    (const GUI_RECT_T      *)&update_time_rect,       //用户要剪切的实际区域
                    (const MMI_STRING_T    *)&update_time_str,
                    &text_style,
                    state,
                    GUISTR_TEXT_DIR_AUTO
                    );    
        
    //add by James li begin
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font_color = MMI_GREEN_COLOR;
    text_style.font = SONG_FONT_24;

    display_string.wstr_ptr = wstr_buf;
    MMIAPIPHONE_GetSimAndServiceString(MN_DUAL_SYS_1, &display_string);

    GUISTR_DrawTextToLCDInRect( 
                MMITHEME_GetDefaultLcdDev(),
                (const GUI_RECT_T      *)&sim_rect,       //the fixed display area
                (const GUI_RECT_T      *)&sim_rect,       //ó??§òa???Dμ?êμ?ê??óò
                (const MMI_STRING_T    *)&display_string,
                &text_style,
                state,
                GUISTR_TEXT_DIR_AUTO
                );
    //add by James li end
}

PUBLIC void MMIZDT_UpdateWeatherWin()
{
    if(MMK_IsFocusWin(MMIZDT_WEATHER_WIN_ID)){
        MMK_SendMsg(MMIZDT_WEATHER_WIN_ID, MSG_FULL_PAINT, NULL);
    }
}

LOCAL MMI_RESULT_E  HandleZDT_WeatherWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_MENU_GROUP_ID_T         group_id        =   0;
    MMI_MENU_ID_T               menu_id         =   0;
    MMI_RESULT_E                recode          =   MMI_RESULT_TRUE;
    GUI_RECT_T      bg_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    MMI_STRING_T        str_data = {0};
    GUI_RECT_T  text_display_rect={0};
    GUISTR_STYLE_T      text_style = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;

    
    switch(msg_id) {
        case MSG_OPEN_WINDOW:
            weather_click_count = 0;

            //MMIZDT_StartWeatherTimer();
            #ifdef WIN32
            ZDT_InitWeather();
            #else
            if(!Is_WeatherGetFromServer())
            {
                YX_API_WT_Send();
            }
            #endif
            break;
        case MSG_GET_FOCUS:

            //MMIZDT_StartWeatherTimer();

            break;
        case MSG_FULL_PAINT:
        {
            DisplayWeather(win_id);                             
        }
            break;
            
#if 1//def TOUCH_PANEL_SUPPORT
        case MSG_TP_PRESS_DOWN:
            {
                GUI_POINT_T   point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                wt_tp_down_x = point.x;
                wt_tp_down_y = point.y;
                
            }
            break;
            
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T   point = {0};
                
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(wt_tp_down_y - point.y  > 90)
                {
                    MMK_SendMsg(MMK_ConvertIdToHandle(win_id), MSG_APP_CANCEL, PNULL);//exit to clock screen
                    break;
                }
                else if(ABS(wt_tp_down_x - point.x) > 40)
                {
                    break;
                }
            }
                
            break;
            
        case MSG_TP_PRESS_MOVE:
            {
                GUI_POINT_T   point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
            }
            break;
        case MSG_TP_PRESS_LONG:
            {
                GUI_POINT_T   point = {0};
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
            }
            break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527

        //case MSG_APP_OK:
        //case MSG_APP_WEB:
        case MSG_CTL_MIDSK:
            break;
        case MSG_TIMER:
        {
            if (*(uint8*)param == weather_timer_id)
            {
                MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
            }
        }
        
        break;
        case MSG_BACKLIGHT_TURN_ON:
        case MSG_BACKLIGHT_TURN_OFF:

        break;
        case MSG_LOSE_FOCUS:
        case MSG_CLOSE_WINDOW:
        { 
            //MMIZDT_StopClassModeTimer();

        }
        
        break;

        case MSG_KEYDOWN_RED:
            break;

        case MSG_KEYUP_RED:        //BUG 天气界面按红键去菜单了
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            break;

        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    return recode;

}
#endif


#ifdef ZTE_WATCH

#ifdef MAINLCD_DEV_SIZE_240X284
#define NO_SIM_POINT {77,56}
#define NO_DATA_POINT {90,65}
#define NO_SIM_TIP_RECT {39,150,201,172}
#define NO_DATA_POINT {77,67}
#define NO_DATA_TIP_RECT {39,150,201,172}
#define NO_SIM_DATA_QUIT_POINT {12,191}
#define NO_SIM_DATA_QUIT_TEXT_RECT {12,191,228,231}
#else
#define NO_SIM_POINT {77,26}
#define NO_DATA_POINT {90,26}
#define NO_SIM_TIP_RECT {39,122,201,145}
#define NO_DATA_POINT {77,37}
#define NO_DATA_TIP_RECT {39,122,201,145}
#define NO_SIM_DATA_QUIT_POINT {12,161}
#define NO_SIM_DATA_QUIT_TEXT_RECT {12,161,228,201}
#endif

LOCAL void Draw_No_Sim(MMI_WIN_ID_T win_id,GUI_LCD_DEV_INFO lcd_dev_info)
{
    GUISTR_STYLE_T text_style = {0};
    MMI_STRING_T text_str = {0};
    GUI_POINT_T img_point;
    GUI_POINT_T no_sim_point = NO_SIM_POINT;
    GUI_POINT_T no_data_point = NO_DATA_POINT;
    GUI_RECT_T no_sim_text_rect = NO_SIM_TIP_RECT;
    GUI_POINT_T quit_point = NO_SIM_DATA_QUIT_POINT;
    GUI_RECT_T quit_text_rect = NO_SIM_DATA_QUIT_TEXT_RECT;
    MMI_IMAGE_ID_T img_id;
    MMI_TEXT_ID_T text_id;
    if(ZDT_SIM_Exsit())
    {
        img_id = IMAGE_NO_DATA_ICON;
        text_id = TXT_NO_DATA;
        img_point = no_data_point;
    }
    else
    {
        img_id = IMAGE_NO_SIM_ICON;
        text_id = TXT_NO_SIM;
        img_point = no_sim_point;
    }
    GUIRES_DisplayImg(&img_point,PNULL,PNULL,win_id,img_id,&lcd_dev_info);
    MMI_GetLabelTextByLang(text_id,&text_str);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_26;
    text_style.font_color = MMI_WHITE_COLOR;
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&no_sim_text_rect,&no_sim_text_rect,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO); 
    GUIRES_DisplayImg(&quit_point,PNULL,PNULL,win_id,IMAGE_COMMON_ONEBTN_KBBG_IMAG,&lcd_dev_info);
    MMI_GetLabelTextByLang(STXT_EXIT,&text_str);
    text_style.align = ALIGN_HVMIDDLE;
    text_style.font = SONG_FONT_26;
    text_style.font_color = MMI_WHITE_COLOR;
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info,&quit_text_rect,&quit_text_rect,&text_str,&text_style,GUISTR_STATE_ALIGN,GUISTR_TEXT_DIR_AUTO); 
    
}

LOCAL MMI_RESULT_E  HandleZDT_NoSimOrDataWinMsg(MMI_WIN_ID_T win_id, MMI_MESSAGE_ID_E msg_id, DPARAM param)
{
    MMI_RESULT_E recode = MMI_RESULT_TRUE;
    GUI_RECT_T full_rect = MMITHEME_GetFullScreenRectEx(win_id);
    GUI_LCD_DEV_INFO  lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
    GUISTR_STYLE_T      text_style = {0};
    MMI_STRING_T        str_data = {0};
    GUISTR_STATE_T      state = GUISTR_STATE_ALIGN|GUISTR_STATE_SINGLE_LINE| GUISTR_STATE_ELLIPSIS;
    GUI_RECT_T tip_str_rect = {0};
    switch(msg_id) 
    {
        case MSG_OPEN_WINDOW:
            break;
        case MSG_FULL_PAINT:
            LCD_FillRect(&lcd_dev_info, full_rect, MMI_BLACK_COLOR); 
            Draw_No_Sim(win_id,lcd_dev_info);
            break;
        case MSG_TP_PRESS_UP:
            {
                GUI_POINT_T   point = {0};
                GUI_RECT_T quit_rect = NO_SIM_DATA_QUIT_TEXT_RECT;
                point.x = MMK_GET_TP_X(param);
                point.y = MMK_GET_TP_Y(param);
                if(GUI_PointIsInRect(point,quit_rect))
                {
                    MMK_CloseWin(win_id);
                }
            }              
            break;

        case MSG_KEYDOWN_RED:
            break;
        
        case MSG_CLOSE_WINDOW:
        case MSG_KEYUP_RED:       
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
            MMK_CloseWin(win_id);
            break;
        default:
            recode = MMI_RESULT_FALSE;
        break;
    }
    return recode;
}

WINDOW_TABLE( MMIZDT_NoSimOrData_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZDT_NoSimOrDataWinMsg),    
    WIN_ID( MMIZDT_NO_SIM_OR_DATA_WIN_ID ),
    WIN_HIDE_STATUS,
    END_WIN
};

//没有SIM卡或者无网络
PUBLIC void MMIZDT_OpenNoSimOrDataWin()
{
   if(MMK_GetFocusWinId() != MMIZDT_NO_SIM_OR_DATA_WIN_ID)
   {
        MMK_CreateWin((uint32*)MMIZDT_NoSimOrData_WIN_TAB,PNULL);
   }
}

//检测SIM卡没插SIM则弹出未插SIM卡窗口 只检测SIM卡状态不检测数据连接状态
PUBLIC BOOLEAN MMIZDT_CheckSimStatus()
{
   if(ZDT_SIM_Exsit())
   {
       return TRUE;
   }
   else
   {
       if(MMK_GetFocusWinId() != MMIZDT_NO_SIM_OR_DATA_WIN_ID)
       {
           MMK_CreateWin((uint32*)MMIZDT_NoSimOrData_WIN_TAB,PNULL);
       }
       return FALSE;
   }

}
#endif

#if 1//add_schedule
static int s_scd_tp_x;
static int s_scd_tp_y;
static BOOLEAN s_is_scd_tp_down = FALSE;
static int scdwin_is_wating = 0;

LOCAL THEMELIST_ITEM_STYLE_T title_style = {0};
LOCAL THEMELIST_ITEM_STYLE_T content_style = {0};
LOCAL THEMELIST_ITEM_STYLE_T none_style = {0};
void MMIZDT_Schedule_ShowList(MMI_WIN_ID_T win_id,uint8 current_day)
{
    BOOLEAN ret = FALSE;

    GUILIST_ITEM_T       		item_t    =  {0};
    GUILIST_ITEM_DATA_T  		item_data = {0};

	MMI_CTRL_ID_T ctrl_id = MMIZDT_SCHEDULE_CTRL_ID;

	uint16 position = 0;

    const uint16 height = 40;//65;

    GUI_RECT_T title0_rect = { 90,  10,  150 , height};//text
    GUI_RECT_T title1_rect = { 150,  10,  160 , height};//text

    GUI_RECT_T content0_rect = { 35,  10,  130 , height};// 上午 12:30
    GUI_RECT_T content1_rect = { 5,  10,  30 , height};// 1
    GUI_RECT_T content2_rect = { 130,  10,  230 , height};// 语文

    GUI_RECT_T none0_rect = { 60,  10,  180 , 200};//text
    GUI_RECT_T none1_rect = { 180,  10,  185 , 200};//text

    SCI_DATE_T date = {0};
    MMI_STRING_T                week_string            = {0};
	wchar				content_text[100 + 1] = {0};
	uint16 uint16_str_len = 0;

	uint8 temp_str[64] = {0};
	MMI_STRING_T tmp_string = {0};
    uint16 class_time = 0;
	wchar				class_time_text[640 + 1] = {0};
	uint16 class_time_len = 0;

	wchar						temp_wstr[GUILIST_STRING_MAX_NUM + 1] = {0};
	wchar						num_wstr[GUILIST_STRING_MAX_NUM + 1] = {0};

	wchar						temp0_wstr[GUILIST_STRING_MAX_NUM + 1] = {0};
	wchar						num0_wstr[GUILIST_STRING_MAX_NUM + 1] = {0};

	uint16 tempStr[101] ={0};

    uint16 am_wstr[10] = {0x4e0a, 0x5348, 0}; //上午\u4e0a\u5348
    uint16 pm_wstr[10] = {0x4e0b, 0x5348, 0}; //下午\u4e0b\u5348  

    uint16 none_wstr[10] = {0x7a7a, 0}; //空

    uint16 alert_wstr[10] = {0x672a, 0x8bbe,0x7f6e, 0x8bfe,0x7a0b, 0xff01, 0}; //未设置课程！ \u672a\u8bbe\u7f6e\u8bfe\u7a0b\uff01

	int i = 0;
	int j = 0;
	
    MMI_TEXT_ID_T week_table[] =
    {
        STXT_MONDAY,//       TXT_SHORT_IDLE_MONDAY,
        STXT_TUESDAY,//      TXT_SHORT_IDLE_TUESDAY,
        STXT_WEDNESDAY,//       TXT_SHORT_IDLE_WEDNESDAY,
        STXT_THURSDAY,//       TXT_SHORT_IDLE_THURSDAY,
        STXT_FRIDAY,//       TXT_SHORT_IDLE_FRIDAY,
        STXT_SATURDAY,//       TXT_SHORT_IDLE_SATURDAY
        STXT_SUNDAY,//       TXT_SHORT_IDLE_SUNDAY,
    };


	THEMELIST_ITEM_STYLE_T* pItemStyle_title = THEMELIST_GetStyle(GUIITEM_STYLE_ONE_LINE_SMALL_TEXT_AND_TEXT);
	THEMELIST_ITEM_STYLE_T* pItemStyle_content = THEMELIST_GetStyle(GUIITEM_STYLE_ONE_LINE_SMALL_TEXT_AND_TEXT);
	THEMELIST_ITEM_STYLE_T* pItemStyle_none = THEMELIST_GetStyle(GUIITEM_STYLE_ONE_LINE_SMALL_TEXT_AND_TEXT);
	SCI_MEMCPY(&title_style, pItemStyle_title, sizeof(*pItemStyle_title));
	SCI_MEMCPY(&content_style, pItemStyle_content, sizeof(*pItemStyle_content));
	SCI_MEMCPY(&none_style, pItemStyle_none, sizeof(*pItemStyle_none));
    
    MMI_GetLabelTextByLang(week_table[current_day-1], &week_string);
	MMIAPICOM_StrcatFromSTRINGToUCS2(content_text, &uint16_str_len, &week_string);
	

	title_style.height = title_style.height_focus = 40;
	title_style.content[0].rect = title_style.content[0].rect_focus = title0_rect;
	title_style.content[0].font= title_style.content[0].font_focus = SONG_FONT_20;
	title_style.content[0].state = GUIITEM_CONTENT_STATE_TEXT_M_ALIGN;
	title_style.content[1].rect = title_style.content[1].rect_focus = title1_rect;
	title_style.content[1].font= title_style.content[1].font_focus = SONG_FONT_20;

	content_style.height = content_style.height_focus = 40;
	content_style.content[0].rect = content_style.content[0].rect_focus = content0_rect;
	content_style.content[0].font= content_style.content[0].font_focus = SONG_FONT_20;
	content_style.content[0].state = 0;
	content_style.content[1].rect = content_style.content[1].rect_focus = content1_rect;
	content_style.content[1].font= content_style.content[1].font_focus = SONG_FONT_20;
	content_style.content[1].state = GUIITEM_CONTENT_STATE_TEXT_R_ALIGN;
	content_style.content[2].rect = content_style.content[2].rect_focus = content2_rect;
	content_style.content[2].font= content_style.content[2].font_focus = SONG_FONT_20;
	content_style.content[2].state = GUIITEM_CONTENT_STATE_TEXT_M_ALIGN;

	none_style.height = none_style.height_focus = 200;
	none_style.content[0].rect = none_style.content[0].rect_focus = none0_rect;
	none_style.content[0].font= none_style.content[0].font_focus = SONG_FONT_20;
	none_style.content[0].state = GUIITEM_CONTENT_STATE_TEXT_M_ALIGN;
	none_style.content[1].rect = none_style.content[1].rect_focus = none1_rect;
	none_style.content[1].font= none_style.content[1].font_focus = SONG_FONT_20;

	item_t.item_data_ptr = &item_data;
	item_t.item_style =  GUIITEM_STYLE_CUSTOMIZE;
	item_t.item_state |= GUIITEM_STATE_SELFADAPT_RECT | GUIITEM_STATE_CONTENT_CHECK|GUIITEM_STATE_NEED_ITEM_CONTENT;
	GUILIST_RemoveAllItems(ctrl_id);
	item_t.item_style =  GUIITEM_STYLE_ONE_LINE_TEXT;

	item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;		
	item_data.item_content[0].item_data.text_buffer.wstr_ptr = content_text;
	item_data.item_content[0].item_data.text_buffer.wstr_len = uint16_str_len;
	item_data.item_content[0].is_custom_font_color = TRUE;
	item_data.item_content[0].custom_font_color = MMI_WHITE_COLOR;//RGB888TO565(0XB2B2B2);//GUI_RGB2RGB565(171,142,112); 	
	item_data.item_content[0].custom_font_color_focus = MMI_WHITE_COLOR;//RGB888TO565(0XB2B2B2);//GUI_RGB2RGB565(171,142,112); 

	item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
	item_data.item_content[1].item_data.text_buffer.wstr_ptr = 0;
	item_data.item_content[1].item_data.text_buffer.wstr_len = 0; 


	ret = GUILIST_AppendItem (ctrl_id, &item_t);
	if(ret)
	{
		//CTRLLIST_SetItemUserData(ctrl_id, position, &list_all_index); 
		GUILIST_SetItemStyleEx(ctrl_id, position, &title_style );
		position++;   
	}

if(YX_DB_is_have_Schedule(current_day-1))	
{
	int item_num =0;
	//memset(temp_str,0,sizeof(temp_str));
	for( i = 0; i < SCHEDULE_TIME_MUN_MAX;i++)	
	{
		//SCI_TRACE_LOW("XX MMIZDT_Schedule_ShowList have  class_content_Type [%d][%d]=%s",current_day-1,i, yx_schedule_Rec.class_content_Type[current_day-1][i].class_content_wstr);
		if(SCI_STRLEN(yx_schedule_Rec.class_content_Type[current_day-1][i].class_content_str) == 0)
		{
			item_num ++;
			continue ;
		}	

        SCI_MEMSET (temp0_wstr, 0, sizeof (temp0_wstr));
        SCI_MEMSET (num0_wstr, 0, sizeof (num0_wstr));
		sprintf ( (char*) temp0_wstr, "%s", yx_schedule_Rec.schedule_time[i]);
		MMIAPICOM_StrToWstr (temp0_wstr, num0_wstr);	

		item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
		item_data.item_content[0].item_data.text_buffer.wstr_ptr = num0_wstr;
		item_data.item_content[0].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (num0_wstr);
		
		/*if (yx_schedule_Rec.schedule_time_end[i] < 12)
		{
			item_data.item_content[0].item_data.text_buffer.wstr_ptr = am_wstr;
			item_data.item_content[0].item_data.text_buffer.wstr_len = MMIAPICOM_Wstrlen(am_wstr);
		}else{
			item_data.item_content[0].item_data.text_buffer.wstr_ptr = pm_wstr;
			item_data.item_content[0].item_data.text_buffer.wstr_len = MMIAPICOM_Wstrlen(pm_wstr);
		}*/
		item_data.item_content[0].is_custom_font_color = TRUE;
		item_data.item_content[0].custom_font_color = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112); 0;//		
		item_data.item_content[0].custom_font_color_focus = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);			

        SCI_MEMSET (temp_wstr, 0, sizeof (temp_wstr));
        SCI_MEMSET (num_wstr, 0, sizeof (num_wstr));
		sprintf ( (char*) temp_wstr, "%d.", ++j);
		MMIAPICOM_StrToWstr (temp_wstr, num_wstr);		
		item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;		
		item_data.item_content[1].item_data.text_buffer.wstr_ptr = num_wstr;
		item_data.item_content[1].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (num_wstr);
		item_data.item_content[1].is_custom_font_color = TRUE;
		item_data.item_content[1].custom_font_color = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);		
		item_data.item_content[1].custom_font_color_focus = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);			

        SCI_MEMSET(tempStr,0,(101)*sizeof(uint16));	
		if(SCI_STRLEN(yx_schedule_Rec.class_content_Type[current_day-1][i].class_content_str) == 0)
		{
			item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;		
			item_data.item_content[2].item_data.text_buffer.wstr_ptr = none_wstr;
			item_data.item_content[2].item_data.text_buffer.wstr_len = MMIAPICOM_Wstrlen (none_wstr);
		}else{
			ZDT_UCS_Str16_to_uint16((uint8*)yx_schedule_Rec.class_content_Type[current_day-1][i].class_content_str, SCI_STRLEN(yx_schedule_Rec.class_content_Type[current_day-1][i].class_content_str) ,tempStr, 100);
			item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;		
			item_data.item_content[2].item_data.text_buffer.wstr_ptr = tempStr;
			item_data.item_content[2].item_data.text_buffer.wstr_len = (uint16) MMIAPICOM_Wstrlen (tempStr);
		}
		item_data.item_content[2].is_custom_font_color = TRUE;
		item_data.item_content[2].custom_font_color = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);		
		item_data.item_content[2].custom_font_color_focus = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);			
		
		ret = GUILIST_AppendItem (ctrl_id, &item_t);
		if(ret)
		{
			//CTRLLIST_SetItemUserData(ctrl_id, position, &i); 
			GUILIST_SetItemStyleEx(ctrl_id, position, &content_style );
			position++;   
		}


	}

    for( i = 0; i < item_num;i++)	
    {
    	//SCI_TRACE_LOW("XX MMIZDT_Schedule_ShowList no_have class_content_Type [%d][%d]=%s",current_day-1,i, yx_schedule_Rec.class_content_Type[current_day-1][i].class_content_wstr);

    	item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
    	item_data.item_content[0].item_data.text_buffer.wstr_ptr = 0;
    	item_data.item_content[0].item_data.text_buffer.wstr_len = 0;
    	item_data.item_content[0].is_custom_font_color = TRUE;
    	item_data.item_content[0].custom_font_color = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112); 0;//		
    	item_data.item_content[0].custom_font_color_focus = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);			
	
    	item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;		
    	item_data.item_content[1].item_data.text_buffer.wstr_ptr = 0;
    	item_data.item_content[1].item_data.text_buffer.wstr_len = 0;
    	item_data.item_content[1].is_custom_font_color = TRUE;
    	item_data.item_content[1].custom_font_color = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);		
    	item_data.item_content[1].custom_font_color_focus = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);			
    

		item_data.item_content[2].item_data_type = GUIITEM_DATA_TEXT_BUFFER;		
		item_data.item_content[2].item_data.text_buffer.wstr_ptr = 0;
		item_data.item_content[2].item_data.text_buffer.wstr_len = 0;
    	item_data.item_content[2].is_custom_font_color = TRUE;
    	item_data.item_content[2].custom_font_color = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);		
    	item_data.item_content[2].custom_font_color_focus = MMI_WHITE_COLOR;//GUI_RGB2RGB565(171,142,112);			
    	
    	ret = GUILIST_AppendItem (ctrl_id, &item_t);
    	if(ret)
    	{
    		//CTRLLIST_SetItemUserData(ctrl_id, position, &i); 
    		GUILIST_SetItemStyleEx(ctrl_id, position, &content_style );
    		position++;   
    	}
    
    }



}else{

		item_data.item_content[0].item_data_type = GUIITEM_DATA_TEXT_BUFFER;		
		item_data.item_content[0].item_data.text_buffer.wstr_ptr = alert_wstr;
		item_data.item_content[0].item_data.text_buffer.wstr_len = MMIAPICOM_Wstrlen (alert_wstr);
		item_data.item_content[0].is_custom_font_color = TRUE;
		item_data.item_content[0].custom_font_color = MMI_WHITE_COLOR;//RGB888TO565(0XB2B2B2);//GUI_RGB2RGB565(171,142,112);	
		item_data.item_content[0].custom_font_color_focus = MMI_WHITE_COLOR;//RGB888TO565(0XB2B2B2);//GUI_RGB2RGB565(171,142,112); 

		item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
		item_data.item_content[1].item_data.text_buffer.wstr_ptr = 0;
		item_data.item_content[1].item_data.text_buffer.wstr_len = 0; 


		ret = GUILIST_AppendItem (ctrl_id, &item_t);
		if(ret)
		{
			//CTRLLIST_SetItemUserData(ctrl_id, position, &list_all_index); 
			GUILIST_SetItemStyleEx(ctrl_id, position, &none_style );
			position++;   
		}


}

}
LOCAL uint8 day_index = 0;
LOCAL BOOLEAN   s_is_cancel_msg = FALSE;
LOCAL BOOLEAN   s_is_ok_msg = FALSE;
LOCAL int   s_msg_num = 0;

LOCAL MMI_RESULT_E  HandleZDT_WatchScheduleWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
	GUI_POINT_T 				point				= {0};
	MMI_RESULT_E			recode	= MMI_RESULT_TRUE;
	MMI_CTRL_ID_T ctrl_id = MMIZDT_SCHEDULE_CTRL_ID;
	BOOLEAN ret = FALSE;
	uint32 pos_user_data = 0; // position user data 
	uint8 * p_index = PNULL;
	GUILIST_INIT_DATA_T list_init = {0}; 
    SCI_DATE_T date = {0};
		
	p_index = (uint8 *) MMK_GetWinAddDataPtr(win_id);

	
	SCI_TRACE_LOW("XX HandleZDT_WatchScheduleWinMsg msg_id=0x%x", msg_id);
	switch (msg_id)
	{
		case MSG_OPEN_WINDOW:
		{
			
			GUI_RECT_T listrect = {0,0,(MMI_MAINSCREEN_WIDTH -1),(MMI_MAINSCREEN_HEIGHT-1)};

			list_init.both_rect.v_rect.left = 0;
			list_init.both_rect.v_rect.right = (MMI_MAINSCREEN_WIDTH -1);
			list_init.both_rect.v_rect.top = 0;
			list_init.both_rect.v_rect.bottom = (MMI_MAINSCREEN_HEIGHT -1) ;//240;//180;

			list_init.both_rect.h_rect.left = 0;
			list_init.both_rect.h_rect.right = (MMI_MAINSCREEN_HEIGHT -1);//240;//180;
			list_init.both_rect.h_rect.top = 0;
			list_init.both_rect.h_rect.bottom = (MMI_MAINSCREEN_WIDTH -1);
			
			list_init.type = GUILIST_TEXTLIST_E;
			GUILIST_CreateListBox(win_id, 0, ctrl_id, &list_init);	
			ret = GUILIST_SetMaxItem(ctrl_id, SCHEDULE_TIME_MUN_MAX+2, FALSE);
			GUILIST_SetRect(MMK_ConvertIdToHandle(ctrl_id), &listrect);
			//GUILIST_SetNeedSplidLine(ctrl_id,TRUE);

			GUILIST_SetNeedSplidLine(ctrl_id,FALSE);//去掉分割线(有可能会切屏)
			GUILIST_SetListState(ctrl_id, GUILIST_STATE_NEED_HIGHTBAR, FALSE);//去掉高亮特效			
			MMK_SetAtvCtrl(win_id,ctrl_id);
			
			TM_GetSysDate(&date);
			day_index = date.wday;
		}
		break;
		
		case MSG_FULL_PAINT:
			{
				SCI_TRACE_LOW("XX HandleZDT_WatchScheduleWinMsg MSG_FULL_PAINT day_index=%d ", day_index);
				MMIZDT_Schedule_ShowList(win_id,day_index);
				s_msg_num = 0;
			}
			break;
			
		case MSG_GET_FOCUS:
			break;
			
		case MSG_LOSE_FOCUS:
			break;
		case MSG_CTL_MIDSK:
		{
			//uint16 index = GUILIST_GetCurItemIndex(ctrl_id);
			//GUILIST_GetItemData(ctrl_id, index, &pos_user_data);// user_data stand position
			//MMIZDT_PB_Call_Start(pos_user_data+1);
		}

		break;
		case MSG_NOTIFY_LIST_SLIDE_STATE_CHANGE:
		{
			GUILIST_POS_INFO_T pos_info = {0};
			
			CTRLLIST_GetCurPosInfo(ctrl_id, &pos_info);

			SCI_TRACE_LOW("XX HandleZDT_WatchScheduleWinMsg MSG_NOTIFY_LIST_SLIDE_STATE_CHANGE s_msg_num=%d", s_msg_num);
			if(s_msg_num == 1)
			{
			
				day_index = day_index+1;
				if(day_index == 8)
				{
					day_index = 1;
				}
				MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
			}else if(s_msg_num == 2)
			{
				day_index = day_index-1;
				if(day_index == 0)
				{
					day_index = 7;
				}		
				MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
			}
			s_msg_num = 0;
		}
		break;
#ifdef TOUCH_PANEL_SUPPORT
	case MSG_TP_PRESS_DOWN:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			s_scd_tp_x = point.x;
			s_scd_tp_y = point.y; 
		}
		break;
		
	case MSG_TP_PRESS_UP:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			if(( z_abs(s_scd_tp_y - point.y) < 15 )&&(point.x - s_scd_tp_x > 15 ))//右滑
			 {
					day_index = day_index-1;
					if(day_index == 0)
					{
						day_index = 7;
					}		
					MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
			 }
			
			if(( z_abs(s_scd_tp_y - point.y) < 15 )&&(s_scd_tp_x - point.x > 15 ))//左滑
			{
				day_index = day_index+1;
				if(day_index == 8)
				{
					day_index = 1;
				}
				MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
			}

		}
		break;
	case MSG_CTL_PENOK: //左滑
	case MSG_CTL_CANCEL: //右滑
		 {	
				s_msg_num++;
				SCI_TRACE_LOW("XX HandleZDT_WatchScheduleWinMsg MSG_CTL s_msg_num=%d ", s_msg_num);

			}
		break;

		
/*	case MSG_CTL_PENOK: //左滑
	 {	
			s_is_ok_msg = TRUE;
			SCI_TRACE_LOW("XX HandleZDT_WatchScheduleWinMsg MSG_CTL_PENOK s_is_cancel_msg=%d ", s_is_cancel_msg);
			if(!s_is_cancel_msg)
			{

				day_index = day_index+1;
				if(day_index == 8)
				{
					day_index = 1;
				}
				MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
			}
			s_is_cancel_msg=FALSE;
		}	
		break;	
	case MSG_CTL_CANCEL: //右滑
		{	
			s_is_cancel_msg = TRUE;
			SCI_TRACE_LOW("XX HandleZDT_WatchScheduleWinMsg MSG_CTL_CANCEL s_is_ok_msg=%d ", s_is_ok_msg);
			if(!s_is_ok_msg)
			{
				day_index = day_index-1;
				if(day_index == 0)
				{
					day_index = 7;
				}		
				MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
			}
			s_is_ok_msg=FALSE;
		}
		break;*/		
	case MSG_TP_PRESS_MOVE:
		{
			GUI_POINT_T   point = {0};
			point.x = MMK_GET_TP_X(param);
			point.y = MMK_GET_TP_Y(param);
			
		}
		break;
	case MSG_TP_PRESS_LONG:
		{
				
		}
		break;
#endif //TOUCH_PANEL_SUPPORT //IGNORE9527
	case MSG_KEYDOWN_RED:
		break;
		
	case MSG_KEYUP_RED:
		MMK_CloseWin(win_id);
		break;
		
		
	case MSG_APP_CANCEL:
	case MSG_KEYUP_CANCEL:	//下键 
			MMK_CloseWin(win_id);
		break;
	
	case MSG_CLOSE_WINDOW:
		if(p_index != PNULL)
		{
			SCI_FREE(p_index);
		}
		break;		

	default:
		recode = MMI_RESULT_FALSE;
		break;
	}
	
	return recode;
}


WINDOW_TABLE( MMIZDT_WATCH_SCHEDULE_WIN_TAB ) = {
	WIN_FUNC( (uint32)HandleZDT_WatchScheduleWinMsg),
	WIN_ID(MMIZDT_WATCH_SCHEDULE_WIN_ID),
	WIN_HIDE_STATUS,
	END_WIN
};


PUBLIC void MMIZDT_OpenScheduleWin(void)
{

	if(ZDT_SIM_Exsit() == FALSE)
	{
		MMIPUB_OpenAlertWinByTextId(PNULL,STR_SIM_NOT_SIM_EXT01,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
		return;
	}
	if (MMIAPICONNECTION_isGPRSSwitchedOff())
	{
		MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_NEED_NET,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
		return;
	}
	if(yx_DB_Set_Rec.net_open == 0)
	{
		MMIPUB_OpenAlertWinByTextId(PNULL,TXT_YX_WCHAT_NEED_OPEN,TXT_NULL,IMAGE_PUBWIN_FAIL,PNULL,PNULL,MMIPUB_SOFTKEY_ONE,PNULL);
		return;
	}


	MMK_CreateWin((uint32 *)MMIZDT_WATCH_SCHEDULE_WIN_TAB,PNULL);
}


#endif

#endif

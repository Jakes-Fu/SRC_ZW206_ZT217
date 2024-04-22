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
#ifndef _MMIZYB_WINTAB_C_
#define _MMIZYB_WINTAB_C_

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

#include "zyb_app.h"
#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "zyb_stream_player.h"

#define  ZYB_TEST_MENU_LEN  20
#define  ZYB_TEST_MENU_NUM  6
const uint8 zyb_test_menu_text[ZYB_TEST_MENU_NUM][ZYB_TEST_MENU_LEN]= 
{
    "Play",
    "Stop",
    "Pause",
    "resume",
    "vol up",
    "vol down"

};

LOCAL MMI_RESULT_E HandleZYB_TestMainMenuWinMsg (
                                      MMI_WIN_ID_T   win_id, 	// 窗口的ID
                                      MMI_MESSAGE_ID_E 	msg_id, 	// 窗口的内部消息ID
                                      DPARAM            param		// 相应消息的参数
                                      );

WINDOW_TABLE( MMIZYB_TEST_MENU_WIN_TAB ) = 
{
    WIN_FUNC((uint32) HandleZYB_TestMainMenuWinMsg),    
    WIN_ID( MMIZYB_TEST_MENU_WIN_ID ),
    WIN_TITLE(TXT_ZYB_TEST),
    //WIN_STATUSBAR,
    WIN_SOFTKEY(TXT_COMMON_OK, TXT_NULL, STXT_RETURN),
    CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMIZYB_TEST_MENU_CTRL_ID),
    END_WIN
};

BOOLEAN MMIZYB_OpenTestMenuWin(void)
{
    MMK_CreateWin((uint32*)MMIZYB_TEST_MENU_WIN_TAB,PNULL);
}

LOCAL void MMIAPISET_CommonTextId(                                    
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
/*****************************************************************************/
// 	Description : to handle the message of atcmd test 
//	Global resource dependence : 
//  Author:jianshengqi
//	Note: 2006/3/19 
/*****************************************************************************/
//extern uint32 test_main_lcd_id[20];
//extern uint32 test_sub_lcd_id[20];

LOCAL MMI_RESULT_E  HandleZYB_TestMainMenuWinMsg(
                                            MMI_WIN_ID_T        win_id, 
                                            MMI_MESSAGE_ID_E    msg_id, 
                                            DPARAM                param
                                            )
{
    MMI_RESULT_E            recode  = MMI_RESULT_TRUE;
    MMI_CTRL_ID_T            ctrl_id = MMIZYB_TEST_MENU_CTRL_ID;
    uint16                    node_id = 0;
    static BOOLEAN            s_need_update   = FALSE;
  //  MMISET_VALIDATE_PRIVACY_RETURN_T    result_info = {MMISET_PROTECT_CL, FALSE};    
#if defined(DISABLE_STATUSBAR_DISPLAY) 
    GUI_RECT_T                          title_rect =   {0, 0, 239, 23};
#else
    GUI_RECT_T                          title_rect = {0, 18, 239, 18 + 23};
#endif
    switch (msg_id)
    {
    case MSG_OPEN_WINDOW:
        {
            uint16 i;
            GUILIST_ITEM_T      item_t = {0};
            GUILIST_ITEM_DATA_T item_data = {0};
            MMI_STRING_T		    item_str = {0};
            uint8				copy_len = 0;
            wchar buff[GUILIST_STRING_MAX_NUM + 1] = {0};

            
            if (MMK_IsIncludeStatusBar(win_id))
            {
                GUIWIN_SetTitleRect(win_id, title_rect);   
            }  
            
            GUILIST_SetMaxItem(ctrl_id, ZYB_TEST_MENU_NUM,FALSE);   
            
            for ( i = 0; i < ZYB_TEST_MENU_NUM; i++ )
            {
                item_t.item_style    = GUIITEM_STYLE_ONE_LINE_ICON_TEXT;
                item_t.item_data_ptr = &item_data;
                copy_len = SCI_STRLEN(zyb_test_menu_text[i]);
                item_data.item_content[0].item_data_type     = GUIITEM_DATA_IMAGE_ID;
                item_data.item_content[0].item_data.image_id = IMAGE_SIM1;
                item_data.item_content[1].item_data_type    = GUIITEM_DATA_TEXT_BUFFER;
                item_data.item_content[1].item_data.text_buffer.wstr_ptr = buff;
                GUI_GB2UCS(item_data.item_content[1].item_data.text_buffer.wstr_ptr, zyb_test_menu_text[i],copy_len );
                item_data.item_content[1].item_data.text_buffer.wstr_len = MIN( GUILIST_STRING_MAX_NUM, copy_len);
                                   
                item_data.softkey_id[0] = TXT_COMMON_OK;
                item_data.softkey_id[1] = TXT_NULL;
                item_data.softkey_id[2] = STXT_RETURN;
                
                GUILIST_AppendItem(ctrl_id, &item_t);      
            }
            
            MMK_SetAtvCtrl(win_id, ctrl_id);
        }

        break;
        
    case MSG_CTL_PENOK:
    case MSG_CTL_MIDSK:
    case MSG_APP_WEB:
    case MSG_CTL_OK:
    case MSG_APP_OK:
    {
        node_id = GUILIST_GetCurItemIndex(  ctrl_id );
        node_id++;
        switch( node_id )
        {
            case  1:
                    MMIZYB_HTTP_TestLongPlay();
                break;
                
            case  2:
                    ZYB_StreamPlayer_Stop();
                break;
                
            case  3:
                    ZYB_StreamPlayer_Pause();
                break;
                
            case  4:
                    ZYB_StreamPlayer_Resume();
                break;
                                
            case  5:
                    ZYB_StreamPlayer_VolumeUp();
                break;
                
            case  6:
                    ZYB_StreamPlayer_VolumeDown();
                break;
            default:
                break;
        }
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


#endif


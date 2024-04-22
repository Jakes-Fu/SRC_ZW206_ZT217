// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_dict_activity.c
 * Auth: Wanglusong (wanglusong01@baidu.com)
 * Desc: duerapp xiaodu dictionary
 */

#include "std_header.h"
#include "window_parse.h"
#include "mmk_app.h"
#include "ctrlsetlist_export.h"
#include "dal_time.h"
#include "guilcd.h"
#include "guifont.h"
#include "guilistbox.h"
#include "guibutton.h"
#include "guitext.h"
#include "guilabel.h"
#include "guirichtext.h"
#include "guisetlist.h"
#include "gui_ucs2b_converter.h"
#include "mmipub.h"
#include "mmi_textfun.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "mmi_event_api.h"
#include "mmi_applet_table.h"
#include "mmidisplay_data.h"
// #include "mmiwifi_export.h"
#include "mmiphone_export.h"
#include "mmicom_time.h"
#include "watch_common_btn.h"
#include "watch_common_list.h"
#include "watch_commonwin_export.h"

#include "lightduer_log.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"

#include "duerapp.h"
#include "duerapp_id.h"
#include "duerapp_text.h"
#include "duerapp_image.h"
#include "duerapp_anim.h"
#include "duerapp_nv.h"
#include "duerapp_main.h"
#include "duerapp_openapi.h"
#include "duerapp_common.h"
#include "duerapp_http.h"
#include "duerapp_statistics.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_res_down.h"
#include "duerapp_main.h"
#include "duerapp_homepage.h"
#include "duerapp_dict_activity.h"
#include "os_api.h"

#include "lightduer_common.h"
/**************************************************************************************************************************
 *                                                        STATIC VARIABLES                                                
 **************************************************************************************************************************/
LOCAL dictionary_info_t s_dict_attr = {0};
LOCAL dictionary_tmp_info_t s_tmp_info = {0};
LOCAL duerapp_dict_candidate_t *pDictCandInfo = NULL;
LOCAL duerapp_dict_card_t *pDictInfo = NULL;
LOCAL BLOCK_ID s_dict_thread_id = 0;
LOCAL msg_dict_candidate_free_cb dict_candidate_free_cb= NULL;
LOCAL msg_dict_delete_cb dict_delete_cb= NULL;

/**************************************************************************************************************************
 *                                                        STATIC FUNCTIONS                                                
 *************************************************************************************************************************/
LOCAL void PRV_DUERAPP_DictionaryGroupWordsWinOpen(void);
LOCAL void PRV_DUERAPP_DictionarySingleWordWinOpen(void);
LOCAL void PRV_DUERAPP_DictionaryMultipleWordsWinOpen(void);
LOCAL BOOLEAN duerapp_dict_is_allowed_use(void);
LOCAL MMIDUERAPP_start_dict_judge(MMI_WIN_ID_T win_id);

LOCAL void PRV_DUERAPP_WinDrawBG(MMI_WIN_ID_T win_id, GUI_COLOR_T color)
{
    GUI_RECT_T win_rect = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    GUI_RECT_T clientRect = DUERAPP_FULL_SCREEN_RECT;

    MMK_GetWinRect(win_id, &win_rect);
    MMK_GetWinLcdDevInfo(win_id, &lcd_dev_info);
    win_rect = MMI_ConvertWinRect(MMICOM_WINPOS_WIN2DISP, win_id, clientRect);
    GUI_FillRect(&lcd_dev_info, win_rect, MMI_BLACK_COLOR);
}

LOCAL void PRV_DUERAPP_Toast(wchar *txt)
{
    WATCH_SOFTKEY_TEXT_ID_T softkey={COMMON_TXT_NULL, COMMON_TXT_NULL, COMMON_TXT_NULL};
    MMI_STRING_T tipSting = {0};
    tipSting.wstr_ptr = txt;
    tipSting.wstr_len = MMIAPICOM_Wstrlen(txt);
    WatchCOM_NoteWin_2Line_Enter(MMI_DUERAPP_QUERY_WIN_ID, &tipSting, 0, softkey, PNULL);
}

LOCAL int PRV_DUERAPP_Strlen_Utf8(char *instr)
{
    int len = 0;
    if (!instr) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return 0;
    }

    while (*instr) {
        if (!(*instr & 0x80)) {
            // 00 - 7F
            instr += 1;
            len += 1;
        } else if ((*instr & 0xC0) == 0x80) {
            // suffix
            instr += 1;
        } else if ((*instr & 0xE0) == 0xC0) {
            // 80 - 7FF
            instr += 2;
            len += 1;
        } else if ((*instr & 0xF0) == 0xE0) {
            // 800 - FFFF
            instr += 3;
            len += 1;
        } else if ((*instr & 0xF8) == 0xF0) {
            // 1000 - 1FFFFF
            instr += 4;
            len += 1;
        } else {

        }
    }
    return len;
}

LOCAL char* PRV_DUERAPP_Get_Utf8_Char(char *instr, char *outstr)
{
    if (!instr || !outstr) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return NULL;
    }

    if (!(*instr & 0x80)) {
        // 00 - 7F
        *outstr++ = *instr++;
    } else if ((*instr & 0xE0) == 0xC0) {
        // 80 - 7FF
        *outstr++ = *instr++;
        *outstr++ = *instr++;
    } else if ((*instr & 0xF0) == 0xE0) {
        // 800 - FFFF
        *outstr++ = *instr++;
        *outstr++ = *instr++;
        *outstr++ = *instr++;
    } else if ((*instr & 0xF8) == 0xF0) {
        // 1000 - 1FFFFF
        *outstr++ = *instr++;
        *outstr++ = *instr++;
        *outstr++ = *instr++;
        *outstr++ = *instr++;
    } else {

    }

    *outstr = '\0';
    return instr;
}

LOCAL void PRV_DUERAPP_Tmp_Info_Reset(void)
{
    memset(s_tmp_info.out_str, 0, 64);
    memset(s_tmp_info.out_wstr, 0, 64);
    memset(s_tmp_info.outdetail_str, 0, 2048);
    memset(s_tmp_info.outdetail_wstr, 0, 2048);
}

LOCAL void PRV_DUERAPP_Form_Create(MMI_HANDLE_T win_handle, MMI_HANDLE_T form_handle, MMI_HANDLE_T child_handle, int index)
{
    GUIFORM_INIT_DATA_T childform_init_data = {0};
    GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    
    childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
    childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
    childform_form_child_ctrl.child_handle = child_handle + index;
    childform_form_child_ctrl.init_data_ptr = &childform_init_data;
    
    GUIFORM_CreatDynaChildCtrl(win_handle, form_handle, &childform_form_child_ctrl);
    GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
}

LOCAL void PRV_DUERAPP_Form_Set_Style(MMI_HANDLE_T form_handle, MMI_HANDLE_T child_handle, int index)
{
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    MMI_HANDLE_T child_form_ctrl_id = child_handle + index;
    GUIFORM_CHILD_WIDTH_T child_form_width = {(235), GUIFORM_CHILD_WIDTH_FIXED};
    uint16 hor_space = 1;//ˮƽ���?
    uint16 ver_space = 1;//��ֱ���?
    
    CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
    CTRLFORM_SetChildWidth(form_handle, child_form_ctrl_id, &child_form_width);
    CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_LEFT);
    CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
    CTRLFORM_PermitChildFont(child_form_ctrl_id, TRUE);
    CTRLFORM_PermitChildBorder(child_form_ctrl_id, TRUE);
    CTRLFORM_SetSpace(child_form_ctrl_id, &hor_space, &ver_space);
}

PUBLIC void PUB_DUERAPP_DictionaryCardWinOpen(dictionary_card_type_t type, dictionary_mmk_msg_info_t *ctx)
{
    switch (type) 
    {
        case DUERAPP_DICT_RENDER_DICTSEL:
        {
            pDictCandInfo = (duerapp_dict_candidate_t *)ctx->userdata;
            dict_candidate_free_cb = ctx->dict_candidate_free_cb;
            if (pDictCandInfo) {
                PRV_DUERAPP_DictionaryGroupWordsWinOpen();
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __func__, __LINE__);
            }
            break;
        }
        
        case DUERAPP_DICT_RENDER_NORMAL:
        {
            pDictInfo = (duerapp_dict_card_t *)ctx->userdata;
            dict_delete_cb = ctx->dict_delete_cb;
            if (pDictInfo) {
                SCI_TraceLow("(%s)(%d)[duer_watch]:word'number(%d)", __func__, __LINE__, pDictInfo->n_words);
                if (pDictInfo->n_words > 1) {//����
                    PRV_DUERAPP_DictionaryMultipleWordsWinOpen();
                } else {
                    PRV_DUERAPP_DictionarySingleWordWinOpen();
                }
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __func__, __LINE__);
            }
            break;
        }
        
        default:
        {
            break;
        }
    }
}

#if defined(DUERAPP_DICTIONARY_VOICE_SUPPORT)//�ʵ�������

LOCAL void PRV_DUERAPP_Dictionary_Thread_Enter(uint32 argc, void *argv)
{
    dictionary_thread_msg_info_t *sig_ptr = PNULL;
    
    while (1) 
    {
        sig_ptr = (dictionary_thread_msg_info_t *)SCI_GetSignal(s_dict_thread_id);
        
        if (!sig_ptr) {
            SCI_TraceLow("(%s)(%d)[duer_watch]:get msg, but error", __func__, __LINE__);
            continue;
        }
        SCI_TraceLow("(%s)(%d)[duer_watch]:msg'type(%d)", __func__, __LINE__, sig_ptr->SignalCode);
        
        switch (sig_ptr->SignalCode) 
        {
            case DUERAPP_DICT_RENDER_DICTSEL:
            {
                break;
            }
            
            case DUERAPP_DICT_RENDER_NORMAL:
            {
                break;
            }
            
            case DUERAPP_DICT_RENDER_EMPTY:
            {
                break;
            }
            
            default:
            {
                break;
            }
        }
        DUER_FREE(sig_ptr);
    }
}

LOCAL void PRV_DUERAPP_Dictionary_Thread_Create(void)
{
    if (s_dict_thread_id) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:task have exist", __func__, __LINE__);
        return;
    }
    s_dict_thread_id = SCI_CreateThread("dict_thread",
                                        "dict_thread_queue",
                                        PRV_DUERAPP_Dictionary_Thread_Enter,
                                        0,
                                        NULL,
                                        2048*8,
                                        50,
                                        SCI_PRIORITY_NORMAL,
                                        // 20,
                                        SCI_PREEMPT, 
                                        SCI_AUTO_START);
    if (SCI_INVALID_BLOCK_ID == s_dict_thread_id) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:create task failed", __func__, __LINE__);
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:create task finish", __func__, __LINE__);
    }
}

LOCAL void PRV_DUERAPP_Dictionary_Param_Reset(dictionary_card_type_t type)
{
    if (DUERAPP_DICT_RENDER_DICTSEL == type) {
        if (pDictCandInfo) {
            dict_candidate_free_cb(pDictCandInfo);
            pDictCandInfo = NULL;
            dict_candidate_free_cb = NULL;
        }
    } else if (DUERAPP_DICT_RENDER_NORMAL == type) {
        if (pDictInfo) {
            dict_delete_cb(pDictInfo);
            pDictInfo = NULL;
            dict_delete_cb = NULL;
        }
    }
}

PUBLIC uint32 PUB_DUERAPP_Dictionary_Thread_Id_Get(void)
{
    return (uint32)s_dict_thread_id;
}

PUBLIC void PUB_DUERAPP_Dictionary_Thread_SendMsg(dictionary_thread_msg_info_t *msg)
{
    /*
        ������
        dictionary_thread_msg_info_t *sig_ptr = SCI_NULL;
        sig_ptr = (dictionary_thread_msg_info_t *)DUER_CALLOC(1, sizeof(dictionary_thread_msg_info_t));
        if (sig_ptr) {
            sig_ptr->SignalSize = sizeof(dictionary_thread_msg_info_t);
            sig_ptr->SignalCode = MSG_DUERAPP_DICT_RENDER_NORMAL;
            sig_ptr->Sender     = PUB_DUERAPP_Dictionary_Thread_Id_Get();
            sig_ptr->dict_delete_cb = duerapp_dict_delete;//�ص����ͷ�
            sig_ptr->userdata = (void *)card;
            PUB_DUERAPP_Dictionary_Thread_SendMsg(sig_ptr);
        }
     */
    if (s_dict_thread_id == NULL
        || msg == NULL) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:null task", __func__, __LINE__);
        return;
    }
    
    if (SCI_SUCCESS != SCI_SendSignal((xSignalHeader *)msg, s_dict_thread_id)) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:send msg failed", __func__, __LINE__);
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:send msg ok", __func__, __LINE__);
    }
}

PUBLIC void PUB_DUERAPP_DictionaryMainWinOpen(void)
{
    MMIDUERAPP_StartDictionaryAsrSet();
    duerapp_dict_init();
    MMIDUERAPP_CreateDuerMainWin(FALSE);
    PRV_DUERAPP_Dictionary_Thread_Create();
}

#endif

#if defined(DUERAPP_DICTIONARY_GROUPWORDS_SUPPORT)
LOCAL MMI_RESULT_E  PRV_DUERAPP_DictionaryGroupWordsMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         );

LOCAL void PRV_DUERAPP_DictionaryGroupWordsUiDraw(void *userdata);

LOCAL void PRV_DUERAPP_DictionaryFormGroupWordsCtrlDestory(BOOLEAN is_create);

WINDOW_TABLE(MMI_DUERAPP_DICTIONARY_GROUP_WORDS_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_DICTIONARY_GROUP_WORDS_WIN_ID),
    WIN_FUNC((uint32)PRV_DUERAPP_DictionaryGroupWordsMsg),
    
    END_WIN
};

LOCAL MMI_RESULT_E  PRV_DUERAPP_DictionaryGroupWordsMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
    duerapp_dict_card_t *pTmpInfo = (duerapp_dict_card_t *)param;
    
    SCI_TraceLow("(%s)(%d)[duer_watch]:win_id(0x%0lx), msg_id(0x%0lx)", __func__, __LINE__, win_id, msg_id);
    
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            PRV_DUERAPP_WinDrawBG(win_id, MMI_BLACK_COLOR);
            break;
        }
        case MSG_FULL_PAINT:
        {
            if (pDictCandInfo) {
                SCI_TraceLow("(%s)(%d)[duer_watch]:Gword'number(%d), Draw", __func__, __LINE__, pDictCandInfo->num);
                PRV_DUERAPP_DictionaryGroupWordsUiDraw((void *)pDictCandInfo);
                PRV_DUERAPP_Dictionary_Param_Reset(DUERAPP_DICT_RENDER_DICTSEL);
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:null", __func__, __LINE__);
            }

            MMIDUERAPP_start_dict_judge(win_id);

            break;
        }
        case MSG_CTL_OK:
        case MSG_CTL_MIDSK:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CLOSE_WINDOW:
        {
            PRV_DUERAPP_Dictionary_Param_Reset(DUERAPP_DICT_RENDER_DICTSEL);
            PRV_DUERAPP_DictionaryFormGroupWordsCtrlDestory(FALSE);
            break;
        }
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    pTmpInfo = NULL;

    return recode;
}

LOCAL MMI_RESULT_E PRV_DUERAPP_DictionaryFormGroupWordsImgButtonCb(MMI_HANDLE_T ctrl_handle)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    int index = -1;
    MMI_STRING_T show_str = {0};
    MMI_HANDLE_T ctrl_id = MMK_GetCtrlId(ctrl_handle);
    
    switch (ctrl_id)
    {
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON1_CTRL_ID :
        {
            index = 0;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON2_CTRL_ID :
        {
            index = 1;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON3_CTRL_ID :
        {
            index = 2;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON4_CTRL_ID :
        {
            index = 3;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON5_CTRL_ID :
        {
            index = 4;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON6_CTRL_ID :
        {
            index = 5;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON7_CTRL_ID :
        {
            index = 6;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON8_CTRL_ID :
        {
            index = 7;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON9_CTRL_ID :
        {
            index = 8;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON10_CTRL_ID :
        {
            index = 9;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON11_CTRL_ID :
        {
            index = 10;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON12_CTRL_ID :
        {
            index = 11;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON13_CTRL_ID :
        {
            index = 12;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON14_CTRL_ID :
        {
            index = 13;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON15_CTRL_ID :
        {
            index = 14;
            break;
        }
        
        default:
        {
            break;
        }
    }
    SCI_TraceLow("(%s)(%d)[duer_watch]:button cb, index(%d)", __func__, __LINE__, index);
    
    if (index >= 0) {
        if (strlen(s_dict_attr.group_word_info.words[index])) {
            int ret = duerapp_dict_query(s_dict_attr.group_word_info.words[index]);
            SCI_TraceLow("(%s)(%d)[duer_watch]:ret(%d), pinyin(%s)", __func__, __LINE__, ret, s_dict_attr.group_word_info.words[index]);
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
        }
    }
    
    return result;
}

LOCAL void PRV_DUERAPP_DictionaryFormGroupWordsCtrlDestory(BOOLEAN is_create)
{
    static BOOLEAN is_have_create = FALSE;
    BOOLEAN state = FALSE;
    int i = 0;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_LABEL_CTRL_ID);
    
    for(i = 0; i < DUERAPP_DICTIONARY_GROUPWORDS_MAX; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON1_CTRL_ID + i);
    }
    
    for(i = 0; i < 2; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID + i);
    }
    
    for(i = 0; i < 2; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_CTRL_ID + i);
    }
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM_CTRL_ID);
}

LOCAL MMI_HANDLE_T PRV_DUERAPP_DictionaryFormGroupWordsCtrlCreate(MMI_HANDLE_T win_handle, void *userdata)
{
    MMI_HANDLE_T ctrl_handle = 0;
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM_CTRL_ID;
    duerapp_dict_candidate_t *pTmpInfo = (duerapp_dict_candidate_t *)userdata;
    int utf8_len = 0;
    int i = 0;

    GUIFORM_INIT_DATA_T childform_init_data = {0};
    GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};

    GUILABEL_INIT_DATA_T title_init_data = {0};
    GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
    
    MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_CTRL_ID;

    // GUIFORM_INIT_DATA_T childform_init_data = {0};
    // GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    
    GUIBUTTON_INIT_DATA_T word_buttom_init_data = {0};
    GUIFORM_DYNA_CHILD_T word_buttom_form_child_ctrl = {0};
    
    if (pTmpInfo == NULL) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return 0;
    }
    
    //0�ͷ�
    PRV_DUERAPP_DictionaryFormGroupWordsCtrlDestory(TRUE);

    //1��̬����form
    {
        GUIFORM_CreatDynaCtrl(win_handle, ctrl_id, GUIFORM_LAYOUT_ORDER);
    }

    //2��̬����form����form
    {       
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        //TITLE��FORM
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        //BUTTON��FORM
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        GUIFORM_CreatDynaChildCtrl(win_handle, ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
    }

    //3��̬����form����form1�Ŀؼ�
    {        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        //��̬����form����form1�Ŀؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
    }

    //4��̬����form����form2����form
    {        
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_CTRL_ID;
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        
        for (i = 0; i < 2; i++) {
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
    }

    //5��̬����form����form2����form�Ŀؼ�
    {       
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID;
        
        word_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON1_CTRL_ID;
        word_buttom_form_child_ctrl.init_data_ptr = &word_buttom_init_data;
        word_buttom_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        
        //��̬����form�ӿؼ�BUTTON
        for (i = 0; i < pTmpInfo->num; i++) {
            i%2?(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM2_CTRL_ID):(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID);
            word_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &word_buttom_form_child_ctrl);
        }
    }

    ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
    
    return ctrl_handle;
}

LOCAL void PRV_DUERAPP_DictionaryGroupWordsUiDraw(void *userdata)
{
    MMI_HANDLE_T form_ctrl_handle = 0;
    int utf8_len = 0;
    int i = 0;
    duerapp_dict_candidate_t *pTmpInfo = (duerapp_dict_candidate_t *)userdata;
    GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_BLACK_COLOR, FALSE};
    GUI_RECT_T form_rect = {5,5,235,MMI_MAINSCREEN_HEIGHT};
    IGUICTRL_T *IGuiCtrl = MMK_GetCtrlPtr(form_ctrl_handle);

    // GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_WHITE_COLOR, FALSE};
    MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_CTRL_ID;
    GUIFORM_CHILD_WIDTH_T width = {235, GUIFORM_CHILD_WIDTH_FIXED};
    GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    uint16 hor_space = 0;//ˮƽ���?
    uint16 ver_space = 0;//��ֱ���?

    //��ʼ��LABEL�ؼ��Ĳ���
    MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_LABEL_CTRL_ID;
    GUI_COLOR_T label_font_color = MMI_BLACK_COLOR;
    GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
    MMI_STRING_T label_wshowtxt = {0};

    // GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
    // MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID;
    
    //��ʼ��BUTTON�ؼ��Ĳ���
    MMI_HANDLE_T button_ctrl_id = 0;
    GUI_BG_T button_bg_attr ={GUI_BG_COLOR, GUI_SHAPE_RECT, 0, RGB8882RGB565(0xCCCCCC), FALSE};
    GUI_FONT_ALL_T font_info = {DUERAPP_DICTIONARY_CONTENT_FONT, MMI_BLACK_COLOR};
    MMI_STRING_T button_text = {0};
    
    // MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID;
    
    // GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
    // GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
    GUI_PADDING_T padding = {0};
    // uint16 hor_space = 0;//ˮƽ���?
    // uint16 ver_space = 5;//��ֱ���?

    if (pTmpInfo == NULL) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    
    if (pTmpInfo->num == 0
        || pTmpInfo->num > DUERAPP_DICTIONARY_GROUPWORDS_MAX) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Error, Gword'cnt(%d)", __FUNCTION__, __LINE__, pTmpInfo->num);
        return;
    }
    
    form_ctrl_handle = PRV_DUERAPP_DictionaryFormGroupWordsCtrlCreate(MMI_DUERAPP_DICTIONARY_GROUP_WORDS_WIN_ID, pTmpInfo);
    
    if (form_ctrl_handle == 0) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    
    //1����form����
    {       
        CTRLFORM_SetRect(form_ctrl_handle, &form_rect);
        CTRLFORM_SetBg(form_ctrl_handle, &form_bg);
        CTRLFORM_PermitChildBg(form_ctrl_handle, TRUE);
        CTRLFORM_PermitChildFont(form_ctrl_handle, TRUE);
        CTRLFORM_PermitChildBorder(form_ctrl_handle, TRUE);
        GUICTRL_SetProgress(IGuiCtrl, FALSE);
    }
    
    //2����form����form����
    {        
        for(i = 0; i < 2; i++) {//��FORM����������FORM
            child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, TRUE);
            CTRLFORM_SetChildSpace(form_ctrl_handle, child_form_ctrl_id, &hor_space, &ver_space);
            CTRLFORM_SetChildWidth(form_ctrl_handle, child_form_ctrl_id, &width);
            if (pTmpInfo->num <= 6) {//������ʾ����
                i == 0 ? (height.add_data = 50) : (height.add_data = 185);//235
                CTRLFORM_SetChildHeight(form_ctrl_handle, child_form_ctrl_id, &height);
            }
        }
    }
    
    //3����form����form1�Ŀؼ�����
    {        
        //���ñ���
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        label_wshowtxt.wstr_ptr = L"���", label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, 6);//�����������ҵĿ�϶
    }
    
    //4����form����form2����form����--���а�ť
    {       
        for(i = 0; i < 2; i++) {//��FORM����form2��form
            child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &form_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, TRUE);
        }
    }
    
    //5����form����form2����form��button�ؼ�����
    {
        //��ť�Ŀ��Ⱥ͸�
        width.add_data = 100;
        height.add_data = 40;
        //FORM���ڲ����հ�
        padding.top = 5;
        padding.bottom = 5;
        //��λBUF����
        memset(&s_dict_attr.group_word_info, 0, sizeof(dictionary_group_words_info_t));
        for (i = 0; i < pTmpInfo->num; i++) {
            button_ctrl_id = MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORMX_BUTTON1_CTRL_ID + i;
            i%2?(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM2_CTRL_ID):(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID);
            child_form_ctrl_id == MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM2_FORM1_CTRL_ID ?(padding.left = (5)):(padding.left = (8));
            CTRLFORM_SetChildWidth(child_form_ctrl_id, button_ctrl_id, &width);
            CTRLFORM_SetChildHeight(child_form_ctrl_id, button_ctrl_id, &height);
            CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
            CTRLFORM_SetChildSpace(child_form_ctrl_id, button_ctrl_id, &hor_space, &ver_space);

            GUIBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
            GUIBUTTON_SetRunSheen(button_ctrl_id, FALSE);
            GUIBUTTON_SetFg(button_ctrl_id, &button_bg_attr);
            GUIBUTTON_SetPressedFg(button_ctrl_id, &button_bg_attr);
            GUIBUTTON_SetFont(button_ctrl_id, &font_info);
            //UTF8������ʾ����
            PRV_DUERAPP_Tmp_Info_Reset();
            utf8_len = strlen(pTmpInfo->candidates[i]);
            GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pTmpInfo->candidates[i], utf8_len);
            button_text.wstr_ptr = s_tmp_info.out_wstr, button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
            GUIBUTTON_SetText(button_ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
            GUIBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_DictionaryFormGroupWordsImgButtonCb);
            //��������
            strcpy(s_dict_attr.group_word_info.words[i], pTmpInfo->candidates[i]);
        }
    }
    
    MMK_SetAtvCtrl(MMI_DUERAPP_DICTIONARY_GROUP_WORDS_WIN_ID, MMI_DUERAPP_DICTIONARY_GROUPWORDS_FORM_CTRL_ID);
}

LOCAL void PRV_DUERAPP_DictionaryGroupWordsWinOpen(void)
{
    SCI_TraceLow("(%s)(%d)[duer_watch]:open", __func__, __LINE__);
    MMK_CreateWin((uint32 *)MMI_DUERAPP_DICTIONARY_GROUP_WORDS_WIN_TAB, PNULL);
}
#endif

#if defined(DUERAPP_DICTIONARY_SINGLEWORD_SUPPORT)

LOCAL MMI_RESULT_E  PRV_DUERAPP_DictionarySingleWordMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         );

LOCAL void PRV_DUERAPP_DictionarySingleWordUiDraw(void *userdata);

LOCAL void PRV_DUERAPP_DictionaryFormSingleWordCtrlDestory(BOOLEAN is_create);

WINDOW_TABLE(MMI_DUERAPP_DICTIONARY_SINGLE_WORD_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_DICTIONARY_SINGLE_WORD_WIN_ID),
    WIN_FUNC((uint32)PRV_DUERAPP_DictionarySingleWordMsg),
    
    END_WIN
};

LOCAL MMI_RESULT_E  PRV_DUERAPP_DictionarySingleWordMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
    duerapp_dict_card_t *pTmpInfo = (duerapp_dict_card_t *)param;
    
    SCI_TraceLow("(%s)(%d)[duer_watch]:win_id(0x%0lx), msg_id(0x%0lx)", __func__, __LINE__, win_id, msg_id);
    
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            PRV_DUERAPP_WinDrawBG(win_id, MMI_BLACK_COLOR);
            break;
        }
        case MSG_FULL_PAINT:
        {
            if (pDictInfo) {
                SCI_TraceLow("(%s)(%d)[duer_watch]:word'number(%d), Draw", __func__, __LINE__, pDictInfo->n_words);
                PRV_DUERAPP_DictionarySingleWordUiDraw((void *)pDictInfo);
                PRV_DUERAPP_Dictionary_Param_Reset(DUERAPP_DICT_RENDER_NORMAL);
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:null", __func__, __LINE__);
            }

            MMIDUERAPP_start_dict_judge(win_id);

            break;
        }
        case MSG_CTL_OK:
        case MSG_CTL_MIDSK:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CLOSE_WINDOW:
        {
            PRV_DUERAPP_Dictionary_Param_Reset(DUERAPP_DICT_RENDER_NORMAL);
            PRV_DUERAPP_DictionaryFormSingleWordCtrlDestory(FALSE);
            break;
        }
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    pTmpInfo = NULL;

    return recode;
}

LOCAL MMI_RESULT_E PRV_DUERAPP_DictionaryFormSingleWordPinyinImgButtonCb(MMI_HANDLE_T ctrl_handle)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    int index = -1;
    int len = 0;
    int ret = 0;
    char text[64] = {0};
    MMI_STRING_T show_str = {0};
    MMI_HANDLE_T ctrl_id = MMK_GetCtrlId(ctrl_handle);
    
    switch (ctrl_id)
    {
        case MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON1_CTRL_ID :
        {
            index = 0;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON2_CTRL_ID :
        {
            index = 1;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON3_CTRL_ID :
        {
            index = 2;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON4_CTRL_ID :
        {
            index = 3;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON5_CTRL_ID :
        {
            index = 4;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON6_CTRL_ID :
        {
            index = 5;
            break;
        }
        
        default:
        {
            break;
        }
    }
    SCI_TraceLow("(%s)(%d)[duer_watch]:button cb, index(%d)", __func__, __LINE__, index);
    
    if (index >= 0) {
        len = strlen(s_dict_attr.single_word_info.pinyin[index]);
        if (len) {
            strncpy(text, s_dict_attr.single_word_info.pinyin[index] + 1, len - 2);//ȥ��������
            ret = duer_dcs_user_interact((const char *)text);
            SCI_TraceLow("(%s)(%d)[duer_watch]:ret(%d), pinyin(%s)", __func__, __LINE__, ret, text);
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
        }
    }
    
    return result;
}

LOCAL void PRV_DUERAPP_DictionaryFormSingleWordCtrlDestory(BOOLEAN is_create)
{
    static BOOLEAN is_have_create = FALSE;
    BOOLEAN state = FALSE;
    int i = 0;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    //FORM1
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_IMGTXTBUTTON_CTRL_ID);
    
    for(i = 0; i < DUERAPP_DICTIONARY_PINYIN_MAX; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_LABEL1_CTRL_ID + i);
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON1_CTRL_ID + i);
    }
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM2_TEXTBOX1_CTRL_ID);
    for(i = 0; i < DUERAPP_DICTIONARY_PINYIN_MAX; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID + i);
    }
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM2_CTRL_ID);
    
    //FORM2
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_TEXTBOX1_CTRL_ID);
    
    //FORM3
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_TEXTBOX1_CTRL_ID);
    
    //FORM4
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_TEXTBOX1_CTRL_ID);
    
    //FORM5
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_TEXTBOX1_CTRL_ID);
    
    //FORM6
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_TEXTBOX1_CTRL_ID);
    
    //Clear Form
    for(i = 0; i < DUERAPP_DICTIONARY_STRUCT_MAX; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID + i);
    }
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_SINGLE_FORM_CTRL_ID);
}

LOCAL MMI_HANDLE_T PRV_DUERAPP_DictionaryFormSingleWordCtrlCreate(MMI_HANDLE_T win_handle, void *userdata, BOOLEAN *is_valid)
{
    MMI_HANDLE_T ctrl_handle = 0;
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM_CTRL_ID;
    duerapp_dict_card_t *pDictInfo = (duerapp_dict_card_t *)userdata;
    duerapp_dict_entry_t *pDictEntryTmp = NULL;
    char *instr = NULL;
    int utf8_len = 0;
    int i = 0;

    GUIFORM_INIT_DATA_T childform_init_data = {0};
    GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
    
    GUIBUTTON_INIT_DATA_T word_buttom_init_data = {0};
    GUIFORM_DYNA_CHILD_T word_buttom_form_child_ctrl = {0};
    
    GUILABEL_INIT_DATA_T pinyin_label_init_data = {0};
    GUIFORM_DYNA_CHILD_T pinyin_label_form_child_ctrl = {0};
    
    GUIBUTTON_INIT_DATA_T pinyin_buttom_init_data = {0};
    GUIFORM_DYNA_CHILD_T pinyin_buttom_form_child_ctrl = {0};
    
    GUITEXT_INIT_DATA_T word_struct_init_data = {0};
    GUIFORM_DYNA_CHILD_T word_struct_form_child_ctrl = {0};
    
    MMI_HANDLE_T child_form1_form1_form2_form1_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID;
    
    MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_CTRL_ID;
    
    GUILABEL_INIT_DATA_T title_init_data = {0};
    GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
    
    GUITEXT_INIT_DATA_T content_init_data = {0};
    GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};

    // GUILABEL_INIT_DATA_T title_init_data = {0};
    // GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
    
    // GUITEXT_INIT_DATA_T content_init_data = {0};
    // GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
    
    // MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_CTRL_ID;
    
    // GUILABEL_INIT_DATA_T title_init_data = {0};
    // GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
    
    // GUITEXT_INIT_DATA_T content_init_data = {0};
    // GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
    
    // GUILABEL_INIT_DATA_T title_init_data = {0};
    // GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
    
    // GUITEXT_INIT_DATA_T content_init_data = {0};
    // GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
    
    // GUILABEL_INIT_DATA_T title_init_data = {0};
    // GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
    
    // GUITEXT_INIT_DATA_T content_init_data = {0};
    // GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
    
    if (pDictInfo == NULL
        || is_valid == NULL) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return 0;
    }
    instr = pDictInfo->words;
    
    //0�ͷ�
    PRV_DUERAPP_DictionaryFormSingleWordCtrlDestory(TRUE);

    //1��̬����form
    {
        GUIFORM_CreatDynaCtrl(win_handle, ctrl_id, GUIFORM_LAYOUT_ORDER);
    }

    //2��̬�����ʽṹ
    if (0 < strlen(instr)) {
        
        word_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_IMGTXTBUTTON_CTRL_ID;
        word_buttom_form_child_ctrl.init_data_ptr = &word_buttom_init_data;
        word_buttom_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        
        pinyin_label_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_LABEL1_CTRL_ID;
        pinyin_label_form_child_ctrl.init_data_ptr = &pinyin_label_init_data;
        pinyin_label_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        pinyin_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON1_CTRL_ID;
        pinyin_buttom_form_child_ctrl.init_data_ptr = &pinyin_buttom_init_data;
        pinyin_buttom_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        
        word_struct_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM2_TEXTBOX1_CTRL_ID;
        word_struct_form_child_ctrl.init_data_ptr = &word_struct_init_data;
        word_struct_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 0);
        
        //��̬����form1��form�ؼ�
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;//�֡�ƴ��(����)FORM
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;//���ס��ʻ����ṹ��FORM
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        //��̬����form1��form1��form�ؼ�
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;//��FORM
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_CTRL_ID, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;//����ƴ��(����)�Ĵ�FORM
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_CTRL_ID, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        //��̬����form1��form1��form2��form�ؼ�
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;//����ƴ��(����)����FORM
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        for (i = 0; i < pDictInfo->n_pinyins; i++) {
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_CTRL_ID, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
        
        //��̬����form1��form1��form1�ؼ�����BUTTON
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID, &word_buttom_form_child_ctrl);
        
        //��̬����form1��form1��form2�ؼ�����ƴ��LABEL������ͼƬ
        for (i = 0; i < pDictInfo->n_pinyins; i++) {
            pinyin_label_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_LABEL1_CTRL_ID + i;
            pinyin_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON1_CTRL_ID + i;
            child_form1_form1_form2_form1_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form1_form1_form2_form1_ctrl_id, &pinyin_label_form_child_ctrl);
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form1_form1_form2_form1_ctrl_id, &pinyin_buttom_form_child_ctrl);
        }
        
        //��̬����form��form�ؼ�������TEXTBOX
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM2_CTRL_ID, &word_struct_form_child_ctrl);
        //��Ч��FORM
        is_valid[0] = TRUE;
    }

    //3��̬��������
    pDictEntryTmp = pDictInfo->head;
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"����")) {
        
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 1);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[1] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //4��̬���������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"�����")) {
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_CTRL_ID;
        
        
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 2);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[2] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //5��̬���������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"�����")) {        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 3);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[3] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //6��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"���")) {
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_CTRL_ID;
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 4);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[4] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //7��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"���")) {
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_CTRL_ID;
                
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 5);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[5] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
    
    return ctrl_handle;
}

LOCAL void PRV_DUERAPP_DictionarySingleWordUiDraw(void *userdata)
{
    MMI_HANDLE_T form_ctrl_handle = 0;
    duerapp_dict_entry_t *pDictEntryTmp = NULL;
    int column = 0;
    int str_len = 0;
    int utf8_len = 0;
    char *instr = NULL;
    int i = 0;
    BOOLEAN is_valid[DUERAPP_DICTIONARY_STRUCT_MAX] = {0};
    duerapp_dict_card_t *pDictInfo = (duerapp_dict_card_t *)userdata;
    
    if (pDictInfo == NULL) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    
    if (pDictInfo->n_words != 1) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Error, n_words(%d)", __FUNCTION__, __LINE__, pDictInfo->n_words);
        return;
    }
    str_len = PRV_DUERAPP_Strlen_Utf8(pDictInfo->words);
    
    if (str_len == 0) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    form_ctrl_handle = PRV_DUERAPP_DictionaryFormSingleWordCtrlCreate(MMI_DUERAPP_DICTIONARY_SINGLE_WORD_WIN_ID, pDictInfo, is_valid);
    
    if (form_ctrl_handle == 0) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    instr = pDictInfo->words;
    
    //1����form����
    {
        GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_BLACK_COLOR, FALSE};
        GUI_RECT_T form_rect = {5,5,235,MMI_MAINSCREEN_HEIGHT};
        IGUICTRL_T *IGuiCtrl = MMK_GetCtrlPtr(form_ctrl_handle);
        
        CTRLFORM_SetRect(form_ctrl_handle, &form_rect);
        CTRLFORM_SetBg(form_ctrl_handle, &form_bg);
        CTRLFORM_PermitChildBg(form_ctrl_handle, TRUE);
        CTRLFORM_PermitChildFont(form_ctrl_handle, TRUE);
        CTRLFORM_PermitChildBorder(form_ctrl_handle, TRUE);
        // GUICTRL_SetProgress(IGuiCtrl, FALSE);
    }

    //2��̬���ôʽṹ
    if (is_valid[0]) {
        GUI_BG_T word_struct_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID;//�ʽṹ��FORM
        MMI_HANDLE_T child_form_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_CTRL_ID;//һ���� ��FORM
        MMI_HANDLE_T child_form_form_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID;//һ ������FORM
        MMI_HANDLE_T child_form1_form1_form2_form1_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID;
        
        GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        uint16 hor_space = 1;//ˮƽ���?
        uint16 ver_space = 1;//��ֱ���?
        GUI_PADDING_T padding = {0};

        //��ʼ��BUTTON�ؼ��Ĳ���(�ֺ�ƴ��(����))
        MMI_HANDLE_T button_ctrl_id = 0;
        GUI_BG_T button_bg_attr ={GUI_BG_IMG, GUI_SHAPE_ROUNDED_RECT, IMAGE_DUERAPP_DICTIONARY_WORD_BG_BIG, MMI_WHITE_COLOR, TRUE};
        GUI_BG_T button_rel_bg ={GUI_BG_NONE, GUI_SHAPE_RECT, 0, MMI_WHITE_COLOR, FALSE};//������������
        GUI_FONT_ALL_T font_info = {DUERAPP_DICTIONARY_BIGBIG_FONT, MMI_BLACK_COLOR};
        MMI_STRING_T button_text = {0};

        //��ʼ��LABEL�ؼ��Ĳ���(ƴ��)
        MMI_HANDLE_T label_ctrl_id = 0;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM2_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 0);
        
        for(i = 0; i < 2; i++) {//�ʽṹ��������FORM
            child_form_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_form_ctrl_id, &word_struct_bg);
            CTRLFORM_SetChildAlign(child_form_ctrl_id, child_form_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_SetAlign(child_form_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildBorder(child_form_form_ctrl_id, TRUE);
        }
        
        for(i = 0; i < 2; i++) {//�ʺ�(ƴ��������)��������FORM
            child_form_form_form_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_form_form_ctrl_id, &word_struct_bg);
            CTRLFORM_SetChildAlign(child_form_form_ctrl_id, child_form_form_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_SetAlign(child_form_form_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_form_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_form_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildBorder(child_form_form_form_ctrl_id, TRUE);
        }
        
        for(i = 0; i < pDictInfo->n_pinyins; i++) {//����(ƴ��������)����FORM
            child_form1_form1_form2_form1_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form1_form1_form2_form1_ctrl_id, &word_struct_bg);
            CTRLFORM_SetChildAlign(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_CTRL_ID, child_form1_form1_form2_form1_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_SetAlign(child_form1_form1_form2_form1_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form1_form1_form2_form1_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form1_form1_form2_form1_ctrl_id, TRUE);
            CTRLFORM_PermitChildBorder(child_form1_form1_form2_form1_ctrl_id, TRUE);
        }
        padding.top = 6;//�������?
        CTRLFORM_SetPadding(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID, &padding);
        
        //WORD BUTTON
        width.add_data = 107;//��ť����
        height.add_data = 107;//��ť�߶�
        padding.left = 3;//������
        padding.top = 10;//�������?
        button_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_IMGTXTBUTTON_CTRL_ID;
        CTRLFORM_SetChildWidth(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID, button_ctrl_id, &height);
        CTRLFORM_SetPadding(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM1_CTRL_ID, &padding);
        GUIBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
        GUIBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        GUIBUTTON_SetFg(button_ctrl_id, &button_bg_attr);
        GUIBUTTON_SetPressedFg(button_ctrl_id, &button_bg_attr);
        GUIBUTTON_SetFont(button_ctrl_id, &font_info);
        //UTF8������ʾ����
        PRV_DUERAPP_Tmp_Info_Reset();
        utf8_len = strlen(instr);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)instr, utf8_len);
        button_text.wstr_ptr = s_tmp_info.out_wstr, button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
        SCI_TraceLow("(%s)(%d)[duer_watch]:instr(%s)", __FUNCTION__, __LINE__, instr);
        GUIBUTTON_SetText(button_ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
        
        //PINYIN
        CTRLFORM_SetSpace(MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_CTRL_ID, &hor_space, &ver_space);
        button_bg_attr.img_id = IMAGE_DUERAPP_DICTIONARY_PINYIN_VOICE;
        //��λBUF����
        memset(&s_dict_attr.single_word_info, 0, sizeof(dictionary_single_word_info_t));
        for (i = 0; i < pDictInfo->n_pinyins; i++) {
            //PINYIN LABEL
            label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_LABEL1_CTRL_ID + i;
            child_form1_form1_form2_form1_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_FORM1_CTRL_ID + i;
            CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
            CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
            CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
            //UTF8������ʾ����
            PRV_DUERAPP_Tmp_Info_Reset();
            utf8_len = strlen(pDictInfo->pinyin[i]);
            GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictInfo->pinyin[i], utf8_len);
            label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
            CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
            //PINYIN BUTTON
            width.add_data = DUERAPP_DICTIONARY_SINGLEWORD_PINYIN_IMG_WIDTH;//��ť����
            height.add_data = DUERAPP_DICTIONARY_SINGLEWORD_PINYIN_IMG_HEIGHT;//��ť�߶�
            button_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_FORM1_FORM2_PINYIN_BUTTON1_CTRL_ID + i;
            CTRLFORM_SetChildWidth(child_form1_form1_form2_form1_ctrl_id, button_ctrl_id, &width);
            CTRLFORM_SetChildHeight(child_form1_form1_form2_form1_ctrl_id, button_ctrl_id, &height);
            GUIBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
            GUIBUTTON_SetRunSheen(button_ctrl_id, FALSE);
            GUIBUTTON_SetFg(button_ctrl_id, &button_bg_attr);
            GUIBUTTON_SetPressedFg(button_ctrl_id, &button_rel_bg);
            GUIBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_DictionaryFormSingleWordPinyinImgButtonCb);
            //��������
            strcpy(s_dict_attr.single_word_info.pinyin[i], pDictInfo->pinyin[i]);
        }
        
        //���ס��ʻ����ṹ��
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        //UTF8������ʾ����
        PRV_DUERAPP_Tmp_Info_Reset();
        utf8_len = strlen(pDictInfo->info);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictInfo->info, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
    }

    //3��̬��������
    pDictEntryTmp = pDictInfo->head;
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[1]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 1);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM2_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        //UTF8������ʾ����
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //4��̬���÷����?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[2]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 2);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM3_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        //UTF8������ʾ����
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //5��̬���ý����?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[3]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 3);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM4_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //6��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[4]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 4);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM5_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //7��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[5]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_SINGLE_FORM1_CTRL_ID, 5);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_SINGLE_FORM6_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
    }
    
    MMK_SetAtvCtrl(MMI_DUERAPP_DICTIONARY_SINGLE_WORD_WIN_ID, MMI_DUERAPP_DICTIONARY_SINGLE_FORM_CTRL_ID);
}

LOCAL void PRV_DUERAPP_DictionarySingleWordWinOpen(void)
{
#if DUERAPP_DICTIONARY_JSON_TEST_SUPPORT == 1
    duerapp_dict_card_t *card = NULL;
    
    MMIDUERAPP_HomePageOpenWinInit();
    card = duerapp_dict_create_card();
    SCI_TraceLow("(%s)(%d)[duer_watch]:json test", __func__, __LINE__);
    
    if (card) {
        baidu_json *json_obj = baidu_json_Parse(json_1word);
        if (json_obj) {
            duerapp_parse_card(card, json_obj);
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
            return;
        }
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:null", __func__, __LINE__);
        return;
    }
    MMK_CreateWin((uint32 *)MMI_DUERAPP_DICTIONARY_SINGLE_WORD_WIN_TAB, PNULL);
#else
    SCI_TraceLow("(%s)(%d)[duer_watch]:open", __func__, __LINE__);
    MMK_CreateWin((uint32 *)MMI_DUERAPP_DICTIONARY_SINGLE_WORD_WIN_TAB, PNULL);
#endif
}

#endif

#if defined(DUERAPP_DICTIONARY_MULTIWORDS_SUPPORT)

LOCAL MMI_RESULT_E  PRV_DUERAPP_DictionaryMultiWordsMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         );

LOCAL void PRV_DUERAPP_DictionaryMultiWordUiDraw(void *userdata);

LOCAL void PRV_DUERAPP_DictionaryFormMultiWordCtrlDestory(BOOLEAN is_create);

WINDOW_TABLE(MMI_DUERAPP_DICTIONARY_MULTIPLE_WORDS_WIN_TAB) =
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_DICTIONARY_MULTIPLE_WORDS_WIN_ID),
    WIN_FUNC((uint32)PRV_DUERAPP_DictionaryMultiWordsMsg),
    
    END_WIN
};

LOCAL MMI_RESULT_E  PRV_DUERAPP_DictionaryMultiWordsMsg(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
    duerapp_dict_card_t *pTmpInfo = (duerapp_dict_card_t *)param;
    
    SCI_TraceLow("(%s)(%d)[duer_watch]:win_id(0x%0lx), msg_id(0x%0lx)", __func__, __LINE__, win_id, msg_id);
    
    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            PRV_DUERAPP_WinDrawBG(win_id, MMI_BLACK_COLOR);
            break;
        }
        case MSG_FULL_PAINT:
        {
            if (pDictInfo) {
                SCI_TraceLow("(%s)(%d)[duer_watch]:word'number(%d), Draw", __func__, __LINE__, pDictInfo->n_words);
                PRV_DUERAPP_DictionaryMultiWordUiDraw((void *)pDictInfo);
                PRV_DUERAPP_Dictionary_Param_Reset(DUERAPP_DICT_RENDER_NORMAL);
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:null", __func__, __LINE__);
            }

            MMIDUERAPP_start_dict_judge(win_id);

            break;
        }
        case MSG_CTL_OK:
        case MSG_CTL_MIDSK:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CLOSE_WINDOW:
        {
            PRV_DUERAPP_Dictionary_Param_Reset(DUERAPP_DICT_RENDER_NORMAL);
            PRV_DUERAPP_DictionaryFormMultiWordCtrlDestory(FALSE);
            break;
        }
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }
    pTmpInfo = NULL;

    return recode;
}

LOCAL MMI_RESULT_E PRV_DUERAPP_DictionaryFormMultiWordsPinyinImgButtonCb(MMI_HANDLE_T ctrl_handle)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    int index = -1;
    int len = 0;
    int ret = 0;
    char text[64] = {0};
    MMI_STRING_T show_str = {0};
    MMI_HANDLE_T ctrl_id = MMK_GetCtrlId(ctrl_handle);
    
    switch (ctrl_id)
    {
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_IMGTXTBUTTON_CTRL_ID :
        {
            index = 0;
            len = strlen(s_dict_attr.multi_word_info.pinyin);
            if (len) {
                strncpy(text, s_dict_attr.multi_word_info.pinyin + 1, len - 2);//ȥ��������
                ret = duer_dcs_user_interact((const char *)text);
                SCI_TraceLow("(%s)(%d)[duer_watch]:ret(%d), pinyin(%s)", __func__, __LINE__, ret, text);
            } else {
                SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
            }
            break;
        }
        
        default:
        {
            break;
        }
    }
    SCI_TraceLow("(%s)(%d)[duer_watch]:button cb, index(%d)", __func__, __LINE__, index);
    
    return result;
}

LOCAL MMI_RESULT_E PRV_DUERAPP_DictionaryFormMultiWordsImgButtonCb(MMI_HANDLE_T ctrl_handle)
{
    MMI_RESULT_E result = MMI_RESULT_TRUE;
    int index = -1;
    MMI_STRING_T show_str = {0};
    MMI_HANDLE_T ctrl_id = MMK_GetCtrlId(ctrl_handle);
    
    switch (ctrl_id)
    {
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON1_CTRL_ID :
        {
            index = 0;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON2_CTRL_ID :
        {
            index = 1;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON3_CTRL_ID :
        {
            index = 2;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON4_CTRL_ID :
        {
            index = 3;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON5_CTRL_ID :
        {
            index = 4;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON6_CTRL_ID :
        {
            index = 5;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON7_CTRL_ID :
        {
            index = 6;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON8_CTRL_ID :
        {
            index = 7;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON9_CTRL_ID :
        {
            index = 8;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON10_CTRL_ID :
        {
            index = 9;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON11_CTRL_ID :
        {
            index = 10;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON12_CTRL_ID :
        {
            index = 11;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON13_CTRL_ID :
        {
            index = 12;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON14_CTRL_ID :
        {
            index = 13;
            break;
        }
        
        case MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON15_CTRL_ID :
        {
            index = 14;
            break;
        }
        
        default:
        {
            break;
        }
    }
    SCI_TraceLow("(%s)(%d)[duer_watch]:button cb, index(%d)", __func__, __LINE__, index);
    
    if (index >= 0) {
        if (strlen(s_dict_attr.multi_word_info.words[index])) {
            duerapp_dict_query(s_dict_attr.multi_word_info.words[index]);
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
        }
    }
    
    return result;
}

LOCAL void PRV_DUERAPP_DictionaryFormMultiWordCtrlDestory(BOOLEAN is_create)
{
    static BOOLEAN is_have_create = FALSE;
    BOOLEAN state = FALSE;
    int i = 0;
    
    if (is_create) {
        if (is_have_create) {
            state = TRUE;
        }
        is_have_create = is_create;
        if (state) {
            goto DESTORY;
        }
    } else {
        if (is_have_create) {
            is_have_create = FALSE;
            goto DESTORY;
        }
    }
    return;
DESTORY:
    //FORM1
    for(i = 0; i < DUERAPP_DICTIONARY_IMGTXTBUTTONX_MAX; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON1_CTRL_ID + i);
    }
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_TEXTBOX_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_IMGTXTBUTTON_CTRL_ID);
    for(i = 0; i < 3; i++) {//3��
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID + i);
    }
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_CTRL_ID);
    
    //FORM2
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM2_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM2_TEXTBOX1_CTRL_ID);
    
    //FORM3
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM3_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM3_TEXTBOX1_CTRL_ID);
    
    //FORM4
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM4_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM4_TEXTBOX1_CTRL_ID);
    
    //FORM5
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM5_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM5_TEXTBOX1_CTRL_ID);
    
    //FORM6
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM6_TITLE_LABEL_CTRL_ID);
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM6_TEXTBOX1_CTRL_ID);
    
    //Clear Form
    for(i = 0; i < DUERAPP_DICTIONARY_STRUCT_MAX; i++) {
        MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID + i);
    }
    MMK_DestroyControl(MMI_DUERAPP_DICTIONARY_MULTI_FORM_CTRL_ID);
}

LOCAL MMI_HANDLE_T PRV_DUERAPP_DictionaryFormMultiWordCtrlCreate(MMI_HANDLE_T win_handle, void *userdata, BOOLEAN *is_valid)
{
    MMI_HANDLE_T ctrl_handle = 0;
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM_CTRL_ID;
    duerapp_dict_card_t *pDictInfo = (duerapp_dict_card_t *)userdata;
    duerapp_dict_entry_t *pDictEntryTmp = NULL;
    char *instr = NULL;
    int utf8_len = 0;
    int i = 0;
    
    if (pDictInfo == NULL
        || is_valid == NULL) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return 0;
    }
    instr = pDictInfo->words;
    
    //0�ͷ�
    PRV_DUERAPP_DictionaryFormMultiWordCtrlDestory(TRUE);

    //1��̬����form
    {
        GUIFORM_CreatDynaCtrl(win_handle, ctrl_id, GUIFORM_LAYOUT_ORDER);
    }

    //2��̬�����ʽṹ
    if (0 < strlen(instr)) {
        GUIFORM_INIT_DATA_T childform_init_data = {0};
        GUIFORM_DYNA_CHILD_T childform_form_child_ctrl = {0};
        
        GUIBUTTON_INIT_DATA_T word_buttom_init_data = {0};
        GUIFORM_DYNA_CHILD_T word_buttom_form_child_ctrl = {0};
        
        GUITEXT_INIT_DATA_T pinyin_textbox_init_data = {0};
        GUIFORM_DYNA_CHILD_T pinyin_textbox_form_child_ctrl = {0};
        
        GUIBUTTON_INIT_DATA_T pinyin_buttom_init_data = {0};
        GUIFORM_DYNA_CHILD_T pinyin_buttom_form_child_ctrl = {0};
        
        MMI_HANDLE_T child_form_ctrl_id = 0;
        int column = 0;
        int words_cnt = 0;
        
        if (pDictInfo->n_words >= 2
            && pDictInfo->n_words <= 4) {
            column = 2;
        } else {
            column = 3;
        }
        
        word_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON1_CTRL_ID;
        word_buttom_form_child_ctrl.init_data_ptr = &word_buttom_init_data;
        word_buttom_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        
        pinyin_textbox_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_TEXTBOX_CTRL_ID;
        pinyin_textbox_form_child_ctrl.init_data_ptr = &pinyin_textbox_init_data;
        pinyin_textbox_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        pinyin_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_IMGTXTBUTTON_CTRL_ID;
        pinyin_buttom_form_child_ctrl.init_data_ptr = &pinyin_buttom_init_data;
        pinyin_buttom_form_child_ctrl.guid = SPRD_GUI_BUTTON_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 0);
        
        //��̬����form1��form�ؼ�
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;//���ֵ�FORM
        childform_form_child_ctrl.guid = SPRD_GUI_FORM_ID;
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_CTRL_ID;
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        childform_init_data.layout_type = GUIFORM_LAYOUT_SBS;//ƴ��(����)��FORM
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_CTRL_ID;
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
        GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        
        //��̬����form1��form1��form�ؼ�
        childform_init_data.layout_type = GUIFORM_LAYOUT_ORDER;//���ֵ�FORM�����?3��
        childform_form_child_ctrl.init_data_ptr = &childform_init_data;
        childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID;
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_CTRL_ID;
        for (i = 0; i < column; i++) {//2-4����2�У�4-15����3�У���������
            childform_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID + i;
            GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &childform_form_child_ctrl);
            GUIFORM_SetStyle(childform_form_child_ctrl.child_handle, GUIFORM_STYLE_NORMAL);
        }
        
        //��̬����form1��form1��form1�ؼ���BUTTON
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID;
        if (column == 2) {
            for (i = 0; i < pDictInfo->n_words; i++) {
                i%2?(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM2_CTRL_ID):(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID);
                word_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON1_CTRL_ID + i;
                GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &word_buttom_form_child_ctrl);
            }
        } else {//3��
            for (i = 0; i < pDictInfo->n_words; i++) {
                child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID + words_cnt;
                words_cnt!= 2?(words_cnt++):(words_cnt = 0);
                word_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON1_CTRL_ID + i;
                GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &word_buttom_form_child_ctrl);
            }
        }
        
        //��̬����form1��form1��form2�ؼ�����ƴ��TEXTBOX
        pinyin_textbox_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_TEXTBOX_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_CTRL_ID, &pinyin_textbox_form_child_ctrl);
        
        //��̬����form1��form1��form2�ؼ�����ƴ��BUTTON
        pinyin_buttom_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_IMGTXTBUTTON_CTRL_ID;
        GUIFORM_CreatDynaChildCtrl(win_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_CTRL_ID, &pinyin_buttom_form_child_ctrl);
        //��Ч��FORM
        is_valid[0] = TRUE;
    }

    //3��̬��������
    pDictEntryTmp = pDictInfo->head;
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"����")) {
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM2_CTRL_ID;
        
        GUILABEL_INIT_DATA_T title_init_data = {0};
        GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
        
        GUITEXT_INIT_DATA_T content_init_data = {0};
        GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM2_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM2_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 1);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[1] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //4��̬���������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"�����")) {
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM3_CTRL_ID;
        
        GUILABEL_INIT_DATA_T title_init_data = {0};
        GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
        
        GUITEXT_INIT_DATA_T content_init_data = {0};
        GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM3_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM3_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 2);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[2] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //5��̬���������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"�����")) {
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM4_CTRL_ID;
        
        GUILABEL_INIT_DATA_T title_init_data = {0};
        GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
        
        GUITEXT_INIT_DATA_T content_init_data = {0};
        GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM4_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM4_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 3);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[3] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //6��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"���")) {
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM5_CTRL_ID;
        
        GUILABEL_INIT_DATA_T title_init_data = {0};
        GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
        
        GUITEXT_INIT_DATA_T content_init_data = {0};
        GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM5_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM5_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 4);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[4] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //7��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    utf8_len = strlen(pDictEntryTmp->title);
    GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
    if (0 == MMIAPICOM_Wstrcmp(s_tmp_info.out_wstr, L"���")) {
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM6_CTRL_ID;
        
        GUILABEL_INIT_DATA_T title_init_data = {0};
        GUIFORM_DYNA_CHILD_T title_form_child_ctrl = {0};
        
        GUITEXT_INIT_DATA_T content_init_data = {0};
        GUIFORM_DYNA_CHILD_T content_form_child_ctrl = {0};
        
        title_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM6_TITLE_LABEL_CTRL_ID;
        title_form_child_ctrl.init_data_ptr = &title_init_data;
        title_form_child_ctrl.guid = SPRD_GUI_LABEL_ID;
        
        content_form_child_ctrl.child_handle = MMI_DUERAPP_DICTIONARY_MULTI_FORM6_TEXTBOX1_CTRL_ID;
        content_form_child_ctrl.init_data_ptr = &content_init_data;
        content_form_child_ctrl.guid = SPRD_GUI_TEXTBOX_ID;
        
        //��̬������form
        PRV_DUERAPP_Form_Create(win_handle, ctrl_id, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 5);
        
        //��̬����form�ӿؼ�TITLE
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &title_form_child_ctrl);
        //��̬����form�ӿؼ�CONTENT
        GUIFORM_CreatDynaChildCtrl(win_handle, child_form_ctrl_id, &content_form_child_ctrl);
        //��Ч��FORM
        is_valid[5] = TRUE;
        pDictEntryTmp = pDictEntryTmp->next;
    }

    ctrl_handle = MMK_GetCtrlHandleByWin(win_handle, ctrl_id);
    
    return ctrl_handle;
}

LOCAL void PRV_DUERAPP_DictionaryMultiWordUiDraw(void *userdata)
{
    MMI_HANDLE_T form_ctrl_handle = 0;
    duerapp_dict_entry_t *pDictEntryTmp = NULL;
    int column = 0;
    int str_len = 0;
    int utf8_len = 0;
    char *instr = NULL;
    int i = 0;
    BOOLEAN is_valid[DUERAPP_DICTIONARY_STRUCT_MAX] = {0};
    duerapp_dict_card_t *pDictInfo = (duerapp_dict_card_t *)userdata;
    
    if (pDictInfo == NULL) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    SCI_TraceLow("(%s)(%d):[duer_watch]:n_words(%d)", __FUNCTION__, __LINE__, pDictInfo->n_words);
    
    if (pDictInfo->n_words < 2) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Error, n_words(%d)", __FUNCTION__, __LINE__, pDictInfo->n_words);
        return;
    }
    str_len = PRV_DUERAPP_Strlen_Utf8(pDictInfo->words);
    
    if (str_len == 0) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    form_ctrl_handle = PRV_DUERAPP_DictionaryFormMultiWordCtrlCreate(MMI_DUERAPP_DICTIONARY_MULTIPLE_WORDS_WIN_ID, pDictInfo, is_valid);

    if (form_ctrl_handle == 0) {
        SCI_TraceLow("(%s)(%d)[duer_watch]:Invalid param", __FUNCTION__, __LINE__);
        return;
    }
    instr = pDictInfo->words;
    
    //����form����
    {
        GUI_BG_T form_bg = {GUI_BG_COLOR, GUI_SHAPE_RECT, 0, MMI_BLACK_COLOR, FALSE};
        GUI_RECT_T form_rect = {5,5,235,MMI_MAINSCREEN_HEIGHT};
        IGUICTRL_T *IGuiCtrl = MMK_GetCtrlPtr(form_ctrl_handle);;
        
        CTRLFORM_SetRect(form_ctrl_handle, &form_rect);
        CTRLFORM_SetBg(form_ctrl_handle, &form_bg);
        CTRLFORM_PermitChildBg(form_ctrl_handle, TRUE);
        CTRLFORM_PermitChildFont(form_ctrl_handle, TRUE);
        CTRLFORM_PermitChildBorder(form_ctrl_handle, TRUE);
        // GUICTRL_SetProgress(IGuiCtrl, FALSE);
    }

    //2��̬���ôʽṹ
    if (is_valid[0]) {
        GUI_BG_T word_struct_bg = {GUI_BG_COLOR, GUI_SHAPE_ROUNDED_RECT, 0, MMI_WHITE_COLOR, FALSE};
        MMI_HANDLE_T child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID;//�ʽṹ��FORM
        
        GUIFORM_CHILD_WIDTH_T width = {0, GUIFORM_CHILD_WIDTH_FIXED};
        GUIFORM_CHILD_HEIGHT_T height = {0, GUIFORM_CHILD_HEIGHT_FIXED};
        uint16 hor_space = 1;//ˮƽ���?
        uint16 ver_space = 1;//��ֱ���?
        GUI_PADDING_T padding = {0};
        int words_cnt = 0;

        //��ʼ��BUTTON�ؼ��Ĳ���(�ֺ�����)
        MMI_HANDLE_T button_ctrl_id = 0;
        GUI_BG_T button_bg_attr ={GUI_BG_IMG, GUI_SHAPE_RECT, IMAGE_DUERAPP_DICTIONARY_WORD_BG_BIG, MMI_WHITE_COLOR, TRUE};
        GUI_BG_T button_rel_bg ={GUI_BG_NONE, GUI_SHAPE_RECT, 0, MMI_WHITE_COLOR, FALSE};//������������
        GUI_FONT_ALL_T font_info = {DUERAPP_DICTIONARY_BIGBIG_FONT, MMI_BLACK_COLOR};
        MMI_STRING_T button_text = {0};

        //��ʼ��TEXT�ؼ��Ĳ���(ƴ��)
        MMI_HANDLE_T textbox_ctrl_id = 0;
        GUI_COLOR_T textbox_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T textbox_font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        MMI_STRING_T textbox_wshowtxt = {0};
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 0);
        
        for(i = 0; i < 2; i++) {//�ʽṹ��������FORM
            child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &word_struct_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, TRUE);
            CTRLFORM_SetSpace(child_form_ctrl_id, &hor_space, &ver_space);
        }

        if (pDictInfo->n_words >= 2
            && pDictInfo->n_words <= 4) {
            column = 2;
        } else {
            column = 3;
        }
        
        for(i = 0; i < column; i++) {//���ֵ�FORM-���?3��
            child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID + i;
            CTRLFORM_SetBg(child_form_ctrl_id, &word_struct_bg);
            CTRLFORM_SetAlign(child_form_ctrl_id, GUIFORM_CHILD_ALIGN_HMIDDLE);
            CTRLFORM_PermitChildBg(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildFont(child_form_ctrl_id, TRUE);
            CTRLFORM_PermitChildBorder(child_form_ctrl_id, TRUE);
        }
        
        //WORD BUTTON
        if (column == 2) {
            width.add_data = 107;//��ť����
            height.add_data = 107;//��ť�߶�
            button_bg_attr.img_id = IMAGE_DUERAPP_DICTIONARY_WORD_BG_BIG;
            font_info.font = DUERAPP_DICTIONARY_BIGBIG_FONT;
        } else {
            width.add_data = 69;//��ť����
            height.add_data = 69;//��ť�߶�
            button_bg_attr.img_id = IMAGE_DUERAPP_DICTIONARY_WORD_BG_NORMAL;
            font_info.font = DUERAPP_DICTIONARY_BIG_FONT;
        }
        padding.top = (2);//�������?
        //��λBUF����
        memset(&s_dict_attr.multi_word_info, 0, sizeof(dictionary_multi_word_info_t));
        
        for (i = 0; i < pDictInfo->n_words; i++) {
            button_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_IMGTXTBUTTON1_CTRL_ID + i;
            if (column == 2) {
                i%2?(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM2_CTRL_ID):(child_form_ctrl_id=MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID);
                child_form_ctrl_id == MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID ?(padding.left = (2)):(padding.left = (3));
            } else {//3��
                child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM1_FORM1_CTRL_ID + words_cnt;
                words_cnt!= 2?(words_cnt++):(words_cnt = 0);
                switch (words_cnt) {
                    case 1: padding.left = (2); break;
                    case 2: padding.left = (4); break;
                    default : padding.left = (6); break;
                }
            }
            CTRLFORM_SetChildWidth(child_form_ctrl_id, button_ctrl_id, &width);
            CTRLFORM_SetChildHeight(child_form_ctrl_id, button_ctrl_id, &height);
            CTRLFORM_SetPadding(child_form_ctrl_id, &padding);
            
            GUIBUTTON_SetTextAlign(button_ctrl_id, ALIGN_HVMIDDLE);
            GUIBUTTON_SetRunSheen(button_ctrl_id, FALSE);
            GUIBUTTON_SetFg(button_ctrl_id, &button_bg_attr);
            GUIBUTTON_SetPressedFg(button_ctrl_id, &button_bg_attr);
            GUIBUTTON_SetFont(button_ctrl_id, &font_info);
            //UTF8������ʾ����
            PRV_DUERAPP_Tmp_Info_Reset();
            instr = PRV_DUERAPP_Get_Utf8_Char(instr, s_tmp_info.out_str);
            SCI_TraceLow("(%s)(%d)[duer_watch]:instr(%s), out_str(%s)", __FUNCTION__, __LINE__, instr, s_tmp_info.out_str);
            utf8_len = strlen(s_tmp_info.out_str);
            GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)s_tmp_info.out_str, utf8_len);
            button_text.wstr_ptr = s_tmp_info.out_wstr,  button_text.wstr_len = MMIAPICOM_Wstrlen(button_text.wstr_ptr);
            GUIBUTTON_SetText(button_ctrl_id, button_text.wstr_ptr, button_text.wstr_len);
            GUIBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_DictionaryFormMultiWordsImgButtonCb);
            //��������
            strcpy(s_dict_attr.multi_word_info.words[i], s_tmp_info.out_str);
        }
        
        //PINYIN TEXTBOX
        textbox_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_TEXTBOX_CTRL_ID;
        GUITEXT_SetAlign(textbox_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(textbox_ctrl_id, &textbox_font_size, &textbox_font_color);
        GUITEXT_IsDisplayPrg(FALSE, textbox_ctrl_id);
        GUITEXT_SetClipboardEnabled(textbox_ctrl_id, FALSE);
        //UTF8������ʾ����
        PRV_DUERAPP_Tmp_Info_Reset();
        SCI_TraceLow("(%s)(%d)[duer_watch]:pinyin[0](%s)", __FUNCTION__, __LINE__, pDictInfo->pinyin[0]);
        utf8_len = strlen(pDictInfo->pinyin[0]);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictInfo->pinyin[0], utf8_len);
        textbox_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, textbox_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(textbox_wshowtxt.wstr_ptr);
        GUITEXT_SetString(textbox_ctrl_id, textbox_wshowtxt.wstr_ptr, textbox_wshowtxt.wstr_len, TRUE);
        //PINYIN BUTTON
        width.add_data = DUERAPP_DICTIONARY_MULTIWORDS_PINYIN_IMG_WIDTH;//��ť����
        height.add_data = DUERAPP_DICTIONARY_MULTIWORDS_PINYIN_IMG_HEIGHT;//��ť�߶�
        child_form_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_CTRL_ID;
        button_bg_attr.img_id = IMAGE_DUERAPP_DICTIONARY_PINYIN_VOICE;
        button_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM1_FORM2_IMGTXTBUTTON_CTRL_ID;
        CTRLFORM_SetChildWidth(child_form_ctrl_id, button_ctrl_id, &width);
        CTRLFORM_SetChildHeight(child_form_ctrl_id, button_ctrl_id, &height);
        GUIBUTTON_SetRunSheen(button_ctrl_id, FALSE);
        GUIBUTTON_SetFg(button_ctrl_id, &button_bg_attr);
        GUIBUTTON_SetPressedFg(button_ctrl_id, &button_rel_bg);
        GUIBUTTON_SetCallBackFuncExt(button_ctrl_id, PRV_DUERAPP_DictionaryFormMultiWordsPinyinImgButtonCb);
        //��������
        strcpy(s_dict_attr.multi_word_info.pinyin, pDictInfo->pinyin[0]);
    }
    
    //3��̬��������
    pDictEntryTmp = pDictInfo->head;
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[1]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM2_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM2_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 1);

        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM2_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        //UTF8������ʾ����
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(label_wshowtxt.wstr_ptr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶

        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //4��̬���÷����?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[2]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM3_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM3_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 2);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM3_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(s_tmp_info.out_wstr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //5��̬���ý����?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[3]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM4_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM4_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 3);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM4_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(s_tmp_info.out_wstr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //6��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[4]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM5_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM5_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 4);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM5_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(s_tmp_info.out_wstr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
        
        pDictEntryTmp = pDictEntryTmp->next;
    }

    //7��̬�������?
    PRV_DUERAPP_Tmp_Info_Reset();
    
    if (is_valid[5]) {
        //��ʼ��LABEL�ؼ��Ĳ���
        MMI_HANDLE_T label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM6_TITLE_LABEL_CTRL_ID;
        GUI_COLOR_T label_font_color = MMI_WHITE_COLOR;
        GUI_FONT_T label_font_size = DUERAPP_DICTIONARY_TITLE_FONT;
        MMI_STRING_T label_wshowtxt = {0};

        //��ʼ��TEXTBOX�ؼ��Ĳ���
        GUIFORM_CHILD_WIDTH_T width = {(240), GUIFORM_CHILD_WIDTH_FIXED};
        MMI_HANDLE_T text_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM6_TEXTBOX1_CTRL_ID;
        GUI_COLOR_T font_color = MMI_WHITE_COLOR;
        GUI_FONT_T font_size = DUERAPP_DICTIONARY_CONTENT_FONT;
        
        //������form����
        PRV_DUERAPP_Form_Set_Style(form_ctrl_handle, MMI_DUERAPP_DICTIONARY_MULTI_FORM1_CTRL_ID, 5);
        
        //���ñ���
        label_ctrl_id = MMI_DUERAPP_DICTIONARY_MULTI_FORM6_TITLE_LABEL_CTRL_ID;
        CTRLLABEL_SetAlign(label_ctrl_id, GUILABEL_ALIGN_LEFT);
        CTRLLABEL_SetFont(label_ctrl_id, label_font_size, label_font_color);
        CTRLLABEL_SetVisible(label_ctrl_id, TRUE, TRUE);
        utf8_len = strlen(pDictEntryTmp->title);
        GUI_UTF8ToWstr(s_tmp_info.out_wstr, utf8_len, (const uint8 *)pDictEntryTmp->title, utf8_len);
        label_wshowtxt.wstr_ptr = s_tmp_info.out_wstr, label_wshowtxt.wstr_len = MMIAPICOM_Wstrlen(s_tmp_info.out_wstr);
        CTRLLABEL_SetText(label_ctrl_id, &label_wshowtxt, TRUE);
        CTRLLABEL_SetMarginEx(label_ctrl_id, DUERAPP_DICTIONARY_TITLE_LEFTRIGHTMARGIN, DUERAPP_DICTIONARY_TITLE_TOPBOTTOMMARGIN);//�����������ҵĿ�϶
        
        //��������
        GUITEXT_SetAlign(text_ctrl_id, ALIGN_LEFT);
        GUITEXT_SetFont(text_ctrl_id, &font_size, &font_color);
        GUITEXT_IsDisplayPrg(FALSE, text_ctrl_id);
        GUITEXT_SetClipboardEnabled(text_ctrl_id, FALSE);
        utf8_len = strlen(pDictEntryTmp->detail);
        GUI_UTF8ToWstr(s_tmp_info.outdetail_wstr, utf8_len, (const uint8 *)pDictEntryTmp->detail, utf8_len);
        GUITEXT_SetString(text_ctrl_id, s_tmp_info.outdetail_wstr, MMIAPICOM_Wstrlen(s_tmp_info.outdetail_wstr), TRUE);
    }
    
    MMK_SetAtvCtrl(MMI_DUERAPP_DICTIONARY_MULTIPLE_WORDS_WIN_ID, MMI_DUERAPP_DICTIONARY_MULTI_FORM_CTRL_ID);
}

LOCAL void PRV_DUERAPP_DictionaryMultipleWordsWinOpen(void)
{
#if DUERAPP_DICTIONARY_JSON_TEST_SUPPORT == 1
    duerapp_dict_card_t *card = NULL;
    
    MMIDUERAPP_HomePageOpenWinInit();
    card = duerapp_dict_create_card();
    SCI_TraceLow("(%s)(%d)[duer_watch]:json test", __func__, __LINE__);
    
    if (card) {
        baidu_json *json_obj = baidu_json_Parse(json_10words);
        if (json_obj) {
            duerapp_parse_card(card, json_obj);
        } else {
            SCI_TraceLow("(%s)(%d)[duer_watch]:error", __func__, __LINE__);
            return;
        }
    } else {
        SCI_TraceLow("(%s)(%d)[duer_watch]:null", __func__, __LINE__);
        return;
    }
    
    MMK_CreateWin((uint32 *)MMI_DUERAPP_DICTIONARY_MULTIPLE_WORDS_WIN_TAB, PNULL);
#else
    SCI_TraceLow("(%s)(%d)[duer_watch]:open", __func__, __LINE__);
    MMK_CreateWin((uint32 *)MMI_DUERAPP_DICTIONARY_MULTIPLE_WORDS_WIN_TAB, PNULL);
#endif
}

#endif

LOCAL BOOLEAN duerapp_dict_is_allowed_use(void)
{
    //���ô��������߼���Ϊ��֤��Ӱ���ֵ��ڴ��ͷţ��Ƚ��н�������ٵ�֧����ά��?
    static unsigned int gs_dict_try_cnt = 0;
    BOOLEAN is_login = duerapp_is_login();
    DUER_USER_INFO *info = duerapp_get_user_info();
    BOOLEAN is_vip = (info && info->is_vip);
    if (is_login && is_vip) {
        gs_dict_try_cnt = 0;
        DUER_LOGI("%s, is vip with login", __FUNCTION__);
    } else {
        if (gs_dict_try_cnt < DUERAPP_DICTIONARY_TRY_USE_CNT) {
            gs_dict_try_cnt++;
            DUER_LOGI("%s, is try", __FUNCTION__);
        } else {
            DUER_LOGI("%s, can not try", __FUNCTION__);
            return FALSE;
        }
    }
    return TRUE;
}

LOCAL MMIDUERAPP_start_dict_judge(MMI_WIN_ID_T win_id)
{
    if (duerapp_dict_is_allowed_use() == FALSE) {
        //���ô��������߼���Ϊ��֤�����ֵ���ڴ�ȫ�������ͷţ��Ƚ��н�������ٵ�֧����ά��
        wchar *wstr_ptr = L"��ת��...";
        duerapp_show_toast(wstr_ptr);
        //��֧����ά�룬��Ҫ���û�������DIDP
        duer_set_didp_version(DIDP_VERSION_NORMAL);     //DIDP version for normal voice interactive mode
        duer_update_user_agent();
        duer_send_link_click_url(LINKCLICK_BUY_VIP);
        MMK_CloseWin(win_id);
    }
}

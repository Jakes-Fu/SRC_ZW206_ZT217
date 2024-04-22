/**--------------------------------------------------------------------------*
**                         Include Files                                     *
**---------------------------------------------------------------------------*/

#include "guilistbox.h"
#include "os_api.h"
#include "sci_types.h"

#include "std_header.h"
#include "window_parse.h"
#include "guilcd.h"
#include "mmidisplay_data.h"
#include "guifont.h"
#include "mmi_textfun.h"
#include "guitext.h"
#include "guilabel.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "watch_common_list.h"
#include "mmi_event_api.h"
#include "mmk_app.h"
#include "guisetlist.h"
#include "guiiconlist.h"
#include "baidu_json.h"
#include "lightduer_log.h"
#include "lightduer_lib.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_types.h"
#include "lightduer_dcs.h"
#include "duerapp.h"
#include "duerapp_id.h"
#include "duerapp_image.h"
#include "duerapp_mplayer.h"
#include "duerapp_common.h"
#include "duerapp_http.h"
#include "duerapp_login.h"
#include "duerapp_center.h"
#include "duerapp_payload.h"
#include "duerapp_main.h"
//#endif
#define DUERAPP_ICONLIST_ICON_NUM 4
#define DUERAPP_CLIENT_RECT {0,0,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT}

//LOCAL void duerapp_res_list_enter(int index);
LOCAL MMI_RESULT_E  HandleListMainWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         );

LOCAL MMI_RESULT_E HandleDuerIconlistWinMsg(
                                            MMI_WIN_ID_T        win_id,
                                            MMI_MESSAGE_ID_E    msg_id,
                                            DPARAM              param
                                            );

/**--------------------------------------------------------------------------*
 **                         CONSTANT VARIABLES                               *
 **--------------------------------------------------------------------------*/
WINDOW_TABLE( DUERAPP_ICONLIST_WIN_TAB ) = 
{
    WIN_HIDE_STATUS,
    WIN_ID(MMI_DUERAPP_ICONLIST_WIN_ID),    
    WIN_FUNC( (uint32)HandleDuerIconlistWinMsg),
    CREATE_ICONLIST_CTRL(MMI_DUERAPP_ICONLIST_CTRL_ID),
    END_WIN
};

WINDOW_TABLE( MMI_DUERAPP_CENTER_LISTBOX_WIN_TAB ) =
{
    WIN_HIDE_STATUS,
    WIN_ID( MMI_DUERAPP_CENTER_LISTBOX_WIN_ID ),
    WIN_FUNC((uint32)HandleListMainWindow ),
    CREATE_LISTBOX_CTRL( GUILIST_TEXTLIST_E, MMI_DUERAPP_CENTER_LISTBOX_CTRL_ID),
    END_WIN
};
/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/

DUEROS_RESOURCE_TYPE_E s_dueros_audio_type;
char* s_p_story_url = "dueros://audio_unicast_story/albumplay?album_id=%s";
char* s_p_skill_url = "dueros://server.dueros.ai/query?q=%s";
DUEROS_STORY_LIST_ITEM_ST *s_real_story_list;
DUEROS_SKILL_LIST_ITEM_ST *s_real_skill_list;
int s_skill_list_size = 0;
int s_story_list_size = 0;
int s_lesson_list_size = 0;
int s_discovery_list_size = 0;
DUEROS_DISCOVERY_LIST_ITEM_ST *s_lesson_list;
DUEROS_DISCOVERY_LIST_ITEM_ST *s_discovery_list;
DUEROS_BLOCK_LIST_ITEM_ST     *s_home_block_list;
DUEROS_BLOCK_LIST_ITEM_ST     *s_skill_block_list;
int                           s_home_block_list_size = 0;
int                           s_skill_block_list_size = 0;
DUEROS_CENTER_CFG_ITEM_ST     *s_center_cfg;


//�б����ܴ����ص�
/**---------------------------------------------------------------------------------------------*
 **                         LOCAL FUNCTION DEFINITION                                           *
 **---------------------------------------------------------------------------------------------*/
LOCAL void MMIDUERAPP_ListItem_Click(int index)
{
    char p_url[256] = {0};
    char *query_url = NULL;
    char *res_id = NULL;
    int url_len = 0;

    switch(s_dueros_audio_type) {
        case DUER_AUDIO_SKILL:
        duer_dcs_close_multi_dialog();
        query_url = s_real_skill_list[index].query_url;
        sprintf(p_url,s_p_skill_url,query_url);
        break;
        case DUER_AUDIO_UNICAST_STORY:
        case DUER_AUDIO_LESSON:
        {
        #if 0
        if (s_real_story_list[index].res_url) {
            strcpy(p_url,s_real_story_list[index].res_url);
        } else {
           res_id = s_real_story_list[index].res_id;
           sprintf(p_url,s_p_story_url,res_id);
        }
        #else
        url_len = strlen(s_real_story_list[index].res_link_url);
        DUER_LOGI("%s res link url len=%d", __FUNCTION__, url_len);
        if (url_len > 0) {
            strncpy(p_url, s_real_story_list[index].res_link_url, MIN(RESOURCE_URL_MAX_LENGTH - 1, url_len));
        } else {
           res_id = s_real_story_list[index].res_id;
           sprintf(p_url,s_p_story_url,res_id);
        }
        #endif
        }
        break;
        default:
        break;
    }

    duer_dcs_dialog_cancel();
    DUER_LOGI("dueros linkurl = %s", p_url);
    duer_send_link_click_url(p_url);

    // �ر���������
    if (MMK_IsOpenWin(MMI_DUERAPP_CENTER_LISTBOX_WIN_ID)) 
    {
        MMK_CloseWin(MMI_DUERAPP_CENTER_LISTBOX_WIN_ID);
    }

    if (MMK_IsOpenWin(MMI_DUERAPP_ICONLIST_WIN_ID)) {
        MMK_CloseWin(MMI_DUERAPP_ICONLIST_WIN_ID);
    }

    duerapp_show_waiting(NULL);
}
PUBLIC void MMIDUERAPP_SkillListItem_Click(char *query_url)
{
    char p_url[256] = {0};
    char *res_id = NULL;
    if (!MMK_IsOpenWin(MMI_DUERAPP_MAIN_WIN_ID)) {
        duerapp_set_interact_mode(DUERAPP_INTERACT_MODE_NORMAL);
        MMIDUERAPP_StartNormalAsrSet();
        MMIDUERAPP_CreateDuerMainWin(FALSE);
    }

    duer_dcs_close_multi_dialog();
    snprintf(p_url, 256, s_p_skill_url, query_url);

    duer_dcs_dialog_cancel();
    DUER_LOGI("dueros linkurl = %s", p_url);
    duer_send_link_click_url(p_url);

    if (MMK_IsOpenWin(MMI_DUERAPP_CENTER_LISTBOX_WIN_ID)) 
    {
        MMK_CloseWin(MMI_DUERAPP_CENTER_LISTBOX_WIN_ID);
    }

    duerapp_show_waiting(NULL);


}

LOCAL void MMIDUERAPP_ResListOpenWin(MMI_WIN_ID_T win_id,MMI_CTRL_ID_T ctrl_id,int index)
{
    GUILIST_EMPTY_INFO_T empty_info = {0};
    DUEROS_DISCOVERY_LIST_ITEM_ST *list_data = NULL;
    int list_size = 0;
    int max_size = 0;
    int i = 0;
    int utf8_len = 0;
    wchar wch_txt[RESOURCE_WCHAR_NAME_MAX_LENGTH] = {0};
    MMI_STRING_T  item_str = {0};
    wchar *text_buffer = L"加载中，请稍等...";

    if (s_dueros_audio_type == DUER_AUDIO_SKILL) {
        if (s_real_skill_list) {
            MMK_SendMsg(win_id,MSG_DUERAPP_SKILL_CALL_BACK,PNULL);
        } else {
            empty_info.text_buffer.wstr_ptr = text_buffer;
            empty_info.text_buffer.wstr_len = MMIAPICOM_Wstrlen(text_buffer);
            GUILIST_SetEmptyInfo(ctrl_id, &empty_info);
            duerapp_get_watch_config();
        }
    } else {
        DUER_LOGI("story_list index = %d",index);
        GUILIST_RemoveAllItems(ctrl_id);
        if (s_dueros_audio_type == DUER_AUDIO_LESSON) {
            list_size = s_lesson_list_size;
            list_data = s_lesson_list;
        } else if (s_dueros_audio_type == DUER_AUDIO_UNICAST_STORY) {
            list_size = s_discovery_list_size;
            list_data = s_discovery_list;
        }
        if (index >= list_size) {
            MMK_CloseWin(win_id);
        } else {
            s_real_story_list = list_data[index].res_list_st;
            if (s_real_story_list) {
                max_size = list_data[index].res_size;
                GUILIST_SetMaxItem(ctrl_id, max_size, FALSE);
                for (i = 0; i < max_size; i++) {

                    utf8_len = strlen(s_real_story_list[i].res_utf8_name);
                    DUER_MEMSET(wch_txt, 0, sizeof(wch_txt));
                    GUI_UTF8ToWstr(wch_txt, utf8_len, s_real_story_list[i].res_utf8_name, utf8_len);
                    // wch_txt[RESOURCE_WCHAR_NAME_MAX_LENGTH - 1] = 0;
                    //debugassert
                    if (wch_txt[RESOURCE_WCHAR_NAME_MAX_LENGTH - 1] != 0) {
                        DUER_LOGE("story list wchar err");
                        SCI_ASSERT(0);
                    }

                    item_str.wstr_ptr = wch_txt;
                    item_str.wstr_len = MMIAPICOM_Wstrlen(item_str.wstr_ptr);
                    WatchCOM_ListItem_Draw_1Str(ctrl_id,item_str);
                }
            } else {
                MMK_CloseWin(win_id);
            }
        }
    }
}

LOCAL void MMIDUERAPP_UpdateSkillList(MMI_WIN_ID_T win_id,MMI_CTRL_ID_T ctrl_id)
{
    int max_size = s_skill_list_size;
    int i = 0;

    GUILIST_RemoveAllItems(ctrl_id);
    GUILIST_SetMaxItem(ctrl_id, max_size, FALSE);
    for (i = 0; i < max_size; i++) {

        // int utf8_len = strlen(s_real_skill_list[i].res_utf8_name);
        // wchar wch_txt[RESOURCE_WCHAR_NAME_MAX_LENGTH] = {0};
        // GUI_UTF8ToWstr(wch_txt, utf8_len, s_real_skill_list[i].res_utf8_name, utf8_len);

        // MMI_STRING_T  item_str = {0};
        // item_str.wstr_ptr = wch_txt; //s_real_skill_list[i].res_name;
        // item_str.wstr_len = MMIAPICOM_Wstrlen(item_str.wstr_ptr);
        // WatchCOM_ListItem_Draw_1Str(ctrl_id,item_str);
    }
    MMK_SetAtvCtrl(win_id,ctrl_id);
    MMK_SendMsg(win_id, MSG_FULL_PAINT, PNULL);
}
/*****************************************************************************/
//  Description : to handle Settings window message
//  Global resource dependence :
//  Author:bin.wang1
//  Note:
/*****************************************************************************/
LOCAL MMI_RESULT_E  HandleListMainWindow(
                                         MMI_WIN_ID_T    win_id,
                                         MMI_MESSAGE_ID_E   msg_id,
                                         DPARAM             param
                                         )
{
    MMI_CTRL_ID_T ctrl_id = MMI_DUERAPP_CENTER_LISTBOX_CTRL_ID;
    MMI_RESULT_E recode =  MMI_RESULT_TRUE;
    int array_index = (int)MMK_GetWinAddDataPtr(win_id);
    int idx = 0;

    switch(msg_id)
    {
        case MSG_OPEN_WINDOW:
        {
            MMIDUERAPP_ResListOpenWin(win_id,ctrl_id,array_index);
            GUILIST_SetBgColor(ctrl_id, MMI_BLACK_COLOR);
            MMK_SetAtvCtrl(win_id, ctrl_id);
            break;
        }

        case MSG_CTL_OK:
        case MSG_CTL_MIDSK:
        case MSG_CTL_PENOK:
        {
            idx = GUILIST_GetCurItemIndex(ctrl_id);
            MMIDUERAPP_ListItem_Click(idx);
            break;
        }
        case MSG_CTL_CANCEL:
        case MSG_APP_CANCEL:
        {
            MMK_CloseWin(win_id);
            break;
        }
        case MSG_CLOSE_WINDOW:
            break;
        case MSG_DUERAPP_SKILL_CALL_BACK:
            //V2�汾δʹ��listչʾ�����б�
            // MMIDUERAPP_UpdateSkillList(win_id,ctrl_id);
            break;
        default:
            recode = MMI_RESULT_FALSE;
            break;
    }

    return recode;
}


LOCAL void MMIDUERAPP_IconListOpenWin(MMI_WIN_ID_T win_id,MMI_CTRL_ID_T ctrl_id) {

    GUI_RECT_T  icon_rect   = {0,0,0,0};
    GUIICONLIST_MARGINSPACE_INFO_T margin_space = {10,10,10,10};
    GUI_BG_T                    bg = {0};
    GUI_BORDER_T                select_border = {0};
    MMI_STRING_T                name_wstr = {0};
    GUI_FONT_ALL_T              font_all = {0};
    uint16                      img_width = 94;
    uint16                      img_height = 94;


    GUIICONLIST_SetStyle(ctrl_id, GUIICONLIST_STYLE_ICON_UIDT);
    GUIICONLIST_SetIconWidthHeight(ctrl_id, img_width, img_height);
    bg.bg_type = GUI_BG_COLOR;
    bg.color = MMI_BLACK_COLOR;
    select_border.width = 0;
    select_border.type = GUI_BORDER_NONE;
    font_all.color = MMI_WHITE_COLOR;
    font_all.font = WATCH_DEFAULT_BIG_FONT;
    GUIICONLIST_SetIconListTextInfo(ctrl_id, font_all);   
    GUIICONLIST_SetItemBorderStyle(ctrl_id, FALSE, &select_border);
    GUIICONLIST_SetItemBorderStyle(ctrl_id, TRUE, &select_border);
    GUIICONLIST_SetIconItemSpace(ctrl_id, margin_space);
    GUIICONLIST_SetLoadType(ctrl_id, GUIICONLIST_LOAD_ALL);
    GUIICONLIST_SetBg(ctrl_id,&bg);

    //����iconlist�ķ�Χ
    GUIAPICTRL_SetRect(ctrl_id, &icon_rect);
}

LOCAL void MMIDUERAPP_UpdateIconList(MMI_WIN_ID_T win_id,MMI_CTRL_ID_T ctrl_id) {

}

LOCAL void duerapp_request_config_with_type(MMI_WIN_ID_T win_id) {
    switch(s_dueros_audio_type) {
        case DUER_AUDIO_UNICAST_STORY:
            if (s_discovery_list) {
                MMK_SendMsg(win_id,MSG_DUERAPP_DISCOVERY_CALL_BACK,PNULL);
            } else {
                duerapp_get_watch_config();
            }
            break;
        case DUER_AUDIO_LESSON:
            if (s_lesson_list) {
                MMK_SendMsg(win_id,MSG_DUERAPP_LESSON_CALL_BACK,PNULL);
            } else {
                duerapp_get_watch_lesson();
            }
            break;
        default:
            break;
    }
}

LOCAL MMI_RESULT_E HandleDuerIconlistWinMsg(
                                            MMI_WIN_ID_T        win_id,
                                            MMI_MESSAGE_ID_E    msg_id,
                                            DPARAM              param
                                            )
{
    MMI_CTRL_ID_T               ctrl_id = MMI_DUERAPP_ICONLIST_CTRL_ID;
    uint16                      icon_index = 0;
    MMI_RESULT_E                result = MMI_RESULT_TRUE;
    uint16  cur_index = 0;
    
    switch( msg_id)
    {
    case MSG_OPEN_WINDOW:
        duerapp_show_waiting_text();
        MMIDUERAPP_IconListOpenWin(win_id,ctrl_id);
        duerapp_request_config_with_type(win_id);
        break;

    case MSG_FULL_PAINT:
        break;

    case MSG_CTL_ICONLIST_APPEND_TEXT:
        break;

    case MSG_CTL_ICONLIST_APPEND_ICON:
        break;

    case MSG_CTL_CANCEL:
        MMK_CloseWin(win_id);
        break;
    case MSG_CTL_OK:
    case MSG_CTL_MIDSK:
    case MSG_CTL_PENOK:
    {
        cur_index = GUIICONLIST_GetCurIconIndex(ctrl_id);
        DUER_LOGI("GUIICONLIST_GetCurIconIndex index = %d",cur_index);
        duerapp_res_list_enter(cur_index);
        break;
    }
    case MSG_DUERAPP_DISCOVERY_CALL_BACK:
        MMIDUERAPP_UpdateIconList(win_id,ctrl_id);
        break;
    case MSG_DUERAPP_LESSON_CALL_BACK:
        MMIDUERAPP_UpdateIconList(win_id,ctrl_id);
        break;
    case MSG_CLOSE_WINDOW:
        break;

    default:
        result = MMI_RESULT_FALSE;
        break;
    }

    return (result);
}


/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/

PUBLIC void duerapp_res_list_enter(int index)
{
    DUER_LOGI("%s", __FUNCTION__);
#if 1 //V2/V3�б�
    MMK_CreateWin((uint32 *)MMI_DUERAPP_CENTER_LISTBOX_WIN_TAB, (ADD_DATA)index);
#else
    MMIDUERAPP_CreatePlayListWin((ADD_DATA)index);
#endif
}

// skill callback
PUBLIC void duerapp_audio_skill_list(DUEROS_SKILL_LIST_ITEM_ST *skill_list,int list_size)
{
    s_real_skill_list = skill_list;
    s_skill_list_size = list_size;
    MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_CENTER_LISTBOX_WIN_ID,MSG_DUERAPP_SKILL_CALL_BACK,PNULL,0);
}

// lesson callback
PUBLIC void duerapp_audio_lesson_list(DUEROS_DISCOVERY_LIST_ITEM_ST *lesson_list,int lesson_size)
{
    s_lesson_list = lesson_list;
    s_lesson_list_size = lesson_size;
    MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_ICONLIST_WIN_ID,MSG_DUERAPP_LESSON_CALL_BACK,PNULL,0);
}

// discovery callback
PUBLIC void duerapp_audio_discovery_list(DUEROS_DISCOVERY_LIST_ITEM_ST *discovery_list,int list_size)
{
    s_discovery_list_size = list_size;
    s_discovery_list = discovery_list;
    #if 0 //�����ӿڣ�����UI�߳̽ӿڴ���ʹ��
    if (MMK_IsOpenWin(MMI_DUERAPP_HOME_WIN_ID)) {
        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_DISCOVERY_CALL_BACK, PNULL, 0);
    } else if (MMK_IsOpenWin(MMI_DUERAPP_ICONLIST_WIN_ID)) {
        MMIAPICOM_OtherTaskToMMI(MMI_DUERAPP_ICONLIST_WIN_ID, MSG_DUERAPP_DISCOVERY_CALL_BACK, PNULL, 0);
    }
    #else
    DUER_LOGI("%s", __FUNCTION__);
    MMK_duer_other_task_to_MMI(MMI_DUERAPP_HOME_WIN_ID, MSG_DUERAPP_DISCOVERY_CALL_BACK, PNULL, 0);
    #endif
}

PUBLIC void duerapp_audio_home_block_list(DUEROS_BLOCK_LIST_ITEM_ST *list,int list_size)
{
    s_home_block_list_size = list_size;
    s_home_block_list = list;
}

PUBLIC void duerapp_audio_skill_block_list(DUEROS_BLOCK_LIST_ITEM_ST *list,int list_size)
{
    s_skill_block_list_size = list_size;
    s_skill_block_list = list;
}

PUBLIC void duerapp_audio_center_config(DUEROS_CENTER_CFG_ITEM_ST *cfg)
{
    s_center_cfg = cfg;
}

// �ͷ��ڴ�
static volatile signed char is_duerapp_free_res_list = 0;
PUBLIC void duerapp_free_res_list(void)
{
    DUER_LOGI("DUER_EXIT: %s-%d\r\n", __func__, __LINE__);
    if (1 == is_duerapp_free_res_list) {
        DUER_LOGI("DUER_EXIT: already freeing...\r\n");
        return;
    }
    is_duerapp_free_res_list = 1;
    duerapp_free_all_list();
    s_lesson_list = NULL;
    s_lesson_list_size = 0;
    s_discovery_list = NULL;
    s_discovery_list_size = 0;
    s_real_skill_list = NULL;
    s_skill_list_size = 0;

    s_home_block_list_size = 0;
    s_home_block_list = NULL;
    s_skill_block_list_size = 0;
    s_skill_block_list = NULL;
    s_center_cfg = NULL;

    is_duerapp_free_res_list = 0;
}

// ��Դ��������ڣ����еĴ��ڱ���Ӵ˴������жϣ�������ֱ�ӽ���
PUBLIC void MMIDUERAPP_CreateIconWin(DUEROS_RESOURCE_TYPE_E audio_type)
{
    s_dueros_audio_type = audio_type;
    if (audio_type == DUER_AUDIO_SKILL) {
        DUER_LOGI("%s", __FUNCTION__);
        MMK_CreateWin( (uint32 *)MMI_DUERAPP_CENTER_LISTBOX_WIN_TAB, PNULL);
    } else {
        MMK_CreateWin((uint32*)DUERAPP_ICONLIST_WIN_TAB, PNULL);
    }
}

PUBLIC void DuerappAudioTypeSet(int audio_type)
{
    s_dueros_audio_type = audio_type;
}

PUBLIC int DiscoveryGetIndexFromTag(char *tag)
{
    int i = 0;
    int index = -1;
    if (!tag) {
        DUER_LOGE("%s tag err", __FUNCTION__);
        return -1;
    }

    if (s_discovery_list && s_discovery_list_size > 0) {
        for (i = 0; i < s_discovery_list_size; i++) {
            if (strcmp(s_discovery_list[i].res_tag, tag) == 0) {
                index = i;
                DUER_LOGI("%s get tag(%s) index=%d", __FUNCTION__, tag, index);
                return index;
            }
        }
    }

    return -1;
}

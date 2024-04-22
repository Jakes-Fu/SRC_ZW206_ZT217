#ifndef _DUERAPP_PAYLOAD_H_
#define _DUERAPP_PAYLOAD_H_

#include "sci_types.h"
#include "mmi_module.h"
#include "mmi_image.h"

#ifdef __cplusplus
extern   "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// directive payload struct define

#define RENCARD_TYPE_LIST_CARD  "ListCard"
#define RENCARD_TYPE_TEXT_CARD  "TextCard"
#define RENCARD_TYPE_STAND_CARD "StandardCard"

#define LINKCLICK_BUY_VIP "dueros://cc33b657-eacb-2b9e-4f7c-e2fe002b1327/buyXiaoduWatchVip?source=user_center"
#define LINKCLICK_NOTIFY "dueros://0a7d4511-26d3-78ab-9652-2781dd6a073f/watchEnabelNotify"

#define OPERATE_BUTTON_MAX 10
#define RESOURCE_TAG_MAX_LENGTH 20
#define RESOURCE_ID_MAX_LENGTH 15
#define RESOURCE_NAME_MAX_LENGTH 30
#define RESOURCE_UTF8_NAME_MAX_LENGTH 96
#define RESOURCE_WCHAR_NAME_MAX_LENGTH 128
#define RESOURCE_URL_MAX_LENGTH 90
#define RESOURCE_QUERY_URL_MAX_LENGTH 65

#define DUERAPP_PARSE_MAGIC_NUM 0xA55A

#define DUER_LIST_CARD_NUMBER_MAX 20//list 卡片最大数量，若修改请同步修改 MMI_DUERAPP_RENDER_CARD_LIST_FORM_CTRL_ID 下的三个控件数量

///////////////////////////////////////////////////////////////////////////////
///

typedef enum QR_LOGIN_TYPE
{
    QR_LOGIN_IN_TYPE  = 0,
    QR_AUTH_TYPE,
    QR_PAY_TYPE,
    QR_MAX
} QR_LOGIN_TYPE_E;

typedef struct
{
	char *title;
	char *subtitle;
    char *url;
    char *art_src_url;
} RENDER_PLAYER_INFO;

typedef struct 
{
	// 二维码类型
	QR_LOGIN_TYPE_E login_type;
	// 内容展示
	wchar *msg;
	// 二维码url
	char *url;
	// 二维码查询状态url
	char *url_status;
} QR_LOGIN_URL_INFO, *Pt_QR_LOGIN_URL_INFO;


/*****************************************************************************/
typedef enum
{
    DUER_AUDIO_SKILL,
    DUER_AUDIO_MUSIC,
    DUER_AUDIO_UNICAST_STORY,
    DUER_AUDIO_LESSON
} DUEROS_RESOURCE_TYPE_E;

typedef enum
{
    DUER_ASSISTS_ERROR,
    DUER_ASSISTS_NOLOGIN,
    DUER_ASSISTS_LOGIN,
} DUERAPP_ASSISTS_RESPONSE_TYPE_E;

typedef struct
{ 
    char              res_tag[RESOURCE_TAG_MAX_LENGTH];
    char              query_url[RESOURCE_QUERY_URL_MAX_LENGTH];
    char              res_utf8_name[RESOURCE_UTF8_NAME_MAX_LENGTH];
    // wchar             res_name[RESOURCE_NAME_MAX_LENGTH];
    int               block_id;
    char              *img_url;
} DUEROS_SKILL_LIST_ITEM_ST,*P_DUEROS_SKILL_LIST_ITEM_ST;

typedef struct
{
    int               block_id;
    // char              block_utf8_name[RESOURCE_UTF8_NAME_MAX_LENGTH];
    wchar             block_name[RESOURCE_NAME_MAX_LENGTH];
    // char              banner_utf8_name[RESOURCE_UTF8_NAME_MAX_LENGTH];
    wchar             banner_name[RESOURCE_NAME_MAX_LENGTH];
    char              *banner_img_url;
    char              *banner_url;
    char              show_type[RESOURCE_TAG_MAX_LENGTH];
} DUEROS_BLOCK_LIST_ITEM_ST,*P_DUEROS_BLOCK_LIST_ITEM_ST;

typedef struct
{
    char              *banner_img_url;
    char              *banner_url;
    char              show_type[RESOURCE_TAG_MAX_LENGTH];
} DUEROS_CENTER_CFG_ITEM_ST,*P_DUEROS_CENTER_CFG_ITEM_ST;

typedef struct
{
    unsigned int      magic_head; 
    char              res_tag[RESOURCE_TAG_MAX_LENGTH];
    char              res_id[RESOURCE_ID_MAX_LENGTH];
    char              res_utf8_name[RESOURCE_UTF8_NAME_MAX_LENGTH];
    // wchar             res_name[RESOURCE_NAME_MAX_LENGTH];
    char              res_link_url[RESOURCE_URL_MAX_LENGTH];
    unsigned int      magic_tail; 
} DUEROS_STORY_LIST_ITEM_ST,*P_DUEROS_STORY_LIST_ITEM_ST;

/**
 * 资源标签、名字、个数
 */
typedef struct
{
    int               res_size;
    char              res_tag[RESOURCE_TAG_MAX_LENGTH];
    // char              res_utf8_name[RESOURCE_UTF8_NAME_MAX_LENGTH];
    wchar             res_name[RESOURCE_NAME_MAX_LENGTH];
    int               block_id;
    char              *img_url;
    char              *skill_query_url;
    DUEROS_STORY_LIST_ITEM_ST *res_list_st;
} DUEROS_DISCOVERY_LIST_ITEM_ST;

/**
 * title 文字限制.
 * 副标题、内容、url不限定字数
 * button 文字限制
 */
typedef struct
{
    wchar             *title;
    uint32            title_color;
    wchar             *subtitle;
    uint32            subtitle_color;
    wchar             *content;
    uint32            content_color;
    char              *link_url;
    BOOLEAN           show_button;
    wchar             button_text[OPERATE_BUTTON_MAX];
    uint32            button_color;
    uint32            button_bg;
    char              *bg_img_url;
    char              *img_url;
} DUEROS_OPERATE_NOTIFY,*P_DUEROS_OPERATE_NOTIFY;

typedef struct
{
    char *content;
} duerapp_text_card_t;

typedef struct
{
    char *content;
    char *image_url;
    char *image_click_url;
} duerapp_standard_card_t;

typedef struct
{
    int index;
    int cnt;
    char image_src[DUER_LIST_CARD_NUMBER_MAX][128];
    char title[DUER_LIST_CARD_NUMBER_MAX][128];
    char content[DUER_LIST_CARD_NUMBER_MAX][128];
    char url[DUER_LIST_CARD_NUMBER_MAX][128];
} duerapp_list_card_t;

typedef struct
{
    int type;//卡片类型
    void *timer_id;
    duerapp_text_card_t *textcard;
    duerapp_standard_card_t *standardcard;
    duerapp_list_card_t *listcard;
} duerapp_render_card_t;

/*****************************************************************************/
#ifdef __cplusplus
}
#endif

#endif
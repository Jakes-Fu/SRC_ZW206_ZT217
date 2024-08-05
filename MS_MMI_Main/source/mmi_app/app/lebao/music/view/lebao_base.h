#ifndef LEBAO_BASE_H
#define LEBAO_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
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
#include "guibutton.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "guires.h"

#include "lebao_waiting.h"

/*********************
 *      DEFINES	
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef MMI_RESULT_E(*LEBAO_BTN_CALLBACK_FUNC)(void);

typedef enum {
	TEXT_LEBAO_BEGIN = 0,
	TEXT_LEBAO_WELCOME,
	TEXT_LEBAO_MENU_CHARTS,
	TEXT_LEBAO_MENU_SEARCH,
	TEXT_LEBAO_MENU_LOCAL,
	TEXT_LEBAO_MENU_CLEAR,
	TEXT_LEBAO_MENU_ORDER,
	TEXT_LEBAO_MENU_SETTING,
	TEXT_LEBAO_ERR_NETWORK,
	TEXT_LEBAO_ERR_SERVICE,
	TEXT_LEBAO_ERR_UPDATE,
	TEXT_LEBAO_TIPS_LOADING,
	TEXT_LEBAO_TIPS_MEMBER_ALREADY,
	TEXT_LEBAO_TIPS_CLEAN,
	TEXT_LEBAO_TIPS_BUSY,
	TEXT_LEBAO_NO_MUSIC,
	TEXT_LEBAO_PLAY_FAILED,
	TEXT_LEBAO_RING_FAILED,
	TEXT_LEBAO_SAY_NAME,
	TEXT_LEBAO_PUSH_TALK,
	TEXT_LEBAO_OVER_TALK,
	TEXT_LEBAO_TIME_SHORT,
	TEXT_LEBAO_SETTING_VOLUME,
	TEXT_LEBAO_SEARCHING,
	TEXT_LEBAO_FOUND_NOTHING,
	TEXT_LEBAO_SCAN_QR,
	TEXT_LEBAO_RING_OK,
	TEXT_LEBAO_MEMORY_FULL,
	TEXT_LEBAO_ERR_DOWNLOAD,
	TEXT_LEBAO_SEARCH_LIMIT,
	TEXT_LEBAO_ADD_FAVORITE,
	TEXT_LEBAO_DOWNLOAD_OK,
	TEXT_LEBAO_NO_SIM_CARD,
	TEXT_LEBAO_NO_MORE,
	TEXT_LEBAO_MUSIC_LIST,
	TEXT_LEBAO_DEL_FAVORITE,
	TEXT_LEBAO_NO_RINGTONE,
	TEXT_LEBAO_MENU_ABOUT,
	TEXT_LEBAO_MENU_CONTACT,
	TEXT_LEBAO_MIGU,
    TEXT_LEBAO_TIPS_PLAYER_SLIDE_UP,
    TEXT_LEBAO_TIPS_DELETE_SONG,
    TEXT_LEBAO_TIPS_FAVORITE_FULL,
    TEXT_LEBAO_TIPS_FAVORITE_EMPTY,
    TEXT_LEBAO_TIPS_VIP_VALID_DATE,
    TEXT_LEBAO_WAITING,
    TEXT_LEBAO_TIPS_SLOW_NETWORK,
    TEXT_LEBAO_TIPS_TIMEOUT,
    TEXT_LEBAO_NEED_VIP,
	TEXT_LEBAO_END
} lebao_text_id_t;

typedef enum
{
	RES_IMAGE_LEBAO_BEGIN = 0,
	RES_IMAGE_LEBAO_LOGO,
	RES_IMAGE_LEBAO_SPLASH,
	RES_IMAGE_LEBAO_MENU_CHATS,
	RES_IMAGE_LEBAO_MENU_SEARCH,
	RES_IMAGE_LEBAO_MENU_LOCAL,
	RES_IMAGE_LEBAO_MENU_ORDER,
	RES_IMAGE_LEBAO_MENU_SETTING,
	RES_IMAGE_LEBAO_MENU_CONTACT,
	RES_IMAGE_LEBAO_MENU_ABOUT,
	RES_IMAGE_LEBAO_BTN_BACK,
	RES_IMAGE_LEBAO_BTN_CLOSE,
	RES_IMAGE_LEBAO_BTN_VOLUME,
	RES_IMAGE_LEBAO_BTN_DELETE,
	RES_IMAGE_LEBAO_BTN_FAVORITE,
	RES_IMAGE_LEBAO_BTN_FAVORITE_SEL,
	RES_IMAGE_LEBAO_BTN_NEXT,
	RES_IMAGE_LEBAO_BTN_PREVIOU,
	RES_IMAGE_LEBAO_BTN_RING,
	RES_IMAGE_LEBAO_BTN_RING_SEL,
	RES_IMAGE_LEBAO_BTN_PLAY,
	RES_IMAGE_LEBAO_BTN_PAUSE,
	RES_IMAGE_LEBAO_BTN_VOL_ADD,
	RES_IMAGE_LEBAO_BTN_VOL_SUB,
	RES_IMAGE_LEBAO_ITEM_D1,
	RES_IMAGE_LEBAO_ITEM_D2,
	RES_IMAGE_LEBAO_ITEM_D3,
	RES_IMAGE_LEBAO_ITEM_D4,
	RES_IMAGE_LEBAO_BTN_DEFAULT,
	RES_IMAGE_LEBAO_RECORD,
	RES_IMAGE_LEBAO_RECORD_GRAY,
	RES_IMAGE_LEBAO_BTN_STAR,
	RES_IMAGE_LEBAO_LINE,
	RES_IMAGE_LEBAO_VIP,
	RES_IMAGE_LEBAO_INDICATOR,
	RES_IMAGE_LEBAO_LOADING,
	RES_IMAGE_LEBAO_END
} lebao_image_id_t;

typedef enum
{
	RES_MMI_LEBAO_CTRL_ID_START = 0,

	// menu
	RES_MMI_LEBAO_CTRL_ID_MENU_OPTION,

	// list
	RES_MMI_LEBAO_CTRL_ID_LIST_MENU,
	RES_MMI_LEBAO_CTRL_ID_LIST_CHARTS,
	RES_MMI_LEBAO_CTRL_ID_LIST_MUSICLIST,
	RES_MMI_LEBAO_CTRL_ID_LIST_PLAYER,
	RES_MMI_LEBAO_CTRL_ID_LIST_LOCAL,
	RES_MMI_LEBAO_CTRL_ID_LIST_ABOUT,
	RES_MMI_LEBAO_CTRL_ID_LIST_SETTING,

	// progress
	RES_MMI_LEBAO_CTRL_ID_PGB_MUSIC,

	// button
	RES_MMI_LEBAO_CTRL_ID_BTN_VOLUME,
	RES_MMI_LEBAO_CTRL_ID_BTN_BACK,
	RES_MMI_LEBAO_CTRL_ID_BTN_PLAY,
	RES_MMI_LEBAO_CTRL_ID_BTN_PAUSE,
	RES_MMI_LEBAO_CTRL_ID_BTN_NEXT,
	RES_MMI_LEBAO_CTRL_ID_BTN_PREV,
	RES_MMI_LEBAO_CTRL_ID_BTN_RING,
	RES_MMI_LEBAO_CTRL_ID_BTN_DEL,
	RES_MMI_LEBAO_CTRL_ID_BTN_VOL_ADD,
	RES_MMI_LEBAO_CTRL_ID_BTN_VOL_SUB,
	RES_MMI_LEBAO_CTRL_ID_BTN_RECORD,
	RES_MMI_LEBAO_CTRL_ID_BTN_FAVORITE,
	RES_MMI_LEBAO_CTRL_ID_BTN_CALL,

	// image
	RES_MMI_LEBAO_CTRL_ID_IMG_QRCODE,
	RES_MMI_LEBAO_CTRL_ID_IMG_RECORD,
	RES_MMI_LEBAO_CTRL_ID_IMG_LOADING,
	RES_MMI_LEBAO_CTRL_ID_IMG_VIP,
	RES_MMI_LEBAO_CTRL_ID_IMG_LOGO,

	// text
	RES_MMI_LEBAO_CTRL_ID_TEXT_CONFIRM,

	// editbox
	RES_MMI_LEBAO_CTRL_ID_EDIT_MOBILE,
	RES_MMI_LEBAO_CTRL_ID_EDIT_CAPTCHA,
	RES_MMI_LEBAO_CTRL_ID_EDIT_USERNAME,
	RES_MMI_LEBAO_CTRL_ID_EDIT_PASSWORD,
	RES_MMI_LEBAO_CTRL_ID_EDIT_CONTENT,

	// label
	RES_MMI_LEBAO_CTRL_ID_LABEL_WELCOME,
	RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE,
	RES_MMI_LEBAO_CTRL_ID_LABEL_SCAN,
	RES_MMI_LEBAO_CTRL_ID_LABEL_VOLUME,
	RES_MMI_LEBAO_CTRL_ID_LABEL_TIMER,
	RES_MMI_LEBAO_CTRL_ID_LABEL_SONG_NAME,
	RES_MMI_LEBAO_CTRL_ID_LABEL_SINGER_NAME,
	RES_MMI_LEBAO_CTRL_ID_LABEL_TIPS,
	RES_MMI_LEBAO_CTRL_ID_LABEL_SCAN_DESC,
	RES_MMI_LEBAO_CTRL_ID_LABEL_VOLUME_TITLE,
	RES_MMI_LEBAO_CTRL_ID_LABEL_CURRENT_TIME,
	RES_MMI_LEBAO_CTRL_ID_LABEL_TOTAL_TIME,
	RES_MMI_LEBAO_CTRL_ID_LABEL_MOBILE,
	RES_MMI_LEBAO_CTRL_ID_LABEL_CAPTCHA,

	RES_MMI_LEBAO_CTRL_ID_MAX
} lebao_ctrl_id_t;

typedef enum
{
	RES_MMI_LEBAO_WIN_ID_START = 0,

	RES_MMI_LEBAO_WIN_ID_SPLASH,
	RES_MMI_LEBAO_WIN_ID_MAINMENU,
	RES_MMI_LEBAO_WIN_ID_SETTING,
	RES_MMI_LEBAO_WIN_ID_CHARTS,
	RES_MMI_LEBAO_WIN_ID_MUSICLIST,
	RES_MMI_LEBAO_WIN_ID_PLAYER,
	RES_MMI_LEBAO_WIN_ID_VOLUME,
	RES_MMI_LEBAO_WIN_ID_SEARCH,
	RES_MMI_LEBAO_WIN_ID_ORDER,
	RES_MMI_LEBAO_WIN_ID_LOCAL,
	RES_MMI_LEBAO_WIN_ID_WAITING,
	RES_MMI_LEBAO_WIN_ID_ABOUT,
	RES_MMI_LEBAO_WIN_ID_CONFIRM,
	RES_MMI_LEBAO_WIN_ID_OPTION,
	RES_MMI_LEBAO_WIN_ID_VIP,
	RES_MMI_LEBAO_WIN_ID_LOGIN,
	RES_MMI_LEBAO_WIN_ID_CONTACT,
	RES_MMI_LEBAO_WIN_ID_SUB_CHART,
	RES_MMI_LEBAO_WIN_ID_SONGLIST,

	RES_MMI_LEBAO_WIN_ID_MAX
} lebao_win_id_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
// test
void lebao_create_circle(void);

const char* lebao_get_text(const int index);
// external relations
void lebao_set_res_image_id(const uint32 begin, const uint32 end);
void lebao_set_res_ctrl_id(const uint32 begin, const uint32 end);
void lebao_set_res_win_id(const uint32 begin, const uint32 end);
uint32  lebao_get_res_image_id(const int index);
uint32  lebao_get_res_ctrl_id(const int index);
uint32  lebao_get_res_win_id(const int index);

void lebao_set_default_backgroud(const MMI_WIN_ID_T win_id, CAF_COLOR_T color);
void lebao_set_title_bar(MMI_CTRL_ID_T labelId, const int index);
void lebao_set_title_bar_text(MMI_CTRL_ID_T labelId, const char * utf8Text);
void lebao_set_title_bar_id(MMI_CTRL_ID_T labelId, const int resid);

void lebao_set_list_empty_info(MMI_CTRL_ID_T listId, const int index);
void lebao_add_list_customize_item(MMI_CTRL_ID_T listId, const int index);
void lebao_replace_list_customize_item(MMI_CTRL_ID_T listId, const int index);

void lebao_add_menu_list_item(MMI_CTRL_ID_T ctrl_id, const int index, MMI_IMAGE_ID_T image_id, const char* utf8txt, const uint32 data);
int lebao_convert_list_item_text(wchar* buffer, const int bufferLen, const char * text, const int textLen);

void lebao_set_artist_lable(MMI_CTRL_ID_T ctrl_id, const char * text);
void lebao_set_song_name_lable(MMI_CTRL_ID_T ctrl_id, const char * text);

void lebao_set_lable_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text);
void lebao_set_lable_text(MMI_CTRL_ID_T ctrl_id, const char * text);
//void lebao_set_lable_gbk_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text);

void lebao_set_text_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text);
//void lebao_set_text_gbk_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text);

void lebao_show_anim_image(MMI_CTRL_ID_T ctrl_id, const char* filename, GUI_RECT_T* img_rect);
void lebao_show_anim_image_by_id(MMI_CTRL_ID_T ctrl_id, const int imgId, GUI_RECT_T* img_rect);
void lebao_show_anim_image_buffer(MMI_CTRL_ID_T ctrl_id, const char* buffer, const int bufferSize, GUI_RECT_T* img_rect);

// msgbox
void lebao_create_waitingbox(const int textId, const int ms);
void lebao_close_waitingbox(void);

void lebao_create_msgbox(const char* title, const int ms);
void lebao_create_msgbox_id(const uint32 textId, const int ms);
void lebao_create_msgbox_resid(const int index, const int ms);
//void lebao_create_msgbox_gbk(const char* title, const int ms);
void lebao_create_msgbox2(const char* content, const char* content2, const int ms);
void lebao_close_msgbox();

/**********************
 *      MACROS
 **********************/

#define LEBAO_ID_TEXT(index)	lebao_get_text(index)
#define LEBAO_ID_IMG(index)		lebao_get_res_image_id(index)
#define LEBAO_ID_CTRL(index)	lebao_get_res_ctrl_id(index)
#define LEBAO_ID_WIN(index)		lebao_get_res_win_id(index)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LEBAO_BASE_H*/

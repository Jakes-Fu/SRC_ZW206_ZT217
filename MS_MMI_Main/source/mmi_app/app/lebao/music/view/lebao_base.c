/**********************
 *      INCLUDES
 *********************/
#include "port_cfg.h"
#include "std_header.h"
#include "lebao_style.h"
#include "lebao_base.h"
#include "lebao_misc.h"

#include "lebao_id.h"
#include "lebao_image.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
	LEBAO_CHARSET_GBK = 0,
	LEBAO_CHARSET_UTF8,
} lebao_charset_type_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int	_lebao_convert_to_wstring(const char * text, const int charsetType, wchar*  buffer, const int bufLen);
static int  _lebao_convert_to_utf8(const wchar * text, const int textLen, char*  buffer, const int bufLen);
static void	_lebao_show_anim_image(MMI_CTRL_ID_T ctrl_id, const int imgId, const char* filename, GUI_RECT_T* img_rect, const BOOLEAN isZoom);
static void	_lebao_set_lable_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text, const int charsetType);
static void	_lebao_set_text_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text, const int charsetType);
static void	_lebao_create_msgbox(const uint32 textId, const char* title, const int ms, const int charsetType);
/**********************
 *  STATIC VARIABLES
 **********************/
static struct
{
	int index;
	uint32 resId;
} _resImgId[] = {
	{ 0, 0 }, // RES_IMAGE_LEBAO_BEGIN
	{ RES_IMAGE_LEBAO_LOGO,				IMAGE_LEBAO_LOGO },
	{ RES_IMAGE_LEBAO_SPLASH,			IMAGE_LEBAO_SPLASH },
	{ RES_IMAGE_LEBAO_MENU_CHATS,		IMAGE_LEBAO_MENU_CHATS },
	{ RES_IMAGE_LEBAO_MENU_SEARCH,		IMAGE_LEBAO_MENU_SEARCH },
	{ RES_IMAGE_LEBAO_MENU_LOCAL,		IMAGE_LEBAO_MENU_LOCAL },
	{ RES_IMAGE_LEBAO_MENU_ORDER,		IMAGE_LEBAO_MENU_ORDER },
	{ RES_IMAGE_LEBAO_MENU_SETTING,		IMAGE_LEBAO_MENU_SETTING },
	{ RES_IMAGE_LEBAO_MENU_CONTACT,		IMAGE_LEBAO_MENU_CONTACT },
	{ RES_IMAGE_LEBAO_MENU_ABOUT,		IMAGE_LEBAO_MENU_ABOUT },
	{ RES_IMAGE_LEBAO_BTN_BACK,			IMAGE_LEBAO_BTN_BACK },
	{ RES_IMAGE_LEBAO_BTN_CLOSE,		IMAGE_LEBAO_BTN_CLOSE },
	{ RES_IMAGE_LEBAO_BTN_VOLUME,		IMAGE_LEBAO_BTN_VOLUME },
	{ RES_IMAGE_LEBAO_BTN_DELETE,		IMAGE_LEBAO_BTN_DELETE },
	{ RES_IMAGE_LEBAO_BTN_FAVORITE,		IMAGE_LEBAO_BTN_FAVORITE },
	{ RES_IMAGE_LEBAO_BTN_FAVORITE_SEL,	IMAGE_LEBAO_BTN_FAVORITE_SEL },
	{ RES_IMAGE_LEBAO_BTN_NEXT,			IMAGE_LEBAO_BTN_NEXT },
	{ RES_IMAGE_LEBAO_BTN_PREVIOU,		IMAGE_LEBAO_BTN_PREVIOU },
	{ RES_IMAGE_LEBAO_BTN_RING,			IMAGE_LEBAO_BTN_RING },
	{ RES_IMAGE_LEBAO_BTN_RING_SEL,		IMAGE_LEBAO_BTN_RING_SEL },
	{ RES_IMAGE_LEBAO_BTN_PLAY,			IMAGE_LEBAO_BTN_PLAY },
	{ RES_IMAGE_LEBAO_BTN_PAUSE,		IMAGE_LEBAO_BTN_PAUSE },
	{ RES_IMAGE_LEBAO_BTN_VOL_ADD,		IMAGE_LEBAO_BTN_VOL_ADD },
	{ RES_IMAGE_LEBAO_BTN_VOL_SUB,		IMAGE_LEBAO_BTN_VOL_SUB },
	{ RES_IMAGE_LEBAO_ITEM_D1,			IMAGE_LEBAO_ITEM_D1 },
	{ RES_IMAGE_LEBAO_ITEM_D2,			IMAGE_LEBAO_ITEM_D2 },
	{ RES_IMAGE_LEBAO_ITEM_D3,			IMAGE_LEBAO_ITEM_D3 },
	{ RES_IMAGE_LEBAO_ITEM_D4,			IMAGE_LEBAO_ITEM_D4 },
	{ RES_IMAGE_LEBAO_BTN_DEFAULT,		IMAGE_LEBAO_BTN_DEFAULT },
	{ RES_IMAGE_LEBAO_RECORD,			IMAGE_LEBAO_RECORD },
	{ RES_IMAGE_LEBAO_RECORD_GRAY,		IMAGE_LEBAO_RECORD_GRAY },
	{ RES_IMAGE_LEBAO_BTN_STAR,			IMAGE_LEBAO_BTN_STAR },
	{ RES_IMAGE_LEBAO_LINE,				IMAGE_LEBAO_LINE },
	{ RES_IMAGE_LEBAO_VIP,				IMAGE_LEBAO_VIP },
	{ RES_IMAGE_LEBAO_INDICATOR,		IMAGE_LEBAO_INDICATOR },
	{ RES_IMAGE_LEBAO_LOADING,			IMAGE_LEBAO_LOADING },
	{ 0, 0 } // RES_IMAGE_LEBAO_END
};

static struct
{
	int index;
	uint32 resId;
} _resCtrlId[] = {
	{ 0, 0 }, // RES_MMI_LEBAO_CTRL_ID_START

	// menu
	{ RES_MMI_LEBAO_CTRL_ID_MENU_OPTION,		MMI_LEBAO_CTRL_ID_MENU_OPTION },

	// list
	{ RES_MMI_LEBAO_CTRL_ID_LIST_MENU,			MMI_LEBAO_CTRL_ID_LIST_MENU },
	{ RES_MMI_LEBAO_CTRL_ID_LIST_CHARTS,		MMI_LEBAO_CTRL_ID_LIST_CHARTS },
	{ RES_MMI_LEBAO_CTRL_ID_LIST_MUSICLIST,		MMI_LEBAO_CTRL_ID_LIST_MUSICLIST },
	{ RES_MMI_LEBAO_CTRL_ID_LIST_PLAYER,		MMI_LEBAO_CTRL_ID_LIST_PLAYER },
	{ RES_MMI_LEBAO_CTRL_ID_LIST_LOCAL,			MMI_LEBAO_CTRL_ID_LIST_LOCAL },
	{ RES_MMI_LEBAO_CTRL_ID_LIST_ABOUT,			MMI_LEBAO_CTRL_ID_LIST_ABOUT },

	// progress
	{ RES_MMI_LEBAO_CTRL_ID_PGB_MUSIC,			MMI_LEBAO_CTRL_ID_PGB_MUSIC },

	// button
	{ RES_MMI_LEBAO_CTRL_ID_BTN_VOLUME,			MMI_LEBAO_CTRL_ID_BTN_VOLUME },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_BACK,			MMI_LEBAO_CTRL_ID_BTN_BACK },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_PLAY,			MMI_LEBAO_CTRL_ID_BTN_PLAY },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_PAUSE,			MMI_LEBAO_CTRL_ID_BTN_PAUSE },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_NEXT,			MMI_LEBAO_CTRL_ID_BTN_NEXT },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_PREV,			MMI_LEBAO_CTRL_ID_BTN_PREV },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_RING,			MMI_LEBAO_CTRL_ID_BTN_RING },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_DEL,			MMI_LEBAO_CTRL_ID_BTN_DEL },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_VOL_ADD,		MMI_LEBAO_CTRL_ID_BTN_VOL_ADD },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_VOL_SUB,		MMI_LEBAO_CTRL_ID_BTN_VOL_SUB },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_RECORD,			MMI_LEBAO_CTRL_ID_BTN_RECORD },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_FAVORITE,		MMI_LEBAO_CTRL_ID_BTN_FAVORITE },
	{ RES_MMI_LEBAO_CTRL_ID_BTN_CALL,			MMI_LEBAO_CTRL_ID_BTN_CALL },

	// image
	{ RES_MMI_LEBAO_CTRL_ID_IMG_QRCODE,			MMI_LEBAO_CTRL_ID_IMG_QRCODE },
	{ RES_MMI_LEBAO_CTRL_ID_IMG_RECORD,			MMI_LEBAO_CTRL_ID_IMG_RECORD },
	{ RES_MMI_LEBAO_CTRL_ID_IMG_LOADING,		MMI_LEBAO_CTRL_ID_IMG_LOADING },
	{ RES_MMI_LEBAO_CTRL_ID_IMG_VIP,			MMI_LEBAO_CTRL_ID_IMG_VIP },
	{ RES_MMI_LEBAO_CTRL_ID_IMG_LOGO,			MMI_LEBAO_CTRL_ID_IMG_LOGO },

	// editbox
	{ RES_MMI_LEBAO_CTRL_ID_EDIT_MOBILE,		MMI_LEBAO_CTRL_ID_EDIT_MOBILE },
	{ RES_MMI_LEBAO_CTRL_ID_EDIT_CAPTCHA,		MMI_LEBAO_CTRL_ID_EDIT_CAPTCHA },
	{ RES_MMI_LEBAO_CTRL_ID_EDIT_USERNAME,		MMI_LEBAO_CTRL_ID_EDIT_USERNAME },
	{ RES_MMI_LEBAO_CTRL_ID_EDIT_PASSWORD,		MMI_LEBAO_CTRL_ID_EDIT_PASSWORD },
	{ RES_MMI_LEBAO_CTRL_ID_EDIT_CONTENT,		MMI_LEBAO_CTRL_ID_EDIT_CONTENT },

	// text
	{ RES_MMI_LEBAO_CTRL_ID_TEXT_CONFIRM,		MMI_LEBAO_CTRL_ID_TEXT_CONFIRM },

	// label
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_WELCOME,		MMI_LEBAO_CTRL_ID_LABEL_WELCOME },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE,		MMI_LEBAO_CTRL_ID_LABEL_TITLE },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_SCAN,			MMI_LEBAO_CTRL_ID_LABEL_SCAN },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_VOLUME,		MMI_LEBAO_CTRL_ID_LABEL_VOLUME },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_TIMER,		MMI_LEBAO_CTRL_ID_LABEL_TIMER },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_SONG_NAME,	MMI_LEBAO_CTRL_ID_LABEL_SONG_NAME },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_SINGER_NAME,	MMI_LEBAO_CTRL_ID_LABEL_SINGER_NAME },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_TIPS,			MMI_LEBAO_CTRL_ID_LABEL_TIPS },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_SCAN_DESC,	MMI_LEBAO_CTRL_ID_LABEL_SCAN_DESC },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_VOLUME_TITLE,	MMI_LEBAO_CTRL_ID_LABEL_VOLUME_TITLE },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_CURRENT_TIME,	MMI_LEBAO_CTRL_ID_LABEL_CURRENT_TIME },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_TOTAL_TIME,	MMI_LEBAO_CTRL_ID_LABEL_TOTAL_TIME },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_MOBILE,		MMI_LEBAO_CTRL_ID_LABEL_MOBILE },
	{ RES_MMI_LEBAO_CTRL_ID_LABEL_CAPTCHA,		MMI_LEBAO_CTRL_ID_LABEL_CAPTCHA },

	{ 0, 0 } // RES_MMI_LEBAO_CTRL_ID_MAX
};

static struct
{
	int index;
	uint32 resId;
} _resWinId[] = {
	{ 0, 0 }, // RES_MMI_LEBAO_WIN_ID_START
	{ RES_MMI_LEBAO_WIN_ID_SPLASH,		MMI_LEBAO_WIN_ID_SPLASH },
	{ RES_MMI_LEBAO_WIN_ID_MAINMENU,	MMI_LEBAO_WIN_ID_MAINMENU },
	{ RES_MMI_LEBAO_WIN_ID_CHARTS,		MMI_LEBAO_WIN_ID_CHARTS },
	{ RES_MMI_LEBAO_WIN_ID_MUSICLIST,	MMI_LEBAO_WIN_ID_MUSICLIST },
	{ RES_MMI_LEBAO_WIN_ID_PLAYER,		MMI_LEBAO_WIN_ID_PLAYER },
	{ RES_MMI_LEBAO_WIN_ID_VOLUME,		MMI_LEBAO_WIN_ID_VOLUME },
	{ RES_MMI_LEBAO_WIN_ID_SEARCH,		MMI_LEBAO_WIN_ID_SEARCH },
	{ RES_MMI_LEBAO_WIN_ID_ORDER,		MMI_LEBAO_WIN_ID_ORDER },
	{ RES_MMI_LEBAO_WIN_ID_LOCAL,		MMI_LEBAO_WIN_ID_LOCAL },
	{ RES_MMI_LEBAO_WIN_ID_WAITING,		MMI_LEBAO_WIN_ID_WAITING },
	{ RES_MMI_LEBAO_WIN_ID_ABOUT,		MMI_LEBAO_WIN_ID_ABOUT },
	{ RES_MMI_LEBAO_WIN_ID_CONFIRM,		MMI_LEBAO_WIN_ID_CONFIRM },
	{ RES_MMI_LEBAO_WIN_ID_OPTION,		MMI_LEBAO_WIN_ID_OPTION },
	{ RES_MMI_LEBAO_WIN_ID_VIP,			MMI_LEBAO_WIN_ID_VIP },
	{ RES_MMI_LEBAO_WIN_ID_LOGIN,		MMI_LEBAO_WIN_ID_LOGIN },
	{ RES_MMI_LEBAO_WIN_ID_CONTACT,		MMI_LEBAO_WIN_ID_CONTACT },
	{ RES_MMI_LEBAO_WIN_ID_SUB_CHART,	MMI_LEBAO_WIN_ID_SUB_CHART },
	{ RES_MMI_LEBAO_WIN_ID_SONGLIST,	MMI_LEBAO_WIN_ID_SONGLIST },	
	{ 0, 0 } // RES_MMI_LEBAO_WIN_ID_MAX
};

// utf-8
#include "lebao_lang.def"

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

// for test
void lebao_create_circle(void)
{
#if defined(_WIN32)
	if (lebao_is_round_watch()) {
		GUI_RECT_T rect = lebao_get_full_screen_rect();
		GUI_LCD_DEV_INFO lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };

		LCD_DrawCircle(&lcd_dev_info, &rect, LEBAO_SCREEN_WIDTH / 2, LEBAO_SCREEN_HEIGHT / 2, LEBAO_SCREEN_WIDTH / 2, GUI_RGB2RGB565(0xff, 0xa0, 0x00));
	}
#endif
}

const char* lebao_get_text(const int index)
{
    if (index > TEXT_LEBAO_BEGIN && index < TEXT_LEBAO_END) {
//        if (MMISET_GetCurrentLanguageType() == 1)
//            return s_lebao_text_id_en[index];
//        else
            return s_lebao_text_id[index];
    }
    else
        return "unknown";
}

void lebao_set_res_image_id(const uint32 begin, const uint32 end)
{
	uint32 i = RES_IMAGE_LEBAO_LOGO;
	uint32 start = begin;
	helper_debug_printf("start=%d, end=%d\n", start, end);

	if (start <= 0 || end <= 0 || start > end)
		return;

	do {
		_resImgId[i].resId = start;

		++i;
		++start;
	} while (_resImgId[i].index != 0 && start <= end);
}

void lebao_set_res_ctrl_id(const uint32 begin, const uint32 end)
{
	uint32 i = RES_MMI_LEBAO_CTRL_ID_MENU_OPTION;
	uint32 start = begin;
	helper_debug_printf("start=%d, end=%d\n", start, end);

	if (start <= 0 || end <= 0 || start > end)
		return;

	do {
		_resCtrlId[i].resId = start;

		++i;
		++start;
	} while (_resCtrlId[i].index != 0 && start <= end);
}

void lebao_set_res_win_id(const uint32 begin, const uint32 end)
{
	uint32 i = RES_MMI_LEBAO_WIN_ID_SPLASH;
	uint32 start = begin;
	helper_debug_printf("start=%d, end=%d\n", start, end);

	if (start <= 0 || end <= 0 || start > end)
		return;

	do {
		_resWinId[i].resId = start;

		++i;
		++start;
	} while (_resWinId[i].index != 0 && start <= end);
}

uint32 lebao_get_res_image_id(const int index)
{
	if (index > RES_IMAGE_LEBAO_BEGIN && index < RES_IMAGE_LEBAO_END)
		return _resImgId[index].resId;

	return 0;
}

uint32 lebao_get_res_ctrl_id(const int index)
{
	if (index > RES_MMI_LEBAO_CTRL_ID_START && index < RES_MMI_LEBAO_CTRL_ID_MAX)
		return _resCtrlId[index].resId;

	return 0;
}

uint32 lebao_get_res_win_id(const int index)
{
	if (index > RES_MMI_LEBAO_WIN_ID_START && index < RES_MMI_LEBAO_WIN_ID_MAX)
		return _resWinId[index].resId;

	return 0;
}

void lebao_set_default_backgroud(const MMI_WIN_ID_T win_id, CAF_COLOR_T color)
{
	GUI_RECT_T bgRect = lebao_get_full_screen_rect();
	GUI_BG_T bgPtr = { 0 };
	GUI_BG_DISPLAY_T bgDisplay = { 0 };

	bgPtr.bg_type = GUI_BG_COLOR;
	bgPtr.color = color;

	bgDisplay.win_handle = MMK_ConvertIdToHandle(win_id);
	bgDisplay.display_rect = bgRect;
	bgDisplay.rect = bgRect;

	GUI_DisplayBg(&bgPtr, &bgDisplay, MMITHEME_GetDefaultLcdDev());

	lebao_create_circle();
}

void lebao_set_title_bar(MMI_CTRL_ID_T labelId, const int index)
{
	if (index != 0) {
		GUI_RECT_T rect = lebao_get_title_rect();

		GUILABEL_SetRect(labelId, &rect, TRUE);
		lebao_set_lable_font(labelId, LEBAO_NORMAL_FONT, LEBAO_TITLE_COLOR, lebao_get_text(index));
#ifdef PLATFORM_UWS6121E
		GUILABEL_SetIsAutoScroll(labelId, TRUE);
#endif
	}
}

void lebao_set_title_bar_text(MMI_CTRL_ID_T labelId, const char * utf8Text)
{
	if (utf8Text != NULL) {
		GUI_RECT_T rect = lebao_get_title_rect();

		GUILABEL_SetRect(labelId, &rect, TRUE);
		lebao_set_lable_font(labelId, LEBAO_NORMAL_FONT, LEBAO_TITLE_COLOR, utf8Text);
    #ifdef PLATFORM_UWS6121E
		GUILABEL_SetIsAutoScroll(labelId, TRUE);
    #endif
	}
}

void lebao_set_title_bar_id(MMI_CTRL_ID_T labelId, const int resId)
{
	GUI_RECT_T rect = lebao_get_title_rect();

	GUILABEL_SetRect(labelId, &rect, TRUE);
	GUILABEL_SetFont(labelId, LEBAO_NORMAL_FONT, LEBAO_TITLE_COLOR);
	GUILABEL_SetTextById(labelId, resId, TRUE);
}

void lebao_set_list_empty_info(MMI_CTRL_ID_T listId, const int index)
{
	GUILIST_EMPTY_INFO_T empty_info = { 0 };
	MMI_STRING_T	buffer = { 0 };
	wchar			w_text[255 + 1] = { 0 };

	buffer.wstr_ptr = w_text;
	buffer.wstr_len = _lebao_convert_to_wstring(lebao_get_text(index), LEBAO_CHARSET_UTF8, w_text, 255);
	empty_info.text_buffer = buffer;
	GUILIST_SetEmptyInfo(listId, &empty_info);
	GUILIST_SetBgColor(listId, LEBAO_BACKGROUD_COLOR);
}

void lebao_add_list_customize_item(MMI_CTRL_ID_T listId, const int index)
{
	GUILIST_ITEM_T			item_t = { 0 };

	item_t.item_style = GUIITEM_STYLE_CUSTOMIZE;
	item_t.user_data = index;

	GUILIST_AppendItem(listId, &item_t);
}

void lebao_replace_list_customize_item(MMI_CTRL_ID_T listId, const int index)
{
	GUILIST_ITEM_T			item_t = { 0 };

	item_t.item_style = GUIITEM_STYLE_CUSTOMIZE;
	item_t.user_data = index;

	GUILIST_ReplaceItem(listId, &item_t, index);
}

void lebao_add_menu_list_item(MMI_CTRL_ID_T ctrl_id, const int index, MMI_IMAGE_ID_T image_id, const char* utf8txt, const uint32 data)
{
	GUILIST_ITEM_T          item_t = { 0 };
	GUILIST_ITEM_DATA_T     item_data = { 0 };
	uint32                  item_index = 0;
	wchar 					w_text[255 + 1] = { 0 };

	item_t.item_style = GUIITEM_STYLE_CUSTOMIZE;
	item_t.item_data_ptr = &item_data;

	item_data.item_content[0].item_data_type = GUIITEM_DATA_IMAGE_ID;
	item_data.item_content[0].item_data.image_id = image_id;

	item_data.item_content[1].item_data_type = GUIITEM_DATA_TEXT_BUFFER;
	item_data.item_content[1].item_data.text_buffer.wstr_ptr = w_text;
	item_data.item_content[1].item_data.text_buffer.wstr_len = _lebao_convert_to_wstring(utf8txt, LEBAO_CHARSET_UTF8, w_text, 255);

	item_t.user_data = data;

	GUILIST_AppendItem(ctrl_id, &item_t);
	lebao_set_menu_list_style(ctrl_id, index);
}

int lebao_convert_list_item_text(wchar* buffer, const int bufferLen, const char * text, const int textLen)
{
	if (text != NULL && textLen > 0 && buffer !=  NULL && bufferLen > 0) {
		uint16 len = GUI_UTF8ToWstr(buffer, bufferLen, (const uint8*)text, textLen);
		return len;
	}
	return 0;
}

void lebao_set_artist_lable(MMI_CTRL_ID_T ctrl_id, const char * text)
{
	lebao_set_lable_font(ctrl_id, lebao_get_big_font(), GUI_RGB2RGB565(126, 126, 126), text);
}

void lebao_set_song_name_lable(MMI_CTRL_ID_T ctrl_id, const char * text)
{
	lebao_set_lable_font(ctrl_id, lebao_get_big_font(), GUI_RGB2RGB565(0xff, 0xa0, 0x00), text);
}

void lebao_set_lable_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text) 
{
	_lebao_set_lable_font(ctrl_id, font, font_color, text, LEBAO_CHARSET_UTF8);
}

void lebao_set_lable_text(MMI_CTRL_ID_T ctrl_id, const char * text)
{
	MMI_STRING_T	buffer = { 0 };
	wchar			w_text[255 + 1] = { 0 };

	buffer.wstr_ptr = w_text;
	buffer.wstr_len = _lebao_convert_to_wstring(text, LEBAO_CHARSET_UTF8, w_text, 255);
	GUILABEL_SetText(ctrl_id, &buffer,TRUE);
}

/*
void lebao_set_lable_gbk_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text) 
{
	_lebao_set_lable_font(ctrl_id, font, font_color, text, LEBAO_CHARSET_GBK);
}
*/

void lebao_set_text_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text)
{
	_lebao_set_text_font(ctrl_id, font, font_color, text, LEBAO_CHARSET_UTF8);
}

/*
void lebao_set_text_gbk_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text)
{
	_lebao_set_text_font(ctrl_id, font, font_color, text, LEBAO_CHARSET_GBK);
}
*/

void lebao_show_anim_image(MMI_CTRL_ID_T ctrl_id, const char* filename, GUI_RECT_T* img_rect)
{
	_lebao_show_anim_image(ctrl_id, 0, filename, img_rect, TRUE);
}

void lebao_show_anim_image_by_id(MMI_CTRL_ID_T ctrl_id, const int imgId, GUI_RECT_T* img_rect)
{
	_lebao_show_anim_image(ctrl_id, imgId, NULL, img_rect, FALSE);
}

void lebao_show_anim_image_buffer(MMI_CTRL_ID_T ctrl_id, const char* buffer, const int bufferSize, GUI_RECT_T* img_rect)
{
	GUIANIM_CTRL_INFO_T     ctrlInfo = { 0 };
	GUIANIM_DISPLAY_INFO_T  dispInfo = { 0 };

	ctrlInfo.is_ctrl_id = TRUE;
	ctrlInfo.ctrl_id = ctrl_id;

	dispInfo.align_style = GUIANIM_ALIGN_HVMIDDLE;
	dispInfo.is_zoom = TRUE;
	dispInfo.is_auto_zoom_in = FALSE;
	dispInfo.res_bg_color = MMI_WINDOW_BACKGROUND_COLOR; // MMI_WHITE_COLOR;
	dispInfo.specify_width = img_rect->right - img_rect->left;
	dispInfo.specify_height = img_rect->bottom - img_rect->top;
	dispInfo.is_disp_frame = FALSE;

	GUIANIM_SetCtrlRect(ctrl_id, img_rect);
	{
		GUIANIM_DATA_INFO_T		dataInfo = { 0 };

		os_memset(&dataInfo, 0, sizeof(dataInfo));
		dataInfo.data_ptr = buffer;
		dataInfo.data_size = bufferSize;
		
		GUIANIM_SetParam(&ctrlInfo, &dataInfo, NULL, &dispInfo);
	}
}

void lebao_create_waitingbox(const int index, const int ms) 
{
	lebao_close_waitingbox();
	_lebao_create_msgbox(index, NULL, ms, LEBAO_CHARSET_UTF8);
}

void lebao_close_waitingbox(void)
{
	lebao_close_msgbox();
}

void lebao_create_msgbox(const char* title, const int ms)
{
	lebao_close_msgbox();
	_lebao_create_msgbox(0, title, ms, LEBAO_CHARSET_UTF8);
}

// index: not the text resid
void lebao_create_msgbox_id(const uint32 index, const int ms)
{
	lebao_close_msgbox();
	_lebao_create_msgbox(index, NULL, ms, LEBAO_CHARSET_UTF8);
}

void lebao_create_msgbox_resid(const int resId, const int ms)
{
	MMI_STRING_T text_buffer = { 0 };

	lebao_close_msgbox();
	if (resId != 0) {
	    MMI_GetLabelTextByLang(resId, &text_buffer);
	}
	lebao_tips_open( &text_buffer, ms, NULL);
}

/*
void lebao_create_msgbox_gbk(const char* title, const int ms)
{
	lebao_close_msgbox();
	_lebao_create_msgbox(0, title, ms, LEBAO_CHARSET_GBK);
}
*/

void lebao_create_msgbox2(const char* content, const char* content2, const int ms)
{
	if (content != NULL && content2 != NULL ) {
		char msg[256] = {0};
		int contentLen = os_strlen(content);
		int contentLen2 = os_strlen(content2);

		os_strncat(msg, content, contentLen < 128 ? contentLen : 128);
		os_strncat(msg, content2, contentLen2 < 96 ? contentLen2 : 96);
		lebao_create_msgbox(msg, ms);
	}
}

void lebao_close_msgbox(void)
{
	lebao_waiting_close(NULL);
	lebao_tips_close();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _lebao_show_anim_image(MMI_CTRL_ID_T ctrl_id, const int imgId, const char* filename, GUI_RECT_T* img_rect, const BOOLEAN isZoom)
{
	GUIANIM_CTRL_INFO_T     ctrlInfo = { 0 };
	GUIANIM_DISPLAY_INFO_T  dispInfo = { 0 };

	ctrlInfo.is_ctrl_id = TRUE;
	ctrlInfo.ctrl_id = ctrl_id;

	dispInfo.align_style = GUIANIM_ALIGN_HVMIDDLE;
	dispInfo.is_zoom = isZoom;
	dispInfo.is_auto_zoom_in = TRUE;
	dispInfo.res_bg_color = MMI_TRANSPARENCE_COLOR; // MMI_WINDOW_BACKGROUND_COLOR; // MMI_WHITE_COLOR;
	dispInfo.specify_width = img_rect->right - img_rect->left;
	dispInfo.specify_height = img_rect->bottom - img_rect->top;
	dispInfo.is_disp_frame = FALSE;
	
	dispInfo.is_bg_buf = TRUE;
	dispInfo.bg.bg_type = GUI_BG_COLOR;
	dispInfo.bg.color = LEBAO_BACKGROUD_COLOR; // MMI_WINDOW_BACKGROUND_COLOR;

	GUIANIM_SetCtrlRect(ctrl_id, img_rect);
	if (filename != NULL) {
		GUIANIM_FILE_INFO_T     fileInfo = { 0 };
		wchar					w_filename[255 + 1] = { 0 };

		fileInfo.full_path_wstr_len = GUI_UTF8ToWstr(w_filename, 255, (const uint8*)filename, os_strlen(filename));
		fileInfo.full_path_wstr_ptr = w_filename;
		GUIANIM_SetParam(&ctrlInfo, NULL, &fileInfo, &dispInfo);
	}
	else {
		GUIANIM_DATA_INFO_T		dataInfo = { 0 };

		os_memset(&dataInfo, 0, sizeof(dataInfo));
		dataInfo.img_id = imgId;
		GUIANIM_SetParam(&ctrlInfo, &dataInfo, NULL, &dispInfo);
	}
}

static int _lebao_convert_to_wstring(const char * text, const int charsetType, wchar*  buffer, const int bufLen)
{
	uint16			w_len = 0;
	uint16			len = os_strlen((text == NULL) ? "" : text);

	if (len <= 0 || buffer == NULL || bufLen <= 0)
		return 0;

	len = (len > bufLen) ? bufLen : len;

	// charsetType: 0 GBk, 1 utf8
	if (charsetType == 1)
		w_len = GUI_UTF8ToWstr(buffer, bufLen, (const uint8*)text, len);
	else
		w_len = GUI_GBToWstr(buffer, (const uint8*)text, len);

	return w_len;
}

static int _lebao_convert_to_utf8(const wchar * text, const int textLen, char*  buffer, const int bufLen)
{
	if (text == NULL || textLen <= 0 || buffer == NULL || bufLen <= 0)
		return 0;

	return GUI_WstrToUTF8(buffer, bufLen, text, textLen);
}

static void _lebao_set_lable_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text, const int charsetType)
{
	MMI_STRING_T	buffer = { 0 };
	wchar			w_text[255 + 1] = { 0 };

	buffer.wstr_ptr = w_text;
	buffer.wstr_len = _lebao_convert_to_wstring(text, charsetType, w_text, 255);

	GUILABEL_SetFont(ctrl_id, font, font_color);
	GUILABEL_SetText(
		ctrl_id, 
		&buffer,
		TRUE);
}

static void _lebao_set_text_font(MMI_CTRL_ID_T ctrl_id, GUI_FONT_T font, GUI_COLOR_T font_color, const char * text, const int charsetType)
{
	MMI_STRING_T	buffer = { 0 };
	wchar			w_text[1023 + 1] = { 0 };

	buffer.wstr_ptr = w_text;
	buffer.wstr_len = _lebao_convert_to_wstring(text, charsetType, w_text, 1023);

	GUITEXT_SetFont(ctrl_id, &font, &font_color);
	GUITEXT_SetString(
		ctrl_id,
		buffer.wstr_ptr,
		buffer.wstr_len,
		TRUE);
}

static void _lebao_create_msgbox(const uint32 index, const char* title, const int ms, const int charsetType)
{
	MMI_STRING_T text_buffer = { 0 };
	wchar w_text[255 + 1] = { 0 };
	char const* tips = title;

	if (index != 0) {
		tips = lebao_get_text(index);
	}

	text_buffer.wstr_ptr = w_text;
	text_buffer.wstr_len = _lebao_convert_to_wstring(tips, charsetType, w_text, 255);

	lebao_tips_open( &text_buffer, ms, NULL);
}


/**********************
 *      INCLUDES
 *********************/
#include "port_cfg.h"
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

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

// theme
// 分辨率不同时，图片资源不同，有128/240/360三套，需要选择合适的
#define	LEBAO_SCREEN_WIDTH_MAX			240 // MMI_MAINSCREEN_WIDTH
#define	LEBAO_SCREEN_HEIGHT_MAX			320 // MMI_MAINSCREEN_HEIGHT

// 圆表需要打开MAINLCD_CIRCULAR_WATCH开关
#ifndef MAINLCD_CIRCULAR_WATCH
 //#define MAINLCD_CIRCULAR_WATCH
#endif

//圆角方屏需要打开MAINLCD_ROUNDED_RECTANGLE_WATCH开关
#ifndef MAINLCD_ROUNDED_RECTANGLE_WATCH
// #define MAINLCD_ROUNDED_RECTANGLE_WATCH
#endif

// 水滴方屏，需修改rect.top

#if (LEBAO_SCREEN_WIDTH_MAX >= 480 && LEBAO_SCREEN_HEIGHT_MAX >= 480)
	// 480 x 480, 480 x 640
	#define LEBAO_SIZE_480
#elif (LEBAO_SCREEN_WIDTH_MAX > 240 && LEBAO_SCREEN_HEIGHT_MAX > 240)
	// 360 x 360, 320 X 360
	#define LEBAO_SIZE_360
#elif (LEBAO_SCREEN_WIDTH_MAX <= 128)
	// 128 x 128, 128 x 160
	#define LEBAO_SIZE_128
#else
	// 170 x 320, 172 x 320, 200 x 320, 240 x 240, 240 x 280, 240 X 320, 320 x 170, 480 x 200
	#define LEBAO_SIZE_240
#endif

// 圆屏
#ifdef MAINLCD_CIRCULAR_WATCH
	static int _isRoundWatch = 1;
	#if defined(LEBAO_SIZE_480)
		#define LEBAO_ROUND_PADDING_		(40)
	#elif defined(LEBAO_SIZE_320)
		#define LEBAO_ROUND_PADDING_		(30)
	#elif defined(LEBAO_SIZE_128)
  		#define LEBAO_ROUND_PADDING_		(8)
	#else
  		#define LEBAO_ROUND_PADDING_		(20)
	#endif
#else
	static int _isRoundWatch = 0;
	#define LEBAO_ROUND_PADDING_		    (0)
#endif

//圆角方屏
#ifdef MAINLCD_ROUNDED_RECTANGLE_WATCH
	#define LEBAO_ROUND_PADDING_		    (20)
#endif

static int _labelCanRoll = 0;

#if defined(LEBAO_SIZE_480)
	#define  LEBAO_TITLE_HEIGHT_		   (50)
	#define  LEBAO_LABEL_HEIGHT_		   (40)
	#include "lebao_style_480.def"
#elif defined(LEBAO_SIZE_360)
	#define  LEBAO_TITLE_HEIGHT_			(50)
	#define  LEBAO_LABEL_HEIGHT_			(40)
	#include "lebao_style_360.def"
#elif defined(LEBAO_SIZE_128)
	#define  LEBAO_TITLE_HEIGHT_			(20)
	#define  LEBAO_LABEL_HEIGHT_			(16)
	#include "lebao_style_128.def"
#else
	#define  LEBAO_TITLE_HEIGHT_			(36)
	#define  LEBAO_LABEL_HEIGHT_			(30)
	#include "lebao_style_240.def"
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lebao_set_round_watch(void)
{
#ifndef MAINLCD_CIRCULAR_WATCH
	_isRoundWatch = 1;
#endif
}

int lebao_is_round_watch(void)
{
#ifdef MAINLCD_CIRCULAR_WATCH
	return 1;
#else
	return _isRoundWatch;
#endif
}

int lebao_is_round_rectangle_watch(void)
{
#ifdef MAINLCD_ROUNDED_RECTANGLE_WATCH
	return 1;
#else
	return 0;
#endif
}

int lebao_is_feature_phone(void)
{
#if defined(LEBAO_FEATURE_PHONE_STYLE)
	return 1;
#else
	return 0;
#endif
}

void lebao_set_label_can_roll(void)
{
	_labelCanRoll = 1;
}

int lebao_get_label_can_roll(void)
{
	return _labelCanRoll;
}

BOOL lebao_get_need_highlight_bar(void)
{
#if defined(LEBAO_FEATURE_PHONE_STYLE)
	return TRUE;
#else
	return FALSE;
#endif
}

int lebao_get_screen_width(void)
{
//	GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
//	return (rect.right - rect.left);
	return LEBAO_SCREEN_WIDTH_MAX;
}

int lebao_get_screen_height(void)
{
//	GUI_RECT_T rect = MMITHEME_GetFullScreenRect();
//	return (rect.bottom - rect.top);
	return LEBAO_SCREEN_HEIGHT_MAX;
}

GUI_RECT_T lebao_get_full_screen_rect(void)
{
	GUI_RECT_T rect = { 0 };
	rect.right = lebao_get_screen_width() - 1;
	rect.bottom = lebao_get_screen_height() - 1;
	return rect;
}

GUI_RECT_T lebao_get_title_rect(void)
{
	GUI_RECT_T rect = { 0 };

	rect.left = LEBAO_LEFT_PADDING + LEBAO_ROUND_PADDING * 7 / 2;
	rect.top = LEBAO_TOP_PADDING;
	rect.right = LEBAO_SCREEN_WIDTH - rect.left;
	rect.bottom = LEBAO_TITLE_HEIGHT;
	return rect;
}

int lebao_get_title_height(void)
{
	return LEBAO_TITLE_HEIGHT_;
}

int lebao_get_label_height(void)
{
	return LEBAO_LABEL_HEIGHT_;
}

void lebao_set_list_default_rect(MMI_CTRL_ID_T listId, int titleHeight)
{
	lebao_set_list_rect(listId, titleHeight, 0);
}

void lebao_set_list_rect(MMI_CTRL_ID_T listId, int titleHeight, int bottomMargin)
{
	GUI_RECT_T rect = { 0 };

	rect.left   = 0;
	rect.top    = titleHeight;
	rect.right  = LEBAO_SCREEN_WIDTH - 1;
	rect.bottom	= LEBAO_SCREEN_HEIGHT - LEBAO_SOFTKEY_BAR_HEIGHT - 1 - bottomMargin;

	GUILIST_SetRect(listId, &rect);
	GUILIST_SetListState(listId, GUILIST_STATE_CYC_SCROLL_ITEM, FALSE);
}

int lebao_get_left_padding(void)
{
	return LEBAO_LEFT_PADDING_;
}

int lebao_get_top_padding(void)
{
	return LEBAO_TOP_PADDING_;
}

int lebao_get_right_padding(void)
{
	return LEBAO_RIGHT_PADDING_;
}

int lebao_get_bottom_padding(void)
{
	return LEBAO_BOTTOM_PADDING_;
}

int lebao_get_round_padding(void)
{
	return LEBAO_ROUND_PADDING_;
}

int lebao_get_bg_color(void)
{
#if defined(LEBAO_FEATURE_PHONE_STYLE)
	return MMI_WHITE_COLOR;
#else
	return MMI_BLACK_COLOR;
#endif
}

int lebao_get_font_color(void)
{
#if defined(LEBAO_FEATURE_PHONE_STYLE)
	return MMI_BLACK_COLOR;
#else
	return MMI_WHITE_COLOR;
#endif
}

int lebao_get_big_font(void)
{
#if defined(LEBAO_SIZE_128X128)
	return MMI_DEFAULT_NORMAL_FONT;
#else
	return WATCH_DEFAULT_NORMAL_FONT;
#endif
}

int lebao_get_normal_font(void)
{
	return MMI_DEFAULT_BIG_FONT;
}

int lebao_get_small_font(void)
{
	return MMI_DEFAULT_NORMAL_FONT;
}

void lebao_set_menu_list_style(const MMI_CTRL_ID_T list_id, const int index)
{
	GUILIST_SetItemStyleEx(list_id, index, &s_item_style_1_icon_80x80_text_fixed_height_90);
}

void lebao_set_charts_list_style(const MMI_CTRL_ID_T list_id, const int index)
{
	GUILIST_SetItemStyleEx(list_id, index, &s_item_style_1_icon_80x80_text_fixed_height_90);
}

void lebao_set_charts_list_style_vip(const MMI_CTRL_ID_T list_id, const int index)
{
	GUILIST_SetItemStyleEx(list_id, index, &s_item_style_2_icon_text_fixed_height_90);
}

void lebao_set_musiclist_list_style(const MMI_CTRL_ID_T list_id, const int index)
{
	GUILIST_SetItemStyleEx(list_id, index, &s_item_style_1_icon_20x72_2_text_text_fixed_height_90);
}

void lebao_set_local_list_style(const MMI_CTRL_ID_T list_id, const int index)
{
	GUILIST_SetItemStyleEx(list_id, index, &s_item_style_1_icon_20x72_2_text_text_fixed_height_90);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
void lebao_splash_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T logoId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_IMG_LOGO);
	GUI_RECT_T logoRect = { 0 }; 
	int width = 0, height = 0;

	GUIRES_GetImgWidthHeight(&width, &height, LEBAO_ID_IMG(RES_IMAGE_LEBAO_SPLASH), win_id);
	logoRect.left = (lebao_get_screen_width() - width) / 2;
	logoRect.right = logoRect.left + width;
	logoRect.top = (lebao_get_screen_height() - height) / 2;
	logoRect.bottom = logoRect.top + height;	
	lebao_show_anim_image_by_id(logoId, LEBAO_ID_IMG(RES_IMAGE_LEBAO_SPLASH), &logoRect);
}

void lebao_menu_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T listId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LIST_MENU);

	GUILIST_SetBgColor(listId, LEBAO_BACKGROUD_COLOR);

	lebao_set_title_bar(
		LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE),
		TEXT_LEBAO_MIGU);

	lebao_set_list_default_rect(listId, LEBAO_TITLE_HEIGHT);
	GUILIST_SetListState(listId, GUILIST_STATE_NEED_HIGHTBAR, LEBAO_NEED_HIGHLIGHT_BAR);
	GUILIST_SetListState(listId, GUILIST_STATE_SPLIT_LINE, FALSE);	
}

void lebao_setting_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T listId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LIST_SETTING);

	lebao_set_title_bar(
		LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE),
		TEXT_LEBAO_MENU_SETTING);

	lebao_set_list_default_rect(listId, LEBAO_TITLE_HEIGHT);
	GUILIST_SetListState(listId, GUILIST_STATE_NEED_HIGHTBAR, LEBAO_NEED_HIGHLIGHT_BAR);
	GUILIST_SetListState(listId, GUILIST_STATE_SPLIT_LINE, FALSE);
}

void lebao_chart_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T listId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LIST_CHARTS);
	
	lebao_set_title_bar(
		LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE),
		TEXT_LEBAO_MENU_CHARTS);
	
	lebao_set_list_default_rect(listId, LEBAO_TITLE_HEIGHT);
}

void lebao_musiclist_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T listId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LIST_MUSICLIST);
	
	lebao_set_title_bar(
		LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE),
		TEXT_LEBAO_MENU_CHARTS);

	lebao_set_list_default_rect(listId, LEBAO_TITLE_HEIGHT);
}

// image size （width=height）
#if defined(LEBAO_SIZE_480)
	#define IMG_DEL_SIZE		(46)
	#define IMG_RECORD_SIZE 	(80)
	#define IMG_VOL_ADD_SIZE	(60)
	#define IMG_WAITING_GIF_WIDTH	  (120)
	#define IMG_WAITING_GIF_HEIGHT	  (50)
#elif defined(LEBAO_SIZE_360)
	#define IMG_DEL_SIZE        (46)
	#define IMG_RECORD_SIZE     (80)
	#define IMG_VOL_ADD_SIZE    (60)
	#define IMG_WAITING_GIF_WIDTH	  (120)
	#define IMG_WAITING_GIF_HEIGHT	  (50)
#elif defined(LEBAO_SIZE_128)
	#define IMG_DEL_SIZE        (18)
	#define IMG_RECORD_SIZE     (40)
	#define IMG_VOL_ADD_SIZE    (20)
	#define IMG_WAITING_GIF_WIDTH	  (60)
	#define IMG_WAITING_GIF_HEIGHT	  (25)
#else
	#define IMG_DEL_SIZE        (36)
	#define IMG_RECORD_SIZE     (80)
	#define IMG_VOL_ADD_SIZE    (40)
	#define IMG_WAITING_GIF_WIDTH     (120)
	#define IMG_WAITING_GIF_HEIGHT    (50)	
#endif

void lebao_local_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T listId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LIST_LOCAL);
	MMI_CTRL_ID_T delId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_DEL);
	GUI_RECT_T delRect = { 0 };
	int listMargin = 0;
	
	lebao_set_title_bar(
		LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE),
		TEXT_LEBAO_MENU_LOCAL);

	delRect.left = (LEBAO_SCREEN_WIDTH - IMG_DEL_SIZE) / 2;
	delRect.top = LEBAO_SCREEN_HEIGHT - IMG_DEL_SIZE;
	delRect.right = delRect.left + IMG_DEL_SIZE;
	delRect.bottom = delRect.top + IMG_DEL_SIZE;
	
	listMargin = IMG_DEL_SIZE + 5;
	
	// button of delete all
	GUIBUTTON_SetRect(delId, &delRect);
	GUIBUTTON_SetRunSheen(delId, FALSE);
	// GUIBUTTON_SetPressedBg(del_id, &);
	
	lebao_set_list_rect(listId, LEBAO_TITLE_HEIGHT, listMargin);
}

void lebao_search_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T titleId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE);
	MMI_CTRL_ID_T timerId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TIMER);
	MMI_CTRL_ID_T tipsId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TIPS);
	MMI_CTRL_ID_T recordId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_RECORD);

	GUI_RECT_T titleRect = { 0 };
	GUI_RECT_T timerRect = { 0 };
	GUI_RECT_T btnRect = { 0 };
	GUI_RECT_T tipRect = { 0 };

	GUI_BG_T btnBg = { GUI_BG_IMG, GUI_SHAPE_RECT, IMAGE_LEBAO_RECORD, 0, TRUE };
	GUI_BG_T btnBgGray = { GUI_BG_IMG, GUI_SHAPE_RECT, IMAGE_LEBAO_RECORD_GRAY, 0, TRUE };

	titleRect.left = LEBAO_LEFT_PADDING + LEBAO_ROUND_PADDING;
	titleRect.top = LEBAO_TOP_PADDING + LEBAO_ROUND_PADDING;
	titleRect.right = LEBAO_SCREEN_WIDTH - LEBAO_RIGHT_PADDING - LEBAO_ROUND_PADDING;
	titleRect.bottom = titleRect.top + LEBAO_TITLE_HEIGHT;

	timerRect.left = LEBAO_LEFT_PADDING;
	timerRect.top = titleRect.bottom + 5;
	timerRect.right = LEBAO_SCREEN_WIDTH - LEBAO_RIGHT_PADDING;
	timerRect.bottom = timerRect.top + LEBAO_TITLE_HEIGHT;

	btnRect.left = (LEBAO_SCREEN_WIDTH - IMG_RECORD_SIZE) / 2;
	btnRect.top = LEBAO_SCREEN_HEIGHT * 5 /8 - IMG_RECORD_SIZE / 2;
	btnRect.right = btnRect.left + IMG_RECORD_SIZE;
	btnRect.bottom = btnRect.top + IMG_RECORD_SIZE;

	tipRect.left = LEBAO_LEFT_PADDING;
	tipRect.top = btnRect.bottom + 5;
	tipRect.right = LEBAO_SCREEN_WIDTH - LEBAO_RIGHT_PADDING;
	tipRect.bottom = tipRect.top + LEBAO_TITLE_HEIGHT;

	btnBg.img_id = LEBAO_ID_IMG(RES_IMAGE_LEBAO_RECORD);
	btnBgGray.img_id = LEBAO_ID_IMG(RES_IMAGE_LEBAO_RECORD_GRAY);

	// 320x170, 480x200
	if(LEBAO_SCREEN_WIDTH > 240 && LEBAO_SCREEN_WIDTH > LEBAO_SCREEN_HEIGHT) {
		titleRect.top -= LEBAO_TOP_PADDING;
		titleRect.bottom = titleRect.top + LEBAO_TITLE_HEIGHT;
	
		timerRect.top = btnRect.top;
		timerRect.bottom = timerRect.top + LEBAO_TITLE_HEIGHT;
		
		timerRect.left = btnRect.right + 10;
		timerRect.right = timerRect.left + 30;
	}

	// title
	GUILABEL_SetRect(titleId, &titleRect, FALSE);
	lebao_set_lable_font(
		titleId, LEBAO_BIG_FONT, GUI_RGB2RGB565(0xff, 0xa0, 0x00), LEBAO_ID_TEXT(TEXT_LEBAO_SAY_NAME));

	// timer
	GUILABEL_SetRect(timerId, &timerRect, FALSE);
	GUILABEL_SetFont(timerId, LEBAO_NORMAL_FONT, MMI_WHITE_COLOR);

	// tip
	GUILABEL_SetRect(tipsId, &tipRect, FALSE);
	lebao_set_lable_font(
		tipsId, LEBAO_NORMAL_FONT, MMI_GRAY_WHITE_COLOR, LEBAO_ID_TEXT(TEXT_LEBAO_PUSH_TALK));

	// record
	GUIBUTTON_SetRect(recordId, &btnRect);
	GUIBUTTON_SetRunSheen(recordId, FALSE);
//	GUIBUTTON_SetHandleLong(recordId, TRUE);
	GUIBUTTON_SetFg(recordId, &btnBgGray);
	GUIBUTTON_SetPressedFg(recordId, &btnBg);
//	GUIBUTTON_SetCallBackFunc(recordId, lebao_player_btn_ring_event_cb);

//	MMK_SetAtvCtrlEx(win_id, recordId, TRUE);
	MMK_SetAtvCtrlEx(win_id, titleId, TRUE);
}

void lebao_volume_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T	volId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_VOLUME);
	MMI_CTRL_ID_T	addId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_VOL_ADD);
	MMI_CTRL_ID_T	subId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_VOL_SUB);

	GUI_RECT_T volRect = { 0 };
	GUI_RECT_T addRect = { 0 };
	GUI_RECT_T subRect = { 0 };

	volRect.left = LEBAO_SCREEN_WIDTH / 3;
	volRect.top = (LEBAO_SCREEN_HEIGHT - LEBAO_LABEL_HEIGHT) / 2;
	volRect.bottom = volRect.top + LEBAO_LABEL_HEIGHT;
	volRect.right = (LEBAO_SCREEN_WIDTH * 2)  / 3;

	addRect.left = LEBAO_SCREEN_WIDTH  / 4 - IMG_VOL_ADD_SIZE / 2;
	addRect.top = LEBAO_SCREEN_HEIGHT * 3 / 4 - IMG_VOL_ADD_SIZE / 2;
	addRect.bottom = addRect.top + IMG_VOL_ADD_SIZE;
	addRect.right = addRect.left + IMG_VOL_ADD_SIZE;

	subRect.left = LEBAO_SCREEN_WIDTH * 3 / 4 -IMG_VOL_ADD_SIZE / 2;
	subRect.top = addRect.top;
	subRect.bottom = subRect.top + IMG_VOL_ADD_SIZE;
	subRect.right = subRect.left + IMG_VOL_ADD_SIZE;

	// title
	lebao_set_title_bar(
		LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE),
		TEXT_LEBAO_SETTING_VOLUME);

	// vol
	GUILABEL_SetRect(volId, &volRect, FALSE);
	GUILABEL_SetFont(volId, LEBAO_NORMAL_FONT, LEBAO_FONT_COLOR);

	// add
	GUIBUTTON_SetRect(addId, &addRect);
	GUIBUTTON_SetRunSheen(addId, FALSE);

	// sub
	GUIBUTTON_SetRect(subId, &subRect);
	GUIBUTTON_SetRunSheen(subId, FALSE);
}

void lebao_waiting_init_control(MMI_WIN_ID_T win_id)
{
	GUI_RECT_T		rect = MMITHEME_GetFullScreenRectEx(win_id);
	GUI_RECT_T		titleRect = { 0 };
	GUI_RECT_T		timeRect = { 0 };
	GUI_RECT_T		gifRect = { 0 }; // 120 * 50, 60 * 25
	
	MMI_CTRL_ID_T tipsId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TIPS);
	MMI_CTRL_ID_T timerId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TIMER);
	MMI_CTRL_ID_T gifId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_IMG_LOADING);
	
	gifRect.left = (LEBAO_SCREEN_WIDTH - IMG_WAITING_GIF_WIDTH) / 2;
	gifRect.top = (LEBAO_SCREEN_HEIGHT * 3 / 4) ;
	gifRect.right = gifRect.left + IMG_WAITING_GIF_WIDTH;
	gifRect.bottom = gifRect.top + IMG_WAITING_GIF_HEIGHT;
	
	lebao_set_title_bar(
		LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE),
		TEXT_LEBAO_WAITING);
	
	lebao_show_anim_image_by_id(gifId, LEBAO_ID_IMG(RES_IMAGE_LEBAO_LOADING), &gifRect);
	
	titleRect.left = rect.left;
	titleRect.top = rect.bottom / 5;
	titleRect.right = rect.right;
	titleRect.bottom = titleRect.top + LEBAO_TITLE_HEIGHT;
	
	GUILABEL_SetRect(tipsId, &titleRect, FALSE);
	lebao_set_lable_font(tipsId, LEBAO_SMALL_FONT, LEBAO_FONT_COLOR, LEBAO_ID_TEXT(TEXT_LEBAO_TIPS_LOADING));
	GUILABEL_SetVisible(tipsId, FALSE, TRUE);
	
	timeRect = titleRect;
	timeRect.top = titleRect.bottom + 10;
	timeRect.bottom = timeRect.top + LEBAO_TITLE_HEIGHT;
	
	GUILABEL_SetRect(timerId, &timeRect, FALSE);
	lebao_set_lable_font(timerId, LEBAO_NORMAL_FONT, LEBAO_TITLE_COLOR, "  ");
}

extern int helper_save_qr_code_bmp(const char* uri, const char* filename);
extern int helper_get_qr_code_bmp(const char* uri, char** buffer, int* size);

void lebao_order_init_control(MMI_WIN_ID_T win_id, const int pageId)
{
	MMI_CTRL_ID_T titleId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_TITLE);
	MMI_CTRL_ID_T tipId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_SCAN);
	MMI_CTRL_ID_T qrId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_IMG_QRCODE);

	GUI_RECT_T titleRect = { 0 };
	GUI_RECT_T tipRect = { 0 };

	titleRect.left = LEBAO_LEFT_PADDING + LEBAO_ROUND_PADDING;
	titleRect.top = LEBAO_TOP_PADDING + LEBAO_ROUND_PADDING / 2;
	titleRect.right = LEBAO_SCREEN_WIDTH - LEBAO_LEFT_PADDING - LEBAO_RIGHT_PADDING - LEBAO_ROUND_PADDING;
	titleRect.bottom = titleRect.top + LEBAO_TITLE_HEIGHT;

	// title
	GUILABEL_SetRect(titleId, &titleRect, FALSE);
	if (pageId == LEBAO_PAGE_ABOUT)
		lebao_set_lable_font(
			titleId, LEBAO_NORMAL_FONT, GUI_RGB2RGB565(0xff, 0xa0, 0x00), LEBAO_ID_TEXT(TEXT_LEBAO_MENU_ABOUT));
	else if (pageId == LEBAO_PAGE_CONTACT)
		lebao_set_lable_font(
			titleId, LEBAO_NORMAL_FONT, GUI_RGB2RGB565(0xff, 0xa0, 0x00), LEBAO_ID_TEXT(TEXT_LEBAO_MENU_CONTACT));
	else	
		lebao_set_lable_font(
			titleId, LEBAO_NORMAL_FONT, GUI_RGB2RGB565(0xff, 0xa0, 0x00), LEBAO_ID_TEXT(TEXT_LEBAO_NEED_VIP));

    if (LEBAO_SCREEN_WIDTH <= 128)
      GUILABEL_SetVisible(titleId, FALSE, TRUE);

	tipRect.left = LEBAO_LEFT_PADDING + LEBAO_ROUND_PADDING * 2;
	tipRect.top = LEBAO_SCREEN_HEIGHT - LEBAO_TITLE_HEIGHT - LEBAO_ROUND_PADDING - LEBAO_BOTTOM_PADDING;
	tipRect.right = LEBAO_SCREEN_WIDTH - tipRect.left;
	tipRect.bottom = tipRect.top + LEBAO_TITLE_HEIGHT;

	// tip
	GUILABEL_SetRect(tipId, &tipRect, FALSE);
	lebao_set_lable_font(
		tipId, LEBAO_NORMAL_FONT, LEBAO_FONT_COLOR, LEBAO_ID_TEXT(TEXT_LEBAO_SCAN_QR));
	if (pageId != LEBAO_PAGE_ORDER)
		GUILABEL_SetVisible(tipId, FALSE, TRUE);
	MMK_SetAtvCtrl(win_id, tipId);
}

// supported in simulator
void lebao_order_show_qrcode(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id, const int pageId, const int onlyBackground, char** qrCodeBmp, int* bufSize)
{
	MMI_CTRL_ID_T tipId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_SCAN);
	GUI_LCD_DEV_INFO lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };
	int qrWidth = 0; // qr image, 21x
	GUI_RECT_T imgRect = { 0 };
	GUI_RECT_T bgRect = { 0 };
	GUI_RECT_T tipRect = { 0 };
	char* qrBmp = NULL;
	int qrSize = 0;

	//qrCodeBmpSize == NULL
	if (*qrCodeBmp == NULL) {
		sds uri = NULL;
		if (pageId == LEBAO_PAGE_ABOUT)
			uri = lebao_srv_get_copyright_uri();
		else if (pageId == LEBAO_PAGE_CONTACT)
			uri = sdsnew("https://kf.migu.cn/code/#/verificationCode?channelId=2200");
		else
			uri = lebao_srv_get_order_uri();

		if (uri != NULL) {
			if (helper_get_qr_code_bmp(uri, &qrBmp, &qrSize) != 0) {
				helper_debug_printf("get QR failed\n");
				sdsfree_val(uri);
				return;
			}
			sdsfree_val(uri);
		}
		else {
			helper_debug_printf("get the URL of QR failed\n");
		}

		if (qrBmp == NULL && qrSize <= 0)
			return;

		*qrCodeBmp = qrBmp;
		*bufSize = qrSize;
	}
	else {
		qrBmp = *qrCodeBmp;
		qrSize = *bufSize;
	}

	if (qrBmp != NULL && qrSize > 54) {
		qrWidth = (unsigned int)((qrBmp[0x12] & 0xFF) | ((qrBmp[0x13] << 8) & 0xFF00) | ((qrBmp[0x14] << 16) & 0xFF0000) | ((qrBmp[0x15] << 24) & 0xFF000000));
//		qrheight = (unsigned int)((qrBmp[0x16] & 0xFF) | ((qrBmp[0x17] << 8) & 0xFF00) | ((qrBmp[0x18] << 16) & 0xFF0000) | ((qrBmp[0x19] << 24) & 0xFF000000));
	}

	qrWidth = (qrWidth <= 0) ? ((LEBAO_SCREEN_WIDTH <= 128) ? 84 : 168) : qrWidth;
	qrWidth = qrWidth > 168 ? 168 : qrWidth;

	// 320x170, 480x200
	if (LEBAO_SCREEN_WIDTH > 240 && LEBAO_SCREEN_WIDTH > LEBAO_SCREEN_HEIGHT) {
		qrWidth = 84;
	}
	// 128x128
	else if (LEBAO_SCREEN_WIDTH <= 128)
		qrWidth = 84;

	imgRect.left = (LEBAO_SCREEN_WIDTH - qrWidth) / 2;
	imgRect.top = (LEBAO_SCREEN_HEIGHT - qrWidth) / 2;

	// 128x128
	if (LEBAO_SCREEN_WIDTH <= 128)
		imgRect.top = imgRect.top / 2;

	imgRect.right = imgRect.left + qrWidth;
	imgRect.bottom = imgRect.top + qrWidth;

	bgRect.left = imgRect.left - 2;
	bgRect.top = imgRect.top - 2;
	bgRect.right = imgRect.right + 2;
	bgRect.bottom = imgRect.bottom + 2;
	LCD_FillRect(&lcd_dev_info, bgRect, MMI_WHITE_COLOR);

	if (onlyBackground != 0)
		return;

	if (qrBmp != NULL && qrSize > 0) {
		lebao_show_anim_image_buffer(ctrl_id, qrBmp, qrSize, &imgRect);
		//	MMK_SetAtvCtrl(win_id, ctrl_id);
	}

	// adjust the tip position
	CTRLLABEL_GetRect(tipId, &tipRect);
	tipRect.top = bgRect.bottom + LEBAO_TOP_PADDING;
	tipRect.bottom = tipRect.top + LEBAO_TITLE_HEIGHT;
	GUILABEL_SetRect(tipId, &tipRect, FALSE);
}


#if defined(LEBAO_SIZE_480)
	#define IMG_PREV_SIZE_		(54)
	#define IMG_NEXT_SIZE_		(54)
	#define IMG_PLAY_SIZE_		(90)
	#define IMG_FAV_SIZE_		(64)
	#define IMG_VOL_SIZE_		(64)
	#define RADIUS_ADD_SIZE_ 	(25)
#elif defined(LEBAO_SIZE_360)
	#define IMG_PREV_SIZE_		(54)
	#define IMG_NEXT_SIZE_		(54)
	#define IMG_PLAY_SIZE_		(90)
	#define IMG_FAV_SIZE_		(64)
	#define IMG_VOL_SIZE_		(64)
	#define RADIUS_ADD_SIZE_ 	(25)
#elif defined(LEBAO_SIZE_128)
	#define IMG_PREV_SIZE_		(18)
	#define IMG_NEXT_SIZE_		(18)
	#define IMG_PLAY_SIZE_		(30)
	#define IMG_FAV_SIZE_		(18)
	#define IMG_VOL_SIZE_		(18)
	#define RADIUS_ADD_SIZE_ 	(10)
#else
	#define IMG_PREV_SIZE_		(36)
	#define IMG_NEXT_SIZE_		(36)
	#define IMG_PLAY_SIZE_		(60)
	#define IMG_FAV_SIZE_		(36)
	#define IMG_VOL_SIZE_		(36)
	#define RADIUS_ADD_SIZE_	(10)
#endif

void lebao_player_init_control(MMI_WIN_ID_T win_id)
{
	MMI_CTRL_ID_T songNameId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_SONG_NAME);
	MMI_CTRL_ID_T singerNameId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_LABEL_SINGER_NAME);
	MMI_CTRL_ID_T prevId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_PREV);
	MMI_CTRL_ID_T playId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_PLAY);
	MMI_CTRL_ID_T nextId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_NEXT);
	MMI_CTRL_ID_T ringId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_RING);
	MMI_CTRL_ID_T favId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_FAVORITE);
	MMI_CTRL_ID_T volId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_VOLUME);

	GUI_RECT_T songRect = { 0 };
	GUI_RECT_T singerRect = { 0 };
	GUI_RECT_T prevRect = { 0 };
	GUI_RECT_T nextRect = { 0 };
	GUI_RECT_T playRect = { 0 };
	GUI_RECT_T favRect = { 0 };
	GUI_RECT_T volumeRect = { 0 };	
	GUI_RECT_T ringRect = { 0 };

    singerRect.left = LEBAO_LEFT_PADDING + LEBAO_ROUND_PADDING * 2;
    singerRect.top = LEBAO_TOP_PADDING + LEBAO_ROUND_PADDING;
	singerRect.right = LEBAO_SCREEN_WIDTH - singerRect.left;
	singerRect.bottom = singerRect.top + LEBAO_LABEL_HEIGHT;

    songRect.left = LEBAO_LEFT_PADDING + LEBAO_ROUND_PADDING;
    songRect.top = singerRect.bottom + LEBAO_TOP_PADDING;
	songRect.right = LEBAO_SCREEN_WIDTH - songRect.left;
	songRect.bottom = songRect.top + LEBAO_LABEL_HEIGHT;

	playRect.left = (LEBAO_SCREEN_WIDTH - IMG_PLAY_SIZE_) / 2;
	playRect.top = LEBAO_SCREEN_HEIGHT / 2 - IMG_PLAY_SIZE_ / 3 + LEBAO_SCREEN_HEIGHT / 20;
	playRect.right = playRect.left + IMG_PLAY_SIZE_;
	playRect.bottom = playRect.top + IMG_PLAY_SIZE_;

	prevRect.left = LEBAO_SCREEN_WIDTH /10;
	if (lebao_is_round_watch() == 0) {
		prevRect.left = LEBAO_SCREEN_WIDTH /12;
	}
	prevRect.top = playRect.top + IMG_PLAY_SIZE_ / 2 - IMG_PREV_SIZE_ / 2;
	prevRect.right = prevRect.left + IMG_PREV_SIZE_;
	prevRect.bottom = prevRect.top + IMG_PREV_SIZE_;

	nextRect.left = LEBAO_SCREEN_WIDTH - prevRect.left - IMG_NEXT_SIZE_;
	nextRect.top = prevRect.top;
	nextRect.right = nextRect.left + IMG_NEXT_SIZE_;
	nextRect.bottom = nextRect.top + IMG_NEXT_SIZE_;

	favRect.left = LEBAO_SCREEN_WIDTH / 4;
	favRect.top = playRect.bottom + LEBAO_BOTTOM_PADDING * 2 + RADIUS_ADD_SIZE_ / 2;
    if (LEBAO_SCREEN_HEIGHT > LEBAO_SCREEN_WIDTH) {
        favRect.top += RADIUS_ADD_SIZE_* 2 / 3;
    }
	if ((favRect.top + IMG_FAV_SIZE_) > LEBAO_SCREEN_HEIGHT)
		favRect.top -= (favRect.top + IMG_FAV_SIZE_ - LEBAO_SCREEN_HEIGHT - LEBAO_BOTTOM_PADDING);

	favRect.right = favRect.left + IMG_FAV_SIZE_;
	favRect.bottom = favRect.top + IMG_FAV_SIZE_;

	volumeRect.left = LEBAO_SCREEN_WIDTH * 3 / 4 - IMG_VOL_SIZE_;
	volumeRect.top = favRect.top;
	volumeRect.right = volumeRect.left + IMG_VOL_SIZE_; 
	volumeRect.bottom = volumeRect.top + IMG_VOL_SIZE_; 

	ringRect = favRect;

	// artist
	GUILABEL_SetRect(singerNameId, &singerRect, FALSE);
	// song
	GUILABEL_SetRect(songNameId, &songRect, FALSE);

	// prev
	GUIBUTTON_SetRect(prevId, &prevRect);
	GUIBUTTON_SetRunSheen(prevId, FALSE);
	//	GUIBUTTON_SetPressedBg(prevId, &prevRect_bg);

	// play / pause
	GUIBUTTON_SetRect(playId, &playRect);
	GUIBUTTON_SetRunSheen(playId, FALSE);
	//	GUIBUTTON_SetPressedBg(playId, &playRect_bg);

	// next
	GUIBUTTON_SetRect(nextId, &nextRect);
	GUIBUTTON_SetRunSheen(nextId, FALSE);
	//	GUIBUTTON_SetPressedBg(nextId, &nextRect_bg);

	// ring setting
	ringRect = favRect;
	GUIBUTTON_SetRect(ringId, &ringRect);
	GUIBUTTON_SetRunSheen(ringId, FALSE);

	// favorite
	GUIBUTTON_SetRect(favId, &favRect);
	GUIBUTTON_SetRunSheen(favId, FALSE);
	
	// volume
	GUIBUTTON_SetRect(volId, &volumeRect);
	GUIBUTTON_SetRunSheen(volId, FALSE);
	//	GUIBUTTON_SetPressedBg(volId, &volume_press_bg);

	if(lebao_get_label_can_roll() != 0)
		MMK_SetAtvCtrlEx(win_id, songNameId, TRUE);
	else
		MMK_SetAtvCtrlEx(win_id, playId, TRUE);
}


extern void _draw_play_progress(const GUI_LCD_DEV_INFO* dev_info_ptr,
	GUI_RECT_T*             clip_rect_ptr,
	int16                   x,
	int16                   y,
	uint16                  radius,
	uint16                  pg,
	GUI_COLOR_T             color);

void lebao_player_draw_progress(int pg)
{
	GUI_LCD_DEV_INFO lcd_dev_info = { GUI_MAIN_LCD_ID, GUI_BLOCK_MAIN };

	GUI_RECT_T rect = lebao_get_full_screen_rect();
	int playWidth = 0, playHeight = 0;
	GUI_RECT_T playRect = { 0 };
	int radius = 0;
	MMI_CTRL_ID_T playId = LEBAO_ID_CTRL(RES_MMI_LEBAO_CTRL_ID_BTN_PLAY);

	CTRLBASE_GetRect(playId, &playRect);
	playWidth = playRect.right - playRect.left;
	playHeight = playRect.bottom - playRect.top;

	radius = (playWidth < playHeight) ? (playWidth / 2 + RADIUS_ADD_SIZE_) :  (playHeight / 2 + RADIUS_ADD_SIZE_);
	// if (LEBAO_SCREEN_WIDTH > 240 && LEBAO_SCREEN_HEIGHT > 240)
	//	radius += 5;

	_draw_play_progress(&lcd_dev_info, &rect, 
		playRect.left + playWidth / 2, playRect.top + playHeight / 2, radius,
		pg * 360 / 100, GUI_RGB2RGB565(0xff, 0xa0, 0x00));
}

// lebao msgbox
void lebao_tips_display_text(MMI_WIN_ID_T win_id, MMI_STRING_T* text_ptr)
{
    GUI_RECT_T       box_rect = {0};
    GUI_RECT_T       text_rect = {0};
    GUISTR_STATE_T   text_state = GUISTR_STATE_ALIGN | GUISTR_STATE_EFFECT | GUISTR_STATE_EFFECT | GUISTR_STATE_ELLIPSIS;
    GUISTR_STYLE_T   text_style = {0};
    GUISTR_INFO_T    text_info = {0};
    GUI_LCD_DEV_INFO lcd_dev_info = {0};
    
    if (text_ptr == NULL || text_ptr->wstr_ptr == NULL ||text_ptr->wstr_len <= 0 )
        return;

    MMK_GetWinLcdDevInfo(win_id,&lcd_dev_info);
    text_style.align = ALIGN_HVMIDDLE; 
    text_style.effect = FONT_EFFECT_NONE;
    text_style.angle = ANGLE_0;
    text_style.font = lebao_get_normal_font();
    text_style.font_color = lebao_get_font_color();
    GUISTR_GetStringInfo(&text_style, text_ptr, text_state, &text_info);
    
    box_rect = lebao_get_full_screen_rect();
	box_rect.left = LEBAO_LEFT_PADDING + LEBAO_ROUND_PADDING /2;
	box_rect.right -= (LEBAO_RIGHT_PADDING + LEBAO_ROUND_PADDING / 2);
	box_rect.top = lebao_get_screen_height() - (lebao_get_screen_height() / 3) - 15;
	box_rect.bottom -=(LEBAO_BOTTOM_PADDING + LEBAO_ROUND_PADDING + 10);

	if (lebao_is_round_watch() || lebao_is_round_rectangle_watch()) {
		int box_height = box_rect.bottom - box_rect.top;
		box_rect.top = (lebao_get_screen_height() - box_height) / 2;
		box_rect.bottom = box_rect.top + box_height;
	}

	text_rect = box_rect;
	text_rect.left += 5;
	text_rect.right -= 5;

	LCD_FillRoundedRect(&lcd_dev_info, box_rect, box_rect, MMI_BLACK_COLOR);
	LCD_DrawRoundedRect(&lcd_dev_info, box_rect, box_rect, GUI_RGB2RGB565(0x30, 0x30, 0x30)); // GUI_RGB2RGB565(0xff, 0xa0, 0x00)
    GUISTR_DrawTextToLCDInRect(&lcd_dev_info, &text_rect, &text_rect, text_ptr, &text_style, text_state, 0);
}


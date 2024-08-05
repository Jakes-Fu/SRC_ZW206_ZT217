#ifndef LEBAO_LAYOUT_H
#define LEBAO_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "std_header.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmipub.h"
#include "guires.h"

/*********************
 *      DEFINES	
 *********************/
 // Download the list pictures of chart from the Server, 0 : hide, 1 : show
#define LEBAO_SHOW_CHART_IMAGE		(1)

// Download the list pictures of music from the Server, 0 : hide, 1 : show
#define LEBAO_SHOW_SONG_IMAGE		(0)

#define LEBAO_SOFTKEY_BAR_HEIGHT	(0)
#define LEBAO_TITLE_COLOR			MMI_GRAY_WHITE_COLOR

#define LEBAO_SCREEN_WIDTH		lebao_get_screen_width()
#define LEBAO_SCREEN_HEIGHT		lebao_get_screen_height()

#define LEBAO_NEED_HIGHLIGHT_BAR  lebao_get_need_highlight_bar()

#define LEBAO_TITLE_HEIGHT		lebao_get_title_height()
#define LEBAO_LABEL_HEIGHT		lebao_get_label_height()
#define LEBAO_LEFT_PADDING		lebao_get_left_padding()
#define LEBAO_TOP_PADDING		lebao_get_top_padding()
#define LEBAO_RIGHT_PADDING		lebao_get_right_padding()
#define LEBAO_BOTTOM_PADDING	lebao_get_bottom_padding()
#define LEBAO_ROUND_PADDING		lebao_get_round_padding()

#define LEBAO_BACKGROUD_COLOR	lebao_get_bg_color()
#define LEBAO_FONT_COLOR		lebao_get_font_color()
#define LEBAO_BIG_FONT			lebao_get_big_font()
#define LEBAO_NORMAL_FONT		lebao_get_normal_font()
#define LEBAO_SMALL_FONT		lebao_get_small_font()


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

// config
void lebao_set_round_watch(void);
int lebao_is_round_watch(void);
int lebao_is_round_rectangle_watch(void);
int lebao_is_feature_phone(void);
void lebao_set_label_can_roll(void);
int lebao_get_label_can_roll(void);

// screen
int lebao_get_screen_width(void);
int lebao_get_screen_height(void);
GUI_RECT_T lebao_get_full_screen_rect(void);

// layout rect
GUI_RECT_T lebao_get_title_rect(void);
void lebao_set_list_default_rect(MMI_CTRL_ID_T listId, int titleHeight);
void lebao_set_list_rect(MMI_CTRL_ID_T listId, int titleHeight, int bottomMargin);

int lebao_get_title_height(void);
int lebao_get_left_padding(void);
int lebao_get_top_padding(void);
int lebao_get_right_padding(void);
int lebao_get_bottom_padding(void);
int lebao_get_round_padding(void);

// theme
int lebao_get_bg_color(void);
int lebao_get_font_color(void);
int lebao_get_big_font(void);
int lebao_get_normal_font(void);
int lebao_get_small_font(void);

// style
void lebao_set_menu_list_style(const MMI_CTRL_ID_T list_id, const int index);
void lebao_set_charts_list_style(const MMI_CTRL_ID_T list_id, const int index);
void lebao_set_charts_list_style_vip(const MMI_CTRL_ID_T list_id, const int index);
void lebao_set_local_list_style(const MMI_CTRL_ID_T list_id, const int index);

// page
void lebao_splash_init_control(MMI_WIN_ID_T win_id);
void lebao_menu_init_control(MMI_WIN_ID_T win_id);
void lebao_chart_init_control(MMI_WIN_ID_T win_id);
void lebao_musiclist_init_control(MMI_WIN_ID_T win_id);
void lebao_local_init_control(MMI_WIN_ID_T win_id);
void lebao_search_init_control(MMI_WIN_ID_T win_id);
void lebao_volume_init_control(MMI_WIN_ID_T win_id);
void lebao_waiting_init_control(MMI_WIN_ID_T win_id);
void lebao_order_init_control(MMI_WIN_ID_T win_id, const int pageId);
void lebao_player_init_control(MMI_WIN_ID_T win_id);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LEBAO_LAYOUT_H*/

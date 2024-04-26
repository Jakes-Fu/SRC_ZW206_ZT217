/*
 * @Author: guojiaqi
 * @Date: 
 * @LastEditors: guojiaqi
 * @LastEditTime: 
 * @FilePath: dsl_poetry_main.h
 * @Description: 
 */

#ifndef _DSL_POETRY_MAIN_H_
#define _DSL_POETRY_MAIN_H_
#include "sci_types.h"
#include "mmk_type.h"
/*----------------------------------------------------------------------------*/
/* Compiler Flag */
/*----------------------------------------------------------------------------*/ 
#ifdef _cplusplus
extern "C"
 {
#endif
#define BASE_POETRY_TEST_URL "http://open-api-test.readboy.com/shici/"
#define BASE_POETRY_URL "http://open.readboy.com/shici/"
#define BASE_DEVICE_IMEI "868750061006384"

#define POETRY_LINE_WIDTH MMI_MAINSCREEN_WIDTH/6
#define POETRY_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/10

#define POETRY_GRADE_TOTAL_NUM 18
#define POETRY_GRADE_LIST_SHOW_ITEM 5
#define POETRY_ITEM_LIST_SHOW_ITEM 4

#define POETRY_PAGE_SIZE 14

#define OFFLINE_DATA  1
#define TEST_API  0

#define DEPOSIT_TEST_API 0

#define MAX_POETRY_LIST_SIZE 350

#define POETRY_TIME_DIFF_FROM_1970_TO_1980     ((365*10+2)*24*3600)//315619200-86400(1970-1979)

typedef struct
{
	char name[100];
}DSL_POETRY_GRADE_NAME;

typedef struct dsl_poetry_page_info{
    uint8 curPage;
    int16 total;
    uint8 curIndex;
    uint8 selectedIndex;
	GUI_RECT_T btn_rect_1;
	GUI_RECT_T btn_rect_2;
}DSL_POETRY_PAGE_T;


typedef struct dsl_poetry_info{
    char id[15];
	char title[100];
	char author[10];
    char years[10];
	//char *origin_content;
	//char *note_content;
	//char *trans_content;
	//char *appre_content;
	//uint8 isfavorite;
} DSL_POETRY_T;
typedef struct DSL_POETRY {
  DSL_POETRY_T poetry[MAX_POETRY_LIST_SIZE];
} DSL_POETRY_LIST_T;

typedef struct dsl_poetry_origin_content{
    char content[1500];
} DSL_POETRY_CONTENT_T;

typedef struct dsl_poetry_trans_content{
    char trans[2500];
} DSL_POETRY_TRANS_CONTENT_T;

typedef struct dsl_poetry_audio_content{
    char audio[200];
} DSL_POETRY_AUDIO_CONTENT_T;


typedef struct dsl_poetry_audio_data{
	char *audio_data;
	int audio_data_len;
}DSL_POETRY_AUDIO_DATA_T;

typedef struct dsl_poetry_datail_info{
    char id[15];
	char title[100];
	char titleAudio[200];
	char author[80];
    char years[80];
	char appre[18000];
	char note[7000];
	int32 version;
	uint8 is_wenyanwen;//ндятнд
	DSL_POETRY_TRANS_CONTENT_T trans[25];
	DSL_POETRY_CONTENT_T content[25];
	DSL_POETRY_AUDIO_CONTENT_T audio[25];
} DSL_POETRY_DETAIL_T;


typedef struct dsl_poetry_update_datail_info{
    char id[15];
	char titleAudio[200];
	uint16 version;
	DSL_POETRY_AUDIO_CONTENT_T audio[25];
	uint16 mp3num;
} DSL_POETRY_UPDATE_DETAIL_T;



typedef struct dsl_poetry_grade_info{
    uint16 id;
	char title[100];
	// char author[10];
    // char years[10];
	//char *origin_content;
	//char *note_content;
	//char *trans_content;
	//char *appre_content;
	//uint8 isfavorite;
} DSL_POETRY_GRADE_INFO_T;
typedef struct dsl_poetry_grade_list{
    // uint16 id;
	// char title[100];
	DSL_POETRY_GRADE_INFO_T grade_info[30];
	// char author[10];
    // char years[10];
	//char *origin_content;
	//char *note_content;
	//char *trans_content;
	//char *appre_content;
	//uint8 isfavorite;
} DSL_POETRY_GRADE_LIST_T;

typedef struct dsl_poetry_all_grade_list{
    // uint16 id;
	// char title[100];
	DSL_POETRY_GRADE_LIST_T all_grade[18];
	// char author[10];
    // char years[10];
	//char *origin_content;
	//char *note_content;
	//char *trans_content;
	//char *appre_content;
	//uint8 isfavorite;
} DSL_POETRY_ALL_GRADE_LIST_T;

typedef struct dsl_temp_favorite_status_list{
    // uint16 id;
	// char title[100];
	uint8 favorite[9000];
	// char author[10];
    // char years[10];
	//char *origin_content;
	//char *note_content;
	//char *trans_content;
	//char *appre_content;
	//uint8 isfavorite;
} DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T;

// typedef struct dsl_poetry_encrypt{
// 	char firstpart[25];
// 	char secondpart[25];
// } DSL_POETRY_ENCRYPT_T;
// typedef enum
// {
//     AUDIO_NOT_PLAYING,
//     AUDIO_IS_DOWNLOADING_TITILE,
//     AUDIO_IS_PLAYING,
//     AUDIO_NOT_PLAYING
// }POETRY_AUDIO_STATUS;
#ifdef _cplusplus
}
#endif
#endif
/*
 * @Author: yfm
 * @Date: 2022-07-28 15:37:09
 * @LastEditors: yfm
 * @LastEditTime: 2022-08-11 11:38:17
 * @FilePath: dsl_word_main.h
 * @Description: 
 */

#ifndef _ZMT_WORD_MAIN_H_
#define _ZMT_WORD_MAIN_H_
#include "sci_types.h"
#include "mmk_type.h"
/*----------------------------------------------------------------------------*/
/* Compiler Flag */
/*----------------------------------------------------------------------------*/ 
#ifdef _cplusplus
extern "C"
 {
#endif
/*******************************************************************/
// Description : open hello sprd windows
// Global resource dependence : 
/******************************************************************/
#define WORD_CARD_LINE_WIDTH MMI_MAINSCREEN_WIDTH/6
#define WORD_CARD_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/10
#define ENGLISE_BOOK_ITEM_PANDDING 10

#define BASE_DEVICE_IMEI "868750061006384"
#define BASE_URL_PATH "http://api.ebag.readboy.com/wisdom-school/"
#define LOCAL_AUDIO_PATH "E:/Word/audio/%s.mp3"
#define LOCAL_BOOK_DIR "E:\\Word"
#define LOCAL_BOOK_INFO_BATH "book_info.json"
typedef struct word_audio{
	char *audio_data_url;
	char *audio_data;
	uint32 audio_data_len;
}ZMT_WORD_AUDIO_T;
typedef struct word{
    char *word;
    char *us;
	int us_audio_request_idx;
	char *us_audio_data;
	int32 us_audio_data_len;
    char *uk;
	int uk_audio_request_idx;
	char *uk_audio_data;
	int32 uk_audio_data_len;
    char *explain;
}ZMT_WORD_T;
typedef struct book{
    char *name;
	char *editionName;
	char *path;
    char *basepath;
    char id[11];
}ZMT_BOOK_T;
typedef struct chapter{
    char *name;
	char *path;
    char id[11];
}ZMT_CHAPTER_T;
typedef struct pageInfo{
    char bookId[11];
    int curIdx;           
    uint8 realIdx;        
    int16 total;         
    uint8 bookIdx;    
    uint8 chapterIdx;   
    uint8 curPageNum;    
    uint8 curPage;
}ZMT_PAGE_INFO;
PUBLIC void MMI_CreateWordWin(void);
PUBLIC void MMI_CreateWordDetailWin(void);

/*----------------------------------------------------------------------------*/
/* Compiler Flag */
/*----------------------------------------------------------------------------*/
#ifdef _cplusplus
}
#endif
#endif
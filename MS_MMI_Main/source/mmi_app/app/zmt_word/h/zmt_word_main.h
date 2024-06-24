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

#define WORD_BOOK_APP_ID "00000001"
#define WORD_BOOK_APP_SECRET "ZmtWordBookSec"

#define WORD_BOOK_HEADR_PATH "http://8.130.95.8:8866/"
#define WORD_BOOK_PUBLISH_PATH "english/getPublisher?%s"
#define WORD_BOOK_ID_PATH "english/get?bookId=%d&%s"
#define WORD_CHAPTER_ID_PATH "english/get?contentId=%d&%s"
#define WORD_BOOK_AUDIO_BASE_PATH "%s%s.mp3"
#define WORD_BOOK_AUDIO_BASE_URL "http://8.130.95.8:8866/file/english/"

#define WORD_PUBLISH_NAME_PATH "E:/word/publish_name.json"
#define WORD_BOOK_CHAPTER_PATH "E:/word/book_%d/book_%d.json"
#define WORD_BOOK_AUDIO_PATH "E:/word/book_%d/%s.mp3"
#define WORD_BOOK_NEW_WORD_PATH "E:/word/book_%d/unmaster_word_%d.json"

#define WORD_PUBLISH_MAX 20
#define WORD_PUBLISH_BOOK_MAX 30
#define WORD_PUBLISH_BOOK_TOTAL_MAX 120
#define WORD_CHAPTER_NUM_MAX 30
#define WORD_CHAPTER_WORD_MAX 50

typedef struct
{
    uint8 count;
}WORD_GRADE_BOOK_T;

typedef struct
{
    int cur_publish_grade_idx;
    int cur_publish_idx;
    int cur_book_idx;
    int cur_chapter_idx;
    int cur_detail_idx;
}WORD_BOOK_INFO_T;

typedef struct
{
    int book_id;
    char * book_name;
    int grade;
}WORD_BOOK_PUBLISH_ITEM_INFO_T;

typedef struct
{
    char * publish_name;
    int publish_id;
    int item_count;
    WORD_BOOK_PUBLISH_ITEM_INFO_T * item_info[WORD_PUBLISH_BOOK_MAX];
}WORD_BOOK_PUBLISH_INFO_T;

typedef struct
{
    char * word;
    char * audio_uri;
    char * audio_data;
    int16 audio_len;
    char * phonetic;
    char * translation;
}WORD_BOOK_DETAIL_T;

typedef struct
{
    int chapter_id;
    char * chapter_name;
    uint8 detail_count;
    WORD_BOOK_DETAIL_T * detail[WORD_CHAPTER_WORD_MAX];
}WORD_BOOK_CHAPTER_T;

PUBLIC void MMI_CreateWordWin(void);
PUBLIC void MMI_CreateWordChapterWin(void);
PUBLIC void MMI_CreateWordDetailWin(void);

PUBLIC void Word_ParseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void WordDetail_PlayPinyinAudio(void);

PUBLIC void Word_ReleaseChapterDetailInfo(void);
PUBLIC void Word_requestChapterDetailInfo(uint16 book_id);
PUBLIC void Word_ReleaseBookInfo(void);
PUBLIC void Word_requestBookInfo(void);

PUBLIC void Word_WriteUnmasterChapterWord(uint16 book_id, uint16 chap_id, char * chap_name, uint8 write_count);
/*----------------------------------------------------------------------------*/
/* Compiler Flag */
/*----------------------------------------------------------------------------*/
#ifdef _cplusplus
}
#endif

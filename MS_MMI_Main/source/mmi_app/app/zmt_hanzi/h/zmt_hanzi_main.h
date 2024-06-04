
#include "sci_types.h"
#include "mmk_type.h"
//#include "dsl_homework_main.h"
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
#define HANZI_CARD_USE_OFF_DATA 0

#define HANZI_CARD_LINE_WIDTH MMI_MAINSCREEN_WIDTH/6
#define HANZI_CARD_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/10

#define HANZI_BASE_DEVICE_IMEI "868750061006384"
#define HANZI_BOOK_HEADER_PATH "https://api.ebag.readboy.com/wisdom-school/"

#define HANZI_CARD_CHAPTER_PATH "E:/hanzi/chapter.json"
#define HANZI_CARD_CHAPTER_WORD_PATH "E:/hanzi/chapter_uint1.json"
#define HANZI_CARD_NEW_WORD_PATH "E:/hanzi/new_word.json"

#define HANZI_BOOK_LIST_MAX_SHOW 5
#define HANZI_BOOK_TOTAL 36 

typedef struct
{
    char name[100];
    uint16 id;
}ZMT_HANZI_BOOK_NAME;

typedef struct HANZI_BOOK_INFO_T
{
    int book_total;
    char book_id[20];
    int cur_book_idx;
    int cur_book_page;
    int cur_section_page;
    int cur_section_idx;
    int cur_section_children_idx;
}HANZI_BOOK_INFO_T;

typedef struct HANZI_CHAPTER_CHILDREN_INFO_T
{
    uint16 chapter_id;
    char * chapter_name;
}HANZI_CHAPTER_CHILDREN_INFO_T;

typedef struct HANZI_CHAPTER_INFO_T
{
    char * chapter_name;
    HANZI_CHAPTER_CHILDREN_INFO_T children[30];
}HANZI_CHAPTER_INFO_T;

typedef struct
{
    char * word;//∫∫◊÷
    char * pingy;//∆¥“Ù
    char * audio_uri;//∫∫◊÷“Ù∆µpath
    int16 audio_len;//“Ù∆µ≥§∂»
    char * audio_data;
    char * remark;//∫∫◊÷ Õ“Â
}HANZI_BOOK_HANZI_INFO;

PUBLIC void MMI_CreateHanziWin(void);
PUBLIC void MMI_CreateHanziChapterWin(void);
PUBLIC void MMI_CreateHanziDetailWin(void);
PUBLIC MMI_RESULT_E MMI_CloseHanziDetailWin(void);

PUBLIC void Hanzi_ParseChapterInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void Hanzi_ReleaseChapterInfo(void);
PUBLIC void Hanzi_ParseDetailInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void HanziDetail_ReleaseDetailInfo(void);
PUBLIC void Hanzi_ParseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void HanziDetail_PlayPinyinAudio(void);

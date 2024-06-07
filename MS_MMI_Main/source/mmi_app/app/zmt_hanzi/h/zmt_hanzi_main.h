
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
#define HANZI_BOOK_HEADER_PATH "http://8.130.95.8:8866/"
#define HANZI_BOOK_GRADE_PATH "hanzi/get?gradeId=%d"
#define HANZI_BOOK_CHAPTER_PATH "hanzi/get?contentId=%d"

#define HANZI_CARD_GRADE_CHAPTER_PATH "E:/hanzi/grade_%d/grade_%d.json"
#define HANZI_CARD_CHAPTER_WORD_PATH "E:/hanzi/grade_%d/%d.json"
#define HANZI_CARD_NEW_WORD_PATH "E:/hanzi/unmaster_hanzi.json"

#define HANZI_CONTENT_CHAPTER_MAX 20
#define HANZI_CHAPTER_WORD_MAX 5
#define HANZI_BOOK_TOTAL 18 

typedef struct
{
	char name[20];
}ZMT_HANZI_GRADE_NAME;

typedef struct HANZI_BOOK_INFO_T
{
    int cur_book_idx;
    int cur_chapter_idx;
    int cur_section_idx;
    int cur_section_children_idx;
}HANZI_BOOK_INFO_T;

typedef struct HANZI_CONTENT_CHAPTER_INFO_T
{
    uint16 chapter_id;
    char * chapter_name;
}HANZI_CONTENT_CHAPTER_INFO_T;

typedef struct HANZI_CONTENT_INFO_T
{
    char * content_name;
    uint16 content_id;
    HANZI_CONTENT_CHAPTER_INFO_T * chapter[HANZI_CONTENT_CHAPTER_MAX];
}HANZI_CONTENT_INFO_T;

typedef struct
{
    char * word;//∫∫◊÷
    char * pingy;//∆¥“Ù
    char * audio_uri;//∫∫◊÷“Ù∆µpath
    int16 audio_len;//“Ù∆µ≥§∂»//0:”–“Ù∆µurl,-1:Œﬁ“Ù∆µurl
    char * audio_data;
    char * similar_word;
    char * annotation;
    char * remark;//–ŒΩ¸◊÷+◊¢ Õ
}HANZI_BOOK_HANZI_INFO;

PUBLIC void MMI_CreateHanziWin(void);
PUBLIC void MMI_CreateHanziChapterWin(void);
PUBLIC void MMI_CreateHanziDetailWin(void);
PUBLIC MMI_RESULT_E MMI_CloseHanziDetailWin(void);

PUBLIC void Hanzi_ReleaseChapterInfo(void);
PUBLIC void Hanzi_ParseChapterInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void Hanzi_requestChapterInfo(uint8 grade_id);

PUBLIC void Hanzi_ReleaseDetailInfo(void);
PUBLIC void Hanzi_ParseDetailInfo(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void Hanzi_requestDetailInfo(uint8 grade_id, uint16 req_id);

PUBLIC void Hanzi_ParseMp3Response(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void HanziDetail_PlayPinyinAudio(void);

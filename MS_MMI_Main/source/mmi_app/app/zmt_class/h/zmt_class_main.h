/*****************************************************************************
** File Name:      zmt_class_main.h                                             *
** Author:         fys                                               *
** Date:           2024/06/27                                                *
******************************************************************************/
#ifndef ZMT_CLASS_MAIN_H
#define ZMT_CLASS_MAIN_H

#include "sci_types.h"
#include "mmk_type.h"
#include "guistring.h"

#define CLASS_SYN_TIMES_DIFF_FROM_1978_TO_1980  ((365*10+2)*24*3600) - (8 * 60 * 60)

#define CLASS_SYN_LINE_WIDTH MMI_MAINSCREEN_WIDTH/6
#define CLASS_SYN_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/10
#define CLASS_SYN_FORM_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/13

#define CLASS_SYN_COURSE_TYPE 1
#define CLASS_SYN_MEDIATYPE 2
#define CLASS_SYN_TAKE_NUMBER 30
#define CLASS_SYN_IS_TEACHER 0
#define CLASS_SYN_API_APP "lubo"
#define CLASS_SYN_API_METHOD "/GKRecite/loadReciteText"

#define CLASS_SYN_APP_ID "yisai"
#define CLASS_SYN_SECRET "22abd792-ec2a-11ee-ae64-043f72e4f00a"
#define CLASS_SYN_UID "1234567890"
#define CLASS_SYN_BASE_API "https://www-preview.gankao.com/api/service/"
#define CLASS_SYN_BASE_TOKEN "app_id=%s&timestamp=%d&token=%s"
#define CLASS_SYN_BASE_SHOWCOURSES_API "showCourses?subject_id=%d&grade_id=%d&course_type=%d&mediatype=%d&take_number=%d&page=%d&%s"
#define CLASS_SYN_BASE_GETCOURSEDETAIL_API "getCourseDetail?course_id=%s&%s"
#define CLASS_SYN_BASE_APIPROXY_API "apiProxy?uid=%s&isteacher=%d&api-app=%s&api-method=%s&api-params=%s&%s"
#define CLASS_SYN_BASE_API_PARAM "{\"typeable\":\"section\",\"typeId\":\"%s\"}"

#define CLASS_SYN_BOOK_FILE_PATH "E:/class/class_book.json"
#define CLASS_SYN_COURSE_FILE_PATH "E:/class/class_course.json"
#define CLASS_SYN_SECTION_FILE_PATH "E:/class/class_section_%d.json"
#define CLASS_SYN_SENTECT_AUDIO_PATH "E:/class/%d/%d/audio_%d.mp3"//"E:/class/book_id/section_id/audio_index.mp3

#define CLASS_SYN_SUBJECT_NUM_MAX 2
#define CLASS_SYN_GRADE_NUM_MAX 9
#define CLASS_SYN_BOOK_NUM_MAX CLASS_SYN_TAKE_NUMBER
#define CLASS_SYN_SECTION_NUM_MAX 30
#define CLASS_SYN_READ_NUM_MAX 50
#define CLASS_SYN_READ_SET_REPEAT_MAX 3

typedef enum
{
    SUBJECT_CHINESE_TYPE = 1,
    SUBJECT_MATH_TYPE,
    SUBJECT_ENGLISH_TYPE,
    SUBJECT_PHYSICS_TYPE,
    SUBJECT_CHEMISTRY_TYPE,
    SUBJECT_BIOLOGY_TYPE,
    SUBJECT_HISTORY_TYPE,
    SUBJECT_POLITICS_TYPE,
    SUBJECT_GEOGRAPHY_TYPE = 27,
    SUBJECT_SCIENCE_TYPE,
}CLASS_SYNC_SUBJECT_TYPE_E;

typedef struct
{
    CLASS_SYNC_SUBJECT_TYPE_E subject_type;
    uint8 grade_idx;
    uint8 book_idx;
    uint8 section_idx; 
}CLASS_SYNC_INFO_T;

typedef struct
{
    char * cate_id;
    char * course_id;
    char * name;
}CLASS_BOOK_INFO_T;

typedef struct
{
    char * type_id;
    char * section_name;
}CLASS_SECTION_INFO_T;

typedef struct
{
    char * text;
    char * audio_url;
    char * audio_data;
    int32 audio_len;
    double audio_duration;
}CLASS_READ_INFO_T;

typedef struct
{
    uint8 cur_idx;
    uint32 cur_ctrl_id;
    BOOLEAN is_play;
    BOOLEAN is_single;
    int repeat_cnt;
    int volume;
    int speed;
    GUI_FONT_T font;
}CLASS_READ_CURRENT_INFO_T;


PUBLIC BOOLEAN Class_PlayAudioMp3(void);

PUBLIC void MMI_CreateClassMainWin(void);
PUBLIC void MMI_CreateClassGradeWin(void);
PUBLIC void MMI_CreateClassBookWin(void);
PUBLIC void MMI_CreateClassSectionWin(void);
PUBLIC void MMI_CreateClassReadWin(void);
PUBLIC void MMI_CreateClassReadSetWin(void);
PUBLIC void ZMTClass_CloseClassPlayer(void);

PUBLIC void Class_ReleaseBookInfo(void);
PUBLIC void Class_ReuestBookInfo(uint8 subject_id, uint8 grade_id, uint8 page);
PUBLIC void Class_ReleaseSectionInfo(void);
PUBLIC void Class_ReuestSectionInfo(char * course_id);
PUBLIC void Class_RealeaseReadInfo(void);
PUBLIC void Class_ReuestReadInfo(char * section_id);
PUBLIC void Class_DeleteReadAudioFile(void);
PUBLIC void Class_RequestMp3Data(char * url, uint16 course_id, uint16 section_id, uint8 idx, BOOLEAN is_download_file);

#endif
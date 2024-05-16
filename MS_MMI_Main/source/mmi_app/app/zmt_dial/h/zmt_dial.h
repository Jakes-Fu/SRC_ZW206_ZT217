/*****************************************************************************
** File Name:      zmt_dial.h                                          *
** Author:           fys                                                        *
** Date:           2024/05/08                                                 *
** Copyright:       *
** Description:                       *
******************************************************************************/

#include "sci_types.h"
#include "mmk_type.h"

#define ZMT_DIAL_USE_FOR_TEST 0

#define ZMT_DIAL_LINE_WIDTH MMI_MAINSCREEN_WIDTH/6
#define ZMT_DIAL_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/10

#define ZMT_DIAL_DIR_BASE_PATH "D:\\zmt_dial"
#define ZMT_DIAL_FILE_DB_PATH "D:\\zmt_dial\\watch_dial.json"
#if ZMT_DIAL_USE_FOR_TEST
#define ZMT_DIAL_FILE_BASE_PATH "D:\\zmt_dial\\dial_0\\dial_0_watch.json"
#else
#define ZMT_DIAL_FILE_BASE_PATH "D:\\zmt_dial\\%s\\%s_watch.json"
#endif

#define WATCH_MAX_ARRAY_COUNT 10
#define WATCH_MAX_PATH_LEN 80
#define WATCH_MAX_DIAL_NAME_LEN 20
#define WATCH_IMAGE_FULL_PATH_MAX_LEN 128

typedef enum {
    ELEMENT_TYPE_INVILID = 0,
    ELEMENT_TYPE_TIME_DIGIT,        //1
    ELEMENT_TYPE_TIME_HAND,         //2
    ELEMENT_TYPE_AM_PM,             //3
    ELEMENT_TYPE_WEEK,              //4
    ELEMENT_TYPE_DATE,              //5
    ELEMENT_TYPE_WEATHER,           //6
    ELEMENT_TYPE_CALORIE,           //7
    ELEMENT_TYPE_STEP,              //8
    ELEMENT_TYPE_DISTANCE,          //9
    ELEMENT_TYPE_BATTERY,           //10
    ELEMENT_TYPE_SLEEP_QUALITY,     //11
    ELEMENT_TYPE_PRESSURE,          //12
    ELEMENT_TYPE_HEART_RATE,        //13
    ELEMENT_TYPE_MSG,                       //14 
    ELEMENT_TYPE_BLOOD_OXYGEN,      //15
    ELEMENT_TYPE_BG_SWITCH,         //16  /*except when updating all elements, always at the last*/ 
    ELEMENT_TYPE_SIGNAL,            //17
    ELEMENT_TYPE_MAX
}watch_element_type_t;

typedef enum {
    IMG_TYPE_TIME_HOUR_TENS = ELEMENT_TYPE_TIME_DIGIT * 10,//10
    IMG_TYPE_TIME_HOUR_UNIT,        //11
    IMG_TYPE_TIME_MIN_TENS,         //12
    IMG_TYPE_TIME_MIN_UNIT,         //13
    IMG_TYPE_TIME_SEC_TENS,         //14
    IMG_TYPE_TIME_SEC_UNIT,         //15
    IMG_TYPE_TIME_CONNECTOR,        //16

    IMG_TYPE_TIME_HOUR_HAND = ELEMENT_TYPE_TIME_HAND * 10,//20
    IMG_TYPE_TIME_MIN_HAND,              //21
    IMG_TYPE_TIME_SEC_HAND,              //22
    IMG_TYPE_TIME_CENTER_DOT,            //23

    IMG_TYPE_TIME_AM_PM = ELEMENT_TYPE_AM_PM * 10,//30

    IMG_TYPE_WEEK_DIGIT = ELEMENT_TYPE_WEEK * 10, //40
    IMG_TYPE_WEEK_HAND,             //41

    IMG_TYPE_YEAR_THOUSAND = ELEMENT_TYPE_DATE * 10,//50
    IMG_TYPE_YEAR_HUNDRED,          //51
    IMG_TYPE_YEAR_TENS,             //52
    IMG_TYPE_YEAR_UNIT,             //53
    IMG_TYPE_MON_TENS,              //54
    IMG_TYPE_MON_UNIT,              //55
    IMG_TYPE_DAY_TENS,              //56
    IMG_TYPE_DAY_UNIT,              //57
    IMG_TYPE_MON,                   //58
    IMG_TYPE_DATE_CONNECTOR,        //59

    IMG_TYPE_WEATHER = ELEMENT_TYPE_WEATHER * 10,//60
    IMG_TYPE_TEMPERATURE_TENS,      //61
    IMG_TYPE_TEMPERATURE_UNIT,      //62
    IMG_TYPE_TEMPERATURE_MINUS,     //63
    IMG_TYPE_TEMPERATURE_CENT,      //64
    IMG_TYPE_WEATHER_UV,            //65
    IMG_TYPE_WEATHER_AQI,           //66

    IMG_TYPE_CALORIE = ELEMENT_TYPE_CALORIE * 10, //70
    IMG_TYPE_CALORIE_HUNDRED,       //71
    IMG_TYPE_CALORIE_TENS,          //72
    IMG_TYPE_CALORIE_UNIT,          //73
    IMG_TYPE_CALORIE_KCAL,          //74
    IMG_TYPE_CALORIE_PROGRESS,      //75

    IMG_TYPE_STEP = ELEMENT_TYPE_STEP * 10,       //80
    IMG_TYPE_STEP_MYRI,             //81
    IMG_TYPE_STEP_THOUSAND,         //82
    IMG_TYPE_STEP_HUNDRED,          //83
    IMG_TYPE_STEP_TENS,             //84
    IMG_TYPE_STEP_UNIT,             //85
    IMG_TYPE_STEP_STEP,             //86
    IMG_TYPE_STEP_PROGRESS,         //87

    IMG_TYPE_DISTANCE = ELEMENT_TYPE_DISTANCE * 10,//90
    IMG_TYPE_DISTANCE_TENS,         //91
    IMG_TYPE_DISTANCE_UNIT,         //92
    IMG_TYPE_DISTANCE_DOT,          //93
    IMG_TYPE_DISTANCE_DECI,         //94
    IMG_TYPE_DISTANCE_KM,           //95
    IMG_TYPE_DISTANCE_PROGRESS,     //96

    IMG_TYPE_BATTERY = ELEMENT_TYPE_BATTERY * 10,  //100
    IMG_TYPE_BATTERY_HUNDRED,       //101
    IMG_TYPE_BATTERY_TENS,          //102
    IMG_TYPE_BATTERY_UNIT,          //103
    IMG_TYPE_BATTERY_PERCENT,       //104
    IMG_TYPE_BATTERY_SIGN,          //105

    IMG_TYPE_SLEEP_QUALITY = ELEMENT_TYPE_SLEEP_QUALITY * 10,//110
    IMG_TYPE_SLEEP_QUALITY_VALUE,//111

    IMG_TYPE_PRESSURE = ELEMENT_TYPE_PRESSURE * 10,         //120
    IMG_TYPE_PRESSURE_HUNDRED,      //121
    IMG_TYPE_PRESSURE_TENS,         //122
    IMG_TYPE_PRESSURE_UNIT,         //123
    IMG_TYPE_PRESSURE_KPA,          //124
    IMG_TYPE_PRESSURE_LEVEL,        //125

    IMG_TYPE_HEART_RATE = ELEMENT_TYPE_HEART_RATE * 10,      //130
    IMG_TYPE_HEART_RATE_HUNDRED,    //131
    IMG_TYPE_HEART_RATE_TENS,       //132
    IMG_TYPE_HEART_RATE_UNIT,       //133
    IMG_TYPE_HEART_RATE_BPM,        //134

    IMG_TYPE_BLOOD_OXYGEN = ELEMENT_TYPE_BLOOD_OXYGEN * 10,  //140
    IMG_TYPE_BLOOD_OXYGEN_TENS,     //141
    IMG_TYPE_BLOOD_OXYGEN_UNIT,     //142
    IMG_TYPE_BLOOD_OXYGEN_PERCENT,  //143

    IMG_TYPE_BG = ELEMENT_TYPE_BG_SWITCH * 10, //150

    IMG_TYPE_SIGNAL_SIGN = ELEMENT_TYPE_SIGNAL * 10 //170
    
}watch_img_type_t;

typedef enum {
    LABEL_TYPE_TIME_HOUR= 0,        //0
    LABEL_TYPE_TIME_MIN,            //1
    LABEL_TYPE_TIME_SEC,            //2
    LABEL_TYPE_TIME_CONNECTOR,      //3 ":" "/"
    LABEL_TYPE_AM_PM,               //4
    LABEL_TYPE_WEEK,                //5  周一/Mon
    LABEL_TYPE_WEEK_EN,             //6  Mon
    LABEL_TYPE_DATE_YEAR,           //7
    LABEL_TYPE_DATE_MONTH,          //8  "01 02 ... 12"
    LABEL_TYPE_DATE_DAY,            //9
    LABEL_TYPE_DATE_CONNECTOR,      //10 ";" "/" "年" "月" "日"
    LABEL_TYPE_MONTH,               //11 "Jan Feb ... Dec"
    LABEL_TYPE_WEATHER_TEMP,        //12
    LABEL_TYPE_WEATHER_UV,          //13
    LABEL_TYPE_WEATHER_AQI,         //14
    LABEL_TYPE_CALORIE,             //15
    LABEL_TYPE_CALORIE_KCAL,        //16
    LABEL_TYPE_STEP,                //17
    LABEL_TYPE_STEP_STEPS,          //18
    LABEL_TYPE_DISTANCE,            //19
    LABEL_TYPE_DISTANCE_KM,         //20
    LABEL_TYPE_BATTERY,             //21
    LABEL_TYPE_SLEEP_QUALITY,       //22
    LABEL_TYPE_PRESSURE,            //23
    LABEL_TYPE_HEART_RATE,          //24
    LABEL_TYPE_HEART_RATE_BPM,      //25
    LABEL_TYPE_BLOOD_OXYGEN,        //26
    LABEL_TYPE_MSG_NUM,//27
    LABEL_TYPE_SIGNAL,//28
}watch_label_type_t;

typedef struct
{
    char sub_img[WATCH_MAX_PATH_LEN];
}watch_element_subimg_char_t;

typedef struct
{
    uint16 img_type;
    GUI_POINT_T img_pos;
    uint16 img_width;
    uint16 img_height;
    uint8 sub_img_cnt;
    watch_element_subimg_char_t sub_img_ll[WATCH_MAX_ARRAY_COUNT];
} watch_element_img_dsc_t;

typedef struct
{
    uint16 label_type;
    char text[WATCH_MAX_PATH_LEN];
    GUI_POINT_T text_pos;
    uint8 font_size;
} watch_element_label_dsc_t;

typedef struct
{
    uint8 element_type;
    uint8 img_cnt; 
    watch_element_img_dsc_t img_ll[WATCH_MAX_ARRAY_COUNT];
    uint8 label_cnt;
    watch_element_label_dsc_t label_ll[WATCH_MAX_ARRAY_COUNT];
} watch_element_t;

typedef struct
{
    char name[WATCH_MAX_DIAL_NAME_LEN];
    char preview_img[WATCH_MAX_PATH_LEN];
    char bg_img[WATCH_MAX_PATH_LEN];
    uint8 dial_type;
    uint8 element_cnt;
    watch_element_t element_ll[WATCH_MAX_ARRAY_COUNT];
} watch_dial_t;

typedef struct
{
    uint16          width;
    uint16          height;
    uint8*          data_ptr;
    uint32          data_size;
}IMG_PROCESS_DATA_T;

typedef struct
{
    char name[80];
    char preview[80];
    int type;
}ZMT_DIAL_INFO_T;

typedef struct
{
    uint16 count;
    ZMT_DIAL_INFO_T * info[10];
}ZMT_DIAL_LIST_INFO_T;

PUBLIC void MMI_CreateZmtDialStoreWin(void);
PUBLIC void ZMT_DialPanelShow(MMI_WIN_ID_T win_id, char * watch_name);

PUBLIC void ZmtDial_ParseDialDB(char * buf, ZMT_DIAL_LIST_INFO_T * dail_list);
PUBLIC ZMT_DIAL_LIST_INFO_T * ZmtWatch_GetPanelList(void);

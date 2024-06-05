
#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"

#ifndef LYRIC_PARSER_H_
#define LYRIC_PARSER_H_

#define LP_DEBUG  SCI_TRACE_LOW
#define LP_ERROR SCI_TRACE_LOW

#define bool BOOLEAN
#define false FALSE
#define true TRUE
#define UINT8 uint8
#define INT32 int32

typedef struct lyric_content_node {
    long timems;
    char *content;
    int content_len;
    struct lyric_content_node *next;
} lyric_content_node;

typedef enum {
    LP_UNKNOW,
    LP_ASCII,
    LP_UNICODE_L, // little endian fffe
    LP_UNICODE_B, // big endian feff
    LP_UTF8,
} FILE_CODE;

typedef enum {
    LP_SYM_UNKNOW,
    LP_SYM_WIN,
    LP_SYM_UNIX,
    LP_SYM_MAC,
} LINE_SYMBOL;

typedef struct {
    int code;               // 编码类型
    int line_sym;           // 行结束符
    char *author;           // [au:歌曲作者]
    char *by;               // [by:lrc编辑作者]
    char *album;            // [al:专辑名]
    char *artist;           // [ar:演唱者]
    char *re;               // [re:lrc文件编辑器或播放器]
    char *title;            // [tl:歌曲标题]
    char *version;          // [ve:程序版本]
    int offset;             // [offset:+/-时间补偿值，以毫秒为单位，正值表示加快，负值表示延后]
    lyric_content_node *lyric_list_head;  // [mm:ss.xx]
    int lyric_num;
} lyric_t;

lyric_t *lyric_init(char *lrc_name);
void lyric_uninit(lyric_t *lrc_p);
void lyric_print(lyric_t *lrc_p);
int lyric_get_node_num(lyric_t *lrc_p);
lyric_content_node *lyric_get_node(lyric_t *lrc_p, long milsec);
int lyric_get_node_index(lyric_t *lrc_p, long milsec);
lyric_content_node *lyric_get_node_by_index(lyric_t *lrc_p, int index);
int lyric_get_all_node_content_len(lyric_t *lrc_p);
char *lyric_get_all_node_content(lyric_t *lrc_p);

#endif /* LYRIC_PARSER_H_ */

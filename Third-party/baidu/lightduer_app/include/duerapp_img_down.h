// Copyright (2021) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_img_down.h
 * Auth: Tangquan (tangquan@baidu.com)
 * Desc: network images.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 7/2021         tangquan         Create                                    *
******************************************************************************/
#ifndef __DUERAPP_IMG_DOWN_H__
#define __DUERAPP_IMG_DOWN_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

// #ifdef LV_CONF_INCLUDE_SIMPLE
// #include "lvgl.h"
// #include "lv_watch_conf.h"
// #else
// #include "../../../lvgl/lvgl.h"
// #include "../../../lv_watch_conf.h"
// #endif

/*********************
 *      DEFINES
 *********************/
#define BIG_TO_LITTLE_INT(before)     ( ((before & 0x000000FF) << 24) |\
                                        ((before & 0x0000FF00) <<  8) |\
                                        ((before & 0x00FF0000) >>  8) |\
                                        ((before & 0xFF000000) >> 24) )

#define BIG_TO_LITTLE_SHORT(before)   ( ((before & 0x00FF) << 8) |\
                                        ((before & 0xFF00) >> 8) )

/**
 * PNG image description
 */

#define FILE_PNG_MAGIC_NUMBER1  0x474E5089
#define FILE_PNG_MAGIC_NUMBER2  0x0A1A0A0D

/**
 * 数据块类型
 */
#define DATA_CHUNK_TYPE_IHDR "IHDR"
#define DATA_CHUNK_TYPE_IDAT "IDAT"
#define DATA_CHUNK_TYPE_IEND "IEND"
#define DATA_CHUNK_TYPE_tEXt "tEXt"
#define DATA_CHUNK_TYPE_iTXt "iTXt"

/**
 * 过滤方式
 */
#define DATA_FILTER_TYPE_DEFAULT 0
#define DATA_FILTER_TYPE_ADD_ROW 1
#define DATA_FILTER_TYPE_ADD_UP  2
#define DATA_FILTER_TYPE_AVERGE  3
#define DATA_FILTER_TYPE_PAETH   4

/* color type masks */
#define PNG_COLOR_MASK_PALETTE    1
#define PNG_COLOR_MASK_COLOR      2
#define PNG_COLOR_MASK_ALPHA      4

/* color types.  Note that not all combinations are legal */
#define PNG_COLOR_TYPE_GRAY         0
#define PNG_COLOR_TYPE_PALETTE      (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB          (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA    (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA   (PNG_COLOR_MASK_ALPHA)

/**
 * JPEG image description
 */
#define SOI         BIG_TO_LITTLE_SHORT(0xFFD8)          //Start of Image
#define SOF0        BIG_TO_LITTLE_SHORT(0xFFC0)          //Start of Frame 0
#define SOF2        BIG_TO_LITTLE_SHORT(0xFFC2)          //Start of Frame 2
#define DHT         BIG_TO_LITTLE_SHORT(0xFFC4)          //Define Huffman Table(s)
#define DQT         BIG_TO_LITTLE_SHORT(0xFFDB)          //Define Quantization Table(s)
#define DRI         BIG_TO_LITTLE_SHORT(0xFFDD)          //Define Restart Interval
#define SOS         BIG_TO_LITTLE_SHORT(0xFFDA)          //Start of Scan
#define RST(n)      BIG_TO_LITTLE_SHORT((0xFFD0 + n))    //Restart
#define APP(n)      BIG_TO_LITTLE_SHORT((0xFFE0 + n))    //Application-sepcific
#define COM         BIG_TO_LITTLE_SHORT(0xFFFE)          //Comment
#define EOI         BIG_TO_LITTLE_SHORT(0xFFD9)          //End of Image

/**********************
 *      TYPEDEFS
 **********************/
#pragma pack(push)
#pragma pack(1)
typedef struct png_header_s
{
    struct _prefix_s
    {
        union _header_u
        {
            unsigned int raw; //0x474E5089
            struct _detail_s
            {
                unsigned char always_89;
                unsigned char p;
                unsigned char n;
                unsigned char g;
            } detail;
        } magic_number;
        unsigned int bound; //0x0A1A0A0D
    } prefix;

    struct _png_chunk_idhr_s
    {
        unsigned int length_b; //0x0D000000   big endian
        char type_code[4]; //"IHDR"  0x52444849
        struct _IDHRData_s
        {
            unsigned int width_b;
            unsigned int height_b;
            unsigned char bitDepth;
            unsigned char colorType;
            unsigned char compressionMethod;
            unsigned char filterMethod;
            unsigned char interlaceMethod;
        } data;
        unsigned int crc32;
    } png_chunk_idhr;
} png_header_t;
#pragma pack(pop)

typedef struct jpeg_segment_s
{
    unsigned short marker;
    unsigned short size;
} jpeg_segment_t;

typedef struct jpeg_header_s
{
    unsigned short marker_soi;
} jpeg_header_t;

typedef struct jpeg_seg_app0_s
{
    jpeg_segment_t header;
    char JFIF_tag[5];
    unsigned char JFIF_version[2];
    unsigned char density_unit[1];
    unsigned char Xdensity[2];    //For alignment reasons, do not use unsigned short or int
    unsigned char Ydensity[2];
    unsigned char Xthumbnail[1];
    unsigned char Ythumbnail[1];
} jpeg_seg_app0_t;

typedef struct jpeg_seg_sof0_s
{
    jpeg_segment_t header;
    unsigned char resv[1];
    unsigned char height[2];
    unsigned char width[2];
} jpeg_seg_sof0_t;

typedef struct {
    // lv_img_header_t header;
    unsigned int data_size;
    unsigned char *data;
} img_dsc_t;

typedef void (*duer_image_download_cb_t)(img_dsc_t *img, void *down_param);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
img_dsc_t *duer_image_analysis(unsigned char *png_data, unsigned int buf_len);
int duer_image_free(img_dsc_t *p);
int duer_image_download(char *url, duer_image_download_cb_t image_cb, void *down_param);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*CALCULATOR_H*/

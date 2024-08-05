#ifndef __LEBAO_CHART_H
#define __LEBAO_CHART_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lebao_chart_open(void* data);
void lebao_chart_close(void* data);
void lebao_chart_close_all();
void lebao_chart_show_image(const int level);
int lebao_chart_is_opened(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*__LEBAO_CHART_H*/

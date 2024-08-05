#ifndef __LEBAO_MUSICLIST_H
#define __LEBAO_MUSICLIST_H

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
void lebao_musiclist_open(void* data);
void lebao_musiclist_close(void* data);
int lebao_musiclist_is_opened(void);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*__LEBAO_MUSICLIST_H*/

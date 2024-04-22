// Copyright (2021) Baidu Inc. All rights reserveed.
/**
 * File: oneapp.h
 * Auth: Tangquan (tangquan@baidu.com)
 * Desc: oneapp.
 */
/******************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                               *
** 3/2021         tangquan         Create                                    *
******************************************************************************/
#ifndef __DUERAPP_WRITING_H__
#define __DUERAPP_WRITING_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#define LOG_LEVEL                   3
#define DUER_PRINT_FUN              DUER_LOGE// SCI_TRACE_HIGH  /*cplog_printf*/
#define DEBUG                       DUER_PRINT_FUN
#define MARK                        DUER_PRINT_FUN("MARK:[%s,%d]\n", __FUNCTION__, __LINE__);

#if defined(LOG_LEVEL) && (LOG_LEVEL >= 3)
#define LOGI(tag, ...)      log('I', (char *)tag, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGRI(tag,msg)      DUER_PRINT_FUN("I [%s,%d] %s:%s\n", __FUNCTION__, __LINE__, tag, msg)
#else
#define LOGI(tag,msg)
#define LOGRI(tag,msg)
#endif

#if defined(LOG_LEVEL) && (LOG_LEVEL >= 2)
#define LOGW(tag, ...)      log('W', (char *)tag, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGRW(tag,msg)      DUER_PRINT_FUN("W [%s,%d] %s:%s\n", __FUNCTION__, __LINE__, tag, msg)
#else
#define LOGW(tag,msg)
#define LOGRW(tag,msg)
#endif

#if defined(LOG_LEVEL) && (LOG_LEVEL >= 1)
#define LOGE(tag, ...)      log('E', (char *)tag, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOGRE(tag,msg)      DUER_PRINT_FUN("E [%s,%d] %s:%s\n", __FUNCTION__, __LINE__, tag, msg)
#else
#define LOGE(tag,msg)
#define LOGRE(tag,msg)
#endif

/**********************
 * EXPORTED VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void duerapp_writing_init(void);
void duerapp_writing_start(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*CALCULATOR_H*/

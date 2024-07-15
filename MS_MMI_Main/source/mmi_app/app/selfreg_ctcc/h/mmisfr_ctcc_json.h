/*****************************************************************************
** File Name:      mmisfr_ctcc_json.h
** Author:
** Copyright (C) 2000-2100, UNISOC Technoloies Co.,Ltd.
** Description:
*****************************************************************************
**                         Important Edit History                            *
** --------------------------------------------------------------------------*
** DATE                         NAME                            DESCRIPTION
** 2020/11/24             mark.zhang1               Create
******************************************************************************/

#ifndef _MMISFR_CTCC_JSON_H_
#define _MMISFR_CTCC_JSON_H_

/**--------------------------------------------------------------------------*
**                         TYPE AND CONSTANT                                 *
**--------------------------------------------------------------------------*/
typedef enum
{
    SFR_JSON_TAG_NONE = 0,
    SFR_JSON_TAG_RET_CODE,
    SFR_JSON_TAG_MAX
} SFR_JSON_TAG_E;

typedef struct
{
    SFR_JSON_TAG_E cur_tag;
    int retCode;
} SFR_JSON_PARSR_CTX;

/**--------------------------------------------------------------------------*
**                         FUNCTION DEFINITION                              *
**--------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description :
//  Param :
//  Author:
//  Note:
/*****************************************************************************/
PUBLIC unsigned char *sfr_get_post_data(uint32 *p_dst_data_len);

/*****************************************************************************/
//  Description :
//  Param :
//  Author:
//  Note:
/*****************************************************************************/
PUBLIC BOOLEAN sfr_parse_data(uint8 *data, uint32 data_len);

#endif
/***************************************************************************
** File Name:      sample_export.h                                             *
** Author:                                                                 *
** Date:           4/01/2009                                               *
** Copyright:      2009 Spreadtrum, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the data struct of        *
**                 system, application, window and control                 *
****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 4/2009         Xiaoqing.Lu      Create                                  *
**																		   *
****************************************************************************/
#ifndef _SPAMPLE_EXPORT_H_
#define _SPAMPLE_EXPORT_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 

#include "sci_types.h"
#include "mmk_type.h"
#include "guistring.h"

/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/ 

#ifdef _cplusplus
	extern   "C"
    {
#endif
/*----------------------------------------------------------------------------*/
/*                         MACRO DEFINITION                                   */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
/*****************************************************************************/
//  Description : open hello sprd windows
//  Global resource dependence : 
//  Author: xiaoqing.lu
//  Note: 
/*****************************************************************************/

PUBLIC void Create_Formula_Win();
//#define  MMIENVSET_PREVIEW_PLAY_RING_TIMES      1  
//typedef enum
//{
//    MMIFORMULA_CALL_RING_FIXED,     //固定铃声
//    MMIFORMULA_CALL_RING_MORE_RING, //更多音乐
//    MMIFORMULA_CALL_RING_MAX_CLASS
//} MMIFORMULA_CALL_RING_CLASS_E;
//
////the class of msg ring
//typedef enum
//{
//    MMIFORMULA_MSG_RING_FIXED,      //固定铃声
//    MMIFORMULA_MSG_RING_MORE_RING,  //更多铃声
//    MMIFORMUAL_MSG_RING_MAX_CLASS
//}MMIFORMULA_MSG_RING_CLASS_E;
//
//typedef struct dsl_temp_favorite_status_list{
//    // uint16 id;
//	// char title[100];
//	uint8 favorite[9000];
//	// char author[10];
//    // char years[10];
//	//char *origin_content;
//	//char *note_content;
//	//char *trans_content;
//	//char *appre_content;
//	//uint8 isfavorite;
//} DSL_POETRY_TEMP_FAVORITE_STATUS_LIST_T;
//
//typedef void(*MMISET_RING_CALLBACK_PFUNC)(MMISRVAUD_REPORT_RESULT_E result, DPARAM param); 
/*****************************************************************************/
// 	Description : Register hello menu group
//	Global resource dependence : none
//  Author: xiaoqing.lu
//	Note:
/*****************************************************************************/
PUBLIC void MMIAPISAMPLE_FORMULA_ModuleInit(void);

/*****************************************************************************/
//  如下宏定义用以实现对外接口更名前后的兼容
/*****************************************************************************/
#ifdef MMI_FUNC_COMPATIBLE_SUPPORT

#define MMI_CreateHelloSprdWin              MMIAPISAMPLE_CreateHelloSprdWin
#define MMIFORMULA_ModuleInit                 MMIAPISAMPLE_FORMULA_ModuleInit
#define MMI_RING_FORMULA_R1_ID RING_FORMULA_R1
typedef void(*MMISET_RING_CALLBACK_PFUNC)(MMISRVAUD_REPORT_RESULT_E result, DPARAM param); 
typedef enum
{
    MMIFORMULA_MSG_RING_FIXED,      //固定铃声
    MMIFORMULA_MSG_RING_MORE_RING,  //更多铃声
    MMIFORMULA_MSG_RING_MAX_CLASS
}MMIFORMULA_MSG_RING_CLASS_E;
typedef enum
{
    MMIFORMULA_CALL_RING_FIXED,     //固定铃声
    MMIFORMULA_CALL_RING_MORE_RING, //更多音乐
    MMIFORMULA_CALL_RING_MAX_CLASS
} MMIFORMULA_CALL_RING_CLASS_E;

/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/
#ifdef _cplusplus
	}
#endif

#endif//_HELLO_SPRD_H_
	#endif
/*
 *	coreApi.h
 *	Release $Name: MATRIXSSL-3-4-2-COMM $
 *
 *	Prototypes for the Matrix core public APIs
 */
/*
 *	Copyright (c) 2013 INSIDE Secure Corporation
 *	All Rights Reserved
 *
 *	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF INSIDE.
 *
 *	Please do not edit this file without first consulting INSIDE support.
 *	Unauthorized changes to this file are not supported by INSIDE.
 *
 *	The copyright notice above does not evidence any actual or intended
 *	publication of such source code.
 *
 *	This Module contains Proprietary Information of INSIDE and should be
 *	treated as Confidential.
 *
 *	The information in this file is provided for the exclusive use of the
 *	licensees of INSIDE. Such users have the right to use, modify,
 *	and incorporate this code into products for purposes authorized by the
 *	license agreement provided they include this notice and the associated
 *	copyright notice with any such product.
 *
 *	The information in this file is provided "AS IS" without warranty.
 */
/******************************************************************************/

#ifndef _h_PS_COREAPI
#define _h_PS_COREAPI

/******************************************************************************/
#include "coreConfig.h" /* Must be first included */
#include "osdep.h"
#include "matrix_list.h"
#include "psmalloc.h"
#include "sci_types.h"
/******************************************************************************/
/*
	psCore return codes
*/
#define PS_CORE_IS_OPEN		1

/******************************************************************************/
/*
	Universal return codes
*/
#define PS_SUCCESS			0
#define PS_FAILURE			-1	

/*	NOTE: Failure return codes MUST be < 0 */
/*	NOTE: The range for core error codes should be between -2 and -29 */
#define PS_ARG_FAIL			-6	/* Failure due to bad function param */
#define PS_PLATFORM_FAIL	-7	/* Failure as a result of system call error */
#define PS_MEM_FAIL			-8	/* Failure to allocate requested memory */	
#define PS_LIMIT_FAIL		-9	/* Failure on sanity/limit tests */	
#define PS_UNSUPPORTED_FAIL	-10 /* Unimplemented feature error */	
#define PS_DISABLED_FEATURE_FAIL -11 /* Incorrect #define toggle for feature */
#define PS_PROTOCOL_FAIL	-12 /* A protocol error occurred */
#define PS_TIMEOUT_FAIL		-13 /* A timeout occurred and MAY be an error */
#define PS_INTERRUPT_FAIL	-14 /* An interrupt occurred and MAY be an error */
#define PS_PENDING			-15 /* In process. Not necessarily an error */ 
#define PS_EAGAIN			-16 /* Try again later. Not necessarily an error */ 

#define	PS_TRUE		1
#define	PS_FALSE 	0	

/******************************************************************************/
/* Public structures */
/******************************************************************************/
/*
	psBuf_t
	Empty buffer:
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|.|
	 ^
	 \end
	 \start
	 \buf
	 size = 16
	 len = (end - start) = 0

	Buffer with data:

     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	|.|.|a|b|c|d|e|f|g|h|i|j|.|.|.|.|
	 ^   ^                   ^
	 |   |                   \end
	 |   \start
	 \buf
	size = 16
	len = (end - start) = 10

	Read from start pointer
	Write to end pointer
	Free from buf pointer
*/
typedef struct {
	unsigned char	*buf;	/* Pointer to the start of the buffer */
	unsigned char	*start;	/* Pointer to start of valid data */
	unsigned char	*end;	/* Pointer to first byte of invalid data */
	int32			size;	/* Size of buffer in bytes */
} psBuf_t;

/******************************************************************************/
/*
	Public APIs
*/
/******************************************************************************/
PSPUBLIC int32		psCoreOpen(void);
PSPUBLIC void		psCoreClose(void);
PSPUBLIC void		psBurnStack(uint32 len);


#ifdef USE_MATRIX_MEMORY_MANAGEMENT
/*
	Deterministic memory support
*/
PSPUBLIC psPool_t*	psOpenPool(char *name, uint32 size, int32 flags,
							void *staticAddr);
PSPUBLIC void		psClosePool(psPool_t *pool);
PSPUBLIC int32		psAddPoolCache(psPool_t *pool, uint32 size);
PSPUBLIC void*		psMalloc(psPool_t *pool, size_t size);
PSPUBLIC void		psFree(void *ptr);
PSPUBLIC void*		psCalloc(psPool_t *pool, size_t n, size_t size);
PSPUBLIC void*		psRealloc(void *ptr, size_t n);
PSPUBLIC void		psDefineHeap(void *heap, int32 bytes);

PSPUBLIC void		psShowPoolStats(psPool_t *pool);
#endif /* USE_MATRIX_MEMORY_MANAGEMENT */

extern int32 psMallocAlign(psPool_t *pool, unsigned char **memptr, 
							unsigned char **trueptr, size_t size);
extern int32 psReallocAlign(unsigned char **memptr, unsigned char **trueptr,
							size_t size);


/******************************************************************************/
/*
	Public interface to OS-dependant core functionality
	
	OS/osdep.c must implement the below functions
*/
PSPUBLIC int32		psGetEntropy(unsigned char *bytes, uint32 size);

PSPUBLIC int32		psGetTime(psTime_t *t);
PSPUBLIC int32		psDiffMsecs(psTime_t then, psTime_t now);
PSPUBLIC int32		psCompareTime(psTime_t a, psTime_t b);

#ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32		psGetFileBuf(psPool_t *pool, const char *fileName,
								 unsigned char **buf, int32 *bufLen);
#endif /* MATRIX_USE_FILE_SYSTEM */

#ifdef USE_MULTITHREADING
PSPUBLIC int32		psCreateMutex(psMutex_t *mutex);
PSPUBLIC int32		psLockMutex(psMutex_t *mutex);
PSPUBLIC int32		psUnlockMutex(psMutex_t *mutex);
PSPUBLIC void		psDestroyMutex(psMutex_t *mutex);
#endif /* USE_MULTITHREADING */

/******************************************************************************/
/*
	Internal list helpers
*/
extern int32 psParseList(psPool_t *pool, char *list, const char separator,
				psList_t **items);
extern void psFreeList(psList_t *list);

#endif /* _h_PS_COREAPI */
/******************************************************************************/


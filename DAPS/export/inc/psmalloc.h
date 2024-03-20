/*
 *	psmalloc.h
 *	Release $Name: MATRIXSSL-3-4-2-COMM $
 *	
 *	Header for psMalloc functions
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

#ifndef _h_PS_MALLOC
#define _h_PS_MALLOC
#include "os_api.h"
/******************************************************************************/
/*
*/
#ifdef PS_UNSUPPORTED_OS
    #include "matrixos.h"
#else
/******************************************************************************/
/*
*/

#include <string.h> /* memset, memcpy */

#define MATRIX_NO_POOL		(void *)0x0

#ifdef USE_MATRIX_MEMORY_MANAGEMENT

/******************************************************************************/
/*
	Define the system equivalent for malloc and free
	If they are not available, don't define this and use psDefineHeap at 
		runtime to provide a static block of memory to use.
	psDefineHeap will also override USE_SYSTEM_MALLOC, so they can both be used
		simultaneously.
 */
#define USE_SYSTEM_MALLOC	/* Undefine if not using system malloc api */

#ifdef USE_SYSTEM_MALLOC
#include <stdlib.h> 	/* malloc, free, etc... */
#define sysMalloc(A)	malloc(A)
#define sysFree(A)		free(A)
#endif /* USE_SYSTEM_MALLOC */
#include "sci_types.h"
/******************************************************************************/
/*
	Define to enable memory statistics, level determines detail
	Level 1 = Pool stats on close (for tuning)
	Level 3 = Pool stats and every alloc/free/realloc/calloc 
*/
/* #define MEM_TABLE */
/* #define MEM_STATS */
#define MEM_STATS_LEVEL		1	/* Should be 1 or 3 */
#define MAX_BLOCK_STATS		64	/* Should be an even number */

/*
	Tuning for the number of cacheable block sizes. Set to 16 to handle numerous
	platforms by default. Should be optimized to fit specific platform needs.
*/
#define MAX_CACHE_TYPES		16

/******************************************************************************/
/*
	Define USE_LARGE_POOLS to enable pools larger than 64KB. This causes
	16 bytes more overhead per allocation, and the minimum block size becomes
	32 bytes.
	"Magic" values to help detect memory corruption and double frees
*/
/* #define USE_LARGE_POOLS */

#ifndef USE_LARGE_POOLS
#define PS_MAGIC_ALLOCED		0xF00D
#define PS_MAGIC_SPLIT			0xC0ED
#define PS_MAGIC_FREED			0xFEED
#define MIN_BLOCK_SIZE		16	/* Smallest allocatable block, multiple of 8 */
#else
#define PS_MAGIC_ALLOCED		0xF00DC0DE
#define PS_MAGIC_SPLIT			0xC0EDBABE
#define PS_MAGIC_FREED			0xFEEDBEEF
#define MIN_BLOCK_SIZE		32	/* Smallest allocatable block, multiple of 8 */
#endif

#define POOL_TYPE_MALLOC	0	/* Pool was created with sysMalloc() */
#define	POOL_TYPE_STATIC	1	/* Pool is in a static buffer */
#define	POOL_TYPE_HEAP		2	/* Pool is in global pool (g_pool) */

/*
	Each allocated block has the following header prepended
*/
typedef struct psMemHeader {
#ifndef USE_LARGE_POOLS
	unsigned short		pad;	/* Pad to 8 byte boundary, holds magic value */
	unsigned short		plen;	/* Number of bytes to previous mem block */
	unsigned short		nlen;	/* Number of bytes to next mem block */
	unsigned short		size;	/* Size in usable bytes of this mem block */
#else
	unsigned int		pad;	/* Pad to 8 byte boundary, holds magic value */
	unsigned int		plen;	/* Number of bytes to previous mem block */
	unsigned int		nlen;	/* Number of bytes to next mem block */
	unsigned int		size;	/* Size in usable bytes of this mem block */
#endif
	DLListEntry			List;	/* 2 pointers */
} psMemHeader_t;

/*
	Each memory pool has the associated data
*/
typedef struct {
	uint32			size;		/* Total size of pool */
	int32			remaining;	/* Free bytes remaining */
	int32			type;		/* System malloc, heap, or static mem */
	int32			flags;		/* Implementation specific usage */
	/* Cached blocks */
	uint32			cacheSize[MAX_CACHE_TYPES];	/* Cache sizes */
	DLListEntry		cache[MAX_CACHE_TYPES];
	DLListEntry		heap[MAX_CACHE_TYPES];
#ifdef MEM_STATS
	/* Statistics */
	int32		memAlloced;	/* Outstanding memory allocations */
	int32		highWater;	/* Max allocated memory in pool lifetime */
	int32		numTypes;	/* Number of distinct allocation sizes requested */
	int32		numMerge;	/* Number of block merges we were able to do */
	int32		numSplit;	/* Number of block splits */
	int32		maxRemainder;	/* Largest unused remainder */
	int32		blockCount[MAX_BLOCK_STATS];	/* Number of blocks this size */
	int32		blockWater[MAX_BLOCK_STATS];	/* High water for this size */
	int32		blockCurr[MAX_BLOCK_STATS];		/* Allocated blocks this size */
	int32		blockSplit[MAX_BLOCK_STATS];	/* Allocated blocks this size */
	int32		blockSize[MAX_BLOCK_STATS];		/* Block to size index table */
	char		name[16];
#endif
} psPool_t;

extern int32		psOpenMalloc(void);
extern void			psCloseMalloc(void);
extern void			*psMallocNoPool(size_t size);
#define psMemset	memset
#define psMemcpy	memcpy

#else /* ! USE_MATRIX_MEMORY_MANAGEMENT */
/******************************************************************************/
/*
	Native memory routines
*/
#include <stdlib.h> 		/* malloc, free, etc... */

#define MAX_MEMORY_USAGE	0
#define psOpenMalloc()		0
#define psCloseMalloc()
#define psDefineHeap(A, B)
#define psAddPoolCache(A, B)
#define psMalloc(A, B)		SCI_ALLOCA(B)//malloc(B)
#define psCalloc(A, B, C)	SCI_CALLOC(B,C)//calloc(B, C)
#define psMallocNoPool		SCI_ALLOCA//malloc
#ifdef WIN32
#define psRealloc(A, B)	    realloc(A, B)//malloc(B)
#else
#define psRealloc			SCI_REALLOC//realloc
#endif
#define psFree				SCI_FREE//free
#define psMemset			SCI_MEMSET//memset
#define psMemcpy			SCI_MEMCPY//memcpy

typedef int32 psPool_t;

#endif /* USE_MATRIX_MEMORY_MANAGEMENT */

/******************************************************************************/

#endif /* !PS_UNSUPPORTED_OS */
#endif /* _h_PS_MALLOC */
/******************************************************************************/


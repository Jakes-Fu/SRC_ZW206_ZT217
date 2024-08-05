#ifndef __LEBAO_RINGSET_H
#define __LEBAO_RINGSET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mmiset_export.h"

void lebao_print_current_ringset(void);
int lebao_set_ringtone(const char* filename, const char* songName, const char* singer);
int lebao_query_ringtone(MMISET_CALL_MORE_RING_T* filePath, MMISET_CALL_MORE_RING_T* songName);
int lebao_set_default_ringtone(void);
int lebao_del_default_ringtone(void);

#ifdef __cplusplus
}
#endif

#endif
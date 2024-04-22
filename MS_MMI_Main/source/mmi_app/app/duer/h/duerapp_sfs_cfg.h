#ifndef DUERAPP_SFS_CFG_H
#define DUERAPP_SFS_CFG_H

#include "sci_types.h"
/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern   "C"
{
#endif


int duer_get_config_str(uint8* pKey, uint8*pVal ,uint16* pLen);

int duer_set_config_str(uint8* pKey, uint8* pVal);

int duer_get_config_int(uint8* pKey, int* pVal);

int duer_set_config_int(uint8* pKey, int val);

int duer_sfs_cfg_init();

uint8* duer_read_file(uint16 *path,uint32* length);

BOOLEAN duer_write_file(uint16* file_name,uint8* pBuf, uint32 length);


#ifdef _cplusplus
	}
#endif

#endif //


#ifndef __OS_NV_UPDATE_H__
#define __OS_NV_UPDATE_H__

#include <oneos_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OTA_PROC_SUCCESS
#define OTA_PROC_SUCCESS 1000
#define OTA_PART_WRITE_ERR 1010
#endif

ota_int32 extern_full_patch_handler(ota_uint32 app, ota_uint32 addr, ota_uint32 size);

#ifdef __cplusplus
}
#endif

#endif


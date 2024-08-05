#include "port_cfg.h"
#include "lebao_ringset.h"

#include "std_header.h"
#include "mmk_app.h"
#include "mmi_common.h"
#include "mmi_module.h"
#include "mmiset_export.h"

#include "lebao_misc.h"

// public variables

// private variables

// private prototypes

//-------------------------------ringtone-------------------------------

extern MMISET_CALL_RING_T MMIAPIENVSET_GetCallRingInfo(MN_DUAL_SYS_E dual_sys, uint8 mode_id);

// W307 / W217
#if defined(PLATFORM_ANTISW3) || defined(PLATFORM_UWS6121E)
extern BOOLEAN MMIAPIENVSET_SetMoreCallRingInfo(
	MN_DUAL_SYS_E dual_sys,
	MMISET_CALL_MORE_RING_T ring_info,
	uint8 mode_id
);
#else
extern BOOLEAN MMIAPIENVSET_SetMoreCallRingInfo(
	MN_DUAL_SYS_E dual_sys,
	MMISET_CALL_MORE_RING_T* ring_info,
	uint8 mode_id
);
#endif

 void lebao_print_current_ringset(void)
{
	MMISET_CALL_RING_T call_ring_info = { MMISET_CALL_RING_FIXED, 0, 0 };

	call_ring_info = MMIAPIENVSET_GetCallRingInfo(MMIAPISET_GetActiveSim(), MMIENVSET_GetCurModeId());

	if (MMISET_CALL_RING_MORE_RING == call_ring_info.call_ring_type) {
		char path[256] = { 0 };
		GUI_WstrToUTF8(path, 255, call_ring_info.more_ring_info.name_wstr, call_ring_info.more_ring_info.name_wstr_len);

		if (!MMIAPIFMM_IsFileExist(call_ring_info.more_ring_info.name_wstr, call_ring_info.more_ring_info.name_wstr_len)) {
			helper_debug_printf("not exist, filename = %s", path);
		}
		else {
			helper_debug_printf("ringtone, filename=%s", path);
		}
	}
	else {
		helper_debug_printf("ringtone, type=%d", call_ring_info.call_ring_type);
	}
}

// for set rintone
int lebao_set_ringtone(const char* filename, const char* songName, const char* singer)
{
	BOOLEAN ret = FALSE;
	MMISET_CALL_MORE_RING_T ring_info = { 0 };
	int len = (filename == NULL) ? 0 : os_strlen(filename);
	if (len <= 0 || len > MMISET_CALL_RING_NAME_MAX_LEN)
		return -1;

	ring_info.name_wstr_len = GUI_GBToWstr(ring_info.name_wstr, (const uint8*)filename, len);
	if (ring_info.name_wstr_len <= 0) return -1;

	if (!MMIAPIFMM_IsFileExist(ring_info.name_wstr, ring_info.name_wstr_len)) {
		helper_debug_printf("not exist, filename = %s", filename);
		return -1;
	}
	
#if defined(PLATFORM_ANTISW3) || defined(PLATFORM_UWS6121E)
	ret = MMIAPIENVSET_SetMoreCallRingInfo(MMIAPISET_GetActiveSim(), ring_info, MMIENVSET_GetCurModeId());
#else
	ret = MMIAPIENVSET_SetMoreCallRingInfo(MMIAPISET_GetActiveSim(), &ring_info, MMIENVSET_GetCurModeId());
#endif

	helper_debug_printf("set ringtone %s, filename = %s", (ret == TRUE) ? "ok" : "failed", filename);
	return ((ret == TRUE) ? 0 : -1);
}

int lebao_query_ringtone(MMISET_CALL_MORE_RING_T* filePath, MMISET_CALL_MORE_RING_T* songName)
{
	const char* ringFile = lebao_get_config()->ringPath;
	const char* ringName = lebao_get_config()->ringName;
		
	if (helper_is_local_file(ringFile) && helper_is_file_exist(ringFile)) {
		if(filePath != NULL && sdslen(ringFile) <= MMISET_CALL_RING_NAME_MAX_LEN) {
			filePath->name_wstr_len = GUI_GBToWstr(filePath->name_wstr, (const uint8*)ringFile, sdslen(ringFile));
		}
	
		if(ringName != NULL && sdslen(ringName) <= MMISET_CALL_RING_NAME_MAX_LEN) {
			songName->name_wstr_len = GUI_UTF8ToWstr(songName->name_wstr,
				MMISET_CALL_RING_NAME_MAX_LEN, (const uint8*)ringName, sdslen(ringName));				
		}
		return 0;
	}

	return -1;
}

int lebao_set_default_ringtone(void)
{
	char* ringFile = lebao_get_config()->ringPath;
	char* ringName = lebao_get_config()->ringName;
	char* ringSinger = lebao_get_config()->ringSinger;

	if (helper_is_local_file(ringFile) && helper_is_file_exist(ringFile)) {
		return lebao_set_ringtone(ringFile, ringName, ringSinger);
	}

	return -1;
}

int lebao_del_default_ringtone(void)
{
	char* ringFile = lebao_get_config()->ringPath;

	if (helper_is_local_file(ringFile) && helper_is_file_exist(ringFile)) {
		helper_delete_file(ringFile);
		
		sdsfree_val(lebao_get_config()->ringPath);
		sdsfree_val(lebao_get_config()->ringName);
		sdsfree_val(lebao_get_config()->ringSinger);
		lebao_save_config(NULL);
	}

	return 0;
}


#include "port_cfg.h"
#include "helper_file.h"
#include "sds.h"

#ifdef REAL_WATCH_RTOS

#include "os_api.h"
#include "sci_types.h"
#include "sfs.h"
#include "mmk_type.h"

#define FILE_DISK_D_NAME            (L"D")
#define FILE_DISK_E_NAME            (L"E")
#define FILE_FULL_PATH_MAX_LEN		(255)

int	helper_fs_sdcard_ready(void)
{
	return (SFS_GetDeviceStatus(FILE_DISK_E_NAME) == SFS_NO_ERROR) ? 1 : 0;
}

const char * helper_get_real_path(const char * path)
{
	return path;
}

os_FILE os_fopen_impl(char const* filename, char const* mode)
{
	SFS_HANDLE file_handle = 0;
	int length = 0;

	if (filename == NULL || mode == NULL)
		return NULL;
	else {
		length = os_strlen(filename);
		if (length > FILE_FULL_PATH_MAX_LEN)
			return NULL;
		else {
			wchar w_filename[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
			uint16 len = 0;

			len = GUI_GBToWstr(w_filename, (const uint8*)filename, length);
			if (len <= 0)
				return NULL;

			if (os_strncmp(mode, "rb", 2) == 0)
				file_handle = SFS_CreateFile(w_filename, SFS_MODE_OPEN_EXISTING | SFS_MODE_READ, NULL, NULL);
			else if (os_strncmp(mode, "wb", 2) == 0)
				file_handle = SFS_CreateFile(w_filename, SFS_MODE_OPEN_ALWAYS | SFS_MODE_WRITE, NULL, NULL);
		}
	}

	return file_handle;
}

size_t os_fwrite_impl(void const* buffer, size_t elementSize, size_t elementCount, os_FILE fileHandle)
{
	uint32 total = elementSize * elementCount;
	uint32 bytes_to_write = total > 2048 ? 2048 : total;
	uint32 bytes_write = 0;
	SFS_ERROR_E rc = SFS_ERROR_ACCESS;
	char const* buf = (const char*)buffer;

	while (bytes_to_write > 0) {
		rc = SFS_WriteFile(fileHandle, buf, bytes_to_write, &bytes_write, NULL);
		if (rc != SFS_ERROR_NONE) {
			return -1;
		}

		total -= bytes_write;
		if (total <= 0 || bytes_write <= 0)
			break;

		buf += bytes_write;
		bytes_to_write = total > 2048 ? 2048 : total;
	}

	return (elementSize * elementCount - total); // (size_t)(buf - (const char *)buffer);
}

int os_fclose_impl(os_FILE fileHandle)
{
	if (fileHandle == NULL)
		return -1;

	return SFS_CloseFile(fileHandle);
}

int os_rename_impl(char const* oldFileName, char const* newFileName)
{
	int oldLen = 0, newLen = 0;

	if (oldFileName == NULL || newFileName == NULL)
		return -1;

	oldLen = os_strlen(oldFileName); // oldLen <= 0 ?
	newLen = os_strlen(newFileName);

	if (oldLen > FILE_FULL_PATH_MAX_LEN || newLen > FILE_FULL_PATH_MAX_LEN)
		return -1;
	else {
		wchar w_oldFileName[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
		wchar w_newFileName[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };

		oldLen = GUI_GBToWstr(w_oldFileName, (const uint8*)oldFileName, oldLen);
		if (oldLen <= 0)
			return -1;

		newLen = GUI_GBToWstr(w_newFileName, (const uint8*)newFileName, newLen);
		if (newLen <= 0)
			return -1;

		return SFS_RenameFile(w_oldFileName, w_newFileName, NULL);
	}

	return 0;
}

int	os_remove_impl(char const* fileName)
{
	return helper_delete_file(fileName);
}

size_t os_fread_impl(void * buffer, size_t elementSize, size_t elementCount, os_FILE fileHandle)
{
	uint32 total = elementSize * elementCount;
	uint32 bytes_to_read = total > 4096 ? 4096 : total;
	uint32 bytes_read = 0;
	SFS_ERROR_E rc = SFS_ERROR_ACCESS;
	char const* buf = (const char*)buffer;

	while (bytes_to_read > 0) {
		rc = SFS_ReadFile(fileHandle, buf, bytes_to_read, &bytes_read, NULL);
		if (rc != SFS_ERROR_NONE) {
			return 0;
		}

		total -= bytes_read;
		if (total <= 0 || bytes_read <= 0)
			break;

		buf += bytes_read;
		bytes_to_read = total > 4096 ? 4096 : total;
	}

	return (elementSize * elementCount - total); // (size_t)(buf - (const char *)buffer);
}

int os_fseek_impl(os_FILE fileHandle, long offset, int origin)
{
	if (fileHandle == NULL)
		return -1;

	return SFS_SetFilePointer(fileHandle, offset, origin);
}

int helper_delete_file(const char* filename)
{
	int error = SFS_NO_ERROR;
	if (NULL == filename) {
		return SFS_ERROR_INVALID_PARAM;
	}
	else {
		wchar w_filename[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
		uint16 len = 0;
		len = GUI_GBToWstr(w_filename, (const uint8*)filename, os_strlen(filename));
		// check len > 3
		error = SFS_DeleteFile(w_filename, NULL);
	}

	return error;
}

int helper_is_file_exist(const char* filename)
{
	int res = SFS_ERROR_NOT_EXIST;
	unsigned short attr;

	if (NULL == filename) {
		return 0;// SFS_ERROR_INVALID_PARAM;
	}
	else {
		wchar w_filename[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
		uint16 len = 0;
		len = GUI_GBToWstr(w_filename, (const uint8*)filename, os_strlen(filename));
		if (len > 0) {
			res = SFS_GetAttr(w_filename, &attr);
		}
	}

	if (res == SFS_NO_ERROR && !(attr & SFS_ATTR_DIR)) {
		return 1;
	}
	//	(res == SFS_ERROR_NOT_EXIST) 

	return 0;
}

int helper_is_folder_exist(const char* folderPath)
{
	int res = SFS_ERROR_NOT_EXIST;
	unsigned short attr;

	if (NULL == folderPath) {
		return 0;// SFS_ERROR_INVALID_PARAM;
	}
	else {
		wchar w_path[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
		uint16 len = 0;
		len = GUI_GBToWstr(w_path, (const uint8*)folderPath, os_strlen(folderPath));
		if (len > 0) {
			res = SFS_GetAttr(w_path, &attr);
		}
	}

	if (res == SFS_NO_ERROR && (attr & SFS_ATTR_DIR)) {
		return 1;
	}

	return 0;
}

int helper_new_folder(const char* dstFolderPath)
{
	int error = SFS_ERROR_FOLD_FULL;

	if (NULL == dstFolderPath)
		return SFS_ERROR_INVALID_PARAM;
	else {
		//Todo: Check if the parent directory exists

		wchar w_path[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
		uint16 len = 0;
		len = GUI_GBToWstr(w_path, (const uint8*)dstFolderPath, os_strlen(dstFolderPath));
		if (len > 0) {
			error = SFS_CreateDirectory(w_path);
		}
	}

	return error;
}

int helper_clear_cache(const char* dstFolderPath, const char* prefix, const char* keyword)
{
	SFS_HANDLE	hFind = NULL;
	SFS_FIND_DATA_T sfsfind = { 0 };
	SFS_ERROR_E		result = SFS_NO_ERROR;
	wchar   full_path[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
	uint16  full_path_len = 0;

	wchar   w_keyword[FILE_FULL_PATH_MAX_LEN + 1] = { 0 };
	uint16  w_keyword_len = 0;

	if (dstFolderPath == NULL || os_strlen(dstFolderPath) <= 0 || keyword == NULL)
		return SFS_ERROR_INVALID_PARAM;

	w_keyword_len = GUI_GBToWstr(w_keyword, (const uint8*)keyword, os_strlen(keyword));

	full_path_len = GUI_GBToWstr(&full_path[full_path_len], (const uint8*)dstFolderPath, os_strlen(dstFolderPath));
	if ('/' != full_path[full_path_len - 1] && 0x5c != full_path[full_path_len - 1]) {
		full_path[full_path_len++] = 0x5c;
	}

	full_path[full_path_len] = 0x2a;
	full_path[full_path_len + 1] = 0;

	hFind = SFS_FindFirstFile(full_path, &sfsfind);

	if (hFind == NULL)
		return SFS_NO_ERROR;

	do {
		uint32 uNameLen = 0;
		uNameLen = MMIAPICOM_Wstrlen((wchar *)sfsfind.name);
		SCI_MEMCPY(&full_path[full_path_len], sfsfind.name, MIN(uNameLen * 2 + 2, FILE_FULL_PATH_MAX_LEN));
		full_path[full_path_len + uNameLen] = '\0';

		if (sfsfind.attr & SFS_ATTR_DIR) {
		}
		else {
			if (MMIAPICOM_Wstrncmp(sfsfind.name, w_keyword, w_keyword_len) == 0) {
				result = SFS_DeleteFile(full_path, PNULL);

				if (SFS_NO_ERROR != result) {
					SFS_FindClose(hFind);
					return result;
				}
			}
		}

		result = SFS_FindNextFile(hFind, &sfsfind);

	} while (SFS_NO_ERROR == result);

	SFS_FindClose(hFind);
	return SFS_NO_ERROR;
}

int helper_clear_image(const char* dstFolderPath)
{
	return helper_clear_cache(dstFolderPath, "_lb_tmp_*", "_lb_tmp_");
}

int helper_clear_mp3(const char* dstFolderPath)
{
	return helper_clear_cache(dstFolderPath, "_lb_mp3_*", "_lb_mp3_");
}

sds helper_load_file(const char* filename) {
	SFS_HANDLE file_handle = 0;
	sds data = NULL;
	int length = 0;

	if (filename == NULL)
		return NULL;
	else {
		file_handle = os_fopen_impl(filename, "rb");
	}

	if (file_handle != NULL) {
		uint32 bytes_read = 0;
		SFS_ERROR_E rc = SFS_ERROR_ACCESS;
		uint32 file_size = 0;
		rc = SFS_GetFileSize(file_handle, &file_size);

		if (rc != SFS_NO_ERROR || file_size <= 0) {
			os_fclose_impl(file_handle);
			return NULL;
		}

		data = sdsMakeRoomFor(sdsempty(), file_size); // Todo : check max size

		bytes_read = os_fread_impl(data, 1, file_size, file_handle);
		if (bytes_read > 0) {
			sdsIncrLen(data, bytes_read);
		}
		else {
			sdsfree(data);
			data = NULL;
		}

		os_fclose_impl(file_handle);
	}

	return data;
}

int helper_save_file(const char* filename, sds data) {
	return helper_save_file_limit(filename, data, 0);
}

int helper_save_file_limit(const char* filename, sds data, const int limitSize) {
	SFS_HANDLE file_handle = 0;
	int writeSize = 0;

	//Todo: max length limited
	if (filename == NULL || data == NULL || sdslen(data) <= 0)
		return -1;
	else {
		if (limitSize > 0)
			writeSize = (sdslen(data) > limitSize) ? limitSize : sdslen(data);
		else
			writeSize = sdslen(data);

		if (helper_get_user_space(filename) < writeSize)
			return -1;

		// Todo : Check if the parent directory exists
		if (helper_is_file_exist(filename) > 0)
			helper_delete_file(filename);

		file_handle = os_fopen_impl(filename, "wb");
	}

	if (file_handle != NULL) {
		size_t ret = -1;
		if (limitSize > 0)
			ret = os_fwrite_impl(data, 1, writeSize, file_handle);
		else
			ret = os_fwrite_impl(data, 1, writeSize, file_handle);

		os_fclose_impl(file_handle);
		return (ret > 0 ? 0 : -1);
	}

	return -1;
}

long helper_get_file_size(const char* filename) {
	SFS_HANDLE file_handle = 0;
	int length = 0;

	if (filename == NULL)
		return -1;

	length = os_strlen(filename);
	if (length <= 0 || length > FILE_FULL_PATH_MAX_LEN)
		return -1;
	else {
		file_handle = os_fopen_impl(filename, "rb");
	}

	if (file_handle != NULL) {
		SFS_ERROR_E rc = SFS_ERROR_ACCESS;
		uint32 file_size = 0;
		rc = SFS_GetFileSize(file_handle, &file_size);

		if (rc != SFS_NO_ERROR || file_size < 0)
			file_size = -1;
		SFS_CloseFile(file_handle);
		return file_size;
	}

	return -1;
}

int	helper_is_local_file(const char* filename)
{
	if (filename != NULL && os_strlen(filename) > 2) {

		return (
			((filename[0] == 'C' || filename[0] == 'D' || filename[0] == 'E') && filename[1] == ':')
			|| (filename[0] == '.' && (filename[1] == '/') || filename[1] == '\\')
			|| filename[0] == '/'
			) ? 1 : 0;
	}

	return 0;
}

int helper_space_is_enough(const char * devname, const int minSize)
{
	int size = (minSize <= 0) ? CFG_SPACE_MIN_SIZE : minSize;
	unsigned long left = helper_get_user_space(devname);

	helper_debug_printf("space left (%u), need (%d)\n", left, minSize);
	return (helper_get_user_space(devname) > size) ? 1 : 0;
}

unsigned long helper_get_user_space(const char * devname)
{
	uint32 high = 0;
	uint32 low = 0;
	int ret = 0;

	if(devname[0] == 'E')
		ret = SFS_GetDeviceFreeSpace((uint16*)FILE_DISK_E_NAME, &high, &low);
	else
		ret = SFS_GetDeviceFreeSpace((uint16*)FILE_DISK_D_NAME, &high, &low);

//	if (high >= 1) {
//		low |= 0x7FFFFFF;
//	}

	if (SFS_NO_ERROR != ret)
		return 0;

	return low;
}

#endif

sds helper_escape_path_sep_sds(const sds src)
{
	int i = 0, j = 0;
	sds dst = NULL;
	if (src == NULL || sdslen(src) <= 0)
		return dst;

	dst = sdsMakeRoomFor(sdsempty(), sdslen(src) * 2 + 1);
	while (src[i] != '\0' && i < (int)sdslen(src)) {
		dst[j] = src[i];
		if (src[i] == '\\') {
			++j;
			dst[j] = '\\';
		}

		++i;
		++j;
	}
	dst[j] = '\0';
	sdsIncrLen(dst, j);
	return dst;
}

sds helper_un_escape_path_sep_sds(const sds src)
{
	int i = 0, j = 0;
	sds dst = NULL;
	if (src == NULL || sdslen(src) <= 0)
		return dst;

	dst = sdsMakeRoomFor(sdsempty(), sdslen(src) * 2 + 1);
	while (src[i] != '\0' && i < (int)sdslen(src)) {
		dst[j] = src[i];
		if (src[i] == '\\' && (i + 1) < (int)sdslen(src) && src[i + 1] == '\\') {
			++i;
			continue;
		}

		++i;
		++j;
	}
	dst[j] = '\0';
	sdsIncrLen(dst, j);
	return dst;
}

int helper_get_filename_from_uri(const char* uri, char * filename, const int size)
{
	size_t uri_len = 0;
	char const* resource_start = NULL;
	char const* resource_end = NULL;

	if (uri == NULL || filename == NULL || size <= 0) return 0;
	if (os_strncmp(uri, "http://", 7) != 0 && os_strncmp(uri, "https://", 8) != 0) return 0;

	uri_len = os_strlen(uri);

	resource_end = os_strchr(uri, '?');
	if (!resource_end) resource_end = uri + uri_len;

	resource_start = resource_end;
	while (resource_start != uri) {
		if (*resource_start == '/')
			break;
		--resource_start;
	}
	if (resource_start == uri) return 0;

	++resource_start;

	resource_end = os_strchr(resource_start, '?');
	if (!resource_end) resource_end = uri + uri_len;

	if ((int)(resource_end - resource_start) > (int)(size + os_strlen("_lb_xxx_")) || (resource_end == resource_start))
		return 0;

	//	os_strcpy(filename, "_lb_mp3_");
	os_strncat(filename, resource_start, (resource_end - resource_start));
	return 1;
}

int helper_get_cachename_from_uri(const char* uri, char * filename, const int size)
{
	size_t uri_len = 0;
	char const* resource_start = NULL;
	char const* resource_end = NULL;
	char const* ext_start = NULL;

	if (uri == NULL || filename == NULL || size <= 0) return 0;
	if (os_strncmp(uri, "http://", 7) != 0 && os_strncmp(uri, "https://", 8) != 0) return 0;

	uri_len = os_strlen(uri);

	resource_end = os_strchr(uri, '?');
	if (!resource_end) resource_end = uri + uri_len;

	resource_start = resource_end;
	while (resource_start != uri) {
		if (*resource_start == '/')
			break;
		--resource_start;
	}
	if (resource_start == uri) return 0;

	++resource_start;

	ext_start = os_strrchr(resource_start, '.');
	if (!ext_start) ext_start = resource_end;

	if ((int)(ext_start - resource_start) > (int)(size + os_strlen("_lb_tmp_")) || ext_start == resource_start)
		return 0;

	//	os_strcpy(filename, "_lb_tmp_");
	os_strncat(filename, resource_start, (ext_start - resource_start));
	os_strncat(filename, ext_start, (resource_end - ext_start));
	return 1;
}

int	helper_str_start_with(const char * src, const char* dst)
{
	size_t len = 0;
	if (src == NULL || dst == NULL) return 0;
	len = os_strlen(dst);
	if (os_strlen(src) < len) return 0;

	return (os_strncmp(dst, src, len) == 0) ? 1 : 0;
}

int	helper_str_end_with(const char* src, const char* dst)
{
	size_t srcLen = 0, dstLen = 0;
	char* ptr = NULL;
	if (src == NULL || dst == NULL) return 0;
	srcLen = os_strlen(src);
	dstLen = os_strlen(dst);
	if (srcLen < dstLen) return 0;

	ptr = (char*)src + srcLen - dstLen;
	return (os_strncmp(ptr, dst, dstLen) == 0) ? 1 : 0;
}

int	helper_pstr_end_with(const char* src, const int srcLen, const char* dst)
{
	size_t dstLen = 0;
	char* ptr = NULL;
	if (src == NULL || dst == NULL || srcLen <= 0) return 0;
	dstLen = os_strlen(dst);
	if ((size_t)srcLen < dstLen) return 0;

	ptr = (char*)src + srcLen - dstLen;
	return (os_strncmp(ptr, dst, dstLen) == 0) ? 1 : 0;
}

sds helper_get_uri_param_value(const char* data, const int dataLen, const char* param, const int paramLen)
{
	char const* start = 0;
	char const* end = 0;

	sds p = sdscatprintf(sdsempty(), "&%s=", param);

	start = os_strstr(data, p);
	if (start == NULL) {
		sdsfree(p);
		p = sdscatprintf(sdsempty(), "?%s=", param);
		start = os_strstr(data, p);
		if (start == NULL) {
			sdsfree(p);
			return NULL;
		}
	}

	sdsfree(p);
	start = start + 1;
	end = os_strstr(start, "&");
	if (end == NULL) {
		end = start + dataLen;
	}

	if (*(start - 1) != '?') {
		start += (paramLen + 1);
		return sdsnewlen(start, end - start);
	}
	else {
		start += (paramLen + 1);
		return sdsnewlen(start, end - start);
	}

	return NULL;
}

typedef unsigned char utf8;  // UTF-8 8bits.
typedef unsigned short ucs2; // Unicode2 16bits.

static int helper_ucs2utf8(utf8 *dst, ucs2 *src)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int next = 0;

	while (src[i] != 0) {
		if (src[i] < 0x80) {
			next = 1;
			dst[j] = 0;
			dst[j] = (utf8)src[i];
		}
		else if (src[i] < 0x800) {
			next = 2;
			dst[j] = 0;
			dst[j + 1] = 0;

			dst[j + 1] = (utf8)((src[i] & 0x3F) | 0x80);
			dst[j] = (utf8)(((src[i] & 0x3F) & 0x1F) | 0xC0);
		}
		else {
			next = 3;
			dst[j] = 0;
			dst[j + 1] = 0;
			dst[j + 2] = 0;

			dst[j] |= ((((utf8)(src[i] >> 12)) & 0xF) | 0xE0);
			dst[j + 1] |= (((utf8)(src[i] >> 6) & 0x3F) | 0x80);
			dst[j + 2] |= (((utf8)(src[i] >> 0) & 0x3F) | 0x80);
		}

		j += next;
		i++;
	}
	dst[j] = 0;
	return j;
}

sds	helper_un_escape_unicode_to_utf8(sds* uni)
{
	int i = 0, j = 0, hex = 0, len = 0;
	char str[16] = { 0 };
	ucs2 ucs[16] = { 0 };
	sds dst = NULL;
	sds tmp = NULL;
	sds src = *uni;
	if (uni == NULL || sdslen(src) <= 0)
		return src;

	len = sdslen(src);
	dst = sdsMakeRoomFor(sdsempty(), len * 2);
	while (src[i] != '\0' && i < len) {
		if (src[i] == '\\' && (i + 1) < len && src[i + 1] == 'u') {
			i += 2;

			os_memcpy(str, src + i, 4);
			os_sscanf(str, "%04x", &hex);
			ucs[0] = hex;

			j += helper_ucs2utf8(dst + j, ucs);
			i += 4;
			continue;
		}

		dst[j] = src[i];

		++i;
		++j;
	}
	dst[j] = '\0';
	sdsIncrLen(dst, j);

	tmp = (*uni);
	(*uni) = dst;
	sdsfree(tmp);
	return (*uni);
}

sds helper_un_escape_url_sep(sds* url)
{
	int i = 0, j = 0;
	sds dst = NULL, tmp = NULL, src = *url;
	if (src == NULL || sdslen(src) <= 0)
		return src;

	dst = sdsMakeRoomFor(sdsempty(), sdslen(src) * 2);
	while (src[i] != '\0' && i < (int)sdslen(src)) {
		if (src[i] == '\\' && (i + 1) < (int)sdslen(src) && src[i + 1] == '/') {
			++i;
			continue;
		}

		dst[j] = src[i];

		++i;
		++j;
	}
	dst[j] = '\0';
	sdsIncrLen(dst, j);

	tmp = (*url);
	(*url) = dst;
	sdsfree(tmp);
	return (*url);
}

int helper_get_image_size(char const* img, const int size, int* width, int* height)
{
    int w = 0, h = 0, ext = FMT_IMG_LEBAO_UNKNOWN;
    char const* ptr = img;

    if (width != NULL) *width = 0;
    if (height != NULL) *height = 0;

    if (img == NULL || size <= 54) // BMP header size
        return -1;

    if ((ptr[0] == 'B') && (ptr[1] == 'M')) // min size > 54
        ext = FMT_IMG_LEBAO_BMP;
    else if (ptr[0] == '\x89' && ptr[1] == 'P' && ptr[2] == 'N' && ptr[3] == 'G') // size > 24 or size > 40
        ext = FMT_IMG_LEBAO_PNG;
    else if (ptr[0] == '\xFF' && ptr[1] == '\xD8') // SOI(0xFFD8)
        ext = FMT_IMG_LEBAO_JPG;
    else if (ptr[0] == 'G' && ptr[1] == 'I' && ptr[2] == 'F') // size > 12
        ext = FMT_IMG_LEBAO_GIF;
    else
        return -1;

    switch (ext)
    {
    case FMT_IMG_LEBAO_BMP:
        w = (unsigned int)((ptr[0x12] & 0xFF) | ((ptr[0x13] << 8) & 0xFF00) | ((ptr[0x14] << 16) & 0xFF0000) | ((ptr[0x15] << 24) & 0xFF000000));
        h = (unsigned int)((ptr[0x16] & 0xFF) | ((ptr[0x17] << 8) & 0xFF00) | ((ptr[0x18] << 16) & 0xFF0000) | ((ptr[0x19] << 24) & 0xFF000000));
        break;

    case FMT_IMG_LEBAO_PNG:
        if (ptr[12] == 'I' && ptr[13] == 'H' && ptr[14] == 'D' && ptr[15] == 'R') {
            w = (unsigned int)(((ptr[0x10] << 24) & 0xFF000000) | ((ptr[0x11] << 16) & 0xFF0000) | ((ptr[0x12] << 8) & 0xFF00) | (ptr[0x13] & 0xFF));
            h = (unsigned int)(((ptr[0x14] << 24) & 0xFF000000) | ((ptr[0x15] << 16) & 0xFF0000) | ((ptr[0x16] << 8) & 0xFF00) | (ptr[0x17] & 0xFF));
        }
        else if (ptr[12] == 'C' && ptr[13] == 'g' && ptr[14] == 'B' && ptr[15] == 'I') {
            w = (unsigned int)(((ptr[0x20] << 24) & 0xFF000000) | ((ptr[0x21] << 16) & 0xFF0000) | ((ptr[0x22] << 8) & 0xFF00) | (ptr[0x23] & 0xFF));
            h = (unsigned int)(((ptr[0x24] << 24) & 0xFF000000) | ((ptr[0x25] << 16) & 0xFF0000) | ((ptr[0x26] << 8) & 0xFF00) | (ptr[0x27] & 0xFF));
        }
        break;

    case FMT_IMG_LEBAO_JPG: {
        unsigned int offset = 2; // 0xFFD8
        unsigned int length = 0;
        ptr = img;

        offset += 2; // 0xFFE0, skip APP[0]
        while (offset + 9 <= (unsigned int)size) {
            ptr = &(img[offset - 2]);

            length = (unsigned int)(((ptr[2] << 8) & 0xFF00) | (ptr[3] & 0xFF));

            // SOF0: 0xFFC0, SOF1: 0xFFC1, SOF2£º0xFFC2
            if (ptr[0] == '\xFF' && (ptr[1] == '\xC0' || ptr[1] == '\xC1' || ptr[1] == '\xC2')) {
                h = (unsigned int)(((ptr[0x05] << 8) & 0xFF00) | (ptr[0x6] & 0xFF));
                w = (unsigned int)(((ptr[0x07] << 8) & 0xFF00) | (ptr[0x8] & 0xFF));
                break;
            }

            offset += (length + 2);
        }
    } break;

    case FMT_IMG_LEBAO_GIF:
        w = (unsigned int)(((ptr[0x07] << 8) & 0xFF00) | (ptr[0x6] & 0xFF));
        h = (unsigned int)(((ptr[0x09] << 24) & 0xFF000000) | (ptr[0x08] & 0xFF));
        break;

    default:
        if (width != NULL) *width = 0;
        if (height != NULL) *height = 0;

        return -1;
    }

    if (width != NULL) *width = w;
    if (height != NULL) *height = h;

    return (w > 0 && h > 0) ? ext : 0;
}

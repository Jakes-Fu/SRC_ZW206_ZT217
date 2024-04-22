
#include "os_api.h"
#include "cJson.h"
#include "sfs.h"
#include "lightduer_log.h"
#include "lightduer_mutex.h"
#include "lightduer_memory.h"
#include "mmi_common.h"
#include "mmk_app.h"

#define DUERAPP_CFG_PATH L"D:\\duer\\duerapp.cfg"

static duer_mutex_t s_mutex = NULL;

uint8* duer_read_file(uint16 *path,uint32* length)
{
	SFS_HANDLE hFile = NULL;
	uint32 file_size = 0;
	uint32 read_size = 0;
	uint32 total_size = 0;
	uint8* pBuf = PNULL;

	hFile = SFS_CreateFile(path, SFS_MODE_READ|SFS_MODE_OPEN_EXISTING, 0, 0);
	
	if (hFile == NULL)
	{
		return PNULL;
	}

	SFS_SetFilePointer(hFile, (__int64)0, SFS_SEEK_BEGIN);
	SFS_GetFileSize(hFile,&file_size);
	
	pBuf = (uint8*)SCI_ALLOCAZ(file_size+1);
	if(!pBuf)
	{
		return PNULL;
	}
	SCI_MEMSET(pBuf,0,file_size+1);
	
	read_size = file_size;
	while(total_size < file_size)
	{
		SFS_ERROR_E ret = SFS_ReadFile(hFile, pBuf, read_size, &read_size, NULL);
		if (SFS_NO_ERROR != ret)
		{
			SFS_CloseFile(hFile);
			DUER_LOGI("%s SFS_ReadFile fail, ret = %d, read_size %d", __FUNCTION__, ret, read_size);
			return PNULL;
		}
		DUER_LOGI("%s success  read_size %d", __FUNCTION__,read_size);
		total_size += read_size;
		read_size = file_size - total_size;
	}

	SFS_CloseFile(hFile);
	
	*length = total_size;
	return pBuf;
}

BOOLEAN duer_write_file(uint16* file_name,uint8* pBuf, uint32 length)
{	
	SFS_HANDLE hFile = NULL;
	uint32 curr = 0;
	uint32 written=0;

	if(length == 0 || PNULL == file_name || pBuf == PNULL)
	{
		return FALSE;
	}

	hFile = SFS_CreateFile(file_name, SFS_MODE_WRITE|SFS_MODE_CREATE_ALWAYS, 0, 0);
	if(hFile==0)
    {
        DUER_LOGI("%s fail", __FUNCTION__);
		return FALSE;
    }

	while(curr < length)
	{
		SFS_ERROR_E ret = SFS_WriteFile(hFile, pBuf, length - curr, &written, PNULL);
		if (SFS_NO_ERROR != ret)
		{
			SFS_CloseFile(hFile);
			SFS_DeleteFile(file_name,PNULL);
			DUER_LOGI("%s SFS_WriteFile fail, ret = %d", __FUNCTION__, ret);
			return FALSE;
		}
		curr += written;
		pBuf += written;
	}
	
	DUER_LOGI("%s written success %d", __FUNCTION__,written);
	
	SFS_CloseFile(hFile);
	return TRUE;
}

int duer_get_config(uint8* pKey, void* pVal, uint16* pLen, int type)
{
	wchar* cfgPath = DUERAPP_CFG_PATH;
	uint8* pBuf = NULL;
	uint32 len = 0;
	cJSON *root = NULL;
	int ret = 0;

	if(pKey == NULL || pVal == NULL || pLen == NULL)
	{
		ret = -1;
		DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
		return -1;
	}

	do
	{
		cJSON *pItem = NULL;
		int length;
		pBuf = duer_read_file(cfgPath, &len);
		if(pBuf == NULL)
		{
			ret = -2;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}
		if(len == 0)
		{
			ret = -3;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}
		root = cJSON_Parse(pBuf);
		if(root == NULL)
		{
			ret = -4;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}
		pItem = cJSON_GetObjectItem(root, pKey);
		if(pItem == NULL)
		{
			DUER_LOGI("%s item not found", __FUNCTION__);
			ret = -5;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}

		if(pItem->type == cJSON_Number)
		{
			*((int*)pVal) = pItem->valueint;
		}
		else if(pItem->type == cJSON_String)
		{
			length = strlen(pItem->valuestring);
			if(length > *pLen)
			{
				ret = -6;
				DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
				break;
			}
			strcpy(pVal, pItem->valuestring);
		}
		else
		{
			ret = -7;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
		}
		
	}
	while(0);
		
	if(pBuf != NULL)
	{
		SCI_FREE(pBuf);
	}
	if(root != NULL)
	{
		cJSON_Delete(root);
	}
	
	DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
	
	return ret;
}


char* duer_get_config_with_ptr(uint8* pKey)
{
	wchar* cfgPath = DUERAPP_CFG_PATH;
	uint8* pBuf = NULL;
	uint32 len = 0;
	cJSON *root = NULL;
	int ret = 0;
	char *pVal = NULL;

	if(pKey == NULL)
	{
		ret = -1;
		DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
		return pVal;
	}

	do
	{
		cJSON *pItem = NULL;
		int length;
		pBuf = duer_read_file(cfgPath, &len);
		if(pBuf == NULL)
		{
			ret = -2;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}
		if(len == 0)
		{
			ret = -3;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}
		root = cJSON_Parse(pBuf);
		if(root == NULL)
		{
			ret = -4;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}
		pItem = cJSON_GetObjectItem(root, pKey);
		if(pItem == NULL)
		{
			DUER_LOGI("%s item not found", __FUNCTION__);
			ret = -5;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			break;
		}
		length = strlen(pItem->valuestring);
		if (length > 0) 
		{
			pVal = DUER_CALLOC(1, length + 1);
            strcpy(pVal, pItem->valuestring);
		}
	}
	while(0);
		
	if(pBuf != NULL)
	{
		SCI_FREE(pBuf);
	}
	if(root != NULL)
	{
		cJSON_Delete(root);
	}
	
	DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
	
	return pVal;
}


int duer_set_config(uint8* pKey, void* pVal, uint16* pLen, int type)
{
	wchar* cfgPath = DUERAPP_CFG_PATH;
	uint8* pBuf = NULL;
	uint32 len = 0;
	cJSON *root = NULL;
	int ret = 0;

	if(pKey == NULL || pVal == NULL || pLen == NULL)
	{
		ret = -1;
		DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
		return -1;
	}

	do
	{
		cJSON *pItem = NULL;
		int length;
		char* body = NULL;
		pBuf = duer_read_file(cfgPath, &len);
		if(pBuf == NULL)
		{
			root = cJSON_CreateObject();
			if(root == NULL)
			{
				ret = -2;
				DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
				break;
			}
			if(type == cJSON_Number)
			{
				cJSON_AddItemToObject(root, pKey, cJSON_CreateNumber(*((int*)pVal)));
			}
			else if(type == cJSON_String)
			{
				cJSON_AddItemToObject(root, pKey, cJSON_CreateString(pVal));
			}
			else
			{
				ret = -3;
				DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
				break;
			}
		}
		else
		{
			root = cJSON_Parse(pBuf);
			if(root == NULL)
			{
				ret = -2;
				DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
				break;
			}

			pItem = cJSON_GetObjectItem(root, pKey);
			if(pItem == NULL)
			{
				if(type == cJSON_Number)
				{
					cJSON_AddItemToObject(root, pKey, cJSON_CreateNumber(*((int*)pVal)));
				}
				else if(type == cJSON_String)
				{
					cJSON_AddItemToObject(root, pKey, cJSON_CreateString(pVal));
				}
				else
				{
					ret = -3;
					DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
					break;
				}			
			}
			else
			{
				if(type == cJSON_Number)
				{
					cJSON_ReplaceItemInObject(root, pKey, cJSON_CreateNumber(*((int*)pVal)));
				}
				else if(type == cJSON_String)
				{
					cJSON_ReplaceItemInObject(root, pKey, cJSON_CreateString(pVal));
				}
				else
				{
					ret = -3;
					DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
					break;
				}	
			}
				
		}
		
		body = cJSON_Print(root);
		if(body != NULL)
		{
			BOOLEAN result = FALSE;
			
			DUER_LOGI("%s %d body %s ", __FUNCTION__, __LINE__, body);
			SFS_DeleteFile(cfgPath, NULL);
			result = duer_write_file(cfgPath, body, strlen(body));
			if(result != TRUE)
			{
				ret = -4;
				DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
			}
            SCI_FREE(body);	
		}
		else
		{
			ret = -5;
			DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
		}
	}
	while(0);
		
	if(pBuf != NULL)
	{
		SCI_FREE(pBuf);
	}
	if(root != NULL)
	{
		cJSON_Delete(root);
	}
	DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
	
	return ret;
}

int duer_get_config_str(uint8* pKey, uint8* pVal ,uint16* pLen)
{
    int ret = -1;

    duer_mutex_lock(s_mutex);
	ret = duer_get_config(pKey, pVal, pLen, cJSON_String);
    duer_mutex_unlock(s_mutex);

    return ret;
}

int duer_set_config_str(uint8* pKey, uint8* pVal)
{
	uint16 len = 0;
    int ret = -1;

	if(pVal == NULL || pKey == NULL)
	{
		DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__);
		return -1;
	}

    duer_mutex_lock(s_mutex);
	len = strlen(pVal);
	ret = duer_set_config(pKey, pVal, &len, cJSON_String);
    duer_mutex_unlock(s_mutex);

    return ret;
}

int duer_get_config_int(uint8* pKey, int* pVal)
{
	uint16 len = 4;
    int ret = -1;

    duer_mutex_lock(s_mutex);
	ret = duer_get_config(pKey, pVal, &len, cJSON_Number);
    duer_mutex_unlock(s_mutex);

    return ret;
}

int duer_set_config_int(uint8* pKey, int val)
{
	uint16 len = 4;
    int ret = -1;
	if(pKey == NULL)
	{
		DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__);
		return -1;
	}
	duer_mutex_lock(s_mutex);
	ret = duer_set_config(pKey, &val, &len, cJSON_Number);
    duer_mutex_unlock(s_mutex);

    return ret;
}

int duer_sfs_cfg_init()
{
    if (s_mutex) {
        DUER_LOGI("duer_sfs_cfg has been initialised.");
        return 0;
    }

    s_mutex = duer_mutex_create();

    return 0;
}


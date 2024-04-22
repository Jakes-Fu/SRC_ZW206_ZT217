/*
 * File Name: tf_nv_nand.c
 * @Author: songtao.wang
 * @DATE: 2020/02/04
 * @Copyright: 2020 Unisoc, Incoporated. All Rights Reserved.
 * @Description: This file defines the basic configs of
 * tf nv nand operations.
 */

#include "nv_item_id.h"
//#include "nv_productionparam_type.h"
#include "sci_ftl_api.h"
//#include "tf_cfg.h"
//#include "integer.h"
//#include "sio_drv.h"
#include "cmd_def.h"
/**---------------------------------------------------------------------------*
 **                         Compiler Flag
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
extern   "C"
{
#endif

#define		SEARCH_BY_NVID		0
#define		SEARCH_FIRST_NVID		1
#define		SEARCH_NEXT_NVID		2
#define		MAX_FIXNV_LENGTH		(64*1024)
#define		INVALID_ID	0xffff
#define		NV_NAME_S_FLAG  "<NVItem name=\""       //nv name start flag
#define		NV_NAME_E_FLAG  "\""                    //nv name end flag
#define		NV_ID_S_FLAG    "<ID>"                  //nv id start flag
#define		NV_ID_E_FLAG    "<\ID>"                 //nv id end flag
#define		NV_NAME_S_FLAG_SIZE  (sizeof(NV_NAME_S_FLAG) -1)
#define		NV_ID_S_FLAG_SIZE  (sizeof(NV_ID_S_FLAG) -1)

uint8		xml_NVBackup_buf[8*1024];

typedef struct _NVITEM_CFG { /* Information of every item */
	char				 name[50];
	unsigned int		 id;
} NVITEM_CFG;


#define FDL_SendTrace rprintf

uint32 find_hex(char * s)
{
	uint32 ret = 0;
	uint32 i;
	for(i = 0; s[i] != 0; i++) {
		if(s[i] == ' ') {    //skip space
			continue;
		}
		if((s[i] == 'x' || s[i] == 'X')&& ret == 0) {   //skip 0x
			continue;
		}else if(s[i] >= '0' && s[i] <= '9') {
			ret = (ret << 4) + (s[i] - '0');
		}else if(s[i] >= 'a' && s[i] <= 'f') {
			ret = (ret << 4) + (s[i] - 'a' + 10);
		}else if(s[i] >= 'A' && s[i] <= 'F') {
			ret = (ret << 4) + (s[i] - 'A' + 10);
		}else{
			break;
		}
	}
	return ret;

}

BOOLEAN get_nv_table(char * buf, NVITEM_CFG* nv_table, int item_count) 
{
	uint8* nv_start, *nv_end, *nv_dispose;
	uint8 i;
	memset(xml_NVBackup_buf, 0, sizeof(xml_NVBackup_buf));
	nv_start = strstr(buf,"<NVBackup");
	nv_end = strstr(buf,"</NVBackup>");
	if(nv_start == NULL || nv_end == NULL) {
		rprintf("The xml file is incomplete!\n");
		return FALSE;
	}
	strncpy(xml_NVBackup_buf, nv_start , nv_end - nv_start);

	nv_dispose = xml_NVBackup_buf;
	for(i = 0; i < 200; i++) {
		nv_start = strstr(nv_dispose, NV_NAME_S_FLAG);
		if(nv_start == NULL) {  //nv name start is NULL,serach NV ID Filash!\n")
			break;
		}
		nv_end = strstr(nv_start + NV_NAME_S_FLAG_SIZE, NV_NAME_E_FLAG);
		if(nv_end == NULL) {
			rprintf("nv name end is NULL,the nv iteam is incomplete!\n");
			continue;
		}
		if(nv_end - nv_start > 49) {
			nv_end = nv_start + 49;
			rprintf("nv_table[%d] name too long !\n", i);
		}
		strncpy(nv_table[i].name, nv_start + NV_NAME_S_FLAG_SIZE, nv_end - nv_start - NV_NAME_S_FLAG_SIZE);
		nv_dispose = nv_end;
		nv_start = strstr(nv_dispose, NV_ID_S_FLAG);
		if(nv_start == NULL) {
			rprintf("nv id start is NULL,the nv iteam is incomplete!\n");
			continue;
		}
		nv_end = strstr(nv_start, NV_ID_E_FLAG);
		if(nv_end == NULL) {
			rprintf("nv id end is NULL,the nv iteam is incomplete!\n");
			continue;
		}
		nv_table[i].id = find_hex(nv_start + NV_ID_S_FLAG_SIZE);

		//Restore the adaptive ID
		if(nv_table[i].id == 0xffffffff && 0 == strcmp(nv_table[i].name,"Calibration")) {
			nv_table[i].id = 0x2;
			strcpy(nv_table[i+1].name,"W_Calibration");
			nv_table[i+1].id = 0x12D;
			i+=1;
		}
		if(nv_table[i].id == 0xffffffff && 0 == strcmp(nv_table[i].name,"IMEI")) {
			nv_table[i].id = 0x5;
			strcpy(nv_table[i+1].name,"IMEI2");
			nv_table[i+1].id = 0x179;
			strcpy(nv_table[i+2].name,"IMEI3");
			nv_table[i+2].id = 0x186;
			strcpy(nv_table[i+3].name,"IMEI4");
			nv_table[i+3].id = 0x1E4;
			i+=3;
		}
	}

	if(i + 1 > item_count - 1) {
		rprintf("too many nv item need to backup!\n");
		return FALSE;
	}
	strcpy(nv_table[i].name,"");
	nv_table[i].id = 0xFFFFFFFF;
	return TRUE;

}

static BOOLEAN ___findItem (uint32 id, uint8 *nvBuf, uint32 nvLength,
							uint32 *itemSize, uint32 *itemPos)
{
	uint32 offset = 4;
	uint16 tmp[2];
	while (1) {
		if (* (uint16 *) (nvBuf + offset) == INVALID_ID) {
			//NV_ERR_TRACE(("NVMERGE:___findItem find the tail\n"));
			break;
		}
		if (offset + sizeof (tmp) > nvLength) {
			// NV_ERR_TRACE(("NVMERGE: ___findItem Surpass the boundary of the part\r\n"));
			break;
		}
		memcpy (tmp, nvBuf + offset, sizeof (tmp));
		offset += sizeof (tmp);
		if (id == (uint32) tmp[0]) {
			*itemPos = offset;
			*itemSize =  tmp[1];
			//NV_LOG_TRACE(("NVMERGE:___findItem id = 0x%x\n", id));
			return TRUE;
		}
		offset += tmp[1];
		offset = (offset + 3) & 0xFFFFFFFC;
	}
	return FALSE;
}

extern void rprintf(const char *fmt, ...);

BOOLEAN mergeItem (uint8 *oldBuf, uint32 oldNVlength, uint8 *newBuf,
				   uint32 newNVlength,uint8* xml_buf)
{
	uint32 i;
	uint32 oldSize, oldPos;
	uint32 newSize, newPos;
	NVITEM_CFG nv_cfg[200] = {0};
	if(get_nv_table(xml_buf, nv_cfg, 200) == FALSE) {
		return FALSE;
	}

	for (i = 0; strcmp (nv_cfg[i].name, "\0"); i++) {
		if (!___findItem (nv_cfg[i].id, oldBuf, oldNVlength, &oldSize, &oldPos)) {
			rprintf ("mergeItem:item %s[%d] not find in old fix nv!\n",nv_cfg[i].name, nv_cfg[i].id);
			continue;
		}
		if (!___findItem (nv_cfg[i].id, newBuf, newNVlength, &newSize, &newPos)) {
			rprintf ("mergeItem:item%s [%d] not find in newfix nv!\n", nv_cfg[i].name, nv_cfg[i].id);
			continue;
		}
		// NV_LOG_TRACE(("NVMERGE:__mergeItem oldSize 0x%x newSize 0x%x\n", oldSize, newSize));
		if (oldSize == newSize) {
			memcpy (newBuf + newPos, oldBuf + oldPos, newSize);
			rprintf("NVMERGE:__mergeItem success id = 0x%x, name = %s\n", nv_cfg[i].id,nv_cfg[i].name);
		} else {
			rprintf ("mergeItem:item %s[%#0x] oldSize[%d] != newSize[%d], fix nv backup faild!\n", nv_cfg[i].name,nv_cfg[i].id, oldSize, newSize);
			return FALSE;
		}
	}
	return TRUE;
}

/*
* pReadBuff: fixnv address in flash
* ReadSize:fixnv partition size,FIXNV_TOTAL_LENGTH=192k
* pSaveBuff:address of target nvitem.bin
* SaveSize:actual size of target nvitem.bin
* xml_buf:UWS6121E_thinmodem.xml
*/

BOOLEAN nvmerge (uint8 *pReadBuff, uint32 ReadSize,
						 uint8 *pSaveBuff, uint32 SaveSize,
						 uint8* xml_buf)
{
	BOOLEAN bRet = TRUE;
	
	//backup nv
	bRet = mergeItem (pReadBuff, ReadSize, pSaveBuff, SaveSize, xml_buf);

	return bRet;
}

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef   __cplusplus
}
#endif


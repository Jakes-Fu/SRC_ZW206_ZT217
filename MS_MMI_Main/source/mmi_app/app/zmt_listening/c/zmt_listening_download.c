
#include "std_header.h"
#include "window_parse.h"
#include "mmk_timer.h"
#include "mmk_app.h"
#include "mmicc_export.h"
#include "guitext.h"
#include "guilcd.h"
#include "guilistbox.h"
#include "guiedit.h"
#include "guilabel.h"
#include "mmi_default.h"
#include "mmi_common.h"
#include "mmidisplay_data.h"
#include "mmi_menutable.h"
#include "mmi_appmsg.h"
#include "mmipub.h"
#include "mmi_common.h"

#include "mmiidle_export.h"
#include "mmi_position.h"
#include "ldo_drvapi.h"
#include "mmi_resource.h"
#include "mmiset_id.h"
#include "version.h"
#include "mmicc_internal.h"
#include "mmisrvrecord_export.h"
#include "mmirecord_export.h"
#include "mmiphone_export.h"
#include "zmt_listening_export.h"
#include "zmt_listening_id.h"
#include "zmt_listening_image.h"
#include "zmt_listening_text.h"

extern uint8 listening_downloading_index;
extern BOOLEAN listening_download_audio;
extern LISTENING_ALBUM_INFO * album_info;
BOOLEAN listening_download_fail = FALSE;

PUBLIC void Listening_GetLrcFileName(char * file_name, int module_id, int album_id, int audio_id)
{
	char file_str[LIST_ITEM_PATH_SIZE_MAX] = {0};

	sprintf(file_str, LISTENING_FILE_LRC_BASE_PATH, module_id,album_id, audio_id);
	strcpy(file_name, file_str);
	SCI_TRACE_LOW("%s: file_name = %s",  __FUNCTION__, file_name);
}

PUBLIC void Listening_GetFileName(char * file_name, int module_id, int album_id, int audio_id)
{
	char file_str[LIST_ITEM_PATH_SIZE_MAX] = {0};
	sprintf(file_str, LISTENING_FILE_BASE_PATH, module_id, album_id, audio_id);
	strcpy(file_name, file_str);
	SCI_TRACE_LOW("%s: file_name = %s",  __FUNCTION__, file_name);
}

//加密
PUBLIC void Listening_Encryption(char * file_buf, uint8 file_len)
{
	int i = 0;
	for (i = 0; i < file_len;++i)
	{
		*file_buf += 7;
	}
}

//解密
PUBLIC void Listening_Decryption(char * file_buf, uint8 file_len)
{
	int i = 0;
	for (i = 0; i < file_len;++i)
	{
		*file_buf -= 7;
	}
}
 
PUBLIC void sort(int *a,char *lrc[],int len)
{
	int i=0;
	int j;
	int t;
	char *tmp;
	for(i=0;i<len-1;i++) 
	{
		for(j=0;j<len-i-1;j++)
		{
			if(a[j]>a[j+1])
			{
				t=a[j];
				a[j]=a[j+1];
				a[j+1]=t;
				tmp=lrc[j];
				lrc[j]=lrc[j+1];
				lrc[j+1]=tmp;
			}
		}
	}
}

PUBLIC int chartoint(char ch){
	return ch - '0';
}
 
PUBLIC int strtoint(char *str){//计算时间，微秒 
	if(isdigit(str[0]) && isdigit(str[1])
		&& isdigit(str[0]) && isdigit(str[0])
		&& isdigit(str[0]) && isdigit(str[0])){
			int mintue = chartoint(str[0]) * 10 + chartoint(str[1]);
			int second = chartoint(str[3]) * 10 + chartoint(str[4]);
			int microsecond = chartoint(str[6]) * 10 + chartoint(str[7]);
			return (mintue * 60 + second) * 1000 + microsecond * 10;
		}
	return -1;
}
 
PUBLIC int praseLRC(char *str, int *time){
	
	if(strlen(str) == 0){//空的行 
		return 0;
	}else{
		char *p, *temp;
		int len=0;
		p = strchr(str, '[');
		if(p != NULL) 
			if(isdigit(*(p + 1))){
				temp = p + 1;
				p = strchr(str, ']');
				temp[p - temp] = '\0';
				//printf("%s\n", temp);
				if((*time = strtoint(temp)) < 0){
					return NULL;//error time
				}
			    temp = ++p;
    			while(*p != '\n'&&*p!='\0'){
					p++;
    				len++;
   				}
				SCI_MEMCPY(str,temp,len);
				str[len]='\0';
		    	return 1;
			}
		return 0;
	}
	return 0;
} 

PUBLIC uint8 getLine(char *order,char *source, int *start)
{
	char *p = source+*start;
	int len=0;
	while(*p != '\n'&&*p!='\0'){
		p++;
		len++;
	}
	if(len !=0){
		SCI_MEMCPY(order,source+*start,len);
		*start=*start+len+1;
		return 1;
	}
	return 0;
}



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
#include "corepush_md5.h"
#include "http_api.h"
#include "in_message.h"
#include "mmi_module.h"
#include "sig_code.h"
#include "zdthttp_api.h"

#define LISTENING_BASE_URL_USE_TEST 0

#define LISTENING_TIMES_DIFF_FROM_1978_TO_1980  ((365*10+2)*24*3600) - (8 * 60 * 60)

#ifdef WIN32
char *LISTENING_BASE_URL = "http://apps.readboy.com";
#else
char *LISTENING_BASE_URL = "https://apps.readboy.com";
#endif

char * LISTENING_UID = "00000000";
char * LISTENING_UID_MD5 = "dd4b21e9ef71e1291183a46b913ae6f2";
char * LISTENING_appID = "listening";
char * LISTENING_appSec = "3c045369002b495267e9db17b605d9f4";

char * LISTENING_MODULE_TYPE_URL = "/api/listening/module_list";
char * LISTENING_ALBUM_LIST_URL = "/api/listening/album_list";
char * LISTENING_AUDIO_LIST_URL = "/api/listening/audio_list";
char * LISTENING_AUDIO_LRC_BASE_URL = "https://img.readboy.com/apps/listening/file/%d/%s.lrc";

#define STREAM_DATA_ORIGIN_MAX_LEN 102400
#define STREAM_DATA_ORIGIN_FIRST_PLAY_LEN 10240
#define STREAM_DATA_ORIGIN_LEFT_LEN 5120

char * listening_downloading_audio_path = NULL;

extern uint8 listening_downloading_index;
extern LISTENING_ALBUM_INFO * album_info;
uint32 request_http_listening_idx = 0;
BOOLEAN listening_download_audio = FALSE;

LOCAL void Listening_MakeMD5(char * md5_str, uint32 timestamp)
{
	char tmp_buf[255] = {0};
	
	sprintf(tmp_buf, "%d", timestamp);
	strcat(tmp_buf, LISTENING_appSec);
	strcat(tmp_buf, LISTENING_UID_MD5);
	SCI_TRACE_LOW("%s: tmp_buf = %s", __FUNCTION__, tmp_buf);
	corepush_md5_str(&tmp_buf, md5_str);
	SCI_TRACE_LOW("%s: md5_str = %s", __FUNCTION__, md5_str);
}

LOCAL void Listening_MakeSn(char * sn_str)
{
	uint32 timestamp = 0;
	char * md5_str = NULL;

	md5_str = (char *)SCI_ALLOC_APPZ(255);
	memset(md5_str, 0, 255);

	timestamp = MMIAPICOM_GetCurTime() + LISTENING_TIMES_DIFF_FROM_1978_TO_1980;
	SCI_TRACE_LOW("%s: timestamp = %d", __FUNCTION__, timestamp);
	
	Listening_MakeMD5(md5_str, timestamp);
	SCI_TRACE_LOW("%s: md5_str = %s", __FUNCTION__, md5_str);
	
	sprintf(sn_str, "%s%d%s%s", LISTENING_UID, timestamp, md5_str, LISTENING_appID);
	SCI_TRACE_LOW("%s: sn_str = %s", __FUNCTION__, sn_str);
	SCI_FREE(md5_str);
}

PUBLIC void Listening_RequestAlbumListInfo(LISTENING_SELECT_MODULE_TYPE module_type)
{
	char * sn_str = SCI_ALLOCA(255);
	char * url = SCI_ALLOCA(255);
	memset(sn_str, 0, 255);
	memset(url, 0, 255);
	
	Listening_MakeSn(sn_str);
	sprintf(url, "%s?stage=%d&&sn=%s", LISTENING_MODULE_TYPE_URL, module_type, sn_str);
	SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
	//request_http_listening_idx++;
       MMIZDT_HTTP_AppSend(TRUE, LISTENING_BASE_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, Listening_ParseAlbumResponse);
	SCI_FREE(sn_str);
}

PUBLIC void Listening_RequestAudioListInfo(uint8 module_id)
{
	char * sn_str = SCI_ALLOCA(255);
	char * url = SCI_ALLOCA(255);
	memset(sn_str, 0, 255);
	memset(url, 0, 255);
	
	Listening_MakeSn(sn_str);
	sprintf(url, "%s?module_id=%d&&sn=%s", LISTENING_ALBUM_LIST_URL, module_id, sn_str);
	SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
	//request_http_listening_idx++;
	MMIZDT_HTTP_AppSend(TRUE, LISTENING_BASE_URL, url, strlen(url), 1000, 0, 0, 0, 0, 0, Listening_ParseAudioResponse);
	SCI_FREE(sn_str);
}

PUBLIC int Listening_StrReplace(char strRes[],char from[], char to[]) 
{
    int i,flag = 0;
    char *p,*q,*ts;
    for(i = 0; strRes[i]; ++i) 
    {
        if(strRes[i] == from[0]) 
        {
            p = strRes + i;
            q = from;
            while(*q && (*p++ == *q++));
            if(*q == '\0') 
            {
                ts = (char *)malloc(strlen(strRes) + 1);
                strcpy(ts,p);
                strRes[i] = '\0';
                strcat(strRes,to);
                strcat(strRes,ts);
                free(ts);
                flag = 1;
            }
        }
    }
    return flag;
}

PUBLIC void Listening_RequestDownloadAudio(uint8 index)
{
	char url[100] = {0};
	strcpy(url, album_info->item_info[index].audio_path);
	//strcpy(url, "http://appdl.ebag.readboy.com/poem/audio/ddf33c3cf7505982734a3f284f885e1e.mp3");
	#ifdef WIN32
	Listening_StrReplace(url, "https", "http");
	#endif
	SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);
    
	#ifndef WIN32
	if(zmt_tfcard_exist() && zmt_tfcard_get_free_kb() > 100 * 1024)
	#else
	if(1)
	#endif
	{
		char file_path[LIST_ITEM_PATH_SIZE_MAX] = {0};
		Listening_GetFileName(
			file_path, 
			album_info->module_id,
			album_info->album_id,
			album_info->item_info[index].audio_id
			);
		if(listening_downloading_audio_path != NULL){
			SCI_FREE(listening_downloading_audio_path);
			listening_downloading_audio_path = NULL;
		}
		listening_downloading_audio_path = SCI_ALLOC_APPZ(strlen(file_path)+1);
		SCI_MEMSET(listening_downloading_audio_path, 0, strlen(file_path)+1);
		SCI_MEMCPY(listening_downloading_audio_path, file_path, strlen(file_path)+1);
		SCI_TRACE_LOW("%s: listening_downloading_audio_path = %s", __FUNCTION__, listening_downloading_audio_path);
		request_http_listening_idx++;
		listening_download_audio = TRUE;
		SCI_TRACE_LOW("%s: index = %d", __FUNCTION__, index);
		album_info->item_info[index].aduio_ready = 1;
		MMK_SendMsg(LISTENING_AUDIO_LIST_WIN_ID, MSG_FULL_PAINT, PNULL);
		//MMIZDT_HTTP_AppSend(TRUE, url, PNULL, 0, 1000, 0, 0, 8000, 0, 0, Listening_ParseAudioDownload);
		//MMIZYB_HTTP_TestLongPlay();
		MMIZDT_HTTP_AppSend(TRUE,url,PNULL,0,101,0,1,600*1000,file_path,strlen(file_path),Listening_ParseAudioDownload);
	}
	else
	{
		MMI_CreateListeningTipWin(PALYER_PLAY_NO_SPACE_TIP);
	}
}


PUBLIC void Listening_RequestDownloadAudioLrc(int module_id, int album_id, int audio_id,char * lrc_name)
{
	char file_path[50] = {0};
	char * url = (char *)SCI_ALLOC_APPZ(255);
	
	url = SCI_ALLOC_APPZ(255);
	memset(url, 0, 255);
	sprintf(url, LISTENING_AUDIO_LRC_BASE_URL, album_id, lrc_name);
	SCI_TRACE_LOW("%s: url = %s", __FUNCTION__, url);

	Listening_GetLrcFileName(file_path, module_id, album_id, audio_id);
	
	request_http_listening_idx++;
	MMIZDT_HTTP_AppSend(TRUE,url,PNULL,0,101,0,1,180*1000,file_path,strlen(file_path),Listening_ParseAudioLrcResponse);
	SCI_TRACE_LOW("%s: url = %s file_path = %s", __FUNCTION__, url, file_path);
}



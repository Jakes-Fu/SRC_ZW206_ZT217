
#include "std_header.h"
#include "mn_type.h"
#include "mmi_nv.h"
#include "sci_types.h"
#include <stdio.h>
#include "nvitem.h"
#ifdef _RTOS
#include "sci_api.h" /*@tony.yao  replase sci_mem.h with sci_api.h*/
#else
#include "tasks_id.h"
#endif
#include "mn_api.h"
#include "mn_events.h"
#include "mn_type.h"
#include "nv_item_id.h"
#include "sio.h"
#include "sig_code.h"
#include "os_api.h"
#include "IN_message.h"
#include "socket_types.h"

#include "mmi_signal_ext.h"
#include "mmk_app.h"
#include "mmk_timer.h"
#include "mmi_text.h"
#include "mmi_common.h"
#include "window_parse.h"
#include "guitext.h"
#include "guilcd.h"
#include "mmi_menutable.h"
#include "guilistbox.h"
#include "mmi_image.h"
#include "guiedit.h"
#include "mmipub.h"
#include "mmi_appmsg.h"
#include "mmiset_export.h"
#include "mmienvset_export.h"
#include "mmisms_export.h"
#include "mmipdp_export.h"
#include "mmiconnection_export.h"
#include "mmiphone_export.h"
#include "mmifmm_export.h"
#include "zmt_listening_nv.h"

#define LISTENING_PLAYER_USE_STYLE_VOLUME 0

#define ALBUM_LIST_SHOW_ITEM_MAX 3
#define AUDIO_LIST_SHOW_ITEM_MAX 4

#define LIST_ALBUM_NAME_SIZE_MAX 50
#define LIST_ITEM_NAME_SIZE_MAX 255
#define LIST_ITEM_PATH_SIZE_MAX 100
#define MODULE_LIST_ITEM_MAX 12
#define ALBUM_LIST_ITEM_MAX 15
#define AUDIO_LIST_ITEM_MAX 100

#define LISTEN_LINE_HIGHT MMI_MAINSCREEN_HEIGHT/10
#define LISTEN_LINE_WIDTH MMI_MAINSCREEN_WIDTH/6

#define LISTEN_LRC_LABEL_NUM 8

#define LISTENING_DIRECTORY_BASE_PATH "E:/Listening/%d_%d"
#define LISTENING_FILE_INFO_PATH "E:/Listening/listening_info.json"
#define LISTENING_FILE_BASE_PATH "E:/Listening/%d_%d/%d.mp3"
#define LISTENING_FILE_LRC_BASE_PATH "E:/Listening/%d_%d/%d.lrc"

typedef enum
{
	PALYER_PLAY_NO_SIM_TIP = 0,
	PALYER_PLAY_NO_TFCARD_TIP,
	PALYER_PLAY_NO_SPACE_TIP,
	PALYER_PLAY_NO_PRE_TIP,
	PALYER_PLAY_NO_NEXT_TIP,
	PALYER_PLAY_DOWNLOADING_TIP,
	PALYER_PLAY_DOWNLOAD_FAIL_TIP,
}LISTENING_TIPS_TYPE_E;

typedef enum
{
	SELECT_MODULE_DEFAULT = 0,//全部
	SELECT_MODULE_PRIMARY_SCHOOL,//小学
	SELECT_MODULE_JUNIOR_SCHOOL,//初中
	SELECT_MODULE_HIGH_SCHOOL,//高中
	SELECT_MODULE_PRE_SCHOOL,//学前
	SELECT_MODULE_LOCAL,//本地
}LISTENING_SELECT_MODULE_TYPE;

typedef struct
{
	LISTENING_PALYER_PLAY_STYLE play_style;
	uint8 play_status;//0:paly , 1:pause , 2:loading
	int bottom_type;//0:no show , 1:play style show , 2:volume show
	int volume;
	uint8 lrc_ready;//0:loading , 1:success , 2:failed
}LISTENING_PALYER_PLAY_INFO;

typedef struct
{
	BOOLEAN is_select;
}LISTEING_LOCAL_SELECT_INFO;

typedef struct
{
	BOOLEAN is_select_delete;
	BOOLEAN is_select_all;
	LISTEING_LOCAL_SELECT_INFO select_info[100];
}LISTEING_LOCAL_DELETE_INFO;

typedef struct
{
	uint8 album_total_num;
	uint8 item_total_num;
	uint8 album_cur_page;
	uint8 item_cur_page;
	uint8 select_cur_class;

	uint8 local_album_cur;
	uint8 local_audio_cur;
	uint8 local_album_total;
	uint8 local_audio_total;
}LISTENING_LIST_INFO;

typedef struct
{
	int album_id;
	char album_name[LIST_ALBUM_NAME_SIZE_MAX];
}LISTENING_MODULE_ITEM_INFO;

typedef struct
{
	int module_id;
	LISTENING_MODULE_ITEM_INFO item_info[ALBUM_LIST_ITEM_MAX];
}LISTENING_MODULE_INFO;

typedef struct
{
	int audio_id;
	char audio_name[LIST_ITEM_PATH_SIZE_MAX];
	//char audio_lrc[LIST_ITEM_NAME_SIZE_MAX];
	char audio_path[LIST_ITEM_PATH_SIZE_MAX];
	int audio_duration;
	uint8 aduio_ready;
}LISTENING_ALBUM_ITEM_INFO;

typedef struct
{
	int module_id;
	int album_id;
	char album_name[LIST_ALBUM_NAME_SIZE_MAX];
	LISTENING_ALBUM_ITEM_INFO item_info[AUDIO_LIST_ITEM_MAX];
}LISTENING_ALBUM_INFO;

typedef struct
{
	BOOLEAN is_local_play;
	uint8 moudle_index;
	uint8 audio_index;
}LISTENING_PLAYER_INFO;

PUBLIC void MMI_TestToOpenPlayerWin(void);

PUBLIC void Listening_InitLocalDataInfo(void);
PUBLIC void Listening_FreeLocalDataInfo(void);
PUBLIC LISTEING_LOCAL_INFO * Listening_GetLocalDataInfo(void);

PUBLIC void Listening_ParseAlbumResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void Listening_ParseAudioResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void Listening_ParseAudioDownload(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);
PUBLIC void Listening_ParseAudioLrcResponse(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 err_id);

PUBLIC void Listening_RequestAlbumListInfo(LISTENING_SELECT_MODULE_TYPE module_type);
PUBLIC void Listening_RequestAudioListInfo(uint8 module_id);
PUBLIC void Listening_RequestDownloadAudio(uint8 index);
PUBLIC void Listening_RequestDownloadAudioLrc(int module_id, int album_id, int audio_id,char * lrc_name);

PUBLIC void MMI_CreateListeningWin(void);
PUBLIC void MMI_CreateListeningAudioWin(uint8 index);
PUBLIC void MMI_CreateListeningPlayerWin(LISTENING_PLAYER_INFO * player_info);
PUBLIC void MMI_CreateListeningPlayerLrcWin(LISTENING_PLAYER_INFO * player_info);
PUBLIC void MMI_CreateListeningLocalWin(void);
PUBLIC void MMI_CreateListeningLocalAudioWin(uint8 index);
PUBLIC void MMI_CreateListeningTipWin(LISTENING_TIPS_TYPE_E type);
PUBLIC void MMI_CloseListeningTipWin();

PUBLIC void ListeningWin_DisplaySecletedClass(MMI_WIN_ID_T win_id, GUI_LCD_DEV_INFO lcd_dev_info);
PUBLIC MMI_RESULT_E ButtonPreClass_CallbackFunc(void);
PUBLIC MMI_RESULT_E ButtonPriClass_CallbackFunc(void);
PUBLIC MMI_RESULT_E ButtonJurClass_CallbackFunc(void);

PUBLIC void ListeningLocalWin_DisplayLocalAlbumList(MMI_WIN_ID_T win_id, MMI_CTRL_ID_T ctrl_id);

PUBLIC BOOLEAN MMI_IsListeningAudioWinOpen(void);

PUBLIC BOOLEAN Listening_PlayMp3(int module_id, int album_id, int audio_id);
PUBLIC uint8 Listening_StartPlayMp3(char* file_name);
PUBLIC void Listening_StopPlayMp3(void);
PUBLIC void ZMTListening_CloseListeningPlayer(void);

PUBLIC void Listening_GetLrcFileName(char * file_name, int module_id, int album_id, int audio_id);
PUBLIC void Listening_GetAudioLrcData(int module_id, int album_id, int audio_id);

PUBLIC void Listening_Encryption(char * file_buf, uint8 file_len);
PUBLIC void Listening_Decryption(char * file_buf, uint8 file_len);

PUBLIC void Listening_DeleteOneAlbum(int module_id);
PUBLIC void Listening_DeleteOneAudio(int del_module_id, int del_audio_id);


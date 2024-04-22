#ifndef __ZDT_YX_VOICE_H__
#define __ZDT_YX_VOICE_H__
/*
和盈互联智能穿戴通讯协议2.0兼容GSM网.pdf
*/
#include "zdt_common.h"
#include "zdt_yx_api.h"
#include "zdt_yx_db.h"

#define MAX_YX_VOC_GROUP_FULL_PATH_SIZE 80

#define MAX_YX_VOC_FILENAME_SIZE 80

#define MAX_YX_VOC_SAVE_SIZE 10

#define YX_VCHAT_DIR "D:\\vChat\\"
#define YX_VCHAT_FILENAME "vchat_record.txt"
#define YX_VCHAT_FILESTATUS "vchat_status.db"

#define YX_VCHAT_DEFAULT_GROUP_ID "common"

typedef enum
{
    YX_GROUP_USER_COMMON, //群聊
    YX_GROUP_USER_BIND, //绑定用户
    YX_GROUP_USER_FRIEND,//好友
} YX_GROUP_USER_TYPE_T;

typedef struct _YX_GROUP_FILENAME_DATA_t
{
    uint8 fullname[MAX_YX_VOC_GROUP_FULL_PATH_SIZE+1];
} YX_GROUP_FILENAME_DATA_T;

typedef struct _YX_VOC_STATUS_DATA_t
{
    uint8 is_read;
    uint8 msg_type;
    uint8 druation;
    uint8  friend_id[YX_DB_FRIEND_MAX_ID_SIZE+1];
} YX_VOC_STATUS_DATA_T;

typedef struct _YX_GROUP_INFO_DATA_t
{
    uint16 file_num;
    uint8  group_id[YX_DB_FRIEND_MAX_ID_SIZE+1];
    uint8  group_name[YX_DB_FRIEND_MAX_NAME_SIZE+1];
    uint8  group_num[YX_DB_FRIEND_MAX_NUMBER_SIZE+1];
    YX_VOC_STATUS_DATA_T  status_arr[MAX_YX_VOC_SAVE_SIZE+1];
    YX_GROUP_FILENAME_DATA_T patchname;
    YX_GROUP_FILENAME_DATA_T filename;
    YX_GROUP_FILENAME_DATA_T status_name;
    YX_GROUP_FILENAME_DATA_T file_arr[MAX_YX_VOC_SAVE_SIZE+1];
    YX_GROUP_USER_TYPE_T user_type;
} YX_GROUP_INFO_DATA_T;

typedef struct _YX_VOC_SEND_DATA_t
{
    uint8          * pFilename;
    uint8          msg_type;
    uint8          emoji_index;
    uint16         len;
} YX_VOC_SEND_DATA_T;

typedef struct _YX_VOC_SEND_NODE_t
{
       YX_VOC_SEND_DATA_T  data;
       struct _YX_VOC_SEND_NODE_t * next_ptr;
} YX_VOC_SEND_NODE_T;

typedef struct _YX_VOC_RCV_FILE_DATA_t
{
    uint16 voc_idx;
    uint8 * voc_data;
    uint16 voc_data_len;
    uint8 pack_idx;
    uint8 pack_sum;
}YX_VOC_RCV_FILE_DATA_T;

typedef struct _YX_VOC_RCV_FILE_NODE_t
{
       YX_VOC_RCV_FILE_DATA_T  data;
       struct _YX_VOC_RCV_FILE_NODE_t * next_ptr;
}YX_VOC_RCV_FILE_NODE_T;


typedef enum  _YX_RECORDER_DISPLAY_STAT
{
    YX_RECOREDER_DISP_IDLE,
    YX_RECOREDER_DISP_RECORDING,
    YX_RECOREDER_DISP_RECORD_OK,
    YX_RECOREDER_DISP_RECORD_FAIL,
    YX_RECOREDER_DISP_SENDING,
    YX_RECOREDER_DISP_SEND_OK,
    YX_RECOREDER_DISP_SEND_FAIL,
    YX_RECOREDER_DISP_MAX
}YX_RECORDER_DISPLAY_STAT;

typedef enum  _YX_RECORDER_STAT
{
	YX_RECOREDER_STOP,
	YX_RECOREDER_RECORD,
    YX_RECOREDER_CANCEL
}YXRecorderStatus;

typedef enum  _YX_VocRspType_STAT
{
	YX_VOCRSP_TYPE_RECORD,
	YX_VOCRSP_TYPE_RECORDSTOP,
	YX_VOCRSP_TYPE_SEND,
	YX_VOCRSP_TYPE_PLAY,
	YX_VOCRSP_TYPE_PLAYSTOP,
	YX_VOCRSP_TYPE_MAX
}YXVocRspTypeStatus;

typedef enum  _YX_VocRecordErr_STAT
{
	YX_VOCRECORD_SUCCESS,
	YX_VOCRECORD_ERR,
	YX_VOCRECORD_ERR_RECORDING,
	YX_VOCRECORD_ERR_NO_CARD,
	YX_VOCRECORD_ERR_NO_SRV,
	YX_VOCRECORD_ERR_LAND,
	YX_VOCRECORD_ERR_SPACE,
	YX_VOCRECORD_ERR_LOWTIME,
	YX_VOCRECORD_ERR_MP3PLAY,
	YX_VOCRECORD_ERR_FMON,
	YX_VOCRECORD_ERR_NET,
	YX_VOCRECORD_ERR_INCALL,
	YX_VOCRECORD_ERR_INHIDDEN,
	YX_VOCRECORD_ERR_MAX
}YXVocRecordErrStatus;

typedef enum  _YX_VocRcdStopErr_STAT
{
	YX_VOCRCDSTOP_SUCCESS,
	YX_VOCRCDSTOP_ERR,
	YX_VOCRCDSTOP_ERR_SPACE,
	YX_VOCRCDSTOP_ERR_LOWTIME,
	YX_VOCRCDSTOP_ERR_TIMEOUT,
	YX_VOCRCDSTOP_ERR_MAX
}YXVocRcdStopErrStatus;

typedef enum  _YX_VocSendErr_STAT
{
	YX_VOCSEND_SUCCESS,
	YX_VOCSEND_ERR,
	YX_VOCSEND_ERR_NET,
	YX_VOCSEND_ERR_FILE,
	YX_VOCSEND_ERR_UNREG,
	YX_VOCSEND_ERR_MAX
}YXVocSendErrStatus;


extern BOOLEAN  YX_VOC_Send_Link_DelAll(YX_APP_T *pMe);
extern int YX_Voice_HandleStop(YX_APP_T * pMe);
extern BOOLEAN YX_VOC_IsRcvFile(char* full_name);

extern BOOLEAN YX_Voice_Record_Start(YX_APP_T * pMe);
extern BOOLEAN YX_Voice_Record_Stop(YX_APP_T * pMe);
extern BOOLEAN YX_Voice_Record_StopTimout(YX_APP_T * pMe);
extern BOOLEAN YX_Voice_Record_Cancel(YX_APP_T * pMe);

extern int YX_Net_Receive_TK_VocFile(YX_APP_T *pMe,uint8 * pFiledata,uint16 DataLen,uint8 * friend_id,int msgType);
extern int YX_Net_Receive_TK2_VocFile(YX_APP_T *pMe,uint8 * pFiledata,uint16 DataLen,uint8 * friend_id,int msgType);
extern int YX_Net_Receive_STK_VocFile(YX_APP_T *pMe,uint8 * pFiledata,uint16 DataLen,uint8 * friend_id,int msgType);

extern int32 YX_Net_Send_TK_VocFile_Start(YX_APP_T *pMe);
extern void YX_Net_Send_TK_VocFile_End(YX_APP_T *pMe,uint8 err_id);

extern BOOLEAN YX_Voice_Send_Start(YX_APP_T * pMe);
extern int32 YX_Net_Send_TK_Emoji(YX_APP_T * pMe);
extern void YX_Net_Send_Emoji_Success();

extern int YX_API_Record_Start(void);
extern int YX_API_Record_Stop(void);
extern int YX_API_Record_Cancel(void);
extern int YX_API_Record_Send(void);
int YX_API_Emoji_Send(uint8 emoji_index);
extern BOOLEAN YX_API_Record_IsRecording(void);
extern BOOLEAN YX_API_Record_FileDelAll(YX_APP_T * pMe);

extern BOOLEAN YX_VCHAT_GetCurGroupInfo(uint8 * utf8_group_id,YX_GROUP_INFO_DATA_T * group_info);
extern BOOLEAN YX_VCHAT_GetAllGroupInfo(void);
extern BOOLEAN YX_Voice_GroupRemoveAll(YX_APP_T * pMe,uint8 * utf8_group_id);
extern uint32 YX_VocReadStatusCheck(uint8 * utf8_group_id);
extern void YX_VocFileStatusWrite(uint8 * full_name ,YX_VOC_STATUS_DATA_T *ui_line_status);

extern BOOLEAN YX_Voice_Allow_Receive(YX_APP_T * pMe);

extern YX_GROUP_INFO_DATA_T * m_pCurGroupInfo;
extern YX_GROUP_INFO_DATA_T * m_pCurRcvGroupInfo;
#endif

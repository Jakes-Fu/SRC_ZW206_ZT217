
#include "mmisrv.h"
#include "mmisrvaud.h"
#include "mmisrvaud_api.h"
#include "mmi_ring.h"
#include "mmiset_export.h"
#include "mmisrvrecord_export.h"
#include "dsp_codec_adp.h"
#include "aud_gen.h"
#include "mp3_adp.h"
#include "sig_code.h"
#include "aac_lc_adp.h"
#include "zyb_stream_player.h"
#include "zyb_mp3_info.h"
#include "zyb_http_api.h"


//#define ZYB_STREAM_PLAY_AMR

#if 1

#define STREAM_DATA_ORIGIN_MAX_LEN 102400
#define STREAM_DATA_ORIGIN_FIRST_PLAY_LEN 10240
#define STREAM_DATA_ORIGIN_LEFT_LEN 5120

#define STREAM_DATA_REQUEST_LEN 51200

typedef struct
{
    uint32  size;              // Total Size of the buffer.
    volatile int32  start_point;       // First data in the buffer.
    volatile int32  end_point;         // Last data in the buffer.
    volatile uint32  status;            // Indicate the buffer status.
    uint32  lost_num;          //
    uint8   *sio_origin_buf_ptr;      // Sio buffer address.
    uint32  file_size;              // Total Size of the file.文件长度
    uint32  play_size;              // had play size 已经塞给播发器的长度
} SIO_ORIGIN_BUF_S;

static uint8  sio_stream_data_origin_buf[STREAM_DATA_ORIGIN_MAX_LEN+1];
static SIO_ORIGIN_BUF_S  sio_stream_data_origin_s;

uint32  SIO_Stream_Data_Origin_GetBufLen(void);

int32 sio_origin_buf_space( SIO_ORIGIN_BUF_S *pRing)
{
  int32 ret = pRing->start_point - pRing->end_point - 1;
  return (ret >= 0) ? ret : (ret + pRing->size);
}

int32 sio_origin_buf_len( SIO_ORIGIN_BUF_S *pRing )
{
  int32 ret = pRing->end_point - pRing->start_point;
  return (ret >= 0) ? ret : (ret + pRing->size);
}

/*
** Remove a character from this ring.
** Returns -1 if there are no characters in this ring.
** Returns the character removed otherwise.
*/
int32 sio_origin_buf_remove( SIO_ORIGIN_BUF_S *pRing )
{
    int32 ret;

    if ( pRing->end_point == pRing->start_point ) {
        return -1;
    }

    ret = pRing->sio_origin_buf_ptr[pRing->start_point++];

    if ( pRing->start_point >= pRing->size )
        pRing->start_point = 0;

    return ret;
}

/*
** Add 'ch' to this ring.
** Returns -1 if there is not enough room.
** Returns 0 otherwise.
*/
int32 sio_origin_buf_add( SIO_ORIGIN_BUF_S *pRing, uint8 ch )
{
    if ( sio_origin_buf_space( pRing ) == 0 ) {
        //sio_origin_buf_remove(pRing);
        return -1;
    }

    pRing->sio_origin_buf_ptr[pRing->end_point++] = ch;

    if ( pRing->end_point >= pRing->size )
        pRing->end_point = 0;

    return 0;
}
int32 sio_origin_buf_empty( SIO_ORIGIN_BUF_S *pRing )
{
    pRing->end_point = pRing->start_point;
    return 0;
}

int32 sio_origin_buf_empty2( SIO_ORIGIN_BUF_S *pRing )
{
    pRing->start_point = pRing->end_point;
    return 0;
}

int32 sio_origin_buf_copy(SIO_ORIGIN_BUF_S *pRing, uint8 * src_buf,uint32 rec_num)
{
    SIO_ORIGIN_BUF_S * sio_origin_buf_ptr = NULL;
    int32 i;
    uint16 nchar;
    int32 ret = 0;
    
    while(rec_num)
    {
        nchar = *src_buf++;
        rec_num--;
        if(sio_origin_buf_add( pRing, nchar ) == -1)
        {
            ret = -1;
            pRing->end_point = pRing->start_point;
        }
    }
    
    return ret;
}
uint32 sio_origin_buf_getdata(SIO_ORIGIN_BUF_S *pRing,uint8 * pOutBuf)
{
    uint32 i = 0;
    uint32 len = pRing->size;
    
    if(len == 0)
    {
        return NULL;
    }
        
    while(pRing->end_point != pRing->start_point )
    {
        if(i >= len)
        {
            break;
        }
        pOutBuf[i++] = pRing->sio_origin_buf_ptr[pRing->start_point++];
        if ( pRing->start_point >= pRing->size)
        {
            pRing->start_point = 0;
        }
    }
    return i;
}

uint32 SIO_Stream_Data_Put(uint8 * src_buf,uint32 rec_num)
{
    uint8 origin_data;
    uint32 j = 0;
    for(j = 0; j < rec_num; j++)
    {
        origin_data = *src_buf++;
        if(sio_origin_buf_add(&sio_stream_data_origin_s, origin_data) == -1)
        {
            break;
        }
    }
    ZYB_STREAM_LOG("SIO_Stream_Data_Put put_len=%d, buf_len=%d",j,SIO_Stream_Data_Origin_GetBufLen());
    return j;
}
uint32  SIO_Stream_Data_Origin_GetBufLen(void)
{
    uint32 len = sio_origin_buf_len(&sio_stream_data_origin_s);
    return len;
}

uint32  SIO_Stream_Data_Origin_GetLeftLen(void)
{
    uint32 len = sio_origin_buf_len(&sio_stream_data_origin_s);
    len = sio_stream_data_origin_s.size - len;
    return len;
}

uint32  SIO_Stream_Data_Origin_Get_Buf(uint8* pOutBuf,uint16 max_len)
{
    uint32 i = 0;
    uint8 u8_ch = 0;
    
    if(max_len == 0 || pOutBuf == NULL)
    {
        return 0;
    }

    while(sio_stream_data_origin_s.end_point != sio_stream_data_origin_s.start_point )
    {
        if(i >= max_len)
        {
            break;
        }
        u8_ch = sio_stream_data_origin_s.sio_origin_buf_ptr[sio_stream_data_origin_s.start_point++];
        pOutBuf[i++] = u8_ch; 
        
        if ( sio_stream_data_origin_s.start_point >= sio_stream_data_origin_s.size)
        {
            sio_stream_data_origin_s.start_point = 0;
        }
    }
    return i;
}

uint32  SIO_Stream_Data_Origin_Get_Stream(uint8* pOutBuf,uint16 max_len,uint16 offset_len)
{
    uint32 i = 0;
    
    if(max_len == 0 || pOutBuf == NULL || offset_len == 0)
    {
        return 0;
    }
    while(sio_stream_data_origin_s.end_point != sio_stream_data_origin_s.start_point )
    {
        if(i >= max_len || i >= offset_len)
        {
            break;
        }
        pOutBuf[i++] = sio_stream_data_origin_s.sio_origin_buf_ptr[sio_stream_data_origin_s.start_point++];
        if ( sio_stream_data_origin_s.start_point >= sio_stream_data_origin_s.size)
        {
            sio_stream_data_origin_s.start_point = 0;
        }
    }
    return i;
}

int32 SIO_Stream_Data_Origin_Put(uint16 ori_data)
{
    int32 ret = 0;
    SIO_ORIGIN_BUF_S *pRing = &sio_stream_data_origin_s;
    if(sio_origin_buf_add( pRing, ori_data ) == -1)
    {
        ret = -1;
    }
    return ret;
}

PUBLIC void SIO_Stream_Data_Set_Origin_File_Size(uint32 size)
{        
    sio_stream_data_origin_s.file_size = size;
}

PUBLIC uint32 SIO_Stream_Data_Get_Origin_File_Size()
{        
    return sio_stream_data_origin_s.file_size;
}

void SIO_Stream_Data_Init(void)
{        
    sio_stream_data_origin_s.sio_origin_buf_ptr    = sio_stream_data_origin_buf;
    sio_stream_data_origin_s.size           = STREAM_DATA_ORIGIN_MAX_LEN;
    sio_stream_data_origin_s.start_point    = 0;
    sio_stream_data_origin_s.end_point      = 0;
    sio_stream_data_origin_s.status         = 0;
    sio_stream_data_origin_s.lost_num       = 0;
    sio_stream_data_origin_s.file_size      = 0;
    sio_stream_data_origin_s.play_size      = 0;
    return;
}
#endif

#if 1
#define IDLE      0
#define DECODING  1
#define MP3_DRIVER_BUF_SIZE  (160*150)

typedef struct 
{
    uint32  audioSamplingRate;  /*sample rate, Hz*/
    uint8   audioChannels;      /*channels*/
}MP3_Info_T;

typedef struct
{
    HAUDIO       audioHandle;    /*device handle*/
    MP3_Info_T   MP3_Info;  /*mp3 information*/
    int          i_state;
}mp3_dec_ctx_t;


//HTTP 接口
uint32 zyb_mp3_data_offset = 0;
uint32 zyb_mp3_data_cur_start_offset = 0;
BOOLEAN zyb_mp3_data_need_check = FALSE;
BOOLEAN zyb_mp3_data_is_over = FALSE;
BOOLEAN zyb_mp3_data_is_getting = FALSE;
BOOLEAN zyb_mp3_data_regetting = FALSE;

//播放相关接口
static SCI_MUTEX_PTR zyb_streamplayer_mutexPtr = NULL;
static char * zyb_audio_url = PNULL;
static zyb_stream_player_status_t zyb_playerStatus = ZYB_STREAM_PLAYER_STATUS_STOP;

//音频格式
LOCAL AUDIO_FORMAT_TYPE audio_format_type = AUDIO_UNKOWN_FORMAT;

uint8 zyb_mp3_buf[MP3_DRIVER_BUF_SIZE]= {0}; 
LOCAL uint32 zyb_mp3_buf_offset = 0; 
LOCAL mp3_dec_ctx_t s_dec_ctx = {0};
LOCAL MP3_STREAM_BUFFER_T  mp3_header_info = {0};
static uint32 zyb_player_Volume = MMISET_VOL_MAX;

extern uint32 VBDecodeOuputBuf[];
PUBLIC BOOLEAN  MMIZYB_HTTP_UrlGetFirst(char * url);
PUBLIC void ZDT_TCP_StopStopDelayTimer(void);

int ZYB_StreamPlayer_Status_CB(zyb_stream_player_status_t  cur_status)
{
    ZYB_STREAM_LOG("ZYB_StreamPlayer_Status_CB cur_status=%d",cur_status);
    zyb_playerStatus = cur_status;
    switch(cur_status)
    {
        case ZYB_STREAM_PLAYER_STATUS_PLAYING: 
            break;
            
        case ZYB_STREAM_PLAYER_STATUS_ERROR: 
                //播放过程中出错停止播放
                MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOSTOP,PNULL,0);
            break;
            
        case ZYB_STREAM_PLAYER_STATUS_PAUSED: 
            break;
            
        case ZYB_STREAM_PLAYER_STATUS_CONNECTING: 
            break;
            
        case ZYB_STREAM_PLAYER_STATUS_CACHED: 
            break;
    }
    return 0;
}


PUBLIC int  MMIZYB_HTTP_ReGetMp3Data(void)
{
    ZYB_STREAM_LOG("MMIZYB_HTTP_ReGetMp3Data zyb_mp3_data_regetting=%d zyb_mp3_data_is_getting=%d",zyb_mp3_data_regetting,zyb_mp3_data_is_getting);
    ZYB_STREAM_LOG("MMIZYB_HTTP_ReGetMp3Data zyb_mp3_data_is_over=%d s_zyb_http_is_poping_data=%d",zyb_mp3_data_is_over,MMIZYB_HTTP_IsSending());
    if(zyb_mp3_data_regetting == FALSE && zyb_mp3_data_is_getting == FALSE && zyb_mp3_data_is_over == FALSE && FALSE == MMIZYB_HTTP_IsSending() && SIO_Stream_Data_Origin_GetLeftLen() >= STREAM_DATA_ORIGIN_LEFT_LEN)
    {
        SCI_GetMutex(zyb_streamplayer_mutexPtr, SCI_WAIT_FOREVER);
        zyb_mp3_data_regetting = TRUE;
        MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET,PNULL,zyb_mp3_data_cur_start_offset+zyb_mp3_data_offset);
        SCI_PutMutex(zyb_streamplayer_mutexPtr);
    }
    return 0;
}

/*****************************************************************************/
//  Description :call back function for mp3 driver
//  Global resource dependence : none
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL AUDIO_STREAM_GET_SRC_RESULT_E ZYB_MP3_callback(//HAUDIO hAudio, uint8** const ppucDataAddr, uint32* const puiDataLength) 
    HAUDIO hAudio, 
    AUDIO_GETSRCDATA_INFO_T * const ptSrcDataInfo,
    void *pvOtherInfo
    )
{
    uint32 puiDataLength;
    uint32 tmp_data;
    SCI_ASSERT(ptSrcDataInfo!=PNULL);/*assert verified*/

    //ptSrcDataInfo->tStreamCurSrcDataInfo.pucDataAddr  = PNULL;
    puiDataLength = 0;
    
    if (s_dec_ctx.i_state != DECODING)
    {
        ptSrcDataInfo->tStreamCurSrcDataInfo.uiDataLength = 0;
        ZYB_STREAM_LOG("ZYB_MP3_callback error");
        return AUDIO_STREAM_GET_SRC_ERROR;
    }
      
    puiDataLength = SIO_Stream_Data_Origin_Get_Stream(zyb_mp3_buf,MP3_DRIVER_BUF_SIZE,MP3_DRIVER_BUF_SIZE);
    ZYB_STREAM_LOG("ZYB_MP3_callback puiDataLength=%d,offset=%ld",puiDataLength,zyb_mp3_buf_offset);
    if(puiDataLength > 0)
    {
        ptSrcDataInfo->tStreamCurSrcDataInfo.pucDataAddr  = (uint8*)zyb_mp3_buf;	
        ptSrcDataInfo->tStreamCurSrcDataInfo.uiDataLength = puiDataLength;
        MMIZYB_HTTP_ReGetMp3Data();
        sio_stream_data_origin_s.play_size += puiDataLength;
    }
    else
    {
        ZYB_STREAM_LOG("ZYB_MP3_callback Err No Data Wait is_over=%d play_size:%d fileSize;%d",zyb_mp3_data_is_over,sio_stream_data_origin_s.play_size,SIO_Stream_Data_Get_Origin_File_Size());
        if(zyb_mp3_data_is_over || sio_stream_data_origin_s.play_size >= SIO_Stream_Data_Get_Origin_File_Size())
        {
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOSTOP,PNULL,0);
            return AUDIO_STREAM_GET_SRC_ERROR;
        }
        else
        {
            MMIZYB_HTTP_ReGetMp3Data();
            return AUDIO_STREAM_GET_SRC_WAIT;
        }
    }
    return AUDIO_STREAM_GET_SRC_SUCCESS;
}


LOCAL void zyb_mp3_dec_dummy_callbak(
    HAUDIO hAudio, 
    uint32 notify_info, 
    uint32 affix_info
)
{
    return;
}

LOCAL void amr_dec_dummy_callbak(
    HAUDIO hAudio, 
    uint32 notify_info, 
    uint32 affix_info
)
{
    return;
}

LOCAL AUDIO_STREAM_GET_SRC_RESULT_E ZYB_AMR_callback(//HAUDIO hAudio, uint8** const ppucDataAddr, uint32* const puiDataLength) 
    HAUDIO hAudio, 
    AUDIO_GETSRCDATA_INFO_T * const ptSrcDataInfo,
    void *pvOtherInfo
    )
{
    uint32 puiDataLength;
    uint32 tmp_data;
    SCI_ASSERT(ptSrcDataInfo!=PNULL);/*assert verified*/

    //ptSrcDataInfo->tStreamCurSrcDataInfo.pucDataAddr  = PNULL;
    puiDataLength = 0;
    
    if (s_dec_ctx.i_state != DECODING)
    {
        ptSrcDataInfo->tStreamCurSrcDataInfo.uiDataLength = 0;
        ZYB_STREAM_LOG("ZYB_MP3_callback error");
        return AUDIO_STREAM_GET_SRC_ERROR;
    }
      
    puiDataLength = SIO_Stream_Data_Origin_Get_Stream(zyb_mp3_buf,MP3_DRIVER_BUF_SIZE,512);
    ZYB_STREAM_LOG("ZYB_MP3_callback puiDataLength=%d,offset=%ld",puiDataLength,zyb_mp3_buf_offset);
    if(puiDataLength > 0)
    {
        ptSrcDataInfo->tStreamCurSrcDataInfo.pucDataAddr  = (uint8*)zyb_mp3_buf;	
        ptSrcDataInfo->tStreamCurSrcDataInfo.uiDataLength = puiDataLength;
        MMIZYB_HTTP_ReGetMp3Data();
    }
    else
    {
        ZYB_STREAM_LOG("ZYB_MP3_callback Err No Data Wait is_over=%d",zyb_mp3_data_is_over);
        if(zyb_mp3_data_is_over)
        {
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOSTOPDELAY,PNULL,0);
            return AUDIO_STREAM_GET_SRC_ERROR;
        }
        else
        {
            MMIZYB_HTTP_ReGetMp3Data();
            return AUDIO_STREAM_GET_SRC_WAIT;
        }
    }
    return AUDIO_STREAM_GET_SRC_SUCCESS;
}

/*****************************************************************************/
//  Description : API of start mp3 stream play
//  Global resource dependence : none
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL int MP3_Play_Start(void)
{
    ZYB_STREAM_LOG("MP3_Play_Start");
    s_dec_ctx.i_state = DECODING;
    AUDIO_SetDevMode(AUDIO_DEVICE_MODE_HANDFREE);
    AUDIO_SetVolume(zyb_player_Volume);
    AUDIO_Play(s_dec_ctx.audioHandle, 0);
    return TRUE;
}

LOCAL int amr_CreateStreamBufferHandle()
{
    uint32 track_buf_size = (80 * 150);
    #ifndef WIN32
    __es83xx_cfg_spk_out_mod(1);
#endif
    s_dec_ctx.audioHandle = AUDIO_CreateStreamBufferHandle(
                                        hAMRCodec,
                                        PNULL,
                                        hAUDDEV,
                                        PNULL,
                                        (uint8 *)PNULL,
                                        0,
                                        (uint32 *)(VBDecodeOuputBuf),
                                        track_buf_size,
                                        amr_dec_dummy_callbak,
                                        ZYB_AMR_callback
                                        );
    if ( !s_dec_ctx.audioHandle)
    {
#ifndef WIN32
    __es83xx_cfg_spk_out_mod(0);
#endif
        return SCI_ERROR;
    }
    return SCI_SUCCESS;
}

LOCAL int mp3_CreateStreamBufferHandle()
{
    uint32 track_buf_size;
    s_dec_ctx.MP3_Info.audioSamplingRate = mp3_header_info.unMp3StreamData.tMp3FormatInfo.uiSampleRate;
    s_dec_ctx.MP3_Info.audioChannels = mp3_header_info.unMp3StreamData.tMp3FormatInfo.uiChannelNum;
    if (s_dec_ctx.MP3_Info.audioSamplingRate <= 12000)
    {
        track_buf_size = MP3_DRIVER_BUF_SIZE >> 2;
    }
    else if (s_dec_ctx.MP3_Info.audioSamplingRate <= 24000)
    {
        track_buf_size = MP3_DRIVER_BUF_SIZE >> 1;
    }
    else
    {
        track_buf_size = MP3_DRIVER_BUF_SIZE;
    }
    s_dec_ctx.audioHandle = AUDIO_CreateStreamBufferHandle(
                                            hMP3DSPCodec,
                                            PNULL,
                                            hAUDDEV,
                                            PNULL,
                                            (uint8 *)&mp3_header_info,
                                            sizeof(MP3_STREAM_BUFFER_T),
                                            (uint32 *)(VBDecodeOuputBuf),
                                            track_buf_size,
                                            zyb_mp3_dec_dummy_callbak,
                                            ZYB_MP3_callback
                                            );
    if ( !s_dec_ctx.audioHandle)
    {
        return SCI_ERROR;
    }
    return SCI_SUCCESS;
}

/*****************************************************************************/
//  Description : API of start mp3 stream open
//  Global resource dependence : none
//  Author:
//  Note: 
/*****************************************************************************/
LOCAL int MP3_Play_Open(void)
{
    ZYB_STREAM_LOG("MP3_Play_Open");
    if (s_dec_ctx.i_state == DECODING)
    {
        return SCI_ERROR;
    }
    if(audio_format_type == AUDIO_AMR_FORMAT)
    {
        return amr_CreateStreamBufferHandle();
    }
    else if(audio_format_type == AUDIO_MP3_FORMAT)
    {
        return mp3_CreateStreamBufferHandle();
    }
    else
    {
        ZYB_STREAM_LOG("MP3_Play_Open error unkown audio format");
    }
}

/*****************************************************************************/
//  Description : API of  mp3 play demo 
//  Global resource dependence : none
//  Note: 
/*****************************************************************************/

LOCAL void MP3_Demo_Start(void)
{
    ZYB_STREAM_LOG("MP3_Demo_Start");
    if(SCI_SUCCESS ==  MP3_Play_Open())
    {
        zyb_mp3_buf_offset = 0;
        MP3_Play_Start();
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_PLAYING);
    }
    else
    {
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_ERROR);
    }
}

/*****************************************************************************/
//  Description : API of  mp3 stop demo 
//  Global resource dependence : none
//  Note: 
/*****************************************************************************/
LOCAL void MP3_Demo_Stop(void)
{
    ZYB_STREAM_LOG("MP3_Demo_Stop");
    if (s_dec_ctx.i_state == DECODING)
    {
        s_dec_ctx.i_state = IDLE;
        AUDIO_Stop(s_dec_ctx.audioHandle);
    }
    if (s_dec_ctx.audioHandle)
    {
        AUDIO_CloseHandle(s_dec_ctx.audioHandle);
        s_dec_ctx.audioHandle = NULL;
#ifndef WIN32
        __es83xx_cfg_spk_out_mod(0);
#endif
    }
    zyb_mp3_buf_offset = 0;
}


/*****************************************************************************/
//  Description : API of  mp3 pause demo 
//  Global resource dependence : none
//  Note: 
/*****************************************************************************/
LOCAL void MP3_Demo_Pause(void)
{
    if (s_dec_ctx.i_state == DECODING)
    {
        AUDIO_Pause(s_dec_ctx.audioHandle);
        s_dec_ctx.i_state = IDLE;
    }

}

/*****************************************************************************/
//  Description : API of  mp3 resume demo 
//  Global resource dependence : none
//  Note: 
/*****************************************************************************/
LOCAL void MP3_Demo_Resume(void)
{
    ZYB_STREAM_LOG("MP3_Demo_Resume");
    if (s_dec_ctx.i_state == IDLE)
    {
        s_dec_ctx.i_state = DECODING;
        AUDIO_Resume(s_dec_ctx.audioHandle);
    }
}

LOCAL void MP3_Demo_SetVolume(uint32 vol)
{
    ZYB_STREAM_LOG("MP3_Demo_SetVolume vol=%d");
    if (s_dec_ctx.audioHandle)
    {
        AUDIO_SetVolume(vol);
    }
}

LOCAL int ZYB_StreamPlayer_Play(void)
{
    ZYB_STREAM_LOG("ZYB_StreamPlayer_Play");
    MP3_Demo_Start();
    return 0;
}

int ZYB_StreamPlayer_Pause(void)
{
    ZYB_STREAM_LOG("ZYB_StreamPlayer_Pause");
    if(zyb_playerStatus == ZYB_STREAM_PLAYER_STATUS_PLAYING)
    {
        MP3_Demo_Pause();
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_PAUSED);
    }
    return 0;
}

int ZYB_StreamPlayer_Resume(void)
{
    ZYB_STREAM_LOG("ZYB_StreamPlayer_Resume");
    if(zyb_playerStatus == ZYB_STREAM_PLAYER_STATUS_PAUSED)
    {
        MP3_Demo_Resume();
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_PLAYING);
    }
    return 0;
}

int ZYB_StreamPlayer_Stop(void)
{
    ZYB_STREAM_LOG("ZYB_StreamPlayer_Stop");
    if(zyb_playerStatus != ZYB_STREAM_PLAYER_STATUS_STOP)
    {
        MP3_Demo_Stop();
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_STOP);
        MMIZYB_HTTP_AppStop();
        SIO_Stream_Data_Init();
        return 1;
    }
    return 0;
}

int ZYB_StreamPlayer_Start(const char *url)
{
    int len = url != NULL ? strlen(url) : 0;

    if(len <= 0) {
        ZYB_STREAM_LOG("ZYB_StreamPlayer_Start  invalid url");
        return -1;
    }
    ZDT_TCP_StopStopDelayTimer();
    if(0 == ZYB_StreamPlayer_Stop())
    {
        SIO_Stream_Data_Init();
    }
    
    ZYB_STREAM_LOG("ZYB_StreamPlayer_Start: %s", url);

    if(zyb_audio_url != PNULL)
    {
        SCI_FREE(zyb_audio_url);
    }

    zyb_audio_url = SCI_ALLOC_APPZ(len + 1);
    if(zyb_audio_url != PNULL)
    {
        strncpy(zyb_audio_url, url, len);
        zyb_audio_url[len] = '\0';
    }
    else
    {
        return -1;
    }

    if(zyb_streamplayer_mutexPtr == NULL) {
    	zyb_streamplayer_mutexPtr = SCI_CreateMutex("zybplayer-mutex", SCI_INHERIT);
    }

    if(FALSE == MMIZYB_HTTP_UrlGetFirst(url))
    {
        return -2;
    }
    ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_CONNECTING);
    return 0;
}

int ZYB_StreamPlayer_SetVolume(uint32 vol)
{
    ZYB_STREAM_LOG("ZYB_StreamPlayer_SetVolume vol=%d",vol);
    if(vol >= MMISET_VOL_MAX)
    {
        vol = MMISET_VOL_MAX;
    }
    MP3_Demo_SetVolume(vol);
    zyb_player_Volume = vol;
    return 0;
}

int ZYB_StreamPlayer_VolumeUp(void)
{
    uint32 vol = zyb_player_Volume;
    ZYB_STREAM_LOG("ZYB_StreamPlayer_VolumeUp vol=%d",vol);
    if(vol < MMISET_VOL_MAX)
    {
        vol++;
        ZYB_StreamPlayer_SetVolume(vol);
    }
    return 0;
}

int ZYB_StreamPlayer_VolumeDown(void)
{
    uint32 vol = zyb_player_Volume;
    ZYB_STREAM_LOG("ZYB_StreamPlayer_VolumeDown vol=%d",vol);
    if(vol > 1)
    {
        vol--;
        ZYB_StreamPlayer_SetVolume(vol);
    }
    return 0;
}

BOOLEAN ZYB_StreamPlayer_IsPlaying(void)
{
    if(zyb_playerStatus != ZYB_STREAM_PLAYER_STATUS_STOP)
    {
        return TRUE;
    }
    return FALSE;
}
#endif

#if 1

PUBLIC int  MMIZYB_HTTP_CheckMp3Format(uint8 * pRcv,uint32 Rcv_len)
{
#ifdef ZYB_STREAM_PLAY_AMR
    mp3_header_info.bType = TRUE;
#else
    rtmp_mp3_frame_info_t info = {0};
    rtmp_mp3_decode_frame_info(pRcv, Rcv_len, &info);
    ZYB_STREAM_LOG("MMIZYB_HTTP_CheckMp3Format channels=%d,hz=%ld",info.channels,info.hz);
    if (info.hz == 0) {
        info.channels = 1;
        info.hz = 44100;
    }
    mp3_header_info.bType = TRUE;
    mp3_header_info.unMp3StreamData.tMp3FormatInfo.uiChannelNum = info.channels;
    mp3_header_info.unMp3StreamData.tMp3FormatInfo.uiSampleRate = info.hz;
#endif
    return 0;
}

PUBLIC int  MMIZYB_HTTP_GetNewMp3Data(void)
{
    if(SIO_Stream_Data_Origin_GetLeftLen() >= STREAM_DATA_ORIGIN_LEFT_LEN)
    {
        MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET,PNULL,zyb_mp3_data_cur_start_offset+zyb_mp3_data_offset);
        return 1;
    }
    return 0;
}

PUBLIC uint32  MMIZYB_HTTP_PutMp3Data(uint8 * pRcv,uint32 Rcv_len)
{
    uint32 put_len = 0;
    put_len = SIO_Stream_Data_Put(pRcv,Rcv_len); 
    zyb_mp3_data_cur_start_offset += put_len;
    return put_len;
}

PUBLIC int  MMIZYB_HTTP_UrlGetMp3Data_CB(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 file_len,uint32 err_id)
{
    ZYB_STREAM_LOG("MMIZYB_HTTP_UrlGetMp3Data_CB(%d) file_len=%d,Rcv_len=%d",is_ok,file_len,Rcv_len);
    if(zyb_playerStatus == ZYB_STREAM_PLAYER_STATUS_STOP)
    {
        return 0;
    }
    if(is_ok)
    {
        if(file_len > 0)
        {
            if(pRcv == NULL || Rcv_len == 0)
            {
                return 0;
            }
            if(zyb_mp3_data_need_check)
            {
                zyb_mp3_data_need_check = FALSE;
                MMIZYB_HTTP_CheckMp3Format(pRcv,Rcv_len);
                MMIZYB_HTTP_PutMp3Data(pRcv,Rcv_len);
                MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOPLAY,PNULL,0);
            }
            else
            {
                MMIZYB_HTTP_PutMp3Data(pRcv,Rcv_len);
            }
        }
        else
        {
            if(MMIZYB_HTTP_GetNewMp3Data() == 0)
            {
                zyb_mp3_data_is_getting = FALSE;
            }
        }
    }
    else
    {
        if(err_id == HTTP_ERROR_SERVER_CLOSE)
        {
            zyb_mp3_data_is_over = TRUE;
            zyb_mp3_data_is_getting = FALSE;
        }
        else
        {
            if(MMIZYB_HTTP_GetNewMp3Data() == 0)
            {
                zyb_mp3_data_is_getting = FALSE;
            }
        }
    }
    return 0;
}

PUBLIC BOOLEAN  MMIZYB_HTTP_UrlGetMp3Data(char * url,uint32 start_len, uint32 end_len)
{
    BOOLEAN res = FALSE;
    res = MMIZYB_HTTP_AppSend(TRUE,url,(uint8 *)PNULL,0,2,0,0,0,0,start_len,end_len,MMIZYB_HTTP_UrlGetMp3Data_CB);
    if(res)
    {
        zyb_mp3_data_is_getting = TRUE;
    }
    return res;
}

BOOLEAN MMIZYB_HTTP_Handle_AudioGet(uint32 offset_len)
{
    uint32 start_len = offset_len;
    uint32 end_len = 0;
    uint32 need_len = SIO_Stream_Data_Origin_GetLeftLen();
    ZYB_STREAM_LOG("MMIZYB_HTTP_Handle_AudioGet offset_len=%d,need_len=%d",offset_len,need_len);
    end_len = start_len+need_len-1;
    if(FALSE == MMIZYB_HTTP_UrlGetMp3Data(zyb_audio_url,start_len,end_len))
    {
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_ERROR);
    }
    SCI_GetMutex(zyb_streamplayer_mutexPtr, SCI_WAIT_FOREVER);
    if(zyb_mp3_data_regetting)
    {
        zyb_mp3_data_regetting = FALSE;
    }
    SCI_PutMutex(zyb_streamplayer_mutexPtr);
    return TRUE;
}

BOOLEAN MMIZYB_HTTP_Handle_AudioPlay(void)
{
    ZYB_StreamPlayer_Play();
    return TRUE;
}

uint8 g_zyb_play_stop_delay_timer_id = 0;
static void ZDT_TCP_HandleStopDelayTimer(
                                uint8 timer_id,
                                uint32 param
                                )
{
    if(timer_id == g_zyb_play_stop_delay_timer_id && 0 != g_zyb_play_stop_delay_timer_id)
    {
        MMK_StopTimer(g_zyb_play_stop_delay_timer_id);
        g_zyb_play_stop_delay_timer_id = 0;
    }
    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOSTOP,PNULL,0);
}

static void ZDT_TCP_StartStopDelayTimer(uint32 time_ms)
{
    if(0 != g_zyb_play_stop_delay_timer_id)
    {
        MMK_StopTimer(g_zyb_play_stop_delay_timer_id);
        g_zyb_play_stop_delay_timer_id = 0;
    }
    
    g_zyb_play_stop_delay_timer_id = MMK_CreateTimerCallback(time_ms, 
                                                                        ZDT_TCP_HandleStopDelayTimer, 
                                                                        (uint32)0, 
                                                                        FALSE);
}

PUBLIC void ZDT_TCP_StopStopDelayTimer(void)
{
    if(0 != g_zyb_play_stop_delay_timer_id)
    {
        MMK_StopTimer(g_zyb_play_stop_delay_timer_id);
        g_zyb_play_stop_delay_timer_id = 0;
    }
}
BOOLEAN MMIZYB_HTTP_Handle_AudioStop(void)
{
    ZYB_StreamPlayer_Stop();
    MMIZDT_CleanPlayAudioIcon();
    return TRUE;
}

BOOLEAN MMIZYB_HTTP_Handle_AudioStopDelay(void)
{
    ZDT_TCP_StartStopDelayTimer(1000);
    return TRUE;
}

LOCAL uint32 get_audio_header_len(const char *header, const int check_size)
{
    int totalSize = 0;
    //amr 格式头最少6个byte
    if(check_size < 6)
    {
        totalSize == 0;
    }
    else if(header[0] == '#' && header[1] == '!'&& header[2] == 'A' && header[3] == 'M'&& header[4] == 'R')
    {
        totalSize == 6; //amr格式
    }
    else
    {
	    if (check_size < 10) 
        {
		    totalSize == 0;
        }
	    else if (header[0] == 'I' && header[1] == 'D' && header[2] == '3')
        {
		   totalSize =	(header[6] & 0x7F) * 0x200000 + (header[7] & 0x7F) * 0x4000 + (header[8] & 0x7F) * 0x80 + (header[9] & 0x7F) + 10;
        }	    
    }
    ZYB_STREAM_LOG("get_audio_header_len header[0]:%c header[1]:%c header[2]:%c",header[0],header[1],header[2]);
	return totalSize;
}

PUBLIC int  MMIZYB_HTTP_UrlGetFirst_CB(BOOLEAN is_ok,uint8 * pRcv,uint32 Rcv_len,uint32 file_len,uint32 err_id)
{
    int id3_len = 0;
    ZYB_STREAM_LOG("MMIZYB_HTTP_UrlGetFirst_CB(%d) file_len=%d,Rcv_len=%d",is_ok,file_len,Rcv_len);
    if(is_ok)
    {
        if(pRcv == NULL || Rcv_len < 10)
        {
            ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_ERROR);
            return 0;
        }
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_CACHED);

        id3_len = get_audio_header_len(pRcv,Rcv_len);

        ZYB_STREAM_LOG("MMIZYB_HTTP_UrlGetFirst_CB id3_len=%d",id3_len);
        zyb_mp3_data_offset = id3_len;
        sio_stream_data_origin_s.play_size = zyb_mp3_data_offset;
        zyb_mp3_data_is_over = FALSE;
        zyb_mp3_data_cur_start_offset = 0;
        zyb_mp3_data_is_getting = FALSE;
        SCI_GetMutex(zyb_streamplayer_mutexPtr, SCI_WAIT_FOREVER);
        zyb_mp3_data_regetting = FALSE;
        SCI_PutMutex(zyb_streamplayer_mutexPtr);
        if(id3_len == 0)
        {
            zyb_mp3_data_need_check = FALSE;
            audio_format_type = AUDIO_MP3_FORMAT;
            MMIZYB_HTTP_CheckMp3Format(pRcv,Rcv_len);
            MMIZYB_HTTP_PutMp3Data(pRcv,Rcv_len);
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET,PNULL,Rcv_len);
            MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOPLAY,PNULL,0);
        }
        else
        {
            uint32 left_len = 0;
            if(Rcv_len > id3_len)
            {
                left_len = Rcv_len-id3_len;
                //amr 格式
                if(id3_len == 6 && left_len < 512)
                {
                    zyb_mp3_data_need_check = FALSE;
                    audio_format_type = AUDIO_AMR_FORMAT;
                    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET,PNULL,zyb_mp3_data_offset);
                }
                else if(left_len < 30720) //mp3 格式
                {
                    zyb_mp3_data_need_check = TRUE;
                    audio_format_type = AUDIO_MP3_FORMAT;
                    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET,PNULL,zyb_mp3_data_offset);
                }
                else
                {
                    zyb_mp3_data_need_check = FALSE;
                    audio_format_type = AUDIO_UNKOWN_FORMAT;
                    MMIZYB_HTTP_CheckMp3Format(pRcv+id3_len,left_len);
                    MMIZYB_HTTP_PutMp3Data(pRcv+id3_len,left_len);
                    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET,PNULL,Rcv_len);
                    MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOPLAY,PNULL,0);
                }
            }
            else
            {
                zyb_mp3_data_need_check = TRUE;
                MMIZYB_SendSigTo_APP(ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET,PNULL,zyb_mp3_data_offset);
            }
        }
    }
    else
    {
        ZYB_StreamPlayer_Status_CB(ZYB_STREAM_PLAYER_STATUS_ERROR);
    }
    return 0;
}

PUBLIC BOOLEAN  MMIZYB_HTTP_UrlGetFirst(char * url)
{
    BOOLEAN res = FALSE;
    res = MMIZYB_HTTP_AppSend(TRUE,(char *)url,(uint8 *)PNULL,0,1,0,0,0,STREAM_DATA_ORIGIN_FIRST_PLAY_LEN,0,STREAM_DATA_ORIGIN_FIRST_PLAY_LEN-1,MMIZYB_HTTP_UrlGetFirst_CB);
    return res;
}

#endif


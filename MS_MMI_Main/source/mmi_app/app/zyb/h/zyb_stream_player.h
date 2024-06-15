#ifndef _AMR_STREAM_DEMO_H_
#define _AMR_STREAM_DEMO_H_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         FUNCTION DEFINITION                              *
 **--------------------------------------------------------------------------*/
#define ZYB_STREAM_LOG            ZYB_STREAM_Trace

typedef enum
{
	ZYB_STREAM_PLAYER_STATUS_STOP = 0,
	ZYB_STREAM_PLAYER_STATUS_CONNECTING,
	ZYB_STREAM_PLAYER_STATUS_CACHED,
	ZYB_STREAM_PLAYER_STATUS_PLAYING,
	ZYB_STREAM_PLAYER_STATUS_PAUSED,
	ZYB_STREAM_PLAYER_STATUS_ERROR,
	ZYB_STREAM_PLAYER_STATUS_MAX,
} zyb_stream_player_status_t;

typedef enum 
{
    AUDIO_UNKOWN_FORMAT = 0,
    AUDIO_AMR_FORMAT, //amr格式音频
    AUDIO_MP3_FORMAT, //mp3格式
} AUDIO_FORMAT_TYPE;

PUBLIC int ZYB_StreamPlayer_Pause(void);
PUBLIC int ZYB_StreamPlayer_Resume(void);
PUBLIC int ZYB_StreamPlayer_Stop(void);
PUBLIC int ZYB_StreamPlayer_Start(const char *url);
PUBLIC int ZYB_StreamPlayer_SetVolume(uint32 vol);
PUBLIC int ZYB_StreamPlayer_VolumeUp(void);
PUBLIC int ZYB_StreamPlayer_VolumeDown(void);
 

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif

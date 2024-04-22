/*************************************************************
 * File  :  video_call_demo.h
 * Module:  agora_video_call API demo
 *
 * This is a part of the RTSA call Service SDK.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/
#ifndef _VIDEO_CALL_H_
#define _VIDEO_CALL_H_

#ifdef __cplusplus
extern  "C"
{
#endif

#if defined(VIDEO_CALL_AGORA_SUPPORT)
#include "agora_video_call.h"
#endif
#if defined(BAIDU_VIDEOCHAT_SUPPORT)
#include "baidu_videocall.h"
#endif
#include "zdt_net.h"

#if defined(BAIDU_VIDEOCHAT_SUPPORT)
typedef void (*VIDEO_CALL_STATUS_CALLBACK)(brtc_msg_t *);
#else
typedef void (*VIDEO_CALL_STATUS_CALLBACK)(avc_msg_t *);
#endif

//int video_call_start();
PUBLIC int video_call_start(char *appid, char *channel_name, VIDEO_CALL_STATUS_CALLBACK status_callback);
PUBLIC int video_call_stop(void);
PUBLIC BOOLEAN  Video_Call_Device_Idle_Check(void);
PUBLIC void Video_Call_Check(void);
PUBLIC BOOLEAN VideoChat_IsInCall();

#ifdef __cplusplus
}
#endif
#endif  /* _VIDEO_CALL_DEMO_H_ */

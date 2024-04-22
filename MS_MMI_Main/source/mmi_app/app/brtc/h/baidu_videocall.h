
#ifndef __BAIDU_VIDEO_CALL_H__
#define __BAIDU_VIDEO_CALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#ifndef WIN32
#include <stdbool.h>
#else
typedef int bool;
#define false 0
#define true 1
#endif



// SAME AS  agora_video_call.h  FOR BAIDU VIDEO UI
typedef enum {
    BRTC_MSG_JOIN_CHANNEL_SUCCESS = 0x1000,
    BRTC_MSG_USER_JOINED,
    BRTC_MSG_USER_OFFLINE,
    BRTC_MSG_CONNECTION_LOST,
    BRTC_MSG_INVALID_APP_ID,
    BRTC_MSG_INVALID_CHANNEL_NAME,
    BRTC_MSG_INVALID_TOKEN,
    BRTC_MSG_TOKEN_EXPIRED,
    BRTC_MSG_FIRST_VIDEO_FRAME_RECV,

    BRTC_MSG_LICENSE_PARAM_INVALID,
    BRTC_MSG_LICENSE_DNS_PARSE_FAILED,
    BRTC_MSG_LICENSE_NET_FAILED,
    BRTC_MSG_LICENSE_ACTIVATE_FAILED,

    BRTC_MSG_KEY_FRAME_REQUEST,
    BRTC_MSG_BWE_TARGET_BITRATE_UPDATE,
} brtc_msg_e;


typedef struct {
    uint32_t target_bitrate;
} brtc_bwe_t;


typedef struct {
    brtc_msg_e     code;

    union {
        brtc_bwe_t bwe;
    } data;
} brtc_msg_t;










#ifdef __cplusplus
}
#endif
#endif /* __BAIDU_VIDEO_CALL_H__ */



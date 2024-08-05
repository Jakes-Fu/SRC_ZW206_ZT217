#ifndef __LEBAO_EVENT_H
#define __LEBAO_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sds.h"

typedef enum
{
	EVT_SRV_LEBAO_BEGIN = 0,
	EVT_SRV_LEBAO_CHART_RESULT,
	EVT_SRV_LEBAO_IMAGE_UPDATE,
	EVT_SRV_LEBAO_MUSICLIST_RESULT,
	EVT_SRV_LEBAO_VOICE_RESULT,
	EVT_SRV_LEBAO_KEYWORD_RESULT,
	EVT_SRV_LEBAO_DOWNLOAD_RESULT,
	EVT_SRV_LEBAO_SAVE_IMAGE_RESULT,
	EVT_SRV_LEBAO_CHART_IMAGE_RESULT,
	EVT_SRV_LEBAO_MUSICLIST_IMAGE_RESULT,
	EVT_SRV_LEBAO_RUNONCE_RESULT,
	EVT_SRV_LEBAO_ORDER_RESULT,
	EVT_SRV_LEBAO_STREAM_DATA,
	EVT_SRV_LEBAO_STREAM_END,
	EVT_SRV_LEBAO_STREAM_STATUS,
	EVT_SRV_LEBAO_USER_AGREEMENT,
	EVT_SRV_LEBAO_IVR_INFO,
	EVT_SRV_LEBAO_TIP_SOUND,
	EVT_SRV_LEBAO_ORDER_CHECK_RESULT,
	EVT_SRV_LEBAO_POST_CAPTCHA_RESULT,
	EVT_SRV_LEBAO_SONGLIST_RESULT,
	EVT_SRV_LEBAO_CONTENTBRIEFLIST_RESULT,
	EVT_SRV_LEBAO_COLLECT_GET_LIST_RESULT,
	EVT_SRV_LEBAO_COLLECT_DEL_LIST_RESULT,
	EVT_SRV_LEBAO_COLLECT_ADD_RESULT,
	EVT_SRV_LEBAO_COLLECT_DEL_RESULT,
	EVT_SRV_LEBAO_DEFAULT_CONTENTBRIEFLIST,

	EVT_CTRL_LEBAO_QUIT = 100,
	EVT_CTRL_LEBAO_CREATE_PAGE,
	EVT_CTRL_LEBAO_CLOSE_PAGE,
	EVT_CTRL_LEBAO_SET_RING,
	
	EVT_CTRL_LEBAO_PLAY_MUSIC,
	EVT_CTRL_LEBAO_PAUSE_MUSIC,
	EVT_CTRL_LEBAO_RESUME_MUSIC,
	EVT_CTRL_LEBAO_STOP_MUSIC,

	EVT_CTRL_LEBAO_START_RECORD,
	EVT_CTRL_LEBAO_STOP_RECORD,
	EVT_CTRL_LEBAO_POST_RECORD,

	EVT_CTRL_LEBAO_SET_VOLUME,
	EVT_CTRL_LEBAO_ADD_FAVORITE,
	EVT_CTRL_LEBAO_DEL_FAVORITE,

	EVT_LEBAO_PLAY_STATUS = 200,
	EVT_LEBAO_RECORD_STATUS,
	EVT_LEBAO_SEARCH_STATUS,
	EVT_LEBAO_URI_UPDATE,
	EVT_LEBAO_UPDATE_INFO,
	EVT_LEBAO_RING_RESULT,
} lebao_evt_id_t;

typedef void(*LEBAO_EVENT_PROC)(const int eventId, const int cmd, void* data);

typedef struct
{
	lebao_evt_id_t evtId;
	LEBAO_EVENT_PROC callback;
} lebao_evt_callback_t;

typedef struct
{
	int eventId;
	int pageId;
	int cmd;
	void* data;
	int priority;
    OS_DATA_CALLBACK dataFreeCb;
} lebao_evt_t;

void lebao_event_init(void);
void lebao_event_close(void);
void lebao_event_task_run(void * data);

void lebao_event_send_priority_cb(const int eventId, const int pageId, const int cmd, void* data, int priority, OS_DATA_CALLBACK dataFreeCb);
void lebao_event_send(const int eventId, const int pageId, const int cmd, void* data);
void lebao_event_send_priority(const int eventId, const int pageId, const int cmd, void* data, int priority);
void lebao_event_send_cb(const int eventId, const int pageId, const int cmd, void* data, OS_DATA_CALLBACK dataFreeCb);
void lebao_event_add_listen(int eventId, int pageId, LEBAO_EVENT_PROC callback);
void lebao_event_remove_listen(int eventId, int pageId);
void lebao_event_add_view(const lebao_evt_callback_t callback[], const int pageId);
void lebao_event_remove_view(const int pageId);

#ifdef __cplusplus
}
#endif

#endif /*__LEBAO_EVENT_H*/

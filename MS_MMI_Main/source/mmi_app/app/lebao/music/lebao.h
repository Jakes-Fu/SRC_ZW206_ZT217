#ifndef __LEBAO_APP_H
#define __LEBAO_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sci_types.h"

typedef int(*LEBAO_COMMON_CALLBACK_T)(const int cmd, const int status, void *data);

typedef enum
{
	LEBAO_STATUS_STOP = 0,
	LEBAO_STATUS_PLAY,
	LEBAO_STATUS_PAUSE,
	LEBAO_STATUS_RESUME,
	LEBAO_STATUS_END,
	LEBAO_STATUS_ERROR,
	LEBAO_STATUS_FAILED,
    LEBAO_STATUS_RING,
	LEBAO_STATUS_NEXT_SONG,
	LEBAO_STATUS_URI_UPDATE,
    LEBAO_STATUS_WAITING,
    LEBAO_STATUS_OUT_OF_DATA
} lebao_play_status_t;

typedef enum
{
	LEBAO_STATUS_RECORD_STOP = 0,
	LEBAO_STATUS_RECORD_START,
	LEBAO_STATUS_RECORD_POST_START,
	LEBAO_STATUS_RECORD_POST_FAILED,
	LEBAO_STATUS_RECORD_ERROR,
	LEBAO_STATUS_RECORD_HAS_RESULT,
	LEBAO_STATUS_RECORD_NO_RESULT,
} lebao_record_status_t;

typedef enum {
	SOUND_LEBAO_SEARCH_LIMIT = 0,
	SOUND_LEBAO_MEMBER_OPENED = 1,
	SOUND_LEBAO_RING_CLICK = 5,
	SOUND_LEBAO_SCAN_QR = 6,
	SOUND_LEBAO_PUSH_TALK = 7,
	SOUND_LEBAO_RING_OK = 9,
	SOUND_LEBAO_DEL_FAVORITE = 11,
	SOUND_LEBAO_WELCOME = 13,
	SOUND_LEBAO_WELCOME2 = 14,
	SOUND_LEBAO_WELCOME3 = 15,
} lebao_sound_id_t;

// lebao_app.c
void StartLebaoApp(void);
void StopLebaoApp(void);

// lebao_wintable.c
void MMIAPIMENU_EnterLebao(void);
void MMIAPIMENU_QuitLebao(void);

void lebao_start_event_task_timer(void);
void lebao_stop_event_task_timer(void);
int lebao_event_task_timer_is_running(void);
void lebao_post_ui_event(void* data);

uint32	lebao_create_win(const int pageId);
void	lebao_close_win(const int pageId);
BOOLEAN	lebao_is_win_opened(const int pageId);

#ifdef __cplusplus
}
#endif

#endif

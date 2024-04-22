
/*****************************************************************************/
#ifndef _DUERAPP_MAIN_H_
#define _DUERAPP_MAIN_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmi_appmsg.h"
#include "sci_types.h"
#include "mmk_type.h"
#include "baidu_json.h"
#include "lightduer_connagent.h"
#include "duerapp_payload.h"
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/ 

#ifdef _cplusplus
	extern   "C"
    {
#endif
/*----------------------------------------------------------------------------*/

#define DUERAPP_FULL_SCREEN_RECT        {0,0,MMI_MAINSCREEN_WIDTH,MMI_MAINSCREEN_HEIGHT}

#define DUERAPP_QUERY_FONT_COLOR        RGB8882RGB565(0xB9B9B9)

typedef enum MMIDUERAPP_ANIMATE_STATE
{
    MMIDUERAPP_ANIMATE_STATE_PREPARE  = 0,
    MMIDUERAPP_ANIMATE_STATE_LISTENING,
    MMIDUERAPP_ANIMATE_STATE_THINKING,
    MMIDUERAPP_ANIMATE_STATE_MAX
}MMIDUERAPP_ANIMATE_STATE_E;

typedef enum MMIDUERAPP_MEDIA_STATE
{
    MEDIA_PLAY_STATE_PLAYING  = 0,
    MEDIA_PLAY_STATE_RESUME,
    MEDIA_PLAY_STATE_PAUSE,
    MEDIA_PLAY_STATE_STOP,
    MEDIA_PLAY_STATE_ERROR,
    MEDIA_PLAY_STATE_FINISH
}MMIDUERAPP_MEDIA_STATE_E;

typedef enum
{
    DUERAPP_INTERACT_MODE_NORMAL,
    DUERAPP_INTERACT_MODE_WRITING,
    DUERAPP_INTERACT_MODE_DICTIONARY,
    DUERAPP_INTERACT_MODE_INTERPRETER
} duerapp_interact_mode_t;

/*----------------------------------------------------------------------------*/
/*                          TYPE AND STRUCT                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                         Function Declare                                   */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/
PUBLIC MMI_HANDLE_T MMIDUERAPP_CreateDuerMainWin(BOOLEAN auto_listen);

PUBLIC void MMIDUERAPP_ScreenInputEvent(char *text);

PUBLIC void MMIDUERAPP_ScreenRenderCardEvent(baidu_json *payload);

PUBLIC void MMIDUERAPP_ScreenRenderWeatherEvent(baidu_json *payload);

PUBLIC void MMIDUERAPP_DuerAsrState(duer_event_t *event);

PUBLIC void MMIDUERAPP_MainSetInitStatus(int status);

PUBLIC void MMIDUERAPP_WatchRequireLogin(baidu_json *payload);

PUBLIC wchar* DUERAPP_GetRandomHint(void);

PUBLIC void free_render_player(RENDER_PLAYER_INFO **render_player_info);

PUBLIC void duerapp_show_miguwin(void);

PUBLIC void duerapp_miguwin_close(void);

void duerapp_set_interact_mode(duerapp_interact_mode_t mode);

#ifdef _cplusplus
	}
#endif

#endif//_DUERAPP_SPRD_H_

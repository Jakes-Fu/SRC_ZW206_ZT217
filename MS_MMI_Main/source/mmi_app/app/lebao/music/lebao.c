#include "port_cfg.h"
#include "lebao.h"

#include "std_header.h"
#include "mmk_app.h"
#include "mmi_common.h"
#include "mmk_timer.h"
#include "mmi_module.h"
#include "mmiset_export.h"

#include "lebao_ctrl.h"
#include "lebao_misc.h"
#include "lebao_base.h"

#include "lebao_id.h"
#include "lebao_text.h"
#include "lebao_image.h"

#include "lebao_audio_dev.h"
#include "lebao_network.h"
#include "lebao_ringset.h"

// public variables

// private prototypes
LOCAL void lebao_event_timer_proc(uint8  timer_id, uint32 param);
LOCAL void _lebao_handle_ui_event(uint8 timer_id, uint32 param);
LOCAL int lebao_abs(x)
{
    return (x >= 0) ? (x) : (-x);
}

// private variables
LOCAL uint8 gLebaoEventTimerId = 0;
LOCAL uint32 _lebaoLastStartTime = 0;


#ifdef _WIN32
int REFPARAM_GetGpsLogValue(void)
{
	return 0;
}

void REFPARAM_SetGpsLogValue(int v)
{
}
#endif

// public functions
void MMIAPIMENU_EnterLebao(void)
{
	int ret = 0;
	uint32 nowTime = timestamp_seconds_int();

	// to avoid clicking too fast, it's actually started
	if(lebao_abs(nowTime - _lebaoLastStartTime) <= 1) {
		helper_debug_printf("restart time is too short or abnormal exit, now=%d, last=%d", nowTime, _lebaoLastStartTime);
		return;
	}

	_lebaoLastStartTime = nowTime;

#ifdef _WIN32
	if(1) {
		StartLebaoApp();
		return;
	}
#endif

	if (MMIAPIUDISK_UdiskIsRun()) {
		MMIPUB_OpenAlertWarningWin(TXT_COMMON_UDISK_USING);
		return;
	}
	else if (MMIAPICC_IsInState(CC_IN_CALL_STATE)) {
		return;
	}

	if (lebao_network_connect() != 0)
		return;

	// GetDefaultLanguage

	// MMIAPIENVSET_CloseActModeKeyRingSet();
	StartLebaoApp();
}

void MMIAPIMENU_QuitLebao(void)
{
	StopLebaoApp();
	lebao_deactive_network();

	_lebaoLastStartTime = 0;
}


void StartLebaoApp(void)
{
	lebao_set_max_volume(MMISET_VOL_MAX);
	lebao_set_headset_default_volume();

	// maximum count of songs played automatically, 1 ~ 100
	lebao_player_set_next_max_count(10);

	// recording mode, 0 : file, 1 : streaming
 #if defined(PLATFORM_UMS9117) || defined(PLATFORM_ANTISW3) || defined(PLATFORM_UWS6121E)
	lebao_ctrl_set_record_mode(0);
 #else
  	lebao_ctrl_set_record_mode(1);
 #endif
	
	// mode of playing tip sound, 0 : no, 1 : migu, 2 : lebao
	lebao_ctrl_set_can_play_tip_sound(2);

	// roll the text back and forth, first fix the ctrllabel.c (align)
	lebao_set_label_can_roll();
	
	// start app
	lebao_start_event_task_timer();

	// external relations, continuous ID, non interruptible, for ctrl lib
	lebao_set_res_image_id(IMAGE_LEBAO_LOGO, IMAGE_LEBAO_LOADING);
	lebao_set_res_ctrl_id(MMI_LEBAO_CTRL_ID_MENU_OPTION, MMI_LEBAO_CTRL_ID_LABEL_CAPTCHA);
	lebao_set_res_win_id(MMI_LEBAO_WIN_ID_SPLASH, MMI_LEBAO_WIN_ID_SONGLIST);

    lebao_set_app_key("yisaiunisocbjhy", "f6b30426");
    lebao_set_home_key("15ee94106b1f481e932d0dd0cb529e04", "JCOJN2WV3L8EL5BX");

  // whether to use HTTPS mode, 0: no, 1: yes
	lebao_set_https_mode(1);

	lebao_ctrl_init();

	// turn ringtone settings on or off, 0 : off, 1 : on
	//lebao_player_set_open_ringtone(1);

	// delete MP3 when the disk space is less than 30kb
	if (lebao_ctrl_can_write_file(CFG_MP3_MIN_FILE_SIZE) != 0) {
//		lebao_create_msgbox_id(TEXT_LEBAO_MEMORY_FULL, 2000);
		lebao_ctrl_clear_mp3();
	}
	else {
		int maxSize = (CFG_MP3_MAX_FILE_SIZE < (300 * 1024)) ? (300 * 1024) : CFG_MP3_MAX_FILE_SIZE;

		if (lebao_ctrl_can_write_file(maxSize * 2) == 0) {
			lebao_get_config()->mp3MaxFileSize = maxSize;
		}
	}

	lebao_print_current_ringset();
}

void StopLebaoApp(void)
{
	// free all resources, timer, thread, list, window...
	lebao_exit();
	_lebaoLastStartTime = 0;
}

void lebao_start_event_task_timer(void)
{
	lebao_stop_event_task_timer();
	gLebaoEventTimerId = MMK_CreateTimerCallback(30, lebao_event_timer_proc, 0, TRUE);	
}

void lebao_stop_event_task_timer(void)
{
	if (gLebaoEventTimerId != 0) {
		MMK_StopTimer(gLebaoEventTimerId);
		gLebaoEventTimerId = 0;
	}
	_lebaoLastStartTime = 0;	
}

int lebao_event_task_timer_is_running(void)
{
    return (gLebaoEventTimerId != 0) ? 1 : 0;
}

void lebao_post_ui_event(void* data)
{
  MMK_CreateTimerCallback(30, _lebao_handle_ui_event, (uint32)data, FALSE);
}


#ifndef WIN32
// #if defined(TRACE_INFO_SUPPORT)
void helper_debug_printf(const char *fmt, ...)
{
   char str[256] = {0};
   va_list args;
   
   va_start(args, fmt);
   os_vsnprintf(str, sizeof(str) - 1, fmt, args);
   va_end(args);

   SCI_TraceLow("{%s} %s", "lebao", str);
}
// #else
// void helper_debug_printf(const char *fmt, ...)
// {
// }
// #endif
#endif

// private functions
LOCAL void lebao_event_timer_proc(uint8 timer_id, uint32 param)
{
	lebao_event_task_run(NULL);
}

LOCAL void _lebao_handle_ui_event(uint8 timer_id, uint32 param)
{
  MMK_StopTimer(timer_id);
  if (param != NULL) {
      os_data_callback_param_t* data = (os_data_callback_param_t*)param;
      if (data->callback)
          data->callback(param);
      os_free_val(data);
  }
}

// useless port functions

void lebao_cache_all_free(void)
{
}

void lebao_png_cache_all_free(void)
{
}

void lebao_img_cache_invalidate_src(const void* src)
{
}

void lebao_set_sleep_lcd_only(int only)
{
    (void)only;
}

void lebao_base_init(void)
{
}

void lebao_print_mem_count(void)
{
}


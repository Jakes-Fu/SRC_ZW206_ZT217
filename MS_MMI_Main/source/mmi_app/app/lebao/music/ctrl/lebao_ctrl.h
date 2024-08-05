#ifndef MMI_LEBAO_CTRL_H
#define MMI_LEBAO_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void	lebao_exit(void);
int		lebao_is_running(void);
int		lebao_is_playing(void);
void	lebao_stop_playing(void);
void	lebao_reset_data(void);
void	lebao_set_app_status_callback(int(*start)(const int), int(*stop)(const int));
void    lebao_set_https_mode(const int mode);
void    lebao_set_api_http_mode(const int mode);

void	lebao_set_app_key(const char * appId, const char * key);
void    lebao_set_home_key(const char* id, const char* key);

void 	lebao_ctrl_init(void);
void	lebao_ctrl_destroy(void);
int		lebao_ctrl_can_write_file(const int needSize);
void	lebao_ctrl_clear_mp3(void);
void	lebao_ctrl_set_start_with_player(const int isPlayer);
int		lebao_ctrl_is_start_with_player(void);

// file control
void	lebao_ctrl_mp3_deleted_callback(int(*callback)(const int cmd, const int status, void *data));

// 0 : file, 1 : buffer
void	lebao_ctrl_set_record_mode(const int mode);

void	lebao_ctrl_play_sound(const int index);
void	lebao_ctrl_set_can_play_tip_sound(const int canPlay);

// setting
void    lebao_set_space_min_size(const int bytes);
void    lebao_set_mp3_max_file_size(const int bytes);
void    lebao_mp3_min_file_size(const int bytes);
void    lebao_set_mp3_max_cache_size(const int bytes);
unsigned int lebao_get_default_free_space_size(void);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*MMI_LEBAO_CTRL_H*/

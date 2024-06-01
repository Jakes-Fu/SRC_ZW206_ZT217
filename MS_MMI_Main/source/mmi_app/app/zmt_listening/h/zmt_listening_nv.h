
#ifndef _HELLO_NV_H_
#define _HELLO_NV_H_

#include "sci_types.h"
#include "mmi_module.h"
/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
    extern   "C"
    {
#endif

/**--------------------------------------------------------------------------*
 **                         TYPE AND CONSTANT                                *
 **--------------------------------------------------------------------------*/
typedef struct
{
	int audio_id;
	char audio_name[100];
}LISTEING_LOCAL_AUDIO_INFO;

typedef struct
{
	int album_id;
	char album_name[100];
	int audio_count;
	LISTEING_LOCAL_AUDIO_INFO audio_info[100];
}LISTEING_LOCAL_ALBUM_INFO;

typedef struct
{
	int module_id;
	int album_count;
	LISTEING_LOCAL_ALBUM_INFO album_info[1];
}LISTEING_LOCAL_MODULE_INFO;

typedef struct
{
	uint8 module_count;
	LISTEING_LOCAL_MODULE_INFO module_info[10];
}LISTEING_LOCAL_INFO;

typedef enum
{
	PALYER_PLAY_STYLE_LOOP = 0,
	PALYER_PLAY_STYLE_RANDOM,
	PALYER_PLAY_STYLE_SINGLE,
}LISTENING_PALYER_PLAY_STYLE;

typedef struct
{
	int volume;
	LISTENING_PALYER_PLAY_STYLE style;
}LISTEING_PLAYER_INFO;


typedef enum
{
	MMINV_LISTENING_FLAG = MMI_MODULE_ZMT_LISTENING << 16,
    //MMINV_LISTENING_LOCAL,
    	MMINV_LISTENING_PLAYER_INFO,

}LISTENING_NV_ITEM_E;

PUBLIC void MMI_RegZmtListeningNv(void);

/**--------------------------------------------------------------------------*
 **                         MACRO DEFINITION                                 *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
    }
#endif

#endif
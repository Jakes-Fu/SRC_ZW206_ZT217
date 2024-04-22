
/*****************************************************************************/
#ifndef _DUERAPP_CENTER_H_
#define _DUERAPP_CENTER_H_

/*----------------------------------------------------------------------------*/
/*                          Include Files                                     */
/*----------------------------------------------------------------------------*/ 
#include "mmi_appmsg.h"
#include "sci_types.h"
#include "mmk_type.h"
#include "baidu_json.h"
#include "lightduer_connagent.h"
#include "duerapp_login.h"
/*----------------------------------------------------------------------------*/
/*                         Compiler Flag                                      */
/*----------------------------------------------------------------------------*/ 

#ifdef _cplusplus
	extern   "C"
    {
#endif
/*----------------------------------------------------------------------------*/

PUBLIC void MMIDUERAPP_CreateUserCenterWin(void);

PUBLIC void duerapp_userinfo_response(Pt_DUER_USER_INFO *pt_user_info);
PUBLIC void MMIDUERAPP_SkillListItem_Click(char *query_url);

PUBLIC void duerapp_feedback_callback(char *code);
PUBLIC void duerapp_audio_skill_list(DUEROS_SKILL_LIST_ITEM_ST *skill_list,int list_size);
PUBLIC void duerapp_audio_lesson_list(DUEROS_DISCOVERY_LIST_ITEM_ST *lesson_list,int lesson_size);
PUBLIC void duerapp_audio_discovery_list(DUEROS_DISCOVERY_LIST_ITEM_ST *discovery_list,int list_size);
PUBLIC void duerapp_audio_home_block_list(DUEROS_BLOCK_LIST_ITEM_ST *list,int list_size);
PUBLIC void duerapp_audio_skill_block_list(DUEROS_BLOCK_LIST_ITEM_ST *list,int list_size);
PUBLIC void duerapp_audio_center_config(DUEROS_CENTER_CFG_ITEM_ST *cfg);
PUBLIC int DiscoveryGetIndexFromTag(char *tag);
PUBLIC void DuerappAudioTypeSet(int audio_type);

PUBLIC void duerapp_res_list_enter(int index);

PUBLIC void duerapp_free_res_list(void);
PUBLIC void duerapp_free_user_info(void);

DUER_USER_INFO* duerapp_get_user_info(void);
PUBLIC void duerapp_login_out_callback();
#ifdef _cplusplus
	}
#endif

#endif//_DUERAPP_CENTER_H_
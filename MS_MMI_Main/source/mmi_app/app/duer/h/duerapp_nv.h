/*****************************************************************************

 ****************************************************************************/


#ifndef _DUERAPP_NV_H_
#define _DUERAPP_NV_H_

/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#include "sci_types.h"
#include "mmi_module.h"
#include "mmicom_data.h"

/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                    *
 **--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern   "C"
{
#endif

    typedef enum
    {
        MMIDUERAPP_USER_PROTOCOL = MMI_MODULE_DUER << MMI_COM_OFFSET,
        MMIDUERAPP_USER_TOKEN_EXPIRE,
        MMIDUERAPP_USER_ACCESSTOKEN,
        MMIDUERAPP_USER_REFRESHTOKEN,
        MMIDUERAPP_MAX_NV_ITEM_NUM
    } MMIDUERAPP_NV_ITEM_E;


    PUBLIC void MMIDUERAPP_RegNv(void);
    PUBLIC BOOLEAN MMIDUERAPP_GetIsUserAgree(void);
    PUBLIC void MMIDUERAPP_SetUserAgree(BOOLEAN is_active);
    PUBLIC char* MMIDUERAPP_GetAccessToken(void);
    PUBLIC void MMIDUERAPP_SetGlobalAccessTokenOnly(char *token);
    PUBLIC void MMIDUERAPP_SetAccessToken(char *token);
    PUBLIC char* MMIDUERAPP_GetDeviceUuid();
    PUBLIC void MMIDUERAPP_SyncAccessToken();
    PUBLIC char* MMIDUERAPP_GetCompositionGrade();
    PUBLIC void MMIDUERAPP_SetCompositionGrade(char *grade);
    PUBLIC BOOLEAN MMIDUERAPP_GetIsSandOnce(void);
    PUBLIC void MMIDUERAPP_SetSandOnce(BOOLEAN is_sand_once);
    PUBLIC int MMIDUERAPP_GetATExpireTime(void);
    PUBLIC void MMIDUERAPP_SetATExpireTime(int time);
    PUBLIC char* MMIDUERAPP_GetRefreshToken(void);
    PUBLIC void MMIDUERAPP_SetRefreshToken(char *token);
    PUBLIC char* MMIDUERAPP_GetWatchConfig();
    PUBLIC int MMIDUERAPP_SetWatchConfig(char *config);
    PUBLIC char* MMIDUERAPP_GetUserResConfig();
    PUBLIC int MMIDUERAPP_SetUserResConfig(const char *config);
    PUBLIC int MMIDUERAPP_SaveSensitiveWordsConfig(const char *body);
    PUBLIC void MMIDUERAPP_FactoryReset(void);
    /**--------------------------------------------------------------------------*
     **                         Compiler Flag                                    *
     **--------------------------------------------------------------------------*/
#ifdef   __cplusplus
}
#endif

#endif //_MMIAPPSAMPLE_NV_H_


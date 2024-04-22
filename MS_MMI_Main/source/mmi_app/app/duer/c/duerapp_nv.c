#define _BAIDU_NV_C_


/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/
#ifdef WIN32
#include "std_header.h"
#endif
#include "duerapp_nv.h"
#include "mmi_modu_main.h"
#include "mmi_nv.h"
#include "duerapp_login.h"
#include "duerapp_sfs_cfg.h"
#include "lightduer_log.h"
#include "lightduer_profile.h"
// #include "duerinput_sensitive_words.h"

#define ACCESS_TOKEN_MAX_LEN 256
#define DEVICE_UUID_MAX_LEN  70
#define COMPOSITION_GRADE_NAME 50

char s_access_token[ACCESS_TOKEN_MAX_LEN] = {0};
char gs_refresh_token[ACCESS_TOKEN_MAX_LEN] = {0};
char s_device_uuid[DEVICE_UUID_MAX_LEN] = {0};
char s_composition_grade[COMPOSITION_GRADE_NAME] = {0};
/*the length of set nv*/
const uint16 duerapp_nv_len[] =
{
    sizeof( BOOLEAN ),
    sizeof( int ),
    sizeof(char)*ACCESS_TOKEN_MAX_LEN,
    sizeof(char)*ACCESS_TOKEN_MAX_LEN,
};

/*****************************************************************************/
// 	Description : register set module nv len and max item
//	Global resource dependence : none
//  Author: liuwenshuai
//	Note:
/*****************************************************************************/
PUBLIC void MMIDUERAPP_RegNv(void)
{
    MMI_RegModuleNv(MMI_MODULE_DUER, duerapp_nv_len, sizeof(duerapp_nv_len) / sizeof(uint16));
}

/*****************************************************************************/
//  Description : is user agree protocol
//  Global resource dependence : 
//  Author: liuwenshuai
/*****************************************************************************/
PUBLIC BOOLEAN MMIDUERAPP_GetIsUserAgree(void)
{
    BOOLEAN is_active = FALSE;
    // int val = 0;
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
    DUER_LOGI("MMIDUERAPP_USER_PROTOCOL: %d\r\n", MMIDUERAPP_USER_PROTOCOL);
    MMINV_READ(MMIDUERAPP_USER_PROTOCOL, &is_active, return_value);
    DUER_LOGI("read is_active: %d, return_value: %d\r\n", is_active, return_value);
    if (MN_RETURN_SUCCESS != return_value) {
        is_active = FALSE;
        MMIDUERAPP_SetUserAgree(FALSE);
    }
#if 0
    if (duer_get_config_int("user_agree", &val) == 0) {
        if (val) {
            is_active = TRUE;
        } else {
            is_active = FALSE;
        }
    }
#endif
    DUER_LOGI("get user_agree=%d.", is_active);
    return is_active;
}

/*****************************************************************************/
//  Description : Set user agree protocol
//  Global resource dependence : 
//  Author: liuwenshuai
//  Note: 
/*****************************************************************************/
PUBLIC void MMIDUERAPP_SetUserAgree(BOOLEAN is_active)
{
    int ret = -1;
    MMINV_WRITE(MMIDUERAPP_USER_PROTOCOL, &is_active);
#if 0
    if (is_active) {
        ret = duer_set_config_int("user_agree", 1);
    } else {
        ret = duer_set_config_int("user_agree", 0);
    }
#endif
    DUER_LOGI("set user_agree=%d, ret=%d.", is_active, ret);
}
/*****************************************************************************/
PUBLIC char* MMIDUERAPP_GetAccessToken(void)
{
    int len = ACCESS_TOKEN_MAX_LEN - 1;
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
    if (s_access_token && strlen(s_access_token) > 1) {
        return s_access_token;
    }
    MMINV_READ(MMIDUERAPP_USER_ACCESSTOKEN, s_access_token, return_value);
    if (return_value != MN_RETURN_SUCCESS) {
        DUER_LOGW("failed to read access_token from nv");
    }
#if 0
    if (duer_get_config_str("ac_token", s_access_token, &len) == 0) {
        DUER_LOGI("get ac_token=%s.", s_access_token);
    } else {
        DUER_LOGI("get ac_token failed!");
    }
#endif
    return s_access_token;
}

PUBLIC void MMIDUERAPP_SetGlobalAccessTokenOnly(char *token)
{
    strcpy(s_access_token, token);
}

PUBLIC void MMIDUERAPP_SetAccessToken(char *token)
{
    // strcpy(s_access_token,token);
    MMIDUERAPP_SetGlobalAccessTokenOnly(token);
    DUER_LOGI("set access token->%s", s_access_token);
    MMINV_WRITE(MMIDUERAPP_USER_ACCESSTOKEN, s_access_token);
#if 0
    if (duer_set_config_str("ac_token", token) == 0) {
        DUER_LOGI("set ac_token success.");
    } else {
        DUER_LOGI("set ac_token failed!");
    }
#endif
    if (strlen(s_access_token) > 1) {
        duer_profile_set_string_param(PROFILE_KEY_ACCESS_TOKEN, s_access_token);
    }
}

PUBLIC void MMIDUERAPP_SyncAccessToken() {
    char *access_token = MMIDUERAPP_GetAccessToken();
    DUER_LOGI("get access token->%s",access_token);
    if (access_token && strlen(access_token) > 1) {
        duer_profile_set_string_param(PROFILE_KEY_ACCESS_TOKEN, access_token);
        duer_s_watch_userinfo_inprogress_reset();
    } else {
        // 加个容错，如果发现accesstoken为空，同步服务端登录信息
        // duerapp_sync_bind_user_info();
        DUER_LOGI("MMIDUERAPP_SyncAccessToken");
        // duerapp_user_login_out();
        duerapp_user_force_login_out();
    }
}

PUBLIC char* MMIDUERAPP_GetDeviceUuid() {
    const char *cuid = duer_profile_get_string_param(PROFILE_KEY_DEVICE_ID);
    strcpy(s_device_uuid,cuid/*"e1c79e2d63332a1dd29efc216cb434ace1c79e2d63332a1dd29efc216cb434ac"*/);
    return s_device_uuid;
}

PUBLIC void MMIDUERAPP_SetCompositionGrade(char *grade)
{
    strcpy(s_composition_grade, grade);
    DUER_LOGI("set s_composition_grade->%s",s_composition_grade);
    if (duer_set_config_str("composition_grade", grade) == 0) {
        DUER_LOGI("set s_composition_grade success.");
    } else {
        DUER_LOGI("set s_composition_grade failed!");
    }
}

PUBLIC char* MMIDUERAPP_GetCompositionGrade()
{
    int len = COMPOSITION_GRADE_NAME - 1;
    if (s_composition_grade && strlen(s_composition_grade) > 1) {
        return s_composition_grade;
    }
    if (duer_get_config_str("composition_grade", s_composition_grade, &len) == 0) {
        DUER_LOGI("get composition_grade=%s.", s_composition_grade);
    } else {
        DUER_LOGI("get composition_grade failed!");
    }
    return s_composition_grade;
}

PUBLIC char* MMIDUERAPP_GetWatchConfig()
{
    char *cfg_ptr = duer_get_config_with_ptr("watch_config");
    if (cfg_ptr != NULL)
    {
        DUER_LOGI("get watchconfig success!");
    } else {
        DUER_LOGI("get watchconfig failed!");
    }
    return cfg_ptr;
}

PUBLIC int MMIDUERAPP_SetWatchConfig(char *config)
{
    if (duer_set_config_str("watch_config", config) == 0) {
        DUER_LOGI("set watchconfig success!");
        return 1;
    } else {
        DUER_LOGI("set watchconfig failed!");
        return 0;
    }
}
/*****************************************************************************/

PUBLIC BOOLEAN MMIDUERAPP_GetIsSandOnce(void)
{
    #define IS_SAND_MAGIC_NUM 0x5C
    BOOLEAN is_sand_once = FALSE;

    int val = 0;
    if (duer_get_config_int("sand_once", &val) == 0) {
        if (val == IS_SAND_MAGIC_NUM) {
            is_sand_once = TRUE;
        } else {
            is_sand_once = FALSE;
        }
    }
    DUER_LOGI("get is_sand_once=%d.", val);
    return is_sand_once;
}

PUBLIC void MMIDUERAPP_SetSandOnce(BOOLEAN is_sand_once)
{
    int ret = -1;
    if (is_sand_once) {
        ret = duer_set_config_int("sand_once", IS_SAND_MAGIC_NUM);
    } else {
        ret = duer_set_config_int("sand_once", 0);
    }
    DUER_LOGI("set sand_once=%d, ret=%d.", is_sand_once, ret);
}
/*****************************************************************************/
PUBLIC int MMIDUERAPP_GetATExpireTime(void)
{
    int val = 0;
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
    MMINV_READ(MMIDUERAPP_USER_TOKEN_EXPIRE, &val, return_value);
    if (return_value != MN_RETURN_SUCCESS) {
        DUER_LOGW("failed to read expire time from nv");
    } else {
        DUER_LOGI("get access token expire time=%d.", val);
    }
#if 0
    if (duer_get_config_int("at_expire", &val) == 0) {
        DUER_LOGI("get access token expire time success");
        return val;
    }
#endif
    return val;
}

PUBLIC void MMIDUERAPP_SetATExpireTime(int time)
{
#if 0
    duer_set_config_int("at_expire", time);
#endif
    DUER_LOGI("set access token expire time=%d.", time);
    MMINV_WRITE(MMIDUERAPP_USER_TOKEN_EXPIRE, &time);
}

/*****************************************************************************/
PUBLIC char* MMIDUERAPP_GetRefreshToken(void)
{
    int len = ACCESS_TOKEN_MAX_LEN - 1;
    MN_RETURN_RESULT_E return_value = MN_RETURN_FAILURE;
    if (gs_refresh_token && strlen(gs_refresh_token) > 1) {
        return gs_refresh_token;
    }
    MMINV_READ(MMIDUERAPP_USER_REFRESHTOKEN, gs_refresh_token, return_value);
    if (return_value != MN_RETURN_SUCCESS) {
        DUER_LOGW("failed to read refresh token from nv");
    }
#if 0
    if (duer_get_config_str("rfsh_token", gs_refresh_token, &len) == 0) {
        DUER_LOGI("get rfsh_token=%s.", gs_refresh_token);
    } else {
        DUER_LOGI("get rfsh_token failed!");
    }
#endif
    return gs_refresh_token;
}

PUBLIC void MMIDUERAPP_SetRefreshToken(char *token)
{
    strcpy(gs_refresh_token, token);
    DUER_LOGI("set rfsh_token->%s", gs_refresh_token);
    MMINV_WRITE(MMIDUERAPP_USER_REFRESHTOKEN, gs_refresh_token);
#if 0
    if (duer_set_config_str("rfsh_token", token) == 0) {
        DUER_LOGI("set rfsh_token success.");
    } else {
        DUER_LOGI("set rfsh_token failed!");
    }
#endif
}
/*****************************************************************************/

PUBLIC char* MMIDUERAPP_GetUserResConfig()
{
    char *cfg_ptr = duer_get_config_with_ptr("watch_res_map_config");
    if (cfg_ptr != NULL)
    {
        DUER_LOGI("get watchresconfig success!");
    } else {
        DUER_LOGI("get watchresconfig failed!");
    }
    return cfg_ptr;
}

PUBLIC int MMIDUERAPP_SetUserResConfig(const char *config)
{
    if (duer_set_config_str("watch_res_map_config", config) == 0) {
        DUER_LOGI("set watchresconfig success!");
        return 1;
    } else {
        DUER_LOGI("set watchresconfig failed!");
        return 0;
    }
}

/*****************************************************************************/
PUBLIC int MMIDUERAPP_SaveSensitiveWordsConfig(const char *body)
{
    int ret = 0;
    BOOLEAN result = TRUE;
    // duer_write_file(INPUT_SENSITIVE_WORDS_PATH, body, strlen(body));
    // duerinput
    // SFS_DeleteFile(INPUT_SENSITIVE_WORDS_PATH, NULL);

    if(result != TRUE)
    {
        ret = -4;
        DUER_LOGI("%s %d ret %d ", __FUNCTION__, __LINE__, ret);
    }
    return ret;
}
/*****************************************************************************/
PUBLIC void MMIDUERAPP_FactoryReset(void)
{
    // const char t_access_token[ACCESS_TOKEN_MAX_LEN] = {0};
    // Reset user agree
    MMIDUERAPP_SetUserAgree(FALSE);
    // reset access token
    // MMIDUERAPP_SetAccessToken((char *)t_access_token);
    MMIDUERAPP_SetAccessToken("");
    MMIDUERAPP_SetATExpireTime(0);
    MMIDUERAPP_SetRefreshToken("");

}

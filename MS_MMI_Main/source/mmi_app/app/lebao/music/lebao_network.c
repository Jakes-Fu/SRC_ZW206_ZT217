#include "port_cfg.h"
#include "lebao_network.h"

#include "std_header.h"
#include "mmk_app.h"
#include "mmi_common.h"
#include "mmk_timer.h"
#include "mmi_module.h"
#include "mmipdp_export.h"
#include "mmiconnection_export.h"
#include "mmiset_export.h"

#include "lebao_text.h"
#include "lebao_base.h"

#ifdef WIFI_SUPPORT
  #if !defined(PLATFORM_UWS6121E)
  #define LEBAO_WIFI_SUPPORT_CRACK   1
  #else
  #define LEBAO_WIFI_SUPPORT_CRACK   0
  #endif
#else
  #define LEBAO_WIFI_SUPPORT_CRACK   0 
#endif  

#if (LEBAO_WIFI_SUPPORT_CRACK != 0)
#include "mmiwifi_export.h"
#endif

// public variables

// private variables
LOCAL int _lebaoNetId = 0;
LOCAL int _isLebaoActiveNetwork = 0;

// private prototypes
LOCAL BOOLEAN lebao_pdp_active(void);
LOCAL void lebao_handle_pdp_msg(MMIPDP_CNF_INFO_T *msg_ptr);

//-------------------------------network-------------------------------

LOCAL void lebao_show_no_network(void)
{
	lebao_create_msgbox_id(TEXT_LEBAO_NO_SIM_CARD, 2000);
}

int lebao_network_connect(void)
{
	_isLebaoActiveNetwork = 0;

#if (LEBAO_WIFI_SUPPORT_CRACK != 0)
	// check the wifi status
	if (lebao_active_network() == TRUE) {
		return 0;
	}
	else
#endif
	{
		uint16 sim_sys = MN_DUAL_SYS_1;
		uint32 sim_num = 0;

		#if defined(LEBAO_PLATFORM_T117) && !defined(_WIN32)
			if (MMISET_IsSSProcessing() || (FALSE == MMIAPISET_GetIsQueryCfu())) {
				// || MMIUSBSHARE_IsRunning() || MMIMMS_IsMMSChangedDataCard()) {
				MMIPUB_OpenAlertWarningWin(TXT_SYS_WAITING_AND_TRY_LATER);
				return -1;
			}
		#endif
		
		sim_num = MMIAPIPHONE_GetSimAvailableNum(&sim_sys, 1);
		if (sim_num <= 0) {
			lebao_show_no_network();
			return -1;
		}

		#ifdef LEBAO_FEATURE_PHONE_STYLE
			if (MMIAPISET_GetFlyMode()) {
				//MMIPUB_OpenAlertWarningWin(TEXT_LEBAO_CAN_NOT_FLY_MODE);
				lebao_create_msgbox_id(TEXT_LEBAO_NO_SIM_CARD, 2000);
				return -1;
			}

			if (lebao_gprs_is_opened() == FALSE) {
				//MMIPUB_OpenAlertWarningWin(TEXT_LEBAO_OPEN_GPRS);
				lebao_create_msgbox_id(TEXT_LEBAO_NO_SIM_CARD, 2000);
				return -1;
			}

			if (lebao_active_network() == FALSE) {
				lebao_show_no_network();
				return -1;
			}
		#else
			if (MMIAPIPDP_PsIsActivedPdpLinkExist() == FALSE) {
				lebao_show_no_network();
				return -1;
			}
		#endif
	}

	return 0;
}


#if defined(MULTI_SIM_SYS_SINGLE)
MN_DUAL_SYS_E lebao_get_active_sim(void)
{
	return MMIAPISET_GetActiveSim();
}
#else
MN_DUAL_SYS_E lebao_get_active_sim(void)
{
	MN_DUAL_SYS_E dualSysUse = MN_DUAL_SYS_1;

	if (!(MMIAPIPHONE_GetSimExistedStatus(MN_DUAL_SYS_1)) && !(MMIAPIPHONE_GetSimExistedStatus(MN_DUAL_SYS_2)))
		return MMIAPISET_GetActiveSim();

	if (MMIAPIPHONE_GetSimExistedStatus(dualSysUse) && MMIAPIPHONE_GetDataServiceSIM(&dualSysUse))
		return dualSysUse;

	dualSysUse = MN_DUAL_SYS_2;
	if (MMIAPIPHONE_GetSimExistedStatus(dualSysUse) && MMIAPIPHONE_GetDataServiceSIM(&dualSysUse))
		return dualSysUse;

	return MMIAPISET_GetActiveSim();
}
#endif

int lebao_get_net_id(void)
{
	return _lebaoNetId;
}

BOOLEAN lebao_active_network(void)
{
	BOOLEAN result = FALSE;

	_lebaoNetId = 0;
	
#if (LEBAO_WIFI_SUPPORT_CRACK != 0)
	// priority  wifi -> ps
	if (MMIWIFI_STATUS_CONNECTED == MMIAPIWIFI_GetStatus())
		return TRUE;

	do {
#if defined(MULTI_SIM_SYS_SINGLE)
		if (MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_1))
			break;
#else
		if (MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_1) || MMIAPIPHONE_IsSimOk(MN_DUAL_SYS_2))
			break;
#endif
		else {
      		lebao_network_enter_setting();
		}
	} while (0);
#endif

	if (MMIAPIPDP_PsIsPdpLinkExist())
		return TRUE;

	if(result == FALSE)
		result = lebao_pdp_active();

	return result;
}

BOOLEAN lebao_deactive_network(void)
{
	_lebaoNetId = 0;
	if (_isLebaoActiveNetwork != 0) {
		_isLebaoActiveNetwork = 0;
		return MMIAPIPDP_Deactive(MMI_MODULE_LEBAO);
	}
	return FALSE;
}

BOOLEAN lebao_network_is_connected(void)
{
#ifndef _WIN32
  #if (LEBAO_WIFI_SUPPORT_CRACK != 0)
  	if (MMIWIFI_STATUS_CONNECTED == MMIAPIWIFI_GetStatus())
  		return TRUE;
	#endif

	return MMIAPIPDP_PsIsActivedPdpLinkExist();
#else
	return TRUE;
#endif
}

void lebao_network_enter_setting(void)
{
#if (LEBAO_WIFI_SUPPORT_CRACK != 0)
	if (MMIWIFI_STATUS_CONNECTED == MMIAPIWIFI_GetStatus())
		return; 
	else {
		#if !defined(PLATFORM_ANTISW3) && !defined(PLATFORM_UWS6121E)
			MMIPDP_ACTIVE_INFO_T active_info = { 0 };
			active_info.app_handler = MMI_MODULE_LEBAO;
			active_info.handle_msg_callback = lebao_handle_pdp_msg;
			active_info.ps_interface = MMIPDP_INTERFACE_WIFI;
			MMIAPIPDP_Active(&active_info);
		#else
			WatchWIFI_MainWin_Enter();
		#endif
	}
#endif
}

void lebao_enter_setting_after_start_failed(int timeoutSeconds)
{
  if (timeoutSeconds >= 5) {
    lebao_network_enter_setting();
  }
}

LOCAL void lebao_handle_pdp_msg(MMIPDP_CNF_INFO_T *msg_ptr)
{
	if (PNULL == msg_ptr) {
		return;
	}

	helper_debug_printf("lebao_handle_pdp_msg: nsapi = %d, msg_id = %d, app = %d", 
		msg_ptr->nsapi,
		msg_ptr->msg_id,
		msg_ptr->app_handler);

	switch (msg_ptr->msg_id)
	{
	case MMIPDP_ACTIVE_CNF:
		if (MMIPDP_RESULT_SUCC == msg_ptr->result) {
			_lebaoNetId = msg_ptr->nsapi;
		}
		else {
			lebao_deactive_network();
#if (LEBAO_WIFI_SUPPORT_CRACK != 0)
			if (MMIPDP_INTERFACE_WIFI == msg_ptr->ps_interface) {
				// Todo : change to GPRS
			}
#endif
		}
		break;

	case MMIPDP_DEACTIVE_CNF:
		break;

	case MMIPDP_DEACTIVE_IND: {
		lebao_deactive_network();
		break;
	}
	
	default:
		break;
	}

	MMI_CheckAllocatedMemInfo();
}

LOCAL BOOLEAN lebao_pdp_active(void)
{
	MN_DUAL_SYS_E dual_sys = lebao_get_active_sim();
	MMIPDP_ACTIVE_INFO_T    active_info = { 0 };
	BOOLEAN                 result = FALSE;
	MMICONNECTION_LINKSETTING_DETAIL_T* linksetting = PNULL;

	linksetting = MMIAPICONNECTION_GetLinkSettingItemByIndex(dual_sys, 0);
	if(linksetting == NULL)
		return FALSE;

	active_info.app_handler = MMI_MODULE_LEBAO;
	active_info.dual_sys = dual_sys;
	active_info.apn_ptr =  (char*)linksetting->apn;
	active_info.user_name_ptr = (char*)linksetting->username;
	active_info.psw_ptr = (char*)linksetting->password;
	active_info.priority = 3;
	active_info.ps_service_rat = MN_UNSPECIFIED;
	active_info.ps_interface = MMIPDP_INTERFACE_GPRS;
	active_info.handle_msg_callback = lebao_handle_pdp_msg;
	active_info.ps_service_type = BROWSER_E;
	active_info.storage = MN_GPRS_STORAGE_ALL;
#ifdef IPVERSION_SUPPORT_V4_V6
	active_info.ip_type = MMICONNECTION_IP_V4;//linksetting->ip_type;
#endif
	if (MMIAPIPDP_Active(&active_info)) {
		_isLebaoActiveNetwork = 1;
		result = TRUE;
	}

	helper_debug_printf("lebao_pdp_active: result = %d", result);
	return result;
}

BOOLEAN lebao_gprs_is_opened(void)
{
#ifdef MMI_GPRS_SUPPORT
	if (MMIAPICONNECTION_GetGPRSSwitchStatus() == MMICONNECTION_SETTING_GPRS_SWITCH_OFF)
		return FALSE;
#endif
	return TRUE;
}


#include "std_header.h"
#include "mn_type.h"
#include "mmi_nv.h"
#include "sci_types.h"
#include <stdio.h>
#include "nvitem.h"
#ifdef _RTOS
#include "sci_api.h" /*@tony.yao  replase sci_mem.h with sci_api.h*/
#else
#include "tasks_id.h"
#endif
#include "mn_api.h"
#include "mn_events.h"
#include "mn_type.h"
#include "nv_item_id.h"
#include "sio.h"
#include "sig_code.h"
#include "os_api.h"
#include "IN_message.h"
#include "socket_types.h"

#include "mmi_signal_ext.h"
#include "mmk_app.h"
#include "mmk_timer.h"
#include "mmi_text.h"
#include "mmi_common.h"
#include "window_parse.h"
#include "guitext.h"
#include "guilcd.h"
#include "mmi_menutable.h"
#include "guilistbox.h"
#include "mmi_image.h"
#include "guiedit.h"
#include "mmipub.h"
#include "mmi_appmsg.h"
#include "mmiset_export.h"
#include "mmienvset_export.h"
#include "mmisms_export.h"
#include "mmipdp_export.h"
#include "mmiconnection_export.h"
#include "mmiphone_export.h"
#include "mmifmm_export.h"


PUBLIC void MMIREADBOY_CreatePoetryWin(void);
PUBLIC void MMIREADBOY_CreateDepositTipWin(void);
PUBLIC void readboy_set_pay_type(void);
PUBLIC void MMIREADBOY_SettingCreateDeposit(void);
PUBLIC uint8  readboy_get_pay_type(void);
/*
PUBLIC DSL_WHITE_LIST_T * MMIREADBOY_ReadWhiteListNvm(void);
PUBLIC void MMIREADBOY_WriteWhiteListNvm(DSL_WHITE_LIST_T * nvm);
PUBLIC uint8 MMIREADBOY_GetWhiteListNum(void);
PUBLIC char * MMIREADBOY_GetWhiteListNumberByIndex(uint8 index);
PUBLIC char * MMIREADBOY_GetWhiteListNameByNumber(char * number);
*/
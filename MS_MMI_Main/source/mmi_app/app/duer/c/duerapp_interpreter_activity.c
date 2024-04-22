// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_interpreter_activity.c
 * Auth: Wanglusong (wanglusong01@baidu.com)
 * Desc: duerapp xiaodu interpreter
 */

#include "std_header.h"
#include "window_parse.h"
#include "mmk_app.h"
#include "ctrlsetlist_export.h"
#include "dal_time.h"
#include "guilcd.h"
#include "guifont.h"
#include "guilistbox.h"
#include "guibutton.h"
#include "guitext.h"
#include "guilabel.h"
#include "guirichtext.h"
#include "guisetlist.h"
#include "gui_ucs2b_converter.h"
#include "mmipub.h"
#include "mmi_textfun.h"
#include "mmi_image.h"
#include "mmi_appmsg.h"
#include "mmi_common.h"
#include "mmi_event_api.h"
#include "mmi_applet_table.h"
#include "mmidisplay_data.h"
// #include "mmiwifi_export.h"
#include "mmiphone_export.h"
#include "mmicom_time.h"
#include "watch_common_btn.h"
#include "watch_common_list.h"
#include "watch_commonwin_export.h"

#include "lightduer_log.h"
#include "lightduer_connagent.h"
#include "lightduer_memory.h"
#include "lightduer_types.h"
#include "lightduer_ap_info.h"

#include "duerapp.h"
#include "duerapp_id.h"
#include "duerapp_text.h"
#include "duerapp_image.h"
#include "duerapp_anim.h"
#include "duerapp_nv.h"
#include "duerapp_main.h"
#include "duerapp_openapi.h"
#include "duerapp_common.h"
#include "duerapp_http.h"
#include "duerapp_statistics.h"
#include "duerapp_payload.h"
#include "duerapp_recorder.h"
#include "duerapp_res_down.h"
#include "duerapp_homepage.h"
#include "duerapp_main.h"
#include "duerapp_interpreter_activity.h"

/**************************************************************************************************************************
 *                                                        STATIC VARIABLES                                                
 **************************************************************************************************************************/

/**************************************************************************************************************************
 *                                                        STATIC FUNCTIONS                                                
 *************************************************************************************************************************/

PUBLIC void PUB_DUERAPP_InterpreterMainWinOpen(void)
{
    MMIDUERAPP_StartInterpreterAsrSet();
    MMIDUERAPP_CreateDuerMainWin(FALSE);
}


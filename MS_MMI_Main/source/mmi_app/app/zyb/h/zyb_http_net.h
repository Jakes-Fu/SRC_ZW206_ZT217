
#ifndef  _MMI_HTTP_NET_H_    
#define  _MMI_HTTP_NET_H_   

#ifdef __cplusplus
    extern "C"
    {
#endif
/**--------------------------------------------------------------------------*
 **                         Include Files                                    *
 **--------------------------------------------------------------------------*/

#include "zyb_common.h"


PUBLIC uint32 ZYBHTTP_Net_GetNetID(void);
PUBLIC BOOLEAN ZYBHTTP_Net_PdpStateGet(void);
PUBLIC BOOLEAN ZYBHTTP_Net_IsActiving(void);
PUBLIC BOOLEAN ZYBHTTP_Net_IsClosing(void);
PUBLIC BOOLEAN ZYBHTTP_Net_IsInit(void);
PUBLIC BOOLEAN ZYBHTTP_Net_Init(void);
PUBLIC BOOLEAN ZYBHTTP_Net_Reset(void);
PUBLIC BOOLEAN ZYBHTTP_Net_Open(void);
PUBLIC BOOLEAN ZYBHTTP_Net_Close(void);

PUBLIC MN_DUAL_SYS_E ZYBHTTP_Net_GetActiveSys(void);

PUBLIC MMI_RESULT_E ZYBHTTP_Net_Handle_AppMsg(PWND app_ptr, uint16 msg_id, DPARAM param);
PUBLIC BOOLEAN MMIZYB_HTTP_Handle_RefreshInd(DPARAM param);

PUBLIC BOOLEAN ZYBHTTP_Net_StatusInd(APP_MN_NETWORK_STATUS_IND_T *status_ptr);
PUBLIC BOOLEAN ZYBHTTP_Net_GPRS_IsOK(MN_DUAL_SYS_E dual_sys);

PUBLIC BOOLEAN  ZYBHTTP_Net_NV_SetActiveSys(MN_DUAL_SYS_E set_sys) ;
PUBLIC MN_DUAL_SYS_E ZYBHTTP_Net_GetActiveSys(void);


#ifdef __cplusplus
    }
#endif

#endif

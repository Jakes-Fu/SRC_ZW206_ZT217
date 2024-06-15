#ifndef _ZDT_GSENSOR_H
#define _ZDT_GSENSOR_H

#ifdef   __cplusplus
extern   "C"
{
#endif

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "sci_types.h"
#include "os_api.h"
#include "sc_reg.h"
#include "isr_drvapi.h"
#include "rtc_drvapi.h"
#ifndef WIN32
#include "sio.h"
#endif

#include "timer_drvapi.h"
#include "adc_drvapi.h"
#include "chip.h"
#include "sig_code.h"

#define ZDT_GSENSOR_TRACE                 SCI_TRACE_LOW

typedef struct
{
    SIGNAL_VARS
    uint8 *        str;
    uint32         len; 
} ZDT_GSENSOR_SIG_T;

typedef enum {
  GSENSOR_TYPE_NULL=0x00,
  GSENSOR_TYPE_DA213,
  GSENSOR_TYPE_QMA,
  GSENSOR_TYPE_QMA6981,
  GSENSOR_TYPE_QMA7981,
  GSENSOR_TYPE_QMA6100,
  GSENSOR_TYPE_MAX
}ZDT_GSENSOR_IC_TYPE_E;

#ifdef ZDT_GSENSOR_SUPPORT
extern BOOLEAN  APP_SendSigTo_GSensor(ZDT_GSENSOR_TASK_SIG_E sig_id, const char * data_ptr,uint32 data_len);
#endif
extern int ZDT_GSensor_Init(void);
extern int ZDT_GSensor_Step(uint32 once_step);
extern int ZDT_GSensor_Open(void);
extern int ZDT_GSensor_Close(void);
extern int ZDT_GSensor_Reset(void);
extern int ZDT_GSensor_TimerOut(void);
extern int ZDT_GSensor_Move(void);
extern int ZDT_GSensor_Still(void);
extern int ZDT_GSensor_Fail(void);
extern uint32 ZDT_GSensor_GetStep(void);
extern BOOLEAN ZDT_GSensor_GetStill(void);
extern BOOLEAN ZDT_GSensor_HW_IsOK(void);
extern ZDT_GSENSOR_IC_TYPE_E ZDT_GSensor_GetType(void);
extern uint32 ZDT_GSensor_GetStepOneDay(void);
extern void ZDT_GSensor_SetStepOneDay(uint32 val);
extern void ZDT_GSensor_RestoreStepOneDay(uint32 val);

extern BOOLEAN  MIR3DA_init(void);
extern BOOLEAN  MIR3DA_open(void);
extern BOOLEAN  MIR3DA_close(void);
extern BOOLEAN MIR3DA_Reset(void);
extern BOOLEAN MIR3DA_GetEvent(void);

extern BOOLEAN  QMAX981_init(void);
extern BOOLEAN  QMAX981_open(void);
extern BOOLEAN  QMAX981_close(void);
extern BOOLEAN  QMAX981_step_reset(void);

#ifdef   __cplusplus
}
#endif

#endif/*_ZDT_GSENSOR_H*/

#ifndef FDL_COMMAND_H
#define FDL_COMMAND_H

#include "dl_engine.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

/******************************************************************************
 * sys_connect
 ******************************************************************************/
int FDL1_SysConnect (PACKET_T *packet, void *arg);

/******************************************************************************
 * data_start
 ******************************************************************************/
int FDL1_DataStart (PACKET_T *packet, void *arg);

/******************************************************************************
 * data_midst
 ******************************************************************************/
int FDL1_DataMidst (PACKET_T *packet, void *arg);

/******************************************************************************
 * data_end
 ******************************************************************************/
int FDL1_DataEnd (PACKET_T *packet, void *arg);

/******************************************************************************
 * data_exec
 ******************************************************************************/
int FDL1_DataExec (PACKET_T *packet, void *arg);

/******************************************************************************
 * set_baudrate
 ******************************************************************************/
int FDL1_SetBaudrate (PACKET_T *packet, void *arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FDL_COMMAND_H */
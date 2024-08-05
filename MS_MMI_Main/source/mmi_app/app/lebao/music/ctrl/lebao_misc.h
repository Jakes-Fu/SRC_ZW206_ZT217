#ifndef __LEBAO_MISC_H
#define __LEBAO_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "port_cfg.h"
#include "lebao_api.h"
#include "lebao_api2.h"
#include "lebao_event.h"
#include "lebao_srv.h"
#include "lebao_config.h"
#include "helper_mp3.h"
#include "helper_file.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void 	lebao_port_init(void);
void 	lebao_port_destroy(void);

// lebao_config.c
extern 	lebao_config_t* lebao_get_config(void);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*__LEBAO_MISC_H*/

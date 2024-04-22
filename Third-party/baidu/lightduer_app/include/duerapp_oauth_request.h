// Copyright (2022) Baidu Inc. All rights reserveed.
/**
 * File: duerapp_oauth_request.h
 * Auth: shichenyu (shichenyu01@baidu.com)
 * Desc: duerapp network request.
 */
/**--------------------------------------------------------------------------*/
#ifndef _DUERAPP_OAUTH_REQUEST_H_
#define _DUERAPP_OAUTH_REQUEST_H_

/**--------------------------------------------------------------------------*
 **                         INCLUDE                                         *
 **--------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------*
 **                         DEFINE                                         *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         TYPEDEFS                                        *
 **--------------------------------------------------------------------------*/

/**--------------------------------------------------------------------------*
 **                         FUNCTION                                        *
 **--------------------------------------------------------------------------*/
PUBLIC void duerapp_get_oauth_refresh(void);
PUBLIC BOOLEAN duerapp_is_need_refresh_token(void);

/**--------------------------------------------------------------------------*/
#endif /* _DUERAPP_OAUTH_REQUEST_H_ */

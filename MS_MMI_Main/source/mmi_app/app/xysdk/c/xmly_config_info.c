#include "xmly_config_info.h"

#define XY_APP_KEY ""
#define XY_APP_SECRET ""
#define XY_SN ""

#define XY_HOMEPAGE_CH_ID  "e4829b16ebd14608bcbc325741ab89b3"


PUBLIC void mmixysdk_api_get_appkey(char *appkey)
{
	if(appkey)
	{
		strcpy(appkey,XY_APP_KEY);
	}
}


PUBLIC void mmixysdk_api_get_appsecret(char *appsecret)
{
	if(appsecret)
	{
		strcpy(appsecret,XY_APP_SECRET);
	}
}

PUBLIC void mmixysdk_api_get_sn(char *sn)
{
	if(sn)
	{
		strcpy(sn,XY_SN);
	}
}

PUBLIC void mmixysdk_api_get_ch_id(char *chid)
{
	if(chid)
	{
		strcpy(chid,XY_HOMEPAGE_CH_ID);
	}
}

/*watch端返回vol值给xmly端
 *
 *
 */
PUBLIC int  mmixysdk_api_get_watch_vol()
{
	return 0;
}

/*xmly端传vol值给watch端
 *
 *
 */
PUBLIC void  mmixysdk_api_set_watch_vol(int vol)
{
	
}

uint32 xmly_iot_socket_get_netid(){

	return MMIAPIPDP_GetPublicPdpLinkNetid();

}

PUBLIC BOOL mmixysdk_api_get_save_record_file_flags()
{
	return FALSE; /*TRUE 保存在D:\\xmly\\xmly_record.pcm，FALSE 不保存*/
}



#include "xmly_config_info.h"


#define XY_APP_KEY "b7b3fe0d44cb4b988940f658e6bc84a2"
#define XY_APP_SECRET "329FBDB09AD7E92920F95049DB998E7A"
#define XY_SN "111128_00_1001128"

#define XY_HOMEPAGE_CH_ID  "3a5415b08cb046c4b276a20f29ea60e4"

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
	//return 0;
	return MMIAPISET_GetMultimVolume();
}

/*xmly端传vol值给watch端
 *
 *
 */
PUBLIC void  mmixysdk_api_set_watch_vol(int vol)
{
	
}
/*watch端返回netid值给xmly端
 *
 *
 */
uint32 xmly_iot_socket_get_netid()
{




}


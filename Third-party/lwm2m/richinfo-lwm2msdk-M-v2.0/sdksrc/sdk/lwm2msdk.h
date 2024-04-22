#ifndef LWM2M_SDK_H  
#define LWM2M_SDK_H  
  
#include <stdio.h>  
#include "liblwm2m.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifdef LWM2M_SUPPORT_4
#define LWM2M_SDK_LOCAL_PORT    (4683)
#define LWM2M_DM_UE_OBJECT_WRITE               668
#define LWM2M_DM_UE_OBJECT_READ               669
#define deviceinfo_num 24
//#define USE_NO_ENCRYPTION

// Resource Id's:
#define RES_DM_DEV_INFO                  6601
//#define RES_DM_APP_INFO                  6602
#define RES_DM_MAC                       6603
#define RES_DM_ROM                       6604
#define RES_DM_RAM                       6605
#define RES_DM_CPU                       6606
#define RES_DM_SYS_VERSION               6607
#define RES_DM_FIRM_VERSION              6608
#define RES_DM_FIRM_NAME                 6609
#define RES_DM_VOLTE                     6610

#define RES_DM_NET_TYPE                  6611
//#define RES_DM_NET_ACCT                  6612
#define RES_DM_PHONE                     6613
//#define RES_DM_LOCATION                  6614

//4.0 add
#define RES_DM_IMSI                       6615
#define RES_DM_SN                         6616
#define RES_DM_ROUTER_MAC                 6617
#define RES_DM_BLUETOOTH_MAC              6618
#define RES_DM_GPU                        6619
#define RES_DM_BOARD                      6620
#define RES_DM_RESOLUTION                 6621

//add 8_22
#define RES_DM_BATTERYCAPACITY            6612
#define RES_DM_SCREENSIZE                 6614
#define RES_DM_NETWORKSTATUS              6622
#define RES_DM_WEARINGSTATUS              6623
#define RES_DM_APP_INFO                   6624
#define RES_DM_IMSI2                      6625
#define RES_DM_BATTERYCAPACITYCURR        6626

struct deviceinfo {
    char name[32];
    int resid;
    BOOLEAN choice;
    char value[128];
};

//4.0 add
#define reportTime   7001
#define reportNum     7002
#define heartBeatTime  7003
#define retryInterval  7004
#define retryNum       7005

#define ADDRESSCFG      7006

//4.0 add
struct ruleConfig
{
    char  rulename[16];
    int resid;
    int value;
};
#define RULECONFIG_NUM 5

struct addressConfig
{
    char  name[16];
    int resid;
    char  value[64];
    int port;
};
#else
#define LWM2M_SDK_LOCAL_PORT    (56831)
#endif
//通知类型
enum _SDK_NOTIFY_TYPE
{
      NOTIFY_TYPE_SYS           = 0x0000  //SDK运行
    , NOTIFY_TYPE_REGISTER      = 0x0100  //注册
    , NOTIFY_TYPE_DEREGISTER              //注销
    , NOTIFY_TYPE_REG_UPDATE              //更新
};
typedef enum _SDK_NOTIFY_TYPE SDK_NOTIFY_TYPE;


enum _SDK_NOTIFY_CODE
{
      NOTIFY_CODE_OK      = 0x0000 //成功
    , NOTIFY_CODE_FAILED 
    , NOTIFY_CODE_UNKNOWEN 
};
typedef enum _SDK_NOTIFY_CODE SDK_NOTIFY_CODE;

//sdk消息通知函数参数
typedef struct
{

	SDK_NOTIFY_TYPE notify_type;
	SDK_NOTIFY_CODE notify_code;
	const char *    notify_msg;
	
} OptNotifyParam;


typedef struct
{
    //sdk消息通知函数--no block
	void  (*NotifyMsg) (OptNotifyParam *optNotifyParam);
	
	//采集设备信息的读函数--函数不能长时阻塞
	//resId  : 设备信息资源号；见规范文档；如6602等；
	//outbuff: resId对应的设备的字符串值； 需要函数内部malloc字符串的存储空间；sdk内部使用完会释放；
	//return： 0；成功； 其他失败
	int   (*DMReadInfo) (int resId,char **outbuff);
	
} OptFuncs;


typedef struct
{
	char szCMEI_IMEI[64]; 	//CMEI/IMEI
	char szCMEI_IMEI2[64]; 	//CMEI/IMEI2
	char szIMSI[64];		//IMSI
	char szDMv[16]; 		//DM版本号
	char szAppKey[64]; 		//appkey
	char szPwd[64];			//pwd秘钥
	
	int  nAddressFamily; 	//4//或6  ipv4，ipv6
	char szSrvIP[32];		//127.0.0.1
    int  nSrvPort;   		//5683
	int  nLifetime;    		//300
	int  nLocalPort;        //本地端口绑定,默认为56830
	int  nBootstrap; 		//是否booststrap 0,not use; other n,use;
#if 1 //modify by unisoc
       char bUseSDKRegUpdate;//use sdk registration update
#endif	
} Options;

#ifdef LWM2M_SUPPORT_4
typedef struct
{
    char brand[64];
    char model[64];
    char sdkVersion[64];
    char apiVersion[64];
    char apiType[64];
    char templateId[64];
    //int templateId;

    char szCMEI_IMEI[64];//CMEI/IMEI
    char szCMEI_IMEI2[64];//CMEI/IMEI2
    char szIMSI[64];//IMSI
    char szDMv[16];//DM版本号
    char szAppKey[64];//appkey
    char szPwd[64];//pwd秘钥

    int  nAddressFamily;//4//或6  ipv4，ipv6
    char szSrvIP[64];//127.0.0.1
    int  nSrvPort;//5683
    int  nLifetime;//300
    int  nLocalPort;//本地端口绑定,默认为56830
    int  nBootstrap;//是否booststrap 0,not use; other n,use;
#if 1 //modify by unisoc
    char bUseSDKRegUpdate;//use sdk registration update
#endif
} Options_V4;
#endif
#ifdef LWM2M_SUPPORT_4
int  LWM2M_SDK_INIT_V4(Options_V4* options,OptFuncs *optfuncs);
#else
//初始化Options:参数,optfuncs:回调函数
int LWM2M_SDK_INIT(Options * options,OptFuncs *optfuncs); 
#endif
//启动sdk，nMode=0,阻塞在当前线程运行；nMode!=0,在新线程中运行;
int LWM2M_SDK_RUN(int nMode);
//退出SDK的循环；注销lwm2m服务
int LWM2M_SDK_STOP(void);
//清理sdk,参数等
int LWM2M_SDK_FINI(void);

#if 1 //modify by unisoc
/* FUNCTION: LWM2M_SDK_UPDATE_REG()
 * Option.bUseSDKRegUpdate = 0: when the lifetime arrives, APP calls this
 * interface to perform registration update. Successfully callback myNotifyMsg 
 * will return NOTIFY_TYPE_REG_UPDATE with success code.
 * RETURNS: 
 */
void LWM2M_SDK_UPDATE_REG(void);

/* FUNCTION: LWM2M_SDK_UPDATE_SOCKET()
 * SDK first successfully registered: if the data link is disconnected, it will
 * be called after successful reactivation of pdp, the SDK will execute to 
 * recreate the soket.
 * RETURNS: 
 */
void LWM2M_SDK_UPDATE_SOCKET(void);

/* FUNCTION: LWM2M_SDK_IS_RUN()
 * Used to determine whether the lwm2m SKD is running
 * RETURNS: 
 */
int LWM2M_SDK_IS_RUN(void);
#ifdef LWM2M_SUPPORT_4
PUBLIC int LWM2M_SDK_UPDATE_RuleConfig(int target_value);
PUBLIC struct addressConfig * LWM2M_SDK_UPDATE_AddressConfig(void);
#endif
#endif

#ifdef __cplusplus
}
#endif
  
#endif

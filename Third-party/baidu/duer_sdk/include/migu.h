#ifndef __MIGU_H
#define __MIGU_H

/*****************************************************************
******************************结构体说明**************************
******************************************************************/
/*音响搜歌请求参数结构体*/
typedef struct boxSearchPostData{
	char * text;
	char * searchType;
	char * sort;
	char * issemantic;
	char * isCorrect;
	char * isCopyright;
	char * expire;
	char * sortType;
	char * contentFilter;
	char * matchType;
	char * searchRange;
	char * pageIndex;
	char * pageSize;
}boxSearchPostData_t;

/*试听行为结构体*/
typedef struct listeningBehavior{
	char * contentId;		//歌曲ID
	char * toneQuality;		//音质。"1"：标清；"2"：高清；"3"：无损
	char * runningTime;		//播放时长(毫秒)
	char * startTime;		//播放开始时间	格式yyyymmdd HHMMSS.mmmmmm
	char * stopTime;		//播放停止时间	格式yyyymmdd HHMMSS.mmmmmm
	char * userId;			//用户ID（合作伙伴自定义，不校验）
	char * deviceType;		//设备类型(1:安卓；2：IOS；3：linux；4：其他)
}listeningBehavior_t;

//=================================================================
//==========================通用接口===============================
//=================================================================
//初始化sdk
int migusdk_init( char * device_info);
//注销SDK
void migusdk_cleanup(void);
//获取SDK版本信息
char * getSdkVersion(char * version);

//=================================================================
//==========================http访问控制接口=======================
//=================================================================
/*停止http访问*/
void http_stop_notify(void);
/*设置最大重试次数（连接失败或者超时之后重新连接），默认为10*/
void http_set_max_resume_retry_count(size_t count);
/*设置最大超时重试次数, 默认为1*/
void http_set_max_timeout_retry_count(size_t count);
/*设置连接超时时间, 默认为0，即使用socket默认超时时间*/
void http_set_connect_timeout_ms(size_t timeout_ms);
/*设置receive超时时间，默认为2秒*/
void http_set_recv_timeout_ms(size_t timeout_ms);


/*
下面是咪咕后台相关接口。 “result” 为接收数据指针，请用户自行分配空间并置0
*/
//=================================================================
//==================获取歌曲、歌单专辑相关操作接口=================
//=================================================================
/*歌曲id查询歌曲信息接口*/
char * get_musicInfo(char * musicId,char *picSize, char *result);


#endif

#ifndef __LEBAO_API2_H
#define __LEBAO_API2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sds.h"
#include "link_list.h"
#include "http_client.h"

#ifndef REAL_WATCH_RTOS
#include <sys/types.h>
#endif

typedef struct result_data {
	sds code;
	sds msg;
} result_data_t;

typedef struct pic_cache_data {
	int needUpdate;
	void* control;
	sds cacheData;
	void* userData;
    int index;
} pic_cache_data_t;

typedef struct member_status_data {
	int memberStatus; // 0 非会员, 1 普通会员, 2 付费会员
	sds memberName;
	sds memberValidityTime;
	int monthlySubscription;
	sds productCode;
} member_status_data_t;

typedef struct device_status_data {
	result_data_t* result;
	int deviceStatus; // 1 待激活, 2 激活中, 3 已激活, 4 已下线
	sds activateTime;
	sds bindMobile;
	link_list_t* status;
	int logined;
	sds channelCode;
} device_status_data_t;

typedef struct column {
	int columnId;
	sds columnName;
	int columnContentType;
	sds pic;

	pic_cache_data_t picCache;
} column_t;

typedef struct column_list {
	result_data_t* result;
	link_list_t* cols;
} column_list_t;

// typedef struct audio_extend {
// 	sds director;
// 	sds actor;
// } audio_extend_t;

// typedef struct extend_info {
// 	sds name;
// 	link_list_t* pic;
// } extend_info_t;

// typedef struct song_extend {
// 	link_list_t* singer;
// 	link_list_t* album;
// } song_extend_t;

// typedef struct book_extend {
// 	link_list_t* author;
// 	link_list_t* publisher;
// 	link_list_t* anchor;
// } book_extend_t;

// typedef struct content_extends {
// 	audio_extend_t* audioExtend;
// 	song_extend_t* songExtend;
// 	book_extend_t* bookExtend;
// } content_extends_t;

typedef struct content_brief {
	sds contentId;
	int contentType;
	sds contentName;
	// link_list_t* pic;
	// content_extends_t* content;
	sds pic;
	sds album;
	sds singer;
	int collectFlag;
	int songBizType;
	int expired;
	sds duration;

    pic_cache_data_t picCache;
	int fromFavPage;
    sds playUrl;
} content_brief_t;

typedef struct songlist {
 	int songlistId;
 	sds songlistName;
 	sds pic;
 	int songCount;
	sds tag;
	int collectFlag;
 	int type;

	pic_cache_data_t picCache;
} songlist_t;

typedef struct songlist_list {
	result_data_t* result;
	songlist_t* songlist;
	link_list_t* contentBriefList;
} songlist_list_t;

// typedef struct banner_item {
// 	int bannerId;
// 	sds name;
// 	sds pic;
// 	int type;
// 	sds songlistId;
// 	sds url;
// } banner_item_t;

// typedef struct column_content_data {
//	link_list_t* columnList;
//	link_list_t* contentList;
//	link_list_t* songlistList;
//	link_list_t* bannerItemList;
// } column_content_data_t;

typedef struct column_resource_data {
	result_data_t* result;
	column_t* column;
	link_list_t* columnList;
	link_list_t* contentBriefList;
	link_list_t* songlistList;
} column_resource_data_t;

typedef struct content_section {
	result_data_t* result;
	sds contentId;
	sds parentContentId;
	int contentType;
	sds contentName;
	int contentIndex;
	sds pic;
	int collectFlag;
	link_list_t* contentPackageId;
	sds playUrl;
	sds playBqUrl;
	sds playHqUrl;
	sds playSqUrl;
	sds lrcUrl;
	link_list_t* songAuthor;
	link_list_t* lyricAuthor;
	link_list_t* author;
	sds singer;
	sds album;
	sds tag;
	sds duration;
	int isLocal;
	sds localPath;
	int songBizType;
	int authResult;
} content_section_t;

typedef struct collect_list {
	result_data_t* result;
	link_list_t* contentBriefList;
} collect_list_t;

typedef struct search_v2_list {
	result_data_t* result;
	link_list_t* contentBriefList;
    link_list_t* albumInfoList;
	int resultNum;
	int pageIndex;
	int pageSize;
} search_v2_list_t;

typedef struct album_info_t {
    sds albumId;
    sds name;
//    sds smallPic;
//    sds middlePic;
//    sds largePic;
} album_info_t;

typedef struct album_data_t {
    result_data_t* result;
    album_info_t* albumInfo;
    link_list_t* contentBriefList;
} album_data_t;

void lebao_clear_pic_cache_data(pic_cache_data_t* picCache);

void lebao_sdk_free_result_data(result_data_t* data);
void lebao_sdk_free_member_status_data(member_status_data_t* data);
void lebao_sdk_free_device_status_data(device_status_data_t* data);
void lebao_sdk_free_column(column_t* data);
void lebao_sdk_free_column_list(column_list_t* data);
void lebao_sdk_free_content_brief(content_brief_t* data);
void lebao_sdk_free_songlist(songlist_t* data);
void lebao_sdk_free_songlist_list(songlist_list_t* data);
void lebao_sdk_free_column_resource_data(column_resource_data_t* data);
void lebao_sdk_free_content_section(content_section_t* data);
void lebao_sdk_free_collect_list(collect_list_t* data);
void lebao_sdk_free_search_v2_list(search_v2_list_t* data);
void lebao_sdk_free_album_info(album_info_t* data);
void lebao_sdk_free_album_data(album_data_t* data);

link_list_t* lebao_sdk_clone_content_brief_list(link_list_t* contentBriefList);

int lebao_sdk_device_status(device_status_data_t* data, http_client_progress_t callback, void* caller);
int lebao_sdk_content_column_query(
	column_list_t* columnList, http_client_progress_t callback, void* caller);
int lebao_sdk_content_column_resource_query(
	const int columnId, const int contentType, column_resource_data_t* columnData, http_client_progress_t callback, void* caller);
int lebao_sdk_content_songlist_song_query(
	const int songlistId, songlist_list_t* songlistList, http_client_progress_t callback, void* caller);
int lebao_sdk_content_song_detail_v2(
	const char* contentId, content_section_t* contentSection, http_client_progress_t callback, void* caller);
int lebao_sdk_content_ring_detail(
	const char* contentId, content_section_t* contentSection, http_client_progress_t callback, void* caller);
int lebao_sdk_content_album_songs(const char* albumId, album_data_t* albumData, http_client_progress_t callback, void* caller);

int lebao_sdk_collect_content_query(collect_list_t* collectList, http_client_progress_t callback, void* caller);
int lebao_sdk_collect_content_add(const char* contentId, http_client_progress_t callback, void* caller);
int lebao_sdk_collect_content_delete(const char* contentId, http_client_progress_t callback, void* caller);

int lebao_sdk_search_v2(const int pageIndex, const int pageSize, const char* keyword, const char* contentType,
	search_v2_list_t* resultList, http_client_progress_t callback, void* caller);
sds lebao_sdk_lingxiyun_iat_v1(link_list_t* stream, const char* filename, const int filesize,
	http_client_progress_t callback, void* caller);

void lebao_sdk_base_set_use_http(const int useHttp);
void lebao_sdk_iat_set_use_http(const int useHttp);
#ifdef __cplusplus
}
#endif

#endif

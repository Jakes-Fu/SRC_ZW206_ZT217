//author: Justin, for watch version
#ifndef __LEBAO_API_H
#define __LEBAO_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sds.h"
#include "link_list.h"
#include "http_client.h"

#ifndef REAL_WATCH_RTOS
#include <sys/types.h>
#endif

typedef struct product_info {
	sds channelCode;
	sds channelKey;
	sds defSeq;
	sds excode;
	sds info;
	sds orderType;
	sds orderUrl;
	sds price;
	sds secretKey;
	sds serviceId;
	sds serviceName;
	sds status;
	sds type;
	sds auth;

	sds asrType;
	int mp3Type;
} product_info_t;

typedef struct user_agreement {
	int protocolVersion;
	sds privacyProtocolUrl;
	sds privacyProtocolContent;
	sds serviceProtocolUrl;
	sds serviceProtocolContent;
} user_agreement_t;

typedef struct tip_sound {
	int index;
	sds cId;
	sds code;
	sds msg;
	sds soundUrl;
	int count;
} tip_sound_t;

typedef enum
{
	API_LEBAO_ASR_IFLYTEK,
	API_LEBAO_ASR_BAIDU,
	API_LEBAO_ASR_MIGU,
	API_LEBAO_ASR_MIGUDCT,
} lebao_api_asr_type_t;

void clear_product(product_info_t* info);
void free_user_agreement(user_agreement_t * data);
void free_tip_sound(tip_sound_t * data);

int lebao_login(http_client_progress_t callback, void* caller);
int lebao_is_vip(http_client_progress_t callback, void* caller, const int postImsi);

int lebao_download_to_file(const char* uri, const char* filename, const size_t rangeStart, const size_t exceedSize, const size_t maxSize, http_client_progress_t callback, void* caller, int* dwSize);
int lebao_download_to_buffer(const char* uri, sds* buffer, const size_t rangeStart, const size_t exceedSize, const size_t maxSize, http_client_progress_t callback, void* caller);
int lebao_download_stream(const char* uri, const size_t rangeStart, const size_t rangeEnd, http_client_body_chunk_t dataCallback, http_client_progress_t callback, void* caller);

product_info_t* lebao_query_product(http_client_progress_t callback, void* caller);

sds lebao_get_asr_token(const lebao_api_asr_type_t type, http_client_progress_t callback, void* caller);
sds lebao_post_baidu_voice(const char* uri, const char* contentType, const char* filename, const int filesize, http_client_progress_t callback, void* caller);
sds lebao_post_baidu_voice_stream(const char* uri, const char* contentType, link_list_t* stream, const int streamsize, http_client_progress_t callback, void* caller);
sds lebao_post_migu_voice(const char* contentType, const char* filename, const int filesize, http_client_progress_t callback, void* caller);
sds lebao_post_migu_voice_stream(const char* contentType, link_list_t* stream, const int streamsize, http_client_progress_t callback, void* caller);

user_agreement_t* lebao_get_user_agreement(http_client_progress_t callback, void* caller);
link_list_t* lebao_query_tips_sound(http_client_progress_t callback, void* caller);

void lebao_api_set_use_http(const int useHttp);
#ifdef __cplusplus
}
#endif

#endif

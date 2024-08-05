#ifndef __HELPER_RSA_H
#define __HELPER_RSA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "port_cfg.h"

int helper_rsa_decrypt(const char* key, sds* src, sds* plain);
int helper_rsa_decrypt2(const char* key, sds* src);
int helper_rsa_encrypt(const char* key, sds* src, sds* enc);
// 3: MD5, 4: HmacSHA1, 6: SHA256
// mbedtls_md_type_t
int helper_hmac(const int type, const char* key, const sds src, sds* sign, int* signLen);
int helper_bin_to_hex(const sds sign, const int signLen, sds* signHex);
int helper_hmac_sha1_hex(const char* key, const sds src, sds* signHex);
int helper_hmac_md5_hex(const char* key, const sds src, sds* signHex);
int helper_sha256_hex(const sds src, sds* signHex);

#ifdef __cplusplus
}
#endif

#endif // __HELPER_RSA_H


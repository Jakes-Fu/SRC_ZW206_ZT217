#include "helper_rsa.h"

#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/sha256.h"

int helper_rsa_decrypt(const char* key, sds* src, sds* plain)
{
	uint8_t iv[18] = { 0 };
	mbedtls_cipher_context_t ctx;
	size_t olen = 0, totalLen = 0, srcLen = 0;
	uint8_t padding = 0;
	size_t i = 0;

	if (key == NULL || src == NULL || plain == NULL)
		return -1;

	srcLen = sdslen(*src);
	if (srcLen <= 0)
		return -1;

    if (*plain == NULL || sdslen(*plain) <= 0) {
        *plain = sdsMakeRoomFor(*plain, srcLen);
    }
	mbedtls_cipher_init(&ctx);
	mbedtls_cipher_set_padding_mode(&ctx, MBEDTLS_PADDING_PKCS7);
	mbedtls_cipher_setup(&ctx,
	mbedtls_cipher_info_from_values(MBEDTLS_CIPHER_AES_128_ECB, 128, MBEDTLS_MODE_ECB));
	mbedtls_cipher_setkey(&ctx, key, 128, MBEDTLS_DECRYPT);
	mbedtls_cipher_set_iv(&ctx, iv, 16);
	mbedtls_cipher_reset(&ctx);
	for (i = 0; i < srcLen / 16; i++) {
		mbedtls_cipher_update(&ctx, (*src) + i * 16, 16, *plain + i * 16, &olen);
		totalLen += olen;
	}
	padding = (*plain)[totalLen - 1];
    if (src != plain)
        sdsIncrLen(*plain, totalLen - padding);
    else
        (*plain)[totalLen - padding] = '\0';

	mbedtls_cipher_finish(&ctx, *plain + totalLen, &olen);
	mbedtls_cipher_free(&ctx);
	return totalLen;
}

int helper_rsa_decrypt2(const char* key, sds* src)
{
    return helper_rsa_decrypt(key, src, src);
}

int helper_rsa_encrypt(const char* key, sds* src, sds* enc)
{
	uint8_t iv[18] = { 0 };
	mbedtls_cipher_context_t ctx;
	size_t olen = 0, totalLen = 0, srcLen = 0;
	uint8_t padding = 0;
	size_t i = 0;

	if (key == NULL || src == NULL || enc == NULL)
		return -1;

	srcLen = sdslen(*src);
	if (srcLen <= 0)
		return -1;

	padding = 16 - srcLen % 16;
	*src = sdsMakeRoomFor(*src, srcLen + 16);
	for (i = 0; i < padding; i++) {
		(*src)[srcLen + i] = padding;
	}

	*enc = sdsMakeRoomFor(*enc, srcLen + 16);
	mbedtls_cipher_init(&ctx);
	mbedtls_cipher_set_padding_mode(&ctx, MBEDTLS_PADDING_PKCS7);
	mbedtls_cipher_setup(&ctx,
	mbedtls_cipher_info_from_values(MBEDTLS_CIPHER_AES_128_ECB, 128, MBEDTLS_MODE_ECB));
	mbedtls_cipher_setkey(&ctx, key, 128, MBEDTLS_ENCRYPT);
	mbedtls_cipher_set_iv(&ctx, iv, 16);
	mbedtls_cipher_reset(&ctx);
	for (i = 0; i < (srcLen + padding) / 16; i++) {
		mbedtls_cipher_update(&ctx, (*src) + i * 16, 16, *enc + i * 16, &olen);
		totalLen += olen;
	}
	sdsIncrLen(*enc, totalLen);

	mbedtls_cipher_finish(&ctx, *enc + totalLen, &olen);
	mbedtls_cipher_free(&ctx);
	return totalLen;
}

int helper_hmac(const int type, const char* key, const sds src, sds* sign, int* signLen)
{
	int len = 0, i = 0;
	int ret = 0;

	mbedtls_md_context_t ctx;
	const mbedtls_md_info_t* info;

	mbedtls_md_init(&ctx);
	info = mbedtls_md_info_from_type(type);
	ret = mbedtls_md_setup(&ctx, info, 1);
	if (ret != 0) {
		goto exit;
	}

	ret = mbedtls_md_hmac_starts(&ctx, (unsigned char*)key, os_strlen(key));
	if (ret != 0) {
		goto exit;
	}

	ret = mbedtls_md_hmac_update(&ctx, (unsigned char*)src, sdslen(src));
	if (ret != 0) {
		goto exit;
	}

	ret = mbedtls_md_hmac_finish(&ctx, *sign);
	if (ret != 0) {
		goto exit;
	}

	len = mbedtls_md_get_size(info);
	sdsIncrLen(*sign, len);
	if (signLen != NULL)
		*signLen = len;

exit:
	mbedtls_md_free(&ctx);

	return ret;
}

int helper_bin_to_hex(const sds sign, const int signLen, sds* signHex)
{
    int i = 0;
    *signHex = sdsMakeRoomFor(sdsempty(), signLen * 2);
    for (i = 0; i < signLen; i++) {
        *signHex = sdscatprintf(*signHex, "%02x", sign[i] & 0xFF);
    }
    sdsIncrLen(*signHex, signLen * 2);
    return 0;
}

int helper_hmac_sha1_hex(const char* key, const sds src, sds* signHex)
{
	int signLen = 0;
	sds sign = sdsMakeRoomFor(sdsempty(), 128);
    int ret = 0;
    int type = MBEDTLS_MD_SHA1;

    ret = helper_hmac(type, key, src, &sign, &signLen);
	if (ret == 0 && signLen > 0) {
        ret = helper_bin_to_hex(sign, signLen, signHex);
	}
	sdsfree_val(sign);
	return ret;
}

int helper_hmac_md5_hex(const char* key, const sds src, sds* signHex)
{
    int signLen = 0;
    sds sign = sdsMakeRoomFor(sdsempty(), 128);
    int ret = 0;
    int type = MBEDTLS_MD_MD5;

    ret = helper_hmac(type, key, src, &sign, &signLen);
    if (ret == 0 && signLen > 0) {
        ret = helper_bin_to_hex(sign, signLen, signHex);
    }
    sdsfree_val(sign);
    return ret;
}

/*
int helper_sha256_hex(const sds src, sds* signHex)
{
	int ret = 0, len = 0, i = 0;
	sds sign = sdsMakeRoomFor(sdsempty(), 128);
	mbedtls_sha256_context ctx;

	mbedtls_sha256_init(&ctx);
	do {
		if ((ret = mbedtls_sha256_starts_ret(&ctx, 0)) != 0)
			break;

		ret = mbedtls_sha256_update_ret(&ctx, src, sdslen(src));
		if (ret != 0)
			break;

		if ((ret = mbedtls_sha256_finish_ret(&ctx, sign)) != 0)
			break;

		*signHex = sdsMakeRoomFor(sdsempty(), 64);
		for (i = 0; i < 32; i++) {
			*signHex = sdscatprintf(*signHex, "%02x", sign[i] & 0xFF);
		}
		sdsIncrLen(*signHex, 64);
	} while (0);

	sdsfree_val(sign);
	mbedtls_sha256_free(&ctx);
	return (ret);
}
*/

#define PBKDF2_SHA256_STATIC
#define PBKDF2_SHA256_IMPLEMENTATION
#include "pbkdf2_sha256.h"

int helper_sha256_hex(const sds src, sds* signHex)
{
	int i = 0;
	SHA256_CTX ctx = { 0 };
	uint8_t md[SHA256_DIGESTLEN] = { 0 };

	pbkdf2_sha256_init(&ctx);
	pbkdf2_sha256_update(&ctx, src, sdslen(src));
	pbkdf2_sha256_final(&ctx, md);

	*signHex = sdsMakeRoomFor(sdsempty(), 64);
	for (i = 0; i < 32; i++) {
		*signHex = sdscatprintf(*signHex, "%02x", md[i] & 0xFF);
	}
	sdsIncrLen(*signHex, 64);
	return 0;
}

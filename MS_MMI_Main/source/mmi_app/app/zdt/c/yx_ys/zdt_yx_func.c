/*
云芯平台
*/
#include "zdt_app.h"
#ifndef WIN32
#ifdef ZDT_PLAT_YX_SUPPORT_YS_AES
#include "mbedtls/pem.h"
#include "mbedtls/base64.h"
#include "mbedtls/des.h"
#include "mbedtls/aes.h"
#include "mbedtls/md5.h"
#include "mbedtls/cipher.h"
#else
#include "mbedtls/md5.h"
#endif
#endif

#ifdef ZDT_PLAT_YX_SUPPORT
void YX_Func_MD5_Generate(uint8 * input, unsigned int input_len,char out_str[32])
{
#ifndef WIN32
#ifdef ZDT_PLAT_YX_SUPPORT_YS
	SCI_MEMCPY(out_str,YX_NET_MD5_KEY,32);
#elif defined(ZDT_PLAT_YX_SUPPORT_TX)
	SCI_MEMCPY(out_str,"g3oCnND6NzhfA6dmNfSnBWZuKyLdeYLd",32);
#else
    mbedtls_md5_context md5_ctx = {0};
    uint8 digest[16] = {0};
    uint16 i = 0;
    mbedtls_md5_init( &md5_ctx );
    mbedtls_md5_starts( &md5_ctx );
    mbedtls_md5_update( &md5_ctx, input, input_len);
    mbedtls_md5_finish( &md5_ctx, digest);
    mbedtls_md5_free( &md5_ctx );
    
   for(i = 0; i < 16; i++)
   {
        sprintf(out_str+(i*2),"%02x",digest[i]);
   }
#endif
#else
    SCI_MEMCPY(out_str,YX_NET_MD5_KEY,32);
#endif
}

void YX_Func_MD5_Test(void)
{
    char out_srt[32] = {0};
    unsigned int len = SCI_STRLEN("SG*8800000015*0002*LKVMg3oCnND6NzhfA6dmNfSnBWZuKyLdeY"); 
    YX_Func_MD5_Generate((uint8 *)"SG*8800000015*0002*LKVMg3oCnND6NzhfA6dmNfSnBWZuKyLdeY",len,out_srt);
    //9fa56f7e41aaf7273f30faff536619e6
    ZDT_LOG("YX_Func_MD5_Test Out %s",out_srt);
}

#ifdef ZDT_PLAT_YX_SUPPORT_YS_AES
static uint8 Hex2Dec(uint8 bHexChar)
{
    uint8  bRtn = 0;

    if(bHexChar >= '0' && bHexChar <= '9')
        bRtn = bHexChar - '0';
    else
    {
        switch(bHexChar)
        {
            case 'A':
            case 'a':
                bRtn = 10;
                break;
            case 'B':
            case 'b':
                bRtn = 11;
                break;
            case 'C':
            case 'c':
                bRtn = 12;
                break;
            case 'D':
            case 'd':
                bRtn = 13;
                break;
            case 'E':
            case 'e':
                bRtn = 14;
                break;
            case 'F':
            case 'f':
                bRtn = 15;
                break;
        }
    }
    return(bRtn);
}

static uint32 decodeHex(uint8 * pSrcData,uint32 SrcDatalen, uint8* pDestData )
{
    uint32 i = 0;
    uint32 j = 0;
    uint8 f  = 0;
    uint8 t = 0;
    uint32 len = SrcDatalen;
    
    if(pSrcData == PNULL || SrcDatalen == 0 || pDestData == PNULL)
    {
        return 0;
    }

    for (i = 0, j = 0; j < len; i++)
    {
        t = Hex2Dec(pSrcData[j++]);
        f = t << 4;
        t = Hex2Dec(pSrcData[j++]);
        f = f | t;
        pDestData[i] = f;
    }
    
    return i;
}

static uint8 * YX_Func_AES_Encrypt(uint8 *data, uint32 data_len,uint32 * pOutlen)
{
    uint8 * encrypt_data = PNULL;
#ifndef WIN32
    mbedtls_aes_context aes_ctx = {0};
    uint8 iv_val[17] = {0};
    uint8 key_val[33] = {0};
    uint32 encrypt_data_len = 0;
    uint8 temp_len = 0;
    uint8 tmp_val = 0;
    uint32 i = 0;
    if(data == NULL || data_len == 0)
    {
        return PNULL;
    }
    temp_len = 16-(data_len%16);
    encrypt_data_len = data_len+temp_len;
    encrypt_data = SCI_ALLOC_APPZ(encrypt_data_len);
    if(encrypt_data == NULL)
    {
        return PNULL;
    }
    SCI_MEMCPY(encrypt_data,data,data_len);
    for(i = data_len; i < (data_len+temp_len); i++)
    {
        encrypt_data[i] = temp_len;
    }
    *pOutlen = encrypt_data_len;
    
    mbedtls_aes_init( &aes_ctx );
#if 1
    SCI_MEMCPY(iv_val,YX_AES_IV_STR,strlen(YX_AES_IV_STR));
    SCI_MEMCPY(key_val,YX_AES_KEY_STR,strlen(YX_AES_KEY_STR));
#else
    decodeHex(YX_AES_IV_STR,strlen(YX_AES_IV_STR),iv_val);
    decodeHex(YX_AES_KEY_STR,strlen(YX_AES_KEY_STR),key_val);
#endif

    
    mbedtls_aes_setkey_enc( &aes_ctx, key_val, 256);

    mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_ENCRYPT, encrypt_data_len, iv_val, encrypt_data, encrypt_data);

    mbedtls_aes_free( &aes_ctx );
#endif
    return encrypt_data;
}

static uint8 * YX_Func_AES_EncryptByIV(uint8 *data, uint32 data_len,uint32 * pOutlen,uint8 * iv_str)
{
    uint8 * encrypt_data = PNULL;
#ifndef WIN32
    mbedtls_aes_context aes_ctx = {0};
    uint8 iv_val[17] = {0};
    uint8 key_val[33] = {0};
    uint32 encrypt_data_len = 0;
    uint8 temp_len = 0;
    uint8 tmp_val = 0;
    uint32 i = 0;
    if(data == NULL || data_len == 0)
    {
        return PNULL;
    }
    temp_len = 16-(data_len%16);
    encrypt_data_len = data_len+temp_len;
    encrypt_data = SCI_ALLOC_APPZ(encrypt_data_len);
    if(encrypt_data == NULL)
    {
        return PNULL;
    }
    SCI_MEMCPY(encrypt_data,data,data_len);
    for(i = data_len; i < (data_len+temp_len); i++)
    {
        encrypt_data[i] = temp_len;
    }
    *pOutlen = encrypt_data_len;
    
    mbedtls_aes_init( &aes_ctx );
#if 1
    SCI_MEMCPY(iv_val,iv_str,strlen(iv_str));
    SCI_MEMCPY(key_val,YX_AES_KEY_STR,strlen(YX_AES_KEY_STR));
#else
    decodeHex(YX_AES_IV_STR,strlen(YX_AES_IV_STR),iv_val);
    decodeHex(YX_AES_KEY_STR,strlen(YX_AES_KEY_STR),key_val);
#endif

    
    mbedtls_aes_setkey_enc( &aes_ctx, key_val, 256);

    mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_ENCRYPT, encrypt_data_len, iv_val, encrypt_data, encrypt_data);

    mbedtls_aes_free( &aes_ctx );
#endif
    return encrypt_data;
}

static uint8 * YX_Func_AES_Decrypt(uint8 *data, uint32 data_len,uint32 * pOutlen)
{
    uint8 * encrypt_data = PNULL;
#ifndef WIN32
    mbedtls_aes_context aes_ctx = {0};
    uint8 iv_val[17] = {0};
    uint8 key_val[33] = {0};
    uint32 encrypt_data_len = 0;
    uint32 temp_len = 0;
    uint32 i = 0;
    if(data == NULL || data_len == 0)
    {
        return PNULL;
    }
    temp_len = data_len/16;
    encrypt_data_len = temp_len*16;
    encrypt_data = SCI_ALLOC_APPZ(encrypt_data_len+1);
    if(encrypt_data == NULL)
    {
        return PNULL;
    }
    SCI_MEMCPY(encrypt_data,data,encrypt_data_len);
    *pOutlen = encrypt_data_len;
    mbedtls_aes_init(&aes_ctx);

#if 1
    SCI_MEMCPY(iv_val,YX_AES_IV_STR,strlen(YX_AES_IV_STR));
    SCI_MEMCPY(key_val,YX_AES_KEY_STR,strlen(YX_AES_KEY_STR));
#else
    decodeHex(YX_AES_IV_STR,strlen(YX_AES_IV_STR),iv_val);
    decodeHex(YX_AES_KEY_STR,strlen(YX_AES_KEY_STR),key_val);
#endif

    mbedtls_aes_setkey_dec( &aes_ctx, key_val, 256);

    mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_DECRYPT, encrypt_data_len, iv_val, encrypt_data, encrypt_data);

    mbedtls_aes_free( &aes_ctx );
#endif
    return encrypt_data;
}

static uint8 * YX_Func_AES_DecryptByIV(uint8 *data, uint32 data_len,uint32 * pOutlen, uint8 * iv_str)
{
    uint8 * encrypt_data = PNULL;
#ifndef WIN32
    mbedtls_aes_context aes_ctx = {0};
    uint8 iv_val[17] = {0};
    uint8 key_val[33] = {0};
    uint32 encrypt_data_len = 0;
    uint32 temp_len = 0;
    uint32 i = 0;
    if(data == NULL || data_len == 0)
    {
        return PNULL;
    }
    temp_len = data_len/16;
    encrypt_data_len = temp_len*16;
    encrypt_data = SCI_ALLOC_APPZ(encrypt_data_len+1);
    if(encrypt_data == NULL)
    {
        return PNULL;
    }
    SCI_MEMCPY(encrypt_data,data,encrypt_data_len);
    *pOutlen = encrypt_data_len;
    mbedtls_aes_init(&aes_ctx);

#if 1
    SCI_MEMCPY(iv_val,iv_str,strlen(iv_str));
    SCI_MEMCPY(key_val,YX_AES_KEY_STR,strlen(YX_AES_KEY_STR));
#else
    decodeHex(YX_AES_IV_STR,strlen(YX_AES_IV_STR),iv_val);
    decodeHex(YX_AES_KEY_STR,strlen(YX_AES_KEY_STR),key_val);
#endif

    mbedtls_aes_setkey_dec( &aes_ctx, key_val, 256);

    mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_DECRYPT, encrypt_data_len, iv_val, encrypt_data, encrypt_data);

    mbedtls_aes_free( &aes_ctx );
#endif
    return encrypt_data;
}

uint8 * YX_Func_Encrypt(uint8 *data, uint32 data_len,uint32 * pOutlen)
{
    uint8 * aes_data = PNULL;
    uint32  aes_len = 0;
    uint32  base64_max_len = 0;
    uint32  base64_rel_len = 0;
    uint8 * encrypt_data = PNULL;
    if(data == PNULL || data_len == 0)
    {
        return PNULL;
    }
    ZDT_CryptTrace(data,data_len);
    aes_data = YX_Func_AES_Encrypt(data,data_len,&aes_len);
    if(aes_data == PNULL)
    {
        return PNULL;
    }
    //ZDT_HexTraceU8(aes_data,aes_len);
    base64_max_len = (aes_len / 3 + 2) * 4+1;
    encrypt_data = SCI_ALLOC_APPZ(base64_max_len+1);
    if (encrypt_data  == NULL)
    {
        SCI_FREE(aes_data);
        return PNULL;
    }
#ifndef WIN32
    mbedtls_base64_encode(encrypt_data, base64_max_len, &base64_rel_len, aes_data, aes_len);
#endif
    *pOutlen = base64_rel_len;
    encrypt_data[base64_rel_len] = 0;
    ZDT_CryptTrace(encrypt_data,base64_rel_len);
    SCI_FREE(aes_data);
    return encrypt_data;
}

uint8 * YX_Func_EncryptByIV(uint8 *data, uint32 data_len,uint32 * pOutlen,uint8 * iv_str)
{
    uint8 * aes_data = PNULL;
    uint32  aes_len = 0;
    uint32  base64_max_len = 0;
    uint32  base64_rel_len = 0;
    uint8 * iv_head_str = PNULL;
    uint8 * encrypt_data = PNULL;
    if(data == PNULL || data_len == 0)
    {
        return PNULL;
    }
    ZDT_CryptTrace(data,data_len);
    aes_data = YX_Func_AES_EncryptByIV(data+1,data_len-2,&aes_len,iv_str);
    if(aes_data == PNULL)
    {
        return PNULL;
    }
    ZDT_CryptTrace(iv_str,16);
    iv_head_str = SCI_ALLOC_APPZ(aes_len + 16);
    if(iv_head_str == PNULL)
    {
        SCI_FREE(aes_data);
        return PNULL;
    }
    SCI_MEMCPY(iv_head_str,iv_str,16);
    SCI_MEMCPY(iv_head_str+16,aes_data,aes_len);
    aes_len += 16;
    SCI_FREE(aes_data);

    ZDT_CryptTrace(iv_head_str,aes_len);
    base64_max_len = (aes_len / 3 + 2) * 4+1;
    encrypt_data = SCI_ALLOC_APPZ(base64_max_len+3);
    if (encrypt_data  == NULL)
    {
        SCI_FREE(iv_head_str);
        return PNULL;
    }
#ifndef WIN32
    mbedtls_base64_encode(encrypt_data+1, base64_max_len, &base64_rel_len, iv_head_str, aes_len);
#endif
    *pOutlen = base64_rel_len+2;
    encrypt_data[0] = '[';
    encrypt_data[base64_rel_len+1] = ']';
    ZDT_CryptTrace(encrypt_data,base64_rel_len+2);
    SCI_FREE(iv_head_str);
    return encrypt_data;
}

uint8 * YX_Func_Decrypt(uint8 *data, uint32 data_len,uint32 * pOutlen)
{
    uint8 * aes_data = PNULL;
    uint8  left_len = 0;
    uint32  aes_len = 0;
    uint32  aes_rel_len = 0;
    uint32  base64_max_len = 0;
    uint32  base64_rel_len = 0;
    uint8 * decrypt_data = PNULL;
    if(data == PNULL || data_len == 0)
    {
        return PNULL;
    }
    ZDT_DecryptTrace(data,data_len);
    base64_max_len = data_len;
    decrypt_data = SCI_ALLOC_APPZ(base64_max_len);
    if (decrypt_data  == NULL)
    {
        return PNULL;
    }
#ifndef WIN32
    mbedtls_base64_decode(decrypt_data, base64_max_len, &base64_rel_len, data, data_len);
#endif
    
    //ZDT_HexTraceU8(decrypt_data,base64_rel_len);
    aes_data = YX_Func_AES_Decrypt(decrypt_data,base64_rel_len,&aes_len);
    if(aes_data == PNULL)
    {
        SCI_FREE(decrypt_data);
        return PNULL;
    }
    left_len = aes_data[aes_len-1];
    aes_rel_len = aes_len - left_len;
    ZDT_DecryptTrace(aes_data,aes_rel_len);
    *pOutlen = aes_rel_len;
    SCI_FREE(decrypt_data);
    return aes_data;
}

uint8 * YX_Func_DecryptByIV(uint8 *data, uint32 data_len,uint32 * pOutlen)
{
    uint8 * aes_data = PNULL;
    uint8  left_len = 0;
    uint32  aes_len = 0;
    uint32  aes_rel_len = 0;
    uint32  base64_max_len = 0;
    uint32  base64_rel_len = 0;
    uint8 * decrypt_data = PNULL;
    uint8 * out_ptr = PNULL;
    uint32 out_len = 0;

    uint8 iv_str[17] = {0};
    if(data == PNULL || data_len == 0)
    {
        return PNULL;
    }
    ZDT_DecryptTrace(data,data_len);
    base64_max_len = data_len;
    decrypt_data = SCI_ALLOC_APPZ(base64_max_len);
    if (decrypt_data  == NULL)
    {
        return PNULL;
    }
#ifndef WIN32
    mbedtls_base64_decode(decrypt_data, base64_max_len, &base64_rel_len, data, data_len);
#endif
    if(base64_rel_len < 18)
    {
        SCI_FREE(decrypt_data);
        return PNULL;
    }
    SCI_MEMCPY(iv_str,decrypt_data,16);
    ZDT_DecryptTrace(iv_str,16);
    aes_data = YX_Func_AES_DecryptByIV(decrypt_data+16,base64_rel_len-16,&aes_len,iv_str);
    if(aes_data == PNULL)
    {
        SCI_FREE(decrypt_data);
        return PNULL;
    }
    left_len = aes_data[aes_len-1];
    aes_rel_len = aes_len - left_len;
    
    SCI_FREE(decrypt_data);
    out_ptr = SCI_ALLOC_APPZ(aes_rel_len+3);
    if(out_ptr == PNULL)
    {
        SCI_FREE(aes_data);
        return PNULL;
    }
    out_ptr[0] = '[';
    SCI_MEMCPY(out_ptr+1,aes_data,aes_rel_len);
    out_ptr[aes_rel_len+1] = ']';
    *pOutlen = aes_rel_len+2;
    SCI_FREE(aes_data);
    ZDT_DecryptTrace(out_ptr,aes_rel_len+2);
    return out_ptr;
}

void YX_Func_TestCrypt(void)
{
    uint8 * pOutData = PNULL;
    uint32 OutDataLen = 0;

    ZDT_LOG("YX_Func_TestCrypt");
    
    pOutData = YX_Func_Encrypt("[tttCnND6NzhfA6dmNfSnBWZuKyLdeYLdYS*352273017386311*0009*LK,0,0,80]",strlen("[tttCnND6NzhfA6dmNfSnBWZuKyLdeYLdYS*352273017386311*0009*LK,0,0,80]"),&OutDataLen);

    if(pOutData != PNULL)
    {
        uint8 * pDecData = PNULL;
        uint32 DecDataLen = 0;
        pDecData = YX_Func_Decrypt(pOutData,OutDataLen,&DecDataLen);
        if(pDecData != PNULL)
        {
            SCI_FREE(pDecData);
        }
        SCI_FREE(pOutData);
    }
    return;
}
#endif

uint8 YX_Func_HexCharToInt(uint8 hexchar)
{
	if (hexchar>=0x30 && hexchar <=0x39)
	{
		return (hexchar - 0x30);
	}
	if (hexchar>= 'a' && hexchar <='f')
	{
		return (hexchar - 'a' +10);
	}
	if (hexchar>= 'A' && hexchar <='F')
	{
		return (hexchar - 'A' +10);
	}
	return 15;
}

uint8 YX_Func_IntToHexChar(uint8 bNibble)
{
    uint8  bRtn = '0';

    if(bNibble <= 9)
        bRtn = bNibble + '0';
    else if(bNibble <= 0x0F)
    {
        switch(bNibble)
        {
            case 10:
                bRtn = 'A';
                break;
            case 11:
                bRtn = 'B';
                break;
            case 12:
                bRtn = 'C';
                break;
            case 13:
                bRtn = 'D';
                break;
            case 14:
                bRtn = 'E';
                break;
            case 15:
                bRtn = 'F';
                break;
        }
    }
    return(bRtn);
}

uint8 YX_Func_IntToHexChar_One(uint8 data,uint8 * dest_buf)
{
    uint8 low = 0;
    uint8 high = 0;
    high = data>>4;
    dest_buf[0] = (char)YX_Func_IntToHexChar(high);
    low = data & 0x0F;
    dest_buf[1] = (char)YX_Func_IntToHexChar(low);
    return 2;
}

int  YX_Func_GetCmdPosPara(uint8** pCmd, uint32* pLen, char* pStr, uint16 wMaxStrLen,uint16 pos)
{
    int  bRtn = 0, c, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    int StartIndex = 0;
    int p = pos;
    
    if(*pLen == 0)
    {
        return -1;
    }
    if(p > 0)
    {
        for(i=0; i < (*pLen); i++)
        {
            if(*(*pCmd + i) == '*')
            {
                p--;
                if(p == 0)
                {
                    StartIndex = i+1;
                    break;
                }
            }
        }
        
        if(p != 0)
        {
            return -2;
        }
    }
    
    for(i=StartIndex; i < (*pLen); i++)
    {
        if(*(*pCmd + i) == '*')
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if((EndIndex-StartIndex) > 0)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=StartIndex; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex - StartIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 1);
            *pLen -= (EndIndex + 1);
        }
    }
    return(bRtn);
}

int  YX_Func_GetNextPara(uint8** pCmd, uint32* pLen, char* pStr, uint32 wMaxStrLen)
{
    int  bRtn = 0, c, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    
    if(*pLen == 0)
    {
        return -1;
    }
    
    for(i=0; i < (*pLen); i++)
    {
        if(*(*pCmd + i) == ',')
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if(EndIndex)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=0; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 1);
            *pLen -= (EndIndex + 1);
        }
    }
    return(bRtn);
}

int  YX_Func_GetNextParaM(uint8** pCmd, uint32* pLen, char* pStr, uint32 wMaxStrLen)
{
    int  bRtn = 0, c, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    
    if(*pLen == 0)
    {
        return -1;
    }
    
    for(i=0; i < (*pLen); i++)
    {
        if(*(*pCmd + i) == ':')
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if(EndIndex)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=0; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 1);
            *pLen -= (EndIndex + 1);
        }
    }
    return(bRtn);
}
int  YX_Func_GetPosPara(uint8** pCmd, uint32* pLen, char* pStr, uint16 wMaxStrLen,uint16 pos)
{
    int  bRtn = 0, c, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    int StartIndex = 0;
    int p = pos;
    
    if(*pLen == 0)
    {
        return -1;
    }
    if(p > 0)
    {
        for(i=0; i < (*pLen); i++)
        {
            if(*(*pCmd + i) == ',')
            {
                p--;
                if(p == 0)
                {
                    StartIndex = i+1;
                    break;
                }
            }
        }
        
        if(p != 0)
        {
            return -2;
        }
    }
    
    for(i=StartIndex; i < (*pLen); i++)
    {
        if(*(*pCmd + i) == ',')
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if((EndIndex-StartIndex) > 0)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=StartIndex; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex - StartIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 1);
            *pLen -= (EndIndex + 1);
        }
    }
    return(bRtn);
}

uint32 YX_Func_IntToHexChar_Array(uint8 *buf,uint32 len,uint8 * dest_buf,uint32 dest_len)
{
    uint8 low = 0;
    uint8 high = 0;
    uint32 i = 0;
    uint32 j = 0;

    if((len*2) > dest_len)
    {
        len = dest_len/2;
    }
    for(i = 0; i < len; i++)
    {
        high = buf[i]>>4;
        dest_buf[j++] = (char)YX_Func_IntToHexChar(high);
        low = buf[i] & 0x0F;
        dest_buf[j++] = (char)YX_Func_IntToHexChar(low);
    }
    dest_buf[j] = 0;
    return j;
}

int  YX_Func_ClockGetNextPara(uint8** pCmd, uint32* pLen, char* pStr, uint16 wMaxStrLen)
{
    int  bRtn = 0, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    
    if(*pLen == 0)
    {
        return -1;
    }
    
    for(i=0; i < (*pLen); i++)
    {
        if(*(*pCmd + i) == ':' || *(*pCmd + i) == '-' )
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if(EndIndex)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=0; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 1);
            *pLen -= (EndIndex + 1);
        }
    }
    return(bRtn);
}

int  YX_Func_TimerGetNextPara(uint8** pCmd, uint32* pLen, char* pStr, uint16 wMaxStrLen)
{
    int  bRtn = 0, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    
    if(*pLen == 0)
    {
        return -1;
    }
    
    for(i=0; i < (*pLen); i++)
    {
        if(*(*pCmd + i) == ':' || *(*pCmd + i) == '-')
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if(EndIndex)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=0; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 1);
            *pLen -= (EndIndex + 1);
        }
    }
    return(bRtn);
}

int  YX_Func_UnicodeGetNextPara(uint8** pCmd, uint32* pLen, char* pStr, uint16 wMaxStrLen)
{
    int  bRtn = 0, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    
    if(*pLen == 0)
    {
        return -1;
    }
    
    for(i=0; i < (*pLen); i++)
    {
        if((*(*pCmd + i) == '0')&&(*(*pCmd + i+1) == '0')&&(*(*pCmd + i+2) == '2')&&(*(*pCmd + i+3) == 'D') )
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if(EndIndex)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=0; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 4);
            *pLen -= (EndIndex + 4);
        }
    }
    return(bRtn);
}


int  YX_Func_TimerUnicodeGetNextPara(uint8** pCmd, uint32* pLen, char* pStr, uint16 wMaxStrLen)
{
    int  bRtn = 0, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    
    if(*pLen == 0)
    {
        return -1;
    }
    
    for(i=0; i < (*pLen); i++)
    {
        if((*(*pCmd + i) == '0')&&(*(*pCmd + i+1) == '0')&&(*(*pCmd + i+2) == '5')&&(*(*pCmd + i+3) == 'F') )
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
        
    EndIndex = i;

    if(EndIndex)
    {
        if(!wMaxStrLen)  // No limit if NULL
            wMaxStrLen = 100;
        for(i=0; i<EndIndex; i++)
        {
            if(wMaxStrLen)
            {
                *(pStr + L) = *(*pCmd + i);
                L ++;
                wMaxStrLen --;
            }
        }
        
        if(wMaxStrLen > 0)
        {
            *(pStr + L) = 0;
        }
        bRtn = EndIndex;
    }
    else
    {   // Nothing or empty
        bRtn = 0;
    }

    // Move command pointer to correct position and change the remainder length
    if(bRtn || bHaveNextComma)  // It does get something in command string
    {
        if(EndIndex == *pLen)   // Already to the end of string
        {
            *pCmd += EndIndex;
            *pLen = 0;
        }
        else                    // There is still something in string
        {
            *pCmd += (EndIndex + 4);
            *pLen -= (EndIndex + 4);
        }
    }
    return(bRtn);
}

//07000800 7：00-8:00
void insert(char * s1,char * s2,int n)
{
int i = 0;
int k = 0;
int j = 0;
int m = 0;

char s3[100] = {0};
if(n < 0 || s1 == NULL || s2 == NULL)//判定
{
//printf("error");
return;
}

for (j = 0; s1[j] != ' ' && s1[j] != '\0'; j++);//得出传进来的s1的长度j
for (k = 0; s2[k] != ' ' && s2[k] != '\0'; k++);//得出传进来的s2的长度k
if(n > j)//当然，第n个字符的位置不可能大于s1的长度j
{
	//printf("error");
	return;
}
else
{
	for (i = n, m = 0;i < j; i++, m++)//这里是将s1 n个字符后的字符赋给s3
	{
	*(s3+m) = *(s1 + i);
	}

	for (i = n,m = 0; m < k; i++, m++)//之后放心的将s2的字符串添加进来
	{
	*(s1 + i) = *(s2+m);
	}

	for (i = n + k, m = 0; i < j + k; i++, m++)
	{
	*(s1+i) = *(s3+m);
	}

	*(s1 + k + j) = ' ';//空格结束字符串
	}

}
uint8 YX_Func_GetDate(uint8 * pData)
{
    SCI_DATE_T          date;
    char tem_buf[50] = {0};
    uint16 len = 0; 

    TM_GetSysDate(&date);
    
    sprintf(tem_buf,"%02d%02d%02d,",(date.year%2000),date.mon,date.mday);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    return len;
}

uint8 YX_Func_GetTime(uint8 * pData)
{
    SCI_TIME_T          time;
    char tem_buf[50] = {0};
    uint16 len = 0; 

    TM_GetSysTime(&time);
    
    sprintf(tem_buf,"%02d%02d%02d,",time.hour,time.min,time.sec);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    return len;
}

uint8 YX_Func_GetDateTime(uint8 * pData)
{
    SCI_TIME_T          time;
    SCI_DATE_T          date;
    char tem_buf[50] = {0};
    uint16 len = 0; 

    TM_GetSysDate(&date);
    TM_GetSysTime(&time);
    
    sprintf(tem_buf,"%02d%02d%02d,%02d%02d%02d,",(date.year%2000),date.mon,date.mday,time.hour,time.min,time.sec);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    return len;
}

uint8 YX_Func_GetDateTimeEx(uint8 * pData)
{
    SCI_TIME_T          time;
    SCI_DATE_T          date;
    char tem_buf[50] = {0};
    uint16 len = 0; 

    TM_GetSysDate(&date);
    TM_GetSysTime(&time);
    
    sprintf(tem_buf,"%02d%02d%02d%02d%02d%02d,",(date.year%2000),date.mon,date.mday,time.hour,time.min,time.sec);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    return len;
}
uint8 YX_Func_GetTimeDateTime(uint8 * pData)
{
    SCI_TIME_T          time;
    SCI_DATE_T          date;
    TM_GetSysDate(&date);
    TM_GetSysTime(&time);
    
    YX_Func_IntToHexChar_One((uint8)time.hour,pData);
    YX_Func_IntToHexChar_One((uint8)time.min,pData+2);
    YX_Func_IntToHexChar_One((uint8)(time.sec),pData+4);
    pData[6] = ',';
    
    YX_Func_IntToHexChar_One((uint8)date.mday,pData+7);
    YX_Func_IntToHexChar_One((uint8)date.mon,pData+9);
    YX_Func_IntToHexChar_One((uint8)(date.year%2000),pData+11);
    pData[13] = ',';
    
    YX_Func_IntToHexChar_One((uint8)time.hour,pData+14);
    YX_Func_IntToHexChar_One((uint8)time.min,pData+16);
    YX_Func_IntToHexChar_One((uint8)time.sec,pData+18);
    pData[20] = ',';
    return 21;
}

uint8 YX_Func_GetBatPercent(uint8 * pData)
{
    CHGMNG_STATE_INFO_T* p_chgmng_info = CHGMNG_GetModuleState();
    uint8 percent = 0;
    char tem_buf[20] = {0};
    uint16 len = 0; 
    if(p_chgmng_info != PNULL)
    {
        percent = p_chgmng_info->bat_remain_cap;
    }
    if(p_chgmng_info->charging_stop_reason == CHGMNG_CHARGEDONE)//yangyu add
    {
        percent = 100;
    }
    
    sprintf(tem_buf,"%d",percent);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint8 YX_Func_GetRssiPercent(uint8 * pData)
{
    uint8	i = 5;
    uint16 rssi = g_zdt_s_station_data.rssiLevel;
    uint8 percent = 0;
    char tem_buf[20] = {0};
    uint16 len = 0; 
    //设置信强度,根据参考手机sagem得到的数据，将idle界面下的信号强度划分成5级
    // 1：1-7； 2：8-11； 3：12-15； 4：16-19； 5：20-63
    if (20 <= rssi)
    {
        i = 5;
    }
    else if (16 <= rssi)
    {
        i = 4;
    }
    else if (12 <= rssi)
    {
        i = 3;
    }
    else if (8 <= rssi)
    {
        i = 2;
    }
    else if (1 <= rssi)
    {
        i = 1;
    }
    else
    {
        i = 0;
    }
    percent = i *20;
    if(percent > 100)
    {
        percent = 100;
    }
    sprintf(tem_buf,"%d",percent);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint8 YX_Func_GetJPNum(uint8 * pData)
{
    uint32 jp_sum = 0;
    char tem_buf[20] = {0};
    uint16 len = 0; 
#ifdef ZDT_GSENSOR_SUPPORT
    #ifndef WIN32
    jp_sum = yx_DB_Set_Rec.alert_jp ? ZDT_GSensor_GetStepOneDay() : 0;//ZDT_GSensor_GetStepOneDay();//ZDT_GSensor_GetStep();//YANGYU
    #endif
#endif
    sprintf(tem_buf,"%d",jp_sum);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint8 YX_Func_GetFGNum(uint8 * pData)
{
    uint32 fg_sum = 0;
    char tem_buf[20] = {0};
    uint16 len = 0; 
    sprintf(tem_buf,"%d",fg_sum);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint8 YX_Func_GetStatus(uint8 * pData,uint16 type)
{
    char tmp_buf[20] = {0};
    uint16 alert = 0;
    uint16 status = 0;
    if(ZDT_GetIsBatLow())
    {
        status |= 0x0001;
    }
    #ifdef FEATURE_ALSENSOR_SUPPORT
    if(alsensor_is_far())
    {
        status |= 0x0008;
    }
    #endif
    #ifdef ZDT_GSENSOR_SUPPORT
    if(ZDT_GSensor_GetStill())
    {
        status |= 0x0010;
    }
    #endif
    if(type == YX_NET_ALERT_TYPE_SOS)
    {
        alert |= 0x0001;
    }
    if(type == YX_NET_ALERT_TYPE_LOWBAT && !ZDT_IsInCharging())
    {
        alert |= 0x0002;
    }
    if(type == YX_NET_ALERT_TYPE_LOSE)
    {
        alert |= 0x0010;
    }
    if(type == YX_NET_ALERT_TYPE_FALLDOWN)
    {
        alert |= 0x0020;
    }
    sprintf(tmp_buf,"%04X%04X,",alert,status);
    SCI_MEMCPY(pData,tmp_buf,9);
    return 9;
}

uint16 YX_Func_GetWIFIInfo(uint8 * pData,uint8 loc_ok)
{
    uint16 len = 0;
#ifdef ZDT_WIFI_SUPPORT
    uint8 wifi_num = 0;
    ZDT_WIFI_DATA_T last_wifi = {0};
    char temp_buf[500] = {0};
    char item_buf[100] = {0};
    uint16 i =0;
    //WIFI
    if(loc_ok & 0x02)
    {
        wifi_num = ZDT_WIFI_Get_DataNum();
        if(wifi_num > 5)
        {
            wifi_num = 5;
        }
    }
    else
    {
        wifi_num = 0;
    }
    if(wifi_num > 0)
    {
        for(i = 0; i < wifi_num; i++)
        {
            ZDT_WIFI_Get_Data(i,&last_wifi);
            if(last_wifi.rssi < 0)
            {
                last_wifi.rssi = 0 - last_wifi.rssi;
            }
            if(i == 0)
            {
                sprintf(temp_buf,"%d,%s,%02x:%02x:%02x:%02x:%02x:%02x,%d",wifi_num,last_wifi.ssid,last_wifi.bssid[0],last_wifi.bssid[1],last_wifi.bssid[2],last_wifi.bssid[3],last_wifi.bssid[4],last_wifi.bssid[5],last_wifi.rssi);
            }
            else
            {
                memset(item_buf,0,100);
                sprintf(item_buf,",%s,%02x:%02x:%02x:%02x:%02x:%02x,%d",last_wifi.ssid,last_wifi.bssid[0],last_wifi.bssid[1],last_wifi.bssid[2],last_wifi.bssid[3],last_wifi.bssid[4],last_wifi.bssid[5],last_wifi.rssi);
                strcat(temp_buf,item_buf);
            }
        }
        len = strlen(temp_buf);
        memcpy(pData,temp_buf,len);
    }
    else
#endif
    {
        pData[0] = '0';
        len = 1;
    }
    return len;
}

#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE
uint8 YX_Func_GetGPSLat(uint8 * pData,float Latitude,uint32 Lat_m,uint8 Lat_Dir)
{
    uint32 d = 0;
    uint32 d_m = 0;
    double tmp = Lat_m/0.6;
    d_m = tmp;
    d = Latitude/180000;
    //sprintf((char *)pData,"%02ld.%06ld",d,d_m);
    sprintf((char *)pData,"%.6f",Latitude);
    
    pData[9] = ',';
    if(Lat_Dir == 'N') //N-S 北纬
    {
        pData[10] = 'N';
    }
    else
    {
        pData[10] = 'S';
    }
    ////pData[10] = 'N'; // wuxx
    
    pData[11] = ',';
    return 12;
}

uint8 YX_Func_GetGPSLong(uint8 * pData,float Longitude,uint8 long_Dir)
{
    double tmp = 0;
    uint32 d = 0;
    uint32 d_m = 0;
    d = Longitude/180000;
    tmp = (Longitude - d*180000)/0.18;
    d_m = tmp;
    //sprintf((char *)pData,"%03ld.%06ld",d,d_m);
    sprintf((char *)pData,"%.6f",Longitude);
    
    pData[10] = ',';
    if(long_Dir == 'W') //N-S 北纬
    {
        pData[11] = 'W';
    }
    else
    {
        pData[11] = 'E';
    }
    ////pData[11] = 'E';// wuxx
    
    pData[12] = ',';
    return 13;
}
#else
uint8 YX_Func_GetGPSLat(uint8 * pData,uint32 Latitude,uint32 Lat_m,uint8 Lat_Dir)
{
    uint32 d = 0;
    uint32 d_m = 0;
    double tmp = Lat_m/0.6;
    d_m = tmp;
    d = Latitude/180000;
    sprintf((char *)pData,"%02ld.%06ld",d,d_m);
    pData[9] = ',';
    if(Lat_Dir == 'N') //N-S 北纬
    {
        pData[10] = 'N';
    }
    else
    {
        pData[10] = 'S';
    }
    pData[11] = ',';
    return 12;
}

uint8 YX_Func_GetGPSLong(uint8 * pData,uint32 Longitude,uint8 long_Dir)
{
    double tmp = 0;
    uint32 d = 0;
    uint32 d_m = 0;
    d = Longitude/180000;
    tmp = (Longitude - d*180000)/0.18;
    d_m = tmp;
    sprintf((char *)pData,"%03ld.%06ld",d,d_m);
    pData[10] = ',';
    if(long_Dir == 'W') //N-S 北纬
    {
        pData[11] = 'W';
    }
    else
    {
        pData[11] = 'E';
    }
    pData[12] = ',';
    return 13;
}
#endif

uint8 YX_Func_GetGPSSateNum(uint8 * pData,uint8 sate_num)
{
    char tem_buf[20] = {0};
    uint16 len = 0; 
    sprintf(tem_buf,"%d",sate_num);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint8 YX_Func_GetGPSSpeed(uint8 * pData,uint32 speed)
{
    char tem_buf[20] = {0};
    uint32 sp_kilo = 0;
    uint32 sp_kilo_d = 0;
    uint16 len = 0; 
    sp_kilo = speed/1000;
    sp_kilo_d = (speed - sp_kilo*1000)/10;
    sprintf(tem_buf,"%ld.%02ld",sp_kilo,sp_kilo_d);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint8 YX_Func_GetGPSCog(uint8 * pData,uint16 cog)
{
    char tem_buf[20] = {0};
    uint16 cur_cog = cog/10;
    uint16 len = 0; 
    sprintf(tem_buf,"%d",cur_cog);
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint8 YX_Func_GetGPSAlt(uint8 * pData,uint32 alt, uint8 alt_dir)
{
    char tem_buf[20] = {0};
    uint32 cur_alt = alt/10;
    uint16 len = 0;
    if(alt_dir > 0)
    {
        sprintf(tem_buf,"%d",cur_alt);
    }
    else
    {
        sprintf(tem_buf,"-%d",cur_alt);
    }
    len = SCI_STRLEN(tem_buf);
    SCI_MEMCPY(pData,tem_buf,len);
    pData[len++] = ',';
    return len;
}

uint16 YX_Func_GetSSInfo(uint8 * pData)
{
    char tem_buf[200] = {0};
    char item_buf[100] = {0};
    uint16 len = 0;
    uint16 i = 0;
    uint16 j = 0;
    int rssi_offset = 156;
    int nbr_count = 0;
    MMI_GMMREG_RAT_E	network_status_rat = MMI_GMMREG_RAT_GPRS;
    network_status_rat = MMIAPIPHONE_GetTDOrGsm(MN_DUAL_SYS_1);
    switch (network_status_rat)
    {
        case MMI_GMMREG_RAT_GPRS:
            rssi_offset = 156;
            break;
        case MMI_GMMREG_RAT_3G:
            rssi_offset = 156;
            break;
        case MMI_GMMREG_RAT_LTE:
            rssi_offset = 141;
            break;
        case MMI_GMMREG_RAT_UNKNOWN:
        default:
            rssi_offset = 156;
            break;
    }

    MMIZDT_Station_Update();
    for (i = 0; i < ZDT_N_STATION_NUM; i++)		// this is for example for simulator
    {
        if(g_zdt_station_data.n_cell[i].is_valid)
        {
            j++;
        }
    }
    nbr_count = j;
    
    ZDT_LOG("YX_Func_GetSSInfo rssiLevel=%d, mcc=%d,,mnc=%d,lac=%d,cell_id=%d",\
    g_zdt_station_data.s_cell.rssiLevel,g_zdt_station_data.s_cell.mcc,g_zdt_station_data.s_cell.mnc,g_zdt_station_data.s_cell.lac,g_zdt_station_data.s_cell.cell_id);

    if(g_zdt_station_data.s_cell.is_valid)
    {
        sprintf(tem_buf,"%d,0,%d,%d,%d,%d,%d",nbr_count+1, \
            g_zdt_station_data.s_cell.mcc,g_zdt_station_data.s_cell.mnc,g_zdt_station_data.s_cell.lac,g_zdt_station_data.s_cell.cell_id,g_zdt_station_data.s_cell.rssiLevel-rssi_offset);
        
        for (i = 0; i < ZDT_N_STATION_NUM; i++)		// this is for example for simulator
        {
            if(g_zdt_station_data.n_cell[i].is_valid)
            {
                SCI_MEMSET(item_buf,0,100);
                sprintf(item_buf,",%d,%d,%d",g_zdt_station_data.s_cell.lac,g_zdt_station_data.n_cell[i].cell_id,g_zdt_station_data.n_cell[i].rssiLevel-rssi_offset);
                strcat(tem_buf,item_buf);
            }
        }
    }
    else
    {
        sprintf(tem_buf,"0");
    }
    len = SCI_STRLEN(tem_buf);
    tem_buf[len++] = ',';
    memcpy(pData,tem_buf,len);
    return len;
}


// 字符全部转换为大写
char* YX_Func_strupr(char* src)
{
	while (*src != '\0')
	{
		if (*src >= 'a' && *src <= 'z')
			//在ASCII表里大写字符的值比对应小写字符的值小32.
			//*p -= 0x20; // 0x20的十进制就是32
			*src -= 32;
		src++;
	}
	return src;
}

// 字符全部转换为小写
char* YX_Func_strlwr(char* src)
{
	while (*src != '\0')
	{
		if (*src > 'A' && *src <= 'Z'){
			//*src += 0x20; 
			*src += 32;
		}
		src++;
	}
	return src;
}

uint16 YX_Func_GetFileName(char *pFullPath,char *pFileName)
{
    uint16 len = 0;
    char* p;
    
    if(pFullPath == NULL || pFileName == NULL)
    {
        return 0;
    }
    
    len = SCI_STRLEN(pFullPath);
    if(len == 0)
    {
        return 0;
    }
    
    
    p = strrchr(pFullPath, '\\');
    if(p == NULL)
    {
        return 0;
    }
    SCI_STRCPY(pFileName,p+1);
    len = strlen(pFileName);
    return len;
}

#endif

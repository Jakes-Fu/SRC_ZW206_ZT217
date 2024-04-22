/****************************************************************************
** File Name:      mmicdma_app.c                                           *
** Author:          jianshengqi                                                       *
** Date:           03/22/2006                                              *
** Copyright:      2006 tlt, Incoporated. All Rights Reserved.       *
** Description:    This file is used to describe the PHS                   *
/****************************************************************************
**                         Important Edit History                          *
** ------------------------------------------------------------------------*
** DATE           NAME             DESCRIPTION                             *
** 03/2006       Jianshq         Create
** 
****************************************************************************/
#ifndef  _MMI_ZYB_APP_C_
#define _MMI_ZYB_APP_C_

/**--------------------------------------------------------------------------*
**                         Include Files                                    *
**--------------------------------------------------------------------------*/

#include "zyb_app.h"
MMI_APPLICATION_T   		g_zyb_app;
static BOOLEAN s_zyb_app_init = FALSE;

static MMI_RESULT_E  HandleZYBAppMsg (PWND app_ptr, 
								 uint16 msg_id, 
								 DPARAM param)
{
    MMI_RESULT_E res = MMI_RESULT_TRUE;
    if (HTTP_SIGNAL_BEG <= msg_id && msg_id <= HTTP_SIGNAL_END)
    {
        res = Zyb_HandleHttpMsg(app_ptr,msg_id,param);
        return res;
    }
    
    if(MMI_RESULT_TRUE == ZYBHTTP_Net_Handle_AppMsg(app_ptr,msg_id,param))
    {
        return MMI_RESULT_TRUE;
    }

    switch(msg_id)
    {                       
        case ZYBHTTP_APP_SIGNAL_HTTP_SEND:
                MMIZYB_HTTP_Handle_Send(param);
            break;
            
        case ZYBHTTP_APP_SIGNAL_HTTP_RESEND:
                MMIZYB_HTTP_Handle_ReSend(param);
            break;
            
        case ZYBHTTP_APP_SIGNAL_HTTP_RCV:
                MMIZYB_HTTP_Handle_Rcv(param);
            break;
            
        case ZYBHTTP_APP_SIGNAL_HTTP_REFRESH:
                MMIZYB_HTTP_Handle_RefreshInd(param);
            break;
            
         case ZYBHTTP_APP_SIGNAL_HTTP_SEND_OVER:
                MMIZYB_HTTP_Handle_SendOver(param);
            break;
            
         case ZYBHTTP_APP_SIGNAL_HTTP_AUDIOGET:
            {
                MMI_ZYB_SIG_T * pp_getdata = (MMI_ZYB_SIG_T *)param;
                MMI_ZYB_DATA_T * p_getdata = &(pp_getdata->data);
                if(p_getdata != PNULL)
                {
                    MMIZYB_HTTP_Handle_AudioGet(p_getdata->len);
                }
                
                if(p_getdata->len > 0 && p_getdata->str != PNULL)
                {
                   SCI_FREE(p_getdata->str);
                }
            }
            break;

         case ZYBHTTP_APP_SIGNAL_HTTP_AUDIOSTOP:
            {
                MMIZYB_HTTP_Handle_AudioStop();
            }
            break;
            
         case ZYBHTTP_APP_SIGNAL_HTTP_AUDIOSTOPDELAY:
            {
                MMIZYB_HTTP_Handle_AudioStopDelay();
             }
            break;
            
         case ZYBHTTP_APP_SIGNAL_HTTP_AUDIOPLAY:
                MMIZYB_HTTP_Handle_AudioPlay();
            break;
            
        default:
                res = MMI_RESULT_FALSE;
            break;
    }
    return res;
}

void MMIZYB_AppInit(void)
{
    g_zyb_app.ProcessMsg     = HandleZYBAppMsg;
    s_zyb_app_init = TRUE;
}
BOOLEAN MMIZYB_AppIsInit(void)
{
    return s_zyb_app_init;
}

void MMIZYB_InitModule(void)
{
    MMIZYB_RegMenuGroup();
    MMIZYB_RegNv();
    MMIZYB_RegWinIdNameArr();
}

BOOLEAN  MMIZYB_SendSigTo_APP(ZYB_APP_SIG_E sig_id, uint8 * data_ptr, uint32 data_len)
{
    uint8 * pstr = NULL;
    MMI_ZYB_SIG_T * psig = PNULL;
    if(s_zyb_app_init == FALSE)
    {
        return FALSE;
    }
    if(data_ptr != PNULL && data_len != 0)
    {
        pstr = SCI_ALLOCA(data_len);//free it in AT task
        if (pstr == PNULL)
        {
                SCI_PASSERT(0, ("MMIZYB_SendSigTo_APP Alloc  %ld FAIL",data_len));
                return FALSE;
        }
        SCI_MEMCPY(pstr,data_ptr,data_len);
    }
    
    //send signal to AT to write uart
    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,sig_id,sizeof(MMI_ZYB_SIG_T),SCI_IdentifyThread());
    psig->data.len = data_len;
    psig->data.str = pstr;
		
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig,P_APP);
    return TRUE;
    
}

static uint8 Nibble2HexChar(uint8 bNibble)
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

static uint32 U8_To_Hex(uint8 *buf,uint32 len,uint8 * dest_buf,uint32 dest_len)
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
        dest_buf[j++] = (char)Nibble2HexChar(high);
        low = buf[i] & 0x0F;
        dest_buf[j++] = (char)Nibble2HexChar(low);
    }
    dest_buf[j] = 0;
    return j;
}

PUBLIC void ZYB_HTTPSendTrace(const char *string, int len)
{
#ifndef RELEASE_INFO
    char tmp_str[128] = {0};
    if (len < 128)
    {
        SCI_MEMCPY(tmp_str,string,len);
        SCI_TRACE_LOW("[ZYB][HTTP] Send:%s",tmp_str);
    }
    else
    {
        char *p;
        char buff[128];

        p = string;
        SCI_MEMSET(buff,0,sizeof(buff));
        while (len > 0)
        {
           int l = len < 127 ? len : 127;
           SCI_MEMCPY(buff, p, l);
          buff[l] = 0;
        
           SCI_TRACE_LOW("[ZYB][HTTP] Send:%s",buff);
        
          p += l;
          len -= l;
        }
    }
#endif
}

PUBLIC void ZYB_HTTPSendTraceU8(const char *str, int len)
{
#ifndef RELEASE_INFO
    uint16 i = 0;
    char dest_buff[130] = {0};
    if(str == PNULL || len == 0)
    {
        return;
    }
    if (len < 64)
    {
        U8_To_Hex(str,len,dest_buff,128);
        SCI_TRACE_LOW("[ZYB][HTTP] Send:%s",dest_buff);
    }
    else
    {
        char *p;
        p = str;
        while (len > 0)
        {
           int l = len < 63 ? len : 63;
           SCI_MEMSET(dest_buff,0,sizeof(dest_buff));
           U8_To_Hex(p,l,dest_buff,128);
           i++;
           SCI_TRACE_LOW("[ZYB][HTTP] Send_%d:%s",i,dest_buff);
            p += l;
            len -= l;
        }
    }
#endif
}

PUBLIC void ZYB_HTTPRcvTrace(const char *string, int len)
{
#ifndef RELEASE_INFO
    char tmp_str[128] = {0};
    if (len < 128)
    {
        SCI_MEMCPY(tmp_str,string,len);
        SCI_TRACE_LOW("[ZYB][HTTP] Rcv:%s",tmp_str);
    }
    else
    {
        char *p;
        char buff[128];

        p = string;
        SCI_MEMSET(buff,0,sizeof(buff));
        while (len > 0)
        {
           int l = len < 127 ? len : 127;
           SCI_MEMCPY(buff, p, l);
          buff[l] = 0;
        
           SCI_TRACE_LOW("[ZYB][HTTP] Rcv:%s",buff);
        
          p += l;
          len -= l;
        }
    }
#endif
}

PUBLIC void ZYB_HTTPRcvTraceU8(const char *str, int len)
{
#ifndef RELEASE_INFO
    uint16 i = 0;
    char dest_buff[130] = {0};
    if(str == PNULL || len == 0)
    {
        return;
    }
    if (len < 64)
    {
        U8_To_Hex(str,len,dest_buff,128);
        SCI_TRACE_LOW("[ZYB][HTTP] Rcv:%s",dest_buff);
    }
    else
    {
        char *p;

        p = str;
        while (len > 0)
        {
           int l = len < 63 ? len : 63;
           SCI_MEMSET(dest_buff,0,sizeof(dest_buff));
           U8_To_Hex(p,l,dest_buff,128);
           i++;
           SCI_TRACE_LOW("[ZYB][HTTP] Rcv_%d:%s",i,dest_buff);
            p += l;
            len -= l;
        }
    }
#endif
}

int  ZYB_Trace_CR_GetNextPara(uint8** pCmd, uint32* pLen, char* pStr, uint16 wMaxStrLen)
{
    int  bRtn = 0, bHaveNextComma = FALSE;
    int    i, L = 0, EndIndex = 0;
    int  is_cr = FALSE;
    if(*pLen == 0)
    {
        return -1;
    }
    
    for(i=0; i < (*pLen); i++)
    {
        if(*(*pCmd + i) == 0x0D || *(*pCmd + i) == 0x0A)
        {
            bHaveNextComma = TRUE;
            break;
        }
    }
    
    if( (i+1) < *pLen && bHaveNextComma && *(*pCmd + i) == 0x0D && *(*pCmd + i + 1) == 0x0A)
    {
        is_cr = TRUE;
    }
    EndIndex = i;

    if(EndIndex)
    {
        if(!wMaxStrLen)
        {
            wMaxStrLen = 100;
        }
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
            if(is_cr)
            {
                *pCmd += (EndIndex + 2);
                *pLen -= (EndIndex + 2);
            }
            else
            {
                *pCmd += (EndIndex + 1);
                *pLen -= (EndIndex + 1);
            }
        }
    }
    return(bRtn);
}

PUBLIC void ZYB_HTTPTraceCR(const char *data_str, int data_len)
{
#ifndef RELEASE_INFO
    int ret = 0;
    uint8 * pTime = NULL;
    uint32 con_len = 0;
    char tmp_str[128] = {0};
    
    if(data_str == NULL || data_len == 0)
    {
        return;
    }
    
    pTime = (uint8 *)data_str;
    con_len = data_len;
    while(ret != -1)
    {
        ret = ZYB_Trace_CR_GetNextPara(&pTime,&con_len,tmp_str,127);
        if(ret > 0)
        {
            ZYB_HTTPRcvTrace(tmp_str,ret);
        }
    }

    return;
#endif
}

PUBLIC void ZYB_HTTP_TraceExt(const char *string, int len)
{
#ifndef RELEASE_INFO
    char tmp_str[128] = {0};
    if (len < 128)
    {
        SCI_MEMCPY(tmp_str,string,len);
        SCI_TRACE_LOW("[ZYB][HTTP]:%s",tmp_str);
    }
    else
    {
        char *p;
        char buff[128];

        p = string;
        SCI_MEMSET(buff,0,sizeof(buff));
        while (len > 0)
        {
           int l = len < 127 ? len : 127;
           SCI_MEMCPY(buff, p, l);
          buff[l] = 0;
          SCI_TRACE_LOW("[ZYB][HTTP]:%s",buff);
          p += l;
          len -= l;
        }
    }
#endif
}

PUBLIC void ZYB_STREAM_TraceExt(const char *string, int len)
{
#ifndef RELEASE_INFO
    char tmp_str[128] = {0};
    if (len < 128)
    {
        SCI_MEMCPY(tmp_str,string,len);
        SCI_TRACE_LOW("[ZYB][STREAM]:%s",tmp_str);
    }
    else
    {
        char *p;
        char buff[128];

        p = string;
        SCI_MEMSET(buff,0,sizeof(buff));
        while (len > 0)
        {
           int l = len < 127 ? len : 127;
           SCI_MEMCPY(buff, p, l);
          buff[l] = 0;
          SCI_TRACE_LOW("[ZYB][STREAM]:%s",buff);
          p += l;
          len -= l;
        }
    }
#endif
}

void ZYB_STREAM_Trace(const char *pszFmt, ...)
{
#ifndef RELEASE_INFO
    char buf[1024] = {0};
    int tmp_len = 0;
    va_list va_args;    
    va_start(va_args, pszFmt);
#ifdef WIN32
    tmp_len = vsprintf(buf,pszFmt, va_args);
#else
    tmp_len = vsnprintf(buf,1023,pszFmt, va_args);
#endif
    if (tmp_len >= 0)
    {
        ZYB_STREAM_TraceExt(buf,tmp_len);
    }
    va_end(va_args);
#endif
}
void ZYB_HTTP_Trace(const char *pszFmt, ...)
{
#ifndef RELEASE_INFO
    char buf[1024] = {0};
    int tmp_len = 0;
    va_list va_args;    
    va_start(va_args, pszFmt);
#ifdef WIN32
    tmp_len = vsprintf(buf,pszFmt, va_args);
#else
    tmp_len = vsnprintf(buf,1023,pszFmt, va_args);
#endif
    if (tmp_len >= 0)
    {
        ZYB_HTTP_TraceExt(buf,tmp_len);
    }
    va_end(va_args);
#endif
}

PUBLIC void ZYB_TraceExt(const char *string, int len)
{
#ifndef RELEASE_INFO
    char tmp_str[128] = {0};
    if (len < 128)
    {
        SCI_MEMCPY(tmp_str,string,len);
        SCI_TRACE_LOW("[ZYB]:%s",tmp_str);
    }
    else
    {
        char *p;
        char buff[128];

        p = string;
        SCI_MEMSET(buff,0,sizeof(buff));
        while (len > 0)
        {
           int l = len < 127 ? len : 127;
           SCI_MEMCPY(buff, p, l);
          buff[l] = 0;
          SCI_TRACE_LOW("[ZYB]:%s",buff);
          p += l;
          len -= l;
        }
    }
#endif
}

void ZYB_Trace(const char *pszFmt, ...)
{
#ifndef RELEASE_INFO
    char buf[1024] = {0};
    int tmp_len = 0;
    va_list va_args;    
    va_start(va_args, pszFmt);
#ifdef WIN32
    tmp_len = vsprintf(buf,pszFmt, va_args);
#else
    tmp_len = vsnprintf(buf,1023,pszFmt, va_args);
#endif
    if (tmp_len >= 0)
    {
        ZYB_TraceExt(buf,tmp_len);
    }
    va_end(va_args);
#endif
}

#endif

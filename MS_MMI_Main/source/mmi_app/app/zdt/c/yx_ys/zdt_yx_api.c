#include "zdt_app.h"

#ifdef ZDT_PLAT_YX_SUPPORT
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129

#define YX_LOCTION_ON_TIME_S 30
#define YX_LOCTION_FIRST_ON_TIME_S 60

static BOOLEAN s_yx_loction_is_first_on = TRUE;

#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE_20231204
BOOLEAN s_yx_loction_is_first_on_for_gps = TRUE;
#define YX_LOCTION_FIRST_ON_FOR_GPS_TIME_S 60// 120 // 180--120

BOOLEAN g_is_gps_location_type = FALSE ;

//是GPS 定位模式
BOOLEAN zdt_is_use_gps_location_type(void)
{
     return g_is_gps_location_type;
}

void zdt_set_is_gps_location_type(uint16 location_type)
{
    //if ((location_type == YX_LOCTION_TYPE_NET)||(location_type == YX_LOCTION_TYPE_ALERT)||(location_type == YX_LOCTION_TYPE_SMS))
    if ((location_type == YX_LOCTION_TYPE_GPS)||(location_type == YX_LOCTION_TYPE_GPS_ALERT))
    {
        g_is_gps_location_type = TRUE;
    }
    else
    {
        g_is_gps_location_type = FALSE;
    }
    
}

#endif

static BOOLEAN YX_LOCTION_Link_AddData(YX_APP_T *pMe,uint16 type,uint8 * pParam,uint16 ParamLen)
{
    YX_LOCTION_LINK_NODE_T  * p1 = NULL;
    YX_LOCTION_LINK_NODE_T  * p2 = NULL;
    uint16 len = ParamLen;
    
    p1=(YX_LOCTION_LINK_NODE_T *)SCI_ALLOC_APPZ(sizeof(YX_LOCTION_LINK_NODE_T));
    if(p1 == NULL)
    {
        return FALSE;
    }
    
    SCI_MEMSET(p1,0,sizeof(YX_LOCTION_LINK_NODE_T));

    if(len > 0 && pParam != NULL)
    {
        p1->data.pParam = (uint8 *)SCI_ALLOC_APPZ(len+1);
        if(p1->data.pParam == NULL)
        {
            SCI_FREE(p1);
            return FALSE;
        }
        SCI_MEMCPY(p1->data.pParam,pParam,len);
        p1->data.pParam[len] = 0;
    }
    p1->data.len = len;
    p1->data.type = type;
    
    p1->next_ptr = NULL;

    if(pMe->m_yx_loction_link_head == NULL)
    {
        pMe->m_yx_loction_link_head = p1;
    }
    else
    {
        p2 = pMe->m_yx_loction_link_head;
        while(p2->next_ptr != NULL)
        {
            p2 = p2->next_ptr;
        }
        p2->next_ptr = p1;
    }
    
    return TRUE;
}

static BOOLEAN  YX_LOCTION_Link_Get(YX_APP_T *pMe,YX_LOCTION_LINK_DATA_T * pData)
{
    YX_LOCTION_LINK_NODE_T  * p1 = NULL;
    
    p1 = pMe->m_yx_loction_link_head;
    
    if(p1 != NULL)
    {
      *pData = p1->data;
        pMe->m_yx_loction_link_head = p1->next_ptr;
        SCI_FREE(p1);
        return TRUE;
    }
        
    return FALSE;
}

static BOOLEAN  YX_LOCTION_Link_TypeExsit(YX_APP_T *pMe,uint16 type)
{
    YX_LOCTION_LINK_NODE_T  * p1 = NULL;
    YX_LOCTION_LINK_NODE_T  * p2 = NULL;
    BOOLEAN res = FALSE;
    
    p2 = p1 = pMe->m_yx_loction_link_head;
    
    while(p1 != NULL)
    {
        if(p1->data.type == type)
        {
            res = TRUE;
            break;
        }
        p2 = p1;
        p1 = p2->next_ptr;
    }
    return res;
}

static BOOLEAN  YX_LOCTION_Link_GetByType(YX_APP_T *pMe,YX_LOCTION_LINK_DATA_T * pData,uint16 type)
{
    YX_LOCTION_LINK_NODE_T  * p1 = NULL;
    YX_LOCTION_LINK_NODE_T  * p2 = NULL;
    BOOLEAN res = FALSE;
    
    p2 = p1 = pMe->m_yx_loction_link_head;
    
    while(p1 != NULL)
    {
        if(p1->data.type == type)
        {
            *pData = p1->data;
            if(p1 == pMe->m_yx_loction_link_head)
            {
                pMe->m_yx_loction_link_head = p1->next_ptr;
            }
            else
            {
                p2->next_ptr = p1->next_ptr;
            }
            SCI_FREE(p1);
            res = TRUE;
            break;
        }
        p2 = p1;
        p1 = p2->next_ptr;
    }
        
    return res;
}

BOOLEAN  YX_LOCTION_Link_DelAll(YX_APP_T *pMe)
{
    YX_LOCTION_LINK_NODE_T  * p1 = NULL;
    
    while(pMe->m_yx_loction_link_head != NULL)
    {
        p1 = pMe->m_yx_loction_link_head;
        pMe->m_yx_loction_link_head = p1->next_ptr;
        if(p1->data.pParam!= NULL)
        {
            SCI_FREE(p1->data.pParam);
        }
        SCI_FREE(p1);
    }
    return TRUE;
}

int YX_LOCTION_CR_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_CR);
    return 0;
}

int YX_LOCTION_Net_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_NET);
    return 0;
}

int YX_LOCTION_Sms_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_SMS);
    return 0;
}

int YX_LOCTION_Alert_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_ALERT);
    return 0;
}

int YX_LOCTION_Friend_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_FRIEND);
    return 0;
}

int YX_LOCTION_WT_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_WT);
    return 0;
}

//GPS定位消息回调
int YX_LOCTION_GPS_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_GPS);
    return 0;
}

//GPS报警消息回调
int YX_LOCTION_GPS_Alert_CallBack (uint8 loc_ok,void * pLoctionData)
{
    YX_API_LOCTION_End(loc_ok,YX_LOCTION_TYPE_GPS_ALERT);
    return 0;
}


BOOLEAN YX_LOCTION_Start(YX_APP_T *pMe,uint16 type,uint8 * pParam,uint16 ParamLen)
{
    BOOLEAN is_exsit = FALSE;
    uint32 time_s = YX_LOCTION_ON_TIME_S;
    if(s_yx_loction_is_first_on)
    {
        time_s = YX_LOCTION_FIRST_ON_TIME_S;

        s_yx_loction_is_first_on = FALSE;
    }
    
    SCI_TraceLow("wuxx, YX_LOCTION_Start,");
    
#if defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE_20231204 // 除CR 外,有GPS 定位
    if((s_yx_loction_is_first_on_for_gps == TRUE)&&((type == YX_LOCTION_TYPE_GPS) || (type == YX_LOCTION_TYPE_GPS_ALERT)))
    {
        time_s = YX_LOCTION_FIRST_ON_FOR_GPS_TIME_S;

        //s_yx_loction_is_first_on_for_gps = FALSE; //GPS 定位成功再置FALSE
    }

    //保存定位方式, 是否GPS
    zdt_set_is_gps_location_type(type);
#endif
    
    if(YX_LOCTION_TYPE_OFF == type)
    {
        
        SCI_TraceLow("wuxx, YX_LOCTION_Start, YX_LOCTION_TYPE_OFF");
        #if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129
            MMIZDTGPS_PowerOff();
        #else
            #ifdef ZDT_WIFI_SUPPORT
            #endif
        #endif
    }
    else
    {
        if(YX_LOCTION_TYPE_CR == type)
        {
            SCI_TraceLow("wuxx, YX_LOCTION_Start, YX_LOCTION_TYPE_CR");
            if(YX_LOCTION_Link_TypeExsit(pMe,type))
            {
                SCI_TraceLow("wuxx, YX_LOCTION_Start, YX_LOCTION_TYPE_CR 01");
                is_exsit =  TRUE;
            }
            YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
            if(is_exsit == FALSE)
            {
             SCI_TraceLow("wuxx, YX_LOCTION_Start, YX_LOCTION_TYPE_CR 02");
                #if 0//defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 20231204 CR  is lbs+wifi GPS_NEW_STYLE_20231204
                    MMIZDTGPS_API_LocStart(time_s,YX_LOCTION_CR_CallBack);
                #else
                    #ifdef ZDT_WIFI_SUPPORT
                        MMIZDTWIFI_API_LocStart(25,YX_LOCTION_CR_CallBack);
                    #endif
                #endif
                
            }
        }
        else if(YX_LOCTION_TYPE_NET == type)
        {
            SCI_TraceLow("wuxx, YX_LOCTION_Start, YX_LOCTION_TYPE_NET");
            if(YX_LOCTION_Link_TypeExsit(pMe,type))
            {
                SCI_TraceLow("wuxx, YX_LOCTION_Start, YX_LOCTION_TYPE_NET 01");
                is_exsit =  TRUE;
            }
            // YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen); //delelted by bao 多次频繁定位只处理一条
            if(is_exsit == FALSE)
            {
                YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
                SCI_TraceLow("wuxx, YX_LOCTION_Start, YX_LOCTION_TYPE_NET 02");
                //bao 优先WIFI定位没有WIFI的情况下再开启GPS
                #if 0 //defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS)// wuxx add 20231129 GPS_NEW_STYLE_20231204
                    MMIZDTGPS_API_LocStart(time_s,YX_LOCTION_Net_CallBack);
                #else
                    #ifdef ZDT_WIFI_SUPPORT
                        MMIZDTWIFI_API_LocStart(25,YX_LOCTION_Net_CallBack);
                    #endif
                #endif
            }
        }
        else if(YX_LOCTION_TYPE_SMS == type)
        {
            if(YX_LOCTION_Link_TypeExsit(pMe,type))
            {
                is_exsit =  TRUE;
            }
            //YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen); //delelted by bao 多次频繁定位只处理一条
            if(is_exsit == FALSE)
            {
                YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
                //bao 优先WIFI定位没有WIFI的情况下再开启GPS
                #if 0 //defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS) // GPS_NEW_STYLE_20231204
                    MMIZDTGPS_API_LocStart(time_s,YX_LOCTION_Sms_CallBack);
                #else
                    #ifdef ZDT_WIFI_SUPPORT
                        MMIZDTWIFI_API_LocStart(25,YX_LOCTION_Sms_CallBack);
                    #endif
                #endif
            }
        }
        else if(YX_LOCTION_TYPE_ALERT == type)
        {
		#if 0  //deleted by bao 报警消息不能过滤
            if(YX_LOCTION_Link_TypeExsit(pMe,type))
            {
                return TRUE;
            }
		#endif
            YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
                #if 0 //defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS) // GPS_NEW_STYLE_20231204
                    MMIZDTGPS_API_LocStart(time_s,YX_LOCTION_Alert_CallBack);
                #else
                    #ifdef ZDT_WIFI_SUPPORT
                        MMIZDTWIFI_API_LocStart(25,YX_LOCTION_Alert_CallBack);
                    #endif
                #endif
        }
        else if(YX_LOCTION_TYPE_FRIEND == type)
        {
            if(YX_LOCTION_Link_TypeExsit(pMe,type))
            {
                return TRUE;
            }
            YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
                //交友时直接用基线或WIFI
                #if 0//defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS) // // 20231204  frind is wifi. GPS_NEW_STYLE_20231204
                    MMIZDTGPS_API_LocStart(time_s,YX_LOCTION_Friend_CallBack);
                #else
                    #ifdef ZDT_WIFI_SUPPORT
                        MMIZDTWIFI_API_LocStart(25,YX_LOCTION_Friend_CallBack);
                    #endif
                #endif
        }
        else if(YX_LOCTION_TYPE_WT == type) //天气定位要求不需要很精准 WIFI加基站定位就满足了
        {
            if(YX_LOCTION_Link_TypeExsit(pMe,type))
            {
                return TRUE;
            }
            YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
            #ifdef ZDT_WIFI_SUPPORT
                MMIZDTWIFI_API_LocStart(25,YX_LOCTION_WT_CallBack);
            #endif
        }
        else if(YX_LOCTION_TYPE_GPS== type)
        {
            if(YX_LOCTION_Link_TypeExsit(pMe,type))
            {
                is_exsit =  TRUE;
            }
            
            if(is_exsit == FALSE)
            {
                YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
                #if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS) // GPS_NEW_STYLE_20231204
                    MMIZDTGPS_API_LocStart(time_s,YX_LOCTION_GPS_CallBack);
                #else
                    #ifdef ZDT_WIFI_SUPPORT
                        MMIZDTWIFI_API_LocStart(25,YX_LOCTION_GPS_CallBack);
                    #endif
                #endif
            }
        }
        else if(YX_LOCTION_TYPE_GPS_ALERT== type)
        {
            YX_LOCTION_Link_AddData(pMe,type,pParam,ParamLen);
            #if defined(ZDT_GPS_SUPPORT)||defined(ZDT_W217_FACTORY_GPS) // GPS_NEW_STYLE_20231204
                MMIZDTGPS_API_LocStart(time_s,YX_LOCTION_GPS_Alert_CallBack);
            #else
                #ifdef ZDT_WIFI_SUPPORT
                    MMIZDTWIFI_API_LocStart(25,YX_LOCTION_GPS_Alert_CallBack);
                #endif
            #endif
        }
    }
    return TRUE;
}

int YX_LOCTION_EndHandle(YX_APP_T *pMe,DPARAM param)
{
    uint32 dwParam = 0;
    MMI_ZDT_SIG_T * pp_getdata = (MMI_ZDT_SIG_T *)param;
    MMI_ZDT_DATA_T * p_getdata = &(pp_getdata->data);
    uint16 type = 0;
    uint8 loc_ok = 0;
    BOOLEAN need_resend = FALSE;
    if(p_getdata != NULL)
    {
        dwParam = p_getdata->len;
    }
    loc_ok = (dwParam & 0x000000FF);
    type = (dwParam >> 16);
    ZDT_LOG("YX_LOCTION_EndHandle type=%d,loc_ok=0x%02x",type,loc_ok);
    switch(type)
    {
        case YX_LOCTION_TYPE_CR:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_CR))
                {
                    if(loc_ok == 0 && need_resend == FALSE)
                    {
                        need_resend = TRUE;
                    }
                    if(need_resend == FALSE) //added by bao 需要等GPS定位后上报
                    {
                        YX_Net_Send_UD(pMe,loc_ok);
                    }
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
                if(need_resend)
                {
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)
                    YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_GPS,NULL,0);
#endif
                }
            }
            break;
            
        case YX_LOCTION_TYPE_NET:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_NET))
                {
                    if(loc_ok == 0 && need_resend == FALSE)
                    {
                        need_resend = TRUE;
                    }
                    if(need_resend == FALSE) //added by bao 需要等GPS定位后上报
                    {
                        YX_Net_Send_UD(pMe,loc_ok);
                        YX_Net_Send_WT_Check(loc_ok);
                    }
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
                if(need_resend)
                {
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)
                    YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_GPS,NULL,0);
#endif
                }
            }
            break;
            
        case YX_LOCTION_TYPE_SMS:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_SMS))
                {
                    if(loc_ok == 0 && need_resend == FALSE)
                    {
                        need_resend = TRUE;
                    }
                    if(need_resend == FALSE) //added by bao 需要等GPS定位后上报
                    {
                        YX_Net_Send_UD(pMe,loc_ok);
                    }
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
                if(need_resend)
                {
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)
                    YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_GPS,NULL,0);
#endif
                }
            }
            break;
            
        case YX_LOCTION_TYPE_ALERT:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_ALERT))
                {
                    if(loc_ok == 0 && need_resend == FALSE)
                    {
                        need_resend = TRUE;
                    }
                    YX_Net_Send_AL(pMe,cur_data.len,loc_ok);
                    if(need_resend) //added by bao 报警消息需要等GPS定位成功后再次上报
                    {
#if defined(ZDT_GPS_SUPPORT) || defined(ZDT_WIFI_SUPPORT) ||defined(ZDT_W217_FACTORY_GPS)
                        YX_LOCTION_Start(pMe,YX_LOCTION_TYPE_GPS_ALERT,NULL,cur_data.len);
#endif
                    }
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
            }
            break;
            
        case YX_LOCTION_TYPE_FRIEND:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_FRIEND))
                {
                    YX_Net_Send_PP(pMe,loc_ok);
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
            }
            break;
        case YX_LOCTION_TYPE_WT:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_WT)) //YX_LOCTION_TYPE_NET 此处类型不对，天气更新会不即时
                {
                    YX_Net_Send_WT_Check(loc_ok);
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
            }
            break;
            
        case YX_LOCTION_TYPE_GPS:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_GPS))
                {
                    YX_Net_Send_UD(pMe,loc_ok);
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
            }
            break;
            
        case YX_LOCTION_TYPE_GPS_ALERT:
            {
                YX_LOCTION_LINK_DATA_T cur_data = {0};
                while(YX_LOCTION_Link_GetByType(pMe,&cur_data,YX_LOCTION_TYPE_GPS_ALERT))
                {
                    if(loc_ok != 0)
                    {
						//报警消息之前有上报，只有GPS定位成功才上报
                        YX_Net_Send_AL(pMe,cur_data.len,loc_ok);
                    }
                    if(cur_data.pParam != NULL)
                    {
                        SCI_FREE(cur_data.pParam);
                    }
                }
            }
            break;
            
        default:
            break;
    }
    return 0;
}

int YX_API_LOCTION_End(uint16 loc_ok,uint16 type)
{
    uint32 len = type;
    len = len << 16;
    len += loc_ok;
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOCTION_END,NULL,len );
    return 0;
}
#endif


int YX_API_Lose_Post(void)
{
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LOSE,NULL,0);
    return 0;
}

int YX_API_BatStatus_Post(uint8 level,uint8 percent)
{
    uint16 bat_status = 0;
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    bat_status = level;
    bat_status = bat_status << 8;
    bat_status += percent;
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_BATSTAUTS,NULL,bat_status);
    return 0;
}

int YX_API_Move_Post(void)
{
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_MOVE,NULL,0);
    return 0;
}

int YX_API_Still_Post(void)
{
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_STILL,NULL,0);
    return 0;
}

int YX_API_JP_Change_Post(void)
{
    //有走动
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_JP_CHANGE,NULL,0);
    return 0;
}

int YX_API_LBS_Change_Post(void)
{
    //基站发生变化
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_LBS_CHANGE,NULL,0);
    return 0;
}

int YX_API_FallDown_Post(void)
{
    if(g_is_yx_power_off_ing == TRUE)
    {
        return 0;
    }
    MMIZDT_SendSigTo_APP(ZDT_APP_SIGNAL_YX_FALLDOWN,NULL,0);
    return 0;
}
#endif
BOOLEAN YX_API_CallNumber_IsValid(char * num)
{
    BOOLEAN res = TRUE;
    if(num == PNULL)
    {
        return FALSE;
    }
    
    if(YX_DB_TIMER_IsValid_Slient())
    {
        res = FALSE;
    }
   
    if(YX_IsRejectUnknownCallOpen())
    {
        uint16 name_wstr[101] ={0};
        uint16 name_len = 0;
        BOOLEAN is_name_exist = FALSE;
        is_name_exist = MMICC_GetYxDbRecCallName(num, name_wstr,&name_len);
        if(!is_name_exist)
        {
            res = FALSE;
        }
    }
    
    return res;

}

BOOLEAN YX_API_SOS_Post(void)
{
    if(YX_Net_Is_Land())
    {
        YX_StartSosAlertLocation();
        return TRUE;
    }
    else
    {
        if(ZDT_SIM_Exsit())
        {
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_NET,1);
#endif
#endif
        }
        else
        {
#ifdef ZDT_LCD_NOT_SUPPORT
#ifdef ZDT_PLAY_RING_CUSTOM
            MMIAPISET_PlayCustomRing(MMISET_CUST_NO_SIM,1);
#endif
#endif
        }
    }
    
    return FALSE;
}

BOOLEAN YX_API_SOS_PostExt(void)
{
    if(YX_Net_Is_Land())
    {
        YX_StartSosAlertLocation();
        return TRUE;
    }
    return FALSE;
}

BOOLEAN YX_API_SOS_PostAfter(void)
{
#ifdef ZDT_LED_SUPPORT
    ZDT_Led_Start_Call_Incoming_Over();
#endif
    if(YX_Net_Is_Land())
    {
        YX_StartSosAlertLocation();
        return TRUE;
    }
    return FALSE;
}


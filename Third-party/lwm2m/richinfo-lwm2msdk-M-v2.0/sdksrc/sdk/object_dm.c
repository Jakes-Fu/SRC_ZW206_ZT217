/*******************************************************************************
/*
 * This object is single instance only, and provide dm info functionality.
 * Object ID is 666.
 * Object instance ID is 0.
 */

#include "liblwm2m.h"
#include "internals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef LWM2M_SUPPORT_4
#include "lwm2msdk.h"
#include "cJSON.h"
#endif
#ifndef LWM2M_SUPPORT_4
// Resource Id's:
#define LWM2M_DM_UE_OBJECT               666

#define RES_DM_DEV_INFO                  6601
#define RES_DM_APP_INFO                  6602
#define RES_DM_MAC                       6603
#define RES_DM_ROM                       6604
#define RES_DM_RAM                       6605
#define RES_DM_CPU                       6606
#define RES_DM_SYS_VERSION               6607
#define RES_DM_FIRM_VERSION              6608
#define RES_DM_FIRM_NAME                 6609
#define RES_DM_VOLTE                     6610

#define RES_DM_NET_TYPE                  6611
#define RES_DM_NET_ACCT                  6612
#define RES_DM_PHONE                     6613
#define RES_DM_LOCATION                  6614


#define RES_VALUE_BUFF_LEN  512
#endif
#ifdef LWM2M_SUPPORT_4
#define FIELDCONFIG 1
#define RULECONFIG 2
#define ADDRESSCONFIG 3

#define RES_VALUE_BUFF_LEN  512

struct deviceinfo g_deviceinfo[] ={  \
    {"imsi",RES_DM_IMSI,FALSE,NULL},
    {"sn",RES_DM_SN,FALSE,NULL},
    {"mac",RES_DM_MAC,FALSE,NULL},
    {"rom",RES_DM_ROM,FALSE,NULL},
    {"ram",RES_DM_RAM,FALSE,NULL},
    {"cpu",RES_DM_CPU,FALSE,NULL},
    {"sysVersion",RES_DM_SYS_VERSION,FALSE,NULL},
    {"softwareVer",RES_DM_FIRM_VERSION,FALSE,NULL},
    {"softwareName",RES_DM_FIRM_NAME,FALSE,NULL},
    {"volte",RES_DM_VOLTE,FALSE,NULL},
    {"netType",RES_DM_NET_TYPE,FALSE,NULL},
    //{"account",RES_DM_NET_ACCT,FALSE,NULL},
    {"phoneNumber",RES_DM_PHONE,FALSE,NULL},
    //{"location",RES_DM_LOCATION,FALSE,NULL},
    {"routerMac",RES_DM_ROUTER_MAC,FALSE,NULL},
    {"bluetoothMac",RES_DM_BLUETOOTH_MAC,FALSE,NULL},
    {"gpu",RES_DM_GPU,FALSE,NULL},
    {"board",RES_DM_BOARD,FALSE,NULL},
    {"resolution",RES_DM_RESOLUTION,FALSE,NULL},
    {"batteryCapacity",RES_DM_BATTERYCAPACITY,FALSE,NULL},
    {"screenSize",RES_DM_SCREENSIZE,FALSE,NULL},
    {"networkStatus",RES_DM_NETWORKSTATUS,FALSE,NULL},
    {"wearingStatus",RES_DM_WEARINGSTATUS,FALSE,NULL},
    {"appInfo",RES_DM_APP_INFO,FALSE,NULL},
    {"imsi2",RES_DM_IMSI2,FALSE,NULL},
    {"batteryCapacityCurr",RES_DM_BATTERYCAPACITYCURR,FALSE,NULL}
};

struct ruleConfig g_ruleConfig[RULECONFIG_NUM] ={
    {"reportTime",reportTime,0},
    {"reportNum",reportNum,0},
    {"heartBeatTime",heartBeatTime,0},
    {"retryInterval",retryInterval,0},
    {"retryNum",retryNum,0}
                                                  };

struct addressConfig g_addressConfig = {"addressConfig",ADDRESSCFG,NULL,0} ;   
#endif

typedef struct
{
    uint8_t state;
    uint8_t result;
    
    char dev_info[RES_VALUE_BUFF_LEN];
    char app_info[RES_VALUE_BUFF_LEN];
    char mac[RES_VALUE_BUFF_LEN];
    char rom[RES_VALUE_BUFF_LEN];
    char ram[RES_VALUE_BUFF_LEN];
    char cpu[RES_VALUE_BUFF_LEN];
    char sys_ver[RES_VALUE_BUFF_LEN];
    char firm_ver[RES_VALUE_BUFF_LEN];
    char firm_name[RES_VALUE_BUFF_LEN];
    char volte[RES_VALUE_BUFF_LEN];    
    
    char net_type[RES_VALUE_BUFF_LEN];
    char net_acct[RES_VALUE_BUFF_LEN];
    char phone[RES_VALUE_BUFF_LEN];
    char location[RES_VALUE_BUFF_LEN];
    
} dmread_data_t;

#ifdef LWM2M_SUPPORT_4
int g_deviceinfo_need_report = 0;
#endif
extern int lwm2m_sdk_dm_get_devinfo(uint16_t resId,char **outbuff);



static uint8_t prv_dm_read(uint16_t instanceId,
                                 int * numDataP,
                                 lwm2m_data_t ** dataArrayP,
                                 lwm2m_object_t * objectP)
{
    int i;
    uint8_t result;
    dmread_data_t * data = (dmread_data_t*)(objectP->userData);
    int total_count;
    int iret;
    LOG(" debug ");
    // this is a single instance object
    if (instanceId != 0)
    {
        LOG(" debug ");
        return COAP_404_NOT_FOUND;
    }
#ifdef LWM2M_SUPPORT_4
     total_count = deviceinfo_num;//19+5;
#else
     total_count = 14;
#endif
    LOG_ARG(" debug *numDataP: %d  ",*numDataP);
    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        LOG(" debug ");
        *dataArrayP = lwm2m_data_new(total_count);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = total_count;

        for(i=0;i<total_count;i++)
        (*dataArrayP)[i].id = 6600+i+1+2;
    }

    i = 0;
    iret = 0;
    do
    {
#ifdef LWM2M_SUPPORT_4
        if((*dataArrayP)[i].id >=RES_DM_MAC && (*dataArrayP)[i].id <= RES_DM_BATTERYCAPACITYCURR )
#else
        if((*dataArrayP)[i].id >=RES_DM_DEV_INFO && (*dataArrayP)[i].id <= RES_DM_LOCATION )
#endif
        {
            //char sztmp[512];
            char *outbuff = NULL;
            iret = lwm2m_sdk_dm_get_devinfo((*dataArrayP)[i].id,&outbuff);
            if(iret!=0)
            {
                LOG_ARG("resId=%u,lwm2m_sdk_dm_get_devinfo,iret=%d",(*dataArrayP)[i].id ,iret);
                result = COAP_404_NOT_FOUND;
            }
            else
            {            
                lwm2m_data_encode_string(outbuff, *dataArrayP + i); 
                result = COAP_205_CONTENT;
            }
            if(outbuff) free(outbuff);
        }
        else
            result = COAP_404_NOT_FOUND;

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}



lwm2m_object_t * get_object_dm(void)
{
    lwm2m_object_t * firmwareObj;

    firmwareObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != firmwareObj)
    {
        memset(firmwareObj, 0, sizeof(lwm2m_object_t));
#ifdef LWM2M_SUPPORT_4
        firmwareObj->objID = LWM2M_DM_UE_OBJECT_READ;
#else
        firmwareObj->objID = LWM2M_DM_UE_OBJECT;
#endif

        /*
         * and its unique instance
         *
         */
        firmwareObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        if (NULL != firmwareObj->instanceList)
        {
            memset(firmwareObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(firmwareObj);
            return NULL;
        }

       
        firmwareObj->readFunc    = prv_dm_read;
        firmwareObj->writeFunc   = 0;
        firmwareObj->executeFunc = 0;
        firmwareObj->userData    = lwm2m_malloc(sizeof(dmread_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables
         */
        if (NULL != firmwareObj->userData)
        {
            dmread_data_t *data = (dmread_data_t*)(firmwareObj->userData);

            data->state = 1;
            data->result = 0;
            strcpy(data->dev_info, "");
            strcpy(data->app_info, "1.0");

            
        }
        else
        {
            lwm2m_free(firmwareObj);
            firmwareObj = NULL;
        }
    }

    return firmwareObj;
}
#ifdef LWM2M_SUPPORT_4
lwm2m_object_t * get_object_dm_read(void)
{
    lwm2m_object_t * firmwareObj;

    firmwareObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != firmwareObj)
    {
        memset(firmwareObj, 0, sizeof(lwm2m_object_t));
        firmwareObj->objID = LWM2M_DM_UE_OBJECT_READ;
        // Not required, but useful for testing.
        firmwareObj->versionMajor = 1;
        firmwareObj->versionMinor = 2;
        /*
         * and its unique instance
         *
         */
        firmwareObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        if (NULL != firmwareObj->instanceList)
        {
            memset(firmwareObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(firmwareObj);
            return NULL;
        }

        firmwareObj->readFunc    = prv_dm_read;
        firmwareObj->writeFunc   = 0;
        firmwareObj->executeFunc = 0;
        firmwareObj->userData    = lwm2m_malloc(sizeof(dmread_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables
         */
        if (NULL != firmwareObj->userData)
        {
            dmread_data_t *data = (dmread_data_t*)(firmwareObj->userData);

            data->state = 1;
            data->result = 0;
            strcpy(data->dev_info, "");
            strcpy(data->app_info, "1.0");
        }
        else
        {
            lwm2m_free(firmwareObj);
            firmwareObj = NULL;
        }
    }
    return firmwareObj;
}

//add by QY
static uint8_t prv_dm_write(uint16_t instanceId,
                                int numData,
                                lwm2m_data_t * dataArray,
                                lwm2m_object_t * objectP)
{
    uint8_t result=COAP_204_CHANGED;
    int i=0;
    cJSON * root;
    cJSON * item;
    LOG_ARG("size = %d",numData);
    for(i=0;i<numData;++i)
    {
        LOG_ARG("id = %d ,dataArray[%d] : %s",dataArray[i].id,i,dataArray[i].value.asBuffer.buffer);
        root=cJSON_Parse(dataArray[i].value.asBuffer.buffer);
        if(NULL != root)
        {
            int j;
            switch(dataArray[i].id)
            {
            case FIELDCONFIG:
                {
                    g_deviceinfo_need_report = 0;
                    for(j=0;j<deviceinfo_num;++j)
                    {
                        item=cJSON_GetObjectItem(root,g_deviceinfo[j].name);
                        g_deviceinfo[j].choice=FALSE;
                        if(NULL!=item)
                        {
                            if(0==strcmp(item->valuestring,"Y"))
                            {
                                LOG_ARG("g_deviceinfo[%d].name = %s   is Y",j,g_deviceinfo[j].name);
                                g_deviceinfo[j].choice=TRUE;
                                g_deviceinfo_need_report++;
                            }
                        }
                    }
                }
                break;
            case RULECONFIG:
                {
                    for(j=0;j<RULECONFIG_NUM;++j)
                    {
                        item=cJSON_GetObjectItem(root,g_ruleConfig[j].rulename);
                        if(NULL!=item)
                        {
                            LOG_ARG("g_ruleConfig[%d].rulename = %s:%d",j,g_ruleConfig[j].rulename,item->valueint);
                            g_ruleConfig[j].value=item->valueint;
                        }
                    }
                }
                break;
            case ADDRESSCONFIG:
                {
                    for(j=0;j<1;++j)
                    {
                        item=cJSON_GetObjectItem(root,g_addressConfig.name);
                        if(NULL!=item)
                        {
                            LOG_ARG("addressConfig  = %s\n",item->valuestring);
                            memcpy(g_addressConfig.value,item->valuestring,strlen(item->valuestring));
                        }
                    }
                }
                break;
             default:
                result = COAP_402_BAD_OPTION;
            }
        }
        if((dataArray[i].id==ADDRESSCONFIG)&&(0!=dataArray[i].value.asBuffer.length))
        {
             //memcpy(g_addressconfig,dataArray[i].value.asBuffer.buffer,dataArray[i].value.asBuffer.length);
        }
        if(NULL!=root)
        {
            cJSON_Delete(root);
        }
    }

    return result;
}

lwm2m_object_t * get_object_dm_write(void)
{
    lwm2m_object_t * firmwareObj;

    firmwareObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != firmwareObj)
    {
        memset(firmwareObj, 0, sizeof(lwm2m_object_t));
        firmwareObj->objID = LWM2M_DM_UE_OBJECT_WRITE;
        // Not required, but useful for testing.
        firmwareObj->versionMajor = 1;
        firmwareObj->versionMinor = 2;
        /*
         * and its unique instance
         *
         */
        firmwareObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        if (NULL != firmwareObj->instanceList)
        {
            memset(firmwareObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(firmwareObj);
            return NULL;
        }

        firmwareObj->readFunc    = 0;
        firmwareObj->writeFunc   = prv_dm_write;
        firmwareObj->executeFunc = 0;
        firmwareObj->userData    = lwm2m_malloc(sizeof(dmread_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables
         */
        if (NULL != firmwareObj->userData)
        {
            dmread_data_t *data = (dmread_data_t*)(firmwareObj->userData);

            data->state = 1;
            data->result = 0;
            strcpy(data->dev_info, "");
            strcpy(data->app_info, "1.0");
        }
        else
        {
            lwm2m_free(firmwareObj);
            firmwareObj = NULL;
        }
    }
    return firmwareObj;
}
#endif

void free_object_dm(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    if (NULL != objectP->instanceList)
    {
        lwm2m_free(objectP->instanceList);
        objectP->instanceList = NULL;
    }
    lwm2m_free(objectP);
}


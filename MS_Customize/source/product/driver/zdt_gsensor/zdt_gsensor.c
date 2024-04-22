#ifndef _ZDT_GSENSOR_C
#define _ZDT_GSENSOR_C

#include "zdt_gsensor.h"
#include "zdt_gsensor_qmax981.h"
#include "priority_system.h"
#include "ldo_drvapi.h"

#define GSENSOR_JP_MOVE_VAL  50
#ifndef abs
#define abs(x) (((x) >= 0) ? (x) : (-(x)))
#endif 

#ifdef ZDT_APP_SUPPORT
extern BOOLEAN  ZDTGSensor_SendTo_APP(ZDT_APP_SIG_E sig_id, uint8 * data_ptr, uint32 data_len);
#endif

static SCI_TIMER_PTR      s_gsensor_still_timer = PNULL;

static BLOCK_ID zdtgsensor_taskid=SCI_INVALID_BLOCK_ID;
BOOLEAN g_is_zdt_gsensor_open = FALSE;
BOOLEAN g_is_zdt_gsensor_init = FALSE;
uint32 gsensor_step_num = 0;
BOOLEAN gsensor_is_still = FALSE;
static uint32 s_jp_one_daly_steps = 0;
static uint32 s_jp_one_daly_start_steps = 0;
static uint32 s_jp_once_steps = 0;
static uint32 s_jp_once_bak_steps = 0;

static BOOLEAN gsensor_is_init = FALSE;
static BOOLEAN gsensor_is_open = FALSE;
static ZDT_GSENSOR_IC_TYPE_E s_gsensor_type = GSENSOR_TYPE_NULL;
static uint8 gsensor_int_status = 0;
static uint32 gsensor_int_ticks = 0;

extern void ZDT_NV_SetTodaySteps(uint32 steps);
uint32 ZDT_GSensor_GetStepOneDay(void) ;


LOCAL uint32 last_save_time = 0; //sec

void ZDT_SaveCurStepsToNv(uint32 steps, BOOLEAN forceSave)
{
    uint32 cur_time = MMIAPICOM_GetCurTime();
    if(forceSave || cur_time - last_save_time >= 5 )
    {
        last_save_time = cur_time;
        ZDT_NV_SetTodaySteps(steps);
    }
}

void ZDT_SaveCurStepsBeforePowerOff()
{
    ZDT_SaveCurStepsToNv(s_jp_one_daly_steps, TRUE);
}

static BOOLEAN gsensor_init(void) 
{
    BOOLEAN ret = FALSE;
    if(gsensor_is_init == FALSE)
    {
        LDO_TurnOnLDO (LDO_LDO_SDIO);
        SCI_Sleep(30);
        if(TRUE == QMAX981_init() )
        {
            qmaX981_chip_type qma_type = QMAX981_TYPE_UNKNOW;
            ret = TRUE;
            qma_type = Get_Cur_Gsensor_Chip();
            if(qma_type == QMAX981_TYPE_6981)
            {
                s_gsensor_type = GSENSOR_TYPE_QMA6981;
            }
            else if(qma_type == QMAX981_TYPE_7981)
            {
                s_gsensor_type = GSENSOR_TYPE_QMA7981;
            }
            else if(qma_type == QMAX981_TYPE_6100)
            {
                s_gsensor_type = GSENSOR_TYPE_QMA6100;
            }
        }
        else if(TRUE == MIR3DA_init())
        {
            ret = TRUE;
            s_gsensor_type = GSENSOR_TYPE_DA213;
        }
        else
        {
            GPIO_ZDT_GSensorIRQCtrl(FALSE);
            s_gsensor_type = GSENSOR_TYPE_NULL;
        }

        ZDT_GSENSOR_TRACE("gsensor_init over ret=%d,type=%d",ret,s_gsensor_type);
        if (ret)
        {
            gsensor_is_init = TRUE;
        }
        else
        {
            //LDO_TurnOffLDO (LDO_LDO_SDIO);
        }
    }
   return ret;
}

static BOOLEAN gsensor_open(void) 
{
    BOOLEAN ret = FALSE;
    ZDT_GSENSOR_TRACE("gsensor_open is_init=%d,type=%d",gsensor_is_init,s_gsensor_type);
    if(gsensor_is_open == FALSE)
    {
        if(s_gsensor_type >= GSENSOR_TYPE_QMA)  //QMA7981 or QMA6100
        {
            ret = QMAX981_open();
        }
        else if(s_gsensor_type == GSENSOR_TYPE_DA213)
        {
            ret = MIR3DA_open();
        }
        if (ret)
        {
            gsensor_is_open = TRUE;
        }
    }
   return ret;
}

static BOOLEAN gsensor_close(void) 
{
    BOOLEAN ret = TRUE;
    ZDT_GSENSOR_TRACE("gsensor_close is_init=%d,type=%d",gsensor_is_init,s_gsensor_type);
    if(gsensor_is_init)
    {
        if(gsensor_is_open)
        {
            if(s_gsensor_type  >= GSENSOR_TYPE_QMA)   //QMA7981 or QMA6100
            {
                QMAX981_close();
            }
            else if(s_gsensor_type == GSENSOR_TYPE_DA213)
            {
                MIR3DA_close();
            }
            gsensor_is_open = FALSE;
        }
        //LDO_TurnOffLDO (LDO_LDO_SDIO);
        gsensor_is_init = FALSE;
    }
   return ret;
}

static BOOLEAN gsensor_reset_step(void) 
{
    BOOLEAN ret = FALSE;
    ZDT_GSENSOR_TRACE("gsensor_reset_step is_init=%d,type=%d",gsensor_is_init,s_gsensor_type);
    if(gsensor_is_init)
    {
        if(gsensor_is_open)
        {
            if(s_gsensor_type  >= GSENSOR_TYPE_QMA)   //QMA7981 or QMA6100
            {
                QMAX981_step_reset();
                ret = TRUE;
            }
            else if(s_gsensor_type == GSENSOR_TYPE_DA213)
            {
                MIR3DA_Reset();
                ret = TRUE;
            }
        }
    }
   return ret;
}

void ZDTGSensor_Set_CurStep(uint32 x)
{
    if(s_jp_once_steps != x)
    {
        if(s_jp_once_steps > x)
        {
            return;
        }
        if(x > s_jp_once_bak_steps && (x-s_jp_once_bak_steps) >= GSENSOR_JP_MOVE_VAL)
        {
            s_jp_once_bak_steps = x;
#ifdef ZDT_PLAT_YX_SUPPORT
            YX_API_JP_Change_Post();
#endif
        }
        s_jp_once_steps = x;
        s_jp_one_daly_steps = s_jp_one_daly_start_steps + s_jp_once_steps;
        if(s_jp_one_daly_steps >= 0xffffff7f)
        {
            s_jp_one_daly_steps = 0;
        }
#ifdef ZDT_APP_SUPPORT
        ZDT_SaveCurStepsToNv(s_jp_one_daly_steps, FALSE);
#endif
    }
}

static void ZDTGSensor_StillTimerOut(uint32 param)
{	    
    gsensor_is_still = TRUE;
#ifdef ZDT_PLAT_YX_SUPPORT
    YX_API_Still_Post();
#endif
    return ;
}

BOOLEAN  ZDTGSensor_StillTimer_Start(void)
{
    if(s_gsensor_still_timer == PNULL)
    {
        s_gsensor_still_timer = SCI_CreateTimer("STILL_Timer", ZDTGSensor_StillTimerOut,
                                    1, 120000, SCI_AUTO_ACTIVATE);
    }
    else
    {
        SCI_ChangeTimer(s_gsensor_still_timer, ZDTGSensor_StillTimerOut, 120000);
        SCI_ActiveTimer(s_gsensor_still_timer);	
    }
    return TRUE;
}

BOOLEAN  ZDTGSensor_StillTimer_Stop(void)
{
    if(s_gsensor_still_timer != PNULL)
    {
        if(SCI_IsTimerActive(s_gsensor_still_timer))
        {
            SCI_DeactiveTimer(s_gsensor_still_timer);
        }
    }
    return TRUE;
}

uint32 ZDTGSensor_GPIO_IntHandler(BOOLEAN is_high)
{
    if(g_is_zdt_gsensor_init == FALSE)
    {
        return 0;
    }
    if(s_gsensor_type  >= GSENSOR_TYPE_QMA)   //QMA7981 or QMA6100
    {
        if(is_high)
        {
            ZDT_GSensor_Still();
        }
        else
        {
            ZDT_GSensor_Move();
        }
    }
    else if(s_gsensor_type == GSENSOR_TYPE_DA213)
    {
        if(is_high)
        {
            ZDT_GSensor_Move();
        }
        else
        {
           ZDT_GSensor_Still();
        }
    }
    return 0;
}

uint32 ZDTGSensor_HandleStepReset(void) 
{
    int x = 0;
    if(gsensor_is_init == FALSE)
    {
        return 0;
    }
    gsensor_reset_step();
    gsensor_step_num = 0;
    s_jp_once_steps = 0;
    s_jp_once_bak_steps = 0;
    return 0;
}

BOOLEAN  ZDTGSensor_HandleStep(uint32 once_step)
{
    if(gsensor_step_num != once_step)
    {
        ZDT_GSENSOR_TRACE("ZDTGSensor_HandleStep %ld-%ld",once_step,gsensor_step_num);
        ZDTGSensor_Set_CurStep(once_step);
        gsensor_step_num = once_step;
    }
    
    
    return TRUE;
}

BOOLEAN  ZDTGSensor_HandleOpen(void)
{
    ZDT_GSENSOR_TRACE("ZDTGSensor Open");
    if(gsensor_open())
    {
        g_is_zdt_gsensor_open = TRUE;
    }
    return TRUE;
}

BOOLEAN  ZDTGSensor_HandleClose(void)
{
    ZDT_GSENSOR_TRACE("ZDTGSensor Close");
    if(gsensor_close())
    {
        g_is_zdt_gsensor_open = FALSE;
    }
    return TRUE;
}
extern uint16  TIMER_PERIOD_QMAX981;
extern void QMA6981_RecordOneXYZ();

BOOLEAN  ZDTGSensor_HandleTimer(void)
{
    //ZDT_GSENSOR_TRACE("ZDTGSensor Timer type=%d,step=%d",s_gsensor_type,gsensor_step_num);
    if(s_gsensor_type  >= GSENSOR_TYPE_QMA)   //QMA7981 or QMA6100
    {
        uint32 cur_step = 0;
        static uint32 timer_count = 1;
    
        if(TIMER_PERIOD_QMAX981 == 1000 || timer_count%20==0)
        {
            qmaX981_custom_get_step(&cur_step);
            if(gsensor_step_num != cur_step)
            {
                ZDT_GSENSOR_TRACE("ZDTGSensor GetStep = %d",cur_step);
                ZDT_GSensor_Step(cur_step);
            }
            if(timer_count%20==0)
            {
                timer_count++;
            }
        }
        else if(TIMER_PERIOD_QMAX981 != 1000)
        {
            timer_count++;
            if(timer_count>=200)
            {
                timer_count = 0;
            }
            QMA6981_RecordOneXYZ();
        }
    }
    else if(s_gsensor_type == GSENSOR_TYPE_DA213)
    {
        MIR3DA_GetEvent();
    }
    return TRUE;
}

BOOLEAN  ZDTGSensor_HandleMove(void)
{
    gsensor_is_still = FALSE;
    //ZDTGSensor_StillTimer_Stop();
    ZDTGSensor_StillTimer_Start();
#ifdef ZDT_PLAT_YX_SUPPORT
    YX_API_Move_Post();
#endif
    //ZDT_GSENSOR_TRACE("ZDTGSensor Move");
    return TRUE;
}

BOOLEAN  ZDTGSensor_HandleStill(void)
{
    return TRUE;
}

BOOLEAN  ZDTGSensor_HandleFail(void)
{
#ifdef ZDT_APP_SUPPORT
    ZDTGSensor_SendTo_APP(ZDT_APP_SIGNAL_GSENSOR_FAIL,PNULL,0);
#endif
    ZDT_GSENSOR_TRACE("ZDTGSensor FAIL");
    return TRUE;
}

void ZDTGSensor_TaskEntry(uint32 argc, void *argv)
{	
    zdtgsensor_taskid=SCI_IdentifyThread();
    if(gsensor_init())
    {
        g_is_zdt_gsensor_init = TRUE;
    }
    ZDT_GSENSOR_TRACE("ZDTGSensor_TaskEntry");
    
    while(1)
    {
    	xSignalHeaderRec*   sig_ptr = 0; 
        
    	sig_ptr = SCI_GetSignal( zdtgsensor_taskid); 
    	switch(sig_ptr->SignalCode)
    	{
                case SIG_ZDT_GSENSOR_STEPRESET:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        ZDTGSensor_HandleStepReset();
                        
                        if(gs_sig->len >  0 && gs_sig->str != NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                case SIG_ZDT_GSENSOR_STEP:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        if(gs_sig->len > 0)
                        {
                            ZDTGSensor_HandleStep(gs_sig->len);
                        }
                        
                        if(gs_sig->len >  0 && gs_sig->str != NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                case SIG_ZDT_GSENSOR_OPEN:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        
                        ZDTGSensor_HandleOpen();
                        
                        if(gs_sig->len >  0 && gs_sig->str!= NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                case SIG_ZDT_GSENSOR_CLOSE:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        
                        ZDTGSensor_HandleClose();
                        
                        if(gs_sig->len >  0 && gs_sig->str!= NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                case SIG_ZDT_GSENSOR_TIMER:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        
                        ZDTGSensor_HandleTimer();
                        
                        if(gs_sig->len >  0 && gs_sig->str!= NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                case SIG_ZDT_GSENSOR_MOVE:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        
                        ZDTGSensor_HandleMove();
                        
                        if(gs_sig->len >  0 && gs_sig->str!= NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                case SIG_ZDT_GSENSOR_STILL:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        
                        ZDTGSensor_HandleStill();
                        
                        if(gs_sig->len >  0 && gs_sig->str!= NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                case SIG_ZDT_GSENSOR_FAIL:
                    {
                        ZDT_GSENSOR_SIG_T *  gs_sig = (ZDT_GSENSOR_SIG_T*)sig_ptr;
                        
                        ZDTGSensor_HandleFail();
                        
                        if(gs_sig->len >  0 && gs_sig->str!= NULL)
                        {
                            SCI_FREE(gs_sig->str);
                        }
                    }
                    break;
                    
                default:
                    break;
    	}
    	SCI_FREE(sig_ptr);
    }
}

void ZDTGSensor_TaskCreate(void)
{
	if(zdtgsensor_taskid == SCI_INVALID_BLOCK_ID)
	{
		zdtgsensor_taskid = SCI_CreateThread("ZDTGSOR_THREAD",
			"ZDTGSOR_QUEUE",
			ZDTGSensor_TaskEntry,
			NULL,
			NULL,
			1024*10,
			200,
			80,//PRI_GPS_TASK,	// 比MMI 74要高 //yangyu modify
			SCI_PREEMPT,
			SCI_AUTO_START
			);
        
		ZDT_GSENSOR_TRACE("ZDTGSensor_TaskCreate, zdtgsensor_taskid=0x%x", zdtgsensor_taskid);
	}
}

void ZDTGSensor_TaskClose(void)
{
	if(SCI_INVALID_BLOCK_ID != zdtgsensor_taskid)
	{
		SCI_TerminateThread(zdtgsensor_taskid);
		SCI_DeleteThread(zdtgsensor_taskid);
		zdtgsensor_taskid = SCI_INVALID_BLOCK_ID;       
	}
	ZDT_GSENSOR_TRACE("ZDTGSensor_TaskClose");
}

BLOCK_ID ZDTGSensor_TaskGetID(void)
{
	return zdtgsensor_taskid;
}

BOOLEAN  APP_SendSigTo_GSensor(ZDT_GSENSOR_TASK_SIG_E sig_id, const char * data_ptr,uint32 data_len)
{
    uint8 * pstr = NULL;
    ZDT_GSENSOR_SIG_T * psig = PNULL;
    BLOCK_ID cur_taskID=ZDTGSensor_TaskGetID();
    
    if(SCI_INVALID_BLOCK_ID == cur_taskID)
    {
        ZDT_GSENSOR_TRACE("APP_SendSigTo_GSensor Err Task sig_id=0x%x,data_len=%d",sig_id,data_len);
        return FALSE;
    }
    
    if(data_len != 0 && data_ptr != NULL)
    {
        pstr = SCI_ALLOCA(data_len);//free it in gps task
        if (pstr == PNULL)
        {
                SCI_PASSERT(0, ("APP_SendTo_ATC Alloc  %ld FAIL",data_len));
                return FALSE;
        }
        SCI_MEMCPY(pstr,data_ptr,data_len);
    }
    
    //send signal to AT to write uart
    SCI_CREATE_SIGNAL((xSignalHeaderRec*)psig,sig_id,sizeof(ZDT_GSENSOR_SIG_T), SCI_IdentifyThread());
    psig->len = data_len;
    psig->str = pstr;
		
    SCI_SEND_SIGNAL((xSignalHeaderRec*)psig, cur_taskID);
    return TRUE;
}

BOOLEAN ZDT_GSensor_IsOpen(void)
{
    return g_is_zdt_gsensor_open;
}

int ZDT_GSensor_Init(void)
{    
    ZDTGSensor_TaskCreate();
    return 0;
}

int ZDT_GSensor_Step(uint32 once_step)
{    
    APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_STEP,PNULL,once_step);
    return 0;
}

int ZDT_GSensor_Open(void)
{    
    APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_OPEN,PNULL,0);
    return 0;
}

int ZDT_GSensor_Close(void)
{    
    APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_CLOSE,PNULL,0);
    return 0;
}

int ZDT_GSensor_Reset(void)
{    
    APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_STEPRESET,PNULL,0);
    return 0;
}

int ZDT_GSensor_TimerOut(void)
{    
    APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_TIMER,PNULL,0);
    return 0;
}

//运动
int ZDT_GSensor_Move(void)
{    
    if(gsensor_int_status == 0)
    {
        uint32 cur_ticks = SCI_GetTickCount();
        if(abs(cur_ticks-gsensor_int_ticks) > 300)
        {
            APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_MOVE,PNULL,0);
            gsensor_int_ticks = cur_ticks;
        }
        gsensor_int_status = 1;
    }
    return 0;
}

//静止
int ZDT_GSensor_Still(void)
{    
    if(gsensor_int_status == 1)
    {
        //APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_STILL,PNULL,0);
        gsensor_int_status = 0;
    }
    return 0;
}

int ZDT_GSensor_Fail(void)
{    
    //跌落
    APP_SendSigTo_GSensor(SIG_ZDT_GSENSOR_FAIL,PNULL,0);
    return 0;
}

uint32 ZDT_GSensor_GetStep(void)
{    
    return gsensor_step_num;
}

BOOLEAN ZDT_GSensor_GetStill(void)
{    
    return gsensor_is_still;
}

BOOLEAN ZDT_GSensor_HW_IsOK(void) 
{
    return gsensor_is_init;
}

ZDT_GSENSOR_IC_TYPE_E ZDT_GSensor_GetType(void)
{
    return s_gsensor_type;
}

uint32 ZDT_GSensor_GetStepOneDay(void) 
{
    #ifdef WIN32
        return 2800;
    #else
        uint32 one_day_step = s_jp_one_daly_steps; //*100/100; //  84/100- 100/100 // wuxx for ZTE 20231219
        return one_day_step;
    #endif
}

void ZDT_GSensor_SetStepOneDay(uint32 val) 
{
    s_jp_one_daly_steps = val;
    s_jp_one_daly_start_steps = val;
    ZDT_GSensor_Reset();
    return;
}

void ZDT_GSensor_RestoreStepOneDay(uint32 val) 
{
    s_jp_one_daly_start_steps = val;
    s_jp_one_daly_steps = val;
    return;
}


#endif/*_ZDT_GSENSOR_C*/


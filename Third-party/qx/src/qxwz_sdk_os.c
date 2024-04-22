#include <stdarg.h>
#include <stdio.h>

#include "qxwz_sdk_os.h"

/*spreadtrum header file*/
#include "dal_time.h"
#include "os_api.h"
static int malloc_free_times = 0;
static long long g_cur_time = 0;
static unsigned int g_cur_tick = 0;
unsigned int qx_os_msleep(unsigned int time)
{
    SCI_Sleep(time);

    return 0;
}

/*spreadtrum get current time*/
/*TODO: obsolete，to delete*/
long long getCurrentTime()
{
	return qx_os_gettime();
}
long long qx_os_gettime()
{
    unsigned int tick = SCI_GetTickCount();
    if(g_cur_time == 0){
        return 0;
    }
    #if 0
     long long seconds = TM_GetTotalSeconds();
     return seconds * 1000;
     #else
     if(tick > g_cur_tick){
        g_cur_time = g_cur_time+(tick-g_cur_tick);
        g_cur_tick = tick;
     }else{
        g_cur_time = g_cur_time+(0xFFFFFFFFu-g_cur_tick)+tick;
        g_cur_tick = tick;
     }
     #endif

     return g_cur_time;
}

/*TODO: obsolete，to delete*/
void setCurrentTime(char *tm)
{
     g_cur_time = atoll(tm);
     g_cur_tick = SCI_GetTickCount();
}

void qx_os_settime(unsigned long long tm)
{
     //g_cur_time = atoll(tm);
     g_cur_time = tm;
     g_cur_tick = SCI_GetTickCount();
}

void *qx_os_malloc(unsigned int size)
{
    return SCI_ALLOC(size);
}

void qx_os_free(void *p)
{
    SCI_FREE(p);
    p = NULL;
}

void qx_os_memcpy(void *dst, const void *src, unsigned int size)
{
    SCI_MEMCPY(dst, src, size);
}

void qx_os_memset(void *dst, int value, unsigned int size)
{
    SCI_MEMSET(dst, value, size);
}
int qx_os_get_memory_log()
{
    return malloc_free_times;
}

//TODO
int qx_os_printf(const char *format, ...)
{
    #define LOG_LENGTH 4096

    char    log_buf[LOG_LENGTH + 2] = { 0 };

    va_list args;
    va_start(args, format);
    vsnprintf(log_buf, LOG_LENGTH, format, args);

    va_end(args);
    SCI_TRACE_LOW("%s", log_buf);
}


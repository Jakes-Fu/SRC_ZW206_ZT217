#include "port_cfg.h"
#include "helper_stamp.h"

#ifdef REAL_WATCH_RTOS
#include "dal_time.h"

// UTC/GMT+08:00, Beijing, China
static unsigned int localtime_to_seconds()
{
	SCI_DATE_T  sys_date = { 0 };
	SCI_TIME_T  sys_time = { 0 };
	int year, mon, days;

	TM_GetSysDate(&sys_date);
	TM_GetSysTime(&sys_time);

	year = sys_date.year;
	mon = sys_date.mon;
	days = 0;

	if ((mon -= 2) <= 0) {
		mon += 12;
		year -= 1;
	}

	days = (year - 1) * 365 + year / 4 - year / 100 + year / 400 + (367 * mon / 12) - 30 + 59 + (sys_date.mday - 1) - 719162;
	return (((unsigned int)((days * 24 + sys_time.hour - 8) * 60 + sys_time.min) * 60) + sys_time.sec);
}

sds timestamp_nowtime()
{
	sds stamp;
	SCI_DATE_T  sys_date = { 0 };
	SCI_TIME_T  sys_time = { 0 };

	TM_GetSysDate(&sys_date);
	TM_GetSysTime(&sys_time);

	stamp = sdscatprintf(sdsempty(), "%d%02d%02d%02d%02d%02d",
		sys_date.year, sys_date.mon, sys_date.mday,
		sys_time.hour, sys_time.min, sys_time.sec);

	return stamp;
}

sds timestamp_nowday()
{
	sds stamp;
	SCI_DATE_T  sys_date = { 0 };

	TM_GetSysDate(&sys_date);
	stamp = sdscatprintf(sdsempty(), "%d%02d%02d",
		sys_date.year, sys_date.mon, sys_date.mday);

	return stamp;
}

sds timestamp_ms()
{
	sds stamp;
	stamp = sdscatprintf(sdsempty(), "%d000", localtime_to_seconds());
	return stamp;
}

sds timestamp_simple_date_format(sds* tzCode)
{
	sds stamp;
	SCI_DATE_T  sys_date = { 0 };
	SCI_TIME_T  sys_time = { 0 };

	TM_GetSysDate(&sys_date);
	TM_GetSysTime(&sys_time);

	stamp = sdscatprintf(sdsempty(), "%d-%02d-%02dT%02d:%02d:%02dZ",
		sys_date.year, sys_date.mon, sys_date.mday,
		sys_time.hour, sys_time.min, sys_time.sec);

	if (tzCode != NULL) {
		*tzCode = sdscatprintf(sdsempty(), "%d-%02d-%02dT%02d%%3A%02d%%3A%02dZ",
			sys_date.year, sys_date.mon, sys_date.mday,
			sys_time.hour, sys_time.min, sys_time.sec);
	}
	return stamp;
}

unsigned int timestamp_seconds_int()
{
	return localtime_to_seconds();
}

unsigned int timestamp_tz_str_to_time(const char* str)
{
	int year, mon, days, hour, minute, seconds;

	if (str == NULL || os_strlen(str) != os_strlen("2021-06-01T07:40:51Z"))
		return 0;

	// 2021-06-01T07:40:51Z
	os_sscanf(str,
		"%d-%d-%dT%d:%d:%dZ",
		&year, &mon, &days, &hour, &minute, &seconds);

	if ((mon -= 2) <= 0) {
		mon += 12;
		year -= 1;
	}

	days = (year - 1) * 365 + year / 4 - year / 100 + year / 400 + (367 * mon / 12) - 30 + 59 + (days - 1) - 719162;
	return (((unsigned int)((days * 24 + hour - 8) * 60 + minute) * 60) + seconds);
}

#endif

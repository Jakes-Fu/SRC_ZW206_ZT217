MINCPATH += BASE/l4/export/inc
ifeq ($(strip $(SFR_SUPPORT_CMCC)), TRUE)
ifeq ($(strip $(IPVERSION_SUPPORT)), V4_V6)
MINCPATH   += DAPS/export/inc/tcpip6
else
MINCPATH   += DAPS/export/inc/tcpip
endif
MINCPATH += DAPS/source/oauth/inc

MINCPATH += ms_mmi_main\source\mmi_app\common\cjson
MINCPATH += MS_MMI_Main/source/mmi_app/app/selfreg_cmcc/h \
            MS_MMI_Main/source/mmi_app/app/selfreg/h \
			Third-party/lwm2m/richinfo-lwm2msdk-M-v2.0/core \
            Third-party/lwm2m/richinfo-lwm2msdk-M-v2.0/sdksrc/sdk \
			external/libc/h \
            external/libc/h/sys \
            external/libc/ \
			rtos/export/inc

MINCPATH += ual/inc/tele
MINCPATH += ual/inc

MSRCPATH += MS_MMI_Main/source/mmi_app/app/selfreg_cmcc/c
SOURCES += mmisfr_cmcc_main.c  mmisfr_cmcc_lwm2m.c
endif

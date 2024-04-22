#Preprocessor
MCFLAG_OPT = --c99

#Directory
MINCPATH = DAPS/export/inc
# MINCPATH += MS_Ref/export/inc

ifeq ($(strip $(IPVERSION_SUPPORT)), V4_V6)
MINCPATH   += DAPS/export/inc/tcpip6
MCFLAG_OPT += -DIPV6_SUPPORT
else
MINCPATH   += DAPS/export/inc/tcpip
endif

#Local Dir inc
MINCPATH += external/oneos/inc \
            external/oneos/inc/kernel \
            external/oneos/inc/sys \
            external/oneos/inc/fal    \
            chip_drv/export/inc \
            chip_drv/export/inc/outdated \
            chip_drv/chip_plf/ANTISW3 \
            MS_MMI_Main/source/mmi_app/app/pdp_6121E/h \
            MS_MMI_Main/source/mmi_app/app/mrapp/h \
            MS_MMI_Main/source/mmi_app/app/audio/h \
            BASE/l4/export/inc \
            BASE/sim/export/inc \
            BASE/PS/export/inc \
            MS_MMI_Main/source/mmi_app/app/connection/h \
            MS_MMI_Main/source/mmi_kernel/include \
            CAF/Template/include \
            MS_MMI_Main/source/mmi_app/kernel/h \
            BASE/atc/export/inc \
            MS_MMI_Main/source/mmi_utility/h \
            MS_MMI_Main/source/mmi_app/app/phone/h \
            MS_MMI_Main/source/mmi_app/app/theme/h \
            MS_MMI_Main/source/mmi_app/app/setting/h \
            MS_MMI_Main/source/mmi_app/app/wallpaper/h \
            MS_MMI_Main/source/mmi_app/app/mainmenu/h \
            MS_MMI_Main/source/mmi_app/app/parse/h \
            MS_MMI_Main/source/mmi_app/common/h \
            MS_MMI_Main/source/resource \
            MS_MMI_Main/source/mmi_gui/include \
            MS_MMI_Main/source/mmi_ctrl/include \
            MS_MMI_Main/source/mmi_service/export/inc \
            Third-party/oneos \
            Third-party/oneos/ota/cmiot/source/inc

MINCPATH += build/$(PROJECT)_builddir/tmp
MINCPATH += $(MMI_DIR)/source/mmi_app/app/otapush/h

MINCPATH += ual/inc
MINCPATH += ual/inc/bt
MINCPATH += ual/inc/tele

ifneq ($(strip $(WIFI_SUPPORT)), NONE)
    MINCPATH += MS_MMI_Main/source/mmi_app/app/wlan/h
    MINCPATH += MS_Customize/source/product/driver/wifi/wifisupp
    MINCPATH += MS_MMI_Main/source/mmi_service/source/mmisrvwifi/h
endif

#Local Dir src
MSRCPATH = external/oneos/src/kernel \
           external/oneos/src/fal \
		   external/oneos/src


#SOURCES = oneos_socket.c   oneos_fs_posix.c

# SOURCES = oneos_socket.c   oneos_fs_posix.c \
#           oneos_clock.c oneos_event.c oneos_msg.c \
# 		  oneos_mutex.c oneos_sem.c oneos_task.c oneos_timer.c os_sem.c os_mutex.c os_clock.c
SOURCES = os_assert.c os_memory.c os_task.c os_util.c  ring_buff.c socket.c os_fal_part.c cmiot_net.c cmiot_fota_process.c

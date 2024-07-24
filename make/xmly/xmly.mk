ifeq ($(strip $(SPRD_APP_RELEASE)), FALSE)
include make/app_main/app_macro.mk
else
include make/app_main/release_app_macro.mk
endif

include make/gui_main/gui_feature.mk


MINCPATH  +=  $(MMI_DIR)/source/mmi_service/export/inc
MINCPATH += build/$(PROJECT)_builddir/tmp
MINCPATH += $(MMI_DIR)/source/mmi_app/kernel/h
MINCPATH += $(MMI_DIR)/source/mmi_app/common/h
MINCPATH += $(MMI_DIR)/source/mmi_kernel/include
MINCPATH += CAF/Interface/source/resource
MINCPATH += CAF/Template/include
MINCPATH += $(MMI_DIR)/source/mmi_utility/h
MINCPATH += DAPS/source/httpc/h
MINCPATH += $(MMI_DIR)/source/mmi_ctrl/include
MINCPATH += $(MMI_DIR)/source/mmi_gui/include
MINCPATH += $(MMI_DIR)/source/mmi_app/app/pdp/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/phone/h
MINCPATH += $(MMI_DIR)/source/resource
MINCPATH += $(MMI_DIR)/source/mmi_app/app/connection/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/fmm/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/setting/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/parse/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/theme/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/keylock/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/wallpaper/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/im/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/mainmenu/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/envset/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/cc/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/cl/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/pb/h
MINCPATH += $(MMI_DIR)/source/mmi_app/app/search/h
MINCPATH += chip_drv/export/inc/outdated

ifeq ($(strip $(IM_ENGINE)), IEKIE)
MINCPATH += Third-party/iekie/inc
else
MINCPATH += Third-party/cstar/include
endif

MINCPATH += $(ATC_INCPATH)
MINCPATH += $(L4_INCPATH)
MINCPATH += $(SIM_INCPATH)
MINCPATH += $(PS_INCPATH)
MINCPATH += $(L1_INCPATH)


ifeq ($(strip $(XYSDK_SRC_SUPPORT)), TRUE)
MINCPATH += $(MMI_DIR)/source/mmi_app/app/xysdk/h
MINCPATH += Third-party/xmly/api/h
MINCPATH += Third-party/xmly/xysdk/h


MSRCPATH += Third-party/xmly/xysdk/c

SOURCES	 += xysdk_main.c xysdk_id.c xysdk_nv.c xysdk_catelist.c xysdk_pdp.c xysdk_hmac_sha1.c xysdk_cJSON.c xysdk_md5.c xysdk_http_task.c\
			xysdk_rbuf.c xysdk_strm_task.c xysdk_http_api.c xysdk_albumlist.c xysdk_player.c xysdk_playlist.c xysdk_user.c 	xyos_voice.c xysdk_security_chain.c	xysdk_subject.c   
endif




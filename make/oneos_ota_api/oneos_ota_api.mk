#Preprocessor
#MCFLAG_OPT = --gnu
MCFLAG_OPT = --c99

#Directory
MINCPATH = external/oneos/inc/kernel \
           external/oneos/inc/fal \
           external/oneos/inc/ \
           Third-party/oneos/ota/cmiot/port \
           Third-party/oneos \
           BASE/l4/export/inc \
           BASE/sim/export/inc \
           BASE/PS/export/inc
        #    Third-party/oneos/ota/cmiot_src \

        #    MS_MMI_Main/source/mmi_kernel/include\
        #    CAF/Template/include\
        #    MS_MMI_Main/source/mmi_utility/h\
        #    MS_MMI_Main/source/mmi_app/app/pdp/h \
        #    MS_MMI_Main/source/mmi_app/app/connection/h \
        #    BASE/l4/export/inc \
        #    MS_MMI_Main/source/mmi_app/kernel/h \
        #    BASE/sim/export/inc\
        #    BASE/PS/export/inc\
        #    BASE/atc/export/inc\
        #    MS_MMI_Main/source/mmi_app/common/h \
        #    MS_MMI_Main/source/resource \

#Local Dir inc
MINCPATH += Third-party/oneos/ota/cmiot/source/inc

#Local Dir src
MSRCPATH = Third-party/oneos/ota/cmiot/source/src \
           Third-party/oneos/ota/cmiot/port/oneos

SOURCES = cmiot_fal.c cmiot_sys.c cmiot_cfg.c cmiot_id.c cmiot_main.c

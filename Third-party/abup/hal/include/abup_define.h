#ifndef _ABUP_DEFINE_H_
#define _ABUP_DEFINE_H_

#define ADUPS_FOTA_SERVICE_OEM			"8910FF"
#define ADUPS_FOTA_SERVICE_MODEL         "TEST01"
#define ADUPS_FOTA_SERVICE_PRODUCT_ID 		"1595793597"
#define ADUPS_FOTA_SERVICE_PRODUCT_SEC 		"2cc696dc93934bf880fc65da1c1736f9"
#define ADUPS_FOTA_SERVICE_DEVICE_TYPE 	"Feature_phone"
#define ADUPS_FOTA_SERVICE_PLATFORM 	    "SC8910FF"


#define ADUPS_FOTA_SDK_VER "IOT4.0_R41388"
#define ADUPS_FOTA_APP_VER "ABUP_V4.0"
#define ADUPS_FOTA_SDK_MAX_MCU_NUM    5

#define ADUPS_FOTA_FILENAME1 "mmi_res_240x320.bin-D"
#define ADUPS_FOTA_ADDRESS1  "0x90000004"
#define ADUPS_FOTA_FILENAME2 "nvitem.bin-F"
#define ADUPS_FOTA_ADDRESS2  "0x90000001"
#define ADUPS_FOTA_FILENAME3 "UIS8910_240x320BAR_16MB_DS_DEBUG_delta_nv.bin-D"
#define ADUPS_FOTA_ADDRESS3  "0x90000100"
#define ADUPS_FOTA_FILENAME4 "UIX8910_UIS8910_240x320BAR_16MB_DS_DEBUG_cp.bin-D"
#define ADUPS_FOTA_ADDRESS4  "0x90000101"
#define ADUPS_FOTA_FILENAME5 "UIX8910_UIS8910_240x320BAR_16MB_DS_DEBUG_stone.bin-D"
#define ADUPS_FOTA_ADDRESS5  "0x80000003"


#define ADUPS_FOTA_REGION_LEN "5120"
#define ADUPS_FOTA_BLOCK_LEN "64"
#define ADUPS_FOTA_PATCH_FORMAT "1"

#define ABUP_MAX_DELTA_NUM 5
#define ABUP_PACKAGE_AREA_BASE 0x00D00000
#define ABUP_PACKAGE_AREA_LEN  0x00300000
#define ABUP_BLOCK_SIZE  64*1024
#define ABUP_LCD_NONE
#define ADUPS_DEVICE_NETWORKTYPE "2G"


#define ABUP_REPORT_DL

#endif

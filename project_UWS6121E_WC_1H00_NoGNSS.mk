
# *************************************************************************************************************************************
#  易赛 增加的宏添加到这里
# 
# *************************************************************************************************************************************
ZDT_RAM_MEM_CUST = TRUE					## 精减内存
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZYB_APP_SUPPORT = TRUE                                 ### 流媒体播放接口，微聊使用
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZDT_PCBA_ZW201_SUPPORT = TRUE                                 ### ZDT 主板驱动宏控
                                                     # Option1:TRUE
                                                     # Option2:FALSE
						     
# 基于GNSS的PATCH ,为了代码NOGNSS共用,注意 GNSS版本的添加宏控制
#W217_UWS6121EG_GNSS_PATCH_20231019 = TRUE
						                                                          


#UI界面宏义开始
_SW_ZDT_PRODUCT_ = TRUE                                 ### 使用ZDT的UI
ZDT_NEW_BIND_STYLE = FALSE  				### 新的绑定流程。  第一步提示下载app，第二步骤提示扫IMEI号绑定。
ZDT_MMI_USE_SMALL_FONT  = FALSE  			### 小字体支持 128x128可用
ADD_TITLE_IN_ALL_SETTING_MENU = TRUE
#UI界面宏义结束
#第三方应用开始

ZDT_ZFB_SUPPORT = FALSE                                  ### 支付宝 需要开启 ZDT_HTTP_APP_SUPPORT
TULING_AI_SUPPORT = FALSE				### 图灵AI						     
BAIRUI_VIDEOCHAT_SUPPORT = FALSE				### 百锐视频通话
LEBAO_MUSIC_SUPPORT = FALSE				### 乐宝音乐
XYSDK_SUPPORT = FALSE					### 喜马拉雅
XYSDK_SRC_SUPPORT = FALSE				### 喜马拉雅源码支持

BAIDU_VIDEOCHAT_SUPPORT = TRUE				### 百度视频通话
BAIDU_AI_SUPPORT = TRUE				### 百度AI
#第三方应用结束


SOS_KEYLONGWEB = TRUE                                 ### SOS 呼叫
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                                                                        
ZDT_RTMP_SUPPORT = FALSE                                 ### 流媒体播放
                                                     # Option1:TRUE
                                                     # Option2:FALSE
						     
ZDT_PLAY_RING_CUSTOM = FALSE                                 ### 自定义音频播放
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
ZDT_KEYLOCK_INT_SUPPORT = FALSE                                 ### 侧键锁
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
SIM_LOCK_CT_SUPPORT = FALSE                                 ### 锁SIM卡
                                                     # Option1:TRUE
                                                     # Option2:FALSE

FILE_LOG_SUPPORT = FALSE                                 ### 文件Log支持
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZDT_APP_SUPPORT = TRUE					### 易赛应用总宏
                                                     # Option1:TRUE
                                                     # Option2:FALSE
ZDT_WIFI_SUPPORT = TRUE					### WIFI 支持
                                                     # Option1:TRUE
                                                     # Option2:FALSE
ZDT_GPS_SUPPORT = FALSE					### GPS 支持
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
ZDT_GPS_SUPPORT_AGPS = FALSE				### GPS 辅助定位
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZDT_GSENSOR_SUPPORT = TRUE#FALSE			      ### 重力加速度传感器
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZDT_SMS_SUPPORT = TRUE 	     ### 短信支持
												  # Option1:TRUE
												  # Option2:FALSE
																										  
ZDT_NET_SUPPORT = TRUE					      ### 4G网络支持
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
ZDT_NET_SUPPORT_FLYMODE_RESET = FALSE					      ### 复位平台网络时需要开启关闭飞行模式
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZDT_UDP_SUPPORT = FALSE					      ### UDP 协议支持
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     
ZDT_TCP_SUPPORT = TRUE					      ### TCP 协议支持
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZDT_HTTP_APP_SUPPORT = TRUE					      ### HTTP 协议支持
                                                     # Option1:TRUE
                                                     # Option2:FALSE
ZDT_PLAT_YX_SUPPORT = TRUE					      ### 易赛通信后台配置,需要开启 ZDT_NET_SUPPORT ZDT_TCP_SUPPORT ZDT_HTTP_APP_SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ifeq ($(strip $(ZDT_PLAT_YX_SUPPORT)),TRUE)						     
####************后台支持列表，同时只能开启一个************
ZDT_PLAT_YX_SUPPORT_FZD = FALSE					      ### 防走丢
ZDT_PLAT_YX_SUPPORT_YS = TRUE					      ### 易赛安康守护
ZDT_PLAT_YX_SUPPORT_YS_AES = TRUE					      ### AES CBC 加密传输支持
ZDT_PLAT_YX_SUPPORT_TX = FALSE					      ### 音信百合后台
ZDT_PLAT_YX_SUPPORT_LZ = FALSE					      ### 乐智后台
ZDT_PLAT_YX_SUPPORT_CY = FALSE					      ### 成研云南移动后台

ZDT_PLAT_YX_SUPPORT_ZTE = TRUE					      ### 易赛改ZTE NUBIA 中兴努比亚后台

ifeq ($(strip $(ZDT_PLAT_YX_SUPPORT_LZ)),TRUE)
ZDT_PLAT_YX_SUPPORT_VOICE = FALSE				### 微聊
else
ifeq ($(strip $(ZDT_PLAT_YX_SUPPORT_CY)),TRUE)
ZDT_PLAT_YX_SUPPORT_VOICE = FALSE				### 微聊
else
ZDT_PLAT_YX_SUPPORT_VOICE = TRUE				### 微聊
endif
endif 

ZDT_PLAT_YX_SUPPORT_FRIEND = TRUE					      ### 碰碰交友
                                                     
ZDT_PLAT_YX_SUPPORT_SOS = TRUE				### SOS设置
endif

ZDT_VIBRATE_SUPPORT = TRUE					### 振动

# 7789P3 240X284
#DRV_LCD_7789P3_240X284 = TRUE
# TP 6133 series
DRV_TP_6133_ZW206_K1_240X284 = FALSE

#MMI LCD SIZE
ZTE_SUPPORT = 240X284								 ### ZTE_SUPPORT:
													 # Option1:NONE
                                                     # Option2:240X240 	#define ZTE_SUPPORT_240X240
                                                     # Option3:240X284 	#define ZTE_SUPPORT_240X284

SETUP_WIZARD_SUPPORT = TRUE         #开机向导

#######主界面配置 3选1########
LAUNCHER_ONE_APP_IN_PAGE = FALSE    #一页一个应用
LAUNCHER_FOUR_APP_IN_PAGE  = TRUE  #一页4个应用 4宫格
LAUNCHER_ALL_APP_IN_PAGE = FALSE    #一页显示所有应用
#######主界面配置 3选1########

ZDT_NET_2G_SUPPORT = FALSE   #是否支持2G网络 节省资源空间
ZDT_NET_3G_SUPPORT = FALSE	#是否支持3G网络 节省资源空间
ZDT_NET_WIFI_SUPPORT = FALSE  #是否支持WIFI网络 节省资源空间
ZDT_MAIN_MENU_SUPPORT = FALSE #旧的菜单资源，节省空间

AUTO_FACTORY_TEST_WITH_WIFI_SCAN = FALSE #扫描到指定wifi ssid自动启动工厂测试 还没实现

ZDT_VIDEOCHAT_SUPPORT = TRUE  #视频通话
###VIDEO_CALL_AGORA_SUPPORT = TRUE #声网视频通话
#声网视频在相机界面来电时,接听挂断会死机
VIDEO_CALL_AGORA_IN_CAMERA_SCREEN_INCOMMING_ERR = TRUE


# 工具箱菜单,可以放菜单用于展示测试
ZDT_TOOLS_MENU_SUPPORT = TRUE

ZTE_WATCH = TRUE
ifeq ($(strip $(ZTE_WATCH)),TRUE)
LAUNCHER_ONE_APP_IN_PAGE = FALSE    #一页一个应用
LAUNCHER_FOUR_APP_IN_PAGE  = FALSE  #一页4个应用 4宫格
LAUNCHER_ALL_APP_IN_PAGE = TRUE    #一页显示所有应用
ZTE_WATCH_K1 = TRUE  #努比亚手表K1

LOCAL_ALARM_CLOCK_SUPPORT = TRUE    ###本地闹钟
#HOME_LOCATION_INQUIRE_SUPPORT = CODE


ZTE_PEDOMETER_SUPPORT = TRUE		###计步

ZTE_WEATHER_SUPPORT = TRUE			###天气

ZTE_STOPWATCH_SUPPORT = TRUE              			      ### 秒表

endif

ifeq ($(strip $(LAUNCHER_ALL_APP_IN_PAGE)),TRUE)
APP_COLUMNS = 2     #菜单列表有多少列 
                    # Option1:2    #define APP_COLUMNS_2
                    # Option2:3    #define APP_COLUMNS_3
APP_ITEM_NAME = TRUE #是否显示应用名称

ifeq ($(strip $(APP_COLUMNS)),3)
APP_ITEM_NAME = FALSE
endif

endif
#MAINLCD_DEV_SIZE_240X284 = FALSE
MAINLCD_DEV_SIZE_240X320 = TRUE

# BF20A6 camera
DRV_CAMERA_BF20A6_CUSTOM = TRUE

#AUDIO PARAM 音频参数控制, Nv_parameter.mk
ZW206_AUDIO_PARAM = TRUE

# modem config for ZW206 ZW202 
ZDT_MODEM_CONFIG_XO_MODE_FOR_ZW206_ZW202 = FALSE

# W217 WIFI TEST
ZDT_W217_FACTORY_WIFI = TRUE

# W217 ZW206 KEY TEST
ZDT_W206_FACTORY_KEY_TEST = TRUE

# W217 GPS TEST, AND NET DATA
ZDT_W217_FACTORY_GPS = FALSE
#静止时不启动周期定位
ZDT_NO_LOC_LBS_WIFI_OR_GPS_WHEN_STILLNESS = TRUE


#持久测试, 老化测试
W217_AGING_TEST_CUSTOM = TRUE

# OCR 语音播放, 可做为DEBUG 调音频参数用
###MMI_OCR_AUDIO_PLAY_TEST_LONG = TRUE

#Shipping mode, 船运模式指令 "#*880088#"
ZDT_SHIPPING_MODE_CMD = TRUE


#指令解除绑定 #*915368#
ZDT_UNBIND_APP_CMD = TRUE



# ZW206 资源控制,未插SIM 卡提示图片等
ZDT_ZW206_240X284_RES_CUSTOM = FALSE

# 未插卡提示图片用动画
ZDT_NO_SIM_ICON_GIF = FALSE

#HOME_LOCATION_INQUIRE_SUPPORT = CODE

#w217 表盘1

#############至少一个为TRUE############## ZTE有自己的表盘不需要公版表盘
WATCH_PANEL_SPACE_SHIP_SUPPORT = FALSE     #宇宙飞船数字表盘
WATCH_PANEL_DOLPHIN_SUPPORT = FALSE        #海豚
WATCH_PANEL_ASTRONAUT_SUPPORT = FALSE      #太空人
WATCH_PANEL_PANDA_SUPPORT = FALSE          #熊猫
WATCH_PANEL_BLACK_GREEN_SUPPORT = FALSE    #黑绿色模拟时钟
WATCH_PANEL_DEER_SUPPORT = FALSE           #小鹿
WATCH_PANEL_SPACE_STATION_SUPPORT = FALSE  #空间站宇航员
#############至少一个为TRUE##############

POWER_OFF_CHARGE_TEXT_SHOW = FALSE  #关机充电显示电量
BATTERY_CHARGE_FULL_REMAIN = TRUE   #电池充电98显示100%电量
BATTERY_PERCENT_TEXT_SHOW = FALSE   #显示电池电量百分比
WECHAT_SEND_EMOJI = TRUE #支持手表发送表情
KEYUP_RED_GOTO_IDLE = TRUE #按power键返回表盘待机界面
PLAY_NEW_SMS_RING = FALSE #接收短信播放铃声或者震动



# *************************************************************************************************************************************
#  大部分公共的配置选项在 uws6121e_watch_base_config.cfg 中，
#  All the common configuration is set in uws6121e_watch_base_config.cfg 
# *************************************************************************************************************************************
MMI_MULTI_SIM_SYS = SINGLE                             ### Platform Support Multi Sim Card Number
                                                     # Option1:SINGLE    #define MMI_MULTI_SIM_SYS_SINGLE
                                                     # Option2:DUAL      #define MMI_MULTI_SIM_SYS_DUAL
include uws6121e_watch_base_config.cfg
include 217sw_patch.mk
# *************************************************************************************************************************************
# 如果有差异的话，需要从uws6121e_watch_base_config.cfg提取出来，放到各自project_xxx.mk的这段注释的后面，
# 注意：
#    (1)目前不支持有重复宏定义，比如SRAM_SIZE = 128MBIT，然后后面有出现一处SRAM_SIZE = 256MBIT，两种不同的定义。
#    (2)这段注释的后面的宏定义不能和uws6121e_watch_base_config.cfg 中的重复，如果有重复需要从uws6121e_watch_base_config.cfg 中拿出来。
#    (3)RVCT_VERSION = RVCT41 或者 RVCT_VERSION = V504 不再出现在project_xxx.mk中，需要编译的时候手动在CMD中设置set CC_VER=RVCT41
#       或者set CC_VER=V504，如果不设置的话，在makefile.rules中会默认设置成RVCT_VERSION = RVCT41。
# Notice:
#    (1). Do not support multi-definition of one micro, such as define SRAM_SIZE = 128MBIT at first, then define SRAM_SIZE = 256MBIT in the end.
#    (2). The micro delow this comments should not re-defined with the micros in uws6121e_watch_base_config.cfg. If there happened re-defined, you
#         should seprate the micro from  uws6121e_watch_base_config.cfg and add it to individual project_xx.mk
#    (3). The difinition of RVCT_VERSION is not set anymore. You should set environment value in CMD terminator, such as set CC_VER=V504.
#         If you do not set CC_VER , the default value RVCT_VERSION = RVCT41 will be set in makefile.rules.
# *************************************************************************************************************************************
# override for 128*128 LCD size
MAINLCD_DEV_SIZE = 240X320                           ### Resolution Of Main LCD Device,  MAINLCD_SIZE is the size of MMI
                                                     # Option1:128X160    #define MAINLCD_DEV_SIZE_128X160
                                                     # Option2:160X128    #define MAINLCD_DEV_SIZE_128X160
                                                     # Option3:176X220    #define MAINLCD_DEV_SIZE_176X220
                                                     # Option4:220X176    #define MAINLCD_DEV_SIZE_220X176
                                                     # Option5:240X320    #define MAINLCD_DEV_SIZE_240X320
                                                     # Option6:320X240    #define MAINLCD_DEV_SIZE_320X240

MAINLCD_SIZE = 240X320                               ### Resolution Of Main LCD:
                                                     # Option1:240X320    #define MAINLCD_SIZE_240X320
                                                     # Option2:176X220    #define MAINLCD_SIZE_176X220
                                                     # Option3:128x160    #define MAINLCD_SIZE_128X160

MMI_RES_DIR = mmi_res_240x240_child_circular         ### resource directory
                                                     # Option1:mmi_res_128x160
                                                     # Option2:mmi_res_176x220
                                                     # Option3:mmi_res_240x320
                                                     # Option4:mmi_res_240x320_mini
                                                     # Option5:mmi_res_240x240_child_circular
MMI_DIR  = MS_MMI_Main															 

FLASH_SIZE = 128MBIT                         		 ### FLASH_SIZE SIZE
                                                     # Option1:16MBIT     #define FLASH_SIZE_16MBIT
                                                     # Option2:32MBIT     #define FLASH_SIZE_32MBIT
                                                     # Option3:64MBIT     #define FLASH_SIZE_64MBIT
                                                     # Option4:128MBIT    #define FLASH_SIZE_128MBIT
						                             # Option5:128MBITX64MBIT    #define FLASH_SIZE_128MBITX64MBIT_OLD
													 # Option6:128MBITX64MBIT_NEW #define FLASH_SIZE_128MBITX64MBIT_NEW

MULTI_FLASH_SUPPORT = FALSE		

#FLASH_SIZE_128MBITX32MBIT = TRUE				 	 # Option1: TRUE FLASH(INTERNAL 16MB + EXTERNAL 4MB)	
													 # Option2: FALSE FLASH(INTERNAL 16MB ONLY)	
											 										 
                                                     
SRAM_SIZE = 128MBIT                                   ### SRAM SIZE
                                                     # Option1:16MBIT     #define SRAM_SIZE_16MBIT
                                                     # Option2:32MBIT     #define SRAM_SIZE_32MBIT
                                                     # Option3:64MBIT     #define SRAM_SIZE_64MBIT
                                                     # Option4:128MBIT     #define SRAM_SIZE_128MBIT
                                                     # Option4:256MBIT     #define SRAM_SIZE_256MBIT
													 
AP_CP_RAM_SIZE = 40X88                               ##APXCP RAM SIZE 
                                                     # Option1:40MBITX88MBIT     #define AP_CP_RAM_SIZE_40X88
													 # Option1:24MBITX104MBIT     #define AP_CP_RAM_SIZE_24X104
                                                   

_USE_3_LINE_2_DATD_ = FALSE                          ### lcd 3 line 2 date support
                                                     # Option2:FALSE
													 
													 
PRODUCT_CONFIG = uws6121e                            ### MS_Customize/source/product/config;scf file;nv;resource
                                                     # default: project name 

PRODUCT_BASELIB_DIR = $(PROJECT)                     ### Dir name for base lib in MS_Code\lib\
                                                     # default: project name 
                                                     

													 
BT_LOG_MENU_SUPPORT = FALSE                          ### BT LOG Menu Select Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE 
                                                     
                                                     
PLATFORM = UWS6121E                                    ### Chip Platform Definition:
                                                     # Option1:SC6530     #define PLATFORM_SC6530
                                                     # Option2:SC6531EFM  #define PLATFORM_SC6531EFM
                                                     # Option2:UIX8910    #define PLATFORM_UIX8910
													 # Option3:UWS6121E   #define PLATFORM_UWS6121E
                                                     
CHIP_VER = UWS6121E                                     ### Chip version:
                                                     # Option1:6800H1     #define CHIP_VER_6800H1
                                                     # Option2:6800H3     #define CHIP_VER_6800H3
	                                                 # Option3:6531EFM    #define CHIP_VER_6531EFM												 
                                                     # Option4:UIX8910MPW    #define CHIP_VER_UIX8910MPW
													 # Option5:UIX8910    #define CHIP_VER_UIX8910
													 # Option5:UIS8910C    #define CHIP_VER_UIS8910C
                                                     # Option6:UIS8910FF    #define CHIP_VER_UIS8910FF
                                                     # Option7:UWS6121E    #define CHIP_VER_UWS6121E

#modem bin folder is defined by two configures: MODEM_BIN_FILE/MODEM_PROJECT_FOLDER													 
MODEM_BIN_FILE = UWS6121E_MODEM                          ### modem bin cache FOLDER
                                                     # Option1:UWS6121E_MODEM
													 
MODEM_PROJECT_FOLDER = UWS6121_cat1bis_volte_nognss      ### modem PROJECT cache FOLDER
                                                     # Option1:UWS6121_cat1bis_volte_gnss
													 # Option2:UWS6121_cat1bis_volte_nognss
													 
													 
FM_S_ANT_SUPPORT = FALSE                              ###
                                                     # Option0:TRUE     
                                                     # Option1:FALSE                                                    

						     													 
BZP_NO_RESOURCE = TRUE								## resource not zip when bzp_support
													# Option1:TRUE
													# Option1:FALSE                                                

BT_PBAP_SUPPORT = FALSE                              ### Switch Of PBAP Profile Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
													 
AUTOTEST_DEVICE_SUPPORT = TRUE                      ### AUTOTEST_DEVICE_SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE
													 
UMEM_SUPPORT = TRUE                                 ### Switch Of nor flash udisk support
                                                     # Option1:TRUE
                                                     # Option2:FALSE	

UDISK_PC_CONNECT = TRUE                             ### Norflash disk pc access support UMEM_SUPPORT MUST set TRUE
                                                     # Option1:TRUE
                                                     # Option2:FALSE     
													 
SIM_LOCK_SUPPORT = FALSE                             ### SIM Lock Support 
                                                     # Option1:TRUE
                                                     # Option2:FALSE													 
													
SIMULATE_FLASH2_SUPPORT = FALSE

CHIP_ECO_SUPPORT = TRUE

POP_MDY_SUPPORT = TRUE

SECURE_BOOT_SUPPORT = FALSE			     ### Secure Boot Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

SECURITY_DATA_SUPPORT = FALSE			     ### Security Data Support
                                                     # Option1:TRUE
                                                     # Option2:FALSE

VIRTUAL_VIBRATE_FUNC = FALSE                         ### VIRTUAL VIBRATE FUNCTION
                                                     # Option0:TRUE     
                                                     # Option1:FALSE 


RF_BAND_SELFADAPTIVE = FALSE                    		### OPEN/CLOSE RF BAND SELFADAPTIVE FUNCTION
													# Option0:TRUE     
													# Option1:FALSE 
CHGMNG_PSE_SUPPORT = FALSE #TRUE                    		### OPEN/CLOSE CHGMNG_PSE_SUPPORT
													# Option0:TRUE     

DEBUG_VERSION  = TRUE                                ### DEBUG_VERSION:
                                                     # Option1:TRUE
                                                     # Option2:FALSE


RELEASE_FLAG = FALSE                                  ### RELEASE_FLAG
                                                     # Option1:TRUE
                                                     # Option2:FALSE 


                                                     
BZP_SUPPORT  = FALSE                                  ###new memory management 
                                                     # Option1:TRUE
                                                     # Option2:FALSE 	
											 
													 
BLUETOOTH_SUPPORT = NONE                             ### Bluetooth Chip Type: 
                                                     # Option1:NONE      #define BLUETOOTH_SUPPORT_NONE
                                                     # Option2:CSR_BC4_6 #define BLUETOOTH_SUPPORT_CRS_BC4_6
                                                     # Option3:CSR_BC4_6_LOW_MEM: low mem and code size , only support OPP HFG
                                                     # Option4:RTL8723   Realtek RTL8723AS wifi/bt combo chip
                                                     # Option5:IS1662    ISSC IS1662S bt chip
                                                     # Option6:SPRD_BT			#define BLUETOOTH_SUPPORT_SPRD_BT
 

UI_P3D_SUPPORT = TRUE

ROTATE_2D_SUPPORT = TRUE

MMI_3DUI_SUPPORT = TRUE

ANALOG_CLK_SUPPORT = TRUE

IKEYBACKUP_SUPPORT = FALSE

UI_MULTILAYER_SUPPORT = TRUE

BROWSER_SUPPORT = FALSE

BROWSER_SUPPORT_DORADO = FALSE

BROWSER_OPERA_START_PAGE = FALSE

OPERA_MINI_SUPPORT = NONE

DL_SUPPORT = FALSE

MMS_SUPPORT = NONE

VOICE_PROCESS_NEW = TRUE

CVS_ENABLE = TRUE

CVSRCD_ENABLE = TRUE

AMR_ENABLE = TRUE

VOIP_SUPPORT = TRUE

AUD_ENCODE_FADE_INOUT = TRUE

DATACOUNTER_SUPPORT = FALSE

SALES_SUPPORT = FALSE

DC_WATCH_UI_SUPPORT= TRUE

HW_6121_pre_w217 = FALSE

AUDIO_CODEC_TYPE = ES8311                            ### audio codec selection
                                                     # Option1:ES8311     #define ES8311
                                                     # Option2:ES8374     #define ES8374

AUDIO_EXTPA_TYPE = GPIO #AW87390 #                              ### audio codec extpa selection
                                                     # Option1:AW87390      #define AW87390
                                                     # Option2:GPIO         #define GPIO
												 
TOUCHPANEL_TYPE	= MULTITP                               ### Touch panel type
                                                     # Option1:HW            #define TOUCHPANEL_TYPE_HW
                                                     # Option2:NONE          #define TOUCHPANEL_TYPE_NONE
                                                     # Option3:MULTITP       #define TOUCHPANEL_TYPE_MULTITP

TPC_TYPE = NONE                                      ### tpc sampling type
                                                     # Option1:SINGLE  #define TPC_TYPE_SINGLE
                                                     # Option2:DOUBLE  #define TPC_TYPE_DOUBLE
                                                     # Option3:NONE    #define TPC_TYPE_NONE

TP_EXCHANGE = NONE                                   ### tp channel exchange type
                                                     # Option1:XY      #define TP_EXCHANGE_XY
                                                     # Option2:NONE    #define TP_EXCHANGE_NONE

CAP_TP_SUPPORT = BL6133                                ### Capacitive Touch panel chip
                                                     # Option0:NONE          #define CAP_TP_SUPPORT_NONE
                                                     # Option1:FT5206        #define CAP_TP_SUPPORT_FT5206
                                                     # Option2:CY8C21434     #define CAP_TP_SUPPORT_CY8C21434
                                                     # Option3:FT6336     #define CAP_TP_SUPPORT_FT6336
                                                     
TOUCH_PANEL_CTRL_SUPPORT  = TRUE                      ###touch panel only affect ctrl
                                                     # Option1:TRUE
                                                     # Option2:FALSE

TOUCH_PANEL_HWICON_SUPPORT = NONE                    ### touch panel hw support
                                                     # Option1:DIAL_PB_SMS_MAINMENU         #define TOUCH_PANEL_HWICON_SUPPORT_DIAL_PB_SMS_MAINMENU
                                                     # Option2:MAINMENU_PB_SMS_CAMERA_MP4   #define TOUCH_PANEL_HWICON_SUPPORT_MAINMENU_PB_SMS_CAMERA_MP4
                                                     # Option3:MAINMENU_SMS_PB_DIAL_MP3     #define TOUCH_PANEL_HWICON_SUPPORT_MAINMENU_SMS_PB_DIAL_MP3
                                                     # Option4:VIRTUAL_2KEY                 #define TOUCH_PANEL_HWICON_SUPPORT_VIRTUAL_2KEY
                                                     # Option5:NONE

TP_VK_SUPPORT = FALSE                                ### touch panel virtual key driver support
                                                     # Option1:TRUE
                                                     # Option2:FALSE
                                                     # Option2:GPIO         #define GPIO=======
                                                     # Option2:GPIO         #define GPIO

PMIC_CHARGER_DETECT_SUPPORT = FALSE                  ### support charger type detect by pmic
                                                     # Option1:FALSE      #6121E is not support right now
                                                     # Option2:TRUE       #for extend in the future

UAL_GNSS_SUPPORT = FALSE                            ### ual gnss
                                                    # Option1:TRUE
                                                    # Option2:FALSE

AT_CMD_OP_SUPPORT = TRUE                             ### support ATC cmd to test daps function
                                                     # Option1:FALSE      not support
                                                     # Option2:TRUE       support

HTTP2_SUPPORT = TRUE                                 ### HTTP2 Profile Support:
                                                     # Option1:FALSE

GPS_SUPPORT = FALSE                                  ### Switch Of GPS Module Support
                                                     # Option1:FALSE

GPS_CHIP_VER = NONE                                  ### Chip version:
                                                     # Option1:GREENEYE2     #define GPS_CHIP_VER_GREENEYE2
                                                     # Option2:NONE

GPS_TRANS_VIA = NONE				                 ### GPS COMM PORT
                                                     # Option1:NONE
                                                     # Option1:UART
                                                     # Option1:SIPC

GPS_MODE = NONE                                      ### Switch Of GPS Mode
                                                     # Option1:GPS_BD
                                                     # Option2:GPS_GLONASS

GPS_TCARD_LOG_SUPPORT = FALSE                        ### Switch Of GPS TCARD LOG SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE

GPS_MINI_LOG_SUPPORT = FALSE                         ### Switch Of GPS TCARD LOG SUPPORT
                                                     # Option1:TRUE
                                                     # Option2:FALSE

GPS_AGPS_SUPPORT = NONE                              ### Switch GPS AGPS SUPPORT: 
                                                     # Option1:NONE      #define GPS_AGPS_SUPPORT_NONE
                                                     # Option2:QX #define GPS_AGPS_SUPPORT_QX
                                                     # Option3:SPRD_AGPS #define GPS_AGPS_SUPPORT_SPRD_AGPS

SPRD_ENGPC_SUPPORT = FALSE                           ### Switch ENGPC Module Support
                                                     # Option1:TRUE      #define SPRD_ENGPC_SUPPORT
                                                     # Option2:FALSE

ENGPC_W217 = FALSE                                   ### Switch ENGPC_W217 Support
                                                     # Option1:TRUE      #define ENGPC_W217
                                                     # Option2:FALSE	
													 
LCD_INSTALL_ANGLE = 0    #90

MSDC_CARD_SUPPORT = TRUE                             ### SD Card Support:
                                                     # Option1:TRUE
                                                     # Option2:FALSE

ZT217_LISENNING_PAD = TRUE
DRV_TP_6133_ZT217_240X320 = TRUE

DYNAMIC_WATCHFACE_SUPPORT = FALSE	#原表盘商城

ZMT_DIAL_STORE_SUPPORT = TRUE		#表盘商城

MATH_COUNT_SUPPORT = TRUE 			#口算达人

LISTENING_PRATICE_SUPPORT = TRUE	#听力练习

WORD_CARD_SUPPORT = TRUE			#单词卡

POETRY_LISTEN_SUPPORT = TRUE		#古诗词

HANZI_CARD_SUPPORT = TRUE			#汉字卡

ZMT_GPT_SUPPORT = TRUE				#对话GPT

LYRIC_PARSER_SUPPORT = TRUE			#lrc歌词解析

FORMULA_SUPPORT = TRUE				#乘法口诀	

#ifdef JPG_DEC_SUPPORT_JPG_HW
#define IMAGE_LEBAO_JPG_FMT   IMG_CMP_JPG_SCI
#else
#define IMAGE_LEBAO_JPG_FMT   IMG_CMP_JPG_STD
#endif

//[[define image information
RES_ADD_IMG(IMAGE_LEBAO_LOGO,		"\\MMI_RES_##\\IMAG\\logo.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_SPLASH,		"\\MMI_RES_##\\IMAG\\welcome.png", IMG_CMP_PNG_ARGB_RLE, 0)

RES_ADD_IMG(IMAGE_LEBAO_MENU_CHATS,	"\\MMI_RES_##\\IMAG\\menu_charts.png",	IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_MENU_SEARCH,"\\MMI_RES_##\\IMAG\\menu_search.png",	IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_MENU_LOCAL, "\\MMI_RES_##\\IMAG\\menu_local.png",	IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_MENU_CLEAR, "\\MMI_RES_##\\IMAG\\menu_clear.png",	IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_MENU_ORDER, "\\MMI_RES_##\\IMAG\\menu_order.png",	IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_MENU_SETTING, "\\MMI_RES_##\\IMAG\\menu_setting.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_BACK, "\\MMI_RES_##\\IMAG\\btn_back.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_CLOSE, "\\MMI_RES_##\\IMAG\\btn_close.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_VOLUME, "\\MMI_RES_##\\IMAG\\btn_volume.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_DELETE, "\\MMI_RES_##\\IMAG\\btn_delete.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_FAVORITE, "\\MMI_RES_##\\IMAG\\btn_favorite.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_FAVORITE_SEL, "\\MMI_RES_##\\IMAG\\btn_favorite_sel.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_NEXT, "\\MMI_RES_##\\IMAG\\next.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_PREVIOU, "\\MMI_RES_##\\IMAG\\previou.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_RING, "\\MMI_RES_##\\IMAG\\ring.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_RING_SEL, "\\MMI_RES_##\\IMAG\\ring_sel.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_PLAY, "\\MMI_RES_##\\IMAG\\play.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_PAUSE, "\\MMI_RES_##\\IMAG\\pause.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_VOL_ADD, "\\MMI_RES_##\\IMAG\\volume_add.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_VOL_SUB, "\\MMI_RES_##\\IMAG\\volume_sub.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_ITEM_D1, "\\MMI_RES_##\\IMAG\\item_d1.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_ITEM_D2, "\\MMI_RES_##\\IMAG\\item_d2.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_ITEM_D3, "\\MMI_RES_##\\IMAG\\item_d3.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_DEFAULT, "\\MMI_RES_##\\IMAG\\btn_default.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_RECORD, "\\MMI_RES_##\\IMAG\\record.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_RECORD_GRAY, "\\MMI_RES_##\\IMAG\\record_gray.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_BTN_STAR, "\\MMI_RES_##\\IMAG\\star.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_LINE, "\\MMI_RES_##\\IMAG\\line.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_VIP, "\\MMI_RES_##\\IMAG\\vip.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_INDICATOR, "\\MMI_RES_##\\IMAG\\indicator.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_LOADING, "\\MMI_RES_##\\IMAG\\loading.gif", IMG_CMP_GIF_SCI, 0)
#if defined(LEBAO_FEATURE_PHONE_STYLE)
RES_ADD_IMG(IMAGE_LEBAO_MENU_ABOUT, "\\MMI_RES_##\\IMAG\\menu_about.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_VOLUME_1, "\\MMI_RES_##\\IMAG\\volume\\vol_1.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_VOLUME_2, "\\MMI_RES_##\\IMAG\\volume\\vol_2.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_VOLUME_3, "\\MMI_RES_##\\IMAG\\volume\\vol_3.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_VOLUME_4, "\\MMI_RES_##\\IMAG\\volume\\vol_4.png", IMG_CMP_PNG_ARGB_RLE, 0)
RES_ADD_IMG(IMAGE_LEBAO_VOLUME_5, "\\MMI_RES_##\\IMAG\\volume\\vol_5.png", IMG_CMP_PNG_ARGB_RLE, 0)
#endif
//]]define image information

//[[define string information
RES_ADD_STRING(TEXT_LEBAO_WELCOME,		"Listen music, set ringtone")
RES_ADD_STRING(TEXT_LEBAO_MAIN_TITLE,	"Migu Music")
RES_ADD_STRING(TEXT_LEBAO_MENU_CHARTS,	"Charts")
RES_ADD_STRING(TEXT_LEBAO_MENU_SEARCH,	"Search")
RES_ADD_STRING(TEXT_LEBAO_MENU_LOCAL,	"Local")
RES_ADD_STRING(TEXT_LEBAO_MENU_CLEAR,	"Clear")
RES_ADD_STRING(TEXT_LEBAO_MENU_ORDER,	"Member")
RES_ADD_STRING(TEXT_LEBAO_MENU_SETTING, "Setting")
RES_ADD_STRING(TEXT_LEBAO_MENU_ABOUT,	"About")
RES_ADD_STRING(TEXT_LEBAO_MENU_SERVICE, "Service")
RES_ADD_STRING(TEXT_LEBAO_MENU_PRIVACY, "Privacy")
RES_ADD_STRING(TEXT_LEBAO_MENU_COPYRIGHT,	"Copyright")
RES_ADD_STRING(TEXT_LEBAO_MENU_FAVORITES,	"Favorites")
RES_ADD_STRING(TEXT_LEBAO_OPT_OPEN,			"Order now")
RES_ADD_STRING(TEXT_LEBAO_OPT_LOGIN,		"Sign in")
RES_ADD_STRING(TEXT_LEBAO_OPT_INTEREST,		"Interest")
RES_ADD_STRING(TEXT_LEBAO_OPT_LOOP,			"Loop")
RES_ADD_STRING(TEXT_LEBAO_OPT_CANCEL_LOOP,	"Cancel loop")
RES_ADD_STRING(TEXT_LEBAO_OPT_REPEAT,		"Repeat")
RES_ADD_STRING(TEXT_LEBAO_OPT_SHUFFLE,		"Shuffle")
RES_ADD_STRING(TEXT_LEBAO_OPT_FAVORITE,		"Favorite")
RES_ADD_STRING(TEXT_LEBAO_OPT_RING,			"Ringtone")
RES_ADD_STRING(TEXT_LEBAO_OPT_FAVORITE_DEL,	"Cancel favorite")
RES_ADD_STRING(TEXT_LEBAO_OPT_RING_DEL,		"Delete ringtone")
RES_ADD_STRING(TEXT_LEBAO_OPT_12530,		"Call 12530")
RES_ADD_STRING(TEXT_LEBAO_LIST,			"List")
RES_ADD_STRING(TEXT_LEBAO_PLAY,			"Play")
RES_ADD_STRING(TEXT_LEBAO_PAUSE,		"Pause")
RES_ADD_STRING(TEXT_LEBAO_STOP,			"Stop")
RES_ADD_STRING(TEXT_LEBAO_RING,			"Ringtone")
RES_ADD_STRING(TEXT_LEBAO_DEL,			"Del")
RES_ADD_STRING(TEXT_LEBAO_EMPTY,		"Empty")
RES_ADD_STRING(TEXT_LEBAO_REMINDER,		"Reminder")
RES_ADD_STRING(TEXT_LEBAO_INTEREST,		"Member interest")
RES_ADD_STRING(TEXT_LEBAO_ONEKEY_SMS,	"Authorization")
RES_ADD_STRING(TEXT_LEBAO_CONFIRM,		"Confirm")
RES_ADD_STRING(TEXT_LEBAO_AGREE,		"Agree")
RES_ADD_STRING(TEXT_LEBAO_SUCCESSED,	"Succeeded")
RES_ADD_STRING(TEXT_LEBAO_FAILED,		"Failed")
RES_ADD_STRING(TEXT_LEBAO_NEED_VIP,		"VIP is required")
RES_ADD_STRING(TEXT_LEBAO_TIP_IVR,		"Call hotline")
RES_ADD_STRING(TEXT_LEBAO_LOGIN,		"Sign in")
RES_ADD_STRING(TEXT_LEBAO_MOBILE,		"Mobile")
RES_ADD_STRING(TEXT_LEBAO_CAPTCHA,		"Captcha")
RES_ADD_STRING(TEXT_LEBAO_PASSWORD,		"Password")
RES_ADD_STRING(TEXT_LEBAO_WAITING,		"Waiting")
RES_ADD_STRING(TEXT_LEBAO_RENEW,		"Renew")
RES_ADD_STRING(TEXT_LEBAO_NO_MUSIC, 		"The music file doesn't exist")
RES_ADD_STRING(TEXT_LEBAO_PLAY_FAILED, 		"Play failed")
RES_ADD_STRING(TEXT_LEBAO_RING_FAILED, 		"Failed to set the ringtone")
RES_ADD_STRING(TEXT_LEBAO_SAY_NAME, 		"Please say the name of song")
RES_ADD_STRING(TEXT_LEBAO_PUSH_TALK, 		"Push to talk")
RES_ADD_STRING(TEXT_LEBAO_OVER_TALK, 		"Release to send")
RES_ADD_STRING(TEXT_LEBAO_TIME_SHORT, 		"It's too short to talk")
RES_ADD_STRING(TEXT_LEBAO_SETTING_VOLUME,	"Volume settings")
RES_ADD_STRING(TEXT_LEBAO_SEARCHING, 		"Searching...")
RES_ADD_STRING(TEXT_LEBAO_FOUND_NOTHING,	"No songs found")
RES_ADD_STRING(TEXT_LEBAO_SCAN_QR, 			"Scan QR code to order VIP")
RES_ADD_STRING(TEXT_LEBAO_RING_OK, 			"Set ringtone successed")
RES_ADD_STRING(TEXT_LEBAO_MEMORY_FULL, 		"Not enough storage available")
RES_ADD_STRING(TEXT_LEBAO_ERR_DOWNLOAD,		"Download failed")
RES_ADD_STRING(TEXT_LEBAO_SEARCH_LIMIT,		"The free times have run out")
RES_ADD_STRING(TEXT_LEBAO_ADD_FAVORITE,		"Added to favorite")
RES_ADD_STRING(TEXT_LEBAO_DEL_FAVORITE,		"Removed")
RES_ADD_STRING(TEXT_LEBAO_DOWNLOAD_OK, 		"Download succeeded")
RES_ADD_STRING(TEXT_LEBAO_NO_SIM_CARD, 		"No SIM card or network found")
RES_ADD_STRING(TEXT_LEBAO_NO_MORE, 			"~~ the end ~~")
RES_ADD_STRING(TEXT_LEBAO_MUSIC_LIST, 		"Music List")
RES_ADD_STRING(TEXT_LEBAO_ERR_NETWORK,		"Network error, Please try again later")
RES_ADD_STRING(TEXT_LEBAO_ERR_SERVICE,		"Please try again later")
RES_ADD_STRING(TEXT_LEBAO_ERR_UPDATE,		"Update failed. Please try again later")
RES_ADD_STRING(TEXT_LEBAO_CAN_NOT_FLY_MODE,	"Cannot be used in flight mode")
RES_ADD_STRING(TEXT_LEBAO_OPEN_GPRS,		"Open data networking service")
RES_ADD_STRING(TEXT_LEBAO_SERVICE_INFO,		"Please visit https://passport.migu.cn/portal/protocol")
RES_ADD_STRING(TEXT_LEBAO_PRIVACY_INFO,		"Please visit https://passport.migu.cn/portal/privacy/protocol")
RES_ADD_STRING(TEXT_LEBAO_COPYRIGHT_INFO,	"Please visit https://www.migu.cn/about/copyright.html")
RES_ADD_STRING(TEXT_LEBAO_PLAYER_NOTIRY,	"Press the left key to set ringtone and 0 key add to favorites")
RES_ADD_STRING(TEXT_LEBAO_PUSH_TALK_PHONE,	"Push OK to talk")
RES_ADD_STRING(TEXT_LEBAO_TIPS_LOADING,		"Loading...")
RES_ADD_STRING(TEXT_LEBAO_TIPS_VIP_ALREADY, "You are already a music member")
RES_ADD_STRING(TEXT_LEBAO_TIPS_CLEAN,		"The disk has been cleaned up")
RES_ADD_STRING(TEXT_LEBAO_TIPS_BUSY,		"Updating, please try again later")
RES_ADD_STRING(TEXT_LEBAO_TIPS_VERIFY,		"Verifying infomation")
RES_ADD_STRING(TEXT_LEBAO_TIPS_NEED_CMCC,	"Non cmcc mobile networks are not supported")
RES_ADD_STRING(TEXT_LEBAO_TIPS_QUERYING,	"Querying")
RES_ADD_STRING(TEXT_LEBAO_TIPS_OPENING,		"Opening")
RES_ADD_STRING(TEXT_LEBAO_TIPS_SUBMIT,		"Submitting")
RES_ADD_STRING(TEXT_LEBAO_TIPS_FAILED,		"Failed. Please try again later")
RES_ADD_STRING(TEXT_LEBAO_TIPS_TRY_IVR,		"Please try call hotline")
RES_ADD_STRING(TEXT_LEBAO_TIPS_LOGIN,		"Sign in")
RES_ADD_STRING(TEXT_LEBAO_TIPS_LOGIN_OK,	"Login succeeded")
RES_ADD_STRING(TEXT_LEBAO_TIPS_LOGIN_FAILED,"Login failed")
RES_ADD_STRING(TEXT_LEBAO_TIPS_TIMEOUT,		"Timeout, exit and try again")
RES_ADD_STRING(TEXT_LEBAO_TIPS_TRY_QR,		"Please scan QR code")
RES_ADD_STRING(TEXT_LEBAO_TIPS_ORDER_OK,	"Successfully ordered VIP")
RES_ADD_STRING(TEXT_LEBAO_TIPS_VIP_TYPE,	"VIP")
RES_ADD_STRING(TEXT_LEBAO_TIPS_THANK,		"Thank you")

//]]define string information

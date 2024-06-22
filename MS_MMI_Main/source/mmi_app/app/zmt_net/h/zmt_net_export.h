
/**--------------------------------------------------------------------------*
**                         Include Files                                     *
**---------------------------------------------------------------------------*/
#include "sci_types.h"
#include "mmk_type.h"
#include "guilistbox.h"
#include "mmi_filemgr.h"
#include "zmt_net.h"

/**--------------------------------------------------------------------------*
**                         TYPE DEFINITION                                   *
**---------------------------------------------------------------------------*/

typedef enum
{
	ZMT_DATA_TIME,
	ZMT_DATA_PEOPLE,
	ZMT_DATA_TXT,
	ZMT_DATA_AUDIO,
	ZMT_DATA_PHOTO
}ZMT_CHAT_DATA_TYPE_E;

typedef struct zmt_data_content
{
    char * uuid;
    char * name;
    char *data_url;
    char * data;
    uint32 data_len;
    uint32 time_stamp;
    ZMT_CHAT_DATA_TYPE_E data_type;
    ZMTTCPRCVHANDLER rec_handle;
    struct zmt_data_content * next_data_content;
}ZMT_DATA_CONTENT_T;



/*---------------------------------------------------------------------------*
**                           FUNCTION     DECLARITION                              *
**---------------------------------------------------------------------------*/

PUBLIC MMI_RESULT_E MMIZMT_Net_Handle_AppMsg(PWND app_ptr, uint16 msg_id, DPARAM param);

PUBLIC void ZMT_Net_TCP_Close(void);
PUBLIC uint32 ZMT_Net_TCPSendFile(ZMT_DATA_CONTENT_T * chatdata);

PUBLIC BOOLEAN ZMT_Net_Is_SendFile_Success(void);



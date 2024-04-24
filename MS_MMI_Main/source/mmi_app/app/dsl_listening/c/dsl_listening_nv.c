
#include "std_header.h"
#include "sci_types.h"
#include "mmi_modu_main.h"
#include "dsl_listening_export.h"
#include "dsl_listening_nv.h"

const uint16 listening_nv_len[] =
{
	sizeof(BOOLEAN),
	//sizeof(LISTEING_LOCAL_INFO),
	sizeof(LISTEING_PLAYER_INFO),
};

PUBLIC void MMI_RegDslListeningNv(void)
{
	MMI_RegModuleNv(MMI_MODULE_DSL_LISTENING, listening_nv_len, sizeof(listening_nv_len)/sizeof(uint16));
}


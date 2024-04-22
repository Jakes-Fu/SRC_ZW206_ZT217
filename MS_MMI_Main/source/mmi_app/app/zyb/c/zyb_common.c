
#include "zyb_common.h"

BOOLEAN ZYB_SIM_Exsit(void)
{
    if(MMIPHONE_IsSimOk(MN_DUAL_SYS_1) == FALSE)
    {
        return FALSE;
    }
    return TRUE;
}
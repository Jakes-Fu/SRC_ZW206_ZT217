////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/chip/branches/gallite441/hal/src/hal_sys_ifc.c $ //
//    $Author: jingyuli $                                                        //
//    $Date: 2016-06-07 16:38:14 +0800 (Bi., 07 Nhlang. 2016) $                     //
//    $Revision: 32439 $                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_sys_ifc.h                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "os_api.h"
#include "chip_plf_export.h"
#include "ifc_manager_cfg.h"
#include "ifc_drvapi.h"
//#include "halp_debug.h"
#include "timer_drvapi.h"

#define  HAL_ASSERT             SCI_PASSERT
#define  REQ_SRC_DMA_ID_RX_CAMERA 0x9 // 展锐修改,优化BF20A6摄像头 横纹.. wuxx add. modify ifc_hal.c and ifc_manager_cfg.c

#if defined OSI_SXR_OS
#include "osiapi.h"
#endif
#define IFC_ASSERT(BOOL,param)  HAL_ASSERT(BOOL, ("Ifc param = %d", param))
//bring_tmp, control reg: bit8~12
//#define DMA_ID_RX_CAMERA(APIFC_CH)   (APIFC_CH==0x13)? 0x9: (APIFC_CH)


/// That structure is used to memorize the owner of
/// each of the IFC channel.
IFC_MANAGER_CHANNEL_LIST g_halIfcChannelOwner[IFC_MANAGER_CHAN_NB];

//UINT8   g_ifc_ch[APIFC_REQUESTID_NUM] = {APIFC_SDMMC1_TX_CH, APIFC_SDMMC1_RX_CH, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xFF,APIFC_CAMERA_RX_CH};
extern const IFC_MANAGER_CFG_INFO_T ifc_mgr_cfg_info[];
uint32 hal_SysEnterCriticalSectionifc(void)
{
    SCI_DisableIRQ();
    //SCI_DisableFIQ();
    return 0;
}
void hal_SysExitCriticalSectionifc(uint32 status)
{
    SCI_RestoreIRQ();
    //SCI_RestoreFIQ();
}
//choise hwp
void hal_IfcOpenedChnl(void)
{
    int i=0;
    SCI_TraceLow("=================opened channel====================");
    for(i=0;i<IFC_MANAGER_CHAN_NB;i++)
    {
        SCI_TraceLow("opened channel:0x%x",g_halIfcChannelOwner[i]);
    }
}
IFC_MANAGER_HWP_LIST hal_SysHwpChoseIfc(IFC_MANAGER_CHANNEL_LIST request_id)
{
	IFC_MANAGER_HWP_LIST hwp_chose = LPS_IFC;
	IFC_MANAGER_CFG_INFO_T ifc_info;
	int i=0;

	for(i=0;i<IFC_MANAGER_CHAN_NB;i++)
	{
		if(ifc_mgr_cfg_info[i].logic_id==request_id)
		{
			ifc_info.base_addr=ifc_mgr_cfg_info[i].base_addr;
			switch(ifc_info.base_addr)
			{
				case REG_LPS_IFC_BASE:
					hwp_chose =LPS_IFC;
					break;
				case REG_AON_IFC_BASE:
					hwp_chose =AON_IFC;
					break;
				case REG_AP_IFC_BASE:
					hwp_chose =AP_IFC;
					break;
				default:
					hwp_chose =IFC_MAX;
					break;
			}
		}
	}
	return hwp_chose;
}

uint8 hal_IfcGetChannelid(IFC_MANAGER_CHANNEL_LIST request_id)
{
	uint8 channel_id=0xff;
	int i=0;
	for(i=0;i<IFC_MANAGER_CHAN_NB;i++)
	{
		if(ifc_mgr_cfg_info[i].logic_id==request_id)
		{
			channel_id=ifc_mgr_cfg_info[i].phy_num;
			return channel_id;
		}
	}
    return channel_id;
}
// =============================================================================
// hal_IfcOpen
// -----------------------------------------------------------------------------
/// Open the System IFC. Initialize some state variables.
// =============================================================================
void hal_IfcOpen(void)
{
    uint8 channel;

    // Initialize the channel table with unknown requests. This is more
    // of a sanity initialization.
    for (channel = 0; channel < IFC_MANAGER_CHAN_NB; channel++)
    {
        g_halIfcChannelOwner[channel] = DMA_IFC_NO_REQWEST;
    }
}


// =============================================================================
// hal_IfcGetOwner
// -----------------------------------------------------------------------------
/// Get the owner ID of an IFC channel (i.e. the module using the channel).
/// @param channel number to check
/// @return The owner number associated with the channel
// =============================================================================
#if 0 //You can find the channel user in ifc_manager file.
IFC_MANAGER_CHANNEL_LIST hal_IfcGetOwner(uint8 channel)//return logic channel
{
    // Here, we consider the transfer as previously finished.
    if (channel == IFC_UNKNOWN_CHANNEL) return DMA_IFC_NO_REQWEST;

    // Channel number too big.
    IFC_ASSERT(channel < IFC_MANAGER_CHAN_NB, channel);

    return g_halIfcChannelOwner[channel];
}

uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	
	channel=hal_IfcGetChannelid(requestId);
	hwp_list=hal_SysHwpChoseIfc(requestId);
	IFC_ASSERT(channel !=0xff, channel);
	switch(hwp_list)
	{
		case LPS_IFC:
			
			break;
		case AON_IFC:
			
			break;
		case AP_IFC:
			
			break;
		default:
			break;
	}
#endif
// =============================================================================
// hal_IfcChannelRelease
// -----------------------------------------------------------------------------
/// Force the release of a channel owned by a request.
///
/// The channel is only released if the specified request
/// owns the channel.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel to release
// =============================================================================
void hal_IfcChannelRelease(IFC_MANAGER_CHANNEL_LIST requestId)
{
	uint32 status;
	uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	
	channel=hal_IfcGetChannelid(requestId);
	hwp_list=hal_SysHwpChoseIfc(requestId);
	if (channel == IFC_UNKNOWN_CHANNEL) return;
	status = hal_SysEnterCriticalSectionifc();//disable_irq
	switch(hwp_list)
	{
		case LPS_IFC:
			hwp_lpsIfc->std_ch[channel].control |= LPS_IFC_REQ_SRC(0x1F);    /*将req_src设置为无效0x1F，避免双核平台requestid与Enable不同步导致start接口中对id通道使用的误判*/
			hwp_lpsIfc->std_ch[channel].control |= LPS_IFC_DISABLE;
			break;
		case AON_IFC:
			hwp_aonIfc->std_ch[channel].control |= AON_IFC_REQ_SRC(0x1F);    /*将req_src设置为无效0x1F，避免双核平台requestid与Enable不同步导致start接口中对id通道使用的误判*/
			hwp_aonIfc->std_ch[channel].control |= AON_IFC_DISABLE;
			break;
		case AP_IFC:
			hwp_apIfc->std_ch[channel].control |= AP_IFC_REQ_SRC(0x1F);    /*将req_src设置为无效0x1F，避免双核平台requestid与Enable不同步导致start接口中对id通道使用的误判*/
			hwp_apIfc->std_ch[channel].control |= AP_IFC_DISABLE;
			break;
		default:
			break;
	}
	hal_SysExitCriticalSectionifc(status);
}
// =============================================================================
// hal_IfcChannelFlush
// -----------------------------------------------------------------------------
/// Empty the FIFO a specified channel.
///
/// If #requestId does not match #channel owner, does not do anything.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel whose FIFO is to be clear.
// =============================================================================
void hal_IfcChannelFlush(IFC_MANAGER_CHANNEL_LIST requestId)
{
	uint32 status;
	uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	
	channel=hal_IfcGetChannelid(requestId);
	hwp_list=hal_SysHwpChoseIfc(requestId);
	if (channel == IFC_UNKNOWN_CHANNEL) return;
	// Check that the channel is really owned by the peripheral
	// which is doing the request, it could have been release
	// automatically or by an IRQ handler.
    	status = hal_SysEnterCriticalSectionifc();
	switch(hwp_list)
	{
		case LPS_IFC:
			hwp_lpsIfc->std_ch[channel].control |= LPS_IFC_FLUSH;
			break;
		case AON_IFC:
			hwp_aonIfc->std_ch[channel].control |= AON_IFC_FLUSH;
			break;
		case AP_IFC:
			hwp_apIfc->std_ch[channel].control |= AP_IFC_FLUSH;
			break;
		default:
			break;
	}
    hal_SysExitCriticalSectionifc(status);
}

// =============================================================================
// hal_IfcChannelIsFifoEmpty
// -----------------------------------------------------------------------------
/// Returns \c TRUE when the FIFO is empty.
///
/// If #requestId does not match #channel owner, returns TRUE.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Channel whose FIFO is to be checked.
// =============================================================================
BOOLEAN hal_IfcChannelIsFifoEmpty(IFC_MANAGER_CHANNEL_LIST requestId)
{
	uint32 status;
	BOOLEAN fifoIsEmpty = TRUE;
	uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	// Here, we consider the transfer as previously finished.
	channel=hal_IfcGetChannelid(requestId);
	if (channel == IFC_UNKNOWN_CHANNEL) return fifoIsEmpty;
	hwp_list=hal_SysHwpChoseIfc(requestId);
	// Check that the channel is really owned by the peripheral
	// which is doing the request, it could have been release
	// automatically or by an IRQ handler.
	status = hal_SysEnterCriticalSectionifc();
	switch(hwp_list)
	{
		case LPS_IFC:
			fifoIsEmpty =
            			(0 != (hwp_lpsIfc->std_ch[channel].status & LPS_IFC_FIFO_EMPTY));
			break;
		case AON_IFC:
			fifoIsEmpty =
            			(0 != (hwp_aonIfc->std_ch[channel].status & AON_IFC_FIFO_EMPTY));
			break;
		case AP_IFC:
			fifoIsEmpty =
            			(0 != (hwp_apIfc->std_ch[channel].status & AP_IFC_FIFO_EMPTY));
			break;
		default:
			break;
	}
	hal_SysExitCriticalSectionifc(status);

	return fifoIsEmpty;
}

// =============================================================================
// hal_IfcTransferStart
// -----------------------------------------------------------------------------
/// Start an IFC transfer
///
/// This is a non blocking function that starts the transfer
/// and returns the hand.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param memStartAddr. Start address of the buffer where data
/// to be sent are located or where to put the data read, according
/// to the request defined by the previous parameter
/// @param xferSize Number of bytes to transfer. The maximum size
/// is 2^20 - 1 bytes.
/// @param ifcMode Mode of the transfer (Autodisable or not, 8 or 32 bits)
/// @return Channel got or HAL_UNKNOWN_CHANNEL.
// =============================================================================
uint8 hal_IfcTransferStart(IFC_MANAGER_CHANNEL_LIST requestId, uint8* memStartAddr, uint32 xferSize, HAL_IFC_MODE_T ifcMode)
{
	uint32 status;
	uint8 channel = IFC_UNKNOWN_CHANNEL;
	IFC_MANAGER_HWP_LIST hwp_list;
	
	channel=hal_IfcGetChannelid(requestId);
	hwp_list=hal_SysHwpChoseIfc(requestId);
	IFC_ASSERT(channel !=0xff, channel);
#if (CHIP_HAS_SYS_IFC_SIZE)
    // Check buffer alignment depending on the mode
    if (ifcMode == HAL_IFC_SIZE_32_MODE_MANUAL || ifcMode == HAL_IFC_SIZE_32_MODE_AUTO)
    {
        // check word alignment
        HAL_ASSERT(((UINT32)memStartAddr%4) == 0,
                   ("HAL IFC: 32 bits transfer misaligned 0x@%08X", memStartAddr));
    }
#if (CHIP_HAS_SYS_IFC_SIZE == 2)
    else if (ifcMode == HAL_IFC_SIZE_16_MODE_MANUAL || ifcMode == HAL_IFC_SIZE_16_MODE_AUTO)
    {
        // check half word alignment
        HAL_ASSERT(((UINT32)memStartAddr%2) == 0,
                   ("HAL IFC: 16 bits transfer misaligned 0x@%08X", memStartAddr));
    }
#endif
    else
    {
        // ifcMode == HAL_IFC_SIZE_8, nothing to check
    }
#endif

    HAL_ASSERT(xferSize < (1<<AP_IFC_TC_LEN),
               ("HAL IFC: Transfer size too large: %d", xferSize));

    HAL_ASSERT((requestId !=IFC_UNKNOWN_CHANNEL), ("apifc requestid %d is error", requestId));

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#if !defined OSI_SXR_OS    // not armcc
    if((U32)memStartAddr&0x20000000==0)
        hal_DcacheFlushAddrRange((UINT32)memStartAddr, (UINT32)memStartAddr+xferSize);
#endif
#endif
   MMU_DmaCacheSync((uint32)memStartAddr, xferSize,DMABUFFER_BIDIRECTIONAL);

    status = hal_SysEnterCriticalSectionifc();
#if defined OSI_SXR_OS
    OSI_DCACHE_FLUSH_INVAL(memStartAddr,xferSize);
#error
#endif
    HAL_ASSERT((channel !=IFC_UNKNOWN_CHANNEL), ("apifc requestId %d ch %d is error", requestId, channel));

    // Check the requested id is not currently already used.
    switch(hwp_list)
	{
		case LPS_IFC:
				HAL_ASSERT((hwp_lpsIfc->std_ch[channel].status & LPS_IFC_ENABLE) == 0,
	                   ("HAL: Attempt to use the LPS_IFC to deal with a %d"
	                   " request still active on channel %d", requestId, channel));

				g_halIfcChannelOwner[channel]     = requestId;
				hwp_lpsIfc->std_ch[channel].start_addr  =  (uint32) memStartAddr;
				hwp_lpsIfc->std_ch[channel].tc          =  xferSize;
				hwp_lpsIfc->std_ch[channel].control     = (LPS_IFC_REQ_SRC(channel)
				        | ifcMode
				        | LPS_IFC_CH_RD_HW_EXCH
				        | LPS_IFC_ENABLE);

			break;
		case AON_IFC:
				HAL_ASSERT((hwp_aonIfc->std_ch[channel].status & AON_IFC_ENABLE) == 0,
	                   ("HAL: Attempt to use theAON_IFC to deal with a %d"
	                   " request still active on channel %d", requestId, channel));

				g_halIfcChannelOwner[channel+2]     = requestId;
				hwp_aonIfc->std_ch[channel].start_addr  =  (uint32) memStartAddr;
				hwp_aonIfc->std_ch[channel].tc          =  xferSize;
				hwp_aonIfc->std_ch[channel].control     = (AON_IFC_REQ_SRC(channel)
				        | ifcMode
				        | AON_IFC_CH_RD_HW_EXCH
				        | AON_IFC_ENABLE);
			break;
            
		case AP_IFC:
				HAL_ASSERT((hwp_apIfc->std_ch[channel].status & AP_IFC_ENABLE) == 0,
	                   ("HAL: Attempt to use theAON_IFC to deal with a %d"
	                   " request still active on channel %d", requestId, channel));

				g_halIfcChannelOwner[channel+8]     = requestId;
				hwp_apIfc->std_ch[channel].start_addr  =  (uint32) memStartAddr;
				hwp_apIfc->std_ch[channel].tc          =  xferSize;
                        if (requestId == DMA_ID_RX_CAMERA){ // 展锐修改,优化BF20A6摄像头 横纹, wuxx add. begin.
                            SCI_TRACE_LOW("shengzhanhe requestId = %d channel = %d", requestId, channel);
                            hwp_apIfc->std_ch[channel].control = (AP_IFC_REQ_SRC(REQ_SRC_DMA_ID_RX_CAMERA)
                                                    | ifcMode
                                                    | AP_IFC_CH_RD_HW_EXCH
                                                    | AP_IFC_ENABLE);

                        } else { // 展锐修改,优化BF20A6摄像头 横纹, wuxx add. end.
                            hwp_apIfc->std_ch[channel].control     = (AP_IFC_REQ_SRC(channel)
                                 | ifcMode
                                 | AP_IFC_CH_RD_HW_EXCH
                                 | AP_IFC_ENABLE);
                        }
                break;
                
		default:
			break;
	}
	hal_SysExitCriticalSectionifc(status);
	return channel;
}
// =============================================================================
// hal_IfcGetTc
// -----------------------------------------------------------------------------
/// Returns the transfer count of the IFC.
/// If #requestId does not match #channel owner, returns 0.
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param channel Number of the channel whose transfer we are concerned for.
/// @return The number of bytes remaining to be transfered.
// =============================================================================
uint32 hal_IfcGetTc(IFC_MANAGER_CHANNEL_LIST requestId)
{
	uint32 status;
	uint32 tc = 0;
	//mon_Event(0xaa001111);
	uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	 //mon_Event(0xaa001111);
	// Here, we consider the transfer as previously finished.
	channel=hal_IfcGetChannelid(requestId);
	if (channel == IFC_UNKNOWN_CHANNEL) 
		return 0;
	 // Channel number too big.
	hwp_list=hal_SysHwpChoseIfc(requestId);
	IFC_ASSERT(channel !=IFC_UNKNOWN_CHANNEL, channel);
	// Check that the channel is really owned by the peripheral
	// which is doing the request, it could have been release
	// automatically or by an IRQ handler.
	status = hal_SysEnterCriticalSectionifc();
	switch(hwp_list)
	{
		case LPS_IFC:
			 tc = hwp_lpsIfc->std_ch[channel].tc;
			break;
		case AON_IFC:
			 tc = hwp_aonIfc->std_ch[channel].tc;
			break;
		case AP_IFC:
			 tc = hwp_apIfc->std_ch[channel].tc;
			break;
		default:
			break;
	}
	hal_SysExitCriticalSectionifc(status);

    return tc;
}
// =============================================================================
// hal_IfcExtendTransfer
// -----------------------------------------------------------------------------
/// Extend the transfer size of the channel of
/// the number ox bytes passed as a parameter.
///
/// Should be used only when a channel has been locked.
///
/// @param xferSize Number of bytes to add to the
/// transfer size of this channel
// =============================================================================
void hal_IfcExtendTransfer(IFC_MANAGER_CHANNEL_LIST requestId, uint16 xferSize)
{
	uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	// Here, we consider the transfer as previously finished.
	channel=hal_IfcGetChannelid(requestId);
	if (channel == IFC_UNKNOWN_CHANNEL) return;
	 // Channel number too big.
	hwp_list=hal_SysHwpChoseIfc(requestId);
	IFC_ASSERT(channel !=IFC_UNKNOWN_CHANNEL, channel);
	// Writing in the Tc register adds the number written
	// to the counter of bytes yet to transfer
	switch(hwp_list)
	{
		case LPS_IFC:
			 hwp_lpsIfc->std_ch[channel].tc = xferSize;
			break;
		case AON_IFC:
			 hwp_aonIfc->std_ch[channel].tc = xferSize;
			break;
		case AP_IFC:
			 hwp_apIfc->std_ch[channel].tc = xferSize;
			break;
		default:
			break;
	}
}

// =============================================================================
// hal_IfcGetChannel
// -----------------------------------------------------------------------------
/// Get an IFC channel opend in NO auto-disable mode
///
/// @param requestId Describe the direction of the tranfer (rx or
/// tx) and the module to or from which data are to be moved.
/// @param ifcMode Mode of the transfer (Autodisable or not, 8 or 32 bits)
/// @return The channel number got or HAL_UNKNOWN_CHANNEL
// =============================================================================
uint8 hal_IfcGetChannel(IFC_MANAGER_CHANNEL_LIST requestId, HAL_IFC_MODE_T ifcMode)
{
	uint8 channel;
	uint32 status;
	IFC_MANAGER_HWP_LIST hwp_list;
	// This function is only relevant/available for manual transferts.
	HAL_ASSERT( ifcMode == HAL_IFC_SIZE_8_MODE_MANUAL
#if (CHIP_HAS_SYS_IFC_SIZE)
#if (CHIP_HAS_SYS_IFC_SIZE == 2)
	            || ifcMode == HAL_IFC_SIZE_16_MODE_MANUAL
#endif
	            || ifcMode == HAL_IFC_SIZE_32_MODE_MANUAL
#endif
	            ,
	            ("hal_IfcGetChannel called with an autodisable %d mode", ifcMode));

	status = hal_SysEnterCriticalSectionifc();

	//channel = g_apifc_ch[requestId];
	hwp_list=hal_SysHwpChoseIfc(requestId);
	channel=hal_IfcGetChannelid(requestId);
	
	if (channel ==0xff)
	{
	    hal_SysExitCriticalSectionifc(status);
	    return IFC_UNKNOWN_CHANNEL;
	}
	switch(hwp_list)
	{
		case LPS_IFC:
			hwp_lpsIfc->std_ch[channel].tc =  0;
			hwp_lpsIfc->std_ch[channel].start_addr = 0;
			g_halIfcChannelOwner[channel] = requestId;
			hwp_lpsIfc->std_ch[channel].control = (LPS_IFC_REQ_SRC(requestId)
				                           | ifcMode
				                           | LPS_IFC_CH_RD_HW_EXCH
				                           | LPS_IFC_ENABLE);
			break;
		case AON_IFC:
			hwp_aonIfc->std_ch[channel].tc =  0;
			hwp_aonIfc->std_ch[channel].start_addr = 0;
			g_halIfcChannelOwner[channel+2] = requestId;
			hwp_aonIfc->std_ch[channel].control = (AON_IFC_REQ_SRC(requestId)
				                           | ifcMode
				                           | AON_IFC_CH_RD_HW_EXCH
				                           | AON_IFC_ENABLE);
			break;
		case AP_IFC:
			hwp_apIfc->std_ch[channel].tc =  0;
			hwp_apIfc->std_ch[channel].start_addr = 0;
			g_halIfcChannelOwner[channel+8] = requestId;
			hwp_apIfc->std_ch[channel].control = (AP_IFC_REQ_SRC(requestId)
				                           | ifcMode
				                           | AP_IFC_CH_RD_HW_EXCH
				                           | AP_IFC_ENABLE);
			break;
		default:
			break;
	}
	
	hal_SysExitCriticalSectionifc(status);

    return TRUE;
}
// =============================================================================
// hal_IfcSetStartAddress
// -----------------------------------------------------------------------------
/// Set the start address of a previously opened (with hal_IfcGetChannel)
/// IFC channel.
///
/// Should be used only when a channel has been locked.
///
/// @param channel Opened channel whose start address will be set.
/// @param startAddress Set start address of the channel.
// =============================================================================
void hal_IfcSetStartAddress(IFC_MANAGER_CHANNEL_LIST requestId, CONST uint8* startAddress)
{
    // channel number too big
	uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	
	channel=hal_IfcGetChannelid(requestId);
	hwp_list=hal_SysHwpChoseIfc(requestId);
	IFC_ASSERT(channel !=IFC_UNKNOWN_CHANNEL, channel);
	switch(hwp_list)
	{
		case LPS_IFC:
			HAL_ASSERT(hwp_lpsIfc->std_ch[channel].tc == 0, ("Attempt to change an LPS_IFC start_address "
				"while tc != 0 (==%d), channel=%d", hwp_lpsIfc->std_ch[channel].tc, channel));

			// Writing in the Tc register adds the number written
			// to the counter of bytes yet to transfer
			hwp_lpsIfc->std_ch[channel].start_addr = (uint32) startAddress;
			break;
		case AON_IFC:
			HAL_ASSERT(hwp_aonIfc->std_ch[channel].tc == 0, ("Attempt to change an AON_IFC start_address "
				"while tc != 0 (==%d), channel=%d", hwp_aonIfc->std_ch[channel].tc, channel));

			// Writing in the Tc register adds the number written
			// to the counter of bytes yet to transfer
			hwp_aonIfc->std_ch[channel].start_addr = (uint32) startAddress;
			break;
		case AP_IFC:
			HAL_ASSERT(hwp_apIfc->std_ch[channel].tc == 0, ("Attempt to change an AP_IFC start_address "
				"while tc != 0 (==%d), channel=%d", hwp_apIfc->std_ch[channel].tc, channel));

			// Writing in the Tc register adds the number written
			// to the counter of bytes yet to transfer
			hwp_apIfc->std_ch[channel].start_addr = (uint32) startAddress;
			break;
		default:
			break;
	}
}

uint32 hal_IfcGetRunStatus(IFC_MANAGER_CHANNEL_LIST requestId)
{
	uint32 status;
	uint32 tc = 0;
	uint8 channel;
	IFC_MANAGER_HWP_LIST hwp_list;
	
	channel=hal_IfcGetChannelid(requestId);
	hwp_list=hal_SysHwpChoseIfc(requestId);
	// Here, we consider the transfer as previously finished.
	if (channel == IFC_UNKNOWN_CHANNEL) return tc;
	// Channel number too big.
	IFC_ASSERT(channel !=IFC_UNKNOWN_CHANNEL, channel);
	status = hal_SysEnterCriticalSectionifc();
	// Check that the channel is really owned by the peripheral
	// which is doing the request, it could have been release
	// automatically or by an IRQ handler.
	switch(hwp_list)
	{
		case LPS_IFC:
			 tc = hwp_lpsIfc->std_ch[channel].status & LPS_IFC_ENABLE;
			break;
		case AON_IFC:
			tc = hwp_aonIfc->std_ch[channel].status & AON_IFC_ENABLE;
			break;
		case AP_IFC:
			tc = hwp_apIfc->std_ch[channel].status & AP_IFC_ENABLE;
			break;
		default:
			break;
	}
	hal_SysExitCriticalSectionifc(status);

	return tc;
}

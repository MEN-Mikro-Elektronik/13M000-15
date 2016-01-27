/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  smb_ich.c
 *
 *      \author  dieter.pfeuffer@men.de
 *        $Date: 2012/01/30 15:11:29 $
 *    $Revision: 1.5 $
 *
 *      \brief   SMBPCI BBIS - SMBPCI_ICH variant specific functions
 *
 *     Required: ---
 *
 *     \switches DBG 
 * */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: smb_ich.c,v $
 * Revision 1.5  2012/01/30 15:11:29  dpfeuffer
 * R: support for FCH_ASF SMB controller
 * M: FCH_ASF specific part added
 *
 * Revision 1.4  2009/10/07 13:23:55  dpfeuffer
 * R:1. busyWait was added to SMB_DESC_ICH struct and must be initialized
 * M:1.a) SMB_DESC_ICH struct initialized with 0x00 to prevent uninitialized
 *        parameters that were added later
 *     b) initialize busyWait with SMB_BUSY_WAIT descriptor value
 *
 * Revision 1.3  2006/02/28 08:40:43  DPfeuffer
 * __SMBPCI_InitLib(): alertPollFreq added
 *
 * Revision 1.2  2006/02/10 17:00:49  SVogel
 * added
 *  + debug messages
 *  + debug level to descriptor
 *
 * Revision 1.1  2005/11/25 15:08:47  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "smbpci_int.h"		/* internal header file */

/****************************** SmbInitLib **********************************/
/** Initialize the SMB library
 *
 *  \param brdHdl     \IN  Bbis handle
 *
 *  \return           \c 0 On success or error code
 */
extern int32 __SMBPCI_InitLib(
	BBIS_HANDLE *brdH
)
{
    int32           error;
	SMB_DESC_ICH    descSmb;

	DBGWRT_1((DBH, "%s baseAddr %08p\n", __FUNCTION__, brdH->virtCtrlBase ));

	OSS_MemFill( brdH->osH, sizeof(descSmb), (char*)&descSmb, 0x00 );

	descSmb.baseAddr = brdH->virtCtrlBase;
	descSmb.dbgLevel = brdH->dbgLev;
	descSmb.alertPollFreq = (int32)brdH->alertPollFreq;
	descSmb.busyWait = (int32)brdH->busyWait;

#ifdef _SMBPCI_FCH_ASF
	if( brdH->smbAddrSpaceType == OSS_ADDRSPACE_MEM )
	    error = SMB_ICH_Init( &descSmb, brdH->osH, &brdH->smbH );
	else
#endif
		error = SMB_ICH_IO_Init( &descSmb, brdH->osH, &brdH->smbH );

    if( error )
    {
	    DBGWRT_ERR((DBH,"*** %s\n", __FUNCTION__));
    }

	return ( error );
}





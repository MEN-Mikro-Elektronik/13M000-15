/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  smb_16z001.c
 *
 *      \author  dieter.pfeuffer@men.de
 *        $Date: 2010/12/20 13:31:54 $
 *    $Revision: 1.3 $
 *
 *      \brief   SMBPCI BBIS - SMBPCI_16Z001 variant specific functions
 *
 *     Required: ---
 *
 *     \switches DBG 
 * */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: smb_16z001.c,v $
 * Revision 1.3  2010/12/20 13:31:54  dpfeuffer
 * R: IO mapped variants for 16Z001 SMB ctrl. required
 * M: variant 16Z001_IO and ISA_16Z001_IO implemented
 *
 * Revision 1.2  2009/10/07 13:24:02  dpfeuffer
 * R:1. some SMB_DESC_MENZ001 parameters were not initialized
 * M:1.a) SMB_DESC_MENZ001 struct initialized with 0x00
 *     b) initialize busyWait with SMB_BUSY_WAIT descriptor value
 *
 * Revision 1.1  2006/08/02 12:25:05  DPfeuffer
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
    int32				error;
	SMB_DESC_MENZ001	descSmb;

	DBGWRT_1((DBH, "%s baseAddr %08x\n", __FUNCTION__, brdH->virtCtrlBase ));

	OSS_MemFill( brdH->osH, sizeof(descSmb), (char*)&descSmb, 0x00 );

	descSmb.baseAddr = brdH->virtCtrlBase;
	descSmb.dbgLevel = brdH->dbgLev;
	descSmb.alertPollFreq = (int32)brdH->alertPollFreq;
	descSmb.busyWait = (int32)brdH->busyWait;
	descSmb.timeOut = 20;

#ifdef MAC_IO_MAPPED
    error = SMB_MENZ001_IO_Init( &descSmb, brdH->osH, &brdH->smbH );
#else
    error = SMB_MENZ001_Init( &descSmb, brdH->osH, &brdH->smbH );
#endif
    if( error ){
	    DBGWRT_ERR((DBH,"*** %s\n", __FUNCTION__));
    }

	return ( error );
}





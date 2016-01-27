/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_sig.c
 *
 *      \author  ulrich.bogensperger@emen.de
 *        $Date: 2004/03/24 09:40:52 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Signal routines
 *      
 *    \switches  none 
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_sig.c,v $
 * Revision 1.1  2004/03/24 09:40:52  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#define _OSS_SIG_C
#include "oss_intern.h"


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*! \page qnxsigusage

  \section qnxsigusagesect QNX notes to OSS signals

  This implementation is straight forward. It uses POSIX 
  signals, typically realtime signals.
  
  See \ref osssigusagesect for more info.
*/

/**********************************************************************/
/** Create signal handle.
 * 
 * \copydoc oss_specification.c::OSS_SigCreate()
 * \qnx Note: Under QNX the signal created is sended to the process whose 
 *      pid is returned by oss->currentPid()
 *
 * See \ref qnxsigusagesect for more info.
 *
 * \sa OSS_SigRemove, OSS_SigSend
 */
int32 OSS_SigCreate(
    OSS_HANDLE       *oss,
    int32            signal,
    OSS_SIG_HANDLE   **sigP
)
{
	OSS_SIG_HANDLE *sig;

    DBGWRT_1((DBH,"OSS - OSS_SigCreate: sig=0x%ld\n", (long)signal));

	*sigP = NULL;

	/* allocate memory for handle */
	sig = (OSS_SIG_HANDLE *)malloc( sizeof(OSS_SIG_HANDLE) );
	if( sig == NULL )
		return( ERR_OSS_MEM_ALLOC );

	sig->pid = oss->currentPid ? oss->currentPid() : 0;
	sig->sig = signal;

	*sigP = sig;

	return( ERR_SUCCESS );
}/*OSS_SigCreate*/

/**********************************************************************/
/** Destroy signal handle.
 *
 * \copydoc oss_specification.c::OSS_SigRemove()
 *
 * See \ref qnxsigusagesect for more info.
 *
 * \sa OSS_SigCreate, OSS_SigSend
 */
int32 OSS_SigRemove(
    OSS_HANDLE     *oss,
    OSS_SIG_HANDLE **sigP)
{
    OSS_SIG_HANDLE *sig = *sigP;
    pid_t pid;

    DBGWRT_1((DBH,"OSS - OSS_SigRemove: sig=%ld pid=%ld\n",
			  (long)sig->sig,(long)sig->pid));

	pid = oss->currentPid ? oss->currentPid() : 0;

	if (sig->pid != pid) {		/* invalid process ? */
		DBGWRT_ERR((DBH," *** OSS_SigRemove: can't remove signal: "
					"wrong own pid\n"));
        return(ERR_OSS_SIG_CLR);
	}

	/* clear pointer before deallocating the handle to     */
	/* prevent interferences if interrupted by OSS_SigSend */
	*sigP = NULL;
	free( sig );

	return( ERR_SUCCESS );
}

/**********************************************************************/
/** Send signal to a process
 * 
 * \copydoc oss_specification.c::OSS_SigSend()
 *
 * See \ref qnxsigusagesect for more info.
 *
 * \sa OSS_SigCreate, OSS_SigRemove
 */
int32 OSS_SigSend(
    OSS_HANDLE *oss,
    OSS_SIG_HANDLE* sig)
{
    DBGWRT_1((DBH,"OSS - OSS_SigSend: sig=%ld pid=%ld\n",
			  (long)sig->sig,(long)sig->pid));

	if( SignalKill_r( ND_LOCAL_NODE, sig->pid, 0, sig->sig, 0, 0 ) )  {
		DBGWRT_ERR((DBH,"*** OSS_SigSend: error sending signal to pid %ld\n",
					(long)sig->pid ));
		return( ERR_OSS_SIG_SEND );
	}

	return( ERR_SUCCESS );
}
	
/**********************************************************************/
/** Get info about signal
 *
 * \copydoc oss_specification.c::OSS_SigInfo()
 */
int32 OSS_SigInfo(
    OSS_HANDLE     *oss,
    OSS_SIG_HANDLE *sig,
    int32          *signalP,
    int32          *pidP)
{
	*signalP = sig->sig;
	*pidP    = sig->pid;

	return( ERR_SUCCESS );
}

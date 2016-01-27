/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_time.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2004/03/19 14:59:31 $
 *    $Revision: 1.2 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Time related routines of the OSS module
 *      
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_time.c,v $
 * Revision 1.2  2004/03/19 14:59:31  ub
 * adapted for MDIS
 *
 * Revision 1.1  2003/08/04 14:01:42  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"


/**********************************************************************/
/** Let process sleep for specified time.
 * 
 * \copydoc oss_specification.c::OSS_Delay()
 *
 * \todo check what happens if signals arrive faster than 1ms!
 * \sa OSS_MikroDelay
 */
int32 OSS_Delay( OSS_HANDLE *oss, int32 msec )
{
	int32 orgMsec = msec;

	while( msec > 0 )
		msec = delay( msec );
	
	return orgMsec;
}

/**********************************************************************/
/** Initialisation routine for OSS_MikroDelay().
 *
 * \copydoc oss_specification.c::OSS_MikroDelayInit()
 *
 * \qnx calls nanospin_calibrate
 * \sa OSS_MikroDelay
 */
int32 OSS_MikroDelayInit( OSS_HANDLE *oss )
{
	DBGWRT_1((DBH,"OSS_MikroDelayInit\n"));

    /* don't disable interrupts while calib. */
	if( nanospin_calibrate( 0 ) < 0 ) {
		DBGWRT_ERR((DBH,"*** OSS_MikroDelayInit: calib failed %d\n", errno ));
		return ERR_OSS_NO_MIKRODELAY;
	}
    return 0;
}/*OSS_MikroDelayInit*/

/**********************************************************************/
/** Delay execution of a process by using a busy-loop.
 * 
 * \copydoc oss_specification.c::OSS_MikroDelay()
 *
 * \qnx \em nanospin_ns is used.
 */
int32 OSS_MikroDelay( OSS_HANDLE *oss, u_int32 usec )
{
	DBGWRT_1((DBH,"OSS_MikroDelay us=%ld\n", usec));

	nanospin_ns(usec*1000);
    return(0);
}/*OSS_MikroDelay*/


/**********************************************************************/
/** Get the tick rate.
 * 
 * \copydoc oss_specification.c::OSS_TickRateGet()
 *
 * \sa OSS_TickGet
 */
int32 OSS_TickRateGet( OSS_HANDLE *oss )
{
	struct _clockperiod cp;

	ClockPeriod_r( CLOCK_REALTIME, NULL, &cp, 0 );

    return( 1000000000 / cp.nsec );
}/*OSS_TickRateGet*/

/**********************************************************************/
/** Get the current system tick.
 * 
 * \copydoc oss_specification.c::OSS_TickGet()
 *
 * \sa OSS_TickRateGet
 */
u_int32 OSS_TickGet(OSS_HANDLE *oss)
{
	struct _clockperiod cp;
	uint64_t nsec;

	ClockPeriod_r( CLOCK_REALTIME, NULL, &cp, 0 );
	ClockTime_r( CLOCK_REALTIME, NULL, &nsec );

	return( nsec / cp.nsec );
}/*OSS_TickGet*/



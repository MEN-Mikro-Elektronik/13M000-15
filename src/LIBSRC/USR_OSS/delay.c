/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  delay.c
 *
 *      \author  ub
 *        $Date: 2004/03/24 09:45:21 $
 *    $Revision: 1.1 $
 * 
 * 	   \project  MDIS4QNX USR_OSS lib
 *
 *  	 \brief  Delay function
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: delay.c,v $
 * Revision 1.1  2004/03/24 09:45:21  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: delay.c,v 1.1 2004/03/24 09:45:21 ub Exp $";

#include "uos_int.h"

/**********************************************************************/
/** Let a process sleep for a specified time
 *
 * \copydoc usr_oss_specification.c::UOS_Delay()
 *
 * \sa UOS_MikroDelay
 */
int32 UOS_Delay(u_int32 msec)
{
	struct timespec request, remain;
	u_int32 start, end;

	request.tv_sec = msec / 1000;
	request.tv_nsec = (msec % 1000) * 1000000;

	start = UOS_MsecTimerGet();
	end   = start + msec;

	// FIX THIS: WHAT IF END<START!!!
	//printf("ns req %d %d\n", request.tv_sec, request.tv_nsec );

	do {
		if( (nanosleep( &request, &remain ) == -1) && (errno==EINTR)){
			request = remain;
			//printf("ns rem %d %d\n", request.tv_sec, request.tv_nsec );			
		}
		else break;

	} while( end > UOS_MsecTimerGet() );

	return UOS_MsecTimerGet()-start;
}



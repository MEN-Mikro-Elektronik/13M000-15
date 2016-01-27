/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  timer.c
 *
 *      \author  ub
 *        $Date: 2004/03/24 09:45:33 $
 *    $Revision: 1.1 $
 * 
 * 	   \project  MDIS4QNX USR_OSS lib
 *
 *  	 \brief  Timer routines
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: timer.c,v $
 * Revision 1.1  2004/03/24 09:45:33  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
static const char RCSid[]="$Id: timer.c,v 1.1 2004/03/24 09:45:33 ub Exp $";

#include "uos_int.h"
#include <sys/timeb.h>

/**********************************************************************/
/** Read the current timer value in mseconds
 *
 * \copydoc usr_oss_specification.c::UOS_MsecTimerGet()
 * \sa UOS_MsecTimerResolution
 */
u_int32 UOS_MsecTimerGet(void)
{
	struct timeb tb;

	ftime( &tb );				/* query the current time */
	
	return (tb.time * 1000) + tb.millitm;
}

/**********************************************************************/
/** Get timer resolution of UOS_MsecTimerGet()
 *
 * \copydoc usr_oss_specification.c::UOS_MsecTimerResolution()
 * \sa UOS_MsecTimerGet
 */
u_int32 UOS_MsecTimerResolution(void)
{
	return(1);					/* resolution of ftime (I hope) */
}



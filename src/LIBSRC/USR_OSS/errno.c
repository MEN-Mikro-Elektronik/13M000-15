/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  errno.c
 *
 *      \author  ub
 *        $Date: 2004/03/24 09:45:24 $
 *    $Revision: 1.1 $
 * 
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  Error code function
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: errno.c,v $
 * Revision 1.1  2004/03/24 09:45:24  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
static const char RCSid[]="$Id: errno.c,v 1.1 2004/03/24 09:45:24 ub Exp $";

#include "uos_int.h"

/**********************************************************************/
/** Get global error code (task/process specific errno)
 *	
 * \copydoc usr_oss_specification.c::UOS_ErrnoGet()
 * \sa UOS_ErrnoSet, UOS_ErrStringTs, UOS_ErrString
 */
u_int32 UOS_ErrnoGet( void )
{
	return(errno);
}

/**********************************************************************/
/** Set global error code (task/process specific errno)
 * 
 * \copydoc usr_oss_specification.c::UOS_ErrnoSet()
 * \sa UOS_ErrnoGet
 */
u_int32 UOS_ErrnoSet( u_int32 errCode )
{
	errno = errCode;
	return(errCode);
}


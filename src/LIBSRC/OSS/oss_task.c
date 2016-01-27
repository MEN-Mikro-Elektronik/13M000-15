/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_task.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2004/03/24 09:40:54 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Task routines
 *      
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_task.c,v $
 * Revision 1.1  2004/03/24 09:40:54  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"

/**********************************************************************/
/** Get current task id.
 *
 * \copydoc oss_specification.c::OSS_GetPid()
 *
 */
u_int32 OSS_GetPid(OSS_HANDLE *oss)
{
	pid_t pid;
	pid = getpid();
    DBGWRT_1((DBH,"OSS_GetPid = %d\n", pid));

	return(pid);
}/*OSS_GetPid*/


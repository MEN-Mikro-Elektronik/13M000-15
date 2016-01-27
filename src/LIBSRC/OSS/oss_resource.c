/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_resource.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2004/03/24 09:40:49 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Resource related routines of the OSS module
 *      
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_resource.c,v $
 * Revision 1.1  2004/03/24 09:40:49  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"


/**********************************************************************/
/** Assign memory, I/O, interrupt resources to a driver
 * 
 * \copydoc oss_specification.c::OSS_AssignResources()
 *
 * \qnx Currently a no-op for QNX.
 * 
 * \sa OSS_UnAssignResources
 */
int32 OSS_AssignResources(
    OSS_HANDLE      *oss,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
    OSS_RESOURCES   res[]
)
{
	DBGWRT_1((DBH,"OSS_AssignResources\n"));
    return( ERR_SUCCESS );
}

/**********************************************************************/
/** UnAssign the resources e.g. memory space, interrupts...
 * 
 * \copydoc oss_specification.c::OSS_UnAssignResources()
 *
 * \qnx Currently a no-op for QNX.
 * 
 * \sa OSS_AssignResources
 */
int32 OSS_UnAssignResources(
    OSS_HANDLE      *oss,
    int32           busType,
    int32           busNbr,
    int32           resNbr,
    OSS_RESOURCES   res[]
)
{
	DBGWRT_1((DBH,"OSS_UnAssignResources\n"));
    return( ERR_SUCCESS );
}


/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_mem.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2004/03/19 14:59:29 $
 *    $Revision: 1.2 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Memory handling functions of the OSS module
 *      
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_mem.c,v $
 * Revision 1.2  2004/03/19 14:59:29  ub
 * adapted for MDIS
 *
 * Revision 1.1  2003/08/04 14:01:39  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#include "oss_intern.h"

/**********************************************************************/
/** Allocates general memory block.	
 *
 * \copydoc oss_specification.c::OSS_MemGet()
 *
 * \sa OSS_MemFree
 */
void* OSS_MemGet
(
    OSS_HANDLE  *oss,
    u_int32     size,
    u_int32     *gotsizeP
)
{
	void *mem = malloc( size );

	if( mem != NULL )
		*gotsizeP = size;
	else
		*gotsizeP = 0;

	DBGWRT_1((DBH,"OSS_MemGet: size=0x%lx allocated addr=0x%p\n", size, mem ));
	return mem;
}/*OSS_MemGet*/

/**********************************************************************/
/** Return memory block.
 *
 * \copydoc oss_specification.c::OSS_MemFree()
 *
 * \sa OSS_MemGet
 */
int32 OSS_MemFree
(
    OSS_HANDLE *oss,
    void       *addr,
    u_int32    size
)
{
	DBGWRT_1((DBH,"OSS_MemFree: addr=0x%p size=0x%lx\n", addr, size));
	free(addr);
    return(0);
}/*OSS_MemFree*/


/**********************************************************************/
/** Check if memory block accessible by caller
 *
 * \copydoc oss_specification.c::OSS_MemChk()
 *
 * \qnx This function is currently a no-op. 
 */
int32 OSS_MemChk(
    OSS_HANDLE *oss,
    void       *addr,
    u_int32    size,
    int32      mode)
{
	return 0;
}


/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_map.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/08/04 14:01:38 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Memory mapping related routines of the OSS module
 *      
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_map.c,v $
 * Revision 1.1  2003/08/04 14:01:38  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"

/**********************************************************************/
/** Map physical address space to virtual address space
 * \copydoc oss_specification.c::OSS_MapPhysToVirtAddr()
 * \sa OSS_UnMapVirtAddr()
 */
int32 OSS_MapPhysToVirtAddr(
    OSS_HANDLE *oss,
    void       *physAddr,
    u_int32    size,
	int32	   addrSpace,
    int32      busType,
    int32      busNbr,
    void       **virtAddrP
)
{
	int32 error = ERR_SUCCESS;

    DBGWRT_1((DBH,"OSS - OSS_MapPhysToVirtAddr: phys=0x%08x/0x%x bytes\n",
			  physAddr, size));

	if( addrSpace == OSS_ADDRSPACE_MEM ) {
		*virtAddrP = mmap_device_memory( NULL, size, 
										 PROT_READ | PROT_WRITE | PROT_NOCACHE,
										 0, (unsigned)physAddr );
	}
	else {
		/* I/O */
		*virtAddrP = (void *)mmap_device_io( size, (unsigned)physAddr );
	}

	if( *virtAddrP == MAP_FAILED ) {
		error = ERR_OSS_MAP_FAILED;
		*virtAddrP = NULL;
	}

	DBGWRT_2((DBH," virtAddr=0x%p errno=0x%x\n", *virtAddrP, errno ));
	return(error);
}

/**********************************************************************/
/** Unmap virtual address space
 * \copydoc oss_specification.c::OSS_UnMapVirtAddr()
 * \sa OSS_MapVirtAddr()
 */
int32 OSS_UnMapVirtAddr(
    OSS_HANDLE *oss,
    void       **virtAddrP,
    u_int32    size,
	int32	   addrSpace
)
{
	int32 error=ERR_SUCCESS;
    DBGWRT_1((DBH,"OSS - OSS_UnMapVirtAddr: virt=0x%08x\n",*virtAddrP));

	if( addrSpace == OSS_ADDRSPACE_MEM ) {
		if( munmap_device_memory( *virtAddrP, size ) == -1 )
			error = ERR_OSS_MAP_FAILED;		
	}
	else {
		if( munmap_device_io( (uintptr_t)*virtAddrP, size ) == -1 )
			error = ERR_OSS_MAP_FAILED;
	}
	*virtAddrP = NULL;
	return error;
}

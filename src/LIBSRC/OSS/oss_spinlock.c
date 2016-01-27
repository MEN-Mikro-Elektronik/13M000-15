/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_spinlock.c
 *
 *      \author  ch
 *        $Date: 2011/09/01 18:09:55 $
 *    $Revision: 1.1 $
 * 
 *     \project  MDIS4 QNX
 *       \brief  Spinlock functions
 *      
 *    \switches  DBG - enable debugging
 *                       
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_spinlock.c,v $
 * Revision 1.1  2011/09/01 18:09:55  channoyer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 2011 MEN Mikro Elektronik GmbH. All rights reserved.
 ****************************************************************************/
 
 #include "oss_intern.h"
 
/**********************************************************************/
/** Create a spin lock.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockCreate()
 *
 *  \windows IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockRemove
 */
int32 OSS_SpinLockCreate(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    **spinlP )
{
    OSS_SPINL_HANDLE *intSpinlHdl=NULL;
    u_int32 size;
    
    DBGWRT_1(( DBH,  "<%s> :OSS_SpinlockCreate: entered\n", oss->instName ));
    DBGWRT_1(( DBH, " size of OSS_SPINL_HANDLE: %d \n", sizeof(OSS_SPINL_HANDLE)));
    *spinlP = NULL;
    
    // alloc mem for spin lock handle
    intSpinlHdl = (OSS_SPINL_HANDLE*)OSS_MemGet(
                    oss, sizeof(OSS_SPINL_HANDLE), &size );
    if( !intSpinlHdl || size != sizeof(OSS_SPINL_HANDLE) )
        return ERR_OSS_MEM_ALLOC;
    
    // init spin lock
    /* TODO */
    // *intSpinlHdl = 0;
	// memset(intSpinlHdl, 0, sizeof(OSS_SPINL_HANDLE));
	if( pthread_spin_init(intSpinlHdl, PTHREAD_PROCESS_SHARED) != EOK )
	  return ERR_OSS_MEM_ALLOC;

    // set spin lock handle
    *spinlP = intSpinlHdl;

    return ERR_SUCCESS;
}
 
/**********************************************************************/
/** Destroy spin lock handle.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockRemove()
 *
 *  \windows Do nothing\n
 *  IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockCreate
 */
int32 OSS_SpinLockRemove(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    **spinlP )
{
    OSS_SPINL_HANDLE    *intSpinlHdl = *spinlP;
    
    DBGWRT_1((DBH,  "<%s> :OSS_SpinLockRemove: entered\n", 
                    oss->instName ));
    
    // invalid spin lock handle?
    if( intSpinlHdl == NULL ){
        DBGWRT_ERR( ( DBH, "<%s> : *** OSS_SpinLockRemove: ",
                           "invalid spin lock handle\n", oss->instName ));
        return ERR_OSS_ILL_HANDLE;
    }

	if( pthread_spin_destroy(intSpinlHdl) != EOK )
	  return ERR_OSS_ILL_HANDLE;

    // clear spin lock handle
    *spinlP = NULL;

    // free memory of spin lock handle
    OSS_MemFree( oss, intSpinlHdl, 0 );

    return ERR_SUCCESS;                   
} 

/**********************************************************************/
/** Acquire spin lock.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockAcquire()
 *
 *  \windows IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockRelease
 */
int32 OSS_SpinLockAcquire(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    *spinl )
{
    DBGWRT_1((DBH,  "<%s> :OSS_SpinLockAcquire: entered\n", 
                    oss->instName ));
                    
    // invalid spin lock handle?
    if( spinl == NULL ){
        DBGWRT_ERR( ( DBH, "<%s> : *** OSS_SpinLockAcquire: ",
                           "invalid spin lock handle\n", oss->instName ));
        return ERR_OSS_ILL_HANDLE;
    }
    
    /* TODO */
    // preempt_disable();  // disable local preemption
    
    // spin
    /* TODO */
    // spin_lock( spinl );
    //InterruptLock( &spinl );
	if( pthread_spin_lock(spinl) != EOK )
	  return ERR_OSS_ILL_HANDLE;

    return ERR_SUCCESS;
}

/**********************************************************************/
/** Release spin lock.
 *
 *  \copydoc oss_specification.c::OSS_SpinLockRelease()
 *
 *  \windows IRQL requirement: Any IRQL
 *
 *  \sa OSS_SpinLockAcquire
 */
int32 OSS_SpinLockRelease(
    OSS_HANDLE          *oss,
    OSS_SPINL_HANDLE    *spinl )
{
    DBGWRT_1((DBH,  "<%s> :OSS_SpinLockRelease: entered\n", 
                oss->instName ));
                    
    // invalid spin lock handle?
    if( spinl == NULL ){
        DBGWRT_ERR( ( DBH, "<%s> : *** OSS_SpinLockRelease: ",
                           "invalid spin lock handle\n", oss->instName ));
        return ERR_OSS_ILL_HANDLE;
    }
    
    /* TODO */
    //if (spin_is_locked( spinl ) ) { 
    //DBGWRT_3((DBH,"   OSS_SpinLockRelease actually restore\n"));
    // spin_unlock( spinl ); 
        
    //preempt_enable(); // re-enable local preemption
    //}
    //InterruptUnlock( &spinl );
	if( pthread_spin_unlock(spinl) != EOK )
	  return ERR_OSS_ILL_HANDLE;

    return ERR_SUCCESS;
}




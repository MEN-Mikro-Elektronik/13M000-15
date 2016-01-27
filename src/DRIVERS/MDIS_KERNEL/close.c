/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  close.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2014/09/22 17:14:32 $
 *    $Revision: 1.5 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  MDIS kernel resource manager
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: close.c,v $
 * Revision 1.5  2014/09/22 17:14:32  channoyer
 * R: Not possible to use MDIS over Qnet
 * M: Add node ID to identify thread using MDIS device
 *
 * Revision 1.4  2011/04/21 18:46:57  channoyer
 * R: Segfault can occur if an interrupt raised during MDIS_FinalClose
 * M: Set device as not initialised before doing the de-init
 *
 * Revision 1.3  2010/06/18 19:20:11  channoyer
 * R: On SMP system the MDIS devices thread can be terminated before the call of ThreadDestroy_r()
 * M: Remove the call of ThreadDestroy_r(): the MDIS devices thread exit when destroying the channel just before.
 *
 * Revision 1.2  2004/03/25 08:50:30  ub
 * added switch USE_VME_SERVER
 *
 * Revision 1.1  2004/03/24 11:35:10  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#include "mk_intern.h"


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int   devicesUsingDriver( char *name );
static MK_THREAD *threadUsedByThisDev( MK_DEV *dev, pid_t pid, int tid, int nid );
static int32 removeSysirq(MK_DEV *dev);
static int32 clrMiface( MK_DEV *dev );
static void  termLockMode( MK_DEV *dev );
static void  unlockSemaphores( MK_PATH *path );



/**********************************************************************/
/** Determine how many devices use given driver
 *
 *  \param  name    \IN   name to look for
 *
 *  \return number of devices
 */
static int
devicesUsingDriver( char *name )
{
	MK_DEV *node;
    int    num = 0;

	for( node=(MK_DEV *)G_devList.head;
		 node->node.next;
		 node = (MK_DEV *)node->node.next ) {

		if( strcmp(node->drv->drvName, name ) == 0 )
			++num;
	}

	return( num );
}


/**********************************************************************/
/** Search for thread with given pid/tid used by device
 *
 *  \param  dev   \IN   device to search for thread
 *  \param  pid   \IN   process id of thread
 *  \param  tid   \IN   thread id of thread
 *
 *  \return pointer to thread or NULL if none found
 */

static MK_THREAD *
threadUsedByThisDev( MK_DEV *dev, pid_t pid, int tid, int nid )
{
	MK_PATH *path;

    for( path=(MK_PATH *)dev->pathList.head;
         path->node.next;
         path = (MK_PATH *)path->node.next ) {

        if( (tid == path->thread->appTid) && 
            (pid == path->thread->appPid) &&
            (nid == path->thread->appNid) )
            goto FOUND;
    }

    return( NULL );

 FOUND:
	return( path->thread );
}

/**********************************************************************/
/** Remove & disable the system interrupt
 *
 *  \param  dev       \IN   device structure
 *
 *  \return 0 on success or \c ERR_xxx error code on error
 */

static int32
removeSysirq(MK_DEV *dev)
{
    DBGWRT_1((DBH, "remove system interrupt for %s: irqId=%d vect=%d\n",
              dev->devName, dev->irqId, dev->irqVector ));

	/* remove system irq */
	if (dev->irqInstalled) {
		DBGWRT_2((DBH," remove system interrupt: vect=%d irqId=%d\n",
				  dev->irqVector, dev->irqId));

		if( dev->brdBusType == OSS_BUSTYPE_VME ) {
#ifdef USE_VME_SERVER
			if( MenC_VmeInterruptDetachEvent( dev->vmeHdl, dev->irqId ) )
                return( ERR_MK_IRQ_REMOVE );
#endif /* USE_VME_SERVER */
        }
		else {
			if( InterruptDetach( dev->irqId ) )
                return( ERR_MK_IRQ_REMOVE );
        }

		dev->irqInstalled = 0;
	}

	return( ERR_SUCCESS );
}


/**********************************************************************/
/** Delete locking semaphores
 *
 *  \param  dev       \IN   device structure
 */

static void
termLockMode( MK_DEV *dev )
{
	int32 n;

	switch(dev->lockMode) {
	case LL_LOCK_NONE:
		break;

	case LL_LOCK_CALL:
        for( n=0; n < mdis_call_num; ++n ) {
            if( dev->semCall[n] )   
                OSS_SemRemove( dev->osh, &dev->semCall[n] );
        }
		break;
	case LL_LOCK_CHAN:
		if( dev->semChanP ){
			for( n=0; n<dev->devNrChan; n++ ){
				if( dev->semChanP[n] )
					OSS_SemRemove( dev->osh, &dev->semChanP[n]);
			}
			OSS_MemFree( dev->osh, dev->semChanP, dev->semChanAlloc );
		}
		break;
	}
}

/**********************************************************************/
/** Unlock all semaphores held by given path.
 *
 *  \param  path    \IN   path to check for locked semaphores
 */

static void
unlockSemaphores( MK_PATH *path )
{
    MK_DEV *dev = path->dev;
    int i;

    /*--- unlock semaphores if locked by the thread using <path> ---*/

    /*--- call sems ---*/
    for( i=0; i<mdis_call_num; ++i ) {
        if( path->locked.call[i] && dev->semCall[i] )
            OSS_SemSignal( dev->osh, dev->semCall[i] );
    }

    /*--- channel sems ---*/
    for( i=0; i<dev->devNrChan; ++i ) {
        if(path->locked.chan[i] && dev->semChanP[i] ) 
            OSS_SemSignal( dev->osh, dev->semChanP[i] );
    }

    /*--- device sem ---*/
    if( path->locked.dev && dev->semDev )
        OSS_SemSignal( dev->osh, dev->semDev );
}

/**********************************************************************/
/** Clears the device slot if this was the last device on slot
 *
 *  \param  dev   \IN   device structure
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32
clrMiface( MK_DEV *dev )
{
	MK_DEV *dev2;
	int32 error;

	/* check if another device is using the interface */
	for( dev2=(MK_DEV *)G_devList.head;
		 dev2->node.next;
		 dev2 = (MK_DEV *)dev2->node.next ){

		if( strcmp(dev2->brdName, dev->brdName) == 0 &&
			dev2->devSlot == dev->devSlot ){

			/* yes, another device is using the interface */
			return 0;
		}
	}

	if( (error = dev->brdJumpTbl.clrMIface(dev->brd, dev->devSlot))) {
		DBGWRT_ERR((DBH,"*** MK: can't deinit board slot\n"));
	}
	return error;
}

/**********************************************************************/
/** Closes MDIS device
 *
 *  \param  mcl   \IN   information about device to be closed
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa MDIS_OpenDevice
 */

int32
MDIS_CloseDevice( MDIS_CLOSE_DEVICE_DATA *mcl )
{
    MK_DEV    *dev;
    MK_THREAD *thread;
    MK_PATH   *path;
    int       error = 0;

    DBGWRT_1((DBH, "MDIS_CloseDevice: \n"));

    path = (MK_PATH*)mcl->path;
    dev  = path->dev;
    DBGWRT_2((DBH, "%s  (0x%p)\n", dev->devName, dev ));


    /*--- check if thread registered with this device ---*/
    if( (thread = threadUsedByThisDev( dev, mcl->pid, mcl->tid, mcl->nid )) == NULL )
        return( ERR_MK_ILL_PARAM );

    MK_LOCK( error );

    if( error )
        return( error );

    /*--- erase thread if not servicing any other device ---*/
    if( --thread->useCount == 0 ) {

        /*--- remove channel ---*/
        ChannelDestroy_r( thread->mkCid );

        /*--- stop thread ---*/
        ThreadDetach_r( thread->mkTid );

        /*--- cleanup ---*/
        free( thread );
    }

    /*--- unlock semaphores locked by this path ---*/
    unlockSemaphores( path );

    /*--- remove path struct ---*/
    OSS_DL_Remove( &path->node );
    free( path->locked.chan );
    free( path );

    /*--- remove device struct ---*/
    if( --dev->useCount == 0 ) {
        MDIS_FinalClose( dev );
    }

    MK_UNLOCK();

    return( ERR_SUCCESS );
}


/**********************************************************************/
/** Close last path to device
 *
 *  \param  dev   \IN   device to be closed
 *
 *  \return 0 on success or \c ERR_xxx error code on error
 *
 *  \sa MDIS_InitialOpen
 */

int32 MDIS_FinalClose( MK_DEV *dev )
{
    int32 error,n;

	if( dev == NULL )
        return ERR_SUCCESS;

    DBGWRT_1((DBH,"MK - FinalClose: device=%s\n", dev->devName));


	/*------------------------------+
	|  de-init device               |
	+------------------------------*/
	dev->initialized = FALSE;
	/* disable irqs (if installed), ignore error */
	if (dev->irqUse) {
		MDIS_EnableIrq( dev, FALSE );
	}


	/* de-init device */
	if (dev->ll && (error = dev->llJumpTbl.exit(&dev->ll ))) {
		DBGWRT_ERR((DBH," *** FinalClose: device exit failed\n"));
	}

	/*------------------------------+
	|  Terminate process locking    |
	+------------------------------*/
	termLockMode( dev );

	/*------------------------------+
	|  cleanup interrupt            |
	+------------------------------*/
	removeSysirq( dev );

	/*------------------------------+
	|  cleanup board slot           |
	+------------------------------*/
	if( dev->brd ){
		DBGWRT_2((DBH," deinit board slot\n"));
		clrMiface( dev );
	}

	/*------------------------------+
	|  remove device semaphore      |
	+------------------------------*/
	if( dev->semDev )
        OSS_SemRemove( dev->osh, &dev->semDev );

	/*------------------------------+
	|  unmap address spaces         |
	+------------------------------*/
	for (n=0; n<dev->spaceCnt; n++) {	/* for all addr spaces */
		if( dev->space[n].flags & MK_MAPPED ){
			if( (error = OSS_UnMapVirtAddr(dev->osh,
										  &dev->space[n].virtAddr,
										  dev->space[n].reqSize,
										  dev->space[n].type))) {
				DBGWRT_ERR((DBH," *** FinalClose: can't unmap addr "
							"space[%d]\n",n));
			}
		}
	}

	/*------------------------------+
	|  close BBIS driver            |
	+------------------------------*/
	DBGWRT_2((DBH," close board\n"));
	bbis_close( dev->brdName );

	/*--------------------------+
	|  unload low level driver  |
	+--------------------------*/
    if( devicesUsingDriver( dev->drv->drvName ) <= 1 ) {
        MK_DRV *drv;

        DBGWRT_3((DBH, "unloading ll driver %s\n", dev->drv->drvName ));

        if( (drv = MDIS_FindDrvByName( dev->drv->drvName )) ) {
            dlclose( drv->drvHandle );
            OSS_DL_Remove( &drv->node );
            free( drv );
        }
    }


	/*-----------------------------------------------+
	|  Deregister device in global MDIS device list  |
	+-----------------------------------------------*/
	if( dev->node.next ){
		OSS_DL_Remove( &dev->node );
	}

	if( dev->osh )
		OSS_Exit( &dev->osh );

	free( dev );

    return( ERR_SUCCESS );
}

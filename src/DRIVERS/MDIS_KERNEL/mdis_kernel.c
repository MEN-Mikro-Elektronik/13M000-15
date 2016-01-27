/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  mdis_kernel.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2014/10/29 11:51:16 $
 *    $Revision: 1.8 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  MDIS kernel resource manager
 *
 *    \switches  USE_VME_SERVER
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mdis_kernel.c,v $
 * Revision 1.8  2014/10/29 11:51:16  channoyer
 * R: mdis_kernel not executed in background
 * M: Add call to procmgr_daemon() to run mdis_kernel in the background
 *
 * Revision 1.7  2014/09/22 17:18:40  channoyer
 * R: Not possible to use MDIS over Qnet
 * M: Add node ID to identify thread using MDIS device
 *
 * Revision 1.6  2013/10/01 11:16:41  channoyer
 * R: MDIS Kernel stop with SIGSEGV when device not closed with M_close
 * M:1. Managed the case of EINTR in MDSI_Handler
 *   2. In MDIS_Watchdog managed the case where the path can be NULL
 *
 * Revision 1.5  2009/07/09 11:03:16  channoyer
 * R: QNX 6.4 warning: pointer targets differ in signedness
 * M: change variable type
 *
 * Revision 1.4  2009/06/05 15:55:42  channoyer
 * R: No possibility to use MDIS kernel server for native driver
 * M: Add messages for native driver
 *
 * Revision 1.3  2008/11/13 15:42:30  channoyer
 * R: Missing menlogo.gif during documentation built
 * M: Change the dummy page name to unique name
 *
 * Revision 1.2  2008/03/13 10:14:28  channoyer
 * + Add a difference between return value 0 and <0 for MsgReceivev_r().
 * Because the return value ESRCH can occured during the call of the function
 * ChannelDestroy_r() during the close. In this case, if the mdis_kernel thread
 * is at an higher priority of the close thread, then the channel was destroyed
 * and the MDIS_Hanlder entered in an endless loop trying to call MsgReceivev_r()
 * on the destroyed channel.
 *
 * Revision 1.1  2004/03/25 08:50:27  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*! \mainpage

\anchor qnx_mdis_kernel
 
 This is the documentation of the MEN MDIS kernel resource manager for QNX. 

 The MDIS kernel is compiled in two variants:
 - \c mdis_kernel : standard version, normally used
 - \c mdis_kernel_dbg : debug version, slower, amount of debug output can 
   be determined using descriptor of low-level driver. See MDIS Descriptor 
   Guide and example configurations for more info.

 The MDIS kernel is a standard QNX resource manager. Its main tasks are
 - allocation and deallocation of resources for the managed MDIS 
   low-level drivers (i.e. handling of the M_open() and M_close() API call).

 - management of hardware interrupts for the MDIS devices 

 - communication with the application

 Implemented features:
 - open()/close()
 - devctl() : Used for opening and closing MDIS devices

 All MDIS calls except for M_open/M_close are communicated directly between 
 application and kernel using the low-level QNX communication mechanisms 
 (MsgSend(), MsgReply() etc.)

 The MDIS kernel resource manager is intended to be used via the accompanying
 MDIS API library which provides the MDIS calls to the application.

*/
/*! \page mdis_kernel_dummy
 \menimages
*/


/*--------------------------------------+
|  INCLUDES                             |
+---------------------------------------*/
#include "mk_intern.h"


/*--------------------------------------+
|  DEFINES                              |
+---------------------------------------*/
#define MK_IRQ_HANDLER_PRIORITY         30


/*--------------------------------------+
|  TYPEDEFS                             |
+---------------------------------------*/

/*! parameters passed via devctl() call to MDIS kernel */
typedef union _mk_data_t {
    MDIS_OPEN_DEVICE_DATA     open;             /*!< parameters for open call*/
    MDIS_CLOSE_DEVICE_DATA    close;            /*!< parameters for close */
} mk_data_t;

/*! return values from devctl() call */
typedef union _mk_return_t {
    MDIS_OPEN_DEVICE_RETURN   open;             /*!< ret values of open call */
} mk_return_t;


/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static resmgr_connect_funcs_t   connect_funcs;
static resmgr_io_funcs_t        io_funcs;
static iofunc_attr_t            attr;

u_int32         mk_dbglevel = OSS_DBG_DEFAULT;    /* debug level */
DBG_HANDLE      *G_dbh;                           /* debug handle */
OSS_HANDLE      *G_osh;                           /* OSS handle */
OSS_SEM_HANDLE  *G_mkLockSem;                     /* MDIS kernel lock */
OSS_DL_LIST     G_drvList;                        /* list of drivers */
OSS_DL_LIST     G_devList;                        /* list of devices */
IRQ_THREAD      G_irqThread;                      /* global irq thread */


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int io_devctl(
    resmgr_context_t *ctp,
    io_devctl_t      *msg,
    RESMGR_OCB_T     *ocb);

static void* MDIS_Watchdog( void *arg );


/**********************************************************************/
/** Main routine
 *
 *  Initialisation of main data structures. Creation of IRQ and Watchdog
 *  handler threads. Contains the resource manager main loop.
 *
 *  \param  argc  \IN      number of arguments
 *  \param  argv  \IN      argument vector
 *
 *  \return error code if init failed
 *
 */

int
main(int argc, char **argv)
{
    resmgr_attr_t        resmgr_attr;
    dispatch_t           *dpp;
    dispatch_context_t   *ctp;
    int                  id, wdTid;


    DBG_MYLEVEL = DBG_ALL;

    /*--- init tracing ---*/
    TRACE_INIT;

    /* trace some kernel calls from this process */
    TRACE_EVENT((_NTO_TRACE_ADDCLASS, _NTO_TRACE_KERCALL));

    TRACE_EVENT(( _NTO_TRACE_SETCLASSPID, _NTO_TRACE_KERCALL, getpid() ));
    TRACE_EVENT(( _NTO_TRACE_SETEVENTPID, _NTO_TRACE_KERCALL,
               __KER_CONNECT_ATTACH, getpid() ));
    TRACE_EVENT(( _NTO_TRACE_SETEVENTPID, _NTO_TRACE_KERCALL,
               __KER_MSG_SENDV, getpid() ));
    TRACE_EVENT(( _NTO_TRACE_SETEVENTPID, _NTO_TRACE_KERCALL,
               __KER_MSG_RECEIVEV, getpid() ));
    TRACE_EVENT(( _NTO_TRACE_SETEVENTPID, _NTO_TRACE_KERCALL,
               __KER_MSG_REPLYV, getpid() ));

    /* trace interrupts */
    TRACE_EVENT((_NTO_TRACE_ADDCLASS, _NTO_TRACE_INT));
    TRACE_EVENT((_NTO_TRACE_DELEVENT, _NTO_TRACE_INT));
    TRACE_EVENT((_NTO_TRACE_ADDEVENT, _NTO_TRACE_INT, 10));

    TRACE_START;
    TRACE_USR("--- start ---" );
    TRACE_EVENT((_NTO_TRACE_FLUSHBUFFER));
    TRACE_STOP;


	/*---init OSS ---*/
	if( OSS_Init( "MDIS_KERNEL", &OSH, MDIS_CurrentPid )){
		return( EXIT_FAILURE );
	}

    OSS_DbgLevelSet( OSH, 0 );

	/*--- create global MDIS lock sem ---*/
	if( OSS_SemCreate(OSH, OSS_SEM_BIN, 1, &G_mkLockSem )){
        OSS_Exit( &OSH );
        return( EXIT_FAILURE );
	}

	/*--- init lists ---*/
	OSS_DL_NewList( &G_drvList );
	OSS_DL_NewList( &G_devList );

    /*--- init bbis library ---*/
    if( bbis_init() ) {
        OSS_Exit( &OSH );
        return( EXIT_FAILURE );
	}

	/*--- install watchdog thread ---*/
    if( pthread_create( &wdTid, NULL, MDIS_Watchdog, NULL ) )
        return( EXIT_FAILURE );

    /*--- create communication channel for irq thread ---*/
    if( (G_irqThread.cid = ChannelCreate_r( 0 )) < 0 )
        return( EXIT_FAILURE );

    if( (G_irqThread.coid = ConnectAttach_r( 0, getpid(),
                                             G_irqThread.cid,
                                             _NTO_SIDE_CHANNEL,
                                             _NTO_COF_CLOEXEC )) < 0 ) {
        return( EXIT_FAILURE );
    }

	/*--- install the global interrupt handler thread ---*/
    if( pthread_create( &G_irqThread.tid, NULL, MDIS_IrqHandler, NULL ) )
        return( EXIT_FAILURE );

    /*--- init resource manager ---*/
    /* initialize dispatch interface */
    if((dpp = dispatch_create()) == NULL) {
        fprintf(stderr, "%s: Unable to allocate dispatch handle.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /*--- initialize resource manager attributes ---*/
    memset(&resmgr_attr, 0, sizeof resmgr_attr);
    resmgr_attr.nparts_max = 1;
    resmgr_attr.msg_max_size = 2048;

    /*--- initialize functions for handling messages ---*/
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);
    io_funcs.devctl = io_devctl; /* for handling _IO_DEVCTL, sent by devctl()*/

    /*--- initialize attribute structure used by the device ---*/
    iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);
    attr.nbytes = 0;

    /*--- attach our device name ---*/
    if((id = resmgr_attach(dpp, &resmgr_attr, "/dev/mdis", _FTYPE_ANY, 0,
                 &connect_funcs, &io_funcs, &attr)) == -1) {
        fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
        return( EXIT_FAILURE );
    }

#if 0 	/* For debugging, keep around stdout/stderr */
	procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE);
#else
	procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NODEVNULL | 
				   PROCMGR_DAEMON_NOCLOSE);
#endif

    /*--- allocate a context structure ---*/
    ctp = dispatch_context_alloc(dpp);

    /*--- start the resource manager message loop ---*/
    while(1) {
        if((ctp = dispatch_block(ctp)) == NULL) {
            fprintf(stderr, "block error\n");
            return( EXIT_FAILURE );
        }
        dispatch_handler(ctp);
    }
}


/**********************************************************************/
/** Handler for QNX devctl() system call on MDIS kernel.
 *
 *  Implements the functionality for opening and closing of  MDIS
 *  devices via the QNX devctl() call.
 *
 *  \param  ctp   \IN      QNX context pointer (see QNX doc)
 *  \param  msg   \INOUT   request from / reply to application
 *  \param  ocb   \IN      QNX open control block
 *
 *  \return devctl return value
 *
 */

static int
io_devctl(
    resmgr_context_t *ctp,
    io_devctl_t      *msg,
    RESMGR_OCB_T     *ocb)
{
    int        nbytes, status;
    mk_data_t   *in;                 /* input */
    mk_return_t *out;                /* output */


    /* let common code handle DCMD_ALL_* cases */
    if ((status = iofunc_devctl_default(ctp, msg, ocb)) != _RESMGR_DEFAULT) {
        return(status);
    }
    status = nbytes = 0;

    /* arguments to M_OPEN and M_CLOSE fit into one single message block */
    in = _DEVCTL_DATA(msg->i);

    /* devctl operations of MDIS kernel */
    switch (msg->i.dcmd) {
    case MDIS_DEVCTL_M_CLOSE:
        DBGWRT_2((DBH,"MDIS_DEVCTL_M_CLOSE from process %d\n",
                  in->close.pid ));

        status = MDIS_CloseDevice( &(in->close) );
        DBGWRT_3((DBH, "MDIS_CloseDevice() returned %d (0x%x)\n", 
                  status, status ));
        nbytes = 0;
        break;

    case MDIS_DEVCTL_M_OPEN:
    {
        MDIS_OPEN_DEVICE_RETURN r;
        DBGWRT_2((DBH, "MDIS_DEVCTL_M_OPEN\n" ));

        status = MDIS_OpenDevice( &(in->open), &r );

        /* Copy the return message */
        nbytes = sizeof( msg->o );
        memset( &msg->o, 0, nbytes );
        out = _DEVCTL_DATA(msg->o);
        out->open = r;
        break;
    }

    default:
        return(ENOSYS);
    }

    /* return result  */
    msg->o.ret_val = status;

    /* Indicate the number of bytes and return the message */
    msg->o.nbytes = nbytes;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes));
}

#ifdef DBG
/*-------- variables for tests ----------*/
static char G_MsgidStr[6][40] = {
    "READ",
    "WRITE",
    "GETBLOCK",
    "SETBLOCK",
    "GETSTAT",
    "SETSTAT"
};
#endif /* DBG */

/**********************************************************************/
/** Handler thread for MDIS devices
 *
 * This code is used by all MDIS kernel threads. For every application 
 * thread which opens an MDIS device the MDIS kernel creates a handler 
 * thread. This "kernel threads" then performs all device operations
 * (read, write etc.) in behalf of the application.
 *
 * All MDIS kernel thread share the same address space and therefore have
 * access to commonly used data structures inside the MDIS kernel.
 *
 * For every application thread only one kernel thread is generated even if
 * it opens more than one device. A kernel thread is removed if the last
 * device of the corresponding app thread is closed.
 *
 *  \param  arg     \IN   thread information
 *
 *  \return never returns
 */

void*
MDIS_Handler( void *arg )
{
    MK_PATH          *path;
    MK_THREAD        *t =  (MK_THREAD*)arg;
    MDIS_MSGHEADER   h;
    iov_t            siov[2];
    int              rcvid;
    int32            rbuf[1];
    u_int8           *buf = NULL;
    struct _msg_info info;
    MDIS_NATDEV_RETURN *mdis;

    SETIOV( &siov[0], &h, sizeof(h) );

    for(;;) {
        int32 rv = ERR_SUCCESS, rsize = 0;

        /*--- get message ---*/
        rcvid = MsgReceivev_r( t->mkCid, &siov[0], 1, &info );

        if( rcvid == 0 ) {
            DBGWRT_2((DBH, "MDIS_Handler tid=%d got pulse: rcvid=0x%x <<<\n", 
                      t->mkTid, rcvid ));
            continue;
        } else if( rcvid < 0 ) {
		  if(ESRCH == -rcvid) {
            DBGWRT_2((DBH, "MDIS_Handler tid=%d got MsgReceivev_r "
                      "error:0x%x <<<\n", t->mkTid, rcvid ));
            break;
		  }
		  if(EINTR == -rcvid) {
            DBGWRT_2((DBH, "MDIS_Handler tid=%d got MsgReceivev_r "
                      "error (EINTR):0x%x <<<\n", t->mkTid, rcvid ));
            continue;
		  }
        }

        path = (MK_PATH*)h.path;
        buf = NULL;
        DBGWRT_2((DBH, "%s: path=0x%p device=%s (0x%p)\n",
                G_MsgidStr[h.id], path, path->dev->devName, path->dev ));

        /*--- select by ID ---*/
        switch( h.id ) {
            case MDIS_MSGID_READ:
                rv = MDIS_Read( path, &rbuf[0] );
                rsize = sizeof(int32);
                break;

            case MDIS_MSGID_WRITE:
                rv = MDIS_Write( path, h.val );
                rsize = 0;
                break;

            case MDIS_MSGID_GETBLOCK:
                if( (buf = malloc( h.length )) == NULL ) {
                    rv = -ERR_OSS_MEM_ALLOC;
                    rsize = 0;
                    break;
                }

                if( (rv = MDIS_Getblock( path, buf, h.length ) ) >= 0 )
                    rsize = rv;
                else
                    rsize = 0;
                break;

            case MDIS_MSGID_SETBLOCK:
                if( (buf = malloc( h.length )) == NULL ) {
                    rv = -ERR_OSS_MEM_ALLOC;
                    rsize = 0;
                    break;
                }

                SETIOV( &siov[1], buf, h.length );

                if( (rv = MsgReadv_r( rcvid, &siov[1], 1, sizeof(h) ) ) < 0 ) {
                    rsize = 0;
                    break;
                }

                rv = MDIS_Setblock( path, buf, h.length );
                break;

            case MDIS_MSGID_GETSTAT:
                if( h.code & M_OFFS_STD ) {
                    /*--- standard code ---*/
                    rv = MDIS_GetstatStd( path, h.code, &rbuf[0] );
                    rsize = sizeof( int32 );
                }
                else if( h.code & M_OFFS_BLK ) {
                    /*--- block code ---*/
                    if( (buf = malloc( h.length )) == NULL ) {
                        rv = -ERR_OSS_MEM_ALLOC;
                        rsize = 0;
                        break;
                    }

                    SETIOV( &siov[1], buf, h.length );

                    if( (rv = MsgReadv_r( rcvid, &siov[1], 1, sizeof(h) ) )
                        < 0 ) {
                        rsize = 0;
                        break;
                    }

                    rv = MDIS_GetstatBlk( path, h.code, buf, h.length );
                    rsize = h.length;
                }
                else
                    rv = -ERR_MK_UNK_CODE;

                break;

            case MDIS_MSGID_SETSTAT:
                if( h.code & M_OFFS_STD ) {
                    /*--- standard code ---*/
                    rv = MDIS_SetstatStd( path, h.code, h.val );
                    rsize = 4;
                }
                else if( h.code & M_OFFS_BLK ) {
                    /*--- block code ---*/
                    if( (buf = malloc( h.length )) == NULL ) {
                        rv = -ERR_OSS_MEM_ALLOC;
                        rsize = 0;
                        break;
                    }

                    SETIOV( &siov[1], buf, h.length );

                    if( (rv = MsgReadv_r( rcvid, &siov[1], 1, sizeof(h) ) )
                        < 0 ) {
                        rsize = 0;
                        break;
                    }

                    rv = MDIS_SetstatBlk( path, h.code, buf, h.length );
                    rsize = 0;
                }
                else
                    rv = -ERR_MK_UNK_CODE;

                break;

            case MDIS_MSGID_NATGET:
                if( (buf = malloc( h.length )) == NULL ) {
                    rv = -ERR_OSS_MEM_ALLOC;
                    rsize = 0;
                    break;
                }
                mdis = (MDIS_NATDEV_RETURN *) buf;
                mdis->physAddr = path->dev->space[0].physAddr;
                mdis->irqVector = path->dev->irqVector;
                rv = ERR_SUCCESS;
                rsize = h.length;
                break;

            case MDIS_MSGID_NATIRQ:
                path->dev->extIrqUse = 1;
                SETIOV( &siov[1], &(path->dev->irqThread), h.length );

                if( (rv = MsgReadv_r( rcvid, &siov[1], 1, sizeof(h) ) ) < 0 ) {
                    rsize = 0;
                    break;
                }
                rsize = 0;
                path->dev->irqThread.coid = ConnectAttach( 0, path->dev->irqThread.tid, path->dev->irqThread.cid, _NTO_SIDE_CHANNEL, 0 );

                break;

            default:
                rv = ERR_MK_UNK_CODE;
        }

        if( buf ) {
            MsgReply_r( rcvid, rv, buf, rsize );
            free( buf );
        }
        else {
            MsgReply_r( rcvid, rv, rbuf, rsize );
        }

    } /* end for(;;) */

    return( NULL );
}


/**********************************************************************/
/** MDIS kernel IRQ thread
 *
 *  This thread handles interrupt requests from all MDIS devices.
 *
 *  \param  arg    \IN   not used
 *
 *  \return always NULL
 */

void*
MDIS_IrqHandler( void *arg )
{
    struct _pulse p;
    struct sched_param param;

    ThreadCtl( _NTO_TCTL_IO, NULL );

	/* raise my priority */
    param.sched_priority = MK_IRQ_HANDLER_PRIORITY;

    if( SchedSet( 0, 0, SCHED_NOCHANGE, &param ) < 0 ) {
        DBGWRT_ERR((DBH, "*** Error: Can't set priority for IrqHandler\n" ));
        return( NULL );
    }

    for(;;) {
        MK_DEV *dev;
        int32  irqFromBB;

        /*--- wait for msg; continue if received msg was not a pulse ---*/
        if( MsgReceive_r( G_irqThread.cid, &p, sizeof(p), NULL ) != 0 ) {
		  IDBGWRT_3((DBH, ">>> message\n" ));
            continue;
		}

        dev = (MK_DEV*)p.value.sival_ptr;

        /*--- handle interrupt ---*/
        IDBGWRT_3((DBH, ">>> irq from %s\n", dev->devName ));

        /*-----------------------------+
        |  board handler service init  |
        +-----------------------------*/
        if( dev->irqSrvInitFunc )
            irqFromBB = dev->brdJumpTbl.irqSrvInit( dev->brd, dev->devSlot );
        else
            irqFromBB = BBIS_IRQ_UNK;

        if( irqFromBB & BBIS_IRQ_EXP ){
            if( dev->initialized )
                slogf( __SLOG_CODE_MENDBG, _SLOG_WARNING,
                       "*** MDIS: BBIS exception on %s / %s\n",
                       dev->brdName, dev->devName );
            dev->exceptionOccurred++;
        }

        if( irqFromBB & (BBIS_IRQ_UNK | BBIS_IRQ_YES)){

            /*-----------------------------+
            |  ext irq call                |
            +-----------------------------*/
            if( dev->extIrqUse ) {
                MsgSend( dev->irqThread.coid, NULL, 0, NULL, 0 );
            }

            /*-----------------------------+
            |  ll irq call                 |
            +-----------------------------*/
            IDBGWRT_2((DBH," call LL driver irq\n" ));

            /* board detected irq on device (or board doesn't know it) */
            if(dev->initialized )		/* device initialisation finished? */
                dev->llJumpTbl.irq( dev->ll );

            dev->irqCnt++;
        }

        /*-----------------------------+
        |  board handler service exit  |
        +-----------------------------*/
        if( dev->irqSrvExitFunc )
            dev->brdJumpTbl.irqSrvExit( dev->brd, dev->devSlot );


        /*--- unmask interrupt ---*/
        if( dev->brdBusType == OSS_BUSTYPE_VME ) {
#ifdef USE_VME_SERVER
            MenC_VmeIrqLevelUnmask( dev->vmeHdl, dev->irqLevel, dev->irqId );
#endif
        }
        else
            InterruptUnmask( dev->irqLevel, dev->irqId );
    }

    return( NULL );
}


/**********************************************************************/
/** Lock the device
 *
 *	If lock mode is CHAN, locks the channel's sempahore
 *	If lock mode is CALL, locks the semaphore pointed to by callSem
 *	Finally, locks the device semaphore
 *
 *  \param  mkPath   \IN   MDIS kernel path
 *  \param  call     \IN   code for MDIS call semaphore to lock
 *                         (used only if lock mode is CALL)
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

int32
MDIS_DevLock( MK_PATH *mkPath, MDIS_CALL call )
{
	MK_DEV *dev = mkPath->dev;
	int32 error;

	switch( dev->lockMode ){
	case LL_LOCK_CHAN:
		if( (error = OSS_SemWait( dev->osh, dev->semChanP[mkPath->chan],
								  OSS_SEM_WAITFOREVER ))){
			DBGWRT_ERR((DBH,"*** MK:DevLock Error 0x%04x locking chansem\n",
						error));
			return( error );
		}

        mkPath->locked.chan[mkPath->chan] = 1;
		break;

	case LL_LOCK_CALL:
		if( (error = OSS_SemWait( dev->osh, dev->semCall[call],
                                  OSS_SEM_WAITFOREVER ))){
			DBGWRT_ERR((DBH,"*** MK:DevLock Error 0x%04x locking callsem\n",
						error));
			return( error );
		}

        mkPath->locked.call[call] = 1;
		break;
	}

	if( (error = OSS_SemWait( dev->osh, dev->semDev, OSS_SEM_WAITFOREVER ))){
		DBGWRT_ERR((DBH,"*** MK:DevLock Error 0x%04x locking devsem\n",
					error));

        /* unlock semaphore if locking not successful */
		switch( dev->lockMode ){
		case LL_LOCK_CHAN:
			OSS_SemSignal( dev->osh, dev->semChanP[mkPath->chan] );
            mkPath->locked.chan[mkPath->chan] = 0;
			break;
		case LL_LOCK_CALL:
			OSS_SemSignal( dev->osh, dev->semCall[call] );
            mkPath->locked.call[call] = 0;
			break;
		}

	}

    mkPath->locked.dev = 1;

	return( error );
}


/**********************************************************************/
/** Unlock the device
 *
 *  \param  mkPath   \IN   MDIS kernel path
 *  \param  call     \IN   code for MDIS call semaphore to lock
 *                         (used only if lock mode is CALL)
 */

void
MDIS_DevUnLock( MK_PATH *mkPath, MDIS_CALL call )
{
	MK_DEV *dev = mkPath->dev;

	OSS_SemSignal( dev->osh, dev->semDev );
    mkPath->locked.dev = 0;

	switch( dev->lockMode ){
	case LL_LOCK_CHAN:
		OSS_SemSignal( dev->osh, dev->semChanP[mkPath->chan] );
        mkPath->locked.chan[mkPath->chan] = 0;
		break;

	case LL_LOCK_CALL:
		OSS_SemSignal( dev->osh, dev->semCall[call] );
        mkPath->locked.call[call] = 0;
		break;
	}
}


/**********************************************************************/
/** Watchdog thread
 * 
 *  Checks all applications which have opened a MDIS device for existence
 *  once per second.
 *  If the application has been finished without calling M_close() then
 *  this thread performs the cleanup tasks (freeing resources, halting 
 *  associated kernel thread) instead.
 *
 *  \param  arg   \IN   not used
 *  \return never returns
 */

static void*
MDIS_Watchdog( void *arg )
{
    MK_DEV        *dev;
    MK_PATH       *path;

    for(;;) {
        delay( 1000 );

        /*--- check all connected applications for existence ---*/
        for( dev=(MK_DEV *)G_devList.head;
             dev->node.next;
             dev = (MK_DEV *)dev->node.next ) {

            path = (MK_PATH *)dev->pathList.head;

            /*
             * check also and first if path is NULL because the value of
             * next node can change during MDIS_CloseDevice()
             */
        	while ( (NULL != path) && (NULL != path->node.next) ) {

                /*
                 * close path if thread which opened it no longer exists
                 */
                if( SignalKill_r( path->thread->appNid,
                                  path->thread->appPid,
                                  path->thread->appTid,
                                  0, 0, 0 ) == ESRCH ) {

                    MDIS_CLOSE_DEVICE_DATA mcl;

                    mcl.path = path;
                    mcl.pid  = path->thread->appPid;
                    mcl.tid  = path->thread->appTid;
                    mcl.nid  = path->thread->appNid;
                    MDIS_CloseDevice( &mcl );
                }
                path = (MK_PATH *)path->node.next;
            }
        }
    }
}

/**********************************************************************/
/** Search in list of all devices for a kernel thread with given
 *  thread id
 *
 *  \param  tid     \IN   thread id to look for
 *
 *  \return ptr to thread struct or NULL if not found
 *
 */
static MK_THREAD *
findThread( pthread_t tid )
{
    MK_DEV  *dev;
	MK_PATH *path;

    for( dev=(MK_DEV *)G_devList.head;
         dev->node.next;
         dev = (MK_DEV *)dev->node.next ) {

        if( dev->pathList.head == NULL )
            goto NOTFOUND;

        for( path=(MK_PATH *)dev->pathList.head;
             path->node.next;
             path = (MK_PATH *)path->node.next ){

            if( tid == path->thread->mkTid )
                goto FOUND;
        }
    }

 NOTFOUND:
    return( NULL );

 FOUND:
	return( path->thread );
}


/**********************************************************************/
/** Return the process id of the currently running thread
 *
 *  Callback for OSS library.
 *
 *  \return current PID
 */
pid_t
MDIS_CurrentPid( void )
{
    MK_THREAD *t;

    if( ! (t = findThread( pthread_self() ) ) )
        return( 0 );

    return( t->appPid );
}

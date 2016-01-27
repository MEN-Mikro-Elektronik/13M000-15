/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  mk_intern.h
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2014/10/29 11:52:19 $
 *    $Revision: 1.6 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  Definitions for MDIS kernel ressource manager
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mk_intern.h,v $
 * Revision 1.6  2014/10/29 11:52:19  channoyer
 * R: mdis_kernel not executed in background
 * M: Add header for procmgr_daemon()
 *
 * Revision 1.5  2014/09/22 17:23:39  channoyer
 * R: Not possible to use MDIS over Qnet
 * M: Add application node ID to struct MK_THREAD
 *
 * Revision 1.4  2009/06/05 15:56:53  channoyer
 * R: No possibility to use MDIS kernel server for native driver
 * M: Add messages for native driver
 *
 * Revision 1.3  2008/11/13 15:42:48  channoyer
 * R: Cosmetics
 * M: Improuve comments with PU remaks
 *
 * Revision 1.2  2008/09/12 16:27:03  gvarlet
 * R: DBG mode tried to open libraries without "_dbg" suffix
 * M: Add "_dbg" suffix to library paths in DBG mode
 *
 * Revision 1.1  2004/03/24 11:35:13  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*--------------------------------------+
|  INCLUDES                             |
+---------------------------------------*/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <devctl.h>
#include <dlfcn.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/trace.h>                  /* QNX event trace */
#include <sys/kercalls.h>
#include <sys/procmgr.h>

#include <MEN/men_typs.h>
#include <MEN/dbg.h>
#include <MEN/oss.h>
#include <MEN/desc.h>
#include <MEN/maccess.h>
#include <MEN/desctyps.h>
#include <MEN/ll_defs.h>
#include <MEN/ll_entry.h>
#include <MEN/bb_defs.h>
#include <MEN/bb_entry.h>
#include <MEN/bbis_bk.h>
#include <MEN/mdis_mk.h>
#include <MEN/mdis_err.h>
#include <MEN/mdis_com.h>
#include <MEN/mdis_api.h>
#include <MEN/men_libc.h>


/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
#define DBG_MYLEVEL mk_dbglevel
#define DBH			G_dbh
#define OSH			G_osh

#define MK_MAX_DRVNAME	39		/* maximum length of LL driver name */
#define MK_MAX_DEVNAME	39		/* maximum length of device name */

/* macros to lock global MDIS sempahore */
#define MK_LOCK(err)		\
 err=OSS_SemWait(OSH,G_mkLockSem,OSS_SEM_WAITFOREVER)

#define MK_UNLOCK() OSS_SemSignal(OSH,G_mkLockSem)

#define MK_DRV_PREFIX	"libmen_ll_"

#ifdef DBG
	#define MK_DRV_SUFFIX	"_dbg.so"
#else
	#define MK_DRV_SUFFIX	".so"
#endif

/* SPACE.flags */
#define MK_MAPPED		0x1
#define MK_REQUESTED	0x2

/* macros for QNX trace facility */
#ifdef TRACE
#define TRACE_EVENT(args) \
if((int)(TraceEvent args )==(-1)) \
{ \
	(void) fprintf \
	( \
	 stderr, \
	 "%s() #%d: function call TraceEvent() failed, errno(%d): %s\n", \
	 __FUNCTION__, \
     __LINE__, \
	 errno, \
	 strerror(errno) \
	); \
}

#else   /* Trace off */
#define TRACE_EVENT(args) 
#endif

#define TRACE_INIT \
        TRACE_EVENT((_NTO_TRACE_DELALLCLASSES))

#define TRACE_START \
        TRACE_EVENT((_NTO_TRACE_START))

#define TRACE_STOP  \
        TRACE_EVENT((_NTO_TRACE_STOP))

#define TRACE_USR( str ) \
        TRACE_EVENT((_NTO_TRACE_INSERTUSRSTREVENT, 123, str ))


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*! enumeration of MDIS calls */
typedef enum {
    mdis_read,
    mdis_write,
    mdis_getblock,
    mdis_setblock,
    mdis_getstat,
    mdis_setstat,
    mdis_call_num                  /* must be last one */
} MDIS_CALL;

/*! driver node structure */
typedef struct {
	OSS_DL_NODE node;			   /*!< node in registered drivers list */
	char drvName[MK_MAX_DRVNAME+1];/*!< driver name */
    void  *drvHandle;              /*!< handle for QNX shared library */
	void (*getEntry)(LL_ENTRY *);  /*!< GetEntry function ptr */
} MK_DRV;


/*! application node structure */
typedef struct {
    int         appPid;            /*!< application process id */
    int         appTid;            /*!< application thread id */
    int         appNid;            /*!< application node id */
    int         mkTid;             /*!< MDIS kernel thread id */
    int         mkCid;             /*!< MDIS kernel comm channel id */
    int         useCount;
} MK_THREAD;


/*! address space info */
typedef struct {
	u_int32	addrMode;              /*!< address mode */
	u_int32	dataMode;              /*!< data mode */
	u_int32	reqSize;               /*!< required size (device) */
	u_int32	availSize;             /*!< available size (board) */
	void    *physAddr;             /*!< physical address */
	void    *virtAddr;             /*!< virtual address */
	u_int32 flags;                 /*!< mapped/requested flags */
	u_int32 type;                  /*!< OSS_ADDRSPACE_MEM or IO */
} SPACE;

/*! MDIS device structure */
typedef struct {
	OSS_DL_NODE 	node;			/*!< node in registered devices list */
	char		    devName[MK_MAX_DEVNAME+1]; /*!< device name */
	char		    brdName[BK_MAX_DEVNAME+1]; /*!< board device name */
	u_int32		   	useCount;		/*!< number of opens */
	int				externalDev;	/*!< flags external, non-MDIS driver */

	OSS_HANDLE		*osh;			/*!< device's OSS handle */
    int             vmeHdl;         /*!< VME server handle */

	u_int32			lockMode;		/*!< device locking mode  */
	OSS_SEM_HANDLE *semDev;			/*!< device semaphore */
	OSS_SEM_HANDLE *semCall[mdis_call_num]; /*!< semaphores for call locking */
	OSS_SEM_HANDLE **semChanP;		/*!< chan semaphore array (chan locking) */
	u_int32			semChanAlloc;	/*!< size allocated for semChanP */

	/* device params */
	u_int32			devSlot;		/*!< device slot number on board */
	u_int32			subDevOffset; 	/*!< subdevice address offset 0 */
	u_int32			devNrChan;		/*!< number of channels */
	u_int32			devAddrMode;	/*!< device address mode */
	u_int32			devDataMode;	/*!< device data mode */
	/* board params */
	u_int32			brdBusType;		/*!< board bus type */
	u_int32			devBusType;		/*!< device bus type */
	u_int32			irqInfo;		/*!< irq info */
	u_int32			addrSpaceType;  /*!< OSS_ADDRSPACE_MEM/IO */
	/* addr spaces */
	u_int32			spaceCnt;		/*!< nr of address spaces */
	SPACE			space[MDIS_MAX_MSPACE];	/*!< addr space params */
	MACCESS			ma[MDIS_MAX_MSPACE];	/*!< addr space access handles */
	/* device interrupt */
	u_int32			irqUse;			/*!< irq required from device */
	u_int32			irqInstalled;	/*!< irq installed */
	u_int32			irqEnable;		/*!< irq enable */
	u_int32			irqVector;		/*!< irq vector */
	u_int32			irqLevel;		/*!< irq level */
	u_int32			irqMode;		/*!< irq mode flags */
	u_int32			irqCnt;			/*!< irq global counter */
    int             irqId;          /*!< irq id from InterruptAttachEvent() */
	/* pci params */
	u_int32			pciBusNbr;		/*!< PCI bus number of device */
	u_int32			pciDevNbr;		/*!< PCI device number of device */
	u_int32			pciFuncNbr;		/*!< PCI function number of device */
	u_int32			pciVendorId; 	/*!< PCI parameters from desc */
	u_int32			pciDeviceId; 	
	u_int32			pciSubSysVendorId; 	
	u_int32			pciSubSysId; 	
	/* board handler */
	BBIS_ENTRY		brdJumpTbl;		/*!< board handler's jump table */
	BBIS_HANDLE		*brd;			/*!< board handler's handle */
	u_int32			irqSrvInitFunc;	/*!< board has irq init function */
	u_int32			irqSrvExitFunc;	/*!< board has irq init function */
	/* ll driver */
	MK_DRV			*drv;			/*!< driver structure */
	LL_ENTRY		llJumpTbl;		/*!< ll driver's jump table */
	LL_HANDLE		*ll;			/*!< ll driver's handle */
	int				initialized; 	/*!< flags device sucessfuly initialized */
	int				exceptionOccurred; /*!< number of exception interrupts */
    int             appCount;       /*!< nbr applications which opened dev */
    OSS_DL_LIST     pathList;       /*!< list of paths using this device */
	u_int32			extIrqUse;
	struct sigevent extEvent;
	int             extRcvid;
    IRQ_THREAD      irqThread;
} MK_DEV;

/*! information about semaphores locked via path */
typedef struct {
    int8        dev;                /*!< device semaphore locked by this path*/
    int8        call[mdis_call_num];/*!< call semaphore */
    int8        *chan;              /*!< channel semaphore */
} LOCK_INFO;

/*! MK path structure containing path info for the calling application */
typedef struct {
	OSS_DL_NODE node;			    /*!< node in list of paths */
	MK_DEV      *dev;               /*!< device entry */
    MK_THREAD   *thread;            /*!< kernel thread */
  	int32       chan;               /*!< current MDIS channel number */
	int32       ioMode;             /*!< current MDIS I/O mode */
    LOCK_INFO   locked;             /*!< info about locked semaphores */
} MK_PATH;


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
extern DBG_HANDLE 		*G_dbh;
extern OSS_HANDLE		*G_osh;
extern u_int32 			mk_dbglevel;
extern OSS_SEM_HANDLE 	*G_mkLockSem;
extern OSS_DL_LIST		G_drvList;
extern OSS_DL_LIST		G_devList;
extern OSS_DL_LIST		G_threadList;
extern IRQ_THREAD       G_irqThread;


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

/* mdis_kernel.c */
void* MDIS_Handler( void *);
void* MDIS_IrqHandler( void *);
int32 MDIS_DevLock( MK_PATH *mkPath, MDIS_CALL call );
void  MDIS_DevUnLock( MK_PATH *mkPath, MDIS_CALL call );
pid_t MDIS_CurrentPid( void );


/* ident.c */
char* MDIS_Ident( void );

/* open.c */
int32 MDIS_OpenDevice( 
    MDIS_OPEN_DEVICE_DATA *mop, 
    MDIS_OPEN_DEVICE_RETURN *r);

int32 MDIS_InitialOpen( 
	char *devName, 
	DESC_HANDLE *devDescHdl,
	DESC_SPEC *devDesc,
	char *brdName, 
	DESC_SPEC *brdDesc,
	MK_DEV **devP);

int32 MDIS_EnableIrq(MK_DEV *dev, int enable);
MK_DRV *MDIS_FindDrvByName( char *name );

/* close.c */
int32 MDIS_CloseDevice( MDIS_CLOSE_DEVICE_DATA *mcl );
int32 MDIS_FinalClose( MK_DEV *dev );

/* calls.c */
int32 MDIS_Read(  MK_PATH *path, int32 *valP );
int32 MDIS_Write( MK_PATH *path, int32 val );

int32 MDIS_Getblock( 
    MK_PATH *path,
    u_int8 *buffer, 
    int32 length );

int32 MDIS_Setblock( 
    MK_PATH *path,
    u_int8 *buffer, 
    int32 length );

int32 MDIS_GetstatStd( 
    MK_PATH *path,
    int32 code, 
    int32 *valueP );

int32 MDIS_GetstatBlk( 
    MK_PATH *path,
    int32 code, 
    u_int8 *buffer, 
    int32 length );

int32 MDIS_SetstatStd( 
    MK_PATH *path,
    int32 code, 
    int32 value );

int32 MDIS_SetstatBlk( 
    MK_PATH *path,
    int32 code, 
    u_int8 *buffer, 
    int32 length );

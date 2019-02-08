/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2008/11/12 16:59:01 $
 *    $Revision: 1.4 $
 * 
 *	   \project  MDIS4 QNX
 *  	 \brief  Core functions of the OSS module
 *
 *    \switches  
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss.c,v $
 * Revision 1.4  2008/11/12 16:59:01  channoyer
 * Cosmetics
 *
 * Revision 1.3  2004/03/25 08:31:32  ub
 * Added switch USE_VME_SERVER
 *
 * Revision 1.2  2004/03/19 14:59:20  ub
 * adapted for MDIS
 *
 * Revision 1.1  2003/08/04 14:01:31  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

/*! \mainpage 
 
 This is the documentation of the MEN OSS module (Operating System Services)
 for QNX.

 Refer to the \ref osscommonspec "OSS Common Specification" for the
 plain common specification.

 The QNX OSS implementation is based mainly on standard POSIX functionality
 supported by QNX. It is implemented as a shared library which must be linked
 dynamically to the calling module. 

 As with most MEN libraries two variants exist: 

 - libmen_oss.so : standard module, should be used normally
 - libmen_oss_dbg.so : contains debug information

 OSS_Init() requests I/O privileges which is needed for the OSS_Irq*() functions.
 Therefore programs using OSS \b must have root privileges.

 \section natcalls Special OSS calls under QNX

 - \b Initialisation and termination: OSS_Init(), OSS_Exit() 
*/ 
/*! \page dummy
 \menimages
*/

/*-----------------------------------------+
|  INCLUDES                                |
+------------------------------------------*/
#include "oss_intern.h"


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*
 * This array defines the mapping between geographical PCI slot numbers
 * and the corresponding PCI device number. It is used by 
 * OSS_PciSlotToPciDevice(). Currently the D201 board driver will call 
 * this function when PCI_BUS_SLOT is specified in the descriptor.
 *
 * It defaults to the standard mapping for CompactPci backplanes. 
 * It can be changed by the user by specifying oss_pci_slot_devnbrs=x,y,...
 * to the command line of insmod when the OSS module is loaded.
 * A value of -1 indicates that the slot is not available.
 * The index in the array is the geographical slot -1. I.e. 
 * oss_pci_slot_devnbrs[1] specifies the device number of slot #2
 */
int oss_pci_slot_devnbrs[16] = {
	-1, 15, 14, 13, 12, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1, -1 
};


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

/**********************************************************************/
/** Initializes the OSS module
 *
 * Creates an instance of OSS and returns a handle for that instance.
 *
 * \param instName		\IN	name of calling module
 * \param ossP			\OUT contains the created handle for that instance
 * \param currentPid    \IN function to find out the pid of the current process
 *
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 * \qnx Attaches to PCI server. Signals sended via \a OSS_SigSend are 
 *      sent to the process with process id \a pid.
 * \sa OSS_Exit
 */ 
int32  OSS_Init( 
    char *instName, 
    OSS_HANDLE **ossP, 
    pid_t (*currentPid)( void ) )
{
	OSS_HANDLE *oss;
	
	*ossP = NULL;
	if( (oss = malloc( sizeof(OSS_HANDLE))) == NULL )
		return ERR_OSS_MEM_ALLOC;

	if( strlen(instName)+1 > sizeof(oss->instName))
		return ERR_OSS_ILL_PARAM;

	strcpy( oss->instName, instName );

	DBGINIT((NULL,&DBH));		/* init debugging */
	oss->dbgLevel = OSS_DBG_DEFAULT;
    oss->currentPid = currentPid;

	DBGWRT_ERR((DBH,"OSS_Init %s 0x%p", instName, oss));

	/*--- request I/O privileges; otherwise IRQ functions will not work ---*/
	ThreadCtl( _NTO_TCTL_IO, 0 );

	/* initialize the interrupt spin lock */
	memset( &(oss->intrSpinLock), 0, sizeof( intrspin_t ) );
	oss->irqLockCount = 0;

	/*--- attach to PCI server ---*/
	if( (oss->pciHandle = pci_attach( 0 )) < 0 )
		DBGWRT_ERR((DBH,"OSS_Init %s: can't attach to PCI server\n",instName));

#ifdef USE_VME_SERVER
    /*--- attach to VME server ---*/
    if( (oss->vmeHandle = MenC_VmeAttach() ) < 0 )
		DBGWRT_ERR((DBH,"OSS_Init %s: can't attach to VME server\n",instName));
#endif

 	*ossP = oss;

	return 0;
}/*OSS_Init*/

/**********************************************************************/
/** Deinitializes the OSS module
 *
 * Destructs an instance of OSS.
 *
 * \param ossP			\IN  contains the handle to destruct\n
 *						\OUT *ossP set to NULL
 * \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 * \sa OSS_Init
 */ 
int32  OSS_Exit( OSS_HANDLE **ossP )
{
	OSS_HANDLE *oss = *ossP;

	if( oss == NULL )
		return ERR_OSS_ILL_PARAM;

#ifdef USE_VME_SERVER
	if( oss->vmeHandle >= 0 )
		MenC_VmeDetach( oss->vmeHandle );
#endif

	if( oss->pciHandle >= 0 )
		pci_detach( oss->pciHandle );

	DBGWRT_1((DBH,"OSS_Exit\n"));
	DBGEXIT((&DBH));

	free( oss );
	*ossP = NULL;

	return 0;
}/*OSS_Exit*/


/**********************************************************************/
/** Set debug level of OSS instance.
 *
 * \copydoc oss_specification.c::OSS_DbgLevelSet
 * \sa OSS_DbgLevelGet
 */
void  OSS_DbgLevelSet( OSS_HANDLE *oss, u_int32 newLevel )
{
    DBG_MYLEVEL = newLevel;
}/*OSS_DbgLevelSet*/

/**********************************************************************/
/** Get debug level of OSS instance.
 *
 * \copydoc oss_specification.c::OSS_DbgLevelGet
 * \sa OSS_DbgLevelSet
 */
u_int32  OSS_DbgLevelGet( OSS_HANDLE *oss )
{
    return( DBG_MYLEVEL );
}/*OSS_DbgLevelGet*/


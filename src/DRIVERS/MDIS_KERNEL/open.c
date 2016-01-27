/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  open.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2014/09/22 17:28:58 $
 *    $Revision: 1.5 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  MDIS kernel resource manager
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: open.c,v $
 * Revision 1.5  2014/09/22 17:28:58  channoyer
 * R: Not possible to use MDIS over Qnet
 * M: Add node ID to identify thread using MDIS device
 *
 * Revision 1.4  2011/02/18 11:43:36  gvarlet
 * R: BBIS handlers require setIrqHandle() function
 * M: added call to BBIS SetIrqHandle()
 *
 * Revision 1.3  2009/07/09 11:03:39  channoyer
 * R: QNX 6.4 warning: pointer targets differ in signedness
 * M: change variable type
 *
 * Revision 1.2  2004/03/25 08:50:28  ub
 * added switch USE_VME_SERVER
 *
 * Revision 1.1  2004/03/24 11:35:08  ub
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

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

int32 G_irqHdl = 0; /* OSS_IRQ_HANDLE */


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static char *tolower(char *s);
static MK_DEV *findDevByName( char *name );
static MK_THREAD *threadOfApp( pid_t pid, int tid, int nid );
static int   readDesc( char *name, int32 *lenP, char **dataP );
static int32 getDeviceParams(MK_DEV *dev);
static int32 getPciBoardParams(MK_DEV *dev, DESC_HANDLE *devDescHdl);
static int32 getBoardParams(MK_DEV *dev);
static int32 checkPciDev( MK_DEV *dev );
static int32 PciGetAddrReg(
	MK_DEV *dev,
	u_int32 pciBaseReg,
	void *addrP,
	u_int32 *sizeP,
	u_int32 *spaceP);
static int32 MapAddressSpaces(MK_DEV *dev, DESC_HANDLE *devDescHdl);
static int32 getIrqParams(MK_DEV *dev);
static int32 addDevToIrqHandler(MK_DEV *dev);
static int32 setMiface( MK_DEV *dev );
static int32 brdIrqEnable( MK_DEV *dev, int enable );


/*--------------------------------------------------------------------------*/
static char *
tolower(char *s)
{
	char *t = s;

	while(*s){
		if( *s>='A' && *s<='Z') *s+='a'-'A';
		s++;
	}
	return t;
}

/**********************************************************************/
/** Search for a device in MDIS device list
 *
 *  \param  name    \IN   name to look for
 *
 *  \return ptr to device struct or NULL if not found
 *
 *  \sa MDIS_FindDrvByName
 */
static MK_DEV *
findDevByName( char *name )
{
	MK_DEV *node;

	for( node=(MK_DEV *)G_devList.head;
		 node->node.next;
		 node = (MK_DEV *)node->node.next ){

		if( strcmp(node->devName, name ) == 0 )
			break;
	}

	if( node->node.next == NULL )
		node=NULL;

	return node;
}


/**********************************************************************/
/** Search for a driver in MDIS driver list
 *
 *  \param  name    \IN   name to look for
 *
 *  \return ptr to driver struct or NULL if not found
 *
 *  \sa findDevByName
 */
MK_DRV *
MDIS_FindDrvByName( char *name )
{
	MK_DRV *node;

	for( node=(MK_DRV *)G_drvList.head;
		 node->node.next;
		 node = (MK_DRV *)node->node.next ){

		if( strcmp(node->drvName, name ) == 0 )
			break;
	}

	if( node->node.next == NULL )
		node=NULL;

    DBGWRT_3((DBH, "MDIS_FindDrvByName: found 0x%p\n", node ));

	return node;
}

/**********************************************************************/
/** Search for a kernel thread belonging to application thread (pid/tid)
 *  in list of all devices
 *
 * A match is detected, if both process id and thread id equal the
 * given values.
 *
 *  \param  pid     \IN   process id to look for
 *  \param  tid     \IN   thread id to look for
 *
 *  \return ptr to thread struct or NULL if not found
 *
 */
static MK_THREAD *
threadOfApp( pid_t pid, int tid, int nid )
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

            if( (tid == path->thread->appTid) && 
                (pid == path->thread->appPid) &&
                (nid == path->thread->appNid) )
                goto FOUND;
        }
    }

 NOTFOUND:
    DBGWRT_3((DBH, "threadOfApp: none found\n" ));
    return( NULL );

 FOUND:
	return( path->thread );
}

/**********************************************************************/
/** Read descriptor from file
 *
 *  Descriptors are always searched in directory MDIS_DESCRIPTOR_DIR.
 *  In the name only one leading slash is allowed.
 *
 *  \param  name   \IN   name of descriptor (without directory and .bin suffix)
 *  \param  lenP   \OUT  length of descriptor in memory
 *  \param  *dataP \OUT  address of descriptor
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */
static int
readDesc( char *name, int32 *lenP, char **descDataP )
{
    char        descFileName[40];
    char        *descData = NULL;
    FILE        *descFp = NULL;
    int32       len;
    int         error=0;


    DBGWRT_1((DBH, "readDesc: name=%s\n", name ));

    /*--- determine name of descriptor file from device name ---*/
    /* sanity check */
	if( strlen(name) >
		(sizeof(descFileName) - strlen(MDIS_DESCRIPTOR_DIR) - 6 )){

		error = ERR_MK_ILL_PARAM;
		goto ERREXIT;
	}

	/*
	 * check for slashes in device name. only a leading slash is allowed
	 */
	if( *name == '/' ) name++; /* skip leading slash */
	{
		char *p = name;
		while( *p ) {
			if( *p++ == '/' ){
				errno = ERR_MK_ILL_PARAM;
				goto ERREXIT;
			}
        }
	}

	strcpy( descFileName, MDIS_DESCRIPTOR_DIR );
	strcat( descFileName, "/" );
	strcat( descFileName, tolower(name) );
	strcat( descFileName, ".bin" );

    DBGWRT_2((DBH, "reading descriptor file %s\n", descFileName ));


	/*--- read the descriptor from file ---*/
	if( (descFp = fopen( descFileName, "rb" )) == NULL ) {
        error = ERR_MK_NO_LLDESC;
		goto ERREXIT;
    }

	/*--- determine length of file ---*/
	fseek( descFp, 0, SEEK_END );
	len = ftell( descFp );
	fseek( descFp, 0, SEEK_SET );

	/*--- alloc buffer for data ---*/
	if( (descData = malloc( len )) == NULL ){
		errno = ERR_OSS_MEM_ALLOC;
		goto ERREXIT;
	}

	if( fread( descData, 1, len, descFp ) != len ) {
        error = ERR_MK_NO_LLDESC;
        free( *descDataP );
		goto ERREXIT;
    }

    fclose( descFp );
    *lenP = len;
    *descDataP = descData;

    return( ERR_SUCCESS );


 ERREXIT:
    DBGWRT_ERR((DBH, "*** readDesc: error=0x%x\n", error ));

    if( descData ) free( descData );
    if( descFp )   fclose( descFp );

    return( error );
}


/**********************************************************************/
/** Open MDIS device
 *
 *  Reads the name of the device's carrier board from its descriptor;
 *  opens and initializes BBIS driver.
 *
 *  \param  mop      \IN   information for open
 *  \param  r        \OUT  filled with return info on success
 *
 *  \return 0 on success or \c ERR_xxx error code on error
 *
 *  \sa MDIS_CloseDevice
 */
int32
MDIS_OpenDevice( MDIS_OPEN_DEVICE_DATA *mop, MDIS_OPEN_DEVICE_RETURN *r )
{
    DESC_SPEC   *devDesc, *brdDesc;
    DESC_HANDLE *devDescHdl = NULL;
    MK_DEV      *dev;
    MK_PATH     *mkPath = NULL;
    MK_THREAD   *thread, *newThread = NULL;
    int32       len;
    u_int32     value;
    char        brdName[MK_MAX_DEVNAME];
    int         error=0;


    DBGWRT_1((DBH, "MDIS_OpenDevice: %s\n", mop->devName ));

    /*--- read device descriptor from file ---*/
    if( (error = readDesc( mop->devName, &len, (char**)&devDesc )) )
        goto ERREXIT;

    /*--- get board name from descriptor ---*/
    if( (error = DESC_Init( devDesc, OSH, &devDescHdl ))){
		DBGWRT_ERR((DBH,"*** BBIS:OpenDevice: can't init device descriptor "
					"err=0x%lx\n", error ));
		goto ERREXIT;
	}

	/* check if descriptor type is of type "device" */
	if( (error = DESC_GetUInt32( devDescHdl, 0, &value, "DESC_TYPE" )) ||
		(value != DESC_TYPE_DEVICE)){
		DBGWRT_ERR((DBH,"*** MDIS_OPEN_DEVICE: %s: DESC_TYPE in dev desc "
					"not found or bad\n", mop->devName));
		goto ERREXIT;
	}


    /* get board name */
    value = sizeof(brdName);
    if( (error = DESC_GetString( devDescHdl, "", brdName, &value,
                                 "BOARD_NAME" ))){
        DBGWRT_ERR((DBH,"*** MDIS_OPEN_DEVICE: BOARD_NAME not found\n"));
        goto ERREXIT;
    }

    DBGWRT_2((DBH," board name = %s\n", brdName ));


    /*--- read board descriptor ---*/
    if( (error = readDesc( brdName, &len, (char**)&brdDesc )) )
        goto ERREXIT;

    MK_LOCK(error);

    if( error ){
        goto ERREXIT;
    }


    /*--- device already in use ? ---*/
    if( (dev = findDevByName( mop->devName )) == NULL ) {
        DBGWRT_2(( DBH, "MDIS_OpenDevice: new device %s on board %s\n", 
                   mop->devName, brdName ));

        /*--- if not, do initial open ---*/
        if( (error = MDIS_InitialOpen( mop->devName,
                                       devDescHdl,
                                       (DESC_SPEC)devDesc,
                                       brdName,
                                       (DESC_SPEC)brdDesc,
                                       &dev))) {
            DBGWRT_ERR((DBH,"*** MDIS_OpenDevice: %s: initial open failed  "
                        "error=0x%x\n", mop->devName, error ));
            MK_UNLOCK();
            goto ERREXIT;
        }
    }

    dev->useCount++;

    /*--- application already known ? ---*/
    if( (thread = threadOfApp( mop->pid, mop->tid, mop->nid )) == NULL ) {

        /*--- no, then create kernel thread and connection ---*/
        int mkCid, mkTid;

        if( (mkCid = ChannelCreate_r( _NTO_CHF_UNBLOCK ) ) < 0 ) { 
            error = ERR_OSS_BUSY_RESOURCE;
            MK_UNLOCK();
            goto ERREXIT;
        }

        if( (newThread = malloc( sizeof( *newThread ))) == NULL )  {
            error = ERR_OSS_MEM_ALLOC;
            MK_UNLOCK();
            goto ERREXIT;
        }

        newThread->mkCid = mkCid;

        if( (mkTid = ThreadCreate_r( 0, MDIS_Handler, newThread, NULL )
                ) < 0 ) {
            error = ERR_OSS_BUSY_RESOURCE;
            MK_UNLOCK();
            goto ERREXIT;
        }

        DBGWRT_2((DBH, "MDIS_OpenDevice: thread created for %s. tid=%d "
                  "using channel cid=%d\n", dev->devName, mkTid, mkCid ));


        /*--- init thread structure ---*/
        newThread->appPid   = mop->pid;
        newThread->appTid   = mop->tid;
        newThread->appNid   = mop->nid;
        newThread->mkTid = mkTid;
        newThread->useCount = 0;

        ++(dev->appCount);
        thread = newThread;
    }

    ++thread->useCount;

    MK_UNLOCK();

    /*--- create and fill path structure ---*/
    if( (mkPath = malloc( sizeof(*mkPath) )) == NULL ) {
        DBGWRT_ERR((DBH, "*** Error: malloc(mkPath) failed.\n" ));
        error = ERR_OSS_MEM_ALLOC;
        goto ERREXIT;
    }

    mkPath->thread       = thread;
    mkPath->chan         = 0;
    mkPath->ioMode       = M_IO_EXEC;
    mkPath->dev          = dev;
    memset( &mkPath->locked, 0, sizeof( mkPath->locked ) );

    /* alloc space for array of channel lock flags */
    mkPath->locked.chan = malloc( sizeof(int8) * dev->devNrChan );

    if( mkPath->locked.chan == NULL ) {
        error = ERR_OSS_MEM_ALLOC;
        goto ERREXIT;
    }
				
    memset( mkPath->locked.chan, 0, sizeof(int8) * dev->devNrChan );
    OSS_DL_AddTail( &dev->pathList, &mkPath->node );


    /*--- prepare return values ---*/
    r->path  = mkPath;
    r->mkCid = thread->mkCid;
    r->mkPid = getpid();

	DESC_Exit( &devDescHdl );
    free( devDesc );

    return( 0 );

 ERREXIT:
    DBGWRT_ERR(( DBH, "*** MDIS_OpenDevice: error=0x%x\n", error ));

    if( mkPath )      free( mkPath );
    if( newThread )   free( newThread );
	if( devDescHdl )  DESC_Exit( &devDescHdl );

    return( error );
}


/**********************************************************************/
/** Get fixed device parameters via Info() function
 *
 *  \param  dev         \IN   device structure
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32
getDeviceParams(MK_DEV *dev)
{
    int32 error;     /* error holder */

	/* nr of addr spaces */
	if ((error = dev->llJumpTbl.info(LL_INFO_ADDRSPACE_COUNT,
									 &dev->spaceCnt))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: can't get "
					"LL_INFO_ADDRSPACE_COUNT\n"));
		return(error);
	}

	DBGWRT_2((DBH," device addr spaces: %d addr space(s)\n",dev->spaceCnt));

	if (dev->spaceCnt > MDIS_MAX_MSPACE) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: too many address spaces "
					"required\n"));
		return(ERR_MK);
	}

	/* device interrupt */
	if ((error = dev->llJumpTbl.info(LL_INFO_IRQ, &dev->irqUse))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: can't get LL_INFO_IRQ\n"));
		return(error);
	}

	DBGWRT_2((DBH," device interrupt: %s\n",dev->irqUse ?
			  "required":"not required"));

    /* device characteristic */
	if ((error = dev->llJumpTbl.info(LL_INFO_HW_CHARACTER,
									&dev->devAddrMode,
									&dev->devDataMode))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: can't get "
					"LL_INFO_HW_CHARACTER\n"));
		return(error);
	}

	DBGWRT_2((DBH," device characteristic: AddrMode=0x%04x, DataMode=0x%04x\n",
			  dev->devAddrMode, dev->devDataMode));

    /* process locking mode */
	if ((error = dev->llJumpTbl.info(LL_INFO_LOCKMODE, &dev->lockMode))) {
		DBGWRT_ERR((DBH," *** GetDeviceParams: warning: can't get "
					"LL_INFO_LOCKMODE\n"));
		dev->lockMode = LL_LOCK_CALL;
	}

	DBGWRT_2((DBH," process locking: lockMode=%d\n",dev->lockMode));

	return(ERR_SUCCESS);
}


/**********************************************************************/
/** Get board parameters from BBIS
 *
 *  \param  dev         \IN   device structure
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32
getBoardParams(MK_DEV *dev)
{
    int32 error;     /* error holder */

	/* bus system type */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_BUSTYPE,
										 &dev->brdBusType))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_BUSTYPE\n"));
		return(error);
	}

    DBGWRT_2((DBH, "%s: board bus type = %d\n", 
              dev->devName, dev->brdBusType ));

	/*
	 * If BBIS_BRDINFO_DEVBUSTYPE not supported by BBIS handler, use
	 * OSS_BUSTYPE_MMODULE
	 */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_DEVBUSTYPE,
										dev->devSlot, &dev->devBusType))) {
		DBGWRT_ERR((DBH," WARNING: GetBoardParams: can't get "
					"BBIS_BRDINFO_DEVBUSTYPE\n"));
		dev->devBusType = OSS_BUSTYPE_MMODULE;
	}

	/*
	 * If BBIS_BRDINFO_ADDRSPACE not supported by BBIS handler, use
	 * OSS_ADDRSPACE_MEM
	 */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_ADDRSPACE,
										dev->devSlot, &dev->addrSpaceType))) {
		DBGWRT_ERR((DBH," WARNING: GetBoardParams: can't get "
					"BBIS_BRDINFO_ADDRSPACE\n"));
		dev->addrSpaceType = OSS_ADDRSPACE_MEM;
	}


	DBGWRT_2((DBH," board bustype=%d, dev bustype=%d addrSpaceType=%d\n",
			  dev->brdBusType,dev->devBusType,dev->addrSpaceType));

	/* irq info */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_INTERRUPTS, dev->devSlot,
										&dev->irqInfo))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_INTERRUPTS\n"));
		return(error);
	}

	DBGWRT_2((DBH," board irq: info=%d\n",dev->irqInfo));

	/* function info */
	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_FUNCTION,
										BBIS_FUNC_IRQSRVINIT,
										&dev->irqSrvInitFunc))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_FUNCTION BBIS_FUNC_IRQSRVINIT\n"));
		return(error);
	}

	if ((error = dev->brdJumpTbl.brdInfo(BBIS_BRDINFO_FUNCTION,
										BBIS_FUNC_IRQSRVEXIT,
										&dev->irqSrvExitFunc))) {
		DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_BRDINFO_FUNCTION BBIS_FUNC_IRQSRVEXIT\n"));
		return(error);
	}

	/*-------------------------------------+
	|  Request parameters for PCI device   |
	+-------------------------------------*/
	if( dev->brdBusType == OSS_BUSTYPE_PCI ||
		dev->devBusType == OSS_BUSTYPE_PCI){

		/*--- get PCI bus number from board  ---*/
		if ((error = dev->brdJumpTbl.cfgInfo(dev->brd,
											BBIS_CFGINFO_BUSNBR,
											&dev->pciBusNbr,
											dev->devSlot))){
			DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_CFGINFO_BUSNBR\n"));
			return(error);
		}
	}
	return(ERR_SUCCESS);
}


/**********************************************************************/
/** Get PCI board parameters from descriptor and BBIS
 *
 *  \param  dev         \IN   device structure
 *  \param  devDescHdl  \IN   descriptor handle for device
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa getBoardParams
 */

static int32
getPciBoardParams(MK_DEV *dev, DESC_HANDLE *devDescHdl)
{
	int32 error;


	if( dev->devBusType == OSS_BUSTYPE_PCI ) {

		/*--- get PCI device number from board ---*/
		if ((error = dev->brdJumpTbl.cfgInfo(dev->brd,
											BBIS_CFGINFO_PCI_DEVNBR,
											dev->devSlot,
											&dev->pciDevNbr))){
			DBGWRT_ERR((DBH," *** GetBoardParams: can't get "
					"BBIS_CFGINFO_PCI_DEVNBR\n"));
			return(error);
		}

		/*--- get PCI function number from descriptor ---*/
		if ((error = DESC_GetUInt32(devDescHdl, 0, &dev->pciFuncNbr,
									"PCI_FUNCTION")) &&
			error != ERR_DESC_KEY_NOTFOUND){

			DBGWRT_ERR((DBH," *** GetBoardParams: error "
						"getting PCI_FUNCTION\n"));
			return(error);
		}

		DBGWRT_2((DBH," PCI bus=%d dev=%d\n",
				  dev->pciBusNbr, dev->pciDevNbr));

		/*--- get PCI Vendor/Device ID from descriptor (optional) ---*/
		if ( ((error = DESC_GetUInt32(devDescHdl, 0, &dev->pciVendorId,
									 "PCI_VENDOR_ID")) &&
                 error != ERR_DESC_KEY_NOTFOUND ) ||

			 ((error = DESC_GetUInt32(devDescHdl, 0, &dev->pciDeviceId,
									 "PCI_DEVICE_ID")) &&
                 error != ERR_DESC_KEY_NOTFOUND )) {

			DBGWRT_ERR((DBH," *** GetBoardParams: error "
						"getting PCI_VENDOR/DEVICE_ID\n"));
			return(error);
		}
		/*--- get PCI SubVendor/SubSys ID from descriptor (optional) ---*/
		if ( ((error = DESC_GetUInt32(devDescHdl, 0xffffffff,
									  &dev->pciSubSysVendorId,
									 "PCI_SUBSYS_VENDOR_ID")) &&
			  error != ERR_DESC_KEY_NOTFOUND ) ||

			 ((error = DESC_GetUInt32(devDescHdl, 0xffffffff,
									  &dev->pciSubSysId,
									 "PCI_SUBSYS_ID")) &&
			  error != ERR_DESC_KEY_NOTFOUND )){
			DBGWRT_ERR((DBH," *** GetBoardParams: error "
						"getting PCI_SUBSYS_VENDOR/SUBSYS_ID\n"));
			return(error);
		}

		DBGWRT_2((DBH," PCI desc params: func=%d vendID=0x%4x devID=0x%4x "
				  "subvendID=0x%4x subdevID=0x%4x\n",
				  dev->pciFuncNbr, dev->pciVendorId, dev->pciDeviceId,
				  dev->pciSubSysVendorId, dev->pciSubSysId ));
	}

	return(ERR_SUCCESS);
}


/**********************************************************************/
/** Check if PCI device present and vendor/devId matches
 *
 *  \param  xx    \IN   blabla
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32
checkPciDev( MK_DEV *dev )
{
	u_int32 vendorId, deviceId, subSysVendorId, subSysId;
	int32 error;

	DBGWRT_1((DBH, "MK - CheckPciDev\n"));

	if( (error = OSS_PciGetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_VENDOR_ID,
								   (int32 *)&vendorId )) ||
		(error = OSS_PciGetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_DEVICE_ID,
								   (int32 *)&deviceId )) ||
		(error = OSS_PciGetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_SUBSYS_VENDOR_ID,
								   (int32 *)&subSysVendorId )) ||
		(error = OSS_PciGetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, OSS_PCI_SUBSYS_ID,
								   (int32 *)&subSysId ))){
		DBGWRT_ERR((DBH," *** CheckPciDev: Error 0x%x reading cfg space\n",
					error ));
		return error;
	}

	DBGWRT_2((DBH, " PCI params from device: vendID=0x%04x devID=0x%04x "
			  "subSysVendID=0x%04x subSysID=0x%04x\n",
			  vendorId, deviceId, subSysVendorId, subSysId));

	/*--- check if device present ---*/
	if( vendorId == 0xffff && deviceId == 0xffff ){
		DBGWRT_ERR(( DBH, " *** CheckPciDev: PCI device not present!\n"));
		return ERR_OSS_PCI_NO_DEVINSLOT; /* due to lack of better error code */
	}

	/*--- check if device matches vendor/device ID ---*/
	if( (vendorId != dev->pciVendorId) || (deviceId != dev->pciDeviceId)){
		DBGWRT_ERR(( DBH, " *** CheckPciDev: wrong vend/devId 0x%x/0x%x!\n",
					 vendorId, deviceId));
		return ERR_OSS_PCI_ILL_DEV; /* due to lack of better error code */
	}

	/*--- check if device matches subsystem vendor/ID ---*/
	if( (dev->pciSubSysVendorId != 0xffffffff &&
		 (dev->pciSubSysVendorId != subSysVendorId)) ||
		(dev->pciSubSysId != 0xffffffff &&
		 (dev->pciSubSysId != subSysId))){

		DBGWRT_ERR(( DBH, " *** CheckPciDev: wrong subsys vend/Id "
					 "0x%x/0x%x!\n",
					 subSysVendorId, subSysId));
		return ERR_OSS_PCI_ILL_DEV; /* due to lack of better error code */
	}

	return ERR_SUCCESS;
}


/**********************************************************************/
/** Determine if PCI address register present and determine size
 *
 *  \param  dev         \IN   device structure
 *	\param  pciBaseReg  \IN   base address register index (0..5)
 *	\param  addrP       \OUT  physical address
 *	\param  sizeP       \OUT  size of region
 *  \param  spaceP      \OUT  space of region (OSS_ADDRSPACE_MEM or IO)
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */
static int32
PciGetAddrReg(
	MK_DEV *dev,
	u_int32 pciBaseReg,
	void *addrP,
	u_int32 *sizeP,
	u_int32 *spaceP)
{
	int32 error;
	u_int32 currentVal, which, mask, size;


	/*--- convert to which parameter for OSS_PciGetConfig ---*/
	switch( pciBaseReg ){
	case 0: which = OSS_PCI_ADDR_0; break;
	case 1: which = OSS_PCI_ADDR_1; break;
	case 2: which = OSS_PCI_ADDR_2; break;
	case 3: which = OSS_PCI_ADDR_3; break;
	case 4: which = OSS_PCI_ADDR_4; break;
	case 5: which = OSS_PCI_ADDR_5; break;
	default: return ERR_OSS_PCI_UNK_REG;
	}

    /*--- disable interrupts while checking register size ---*/
    InterruptDisable();

	if( (error = OSS_PciGetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, which,
								   (int32 *)&currentVal )))
		goto PCIERR;


	/*--- write ffs to PCI address reg ---*/
	if(	(error = OSS_PciSetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, which, 0xffffffff )))
		goto PCIERR;

	/*--- read mask to determine its size ---*/
	if( (error = OSS_PciGetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, which, (int32 *)&mask )))
		goto PCIERR;

	/*--- rewrite original value ---*/
	if(	(error = OSS_PciSetConfig( dev->osh, dev->pciBusNbr, dev->pciDevNbr,
								   dev->pciFuncNbr, which, (int32)currentVal)))
		goto PCIERR;


	InterruptEnable();

	size = mask;

	if( mask & 1 )				/* mask out non-address bits */
		size &= ~0x3;
	else
		size &= ~0xf;

	size = ~size + 1;

	DBGWRT_3((DBH, "   pci addr reg %d = 0x%08x, mask 0x%08x, size=0x%x "
			  "type=%s\n", pciBaseReg, currentVal, mask, size,
			  (mask & 1) ? "I/O" : "MEM" ));


	if( mask == 0 ){
		DBGWRT_ERR((DBH," *** PciGetAddrReg: Address reg %d non existent!\n",
					pciBaseReg ));
		return ERR_OSS_PCI_ILL_ADDRNBR;
	}

	/*--- now get the physical address (CPU view) ---*/
	if( (error = OSS_BusToPhysAddr( dev->osh, OSS_BUSTYPE_PCI, addrP,
									dev->pciBusNbr, dev->pciDevNbr,
									dev->pciFuncNbr, pciBaseReg ))){

		DBGWRT_ERR((DBH," *** PciGetAddrReg: OSS_BusToPhysAddr failed!\n"));
		return error;
	}

	*sizeP = size;
	*spaceP = (mask & 1) ? OSS_ADDRSPACE_IO : OSS_ADDRSPACE_MEM;

	return ERR_SUCCESS;

 PCIERR:
	DBGWRT_ERR((DBH," *** PciGetAddrReg: Error accessing cfg space\n"));
	return error;
}


/**********************************************************************/
/** Map all address spaces required from device
 *
 *  \param  dev         \IN   device structure
 *  \param  devDescHdl  \IN   descriptor handle for device
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32
MapAddressSpaces(MK_DEV *dev, DESC_HANDLE *devDescHdl)
{
    int32 error;     /* error holder */
	u_int32 n, subDevOff;

	/* for all addr spaces */
	for (n=0; n<dev->spaceCnt; n++) {
		DBGWRT_2((DBH," space[%d]:\n",n));

		/* by default, use addrSpaceType from BBIS */
		dev->space[n].type = dev->addrSpaceType;

		/*------------------------------------------+
        |  get addr space [n] required from device  |
        |  (device addr space)                      |
        +------------------------------------------*/
		if ((error = dev->llJumpTbl.info(LL_INFO_ADDRSPACE,n,
										 &dev->space[n].addrMode,
										 &dev->space[n].dataMode,
										 &dev->space[n].reqSize ))) {
			DBGWRT_ERR((DBH," *** MapAddressSpaces: can't get "
						"LL_INFO_ADDRSPACE\n"));
			return(error);
		}

		DBGWRT_2((DBH,"  required : addrMode=0x%04x dataMode=0x%04x size=%d\n",
				  dev->space[n].addrMode, dev->space[n].dataMode,
				  dev->space[n].reqSize));

		if( dev->space[n].addrMode == MDIS_MA_PCICFG ){
			/*------------------------------------------------+
			|  LL driver requests access to PCI config space  |
			+------------------------------------------------*/
			if( dev->devBusType != OSS_BUSTYPE_PCI ){
			    DBGWRT_ERR((DBH," *** MapAddressSpaces: MDIS_MA_PCICFG allowed"
							"on PCI devices only!\n"));

				return( ERR_BBIS_ILL_ADDRMODE );
			}

			dev->space[n].virtAddr = (void *)
				((dev->pciBusNbr  << 16) |
				 (dev->pciDevNbr  << 11) |
				 (dev->pciFuncNbr << 8));

			goto MAPPED;		/* we're done */
		}

		if( dev->devBusType != OSS_BUSTYPE_PCI ){
			/*------------------------------------------------+
			|  NON PCI device, ask BBIS for address and size  |
			+------------------------------------------------*/

			/*------------------------------------------+
			|  get addr space [n] available on board    |
			|  (bus addr space)                         |
			+------------------------------------------*/
			if ((error = dev->brdJumpTbl.getMAddr(dev->brd,
												  dev->devSlot,
												  dev->space[n].addrMode,
												  dev->space[n].dataMode,
												  &dev->space[n].physAddr,
												  &dev->space[n].availSize))) {
				DBGWRT_ERR((DBH," *** MapAddressSpaces: can't getMAddr "
							"space[%d]\n",n));
				return(error);
			}
		}
		else {
			/*----------------------------------------------+
			|  PCI device, query the PCI base address regs  |
			+----------------------------------------------*/
			u_int32 pciBaseReg;

			/*--- check if PCI_BASEREG_ASSIGN specified in desc ---*/
			if ((error = DESC_GetUInt32(devDescHdl, n, &pciBaseReg,
										"PCI_BASEREG_ASSIGN_%d", n)) &&
				error != ERR_DESC_KEY_NOTFOUND){

				DBGWRT_ERR((DBH," *** MapAddressSpaces: error "
							"getting PCI_BASEREG_ASSIGN_%d\n", n));
				return(error);
			}

			DBGWRT_2((DBH,"  Using PCI base reg %d for addrspace %d\n",
					  pciBaseReg, n ));


			/*--- get size and address of PCI base reg ---*/
			if( (error = PciGetAddrReg( dev, pciBaseReg,
										&dev->space[n].physAddr,
										&dev->space[n].availSize,
										&dev->space[n].type)))
				return error;

		}

		DBGWRT_2((DBH,"  available: addr=0x%08x size=%d space=%d\n",
				  dev->space[n].physAddr, dev->space[n].availSize,
				  dev->space[n].type));

		/*------------------------------------------+
		|  Check if descriptor SUBDEVICE_OFFSET_x   |
		|  is present, add offset if so             |
		+------------------------------------------*/
		subDevOff = 0;
		if ((error = DESC_GetUInt32(devDescHdl, 0, &subDevOff,
									"SUBDEVICE_OFFSET_%d", n)) &&
			error != ERR_DESC_KEY_NOTFOUND){

			DBGWRT_ERR((DBH," *** MapAddressSpaces: error "
						"getting SUBDEVICE_OFFSET\n"));
			return(error);
		}

		if( subDevOff ){
			DBGWRT_2((DBH,"  subdevoff=0x%x\n", subDevOff ));
			dev->space[n].physAddr =
				(void *)((char *)dev->space[n].physAddr + subDevOff);
		}
		if( n==0 )
			dev->subDevOffset = subDevOff;

		/*------------------------------------------+
        |  check size                               |
        +------------------------------------------*/
		if (dev->space[n].availSize < dev->space[n].reqSize + subDevOff) {
			DBGWRT_ERR((DBH," *** MapAddressSpaces: size conflict "
						"space[%d]:",n));
			DBGWRT_ERR((DBH," required=%d <==> available=%d\n",
						dev->space[n].reqSize, dev->space[n].availSize));
			return(ERR_MK_ILL_MSIZE);
		}

		/*------------------------------------------+
        |  map addr space                           |
        +------------------------------------------*/
		if ((error = OSS_MapPhysToVirtAddr(dev->osh,
										   dev->space[n].physAddr,
										   dev->space[n].reqSize,
										   dev->space[n].type,
										   dev->devBusType,
										   0,
										   &dev->space[n].virtAddr
										   ))) {
			DBGWRT_ERR((DBH," *** MapAddressSpaces: can't map addr "
						"space[%d]\n",n));
			return(error);
		}
		dev->space[n].flags |= MK_MAPPED;
MAPPED:
		dev->ma[n] = (MACCESS)dev->space[n].virtAddr;
		DBGWRT_2((DBH,"  mapped to: addr=0x%08x\n",dev->space[n].virtAddr));
	}

	return(ERR_SUCCESS);
}


/**********************************************************************/
/** Determine interrupt parameters of device
 *
 *  \param  dev         \IN   device structure
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32
getIrqParams(MK_DEV *dev)
{
    int32 error;     /* error holder */

	/* device irq available on board slot ? */
    if ((dev->irqInfo & BBIS_IRQ_DEVIRQ) == 0) {
		DBGWRT_ERR((DBH," *** GetIrqParams: board supports no device irq\n"));
		return(ERR_MK_NO_IRQ);
	}


	if( dev->devBusType == OSS_BUSTYPE_PCI ){
	    int32 intLine;

		/*-----------------------------------------+
		|  PCI devices, get IRQ from config space  |
		+-----------------------------------------*/
		if( (error = OSS_PciGetConfig( dev->osh, dev->pciBusNbr,
									   dev->pciDevNbr,
									   dev->pciFuncNbr,
									   OSS_PCI_INTERRUPT_LINE,
									   &intLine ))){

			DBGWRT_ERR((DBH," *** GetIrqParams: Error accessing "
						"cfg space\n"));
			return error;
		}

		if( intLine == 0xff ){
			DBGWRT_ERR((DBH," *** GetIrqParams: PCI dev supports no "
						"device irq\n"));
			return(ERR_MK_NO_IRQ);
		}

		dev->irqMode = BBIS_IRQ_SHARED;
		dev->irqLevel = intLine;

		/*--- convert to vector number ---*/
		if( (error = OSS_IrqLevelToVector( dev->osh, OSS_BUSTYPE_PCI,
										   intLine,
										   (int32*)&dev->irqVector ))){

			DBGWRT_ERR((DBH," *** GetIrqParams: Can't convert level to "
						"vector\n"));
			return error;
		}

		DBGWRT_2((DBH,"%s: slot %d interrupt: vector=%d, level=%d\n",
				  dev->devName, dev->devSlot, dev->irqVector, dev->irqLevel));
	}
	else {
		/*----------------------------+
		|  Non PCI devices, ask BBIS  |
		+----------------------------*/

		/* get irq params for board slot */
		if ((error = dev->brdJumpTbl.cfgInfo(dev->brd,
											BBIS_CFGINFO_IRQ,
											dev->devSlot,
											&dev->irqVector,
											&dev->irqLevel,
											&dev->irqMode ))) {
			DBGWRT_ERR((DBH," *** GetIrqParams: can't get "
						"BBIS_CFGINFO_IRQ\n"));
			return(error);
		}

		DBGWRT_2((DBH,"%s: slot %d interrupt: vector=%d, level=%d, "
				  "mode=0x%04x\n",
				  dev->devName, dev->devSlot, dev->irqVector, dev->irqLevel, 
                  dev->irqMode));
	}

	return( ERR_SUCCESS );
}


/**********************************************************************/
/** Add device to global interrupt handler
 *
 *  \param  dev         \IN   device structure
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32
addDevToIrqHandler(MK_DEV *dev)
{
    struct sigevent   event;

    DBGWRT_2((DBH,"%s: install system interrupt: vect=%d level %d\n",
			  dev->devName, dev->irqVector, dev->irqLevel));

    /*--- prepare sigevent structure ---*/
    SIGEV_PULSE_INIT( &event, G_irqThread.coid, SIGEV_PULSE_PRIO_INHERIT,
                      _PULSE_CODE_MINAVAIL, dev );


    /*--- attach event to irq ---*/
	if( dev->brdBusType == OSS_BUSTYPE_VME ) {
#ifdef USE_VME_SERVER
        if( (dev->irqId = MenC_VmeInterruptAttachEvent( 
                 dev->vmeHdl, dev->irqVector, dev->irqLevel, &event, 0)) < 0 ){

            DBGWRT_ERR((DBH,"*** Error: VmeInterruptAttachEvent returned %d\n",
                        dev->irqId ));
            return( ERR_OSS_BUSY_RESOURCE );
        }
#endif /* USE_VME_SERVER */
    }
    else {

        if( (dev->irqId = InterruptAttachEvent_r( dev->irqVector, &event, 
                                                  _NTO_INTR_FLAGS_TRK_MSK ))
            < 0 ){

            DBGWRT_ERR((DBH, "*** Error: InterruptAttachEvent returned %d\n", 
                        dev->irqId ));
            return( ERR_OSS_BUSY_RESOURCE );
        }
    }

    DBGWRT_3((DBH, "addDevToIrqHandler: irqId=%d\n", dev->irqId ));
    dev->irqInstalled = 1;

	return( ERR_SUCCESS );
}


/**********************************************************************/
/** Set up the device slot with required address/data mode
 *
 *	Checks wether the slot is already enabled by another device with a 
 *  different subdevice offset / pciFunc. Initialize device slot if not 
 *  already done		   
 *
 *  \param  dev   \IN   device structure
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32 
setMiface( MK_DEV *dev )
{
	MK_DEV *dev2;
	int32 error;

	/* check if another device has already initialized the interface */
	for( dev2=(MK_DEV *)G_devList.head; 
		 dev2->node.next;
		 dev2 = (MK_DEV *)dev2->node.next ){

		if( strcmp(dev2->brdName, dev->brdName) == 0 &&
			dev2->devSlot == dev->devSlot && 
			dev->devSlot < BBIS_SLOTS_ONBOARDDEVICE_START){
			
			/*
			 * Check if a device with the same offset/pciFunc is already
			 * initialized. This would be an error
			 */
			if( dev2->subDevOffset == dev->subDevOffset &&
				dev2->pciFuncNbr == dev->pciFuncNbr ){
					DBGWRT_ERR((DBH,"*** MK: Device slot already in use: "
								" %s/%d/%d/%x\n",
								dev2->brdName, dev2->devSlot, 
								dev2->pciFuncNbr, 
								dev2->subDevOffset));
					return ERR_LL_DEV_BUSY;	/* better error code? */
			}

			/* 
			 * the device slot is already initialized. Check if it was
			 * initialized with the same parameters
			 */
			if( (dev->devAddrMode != dev2->devAddrMode) ||
				(dev->devDataMode != dev2->devDataMode)) {
					DBGWRT_ERR((DBH,"*** MK: Device slot initialized with "
								" different parameters by %s/%d\n",
								dev2->brdName, dev2->devSlot));
					return ERR_BBIS_ILL_DATAMODE;
			}

			/* otherwise we're done */
			return 0;
		}
	}

	/* device slot hasn't been initialized yet. do it now. */
	if ((error = dev->brdJumpTbl.setMIface(dev->brd,
										   dev->devSlot,
										   dev->devAddrMode,
										   dev->devDataMode))) {
		DBGWRT_ERR((DBH,"*** MK: can't init board slot\n"));
	}
	return error;
}


/**********************************************************************/
/** Init process lock mode
 *
 *  \param  dev   \IN   device structure
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

int32 initLockMode(MK_DEV *dev)
{
	u_int32 n;
	int32 error=0;

	switch(dev->lockMode) {
		case LL_LOCK_NONE:
			DBGWRT_2((DBH," prepare lockMode LL_LOCK_NONE\n"));
			/* nothing to do */
			break;

		case LL_LOCK_CALL:
			DBGWRT_2((DBH," prepare lockMode LL_LOCK_CALL\n"));

			/* create call sem's */
            for( n=0; n<mdis_call_num; ++n ) {
                if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
                                            &dev->semCall[n] )))
                    break;
            }
			break;

		case LL_LOCK_CHAN:
			DBGWRT_2((DBH," prepare lockMode LL_LOCK_CHAN\n"));

			/* alloc space for array of sem ptr's */
			dev->semChanP = OSS_MemGet( 
				dev->osh, 
				sizeof(OSS_SEM_HANDLE *) * dev->devNrChan,
				&dev->semChanAlloc );

			if( dev->semChanP == NULL )
				return(ERR_OSS_MEM_ALLOC);
				
			memset( dev->semChanP, 0, sizeof(OSS_SEM_HANDLE *) * 
					dev->devNrChan);

			/* init sems */
			for (n=0; n<dev->devNrChan; n++)
				if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN,1,
											&dev->semChanP[n] )))
					break;

			break;
		default:
			DBGWRT_ERR((DBH," *** open: unknown lockMode=%d\n",dev->lockMode));
			return(ERR_MK);
	}

	return(error);
}


/**********************************************************************/
/** Enable/disable board interrupt
 *
 *  If <enable>=1 enable board interrupt if irq has not been 
 *  enabled on that slot.
 *  If <enable>=0 disable board interrupt if irq has not been 
 *  enabled on that slot by another device.
 *
 *  \param  dev       \IN   device structure
 *  \param  enable    \IN   0=disable, 1=enable
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

static int32 
brdIrqEnable( MK_DEV *dev, int enable )
{
	MK_DEV *dev2;
	int32 error=0;

	/* check if another device has already enabled irq for that slot */
	for( dev2=(MK_DEV *)G_devList.head; 
		 dev2->node.next;
		 dev2 = (MK_DEV *)dev2->node.next ){

		if( dev2 == dev ) continue;	/* ignore my device */

		if( strcmp(dev2->brdName, dev->brdName) == 0 &&
			dev2->devSlot == dev->devSlot ){
			

			if( dev2->irqEnable )
				return 0;		/* irq already/still enabled */
		}
	}

	/* device slot's irq hasn't been enabled. enable/disable it now. */
	if( dev->brdJumpTbl.irqEnable )
		error = dev->brdJumpTbl.irqEnable(dev->brd, dev->devSlot, enable);

	return error;
}


/**********************************************************************/
/** Enable/disable interrupt on board+device
 *
 * System interrupt is masked while disabling
 *
 *  \param  dev       \IN   device structure
 *  \param  enable    \IN   0=disable, 1=enable
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

int32 
MDIS_EnableIrq(MK_DEV *dev, int enable)
{
    int32 error = ERR_SUCCESS;     /* error holder */
    u_int32 ret1=0, ret2=0;

	DBGWRT_1((DBH," EnableIrq: \n"));
	DBGWRT_2((DBH," %sable irqs\n", enable ? "en":"dis"));

	/*---------------------+
    |  enable interrupt    |
    +---------------------*/
	if (enable) {

		dev->irqEnable = 1;

        /* VME interrupt level already enabled by VmeInterruptAttachEvent() */

		/* enable board slot's irq */
		ret1 = brdIrqEnable( dev, 1 );

		/* enable on device */
		if( dev->llJumpTbl.setStat )
			ret2 = dev->llJumpTbl.setStat(dev->ll, M_MK_IRQ_ENABLE, 0, 1);

		if (ret1 && ret2) {		/* none of the calls was successful ? */
			dev->irqEnable = 0;
			error = ERR_MK_IRQ_ENABLE;
		}
	}
	/*---------------------+
    |  disable interrupt   |
    +---------------------*/
	else {
		OSS_IrqMask(dev->osh, NULL);

		dev->irqEnable = 0;

		/* disable on device */
		if( dev->ll && dev->llJumpTbl.setStat )
			ret2 = dev->llJumpTbl.setStat(dev->ll, M_MK_IRQ_ENABLE, 0, 0);

		/* disable board slot's irq */
		ret1 = brdIrqEnable( dev, 0 );

		OSS_IrqUnMask(dev->osh, NULL);

		if (ret1 && ret2) {		/* none of the calls was successful ? */
			error = ERR_MK_IRQ_ENABLE;
		}
	}

#ifdef DBG
	if (ret1) {
		DBGWRT_ERR((DBH," *** EnableIrq: warning: can't %sable irq on board\n",
					enable ? "en":"dis"));
	}
	if (ret2) {
		DBGWRT_ERR((DBH," *** EnableIrq: warning: can't %sable "
					"irq on device\n",
					enable ? "en":"dis"));
	}
#endif

	return(error);
}


/**********************************************************************/
/** First open of an MDIS device
 *
 *  \param  devName    \IN   name of new device
 *  \param  devDescHdl \IN   handle of device descriptor
 *  \param  devDesc    \IN   device descriptor data
 *  \param  brdName    \IN   name of carrier board
 *  \param  brdDesc    \IN   board descriptor data
 *
 *  \param  devP       \OUT  ptr to new device struct
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa MDIS_OpenDevice
 */
int32
MDIS_InitialOpen(
	char *devName,
	DESC_HANDLE *devDescHdl,
	DESC_SPEC *devDesc,
	char *brdName,
	DESC_SPEC *brdDesc,
	MK_DEV **devP)
{
    MK_DEV  *dev;
	MK_DRV  *drv;
	char    hwName[MK_MAX_DRVNAME+1];
	char    drvName[MK_MAX_DRVNAME+1];
    void    *drvHandle = NULL;
	u_int32 size;
	u_int32 value;
    int32   error;


    DBGWRT_1((DBH,"MK - InitialOpen: dev=%s brd=%s\n", devName, brdName));

	*devP = NULL;

	/* allocate the device structure */
	if( (dev = malloc( sizeof(*dev) )) == NULL )
		return( ERR_OSS_MEM_ALLOC );

	memset( dev, 0, sizeof(*dev));
	strncpy( dev->devName, devName, sizeof(dev->devName) );
	strncpy( dev->brdName, brdName, sizeof(dev->brdName) );

	/* init OSS for device */
	if( (error = OSS_Init( dev->devName, &dev->osh, MDIS_CurrentPid )))
		goto ERREXIT;


	/*------------------------------+
	|  get/set debug levels         |
	+------------------------------*/
	if ((error = DESC_GetUInt32(devDescHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_OSS")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto ERREXIT;

	OSS_DbgLevelSet(dev->osh, value);

	if ((error = DESC_GetUInt32(devDescHdl, OSS_DBG_DEFAULT, &value,
								"DEBUG_LEVEL_DESC")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto ERREXIT;

	DESC_DbgLevelSet(devDescHdl, value);

	if ((error = DESC_GetUInt32(devDescHdl, OSS_DBG_DEFAULT, &DBG_MYLEVEL,
								"DEBUG_LEVEL_MK")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		goto ERREXIT;

	/*------------------------------+
	|  get device slot              |
	+------------------------------*/
	if ((error = DESC_GetUInt32(devDescHdl, 0, &dev->devSlot, "DEVICE_SLOT")))
		goto ERREXIT;

	/*------------------------------+
	|  get hardware name            |
	+------------------------------*/
	size = sizeof(hwName) - strlen(MK_DRV_PREFIX) - strlen(MK_DRV_SUFFIX);

	if ((error = DESC_GetString(devDescHdl, "", hwName, &size, "HW_TYPE")))
		goto ERREXIT;

	/*------------------------------+
	|  Find the LL driver           |
	+------------------------------*/
	/*
	 * Since some old descriptors for M-modules <M100 have leading
	 * zeroes in hw name (e.g. M008), but ll drivers have the name
	 * libmen_ll_m8, remove the leading zeroes in that case.
	 */
	{
		int i,j;

		strcpy( drvName, MK_DRV_PREFIX );
		i = strlen(MK_DRV_PREFIX);
        j = 0;

		drvName[i++] = hwName[j++];

		if( hwName[0] == 'm' || hwName[0] == 'M'){
			while( hwName[j] == '0' ) /* skip leading zeroes */
				j++;
		}
		/*--- copy rest of string ---*/
		strcpy( &drvName[i], &hwName[j] );
	}

    strcat( drvName, MK_DRV_SUFFIX );
	tolower( drvName );

	DBGWRT_2((DBH, " info: device %s driver %s slot %d\n",
			  devName, drvName, dev->devSlot ));


    /*--- if not already loaded: load shared library ---*/
	if( (drv = MDIS_FindDrvByName( drvName )) == NULL ) {
        void (*getEntry)(LL_ENTRY *);

        DBGWRT_2((DBH, "loading driver %s\n", drvName ));

        /*--- load shared library ---*/
        if( (drvHandle = dlopen( drvName, RTLD_NOW | RTLD_LOCAL |
                                 RTLD_WORLD )) == NULL ) {
            error = ERR_MK_NO_LLDRV;
            DBGWRT_ERR((DBH,"*** MDIS_InitialOpen: can't find driver %s\n",
                        drvName ));
            goto ERREXIT;
        }

        /*--- get address of getEntry routine ---*/
        if( (getEntry = dlsym( drvHandle, "LL_GetEntry" )) == NULL ) {
            error = ERR_MK_NO_LLDRV;
            DBGWRT_ERR((DBH,"*** MDIS_InitialOpen: can't find getEntry "
                        "routine in %s\n", drvName ));
            goto ERREXIT;
        }


        /*--- add driver to driver list ---*/
        if( (drv = malloc( sizeof( *drv ) )) == NULL ) {
            error = ERR_OSS_MEM_ALLOC;
            goto ERREXIT;
        }

        strncpy( drv->drvName, drvName, sizeof(drv->drvName) );
        drv->drvHandle = drvHandle;
        drv->getEntry = getEntry;

        OSS_DL_AddTail( &G_drvList, &drv->node );
    }

	dev->drv = drv;				/* save driver link */

	/*--- get driver entry points ---*/
	drv->getEntry( &dev->llJumpTbl );


	/*------------------------------+
	|  prepare board handler        |
	+------------------------------*/

	/* open BBIS driver */
	if( (error = bbis_open( brdName, brdDesc, &dev->brd, &dev->brdJumpTbl ))) {
		DBGWRT_ERR((DBH,"*** MDIS_InitialOpen: %s can't open BBIS dev %s\n",
					devName, brdName ));
		goto ERREXIT;
	}


	/*------------------------------+
	|  get fixed parameters         |
	+------------------------------*/
	if ((error = getDeviceParams(dev)))				    /* device params */
		goto ERREXIT;

	if ((error = getBoardParams(dev)))	                /* board params */
		goto ERREXIT;

	if ((error = getPciBoardParams(dev, devDescHdl)))	/* PCI params */
		goto ERREXIT;


	/*--- for PCI devices, check if device present and vend/devId match ---*/
	if( dev->devBusType == OSS_BUSTYPE_PCI ){

		if( (error = checkPciDev(dev)))
			goto ERREXIT;
	}

    /*--- for devices connected via VME bus open VME server ---*/
    if( dev->brdBusType == OSS_BUSTYPE_VME ) {
#ifdef USE_VME_SERVER
        if( (dev->vmeHdl = MenC_VmeAttach()) < 0 )
            goto ERREXIT;
#endif /* USE_VME_SERVER */
    }


	/*------------------------------+
	|  map address spaces           |
	+------------------------------*/
	if ((error = MapAddressSpaces(dev, devDescHdl)))
		goto ERREXIT;


	/*-----------------------------------+
	|  install interrupt                 |
	+-----------------------------------*/
	if (dev->irqUse || (dev->irqInfo & BBIS_IRQ_EXPIRQ)) {

		/* determine interrupt parameters */
		if( (error = getIrqParams( dev )))
			goto ERREXIT;

		if( (error = addDevToIrqHandler( dev )))
			goto ERREXIT;
	}

	if (dev->irqUse) {
		if ((error = DESC_GetUInt32(devDescHdl, 0, &dev->irqEnable,
									"IRQ_ENABLE")) &&
			error != ERR_DESC_KEY_NOTFOUND)
			goto ERREXIT;
	}
	
	/*----------------------------------------+
	 | tell BBIS the irqHandle - if supported |
	 +----------------------------------------*/
	if( dev->brdJumpTbl.setIrqHandle != NULL )	{
		error = dev->brdJumpTbl.setIrqHandle( dev->brd,
											  (OSS_IRQ_HANDLE*)&G_irqHdl );
		if( error ) {
			DBGWRT_ERR(( DBH, "*** open.c: BB: setIrqHandle failed! \n" ));
			goto ERREXIT;
		}/*if*/
	}

 	/*------------------------------+
	|  init board slot              |
	+------------------------------*/
	if( (error = setMiface( dev )))
		goto ERREXIT;

	/* create device semaphore */
	if( (error = OSS_SemCreate( dev->osh, OSS_SEM_BIN, 1, &dev->semDev )))
		goto ERREXIT;


	/*------------------------------+
	|  init device                  |
	+------------------------------*/
	if ((error = dev->llJumpTbl.init(devDesc,
                                     dev->osh,
									 dev->ma,
									 dev->semDev,
									 NULL, /* irq handle */
									 &dev->ll ))) {
		dev->ll = NULL;
		DBGWRT_ERR((DBH," *** open: can't init device error=0x%lx\n", error));
	}
	else
		dev->initialized = TRUE;

	if( dev->exceptionOccurred ){
		slogf( __SLOG_CODE_MENDBG, _SLOG_WARNING,
               "*** MDIS: %d BBIS exceptions on %s / %s during "
				"device init. Check for correct module slot\n",
				dev->exceptionOccurred, dev->brdName, dev->devName );
		error = ERR_BUSERR;
		goto ERREXIT;
	}

	if( error )
		goto ERREXIT;
		
	/*------------------------------+
	|  get nr of device channels    |
	+------------------------------*/
	if ((error = dev->llJumpTbl.getStat(dev->ll,
										M_LL_CH_NUMBER,
										0,
										(int32*)&dev->devNrChan ))) {
		DBGWRT_ERR((DBH," *** open: can't get M_LL_CH_NUMBER\n"));
		goto ERREXIT;
	}

	DBGWRT_2((DBH," device channels: %d channels\n",dev->devNrChan));


	/*------------------------------+
	|  prepare process locking      |
	+------------------------------*/
	if ((error = initLockMode(dev)))
		goto ERREXIT;


	/*------------------------------+
	|  get irq enable               |
	|  enable interrupt             |
	+------------------------------*/
	if (dev->irqUse) {
		MDIS_EnableIrq(dev, dev->irqEnable);
	}

	OSS_DL_NewList( &dev->pathList );
    OSS_DL_AddTail( &G_devList, &dev->node );
    
    *devP = dev;

    return( ERR_SUCCESS );

 ERREXIT:
    return( error );
}

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  bb_smbpci.c
 *
 *      \author  dieter.pfeuffer@men.de
 *        $Date: 2014/08/27 09:51:59 $
 *    $Revision: 1.24 $
 *
 *      \brief   SMBPCI BBIS for SMB controllers on PCI bus
 *
 *     Required: ---
 *
 *     \switches  _ONE_NAMESPACE_PER_DRIVER_, SMBPCI_VARIANT, DBG
 *
 *               SMBPCI_VARIANT - prefix for global symbols
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: bb_smbpci.c,v $
 * Revision 1.24  2014/08/27 09:51:59  MRoth
 * R: no support for F23P and G24 (ICH compatible)
 * M: added Device IDs VAR_PCI_DEV_ID_10 and VAR_PCI_DEV_ID_11
 *
 * Revision 1.23  2012/03/23 09:12:52  dpfeuffer
 * R:1. support for G22/F22 (ICH compatible)
 *   2. case VAR_PCI_DEV_ID_9 added
 *
 * Revision 1.22  2012/01/30 15:07:56  dpfeuffer
 * R: support for FCH and FCH_ASF SMB controller
 * M: FCH and FCH_ASF specific part added
 *
 * Revision 1.21  2011/05/20 11:46:31  CRuff
 * R: 1. support of pci domains
 * M: 1a) added handling of code BBIS_CFGINFO_PCI_DOMAIN to SMBPCI_CfgInfo()
 *    1b) added new member domainNbr to BBIS_HANDLE
 *    1c) extend debug prints of pci bus number to also print the domain number
 *    1d) call all occurrences of OSS_PciXetConfig (and other OSS calls)
 *        with merged bus and domain
 *    1e) in ParsePciPath(): look for root device on all busses instead of
 *        expecting root device on bus 0
 *
 * Revision 1.20  2010/12/20 13:31:47  dpfeuffer
 * R:1. 16Z001 SMB ctrl. at ISA bus
 *   2. IO mapped variants for 16Z001 SMB ctrl. required
 *   3. M_BB_ID_CHECK and M_BB_ID_SIZE swapped by mistake
 *   4. OSS_UnAssignResources specifies wrong bus type OSS_BUSTYPE_VME
 * M:1. variant ISA_16Z001 implemented
 *   2. variant 16Z001_IO and ISA_16Z001_IO implemented
 *   3.a) M_BB_ID_CHECK fixed
 *     b) M_BB_ID_SIZE removed
 *   4. bus type fixed
 *
 * Revision 1.19  2010/09/27 11:18:53  CKauntz
 * R: SMBPCI_BrdExit does not release resources allocated in BrdInit
 * M: Fixed
 *
 * Revision 1.18  2010/07/15 14:33:53  MRoth
 * R: no support for G20 and F21P
 * M: added Device IDs VAR_PCI_DEV_ID_7 and VAR_PCI_DEV_ID_8 to SMBPCI_Init()
 *
 * Revision 1.17  2010/06/14 14:09:10  dpfeuffer
 * R: some error checks for too high mSlot parameter was wrong
 * M: mSlot > MAX_DEVS checks fixed to mSlot >= MAX_DEVS
 *
 * Revision 1.16  2010/06/11 15:37:01  CRuff
 * R: 1. allow driver usage for non-PCI SMB controllers
 *    2. SMB_BUSY_WAIT default value not sufficient in some cases
 * M: 1a) move the check if SmbHandle is set in BSP to SMBPCI_Init
 *    1b) SMBPCI_Init: only scan PCI paramters, perform ID Check and do PCI
 *        controller mapping if SmbHandle is empty
 *    1c) for variant SMBPCI_NAT, never initialize SMB Lib; SMBHandle should
 *        always be set by BSP
 *    2. increase SMB_BUSY_WAIT default value to 100
 *
 * Revision 1.15  2009/10/07 13:23:52  dpfeuffer
 * R: initialization of busyWait in SMB_DESC_XXX struct
 * M: SMB_BUSY_WAIT descriptor value added
 *
 * Revision 1.14  2009/08/03 08:37:15  dpfeuffer
 * R:1. porting to MDIS5 according porting guide rev. 0.7
 *   2. support of multifunction bridges
 * M:1. cosmetics
 *   2.a) in PciParseDev(): hand over the function num to OSS_PciGetConfig
 *        if necessary
 *     b) allow multifunction bridge type when pci headertype is checked
 *
 * Revision 1.13  2009/05/18 13:19:14  dpfeuffer
 * R: SMBPCI_16Z001(_F208) MEN PCI vendor-id (0x1A88) not supported
 * M: SMBPCI_Init(): VAR_PCI_VEN_ID_2 check added
 *
 * Revision 1.12  2009/04/03 15:16:42  dpfeuffer
 * R: MDIS4 for Windows: MK does not support OSS_BUSTYPE_SMB as device bus type
 * M: Under MDIS4 for Windows SMBPCI_BrdInfo(BBIS_BRDINFO_DEVBUSTYPE) uses
 *    OSS_BUSTYPE_NONE as device bus type
 *
 * Revision 1.11  2009/03/19 13:36:50  dpfeuffer
 * R: no support for MDIS5
 * M: SetStat/GetStat interface modified for MDIS5
 *
 * Revision 1.10  2009/03/19 11:26:27  dpfeuffer
 * R: under Windows, a precise device bustype is required to indicate SMB
 * M: SMBPCI_BrdInfo(BBIS_BRDINFO_DEVBUSTYPE) changed from
 *    OSS_BUSTYPE_NONE to OSS_BUSTYPE_SMB
 *
 * Revision 1.9  2008/10/20 13:19:50  MSoehnlein
 * Added SCH support
 *
 * Revision 1.8  2008/01/22 14:18:07  ufranke
 * fixed
 *  - now compilable for !ONE_NAMESPACE_PER_DRIVER
 *
 * Revision 1.7  2007/10/09 15:20:39  JWu
 * added support for ICH8 (F18)
 *
 * Revision 1.6  2007/04/10 13:14:20  DPfeuffer
 * 16Z001 driver variant implemented
 *
 * Revision 1.5  2006/12/20 12:53:10  ufranke
 * changed
 *  - fkt13 replaced by setIrqHandle
 *
 * Revision 1.4  2006/04/28 13:54:56  DPfeuffer
 * added support for ICH0, ICH2, ICH7
 *
 * Revision 1.3  2006/02/28 08:40:30  DPfeuffer
 * SMBPCI_Init(): SMB_ALERT_POLL_FREQ desc-key added
 *
 * Revision 1.2  2006/02/10 17:04:03  SVogel
 * added
 *     + alreadyInitialized (check if smb handle was already initialized)
 *
 * Revision 1.1  2005/11/25 15:08:46  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#define RCSID 	"$Id: bb_smbpci.c,v 1.24 2014/08/27 09:51:59 MRoth Exp $"

#include "smbpci_int.h"		/* internal header file */


/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* init/exit */
static int32 SMBPCI_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 SMBPCI_BrdInit(BBIS_HANDLE*);
static int32 SMBPCI_BrdExit(BBIS_HANDLE*);
static int32 SMBPCI_Exit(BBIS_HANDLE**);
/* info */
static int32 SMBPCI_BrdInfo(u_int32, ...);
static int32 SMBPCI_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 SMBPCI_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 SMBPCI_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  SMBPCI_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 SMBPCI_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 SMBPCI_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 SMBPCI_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 SMBPCI_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 SMBPCI_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 SMBPCI_SetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64);
static int32 SMBPCI_GetStat(BBIS_HANDLE*, u_int32, int32, INT32_OR_64*);
/* unused */
static int32 SMBPCI_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *brdH, int32 retCode);

/* SMB ctrl at PCIbus */
#ifndef VAR_ISA
	static int32 ParsePciPath( BBIS_HANDLE *h, u_int32 *pciBusNbrP );
	static int32 PciParseDev( BBIS_HANDLE *h, u_int32 pciBusNbr,
							  u_int32 pciDevNbr, int32 *vendorIDP,
							  int32 *deviceIDP, int32 *headTypeP, int32 *secondBusP);
	static int32 PciCfgErr( BBIS_HANDLE *h, char *funcName, int32 error,
							u_int32 pciBusNbr, u_int32 pciDevNbr, u_int32 reg );
#endif /* SMB ctrl at PCIbus */

static int32 CfgInfoSlot( BBIS_HANDLE *brdH, va_list argptr );


/****************************** SMBPCI_GetEntry ******************************/
/** Initialize driver's jump table
 *
 *  \param bbisP     \OUT Pointer to the initialized jump table structure
 */
#ifdef _ONE_NAMESPACE_PER_DRIVER_
	extern void BBIS_GetEntry( BBIS_ENTRY *bbisP )
#else
	extern void __SMBPCI_GetEntry( BBIS_ENTRY *bbisP )
#endif
{
    /* init/exit */
    bbisP->init         =   SMBPCI_Init;
    bbisP->brdInit      =   SMBPCI_BrdInit;
    bbisP->brdExit      =   SMBPCI_BrdExit;
    bbisP->exit         =   SMBPCI_Exit;
    bbisP->fkt04        =   SMBPCI_Unused;
    /* info */
    bbisP->brdInfo      =   SMBPCI_BrdInfo;
    bbisP->cfgInfo      =   SMBPCI_CfgInfo;
    bbisP->fkt07        =   SMBPCI_Unused;
    bbisP->fkt08        =   SMBPCI_Unused;
    bbisP->fkt09        =   SMBPCI_Unused;
    /* interrupt handling */
    bbisP->irqEnable    =   SMBPCI_IrqEnable;
    bbisP->irqSrvInit   =   SMBPCI_IrqSrvInit;
    bbisP->irqSrvExit   =   SMBPCI_IrqSrvExit;
    bbisP->setIrqHandle =   NULL;
    bbisP->fkt14        =   SMBPCI_Unused;
    /* exception handling */
    bbisP->expEnable    =   SMBPCI_ExpEnable;
    bbisP->expSrv       =   SMBPCI_ExpSrv;
    bbisP->fkt17        =   SMBPCI_Unused;
    bbisP->fkt18        =   SMBPCI_Unused;
    bbisP->fkt19        =   SMBPCI_Unused;
    /* */
    bbisP->fkt20        =   SMBPCI_Unused;
    bbisP->fkt21        =   SMBPCI_Unused;
    bbisP->fkt22        =   SMBPCI_Unused;
    bbisP->fkt23        =   SMBPCI_Unused;
    bbisP->fkt24        =   SMBPCI_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   SMBPCI_SetStat;
    bbisP->getStat      =   SMBPCI_GetStat;
    bbisP->setMIface    =   SMBPCI_SetMIface;
    bbisP->clrMIface    =   SMBPCI_ClrMIface;
    bbisP->getMAddr     =   SMBPCI_GetMAddr;
    bbisP->fkt30        =   SMBPCI_Unused;
    bbisP->fkt31        =   SMBPCI_Unused;
}

/******************************** SMBPCI_Init ********************************/
/** Initialize the bbis board driver.
 *
 *  The function allocates and returns the board handle, queries the board
 *  descriptor, assigns the resources and maps the required address spaces.
 *
 *  The function decodes \ref descriptor_entries "these descriptor entries"
 *  in addition to the general descriptor keys.
 *
 *  Note: No HW access is allowed within this function!
 *
 *  \param osH       \IN  OSS handle
 *  \param descP     \IN  Pointer to descriptor data
 *  \param brdHdlP   \OUT Pointer to bbis handle
 *
 *  \return	         \c 0 On success or error code
 */
static int32 SMBPCI_Init(
    OSS_HANDLE      *osH,
    DESC_SPEC       *descP,
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE		*brdH = NULL;
	u_int32			gotsize, value, i, devNameSize;
    int32			status;
	int32			devCount=0;	/* number of devices specified in desc */

/* SMB ctrl at PCIbus */
#ifndef VAR_ISA
    int32			id;
	void*			bar;
	u_int32			ctrlrBar=0, ctrlrOffset=0;
	u_int32 		mechSlot;
/* SMB ctrl at ISAbus */
#else
	u_int32		isaAddr;			/* ISA base address */
#endif

/* SCH */
#if defined( _SMBPCI_SCH )
	int32			physBase;
#endif

    /*-------------------------------+
    | initialize the board structure |
    +-------------------------------*/
	*brdHdlP = NULL;

    /* get memory for the board structure */
    brdH = (BBIS_HANDLE*) (OSS_MemGet(
        osH, sizeof(BBIS_HANDLE), &gotsize ));
    if ( brdH == NULL )
        return ERR_OSS_MEM_ALLOC;

	/* clear */
    OSS_MemFill(osH, gotsize, (char*)brdH, 0);

    /* store data into the board structure */
    brdH->ownMemSize = gotsize;
    brdH->osH = osH;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
    /* init descH */
    status = DESC_Init( descP, osH, &brdH->descH );
    if (status)
		return( Cleanup(brdH,status) );

    /* get DEBUG_LEVEL_DESC - optional*/
    status = DESC_GetUInt32(brdH->descH, OSS_DBG_DEFAULT, &value,
				"DEBUG_LEVEL_DESC");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdH,status) );

	/* set debug level for DESC module */
	DESC_DbgLevelSet(brdH->descH, value);

    /* get DEBUG_LEVEL - optional */
    status = DESC_GetUInt32( brdH->descH, OSS_DBG_DEFAULT, &(brdH->dbgLev),
                "DEBUG_LEVEL");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdH,status) );

    DBGWRT_1((DBH,"BB - %s_Init\n",VAR_NAME_STR));

    /* scan smb keys */

    /* SMB_BUSNBR */
    if( (status = DESC_GetUInt32( brdH->descH, 0,
								&brdH->smbBusNbr, "SMB_BUSNBR")) )
		return( Cleanup(brdH,status) );

	/* get SMBDEV_n */
	for( i=0; i<MAX_DEVS; i++ ){
		/* SMBDEV_n/NAME - required for PnP OS (BBIS_CFGINFO_SLOT) */
		devNameSize = DEVNAME_SIZE;
		status = DESC_GetString( brdH->descH, BBIS_SLOT_STR_UNK, brdH->smb[i].name,
								 &devNameSize, "SMBDEV_%d/NAME", i );
		if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
			return( Cleanup(brdH,status) );

		if( status == ERR_SUCCESS ){
			devCount++;
			DBGWRT_2(( DBH, " devCount=%d, SMBDEV_%d/NAME = %s\n",
				devCount, i, brdH->smb[i].name ));
		}
	}

    /* get SMB_ALERT_POLL_FREQ - optional */
    status = DESC_GetUInt32( brdH->descH, 0, &brdH->alertPollFreq,
                "SMB_ALERT_POLL_FREQ");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdH,status) );

    /* get SMB_BUSY_WAIT - optional */
    status = DESC_GetUInt32( brdH->descH, 100, &brdH->busyWait,
                "SMB_BUSY_WAIT");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdH,status) );

	/* check if any device specified */
	if( devCount == 0 ){
		DBGWRT_ERR((DBH, "*** %s_Init: No SMB devices in descriptor!\n", VAR_NAME_STR));
		return( Cleanup(brdH,ERR_BBIS_DESC_PARAM) );
	}

	/*------------------------------+
	|  set SMB addr. space default  |
	+------------------------------*/
	/* note: this may be overwritten from FCH_ASF variant */
	brdH->smbAddrSpaceType = ADDRSPACE;
	brdH->smbResourceType = RES_SPACETYPE;

	/*------------------------------+
	|  check smb handle             |
	+------------------------------*/
	/* try to get the SMB handle from OSS */
    status = OSS_GetSmbHdl( brdH->osH, brdH->smbBusNbr, &brdH->smbH);
    if (!status) {

    	DBGWRT_1((DBH, "SmbHdl already set"));

    	/* smb handle already set*/
    	brdH->alreadyInitialized = 1;

    } else {
    	/* SMBPCI BBIS variant without SMB_XXX_Init */
#ifdef _SMBPCI_NAT
		DBGWRT_ERR((DBH, "*** %s_Init: No SMB handle from OSS!\n", VAR_NAME_STR));
		return( Cleanup(brdH,ERR_BBIS_ILL_PARAM) );
#else

    	/* smb handle not set by bsp */
    	brdH->alreadyInitialized = 0;

		/* PCI_DOMAIN_NUMBER - optional */
		status = DESC_GetUInt32( brdH->descH, 0, &brdH->domainNbr, 
								 "PCI_DOMAIN_NUMBER");
								 
		if ( status == ERR_DESC_KEY_NOTFOUND ) {
			/* default pci domain is 0 */
			brdH->domainNbr = 0;
		}
		
/* SMB ctrl at PCIbus */
#ifndef VAR_ISA
	
	    /* PCI_BUS_NUMBER - required if PCI_BUS_PATH not given  */
	    status = DESC_GetUInt32( brdH->descH, 0, &brdH->busNbr,
								 "PCI_BUS_NUMBER");

		if( status == ERR_DESC_KEY_NOTFOUND ){

			/* PCI_BUS_PATH - required if PCI_DEVICE_NUMBER not given */
			brdH->pciPathLen = MAX_PCI_PATH;

			status = DESC_GetBinary( brdH->descH, (u_int8*)"", 0,
									 brdH->pciPath, &brdH->pciPathLen,
									 "PCI_BUS_PATH");

			if( status ){
				DBGWRT_ERR((DBH, "*** BB - %s_Init: Desc Key PCI_BUS_PATH "
							"AND PCI_BUS_NUMBER not found!\n",	VAR_NAME_STR));
				return( Cleanup(brdH,status) );
			}

			/*--------------------------------------------------------+
			|  parse the PCI_PATH to determine bus number of devices  |
			+--------------------------------------------------------*/
#ifdef DBG
			DBGWRT_2((DBH, " PCI_PATH="));
			for(i=0; i<brdH->pciPathLen; i++){
				DBGWRT_2((DBH, "0x%x", brdH->pciPath[i]));
			}
			DBGWRT_2((DBH, "\n"));
#endif
			if( (status = ParsePciPath( brdH, &brdH->busNbr )) )
				return( Cleanup(brdH,status));

		} /* if( status == ERR_DESC_KEY_NOTFOUND ) */
		else {
			if( status == ERR_SUCCESS) {
				DBGWRT_1((DBH,"BB - %s: Using main PCI Bus Number from desc %d\n",
						  VAR_NAME_STR, brdH->busNbr ));
			}
			else {
				return( Cleanup(brdH,status) );
			}
		}

	    /* PCI_DEVICE_NUMBER - required if no PCI BUS SLOT given */
	    status = DESC_GetUInt32( brdH->descH, 0xffff, &brdH->pciDevNbr, "PCI_DEVICE_NUMBER");
	    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
	        return( Cleanup(brdH,status) );

		if(status==ERR_DESC_KEY_NOTFOUND){

			/* PCI_BUS_SLOT - required if PCI_DEVICE_NUMBER not given */
	    	status = DESC_GetUInt32( brdH->descH, 0, &mechSlot, "PCI_BUS_SLOT");

			if( status==ERR_DESC_KEY_NOTFOUND ){
				DBGWRT_ERR((DBH, "*** BB - %s_Init: Found neither Desc Key "
	                        "PCI_BUS_SLOT nor PCI_DEVICE_NUMBER !\n", VAR_NAME_STR));
			}

			if( status )
	        	return( Cleanup(brdH,status) );

		    /* convert PCI slot into PCI device id */
	    	if( (status = OSS_PciSlotToPciDevice( osH, 
	    		                                  OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr), 
	    		                                  mechSlot, (int32*)&brdH->pciDevNbr)) )
				return( Cleanup(brdH,status) );
	    }

	    /* get PCI_FUNCTION_NUMBER - optional */
	    status = DESC_GetUInt32( brdH->descH, 0, &brdH->pciFuncNbr,
	                "PCI_FUNCTION_NUMBER");
	    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
	        return( Cleanup(brdH,status) );

	    /* ID_CHECK */
	    status = DESC_GetUInt32( brdH->descH, 1, &brdH->idCheck, "ID_CHECK");
	    if( status && (status!=ERR_DESC_KEY_NOTFOUND) )
	        return( Cleanup(brdH,status) );


		if( brdH->idCheck ){
			/*------------------------------+
			| check vendor-id and device-id |
			+------------------------------*/
			/* get vendor-id */
			if( (status = OSS_PciGetConfig( osH, 
				                            OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr), 
				                            brdH->pciDevNbr, brdH->pciFuncNbr, OSS_PCI_VENDOR_ID, &id )) )
				return( Cleanup(brdH,status) );

			/* verify vendor-id */
			switch( id ){
			case VAR_PCI_VEN_ID:
#ifdef VAR_PCI_VEN_ID_2
			case VAR_PCI_VEN_ID_2:
#endif
				DBGWRT_2(( DBH, " VAR_PCI_VEN_ID[_X]=0x%x found\n", id));
				break;

			default:
				DBGWRT_ERR((DBH, "*** BB - %s_Init: illegal vendor-id=0x%4x\n",
					VAR_NAME_STR, id));
				return( Cleanup(brdH,ERR_BBIS_ILL_ID) );
			}

			/* get device-id */
			if( (status = OSS_PciGetConfig( osH, 
				                            OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr), 
				                            brdH->pciDevNbr, brdH->pciFuncNbr, OSS_PCI_DEVICE_ID, &id )) )
				return( Cleanup(brdH,status) );

			/* verify device-id */
			switch( id ){
			case VAR_PCI_DEV_ID:

#ifdef VAR_PCI_DEV_ID_2
			case VAR_PCI_DEV_ID_2:
#endif
#ifdef VAR_PCI_DEV_ID_3
			case VAR_PCI_DEV_ID_3:
#endif
#ifdef VAR_PCI_DEV_ID_4
			case VAR_PCI_DEV_ID_4:
#endif
#ifdef VAR_PCI_DEV_ID_5
			case VAR_PCI_DEV_ID_5:
#endif
#ifdef VAR_PCI_DEV_ID_6
			case VAR_PCI_DEV_ID_6:
#endif
#ifdef VAR_PCI_DEV_ID_7
			case VAR_PCI_DEV_ID_7:
#endif
#ifdef VAR_PCI_DEV_ID_8
			case VAR_PCI_DEV_ID_8:
#endif
#ifdef VAR_PCI_DEV_ID_9
			case VAR_PCI_DEV_ID_9:
#endif
#ifdef VAR_PCI_DEV_ID_10
			case VAR_PCI_DEV_ID_10:
#endif
#ifdef VAR_PCI_DEV_ID_11
			case VAR_PCI_DEV_ID_11:
#endif
				DBGWRT_2(( DBH, " VAR_PCI_DEV_ID[_X]=0x%x found\n", id));
				break;

			default:
				DBGWRT_ERR((DBH, "*** BB - %s_Init: illegal device-id=0x%4x\n",
					VAR_NAME_STR, id));
				return( Cleanup(brdH,ERR_BBIS_ILL_ID) );
			}
		} /* if( brdH->idCheck ) */

/* AMD FCH */
#if defined( _SMBPCI_FCH ) || defined( _SMBPCI_FCH_ASF )
{
		MACCESS ma;
		u_int8 bi, b[4];
		u_int32 d32=0;

		/* map index and data register used for indirect addressing */
		status = OSS_MapPhysToVirtAddr( osH,
						(void*)((INT32_OR_64)VAR_PMIO_IDXDATA), VAR_PMIO_SIZE,
						ADDRSPACE, BUSTYPE,
						brdH->busNbr, &ma );
			if( status )
				return( Cleanup(brdH,status) );

		/* read 16/32bit base address register */
		for( bi=0; bi<VAR_PM_SMBADDR_BYTES; bi++ ){
			MWRITE_D8(ma,VAR_PMIO_IDX_OFF,VAR_PM_SMBADDR + bi);
			b[bi] = MREAD_D8(ma, VAR_PMIO_DATA_OFF);
			d32 |= ((u_int32)b[bi])<<(bi*8);
		}

		/* unmap index and data register */
		OSS_UnMapVirtAddr( osH, &ma, VAR_PMIO_SIZE, ADDRSPACE );

		DBGWRT_2(( DBH, " d32=0x%x\n", d32));

	/* FCH standard SMBus controller */
#	if defined( _SMBPCI_FCH )
		bar = (void*)( (U_INT32_OR_64)d32 & VAR_PM_SMBUS0EN_BADDR );

	/* FCH_ASF SMBus controller */
#	else
		bar = (void*)( ((U_INT32_OR_64)d32 & VAR_PM_ACPIMMIOEN_ASF )
			+ VAR_PM_ACPIMMIOEN_ASFOFF);

		DBGWRT_2(( DBH, " VAR_PM_ACPIMMIOEN_ENBL=%d, _IO=%d\n",
			(d32 & VAR_PM_ACPIMMIOEN_ENBL) ? 1:0,
			(d32 & VAR_PM_ACPIMMIOEN_IO) ? 1:0));

		/*
		 * FCH_ASF variant uses MAC_IO_MAPPED
		 * If the SMB controller regs are mem mapped,
		 * then we have to overwrite the SMB addr. space defaults.
		 */
		if( !(d32 & VAR_PM_ACPIMMIOEN_IO) ){
			brdH->smbAddrSpaceType = OSS_ADDRSPACE_MEM;
			brdH->smbResourceType = OSS_RES_MEM;
		}

		/* verify that address space is enabled */
		if( (d32 & VAR_PM_ACPIMMIOEN_ENBL) ? 0:1 ){
			DBGWRT_ERR((DBH, "*** BB - %s_Init: "
				"address space not enabled\n", VAR_NAME_STR));
			return( Cleanup(brdH,ERR_BBIS_ILL_ADDRMODE) );
		}
#	endif

}
/* not AMD FCH */ 
#else

#ifdef VAR_CTRLR_BAR
		ctrlrBar = VAR_CTRLR_BAR;
#else
	    /* get CTRLR_BAR */
	    if( (status = DESC_GetUInt32( brdH->descH, 0, &ctrlrBar,
	                "CTRLR_BAR")) )
			return( Cleanup(brdH,status) );
#endif

	    /* get CTRLR_OFFSET - optional */
	    status = DESC_GetUInt32( brdH->descH, VAR_CTRLR_OFFSET, &ctrlrOffset,
	                "CTRLR_OFFSET");
	    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
	        return( Cleanup(brdH,status) );

	    /*-------------------------------------------+
	    | get, assign and map ctrl register space    |
	    +-------------------------------------------*/
		/* get base address ctrl registers */
/* ICH */
#ifndef _SMBPCI_SCH
		if( (status = OSS_BusToPhysAddr( osH, OSS_BUSTYPE_PCI, &bar,
										 OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr), 
										 brdH->pciDevNbr, brdH->pciFuncNbr, ctrlrBar)) )
			return( Cleanup(brdH,status) );
/* SCH */
#else
		/* Since OSS_BusToPhysAddr does not handle configuration registers other */
		/* than BAR0 - BAR5 we have to do read the SMBase address on our own */

		physBase = 0;

		OSS_PciGetConfig(
				osH,
				OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr),
				brdH->pciDevNbr,
				brdH->pciFuncNbr,
				(OSS_PCI_ACCESS_32 | ctrlrBar),
				&physBase);

		/* Bit 15:6 is SMBus Base Address */
		bar = (void *)((U_INT32_OR_64)physBase & 0xFFC0);

		/* Check address and abort if not valid */
		if( bar == 0 )
			return( Cleanup(brdH, ERR_DESC_KEY_NOTFOUND) );

#endif /* _SMBPCI_SCH */

/* not AMD FCH */
#endif

		/* compute module ctrl register space */
		brdH->res[RES_CTRL_ADDR].u.mem.physAddr =
			(void*)((INT32_OR_64)bar + ctrlrOffset );

/* SMB ctrl at ISAbus */
#else
		/* get DEVICE_ADDR */
		status = DESC_GetUInt32( brdH->descH, 0, &isaAddr,
					"DEVICE_ADDR");
		if ( status ){
			DBGWRT_ERR((DBH, "*** BB - %s_Init: Desc Key DEVICE_ADDR "
				"not found\n", VAR_NAME_STR));
			return( Cleanup(brdH,status) );
		}

		/* compute module ctrl register space */
		brdH->res[RES_CTRL_ADDR].u.mem.physAddr =
			(void*)((INT32_OR_64)isaAddr );

#endif /* SMB ctrl at ISAbus */

	    /* assign the resources */
		brdH->res[RES_CTRL_ADDR].type = brdH->smbResourceType;
		brdH->res[RES_CTRL_ADDR].u.mem.size = VAR_CTRLR_SIZE;

	    if( (status = OSS_AssignResources( osH, BUSTYPE, 
	    	                               OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr),
	                                       RESOURCE_NBR, brdH->res )) )
			return( Cleanup(brdH,status) );

#ifdef OSS_HAS_UNASSIGN_RESOURCES
	brdH->resourcesAssigned = TRUE;
#endif

		DBGWRT_2(( DBH, " SMB-ctrlr physAddr=%08p\n",
			brdH->res[RES_CTRL_ADDR].u.mem.physAddr));

	    /* map SMB ctrl register space */
		status = OSS_MapPhysToVirtAddr( osH,
						brdH->res[RES_CTRL_ADDR].u.mem.physAddr, brdH->res[RES_CTRL_ADDR].u.mem.size,
						brdH->smbAddrSpaceType, BUSTYPE,
						OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr), 
						&brdH->virtCtrlBase );
	        if( status )
				return( Cleanup(brdH,status) );
#endif /* _SMBPCI_NAT */
	} /* if ( !brdH->alreadyInitialized ) */

	/* exit descH */
    status = DESC_Exit( &brdH->descH );
    if (status)
        return( Cleanup(brdH,status) );

  	*brdHdlP = brdH;

    return 0;
}

/****************************** SMBPCI_BrdInit *******************************/
/** Board initialization
 *
 *  The function initializes the board with the definitions made in the
 *  descriptor. For the M-Module slots, the interrupt levels and vectors are
 *  configured. The interrupt is disabled.
 *  The A32 VME base address will be set if required.
 *
 *  \param brdH	     \IN  Bbis handle
 *
 *  \return	         \c 0 On success or error code
 */
static int32 SMBPCI_BrdInit(
    BBIS_HANDLE     *brdH )
{
    int32			status;

	DBGWRT_1((DBH, "BB - %s_BrdInit\n",VAR_NAME_STR));

    /* try to get the SMB handle from OSS */

    if ( !brdH->alreadyInitialized )
    {
#ifdef _SMBPCI_NAT
		DBGWRT_ERR((DBH,"*** %s_BrdInit: No Smb hdl exists for native SMB BBIS. This should never happen!\n", __FUNCTION__));
		return (ERR_BBIS_ILL_PARAM );
#else
		/* got no SMB handle from OSS */
    	DBGWRT_1((DBH, "%s_BrdInit no SMB hdl from OSS: call __SMBPCI_InitLib\n",VAR_NAME_STR));

        /*------------------------------+
        |  init SMB library             |
        +------------------------------*/
        status = __SMBPCI_InitLib( brdH );
    	if ( status )
    	{
		    return status;
    	}

	    if( (status = OSS_SetSmbHdl( brdH->osH, brdH->smbBusNbr, brdH->smbH)) )
    	{
	    	DBGWRT_ERR((DBH,"*** %s_BrdInit: OSS_SetSmbHdl\n",VAR_NAME_STR));
		    return status;
	    }
#endif /* _SMBPCI_NAT */
    }

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* drivers ident function */
	brdH->idFuncTbl.idCall[0].identCall = Ident;
	/* libraries ident functions */
	brdH->idFuncTbl.idCall[1].identCall = DESC_Ident;
	brdH->idFuncTbl.idCall[2].identCall = OSS_Ident;
    brdH->idFuncTbl.idCall[3].identCall = ((SMB_HANDLE*)brdH->smbH)->Ident;
	
    /* terminator */
	brdH->idFuncTbl.idCall[4].identCall = NULL;

	return 0;
}

/****************************** SMBPCI_BrdExit *******************************/
/** Board deinitialization
 *
 *  \param brdH      \IN  Bbis handle
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_BrdExit(
    BBIS_HANDLE     *brdH )
{
	DBGWRT_1((DBH, "BB - %s_BrdExit\n",VAR_NAME_STR));

    /*------------------------------+
    |  exit SMB library             |
    +------------------------------*/
	if( brdH->smbH )
	{
        if( !brdH->alreadyInitialized )
        {
    		((SMB_HANDLE*)brdH->smbH)->Exit( &brdH->smbH );
	        OSS_SetSmbHdl( brdH->osH, brdH->smbBusNbr, NULL); /* remove from OSS smb table */
    	}
    }
    return 0;
}

/****************************** SMBPCI_Exit **********************************/
/** Cleanup memory
 *
 *  \param brdHdlP   \IN  Pointer to bbis handle
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_Exit(
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE	*brdH = *brdHdlP;
	int32		error = 0;

    DBGWRT_1((DBH, "BB - %s_Exit\n",VAR_NAME_STR));

    /*------------------------------+
    |  cleanup memory               |
    +------------------------------*/
	error = Cleanup(brdH, error);
    *brdHdlP = NULL;

    return error;
}

/****************************** SMBPCI_BrdInfo *******************************/
/** Get information about hardware and driver requirements
 *
 *  Following info codes are supported:
 *
 *  \code
 *  Code                      Description
 *  ------------------------  ------------------------------------------------
 *  BBIS_BRDINFO_BUSTYPE      Bustype of the specified board.
 *  BBIS_BRDINFO_DEVBUSTYPE   Bustype of the specified device
 *                             (not the board bus type!).
 *  BBIS_BRDINFO_FUNCTION     Information if an optional BBIS function is
 *                             supported or not.
 *  BBIS_BRDINFO_NUM_SLOTS    Number of devices used from the driver.
 *  BBIS_BRDINFO_INTERRUPTS   Supported interrupt capability
 *                             (BBIS_IRQ_DEVIRQ/BBIS_IRQ_EXPIRQ) of the
 *                             specified device.
 *  BBIS_BRDINFO_ADDRSPACE    Address characteristic
 *                             (OSS_ADDRSPACE_MEM/OSS_ADDRSPACE_IO) of the
 *                             specified device.
 *  BBIS_BRDINFO_BRDNAME      Short hardware name and type of the board without
 *                             any prefix or suffix.
 *                             The name must not contain any non-printing characters.
 *                             The length of the returned string, including the
 *                             terminating null character, must not exceed
 *                             BBIS_BRDINFO_BRDNAME_MAXSIZE.
 *                             Examples: D201 board, PCI device, Chameleon FPGA\endcode
 *
 *  \param code      \IN  Reference to the information we need
 *  \param ...       \IN  Argument(s)
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_BrdInfo(
    u_int32 code,
    ... )
{
	int32		status = ERR_SUCCESS;
    va_list     argptr;

    va_start(argptr,code);

    switch( code ){

    /* supported functions */
    case BBIS_BRDINFO_FUNCTION:
    {
        u_int32 funcCode = va_arg( argptr, u_int32 );
        u_int32 *used = va_arg( argptr, u_int32* );

        funcCode = funcCode; /* dummy to avoid warning */
		*used = FALSE;
		break;
	}

    /* number of devices */
    case BBIS_BRDINFO_NUM_SLOTS:
    {
        u_int32 *numSlot = va_arg( argptr, u_int32* );

		/*
		 * No board handle here, return maximum
		 */
        *numSlot = MAX_DEVS;
        break;
    }

	/* bus type */
    case BBIS_BRDINFO_BUSTYPE:
    {
        u_int32 *busType = va_arg( argptr, u_int32* );

		*busType = BUSTYPE;
        break;
    }

    /* device bus type */
    case BBIS_BRDINFO_DEVBUSTYPE:
    {
        u_int32 mSlot       = va_arg( argptr, u_int32 );
        u_int32 *devBusType = va_arg( argptr, u_int32* );

        mSlot = mSlot; /* dummy */
/* MDIS4 for Windows: MK does not support OSS_BUSTYPE_CHAM */
#if ( defined(WINNT) && !defined(_MDIS5_) )
			*devBusType = OSS_BUSTYPE_NONE;
#else
			*devBusType = OSS_BUSTYPE_SMB;
#endif
        break;
    }

    /* interrupt capability */
    case BBIS_BRDINFO_INTERRUPTS:
    {
        u_int32 mSlot = va_arg( argptr, u_int32 );
        u_int32 *irqP = va_arg( argptr, u_int32* );

		mSlot = mSlot; /* dummy access to avoid compiler warning */
		*irqP = 0;
        break;
    }

    /* address space type */
    case BBIS_BRDINFO_ADDRSPACE:
    {
        u_int32 mSlot      = va_arg( argptr, u_int32 );
        u_int32 *addrSpace = va_arg( argptr, u_int32* );

		mSlot = mSlot; /* dummy access to avoid compiler warning */
		*addrSpace = ADDRSPACE;
        break;
    }

    /* board name */
    case BBIS_BRDINFO_BRDNAME:
    {
        char	*brdName = va_arg( argptr, char* );
		char	*from;

		/*
		 * build hw name (e.g. SMB controller)
		 */
		from = VAR_NAME_STR;
	    while( (*brdName++ = *from++) );	/* copy string */
		from = " controller";
	    while( (*brdName++ = *from++) );	/* copy string */

        break;
    }

    /* error */
    default:
        status = ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** SMBPCI_CfgInfo *******************************/
/** Get information about board configuration
 *
 *  Following info codes are supported:
 *
 *  \code
 *  Code                      Description
 *  ------------------------  ------------------------------------------------
 *  BBIS_CFGINFO_BUSNBR       Number of the bus on which the board resides.
 *  BBIS_CFGINFO_PCI_DOMAIN	  Number of the PCI domain (default: 0)
 *  BBIS_CFGINFO_IRQ          Device interrupt vector, level and mode of the
 *                             specified device.
 *  BBIS_CFGINFO_EXP          Exception interrupt vector, level and mode of
 *                             the specified device.
 *  BBIS_CFGINFO_SLOT         Information, if the specified device slot is
 *                             occupied or empty, the device id and device
 *                             revision of the plugged device, the name of the
 *                             slot and the name of the plugged device.\endcode
 *
 *  \param brdH      \IN  Bbis handle
 *  \param code      \IN  Reference to the information we need
 *  \param ...       \IN  Argument(s)
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_CfgInfo(
    BBIS_HANDLE     *brdH,
    u_int32         code,
    ... )
{
    va_list		argptr;
    int32       status=ERR_SUCCESS;


    DBGWRT_1((DBH, "BB - %s_CfgInfo (code=0x%04x)\n",VAR_NAME_STR,code));

    va_start(argptr,code);

    switch ( code ) {
        /* bus number */
        case BBIS_CFGINFO_BUSNBR:
        {
            u_int32 *busNbr = va_arg( argptr, u_int32* );
            u_int32 mSlot   = va_arg( argptr, u_int32 );

			if( mSlot >= MAX_DEVS )
				status = ERR_BBIS_ILL_SLOT;
			else
				*busNbr = brdH->busNbr;

            break;
        }
        
        /* pci domain number */
		case BBIS_CFGINFO_PCI_DOMAIN:
		{
			/* domain number */
			u_int32 *domainNbr = va_arg( argptr, u_int32* );
	        u_int32 mSlot      = va_arg( argptr, u_int32 );
	
			*domainNbr = brdH->domainNbr;
			mSlot = mSlot; /* dummy access to avoid compiler warning */
			break;
		}

        /* interrupt information */
		case BBIS_CFGINFO_IRQ:
		{
			u_int32 mSlot   = va_arg( argptr, u_int32 );
			u_int32 *vector = va_arg( argptr, u_int32* );
			u_int32 *level  = va_arg( argptr, u_int32* );
			u_int32 *mode   = va_arg( argptr, u_int32* );

			if( mSlot >= MAX_DEVS ){
				status = ERR_BBIS_ILL_SLOT;
			}
			else {
				*mode = BBIS_IRQ_NONE;
				*level = 0;
				*vector = 0;
			}
			break;
		}

        /* exception interrupt information */
        case BBIS_CFGINFO_EXP:
        {
            u_int32 mSlot   = va_arg( argptr, u_int32 );
            u_int32 *vector = va_arg( argptr, u_int32* );
            u_int32 *level  = va_arg( argptr, u_int32* );
            u_int32 *mode   = va_arg( argptr, u_int32* );

			mSlot = mSlot;		/* dummy access to avoid compiler warning */
			*vector = *vector;	/* dummy access to avoid compiler warning */
			*level = *level;	/* dummy access to avoid compiler warning */
			*mode = 0;			/* no extra exception interrupt */
			break;
        }

		/* slot information for PnP support*/
		case BBIS_CFGINFO_SLOT:
		{
			status = CfgInfoSlot( brdH, argptr );
			break;
		}

        /* error */
        default:
			DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",VAR_NAME_STR,code));
			status = ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** SMBPCI_IrqEnable *****************************/
/** Interrupt enable / disable
 *
 *  Do nothing.
 *
 * \param brdH       \IN  Bbis handle
 * \param mSlot      \IN  Module slot number
 * \param enable     \IN  Interrupt setting
 *
 * \return           \c 0 On success or error code
 */
 static int32 SMBPCI_IrqEnable(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot,
    u_int32         enable )
{
    DBGWRT_1((DBH, "BB - %s_IrqEnable: mSlot=0x%04x enable=%d\n",
		VAR_NAME_STR,mSlot,enable));

	return 0;
}

/****************************** SMBPCI_IrqSrvInit ****************************/
/** Called at the beginning of an interrupt
 *
 *  Do nothing.
 *
 *  \param brdH      \IN  Bbis handle
 *  \param mSlot     \IN  Module slot number
 *
 *  \return          \c 0 BBIS_IRQ_NO | BBIS_IRQ_YES
 */
static int32 SMBPCI_IrqSrvInit(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot)
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvInit: mSlot=0x%04x\n",VAR_NAME_STR,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** SMBPCI_IrqSrvExit ****************************/
/** Called at the end of an interrupt
 *
 *  Do nothing.
 *
 *  \param brdH      \IN  Bbis handle
 *  \param mSlot     \IN  Module slot number
 */
static void SMBPCI_IrqSrvExit(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvExit: mSlot=0x%04x\n",VAR_NAME_STR,mSlot));
}

/****************************** SMBPCI_ExpEnable *****************************/
/** Exception interrupt enable / disable
 *
 *  Do nothing.
 *
 *  \param brdH      \IN  Bbis handle
 *  \param mSlot     \IN  Module slot number
 *  \param enable    \IN  Interrupt setting
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_ExpEnable(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot,
	u_int32			enable)
{
	IDBGWRT_1((DBH, "BB - %s_ExpEnable: mSlot=0x%04x\n",VAR_NAME_STR,mSlot));

	return 0;
}

/****************************** SMBPCI_ExpSrv ********************************/
/** Called at the beginning of an exception interrupt
 *
 *  Do nothing.
 *
 *  \param brdH      \IN  Bbis handle
 *  \param mSlot     \IN  Module slot number
 *
 *  \return          \c BBIS_IRQ_NO
 */
static int32 SMBPCI_ExpSrv(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_ExpSrv: mSlot=0x%04x\n",VAR_NAME_STR,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** SMBPCI_SetMIface *****************************/
/** Set device interface
 *
 *  Do nothing.
 *
 *  \param brdH      \IN  Bbis handle
 *  \param mSlot     \IN  Module slot number
 *  \param addrMode  \IN  MDIS_MODE_A08 | MDIS_MODE_A24
 *  \param dataMode  \IN  MDIS_MODE_SMB6 | MDIS_MODE_D32
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_SetMIface(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode)
{
	DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=0x%04x\n",VAR_NAME_STR,mSlot));

    return 0;
}

/****************************** SMBPCI_ClrMIface *****************************/
/** Clear device interface
 *
 *  Do nothing.
 *
 *  \param brdH      \IN  Bbis handle
 *  \param mSlot     \IN  Module slot number
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_ClrMIface(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot)
{
	DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=0x%04x\n",VAR_NAME_STR,mSlot));

    return 0;
}

/****************************** SMBPCI_GetMAddr ******************************/
/** Get physical address description
 *
 *  - check module slot number
 *
 *  \param brdH      \IN  Bbis handle
 *  \param mSlot     \IN  Module slot number
 *  \param addrMode  \IN  MDIS_MA08 | MDIS_MA24
 *  \param dataMode  \IN  MDIS_MD16 | MDIS_MD32
 *  \param mAddr     \IN  Pointer to address space
 *  \param mSize     \IN  Size of address space
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_GetMAddr(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode,
    void            **mAddr,
    u_int32         *mSize )
{
	DBGWRT_1((DBH, "BB - %s_GetMAddr: mSlot=0x%04x\n",VAR_NAME_STR,mSlot));

	if ( mSlot >= MAX_DEVS )
		return ERR_BBIS_ILL_SLOT;

    return 0;
}

/****************************** SMBPCI_SetStat *******************************/
/** Set driver status
 *
 *  Following status codes are supported:
 *
 *  \code
 *  Code                 Description                Values
 *  -------------------  -------------------------  ----------
 *  M_BB_DEBUG_LEVEL     board debug level          see dbg.h\endcode
 *
 *  \param brdH				\IN  Bbis handle
 *  \param mSlot			\IN  Module slot number
 *  \param code				\IN  Setstat code
 *  \param value32_or_64	\IN  Setstat value or ptr to blocksetstat data
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_SetStat(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     value32_or_64 )
{
    int32 value = (int32)value32_or_64; /* 32bit value */

	DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=0x%04x code=0x%04x value=0x%x\n",
			  VAR_NAME_STR, mSlot, code, value));

    switch (code) {

		/* set debug level */
        case M_BB_DEBUG_LEVEL:
            brdH->dbgLev = value;
            break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** SMBPCI_GetStat *******************************/
/** Get driver status
 *
 *  Following status codes are supported:
 *
 *  \code
 *  Code                 Description                Values
 *  -------------------  -------------------------  ----------
 *  M_BB_DEBUG_LEVEL     driver debug level         see dbg.h
 *  M_BB_ID_CHECK        board id is checked        0..1
 *  M_BB_GET_SMBHDL      get SMB2 handle			void*
 *  M_MK_BLK_REV_ID      ident function table ptr   -\endcode
 *
 *  \param brdH				\IN  Bbis handle
 *  \param mSlot			\IN  Module slot number
 *  \param code				\IN  Getstat code
 *  \param value32_or_64P	\OUT Getstat value or ptr to blockgetstat data
 *
 *  \return          \c 0 On success or error code
 */
static int32 SMBPCI_GetStat(
    BBIS_HANDLE     *brdH,
    u_int32         mSlot,
    int32           code,
    INT32_OR_64     *value32_or_64P )
{
	int32 *valueP = (int32*)value32_or_64P;	/* pointer to 32bit value */

	DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=0x%04x code=0x%04x\n",
		VAR_NAME_STR,mSlot,code));

    switch (code) {
        /* get debug level */
        case M_BB_DEBUG_LEVEL:
            *valueP = brdH->dbgLev;
            break;

/* SMB ctrl at PCIbus */
#ifndef VAR_ISA
        /* get board id check state */
        case M_BB_ID_CHECK:
			*valueP = brdH->idCheck;
			break;
#endif

		/* get SMB2 handle */
        case M_BB_GET_SMBHDL:
			*value32_or_64P = (INT32_OR_64)brdH->smbH;
			break;

        /* ident table */
        case M_MK_BLK_REV_ID:
			*value32_or_64P = (INT32_OR_64)&brdH->idFuncTbl;
			break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** SMBPCI_Unused ********************************/
/** Dummy function for unused jump table entries
 *
 *  \return          \c ERR_BBIS_ILL_FUNC
 */
static int32 SMBPCI_Unused( void )
{
    return ERR_BBIS_ILL_FUNC;
}

/****************************** Ident ***************************************/
/** Return ident string
 *
 *  \return          \c pointer to ident string
 */
static char* Ident( void )
{
	return( VAR_NAME_STR " Base Board Handler: " RCSID );
}

/****************************** Cleanup *************************************/
/** Close all handles, unmap addresses, free memory
 *
 *	NOTE: The brdH handle is invalid after calling this function.
 *
 *  \param brdH      \IN  Bbis handle
 *  \param retCode   \IN  Code to return
 *
 *  \return          \c 0 On success or error code
 */
static int32 Cleanup(
   BBIS_HANDLE  *brdH,
   int32        retCode
)
{
    /*------------------------------+
    |  exit SMB library             |
    +------------------------------*/
	if( brdH->smbH )
	{
        if( !brdH->alreadyInitialized )
        {
    		((SMB_HANDLE*)brdH->smbH)->Exit( &brdH->smbH );
	        OSS_SetSmbHdl( brdH->osH, brdH->smbBusNbr, NULL); /* remove from OSS smb table */
    	}
    }

	/*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if (brdH->descH)
		DESC_Exit(&brdH->descH);

	/* cleanup debug */
	DBGEXIT((&DBH));

    /*------------------------------+
    |  unmap addresses              |
    +------------------------------*/
    /* unmap SMB control register space */
	if( brdH->virtCtrlBase )
		OSS_UnMapVirtAddr( brdH->osH, &brdH->virtCtrlBase,
						   brdH->res[RES_CTRL_ADDR].u.mem.size, brdH->smbAddrSpaceType );

    /*------------------------------+
    |  unassign resources           |
    +------------------------------*/
#ifdef OSS_HAS_UNASSIGN_RESOURCES
	if( brdH->resourcesAssigned ){
		/* unassign the resources */
		OSS_UnAssignResources( brdH->osH, BUSTYPE, 
		                       OSS_MERGE_BUS_DOMAIN(brdH->busNbr, brdH->domainNbr),
							   RESOURCE_NBR, brdH->res );
	}
#endif

    /*------------------------------+
    |  free memory                  |
    +------------------------------*/
    /* release memory for the board handle */
    OSS_MemFree( brdH->osH, (int8*)brdH, brdH->ownMemSize);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}

/* SMB ctrl at PCIbus */
#ifndef VAR_ISA
/****************************** ParsePciPath ********************************/
/** Parses the specified PCI_BUS_PATH to find out PCI Bus Number
 *
 *  \param brdH        \IN  Bbis handle
 *  \param pciBusNbrP  \OUT PCI bus number
 *  \param retCode     \IN  Code to return
 *
 *  \return            \c 0 On success or error code
 */
static int32 ParsePciPath( BBIS_HANDLE *brdH, u_int32 *pciBusNbrP ) 	/* nodoc */
{
	u_int32 i;
	int32 pciBusNbr=0, pciDevNbr;
	int32 error;
	int32 vendorID, deviceID, headerType, secondBus;

	/* parse whole pci path until our device is reached */
	for(i=0; i<brdH->pciPathLen; i++){

		pciDevNbr = brdH->pciPath[i];
		
		if ( ( i==0 ) && ( 0 != brdH->domainNbr ) ) {
			/* as we do not know the numbering order of busses on pci domains, 
			   try to find the device on all busses instead of looking for the 
			   first bus on the domain                                        */
			for(pciBusNbr=0; pciBusNbr<0xff; pciBusNbr++) {
				error = PciParseDev( brdH, OSS_MERGE_BUS_DOMAIN(pciBusNbr, brdH->domainNbr), 
				                     brdH->pciPath[0], &vendorID, &deviceID, &headerType,
								     &secondBus );
				if ( error == ERR_SUCCESS ) 
					break; /* found device */
			}
			
			if ( error != ERR_SUCCESS ) { /* device not found */
				DBGWRT_ERR((DBH,"*** BB - %s: first device 0x%02x in pci bus path "
				                "not found on domain %d!\n",
				                VAR_NAME_STR, brdH->pciPath[0], brdH->domainNbr ));
				return error;               
			}    
		} else {
			/* parse device only once */
			if( (error = PciParseDev( brdH, OSS_MERGE_BUS_DOMAIN(pciBusNbr, brdH->domainNbr), 
				                      pciDevNbr, &vendorID, &deviceID, &headerType,
									  &secondBus )))
				return error;
		}

		if( vendorID == 0xffff && deviceID == 0xffff ){
			DBGWRT_ERR((DBH,"*** BB - %s:ParsePciPath: Nonexistant device "
						"domain %d bus %d dev %d\n", VAR_NAME_STR, brdH->domainNbr, 
						pciBusNbr, pciDevNbr ));
			return ERR_BBIS_NO_CHECKLOC;
		}

		/*--- device is present, is it a bridge ? ---*/
		if( (headerType & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE ){
			DBGWRT_ERR((DBH,"*** BB - %s:ParsePciPath: Device is not a bridge!"
						"domain %d bus %d dev %d vend=0x%x devId=0x%x\n",
						VAR_NAME_STR, brdH->domainNbr, pciBusNbr, pciDevNbr, vendorID,
						deviceID ));

			return ERR_BBIS_NO_CHECKLOC;
		}

		/*--- it is a bridge, determine its secondary bus number ---*/
		DBGWRT_2((DBH, " domain %d bus %d dev 0x%x: vend=0x%x devId=0x%x second bus %d\n",
				  brdH->domainNbr, pciBusNbr, pciDevNbr, vendorID, deviceID, secondBus ));

		/*--- continue with new bus ---*/
		pciBusNbr = secondBus;
	}

	DBGWRT_1((DBH,"BB - %s: Main PCI Bus Number is %d\n", VAR_NAME_STR,
			  pciBusNbr ));

	*pciBusNbrP = pciBusNbr;

	return ERR_SUCCESS;
}

/****************************** PciParseDev *********************************/
/** Get parameters from specified PCI device's config space
 *
 *  \param brdH        \IN  Bbis handle
 *  \param pciBusNbr   \IN  Pci bus number
 *  \param pciDevNbr   \IN  Pci dev number
 *  \param vendorIDP   \OUT  Vendor id
 *  \param deviceIDP   \OUT  Device id
 *  \param headerTypeP \OUT  Header type
 *  \param secondBusP  \OUT  Secondary bus number (only valid for bridge)
 *
 *  \return            \c 0 On success or error code (only fails if config access error)
 */
static int32 PciParseDev(
	BBIS_HANDLE *brdH,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	int32 *vendorIDP,
	int32 *deviceIDP,
	int32 *headerTypeP,
	int32 *secondBusP)		/* nodoc */
{
	int32 error;

	u_int32 pciMainDevNbr;
	u_int32 pciDevFunc;

	pciMainDevNbr = pciDevNbr;
	pciDevFunc = 0;

	if (pciDevNbr > 0x1f)
	{
		// seperate the function number from the device number
		pciDevFunc = pciDevNbr >> 5;
		pciMainDevNbr = (pciDevNbr & 0x0000001f);
	}

	/*--- check to see if device present ---*/
	error = OSS_PciGetConfig( brdH->osH, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  OSS_PCI_VENDOR_ID, vendorIDP );

	if( error == 0 )
		error = OSS_PciGetConfig( brdH->osH, pciBusNbr, pciMainDevNbr, pciDevFunc,
									  OSS_PCI_DEVICE_ID, deviceIDP );

	if( error )
		return PciCfgErr(brdH,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,OSS_PCI_DEVICE_ID);

	if( *vendorIDP == 0xffff && *deviceIDP == 0xffff )
		return ERR_SUCCESS;		/* not present */

	/*--- device is present, is it a bridge ? ---*/
	error = OSS_PciGetConfig( brdH->osH, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  OSS_PCI_HEADER_TYPE, headerTypeP );

	if( error )
		return PciCfgErr(brdH,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,OSS_PCI_HEADER_TYPE);

	DBGWRT_2((DBH, " domain %d bus %d dev %d.%d: vend=0x%x devId=0x%x hdrtype %d\n",
			  OSS_DOMAIN_NBR( pciBusNbr ), OSS_BUS_NBR( pciBusNbr ), pciMainDevNbr, 
			  pciDevFunc, *vendorIDP, *deviceIDP, *headerTypeP ));

	if( ( *headerTypeP & ~OSS_PCI_HEADERTYPE_MULTIFUNCTION) != OSS_PCI_HEADERTYPE_BRIDGE_TYPE )
		return ERR_SUCCESS;		/* not bridge device */


	/*--- it is a bridge, determine its secondary bus number ---*/
	error = OSS_PciGetConfig( brdH->osH, pciBusNbr, pciMainDevNbr, pciDevFunc,
							  PCI_SECONDARY_BUS_NUMBER | OSS_PCI_ACCESS_8,
							  secondBusP );

	if( error )
		return PciCfgErr(brdH,"PciParseDev", error,
						 pciBusNbr,pciDevNbr,
						 PCI_SECONDARY_BUS_NUMBER | OSS_PCI_ACCESS_8);

	return ERR_SUCCESS;
}

/****************************** PciCfgErr ***********************************/
/** Print Debug message
 *
 *  \param brdH        \IN  Bbis handle
 *  \param funcName    \IN  Function name
 *  \param error       \IN  Error code
 *  \param pciBusNbr   \IN  Pci bus number
 *  \param pciDevNbr   \IN  Pci dev number
 *  \param reg	       \IN  Register
 *
 *  \return            \c 0 On success or error code
 */

static int32 PciCfgErr(
	BBIS_HANDLE *brdH,
	char *funcName,
	int32 error,
	u_int32 pciBusNbr,
	u_int32 pciDevNbr,
	u_int32 reg )		/* nodoc */
{
	u_int32 pciMainDevNbr = pciDevNbr;
	u_int32 pciDevFunc = 0;

	if (pciDevNbr > 0x1f)
	{
		// device number contains function in upper 3 bit
		pciDevFunc = pciDevNbr >> 5;  // devNbr e.g. 0b 0101 1110
		pciMainDevNbr = pciDevNbr & 0x0000001f;
	}

	DBGWRT_ERR((DBH,"*** BB - %s %s: PCI access error 0x%x "
				"domain %d bus %d dev %d.%d reg 0x%x\n", VAR_NAME_STR, funcName, error,
				OSS_DOMAIN_NBR( pciBusNbr ), OSS_BUS_NBR( pciBusNbr ), pciMainDevNbr, 
				pciDevFunc, reg ));
	return error;
}
#endif /* SMB ctrl at PCIbus */

/****************************** CfgInfoSlot ********************************/
/** Fulfils the BB_CfgInfo(BBIS_CFGINFO_SLOT) request
 *
 *	The variable-argument list (argptr) contains the following parameters in
 *  the given order:
 *
 *  \code
 *  Input
 *  -----
 *  mSlot (u_int32) - device slot number
 *
 *  Output
 *  ------
 *  occupied (u_int32*) - occupied information
 *   - pluggable device:
 *     BBIS_SLOT_OCCUP_YES if slot is occupied
 *     or BBIS_SLOT_OCCUP_NO if slot is empty
 *   - onboard device:
 *     BBIS_SLOT_OCCUP_ALW if device is enabled
 *     or BBIS_SLOT_OCCUP_DIS if device is disabled
 *
 *  devId (u_int32*) - device id (4-byte hex value)
 *    The device id should identify the type of the device but should not
 *    contain enough information to differentiate between two identical
 *    devices. If the device id is unknown BBIS_SLOT_NBR_UNK must be returned.
 *    - M-Module:
 *      id-prom-magic-word << 16 | id-prom-module-id
 *      Example: 0x53460024
 *      Note: The returned device id must be identical to the "autoid" value
 *            in the device drivers xml file.
 *
 *  devRev (u_int32*) - device revision (4-byte hex value)
 *    - M-Module: id-prom-layout-revision << 16 |
 *                id-prom-product-variant
 *                example: 0x01091400
 *      or BBIS_SLOT_NBR_UNK if device revision is unknown
 *
 *  slotName (char*) - slot name
 *    The slot name should consist of the slot type and the slot label but must
 *    not contain any non-printing characters. The length of the returned
 *    string, including the terminating null character, must not exceed
 *    BBIS_SLOT_STR_MAXSIZE.
 *    format : "<slot type> <slot label>"
 *
 *    Examples:
 *    - M-Module:		"M-Module slot 0"
 *    - Onboard-Dev:	"onboard slot 0x1000"
 *
 *  devName (char*) - device name
 *    The device name should identify the type of the device but should not
 *    contain enough information to differentiate between two identical
 *    devices. Furthermore, the device name should refer to the appropriate
 *    device driver name if possible.
 *
 *    The returned string must not contain any non-printing characters or
 *    blanks. The length of the returned string, including the terminating null
 *    character, must not exceed BBIS_SLOT_STR_MAXSIZE.
 *
 *    Examples:
 *    - M-Module:		"M34", "MS9"
 *    - Onboard-Dev:	"SMBPCI_TRIG"
 *
 *    If the device name is unknown BBIS_SLOT_STR_UNK must be returned.
 *
 *    Note: The returned device name must be identical to the "hwname" value in
 *    the device drivers xml file.\endcode
 *
 *  \param brdH      \IN  Bbis handle
 *  \param argptr    \IN  Argument(s)
 *
 *  \return          \c 0 On success or error code
 */
static int32 CfgInfoSlot( BBIS_HANDLE *brdH, va_list argptr )
{
	u_int32 mSlot     = va_arg( argptr, u_int32 );
    u_int32 *occupied = va_arg( argptr, u_int32* );
    u_int32 *devId    = va_arg( argptr, u_int32* );
    u_int32 *devRev   = va_arg( argptr, u_int32* );
	char	*slotName = va_arg( argptr, char* );
	char	*devName  = va_arg( argptr, char* );

	/* default parameters to return */
	*occupied = BBIS_SLOT_OCCUP_DIS;
	*devId    = BBIS_SLOT_NBR_UNK;
	*devRev   = BBIS_SLOT_NBR_UNK;
	*slotName = '\0';
	*devName  = '\0';

	/* illegal slot? */
	if( (mSlot >= MAX_DEVS) ||
		 (*(brdH->smb[mSlot].name) == '\0' )){
		/*
		 * no debug print here because it will be called under Windows
		 * with mSlot=0x00..0x0f and 0x1000..0x100f
		 */
		return ERR_BBIS_ILL_SLOT;
	}

	*occupied = BBIS_SLOT_OCCUP_ALW;

	OSS_Sprintf( brdH->osH, slotName, "smb-slot %d", mSlot);
	OSS_StrCpy( brdH->osH, brdH->smb[mSlot].name, devName );

    DBGWRT_2((DBH," slotName=%s, devName=%s\n", slotName, devName));

	/* return on success */
	return ERR_SUCCESS;
}












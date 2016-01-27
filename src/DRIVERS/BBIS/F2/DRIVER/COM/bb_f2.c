/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_f2.c
 *      Project: F2 board handler
 *
 *       Author: ds
 *        $Date: 2006/12/20 12:43:20 $
 *    $Revision: 1.5 $
 *
 *  Description: F2 board handler routines
 *
 *               The F2 supports several onboard-devices:
 *               - The onboard-devices corresponds to ISA or PCI devices.
 *               - The onboard-device uses the following resources:
 *
 *               slot   | device (function)          | bus | resources
 *               -------+----------------------------+-----+--------------
 *               0x1000 | Z8536                      |     |
 *						|  (Watchdog, SMB-Ctr, Trig) | ISA | io space, IRQ
 *               0x1000 | LM79                       |     |
 *               OR     |  (Systemmonitor)           | ISA | IRQ
 *               0x1002 |                            |     |
 *                      |                            |     |
 *               0x1001 | ALI M1543C                 |     |
 *						|  (User-LEDs, GPIOs)        | PCI | cfg space
 *
 *               Note:
 *               The F2 board handler also access the FDC37C669 ISA device
 *               to do some interrupt configurations.
 *
 *     Required: ---
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_, F2
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_f2.c,v $
 * Revision 1.5  2006/12/20 12:43:20  ufranke
 * fixed
 *  - function pointer setIrqHandle should be NULL if not implemented
 *    or must be implemented completely
 *
 * Revision 1.4  2006/12/15 16:18:28  ts
 * replaced BBIS fkt13 with setIrqHandle
 *
 * Revision 1.3  2001/09/18 15:24:26  kp
 * 1) Added an alternative slot number for LM78 (0x1002). Old number 0x1000
 * can still be used.
 * 2) Added call to OSS_UnAssignResources
 *
 * Revision 1.2  2000/12/20 10:04:40  Schmidt
 * F2_GetStat: general check of mSlot range was wrong
 *
 * Revision 1.1  2000/06/09 14:09:49  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: bb_f2.c,v 1.5 2006/12/20 12:43:20 ufranke Exp $";

#define _NO_BBIS_HANDLE		/* bb_defs.h: don't define BBIS_HANDLE struct */

#include <MEN/mdis_com.h>
#include <MEN/men_typs.h>   /* system dependend definitions   */
#include <MEN/dbg.h>        /* debug functions                */
#include <MEN/oss.h>        /* oss functions                  */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/bb_defs.h>    /* bbis definitions				  */
#include <MEN/mdis_err.h>   /* MDIS error codes               */
#include <MEN/mdis_api.h>   /* MDIS global defs               */

#if F2
#	include "f2_int.h"		/* F2 specific defines */ 
#endif

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL		brdHdl->dbgLev
#define DBH             brdHdl->dbgHdl
#define BIOS_DEFAULT	0xff

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
typedef struct {
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/* id function table			*/
    u_int32     ownMemSize;			/* own memory size				*/
    OSS_HANDLE* osHdl;				/* os specific handle			*/
    DESC_HANDLE *descHdl;			/* descriptor handle pointer	*/
    int32       mechSlot;			/* mechanical slot              */
    u_int32     dbgLev;				/* debug level for BBIS         */
	DBG_HANDLE  *dbgHdl;			/* debug handle					*/
	MACCESS		fdcAddr;			/* FDC37C669 virtual address	*/
	u_int32		irqLev[3];			/* IRQ levels (IRQIN/COM3/COM4) */
	u_int8		fdcIrqMask[3];		/* FDC37C669 IRQ masks			*/
	u_int8		resourcesAssigned; 	/* resources assigned		    */
} BBIS_HANDLE;

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>	/* bbis jumptable		*/
#include <MEN/bb_f2.h>		/* F2 bbis header file	*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* init/exit */
static int32 F2_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 F2_BrdInit(BBIS_HANDLE*);
static int32 F2_BrdExit(BBIS_HANDLE*);
static int32 F2_Exit(BBIS_HANDLE**);
/* info */
static int32 F2_BrdInfo(u_int32, ...);
static int32 F2_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 F2_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 F2_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  F2_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 F2_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 F2_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 F2_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 F2_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 F2_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 F2_SetStat(BBIS_HANDLE*, u_int32, int32, int32);
static int32 F2_GetStat(BBIS_HANDLE*, u_int32, int32, int32*);
/* unused */
static int32 F2_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);


/**************************** F2_GetEntry ***********************************
 *
 *  Description:  Initialize drivers jump table.
 *
 *---------------------------------------------------------------------------
 *  Input......:  bbisP     pointer to the not initialized structure
 *  Output.....:  *bbisP    initialized structure
 *  Globals....:  ---
 ****************************************************************************/
#ifdef _ONE_NAMESPACE_PER_DRIVER_
	extern void BBIS_GetEntry( BBIS_ENTRY *bbisP )
#else
# ifdef F2 
	extern void F2_GetEntry( BBIS_ENTRY *bbisP )
# endif
#endif
{
    /* init/exit */
    bbisP->init         =   F2_Init;
    bbisP->brdInit      =   F2_BrdInit;
    bbisP->brdExit      =   F2_BrdExit;
    bbisP->exit         =   F2_Exit;
    bbisP->fkt04        =   F2_Unused;
    /* info */
    bbisP->brdInfo      =   F2_BrdInfo;
    bbisP->cfgInfo      =   F2_CfgInfo;
    bbisP->fkt07        =   F2_Unused;
    bbisP->fkt08        =   F2_Unused;
    bbisP->fkt09        =   F2_Unused;
    /* interrupt handling */
    bbisP->irqEnable    =   F2_IrqEnable;
    bbisP->irqSrvInit   =   F2_IrqSrvInit;
    bbisP->irqSrvExit   =   F2_IrqSrvExit;
    bbisP->setIrqHandle =   NULL;
    bbisP->fkt14        =   F2_Unused;
    /* exception handling */
    bbisP->expEnable    =   F2_ExpEnable;
    bbisP->expSrv       =   F2_ExpSrv;
    bbisP->fkt17        =   F2_Unused;
    bbisP->fkt18        =   F2_Unused;
    bbisP->fkt19        =   F2_Unused;
    /* */
    bbisP->fkt20        =   F2_Unused;
    bbisP->fkt21        =   F2_Unused;
    bbisP->fkt22        =   F2_Unused;
    bbisP->fkt23        =   F2_Unused;
    bbisP->fkt24        =   F2_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   F2_SetStat;
    bbisP->getStat      =   F2_GetStat;
    bbisP->setMIface    =   F2_SetMIface;
    bbisP->clrMIface    =   F2_ClrMIface;
    bbisP->getMAddr     =   F2_GetMAddr;
    bbisP->fkt30        =   F2_Unused;
    bbisP->fkt31        =   F2_Unused;
}

/****************************** F2_Init *************************************
 *
 *  Description:  Allocate and return board handle.
 *
 *                - initializes the board handle
 *                - reads and saves board descriptor entries
 *                - assign resources
 *                - map used addresses
 *
 *                The following descriptor keys are used:
 *
 *                Deskriptor key           Default          Range
 *                -----------------------  ---------------  -------------
 *                DEBUG_LEVEL_DESC         OSS_DBG_DEFAULT  see dbg.h
 *                DEBUG_LEVEL              OSS_DBG_DEFAULT  see dbg.h
 *                IRQ_CIO_LM			   BIOS setting     0,5,7,9,10,11,12
 *                IRQ_COM3				   BIOS setting     0,5,7,9,10,11,12
 *                IRQ_COM4				   BIOS setting     0,5,7,9,10,11,12
 *
 *                IRQ_.. specifies optionally the IRQ that will be used for
 *                the Z8536/LM79 device, COM3 port and COM4 port. If IRQ_..
 *                is present, the IRQ setting from the BIOS will be
 *                overwritten - otherwise the BIOS mapping will be used. 
 *                If IRQ_..=0 no IRQ will be assigned.
 *
 *                Note: The current F2 BIOS doesn't assign any IRQ to the
 *                      Z8536/LM79 device. Therefor IRQ_CIO_LM must be
 *                      specified to provide an IRQ for the device slot 0x1000.
 *
 *                      Furthermore, the current BIOS doesn't assign the proper
 *                      IRQ for COM3/COM4 specified in the BIOS setup. Therefore
 *                      it is recommended to specify IRQ_COM3/4 to select the
 *                      proper IRQs.
 *
 *---------------------------------------------------------------------------
 *  Input......:  osHdl     pointer to os specific structure             
 *                descSpec  pointer to os specific descriptor specifier  
 *                brdHdlP   pointer to not initialized board handle structure            
 *  Output.....:  *brdHdlP  initialized board handle structure  
 *				  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_Init(
    OSS_HANDLE      *osHdl,
    DESC_SPEC       *descSpec,
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE		*brdHdl = NULL;
	u_int32			gotsize;
    int32			status;
    u_int32			value;
    OSS_RESOURCES	res;
	u_int8			i;

    /*-------------------------------+
    | initialize the board structure |
    +-------------------------------*/
    /* get memory for the board structure */
    *brdHdlP = brdHdl = (BBIS_HANDLE*) (OSS_MemGet(
        osHdl, sizeof(BBIS_HANDLE), &gotsize ));
    if ( brdHdl == NULL )
        return ERR_OSS_MEM_ALLOC;

    /* store data into the board structure */
    brdHdl->ownMemSize = gotsize;
    brdHdl->osHdl = osHdl;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* drivers ident function */
	brdHdl->idFuncTbl.idCall[0].identCall = Ident;
	/* libraries ident functions */
	brdHdl->idFuncTbl.idCall[1].identCall = DESC_Ident;
	brdHdl->idFuncTbl.idCall[2].identCall = OSS_Ident;
	/* terminator */
	brdHdl->idFuncTbl.idCall[3].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    DBGWRT_1((DBH,"BB - %s_Init\n",BBNAME));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
    /* init descHdl */
    status = DESC_Init( descSpec, osHdl, &brdHdl->descHdl );
    if (status)
		return( Cleanup(brdHdl,status) );

    /* get DEBUG_LEVEL_DESC */
    status = DESC_GetUInt32(brdHdl->descHdl, OSS_DBG_DEFAULT, &value,
				"DEBUG_LEVEL_DESC");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

	/* set debug level for DESC module */
	DESC_DbgLevelSet(brdHdl->descHdl, value);

    /* get DEBUG_LEVEL */
    status = DESC_GetUInt32( brdHdl->descHdl, OSS_DBG_DEFAULT, &(brdHdl->dbgLev),
                "DEBUG_LEVEL");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    /* get IRQ_CIO_LM */
    status = DESC_GetUInt32( brdHdl->descHdl, BIOS_DEFAULT, &(brdHdl->irqLev[0]),
                "IRQ_CIO_LM");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    /* get IRQ_COM3 */
    status = DESC_GetUInt32( brdHdl->descHdl, BIOS_DEFAULT, &(brdHdl->irqLev[1]),
                "IRQ_COM3");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

    /* get IRQ_COM4 */
    status = DESC_GetUInt32( brdHdl->descHdl, BIOS_DEFAULT, &(brdHdl->irqLev[2]),
                "IRQ_COM4");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );

	/* check IRQ levels, build IRQ mask for FDC */
	for( i=0; i<3; i++ ){
		switch( brdHdl->irqLev[i] ){
		/* no IRQ */
		case  0: brdHdl->fdcIrqMask[i] = 0x0; break;
		/* IRQ 5,7,9,10,11,12 */
		case  5: brdHdl->fdcIrqMask[i] = 0x1; break;
		case  7: brdHdl->fdcIrqMask[i] = 0x3; break;
		case  9: brdHdl->fdcIrqMask[i] = 0x4; break;
		case 10: brdHdl->fdcIrqMask[i] = 0x5; break;
		case 11: brdHdl->fdcIrqMask[i] = 0x6; break;
		case 12: brdHdl->fdcIrqMask[i] = 0x8; break;
		/* use BIOS configuration */
		case BIOS_DEFAULT: break;
		default:
			/* illegal IRQ level */
			return( Cleanup(brdHdl,ERR_LL_DESC_PARAM) );
		}
	}

    /* exit descHdl */
    status = DESC_Exit( &brdHdl->descHdl );
    if (status)
        return( Cleanup(brdHdl,status) );

    /*------------------------------+
    | assign resources              |
    +------------------------------*/
    res.type = OSS_RES_IO;
    res.u.mem.physAddr = BRD_FDC_IO_ADDR;
    res.u.mem.size = BRD_FDC_IO_SIZE;

    if( (status = OSS_AssignResources( osHdl, OSS_BUSTYPE_ISA, BRD_FDC_BUSNBR,
                                       1, &res )) )
		return( Cleanup(brdHdl,status) );

	brdHdl->resourcesAssigned = TRUE;

    /*------------------------------+
    | map used addresses            |
    +------------------------------*/
    /* board runtime registers */
    if( (status = OSS_MapPhysToVirtAddr( osHdl, BRD_FDC_IO_ADDR, BRD_FDC_IO_SIZE,
										 OSS_ADDRSPACE_IO, OSS_BUSTYPE_ISA,
										 BRD_FDC_BUSNBR, 
										 (void *)&brdHdl->fdcAddr )) )
		return( Cleanup(brdHdl,status) );

    return 0;
}

/****************************** F2_BrdInit **********************************
 *
 *  Description:  Board initialization.
 *
 *                Perform IRQ setting in the FDC37C669 ISA device. 
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure    
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_BrdInit(
    BBIS_HANDLE     *brdHdl )
{
	u_int8	regVal;

	DBGWRT_1((DBH, "BB - %s_BrdInit\n",BBNAME));

	/* enter configuration mode */
	MWRITE_D8( brdHdl->fdcAddr, 0, 0x55 );
	MWRITE_D8( brdHdl->fdcAddr, 0, 0x55 );

    /*------------------------------+
    | CIO/LM IRQ                    |
    +------------------------------*/
	/* select configuration register CR29 */
	MWRITE_D8( brdHdl->fdcAddr, 0, 0x29);
	
	/* read current IRQ-Mask */
	regVal = MREAD_D8( brdHdl->fdcAddr, 1 );
	
	/* set IRQ (bits 3:0) */
	if( brdHdl->irqLev[0] != BIOS_DEFAULT ){
		MWRITE_D8( brdHdl->fdcAddr, 1, (regVal & 0xf0)
				   | (brdHdl->fdcIrqMask[0] & 0x0f) );
	}
	/* determine IRQ */
	else{
		switch( regVal & 0x0f ){
		case 0x0: brdHdl->irqLev[0] =  0; break;
		case 0x1: brdHdl->irqLev[0] =  5; break;
		case 0x3: brdHdl->irqLev[0] =  7; break;
		case 0x4: brdHdl->irqLev[0] =  9; break;
		case 0x5: brdHdl->irqLev[0] = 10; break;
		case 0x6: brdHdl->irqLev[0] = 11; break;
		case 0x8: brdHdl->irqLev[0] = 12; break;
		default:
			/* illegal IRQ level */
			return( ERR_LL_DEV_NOTRDY );
		}
	}

    /*------------------------------+
    | COM3 IRQ                      |
    +------------------------------*/
	if( brdHdl->irqLev[1] != BIOS_DEFAULT ){
		/* select configuration register CR28 */
		MWRITE_D8( brdHdl->fdcAddr, 0, 0x28);
		/* select IRQ (bits 7:4) */
		regVal = MREAD_D8( brdHdl->fdcAddr, 1 );
		MWRITE_D8( brdHdl->fdcAddr, 1, (regVal & 0x0f)
				   | ((brdHdl->fdcIrqMask[1]<<4) & 0xf0) );
	}

    /*------------------------------+
    | COM4 IRQ                      |
    +------------------------------*/
	if( brdHdl->irqLev[2] != BIOS_DEFAULT ){
		/* select configuration register CR28 */
		MWRITE_D8( brdHdl->fdcAddr, 0, 0x28);
		/* select IRQ (bits 3:0) */
		regVal = MREAD_D8( brdHdl->fdcAddr, 1 );
		MWRITE_D8( brdHdl->fdcAddr, 1, (regVal & 0xf0)
				   | (brdHdl->fdcIrqMask[2] & 0x0f) );
	}

	/* exit configuration mode */
	MWRITE_D8( brdHdl->fdcAddr, 0, 0xaa );

	return 0;
}

/****************************** F2_BrdExit **********************************
 *
 *  Description:  Board deinitialization.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_BrdExit(
    BBIS_HANDLE     *brdHdl )
{
	DBGWRT_1((DBH, "BB - %s_BrdExit\n",BBNAME));

    return 0;
}

/****************************** F2_Exit *************************************
 *
 *  Description:  Cleanup memory.
 *
 *                - deinitializes the bbis handle
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdlP   pointer to board handle structure
 *  Output.....:  *brdHdlP  NULL
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_Exit(
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE	*brdHdl = *brdHdlP;
	int32		error = 0;

    DBGWRT_1((DBH, "BB - %s_Exit\n",BBNAME));

    /*------------------------------+
    |  de-init hardware             |
    +------------------------------*/
	/* nothing to do */

    /*------------------------------+
    |  cleanup memory               |
    +------------------------------*/
	error = Cleanup(brdHdl, error);
    *brdHdlP = NULL;

    return error;
}

/****************************** F2_BrdInfo **********************************
 *
 *  Description:  Get information about hardware and driver requirements.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  -----------------------------
 *                BBIS_BRDINFO_BUSTYPE      board bustype
 *                BBIS_BRDINFO_DEVBUSTYPE   device bustype
 *                BBIS_BRDINFO_FUNCTION     used optional functions
 *                BBIS_BRDINFO_NUM_SLOTS    number of slots
 *				  BBIS_BRDINFO_INTERRUPTS   interrupt characteristics
 *                BBIS_BRDINFO_ADDRSPACE    address characteristic
 *
 *                The BBIS_BRDINFO_BUSTYPE code returns the bustype of
 *                the specified board.
 *
 *                The BBIS_BRDINFO_DEVBUSTYPE code returns the bustype of
 *                the specified device - not the board bus type. 
 * 				  
 *                The BBIS_BRDINFO_FUNCTION code returns the information
 *                if an optional BBIS function is supported or not.
 *
 *                The BBIS_BRDINFO_NUM_SLOTS code returns the number of
 *                devices used from the driver.
 *
 *                The BBIS_BRDINFO_INTERRUPTS code returns the supported
 *                interrupt capability (BBIS_IRQ_DEVIRQ/BBIS_IRQ_EXPIRQ)
 *                of the specified device.
 *
 *                The BBIS_BRDINFO_ADDRSPACE code returns the address
 *                characteristic (OSS_ADDRSPACE_MEM/OSS_ADDRSPACE_IO)
 *                of the specified device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  code      reference to the information we need    
 *                ...       variable arguments                      
 *  Output.....:  *...      variable arguments
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_BrdInfo(
    u_int32 code,
    ... )
{
	int32		status = ERR_SUCCESS;
    va_list     argptr;

    va_start(argptr,code);

    switch ( code ) {
        
        /* supported functions */
        case BBIS_BRDINFO_FUNCTION:
        {
            u_int32 funcCode = va_arg( argptr, u_int32 );
            u_int32 *status = va_arg( argptr, u_int32* );

			/* no optional BBIS function do anything */
            *status = FALSE;
            funcCode = funcCode; /* dummy access to avoid compiler warning */
            break;
        }

        /* number of devices */
        case BBIS_BRDINFO_NUM_SLOTS:
        {
            u_int32 *numSlot = va_arg( argptr, u_int32* );

            *numSlot = BRD_NBR_OF_DEV;
            break;
        }
		
		/* bus type */
        case BBIS_BRDINFO_BUSTYPE:
        {
            u_int32 *busType = va_arg( argptr, u_int32* );

			*busType = OSS_BUSTYPE_NONE;
            break;
        }

        /* device bus type */
        case BBIS_BRDINFO_DEVBUSTYPE:
        {
            u_int32 mSlot       = va_arg( argptr, u_int32 );
            u_int32 *devBusType = va_arg( argptr, u_int32* );

			switch (mSlot) {
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				*devBusType = OSS_BUSTYPE_ISA;
				break;
			/* PCI-DEV 0 */
			case 0x1001:
				*devBusType = OSS_BUSTYPE_PCI;
				break;
			/* unknown */
			default:
				va_end( argptr );
				return ERR_BBIS_ILL_SLOT;
			}

            break;
        }

        /* interrupt capability */
        case BBIS_BRDINFO_INTERRUPTS:
        {
            u_int32 mSlot = va_arg( argptr, u_int32 );
            u_int32 *irqP = va_arg( argptr, u_int32* );

			switch (mSlot) {
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				*irqP = BBIS_IRQ_DEVIRQ;
				break;
			/* PCI-DEV 0 */
			case 0x1001:
				*irqP = 0;
				break;
			/* unknown */
			default:
				va_end( argptr );
				return ERR_BBIS_ILL_SLOT;
			}

            break;
        }

        /* address space type */
        case BBIS_BRDINFO_ADDRSPACE:
        {
            u_int32 mSlot      = va_arg( argptr, u_int32 );
            u_int32 *addrSpace = va_arg( argptr, u_int32* );

			switch (mSlot) {
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				*addrSpace = OSS_ADDRSPACE_IO;
				break;
			/* PCI-DEV 0 */
			case 0x1001:
				*addrSpace = OSS_ADDRSPACE_PCICFG;
				break;
			/* unknown */
			default:
				va_end( argptr );
				return ERR_BBIS_ILL_SLOT;
			}

            break;
        }

        /* error */
        default:
            status = ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** F2_CfgInfo **********************************
 *
 *  Description:  Get information about board configuration.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  ------------------------------
 *                BBIS_CFGINFO_BUSNBR       bus number
 *                BBIS_CFGINFO_IRQ          interrupt parameters
 *                BBIS_CFGINFO_EXP          exception interrupt parameters
 *                BBIS_CFGINFO_PCI_DEVNBR   pci device number
 *
 *                The BBIS_CFGINFO_BUSNBR code returns the number of the
 *                bus on which the board resides.
 *
 *                The BBIS_CFGINFO_IRQ code returns the device interrupt
 *                vector, level and mode of the specified device.
 *
 *                The BBIS_CFGINFO_EXP code returns the exception interrupt
 *                vector, level and mode of the specified device.
 *
 *                The BBIS_CFGINFO_PCI_DEVNBR code are only used for pci
 *                devices and returns the PCI device number of the specified
 *                device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure       
 *                code      reference to the information we need    
 *                ...       variable arguments                      
 *  Output.....:  ...       variable arguments
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_CfgInfo(
    BBIS_HANDLE     *brdHdl,
    u_int32         code,
    ... )
{
    va_list		argptr;
    int32       status=ERR_SUCCESS;


    DBGWRT_1((DBH, "BB - %s_CfgInfo (code=0x%04x)\n",BBNAME,code));

    va_start(argptr,code);

    switch ( code ) {
        /* bus number */
        case BBIS_CFGINFO_BUSNBR:
        {
            u_int32 *busNbr = va_arg( argptr, u_int32* );
            u_int32 mSlot   = va_arg( argptr, u_int32 );

			switch (mSlot) {
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				*busNbr = BRD_ISADEV0_BUSNBR;
				break;
			/* PCI-DEV 0 */
			case 0x1001:
				*busNbr = BRD_PCIDEV0_BUSNBR;
				break;
			/* unknown */
			default:
				DBGWRT_ERR((DBH,"*** %s_CfgInfo: mSlot=0x%04x not supported\n",BBNAME,mSlot));
				va_end( argptr );
				return ERR_BBIS_ILL_SLOT;
			}

            break;
        }

        /* interrupt information */
        case BBIS_CFGINFO_IRQ:
        {
            u_int32 mSlot   = va_arg( argptr, u_int32 );
            u_int32 *vector = va_arg( argptr, u_int32* );
            u_int32 *level  = va_arg( argptr, u_int32* );
            u_int32 *mode   = va_arg( argptr, u_int32* );

			switch (mSlot) {
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				/* no interrupt assigned */
				if( brdHdl->irqLev[0] == 0 ){
					*mode  = BBIS_IRQ_NONE;
					*level = 0;
					*vector = 0;
				}
				/* interrupt available */
				else{
					*mode  = BBIS_IRQ_SHARED;
					*level = brdHdl->irqLev[0];
					/* convert level to vector */
					status = OSS_IrqLevelToVector( brdHdl->osHdl, OSS_BUSTYPE_ISA,
												   *level, (int32*)vector );
				}
				break;
			/* PCI-DEV 0 */
			case 0x1001:
				*mode  = BBIS_IRQ_NONE;
				*level = 0;
				*vector = 0;
				break;
			/* unknown */
			default:
				DBGWRT_ERR((DBH,"*** %s_CfgInfo: mSlot=0x%04x not supported\n",BBNAME,mSlot));
				va_end( argptr );
				return ERR_BBIS_ILL_SLOT;
			}

			DBGWRT_2((DBH, " mSlot=0x%04x : IRQ mode=0x%x, level=0x%x, vector=0x%x\n",
				mSlot, *mode, *level, *vector));
			
			if (status) {
		        va_end( argptr );
				return status;
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

            /* no extra exception interrupt */
            *mode = 0;
            mSlot = mSlot;		/* dummy access to avoid compiler warning */
            vector = vector;	/* dummy access to avoid compiler warning */
            level = level;		/* dummy access to avoid compiler warning */
            mode = mode;		/* dummy access to avoid compiler warning */
            break;
        }

        /* PCI device number */
        case BBIS_CFGINFO_PCI_DEVNBR:
        {
			u_int32 mSlot      = va_arg( argptr, u_int32 );
			u_int32 *pciDevNbr = va_arg( argptr, u_int32* );
		
			switch (mSlot) {
			/* PCI-DEV 0 */
			case 0x1001:
				*pciDevNbr = BRD_PCIDEV0_DEVNBR;
				break;
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				va_end( argptr );
				return ERR_BBIS_UNK_CODE;
			/* unknown */
			default:
				DBGWRT_ERR((DBH,"*** %s_CfgInfo: mSlot=0x%04x not supported\n",BBNAME,mSlot));
				va_end( argptr );
				return ERR_BBIS_ILL_SLOT;
			}

            break;
        }

        /* error */
        default:
			DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",BBNAME,code));
            va_end( argptr );
            return ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return 0;
}

/****************************** F2_IrqEnable ********************************
 *
 *  Description:  Interrupt enable / disable.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                enable    interrupt setting                   
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_IrqEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         enable )
{
    DBGWRT_1((DBH, "BB - %s_IrqEnable: mSlot=0x%04x enable=%d\n",BBNAME,mSlot,enable));

	return 0;
}

/****************************** F2_IrqSrvInit *******************************
 *
 *  Description:  Called at the beginning of an interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *  Output.....:  return    BBIS_IRQ_UNK
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_IrqSrvInit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvInit: mSlot=0x%04x\n",BBNAME,mSlot));

    return BBIS_IRQ_UNK;
}

/****************************** F2_IrqSrvExit *******************************
 *
 *  Description:  Called at the end of an interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *  Output.....:  ---
 *  Globals....:  ---
 ****************************************************************************/
static void F2_IrqSrvExit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvExit: mSlot=0x%04x\n",BBNAME,mSlot));
}

/****************************** F2_ExpEnable ********************************
 *
 *  Description:  Exception interrupt enable / disable.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                enable    interrupt setting                   
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_ExpEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
	u_int32			enable)
{
	IDBGWRT_1((DBH, "BB - %s_ExpEnable: mSlot=0x%04x\n",BBNAME,mSlot));

	return 0;
}

/****************************** F2_ExpSrv ***********************************
 *
 *  Description:  Called at the beginning of an exception interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *  Output.....:  return    BBIS_IRQ_NO
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_ExpSrv(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_ExpSrv: mSlot=0x%04x\n",BBNAME,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** F2_SetMIface ********************************
 *
 *  Description:  Set device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24       
 *                dataMode  MDIS_MODE_F26 | MDIS_MODE_D32       
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_SetMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode)
{
	DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=0x%04x\n",BBNAME,mSlot));

    return 0;
}

/****************************** F2_ClrMIface ********************************
 *
 *  Description:  Clear device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_ClrMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=0x%04x\n",BBNAME,mSlot));

    return 0;
}

/****************************** F2_GetMAddr *********************************
 *
 *  Description:  Get physical address description.
 *
 *                - check device number
 *                - assign address spaces
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24    
 *                dataMode  MDIS_MODE_F26 | MDIS_MODE_D32      
 *                mAddr     pointer to address space            
 *                mSize     size of address space               
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_GetMAddr(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode,
    void            **mAddr,
    u_int32         *mSize )
{
	DBGWRT_1((DBH, "BB - %s_GetMAddr: mSlot=0x%04x\n",BBNAME,mSlot));

	/* check slot number */
	if ( (mSlot < BBIS_SLOTS_ONBOARDDEVICE_START) ||
		 (mSlot >= (BBIS_SLOTS_ONBOARDDEVICE_START + BRD_NBR_OF_DEV)) ) {
		DBGWRT_ERR((DBH,"*** %s_GetMAddr: mSlot=0x%04x not supported\n",BBNAME,mSlot));
		return ERR_BBIS_ILL_SLOT;
	}

    switch (mSlot) {
	/* ISA-DEV 0 */
    case 0x1000:
	case 0x1002:
		*mAddr = BRD_ISADEV0_IO_ADDR;
		*mSize = BRD_ISADEV0_IO_SIZE;
        break;
	/* PCI-DEV 0 */
    case 0x1001:
		*mAddr = (void*)0;
		*mSize = 0;
        break;
    }

	DBGWRT_2((DBH, " mSlot=0x%04x : io address=0x%x, length=0x%x\n",
		mSlot, *mAddr, *mSize));

    return 0;
}

/****************************** F2_SetStat **********************************
 *
 *  Description:  Set driver status
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_BB_DEBUG_LEVEL     board debug level          see dbg.h
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure           
 *                mSlot     module slot number                          
 *                code      setstat code                                
 *                value     setstat value or ptr to blocksetstat data   
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_SetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    int32           value )
{
    DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=0x%04x code=0x%04x value=0x%x\n",
			  BBNAME, mSlot, code, value));

    switch (code) {
        /* set debug level */
        case M_BB_DEBUG_LEVEL:
            brdHdl->dbgLev = value;
            break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** F2_GetStat **********************************
 *
 *  Description:  Get driver status
 *
 *                Following status codes are supported:
 *
 *                Code                 Description                Values
 *                -------------------  -------------------------  ----------
 *                M_BB_DEBUG_LEVEL     driver debug level         see dbg.h
 *                M_BB_IRQ_VECT        interrupt vector           0..max
 *                M_BB_IRQ_LEVEL       interrupt level            0..max
 *                M_BB_IRQ_PRIORITY    interrupt priority         0
 *                M_MK_BLK_REV_ID      ident function table ptr   -
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure           
 *                mSlot     module slot number                          
 *                code      getstat code                                
 *  Output.....:  valueP    getstat value or ptr to blockgetstat data
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_GetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    int32           *valueP )
{
    int32       status;

    DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=0x%04x code=0x%04x\n",BBNAME,mSlot,code));

    switch (code) {
        /* get debug level */
        case M_BB_DEBUG_LEVEL:
            *valueP = brdHdl->dbgLev;
            break;

        /* get IRQ vector */
        case M_BB_IRQ_VECT:

			switch (mSlot) {
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				if( brdHdl->irqLev[0] ){
					/* convert level to vector */
					if( (status = OSS_IrqLevelToVector( brdHdl->osHdl, OSS_BUSTYPE_ISA,
														brdHdl->irqLev[0], valueP )) )
						return status;
				}
				else
					*valueP = 0; /* no interrupt available */
				break;
			/* PCI-DEV 0 */
			case 0x1001:
				*valueP = 0;
				break;
			default:
				/* illegal slot number */
				DBGWRT_ERR((DBH,"*** %s_GetStat: mSlot=0x%04x not supported\n",BBNAME,mSlot));
				return ERR_BBIS_ILL_SLOT;
			}
			break;

        /* get IRQ level */
        case M_BB_IRQ_LEVEL:

			switch (mSlot) {
			/* ISA-DEV 0 */
			case 0x1000:
			case 0x1002:
				*valueP = brdHdl->irqLev[0];
				break;
			/* PCI-DEV 0 */
			case 0x1001:
				*valueP = 0;
				break;
			default:
				/* illegal slot number */
				DBGWRT_ERR((DBH,"*** %s_GetStat: mSlot=0x%04x not supported\n",BBNAME,mSlot));
				return ERR_BBIS_ILL_SLOT;
			}
            break;

        /* get IRQ priority */
        case M_BB_IRQ_PRIORITY:
			*valueP = 0;
			break;

        /* ident table */
        case M_MK_BLK_REV_ID:
			*valueP = (int32)&brdHdl->idFuncTbl;
			break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** F2_Unused ***********************************
 *
 *  Description:  Dummy function for unused jump table entries.
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  return  ERR_BBIS_ILL_FUNC
 *  Globals....:  ---
 ****************************************************************************/
static int32 F2_Unused( void )		/* nodoc */
{
    return ERR_BBIS_ILL_FUNC;
}

/*********************************** Ident **********************************
 *
 *  Description:  Return ident string 
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *  Output.....:  return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static char* Ident( void )		/* nodoc */
{
	return ( 
#ifdef F2
		"F2 - F2"
#endif
		"  Base Board Handler: $Id: bb_f2.c,v 1.5 2006/12/20 12:43:20 ufranke Exp $" );
}

/********************************* Cleanup **********************************
 *
 *  Description:  Close all handles, unmap addresses, free memory
 *
 *		          NOTE: The brdHdl handle is invalid after calling this
 *                      function.
 *			   
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure           
 *                retCode	return value
 *  Output.....:  return	retCode
 *  Globals....:  -
 ****************************************************************************/
static int32 Cleanup(
   BBIS_HANDLE  *brdHdl,
   int32        retCode		/* nodoc */
)
{
    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if (brdHdl->descHdl)
		DESC_Exit(&brdHdl->descHdl);

	/* cleanup debug */
	DBGEXIT((&DBH));

    /*------------------------------+
    |  unmap addresses              |
    +------------------------------*/
	if( brdHdl->fdcAddr )
		OSS_UnMapVirtAddr( brdHdl->osHdl, (void *)&brdHdl->fdcAddr,
						   BRD_FDC_IO_SIZE, OSS_ADDRSPACE_IO );

#ifdef OSS_HAS_UNASSIGN_RESOURCES	
	{
		OSS_RESOURCES	res;
		res.type = OSS_RES_IO;
		res.u.mem.physAddr = BRD_FDC_IO_ADDR;
		res.u.mem.size = BRD_FDC_IO_SIZE;

		if( brdHdl->resourcesAssigned )
			OSS_UnAssignResources( brdHdl->osHdl, OSS_BUSTYPE_ISA, 
								   BRD_FDC_BUSNBR,
								   1, &res );
	}
#endif

    /*------------------------------+
    |  free memory                  |
    +------------------------------*/
    /* release memory for the board handle */
    OSS_MemFree( brdHdl->osHdl, (int8*)brdHdl, brdHdl->ownMemSize);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}



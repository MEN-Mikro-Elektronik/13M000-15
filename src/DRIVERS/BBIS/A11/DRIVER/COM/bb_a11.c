/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: bb_a11.c
 *      Project: A11 board handler
 *
 *       Author: kp
 *        $Date: 2006/12/20 12:24:45 $
 *    $Revision: 1.2 $
 *
 *  Description: A11
 *
 *               The A11 supports several onboard-devices:
 *
 *               device  
 *               -----------------------------------------------------------
 *               0x1000  Z8536  
 *               0x1001  LM78 (HW-Monitor)
 *				 0x1002	 LEDs
 *
 *     ******* NOTE: Z8536/LEDs functionality NOT VERIFIED!!! ********
 *
 *     			 The onboard PC-MIP slots are handled by the generic PCI BBIS
 *
 *     Required: ---
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_a11.c,v $
 * Revision 1.2  2006/12/20 12:24:45  ufranke
 * changed
 *  - fkt13 replaced by setIrqHandle
 *
 * Revision 1.1  1999/08/12 09:14:53  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Id: bb_a11.c,v 1.2 2006/12/20 12:24:45 ufranke Exp $";

#define _NO_BBIS_HANDLE		/* bb_defs.h: don't define BBIS_HANDLE struct */

#include <MEN/mdis_com.h>
#include <MEN/men_typs.h>   /* system dependend definitions   */
#include <MEN/dbg.h>        /* debug functions                */
#include <MEN/oss.h>        /* oss functions                  */
#include <MEN/desc.h>       /* descriptor functions           */
#include <MEN/bb_defs.h>    /* bbis definitions				  */
#include <MEN/mdis_err.h>   /* MDIS error codes               */
#include <MEN/mdis_api.h>   /* MDIS global defs               */

#include "a11_int.h"		/* A11 specific defines */ 

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL		brdHdl->debugLevel
#define DBH             brdHdl->debugHdl


#define CFIDX(i) ((i)-BBIS_SLOTS_ONBOARDDEVICE_START)

/* include files which need BBIS_HANDLE */
#include <MEN/bb_entry.h>	/* bbis jumptable				  */
#include <MEN/bb_a11.h>		/* A11 bbis header file			  */

typedef struct {
	u_int32 devBusType;
	u_int32 interrupts;
	u_int32 addrSpace;
	int32 	pciBusNbr;
	int32 	pciDevNbr;
	u_int32 irqLevel;
	u_int32 irqMode;
	u_int32 addr;
	u_int32 size;
} A11_SLOT_CFG;
	
/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
const A11_SLOT_CFG G_slotCfg[BRD_NBR_OF_BRDDEV] = {
	/*--- Z8536 ---*/
	{ OSS_BUSTYPE_ISA, BBIS_IRQ_DEVIRQ, OSS_ADDRSPACE_MEM, -1, -1, 9, 
	  BBIS_IRQ_SHARED, 0x80000844, 0x4 },
	/*--- LM78 ---*/
	{ OSS_BUSTYPE_NONE, BBIS_IRQ_DEVIRQ, OSS_ADDRSPACE_MEM, -1, -1, 5, 
	  BBIS_IRQ_EXCLUSIVE, 0, 0 },
	/*--- LEDs ---*/
	{ OSS_BUSTYPE_PCI, 0, OSS_ADDRSPACE_PCICFG, 0, 0x1c,
	  BBIS_IRQ_NONE, 0, 0 }
};


/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
/* init/exit */
static int32 A11_Init(OSS_HANDLE*, DESC_SPEC*, BBIS_HANDLE**);
static int32 A11_BrdInit(BBIS_HANDLE*);
static int32 A11_BrdExit(BBIS_HANDLE*);
static int32 A11_Exit(BBIS_HANDLE**);
/* info */
static int32 A11_BrdInfo(u_int32, ...);
static int32 A11_CfgInfo(BBIS_HANDLE*, u_int32, ...);
/* interrupt handling */
static int32 A11_IrqEnable(BBIS_HANDLE*, u_int32, u_int32);
static int32 A11_IrqSrvInit(BBIS_HANDLE*, u_int32);
static void  A11_IrqSrvExit(BBIS_HANDLE*, u_int32);
/* exception handling */
static int32 A11_ExpEnable(BBIS_HANDLE*,u_int32, u_int32);
static int32 A11_ExpSrv(BBIS_HANDLE*,u_int32);
/* get module address */
static int32 A11_SetMIface(BBIS_HANDLE*, u_int32, u_int32, u_int32);
static int32 A11_ClrMIface(BBIS_HANDLE*,u_int32);
static int32 A11_GetMAddr(BBIS_HANDLE*, u_int32, u_int32, u_int32, void**, u_int32*);
/* getstat/setstat */
static int32 A11_SetStat(BBIS_HANDLE*, u_int32, int32, int32);
static int32 A11_GetStat(BBIS_HANDLE*, u_int32, int32, int32*);
/* unused */
static int32 A11_Unused(void);
/* miscellaneous */
static char* Ident( void );
static int32 Cleanup(BBIS_HANDLE *brdHdl, int32 retCode);


/**************************** A11_GetEntry ***********************************
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
	extern void A11_GetEntry( BBIS_ENTRY *bbisP )
#endif
{
    /* init/exit */
    bbisP->init         =   A11_Init;
    bbisP->brdInit      =   A11_BrdInit;
    bbisP->brdExit      =   A11_BrdExit;
    bbisP->exit         =   A11_Exit;
    bbisP->fkt04        =   A11_Unused;
    /* info */
    bbisP->brdInfo      =   A11_BrdInfo;
    bbisP->cfgInfo      =   A11_CfgInfo;
    bbisP->fkt07        =   A11_Unused;
    bbisP->fkt08        =   A11_Unused;
    bbisP->fkt09        =   A11_Unused;
    /* interrupt handling */
    bbisP->irqEnable    =   A11_IrqEnable;
    bbisP->irqSrvInit   =   A11_IrqSrvInit;
    bbisP->irqSrvExit   =   A11_IrqSrvExit;
    bbisP->setIrqHandle =   NULL;
    bbisP->fkt14        =   A11_Unused;
    /* exception handling */
    bbisP->expEnable    =   A11_ExpEnable;
    bbisP->expSrv       =   A11_ExpSrv;
    bbisP->fkt17        =   A11_Unused;
    bbisP->fkt18        =   A11_Unused;
    bbisP->fkt19        =   A11_Unused;
    /* */
    bbisP->fkt20        =   A11_Unused;
    bbisP->fkt21        =   A11_Unused;
    bbisP->fkt22        =   A11_Unused;
    bbisP->fkt23        =   A11_Unused;
    bbisP->fkt24        =   A11_Unused;
    /*  getstat / setstat / address setting */
    bbisP->setStat      =   A11_SetStat;
    bbisP->getStat      =   A11_GetStat;
    bbisP->setMIface    =   A11_SetMIface;
    bbisP->clrMIface    =   A11_ClrMIface;
    bbisP->getMAddr     =   A11_GetMAddr;
    bbisP->fkt30        =   A11_Unused;
    bbisP->fkt31        =   A11_Unused;
}

/****************************** A11_Init *************************************
 *
 *  Description:  Allocate and return board handle.
 *
 *                - initializes the board handle
 *                - reads and saves board descriptor entries
 *
 *                The following descriptor keys are used:
 *
 *                Deskriptor key           Default          Range
 *                -----------------------  ---------------  -------------
 *                DEBUG_LEVEL_DESC         OSS_DBG_DEFAULT  see dbg.h
 *                DEBUG_LEVEL              OSS_DBG_DEFAULT  see dbg.h
 *---------------------------------------------------------------------------
 *  Input......:  osHdl     pointer to os specific structure             
 *                descSpec  pointer to os specific descriptor specifier  
 *                brdHdlP   pointer to not initialized board handle structure            
 *  Output.....:  *brdHdlP  initialized board handle structure  
 *				  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 A11_Init(
    OSS_HANDLE      *osHdl,
    DESC_SPEC       *descSpec,
    BBIS_HANDLE     **brdHdlP )
{
    BBIS_HANDLE	*brdHdl = NULL;
	u_int32     gotsize;
    int32       status;
    u_int32		value;

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
	brdHdl->idFuncTbl.idCall[2].identCall = DESC_Ident;
	brdHdl->idFuncTbl.idCall[3].identCall = OSS_Ident;
	/* terminator */
	brdHdl->idFuncTbl.idCall[5].identCall = NULL;

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
    status = DESC_GetUInt32( brdHdl->descHdl, OSS_DBG_DEFAULT, 
							 &(brdHdl->debugLevel),
                "DEBUG_LEVEL");
    if ( status && (status!=ERR_DESC_KEY_NOTFOUND) )
        return( Cleanup(brdHdl,status) );


    return 0;
}

/****************************** A11_BrdInit **********************************
 *
 *  Description:  Board initialization.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure    
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 A11_BrdInit(
    BBIS_HANDLE     *brdHdl )
{
	DBGWRT_1((DBH, "BB - %s_BrdInit\n",BBNAME));

	return 0;
}

/****************************** A11_BrdExit **********************************
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
static int32 A11_BrdExit(
    BBIS_HANDLE     *brdHdl )
{
	DBGWRT_1((DBH, "BB - %s_BrdExit\n",BBNAME));

    return 0;
}

/****************************** A11_Exit *************************************
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
static int32 A11_Exit(
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

/****************************** A11_BrdInfo **********************************
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
static int32 A11_BrdInfo(
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
            break;
        }

        /* number of devices */
        case BBIS_BRDINFO_NUM_SLOTS:
        {
            u_int32 *numSlot = va_arg( argptr, u_int32* );

            *numSlot = BRD_NBR_OF_BRDDEV;
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

			*devBusType = G_slotCfg[CFIDX(mSlot)].devBusType;
            break;
        }

        /* interrupt capability */
        case BBIS_BRDINFO_INTERRUPTS:
        {
            u_int32 mSlot = va_arg( argptr, u_int32 );
            u_int32 *irqP = va_arg( argptr, u_int32* );

            *irqP = G_slotCfg[CFIDX(mSlot)].interrupts;
            break;
        }

        /* address space type */
        case BBIS_BRDINFO_ADDRSPACE:
        {
            u_int32 mSlot      = va_arg( argptr, u_int32 );
            u_int32 *addrSpace = va_arg( argptr, u_int32* );

			*addrSpace = G_slotCfg[CFIDX(mSlot)].addrSpace;
            break;
        }

        /* error */
        default:
            status = ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** A11_CfgInfo **********************************
 *
 *  Description:  Get information about board configuration.
 *
 *                Following info codes are supported:
 *
 *                Code                      Description
 *                ------------------------  ------------------------------
 *                BBIS_CFGINFO_BUSNBR       bus number
 *				  BBIS_CFGINFO_PCI_DEVNBR	PCI device number	
 *                BBIS_CFGINFO_IRQ          interrupt parameters
 *                BBIS_CFGINFO_EXP          exception interrupt parameters
 *
 *                The BBIS_CFGINFO_BUSNBR code returns the number of the
 *                bus on which the specified device resides
 *
 *                The BBIS_CFGINFO_PCI_DEVNBR code returns the device number 
 *                on the PCI bus on which the specified device resides
 *
 *                The BBIS_CFGINFO_IRQ code returns the device interrupt
 *                vector, level and mode of the specified device.
 *
 *                The BBIS_CFGINFO_EXP code returns the exception interrupt
 *                vector, level and mode of the specified device.
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure       
 *                code      reference to the information we need    
 *                ...       variable arguments                      
 *  Output.....:  ...       variable arguments
 *                return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 A11_CfgInfo(
    BBIS_HANDLE     *brdHdl,
    u_int32         code,
    ... )
{
    va_list		argptr;
    int32       status=0;


    DBGWRT_1((DBH, "BB - %s_CfgInfo\n",BBNAME));

    va_start(argptr,code);

    switch ( code ) {
        /* bus number */
        case BBIS_CFGINFO_BUSNBR:
        {
            u_int32 *busNbr = va_arg( argptr, u_int32* );
            u_int32 mSlot   = va_arg( argptr, u_int32 );

			if ( G_slotCfg[CFIDX(mSlot)].pciBusNbr >= 0 )
				*busNbr = G_slotCfg[CFIDX(mSlot)].pciBusNbr;
			else
				status = ERR_BBIS_UNK_CODE;

            break;
        }

        /* PCI device number */
        case BBIS_CFGINFO_PCI_DEVNBR:
        {
			u_int32 mSlot      = va_arg( argptr, u_int32 );
			u_int32 *pciDevNbr = va_arg( argptr, u_int32* );
		
			if ( G_slotCfg[CFIDX(mSlot)].pciDevNbr >= 0 )
				*pciDevNbr = G_slotCfg[CFIDX(mSlot)].pciDevNbr;
			else
				status = ERR_BBIS_UNK_CODE;
            break;
        }

        /* interrupt information */
        case BBIS_CFGINFO_IRQ:
        {
            u_int32 mSlot   = va_arg( argptr, u_int32 );
            u_int32 *vector = va_arg( argptr, u_int32* );
            u_int32 *level  = va_arg( argptr, u_int32* );
            u_int32 *mode   = va_arg( argptr, u_int32* );

		    /* check device number */
			if ( (mSlot < BBIS_SLOTS_ONBOARDDEVICE_START) ||
				 (mSlot >= (BBIS_SLOTS_ONBOARDDEVICE_START + BRD_NBR_OF_BRDDEV)) ) {
				DBGWRT_ERR((DBH,"*** %s_CfgInfo: mSlot=0x%x not supported\n",BBNAME,mSlot));
		        va_end( argptr );
			    return ERR_BBIS_ILL_SLOT;
			}

			*mode  = G_slotCfg[CFIDX(mSlot)].irqMode;
			*level = G_slotCfg[CFIDX(mSlot)].irqLevel;

			/* convert level to vector */
			status = OSS_IrqLevelToVector(
						brdHdl->osHdl,
						G_slotCfg[CFIDX(mSlot)].devBusType,
						(int32)*level,
						(int32*)vector );

			DBGWRT_2((DBH, " dev:%d : IRQ mode=0x%x, level=0x%x, vector=0x%x\n",
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
            break;
        }


        /* error */
        default:
			DBGWRT_ERR((DBH,"*** %s_CfgInfo: code=0x%x not supported\n",BBNAME,code));
            va_end( argptr );
            return ERR_BBIS_UNK_CODE;
    }

    va_end( argptr );
    return status;
}

/****************************** A11_IrqEnable ********************************
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
static int32 A11_IrqEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         enable )
{
    DBGWRT_1((DBH, "BB - %s_IrqEnable: mSlot=%d enable=%d\n",BBNAME,mSlot,enable));

	return 0;
}

/****************************** A11_IrqSrvInit *******************************
 *
 *  Description:  Called at the beginning of an interrupt.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *  Output.....:  return    BBIS_IRQ_NO
 *  Globals....:  ---
 ****************************************************************************/
static int32 A11_IrqSrvInit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvInit: mSlot=%d\n",BBNAME,mSlot));

    return BBIS_IRQ_UNK;
}

/****************************** A11_IrqSrvExit *******************************
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
static void A11_IrqSrvExit(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_IrqSrvExit: mSlot=%d\n",BBNAME,mSlot));
}

/****************************** A11_ExpEnable ********************************
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
static int32 A11_ExpEnable(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
	u_int32			enable)
{
	IDBGWRT_1((DBH, "BB - %s_ExpEnable: mSlot=%d\n",BBNAME,mSlot));

	return 0;
}

/****************************** A11_ExpSrv ***********************************
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
static int32 A11_ExpSrv(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot )
{
	IDBGWRT_1((DBH, "BB - %s_ExpSrv: mSlot=%d\n",BBNAME,mSlot));

	return BBIS_IRQ_NO;
}

/****************************** A11_SetMIface ********************************
 *
 *  Description:  Set device interface.
 *
 *                Do nothing
 *
 *---------------------------------------------------------------------------
 *  Input......:  brdHdl    pointer to board handle structure   
 *                mSlot     module slot number                  
 *                addrMode  MDIS_MODE_A08 | MDIS_MODE_A24       
 *                dataMode  MDIS_MODE_A116 | MDIS_MODE_D32       
 *  Output.....:  return    0
 *  Globals....:  ---
 ****************************************************************************/
static int32 A11_SetMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode)
{
	DBGWRT_1((DBH, "BB - %s_SetMIface: mSlot=%d\n",BBNAME,mSlot));

    return 0;
}

/****************************** A11_ClrMIface ********************************
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
static int32 A11_ClrMIface(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot)
{
	DBGWRT_1((DBH, "BB - %s_ClrMIface: mSlot=%d\n",BBNAME,mSlot));

    return 0;
}

/****************************** A11_GetMAddr *********************************
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
 *                dataMode  MDIS_MODE_A116 | MDIS_MODE_D32      
 *                mAddr     pointer to address space            
 *                mSize     size of address space               
 *  Output.....:  return    0 | error code
 *  Globals....:  ---
 ****************************************************************************/
static int32 A11_GetMAddr(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    u_int32         addrMode,
    u_int32         dataMode,
    void            **mAddr,
    u_int32         *mSize )
{
	DBGWRT_1((DBH, "BB - %s_GetMAddr: mSlot=%d\n",BBNAME,mSlot));

	/* check device number */
	if ( (mSlot < BBIS_SLOTS_ONBOARDDEVICE_START) ||
		 (mSlot >= (BBIS_SLOTS_ONBOARDDEVICE_START + BRD_NBR_OF_BRDDEV)) ) {
		DBGWRT_ERR((DBH,"*** %s_CfgInfo: mSlot=0x%x not supported\n",BBNAME,mSlot));
		return ERR_BBIS_ILL_SLOT;
	}

	/* assign address spaces */
	*mAddr = (void *)G_slotCfg[CFIDX(mSlot)].addr;
    *mSize = G_slotCfg[CFIDX(mSlot)].size;

	DBGWRT_2((DBH, " dev:%d : io address=0x%x, length=0x%x\n",
		mSlot, *mAddr, *mSize));

    return 0;
}

/****************************** A11_SetStat **********************************
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
static int32 A11_SetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    int32           value )
{
    DBGWRT_1((DBH, "BB - %s_SetStat: mSlot=%d code=0x%04x value=0x%x\n",
			  BBNAME, mSlot, code, value));

    switch (code) {
        /* set debug level */
        case M_BB_DEBUG_LEVEL:
            brdHdl->debugLevel = value;
            break;

        /* unknown */
        default:
            return ERR_BBIS_UNK_CODE;
    }

    return 0;
}

/****************************** A11_GetStat **********************************
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
static int32 A11_GetStat(
    BBIS_HANDLE     *brdHdl,
    u_int32         mSlot,
    int32           code,
    int32           *valueP )
{
    int32       status;

    DBGWRT_1((DBH, "BB - %s_GetStat: mSlot=%d code=0x%04x\n",BBNAME,mSlot,code));

    switch (code) {
        /* get debug level */
        case M_BB_DEBUG_LEVEL:
            *valueP = brdHdl->debugLevel;
            break;

        /* get IRQ vector */
        case M_BB_IRQ_VECT:
			/* convert level to vector */
			status = OSS_IrqLevelToVector(
						brdHdl->osHdl,
						G_slotCfg[CFIDX(mSlot)].devBusType,
						G_slotCfg[CFIDX(mSlot)].irqLevel,
						valueP );
			if (status)
				return status;
			break;

        /* get IRQ level */
        case M_BB_IRQ_LEVEL:
			*valueP = G_slotCfg[CFIDX(mSlot)].irqLevel;
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

/****************************** A11_Unused ***********************************
 *
 *  Description:  Dummy function for unused jump table entries.
 *
 *---------------------------------------------------------------------------
 *  Input......:  ---
 *  Output.....:  return  ERR_BBIS_ILL_FUNC
 *  Globals....:  ---
 ****************************************************************************/
static int32 A11_Unused( void )		/* nodoc */
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
		"A11 - A11"
		"  Base Board Handler: $Id: bb_a11.c,v 1.2 2006/12/20 12:24:45 ufranke Exp $" );
}

/********************************* Cleanup **********************************
 *
 *  Description:  Close all handles, free memory and return error code
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
    |  free memory                  |
    +------------------------------*/
    /* release memory for the board handle */
    OSS_MemFree( brdHdl->osHdl, (int8*)brdHdl, brdHdl->ownMemSize);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}

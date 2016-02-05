/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  xm01bc_drv.c
 *
 *      \author  matthias.soehnlein@men.de
 *        $Date: 2015/03/11 18:15:47 $
 *    $Revision: 1.4 $
 *
 *      \brief   Low-level driver for XM01BC device on SMBus
 *
 *     Required: OSS, DESC, DBG, libraries
 *
 *     \switches _ONE_NAMESPACE_PER_DRIVER_, XM01BC_SW
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: xm01bc_drv.c,v $
 * Revision 1.4  2015/03/11 18:15:47  ts
 * R: operating hours displayed were only max. 255 hours
 * M: fixed GetStat OPHRS_CNT to use ReadWordData
 *
 * Revision 1.3  2009/09/04 11:57:35  MRoth
 * R: Porting to MIDS5
 * M: a) added support for 64bit (Set/GetStat prototypes)
 *    b) added casts
 *    c) changed location of xm01bc_drv.h include
 *
 * Revision 1.2  2009/02/06 15:46:13  MSoehnlein
 * R:1. Added MM1/ XM2/ F19 Support
 *   2. MDIS5 porting
 * M:1.a) added "Hardware ID" query
 *     b) added "Number of Supervised Voltages" query
 *     c) added "Number of Error Counters" query
 *     d) added "RESET_IN Blocking Mode" query
 *     e) increased number of error counters to 31
 *     f) increased number of supervised voltages to 8
 *     g) renamed Board Controller Interface (BCI) from XM01BCI to CPUBCI
 *   2.a) Changed parameter of XM01BC_SetStat() and XM01BC_GetStat()
 *          from "int32 value" to "INT32_OR_64 value32_or_64"
 *          and "int32 *valueP" to "INT32_OR_64 *value32_or_64P"
 *
 * Revision 1.1  2008/10/31 17:13:31  MSoehnlein
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#define _NO_LL_HANDLE		/* ll_defs.h: don't define LL_HANDLE struct */

#ifdef XM01BC_SW			/* swapped variant */
#	define MAC_MEM_MAPPED
#	define ID_SW
#endif

#include <MEN/men_typs.h>   /* system dependent definitions */
#include <MEN/maccess.h>    /* hw access macros and types */
#include <MEN/dbg.h>        /* debug functions */
#include <MEN/oss.h>        /* oss functions */
#include <MEN/desc.h>       /* descriptor functions */
#include <MEN/mdis_api.h>   /* MDIS global defs */
#include <MEN/mdis_com.h>   /* MDIS common defs */
#include <MEN/mdis_err.h>   /* MDIS error codes */
#include <MEN/ll_defs.h>    /* low-level driver definitions */
#include <MEN/smb2.h>		/* SMB2 definitions */
#include <MEN/cpubci.h>	    /* CPUBCI definitions */
#include <MEN/xm01bci.h>	/* XM01BCI definitions */
#include <MEN/wdog.h>		/* watchdog specific definitions */

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/* general defines */
#define CH_BYTES			4			/**< Number of bytes per channel */
#define USE_IRQ				FALSE		/**< Interrupt required  */
#define ADDRSPACE_COUNT		0			/**< Number of required address spaces */
#define ADDRSPACE_SIZE		0			/**< Size of address space */

/* debug defines */
#define DBG_MYLEVEL			llHdl->dbgLevel   /**< Debug level */
#define DBH					llHdl->dbgHdl     /**< Debug handle */

/* macro to map SMB2 error code into device specific error code range */
#define SMB2_ERRMAP(err)	(((err) && ((err) < (SMB_ERR_LAST))) ? \
								((err) += (ERR_DEV)) : (err))

#define BUS_IDLE_DELAY		1 			/**< Additional SMBus idle time */

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/** low-level handle */
typedef struct {
	/* general */
    int32           memAlloc;		/**< Size allocated for the handle */
    OSS_HANDLE      *osHdl;         /**< OSS handle */
    OSS_IRQ_HANDLE  *irqHdl;        /**< IRQ handle */
    DESC_HANDLE     *descHdl;       /**< DESC handle */
	MDIS_IDENT_FUNCT_TBL idFuncTbl;	/**< ID function table */
	/* debug */
    u_int32         dbgLevel;		/**< Debug level */
	DBG_HANDLE      *dbgHdl;        /**< Debug handle */
	/* cpubc specific */
	SMB_HANDLE		*smbH;			/**< ptr to SMB_HANDLE struct */
	u_int16			smbAddr;		/**< SMB address of CPUBC */
	u_int16 fwRev[CPUBCI_GETREV_NUM]; /**< PIC firmware revision */
	u_int8			fwRevFirst;		/**< 1: first release of XM01 PIC firmware */
	u_int16			hwVariant;		/**< variant of CPU board */
	u_int8          numVoltages;    /**< number of supervised voltages */
	u_int8          numErrCntrs;    /**< number of error counters */
	u_int8			wdStatus;		/**< Watchdog status */
    OSS_SIG_HANDLE  *sigHdl;        /**< signal handle */
    OSS_ALARM_HANDLE *alarmHdl;		/**< alarm handle */
	u_int32			pollPeriod;		/**< poll period for alarm [ms] */
	u_int8			voltSvState;	/**< last voltage supervision state */
} LL_HANDLE;

/* include files which need LL_HANDLE */
#include <MEN/ll_entry.h>   /* low-level driver jump table  */
#include <MEN/xm01bc_drv.h>	/* XM01BC driver header file */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
static int32 XM01BC_Init(DESC_SPEC *descSpec, OSS_HANDLE *osHdl,
					   MACCESS *ma, OSS_SEM_HANDLE *devSemHdl,
					   OSS_IRQ_HANDLE *irqHdl, LL_HANDLE **llHdlP);
static int32 XM01BC_Exit(LL_HANDLE **llHdlP );
static int32 XM01BC_Read(LL_HANDLE *llHdl, int32 ch, int32 *value);
static int32 XM01BC_Write(LL_HANDLE *llHdl, int32 ch, int32 value);
static int32 XM01BC_SetStat(LL_HANDLE *llHdl,int32 ch, int32 code, INT32_OR_64 value32_or_64);
static int32 XM01BC_GetStat(LL_HANDLE *llHdl, int32 ch, int32 code,INT32_OR_64 *value32_or_64P);
static int32 XM01BC_BlockRead(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							int32 *nbrRdBytesP);
static int32 XM01BC_BlockWrite(LL_HANDLE *llHdl, int32 ch, void *buf, int32 size,
							 int32 *nbrWrBytesP);
static int32 XM01BC_Irq(LL_HANDLE *llHdl );
static int32 XM01BC_Info(int32 infoType, ... );

static char* Ident( void );
static int32 Cleanup(LL_HANDLE *llHdl, int32 retCode);

/* XM01BC specific helper functions */
static void AlarmHandler(void *arg);

/* SMBusWrapper functions for delay insertion */
static int32 SMBusWriteByte( LL_HANDLE *llHdl, u_int32 flags, u_int8 data );
static int32 SMBusWriteByteData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int8 data );
static int32 SMBusReadByteData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int8 *data );
static int32 SMBusWriteWordData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int16 data );
static int32 SMBusReadWordData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int16 *data );



/****************************** XM01BC_GetEntry ********************************/
/** Initialize driver's jump table
 *
 *  \param drvP     \OUT Pointer to the initialized jump table structure
 */
#ifdef _ONE_NAMESPACE_PER_DRIVER_
    void LL_GetEntry( LL_ENTRY* drvP )
#else
    void __XM01BC_GetEntry( LL_ENTRY* drvP )
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */
{
    drvP->init        = XM01BC_Init;
    drvP->exit        = XM01BC_Exit;
    drvP->read        = XM01BC_Read;
    drvP->write       = XM01BC_Write;
    drvP->blockRead   = XM01BC_BlockRead;
    drvP->blockWrite  = XM01BC_BlockWrite;
    drvP->setStat     = XM01BC_SetStat;
    drvP->getStat     = XM01BC_GetStat;
    drvP->irq         = XM01BC_Irq;
    drvP->info        = XM01BC_Info;
}

/******************************** XM01BC_Init **********************************/
/** Allocate and return low-level handle, initialize hardware
 *
 * The function initializes the XM01BC device with the definitions made
 * in the descriptor.
 *
 * The function decodes \ref descriptor_entries "these descriptor entries"
 * in addition to the general descriptor keys.
 *
 *  \param descP      \IN  Pointer to descriptor data
 *  \param osHdl      \IN  OSS handle
 *  \param ma         \IN  HW access handle
 *  \param devSemHdl  \IN  Device semaphore handle
 *  \param irqHdl     \IN  IRQ handle
 *  \param llHdlP     \OUT Pointer to low-level driver handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 XM01BC_Init(
    DESC_SPEC       *descP,
    OSS_HANDLE      *osHdl,
    MACCESS         *ma,
    OSS_SEM_HANDLE  *devSemHdl,
    OSS_IRQ_HANDLE  *irqHdl,
    LL_HANDLE       **llHdlP
)
{
    LL_HANDLE	*llHdl = NULL;
    u_int32		gotsize, smbBusNbr;
    int32		error;
    u_int32		value;
	u_int8		n;

    /*------------------------------+
    |  prepare the handle           |
    +------------------------------*/
	*llHdlP = NULL;		/* set low-level driver handle to NULL */

	/* alloc */
    if((llHdl = (LL_HANDLE*)OSS_MemGet(
    				osHdl, sizeof(LL_HANDLE), &gotsize)) == NULL)
       return(ERR_OSS_MEM_ALLOC);

	/* clear */
    OSS_MemFill(osHdl, gotsize, (char*)llHdl, 0x00);

	/* init */
    llHdl->memAlloc   = gotsize;
    llHdl->osHdl      = osHdl;
    llHdl->irqHdl     = irqHdl;

    /*------------------------------+
    |  init id function table       |
    +------------------------------*/
	/* driver's ident function */
	llHdl->idFuncTbl.idCall[0].identCall = Ident;
	/* library's ident functions */
	llHdl->idFuncTbl.idCall[1].identCall = DESC_Ident;
	llHdl->idFuncTbl.idCall[2].identCall = OSS_Ident;
	/* terminator */
	llHdl->idFuncTbl.idCall[3].identCall = NULL;

    /*------------------------------+
    |  prepare debugging            |
    +------------------------------*/
	DBG_MYLEVEL = OSS_DBG_DEFAULT;	/* set OS specific debug level */
	DBGINIT((NULL,&DBH));

    /*------------------------------+
    |  scan descriptor              |
    +------------------------------*/
	/* prepare access */
    if((error = DESC_Init(descP, osHdl, &llHdl->descHdl)))
		return( Cleanup(llHdl,error) );

    /* DEBUG_LEVEL_DESC */
    if((error = DESC_GetUInt32(llHdl->descHdl, OSS_DBG_DEFAULT,
								&value, "DEBUG_LEVEL_DESC")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

	DESC_DbgLevelSet(llHdl->descHdl, value);	/* set level */

    /* DEBUG_LEVEL */
    if((error = DESC_GetUInt32(llHdl->descHdl, OSS_DBG_DEFAULT,
								&llHdl->dbgLevel, "DEBUG_LEVEL")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );

    DBGWRT_1((DBH, "LL - XM01BC_Init\n"));

    /* SMB_BUSNBR (required) */
    if((error = DESC_GetUInt32(llHdl->descHdl, 0,
								&smbBusNbr, "SMB_BUSNBR")))
		return( Cleanup(llHdl,error) );

    /* SMB_DEVADDR (required) */
    if((error = DESC_GetUInt32(llHdl->descHdl, 0,
								&value, "SMB_DEVADDR")))
		return( Cleanup(llHdl,error) );
	llHdl->smbAddr = (u_int16)value;

    /* POLL_PERIOD [ms] */
    if((error = DESC_GetUInt32(llHdl->descHdl, 500,
								&llHdl->pollPeriod, "POLL_PERIOD")) &&
		error != ERR_DESC_KEY_NOTFOUND)
		return( Cleanup(llHdl,error) );
	/* check range */
	if( (llHdl->pollPeriod < 50) ||
		(llHdl->pollPeriod > 5000))
		return( Cleanup(llHdl,ERR_LL_DESC_PARAM) );

    /*------------------------------+
    |  init alarm                   |
    +------------------------------*/
    if((error = OSS_AlarmCreate(llHdl->osHdl, AlarmHandler, llHdl,
								 &llHdl->alarmHdl)) )
		return( Cleanup(llHdl,error));

    /*------------------------------+
    |  get SMB handle               |
    +------------------------------*/
	if((error = OSS_GetSmbHdl( llHdl->osHdl, smbBusNbr,
								(void**)&llHdl->smbH) ))
		return( Cleanup(llHdl,error) );

    /*------------------------------+
    |  init hardware                |
    +------------------------------*/
	/* Get and store hardware variant and firmware revision */
	if( (error = SMBusReadWordData( llHdl, 0, CPUBCI_HW_VARIANT,
										&llHdl->hwVariant ) ))
		return( Cleanup(llHdl,error) );

    /* If hardware variant could not be read */
    if( llHdl->hwVariant == CPUBCI_HW_VAR_INVALID )
        return( Cleanup(llHdl,ERR_LL_READ) );

    /* If hardware variant is XM01 with first revision firmware */
    if( llHdl->hwVariant == CPUBCI_HW_VAR_XM01_1 )
        llHdl->fwRevFirst = TRUE;

	/* Get and store firmware revision */
	for( n=0; n<CPUBCI_GETREV_NUM; n++ ) {
		if( (error = SMBusReadWordData( llHdl, 0, (u_int8)CPUBCI_GETREV(n),
											&llHdl->fwRev[n] ) ))
			return( Cleanup(llHdl,error) );
	}

    /* Get and store number of supervised voltages */
    if( !llHdl->fwRevFirst )
    {
    	if( (error = SMBusReadByteData( llHdl, 0, CPUBCI_NUM_VOLTS,
    										&llHdl->numVoltages ) ))
    		return( Cleanup(llHdl,error) );
    }
    else
    {
        llHdl->numVoltages = XM01BCI_VOLT_NUM;
    }


     /* Get and store number of error counters */
    if( !llHdl->fwRevFirst )
    {
    	if( (error = SMBusReadByteData( llHdl, 0, CPUBCI_NUM_ERR_CNTRS,
    										&llHdl->numErrCntrs ) ))
    		return( Cleanup(llHdl,error) );
    }
    else
    {
        llHdl->numErrCntrs = XM01BCI_ERRCNT_NUM;
    }





	/* Get external watchdog (Main CPU) state */
	if( (error = SMBusReadByteData( llHdl, 0, CPUBCI_WDOG_STATE,
										&llHdl->wdStatus ) ))
		return( Cleanup(llHdl,error) );

    /* Set low-level driver handle */
	*llHdlP = llHdl;

	return(ERR_SUCCESS);
}

/****************************** XM01BC_Exit ************************************/
/** De-initialize hardware and clean up memory
 *
 *  The function deinitializes the XM01BC device.
 *
 *  \param llHdlP      \IN  Pointer to low-level driver handle
 *
 *  \return           \c 0 On success or error code
 */
static int32 XM01BC_Exit(
   LL_HANDLE    **llHdlP
)
{
    LL_HANDLE *llHdl = *llHdlP;
	int32 error = 0;

    DBGWRT_1((DBH, "LL - XM01BC_Exit\n"));

    /*------------------------------+
    |  de-init hardware             |
    +------------------------------*/

    /*------------------------------+
    |  clean up memory               |
    +------------------------------*/
	*llHdlP = NULL;		/* set low-level driver handle to NULL */
	error = Cleanup(llHdl,error);

	return(error);
}

/****************************** XM01BC_Read ************************************/
/** Read a value from the device
 *
 *  The function is not supported and always returns an ERR_LL_ILL_FUNC error.
 *
 *  \param llHdl      \IN  Low-level handle
 *  \param ch         \IN  Current channel
 *  \param valueP     \OUT Read value
 *
 *  \return           \c 0 On success or error code
 */
static int32 XM01BC_Read(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 *valueP
)
{
    DBGWRT_1((DBH, "LL - XM01BC_Read: ch=%d\n",ch));

	return(ERR_LL_ILL_FUNC);
}

/****************************** XM01BC_Write ***********************************/
/** Write a value to the device
 *
 *  The function is not supported and always returns an ERR_LL_ILL_FUNC error.
 *
 *  \param llHdl      \IN  Low-level handle
 *  \param ch         \IN  Current channel
 *  \param value      \IN  Read value
 *
 *  \return           \c ERR_LL_ILL_FUNC
 */
static int32 XM01BC_Write(
    LL_HANDLE *llHdl,
    int32 ch,
    int32 value
)
{
    DBGWRT_1((DBH, "LL - XM01BC_Write: ch=%d\n",ch));

	return(ERR_LL_ILL_FUNC);
}

/****************************** XM01BC_SetStat *********************************/
/** Set the driver status
 *
 *  The driver supports \ref getstat_setstat_codes "these status codes"
 *  in addition to the standard codes (see mdis_api.h).
 *
 *  \param llHdl  	       \IN  Low-level handle
 *  \param code            \IN  \ref getstat_setstat_codes "status code"
 *  \param ch              \IN  Current channel
 *  \param value32_or_64   \IN  Data or
 *                              pointer to block data structure
 *                              (M_SG_BLOCK) for block status codes
 *
 *  \return                \c 0 On success or error code
 */
static int32 XM01BC_SetStat(
    LL_HANDLE  *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64  value32_or_64
)
{
	int32	value = (int32) value32_or_64;	/* 32bit value */
	int32	error = ERR_SUCCESS;
	u_int8	byte;

    DBGWRT_1((DBH, "LL - XM01BC_SetStat: ch=%d code=0x%04x value=0x%x\n",
			  ch,code,value));


	switch(code) {

        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_LL_DEBUG_LEVEL:
            llHdl->dbgLevel = value;
            break;


        /*--------------------------+
        |  channel direction        |
        +--------------------------*/
        case M_LL_CH_DIR:
			if( value != M_CH_IN )
				error = ERR_LL_ILL_DIR;
            break;


        /*--------------------------+
        |  start WDOG               |
        +--------------------------*/
        case WDOG_START:
			if( (error = SMBusWriteByte( llHdl, 0, CPUBCI_WDOG_ON )) )
				break;

			/* verify the WDOG start */
			if( (error = SMBusReadByteData( llHdl, 0, CPUBCI_WDOG_STATE, &byte)) )
				break;

			if( !byte ){
				error = ERR_LL_DEV_NOTRDY;
				break;
			}

			llHdl->wdStatus = 1;
            break;


        /*--------------------------+
        |  stop WDOG                |
        +--------------------------*/
        case WDOG_STOP:
			if( (error = SMBusWriteByteData( llHdl, 0, CPUBCI_WDOG_OFF,
						CPUBCI_WDOG_OFF_BYTE )) )
				break;

			/* verify the WDOG stop */
			if( (error = SMBusReadByteData( llHdl, 0, CPUBCI_WDOG_STATE,
						&byte)) )
				break;
			if( byte ){
				error = ERR_LL_DEV_NOTRDY;
				break;
			}
			llHdl->wdStatus = 0;
            break;


        /*--------------------------+
        |  trigger WDOG             |
        +--------------------------*/
        case WDOG_TRIG:
			/* watchdog not enabled? */
			if ( llHdl->wdStatus == 0 ) {
				error = ERR_LL_DEV_NOTRDY;
				break;
			}

			error = SMBusWriteByte( llHdl, 0, CPUBCI_WDOG_TRIG );
            break;


        /*--------------------------+
        |  set WDOG time            |
        +--------------------------*/
        case WDOG_TIME:
			/* Out of range? */
			if( ( (value %  CPUBCI_WDOG_TIME_UNIT) != 0 ) ||
				  (value < (CPUBCI_WDOG_TIME_UNIT * CPUBCI_WDOG_TIME_MIN) ) ||
				  (value > (CPUBCI_WDOG_TIME_UNIT * CPUBCI_WDOG_TIME_MAX) ) )
		    {
				error = ERR_LL_ILL_PARAM;
				break;
			}

			error = SMBusWriteWordData( llHdl, 0, CPUBCI_WDOG_TIME,
									(u_int16)(value / CPUBCI_WDOG_TIME_UNIT) );
            break;


        /*--------------------------+
        |  arm WDOG                 |
        +--------------------------*/
        case XM01BC_WDOG_ARM:
			error = SMBusWriteByte( llHdl, 0, CPUBCI_WDOG_ARM );
			break;



		/*--------------------------+
        |  software warm reset      |
        +--------------------------*/
        case XM01BC_SWRESET:
			/* right keyword? */
			if ( value != CPUBCI_SW_RESET_WORD ) {
				error = ERR_LL_ILL_PARAM;
				break;
			}

			error = SMBusWriteWordData( llHdl, 0, CPUBCI_SW_RESET,
						CPUBCI_SW_RESET_WORD );

            break;


		/*--------------------------+
        |  software cold reset      |
        +--------------------------*/
        case XM01BC_SWCOLDRESET:
			/* right keyword? */
			if ( value != CPUBCI_SW_RESET_WORD ) {
				error = ERR_LL_ILL_PARAM;
				break;
			}

			error = SMBusWriteWordData( llHdl, 0, CPUBCI_SW_COLD_RESET,
						CPUBCI_SW_RESET_WORD );

            break;


	   /*------------------------------------------+
		|  install signal for voltage supervision  |
		+-----------------------------------------*/
		case XM01BC_VOLT_SIGSET:
		{
			u_int32				realMsec;

			/* illegal signal code ? */
			if( value == 0 ){
				DBGWRT_ERR((DBH, " *** XM01BC_SetStat: illegal signal code=0x%x",
					value));
				error = ERR_LL_ILL_PARAM;
				break;
			}

			/* already defined ? */
			if( llHdl->sigHdl != NULL ){
				DBGWRT_ERR((DBH, " *** XM01BC_SetStat: signal already installed"));
				error = ERR_OSS_SIG_SET;
				break;
			}

			/* install signal+alarm */
			if( (error = OSS_SigCreate(llHdl->osHdl, value, &llHdl->sigHdl)) )
				break;

			if( (error = OSS_AlarmSet(llHdl->osHdl, llHdl->alarmHdl,
									  llHdl->pollPeriod, 1, &realMsec)) )
				break;
			break;
		}


		/*---------------------------------------+
		|  deinstall signal for shutdown event   |
		+---------------------------------------*/
		case XM01BC_VOLT_SIGCLR:
			/* not defined ? */
			if( llHdl->sigHdl == NULL ){
				DBGWRT_ERR((DBH, " *** XM01BC_SetStat: signal not installed"));
				error = ERR_OSS_SIG_CLR;
				break;
			}

			/* remove signal+alarm */
			if( (error = OSS_SigRemove(llHdl->osHdl, &llHdl->sigHdl)) )
				break;

			if( (error = OSS_AlarmClear(llHdl->osHdl, llHdl->alarmHdl)) )
				break;

			break;


		/*----------------------------+
		|  clear all error counters   |
		+----------------------------*/
		case XM01BC_ERRCNT_CLR:
			error = SMBusWriteByteData( llHdl, 0, CPUBCI_ERR_CNT_CLR, CPUBCI_ERR_CNT_BYTE );
			break;


		/*----------------------------+
		|  clear all error registers  |
		+----------------------------*/
		case XM01BC_ERR_REG_CLR:
			error = SMBusWriteByteData( llHdl, 0, CPUBCI_ERR_REG_CLR, CPUBCI_ERR_REG_BYTE );
			break;


		/*----------------------------+
		|  set LED state              |
		+----------------------------*/
		case XM01BC_LED_CTRL:
			error = SMBusWriteByteData( llHdl, 0, CPUBCI_LED_CTRL, (u_int8)value );
			break;


		/*----------------------------+
		|  unknown                    |
		+----------------------------*/
		default:
			error = ERR_LL_UNK_CODE;

	} /* switch( code ) */

	return(error);
}

/****************************** XM01BC_GetStat *********************************/
/** Get the driver status
 *
 *  The driver supports \ref getstat_setstat_codes "these status codes"
 *  in addition to the standard codes (see mdis_api.h).
 *
 *  \param llHdl           \IN  Low-level handle
 *  \param code            \IN  \ref getstat_setstat_codes "status code"
 *  \param ch              \IN  Current channel
 *  \param value32_or_64P  \IN  Pointer to block data structure
 *                              (M_SG_BLOCK) for block status codes
 *  \param value32_or_64P  \OUT Pointer to data or
 *                              Pointer to block data structure
 *                              (M_SG_BLOCK) for block status codes
 *
 *  \return                \c 0 On success or error code
 */
static int32 XM01BC_GetStat(
    LL_HANDLE *llHdl,
    int32  code,
    int32  ch,
    INT32_OR_64  *value32_or_64P
)
{
	int32		*valueP		= (int32*) value32_or_64P;		/* pointer to 32bit value */
	INT32_OR_64	*value64P	= value32_or_64P;				/* stores 32/ 64bit pointer */
	M_SG_BLOCK 	*blk 		= (M_SG_BLOCK*)value32_or_64P;	/* stores block struct pointer */

	int32	error = ERR_SUCCESS;
	u_int16	word;
	u_int8	byte, c = (u_int8)ch;

    DBGWRT_1((DBH, "LL - XM01BC_GetStat: ch=%d code=0x%04x\n",
			  ch,code));

    switch(code)
    {
        /*--------------------------+
        |  debug level              |
        +--------------------------*/
        case M_LL_DEBUG_LEVEL:
            *valueP = llHdl->dbgLevel;
            break;
        /*--------------------------+
        |  number of channels       |
        +--------------------------*/
        case M_LL_CH_NUMBER:
            *valueP = llHdl->numVoltages;
            break;
        /*--------------------------+
        |  channel direction        |
        +--------------------------*/
        case M_LL_CH_DIR:
            *valueP = M_CH_IN;
            break;
        /*--------------------------+
        |  channel length [bits]    |
        +--------------------------*/
        case M_LL_CH_LEN:
            *valueP = 32;
            break;
        /*--------------------------+
        |  channel type info        |
        +--------------------------*/
        case M_LL_CH_TYP:
            *valueP = M_CH_PROFILE_WDOG;
            break;
        /*--------------------------+
        |   ident table pointer     |
        |   (treat as non-block!)   |
        +--------------------------*/
        case M_MK_BLK_REV_ID:
           *value64P = (INT32_OR_64)&llHdl->idFuncTbl;
           break;


		/*--------------------------+
		|  get WDOG time            |
		+--------------------------*/
		case WDOG_TIME:
			error = SMBusReadWordData( llHdl, 0, CPUBCI_WDOG_TIME, &word);
			*valueP = (u_int32)(word * CPUBCI_WDOG_TIME_UNIT);
			break;


		/*--------------------------+
		|  get WDOG state           |
		+--------------------------*/
		case WDOG_STATUS:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_WDOG_STATE, &byte);
			*valueP = (u_int32)byte;
			break;


        /*--------------------------+
        |  WDOG shot                |
        +--------------------------*/
        case WDOG_SHOT:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERR_RST_RSN, &byte);
            if( llHdl->hwVariant != CPUBCI_HW_VAR_XM01_1 )
                *valueP = (int32)(byte == CPUBCI_RST_EXT_WDOG ? 1 : 0);
            else
                *valueP = (int32)(byte == XM01BCI_RST_EXT_WDOG ? 1 : 0);

            break;


		/*--------------------------+
		|  get input voltage        |
		+--------------------------*/
		case XM01BC_VOLT_ACT:
			error = SMBusReadWordData( llHdl, 0, CPUBCI_VOLT_ACT(c), &word);
			*valueP = (u_int32)word;
			break;


		/*--------------------------+
		|  get lower voltage limit  |
		+--------------------------*/
		case XM01BC_VOLT_LOW:
			error = SMBusReadWordData( llHdl, 0, CPUBCI_VOLT_LOW(c), &word);
			*valueP = (u_int32)word;
			break;


		/*--------------------------+
		|  get upper voltage limit  |
		+--------------------------*/
		case XM01BC_VOLT_HIGH:
			error = SMBusReadWordData( llHdl, 0, CPUBCI_VOLT_HIGH(c), &word);
			*valueP = (u_int32)word;
			break;


		/*--------------------------+
		|  get supervision state    |
		+--------------------------*/
		case XM01BC_VOLT_SVSTATE:
			*valueP = (int32)llHdl->voltSvState;
			break;


		/*--------------------------+
		|  get error counter 00     |
		+--------------------------*/
		case XM01BC_ERRCNT_00:
             if( (int8)(llHdl->numErrCntrs) >= 0 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(0), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 01     |
		+--------------------------*/
		case XM01BC_ERRCNT_01:
             if( llHdl->numErrCntrs >= 1 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(1), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 02     |
		+--------------------------*/
		case XM01BC_ERRCNT_02:
             if( llHdl->numErrCntrs >= 2 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(2), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 03     |
		+--------------------------*/
		case XM01BC_ERRCNT_03:
             if( llHdl->numErrCntrs >= 3 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(3), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 04     |
		+--------------------------*/
		case XM01BC_ERRCNT_04:
             if( llHdl->numErrCntrs >= 4 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(4), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 05     |
		+--------------------------*/
		case XM01BC_ERRCNT_05:
             if( llHdl->numErrCntrs >= 5 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(5), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 06     |
		+--------------------------*/
		case XM01BC_ERRCNT_06:
             if( llHdl->numErrCntrs >= 6 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(6), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 07     |
		+--------------------------*/
		case XM01BC_ERRCNT_07:
             if( llHdl->numErrCntrs >= 7 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(7), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 08     |
		+--------------------------*/
		case XM01BC_ERRCNT_08:
             if( llHdl->numErrCntrs >= 8 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(8), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 09     |
		+--------------------------*/
		case XM01BC_ERRCNT_09:
             if( llHdl->numErrCntrs >= 9 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(9), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 10     |
		+--------------------------*/
		case XM01BC_ERRCNT_10:
             if( llHdl->numErrCntrs >= 10 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(10), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 11     |
		+--------------------------*/
		case XM01BC_ERRCNT_11:
             if( llHdl->numErrCntrs >= 11 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(11), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 12     |
		+--------------------------*/
		case XM01BC_ERRCNT_12:
             if( llHdl->numErrCntrs >= 12 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(12), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 13     |
		+--------------------------*/
		case XM01BC_ERRCNT_13:
             if( llHdl->numErrCntrs >= 13 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(13), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 14     |
		+--------------------------*/
		case XM01BC_ERRCNT_14:
             if( llHdl->numErrCntrs >= 14 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(14), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 15     |
		+--------------------------*/
		case XM01BC_ERRCNT_15:
             if( llHdl->numErrCntrs >= 15 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(15), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 16     |
		+--------------------------*/
		case XM01BC_ERRCNT_16:
             if( llHdl->numErrCntrs >= 16 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(16), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 17     |
		+--------------------------*/
		case XM01BC_ERRCNT_17:
             if( llHdl->numErrCntrs >= 17 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(17), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 18     |
		+--------------------------*/
		case XM01BC_ERRCNT_18:
             if( llHdl->numErrCntrs >= 18 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(18), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 19     |
		+--------------------------*/
		case XM01BC_ERRCNT_19:
             if( llHdl->numErrCntrs >= 19 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(19), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 20     |
		+--------------------------*/
		case XM01BC_ERRCNT_20:
             if( llHdl->numErrCntrs >= 20 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(20), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 21     |
		+--------------------------*/
		case XM01BC_ERRCNT_21:
             if( llHdl->numErrCntrs >= 21 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(21), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 22     |
		+--------------------------*/
		case XM01BC_ERRCNT_22:
             if( llHdl->numErrCntrs >= 22 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(22), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 23     |
		+--------------------------*/
		case XM01BC_ERRCNT_23:
             if( llHdl->numErrCntrs >= 23 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(23), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 24     |
		+--------------------------*/
		case XM01BC_ERRCNT_24:
             if( llHdl->numErrCntrs >= 24 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(24), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 25     |
		+--------------------------*/
		case XM01BC_ERRCNT_25:
             if( llHdl->numErrCntrs >= 25 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(25), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 26     |
		+--------------------------*/
		case XM01BC_ERRCNT_26:
             if( llHdl->numErrCntrs >= 26 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(26), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 27     |
		+--------------------------*/
		case XM01BC_ERRCNT_27:
             if( llHdl->numErrCntrs >= 27 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(27), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 28     |
		+--------------------------*/
		case XM01BC_ERRCNT_28:
             if( llHdl->numErrCntrs >= 28 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(28), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 29     |
		+--------------------------*/
		case XM01BC_ERRCNT_29:
            if( llHdl->numErrCntrs >= 29 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(29), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get error counter 30     |
		+--------------------------*/
		case XM01BC_ERRCNT_30:
             if( llHdl->numErrCntrs >= 30 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERRCNT(30), &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    error = ERR_LL_ILL_FUNC;
			break;


		/*--------------------------+
		|  get last error           |
		+--------------------------*/
		case XM01BC_ERR_LAST:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERR_LAST, &byte);
			*valueP = (u_int32)byte;
			break;


		/*--------------------------+
		|  get last reset reason    |
		+--------------------------*/
		case XM01BC_ERR_RST_RSN:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_ERR_RST_RSN, &byte);
			*valueP = (u_int32)byte;
			break;


		/*--------------------------+
		|  get power failure flags  |
		+--------------------------*/
		case XM01BC_ERR_PWR_FLGS:
			error = SMBusReadByteData(llHdl, 0, CPUBCI_ERR_PWR_FLAGS, &byte);
			*valueP = (u_int32)byte;
			break;


		/*--------------------------+
		|  get power cycle counter  |
		+--------------------------*/
		case XM01BC_PWRCYCL_CNT:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_PWRCYCL_CNT, &byte);
			*valueP = (u_int32)byte;
			break;


		/*------------------------------+
		|  get operating hours counter  |
		+------------------------------*/
		case XM01BC_OPHRS_CNT:
			error = SMBusReadWordData(llHdl, 0, CPUBCI_OPHRS_CNT, &word);
			*valueP = (u_int32)word;
			break;


		/*--------------------------+
		|  get LED state            |
		+--------------------------*/
		case XM01BC_LED_CTRL:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_LED_CTRL, &byte);
			*valueP = (u_int32)byte;
			break;


		/*--------------------------+
		|  get EXT_PWR_OK mode      |
		+--------------------------*/
		case XM01BC_EXT_PWR_OK_MODE:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_EXT_PWR_OK_MODE, &byte);
			*valueP = (u_int32)byte;
			break;


		/*--------------------------+
		|  get power resume mode    |
		+--------------------------*/
		case XM01BC_RESUME_MODE:
			error = SMBusReadByteData( llHdl, 0, CPUBCI_RESUME_MODE, &byte);
			*valueP = (u_int32)byte;
			break;


		/*--------------------------+
		|  get RESET_IN mode        |
		+--------------------------*/
		case XM01BC_RST_IN_MODE:
            if( llHdl->hwVariant != CPUBCI_HW_VAR_XM01
                && llHdl->hwVariant != CPUBCI_HW_VAR_XM01_1 )
		    {
    			error = SMBusReadByteData( llHdl, 0, CPUBCI_RST_IN_MODE, &byte);
    			*valueP = (u_int32)byte;
		    }
		    else
    		    *valueP = (u_int32)XM01BC_RST_IN_ENABLED;
			break;


		/*--------------------------+
		|  get firmware revision    |
		+--------------------------*/
		case XM01BC_BLK_FWREV:
		{
			char*		dataP = (char*)blk->data;

			/* check blk size */
			if( blk->size < XM01BC_BLK_FWREV_SIZE ) {
				error = ERR_LL_USERBUF;
				break;
			}

			dataP[0] = ((llHdl->fwRev[0] / 10 ) % 10 ) + 0x30;
			dataP[1] = (llHdl->fwRev[0] % 10 ) + 0x30;
			dataP[2] = ((llHdl->fwRev[1] / 10 ) % 10 ) + 0x30;
			dataP[3] = (llHdl->fwRev[1] % 10 ) + 0x30;
			dataP[4] = ((llHdl->fwRev[2] / 10 ) % 10 ) + 0x30;
			dataP[5] = (llHdl->fwRev[2] % 10 ) + 0x30;

		}
		break;


		/*---------------------------------+
		|  get firmware revision extended  |
		+---------------------------------*/
		case XM01BC_BLK_FWREV_EXT:
		{
			u_int16*	dataP = (u_int16*)blk->data;

			/* check blk size */
			if( blk->size < XM01BC_BLK_FWREV_SIZE ) {
				error = ERR_LL_USERBUF;
				break;
			}

			dataP[0] = llHdl->fwRev[0];
			dataP[1] = llHdl->fwRev[1];
			dataP[2] = llHdl->fwRev[2];

		}
		break;


		/*--------------------------+
		|  get hardware variant     |
		+--------------------------*/
		case XM01BC_HW_VARIANT:
            *valueP = (u_int32)llHdl->hwVariant;
			break;


		/*-------------------------------+
		|  get number of error counters  |
		+-------------------------------*/
		case XM01BC_NUM_ERRCNTRS:
            *valueP = (u_int32)llHdl->numErrCntrs;
			break;


		/*--------------------------+
		|  unknown                  |
		+--------------------------*/
		default:
			error = ERR_LL_UNK_CODE;
	}

	return(error);
}

/******************************* XM01BC_BlockRead ******************************/
/** Read a data block from the device
 *
 *  The function is not supported and always returns an ERR_LL_ILL_FUNC error.
 *
 *  \param llHdl       \IN  Low-level handle
 *  \param ch          \IN  Current channel
 *  \param buf         \IN  Data buffer
 *  \param size        \IN  Data buffer size
 *  \param nbrRdBytesP \OUT Number of read bytes
 *
 *  \return            \c 0 On success or error code
 */
static int32 XM01BC_BlockRead(
     LL_HANDLE *llHdl,
     int32     ch,
     void      *buf,
     int32     size,
     int32     *nbrRdBytesP
)
{
    DBGWRT_1((DBH, "LL - XM01BC_BlockRead: ch=%d, size=%d\n",ch,size));

	/* return number of read bytes */
	*nbrRdBytesP = 0;

	return(ERR_LL_ILL_FUNC);
}

/****************************** XM01BC_BlockWrite ******************************/
/** Write a data block from the device
 *
 *  The function is not supported and always returns an ERR_LL_ILL_FUNC error.
 *
 *  \param llHdl  	   \IN  Low-level handle
 *  \param ch          \IN  Current channel
 *  \param buf         \IN  Data buffer
 *  \param size        \IN  Data buffer size
 *  \param nbrWrBytesP \OUT Number of written bytes
 *
 *  \return            \c ERR_LL_ILL_FUNC
 */
static int32 XM01BC_BlockWrite(
     LL_HANDLE *llHdl,
     int32     ch,
     void      *buf,
     int32     size,
     int32     *nbrWrBytesP
)
{
    DBGWRT_1((DBH, "LL - XM01BC_BlockWrite: ch=%d, size=%d\n",ch,size));

	/* return number of written bytes */
	*nbrWrBytesP = 0;

	return(ERR_LL_ILL_FUNC);
}


/****************************** XM01BC_Irq ************************************/
/** Interrupt service routine - unused
 *
 *  If the driver can detect the interrupt's cause it returns
 *  LL_IRQ_DEVICE or LL_IRQ_DEV_NOT, otherwise LL_IRQ_UNKNOWN.
 *
 *  \param llHdl  	   \IN  Low-level handle
 *  \return LL_IRQ_DEVICE	IRQ caused by device
 *          LL_IRQ_DEV_NOT  IRQ not caused by device
 *          LL_IRQ_UNKNOWN  Unknown
 */
static int32 XM01BC_Irq(
   LL_HANDLE *llHdl
)
{
	return(LL_IRQ_DEV_NOT);
}

/****************************** XM01BC_Info ***********************************/
/** Get information about hardware and driver requirements
 *
 *  The following info codes are supported:
 *
 * \code
 *  Code                      Description
 *  ------------------------  -----------------------------
 *  LL_INFO_HW_CHARACTER      Hardware characteristics
 *  LL_INFO_ADDRSPACE_COUNT   Number of required address spaces
 *  LL_INFO_ADDRSPACE         Address space information
 *  LL_INFO_IRQ               Interrupt required
 *  LL_INFO_LOCKMODE          Process lock mode required
 * \endcode
 *
 *  The LL_INFO_HW_CHARACTER code returns all address and
 *  data modes (ORed) which are supported by the hardware
 *  (MDIS_MAxx, MDIS_MDxx).
 *
 *  The LL_INFO_ADDRSPACE_COUNT code returns the number
 *  of address spaces used by the driver.
 *
 *  The LL_INFO_ADDRSPACE code returns information about one
 *  specific address space (MDIS_MAxx, MDIS_MDxx). The returned
 *  data mode represents the widest hardware access used by
 *  the driver.
 *
 *  The LL_INFO_IRQ code returns whether the driver supports an
 *  interrupt routine (TRUE or FALSE).
 *
 *  The LL_INFO_LOCKMODE code returns which process locking
 *  mode the driver needs (LL_LOCK_xxx).
 *
 *  \param infoType	   \IN  Info code
 *  \param ...         \IN  Argument(s)
 *
 *  \return            \c 0 On success or error code
 */
static int32 XM01BC_Info(
   int32  infoType,
   ...
)
{
    int32   error = ERR_SUCCESS;
    va_list argptr;

    va_start(argptr, infoType );

    switch(infoType) {
		/*-------------------------------+
        |  hardware characteristics      |
        |  (all addr/data modes ORed)   |
        +-------------------------------*/
        case LL_INFO_HW_CHARACTER:
		{
			u_int32 *addrModeP = va_arg(argptr, u_int32*);
			u_int32 *dataModeP = va_arg(argptr, u_int32*);

			*addrModeP = MDIS_MA08;
			*dataModeP = MDIS_MD08 | MDIS_MD16;
			break;
	    }
		/*-------------------------------+
        |  nr of required address spaces |
        |  (total spaces used)           |
        +-------------------------------*/
        case LL_INFO_ADDRSPACE_COUNT:
		{
			u_int32 *nbrOfAddrSpaceP = va_arg(argptr, u_int32*);

			*nbrOfAddrSpaceP = ADDRSPACE_COUNT;
			break;
	    }
		/*-------------------------------+
        |  address space type            |
        |  (widest used data mode)       |
        +-------------------------------*/
        case LL_INFO_ADDRSPACE:
		{
			u_int32 addrSpaceIndex = va_arg(argptr, u_int32);
			u_int32 *addrModeP = va_arg(argptr, u_int32*);
			u_int32 *dataModeP = va_arg(argptr, u_int32*);
			u_int32 *addrSizeP = va_arg(argptr, u_int32*);

			if((int32)addrSpaceIndex >= ADDRSPACE_COUNT)
				error = ERR_LL_ILL_PARAM;
			else {
				*addrModeP = MDIS_MA08;
				*dataModeP = MDIS_MD16;
				*addrSizeP = ADDRSPACE_SIZE;
			}

			break;
	    }
		/*-------------------------------+
        |   interrupt required           |
        +-------------------------------*/
        case LL_INFO_IRQ:
		{
			u_int32 *useIrqP = va_arg(argptr, u_int32*);

			*useIrqP = USE_IRQ;
			break;
	    }
		/*-------------------------------+
        |   process lock mode            |
        +-------------------------------*/
        case LL_INFO_LOCKMODE:
		{
			u_int32 *lockModeP = va_arg(argptr, u_int32*);

			*lockModeP = LL_LOCK_CALL;
			break;
	    }
		/*-------------------------------+
        |   (unknown)                    |
        +-------------------------------*/
        default:
          error = ERR_LL_ILL_PARAM;
    }

    va_end(argptr);
    return(error);
}

/*******************************  Ident  ***********************************/
/** Return ident string
 *
 *  \return            Pointer to ident string
 */
static char* Ident( void )
{
    return( "XM01BC - XM01BC low-level driver: "
            "$Id: xm01bc_drv.c,v 1.4 2015/03/11 18:15:47 ts Exp $" );
}

/********************************* Cleanup *********************************/
/** Close all handles, free memory and return error code
 *
 *	\warning The low-level handle is invalid after this function is called.
 *
 *  \param llHdl      \IN  Low-level handle
 *  \param retCode    \IN  Return value
 *
 *  \return           \IN   retCode
 */
static int32 Cleanup(
   LL_HANDLE    *llHdl,
   int32        retCode
)
{
    /*------------------------------+
    |  close handles                |
    +------------------------------*/
	/* clean up desc */
	if(llHdl->descHdl)
		DESC_Exit(&llHdl->descHdl);

	/* clean up alarm */
	if (llHdl->alarmHdl)
		OSS_AlarmRemove(llHdl->osHdl, &llHdl->alarmHdl);

	/* clean up signal */
	if (llHdl->sigHdl)
		OSS_SigRemove(llHdl->osHdl, &llHdl->sigHdl);

	/* clean up debug */
	DBGEXIT((&DBH));

    /*------------------------------+
    |  free memory                  |
    +------------------------------*/
    /* free my handle */
    OSS_MemFree(llHdl->osHdl, (int8*)llHdl, llHdl->memAlloc);

    /*------------------------------+
    |  return error code            |
    +------------------------------*/
	return(retCode);
}

/******************************* AlarmHandler *******************************
 *
 *  Description: Handler for alarm
 *
 *---------------------------------------------------------------------------
 *  Input......: arg		ll handle
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void AlarmHandler(void *arg)
{
	LL_HANDLE	*llHdl = (LL_HANDLE*)arg;
	int32		error;
	u_int8		pwrFlags;
	u_int16		lowerLimit;
	u_int16		upperLimit;
	u_int16		actualValue;
	u_int8		currChannel;

	DBGWRT_3((DBH,">>> LL - XM01BC AlarmHandler:\n"));

	pwrFlags = 0;

	for( currChannel=0; currChannel < llHdl->numVoltages; currChannel++ )
	{
		if( (error = llHdl->smbH->ReadWordData( llHdl->smbH, 0, llHdl->smbAddr,
					CPUBCI_VOLT_LOW(currChannel), &lowerLimit)) ) {
			break;
		}
		if( (error = llHdl->smbH->ReadWordData( llHdl->smbH, 0, llHdl->smbAddr,
						CPUBCI_VOLT_HIGH(currChannel), &upperLimit)) ) {
			break;
		}
		if( (error = llHdl->smbH->ReadWordData( llHdl->smbH, 0, llHdl->smbAddr,
						CPUBCI_VOLT_ACT(currChannel), &actualValue)) ) {
			break;
		}
		if( lowerLimit && (actualValue < lowerLimit) ) {
			pwrFlags |= (0x01 << currChannel);
		}
		if( upperLimit && (actualValue > upperLimit) ) {
			pwrFlags |= (0x01 << currChannel);
		}
	}

	if( !error && pwrFlags ){
		llHdl->voltSvState = pwrFlags;
		DBGWRT_2((DBH, " voltSvState=0x%x --> send signal\n",
			llHdl->voltSvState));
		OSS_SigSend( llHdl->osHdl, llHdl->sigHdl );
	}
}


/***************************** SMBusWriteByte ******************************
 *
 *  Description: Wrapper function for SMBus Write Byte Data Transfers
 *
 *---------------------------------------------------------------------------
 *  \param llHdl       \IN  valid LL_HANDLE
 *	\param flags       \IN  flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param data        \OUT pointer for data read
 *
 *  \return            \c 0 On success or error code
 ****************************************************************************/
static int32 SMBusWriteByte( LL_HANDLE *llHdl, u_int32 flags, u_int8 data )
{
	int32	result;

	result = llHdl->smbH->WriteByte(llHdl->smbH, flags, llHdl->smbAddr, data);

	if( llHdl->fwRevFirst )
	{
		/* Insert additional Bus idle time */
		OSS_Delay( llHdl->osHdl, BUS_IDLE_DELAY );
	}

	return result;
}



/***************************** SMBusWriteByteData *****************************
 *
 *  Description: Wrapper function for SMBus Write Byte Data Transfers
 *
 *---------------------------------------------------------------------------
 *  \param llHdl       \IN  valid LL_HANDLE
 *	\param flags       \IN  flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param cmdAddr     \IN  device command or index value to write
 *	\param data        \IN  data
 *
 *  \return            \c 0 On success or error code
 ****************************************************************************/
static int32 SMBusWriteByteData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int8 data )
{
	int32	result;

	result = llHdl->smbH->WriteByteData(llHdl->smbH, flags, llHdl->smbAddr,
											cmdAddr, data);

	if( llHdl->fwRevFirst )
	{
		/* Insert additional Bus idle time */
		OSS_Delay( llHdl->osHdl, BUS_IDLE_DELAY );
	}

	return result;
}



/***************************** SMBusReadByteData *****************************
 *
 *  Description: Wrapper function for SMBus Read Byte Data Transfers
 *
 *---------------------------------------------------------------------------
 *  \param llHdl       \IN  valid LL_HANDLE
 *	\param flags       \IN  flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param cmdAddr     \IN  device command or index value to write
 *	\param data        \OUT pointer for data read
 *
 *  \return            \c 0 On success or error code
 ****************************************************************************/
static int32 SMBusReadByteData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int8 *data )
{
	int32	result;

	result = llHdl->smbH->ReadByteData(llHdl->smbH, flags, llHdl->smbAddr,
											cmdAddr, data);

	if( llHdl->fwRevFirst )
	{
		/* Insert additional Bus idle time */
		OSS_Delay( llHdl->osHdl, BUS_IDLE_DELAY );
	}

	return result;
}


/***************************** SMBusWriteWordData *****************************
 *
 *  Description: Wrapper function for SMBus Write Word Data Transfers
 *
 *---------------------------------------------------------------------------
 *  \param llHdl       \IN  valid LL_HANDLE
 *	\param flags       \IN  flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param cmdAddr     \IN  device command or index value to write
 *	\param data        \IN  data
 *
 *  \return            \c 0 On success or error code
 ****************************************************************************/
static int32 SMBusWriteWordData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int16 data )
{
	int32	result;

	result = llHdl->smbH->WriteWordData(llHdl->smbH, flags, llHdl->smbAddr,
											cmdAddr, data);

	if( llHdl->fwRevFirst )
	{
		/* Insert additional Bus idle time */
		OSS_Delay( llHdl->osHdl, BUS_IDLE_DELAY );
	}

	return result;
}



/***************************** SMBusReadWordData *****************************
 *
 *  Description: Wrapper function for SMBus Read Word Data Transfers
 *
 *---------------------------------------------------------------------------
 *  \param llHdl       \IN  valid LL_HANDLE
 *	\param flags       \IN  flags indicating RX/TX mode (10-bit addr, PEC, ...)
 *	\param cmdAddr     \IN  device command or index value to write
 *	\param data        \OUT pointer for data read
 *
 *  \return            \c 0 On success or error code
 ****************************************************************************/
static int32 SMBusReadWordData( LL_HANDLE *llHdl, u_int32 flags,
									u_int8 cmdAddr, u_int16 *data )
{
	int32	result;

	result = llHdl->smbH->ReadWordData(llHdl->smbH, flags, llHdl->smbAddr,
											cmdAddr, data);

	if( llHdl->fwRevFirst )
	{
		/* Insert additional Bus idle time */
		OSS_Delay( llHdl->osHdl, BUS_IDLE_DELAY );
	}

	return result;
}

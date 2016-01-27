/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  calls.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2008/09/12 16:26:55 $
 *    $Revision: 1.3 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  Implementation of MDIS system calls
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: calls.c,v $
 * Revision 1.3  2008/09/12 16:26:55  gvarlet
 * R: Type "native_int" no longer exists
 * M: Change from native_int to u_int32
 *
 * Revision 1.2  2004/03/25 08:50:34  ub
 * added function comments
 *
 * Revision 1.1  2004/03/24 11:35:12  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*--------------------------------------+
|  INCLUDES                             |
+---------------------------------------*/
#include "mk_intern.h"


/*--------------------------------------+
|  DEFINES                              |
+---------------------------------------*/
/* add idStr+NL to idBuf if doExec */
#define ADD_STRING(idStr, idBuf, doExec)    { \
				if (doExec) strcpy(idBuf,idStr); \
				idBuf += strlen(idStr); \
				if (doExec) strcpy(idBuf,"\n"); \
				idBuf += strlen("\n"); \
				}

/*--------------------------------------+
|  TYPEDEFS                             |
+---------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int32 getRevId(MK_DEV *dev, char *buf, u_int32 *size, u_int32 doExec);


/**********************************************************************/
/** Read data from MDIS device
 *
 *  Calls the driver's read routine via driver jump table.
 *  Reads a single 32bit value from device.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  valP      \OUT  result pointer
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

int32
MDIS_Read( MK_PATH *path, int32 *valP )
{
	MK_DEV *dev = path->dev;
	int32 error;

    DBGWRT_1((DBH, "+++ MDIS_Read: dev=0x%p\n", dev ));

	if( (error = MDIS_DevLock( path, mdis_read ))) 
        return( -error );

	error = dev->llJumpTbl.read( dev->ll, path->chan, valP );

	MDIS_DevUnLock( path, mdis_read );

	/* increment channel */
	if (path->ioMode==M_IO_EXEC_INC && error==0)
		if (++(path->chan) == dev->devNrChan)
			path->chan = 0;

    return( -error );
}


/**********************************************************************/
/** Write data to MDIS device
 *
 *  Calls the driver's write routine via driver jump table.
 *  Writes a single 32bit value to device.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  val       \IN   value to write
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

int32
MDIS_Write( MK_PATH *path, int32 val )
{
	MK_DEV *dev = path->dev;
	int32 error;

    DBGWRT_1((DBH, "+++ MDIS_Write: dev=0x%p  val=%d\n", dev, val ));

	if( (error = MDIS_DevLock( path, mdis_write ))) 
        return( -error );

	error = dev->llJumpTbl.write(dev->ll, path->chan, val);

	MDIS_DevUnLock( path, mdis_write );

	/* increment channel */
	if (path->ioMode==M_IO_EXEC_INC && error==0)
		if (++(path->chan) == dev->devNrChan)
			path->chan = 0;
	
    return( -error );
}

/**********************************************************************/
/** Read data block from MDIS device
 *
 *  Calls the driver's blockRead routine via driver jump table.
 *  Reads a data block of arbitrary length.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  buffer    \OUT  pointer to result buffer
 *  \param  length    \IN   length of buffer (in bytes)
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

int32
MDIS_Getblock( MK_PATH *path, u_int8 *buffer, int32 length )
{
	MK_DEV *dev = path->dev;
	int32 error, readCount;

    DBGWRT_1((DBH, "+++ MDIS_Getblock: dev=0x%p  length=%d\n", dev, length ));

	if( (error = MDIS_DevLock( path, mdis_getblock )) == 0 ) {

		/*--- call LL driver's blockRead ---*/
		error = dev->llJumpTbl.blockRead( dev->ll, path->chan, buffer,
										  length, &readCount);
		MDIS_DevUnLock( path, mdis_getblock );
	}

    return( error ? -error : readCount );
}

/**********************************************************************/
/** Write data block to MDIS device
 *
 *  Calls the driver's blockWrite routine via driver jump table.
 *  Writes a data block of arbitrary length.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  buffer    \IN   pointer to buffer containing data to write
 *  \param  length    \IN   length of buffer (in bytes)
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 */

int32
MDIS_Setblock( MK_PATH *path, u_int8 *buffer, int32 length )
{
	MK_DEV *dev = path->dev;
	int32 error, writeCount;

    DBGWRT_1((DBH, "+++ MDIS_Setblock: dev=0x%p  length=%d\n", dev, length ));

    if( (error = MDIS_DevLock( path, mdis_setblock )) == 0 ) {

        /*--- call LL driver's blockWrite ---*/
        error = dev->llJumpTbl.blockWrite( dev->ll, path->chan, buffer,
                                           length, &writeCount);
        MDIS_DevUnLock( path, mdis_setblock );
    }

    return( error ? -error : writeCount );
}


/**********************************************************************/
/** Create revision ident string and/or calculate size
 *
 *  detailed description 
 *
 *  \param  dev    \IN    MK device struct
 *  \param  buf    \OUT   destination string buffer or NULL
 *  \param  size   \OUT   size of ident string
 *  \param  doExec \IN    0 = only size is returned
 *                        1 = string is created in buf
 *
 *  \return pointer to ident string
 */

static int32 
getRevId(MK_DEV *dev, char *buf, u_int32 *size, u_int32 doExec)
{
	MDIS_IDENT_FUNCT_TBL *funcTbl;
	char *idBuf = buf;
	int32 error;
	u_int32 i;

	DBGWRT_1((DBH," getRevId: exec=%d\n",doExec));

	/*-----------------------------+
    |  MK ident strings            |
    +-----------------------------*/
	ADD_STRING("-----[ MDIS Kernel ]-----", idBuf, doExec);
	ADD_STRING(MDIS_Ident(), idBuf, doExec);	/* Kernel */

	/*-----------------------------+
    |  LL ident strings            |
    +-----------------------------*/
	ADD_STRING("-----[ Low Level Driver ]-----", idBuf, doExec);

	/* get function table ptr */
    if ((error = dev->llJumpTbl.getStat(dev->ll, M_MK_BLK_REV_ID, 0, 
									   (int32*)&funcTbl))) {
		DBGWRT_ERR((DBH," *** getRevId: can't get LL ident table\n"));
		return(error);
	}

	DBGDMP_3((DBH," functTbl",funcTbl,sizeof(MDIS_IDENT_FUNCT_TBL),4));

	/* call ident functions */
    for (i=0; i<=MDIS_MAX_IDCALLS; i++) {
        if (funcTbl->idCall[i].identCall == NULL )		/* last entry ? */
           break;										/* all done */

		ADD_STRING(funcTbl->idCall[i].identCall(), idBuf, doExec);
	}

	/*-----------------------------+
    |  BK ident strings            |
    +-----------------------------*/
	ADD_STRING("-----[ BBIS Kernel ]-----", idBuf, doExec);
	ADD_STRING(bbis_ident(), idBuf, doExec);	/* Kernel */

	ADD_STRING("-----[ BBIS Driver ]-----", idBuf, doExec);

	if( (error = dev->brdJumpTbl.getStat( dev->brd, 0, M_MK_BLK_REV_ID, 
										  (int32*)&funcTbl ))) {
		DBGWRT_ERR((DBH," *** getRevId: can't get BB ident table\n"));
		return(error);
	}

    for (i=0; i<=MDIS_MAX_IDCALLS; i++) {
        if (funcTbl->idCall[i].identCall == NULL )		/* last entry ? */
           break;										/* all done */

		ADD_STRING(funcTbl->idCall[i].identCall(), idBuf, doExec);
	}

	/* NULL terminator */
	if (doExec) *idBuf = 0;
	idBuf++;

	/* calculate size */
	*size = idBuf-buf;
	DBGWRT_1((DBH,"MK - getRevId end: size=%d exec=%d\n",*size, doExec));

	return( ERR_SUCCESS );
}



/**********************************************************************/
/** Handle getstat codes of MDIS kernel
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  valueP    \OUT  for std codes: ptr to value
 *						    for blk codes: M_SG_BLOCK struct
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llGetStat bbGetStat
 */

static int32
mkGetStat(MK_PATH *mkPath, int32 code, int32 *valueP)
{
	int32 error = 0;
	M_SG_BLOCK *blk = (M_SG_BLOCK*)valueP;
	MK_DEV *dev = mkPath->dev;

	DBGWRT_2((DBH," M_MK_xxx code\n"));

	/*------------------------------+
	| handle code                   |
	+------------------------------*/
    switch (code) {
		/*---------------------+
        |  STD codes           |
        +---------------------*/
        case M_MK_NBR_ADDR_SPACE:
		    *valueP = dev->spaceCnt;
            break;

        case M_MK_CH_CURRENT:
            *valueP = mkPath->chan;
            break;

        case M_MK_IO_MODE:
            *valueP = mkPath->ioMode;
            break;

        case M_MK_IRQ_ENABLE:
		    *valueP = dev->irqEnable;
            break;

        case M_MK_IRQ_COUNT:
		    *valueP = dev->irqCnt;
            break;

        case M_MK_DEBUG_LEVEL:
		    *valueP = DBG_MYLEVEL;
            break;

        case M_MK_API_DEBUG_LEVEL:
		    *valueP = DBG_OFF;
            break;

        case M_MK_OSS_DEBUG_LEVEL:
		    *valueP = OSS_DbgLevelGet(dev->osh);
            break;

        case M_MK_LOCKMODE:
		    *valueP = dev->lockMode;
            break;

        case M_MK_PATHCNT:
		    *valueP = dev->useCount;
            break;

        case M_MK_DEV_SLOT:
		    *valueP = dev->devSlot;
            break;

        case M_MK_DEV_ADDRMODE:
		    *valueP = dev->devAddrMode;
            break;

        case M_MK_DEV_DATAMODE:
		    *valueP = dev->devDataMode;
            break;

        case M_MK_BUSTYPE:
		    *valueP = dev->brdBusType;
            break;

        case M_MK_IRQ_INFO:
		    *valueP = dev->irqInfo;
            break;

        case M_MK_IRQ_MODE:
		    *valueP = dev->irqMode;
            break;

        case M_MK_IRQ_INSTALLED:
		    *valueP = dev->irqInstalled;
            break;

        case M_MK_TICKRATE:
		    *valueP = OSS_TickRateGet(dev->osh);
            break;

	    case M_MK_REV_SIZE:
			/* get string size */
			if ((error = getRevId(dev, NULL, (u_int32*)valueP, FALSE)))
				return(error);
            break;

		/*---------------------+
        |  BLK codes           |
        +---------------------*/
        case M_MK_BLK_PHYSADDR:
		{
			M_ADDRSPACE *sp = (M_ADDRSPACE*)blk->data;

			if (blk->size < sizeof(M_ADDRSPACE))	/* check user buf */
				return(ERR_MK_USERBUF);

		    if (sp->space >= dev->spaceCnt)			/* check space index */
				return(ERR_MK_ILL_PARAM);

			sp->addr = (u_int32)dev->space[sp->space].physAddr;
			sp->size = (u_int32)dev->space[sp->space].reqSize;
            break;
		}

        case M_MK_BLK_VIRTADDR:
		{
			M_ADDRSPACE *sp = (M_ADDRSPACE*)blk->data;

			if (blk->size < sizeof(M_ADDRSPACE))	/* check user buf */
				return(ERR_MK_USERBUF);

		    if (sp->space >= dev->spaceCnt)			/* check space index */
				return(ERR_MK_ILL_PARAM);

			sp->addr = (u_int32)dev->space[sp->space].virtAddr;
			sp->size = (u_int32)dev->space[sp->space].availSize;
            break;
		}


        case M_MK_BLK_BB_HANDLER:
		{
			u_int32 size = OSS_StrLen(dev->osh, dev->brdName) + 1;

			if (blk->size < size)		/* check user buf */
				return(ERR_MK_USERBUF);

			OSS_MemCopy(dev->osh, size, dev->brdName, (char*)blk->data);
			blk->size = size;
            break;
		}

        case M_MK_BLK_DEV_NAME:
		{
			u_int32 size = OSS_StrLen(dev->osh, dev->devName) + 1;

			if (blk->size < size)		/* check user buf */
				return(ERR_MK_USERBUF);

			OSS_MemCopy(dev->osh, size, dev->devName, (char*)blk->data);
			blk->size = size;
            break;
		}

        case M_MK_BLK_REV_ID:
		{
			u_int32 size;

			/* get string size */
			if ((error = getRevId(dev, NULL, &size, FALSE)))
				return(error);

			if (blk->size < size)		/* check user buf */
				return(ERR_MK_USERBUF);

			/* create strings */
			if ((error = getRevId(dev, (char*)blk->data, &size, TRUE)))
				return(error);

			blk->size = size;
            break;
		}

        /*---------------------------+
        |  other codes               |
        +---------------------------*/
        default:
			error = ERR_MK_UNK_CODE;
            break;
    }

	return( error );
}


/**********************************************************************/
/** Call the device drivers GetStat() routine, lock processes
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  valueP    \OUT  either ptr to value or M_SG_BLOCK struct
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa mkGetStat bbGetStat
 */

static int32 
llGetStat(MK_PATH *mkPath, int32 code, int32 *valueP)
{
	MK_DEV *dev = mkPath->dev;
	int32 error;

	if( (error = MDIS_DevLock( mkPath, mdis_getstat ))) 
        return error;

	error = dev->llJumpTbl.getStat(dev->ll, code, mkPath->chan, valueP);

	MDIS_DevUnLock( mkPath, mdis_getstat );
	return( error );
}


/**********************************************************************/
/** Call the board drivers GetStat() routine
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  valueP    \OUT  either ptr to value or M_SG_BLOCK struct
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llGetStat mkGetStat
 */

static int32
bbGetStat(MK_PATH *mkPath, int32 code, int32 *valueP)
{
	MK_DEV *dev = mkPath->dev;
	return dev->brdJumpTbl.getStat(dev->brd, dev->devSlot, code, valueP);
}


/**********************************************************************/
/** Perform simple getstat call on MDIS device
 *
 *  Returns a single 32bit value.
 *
 *  Depending on the kind of the given getstat code this function gets
 *  the required information from the low-level driver, the carrier board
 *  driver or the MDIS kernel itself.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  valueP    \OUT  ptr to result
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llGetStat mkGetStat bbGetStat
 */

int32
MDIS_GetstatStd( MK_PATH *path, int32 code, int32 *valueP )
{
    int32 error;

    DBGWRT_1((DBH, "+++ MDIS_GetstatStd: code=0x%x\n", code ));

    switch( code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = llGetStat( path, code, valueP ); break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = bbGetStat( path, code, valueP ); break;
		case M_OFFS_MK:
			error = mkGetStat( path, code, valueP ); break;
		default:
			error = ERR_MK_UNK_CODE;
    }


    return( error );
}


/**********************************************************************/
/** Perform extended getstat call on MDIS device
 *
 *  Returns buffer containing required info.
 *
 *  Depending on the kind of the given getstat code this function gets
 *  the required information from the low-level driver, the carrier board
 *  driver or the MDIS kernel itself.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  buffer    \OUT  ptr to result buffer
 *  \param  length    \IN   length of buffer
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llGetStat mkGetStat bbGetStat
 */

int32
MDIS_GetstatBlk( MK_PATH *path, int32 code, u_int8 *buffer, int32 length )
{
    M_SG_BLOCK sg;
    int32 error;

    DBGWRT_1((DBH, "+++ MDIS_GetstatBlk: code=0x%x\n", code ));
    sg.data = buffer;
    sg.size = length;

    switch( code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = llGetStat( path, code, (int32*)&sg ); 
			break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = bbGetStat( path, code, (int32*)&sg ); 
			break;
		case M_OFFS_MK:
			error = mkGetStat( path, code, (int32*)&sg ); 
			break;
		default:
			error = ERR_MK_UNK_CODE;
    }

    return( error );
}


/**********************************************************************/
/** Handle setstat codes of MDIS kernel
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  valueP    \OUT  for std codes: ptr to value
 *						    for blk codes: M_SG_BLOCK struct
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llGetStat bbGetStat
 */

static int32 
mkSetStat( MK_PATH *mkPath, u_int32 code, void *arg )
{
	int32 error = 0;
	int32 value = (int32)arg;
	MK_DEV *dev = mkPath->dev;

	DBGWRT_2((DBH,"mkSetStat: M_MK_xxx code: 0x%x\n", code));
	
	/*------------------------------+
	| handle code                   |
	+------------------------------*/
    switch (code) {
        case M_MK_CH_CURRENT:
		    if (!IN_RANGE(value, 0, dev->devNrChan-1))
				return(ERR_MK_ILL_PARAM);

            mkPath->chan = value;
            break;

        case M_MK_IO_MODE:
		    if (!(value==M_IO_EXEC || value==M_IO_EXEC_INC))
				return(ERR_MK_ILL_PARAM);

            mkPath->ioMode = value;
            break;

        case M_MK_IRQ_ENABLE:
		    if (!IN_RANGE(value, 0, 1))
				return(ERR_MK_ILL_PARAM);

		    error = MDIS_EnableIrq(mkPath->dev, value);
            break;

        case M_MK_IRQ_COUNT:
		    dev->irqCnt = value;
            break;

        case M_MK_DEBUG_LEVEL:
		    DBG_MYLEVEL = value;
            break;

        case M_MK_API_DEBUG_LEVEL:
		    /* nothing to do */
            break;

        case M_MK_OSS_DEBUG_LEVEL:
		    OSS_DbgLevelSet(dev->osh, value);
            break;

        /*---------------------------+
        |  other codes               |
        +---------------------------*/
        default:
			error = ERR_MK_UNK_CODE;
            break;
    }

	return( error );
}

/**********************************************************************/
/** Call the device drivers SetStat() routine, lock processes
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  value     \OUT  either the value itself or M_SG_BLOCK struct
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa mkGetStat bbGetStat
 */

static int32 
llSetStat(MK_PATH *path, int32 code, void *value)
{
	MK_DEV *dev = path->dev;
	int32 error;

	if( (error = MDIS_DevLock( path, mdis_setstat ))) 
        return error;

	error = dev->llJumpTbl.setStat(dev->ll, code, path->chan, (int32)value);

	MDIS_DevUnLock( path, mdis_setstat );
	return( error );
}

/**********************************************************************/
/** Call the board drivers SetStat() routine
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  valueP    \OUT  either the value itself or M_SG_BLOCK struct
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llGetStat mkGetStat
 */

static int32 
bbSetStat(MK_PATH *mkPath, int32 code, void *value)
{
	MK_DEV *dev = mkPath->dev;
	return dev->brdJumpTbl.setStat(dev->brd, dev->devSlot, code, (int32)value);
}


/**********************************************************************/
/** Perform simple setstat call on MDIS device
 *
 *  Writes a single 32bit value.
 *
 *  Depending on the kind of the given setstat code this function writes
 *  the given information to the low-level driver, the carrier board
 *  driver or the MDIS kernel itself.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  value     \IN   value to set
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llSetStat mkSetStat bbSetStat
 */

int32
MDIS_SetstatStd( MK_PATH *path, int32 code, int32 value )
{
    int32 error;

    DBGWRT_1((DBH, "+++ MDIS_SetstatStd: code=0x%x\n", code ));

    switch( code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = llSetStat( path, code, (void*)value ); break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = bbSetStat( path, code, (void*)value ); break;
		case M_OFFS_MK:
			error = mkSetStat( path, code, (void*)value ); break;
		default:
			error = ERR_MK_UNK_CODE;
    }

    return( error );
}


/**********************************************************************/
/** Perform extended setstat call on MDIS device
 *
 *  Requires buffer containing data to write.
 *
 *  Depending on the kind of the given setstat code this function writes
 *  the given information to the low-level driver, the carrier board
 *  driver or the MDIS kernel itself.
 *
 *  \param  mkPath    \IN   MK path structure
 *  \param  code      \IN   status code
 *  \param  buffer    \IN   ptr to buffer containing data to write
 *  \param  length    \IN   length of buffer  (in bytes)
 *
 *  \return 0 on success or \c ERR_OSS_xxx error code on error
 *
 *  \sa llSetStat mkSetStat bbSetStat
 */

int32
MDIS_SetstatBlk( MK_PATH *path, int32 code, u_int8 *buffer, int32 length )
{
    M_SG_BLOCK sg;
    int32 error;

    DBGWRT_1((DBH, "+++ MDIS_SetstatBlk: code=0x%x\n", code ));
    sg.data = buffer;
    sg.size = length;

    switch( code & 0x0f00 ){
		case M_OFFS_LL:
		case M_OFFS_DEV:
			error = llSetStat( path, code, &sg ); 
			break;
		case M_OFFS_BB:
		case M_OFFS_BRD:
			error = bbSetStat( path, code, &sg ); 
			break;
		case M_OFFS_MK:
			error = mkSetStat( path, code, &sg ); 
			break;
		default:
			error = ERR_MK_UNK_CODE;
    }

    return( error );
}

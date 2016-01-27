/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: a11_int.h
 *
 *       Author: kp
 *        $Date: 1999/08/12 09:14:54 $
 *    $Revision: 1.1 $
 *
 *  Description: Internal header file for A11 BBIS driver
 *
 *     Switches: ---
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: a11_int.h,v $
 * Revision 1.1  1999/08/12 09:14:54  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _A11_H
#define _A11_H

#ifdef __cplusplus
    extern "C" {
#endif

/*--------------------------------------------------------------------------+
|    DEFINES                                                                |
+--------------------------------------------------------------------------*/
/* debug settings */
#define DBG_MYLEVEL		brdHdl->debugLevel
#define DBH             brdHdl->debugHdl

#define BRD_NBR_OF_BRDDEV		3		/* number of onboard devices		*/

#define BBNAME "A11"
/*---------------------------------------------------------------------------+
|    TYPEDEFS                                                                |
+---------------------------------------------------------------------------*/
/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
typedef struct {
	MDIS_IDENT_FUNCT_TBL idFuncTbl;				/* id function table			*/
    u_int32     ownMemSize;						/* own memory size				*/
    OSS_HANDLE* osHdl;							/* os specific handle			*/
    DESC_HANDLE *descHdl;						/* descriptor handle pointer	*/
    u_int32     debugLevel;						/* debug level for BBIS         */
	DBG_HANDLE  *debugHdl;						/* debug handle					*/
} BBIS_HANDLE;


/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
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

#ifdef __cplusplus
    }
#endif

#endif /* _A11_H */

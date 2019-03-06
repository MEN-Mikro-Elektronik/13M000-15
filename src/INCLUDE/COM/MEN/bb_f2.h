/***********************  I n c l u d e  -  F i l e  ************************
 *  
 *         Name: bb_f2.h
 *
 *       Author: ds
 *        $Date: 2000/06/09 14:09:58 $
 *    $Revision: 2.1 $
 * 
 *  Description: Header file for F2 BBIS driver
 *               - F2 specific status codes
 *               - F2 function prototypes
 *
 *     Switches: _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bb_f2.h,v $
 * Revision 2.1  2000/06/09 14:09:58  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#ifndef _BB_F2_H_
#define _BB_F2_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*---------------------------------------------------------------------------+
|    DEFINES                                                                 |
+---------------------------------------------------------------------------*/
/* F2 specific status codes (STD) */		/* S,G: S=setstat, G=getstat */
/*#define F2_XXX		M_BRD_OF+0x00*/		/* G,S: xxx */
/* none */

/* F2 specific status codes (BLK)*/			/* S,G: S=setstat, G=getstat */
/*#define F2_XXX		M_BRD_BLK_OF+0x00*/	/* G,S: xxx */
/* none */

/*---------------------------------------------------------------------------+
|    PROTOTYPES                                                              |
+---------------------------------------------------------------------------*/
#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void F2_GetEntry( BBIS_ENTRY* bbisP );
#endif /* _ONE_NAMESPACE_PER_DRIVER_ */

#ifdef __cplusplus
	}
#endif

#endif /* _BB_F2_H_ */

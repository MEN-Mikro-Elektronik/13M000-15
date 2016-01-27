/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: bbis_bk.h
 *
 *       Author: ub
 *        $Date: 2004/03/23 17:58:39 $
 *    $Revision: 1.1 $
 *
 *  Description: BBIS kernel definitions
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: bbis_bk.h,v $
 * Revision 1.1  2004/03/23 17:58:39  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _BBIS_BK_H
#define _BBIS_BK_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/
/* none */

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
#define BK_MAX_DEVNAME	39		/* maximum length of device name */

/*-----------------------------------------+
|  GLOBALS                                 |
+-----------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
extern int bbis_open(
	char *devName,
	DESC_SPEC *bbDesc,
	BBIS_HANDLE **bbHdlP,
	BBIS_ENTRY *bbEntries);

extern int bbis_close(char *devName);

extern char *bbis_ident(void);

extern int bbis_init( void );


#ifdef __cplusplus
   }
#endif

#endif  /* _BBIS_BK_H  */


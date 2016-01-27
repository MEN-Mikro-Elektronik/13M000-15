/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: dbg_os.h
 *
 *       Author: kp
 *        $Date: 2004/03/23 18:03:53 $
 *    $Revision: 1.2 $
 *
 *  Description: QNX6 macros to display driver debug messages
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: dbg_os.h,v $
 * Revision 1.2  2004/03/23 18:03:53  ub
 * Cosmetics
 *
 * Revision 1.1  2003/08/04 16:18:32  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
#ifndef _DBG_OS_H
#define _DBG_OS_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <sys/slogcodes.h>
#include <sys/slog.h>

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define __SLOG_CODE_MENDBG 40399


#define DBG_WRITE_DEFINED_BY_DBG_OS_H
#define DBG_Write(dbh,fmt,args...) \
 slogf(__SLOG_CODE_MENDBG, _SLOG_DEBUG1, fmt , ## args )


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifdef __cplusplus
   }
#endif
#endif /*_DBG_OS_H*/

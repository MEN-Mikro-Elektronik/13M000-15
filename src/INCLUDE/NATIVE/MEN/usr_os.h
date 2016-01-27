/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: usr_os.h
 *
 *       Author: ub
 *        $Date: 2004/03/24 09:45:19 $
 *    $Revision: 1.1 $
 *
 *  Description: QNX specific data types and defs for using the UOS lib
 *
 *     Switches: 
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: usr_os.h,v $
 * Revision 1.1  2004/03/24 09:45:19  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _USR_OS_H_
#define _USR_OS_H_

#include <stdio.h>
#include <signal.h>
#include <gulliver.h>

#ifdef __cplusplus
   extern "C" {
#endif

/*-----------------------------------------+
|  DEFINES                                 |
+------------------------------------------*/
/* error code offset */
#define ERR_UOS				0x2000

/* no support for shared memory and callbacks */
#define NO_SHARED_MEM
#define NO_CALLBACK

/* some predefined signal codes */
#define UOS_SIG_USR1		(SIGRTMIN)
#define UOS_SIG_USR2		(SIGRTMIN+1)

/* endianness of target */
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#undef _BIG_ENDIAN_
#undef _LITTLE_ENDIAN_

#if __BIGENDIAN__
# define _BIG_ENDIAN_
# define BIG_ENDIAN
#endif

#if __LITTLEENDIAN__
# define LITTLE_ENDIAN
# define _LITTLE_ENDIAN_
#endif


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

#ifdef __cplusplus
   }
#endif

#endif /*_USR_OS_H_*/

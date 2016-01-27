/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  men_typs.h
 *
 *      \author  uf
 *        $Date: 2014/04/07 15:25:58 $
 *    $Revision: 1.32 $
 *
 *     \project  MEN global
 *       \brief  MEN types and useful definitions
 *    \switches  WINNT
 *               OS9
 *               MSDOS
 *               LYNX
 *               HPRT
 *               HPUX
 *               VXWORKS
 *				 LINUX
 *               __QNX__
 *
 *               OS9 switches
 *               - _UCC
 *               - _TYPES_H
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: men_typs.h,v $
 * Revision 1.32  2014/04/07 15:25:58  dpfeuffer
 * R: doxygen documentation for 13bl50w70 sw requires men_typs.h
 * M: comments completely revised
 *
 * Revision 1.31  2013/08/30 14:58:15  ts
 * R: customer using PP04 had compile problem under 3.4.18-rt
 * M: customer system defined _LP64, not _LIN64, so use this define too
 *
 * Revision 1.30  2009/08/04 16:41:17  CRuff
 * R: make Linux version 64bit compatible
 * M: change type definitions to be compatible to LP64 data model
 *    (used for 64bit Linux systems)
 *
 * Revision 1.29  2009/03/31 11:25:39  ufranke
 * R: warning VSB config file missing
 * M: fixed
 *
 * Revision 1.28  2008/08/22 12:14:00  dpfeuffer
 * R: native_int error prone for 64-bit and removed from M_ADDRSPACE struct
 *    in mdis_api.h
 * M: native_int typedef removed
 *
 * Revision 1.27  2008/08/18 15:13:00  CKauntz
 * Added MENTYPS_64BIT switch to decide in common code 32 bit - 64 bit
 *
 * Revision 1.26  2008/05/27 13:34:13  dpfeuffer
 * - U_INT32_OR_64 define added
 *
 * Revision 1.25  2008/05/09 14:27:45  DPfeuffer
 * - set WINNT switch if _WIN32 switch is set but not VCIRTX switch is set
 *
 * Revision 1.24  2008/02/20 11:01:19  DPfeuffer
 * - INT32_OR_64 and __MAPILIB for Windows 64bit defined
 * - set WINNT switch if _WIN32 switch is set
 *
 * Revision 1.23  2008/01/18 14:23:53  ufranke
 * added
 *  + type define INT32_OR_64 for 32bit OS
 *
 * Revision 1.22  2005/06/23 16:03:11  kp
 * Copyright line changed
 *
 * Revision 1.21  2004/04/29 15:25:51  cs
 * changed typedefs for int8, int16, int32 to signed
 *
 * Revision 1.20  2004/04/02 12:46:45  kp
 * define __MAPILIB only if it was not defined before (MDVE)
 *
 * Revision 1.19  2004/03/19 11:29:18  ub
 * QNX added
 *
 * Revision 1.18  2004/02/06 17:12:32  dpfeuffer
 * int64, u_int64 for VCIRTX added
 *
 * Revision 1.17  2003/11/11 13:03:11  dschmidt
 * int64, u_int64 for WINNT added
 *
 * Revision 1.16  2003/03/24 08:38:04  UFranke
 * changed
 *   + 64bit integer types from typedef to define for old compilers
 *
 * Revision 1.15  2003/03/06 12:57:40  UFranke
 * added
 *   + 64bit integer types
 *
 * Revision 1.14  2001/08/07 16:09:53  ww
 * added _OSK switch
 *
 * Revision 1.13  2001/02/15 09:45:30  Schmidt
 * change from revision 1.11 was lost, __MAPILIB define added again
 *
 * Revision 1.12  2001/01/19 14:37:21  kp
 * LINUX added
 *
 * Revision 1.10  2000/06/28 11:35:12  Schmidt
 * MEN_NL, MEN_PATHSEP : WINNT defines added
 *
 * Revision 1.9  2000/05/15 13:47:04  ww
 * added OSK
 *
 * Revision 1.8  1998/12/17 14:24:08  kp
 * added OS9000 switches
 *
 * Revision 1.7  1998/07/20 13:04:42  see
 * macros for converting old style men types (UnBitXX) added
 * since including typdefs.h caused OS name clashes
 *
 * Revision 1.6  1998/06/22 17:56:21  see
 * bug fixed: illegal cpp directive #elseif
 *
 * Revision 1.5  1998/06/18 15:04:19  kp
 * added MENMON switch for types.h
 * added VOLATILE define
 *
 * Revision 1.4  1998/05/14 15:29:44  kp
 * native_int type added
 *
 * Revision 1.3  1998/04/17 13:36:14  see
 * macro IN_RANGE added
 * _EDITION macro removed
 *
 * Revision 1.2  1998/03/27 17:11:19  see
 * MEN_PATHSEP added
 * NL renamed to MEN_NL to prevent name conflicts
 * switches better commented
 *
 * Revision 1.1  1997/11/03 12:21:25  FRANKE
 * initial checkin
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1997-2014 MEN Mikro Elektronik GmbH. All rights reserved.
 ****************************************************************************/

#ifndef _MEN_TYPS_H
#define _MEN_TYPS_H

/*-------------------------------------------------------------------------+
|  OS specifics                                                            |
+-------------------------------------------------------------------------*/
/*
 * Windows: Set WINNT if _WIN32 is set (but not VCIRTX)
 *          Note: _WIN32 is always set from build utility and VC
 *                but WINNT is only set from build.
 */
#if ( defined( _WIN32 ) && !defined(VCIRTX) )
  #ifndef WINNT
    #define WINNT
  #endif
#endif

#ifdef VXWORKS
	#ifndef _VSB_CONFIG_FILE
		#define _VSB_CONFIG_FILE "../lib/h/config/vsbConfig.h"
	#endif
#endif

/*-------------------------------------------------------------------------+
|  integer types                                                           |
+-------------------------------------------------------------------------*/
#if ((defined(_OSK) || defined(OSK) || defined(OS9) || defined(OS9000)) && defined(_UCC))
# if !defined(_TYPES_H)
#    include <types.h>
# endif
#elif defined(MENMON) && defined(_UCC)
# if !defined(_TYPES_H)
#    include <types.h>
# endif
#else

/* standard integer types */
typedef unsigned char   u_int8;			/**< 8-bit unsigned integer */
typedef signed char     int8;			/**< 8-bit signed integer */

typedef unsigned short  u_int16;		/**< 16-bit unsigned integer */
typedef signed short    int16;			/**< 16-bit signed integer */

#if defined(_LIN64) || defined(_LP64)
 typedef unsigned int   u_int32;		/**< 32-bit unsigned integer */
 typedef signed int		int32;			/**< 32-bit signed integer */
#else
 typedef unsigned long  u_int32;		/**< 32-bit unsigned integer */
 typedef signed long    int32;			/**< 32-bit signed integer */
#endif 

#if defined(WINNT) || defined(VCIRTX)
 typedef unsigned __int64	u_int64;	/**< 64-bit unsigned integer */
 typedef __int64			int64;		/**< 64-bit signed integer */
#else
# define u_int64		unsigned long long  
# define int64			long long
#endif
#endif

/*-------------------------------------------------------------------------+
|  useful definitions                                                      |
+-------------------------------------------------------------------------*/
#ifndef TRUE
# define TRUE    1			/**< logical TRUE */
#endif

#ifndef FALSE
# define FALSE   0			/**< logical FALSE */
#endif

#ifndef NULL
# define NULL    0			/**< zero pointer */
#endif

#ifndef VOLATILE
# define VOLATILE volatile	/**< volatile keyword */
#endif

/* convert old style men types (don't use in new projects !!!) */
#define UnBit8      u_int8
#define UnBit16     u_int16
#define UnBit32     u_int32
#define Bit8        int8
#define Bit16       int16
#define Bit32       int32

/* MEN API calling convention and 64-bit support */
#if defined(WINNT)
  /* 64-bit compiler */
  #if defined(_WIN64)
#define __MAPILIB						/**< MEN API calling convention */
    #define INT32_OR_64		int64		/**< 32 or 64-bit signed integer */
	#define U_INT32_OR_64	u_int64		/**< 32 or 64-bit unsigned integer */
	#define MENTYPS_64BIT	
  /* 32-bit compiler */
  #else
    #define __MAPILIB 		__stdcall	/**< MEN API calling convention */
    #define INT32_OR_64		int32		/**< 32 or 64-bit signed integer */
	#define U_INT32_OR_64	u_int32		/**< 32 or 64-bit unsigned integer */
  #endif /* _WIN64 */
#endif /* WINNT */

#if defined(_LIN64)
    #define __MAPILIB
    #define INT32_OR_64		int64		/**< 32 or 64-bit signed integer */
	#define U_INT32_OR_64	u_int64		/**< 32 or 64-bit unsigned integer */
	#define MENTYPS_64BIT	
#endif 	

#ifndef INT32_OR_64
	#define INT32_OR_64		int32		/**< 32 or 64-bit signed integer */
#endif
#ifndef U_INT32_OR_64
	#define U_INT32_OR_64	u_int32		/**< 32 or 64-bit unsigned integer */
#endif
 
#ifndef __MAPILIB
#  define __MAPILIB						/**< MEN API calling convention */
#endif

/*-------------------------------------------------------------------------+
|  useful macros                                                           |
+-------------------------------------------------------------------------*/
/** range checking: check if v is in range [b..e] */
#define IN_RANGE(v,b,e) ((v)>=(b) && (v)<=(e))

/*-------------------------------------------------------------------------+
|  special character definitions                                           |
+-------------------------------------------------------------------------*/
/** newline */
#if defined(MSDOS) || defined(LYNX) || defined(HPRT) || defined(HPUX) || defined(VXWORKS) || defined(WINNT) || defined(LINUX) || defined(__QNX__)
# define MEN_NL 0x0a
#endif
#if defined(OS9) || defined(OS9000)
# define MEN_NL 0x0d
#endif

/** path separator */
#if defined(OS9) || defined(OS9000) || defined(LYNX) || defined(HPRT) || defined(HPUX) || defined(VXWORKS) || defined(LINUX) || defined(__QNX__)
# define MEN_PATHSEP 0x2f
#endif
#if defined(MSDOS) || defined(WINNT) 
# define MEN_PATHSEP 0x5c
#endif

#endif  /* _MEN_TYPS_H  */







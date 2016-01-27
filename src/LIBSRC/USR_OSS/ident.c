/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  ident.c
 *
 *      \author  ub
 *        $Date: 2004/03/24 09:46:13 $
 *    $Revision: 1.1 $
 * 
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  USR_OSS global edition history 
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ident.c,v $
 * Revision 1.1  2004/03/24 09:46:13  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
static const char RCSid[]="$Header: /dd2/CVSR/QNX/src/LIBSRC/USR_OSS/ident.c,v 1.1 2004/03/24 09:46:13 ub Exp $";

#include "uos_int.h"

/*! \mainpage 
 
 This is the documentation of the MEN USR_OSS library (User state
 Operating System Services) for QNX.

 Refer to the \ref usrosscommonspec "USR_OSS Common Specification" for the
 plain common specification.

 For QNX, the USR_OSS module is compiled as a static or shared library
 \c libusr_oss.a or \c libusr_oss.so which is linked to the application.

 The QNX implementation does not add any specific calls to the USR_OSS
 library.

*/
/*! \page dummy
 \menimages
*/

/**********************************************************************/
/** Return ident string of UOS module
 * \copydoc usr_oss_specification.c::UOS_Ident()
 */
char* UOS_Ident( void )
{
	return("UOS - User Operating System Services: $Id: ident.c,v 1.1 2004/03/24 09:46:13 ub Exp $");
}







/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  bk_ident.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2014/10/30 10:39:12 $
 *    $Revision: 1.9 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  BBIS Kernel library version info
 *      
 *    \switches  
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: bk_ident.c,v $
 * Revision 1.9  2014/10/30 10:39:12  ch
 * (AUTOCI) Checkin due to new revision 01.6 of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.8  2014/10/29 16:06:38  ch
 * (AUTOCI) Checkin due to new revision 01.5 of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.7  2010/06/21 19:20:58  ch
 * (AUTOCI) Checkin due to new revision 01.4 of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.6  2009/07/09 12:39:30  channoyer
 * (AUTOCI) Checkin due to new revision 01.03 of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.5  2009/06/07 15:21:51  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.2 of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.4  2008/11/12 16:32:58  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.1a of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.3  2008/11/10 15:00:00  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.1 of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.2  2008/11/06 20:37:13  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.0 of fileset QNX/LIBSRC/bbis_kernel
 *
 * Revision 1.1  2004/03/23 17:59:09  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2004 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char *IdentString="MEN BBIS Kernel - %FSREV QNX/LIBSRC/bbis_kernel 01.6 2014-10-30%";

/*! \mainpage 

 \anchor qnx_bbis_kernel
 
 This is the documentation of the MEN BBIS kernel library for QNX. The BBIS
 kernel provides some basic services used by the MDIS kernel for opening and
 closing carrier board devices (like MEN A201, D203 etc.).

 For QNX, the BBIS kernel library is compiled in two variants:
 - \c libmen_bbis_kernel.so: standard library, normally used
 - \c libmen_bbis_kernel_dbg.so: debug version

*/
/*! \page bbis_dummy
 \menimages
*/


/**********************************************************************/
/** Return BBIS ident string
 *
 *  \return  pointer to ident string
 *
 */

char* bbis_ident( void )
{
    return( (char*)IdentString );
}

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ident.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2014/10/30 12:57:40 $
 *    $Revision: 1.10 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  Return identification of MDIS kernel
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ident.c,v $
 * Revision 1.10  2014/10/30 12:57:40  ch
 * (AUTOCI) Checkin due to new revision Release-1.6 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.9  2011/11/15 16:16:44  ch
 * (AUTOCI) Checkin due to new revision Release-1.5 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.8  2010/06/21 21:03:57  ch
 * (AUTOCI) Checkin due to new revision 01.4 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.7  2009/07/13 21:27:40  channoyer
 * (AUTOCI) Checkin due to new revision 01.03 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.6  2009/06/09 10:07:47  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.2 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.5  2009/06/09 00:16:59  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.2 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.4  2008/11/13 16:43:48  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.1 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.3  2008/11/13 15:48:28  channoyer
 * (AUTOCI) Checkin due to new revision Release-1.1 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.2  2004/03/25 08:51:49  ub
 * (AUTOCI) Checkin due to new revision Release-1.0 of fileset ART/13M000-15/13M000-15
 *
 * Revision 1.1  2004/03/24 11:36:14  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2004 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*--------------------------------------+
|  INCLUDES                             |
+---------------------------------------*/
#include "mk_intern.h"

char* 
MDIS_Ident( void )
{
	return("MK - MDIS Kernel: %FSREV ART/13M000-15/13M000-15 Release-1.6 2014-10-30%");
}

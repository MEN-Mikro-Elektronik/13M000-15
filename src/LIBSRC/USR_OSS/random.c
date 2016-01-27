/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  random.c
 *
 *      \author  ub
 *        $Date: 2004/03/24 09:45:31 $
 *    $Revision: 1.1 $
 * 
 * 	   \project  MDIS4Linux USR_OSS lib
 *
 *  	 \brief  Create random integers
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: random.c,v $
 * Revision 1.1  2004/03/24 09:45:31  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

static char RCSid[]="$Id: random.c,v 1.1 2004/03/24 09:45:31 ub Exp $";

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

/**********************************************************************/
/** Create a new pseudo random integer value
 * 
 * \copydoc usr_oss_specification.c::UOS_Random()
 * \sa UOS_RandomMap
 */
u_int32 UOS_Random(u_int32 old)
{
	register u_int32 a = old;
			 
	a <<= 11;
	a += old;
	a <<= 2;
	old += a;
	old += 13849;
	return old;
}

/**********************************************************************/
/** Map created integer value into specified range
 *
 * \copydoc usr_oss_specification.c::UOS_RandomMap()
 * 
 * \sa UOS_Random
 */
u_int32 UOS_RandomMap(u_int32 val, u_int32 ra, u_int32 re)
{
   double  f;
   u_int32 r;

   f = (double)val / 0xffffffff;      			/* make double 0..1 */
   r = (f * (double)(re-ra)) + 0.5 + ra;    	/* expand to set, add offset */

   return(r);
}






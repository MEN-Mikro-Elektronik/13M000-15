/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_swap.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/08/04 14:01:40 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Byte swapping routines
 *      
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_swap.c,v $
 * Revision 1.1  2003/08/04 14:01:40  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"

/**********************************************************************/
/** Swap bytes in 16-bit word.
 * \copydoc oss_specification.c::OSS_Swap16()
 */
u_int16 OSS_Swap16( u_int16 word )
{
    return( (word>>8) | (word<<8) );
}

/**********************************************************************/
/** Swap bytes in 32-bit word.
 * \copydoc oss_specification.c::OSS_Swap32()
 */
u_int32 OSS_Swap32( u_int32 dword )
{
    return(  (dword>>24) | (dword<<24) | \
             ((dword>>8) & 0x0000ff00) | \
             ((dword<<8) & 0x00ff0000)   );
}

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_isa_pnp.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2003/08/04 14:01:37 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  ISA PNP support
 *      
 *    \switches  -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_isa_pnp.c,v $
 * Revision 1.1  2003/08/04 14:01:37  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"

/**********************************************************************/
/** Get ISAPNP configuration data.
 *
 * \copydoc oss_specification.c::OSS_IsaGetConfig()
 *
 * \qnx not implemented. Returns \c ERR_OSS_ISA_NO_PNPDATA
 */
int32 OSS_IsaGetConfig(
		OSS_HANDLE	*oss,
		int8		*cardVendId,
		int16		cardProdId,
		int8		*devVendId,
		int16		devProdId,
		int32       devNbr,
		int32       resType,
		int32       resNbr,
		u_int32		*valueP
)
{
    DBGWRT_ERR(( DBH, "*** OSS_IsaGetConfig not implemented\n"));

	return( ERR_OSS_ISA_NO_PNPDATA );
}/*OSS_IsaGetConfig*/

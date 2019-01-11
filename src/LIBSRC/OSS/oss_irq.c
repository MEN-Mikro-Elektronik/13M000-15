/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_irq.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2004/03/24 09:40:47 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4QNX
 *  	 \brief  Interrupt related routines
 *      
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_irq.c,v $
 * Revision 1.1  2004/03/24 09:40:47  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"

/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/
/* (none) */

/**********************************************************************/
/** Mask device interrupts.
 * \copydoc oss_specification.c::OSS_IrqMaskR()
 *
 * \qnx Masks \b all processor interrupts
 * \sa OSS_IrqRestore()
 */
OSS_IRQ_STATE OSS_IrqMaskR( OSS_HANDLE *oss, OSS_IRQ_HANDLE* irqHandle )
{
	DBGWRT_1((DBH,"OSS_IrqMaskR\n"));
	InterruptLock( &(oss->intrSpinLock) );
	return( 0 );
}

/**********************************************************************/
/** Unmask device interrupts.
 * \copydoc oss_specification.c::OSS_IrqRestore()
 *
 * \sa OSS_IrqMaskR()
 */
void OSS_IrqRestore( 
	OSS_HANDLE *oss, 
	OSS_IRQ_HANDLE* irqHandle, 
	OSS_IRQ_STATE oldState )
{
	DBGWRT_1((DBH,"OSS_IrqRestore\n"));
	InterruptUnlock( &(oss->intrSpinLock) );
}

/**********************************************************************/
/** Mask device interrupts (old implementation).
 * \copydoc oss_specification.c::OSS_IrqMask()
 *
 * \qnx Masks \b all processor interrupts
 * \sa OSS_IrqUnMask()
 */
void OSS_IrqMask( OSS_HANDLE *oss, OSS_IRQ_HANDLE* irqHandle )
{
	DBGWRT_1((DBH,"OSS_IrqMask \n"));

	OSS_IrqMaskR( oss, irqHandle );

}

/**********************************************************************/
/** Unmask device interrupts (old implementation).
 * \copydoc oss_specification.c::OSS_IrqUnMask()
 * \sa OSS_IrqMask()
 */
void OSS_IrqUnMask( OSS_HANDLE *oss, OSS_IRQ_HANDLE *irqHandle )
{
	DBGWRT_1((DBH,"OSS_IrqUnMask \n"));

	OSS_IrqRestore( oss, irqHandle, 0 );			
}

/**********************************************************************/
/** Compute interrupt vector from interrupt number and bus type.
 * \copydoc oss_specification.c::OSS_IrqLevelToVector()
 * 
 * \qnx The QNX implementation returns simply the given IRQ number.
 */
int32 OSS_IrqLevelToVector(
	OSS_HANDLE *oss,
	int32      busType,
	int32      irqNbr,
	int32      *vectorP
)
{
    DBGCMD( static const char functionName[] = "OSS_IrqLevelToVector"; )

	DBGWRT_1((DBH,"%s\n", functionName));

	switch( busType ){
	case OSS_BUSTYPE_PCI:
	case OSS_BUSTYPE_ISA:
	case OSS_BUSTYPE_ISAPNP:
		*vectorP = irqNbr;
		break;
	default:
        DBGWRT_ERR(( DBH, "*** %s: busType %ld not supported\n",
                      functionName, busType ));
		return( ERR_OSS_ILL_PARAM );
	}/*switch*/

	DBGWRT_2((DBH," vector=%ld\n", *vectorP ));

	return( 0 );
}/*OSS_IrqLevelToVector*/




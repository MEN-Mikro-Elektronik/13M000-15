/***********************  I n c l u d e  -  F i l e  ***********************/
/*!
 *        \file  variant.h
 *
 *      \author  dieter.pfeuffer@men.de
 *        $Date: 2014/08/27 09:52:01 $
 *    $Revision: 1.14 $
 *
 *       \brief  Header file for SMBPCI BBIS with variant specific stuff
 *
 *    \switches  SMBPCI_VARIANT
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: variant.h,v $
 * Revision 1.14  2014/08/27 09:52:01  MRoth
 * R: no support for F23P and G24 (ICH compatible)
 * M: added Device IDs VAR_PCI_DEV_ID_10 and VAR_PCI_DEV_ID_11
 *
 * Revision 1.13  2012/03/23 09:12:55  dpfeuffer
 * R:1. support for G22/F22 (ICH compatible)
 *   2. support for MM2 (SCH compatible)
 * M:1. ICH part completed
 *   2. SCH part completed
 *
 * Revision 1.12  2012/01/30 15:11:33  dpfeuffer
 * R: support for FCH and FCH_ASF SMB controller
 * M: FCH and FCH_ASF specific part added
 *
 * Revision 1.11  2010/12/20 13:31:51  dpfeuffer
 * R:1. 16Z001 SMB ctrl. at ISA bus
 *   2. IO mapped variants for 16Z001 SMB ctrl. required
 * M:1. variant ISA_16Z001 implemented
 *   2. variant 16Z001_IO and ISA_16Z001_IO implemented
 *
 * Revision 1.10  2010/07/15 14:33:56  MRoth
 * R: no support for G20 and F21P
 * M: added defines VAR_PCI_DEV_ID_7 and VAR_PCI_DEV_ID_8
 *
 * Revision 1.9  2010/06/11 15:48:09  CRuff
 * R: new variant _SMB_NATIVE added
 * M: added defines for new variant _SMB_NATIVE
 *
 * Revision 1.8  2009/05/18 13:19:18  dpfeuffer
 * R: SMBPCI_16Z001(_F208) MEN PCI vendor-id (0x1A88) not supported
 * M: MEN PCI vendor-id added
 *
 * Revision 1.7  2008/10/20 13:19:53  MSoehnlein
 * Added SCH support
 *
 * Revision 1.6  2008/01/22 14:17:12  ufranke
 * fixed
 *  - now compilable for !ONE_NAMESPACE_PER_DRIVER
 *
 * Revision 1.5  2007/10/09 15:20:41  JWu
 * added support for ICH8 (F18)
 *
 * Revision 1.4  2007/04/10 13:14:23  DPfeuffer
 * 16Z001 driver variant implemented
 *
 * Revision 1.3  2006/08/02 12:25:00  DPfeuffer
 * added SMBPCI_16Z001_F208 variant
 *
 * Revision 1.2  2006/04/28 13:54:59  DPfeuffer
 * added support for ICH0, ICH2, ICH7
 *
 * Revision 1.1  2005/11/25 15:08:50  dpfeuffer
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/


#ifndef _VARIANT_H
#define _VARIANT_H

#ifdef __cplusplus
    extern "C" {
#endif


/*-----------------------------------------+
|  ICH/PCH SMBus controller                |
+-----------------------------------------*/
#if defined( _SMBPCI_ICH )
#	define VAR_NAME					SMBPCI_ICH		/**< hw-name and func-prefix */
#	define VAR_NAME_STR				"SMBPCI_ICH"	/**< hw-name and func-prefix */
#	define VAR_PCI_VEN_ID			0x8086			/* pci vendor-id             */
#	define VAR_PCI_DEV_ID			0x266A			/* pci device-id - ICH6         (F14)      */
#	define VAR_PCI_DEV_ID_2			0x27DA			/* pci device-id - ICH7         (F15)      */
#	define VAR_PCI_DEV_ID_3			0x2410			/* pci device-id - ICH0         (EM07)     */
#	define VAR_PCI_DEV_ID_4			0x2443			/* pci device-id - ICH2         (EM07n)    */
#   define VAR_PCI_DEV_ID_5			0x283E			/* pci device-id - ICH8         (F18E)     */
#   define VAR_PCI_DEV_ID_6			0x2930			/* pci device-id - ICH9         (F19P)     */
#   define VAR_PCI_DEV_ID_7			0x3B30			/* pci device-id - PCH          (G20)      */
#   define VAR_PCI_DEV_ID_8			0x1C22			/* pci device-id - PCH          (F21P)     */
#	define VAR_PCI_DEV_ID_9			0x1E22			/* pci device-id - PantherPoint (G22/F22P) */
#	define VAR_PCI_DEV_ID_10		0x8C22			/* pci device-id - LynxPoint - Haswell/Broadwell (F23P/G24) */
#	define VAR_PCI_DEV_ID_11		0x9CA2			/* pci device-id - WildcatPoint (n/a)      */
#	define VAR_CTRLR_BAR			4				/* BAR to ctrl registers        */
#	define VAR_CTRLR_OFFSET			0x00			/* offset to ctrl registers     */
#	define VAR_CTRLR_SIZE			0x18			/* size of ctrl registers       */
/*-----------------------------------------+
|  SCH SMBus controller                    |
+-----------------------------------------*/
#elif defined( _SMBPCI_SCH )
#	define VAR_NAME					SMBPCI_SCH		/**< hw-name and func-prefix  */
#	define VAR_NAME_STR				"SMBPCI_SCH"	/**< hw-name and func-prefix  */
#	define VAR_PCI_VEN_ID			0x8086			/* pci vendor-id              */
#	define VAR_PCI_DEV_ID			0x8119			/* pci device-id - SCH (XM01) */
#	define VAR_PCI_DEV_ID_2			0x8186			/* pci device-id - TopCliff (MM2) */
#	define VAR_CTRLR_BAR			0x40			/* Offset to BAR register     */
#	define VAR_CTRLR_OFFSET			0x00			/* offset to ctrl registers   */
#	define VAR_CTRLR_SIZE			0x40			/* size of ctrl registers     */
/*-----------------------------------------+
|  FCH SMBus controllers (standard/asf)    |
+-----------------------------------------*/
#elif defined( _SMBPCI_FCH ) || defined( _SMBPCI_FCH_ASF )
#	define VAR_PCI_VEN_ID			0x1002			/* pci vendor-id                */
#	define VAR_PCI_DEV_ID			0x4385			/* pci device-id - A55E (SC24)  */
#	define VAR_PMIO_IDXDATA			0xCD6			/* PM I/O register space */
#	define VAR_PMIO_SIZE			2				/* PM I/0 register size  */
#	define VAR_PMIO_IDX_OFF			0x000			/* offset to index register (r/w, 8bits) */
#	define VAR_PMIO_DATA_OFF		0x001			/* offset to data register (r/w, 8bits)  */
	/* FCH standard SMBus controller */
#	if defined( _SMBPCI_FCH )
#		define VAR_NAME					SMBPCI_FCH		/**< hw-name and func-prefix	*/
#		define VAR_NAME_STR				"SMBPCI_FCH"	/**< hw-name and func-prefix	*/
#		define VAR_PM_SMBADDR			0x2C			/* PM Smbus0En (r/w, 16bits)   */
#		define VAR_PM_SMBADDR_BYTES		2				/* PM Smbus0En byte size       */
#		define VAR_PM_SMBUS0EN_BADDR	0xFFE0			/* PM SmBus0_baddr mask (15:5) */
#		define VAR_CTRLR_SIZE			0x14			/* size of ctrl registers      */
	/* FCH_ASF SMBus controller */
#	else
#		define VAR_NAME					SMBPCI_FCH_ASF	/**< hw-name and func-prefix	*/
#		define VAR_NAME_STR				"SMBPCI_FCH_ASF"/**< hw-name and func-prefix	*/
#		define VAR_PM_SMBADDR			0x24			/* PM AcpiMmioEn (r/w, 32bits)          */
#		define VAR_PM_SMBADDR_BYTES		4				/* PM AcpiMmioEn byte size              */
#		define VAR_PM_ACPIMMIOEN_ENBL	0x00000001		/* PM AcpiMMioDecodeEn mask (1:enabled) */
#		define VAR_PM_ACPIMMIOEN_IO		0x00000002		/* PM AcpiMMIoSel mask (1:i/o mapped)   */
#		define VAR_PM_ACPIMMIOEN_ASF	0xFFFFF000		/* PM AcpiMMioAddr 31:12 mask           */
#		define VAR_PM_ACPIMMIOEN_ASFOFF	0x900			/* offset to ASF registers              */
#		define VAR_CTRLR_SIZE			0x17			/* size of ctrl registers               */
#	endif
/*-----------------------------------------+
|  16Z001 SMBus controller on F208         |
+-----------------------------------------*/
#elif defined( _SMBPCI_16Z001_F208 )
#	define VAR_NAME					SMBPCI_16Z001_F208		/**< hw-name and func-prefix	*/
#	define VAR_NAME_STR				"SMBPCI_16Z001_F208"	/**< hw-name and func-prefix	*/
#	define VAR_PCI_VEN_ID			0x1172			/* pci vendor-id (Altera) */
#	define VAR_PCI_VEN_ID_2			0x1A88			/* pci vendor-id (MEN) */
#	define VAR_PCI_DEV_ID			0x4D45			/* pci device-id */
#	define VAR_CTRLR_BAR			3				/* BAR to ctrl registers	*/
#	define VAR_CTRLR_OFFSET			0x00			/* offset to ctrl registers */
#	define VAR_CTRLR_SIZE			0x0D			/* size of ctrl registers */
/*-----------------------------------------+
|  16Z001 SMBus controller                 |
+-----------------------------------------*/
#elif (defined( _SMBPCI_16Z001 ) || defined( _SMBPCI_16Z001_IO ))
#	ifdef _SMBPCI_16Z001
#		define VAR_NAME				SMBPCI_16Z001	/**< hw-name and func-prefix	*/
#		define VAR_NAME_STR			"SMBPCI_16Z001"	/**< hw-name and func-prefix	*/
#	else
#		define VAR_NAME				SMBPCI_16Z001_IO	/**< hw-name and func-prefix	*/
#		define VAR_NAME_STR			"SMBPCI_16Z001_IO"	/**< hw-name and func-prefix	*/
#	endif
#	define VAR_PCI_VEN_ID			0x1172			/* pci vendor-id (Altera) */
#	define VAR_PCI_VEN_ID_2			0x1A88			/* pci vendor-id (MEN) */
#	define VAR_PCI_DEV_ID			0x4D45			/* pci device-id */
#	define VAR_CTRLR_OFFSET			0x00			/* offset to ctrl registers */
#	define VAR_CTRLR_SIZE			0x0D			/* size of ctrl registers */
/*-----------------------------------------+
|  16Z001 SMBus controller @ ISA bus       |
+-----------------------------------------*/
#elif (defined( _SMBPCI_ISA_16Z001 ) || defined( _SMBPCI_ISA_16Z001_IO ))
#	ifdef _SMBPCI_ISA_16Z001
#		define VAR_NAME				SMBISA_16Z001	/**< hw-name and func-prefix	*/
#		define VAR_NAME_STR			"SMBISA_16Z001"	/**< hw-name and func-prefix	*/
#	else
#		define VAR_NAME				SMBISA_16Z001_IO	/**< hw-name and func-prefix	*/
#		define VAR_NAME_STR			"SMBISA_16Z001_IO"	/**< hw-name and func-prefix	*/
#	endif
#	define VAR_CTRLR_SIZE			0x0D			/* size of ctrl registers */
#	define VAR_ISA					1				/* SMB ctrl at ISA bus */
/*-----------------------------------------+
|  Native SMB Controller (OnChip)          |
+-----------------------------------------*/
#elif defined( _SMB_NATIVE )
#   define VAR_NAME					SMB_NATIVE		/**< hw-name and func-prefix	*/
#	define VAR_NAME_STR				"SMB_NATIVE"	/**< hw-name and func-prefix	*/
#endif

#ifndef VAR_NAME
#	error "*** VAR_NAME must be set"
#endif

#ifndef VAR_NAME_STR
#	error "*** VAR_NAME_STR must be set"
#endif

#ifdef __cplusplus
    }
#endif

#endif /* _VARIANT_H */






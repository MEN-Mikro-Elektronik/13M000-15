/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: f2_int.h
 *
 *       Author: ds
 *        $Date: 2000/06/09 14:09:53 $
 *    $Revision: 1.1 $
 *
 *  Description: Internal header file for F2 BBIS driver
 *
 *               The F2 supports two onboard-devices:
 *               - The onboard-devices corresponds to ISA or PCI devices.
 *               - The onboard-device uses the following resources:
 *
 *               slot   | device (function)          | bus | resources
 *               -------+----------------------------+-----+--------------
 *               0x1000 | Z8536                      |     |
 *						|  (Watchdog, SMB-Ctr, Trig) | ISA | io space, IRQ
 *                      | LM79                       |     |
 *                      |  (Systemmonitor)           | ISA | IRQ
 *               0x1001 | ALI M1543C                 |     |
 *						|  (User-LEDs, GPIOs)        | PCI | cfg space
 *
 *
 *     Switches: ---
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: f2_int.h,v $
 * Revision 1.1  2000/06/09 14:09:53  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _F2_H
#define _F2_H

#ifdef __cplusplus
    extern "C" {
#endif


#define MAC_IO_MAPPED
#include <MEN/maccess.h>    /* hw access macros and types     */

/*--------------------------------------------------------------------------+
|    DEFINES                                                                |
+--------------------------------------------------------------------------*/
#define BBNAME					"F2"	/* board function prefix	*/
#define BRD_NBR_OF_DEV			2		/* number of devices		*/

/* ISA device 0 (Z8536) */
#define BRD_ISADEV0_BUSNBR		0				/* bus number		*/
#define	BRD_ISADEV0_IO_ADDR		((void*)0x100)	/* IO address		*/
#define	BRD_ISADEV0_IO_SIZE		0x4				/* IO size			*/

/* PCI devices 0 (ALI 1543C) */
#define BRD_PCIDEV0_BUSNBR		0				/* bus number		*/
#define	BRD_PCIDEV0_DEVNBR		0x07			/* device number	*/
		
/* internal used ISA device FDC37C669 */
#define BRD_FDC_BUSNBR			0				/* bus number		*/
#define	BRD_FDC_IO_ADDR			((void*)0x3f0)	/* IO address		*/
#define	BRD_FDC_IO_SIZE			0x2				/* IO size			*/


#ifdef __cplusplus
    }
#endif

#endif /* _F2_H */

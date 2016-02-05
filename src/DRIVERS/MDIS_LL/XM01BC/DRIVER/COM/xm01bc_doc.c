/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  xm01bc_doc.c
 *
 *      \author  matthias.soehnlein@men.de
 *        $Date: 2009/09/04 11:57:38 $
 *    $Revision: 1.3 $
 *
 *      \brief   User documentation for XM01BC device driver
 *
 *     Required: -
 *
 *     \switches -
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: xm01bc_doc.c,v $
 * Revision 1.3  2009/09/04 11:57:38  MRoth
 * R: Porting to MDIS5
 * M: Changed description to MDIS5; added compatibility hint
 *
 * Revision 1.2  2009/02/06 15:46:15  MSoehnlein
 * R:1. Added MM1/ XM2/ F19 Support
 * M:1. renamed Board Controller Interface (BCI) from XM01BCI to CPUBCI
 *
 * Revision 1.1  2008/10/31 17:13:33  MSoehnlein
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

/*! \mainpage
    This is the documentation of the MDIS5 low-level driver for the
	CPU board-controller of XM1/MM1/XM2/F19 CPU board (XM01BC).

    The CPU board-controller is a PIC microcontroller that resides on the SMBus
	of the CPU board. The CPU board-controller manages board power sequencing,
	performs voltage and temperature supervision, provides system watchdog and
	SW-reset functionality and error state logging.

    <b>Common Notes:</b>
	- The driver requires MDIS with SMB2 support
	- MDIS5 32bit drivers are compatible to the MDIS4 drivers but must not
      be mixed with MDIS4 drivers at one target system.

    \n
    \section Variants Variants
    The XM01BC driver can be built in the following variants at compilation time:

    \code
    Driver              Variant Description
    --------            --------------------------------
    Standard            XM01BC standard driver
    \endcode

    \n \section FuncDesc Functional Description

    \n \subsection General General
	All driver functionality is provided via M_setstat() and M_getstat().

	When the first path is opened to an XM01BC device, the HW and the driver are being
	initialized with default values	(see section about \ref descriptor_entries).

    \n \subsection channels Logical channels
	The driver provides logical channels that relates to the supervised
	voltages of the XM01BC. All other functionality is channel independent.\n

    \n \section api_functions Supported API Functions

    <table border="0">
    <tr>
        <td><b>API Function</b></td>
        <td><b>Functionality</b></td>
        <td><b>Corresponding Low-Level Function</b></td></tr>

    <tr><td>M_open()      </td><td>Open device              </td>
	<td>XM01BC_Init()</td></tr>
    <tr><td>M_close()     </td><td>Close device             </td>
    <td>XM01BC_Exit())</td></tr>
    <tr><td>M_setstat()   </td><td>Set device parameter     </td>
    <td>XM01BC_SetStat()</td></tr>
    <tr><td>M_getstat()   </td><td>Get device parameter     </td>
    <td>XM01BC_GetStat()</td></tr>
    <tr><td>M_errstringTs() </td><td>Generate error message </td>
    <td>-</td></tr>
    </table>

    \n \section descriptor_entries XM01BC Descriptor Entries

    The low-level driver initialization routine decodes the following entries
    ("keys") in addition to the general descriptor keys:

    <table border="0">
    <tr><td><b>Descriptor entry</b></td>
        <td><b>Description</b></td>
        <td><b>Values</b></td>
    </tr>
    <tr><td>SMB_BUSNBR</td>
        <td>SMBus bus number</td>
        <td>0, 1, 2, ...\n
			Default: 0</td>
    </tr>
    <tr><td>SMB_DEVADDR</td>
        <td>SMBus address of XM01BC</td>
        <td>0x00, 0xff\n
			Default: 0x9a</td>
    </tr>
     <tr><td>POLL_PERIOD</td>
        <td>Poll period for voltage supervision\n
		    (only for FW-Rev. 2.0 and later)</td>
        <td>50..5000ms\n
			Default: 500ms</td>
    </tr>
    </tr>
    </table>

    \n \subsection xm01bc_min   Minimum descriptor
    xm01bc_min.dsc (see Examples section)\n
    Demonstrates the minimum set of options necessary for using the driver.

    \n \subsection xm01bc_max   Maximum descriptor
    xm01bc_max.dsc (see Examples section)\n
    Shows all possible configuration options for this driver.

    \n \section codes XM01BC specific Getstat/Setstat codes
	For the watchdog functionality, the driver supports a subset of the WDOG
	Getstat/Setstat codes (defined in wdog.h):
	- #WDOG_START
	- #WDOG_STOP
	- #WDOG_TRIG
	- #WDOG_TIME
	- #WDOG_STATUS\n

    \n \section programs Overview of provided programs

    \subsection xm01bc_ctrl  Tool that controls the XM01BC PIC (example of using the XM01BC driver)
    xm01bc_ctrl.c (see Examples section)

    \subsection wdog_test  Tool to service and test the watchdog
    wdog_test.c (see example section)

    \subsection wdog_simp  Simple example program for watchdog drivers
    wdog_simp.c (see example section)
*/

/** \example xm01bc_ctrl.c */
/** \example wdog_test.c */
/** \example wdog_simp.c */
/** \example xm01bc_min.dsc */
/** \example xm01bc_max.dsc */

/*! \page dummy
  \menimages
*/

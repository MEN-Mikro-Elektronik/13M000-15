/***********************  I n c l u d e  -  F i l e  ***********************/
/*!
 *        \file  xm01bc_drv.h
 *
 *      \author  matthias.soehnlein@men.de
 *        $Date: 2009/09/04 11:57:42 $
 *    $Revision: 3.3 $
 *
 *       \brief  Header file for XM01BC driver containing
 *               XM01BC specific status codes and
 *               XM01BC function prototypes
 *
 *    \switches  _ONE_NAMESPACE_PER_DRIVER_
 *               _LL_DRV_
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: xm01bc_drv.h,v $
 * Revision 3.3  2009/09/04 11:57:42  MRoth
 * R: Porting to MDIS5, new MDIS_API and men_typs
 * M: for backward compatibility to MDIS4 optionally define new types
 *
 * Revision 3.2  2009/02/06 15:46:24  MSoehnlein
 * R:1. Added MM1/ XM2/ F19 Support
 * M:1.a) added "Hardware ID" query
 *     b) added "Number of Supervised Voltages" query
 *     c) added "Number of Error Counters" query
 *     d) added "RESET_IN Blocking Mode" query
 *     e) increased number of error counters to 31
 *     f) increased number of supervised voltages to 8
 *     g) renamed Board Controller Interface (BCI) from XM01BCI to CPUBCI
 *
 * Revision 3.1  2008/10/31 17:13:41  MSoehnlein
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN Mikro Elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#ifndef _XM01BC_DRV_H
#define _XM01BC_DRV_H

#ifdef __cplusplus
      extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/

/*-----------------------------------------+
|  DEFINES                                 |
+-----------------------------------------*/
/** \name XM01BC specific Getstat/Setstat codes
 *  \anchor getstat_setstat_codes
 */
/**@{*/
#define XM01BC_WDOG_ARM		M_DEV_OF+0x00   /**< S: arm external watchdog \n
											      Values: -\n
												  Default: -\n*/

#define XM01BC_SWRESET		M_DEV_OF+0x01   /**< S: perform a software warm reset \n
											      Values: 0xdead\n
												  Default: -\n*/

#define XM01BC_SWCOLDRESET	M_DEV_OF+0x02   /**< S: perform a software cold reset \n
											      Values: 0xdead\n
												  Default: -\n*/

#define XM01BC_VOLT_ACT		M_DEV_OF+0x02   /**< G: get PIC input voltage of current channel\n
											      Values: [mV]\n
												  Default: -\n*/

#define XM01BC_VOLT_LOW		M_DEV_OF+0x03   /**< G: get lower voltage limit of current channel\n
											      Values: [mV]\n
												  Default: -\n*/

#define XM01BC_VOLT_HIGH	M_DEV_OF+0x04   /**< G: get upper voltage limit of current channel\n
											      Values: [mV]\n
												  Default: -\n*/

#define XM01BC_VOLT_SIGSET	M_DEV_OF+0x05   /**< S: install an user signal for voltage supervision\n
											      Values: signal number (see usr_os.h)\n
												  Default: -\n*/

#define XM01BC_VOLT_SIGCLR	M_DEV_OF+0x06   /**< S: deinstall the user signal for voltage supervision\n
											      Values: -\n
												  Default: -\n*/

#define XM01BC_VOLT_SVSTATE	M_DEV_OF+0x07   /**< G: last supervision error state,
												  querried from the driver’s poll-loop\n
											      Values: bit 7..0 corresponds to supervised voltage 7..0\n
												  Default: -\n*/

#define XM01BC_ERRCNT_00	M_DEV_OF+0x08   /**< G: 8-bit error counter 0\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_01	M_DEV_OF+0x09   /**< G: 8-bit error counter 1\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_02	M_DEV_OF+0x0a   /**< G: 8-bit error counter 2\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_03	M_DEV_OF+0x0b   /**< G: 8-bit error counter 3\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_04	M_DEV_OF+0x0c   /**< G: 8-bit error counter 4\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_05	M_DEV_OF+0x0d   /**< G: 8-bit error counter 5\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_06	M_DEV_OF+0x0e   /**< G: 8-bit error counter 6\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_07	M_DEV_OF+0x0f   /**< G: 8-bit error counter 7\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_08	M_DEV_OF+0x10   /**< G: 8-bit error counter 8\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_09	M_DEV_OF+0x11   /**< G: 8-bit error counter 9\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_10	M_DEV_OF+0x12   /**< G: 8-bit error counter 10\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_11	M_DEV_OF+0x13   /**< G: 8-bit error counter 11\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_12	M_DEV_OF+0x14   /**< G: 8-bit error counter 12\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_13	M_DEV_OF+0x15   /**< G: 8-bit error counter 13\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_14	M_DEV_OF+0x16   /**< G: 8-bit error counter 14\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_CLR	M_DEV_OF+0x17   /**< S: clear all error counters\n
											      Values: -\n
												  Default: -\n*/

#define XM01BC_ERR_LAST		M_DEV_OF+0x18   /**< G: get last error\n
											      Values: 0x00..0xff\n
												  Default: -\n*/

#define XM01BC_ERR_RST_RSN	M_DEV_OF+0x19   /**< G: get last reset reason\n
											      Values: 0x00..0xff\n
												  Default: -\n*/

#define XM01BC_ERR_PWR_FLGS	M_DEV_OF+0x1a   /**< G: get power failure flags\n
											      Values: bit 7..0 corresponds to supervised voltage 7..0\n
												  Default: -\n*/

#define XM01BC_ERR_REG_CLR	M_DEV_OF+0x1b   /**< S: clear all error registers\n
											      Values: -\n
												  Default: -\n*/

#define XM01BC_PWRCYCL_CNT	M_DEV_OF+0x1c   /**< G: get 16-bit power cycle counter\n
											      Values: 0x00..0xffff\n
												  Default: -\n*/

#define XM01BC_OPHRS_CNT	M_DEV_OF+0x1d   /**< G: get 16-bit operating hours counter\n
											      Values: 0x00..0xffff\n
												  Default: -\n*/

#define XM01BC_LED_CTRL		M_DEV_OF+0x1e   /**< G/S: get/set LED state\n
											      Values: 0x00..0x01\n
												  Default: -\n*/

#define XM01BC_ATX_PWR_OK_MODE	(XM01BC_EXT_PWR_OK_MODE) /**< Synonym for XM01BC_EXT_PWR_OK_MODE*/

#define XM01BC_EXT_PWR_OK_MODE	M_DEV_OF+0x1f   /**< G/S: get/set EXT_PWR_OK failure mode\n
											      Values: 0x00..0x01\n
												  Default: -\n*/

#define XM01BC_RESUME_MODE	M_DEV_OF+0x20   /**< G/S: get/set power resume mode\n
											      Values: 0x00..0x02\n
												  Default: -\n*/

#define XM01BC_RST_IN_MODE	M_DEV_OF+0x21   /**< G/S: get/set RESET_IN mode\n
											      Values: 0x00..0x02\n
												  Default: -\n*/

#define XM01BC_NUM_ERRCNTRS	M_DEV_OF+0x22   /**< G Get number of error counters\n
											      Values: 0x00..0xff\n
												  Default: -\n*/

#define XM01BC_NUM_VOLTS	M_DEV_OF+0x23   /**< G Get number of supervised voltages\n
											      Values: 0x00..0xff\n
												  Default: -\n*/

#define XM01BC_HW_VARIANT	M_DEV_OF+0x24   /**< G Get number of detect hardware variant\n
											      Values: 0x00..0xffff\n
												  Default: -\n*/

#define XM01BC_ERRCNT_15	M_DEV_OF+0x25   /**< G: 8-bit error counter 15\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_16	M_DEV_OF+0x26   /**< G: 8-bit error counter 16\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_17	M_DEV_OF+0x27   /**< G: 8-bit error counter 17\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_18	M_DEV_OF+0x28   /**< G: 8-bit error counter 18\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_19	M_DEV_OF+0x29   /**< G: 8-bit error counter 19\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_20	M_DEV_OF+0x2A   /**< G: 8-bit error counter 20\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_21	M_DEV_OF+0x2B   /**< G: 8-bit error counter 21\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_22	M_DEV_OF+0x2C   /**< G: 8-bit error counter 22\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_23	M_DEV_OF+0x2D   /**< G: 8-bit error counter 23\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_24	M_DEV_OF+0x2E   /**< G: 8-bit error counter 24\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_25	M_DEV_OF+0x2F   /**< G: 8-bit error counter 25\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_26	M_DEV_OF+0x30   /**< G: 8-bit error counter 26\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_27	M_DEV_OF+0x31   /**< G: 8-bit error counter 27\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_28	M_DEV_OF+0x32   /**< G: 8-bit error counter 28\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_29	M_DEV_OF+0x33   /**< G: 8-bit error counter 29\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/

#define XM01BC_ERRCNT_30	M_DEV_OF+0x34   /**< G: 8-bit error counter 30\n
											      Values: 0x00..0xff\n
												  Default: 0\n*/





/* XM01BC specific Getstat/Setstat block codes */
#define XM01BC_BLK_FWREV		M_DEV_BLK_OF+0x00   /**< S: firmware revision\n
												      Values: six ascii characters\n
													  Default: -\n*/


#define XM01BC_BLK_FWREV_EXT	M_DEV_BLK_OF+0x01   /**< S: firmware revision\n
												      Values: six ascii characters\n
													  Default: -\n*/


/**@}*/

#define XM01BC_RESUME_OFF         (0x00)    /**< Power resume mode off */
#define XM01BC_RESUME_ON          (0x01)    /**< Power resume mode on */
#define XM01BC_RESUME_FORMER      (0x02)    /**< Power resume mode before power loss */

#define XM01BC_EXT_PWR_OK_IGN     (0x00)    /**< Ignore EXT power ok failure */
#define XM01BC_EXT_PWR_OK_ERR     (0x01)    /**< Treat EXT power ok failure as error */

#define XM01BC_RST_IN_ENABLED     (0x00)    /**< RESET_IN enabled */
#define XM01BC_RST_IN_BLOCKED     (0x01)    /**< RESET_IN blocked */

#define XM01BC_NUM_ERR_CNTRS      (14)      /**< Number of error counters */

#define XM01BC_BLK_FWREV_SIZE	XM01BCI_GETREV_NUM * sizeof(u_int16)

#ifndef  XM01BC_VARIANT
# define XM01BC_VARIANT XM01BC
#endif

# define _XM01BC_GLOBNAME(var,name) var##_##name
#ifndef _ONE_NAMESPACE_PER_DRIVER_
# define XM01BC_GLOBNAME(var,name) _XM01BC_GLOBNAME(var,name)
#else
# define XM01BC_GLOBNAME(var,name) _XM01BC_GLOBNAME(XM01BC,name)
#endif

#define __XM01BC_GetEntry    XM01BC_GLOBNAME(XM01BC_VARIANT,GetEntry)

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/
#ifdef _LL_DRV_
#ifndef _ONE_NAMESPACE_PER_DRIVER_
	extern void __XM01BC_GetEntry(LL_ENTRY* drvP);
#endif
#endif /* _LL_DRV_ */

/*-----------------------------------------+
|  BACKWARD COMPATIBILITY TO MDIS4         |
+-----------------------------------------*/
#ifndef U_INT32_OR_64
 /* we have an MDIS4 men_types.h and mdis_api.h included */
 /* only 32bit compatibility needed!                     */
 #define INT32_OR_64  int32
 #define U_INT32_OR_64 u_int32
 typedef INT32_OR_64  MDIS_PATH;
#endif /* U_INT32_OR_64 */


#ifdef __cplusplus
      }
#endif

#endif /* _XM01BC_DRV_H */

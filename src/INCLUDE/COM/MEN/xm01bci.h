/***********************  I n c l u d e  -  F i l e  ************************/
/*!  
 *        \file  xm01bci.h
 *
 *      \author  matthias.soehnlein@men.de
 *        $Date: 2014/04/07 15:31:20 $
 *    $Revision: 3.4 $
 * 
 *  	 \brief  Header file for XM01 board-controller firmware interface
 *                      
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: xm01bci.h,v $
 * Revision 3.4  2014/04/07 15:31:20  dpfeuffer
 * R: doxygen documentation groups for 13bl50w70 sw required
 * M: \defgroup _XM01BCI_PWR_FLAGS and _XM01BCI_LAST_ERRORS added
 *
 * Revision 3.3  2009/02/06 15:46:28  MSoehnlein
 * R:1. Added MM1/ XM2/ F19 Support
 * M:1.a) added "Hardware ID" query
 *     b) added "Number of Supervised Voltages" query
 *     c) added "Number of Error Counters" query
 *     d) added "RESET_IN Blocking Mode" query
 *     e) increased number of error counters to 31
 *     f) increased number of supervised voltages to 8
 *     g) renamed Board Controller Interface (BCI) from XM01BCI to CPUBCI
 *   2.a) Changed parameter of XM01BC_SetStat() and XM01BC_GetStat()
 *          from "int32 value" to "INT32_OR_64 value32_or_64"
 *          and "int32 *valueP" to "INT32_OR_64 *value32_or_64P"
 *
 * Revision 3.2  2008/08/01 09:46:39  MSöhnlein
 * Completed initial implementation
 *
 * Revision 3.1  2008/04/29 10:59:10  MSöhnlein
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN Mikro Elektronik GmbH, Nuremberg, Germany 
 ****************************************************************************/

#ifndef _XM01BCI_H
#define _XM01BCI_H

#ifdef __cplusplus
	extern "C" {
#endif


/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/

#define XM01BCI_WDOG_ON			(CPUBCI_WDOG_ON)
#define XM01BCI_WDOG_OFF		(CPUBCI_WDOG_OFF)
#define XM01BCI_WDOG_OFF_BYTE	(CPUBCI_WDOG_OFF_BYTE)
#define XM01BCI_WDOG_TRIG		(CPUBCI_WDOG_TRIG)
#define XM01BCI_WDOG_TIME		(CPUBCI_WDOG_TIME)
#define XM01BCI_WDOG_TIME_UNIT	(CPUBCI_WDOG_TIME_UNIT)
#define XM01BCI_WDOG_TIME_MIN	(CPUBCI_WDOG_TIME_MIN)
#define XM01BCI_WDOG_TIME_MAX	(CPUBCI_WDOG_TIME_MAX)
#define XM01BCI_WDOG_STATE		(CPUBCI_WDOG_STATE)
#define XM01BCI_WDOG_ARM		(CPUBCI_WDOG_ARM)



#define XM01BCI_RESUME_MODE		(CPUBCI_RESUME_MODE)
#define XM01BCI_RESUME_OFF		(CPUBCI_RESUME_OFF)
#define XM01BCI_RESUME_ON		(CPUBCI_RESUME_ON)
#define XM01BCI_RESUME_FORMER	(CPUBCI_RESUME_FORMER)


#define XM01BCI_ATX_PWR_OK_MODE	(CPUBCI_EXT_PWR_OK_MODE)
#define XM01BCI_ATX_PWR_OK_IGN	(CPUBCI_EXT_PWR_OK_IGN)
#define XM01BCI_ATX_PWR_OK_ERR	(CPUBCI_EXT_PWR_OK_ERR)


#define XM01BCI_DBG_MODE		(CPUBCI_DBG_MODE)


#define XM01BCI_SW_RESET		(CPUBCI_SW_RESET)
#define XM01BCI_SW_COLD_RESET	(CPUBCI_SW_COLD_RESET)
#define XM01BCI_SW_RESET_WORD	(CPUBCI_SW_RESET_WORD)



#define XM01BCI_VOLT_LOW(c)     (CPUBCI_VOLT_LOW(c))
#define XM01BCI_VOLT_HIGH(c)    (CPUBCI_VOLT_HIGH(c))
#define XM01BCI_VOLT_ACT(c)     (CPUBCI_VOLT_ACT(c))
#define XM01BCI_ERRCNT(ec)      (CPUBCI_ERRCNT(ec))
#define XM01BCI_ERR_CNT_CLR		(CPUBCI_ERR_CNT_CLR)
#define XM01BCI_ERR_CNT_BYTE	(CPUBCI_ERR_CNT_BYTE)
#define XM01BCI_GETREV_NUM		(CPUBCI_GETREV_NUM)
#define XM01BCI_GETREV(fww)		(CPUBCI_GETREV(fww))


#define XM01BCI_NUM_ERR_CNT		(CPUBCI_NUM_ERR_CNTRS)
#define XM01BCI_NUM_INP_VLT		(CPUBCI_NUM_VOLTS)
#define XM01BCI_HW_VARIANT		(CPUBCI_HW_VARIANT)


#define XM01BCI_ERR_LAST		(CPUBCI_ERR_LAST)
#define XM01BCI_ERR_PWR_FLAGS	(CPUBCI_ERR_PWR_FLAGS)
#define XM01BCI_ERR_RST_RSN		(CPUBCI_ERR_RST_RSN)
#define XM01BCI_PWRCYCL_CNT		(CPUBCI_PWRCYCL_CNT)
#define XM01BCI_OPHRS_CNT		(CPUBCI_OPHRS_CNT)

#define XM01BCI_ERR_REG_CLR		(CPUBCI_ERR_REG_CLR)
#define XM01BCI_ERR_REG_BYTE	(CPUBCI_ERR_REG_BYTE)


#define XM01BCI_LED_CTRL		(CPUBCI_LED_CTRL)


/* Debugging modes */
#define XM01BCI_DBG_NONE   		(CPUBCI_DBG_NONE)
#define XM01BCI_DBG_STATES 		(CPUBCI_DBG_STATES)
#define XM01BCI_DBG_1MS    		(CPUBCI_DBG_1MS)
#define XM01BCI_DBG_100MS  		(CPUBCI_DBG_100MS)
#define XM01BCI_DBG_WDOG  		(CPUBCI_DBG_WDOG)
#define XM01BCI_DBG_OPHRSCNT	(CPUBCI_DBG_OPHRSCNT)
#define XM01BCI_DBG_FULL   		(CPUBCI_DBG_FULL)


#define XM01BCI_VOLT_NUM        (2)		    /**< Number of supported voltages
                                                 Note: c=0..XM01BCI_VOLT_NUM-1 */
#define XM01BCI_ERRCNT_NUM      (15)	    /**< Number of supported error counters
                                                 Note: ec=0..XM01BCI_ERRCNT_NUM-1 */

/**
* \defgroup _XM01BCI_PWR_FLAGS BMC Power failure flags
*  The XM01BCI_PFF_XXX power failure flags indicates several power failure events,
*  recognized from the BMC firmware.
*  @{
*/
#define XM01BCI_PFF_INP			(0x01)		/**< Input voltage failure */
#define XM01BCI_PFF_ATX			(0x02)		/**< ATX_PWRGD failure */
#define XM01BCI_PFF_5130_1		(0x04)		/**< LVL_1_PWRGD failure */
#define XM01BCI_PFF_DDR			(0x08)		/**< DDR_PWRGD failure*/
#define XM01BCI_PFF_5130_2		(0x10)		/**< LVL_2_PWRGD failure */
#define XM01BCI_PFF_IMVP		(0x20)		/**< IMVP_PWRGD failure */
#define XM01BCI_PFF_BATT		(0x80)		/**< Battery failure */
/*! @} */


/* Last errors */
/**
* \defgroup _XM01BCI_LAST_ERRORS BMC Last error codes
*  The XM01BCI_ERR_XXX error codes are used from the BMC firmware to indicate
*  BMC error reasons.
*  @{
*/
#define XM01BCI_ERR_NONE				(CPUBCI_ERR_NONE)
#define XM01BCI_ERR_CTSTRPHC_SHTDWN		(0x01)		/**< Catastrophic shutdown requested by SCH */
#define XM01BCI_ERR_INP_TOO_LOW			(0x02)		/**< Input voltage below lower limit */
#define XM01BCI_ERR_INP_TOO_HIGH		(0x03)		/**< Input voltage above upper limit */
#define XM01BCI_ERR_NO_ATX_PWR_OK		(0x04)		/**< No ATX_PWR_OK signal detected */
#define XM01BCI_ERR_NO_PWRGD_5130_1		(0x05)		/**< Level 1 power good signal lost */
#define XM01BCI_ERR_NO_DDRVR_PWRGD		(0x06)		/**< DDR voltage power good signal lost */
#define XM01BCI_ERR_NO_PWRGD_5130_2		(0x07)		/**< Level 2 power good signal lost */
#define XM01BCI_ERR_NO_PM_CPU_PWRGD		(0x08)		/**< CPU voltage power good signal lost */
#define XM01BCI_ERR_BIOS_TIMEOUT_1		(0x09)		/**< First time BIOS live sign timed out */
#define XM01BCI_ERR_BIOS_TIMEOUT_2		(0x0A)		/**< Second time BIOS live sign timed out */
#define XM01BCI_ERR_BIOS_TIMEOUT_3		(0x0B)		/**< Third time BIOS live sign timed out */
#define XM01BCI_ERR_BIOS_TIMEOUT_4		(0x0C)		/**< Forth time BIOS live sign timed out */
#define XM01BCI_ERR_CPU_RST_TIMEOUT		(0x0D)		/**< CPU_RST timed out */
#define XM01BCI_INVALID_MAIN_STATE		(CPUBCI_INVALID_MAIN_STATE)
/*! @} */


/* Reset Reasons */
#define XM01BCI_RST_NONE				(CPUBCI_RST_NONE)
#define XM01BCI_RST_CTSTRPHC			(0x01)		/**< Catastrophic shutdown */
#define XM01BCI_RST_PWR_FAILURE			(0x02)		/**< Power failure */
#define XM01BCI_RST_EXT_REQ				(0x03)		/**< External reset detected */
#define XM01BCI_RST_WARM_REQ			(0x04)		/**< Warm reset requested by SCH */
#define XM01BCI_RST_COLD_REQ			(0x05)		/**< Cold reset requested by SCH */
#define XM01BCI_RST_EXT_WDOG			(0x06)		/**< External Watchdog (Main CPU) timeout reached */
#define XM01BCI_RST_BIOS_TIMEOUT_1		(0x07)		/**< First time BIOS live sign timed out */
#define XM01BCI_RST_BIOS_TIMEOUT_2		(0x08)		/**< Second time BIOS live sign timed out */
#define XM01BCI_RST_BIOS_TIMEOUT_3		(0x09)		/**< Third time BIOS live sign timed out */
#define XM01BCI_RST_BIOS_TIMEOUT_4		(0x0A)		/**< Forth time BIOS live sign timed out */
#define XM01BCI_RST_SW_REQ				(0x0B)		/**< Software reset request (SMBus) */
#define XM01BCI_RST_SW_COLD_REQ			(0x0C)		/**< Software cold reset request (SMBus) */
#define XM01BCI_RST_INT_WDOG			(0x0D)		/**< Internal Watchdog (PIC) timeout reached */
#define XM01BCI_RST_INT_BRWN_OUT		(0x0E)		/**< Internal (PIC) brown out reset */
#define XM01BCI_RST_PWR_BTN				(0x0F)		/**< Power button override */
#define XM01BCI_RST_S4S5_REQ			(0x10)		/**< S4/S5 sleep request by SCH */
#define XM01BCI_RST_CPU_RST_TIMEOUT		(0x11)		/**< CPU_RST timed out */


/* Error counters */
#define XM01BCI_ERR_HOT			(CPUBCI_ERRCNT( 0))	/* too hot */
#define XM01BCI_ERR_HOTSHTDWN 	(CPUBCI_ERRCNT( 1))	/* too hot and shutdown */
#define XM01BCI_ERR_ATX			(CPUBCI_ERRCNT( 2))	/* PWGD ATX failure */
#define XM01BCI_ERR_5130_1		(CPUBCI_ERRCNT( 3))	/* PWGD LVL_1 failure */
#define XM01BCI_ERR_DDR			(CPUBCI_ERRCNT( 4))	/* PWGD DDR failure */
#define XM01BCI_ERR_5130_2		(CPUBCI_ERRCNT( 5))	/* PWGD LVL_2 failure  */
#define XM01BCI_ERR_CPU			(CPUBCI_ERRCNT( 6))	/* PWGD CPU failure */
#define XM01BCI_ERR_EXT_WDOG	(CPUBCI_ERRCNT( 7))	/* Main CPU watchdog fired */
#define XM01BCI_ERR_INT_WDOG	(CPUBCI_ERRCNT( 8))	/* PIC watchdog fired */
#define XM01BCI_ERR_PIC_BO		(CPUBCI_ERRCNT( 9))	/* PIC brown-out */
#define XM01BCI_ERR_BIOS_1		(CPUBCI_ERRCNT(10))	/* BIOS timeout 1 counter */
#define XM01BCI_ERR_BIOS_2		(CPUBCI_ERRCNT(11))	/* BIOS timeout 2 counter */
#define XM01BCI_ERR_BIOS_3		(CPUBCI_ERRCNT(12))	/* BIOS timeout 3 counter */
#define XM01BCI_ERR_BIOS_4		(CPUBCI_ERRCNT(13))	/* BIOS timeout 4 counter */
#define XM01BCI_ERR_CPU_RST		(CPUBCI_ERRCNT(14))	/* CPU_RST failure */



/*-----------------------------------------+
|  TYPEDEFS                                |
+-----------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+-----------------------------------------*/

/*-----------------------------------------+
|  variable declarations                   |
+-----------------------------------------*/



#ifdef __cplusplus
	}
#endif

#endif	/* _XM01BCI_H */


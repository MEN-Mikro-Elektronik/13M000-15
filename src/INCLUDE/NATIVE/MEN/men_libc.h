/***********************  I n c l u d e  -  F i l e  ************************/
/*  
 *        \file  men_libc.h
 *
 *      \author  kp
 *        $Date: 2003/08/04 16:09:18 $
 *    $Revision: 1.2 $
 * 
 *       \brief  Header file for library men-libc.a
 *                      
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: men_libc.h,v $
 * Revision 1.2  2003/08/04 16:09:18  kp
 * cosmetics
 *
 * Revision 1.1  2002/04/26 16:25:22  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#ifndef _MEN_LIBC_H
#define _MEN_LIBC_H

#ifdef __cplusplus
	extern "C" {
#endif

/*! \mainpage
  
  This documentation describes the functions provided by the \c men-libc.a
  library. It containes supplementary routines developed by MEN.

  The user application must be linked with \c libmen-libc.a, i.e. the
  \c Makefile or \c common.mk should contain the following lines:

  \code
  LIBS += men-libc
  EXTRA_INCVPATH += c:/menqnx/src/INCLUDE/COM c:/menqnx/src/INCLUDE/NATIVE
  EXTRA_LIBVPATH += c:/menqnx/$(OS)/$(CPUDIR)/usr/lib
  \endcode

  Substitute \c c:/menqnx with the directory where the MEN BSP tree has
  been installed.

  Additionally, your souce code should include the header file:
  \code
  #include <MEN/men_libc.h>
  \endcode


  \section smb System managment bus (SMB) functions

  The \ref smbfunc may be used to access devices on the SM bus. The
  BSP specific smb-server must be running in order to access the
  devices.

  
  \section vme VMEbus functions

  The \ref vmefunc can be used to access the (BSP specific) VME
  resource manager. It provides functions for:

  - VME Interrupt handling
  - Issue VME interrupts
  - Translate between VME and CPU addresses 


*/

/*! \page dummy
\menimages
*/


/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#ifndef _SMB_H
/*! \defgroup smbcodes SMB error codes
 */
/**@{*/
#define SMB_ERR_NO           		0	/**< no error */
#define SMB_ERR_DESCRIPTOR   		1	/**< no error. */
#define SMB_ERR_NO_MEM       		2	/**< no memory */
#define SMB_ERR_ADDR    	 		3	/**< bad address */
#define SMB_ERR_BUSY    	 		4	/**< bus busy */
#define SMB_ERR_COLL    	 		5 	/**< SMB collision */
#define SMB_ERR_NO_DEVICE	 		6	/**< can't contact device */
#define SMB_ERR_PARAM		 		7	/**< bad parameter */
#define SMB_ERR_SERVER				255	/**< communication pb with server */
#endif
/**@}*/

/*! \defgroup vmecodes Space/Data width codes for MenC_VmeBusToPhys() */
/**@{*/
#define VME_MODE_A16		0x00000000 /**< VME A16 space */
#define VME_MODE_A24		0x00000001 /**< VME A24 space */
#define VME_MODE_A32		0x00000002 /**< VME A32 space */
#define VME_MODE_USER		0x00000010 /**< AM codes: User access */
#define VME_MODE_SUPER		0x00000020 /**< AM codes: Supervisor access */
#define VME_MODE_DATA		0x00000040 /**< AM codes: data access */
#define VME_MODE_PGM		0x00000080 /**< AM codes: program access */

#define VME_MODE_PREN		0x00002000 /* allow prefetched reads */
#define VME_MODE_PWEN		0x00004000 /* allow posted writes */
#define VME_MODE_LLRMW		0x00008000 /* lock PCI on VME RMW cycles */

#define VME_MODE_D08		0x00000000 /**<  8 bit data width */
#define VME_MODE_D16		0x00400000 /**< 16 bit data width */
#define VME_MODE_D32		0x00800000 /**< 32 bit data width */
#define VME_MODE_D64		0x00C00000 /**< 64 bit data width */

#define VME_MODE_ALLOW_BLT	0x00000100 /* allow VME block transfers */
#define VME_MODE_TO_VME		0x80000000 /* direction: Local -> VME */
#define VME_MODE_FROM_VME	0x00000000 /* direction: VME -> Local */
/**@}*/


/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/

/* SMB */
int MenC_SmbAttach( void );

int MenC_SmbWrite1( int fd, 
					uint8_t smbBusNo, 
					uint8_t smbAddr, 
					uint8_t data ); 

int MenC_SmbWrite2( int fd, 
					uint8_t smbBusNo, 
					uint8_t smbAddr, 
					uint8_t data1,
					uint8_t data2); 

int MenC_SmbRead( int fd, 
				  uint8_t smbBusNo, 
				  uint8_t smbAddr, 
				  uint8_t *dataP);

int MenC_SmbWriteRead( int fd, 
					   uint8_t smbBusNo, 
					   uint8_t smbAddr, 
					   uint8_t outData,
					   uint8_t *inDataP);

int MenC_SmbWriteRead2( int fd, 
						uint8_t smbBusNo, 
						uint8_t smbAddr, 
						uint8_t outData,
						uint8_t *inData1P,
						uint8_t *inData2P);

void MenC_SmbDetach( int fd );



int MenC_VmeAttach( void );
void MenC_VmeDetach( int fd );
int MenC_VmeInterruptAttachEvent( 
	int fd,
	int vector, 
	int level,
	const struct sigevent *event,
	unsigned flags );
int MenC_VmeInterruptDetachEvent( int fd, int id);
int MenC_VmeIssueIrq( int fd, int level, int vector );
int MenC_VmeCheckIrq ( int fd, int *plevels );
int MenC_VmeIrqLevelUnmask ( int fd, int level, int id );
int MenC_VmeIrqLevelMask ( int fd, int level, int id );
int MenC_VmeBusToPhys( int fd, unsigned space, unsigned long vmeAdrs, 
					   uint64_t *physAdrsP );


#ifdef __cplusplus
	}
#endif

#endif	/* _MEN_LIBC_H */

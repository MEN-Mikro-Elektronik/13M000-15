/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  bk_intern.h
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2004/03/23 17:58:43 $
 *    $Revision: 1.1 $
 * 
 *	   \project  MDIS4 QNX
 *  	 \brief  private definitions for BBIS kernel library
 *
 *    \switches  
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: bk_intern.h,v $
 * Revision 1.1  2004/03/23 17:58:43  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000-2004 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#ifndef _BK_INTERN_H
#  define _BK_INTERN_H

#  ifdef __cplusplus
      extern "C" {
#  endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <dlfcn.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/pci.h>

#include <MEN/men_typs.h>
#include <MEN/mdis_err.h>

#include <MEN/oss.h>
#include <MEN/dbg.h>
#include <MEN/desc.h>
#include <MEN/desctyps.h>
#include <MEN/bb_defs.h>
#include <MEN/bb_entry.h>
#include <MEN/bbis_bk.h>


/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define DBG_MYLEVEL         bk_dbglevel
#define DBH 				G_bkDbh
#define OSH                 G_bkOsh
#define BK_MAX_DRVNAME      39

/*! #defines for the global BBIS semaphore */
#define BK_LOCK( err ) \
                        err=OSS_SemWait(OSH,G_bkLockSem,OSS_SEM_WAITFOREVER)

#define BK_UNLOCK()     OSS_SemSignal(OSH,G_bkLockSem)


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*! driver node structure */
typedef struct {
	OSS_DL_NODE node;			     /*!< node in registered drivers list */
	char  drvName[BK_MAX_DRVNAME+1]; /*!< driver name */
    void  *drvHandle;                /*!< handle for QNX shared library */
	void  (*getEntry)(BBIS_ENTRY *); /*!< GetEntry function ptr */
} BK_DRV;

/*! BBIS device structure */
typedef struct {
	OSS_DL_NODE node;               /*!< node in registered devices list */
	char         devName[BK_MAX_DEVNAME+1]; /*!< device name */
    int	         useCount;	        /*!< number of opens */
    OSS_HANDLE  *osh;               /*!< bb driver's OSS handle */

	BK_DRV       *drv;              /*!< bb driver structure */
	BBIS_HANDLE  *bb;               /*!< bb driver's handle */
} BK_DEV;


/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/

#  ifdef __cplusplus
      }
#  endif

#endif/*_BK_INTERN_H*/

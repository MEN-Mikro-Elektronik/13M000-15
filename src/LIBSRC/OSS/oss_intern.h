/***********************  I n c l u d e  -  F i l e  ************************
 *  
 *         Name: oss_intern.h
 *
 *       Author: kp
 *        $Date: 2004/03/19 14:59:27 $
 *    $Revision: 1.2 $
 * 
 *  Description: oss internal defines
 *     Switches: DBG
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_intern.h,v $
 * Revision 1.2  2004/03/19 14:59:27  ub
 * adapted for MDIS
 *
 * Revision 1.1  2003/08/04 14:01:36  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/

#ifndef _OSS_INTERN_H
#  define _OSS_INTERN_H

#  ifdef __cplusplus
extern "C"
{
#  endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/netmgr.h>
#include <hw/pci.h>

#include <MEN/men_typs.h>
#include <MEN/mdis_err.h>
#include <MEN/men_pci2vme.h>
#include <MEN/men_libc.h>

#include <MEN/dbg.h>            /* debug module */

/*-----------------------------------------+
 |  TYPEDEFS                                |
 +------------------------------------------*/
typedef struct
{
    char instName[40];          /* name of OSS instance */
    int32 dbgLevel;             /* debug level */
    DBG_HANDLE *dbh;            /* debug handle */
    int pciHandle;              /* PCI server attach handle */
    int vmeHandle;              /* VME server handle */
    intrspin_t intrSpinLock;    /* Spin Lock for SMP critical sections */
    pid_t (*currentPid)( void );/* function to find out the pid of the */
                                /* current process */
} OSS_HANDLE;

#include <MEN/oss.h>

/*-----------------------------------------+
 |  DEFINES & CONST                         |
 +------------------------------------------*/
#define DBG_MYLEVEL oss->dbgLevel
#define DBH         oss->dbh

#define OSS_ALARM_SIGNAL    SIGALRM     /* signal used to notify thread */

/*-----------------------------------------+
 |  GLOBALS                                 |
 +------------------------------------------*/
extern int oss_pci_slot_devnbrs[16];

/*-----------------------------------------+
 |  PROTOTYPES                              |
 +------------------------------------------*/

#  ifdef __cplusplus
}
#  endif

#endif/*_OSS_INTERN_H*/

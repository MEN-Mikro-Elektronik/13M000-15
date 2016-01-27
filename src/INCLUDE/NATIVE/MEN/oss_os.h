/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: oss_os.h
 *
 *       Author: ub
 *        $Date: 2011/09/02 19:01:35 $
 *    $Revision: 1.5 $
 *
 *  Description: QNX specific data types and defines
 *
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_os.h,v $
 * Revision 1.5  2011/09/02 19:01:35  channoyer
 * R: Missing header
 * M: Include pthread.h
 *
 * Revision 1.4  2011/09/01 18:09:14  channoyer
 * R: incorrect typedef for OSS_SPINL_HANDLE
 * M: change typedef to pthread_spinlock_t
 *
 * Revision 1.3  2011/01/19 15:08:12  channoyer
 * R: OSS_SpinLockXxx function introduction
 * M: OSS_SPINL_HANDLE added
 *
 * Revision 1.2  2004/03/19 11:37:17  ub
 * Added QNX specific definitions
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/
#ifndef _OSS_OS_H
#define _OSS_OS_H

#ifdef __cplusplus
   extern "C" {
#endif

#include <stdarg.h>
#include <sys/neutrino.h>
#include <pthread.h>

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
#define NO_CALLBACK		/* flag for oss.h not to include callback protos */
#define NO_SHARED_MEM	/* flag for oss.h not to include shared mem protos */

#define OSS_DBG_DEFAULT 0xffffffff

#if defined __BIGENDIAN__
# define _BIG_ENDIAN_
#endif

#if defined __LITTLEENDIAN__
# define _LITTLE_ENDIAN_
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
typedef void OSS_IRQ_HANDLE;

#define OSS_HAS_IRQMASKR
typedef u_int32 OSS_IRQ_STATE;

#define OSS_HAS_ALARMMASK
typedef OSS_IRQ_STATE OSS_ALARM_STATE;


/*--- SEMAPHORE HANDLE ---*/
#ifndef _OSS_SEM_C
typedef void OSS_SEM_HANDLE;
#else  /* _OSS_SEM_C defined */

typedef struct
{
	union {
		pthread_mutex_t mutex;             /* QNX mutex */
		sem_t           sem;               /* QNX semaphore */
	} h;
	int32  semType;				           /* OSS_SEM_BIN or OSS_SEM_COUNT */
} OSS_SEM_HANDLE;

#endif /* _OSS_SEM_C */


/*--- SIGNAL HANDLE ---*/
#ifndef _OSS_SIG_C
typedef void OSS_SIG_HANDLE;
#else  /* _OSS_SIG_C defined */

typedef struct
{
	pid_t pid;					           /* process id */
	int   sig;					           /* signal number */
} OSS_SIG_HANDLE;

#endif /* _OSS_SIG_C */


/*--- ALARM HANDLE ---*/
#ifndef _OSS_ALARM_C
typedef void OSS_ALARM_HANDLE;
#else /* _OSS_ALARM_C defined */

typedef struct 
{
	timer_t           timer;                /* QNX alarm timer */
	struct sigaction  oldAction;            /* saved previous action */
	int               active;               /* alarm currently active */
	int               cyclic;               /* cyclic alarm */
	void              (*funct)(void *arg);	/* alarm routine to be called */
	void              *arg;                 /* value to pass to function */
} OSS_ALARM_HANDLE;

#endif /* _OSS_ALARM_HANDLE */

/*--- SPIN LOCK HANDLE ---*/
       typedef pthread_spinlock_t OSS_SPINL_HANDLE;

#ifndef _OSS_INTERN_H
typedef void OSS_HANDLE;
#endif

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
extern int32  OSS_Init( 
    char *instName, 
    OSS_HANDLE **ossP, 
    pid_t currentPid( void ) );
extern int32  OSS_Exit( OSS_HANDLE **ossP );


#ifdef __cplusplus
   }
#endif
#endif /*_OSS_OS_H*/


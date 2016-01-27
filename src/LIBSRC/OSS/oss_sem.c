/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_sem.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2004/03/24 09:40:51 $
 *    $Revision: 1.1 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  Semaphore routines
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_sem.c,v $
 * Revision 1.1  2004/03/24 09:40:51  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#define _OSS_SEM_C
#include "oss_intern.h"


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/

/*! \page qnxsemusage

  \section qnxsemusagesect QNX notes to OSS semaphores

  This implementation uses POSIX semaphores.

  See \ref osssemusagesect for more info.
*/

/**********************************************************************/
/** Create a semaphore.
 *
 * \copydoc oss_specification.c::OSS_SemCreate()
 *
 * See \ref qnxsemusagesect for more info.
 *
 * \sa OSS_SemRemove, OSS_SemWait, OSS_SemSignal
 */
int32 OSS_SemCreate(
    OSS_HANDLE 	   *oss,
    int32          semType,
    int32          initVal,
    OSS_SEM_HANDLE **semP)
{
    OSS_SEM_HANDLE *semHandle;

    DBGWRT_1((DBH,"OSS_SemCreate()\n"));

	*semP = NULL;

	/* sanity check */
	if( initVal < 0 )
		return( ERR_OSS_ILL_PARAM );

	if( (semType == OSS_SEM_BIN) && (initVal > 1) )
		return( ERR_OSS_ILL_PARAM );


	/* allocate memory for semaphore */
	semHandle = malloc( sizeof(OSS_SEM_HANDLE) );

	if( semHandle == NULL )
		return( ERR_OSS_MEM_ALLOC );

	semHandle->semType	= semType;

	if( sem_init( &(semHandle->h.sem), 0, initVal ) ) {
		free( semHandle );
		return( ERR_OSS_SEM_CREATE );
	}

	*semP = semHandle;

    return( ERR_SUCCESS );
}/*OSS_SemCreate*/

/**********************************************************************/
/** Destroy semaphore handle.
 *
 * \copydoc oss_specification.c::OSS_SemRemove()
 *
 * See \ref qnxsemusagesect for more info.
 *
 * \sa OSS_SemCreate, OSS_SemWait, OSS_SemSignal
 */
int32 OSS_SemRemove(
    OSS_HANDLE *oss,
    OSS_SEM_HANDLE** semHandleP)
{
    OSS_SEM_HANDLE *semHandle;

    DBGWRT_1((DBH,"OSS_SemRemove() OSS_SEM_HANDLE = 0x%p\n", *semHandleP ));

	semHandle   = *semHandleP;
	*semHandleP = NULL;

	if( sem_destroy( &(semHandle->h.sem) ) ) {
		return( ERR_OSS_SEM_REMOVE );
	}

	if( semHandle )
		free( semHandle );

	return( ERR_SUCCESS );
}/*OSS_SemRemove*/


/**********************************************************************/
/** Add given number of milliseconds to the current system time
 *
 *  \param  time  \IN   timespec struct for result
 *  \param  msec  \IN   milliseconds to add
 *
 *  \return resulting time
 */
static struct timespec *
addToClock( struct timespec *time, int32 msec )
{
    _uint64         nsec;

    /* compute absolute time from current time and timeout */
    clock_gettime( CLOCK_REALTIME, time );
    nsec = timespec2nsec( time ) + (_uint64)msec * 1000000;
    nsec2timespec( time, nsec );

    return( time );
}


/**********************************************************************/
/** Wait for semaphore.
 *
 * \copydoc oss_specification.c::OSS_SemWait()
 *
 * See \ref qnxsemusagesect for more info.
 *
 * \qnx
 * - all signals < \c SIGRTMIN are assumed to be deadly.
 *
 * \sa OSS_SemCreate, OSS_SemRemove, OSS_SemSignal
 */
int32 OSS_SemWait(
    OSS_HANDLE      *oss,
    OSS_SEM_HANDLE  *sem,
    int32           msec)
{
    int   res;
	int32 error = ERR_SUCCESS;


	if( msec == OSS_SEM_WAITFOREVER ) {
		res = sem_wait( &(sem->h.sem) );
	}
	else if( msec == OSS_SEM_NOWAIT ) {
		res = sem_trywait( &(sem->h.sem) );
	}
	else {
        struct timespec time;
		res = sem_timedwait( &(sem->h.sem), addToClock( &time, msec ) );
	}

	if( res ) {
		switch( errno ) {
			case EBUSY:
			case EAGAIN:				
			case EDEADLK:				
			case ETIMEDOUT: error = ERR_OSS_TIMEOUT;      break;
			case EINTR:     error = ERR_OSS_SIG_OCCURED;  break;
			default:        error = ERR_SUCCESS;
		}
	}
	else 
		error = ERR_SUCCESS;
			
	return( error );
}/*OSS_SemWait*/

/**********************************************************************/
/** Signal semaphore.
 *
 * \copydoc oss_specification.c::OSS_SemSignal()
 *
 * See \ref qnxsemusagesect for more info.
 *
 * \sa OSS_SemCreate, OSS_SemRemove, OSS_SemWait
 */
int32 OSS_SemSignal(
    OSS_HANDLE *oss,
    OSS_SEM_HANDLE* sem)
{
    DBGWRT_1((DBH,"OSS_SemSignal() sem = 0x%p\n", sem ));

	if( sem_post( &(sem->h.sem) ) )
		return( ERR_OSS_ILL_HANDLE );

	return( ERR_SUCCESS );
}/*OSS_SemSignal*/

/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  oss_alarm.c
 *
 *      \author  ulrich.bogensperger@men.de
 *        $Date: 2011/08/10 17:29:27 $
 *    $Revision: 1.3 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  Alarm routines
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_alarm.c,v $
 * Revision 1.3  2011/08/10 17:29:27  channoyer
 * R:1. MDIS failed when Alarms used in LL driver
 *   2. Error with large value for the timer
 * M:1.a) Change AlarmHandler to run it as a thread
 *   1.b) Use pulse for event when the timer fires
 *   2. Use of u_int64
 *
 * Revision 1.2  2010/03/24 18:31:55  channoyer
 * R: division by zero when cp.nsec < 1000000
 * M: make the conversion to msec after the round
 *
 * Revision 1.1  2004/03/24 09:40:46  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003-2010 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/

#define _OSS_ALARM_C
#include "oss_intern.h"

static void* AlarmHandler( void *arg );

/*! \page qnxossalarmusage

  \section qnxossalarmusagesect QNX notes to OSS alarms

  Under QNX, the alarms are implemented using a QNX timer which sends
  a SIGALRM event to the creating thread. On receival the specified handler
  routine is called in routine AlarmHandler().

  Alarm routines are called in the context of the thread which called 
  OSS_AlarmCreate. See the chapter about QNX system call SignalAction()
  for more info.

  See \ref ossalarmusagesect for more info.
*/

#define CODE_TIMER   1

pthread_t           G_alarmThreadId = 0;
pthread_mutex_t     G_alarmMutex = PTHREAD_MUTEX_INITIALIZER;
int                 G_coid;

/**********************************************************************/
/** Create an alarm.
 *
 * \copydoc oss_specification.c::OSS_AlarmCreate()
 *
 * See \ref qnxossalarmusagesect for more info.
 *
 * \sa OSS_AlarmRemove, OSS_AlarmSet, OSS_AlarmClear
 */
int32 OSS_AlarmCreate(
    OSS_HANDLE       *oss,
    void             (*funct)(void *arg),
	void             *arg,
    OSS_ALARM_HANDLE **alarmP)
{
	struct sigevent   event;

    DBGWRT_1((DBH,"OSS - OSS_AlarmCreate func=0x%p arg=0x%lx\n", funct, arg));

	pthread_mutex_lock( &G_alarmMutex );
    if( 0 == G_alarmThreadId ) {
    	pthread_create( &G_alarmThreadId, NULL, AlarmHandler, NULL );
    }
	pthread_mutex_unlock( &G_alarmMutex );

	/* get memory for handle */
	if( (*alarmP = malloc( sizeof( OSS_ALARM_HANDLE ) )) == NULL )
       return( ERR_OSS_MEM_ALLOC );

	SIGEV_PULSE_INIT (&event, G_coid,
					  SIGEV_PULSE_PRIO_INHERIT, CODE_TIMER, *alarmP);

	/* create timer */
	if( ((*alarmP)->timer = TimerCreate_r( CLOCK_REALTIME, &event )) == -1 ) {
		free( *alarmP );
		return( ERR_OSS_ALARM_CREATE );
	}

	(*alarmP)->arg    = arg;
	(*alarmP)->funct  = funct;
	(*alarmP)->active = FALSE;

	return ERR_SUCCESS;
}


/**********************************************************************/
/** Destroys alarm handle.
 *
 * \copydoc oss_specification.c::OSS_AlarmRemove()
 *
 * See \ref qnxossalarmusagesect for more info.
 *
 * \sa OSS_AlarmCreate, OSS_AlarmSet, OSS_AlarmClear
 */
int32 OSS_AlarmRemove(
    OSS_HANDLE     *oss,
    OSS_ALARM_HANDLE **alarmP)
{

	if( TimerDestroy_r( (*alarmP)->timer ) )
		return( ERR_OSS_ALARM_REMOVE );

	free( *alarmP );
	*alarmP = NULL;

	return( ERR_SUCCESS );
}


/**********************************************************************/
/** Activate an installed alarm routine
 *
 * \copydoc oss_specification.c::OSS_AlarmSet()
 *
 * See \ref qnxossalarmusagesect for more info.
 *
 * \sa OSS_AlarmCreate, OSS_AlarmRemove, OSS_AlarmClear
 */
int32 OSS_AlarmSet(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE *alarm,
    u_int32          msec,
    u_int32          cyclic,
    u_int32          *realMsecP)
{
	struct _itimer      it;
	struct _clockperiod cp;
	int                 rc;
	u_int64       nsec;

	DBGWRT_1((DBH, "OSS_AlarmSet: msec=%d  cyclic=%d\n", msec, cyclic ));

	/* round desired time in respect to timer resolution */
	ClockPeriod_r( CLOCK_REALTIME, NULL, &cp, 0 );

	nsec = (u_int64)msec * (u_int64)cp.nsec;

	if( realMsecP ) {
	  msec = nsec / 1000000;
	  *realMsecP = msec;
	}

	/* set time */
	it.nsec          = nsec;
	it.interval_nsec = 0;

	if( cyclic ) {
		it.interval_nsec = it.nsec;
	}

	/* start timer */
	if(( rc = TimerSettime_r( alarm->timer, 0, &it, NULL ) )) {
		DBGWRT_ERR(( DBH,"*** Error: TimerSettime_r returned %d\n", rc ));
		return( ERR_OSS_ALARM_SET );
    }

	alarm->cyclic = cyclic;
	alarm->active = TRUE;

	return( ERR_SUCCESS );
}

/**********************************************************************/
/** Deactivate an installed alarm routine
 *
 * \copydoc oss_specification.c::OSS_AlarmClear()
 *
 * See \ref qnxossalarmusagesect for more info.
 *
 * \sa OSS_AlarmCreate, OSS_AlarmRemove, OSS_AlarmSet
 */
int32 OSS_AlarmClear(
    OSS_HANDLE       *oss,
    OSS_ALARM_HANDLE *alarm)
{
	/* disarm timer */
	if( OSS_AlarmSet( oss, alarm, 0, 0, NULL ) )
		return( ERR_OSS_ALARM_CLR );

	alarm->active = FALSE;

	return( ERR_SUCCESS ) ;
}

/**********************************************************************/
/** Mask alarms
 *
 * \copydoc oss_specification.c::OSS_AlarmMask()
 *
 * \qnx Masks \b all processor interrupts.
 * \sa OSS_AlarmRestore()
 */
OSS_ALARM_STATE OSS_AlarmMask( OSS_HANDLE *oss )
{
	DBGWRT_1((DBH, "OSS_AlarmMask: disabling interrupts\n" ));

	InterruptDisable();

	return( 0 ) ;
}

/**********************************************************************/
/** Unmask alarms
 *
 * \copydoc oss_specification.c::OSS_AlarmRestore()
 *
 * \sa OSS_AlarmMask()
 */
void OSS_AlarmRestore( OSS_HANDLE *oss, OSS_ALARM_STATE oldState )
{
	DBGWRT_1((DBH, "OSS_AlarmRestore: enabling interrupts\n" ));

	InterruptEnable();
}

/**********************************************************************/
void* AlarmHandler( void *arg )
{
	int               rcvid;
	struct _pulse     pulse;
	OSS_ALARM_HANDLE *alarm = NULL;
	int               chid;

    chid = ChannelCreate (0);
    if (chid == -1) {
        exit (EXIT_FAILURE);
    }

    G_coid = ConnectAttach (0, 0, chid, 0, 0);
    if (G_coid == -1) {
        exit (EXIT_FAILURE);
    }

	for (;;) {
		rcvid = MsgReceive(chid, &pulse, sizeof(pulse), NULL);
		if (rcvid == 0) { /* we got a pulse */
			switch ( pulse.code ) {
			case CODE_TIMER:
				alarm = (OSS_ALARM_HANDLE *) pulse.value.sival_ptr;

				/* jump into installed function */
				if( alarm->funct ) {
					alarm->funct(alarm->arg);
				}

				if( ! alarm->cyclic )
					alarm->active = FALSE;

				break;
			} /* else other pulses ... */
		} /* else other messages ... */
	}

}

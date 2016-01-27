/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  key.c
 *
 *      \author  ub
 *        $Date: 2004/03/24 09:45:28 $
 *    $Revision: 1.1 $
 * 
 * 	   \project  MDIS4QNX USR_OSS lib
 *
 *  	 \brief  Check for key pressed
 *
 *    \switches  none
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: key.c,v $
 * Revision 1.1  2004/03/24 09:45:28  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
static const char RCSid[]="$Id: key.c,v 1.1 2004/03/24 09:45:28 ub Exp $";

#include "uos_int.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>


struct termios G_oldTerm;

static void SetRawMode()
{
	struct termios new;

	tcgetattr( 0, &G_oldTerm );
	new = G_oldTerm;
	new.c_lflag &= ~ICANON;
	tcsetattr( 0, TCSADRAIN, &new );
}

static void RestoreMode()
{
	tcsetattr( 0, TCSADRAIN, &G_oldTerm );
}

/**********************************************************************/
/** Check if any key pressed on console
 * 
 * \copydoc usr_oss_specification.c::UOS_KeyPressed()
 *
 * \qnx temporarily switches console to raw mode
 *
 * \sa UOS_KeyWait
 */
int32 UOS_KeyPressed( void )
{
	char c;
	int nread=0;

	SetRawMode();
	ioctl( 0, FIONREAD, &nread );
	if( nread > 0 ){
		read(0,&c,1);				/* read this char */
		RestoreMode();
		return c;
	}
	
	RestoreMode();
	return -1;
}

/**********************************************************************/
/** Wait until any key pressed on console
 * 
 * \copydoc usr_oss_specification.c::UOS_KeyWait()
 *
 * \qnx temporarily switches console to raw mode
 *
 * \sa UOS_KeyPressed
 */
int32 UOS_KeyWait( void )
{
	char c;

	SetRawMode();

	if (read(0,&c,1) == -1){	/*  wait for one char - read error ? */
		RestoreMode();
		return(-1);				/* all done. */
	}
	else {
		RestoreMode();
		return((int32)c);		/* return char */
	}
}


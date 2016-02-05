/****************************************************************************
 ************                                                    ************
 ************                   WDOG_SIMP                        ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: ds
 *        $Date: 2009/08/31 14:14:23 $
 *    $Revision: 1.3 $
 *
 *  Description: Simple example program for Watchdog drivers.
 *
 *               Start Watchdog and trigger Watchdog until keypress,
 *               !!! THEN THE SYSTEM WILL BE RESET !!!.
 *
 *     Required: libraries: mdis_api, usr_oss
 *     Switches: -
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: wdog_simp.c,v $
 * Revision 1.3  2009/08/31 14:14:23  MRoth
 * R: Porting to MDIS5
 * M: added support for 64bit (MDIS_PATH)
 *
 * Revision 1.2  2004/09/02 15:14:20  dpfeuffer
 * minor modifications for MDIS4/2004 conformity
 *
 * Revision 1.1  1999/06/23 12:07:49  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/mdis_api.h>
#include <MEN/usr_oss.h>
#include <MEN/wdog.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define WATCHDOG_TIME 100

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void PrintError(char *info);


/********************************* main *************************************
 *
 *  Description: Program main function
 *
 *---------------------------------------------------------------------------
 *  Input......: argc,argv	argument counter, data ..
 *  Output.....: return	    success (0) or error (1)
 *  Globals....: -
 ****************************************************************************/
int main(int argc, char *argv[])
{
	MDIS_PATH path;
	int32	count=0;
	char	*device;

	if (argc < 2 || strcmp(argv[1],"-?")==0) {
		printf("Syntax: wdog_simp <device>                    \n");
		printf("Function: Example for Watchdog drivers        \n");
		printf("          Start watchdog and trigger watchdog \n");
		printf("          all %dmsec until keypress.          \n", WATCHDOG_TIME);
		printf("          !!! CAUTION:                                     !!!\n");
		printf("          !!! STOPPING THE TRIGGER (MAYBE ALSO TERMINATION !!!\n");
		printf("          !!! OF THE PROGRAM) WILL RESET YOUR SYSTEM       !!!\n");
		printf("Options:\n");
		printf("    device       device name\n");
		printf("\n");
		return(1);
	}

	device = argv[1];

	/*--------------------+
    |  open path          |
    +--------------------*/
	if ((path = M_open(device)) < 0) {
		PrintError("open");
		return(1);
	}

	/*--------------------+
    |  start watchdog     |
    +--------------------*/
	if ((M_setstat(path, WDOG_START, 0)) < 0) {
		PrintError("setstat WDOG_START");
		goto abort;
	}
	printf("Watchdog started - trigger all %dmsec\n", WATCHDOG_TIME);

	/*--------------------+
    |  trigger watchdog   |
    +--------------------*/
	do {
		UOS_Delay(WATCHDOG_TIME);
		count++;
		printf("  (%6d) Trigger watchdog - Press any key to stop the trigger\n", count);
		if ((M_setstat(path, WDOG_TRIG, 0)) < 0) {
			PrintError("setstat WDOG_TRIG");
			goto abort;
		}
	} while(UOS_KeyPressed() == -1);

	UOS_Delay(10000); /* force system reset */

	/*--------------------+
    |  cleanup            |
    +--------------------*/
	abort:
	if (M_close(path) < 0)
		PrintError("close");

	return(0);
}

/********************************* PrintError *******************************
 *
 *  Description: Print MDIS error message
 *
 *---------------------------------------------------------------------------
 *  Input......: info	info string
 *  Output.....: -
 *  Globals....: -
 ****************************************************************************/
static void PrintError(char *info)
{
	printf("*** can't %s: %s\n", info, M_errstring(UOS_ErrnoGet()));
}

/****************************************************************************
 ************                                                    ************
 ************                  XM01BC_CTRL                       ************
 ************                                                    ************
 ****************************************************************************/
/*!
 *         \file xm01bc_ctrl.c
 *       \author matthias.soehnlein@men.de
 *        $Date: 2009/09/04 11:57:40 $
 *    $Revision: 1.3 $
 *
 *        \brief Tool to control XM01BC PIC
 *
 *     Required: libraries: mdis_api, usr_oss, usr_utl
 *     \switches (none)
 */
 /*-------------------------------[ History ]--------------------------------
 *
 * $Log: xm01bc_ctrl.c,v $
 * Revision 1.3  2009/09/04 11:57:40  MRoth
 * R: Porting to MDIS5
 * M: added support for 64bit (MDIS_PATH)
 *
 * Revision 1.2  2009/02/06 15:46:20  MSoehnlein
 * R:1. Added MM1/ XM2/ F19 Support
 * M:1.a) added "Hardware ID" query
 *     b) added "Number of Supervised Voltages" query
 *     c) added "Number of Error Counters" query
 *     d) added "RESET_IN Blocking Mode" query
 *     e) increased number of error counters to 31
 *     f) increased number of supervised voltages to 8
 *     g) renamed Board Controller Interface (BCI) from XM01BCI to CPUBCI
 *
 * Revision 1.1  2008/10/31 17:13:38  MSoehnlein
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/usr_utl.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>
#include <MEN/xm01bc_drv.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define NONE			0
#define MAX_VOLT_CH		8

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
static MDIS_PATH G_Path;

static struct {
	u_int32	low;
	u_int32	high;
} G_VoltCh[MAX_VOLT_CH];	/* volt channel limits */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void usage(void);
static void PrintMdisError(char *info);
static void PrintUosError(char *info);

/********************************* usage ***********************************/
/**  Print program usage
 */
static void usage(void)
{
	printf("Usage: xm01bc_ctrl [<opts>] <device> [<opts>]            \n");
	printf("\n");
	printf("Function: Control XM01BC PIC                             \n");
	printf("\n");
	printf("Options:                                                 \n");
	printf("  device      device name                                \n");
	printf("  -v          show voltage values                        \n");
	printf("  -s          do voltage supervision (requires option -v)\n");
	printf("\n");
	printf("  -r=0xdead   perform SW warm reset (!! dangerous !!)    \n");
	printf("  -R=0xdead   perform SW cold reset (!! dangerous !!)    \n");
	printf("\n");
	printf("  -e          show error counters                        \n");
	printf("  -c          clear error counters                       \n");
	printf("  -n          get number of error counters               \n");
	printf("\n");
	printf("  -f          show firmware revision                     \n");
	printf("  -F          show firmware revision extended            \n");
	printf("\n");
	printf("  -w          show last reset reason                     \n");
	printf("  -x          show last error                            \n");
	printf("  -y          show power failure flag                    \n");
	printf("  -z          clear failure registers                    \n");
	printf("\n");
	printf("  -o          show operating hours counter               \n");
	printf("  -p          show power cycle counter                   \n");
	printf("\n");
	printf("  -l          get LED state                              \n");
	printf("  -L=<state>  set LED state                              \n");
	printf("\n");
	printf("  -u          get power resume mode                      \n");
	printf("  -a          get EXT_PWR_OK resume mode                 \n");
	printf("  -b          get RESET_IN mode                          \n");
	printf("\n");
	printf("  -h          get hardware variant ID                    \n");
	printf("\n");
	printf("  -q          exit QM-Mode (for production tests only)   \n");
	printf("\n");
	printf("(c) 2008 by MEN mikro elektronik GmbH                    \n");
	printf("\n");
	printf("\n");
}

/********************************* main ************************************/
/** Program main function
 *
 *  \param argc       \IN  argument counter
 *  \param argv       \IN  argument vector
 *
 *  \return	          success (0) or error (1)
 */
int main( int argc, char *argv[])
{
	char	*device,*str,*errstr,buf[40];
	int32	volt, superv, swr, scr, err, clear, frev, frev_ext;
	int32	lastReason, lastError, powerFail, clearFail, opHours, pwrCycle;
	int32	getLedState, setLedState, newLedState;
	int32	getResumeMode, getExtMode, getResetMode;
	int32   getHwVariant, getNumErrCntrs;
	int32   exitQmMode;
	int32	n, val, ch;
	int32	chNbr, in;
	int32	errCntrNbr;

	printf("\n");

	/*--------------------+
	|  check arguments    |
	+--------------------*/
	if( (errstr = UTL_ILLIOPT("vsr=R=ecnfFwxyzoplL=uabhq?", buf)) )	/* check args */
	{
		printf("*** %s\n", errstr);
		return(1);
	}

	if( UTL_TSTOPT("?") )						/* help requested ? */
	{
		usage();
		return(1);
	}

	/*--------------------+
	|  get arguments      |
	+--------------------*/
	for (device=NULL, n=1; n<argc; n++)
		if( *argv[n] != '-' )
		{
			device = argv[n];
			break;
		}

	if( !device || argc<3 )
	{
		usage();
		return(1);
	}

	volt = (UTL_TSTOPT("v") ? 1 : NONE);
	superv = (UTL_TSTOPT("s") ? 1 : NONE);
	if( superv && !volt )
	{
		printf("*** option -s requires option -v\n");
		return(1);
	}

	/* Software warm reset */
	if( (str = UTL_TSTOPT("r=")) )
		sscanf( str, "%x", &swr );
	else
		swr = NONE;

	/* Software cold reset */
	if( (str = UTL_TSTOPT("R=")) )
		sscanf( str, "%x", &scr );
	else
		scr = NONE;


	/* Show error counters */
	err = (UTL_TSTOPT("e") ? 1 : NONE);

	/* Clear error counters */
	clear = (UTL_TSTOPT("c") ? 1 : NONE);

	/* Show firmware revision */
	frev = (UTL_TSTOPT("f") ? 1 : NONE);

	/* Show firmware revision extended */
	frev_ext = (UTL_TSTOPT("F") ? 1 : NONE);


	/* Show last reset reason */
	lastReason = (UTL_TSTOPT("w") ? 1 : NONE);

	/* Show last error */
	lastError = (UTL_TSTOPT("x") ? 1 : NONE);

	/* Show power failure flags */
	powerFail = (UTL_TSTOPT("y") ? 1 : NONE);

	/* Clear failure registers */
	clearFail = (UTL_TSTOPT("z") ? 1 : NONE);


	/* Show operating hours counter */
	opHours = (UTL_TSTOPT("o") ? 1 : NONE);

	/* Show power cycle counter */
	pwrCycle = (UTL_TSTOPT("p") ? 1 : NONE);


	/* get LED state */
	getLedState = (UTL_TSTOPT("l") ? 1 : NONE);

	/* set LED state */
	if( (str = UTL_TSTOPT("L=")) )
	{
		sscanf( str, "%x", &newLedState );
		setLedState = 1;
	}
	else
		setLedState = NONE;


	/* get power resume mode */
	getResumeMode = (UTL_TSTOPT("u") ? 1 : NONE);


	/* get ATX_PWR_OK mode */
	getExtMode = (UTL_TSTOPT("a") ? 1 : NONE);


	/* get RESET_IN mode */
	getResetMode = (UTL_TSTOPT("b") ? 1 : NONE);

	/* exit QM-Mode */
	exitQmMode = (UTL_TSTOPT("q") ? 1 : NONE);


	/* get hardware variant */
	getHwVariant = (UTL_TSTOPT("h") ? 1 : NONE);

	/* get number of error counters */
	getNumErrCntrs = (UTL_TSTOPT("n") ? 1 : NONE);



	/*--------------------+
	|  open path          |
	+--------------------*/
	if( (G_Path = M_open(device)) < 0)
	{
		PrintMdisError("open");
		return(1);
	}

	/*----------------------+
	|  show voltage values  |
	+----------------------*/
	if( volt != NONE )
	{
		printf( "ch    in[mV]     lower limit[mV]     upper limit[mV]\n");


		if ((M_getstat(G_Path, M_LL_CH_NUMBER, &chNbr)) < 0)
		{
			PrintMdisError("getstat M_LL_CH_NUMBER");
			goto abort;
		}

		if( chNbr > MAX_VOLT_CH ){
			printf("xm01bc_ctrl supports only %d volt-ch (drv. reports %d)",
				MAX_VOLT_CH, chNbr);
			chNbr = MAX_VOLT_CH;
		}

		for( ch=0; ch<chNbr; ch++ )
		{
			/* select channel */
			if( (M_setstat(G_Path, M_MK_CH_CURRENT, ch)) < 0)
			{
				PrintMdisError("setstat M_MK_CH_CURRENT");
				goto abort;
			}

			/* get in voltage */
			if ((M_getstat(G_Path, XM01BC_VOLT_ACT, &in)) < 0)
			{
				PrintMdisError("getstat XM01BC_VOLT_ACT");
				goto abort;
			}

			/* get lower voltage limit */
			if ((M_getstat(G_Path, XM01BC_VOLT_LOW, &G_VoltCh[ch].low)) < 0)
			{
				PrintMdisError("getstat XM01BC_VOLT_LOW");
				goto abort;
			}

			/* get upper voltage limit */
			if ((M_getstat(G_Path, XM01BC_VOLT_HIGH, &G_VoltCh[ch].high)) < 0)
			{
				PrintMdisError("getstat XM01BC_VOLT_HIGH");
				goto abort;
			}

			printf(" %d     %5d               ", ch, in);

			if( G_VoltCh[ch].low )
				printf("%5d               ", G_VoltCh[ch].low);
			else
				printf(" -                ");

			if( G_VoltCh[ch].high )
				printf("%5d\n", G_VoltCh[ch].high);
			else
				printf("    -\n");
		}

		printf("\n");
	}


	/*----------------------+
	|  voltage supervision  |
	+----------------------*/
	if( superv != NONE )
	{
		u_int32 rcvSig;
		int32	lastSvState;

		printf("Voltage supervision:\n");
		printf("The driver polls the voltage supervision state and sends\n");
		printf("a signal to the app. if a voltage over-/underrun occures.\n");
		printf("Then, the app. gets the last polled voltage supervision state.\n");

		if ((M_getstat(G_Path, M_LL_CH_NUMBER, &chNbr)) < 0)
		{
			PrintMdisError("getstat M_LL_CH_NUMBER");
			goto abort;
		}

		if( chNbr > MAX_VOLT_CH ){
			printf("xm01bc_ctrl supports only %d volt-ch (drv. reports %d)",
				MAX_VOLT_CH, chNbr);
			chNbr = MAX_VOLT_CH;
		}

		/* init signal handling */
		if( UOS_SigInit(NULL) )
		{
			PrintUosError("SigInit");
			return(1);
		}

		/* install signal */
		if( UOS_SigInstall(UOS_SIG_USR1) )
		{
			PrintUosError("SigInstall");
			goto abort;
		}

		UOS_SigMask();

		/* install signal for voltage supervision */
		if( (M_setstat(G_Path, XM01BC_VOLT_SIGSET, (int32)UOS_SIG_USR1) < 0))
		{
			PrintMdisError("setstat XM01BC_VOLT_SIGSET");
			goto abort;
		}

		printf("--- press any key to abort ---\n");

		do {
			UOS_SigWait( 100, &rcvSig );

			/* our signal? */
			if( rcvSig == UOS_SIG_USR1 )
			{

				/* get last supervision error state */
				if ((M_getstat(G_Path, XM01BC_VOLT_SVSTATE, &lastSvState)) < 0)
				{
					PrintMdisError("getstat XM01BC_VOLT_SVSTATE");
					goto abort;
				}

				for( ch=0; ch<chNbr; ch++ )
				{

					if( !G_VoltCh[ch].low && !G_VoltCh[ch].high)
						continue;

					/* voltage out of range? */
					if( 0x01 & (lastSvState >> ch) )
					{

						printf(">>> volt-ch %d out of range (%dmV..%dmV)\n",
							ch,
							G_VoltCh[ch].low,
							G_VoltCh[ch].high);
					}
				}
			}
		} while( (UOS_KeyPressed() == -1) );

		printf("\n");
	}


	/*--------------------+
	|  sw reset           |
	+--------------------*/
	/* Software cold reset */
	if( scr != NONE )
	{
		if( (M_setstat(G_Path, XM01BC_SWCOLDRESET, scr)) < 0)
		{
			PrintMdisError("setstat XM01BC_SWCOLDRESET");
			goto abort;
		}
	}

	/* Software warm reset */
	if( swr != NONE )
	{
		if( (M_setstat(G_Path, XM01BC_SWRESET, swr)) < 0)
		{
			PrintMdisError("setstat XM01BC_SWRESET");
			goto abort;
		}
	}

	/*----------------------+
	|  show error counters  |
	+----------------------*/
	if( err != NONE )
	{
		printf("Error Counters:\n");

		if ((M_getstat(G_Path, XM01BC_NUM_ERRCNTRS, &errCntrNbr)) < 0)
		{
			if( UOS_ErrnoGet() == ERR_LL_UNK_CODE )
				errCntrNbr = XM01BC_NUM_ERR_CNTRS;
			else
			{
				PrintMdisError("getstat XM01BC_NUM_ERRCNTRS");
				goto abort;
			}
		}

		/* error counter 0 */
		if( errCntrNbr > 0 )
		{
			if( M_getstat(G_Path, XM01BC_ERRCNT_00, &val) < 0 )
			{
				PrintMdisError("getstat XM01BC_ERRCNT_00");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_00      : %u\n", (u_int8)val );
		}

		/* error counter 1 */
		if( errCntrNbr > 1 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_01, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_01");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_01      : %u\n", (u_int8)val );
		}

		/* error counter 2 */
		if( errCntrNbr > 2 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_02, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_02");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_02      : %u\n", (u_int8)val );
		}

		/* error counter 3 */
		if( errCntrNbr > 3 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_03, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_03");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_03      : %u\n", (u_int8)val );
		}

		/* error counter 4 */
		if( errCntrNbr > 4 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_04, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_04");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_04      : %u\n", (u_int8)val );
		}

		/* error counter 5 */
		if( errCntrNbr > 5 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_05, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_05");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_05      : %u\n", (u_int8)val );
		}

		/* error counter 6 */
		if( errCntrNbr > 6 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_06, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_06");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_06      : %u\n", (u_int8)val );
		}

		/* error counter 7 */
		if( errCntrNbr > 7 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_07, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_07");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_07      : %u\n", (u_int8)val );
		}

		/* error counter 8 */
		if( errCntrNbr > 8 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_08, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_08");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_08      : %u\n", (u_int8)val );
		}

		/* error counter 9 */
		if( errCntrNbr > 9 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_09, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_09");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_09      : %u\n", (u_int8)val );
		}

		/* error counter 10 */
		if( errCntrNbr > 10 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_10, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_10");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_10      : %u\n", (u_int8)val );
		}

		/* error counter 11 */
		if( errCntrNbr > 11 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_11, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_11");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_11      : %u\n", (u_int8)val );
		}

		/* error counter 12 */
		if( errCntrNbr > 12 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_12, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_12");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_12      : %u\n", (u_int8)val );
		}

		/* error counter 13 */
		if( errCntrNbr > 13 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_13, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_13");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_13      : %u\n", (u_int8)val );
		}

		/* error counter 14 */
		if( errCntrNbr > 14 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_14, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_14");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_14      : %u\n", (u_int8)val );
		}

		/* error counter 15 */
		if( errCntrNbr > 15 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_15, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_15");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_15      : %u\n", (u_int8)val );
		}

		/* error counter 16 */
		if( errCntrNbr > 16 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_16, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_16");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_16      : %u\n", (u_int8)val );
		}

		/* error counter 17 */
		if( errCntrNbr > 17 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_17, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_17");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_17      : %u\n", (u_int8)val );
		}

		/* error counter 18 */
		if( errCntrNbr > 18 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_18, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_18");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_18      : %u\n", (u_int8)val );
		}

		/* error counter 19 */
		if( errCntrNbr > 19 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_19, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_19");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_19      : %u\n", (u_int8)val );
		}

		/* error counter 20 */
		if( errCntrNbr > 20 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_20, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_20");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_20      : %u\n", (u_int8)val );
		}

		/* error counter 21 */
		if( errCntrNbr > 21 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_21, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_21");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_21      : %u\n", (u_int8)val );
		}

		/* error counter 22 */
		if( errCntrNbr > 22 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_22, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_22");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_22      : %u\n", (u_int8)val );
		}

		/* error counter 23 */
		if( errCntrNbr > 23 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_23, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_23");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_23      : %u\n", (u_int8)val );
		}

		/* error counter 24 */
		if( errCntrNbr > 24 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_24, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_24");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_24      : %u\n", (u_int8)val );
		}

		/* error counter 25 */
		if( errCntrNbr > 25 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_25, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_25");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_25      : %u\n", (u_int8)val );
		}

		/* error counter 26 */
		if( errCntrNbr > 26 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_26, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_26");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_26      : %u\n", (u_int8)val );
		}

		/* error counter 27 */
		if( errCntrNbr > 27 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_27, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_27");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_27      : %u\n", (u_int8)val );
		}

		/* error counter 28 */
		if( errCntrNbr > 28 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_28, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_28");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_28      : %u\n", (u_int8)val );
		}

		/* error counter 29 */
		if( errCntrNbr > 29 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_29, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_29");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_29              : %u\n", (u_int8)val );
		}

		/* error counter 30 */
		if( errCntrNbr > 30 )
		{
			if ((M_getstat(G_Path, XM01BC_ERRCNT_30, &val)) < 0)
			{
				PrintMdisError("getstat XM01BC_ERRCNT_30");
				goto abort;
			}
			printf("- XM01BC_ERRCNT_30      : %u\n", (u_int8)val );
		}
		printf("\n");
	}

	/*--------------------+
	|  clr error counters |
	+--------------------*/
	if( clear != NONE )
	{
		printf("Clearing error counters...\n");
		if( (M_setstat(G_Path, XM01BC_ERRCNT_CLR, 0)) < 0)
		{
			PrintMdisError("setstat XM01BC_ERRCNT_CLR");
			goto abort;
		}
		printf("done.\n");
		printf("\n");
	}

	/*----------------------------------+
	|  get number of error counters     |
	+----------------------------------*/
	if( getNumErrCntrs != NONE)
	{
		if ((M_getstat(G_Path, XM01BC_NUM_ERRCNTRS, &val)) < 0)
		{
			if( UOS_ErrnoGet() == ERR_LL_UNK_CODE )
				printf("Number of Error Counters: Function not supported by driver\n\n");
			else
			{
				PrintMdisError("getstat XM01BC_NUM_ERRCNTRS");
				goto abort;
			}
		}
		else
		{
			printf("Number of Error Counters: %u\n", (u_int8)val);
			printf("\n");
		}
	}

	/*-------------------------------+
	|  show firmware revision        |
	+-------------------------------*/
	if( frev != NONE )
	{
		M_SG_BLOCK	blk;
		char		rev[6];

		blk.size = sizeof(rev);
		blk.data = (void*)&rev;

		if ((M_getstat(G_Path, XM01BC_BLK_FWREV, (int32*)&blk)) < 0)
		{
			PrintMdisError("getstat XM01BC_BLK_FWREV");
			goto abort;
		}
		printf("Firmware rev.           : %c%c.%c%c.%c%c\n",
			rev[0],rev[1],rev[2],rev[3],rev[4],rev[5]);
		printf("\n");
	}

	/*----------------------------------+
	|  show firmware revision extended  |
	+----------------------------------*/
	if( frev_ext != NONE )
	{
		M_SG_BLOCK	blk;
		u_int16		rev[3];

		blk.size = sizeof(rev);
		blk.data = (void*)&rev;

		if ((M_getstat(G_Path, XM01BC_BLK_FWREV_EXT, (int32*)&blk)) < 0)
		{
			PrintMdisError("getstat XM01BC_BLK_FWREV_EXT");
			goto abort;
		}
		printf("Firmware rev. (extended): %02u.%02u.%02u\n", rev[0],rev[1],rev[2]);
		printf("\n");
	}

	/*----------------------------------+
	|  show last reset reason           |
	+----------------------------------*/
	if( lastReason != NONE )
	{
		if ((M_getstat(G_Path, XM01BC_ERR_RST_RSN, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_ERR_RST_RSN");
			goto abort;
		}
		printf("Last reset reason       : 0x%02x\n", (u_int8)val);
		printf("\n");
	}

	/*----------------------------------+
	|  show last error                  |
	+----------------------------------*/
	if( lastError != NONE )
	{
		if ((M_getstat(G_Path, XM01BC_ERR_LAST, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_ERR_LAST");
			goto abort;
		}
		printf("Last error              : 0x%02x\n", (u_int8)val);
		printf("\n");
	}

	/*----------------------------------+
	|  show power failure flags         |
	+----------------------------------*/
	if( powerFail != NONE )
	{
		if ((M_getstat(G_Path, XM01BC_ERR_PWR_FLGS, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_ERR_PWR_FLGS");
			goto abort;
		}
		printf("Power failure flags     : 0x%02x\n", (u_int8)val);
		printf("\n");
	}

	/*----------------------------------+
	|  clear power failure flags        |
	+----------------------------------*/
	if( clearFail != NONE )
	{
		printf("Clearing failure registers...\n");
		if( (M_setstat(G_Path, XM01BC_ERR_REG_CLR, 0)) < 0)
		{
			PrintMdisError("setstat XM01BC_ERR_REG_CLR");
			goto abort;
		}
		printf("done.\n");
		printf("\n");
	}

	/*----------------------------------+
	|  show operating hours counter     |
	+----------------------------------*/
	if( opHours != NONE )
	{
		if ((M_getstat(G_Path, XM01BC_OPHRS_CNT, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_OPHRS_CNT");
			goto abort;
		}
		printf("Operating hours         : %uh\n", (u_int16)val);
		printf("\n");
	}

	/*----------------------------------+
	|  show power cycle counter         |
	+----------------------------------*/
	if( pwrCycle != NONE )
	{
		if ((M_getstat(G_Path, XM01BC_PWRCYCL_CNT, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_PWRCYCL_CNT");
			goto abort;
		}
		printf("Power cycles            : %u\n", (u_int16)val);
		printf("\n");
	}

	/*----------------------------------+
	|  set LED state                    |
	+----------------------------------*/
	if( setLedState != NONE)
	{
		if( (M_setstat(G_Path, XM01BC_LED_CTRL, newLedState)) < 0)
		{
			PrintMdisError("setstat XM01BC_LED_CTRL");
			goto abort;
		}
		/* show new led state */
		getLedState = 1;
	}

	/*----------------------------------+
	|  get LED state                    |
	+----------------------------------*/
	if( getLedState != NONE)
	{
		if ((M_getstat(G_Path, XM01BC_LED_CTRL, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_LED_CTRL");
			goto abort;
		}
		printf("LED state               : %s\n", (val ? "On" : "Off") );
		printf("\n");
	}

	/*----------------------------------+
	|  get power resume mode            |
	+----------------------------------*/
	if( getResumeMode != NONE)
	{
		if ((M_getstat(G_Path, XM01BC_RESUME_MODE, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_RESUME_MODE");
			goto abort;
		}

		printf("Power Resume mode       : ");

		switch(val)
		{
			case XM01BC_RESUME_OFF:
				printf("Off\n");
				break;

			case XM01BC_RESUME_ON:
				printf("On\n");
				break;

			case XM01BC_RESUME_FORMER:
				printf("Former State\n");
				break;

			default:
				printf("invalid (0x%02x)\n", (u_int8)val);
				break;

		}
		printf("\n");
	}

	/*----------------------------------+
	|  get RESET_IN Blocking mode       |
	+----------------------------------*/
	if( getResetMode != NONE)
	{
		if ((M_getstat(G_Path, XM01BC_RST_IN_MODE, &val)) < 0)
		{
			if( UOS_ErrnoGet() == ERR_LL_UNK_CODE )
				printf("RESET_IN Blocking mode  : Function not supported by driver\n\n");
			else
				PrintMdisError("getstat XM01BC_RES_IN_MODE");
			goto abort;
		}
		else
		{
			printf("RESET_IN Blocking mode  : ");

			switch(val)
			{
				case XM01BC_RST_IN_ENABLED:
					printf("not blocked\n");
					break;

				case XM01BC_RST_IN_BLOCKED:
					printf("blocked\n");
					break;

				default:
					printf("invalid (0x%02x)\n", (u_int8)val);
					break;

			}
			printf("\n");
		}

	}

	/*----------------------------------+
	|  get EXT_PWR_OK mode              |
	+----------------------------------*/
	if( getExtMode != NONE)
	{
		if ((M_getstat(G_Path, XM01BC_EXT_PWR_OK_MODE, &val)) < 0)
		{
			PrintMdisError("getstat XM01BC_EXT_PWR_OK_MODE");
			goto abort;
		}

		printf("EXT_PWR_OK mode         : ");

		switch(val)
		{
			case XM01BC_EXT_PWR_OK_IGN:
				printf("Check on start only\n");
				break;

			case XM01BC_EXT_PWR_OK_ERR:
				printf("Check always\n");
				break;

			default:
				printf("invalid (0x%02x)\n", (u_int8)val);
				break;

		}
		printf("\n");
	}

	/*----------------------------------+
	|  get hardware variant             |
	+----------------------------------*/
	if( getHwVariant != NONE)
	{
		if ((M_getstat(G_Path, XM01BC_HW_VARIANT, &val)) < 0)
		{
			if( UOS_ErrnoGet() == ERR_LL_UNK_CODE )
				printf("Detected Hardware ID    : Function not supported by driver\n\n");
			else
			{
				PrintMdisError("getstat XM01BC_HW_VARIANT");
				goto abort;
			}
		}
		else
		{
			printf("Detected Hardware ID    : 0x%04x\n", (u_int16)val);
			printf("\n");
		}
	}

	/*----------------------------------+
	|  exit QM mode                     |
	+----------------------------------*/
	if( exitQmMode != NONE)
	{
		printf("leaving QM-Mode (for production tests only)...\n");
		if( (M_setstat(G_Path, XM01BC_WDOG_ARM, 0)) < 0)
		{
			PrintMdisError("setstat XM01BC_WDOG_ARM");
			goto abort;
		}
		printf("done.\n");
		printf("\n");
	}

	/*--------------------+
	|  cleanup            |
	+--------------------*/
	abort:

	if( superv != NONE ){
		UOS_SigExit();
	}

	if( M_close(G_Path) < 0 )
		PrintMdisError("close");

	return(0);
}

/********************************* PrintMdisError **************************/
/** Print MDIS error message
 *
 *  \param info       \IN  info string
*/
static void PrintMdisError(char *info)
{
	printf("*** can't %s: %s\n", info, M_errstring(UOS_ErrnoGet()));
	printf("\n");
}

/********************************* PrintUosError ***************************/
/** Print User OSS error message
 *
 *  \param info       \IN  info string
*/
static void PrintUosError(char *info)
{
	printf("*** can't %s: %s\n", info, UOS_ErrString(UOS_ErrnoGet()));
	printf("\n");
}


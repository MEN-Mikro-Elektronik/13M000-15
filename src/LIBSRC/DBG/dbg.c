/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  dbg.c
 *      Project: DBG library
 *
 *      \author  kp
 *        $Date: 2008/11/06 20:15:13 $
 *    $Revision: 1.3 $
 *
 *	   \project  MDIS4QNX
 *  	 \brief  Debug functions for QNX
 *				 Main task is done by the macros in dbg_os.h. This
 *				 file exports only the DBG_MemDump routine and Init/Exit
 *
 *    \switches  
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: dbg.c,v $
 * Revision 1.3  2008/11/06 20:15:13  channoyer
 * R: Missing doxygen documentation
 * M: Add doxygen documentation
 *
 * Revision 1.2  2004/03/19 11:38:00  ub
 * Cosmetics
 *
 * Revision 1.1  2003/08/04 16:18:35  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

const char DBG_RCSid[]="$Id: dbg.c,v 1.3 2008/11/06 20:15:13 channoyer Exp $";

/*! \mainpage 


*/
/*! \page dbg_dummy
 \menimages
*/

#include <MEN/men_typs.h>

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef void DBG_HANDLE;

#include <stdio.h>
/* dummy definition for dbg.h */
#define DBG_MYLEVEL     null
/* say: don't declare DBG_HANDLE */
#define _NO_DBG_HANDLE
/* DBG_HANDLE needs to be declared BEFORE including dbg.h */
#include <MEN/dbg.h>



/****************************** DBG_Init ************************************
 *
 *  Description:  Initializes the debug output, allocates and returns debug
 *                handle.
 *				  Does nothing on QNX.
 *---------------------------------------------------------------------------
 *  Input......:  name    name of debug output data module or NULL
 *				  dbgP    pointer to variable where debug handle will stored
 *
 *  Output.....:  *dbgP   debug handle
 *                return  success (0) or error code
 *  Globals....:  -
 ****************************************************************************/
int32 DBG_Init( char *name, DBG_HANDLE **dbgP )
{
	*dbgP = (DBG_HANDLE *)1;
    return(0);
}/*DBG_Init*/

/****************************** DBG_Exit ************************************
 *
 *  Description:  Terminate the debug output
 *
 *---------------------------------------------------------------------------
 *  Input......:  dbgP    pointer to variable where debug handle is stored
 *  Output.....:  return  0 
 *  Globals....:  -
 ****************************************************************************/
int32  DBG_Exit( DBG_HANDLE **dbgP )
{
    *dbgP = NULL;
    return( 0 );
}/*DBG_Exit*/


/*************************** DBG_Memdump ************************************
 *
 *  Description:  Print a formatted hex dump
 *
 *                Trailing blanks in the description string define the
 *                indention of all following debug lines.
 *
 *---------------------------------------------------------------------------
 *  Input......:  txt     description string or NULL (first line)
 *                buf     memory to dump
 *                len     nr of bytes
 *                fmt     dump format [bytes]
 *                        1=byte, 2=word, 4=long, 8=64bit 
 *  Output.....:  return  0
 *  Globals....:  -
 ****************************************************************************/
int32  DBG_Memdump
(
   DBG_HANDLE *dbg,
   char    *txt,
   void    *buf,
   u_int32 len,
   u_int32 fmt
)
{
    long i,line=1, blanks=0, n;
    char *k=txt, *k0, *kmax=(char*)buf+len;
	char lbuf[256];

    if (txt) {
        while(*k++ == ' ')          /* count trailing blanks */
            blanks++;

        DBG_Write(dbg, "%s (%ld bytes)\n",txt,len);
    }

    for (k=k0=buf; k0<kmax; k0+=16,line++) {
        for (n=0; n<blanks; n++)    /* write trailing blanks */
            sprintf(lbuf," ");
   
        sprintf(lbuf,"%08lx+%04x: ",(long)buf, (short)(k-(char*)buf));

        switch(fmt) {                                      /* dump hex: */
           case 8 : for (k=k0,i=0; i<16; i+=8, k+=8)       /* 64-bit aligned */
                       if (k<kmax)  sprintf(lbuf,"%08lx%08lx ",
                                              *(long*)k,*(long*)k+4);
                       else         sprintf(lbuf,"                 ");
                    break;
           case 4 : for (k=k0,i=0; i<16; i+=4, k+=4)       /* long aligned */
                       if (k<kmax)  sprintf(lbuf,"%08lx ",*(long*)k);
                       else         sprintf(lbuf,"         ");
                    break;
           case 2 : for (k=k0,i=0; i<16; i+=2, k+=2)       /* word aligned */
                       if (k<kmax)  sprintf(lbuf,"%04x ",*(short*)k & 0xffff);
                       else         sprintf(lbuf,"     ");
                    break;
           default: for (k=k0,i=0; i<16; i++, k++)         /* byte aligned */
                       if (k<kmax)  sprintf(lbuf,"%02x ",*k & 0xff);
                       else         sprintf(lbuf,"   ");
        }

        for (k=k0,i=0; i<16 && k<kmax; i++, k++)           /* dump ascii's: */
            if ( ((u_int8)*k)>=32 && ((u_int8)*k)<=127 )
               sprintf(lbuf,"%c", *k);
            else
               sprintf(lbuf,".");

		DBG_Write( dbg, "%s\n", lbuf );
    }
    return(0);
}/*DBG_Memdump*/

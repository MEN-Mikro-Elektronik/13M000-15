/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  m_errstr.c
 *
 *      \author  ub
 *        $Date: 2004/03/24 10:38:54 $
 *    $Revision: 1.1 $
 * 
 *  	 \brief  MDIS error string functions for MDIS_API
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: m_errstr.c,v $
 * Revision 1.1  2004/03/24 10:38:54  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <MEN/men_typs.h>   /* MEN type definitions      */
#include <MEN/mdis_api.h>   /* MDIS api                  */
#include <MEN/mdis_err.h>   /* MDIS error codes          */
#include <MEN/mdis_ers.h>   /* MDIS error string table   */
#include <MEN/mdis_mk.h>

/**********************************************************************/
/** Convert MDIS error code to static string
 * 
 * \copydoc mdis_api_specification.c::M_errstring()
 * \sa M_errstringTs
 */
char* M_errstring(int32 errCode)
{
    static char errMsg[128];

	return M_errstringTs( errCode, errMsg );
}
/**********************************************************************/
/** Convert MDIS error code to string
 * 
 * \copydoc mdis_api_specification.c::M_errstringTs()
 * \sa M_errstring
 */
char* M_errstringTs(int32 errCode, char *strBuf)
{
    char        *errString = NULL;
    u_int32     n;

    /*----------------------+
    |  MDIS error code      |
    +----------------------*/
	if (errCode==ERR_SUCCESS || IN_RANGE(errCode,ERR_OS,ERR_END)) {
		/* search in MDIS msg table */
		for (n=0; n < MDIS_NBR_OF_ERR; n++) {
			if (errCode == errStrTable[n].errCode) {   /* found ? */
				errString = errStrTable[n].errString;
				break;
			}
		}

		if (errString)
			sprintf(strBuf,"ERROR (MDIS) 0x%04lx:  %s",errCode,errString);
		else 
			sprintf(strBuf,"ERROR (MDIS) 0x%04lx:  unknown MDIS error",errCode);
    }
    /*----------------------+
    |  else: system error   |
    +----------------------*/
	else {
		sprintf(strBuf,"ERROR (QNX) #%ld:  %s",
				errCode, strerror( errCode ) );
	}

    return(strBuf);
}




/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  mdis_api.c
 *
 *      \author  ub
 *        $Date: 2014/10/03 15:35:17 $
 *    $Revision: 1.7 $
 *
 *  	 \brief  MDIS user space programming interface for QNX
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: mdis_api.c,v $
 * Revision 1.7  2014/10/03 15:35:17  channoyer
 * R: Not possible to use MDIS over Qnet
 * M:1. Detect if mdis device is a Qnet path
 *   2. Add node ID
 *
 * Revision 1.6  2009/06/05 16:00:09  channoyer
 * R: struct MDIS_PATH_QNX now define in mdis_mk.h
 * M: remove MDIS_PATH_QNX
 *
 * Revision 1.5  2008/11/12 16:31:29  channoyer
 * Cosmetics
 *
 * Revision 1.4  2008/09/12 16:12:43  gvarlet
 * R: Conflict with MDIS_PATH type used in men_typs.h
 * M: Change MDIS_PATH to MDIS_PATH_QNX
 *
 * Revision 1.3  2008/03/13 10:46:10  channoyer
 * + Add previous change to DoOpen() too.
 *
 * Revision 1.2  2008/03/13 09:54:04  channoyer
 * Changed prototypes of M_open and M_setblock (added const attributes) accordingly to Rev1.11 of mdis_api.h
 *
 * Revision 1.1  2004/03/24 10:38:55  ub
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 1997-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

static const char RCSid[]="$Header: /dd2/CVSR/QNX/src/LIBSRC/MDIS_API/mdis_api.c,v 1.7 2014/10/03 15:35:17 channoyer Exp $";

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

#include <unistd.h>
#include <devctl.h>
#include <fcntl.h>
#include <pthread.h>

#include <MEN/men_typs.h>   /* MEN type definitions      */
#include <MEN/mdis_api.h>   /* MDIS api                  */
#include <MEN/mdis_err.h>   /* MDIS error codes          */
#include <MEN/mdis_mk.h>    /* MDIS kernel               */

/*! \mainpage

 This is the documentation of the MDIS API for user space.

 Refer to the \ref mdisapicommonspec "MDIS_API Common Specification" for the
 plain common specification.
*/


/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/


/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int32 DoOpen(
	const char *device,
	int ioctlCode );


/**********************************************************************/
/** Open path to device
 *
 * \copydoc mdis_api_specification.c::M_open()
 *
 * \sa M_close
 */
int32  M_open(const char *device)
{
	return( DoOpen( device, MDIS_DEVCTL_M_OPEN ) );
}

/**********************************************************************/
/** Close path to device
 *
 * \copydoc mdis_api_specification.c::M_close()
 * \sa M_open
 */
int32  M_close(int32 path)
{
    MDIS_CLOSE_DEVICE_DATA mc;
    MDIS_PATH_QNX *mp = (MDIS_PATH_QNX*)path;
	int32 rv = -1;
    int rc;

    if( mp == NULL ) {
        errno = EFAULT;
        return( -1 );
    }

    /*--- remove connection to MDIS kernel thread ---*/
    if( ConnectDetach( mp->coid ) ) {
        return( -1 );
    }

    /*--- tell MDIS kernel to close device ---*/
    mc.path = mp->path;
    mc.pid = getpid();
    mc.tid = pthread_self();
    mc.nid = mp->nid;

    if(( rv = devctl( mp->fd, MDIS_DEVCTL_M_CLOSE, &mc, sizeof(mc), &rc))) {
        errno = rv;
        return( -1 );
    }

    /* check return code from MDIS_CloseDevice() */
    if( rc ) {
        errno = rc;
        return( -1 );
    }

    /*--- cleanup ---*/
    close( mp->fd );
    free( mp );

	return( 0 );
}

/**********************************************************************/
/** Read 32-bit integer value from device
 *
 * \copydoc mdis_api_specification.c::M_read()
 * \sa M_getblock, M_write
 */
int32 M_read(int32 path, int32 *valueP)
{
    MDIS_PATH_QNX        *mp = (MDIS_PATH_QNX*)path;
    int32            rbuf[1];
    MDIS_MSGHEADER   h;
    int              rv;
    iov_t            siov;

    MDIS_SETMSG( &h, mp->path, MDIS_MSGID_READ, 0, 0, 0 );
    SETIOV( &siov, &h, sizeof(h) );

    if( (rv = MsgSendvs( mp->coid, &siov, 1, rbuf, sizeof(rbuf))) > ERR_OS )
        errno = rv;

    if( rv )
        return( -1 );

    *valueP = rbuf[0];

	return( 0 );
}

/**********************************************************************/
/** Write 32-bit integer value to device
 *
 * \copydoc mdis_api_specification.c::M_write()
 * \sa M_setblock, M_read
 */
int32 M_write(int32 path, int32 value)
{
    MDIS_PATH_QNX *mp = (MDIS_PATH_QNX*)path;
    int32            rbuf[1];
    MDIS_MSGHEADER   h;
    int              rv;
    iov_t            siov;

    MDIS_SETMSG( &h, mp->path, MDIS_MSGID_WRITE, 0, value, 0 );
    SETIOV( &siov, &h, sizeof(h) );

    if( (rv = MsgSendvs( mp->coid, &siov, 1, rbuf, sizeof(rbuf))) > ERR_OS )
        errno = rv;

    if( rv )
        return( -1 );

	return( 0 );
}

/**********************************************************************/
/** Get status from device
 *
 * \copydoc mdis_api_specification.c::M_getstat()
 * \sa M_setstat
 */
int32 M_getstat(int32 path, int32 code, int32 *dataP)
{
    MDIS_PATH_QNX        *mp = (MDIS_PATH_QNX*)path;
    MDIS_MSGHEADER   h;
    int32            rbuf[1];
	int32            rv=0;

    if( code & M_OFFS_STD ) {
        /*--- standard code ---*/
        iov_t            siov;

        MDIS_SETMSG( &h, mp->path, MDIS_MSGID_GETSTAT, code, 0, 0 );
        SETIOV( &siov, &h, sizeof(h) );

        if( (rv = MsgSendvs( mp->coid, &siov, 1, rbuf, sizeof(rbuf) )) > 
            ERR_OS )
            errno = rv;

        *dataP = rbuf[0];
    }
    else if( code & M_OFFS_BLK) {
        /*--- block code ---*/
        iov_t      siov[2];
        M_SG_BLOCK *sg = (M_SG_BLOCK*)dataP;

        /*--- prepare message header ---*/
        MDIS_SETMSG( &h, mp->path, MDIS_MSGID_GETSTAT, code, 0, sg->size );
        SETIOV( &siov[0], &h, sizeof(h) );
        SETIOV( &siov[1], sg->data, sg->size );

        /*--- send message; receive result in buffer ---*/
        if( (rv = MsgSendvs( mp->coid, siov, 2, sg->data, sg->size )) > ERR_OS)
            errno = rv;
    }
    else {
        rv = -1;
        errno = ERR_MK_UNK_CODE;
    }

    if( rv )
        return( -1 );

	return( 0 );
}


/**********************************************************************/
/** Set status of device
 *
 * \copydoc mdis_api_specification.c::M_setstat()
 * \sa M_getstat
 */
int32 M_setstat(int32 path, int32 code, int32 data)
{
    MDIS_PATH_QNX        *mp = (MDIS_PATH_QNX*)path;
    MDIS_MSGHEADER   h;
	int32            rv=0;

    if( code & M_OFFS_STD ) {
        /*--- standard code ---*/
        iov_t siov;

        MDIS_SETMSG( &h, mp->path, MDIS_MSGID_SETSTAT, code, data, 0 );
        SETIOV( &siov, &h, sizeof(h) );
        if( (rv = MsgSendvs( mp->coid, &siov, 1, NULL, 0 )) > ERR_OS )
            errno = rv;

    }
    else if( code & M_OFFS_BLK) {
        /*--- block code ---*/
        iov_t      siov[2];
        M_SG_BLOCK *sg = (M_SG_BLOCK*)data;

        MDIS_SETMSG( &h, mp->path, MDIS_MSGID_SETSTAT, code, 0, sg->size );
        SETIOV( &siov[0], &h, sizeof(h) );
        SETIOV( &siov[1], sg->data, sg->size );

        if( (rv = MsgSendvs( mp->coid, siov, 2, NULL, 0 )) >= ERR_OS )
            errno = rv;
    }
    else {
        rv = -1;
        errno = ERR_MK_UNK_CODE;
    }

    if( rv )
        return( -1 );

	return( 0 );
}

/**********************************************************************/
/** Read data block from device
 *
 * \copydoc mdis_api_specification.c::M_getblock()
 * \sa M_read, M_setblock
 */
int32 M_getblock(int32 path, u_int8 *buffer, int32 length)
{
    MDIS_PATH_QNX        *mp = (MDIS_PATH_QNX*)path;
    MDIS_MSGHEADER   h;
    iov_t            siov;
    int              rv;

    /*--- prepare message header ---*/
    MDIS_SETMSG( &h, mp->path, MDIS_MSGID_GETBLOCK, 0, 0, length );
    SETIOV( &siov, &h, sizeof(h) );

    /*--- send message; receive result in buffer ---*/
    if( (rv = MsgSendvs( mp->coid, &siov, 1, buffer, length )) < -ERR_OS )
        errno = -rv;

    if( rv < 0 )
        return( -1 );

	return( rv );
}

/**********************************************************************/
/** Write data block to device
 *
 * \copydoc mdis_api_specification.c::M_setblock()
 * \sa M_write, M_getblock
 */
int32 M_setblock(int32 path, const u_int8 *buffer, int32 length)
{
    MDIS_PATH_QNX        *mp = (MDIS_PATH_QNX*)path;
    MDIS_MSGHEADER   h;
    iov_t            siov[2];
    int              rv;

    /*--- prepare message header ---*/
    MDIS_SETMSG( &h, mp->path, MDIS_MSGID_SETBLOCK, 0, 0, length );
    SETIOV( &siov[0], &h, sizeof(h) );
    SETIOV( &siov[1], buffer, length );

    /*--- send message ---*/
    if( (rv = MsgSendvs( mp->coid, siov, 2, NULL, 0 )) < -ERR_OS )
        errno = rv;

    if( rv < 0 )
        return( -1 );

	return( rv );
}


static int32 DoOpen(
	const char *devname,
	int ioctlCode )
{
	int fd, rv=-1, rc, coid, nd = 0;
	MDIS_OPEN_DEVICE_DATA moData;
    MDIS_OPEN_DEVICE_RETURN *r = (MDIS_OPEN_DEVICE_RETURN*)&moData;
    MDIS_PATH_QNX *mp;
    char *endstr = NULL;
    char mdisPath[50] = MDIS_DEV_NAME;


    nd = netmgr_strtond( devname, &endstr);
    if (-1 == nd) { /* Not a qnet path? Give a try with local path. */
        nd = 0;
    } else {
        if( -1 == netmgr_ndtostr(ND2S_DIR_SHOW | ND2S_DOMAIN_HIDE | ND2S_NAME_SHOW, nd, mdisPath, sizeof(mdisPath))) {
            return( -1 );
        }
        strncat( mdisPath, MDIS_DEV_NAME, sizeof(mdisPath) );
    }
    if( *endstr == '/' ) endstr++; /* skip leading / */
    strncpy( moData.devName, endstr, sizeof(moData.devName) );

    moData.pid = getpid();
    moData.tid = pthread_self();
    moData.nid = netmgr_remote_nd(nd, ND_LOCAL_NODE);

	/* open global MDIS device */
	if( (fd = open(mdisPath, O_RDWR)) < 0 ) {
		return( -1 );
    }

    /* ask driver to open MDIS device */
    if(( rv = devctl( fd, MDIS_DEVCTL_M_OPEN, &moData, sizeof(moData), &rc ))){
        errno = rv;
        return( -1 );
    }

    /* check return code from MDIS_OpenDevice() */
    if( rc ) {
        errno = rc;
        return( -1 );
    }

    /* create connection to MDIS kernel thread */
    if( (coid = ConnectAttach( nd, r->mkPid, r->mkCid,
                                 _NTO_SIDE_CHANNEL, 0 )) == -1) {
        return( -1 );
    }

    if( (mp = malloc( sizeof( *mp ))) == NULL ) {
        errno = ENOMEM;
        return( -1 );
    }

    mp->path = r->path;
    mp->coid = coid;
    mp->fd   = fd;
    mp->nid  = moData.nid;

    return( (int32)mp );
}

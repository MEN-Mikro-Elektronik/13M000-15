/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  smb.c
 *      Project: men-libc for QNX6
 *
 *      \author  kp
 *        $Date: 2002/04/29 08:52:24 $
 *    $Revision: 1.1 $
 *
 *       \brief  SMB related functions
 *                      
 *                      
 *     Required: -
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: smb.c,v $
 * Revision 1.1  2002/04/29 08:52:24  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: smb.c,v 1.1 2002/04/29 08:52:24 kp Exp $";

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/iomsg.h>
#include <sys/dispatch.h>
#include <MEN/men_libc.h>
#include <MEN/smb_serv.h>

/*! \defgroup smbfunc SMB functions
 */
/**@{*/


/***********************************************************************/
/** Attach to the SMB server \c "/dev/smb".
 *  
 *  The returned value must be passed to all further MenC_SmbXXX functions
 *  \return fd or -1 on error (errno set)
 */
int MenC_SmbAttach( void )
{
	return open( SMB_SERV_NAME, O_RDWR );
}

static int Action( int fd, struct _smb_message *msg, int subtype )
{
	int ret;

	msg->msg_hdr.i.type 		= _IO_MSG;
	msg->msg_hdr.i.combine_len 	= sizeof( struct _smb_message );
	msg->msg_hdr.i.mgrid 		= _IOMGR_SMB;
	msg->msg_hdr.i.subtype 		= subtype;

	/* send message to server and wait for reply */
    ret = MsgSend_r( fd, msg, sizeof( struct _smb_message ), msg, 
					 sizeof( struct _smb_message ) );

	if( ret < 0 ){
		/* an error from MsgSend */
		ret = SMB_ERR_SERVER;
	}
	/* otherwise ret contains the status return value from server */
	return ret;
}

/***********************************************************************/
/** Write single byte to SMB device.
 *
 *	\param fd			handle (value returned by MenC_SmbAttach())
 *	\param smbBusNo		smb bus number on CPU (0..n)
 *	\param smbAddr		SMB device address
 *	\param data			data to write to device
 *
 *	\return	\ref smbcodes
 */
int MenC_SmbWrite1( int fd, 
					uint8_t smbBusNo, 
					uint8_t smbAddr, 
					uint8_t data )
{
	struct _smb_message msg;

	msg.smbBusNo 	= smbBusNo;
	msg.smbAddr		= smbAddr;
	msg.outData1	= data;

	return Action( fd, &msg, IOM_SMB_WRITE1 );
}

/***********************************************************************/
/** Write two bytes to SMB device.
 *
 *	\param fd			handle (value returned by MenC_SmbAttach())
 *	\param smbBusNo		smb bus number on CPU (0..n)
 *	\param smbAddr		SMB device address
 *	\param data1		first data byte to write to device
 *	\param data2		second data byte to write to device
 *
 *	\return	\ref smbcodes
 */
int MenC_SmbWrite2( int fd, 
					uint8_t smbBusNo, 
					uint8_t smbAddr, 
					uint8_t data1,
					uint8_t data2)
{
	struct _smb_message msg;

	msg.smbBusNo 	= smbBusNo;
	msg.smbAddr		= smbAddr;
	msg.outData1	= data1;
	msg.outData2	= data2;

	return Action( fd, &msg, IOM_SMB_WRITE2 );
}

/***********************************************************************/
/** Read single byte from SMB device.
 *
 *	\param fd			handle (value returned by MenC_SmbAttach())
 *	\param smbBusNo		smb bus number on CPU (0..n)
 *	\param smbAddr		SMB device address
 *	\param dataP		Output: contains data byte read from device
 *
 *	\return	\ref smbcodes
 */
int MenC_SmbRead( int fd, 
				  uint8_t smbBusNo, 
				  uint8_t smbAddr, 
				  uint8_t *dataP)
{
	struct _smb_message msg;
	int ret;

	msg.smbBusNo 	= smbBusNo;
	msg.smbAddr		= smbAddr;

	ret = Action( fd, &msg, IOM_SMB_READ );
	
	if( ret == SMB_ERR_NO ){
		*dataP = msg.inData;
	}
	return ret;
}
	
/***********************************************************************/
/** Write and read byte from SMB device.
 *
 *	\param fd			handle (value returned by MenC_SmbAttach())
 *	\param smbBusNo		smb bus number on CPU (0..n)
 *	\param smbAddr		SMB device address
 *	\param outData		data byte to send to device
 *	\param inDataP		Output: contains data byte read from device
 *
 *	\return	\ref smbcodes
 */
int MenC_SmbWriteRead( int fd, 
					   uint8_t smbBusNo, 
					   uint8_t smbAddr, 
					   uint8_t outData,
					   uint8_t *inDataP)
{
	struct _smb_message msg;
	int ret;

	msg.smbBusNo 	= smbBusNo;
	msg.smbAddr		= smbAddr;
	msg.outData1	= outData;

	ret = Action( fd, &msg, IOM_SMB_WRITEREAD );
	
	if( ret == SMB_ERR_NO ){
		*inDataP = msg.inData;
	}
	return ret;
}
	
/***********************************************************************/
/** Write one and read two bytes from SMB device.
 *
 *	\param fd			handle (value returned by MenC_SmbAttach())
 *	\param smbBusNo		smb bus number on CPU (0..n)
 *	\param smbAddr		SMB device address
 *	\param outData		data byte to send to device
 *	\param inData1P		Output: contains first data byte read from device
 *	\param inData2P		Output: contains second data byte read from device
 *
 *	\return	\ref smbcodes
 */
int MenC_SmbWriteRead2( int fd, 
						uint8_t smbBusNo, 
						uint8_t smbAddr, 
						uint8_t outData,
						uint8_t *inData1P,
						uint8_t *inData2P)
{
	struct _smb_message msg;
	int ret;

	msg.smbBusNo 	= smbBusNo;
	msg.smbAddr		= smbAddr;
	msg.outData1	= outData;

	ret = Action( fd, &msg, IOM_SMB_WRITEREAD2 );
	
	if( ret == SMB_ERR_NO ){
		*inData1P = msg.inData;
		*inData2P = msg.inData2;
	}
	return ret;
}
	

/***********************************************************************/
/** Detach from SMB server
 *
 *	\param fd			handle (value returned by MenC_SmbAttach())
 */
void MenC_SmbDetach( int fd )
{
	close(fd);
}

/**@}*/



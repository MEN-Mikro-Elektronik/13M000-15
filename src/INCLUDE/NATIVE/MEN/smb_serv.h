#ifndef _smb_serv_h_
#define _smb_serv_h_

#include	<sys/iomsg.h>

#define SMB_SERV_NAME "/dev/smb"

/* value for _smb_message.msg_hdr.i.mgrid */
#define _IOMGR_SMB 0xf001

/* values for _smb_message.msg_hdr.i.subtype */
#define IOM_SMB_WRITE1		1		/* write outData1 */
#define IOM_SMB_WRITE2		2		/* write outData1 and outData2 */
#define IOM_SMB_READ		3		/* read inData */
#define IOM_SMB_WRITEREAD	4		/* write outData1, read inData */
#define IOM_SMB_WRITEREAD2	5		/* write outData1, read inData + inData2*/

struct	_smb_reply_hdr {
	ulong_t	 reply_status;
	ulong_t	 reply_length;
};

struct	_smb_message {
	io_msg_t					 msg_hdr;
	/*struct	_smb_reply_hdr		 rep_hdr;*/
	uint8_t	smbBusNo;			/* input */
	uint8_t	smbAddr;			/* input */
	uint8_t	outData1;			/* input */
	uint8_t	outData2;			/* input */
	uint8_t	inData;				/* output */
	uint8_t	inData2;			/* output */
};

#endif

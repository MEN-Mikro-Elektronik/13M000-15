/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  vme.c
 *      Project: men-libc for QNX6
 *
 *      \author  kp
 *        $Date: 2003/08/04 16:09:16 $
 *    $Revision: 1.2 $
 *
 *       \brief  VME related functions
 *                      
 *                      
 *     Required: -
 *
 *     Switches: -
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme.c,v $
 * Revision 1.2  2003/08/04 16:09:16  kp
 * cosmetics
 *
 * Revision 1.1  2002/04/29 08:52:25  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany 
 ****************************************************************************/
 
static const char RCSid[]="$Id: vme.c,v 1.2 2003/08/04 16:09:16 kp Exp $";

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
#include <MEN/vme_serv.h>

/*! \defgroup vmefunc VME functions
 */
/**@{*/

/***********************************************************************/
/** Attach to the VME server \c "/dev/vme".
 *	
 *  Opens the communication channel with the VME server.		   
 *	The returned value must be passed to all further MenC_VmeXXX functions	   
 *
 *  \return file descriptor or -1 on error (errno set)
 *	\sa MenC_VmeDetach
 */
int MenC_VmeAttach( void )
{
	return open( VME_SERV_NAME, O_RDWR );
}

/***********************************************************************/
/** Detach from VME server (close connection).
 *
 *	Note that detach occurs automatically when process exits		   
 *	\param fd			handle (value returned by MenC_VmeAttach())
 */
void MenC_VmeDetach( int fd )
{
	close(fd);
}

static int Action( int fd, struct _vme_message *msg, int subtype )
{
	int ret;

	msg->msg_hdr.i.type 		= _IO_MSG;
	msg->msg_hdr.i.combine_len 	= sizeof( struct _vme_message );
	msg->msg_hdr.i.mgrid 		= _IOMGR_VME;
	msg->msg_hdr.i.subtype 		= subtype;

	/* send message to server and wait for reply */
    ret = MsgSend_r( fd, msg, sizeof( struct _vme_message ), msg, 
					 sizeof( struct _vme_message ) );

	if( ret < 0 ){
		/* an error from MsgSend */
		ret = EIO;
	}
	/* otherwise ret contains the status return value from server */
	return ret;
}


/***********************************************************************/
/** Attach to a VME interrupt vector.
 *
 *	This routine attaches the calling thread to a VME interrupt vector that
 *  occurs on the specified level. If this is the first attach on that
 *  level, it implicitely unmasks that VME level in the VME bridge.		   
 *
 *  When the interrupt occurs, the VME server masks the VME interrupt level
 *  and send the \a event to your attached thread. Your thread must then
 *  clear the interrupt in the interrupting VME device (if it was not
 *  implicitely cleared by the VME IACK cycle) and must then call 
 *  MenC_VmeIrqLevelUnmask() to unmask the VME interrupt level again.
 *
 *  The \a event is sent from the VME server using \em MsgDeliverEvent(). 
 *  The event types that can be sent are typically 
 *  - \c SIGEV_PULSE:   must be received using MsgReceivePulse() or 
 *  - \c SIGEV_SIGNAL:  received by a signal handler
 *
 *  Note that you cannot use \c SIGEV_INTR.
 * 
 *  The following code snippet illustrates how to receive a SIGEV_PULSE
 *  when the interrupt occurs:
 *  \code
 *    fd = MenC_VmeAttach();
 *    // create a channel on which we can receive the pulse 
 *    chid = ChannelCreate(0);
 *    coid = ConnectAttach( 0, 0, chid, _NTO_SIDE_CHANNEL, 0 );
 *
 *    SIGEV_PULSE_INIT( &event, coid, SIGEV_PULSE_PRIO_INHERIT, 
 *                      SI_MINAVAIL, 0 );
 *
 *    intId = MenC_VmeInterruptAttachEvent( fd, vector, level, &event, 0 );
 *
 *    while(1){ 
 *        struct _pulse pulse;
 *
 *        rcvid = MsgReceivePulse( chid, &pulse, sizeof(pulse), NULL );
 *        ...
 *        // handle the interrupt
 *        MenC_VmeIrqLevelUnmask( fd, level, intId );
 *    }
 *  \endcode
 *  To detach from the vector/level, call  MenC_VmeInterruptDetachEvent().
 *  Note that the detach is automatically performed when you close the 
 *  communication channel to the VME server (e.g. when your program exits).
 *
 *  Note that more than one client can attach to the same vector.
 *
 *  The client threads shall have a priority below the vme-server's 
 *  interrupt thread (typically 40), otherwise strange results may occur
 *
 *  \param fd			handle (value returned by MenC_VmeAttach())
 *	\param vector		VME interrupt number to attach to
 *	\param level		VME interrupt level
 *	\param event		Event that should be sent on irq occurance
 *	\param flags		not yet used (pass 0)
 *
 *  \return >=0 = interrupt ID or negative error number on error
 *
 *	\sa MenC_VmeInterruptDetachEvent, MenC_VmeIrqLevelUnmask
 *
 */
int MenC_VmeInterruptAttachEvent( 
	int fd,
	int vector, 
	int level,
	const struct sigevent *event,
	unsigned flags )
{
	struct _vme_message msg;
	int ret;

	msg.p.irqAttach.vector = vector;
	msg.p.irqAttach.level  = level;	
	msg.p.irqAttach.event  = *event;
	msg.p.irqAttach.flags  = flags;

	ret = Action( fd, &msg, IOM_VME_IRQ_ATTACH );
	if( ret == 0 )
		ret = msg.p.irqAttach.id;
	else
		ret = -ret;
	return ret;
}

/***********************************************************************/
/** Detach from a VME interrupt vector.
 *
 *	If this was the last attach to a VME interrupt level, the interrupt
 *  level is masked (disabled)		   
 *
 *  \param fd	handle (value returned by MenC_VmeAttach())
 *  \param id	ID that has been returned by MenC_VmeInterruptAttachEvent()
 *
 *  \return:	error number
 */
int MenC_VmeInterruptDetachEvent( int fd, int id)
{
	struct _vme_message msg;

	msg.p.irqAttach.id	   = id;

	return Action( fd, &msg, IOM_VME_IRQ_DETACH );
}

/***********************************************************************/
/** Trigger a VMEbus interrupt.
 *			   
 *  Triggers an interrupt on the VMEbus at the specified level. When IACK is
 *  received places the specified vector on the bus.
 *
 *  If interrupter busy, returns \c EBUSY.
 *
 *  This function does not wait until the vector has been fetched by 
 *  another CPU. You may call MenC_VmeCheckIrq() to see which levels 
 *  are still pending.
 *
 *  \param fd			handle (value returned by MenC_VmeAttach())
 *	\param level		VME interrupt level (1..7)
 *	\param vector		VME interrupt vector (0..255)
 *
 *  \return error number 
 */
int MenC_VmeIssueIrq( int fd, int level, int vector )
{
	struct _vme_message msg;

	msg.p.issueIrq.level = level;
	msg.p.issueIrq.vector = vector;

	return Action( fd, &msg, IOM_VME_ISSUE_IRQ );
}

/***********************************************************************/
/** Check which interrupter levels are pending.
 *			   
 *	Can be used to check if a interrupt triggered by MenC_VmeIssueIrq()	   
 *  has been received by another CPU
 *
 *  \param fd		handle (value returned by MenC_VmeAttach())
 *  \param plevels	Output: bit mask with pending interrupter levels
 *							(Bit1=level 1, Bit7=level 7)
 *
 *  \return error number 
 */
int MenC_VmeCheckIrq ( int fd, int *plevels )
{
	struct _vme_message msg;
	int ret;

	*plevels = 0;

	ret = Action( fd, &msg, IOM_VME_CHECK_IRQ );
	
	if( ret == 0 ){
		*plevels = msg.p.irqLevels;
	}
	return ret;
}

/***********************************************************************/
/** Enables the specified VME interrupt level.
 *			   
 *  Unmask the VME interrupt level. Note that the VME server keeps
 *  track on the number of masks/unmask calls. The interrupt is
 *  unmasked only if the number of unmask calls is equal to the number
 *  previous mask calls.
 *
 *  \remark \a id is not yet used, but should be passed correctly to
 *  be compatible with future enhancements  
 *
 *  \param fd		handle (value returned by MenC_VmeAttach())
 *	\param level	VME interupt level 
 *	\param id		ID that has been returned by MenC_VmeInterruptAttachEvent()
 *
 *
 *  \return error number 
 */
int MenC_VmeIrqLevelUnmask ( int fd, int level, int id )
{
	struct _vme_message msg;

	msg.p.irqMask.level = level;
	msg.p.irqMask.id 	= id;
	
	return Action( fd, &msg, IOM_VME_IRQ_UNMASK );
}
	
/***********************************************************************/
/** Disables the specified VME interrupt level.
 *			   
 *  The level is always masked immediately.  
 *
 *  \remark \a id is not yet used, but should be passed correctly to
 *  be compatible with future enhancements  
 *
 *  \param fd		handle (value returned by MenC_VmeAttach())
 *	\param level	VME interupt level 
 *	\param id		ID that has been returned by MenC_VmeInterruptAttachEvent()
 *
 *
 *  \return error number 
 */
int MenC_VmeIrqLevelMask ( int fd, int level, int id )
{
	struct _vme_message msg;

	msg.p.irqMask.level = level;
	msg.p.irqMask.id 	= id;
	
	return Action( fd, &msg, IOM_VME_IRQ_MASK );
}


/***********************************************************************/
/** Convert VME address to CPU physical address.
 *			   
 *	The returned address has to be mapped by the calling program using
 *  mmap_device_memory()
 *
 *  \param fd 	handle (value returned by MenC_VmeAttach)
 *	\param space VME space to use (VME_MODE_Axx) or'ed with
 *		    	 data width (VME_MODE_Dxx) and or'ed with
 *				 modifiers (VME_MODE_SUPER or VME_MODE_USER), 
 *	             see \ref vmecodes.
 *	\param vmeAdrs	offset in VME space
 *	\param physAdrsP Output: CPU physical address
 *
 *  \return error number:\n
 *	- \c EINVAL if vmeadrs is out of range\n
 *	- \c ENODEV if unsupported mode specified\n
 */
int MenC_VmeBusToPhys( int fd, unsigned space, unsigned long vmeAdrs, 
					   uint64_t *physAdrsP )
{
	struct _vme_message msg;
	int ret;

	*physAdrsP = 0;
	msg.p.busToPhys.space = space;
	msg.p.busToPhys.vmeAdrs = vmeAdrs;

	ret = Action( fd, &msg, IOM_VME_BUS_TO_PHYS );
	
	if( ret == 0 ){
		*physAdrsP = msg.p.busToPhys.physAdrs;
	}
	return ret;
}

#if 0
// NYI	
/********************************* MenC_VmeBlockTransfer *********************
 *
 *  Description: VME block transfer routine
 *  
 *  This routine does not wait until transfer complete. Use 
 *	MenC_VmeCheckBlockTransfer to determine termination.
 *			   
 *  VME block transfers are made only if the VME_MODE_ALLOW_BLT bit is set.
 *  Otherwise, normal accesses are used.
 *
 *  TUNDRA UNIVERSE II NOTES:
 *  -------------------------
 *  - memAddr must be an address within the PCI memory space
 *  - lower 3 bits of memAddr and vmeAddr must be identical
 *  - for vOn/VOff see description of DGCS register
 *  - maximum size is 0xffffff bytes
 *---------------------------------------------------------------------------
 *  Input......: fd			handle (value returned by MenC_VmeAttach)
 *				 memAddr		local address
 *				 vmeMode		space, modifiers, direction, data with 
 *								(see os_men.h)
 *				 vmeAddr		address on VMEbus
 *				 size			number of bytes to transfer
 *				 vOn			VMEbus on counter
 *				 vOff			VMEbus off counter
 *  Output.....: returns:		error code
 *  Globals....: -
 ****************************************************************************/
int MenC_VmeBlockTransfer(		/* nodoc */
	int fd,
	void *memAddr,
	unsigned vmeMode,
	unsigned vmeAddr,
	unsigned size,
	unsigned vOn,
	unsigned vOff)
{
	// NYI
	return EINVAL;
}
	
/***************************** MenC_VmeCheckBlockTransfer *********************
 *
 *  Description: Check if VME blocktransfer finished
 *  
 *---------------------------------------------------------------------------
 *  Input......: fd			handle (value returned by MenC_VmeAttach)
 *  Output.....: returns:		error code
 *								0       : transfer finished successfully
 *								EBUSY	: still running
 *								EFAULT  : PCI/VME bus error
 *								EINVAL  : some other error
 *  Globals....: -
 ****************************************************************************/
int MenC_VmeCheckBlockTransfer( int fd ) /* nodoc */
{
	// NYI
	return EINVAL;
}
#endif

/**@}*/






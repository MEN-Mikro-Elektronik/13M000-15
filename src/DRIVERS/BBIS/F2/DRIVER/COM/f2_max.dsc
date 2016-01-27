#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/06/09 14:11:01 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for F2
#
#****************************************************************************

F2 {
	#------------------------------------------------------------------------
	#	general parameters (don't modify)
	#------------------------------------------------------------------------
    DESC_TYPE           = U_INT32  2			# descriptor type (2=board)
    HW_TYPE             = STRING   F2			# hardware name of device

	#------------------------------------------------------------------------
	#	debug levels (optional)
	#   this keys have only effect on debug drivers
	#------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32  0xc0008000	# BBIS driver
    DEBUG_LEVEL_BK      = U_INT32  0xc0008000	# BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32  0xc0008000	# OSS calls
    DEBUG_LEVEL_DESC    = U_INT32  0xc0008000	# DESC calls

	#------------------------------------------------------------------------
	#	device parameters
	#------------------------------------------------------------------------
	# IRQ settings - possible values: 0,5,7,9,10,11,12 (0 = no IRQ)
	IRQ_CIO_LM			= U_INT32  5	# IRQ for on-board device 0x1000 (Z8536/LM78)
	IRQ_COM3			= U_INT32  7	# IRQ for COM3
	IRQ_COM4			= U_INT32  9	# IRQ for COM4
}

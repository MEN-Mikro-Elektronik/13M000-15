#************************** BBIS3 board descriptor **************************
#
#        Author: ds
#         $Date: 2000/06/09 14:11:03 $
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
	#	device parameters
	#------------------------------------------------------------------------
	# IRQ settings - possible values: 0,5,7,9,10,11,12 (0 = no IRQ)
	IRQ_CIO_LM			= U_INT32  5	# IRQ for on-board device 0x1000 (Z8536/LM78)
	IRQ_COM3			= U_INT32  7	# IRQ for COM3
	IRQ_COM4			= U_INT32  9	# IRQ for COM4
}

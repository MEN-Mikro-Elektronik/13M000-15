#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffer@men.de
#         $Date: 2010/12/20 13:32:15 $
#     $Revision: 1.1 $
#
#   Description: Metadescriptor for SMBISA_16Z001
#
#****************************************************************************

SMBISA_16Z001 {
    #------------------------------------------------------------------------
    #   general parameters (don't modify)
    #------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       2             # descriptor type (2=board)
    HW_TYPE         = STRING        SMBISA_16Z001 # hardware name of board

    #------------------------------------------------------------------------
    #   16Z001 Controller location
    #------------------------------------------------------------------------
    DEVICE_ADDR         = U_INT32   0x00f00000    # 16Z001 base address (mem)

    #------------------------------------------------------------------------
    #   SMBus parameters
    #------------------------------------------------------------------------
    SMB_BUSNBR          = U_INT32  0        # SMB bus number
    #--- SMB devices (for PnP OS)
    SMBDEV_0 {
        NAME            = STRING   SMB2     # ID EEPROM (requires generic SMB2 driver)  
    } 
}
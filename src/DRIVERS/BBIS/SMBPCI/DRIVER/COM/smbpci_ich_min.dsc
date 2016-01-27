#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffer@men.de
#         $Date: 2010/12/20 13:32:03 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for SMBPCI_ICH (example for SC14)
#
#****************************************************************************

SMBPCI_ICH {
    #------------------------------------------------------------------------
    #   general parameters (don't modify)
    #------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       2             # descriptor type (2=board)
    HW_TYPE         = STRING        SMBPCI_ICH    # hardware name of board

    #------------------------------------------------------------------------
    #   PCI configuration (e.g. SC14 ICH SMB Controller)
    #------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0x00     # optional overwrites PCI_BUS_PATH
    PCI_DEVICE_NUMBER   = U_INT32  0x1f     # PCI device number
    PCI_FUNCTION_NUMBER = U_INT32  0x03     # PCI function number

    #------------------------------------------------------------------------
    #   SMBus parameters (e.g. SC14)
    #------------------------------------------------------------------------
    SMB_BUSNBR          = U_INT32  0        # SMB bus number
    #--- SMB devices (for PnP OS)
    SMBDEV_0 {
        NAME            = STRING   LM63     # e.g. LM63 device on SC14 ICH SMBus  
    } 
    SMBDEV_1 {
        NAME            = STRING   EEPROM   # EEPROM device on SC14 ICH SMBus 
    }   
}
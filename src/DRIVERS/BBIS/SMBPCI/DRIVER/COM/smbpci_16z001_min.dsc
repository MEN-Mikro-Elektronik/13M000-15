#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffer@men.de
#         $Date: 2010/12/20 13:32:12 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for SMBPCI_16Z001 (example for F210)
#
#****************************************************************************

SMBPCI_16Z001 {
    #------------------------------------------------------------------------
    #   general parameters (don't modify)
    #------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       2             # descriptor type (2=board)
    HW_TYPE         = STRING        SMBPCI_16Z001 # hardware name of board

    #------------------------------------------------------------------------
    #   16Z001 Controller location
    #------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0x04     # optional overwrites PCI_BUS_PATH
    PCI_DEVICE_NUMBER   = U_INT32  0x0c     # PCI device number
    CTRLR_BAR           = U_INT32  4        # BAR where 16Z001 resides

    #------------------------------------------------------------------------
    #   SMBus parameters
    #------------------------------------------------------------------------
    SMB_BUSNBR          = U_INT32  0        # SMB bus number
    #--- SMB devices (for PnP OS)
    SMBDEV_0 {
        NAME            = STRING   SMB2     # ID EEPROM (requires generic SMB2 driver)  
    } 
}
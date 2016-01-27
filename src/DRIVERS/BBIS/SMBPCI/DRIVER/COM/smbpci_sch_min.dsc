#************************** BBIS3 board descriptor **************************
#
#        Author: matthias.soehnlein@men.de
#         $Date: 2010/12/20 13:32:07 $
#     $Revision: 1.2 $
#
#   Description: Metadescriptor for SMBPCI_SCH (example for XM01)
#
#****************************************************************************

SMBPCI_SCH {
    #------------------------------------------------------------------------
    #   general parameters (don't modify)
    #------------------------------------------------------------------------
    DESC_TYPE       = U_INT32       2             # descriptor type (2=board)
    HW_TYPE         = STRING        SMBPCI_SCH    # hardware name of board

    #------------------------------------------------------------------------
    #   PCI configuration (e.g. XM01 SCH SMB Controller)
    #------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0x00     # optional overwrites PCI_BUS_PATH
    PCI_DEVICE_NUMBER   = U_INT32  0x1f     # PCI device number
    PCI_FUNCTION_NUMBER = U_INT32  0x00     # PCI function number

    #------------------------------------------------------------------------
    #   SMBus parameters (e.g. XM01)
    #------------------------------------------------------------------------
    SMB_BUSNBR          = U_INT32  0        # SMB bus number
    #--- SMB devices (for PnP OS)
    SMBDEV_0 {
        NAME            = STRING   XM01BC   # Board Controller device on XM01 SCH SMBus 
    } 
    SMBDEV_1 {
        NAME            = STRING   LM63     # e.g. LM63 device on XM01 SCH SMBus  
    }   
    SMBDEV_2 {
        NAME            = STRING   SMBUS    # Generic SMBus device 
    }   
}
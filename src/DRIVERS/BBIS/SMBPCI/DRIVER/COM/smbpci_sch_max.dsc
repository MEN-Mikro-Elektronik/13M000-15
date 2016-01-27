#************************** BBIS3 board descriptor **************************
#
#        Author: matthias.soehnlein@men.de
#         $Date: 2010/12/20 13:32:05 $
#     $Revision: 1.3 $
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
    #   debug levels (optional)
    #------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32   0xc0008000    # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32   0xc0008000    # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32   0xc0008000    # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32   0xc0008000    # DESC calls

    #------------------------------------------------------------------------
    #   PCI configuration (e.g. XM01 SCH SMB Controller)
    #------------------------------------------------------------------------
    PCI_BUS_NUMBER      = U_INT32  0x00     # optional overwrites PCI_BUS_PATH
    PCI_DEVICE_NUMBER   = U_INT32  0x1f     # PCI device number
    PCI_FUNCTION_NUMBER = U_INT32  0x00     # PCI function number
    CTRLR_BAR           = U_INT32  0x40     # Offset to BAR register

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
    SMB_ALERT_POLL_FREQ = U_INT32  1000     # SMB2 polls all 100ms for SMB alerts
    SMB_BUSY_WAIT       = U_INT32  20       # 20ms wait time for SMB2 blocking semaphore

    #------------------------------------------------------------------------
    #   other parameters
    #------------------------------------------------------------------------
    ID_CHECK            = U_INT32  1        # check PCI vendor-id and device-id
}

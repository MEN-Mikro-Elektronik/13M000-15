#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffer@men.de
#         $Date: 2010/12/20 13:32:00 $
#     $Revision: 1.4 $
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
    #   debug levels (optional)
    #------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32   0xc0008000    # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32   0xc0008000    # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32   0xc0008000    # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32   0xc0008000    # DESC calls

    #------------------------------------------------------------------------
    #   PCI configuration (e.g. SC14 ICH SMB Controller)
    #------------------------------------------------------------------------
    #PCI_BUS_PATH       = BINARY   0x08     # device IDs of bridges to PCI bus
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
    SMB_ALERT_POLL_FREQ = U_INT32  1000     # SMB2 polls all 100ms for SMB alerts
    SMB_BUSY_WAIT       = U_INT32  20       # 20ms wait time for SMB2 blocking semaphore

    #------------------------------------------------------------------------
    #   other parameters
    #------------------------------------------------------------------------
    ID_CHECK            = U_INT32  1        # check PCI vendor-id and device-id
}

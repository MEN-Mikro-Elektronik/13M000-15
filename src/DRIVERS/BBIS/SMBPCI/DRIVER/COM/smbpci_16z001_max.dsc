#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffer@men.de
#         $Date: 2010/12/20 13:32:10 $
#     $Revision: 1.3 $
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
    #   debug levels (optional)
    #------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32   0xc0008000    # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32   0xc0008000    # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32   0xc0008000    # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32   0xc0008000    # DESC calls

    #------------------------------------------------------------------------
    #   16Z001 Controller location
    #------------------------------------------------------------------------
    #PCI_BUS_PATH       = BINARY   0x08     # device IDs of bridges to PCI bus
    PCI_BUS_NUMBER      = U_INT32  0x04     # optional overwrites PCI_BUS_PATH
    PCI_DEVICE_NUMBER   = U_INT32  0x0c     # PCI device number
    PCI_FUNCTION_NUMBER = U_INT32  0x00     # PCI function number
    CTRLR_BAR           = U_INT32  4        # BAR location of 16Z001 registers
    CTRLR_OFFSET        = U_INT32  0x00     # offset to 16Z001 registers

    #------------------------------------------------------------------------
    #   SMBus parameters
    #------------------------------------------------------------------------
    SMB_BUSNBR          = U_INT32  0        # SMB bus number
    #--- SMB devices (for PnP OS)
    SMBDEV_0 {
        NAME            = STRING   SMB2     # ID EEPROM (requires generic SMB2 driver)  
    } 
    SMB_ALERT_POLL_FREQ = U_INT32  1000     # SMB2 polls all 100ms for SMB alerts
    SMB_BUSY_WAIT       = U_INT32  20       # 20ms wait time for SMB2 blocking semaphore

    #------------------------------------------------------------------------
    #   other parameters
    #------------------------------------------------------------------------
    ID_CHECK            = U_INT32  1        # check PCI vendor-id and device-id
}

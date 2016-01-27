#************************** BBIS3 board descriptor **************************
#
#        Author: dieter.pfeuffer@men.de
#         $Date: 2010/12/20 13:32:14 $
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
    #   debug levels (optional)
    #------------------------------------------------------------------------
    DEBUG_LEVEL         = U_INT32   0xc0008000    # BBIS driver
    DEBUG_LEVEL_BK      = U_INT32   0xc0008000    # BBIS kernel
    DEBUG_LEVEL_OSS     = U_INT32   0xc0008000    # OSS calls
    DEBUG_LEVEL_DESC    = U_INT32   0xc0008000    # DESC calls

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
    SMB_ALERT_POLL_FREQ = U_INT32  1000     # SMB2 polls all 100ms for SMB alerts
    SMB_BUSY_WAIT       = U_INT32  20       # 20ms wait time for SMB2 blocking semaphore
}

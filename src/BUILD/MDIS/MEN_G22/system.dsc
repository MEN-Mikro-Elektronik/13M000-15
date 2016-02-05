#
# Automatically generated by mdiswiz 2.02.025-qnx-3.2
# 2016-02-05
#

cpu {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x0
    HW_TYPE = STRING PC
    _WIZ_MODEL = STRING G22
}
smbpci_ich_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x2
    HW_TYPE = STRING SMBPCI_ICH
    _WIZ_MODEL = STRING SMBPCI_ICH
    _WIZ_BUSIF = STRING cpu,7

    # ------------------------------------------------------------------------
    #  		PCI configuration
    # ------------------------------------------------------------------------
    PCI_BUS_NUMBER = U_INT32 0x0
    PCI_DEVICE_NUMBER = U_INT32 0x1f
    PCI_FUNCTION_NUMBER = U_INT32 0x3

    # ------------------------------------------------------------------------
    #  		SMBus parameters
    # ------------------------------------------------------------------------
    SMB_BUSNBR = U_INT32 0x0
    SMBDEV_0 {
        NAME = STRING xm01bc_1
    }
    SMB_ALERT_POLL_FREQ = U_INT32 0x0
    SMB_BUSY_WAIT = U_INT32 0x64

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008000
    DEBUG_LEVEL_BK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
xm01bc_1 {

    # ------------------------------------------------------------------------
    #        general parameters (don't modify)
    # ------------------------------------------------------------------------
    DESC_TYPE = U_INT32 0x1
    HW_TYPE = STRING XM01BC
    _WIZ_MODEL = STRING XM01BC

    # ------------------------------------------------------------------------
    #        reference to base board
    # ------------------------------------------------------------------------
    BOARD_NAME = STRING smbpci_ich_1
    DEVICE_SLOT = U_INT32 0x0

    # ------------------------------------------------------------------------
    #        device parameters
    # ------------------------------------------------------------------------

    # ------------------------------------------------------------------------
    #  		SMBus parameters
    # ------------------------------------------------------------------------

    # SMBus bus number
    SMB_BUSNBR = U_INT32 0

    # SMBus address of XM1BC
    SMB_DEVADDR = U_INT32 0x9a

    # ------------------------------------------------------------------------
    #        debug levels (optional)
    #        these keys have only effect on debug drivers
    # ------------------------------------------------------------------------
    DEBUG_LEVEL = U_INT32 0xc0008003
    DEBUG_LEVEL_MK = U_INT32 0xc0008000
    DEBUG_LEVEL_OSS = U_INT32 0xc0008000
    DEBUG_LEVEL_DESC = U_INT32 0xc0008000
}
# EOF

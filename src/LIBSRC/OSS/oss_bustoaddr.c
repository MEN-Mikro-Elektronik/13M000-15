/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!  
 *        \file  oss_bustoaddr.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2010/04/30 15:13:33 $
 *    $Revision: 1.5 $
 * 
 *     \project  MDIS4QNX
 *       \brief  Bus Address translation routines and PCI access routines
 *      
 *    \switches  USE_VME_SERVER
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: oss_bustoaddr.c,v $
 * Revision 1.5  2010/04/30 15:13:33  channoyer
 * R: OSS_BusToPhysAddr, for OSS_BUSTYPE_PCI address types, return address with lowers bits set
 * M: Remove these bits
 *
 * Revision 1.4  2008/09/26 16:07:49  channoyer
 * R: Cosmetics
 * M: Surround Uint64tToVoid() with "#ifdef USE_VME_SERVER" to avoid warnings
 *
 * Revision 1.3  2004/03/25 08:31:34  ub
 * Added switch USE_VME_SERVER
 *
 * Revision 1.2  2004/03/19 14:59:22  ub
 * adapted for MDIS
 *
 * Revision 1.1  2003/08/04 14:01:32  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2003-2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
 ****************************************************************************/

#include "oss_intern.h"

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/*-----------------------------------------+
|  STATICS                                 |
+------------------------------------------*/

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifdef USE_VME_SERVER
static void *Uint64tToVoid( OSS_HANDLE *oss, uint64_t n );
#endif /* USE_VME_SERVER */
static int32 PciGetReg( OSS_HANDLE *oss, 
                        u_int32 which, 
                        int16 *idxP, 
                        int16 *accessP );


/**********************************************************************/
/** Convert bus address to CPU physical address.
 *
 * \copydoc oss_specification.c::OSS_BusToPhysAddr()
 *
 * \sa OSS_MapPhysToVirtAddr(), OSS_AssignResources()
 */
int32 OSS_BusToPhysAddr(
    OSS_HANDLE *oss,
    int32       busType,
    void       **physicalAddrP,
    ...
)
{
    int32 error = ERR_SUCCESS;
    va_list argptr;

    va_start( argptr, physicalAddrP );

    *physicalAddrP = NULL;

    switch( busType )
    {
    case OSS_BUSTYPE_NONE:
        *physicalAddrP = va_arg( argptr, void* );
        break;
        
    case OSS_BUSTYPE_PCI:
    {
        int32    busNbr      = va_arg( argptr, u_int32 );
        int32    pciDevNbr   = va_arg( argptr, u_int32 );
        int32    pciFunction = va_arg( argptr, u_int32 );
        int32    addrNbr     = va_arg( argptr, u_int32 );
        uint8_t devfunc = PCI_DEVFUNC( pciDevNbr, pciFunction );
        int32    physAdr = NULL;

        void *pciDevHdl;
        struct pci_dev_info info;

        if( oss->pciHandle < 0 ) /* PCI server not found */
            return ERR_OSS_UNK_BUSTYPE;
        
        if( addrNbr < 0 || addrNbr > 5 )
            return ERR_OSS_PCI_ILL_ADDRNBR;

        memset( &info, 0, sizeof(info));
        info.BusNumber  = busNbr;
        info.DevFunc    = devfunc;      

        pciDevHdl = pci_attach_device( 
            NULL, 
            PCI_SHARE | PCI_SEARCH_BUSDEV | PCI_INIT_ALL | PCI_PERSIST,
            0,
            &info );

        if( pciDevHdl == NULL ){
            DBGWRT_ERR( ( DBH, "*** OSS_BusToPhysAddr: pci_attach_device "
                          "dev %lx/%lx/%lx failed errno=%d\n", 
                          busNbr, pciDevNbr, pciFunction, errno ));
            
            return ERR_OSS_PCI_ILL_DEV;
        }

        OSS_PciGetConfig( oss, busNbr, pciDevNbr, pciFunction,
                          OSS_PCI_ADDR_0+addrNbr, &physAdr );

        pci_detach_device( pciDevHdl );

        if( PCI_IS_IO(physAdr) ) {
            *physicalAddrP = (void *)PCI_IO_ADDR(physAdr);
        } else {
            *physicalAddrP = (void *)PCI_MEM_ADDR(physAdr);
        }

        break;                                         
    }

#ifdef USE_VME_SERVER
    case OSS_BUSTYPE_VME:
    {
        void     *vmeadrs   = va_arg( argptr, void *);
        u_int32  vmespace   = va_arg( argptr, u_int32 );
        int      space      = 0;
        uint64_t physAdr;

        switch(vmespace & 0xff){
        case OSS_VME_A16:   space |= VME_MODE_A16; break;
        case OSS_VME_A24:   space |= VME_MODE_A24; break;
        case OSS_VME_A32:   space |= VME_MODE_A32; break;
        default:
            error = ERR_OSS_VME_ILL_SPACE;
            break;
        }

        switch(vmespace & 0xff00){
        case OSS_VME_D16:   space |= VME_MODE_D16; break;
        case OSS_VME_D32:   space |= VME_MODE_D32; break;
        default:
            error = ERR_OSS_VME_ILL_SPACE;
            break;
        }

        if( error ) break;

        error = MenC_VmeBusToPhys( oss->vmeHandle, space, (u_int32)vmeadrs, 
                                   &physAdr );
        *physicalAddrP = Uint64tToVoid( oss, physAdr );
        break;
    }
#endif /* USE_VME_SERVER */

    default:
        DBGWRT_ERR( ( DBH, "*** OSS_BusToPhysAddr: bustype %d not supported\n",
                      busType ));
        error = ERR_OSS_UNK_BUSTYPE;
        break;
    }/*switch*/

    va_end( argptr );
    DBGWRT_2((DBH, "OSS_BusToPhysAddr: physAddr=0x%p error=0x%lx\n", 
              *physicalAddrP, error ));
    return( error );
}/*OSS_BusToPhysAddr*/


/**********************************************************************/
/** Read a register from PCI configuration space.
 *
 * \copydoc oss_specification.c::OSS_PciGetConfig()
 *
 * \sa OSS_PciSetConfig, OSS_BusToPhysAddr
 */
int32 OSS_PciGetConfig(
    OSS_HANDLE *oss,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       *valueP
)
{
    int32   retCode = 0;
    uint8_t devfn = PCI_DEVFUNC( pciDevNbr, pciFunction );
    uint32_t    value32;
    uint16_t    value16;
    uint8_t     value8;
    int16 idx, access;

    DBGWRT_1((DBH,"OSS_PciGetConfig bus 0x%lx dev 0x%lx func 0x%lx "
              "which 0x%lx\n",
              busNbr, pciDevNbr, pciFunction, which));

    *valueP = 0;

    if( oss->pciHandle < 0 ) /* PCI server not found */
        return ERR_OSS_UNK_BUSTYPE;

    /*--- determine config reg offset and access ---*/

    if( (retCode = PciGetReg( oss, which, &idx, &access )))
        goto GETCFG_END;

    switch( access )
    {
        case 4:
            retCode = pci_read_config32( busNbr, devfn, idx, 1, 
										 (char *)&value32 );
            *valueP = (u_int32)value32;
            break;

        case 2:
            retCode = pci_read_config16( busNbr, devfn, idx, 1, 
										 (char *)&value16 );
            *valueP = (u_int32)value16;
            break;

        case 1:
            retCode = pci_read_config8( busNbr, devfn, idx, 1, 
										(char *)&value8 );
            *valueP = (u_int32)value8;
            break;
    }/*switch*/

    if( retCode )
    {
        DBGWRT_ERR(( DBH, "*** OSS_PciGetConfig: error 0x%x reading cfg "
					 "space: reg 0x%04x\n", retCode, idx ));
        retCode = ERR_OSS_PCI;
        *valueP = 0;
    }/*if*/

    DBGWRT_2((DBH, "  value=0x%08x\n", *valueP));

GETCFG_END:

    return( retCode );
}/*OSS_PciGetConfig*/

/**********************************************************************/
/** Write to a register in PCI configuration space.
 *
 * \copydoc oss_specification.c::OSS_PciSetConfig()
 *
 * \sa OSS_PciGetConfig
 */
int32 OSS_PciSetConfig(
    OSS_HANDLE *oss,
    int32       busNbr,
    int32       pciDevNbr,
    int32       pciFunction,
    int32       which,
    int32       value
)
{
    int32   retCode = 0;
    int16 idx, access;
    uint8_t devfn = PCI_DEVFUNC( pciDevNbr, pciFunction );
    uint32_t    value32;
    uint16_t    value16;
    uint8_t     value8;

    DBGWRT_1((DBH,"OSS_PciSetConfig bus %lx dev %lx func %lx which %lx\n",
              busNbr, pciDevNbr, pciFunction, which));

    if( oss->pciHandle < 0 ) /* PCI server not found */
        return ERR_OSS_UNK_BUSTYPE;

    /*--- determine config reg offset and access ---*/

    if( (retCode = PciGetReg( oss, which, &idx, &access )))
        goto SETCFG_END;

    switch( access )
    {
        case 4:
            value32 = value;
            retCode = pci_write_config32( 
                busNbr, devfn, idx, 1, (char *)&value32 );
            break;

        case 2:
            value16 = value;
            retCode =  pci_write_config16( 
                busNbr, devfn, idx, 1, (char *)&value16 );
            break;

        case 1:
            value8 = value;
            retCode = pci_write_config8( 
                busNbr, devfn, idx, 1, (char *)&value8);
            break;  
    }/*switch*/

    if( retCode )
    {
        DBGWRT_ERR(( DBH, "*** OSS_PciGetConfig: error 0x%x writing cfg "
                     "space: reg 0x%04x\n", retCode, idx ));
        retCode = ERR_OSS_PCI;
    }/*if*/

SETCFG_END:

    return( retCode );
}/*OSS_PciSetConfig*/

/**********************************************************************/
/** Convert mechanical slot number to PCI device number
 *
 * \copydoc oss_specification.c::OSS_PciSlotToPciDevice()
 *
 * \qnx The conversion is done by using the global array
 * #oss_pci_slot_devnbrs. 
 *
 * \qnx The PCI bus number is currently ignored
 */
int32 OSS_PciSlotToPciDevice(
    OSS_HANDLE *oss,
    int32      busNbr,
    int32      mechSlot,
    int32      *pciDevNbrP
)
{
    int32 retCode=0;

    DBGWRT_1((DBH,"OSS_PciSlotToPciDevice mechSlot=%d\n", mechSlot));

    *pciDevNbrP = -1;

    if( mechSlot < 1 || mechSlot > 17) {
        retCode = ERR_OSS_ILL_PARAM;
        DBGWRT_ERR((DBH, "*** OSS_PciSlotToPciDevice: Bad mechSlot %d\n",
                    mechSlot ));
        goto END;
    }/*if*/

    if( oss_pci_slot_devnbrs[mechSlot-1] == -1 ){
        DBGWRT_ERR((DBH, "*** OSS_PciSlotToPciDevice: MechSlot %d not "
                    "available\n", mechSlot ));
        retCode = ERR_OSS_ILL_PARAM;
    }
    else {
        *pciDevNbrP = oss_pci_slot_devnbrs[mechSlot-1];
        DBGWRT_2((DBH," pciDevNbr=0x%lx\n", *pciDevNbrP ));
    }
END:
    return( retCode );

}/*OSS_PciSlotToPciDevice*/

/********************************* PciGetReg ********************************
 *
 *  Description: Convert <which> parameter of OSS_PciGet/SetConfig 
 *             
 *               Convert to register index and access size
 *---------------------------------------------------------------------------
 *  Input......: oss        OSS handle
 *              which parameter as passed to OSS_PciGet/SetConfig 
 *  Output.....: return         success (0) or error code
 *              *idxP           register index
 *              *accessP        access size (1,2,4)
 *  Globals....: -
 ****************************************************************************/
static int32 PciGetReg( 
    OSS_HANDLE *oss, 
    u_int32 which, 
    int16 *idxP, 
    int16 *accessP )
{
    const struct {
        int16 idx;              /* PCI configuration space byte index */
        int16 access;           /* access mode byte/word/long */
    } regTbl[] = {
        { 0, 0 },               /* - */
        { 0x00, 2 },            /* OSS_PCI_VENDOR_ID */
        { 0x02, 2 },            /* OSS_PCI_DEVICE_ID */
        { 0x04, 2 },            /* OSS_PCI_COMMAND */
        { 0x06, 2 },            /* OSS_PCI_STATUS */
        { 0x08, 1 },            /* OSS_PCI_REVISION_ID */
        { 0x09, 1 },            /* OSS_PCI_CLASS */
        { 0x0a, 1 },            /* OSS_PCI_SUB_CLASS */
        { 0x0b, 1 },            /* OSS_PCI_PROG_IF */
        { 0x0c, 1 },            /* OSS_PCI_CACHE_LINE_SIZE */
        { 0x0d, 1 },            /* OSS_PCI_PCI_LATENCY_TIMER */
        { 0x0e, 1 },            /* OSS_PCI_HEADER_TYPE */
        { 0x0f, 1 },            /* OSS_PCI_BIST */
        { 0x10, 4 },            /* OSS_PCI_ADDR_0 */
        { 0x14, 4 },            /* OSS_PCI_ADDR_1 */
        { 0x18, 4 },            /* OSS_PCI_ADDR_2 */
        { 0x1c, 4 },            /* OSS_PCI_ADDR_3 */
        { 0x20, 4 },            /* OSS_PCI_ADDR_4 */
        { 0x24, 4 },            /* OSS_PCI_ADDR_5 */
        { 0x28, 4 },            /* OSS_PCI_CIS */
        { 0x2c, 2 },            /* OSS_PCI_SUBSYS_VENDOR_ID */
        { 0x2e, 2 },            /* OSS_PCI_SUBSYS_ID */
        { 0x30, 4 },            /* OSS_PCI_EXPROM_ADDR */
        { 0x3d, 1 },            /* OSS_PCI_INTERRUPT_PIN */
        { 0x3c, 1 }             /* OSS_PCI_INTERRUPT_LINE */
    };

    switch( which & OSS_PCI_ACCESS ){
    case 0x00000000:
        
        /*--- standard PCI regs ---*/
        if( which < OSS_PCI_VENDOR_ID || 
            which > OSS_PCI_INTERRUPT_LINE ){
            DBGWRT_ERR((DBH," *** OSS:PciGetReg: bad which parameter 0x%x\n", 
                        which));
            return ERR_OSS_PCI_UNK_REG;
        }
        *idxP       = regTbl[which].idx;
        *accessP    = regTbl[which].access;
        break;

    case OSS_PCI_ACCESS_8:
        /*--- byte access to any PCI config reg ---*/
        *idxP       = which & 0xff;
        *accessP    = 1;
        break;

    case OSS_PCI_ACCESS_16:
        /*--- word access to any PCI config reg ---*/
        *idxP       = which & 0xff;
        *accessP    = 2;
        break;

    case OSS_PCI_ACCESS_32:
        /*--- long access to any PCI config reg ---*/
        *idxP       = which & 0xff;
        *accessP    = 4;
        break;

    default:
        DBGWRT_ERR((DBH," *** OSS:PciGetReg: bad which parameter 0x%x\n",
                    which));
        return ERR_OSS_PCI_UNK_REG;
    }
    return 0;
}

#ifdef USE_VME_SERVER
static void *Uint64tToVoid( OSS_HANDLE *oss, uint64_t n )
{
    if( sizeof(void *) < sizeof(uint64_t) ){            
        uint64_t mask_h;

        mask_h = (~(1LL<<((8*sizeof(void*))))) << 
            ((sizeof(uint64_t) - sizeof(void*)) * 8);

        if( (n & mask_h) != 0 ) {
            DBGWRT_ERR((DBH,"*** OSS: Uint64tToVoid: Losing bits %llx\n",
                        n ));
            return NULL;
        }
    }

    return (void *)n;
}
#endif /* USE_VME_SERVER */

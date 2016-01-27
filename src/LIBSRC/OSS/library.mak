#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2011/09/01 18:09:35 $
#      $Revision: 1.3 $
#
#    Description: makefile for QNX OSS lib
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.3  2011/09/01 18:09:35  channoyer
#   R: OSS_SpinLockXxx function introduction
#   M: added new OSS_SpinLockXxx source file
#
#   Revision 1.2  2010/04/30 14:46:51  channoyer
#   R: No smb support
#   M: Add build of oss_smb.c
#
#   Revision 1.1  2004/03/24 09:40:43  ub
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=oss

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_MOD_DIR)/oss_intern.h  \
		 $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h \
         $(MEN_INC_DIR)/../../NATIVE/MEN/dbg_os.h     \

MAK_INP1=oss$(INP_SUFFIX)
MAK_INP2=oss_ident$(INP_SUFFIX)
MAK_INP3=oss_clib$(INP_SUFFIX)
MAK_INP4=oss_mem$(INP_SUFFIX)
MAK_INP5=oss_time$(INP_SUFFIX)
MAK_INP6=oss_swap$(INP_SUFFIX)
MAK_INP7=oss_bustoaddr$(INP_SUFFIX)
MAK_INP8=oss_isa_pnp$(INP_SUFFIX)
MAK_INP9=oss_alarm$(INP_SUFFIX)
MAK_INP10=oss_sig$(INP_SUFFIX)
MAK_INP11=oss_irq$(INP_SUFFIX)
MAK_INP12=oss_resource$(INP_SUFFIX)
MAK_INP13=oss_map$(INP_SUFFIX)
MAK_INP14=oss_sem$(INP_SUFFIX)
MAK_INP15=oss_dl_list$(INP_SUFFIX)
MAK_INP16=oss_task$(INP_SUFFIX)
MAK_INP17=oss_smb$(INP_SUFFIX)
MAK_INP18=oss_spinlock$(INP_SUFFIX)



MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \
        $(MAK_INP3) \
        $(MAK_INP4) \
        $(MAK_INP5) \
        $(MAK_INP6) \
        $(MAK_INP7) \
        $(MAK_INP8) \
        $(MAK_INP9) \
        $(MAK_INP10) \
        $(MAK_INP11) \
        $(MAK_INP12) \
        $(MAK_INP13) \
        $(MAK_INP14) \
        $(MAK_INP15) \
        $(MAK_INP16) \
        $(MAK_INP17) \
        $(MAK_INP18) \


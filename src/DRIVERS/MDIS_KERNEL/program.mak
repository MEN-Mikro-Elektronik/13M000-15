#***************************  M a k e f i l e  *******************************
#
#         Author: ub
#          $Date: 2004/03/25 08:50:32 $
#      $Revision: 1.2 $
#
#    Description: Makefile definitions for MEN MDIS kernel
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.2  2004/03/25 08:50:32  ub
#   removed library men-libc (now in kernel.mak)
#
#   Revision 1.1  2004/03/24 11:35:11  ub
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=mdis_kernel

MAK_LIBS=

MAK_SWITCH = -DMAC_MEM_MAPPED 
#-DTRACE

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
		 $(MEN_INC_DIR)/dbg.h         \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/maccess.h     \
         $(MEN_INC_DIR)/desctyps.h    \
         $(MEN_INC_DIR)/ll_defs.h     \
         $(MEN_INC_DIR)/ll_entry.h    \
         $(MEN_INC_DIR)/bb_defs.h     \
         $(MEN_INC_DIR)/bb_entry.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/mdis_com.h    \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_MOD_DIR)/mk_intern.h   \
         $(MEN_INC_DIR)/../../NATIVE/MEN/dbg_os.h     \

MAK_INP1=mdis_kernel$(INP_SUFFIX)
MAK_INP2=ident$(INP_SUFFIX)
MAK_INP3=open$(INP_SUFFIX)
MAK_INP4=close$(INP_SUFFIX)
MAK_INP5=calls$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2) $(MAK_INP3) $(MAK_INP4) $(MAK_INP5)

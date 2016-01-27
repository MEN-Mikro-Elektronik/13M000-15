#***************************  M a k e f i l e  *******************************
#
#         Author: ulrich.bogensperger@men.de
#          $Date: 2004/03/23 17:58:41 $
#      $Revision: 1.1 $
#
#    Description: makefile for QNX BBIS kernel lib
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  2004/03/23 17:58:41  ub
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=bbis_kernel

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/oss.h         \
         $(MEN_INC_DIR)/dbg.h         \
         $(MEN_INC_DIR)/desc.h        \
         $(MEN_INC_DIR)/maccess.h     \
         $(MEN_INC_DIR)/bb_defs.h     \
         $(MEN_INC_DIR)/bb_entry.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/../../NATIVE/MEN/dbg_os.h     \
         $(MEN_INC_DIR)/../../NATIVE/MEN/oss_os.h     \
         $(MEN_INC_DIR)/../../NATIVE/MEN/bbis_bk.h    \
         $(MEN_MOD_DIR)/bk_intern.h   \

MAK_INP1=bbis_kernel$(INP_SUFFIX) 
MAK_INP2=bk_ident$(INP_SUFFIX)    

MAK_INP=$(MAK_INP1) $(MAK_INP2) \

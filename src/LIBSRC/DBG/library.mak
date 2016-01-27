#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#      $Revision: 1.1 $
#
#    Description: makefile for Linux DBG module
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  2004/03/23 18:03:50  ub
#   Initial Revision
#
#   Revision 1.1  2001/01/19 14:38:26  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=dbg

MAK_SWITCH=-DDBG_MODULE

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/dbg.h         \
         $(MEN_INC_DIR)/../../NATIVE/MEN/dbg_os.h

MAK_INP1=dbg$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

#***************************  M a k e f i l e  *******************************
#
#         Author: CH
#          $Date: 2008/10/29 16:22:28 $
#      $Revision: 1.1 $
#
#    Description: makefile for QNX MEN-LIBC lib
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library.mak,v $
#   Revision 1.1  2008/10/29 16:22:28  channoyer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=men-libc

MAK_INCL=$(MEN_INC_DIR)/../../NATIVE/MEN/men_libc.h    \
         $(MEN_INC_DIR)/../../NATIVE/MEN/vme_serv.h    \
         $(MEN_INC_DIR)/../../NATIVE/MEN/smb_serv.h    \

MAK_INP1=vme$(INP_SUFFIX)
MAK_INP2=smb$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) \
        $(MAK_INP2) \


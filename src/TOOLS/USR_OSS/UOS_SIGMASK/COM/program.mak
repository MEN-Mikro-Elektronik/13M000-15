#***************************  M a k e f i l e  *******************************
#
#         Author: kp
#          $Date: 2001/01/19 14:57:21 $
#      $Revision: 1.1 $
#
#    Description: Makefile definitions for USR_OSS test program
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.1  2001/01/19 14:57:21  kp
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright 1999 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=uos_sigmask

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/usr_oss.h

MAK_INP1=uos_sigmask$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) 

#***************************  M a k e f i l e  *******************************
#
#         Author: matthias.soehnlein@men.de
#          $Date: 2009/02/06 15:46:22 $
#      $Revision: 1.2 $
#
#    Description: Makefile definitions for XM01BC_CTRL tool
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: program.mak,v $
#   Revision 1.2  2009/02/06 15:46:22  MSoehnlein
#   R:1. Added MM1/ XM2/ F19 Support
#   M:1. renamed Board Controller Interface (BCI) from XM01BCI to CPUBCI
#
#   Revision 1.1  2008/10/31 17:13:40  MSoehnlein
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=xm01bc_ctrl

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/mdis_api$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/usr_utl$(LIB_SUFFIX)

MAK_INCL=$(MEN_INC_DIR)/men_typs.h    \
         $(MEN_INC_DIR)/usr_oss.h     \
         $(MEN_INC_DIR)/usr_utl.h     \
         $(MEN_INC_DIR)/mdis_api.h    \
         $(MEN_INC_DIR)/mdis_err.h    \
         $(MEN_INC_DIR)/xm01bci.h     \
         $(MEN_INC_DIR)/cpubci.h      \
         $(MEN_INC_DIR)/xm01bc_drv.h  \
         
MAK_INP1=xm01bc_ctrl$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

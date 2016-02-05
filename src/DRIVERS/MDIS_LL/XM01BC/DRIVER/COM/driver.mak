#***************************  M a k e f i l e  *******************************
#
#         Author: matthias.soehnleni@men.de
#          $Date: 2009/02/06 15:46:18 $
#      $Revision: 1.2 $
#
#    Description: Makefile definitions for the XM01BC driver
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver.mak,v $
#   Revision 1.2  2009/02/06 15:46:18  MSoehnlein
#   R:1. Added MM1/ XM2/ F19 Support
#   M:1. renamed Board Controller Interface (BCI) from XM01BCI to CPUBCI
#
#   Revision 1.1  2008/10/31 17:13:34  MSoehnlein
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=xm01bc

MAK_SWITCH=$(SW_PREFIX)MAC_MEM_MAPPED

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)	\

MAK_INCL=$(MEN_INC_DIR)/xm01bc_drv.h	\
		 $(MEN_INC_DIR)/men_typs.h	\
		 $(MEN_INC_DIR)/maccess.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/oss.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
		 $(MEN_INC_DIR)/ll_defs.h	\
		 $(MEN_INC_DIR)/ll_entry.h	\
 		 $(MEN_INC_DIR)/smb2.h		\
 		 $(MEN_INC_DIR)/cpubci.h	\
 		 $(MEN_INC_DIR)/xm01bci.h	\
 		 $(MEN_INC_DIR)/wdog.h	\

MAK_INP1=xm01bc_drv$(INP_SUFFIX)

MAK_INP=$(MAK_INP1)

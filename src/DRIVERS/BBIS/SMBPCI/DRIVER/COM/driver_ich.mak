#***************************  M a k e f i l e  *******************************
#  
#         Author: dieter.pfeuffer@men.de
#          $Date: 2008/01/22 14:19:36 $
#      $Revision: 1.3 $
#  
#    Description: Makefile definitions for SMBPCI BBIS - variant ICH
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_ich.mak,v $
#   Revision 1.3  2008/01/22 14:19:36  ufranke
#   fixed
#    - now compilable for !ONE_NAMESPACE_PER_DRIVER
#
#   Revision 1.2  2006/08/02 12:25:03  DPfeuffer
#   SMBPCI_VARIANT replaced by SMBPCI_ICH
#
#   Revision 1.1  2005/11/25 15:08:51  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=smbpci_ich

MAK_SWITCH=$(SW_PREFIX)MAC_IO_MAPPED \
		   $(SW_PREFIX)_SMBPCI_ICH
		    	 	 
MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
	 	 $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/smb2_io$(LIB_SUFFIX)

MAK_INCL=$(MEN_MOD_DIR)/variant.h	\
		 $(MEN_MOD_DIR)/smbpci_int.h\
		 $(MEN_INC_DIR)/bb_smbpci.h	\
		 $(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/maccess.h	\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
		 $(MEN_INC_DIR)/men_typs.h	\
		 $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_smbpci$(INP_SUFFIX)
MAK_INP2=smb_ich$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2)


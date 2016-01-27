#***************************  M a k e f i l e  *******************************
#  
#         Author: matthias.soehnlein@men.de
#          $Date: 2008/10/20 13:19:55 $
#      $Revision: 1.1 $
#  
#    Description: Makefile definitions for SMBPCI BBIS - variant SCH
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_sch.mak,v $
#   Revision 1.1  2008/10/20 13:19:55  MSoehnlein
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=smbpci_sch

MAK_SWITCH=$(SW_PREFIX)MAC_IO_MAPPED \
		   $(SW_PREFIX)_SMBPCI_SCH
		    	 	 
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
MAK_INP2=smb_sch$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2)


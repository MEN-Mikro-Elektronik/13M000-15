#***************************  M a k e f i l e  *******************************
#
#         Author: christian.schuster@men.de
#          $Date: 2007/07/11 09:34:21 $
#      $Revision: 2.2 $
#
#    Description: Makefile descriptor file for CHAMELEON lib (io access)
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_usr_io.mak,v $
#   Revision 2.2  2007/07/11 09:34:21  CKauntz
#   only Chameleon V2 API exported
#   added CHAMV2_VARIANT
#
#   Revision 2.1  2005/07/08 17:58:08  cs
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=chameleon_usr_io

MAK_INCL=$(MEN_MOD_DIR)/chameleon_int.h \
		 $(MEN_MOD_DIR)/chameleon.c     \
		 $(MEN_INC_DIR)/men_typs.h    	\
		 $(MEN_INC_DIR)/chameleon.h		\
		 $(MEN_INC_DIR)/maccess.h		\
		 $(MEN_INC_DIR)/oss.h

MAK_SWITCH=$(SW_PREFIX)MAC_IO_MAPPED 		\
		   $(SW_PREFIX)CHAM_VARIANT=IO  	\
           $(SW_PREFIX)CHAMV2_VARIANT=Io	\
           $(SW_PREFIX)MAC_USERSPACE

MAK_INP1 = chameleonv2$(INP_SUFFIX)
MAK_INP2 = chameleon_strings$(INP_SUFFIX)

MAK_INP  = $(MAK_INP1) \
		   $(MAK_INP2)

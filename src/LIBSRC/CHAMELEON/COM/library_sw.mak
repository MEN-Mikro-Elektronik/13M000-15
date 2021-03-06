#***************************  M a k e f i l e  *******************************
#
#         Author: christian.kauntz@men.de
#          $Date: 2007/07/11 10:34:25 $
#      $Revision: 2.1 $
#
#    Description: Makefile descriptor file for CHAMELEON lib
#
#---------------------------------[ History ]---------------------------------
#
#   $Log: library_sw.mak,v $
#   Revision 2.1  2007/07/11 10:34:25  CKauntz
#   Initial Revision
#
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2002 by MEN mikro elektronik GmbH, Nuernberg, Germany
#*****************************************************************************

MAK_NAME=chameleon_sw

MMAK_INCL=$(MEN_MOD_DIR)/chameleon_int.h \
		 $(MEN_MOD_DIR)/chameleon.c     \
		 $(MEN_INC_DIR)/men_typs.h    	\
		 $(MEN_INC_DIR)/chameleon.h		\
		 $(MEN_INC_DIR)/maccess.h		\
		 $(MEN_INC_DIR)/oss.h

MAK_SWITCH= $(SW_PREFIX)MAC_MEM_MAPPED        \
			$(SW_PREFIX)MAC_BYTESWAP          \
            $(SW_PREFIX)CHAMV2_VARIANT=MemSw

MAK_INP1 = chameleonv2$(INP_SUFFIX)
MAK_INP2 = chameleon_strings$(INP_SUFFIX)

MAK_INP  = $(MAK_INP1) \
		   $(MAK_INP2)


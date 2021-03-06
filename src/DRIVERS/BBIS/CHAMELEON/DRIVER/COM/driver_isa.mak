#***************************  M a k e f i l e  *******************************
#  
#         Author: dieter.pfeuffer@men.de
#          $Date: 2012/02/08 09:20:19 $
#      $Revision: 1.3 $
#  
#    Description: Makefile definitions for CHAMELEON BBIS - variant ISA
#                      
#---------------------------------[ History ]---------------------------------
#
#   $Log: driver_isa.mak,v $
#   Revision 1.3  2012/02/08 09:20:19  dpfeuffer
#   R: rollback of LIBSRC\CHAMELEON\COM\library.mak
#   M: undo of last modification
#
#   Revision 1.2  2012/02/06 11:38:40  dpfeuffer
#   R: LIBSRC\CHAMELEON\COM\library.mak: added CHAM_VARIANT=MEM changes
#      function names of CHAM_ModCodeToDevId and CHAM_DevIdToName
#   M: CHAM_VARIANT=MEM added here too to resolve external symbols
#
#   Revision 1.1  2011/01/19 11:02:59  dpfeuffer
#   Initial Revision
#
#-----------------------------------------------------------------------------
#   (c) Copyright by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

MAK_NAME=chameleon_isa

MAK_LIBS=$(LIB_PREFIX)$(MEN_LIB_DIR)/desc$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/chameleon_io$(LIB_SUFFIX) \
         $(LIB_PREFIX)$(MEN_LIB_DIR)/oss$(LIB_SUFFIX)	\
         $(LIB_PREFIX)$(MEN_LIB_DIR)/dbg$(LIB_SUFFIX)

MAK_SWITCH=$(SW_PREFIX)CHAM_ISA

MAK_INCL=$(MEN_INC_DIR)/bb_chameleon.h	\
		 $(MEN_INC_DIR)/bb_defs.h	\
		 $(MEN_INC_DIR)/bb_entry.h	\
		 $(MEN_INC_DIR)/dbg.h		\
		 $(MEN_INC_DIR)/desc.h		\
		 $(MEN_INC_DIR)/mdis_api.h	\
		 $(MEN_INC_DIR)/mdis_com.h	\
		 $(MEN_INC_DIR)/mdis_err.h	\
         $(MEN_INC_DIR)/men_typs.h	\
         $(MEN_INC_DIR)/oss.h

MAK_INP1=bb_chameleon$(INP_SUFFIX)
MAK_INP2=io_access$(INP_SUFFIX)

MAK_INP=$(MAK_INP1) $(MAK_INP2)



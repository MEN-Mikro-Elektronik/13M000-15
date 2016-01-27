#***************************  M a k e f i l e  *******************************
#  
#         Author: ub
#          $Date: 2004/03/24 11:35:35 $
#      $Revision: 1.1 $
#  
#    Description: Make a QNX MDIS user state library (static library)
#                      
#       Switches: 
#				  COMMAKE		common makefile name (with path)
#				  WARN_LEVEL 	(default -Wall)
#				  THIS_DIR		directory where this file is located
#				  QNX_SRC_DIR	absolute path to directory that contains the
#								QNX source tree
#		Rules:	  all			main rule for libraries
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

LIB_PREFIX      :=
LIB_SUFFIX      := .a
MEN_LIB_DIR     := $(MEN_QNX_DIR)/qnx6/$(ARCH)/usr/lib
INP_SUFFIX      := .o
SW_PREFIX		:= -D
INC_DIR         := $(QNX_SRC_DIR)/INCLUDE/COM
MEN_INC_DIR     := $(INC_DIR)/MEN

MEN_MOD_DIR		:= $(dir $(COMMAKE))


#**************************************
#	include common makefile
#
ifdef COMMAKE
	include $(COMMAKE)
endif

COMP_NAME := $(MAK_NAME)
LIB_NAME  := lib$(LIB_PREFIX)$(COMP_NAME)$(LIB_SUFFIX)
COMP_OBJ  := $(STATIC_LIB_INSTALL_DIR)/$(LIB_NAME)


TMP_OBJ_DIR	  := OBJ/$(COMP_NAME)_static

TMP_COMP_OBJS := $(MAK_INP)

vpath %.c $(MEN_MOD_DIR)

#**************************************
#   include pathes
#
NAT_INC_DIR     :=  $(QNX_SRC_DIR)/INCLUDE/NATIVE

INC_DIRS		:= \
    -I$(INC_DIR)                    \
    -I$(NAT_INC_DIR)                \
    -I$(MEN_MOD_DIR)

#**************************************
#   Compiler flags
#

DEF             :=$(MDIS_EXTRA_DEFS)

ifndef MAK_SWITCH
    MAK_SWITCH := 
endif

ifndef WARN_LEVEL
# enable all warnings, but disable warning about wrong type passing to printf 
    WARN_LEVEL := -Wall -Wno-format
endif

ifeq "$(ARCH)"  "ppcbe"
    TARGET_FLAGS := -Vgcc_ntoppc -EB
else
    ifeq "$(ARCH)" "x86"
        TARGET_FLAGS := -Vgcc_ntox86
    else
        FAILURE! ARCH must be ppcbe or x86.
    endif
endif

CFLAGS          :=  $(TARGET_FLAGS) -c -O2 $(DEF) \
					$(MAK_SWITCH) $(INC_DIRS) $(WARN_LEVEL)


#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all buildmsg buildinobjdir build

all: buildmsg $(TMP_OBJ_DIR) $(STATIC_LIB_INSTALL_DIR) buildinobjdir 

buildmsg:
	@$(ECHO) 
	@$(ECHO) ++++++++ Building static user library $(LIB_NAME) +++++++++++

$(TMP_OBJ_DIR):
	- @ mkdir -p $(TMP_OBJ_DIR)
	@$(ECHO) Directory $(TMP_OBJ_DIR) created

$(STATIC_LIB_INSTALL_DIR):
	- @ mkdir -p $(STATIC_LIB_INSTALL_DIR)
	@$(ECHO) Directory $(STATIC_LIB_INSTALL_DIR) created

buildinobjdir:
	@$(MAKE) -C $(TMP_OBJ_DIR) --no-print-directory\
	 -f $(USRLIB_MAK) build


#----------------------------------------------------------------------
# Rules for level #1 of make
#

build: $(COMP_OBJ)

# override default buildin rule for .c -> .o files
%.o : %.c
	@$(ECHO) Compiling $*.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $(notdir $@)

# create static library
$(COMP_OBJ): $(TMP_COMP_OBJS)
	@$(AR) -r $(COMP_OBJ) $(notdir $(TMP_COMP_OBJS))

# Dependencies
$(TMP_COMP_OBJS): 	$(MAK_INCL) \
			 		$(COMMAKE) 

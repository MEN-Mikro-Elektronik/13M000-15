#***************************  M a k e f i l e  *******************************
#  
#         Author: ub
#          $Date: 2008/11/13 15:47:27 $
#      $Revision: 1.2 $
#  
#    Description: Make a QNX MDIS user state program
#                 Compile to use shared libraries
#     
#       Switches: 
#				  COMMAKE		common makefile name (with path)
#				  WARN_LEVEL 	(default -Wall)
#				  THIS_DIR		path of config directory
#				  QNX_SRC_DIR	absolute path to directory that contains the
#								QNX source tree
#		Rules:	  all			main rule for programs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

LIB_PREFIX      := 
LIB_SUFFIX      := .so
MEN_LIB_DIR     := $(MEN_QNX_DIR)/qnx6/$(ARCH)/usr/lib
INC_DIR         := $(QNX_SRC_DIR)/INCLUDE/COM
MEN_INC_DIR     := $(INC_DIR)/MEN
INP_SUFFIX      := .o
SW_PREFIX		:= -D

# obsolete...
OPT_1           := opt1
OPT_2           := opt1
OPT_3           := opt1

MEN_MOD_DIR		:= $(dir $(COMMAKE))


#**************************************
# include common makefile
#
ifdef COMMAKE
	include $(COMMAKE)
endif

#**************************************
#	determine target name 
#
COMP_NAME := $(MAK_NAME)
COMP_OBJ  := $(BIN_INSTALL_DIR)/$(COMP_NAME)

TMP_OBJ_DIR	  := OBJ/$(COMP_NAME)

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
    MAK_SWITCH := -DMAC_MEM_MAPPED
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

CFLAGS          := $(TARGET_FLAGS) -O2 $(DEF) \
					$(MAK_SWITCH) $(INC_DIRS) $(WARN_LEVEL) $(DBG) -ggdb

LDFLAGS			:= $(TARGET_FLAGS) -lm

#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all buildmsg buildinobjdir build 

all: buildmsg $(TMP_OBJ_DIR) $(BIN_INSTALL_DIR) buildinobjdir

buildmsg:
	@$(ECHO)
	@$(ECHO) ++++++++ Building $(COMP_NAME) +++++++++++

$(TMP_OBJ_DIR):
	- @ mkdir -p $(TMP_OBJ_DIR)
	@$(ECHO) Directory $(TMP_OBJ_DIR) created

$(BIN_INSTALL_DIR):
	- @ mkdir -p $(BIN_INSTALL_DIR)
	@$(ECHO) Directory $(BIN_INSTALL_DIR) created

buildinobjdir:
	@$(MAKE) -C $(TMP_OBJ_DIR) --no-print-directory \
	-f $(USRPROG_MAK) build

#----------------------------------------------------------------------
# Rules for level #1 of make
#

build: $(COMP_OBJ)

# override default builtin rule for .c -> .o files
%.o : %.c
	@$(ECHO) Compiling $*.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# link: add all directories specified in MAK_LIBS to link path
$(COMP_OBJ): $(TMP_COMP_OBJS)
	$(Q)$(CC) $(LDFLAGS) $(TMP_COMP_OBJS) \
		$(addprefix -L,$(sort $(dir $(MAK_LIBS)) \
		$(addsuffix /,$(LIB_INSTALL_DIR)))) \
		$(addprefix -l,$(basename $(notdir $(MAK_LIBS)))) \
		-o $@

# Dependencies
$(TMP_COMP_OBJS): 	$(MAK_INCL) \
			 		$(COMMAKE) 

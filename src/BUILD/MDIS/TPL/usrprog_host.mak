#***************************  M a k e f i l e  *******************************
#  
#         Author: ch
#          $Date: 2008/11/13 15:47:09 $
#      $Revision: 1.1 $
#  
#    Description: Make a host MDIS user state program for QNX
#     
#       Switches: 
#				  COMMAKE		common makefile name (with path)
#				  WARN_LEVEL 	(default -Wall)
#				  QNX_SRC_DIR	absolute path to directory that contains the
#								QNX source tree
#		Rules:	  all			main rule for programs
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2008 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

LIB_PREFIX      :=
LIB_SUFFIX      := .a
MEN_LIB_DIR     := $(MEN_QNX_DIR)/qnx6/$(ARCH)/usr/lib
INC_DIR         := $(QNX_SRC_DIR)/INCLUDE/COM
MEN_INC_DIR     := $(INC_DIR)/MEN
INP_SUFFIX      := .o
SW_PREFIX		:= -D

CC              := gcc

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
ifeq "$(HOST_DEV)" "Linux"
	export BINHOST_INSTALL_DIR := $(MEN_QNX_DIR)/LINUX/BIN/
else
	ifeq "$(HOST_DEV)" "QNX"
		export BINHOST_INSTALL_DIR := /usr/local/bin/
	endif
endif

COMP_OBJ  := $(BINHOST_INSTALL_DIR)$(COMP_NAME)

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
ifeq "$(HOST_DEV)" "Linux"
	DEF             :=-DLINUX $(MDIS_EXTRA_DEFS)
else
	DEF             :=$(MDIS_EXTRA_DEFS)
endif

ifndef MAK_SWITCH
    MAK_SWITCH := -DMAC_MEM_MAPPED
endif

ifndef WARN_LEVEL
# enable all warnings, but disable warning about wrong type passing to printf 
    WARN_LEVEL := -Wall -Wno-format
endif

CFLAGS          := -O2 $(DEF) \
					$(MAK_SWITCH) $(INC_DIRS) $(WARN_LEVEL)

LDFLAGS			:= -lm -static

#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all buildmsg buildinobjdir build 

all: buildmsg $(TMP_OBJ_DIR) $(BINHOST_INSTALL_DIR) buildinobjdir

buildmsg:
	@$(ECHO)
	@$(ECHO) ++++++++ Building $(COMP_NAME) +++++++++++

$(TMP_OBJ_DIR):
	- @ mkdir -p $(TMP_OBJ_DIR)
	@$(ECHO) Directory $(TMP_OBJ_DIR) created

$(BINHOST_INSTALL_DIR):
	- @ mkdir -p $(BINHOST_INSTALL_DIR)
	@$(ECHO) Directory $(BINHOST_INSTALL_DIR) created

buildinobjdir:
	@$(MAKE) -C $(TMP_OBJ_DIR) --no-print-directory \
	 -f $(USRHOST_MAK) build

#----------------------------------------------------------------------
# Rules for level #1 of make
#

build: $(COMP_OBJ)

# override default builtin rule for .c -> .o files (just to get less messages)
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

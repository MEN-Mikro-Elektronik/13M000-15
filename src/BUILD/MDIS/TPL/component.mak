#***************************  M a k e f i l e  *******************************
#  
#         Author: ub
#          $Date: 2008/11/13 15:45:34 $
#      $Revision: 1.3 $
#  
#    Description: Make a QNX MDIS core component 
#                      
#       Switches: 
#				  COMMAKE		common makefile name (with path)
#				  DEBUG			(dbg/nodbg) (dbg means debug prints enabled)
#				  COMP_PREFIX 	component prefix (e.g ll_)
#				  WARN_LEVEL 	(default -Wall)
#				  LLDRV 		define as -D_LL_DRV_ or nothing
#				  MEN_QNX_DIR	absolute path to QNX src directory
#		Rules:	  all			main rule for component
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

LIB_PREFIX      :=
LIB_SUFFIX      := .so
QNX_LIB_DIR     := $(MEN_QNX_DIR)/qnx6/$(ARCH)/usr/lib
INC_DIR         := $(QNX_SRC_DIR)/INCLUDE/COM
MEN_INC_DIR     := $(INC_DIR)/MEN
INP_SUFFIX		:= .o
SW_PREFIX		:= -D


#**************************************
# include common makefile
#
ifdef COMMAKE
	include $(COMMAKE)
endif

MEN_MOD_DIR		:= $(dir $(COMMAKE))


#**************************************
#	determine target name 
#
ifeq ($(DEBUG),dbg)
    DBGSUFFIX	:= _dbg
    DBG			:= -DDBG
    DBGSTR		:= debug
    COMP_NAME	:= $(COMP_PREFIX)$(MAK_NAME)$(DBGSUFFIX)
endif
ifeq ($(DEBUG),nodbg)
    DBGSUFFIX	:= _nodbg
    DBG			:=
    DBGSTR		:= non-debug
    COMP_NAME	:= $(COMP_PREFIX)$(MAK_NAME)
endif 

ifndef DBGSUFFIX
	FAILURE! DEBUG must be dbg or nodbg!
endif

LIB_NAME  		:= lib$(COMP_NAME)$(LIB_SUFFIX)
COMP_OBJ  		:= $(LIB_INSTALL_DIR)/$(LIB_NAME)

TMP_OBJ_DIR	  	:= OBJ/$(COMP_NAME)
TMP_COMP_OBJS 	:= $(MAK_INP)


#**************************************
# search for C sources in MEN_MOD_DIR
#
vpath %.c $(MEN_MOD_DIR)


#**************************************
#   include pathes
#
NAT_INC_DIR     :=  $(QNX_SRC_DIR)/INCLUDE/NATIVE

INC_DIRS		:=	\
	-I$(INC_DIR)  			\
    -I$(NAT_INC_DIR)		\
    -I$(MEN_MOD_DIR)


#**************************************
#   Compiler flags
#

DEF             :=-DCOMP_NAME=\"$(COMP_NAME)\" \
				  -D_ONE_NAMESPACE_PER_DRIVER_ $(MDIS_EXTRA_DEFS)

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
        FAILURE! ARCH must be either ppcbe or x86.
    endif
endif

CFLAGS          :=  $(TARGET_FLAGS) -c -O -shared $(DEF) $(DBG) \
					$(MAK_SWITCH) $(INC_DIRS) $(WARN_LEVEL) $(LLDRV)

# use MEN VME server if desired
ifeq ($(USE_VME_SERVER),yes)
	CFLAGS      += -DUSE_VME_SERVER
endif


#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all buildmsg buildinobjdir build

all: buildmsg $(TMP_OBJ_DIR) $(LIB_INSTALL_DIR) buildinobjdir

buildmsg:
	@$(ECHO) 
	@$(ECHO) ++++++++ Building $(DBGSTR) version of module $(COMP_NAME) +++++++++++


$(TMP_OBJ_DIR):
	- @ mkdir -p $(TMP_OBJ_DIR)
	@$(ECHO) Directory $(TMP_OBJ_DIR) created

$(LIB_INSTALL_DIR):
	- @ mkdir -p $(LIB_INSTALL_DIR)
	@$(ECHO) Directory $(LIB_INSTALL_DIR) created

buildinobjdir:
	@$(MAKE) -C $(TMP_OBJ_DIR) --no-print-directory -f $(COMP_MAK) build


#----------------------------------------------------------------------
# Rules for level #1 of make
#

build: $(COMP_OBJ) 


# override default buildin rule for .c -> .o files
%.o : %.c
	@$(ECHO) Compiling $*.c
	$(Q)$(CC) $(CFLAGS) $(MODVERDEF) $(CPPFLAGS) -c $< -o $(notdir $@)

# link component
$(COMP_OBJ): $(TMP_COMP_OBJS) 
	$(Q)$(CC) $(TARGET_FLAGS) -shared -Wl,-soname,$(LIB_NAME) \
		-o $(COMP_OBJ) $(notdir $(TMP_COMP_OBJS)) 

# Dependencies
$(TMP_COMP_OBJS): 	$(MAK_INCL) \
			 		$(COMMAKE) 

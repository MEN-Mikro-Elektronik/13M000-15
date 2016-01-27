#***************************  M a k e f i l e  *******************************
#  
#         Author: ub
#          $Date: 2011/04/19 16:41:48 $
#      $Revision: 1.7 $
#  
#    Description: Defines top level rules for MDIS components
#                 This file should be included at the bottom of the 
#				  configurations makefile
#     
#       Switches: 
#		Rules:	  
#
#-----------------------------------------------------------------------------
#   (c) Copyright 2003 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

# determine current directory and configuration name
cwd := $(shell pwd)

ifeq ($(V),1)
 Q =
else
 Q = @
endif
export Q

export THIS_DIR		:= $(cwd)

# determine host dev system
host_dev			:= $(shell uname)
export HOST_DEV		:= $(host_dev)

ifndef QNX_SRC_DIR
	QNX_SRC_DIR		:= $(MEN_QNX_DIR)/src
endif

ifndef QNX_TARGET_DIR
	QNX_TARGET_DIR	:= $(MEN_QNX_DIR)/qnx6/$(ARCH)
endif

export MEN_QNX_DIR
export QNX_SRC_DIR
export QNX_TARGET_DIR
export ARCH
export CC			:= $(QNX_HOST)/usr/bin/qcc
export LD			:= $(QNX_HOST)/usr/bin/qcc
export AR			:= $(QNX_HOST)/usr/bin/ntoppc-ar

export NAT_PATH		:= $(QNX_SRC_DIR)/DRIVERS
export LL_PATH		:= $(QNX_SRC_DIR)/DRIVERS/MDIS_LL
export BB_PATH		:= $(QNX_SRC_DIR)/DRIVERS/BBIS
export LS_PATH		:= $(QNX_SRC_DIR)/LIBSRC
export TO_PATH		:= $(QNX_SRC_DIR)/TOOLS

export PRG_OUTPUT_DIR := $(QNX_TARGET_DIR)/usr/bin
export LIB_OUTPUT_DIR := $(QNX_TARGET_DIR)/usr/lib


#-----------------------------------------------------------
# MDIS COMMON COMPONENTS 
#
KERNEL			:= \
					MDIS_KERNEL/program.mak \

ALL_CORE 		+= \
					BBIS_KERNEL/library.mak \
					OSS/library.mak \
					DBG/library.mak \
					DESC/COM/library.mak \
					ID/COM/library.mak \
					ID/COM/library_sw.mak \
					MBUF/COM/library.mak \
					PLD/COM/library.mak \
					PLD/COM/library_sw.mak \
					SMB/PORT/COM/library.mak \
					SMB/PORT/COM/library_io.mak \
					CHAMELEON/COM/library.mak \
					CHAMELEON/COM/library_io.mak \
					men-libc/library.mak \
					SMB2/COM/library.mak \
					SMB2/COM/library_io.mak \

KERNEL_CORE_LIBS   += \
					men_oss          \
					men_dbg          \
					men_desc         \
					men_id           \
					men_id_sw        \
					men_mbuf         \
					men_pld          \
					men_pld_sw       \
					men_smb_port     \
					men_smb_port_io  \
					men_bbis_kernel  \
					men_chameleon    \
					men_chameleon_io \
					men_men-libc     \
					men_smb2         \
					men_smb2_io      \

ALL_USR_LIBS 	+=  \
					USR_OSS/library.mak \
					USR_UTL/COM/library.mak \
					MDIS_API/library.mak \

ALL_COM_TOOLS	+=  \

ALL_NATIVE_TOOLS	+=  \

ifeq "$(HOST_DEV)" "Linux"
ALL_DEV_TOOLS	+= \
					DESCGEN/COM/program.mak
else
	ifeq "$(HOST_DEV)" "QNX"
ALL_DEV_TOOLS	+= \
					DESCGEN/COM/program.mak
	endif
endif


#-----------------------------------------------------------
# some tools
ifndef MAKE
 export MAKE			:= make
endif

ifndef ECHO
 export ECHO			:= echo
endif

ifndef DEBUG
	DEBUG := nodbg
endif

export BIN_INSTALL_DIR
export LIB_INSTALL_DIR
export STATIC_LIB_INSTALL_DIR
export DESC_INSTALL_DIR
export DEBUG
export ALL_DESC
export MDIS_EXTRA_DEFS
export USE_VME_SERVER

export _DESCGEN    := descgen
export CONFIG_NAME := $(notdir $(THIS_DIR))
export TPL_DIR	   := $(QNX_SRC_DIR)/BUILD/MDIS/TPL/
export COMP_MAK    := $(TPL_DIR)component.mak
export KERNEL_MAK  := $(TPL_DIR)kernel.mak
export USRLIB_MAK  := $(TPL_DIR)usrlib_$(LIB_MODE).mak
export USRPROG_MAK := $(TPL_DIR)usrprog_$(LIB_MODE).mak
export USRHOST_MAK := $(TPL_DIR)usrprog_host.mak
export DESC_MAK    := $(TPL_DIR)desc.mak

MOD_OUTPUT_DIR 	   := MODULES
OBJ_DIR			   := OBJ

MAKEIT 				= $(Q)$(MAKE) --no-print-directory

# always generate debug binaries if DEBUG given on comand line
ifeq "$(origin DEBUG)" "command line"
	ALL_DBGS := $(DEBUG)
endif


#----------------------------------------
# Get byteorder of target machine (for descgen)
#
ifndef DESCGEN_BYTEORDER
    ifeq "$(ARCH)"  "ppcbe"
		export DESCGEN_BYTEORDER = -bin_big
     else
        ifeq "$(ARCH)" "x86"
			export DESCGEN_BYTEORDER = -bin_lit
        else
            FAILURE! ARCH must be either ppcbe or x86.
        endif
     endif
endif


#----------------------------------------
# Rules
#
.PHONY:	buildall buildmods buildusr builddesc clean \
		kernel all_ll all_bb all_core all_raw all_usr_libs \
		all_ll_tools all_com_tools all_native_tools all_dev_tools all_desc \
		$(ALL_DBGS) $(ALL_LL_DRIVERS) \
		$(ALL_BB_DRIVERS) $(ALL_CORE) $(KERNEL) \
		$(ALL_USR_LIBS) $(ALL_COM_TOOLS) $(ALL_NATIVE_TOOLS) $(ALL_DEV_TOOLS) $(ALL_RAW) \
		$(ALL_NATIVE_DRIVERS) \

# rule to make everything
buildall: buildmods buildusr builddesc

# rule to build debug and/or non-debug version of all modules
buildmods: $(ALL_DBGS)

$(ALL_DBGS): 
	$(MAKEIT) RULE=$(RULE) DEBUG=$@ buildfordbg

buildfordbg: all_core kernel \
			 all_ll all_bb all_raw \

# rule to build all user state libs and programs
buildusr: all_usr_libs all_ll_tools all_com_tools all_native_tools all_dev_tools

all_raw: all_usr_libs

# rule to build all descriptors
builddesc: all_desc

clean: 
	@$(ECHO) "Removing all objects, binaries, libraries"
	rm -rf OBJ 


kernel:				$(KERNEL)
all_ll:				$(ALL_LL_DRIVERS)
all_bb:				$(ALL_BB_DRIVERS)
all_core: 			$(ALL_CORE)
all_usr_libs: 		$(ALL_USR_LIBS)
all_ll_tools:		$(ALL_LL_TOOLS)
all_com_tools:		$(ALL_COM_TOOLS)
all_native_tools:	$(ALL_NATIVE_TOOLS)
all_dev_tools:		$(ALL_DEV_TOOLS)
all_desc:			$(ALL_DESC)
all_raw: 			$(ALL_RAW) $(ALL_NATIVE_DRIVERS)

printit:
	@echo CC is $(CC)
	@echo LD is $(LD)	
	@echo AR is $(AR)
	@echo CFLAGS is $(CFLAGS)


$(KERNEL):
	$(MAKEIT) -f $(KERNEL_MAK) \
		KERNEL_CORE_LIBS='$(KERNEL_CORE_LIBS)' \
		COMMAKE=$(NAT_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_

$(ALL_LL_DRIVERS):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(LL_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ll_ \
		LLDRV=-D_LL_DRV_

$(ALL_BB_DRIVERS):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(BB_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_bb_ 

$(ALL_CORE):
	$(MAKEIT) -f $(COMP_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_

$(ALL_USR_LIBS):
	$(MAKEIT) -f $(USRLIB_MAK) $(RULE) \
		COMMAKE=$(LS_PATH)/$@ \
		COMP_PREFIX=

$(ALL_LL_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(LL_PATH)/$@ \
		COMP_PREFIX=

$(ALL_COM_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(TO_PATH)/$@ \
		COMP_PREFIX=

$(ALL_NATIVE_TOOLS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(TO_PATH)/$@ \
		COMP_PREFIX=

$(ALL_DEV_TOOLS):
	$(MAKEIT) -f $(USRHOST_MAK) $(RULE) \
		COMMAKE=$(TO_PATH)/$@ \
		COMP_PREFIX=

$(ALL_DESC):
	$(MAKEIT) -f $(DESC_MAK) $(RULE) DESC=$@ 

$(ALL_RAW) $(ALL_NATIVE_DRIVERS):
	$(MAKEIT) -f $(USRPROG_MAK) $(RULE) \
		COMMAKE=$(QNX_SRC_DIR)/$@ \
		DEBUG=$(DEBUG) COMP_PREFIX=men_ LLDRV=

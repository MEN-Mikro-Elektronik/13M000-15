# MDIS project makefile
# Generated by mdiswiz
# 1.8.016-Qnx-1

# Check QNX Develpoment Environment Version

# The directory where you have installed the MDIS system
# package for QNX, e.g. /MEN/QNX.
# Note: FORWARD SLASHES '/' needed.

MEN_QNX_DIR := c:/work/QNX

ifndef QNX_SRC_DIR
	QNX_SRC_DIR := $(MEN_QNX_DIR)/src

endif

# This directory is used to install the binaries for the
# target.
# e.g.: $(QNX_TARGET)/$(ARCH) if you want to install them
# directly on your QNX target tree.

QNX_TARGET_DIR := $(MEN_QNX_DIR)/qnx6/$(ARCH)

# Processor architecture to create binaries for

ARCH := ppcbe

# define the directory in which the executable binaries
# are to be installed
# Usually this is the target's /usr/bin
# or /usr/local/bin directory

BIN_INSTALL_DIR := $(QNX_TARGET_DIR)/usr/bin

# define the directory in which the libraries (static/shared)
# are to be installed
# Usually this is the target's /usr/local/lib/mdis directory

LIB_INSTALL_DIR := $(QNX_TARGET_DIR)/usr/lib/mdis

STATIC_LIB_INSTALL_DIR := $(LIB_INSTALL_DIR)

DESC_INSTALL_DIR := $(QNX_TARGET_DIR)/etc/mdis

# Define whether to build/use static or shared user state
# libraries. This affects only ALL_*_TOOLS and the libraries
# specified by ALL_USR_LIBS.

LIB_MODE := shared

# CPU has VMEbus yes or no

USE_VME_SERVER := yes

# Define whether to build and install the release (nodbg) or
# debug (dbg) versions of the modules. The debug version of the
# modules issue debug messages for troubleshooting. The message
# display command is sloginfo.

ALL_DBGS := dbg

ALL_DESC := system

ALL_LL_DRIVERS := \
	M059/DRIVER/COM/driver.mak \
	M078/DRIVER/COM/driver_sw.mak \
	\

ALL_BB_DRIVERS := \
	A12/DRIVER/COM/driver.mak \
	A201/DRIVER/COM/driver.mak \
	\

ALL_USR_LIBS := \

ALL_LL_TOOLS := \
	M059/EXAMPLE/M59_SIMP/COM/program.mak \
	M059/TOOLS/M59_SAMPLE/COM/program.mak \
	M078/EXAMPLE/M78_SIMP/COM/program.mak \
	M078/TEST/M78_ACTEST/COM/program.mak \
	M078/TEST/M78_TRIGTEST/COM/program.mak \
	M078/TEST/M78_MULTITRIG/COM/program.mak \
	M078/TOOLS/M78_EEPOT/COM/program.mak \
	M078/TOOLS/M78_PLL/COM/program.mak \
	\

ALL_COM_TOOLS := \
	MDIS_API/M_ERRSTR/COM/program.mak \
	MDIS_API/M_GETBLOCK/COM/program.mak \
	MDIS_API/M_GETSTAT/COM/program.mak \
	MDIS_API/M_GETSTAT_BLK/COM/program.mak \
	MDIS_API/M_MOD_ID/COM/program.mak \
	MDIS_API/M_OPEN/COM/program.mak \
	MDIS_API/M_READ/COM/program.mak \
	MDIS_API/M_REV_ID/COM/program.mak \
	MDIS_API/M_SETBLOCK/COM/program.mak \
	MDIS_API/M_SETSTAT/COM/program.mak \
	MDIS_API/M_SETSTAT_BLK/COM/program.mak \
	MDIS_API/M_WRITE/COM/program.mak \
	\

ALL_NATIVE_DRIVERS := \

ALL_NATIVE_LIBS := \

ALL_NATIVE_TOOLS := \

include $(MEN_QNX_DIR)/src/BUILD/MDIS/TPL/rules.mak


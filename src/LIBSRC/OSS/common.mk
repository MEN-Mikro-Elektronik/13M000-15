
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=

#===== NAME - name of the project (default - name of project directory).
NAME=men_oss

#===== CCFLAGS - add the flags to the C compiler command line. 
CCFLAGS+=-O0

#===== INSTALLDIR - Subdirectory where the executable or library is to be installed. 
INSTALLDIR=usr/lib

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
EXTRA_INCVPATH+=$(MEN_QNX_DIR)/src/INCLUDE/COM  \
	$(MEN_QNX_DIR)/src/INCLUDE/NATIVE

#===== EXTRA_LIBVPATH - a space-separated list of directories to search for library files.
EXTRA_LIBVPATH_ppcbe+=$(MEN_QNX_TARGET)/x86/usr/lib
EXTRA_LIBVPATH_x86+=$(MEN_QNX_TARGET)/x86/usr/lib

#===== POST_BUILD - extra steps to do after building the image.
define POST_BUILD_ppcbe
-@$(CP_HOST) $(BUILDNAME) $(MEN_QNX_TARGET)/ppcbe/usr/lib/
endef
define POST_BUILD_x86
-@$(CP_HOST) $(BUILDNAME) $(MEN_QNX_TARGET)/x86/usr/lib/
endef


include $(MKFILES_ROOT)/qmacros.mk
define PINFO
PINFO DESCRIPTION=MEN OSS
endef
ifndef QNX_INTERNAL
QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk


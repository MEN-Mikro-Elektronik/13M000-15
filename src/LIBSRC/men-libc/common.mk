
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

USEFILE=
EXCLUDE_OBJS=

INSTALLDIR=usr/lib

include $(MKFILES_ROOT)/qmacros.mk
define PINFO
PINFO DESCRIPTION=MEN libc
endef

include $(MKFILES_ROOT)/qtargets.mk


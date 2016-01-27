#***************************  M a k e f i l e  *******************************
#  
#         Author: kp
#          $Date: 2008/11/13 15:45:49 $
#      $Revision: 1.2 $
#  
#    Description: Make QNX MDIS descriptors
#                      
#       Switches: DESC			filename of descriptor (without extension)
#				  THIS_DIR		directory where this file is located
#				  MEN_QNX_DIR	absolute path to directory that contains the
#								MDIS4QNX tree
#
#		Rules:	  all			make descriptor
#			     
#				  		  
#-----------------------------------------------------------------------------
#   (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuernberg, Germany 
#*****************************************************************************

OBJOUTPUT_DIR := $(QNX_TARGET_DIR)/etc/mdis

vpath %.dsc $(THIS_DIR)

#----------------------------------------------------------------------
# Rules for level #0 of make
#
.PHONY: all clean buildmsg

all: buildmsg $(OBJOUTPUT_DIR)/$(DESC).bin

buildmsg:
	@$(ECHO) +
	@$(ECHO) ++++++++ Building descriptor $(DESC) +++++++++++

$(OBJOUTPUT_DIR)/$(DESC).bin: $(THIS_DIR)/$(DESC).dsc
	- @mkdir -p $(OBJOUTPUT_DIR)
	$(Q)$(_DESCGEN) $(DESCGEN_BYTEORDER) -r $< -o=$(OBJOUTPUT_DIR)


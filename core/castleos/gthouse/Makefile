
#
# Config
#

include config.mk

ifeq ($(CONFIG_USE_VERBOSE),1)
# Not silent. It prints the commands.
	Q =
else
# silent
	Q = @
endif


all:

	@echo "gthouse: all"

# libk and rtl
	$(Q)make -C libk/

# Top level routines
	$(Q)make -C sys/

# Kernel-side support for user interaction
	$(Q)make -C wink/


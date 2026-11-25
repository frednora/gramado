
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

# fs
	$(Q)make -C fs/

# Top level network support
	$(Q)make -C gramnet/

# libk and rtl
	$(Q)make -C libk/

# Network support
	$(Q)make -C net/

# Top level routines
	$(Q)make -C sys/

# Kernel-side support for user interaction
	$(Q)make -C wink/


clean-all:
	-rm fs/*.o
	-rm gramnet/*.o
	-rm libk/*.o
	-rm net/*.o
	-rm sys/*.o
	-rm wink/*.o



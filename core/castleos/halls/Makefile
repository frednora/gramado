# Compile all in halls/ folder


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

	@echo "halls: all"

# Devices (exec, per, req)
	$(Q)make -C dev/

# BUSes
	$(Q)make -C platform/

# Human-Computer Interfce devices
	$(Q)make -C hcidev/

clean-all:
	-rm dev/*.o
	-rm hcidev/*.o
	-rm platform/*.o

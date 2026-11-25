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

# Character devices.
# mainly HCI (Human-Computer Interface) devices
	$(Q)make -C chardev/

# Devices (exec, per, req)
	$(Q)make -C dev/

# BUSes
	$(Q)make -C platform/

clean-all:
	-rm chardev/*.o
	-rm dev/*.o
	-rm platform/*.o


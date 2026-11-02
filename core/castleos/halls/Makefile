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

# fs
	$(Q)make -C fs/

# BUSes
	$(Q)make -C platform/

# Devices (exec, per, req)
	$(Q)make -C dev/

# Human-Computer Interfce devices
	$(Q)make -C hcidev/

clean:
# ...

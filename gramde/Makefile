# Windowing System

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

# Commands
	$(Q)make -C cmds/

# Desktop Environment
	$(Q)make -C winsys/

clean:
#todo

clean-all:
#todo


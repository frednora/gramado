# Garden

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

	@echo "garden: all"

# Entries: 
#  + Boot time entry
#  + AP initialization entry
#  + Interrupt time entry
	$(Q)make -C entry/

	$(Q)make -C gramnet/

	$(Q)make -C net/

clean:
	-rm entry/*.o
clean-all:
	-rm entry/*.o
	-rm gramnet/*.o
	-rm net/*.o


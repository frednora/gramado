# It compiles all in dev/ folder

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

# Application interaction hall

# AIH: Application Interaction Hall
# -- app::cpu --------------------
# Interrupt origin: cpu
# Execution unit. (cpu, process, threads)

	$(Q)make -C exec/ke/

# AIH: Application Interaction Hall
# -- app::memory --------------------
# Interrupt origin: memory

	$(Q)make -C req/mm/

# AIH: Application Interaction Hall
# -- app::disk+net --------------------
# Interrupt origin: irq

	$(Q)make -C per/blkdev/
	$(Q)make -C per/netdev/
#	$(Q)make -C per/fs/
	$(Q)make -C per/

clean:
#todo

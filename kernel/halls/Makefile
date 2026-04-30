# Compile all in halls/ folder

all:

#
# platform/
#

	make -C platform/acpi/
	make -C platform/bus/

#
# dev/
#

# Application interaction hall

# AIH: Application Interaction Hall
# -- app::cpu --------------------
# Interrupt origin: cpu
# Execution unit. (cpu, process, threads)

	make -C dev/exec/ke/

# AIH: Application Interaction Hall
# -- app::memory --------------------
# Interrupt origin: memory

	make -C dev/req/mm/

# AIH: Application Interaction Hall
# -- app::disk --------------------
# Interrupt origin: irq

	make -C dev/per/blkdev/
	make -C dev/per/netdev/
	make -C dev/per/fs/
	make -C dev/per/net/
	make -C dev/per/

# UIH: User Interaction Hall
# -- user::io_event --------------------
# Interrupt origin: irq
# user interaction hall

	make -C dev/chardev/

# ------------------------------------

clean:
# ...

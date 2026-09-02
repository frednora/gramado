# It compiles all in dev/ folder

all:


# Application interaction hall

# AIH: Application Interaction Hall
# -- app::cpu --------------------
# Interrupt origin: cpu
# Execution unit. (cpu, process, threads)

	make -C exec/ke/

# AIH: Application Interaction Hall
# -- app::memory --------------------
# Interrupt origin: memory

	make -C req/mm/

# AIH: Application Interaction Hall
# -- app::disk+net --------------------
# Interrupt origin: irq

	make -C per/blkdev/
	make -C per/netdev/
#	make -C per/fs/
	make -C per/

clean:
#todo
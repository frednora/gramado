# Compile all in kres/ folder

all:


#
# platform
#

	make -C platform/acpi/
	make -C platform/bus/

#
# app halls
#

# -- cpu --------------------
    # Origin: cpu
	make -C apphalls/exec/ke/
# -- memory --------------------
    # Origin: memory
	make -C apphalls/req/mm/
# -- disk --------------------
    # Origin: irq
	make -C apphalls/per/blkdev/
	make -C apphalls/per/netdev/
	make -C apphalls/per/fs/
	make -C apphalls/per/net/
	make -C apphalls/per/

#
# user halls
#

# ------------------------------------
	make -C uhalls/chardev/

clean:
# ...

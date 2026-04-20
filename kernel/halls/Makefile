# Compile all in kres/ folder

all:


#
# platform
#

	make -C platform/acpi/
	make -C platform/bus/

#
# application interaction hall
#

# -- cpu --------------------
    # Origin: cpu
	make -C dev/exec/ke/
# -- memory --------------------
    # Origin: memory
	make -C dev/req/mm/
# -- disk --------------------
    # Origin: irq
	make -C dev/per/blkdev/
	make -C dev/per/netdev/
	make -C dev/per/fs/
	make -C dev/per/net/
	make -C dev/per/

#
# user interaction hall
#

	make -C dev/chardev/


# ------------------------------------

clean:
# ...

# Compile all in kres/ folder

all:


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
	make -C apphalls/per/bus/
	make -C apphalls/per/

#
# user halls
#

# ------------------------------------
	make -C uhalls/chardev/

clean:
# ...

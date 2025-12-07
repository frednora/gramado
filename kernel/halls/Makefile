# Compile all in kres/ folder

all:


# Origin: cpu
	make -C exec/ke/

# Origin: irq
	make -C per/blkdev/
	make -C per/chardev/
	make -C per/netdev/
	make -C per/fs/
	make -C per/net/
	make -C per/bus/
	make -C per/

# Origin: memory
	make -C req/mm/

# ...

clean:
# ...

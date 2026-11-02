

all:
	make -C winsys/
	make -C core/

winsys:
	make -C winsys/
core:
	make -C core/

tools:
	echo "Build tool"
run:
	cd core/ && ./run  


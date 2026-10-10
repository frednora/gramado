

all:
	make -C sun/
	make -C os/
sun:
	make -C sun/
os:
	make -C os/
tools:
	echo "Build tool"
run:
	cd os/ && ./run  



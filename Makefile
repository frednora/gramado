

all:
	make -C os/

os:
	make -C os/

tools:
	echo "Build tool"

run:
	cd os/ && ./run  

	

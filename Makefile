

all:
	make -C ead/
	make -C os/
ead:
	make -C ead/
os:
	make -C os/
tools:
	echo "Build tool"
run:
	cd os/ && ./run  



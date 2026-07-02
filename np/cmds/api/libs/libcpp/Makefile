
# Build the libcpp.
# Ring3 library
# History:
# 2024 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = libcpp

#todo
CPPFLAGS := \
	-Wall \
	-m64 \
	-nostdlib \
	-nostdinc \



.PHONY: all all-cpp finalize clean

all: all-cpp finalize clean
	@echo "Ok?"
all-cpp:
	g++ -c osdep/crt/crt0.cpp -I include/ -o crt0.o
	g++ -c libcpp.cpp         -I include/ -o libcpp.o 
# ...
finalize:
	-mkdir obj

	-cp crt0.o   obj/
	-cp libcpp.o obj/
clean:
	-rm *.o
clean-all:
	-rm *.o
	-rm obj/*.o

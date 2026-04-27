# Target: CPP00.BIN
# Makefile for a C++ program for Gramado OS.
# Created by Fred Nora.


LINKFLAGS = -m elf_x86_64 -T ../link.ld

CPPFLAGS = -m64 \
-ffreestanding \
-fno-builtin \
-fleading-underscore \
-nodefaultlibs    

all:
	g++ -c cpprt0.cpp $(CPPFLAGS) -o cpprt0.o 
	ld $(LINKFLAGS) -o CPP00.BIN cpprt0.o
	-mv CPP00.BIN bin/

clean:
	-rm *.o

clean-all:
	-rm *.o
	-rm *.BIN
	-rm bin/*.BIN


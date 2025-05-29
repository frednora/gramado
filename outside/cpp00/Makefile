
# This is a test.


all:
	g++ -m64 -ffreestanding -fno-builtin -fleading-underscore -nodefaultlibs -c cpprt0.cpp -o cpprt0.o 
	ld -m elf_x86_64 -T ../link.ld -o CPP00.BIN cpprt0.o
	-mv CPP00.BIN bin/

clean:
	-rm *.o

clean-all:
	-rm *.o
	-rm *.BIN
	-rm bin/*.BIN


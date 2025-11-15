# vga service
# Testing VGA interface using io library in ring3.
# 2019 - Created by Fred Nora.

linkTarget = VGAD.BIN

# See: https://linux.die.net/man/1/ld
# -s --strip-all:   Omit all symbol information from the output file.
# -x --discard-all: Delete all local symbols.
# -static: Do not link against shared libraries. 
# -M --print-map:   Print a link map to the standard output.
# LDFLAGS := -m elf_x86_64 -static
# LDFLAGS := -m elf_x86_64 -s -x -static  
linkFlags  = -m elf_x86_64 \
-s -x -static -T ../link.ld


VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = vga1

CFLAGS = -m64 \
	-std=gnu89 \
	-nodefaultlibs \
	-nostdinc \
	-nostdlib \
	-static \
	-fgnu89-inline \
	-ffreestanding \
	-fno-builtin \
	-fno-pie \
	-fleading-underscore \
	-fno-stack-protector \
	-s \
	-Werror=strict-prototypes    

# Libraries
LIBC     = ../../libs/rtl/include/
LIBCOBJ  = ../../libs/rtl/obj
LIBIO    = ../../libs/libio01/include/
LIBIOOBJ = ../../libs/libio01/obj

#Objects
myObjects = crt0.o main.o vgad.o rtl.o \
ctype.o stdio.o stdlib.o string.o \
unistd.o termios.o ioctl.o fcntl.o libio.o    


.PHONY: all all-c jackpot-link finalize clean

all: main.o all-c jackpot-link finalize clean
	@echo "Ok?"
main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -I $(LIBIO) -o main.o
all-c:

	gcc  -c vgad.c  $(CFLAGS) -I $(LIBC) -I $(LIBIO) -o vgad.o

	cp $(LIBCOBJ)/crt0.o   .
	cp $(LIBCOBJ)/rtl.o  .

	cp $(LIBIOOBJ)/libio.o  .

	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/unistd.o  .
	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .
	cp $(LIBCOBJ)/fcntl.o  .

jackpot-link:
	ld $(linkFlags) -o $(linkTarget) $(myObjects)    
finalize:
	cp $(linkTarget) ../../bin
clean:
	-rm *.o
	-rm $(linkTarget) 

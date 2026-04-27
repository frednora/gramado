
# WEBD.BIN
# Makefile to build the Gramado Web Server.

linkTarget = WEBD.BIN

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
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION = 
#NAME = WEBD

# Make variables (CC, etc...)
AS	= as
LD	= ld
CC	= gcc
AR	= ar
MAKE	= make
NASM	= nasm
OBJCOPY	= objcopy
OBJDUMP	= objdump
LEX	= flex
YACC	= bison
PERL	= perl
PYTHON	= python
PYTHON2	= python2
PYTHON3	= python3
RUBY	= ruby


# usermode cross compiler flags.
CFLAGS = -Wall
#CFLAGS += -O3
CFLAGS += -Wundef
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wno-uninitialized
CFLAGS += -Wno-format-security
CFLAGS += -Wno-trigraphs
CFLAGS += -Werror=strict-prototypes
CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -Werror=return-type
CFLAGS += -std=gnu89
CFLAGS += -m64
CFLAGS += -s
CFLAGS += -static
CFLAGS += -fcommon
CFLAGS += -fgnu89-inline
CFLAGS += -finline-functions
CFLAGS += -fshort-wchar
CFLAGS += -ffreestanding
CFLAGS += -fleading-underscore
CFLAGS += -fstrict-overflow
CFLAGS += -nostdlib
CFLAGS += -nodefaultlibs
CFLAGS += -nostdinc
CFLAGS += -fno-builtin
CFLAGS += -fno-stack-protector
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-PIE
CFLAGS += -fno-pie
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -fno-optimize-sibling-calls    

# Libraries
LIBC    = ../../libs/rtl/include/
LIBCOBJ = ../../libs/rtl/obj

CINCLUDES  = -I $(LIBC)  
CINCLUDES += -I .    

# Objects
myObjects = crt0.o main.o rtl.o \
ctype.o stdio.o stdlib.o string.o conio.o socket.o signal.o unistd.o \
termios.o ioctl.o fcntl.o    

.PHONY: all link clean 

all: main.o link clean

main.o: 
	gcc -c main.c $(CFLAGS) $(CINCLUDES) -o main.o 

# rtl/
	cp $(LIBCOBJ)/crt0.o .
	cp $(LIBCOBJ)/rtl.o .

	cp $(LIBCOBJ)/ctype.o .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/conio.o .
	cp $(LIBCOBJ)/unistd.o .
	cp $(LIBCOBJ)/socket.o .
	cp $(LIBCOBJ)/signal.o .
	cp $(LIBCOBJ)/termios.o .
	cp $(LIBCOBJ)/ioctl.o .
	cp $(LIBCOBJ)/fcntl.o .

link:
	$(LD) $(linkFlags) -o $(linkTarget) $(myObjects)
	cp $(linkTarget) ../../bin
clean:
	-rm *.o
	-rm $(linkTarget)	


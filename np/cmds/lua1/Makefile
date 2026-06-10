# goal:
# The purpose here is compiling lua 1.0 to run 
# as ring 3 64bit program for Gramado OS.
# ps: If this program was created in the 32bit era,
# we're gonna port it to 64bit.
# See:
# Lua 1.0 in https://lua.org/versions.html


VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = gramcnf

# usermode cross compiler flags.
CFLAGS = -Wall
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
#CFLAGS += -O3

LIBC    = ../alpha/libs/rtl/include/
LIBCOBJ = ../alpha/libs/rtl/obj

# Objects
myObjects = crt0.o rtl.o \
ctype.o stdlib.o stdio.o string.o unistd.o termios.o ioctl.o fcntl.o math.o fscanf.o \
hash.o inout.o lex_yy.o opcode.o table.o y_tab.o iolib.o mathlib.o strlib.o lua.o \
main.o    



.PHONY: all all-c gramcc-link finalize clean

all: main.o all-c gramcc-link finalize clean
	@echo "Ok?"

main.o:
	gcc  -c  main.c  -I $(LIBC) $(CFLAGS)  -o  main.o

all-c:

# from lua
	gcc  -c  hash.c    -I $(LIBC) $(CFLAGS) -o hash.o
	gcc  -c  inout.c   -I $(LIBC) $(CFLAGS) -o inout.o
	gcc  -c  lex_yy.c  -I $(LIBC) $(CFLAGS) -o lex_yy.o
	gcc  -c  opcode.c  -I $(LIBC) $(CFLAGS) -o opcode.o
	gcc  -c  table.c   -I $(LIBC) $(CFLAGS) -o table.o
	gcc  -c  y_tab.c   -I $(LIBC) $(CFLAGS) -o y_tab.o
	gcc  -c  iolib.c   -I $(LIBC) $(CFLAGS) -o iolib.o
	gcc  -c  mathlib.c -I $(LIBC) $(CFLAGS) -o mathlib.o
	gcc  -c  strlib.c  -I $(LIBC) $(CFLAGS) -o strlib.o
	gcc  -c  lua.c     -I $(LIBC) $(CFLAGS) -o lua.o
# ...


# rtl
	cp $(LIBCOBJ)/crt0.o    .
	cp $(LIBCOBJ)/rtl.o    .

	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o   .
	cp $(LIBCOBJ)/stdlib.o  .
	cp $(LIBCOBJ)/string.o  .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .
	cp $(LIBCOBJ)/termios.o .
	cp $(LIBCOBJ)/ioctl.o   .
	cp $(LIBCOBJ)/fcntl.o   .

#test
	cp $(LIBCOBJ)/math.o   .
	cp $(LIBCOBJ)/fscanf.o   .

gramcc-link:
	ld -m elf_x86_64 -T ../link.ld -o LUA1.BIN $(myObjects)    
finalize:
	cp LUA1.BIN  ../bin
clean:
	-rm *.o
	-rm LUA1.BIN 


# js:
# html/css/js Language Interpreter for Gramado OS.
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = js

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

LIBC    = ../api/libs/rtl/include/
LIBCOBJ = ../api/libs/rtl/obj

# Objects
myObjects = crt0.o rtl.o \
ctype.o stdlib.o stdio.o string.o unistd.o termios.o ioctl.o fcntl.o \
globals.o jslex.o jspar.o jsvm.o tree.o compiler.o \
main.o    


.PHONY: all all-c gramcnf-link finalize clean

all: main.o all-c gramcnf-link finalize clean
	@echo "Ok?"

main.o:
	gcc  -c  main.c  -I $(LIBC) $(CFLAGS)  -o  main.o
all-c:

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

# from gramc
	gcc  -c  globals.c   -I $(LIBC) $(CFLAGS) -o globals.o

# js/
	gcc  -c  js/jslex.c     -I $(LIBC) $(CFLAGS) -o jslex.o
	gcc  -c  js/jspar.c     -I $(LIBC) $(CFLAGS) -o jspar.o
	gcc  -c  js/jsvm.c      -I $(LIBC) $(CFLAGS) -o jsvm.o

	gcc  -c  tree.c      -I $(LIBC) $(CFLAGS) -o tree.o
	gcc  -c  compiler.c  -I $(LIBC) $(CFLAGS) -o compiler.o

# Generate the executable file
gramcnf-link:
	ld -m elf_x86_64 -T ../link.ld -o JS.BIN $(myObjects)    

finalize:
	cp JS.BIN  ../bin
clean:
	-rm *.o
	-rm JS.BIN 

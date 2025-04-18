#history:
#2019 - Created by Fred Nora.
#todo:
#fazer um makefile pra compilar o shell
#obs: #bugbug tem que arrumas as coisas aqui.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = 

linkFlags  = -m elf_x86_64 \
-s -x -static -T ../../link.ld

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

# API
LIBC      = ../../api/libs/rtl/include/
LIBCOBJ   = ../../api/libs/rtl/obj
LIBGWS    = ../../api/libs/libgws/include/
LIBGWSOBJ = ../../api/libs/libgws/obj

# Objects
myObjects = crt0.o main.o globals.o terminal.o rtl.o \
ctype.o stdio.o fcntl.o stdlib.o string.o \
unistd.o termios.o ioctl.o socket.o \
libgws.o   


.PHONY: all all-c hello-link finalize clean

all: main.o all-c hello-link finalize clean
	@echo "Ok?"
main.o:
	gcc -c main.c $(CFLAGS) -I $(LIBC) -I $(LIBGWS) -I include/ -o main.o
all-c:

	gcc -c globals.c  $(CFLAGS) -I $(LIBC) -I $(LIBGWS) -I include/ -o globals.o
	gcc -c terminal.c $(CFLAGS) -I $(LIBC) -I $(LIBGWS) -I include/ -o terminal.o

# Copy

	cp $(LIBCOBJ)/crt0.o   .
	cp $(LIBCOBJ)/rtl.o  .

	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/fcntl.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/unistd.o  .

	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .
	cp $(LIBCOBJ)/socket.o  .

# libgws
	cp $(LIBGWSOBJ)/libgws.o .

hello-link:
	ld $(linkFlags) -o TERMINAL.BIN $(myObjects)    
finalize:
	cp TERMINAL.BIN  ../../bin
clean:
	-rm *.o
	-rm TERMINAL.BIN 
	

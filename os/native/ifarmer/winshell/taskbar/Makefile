# Compile the taskbar application.
# This is the main program for the UI Shell.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = taskbar

linkTarget = TASKBAR.BIN  

linkFlags  = -m elf_x86_64 \
-s -x -static -T ../link.ld

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

# rtl (libc)
LIBC      = ../api/garden/rtl/include/
LIBCOBJ   = ../api/garden/rtl/obj

# libgws - For server-side services
LIBGWS    = ../api/gthouse/libgws/include/
LIBGWSOBJ = ../api/gthouse/libgws/obj

# libgui - for client-side services
LIBGUI    = ../api/gthouse/libgui/include/
LIBGUIOBJ = ../api/gthouse/libgui/obj

# Objects
myObjects = crt0.o rtl.o \
ctype.o stdio.o fcntl.o stdlib.o string.o unistd.o termios.o ioctl.o \
socket.o \
math.o \
libgws.o libgui.o \
main.o    


.PHONY: all all-c link_program finalize clean

all: main.o all-c link_program finalize clean
	@echo "Ok?"

main.o:
	gcc -c main.c $(CFLAGS) -I $(LIBC) -I $(LIBGWS) -I $(LIBGUI) -o main.o

all-c:

# rtl (libc)
	cp $(LIBCOBJ)/crt0.o  .
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
	cp $(LIBCOBJ)/math.o  .

# libgws
	cp $(LIBGWSOBJ)/libgws.o .

# libgui
	cp $(LIBGUIOBJ)/libgui.o .

link_program:
	ld $(linkFlags) -o $(linkTarget) $(myObjects)    

finalize:
	cp $(linkTarget)  ../bin
clean:
	-rm *.o
	-rm $(linkTarget)  



#browser.bin
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

linkFlags = -m elf_x86_64 \
-s -x -static -T ../link.ld

linkTarget = TEABOX.BIN  


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
# All we need for this projects is inside the project.
# NO external libraries.
LIBC      = 02engine/box/libs/rtl/include/
LIBCOBJ   = 02engine/box/libs/rtl/obj

LIBGWS    = 02engine/box/libs/libgws/include/
LIBGWSOBJ = 02engine/box/libs/libgws/obj

LIBDISP   = 02engine/box/libs/libdisp/include/
LIBGR     = 02engine/box/libs/libgr/include/
LIBGR3D   = 02engine/box/libs/libgr3d/include/


#LIBBOX = 02engine/box/bin/
LIBBOXOBJ = 02engine/box/bin

# Objects
myObjects = crt0.o main.o globals.o ui.o rtl.o \
ctype.o stdio.o fcntl.o stdlib.o string.o \
unistd.o termios.o ioctl.o socket.o \
libgws.o    

myObjects += demo01main.o
myObjects += engglob.o 
myObjects += libdisp.o
myObjects += libgr.o
myObjects += libgr3d.o
myObjects += archive_basics1.o
myObjects += archive_basics2.o
myObjects += archive_fx.o    
myObjects += archive_extra.o    
myObjects += wm.o   


myObjects += math.o   


.PHONY: all all-c hello-link finalize clean

all: main.o all-c hello-link finalize clean
	@echo "Ok?"
main.o:
	gcc -c main.c $(CFLAGS) \
-I $(LIBC) -I $(LIBGWS) -I $(LIBDISP) -I $(LIBGR) -I $(LIBGR3D) \
-I include/ -o main.o

all-c:

	gcc -c globals.c $(CFLAGS) \
-I $(LIBC) -I $(LIBGWS) -I $(LIBDISP) -I $(LIBGR) -I $(LIBGR3D) \
-I include/ -o globals.o

	gcc -c 00ui/ui.c  $(CFLAGS) \
-I $(LIBC) -I $(LIBGWS) -I $(LIBDISP) -I $(LIBGR) -I $(LIBGR3D) \
-I include/ -o ui.o

# imports
# We need to compile the libraries before getting them.

	make -C 02engine/box/libs/
	make -C 02engine/box/src/

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

# todo: 
# Import the rest of the libs from box/

	cp $(LIBBOXOBJ)/demo01main.o  .
	cp $(LIBBOXOBJ)/engglob.o  .
	cp $(LIBBOXOBJ)/libdisp.o  .
	cp $(LIBBOXOBJ)/libgr.o  .
	cp $(LIBBOXOBJ)/libgr3d.o  .
	cp $(LIBBOXOBJ)/archive_basics1.o  .
	cp $(LIBBOXOBJ)/archive_basics2.o  .
	cp $(LIBBOXOBJ)/archive_fx.o  .
	cp $(LIBBOXOBJ)/archive_extra.o  .
	cp $(LIBBOXOBJ)/wm.o  .

	cp $(LIBBOXOBJ)/math.o  .


hello-link:
	ld $(linkFlags) -o $(linkTarget) $(myObjects)    
finalize:
	cp $(linkTarget)  ../../bin/
clean:
	-rm *.o
	-rm $(linkTarget) 
clean-all:
	-rm *.o
	-rm $(linkTarget)
	-rm bin/*.BIN 

	-rm 02engine/box/bin/*.o
	-rm 02engine/box/libs/*.o
	-rm 02engine/box/src/*.o


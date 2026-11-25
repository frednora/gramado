# Build INIT.BIN
# This is the first program that runs in ring 3.
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = 

#
# Config
#

include config.mk

ifeq ($(CONFIG_USE_VERBOSE),1)
# Not silent. It prints the commands.
	Q =
else
# silent
	Q = @
endif


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
LIBC    = ../api/garden/rtl/include/
LIBCOBJ = ../api/garden/rtl/obj


# Objects
myObjects = initcrt0.o rtl.o \
ctype.o stdio.o fcntl.o stdlib.o string.o unistd.o termios.o ioctl.o \
socket.o signal.o \
initsrv.o main.o 


.PHONY: all all-c link_app finalize clean

all: main.o all-c link_app finalize clean
	@echo "Ok?"

main.o:

	$(Q)gcc  -c initcrt0.c  $(CFLAGS) -I $(LIBC) -o initcrt0.o
	$(Q)gcc  -c main.c      $(CFLAGS) -I $(LIBC) -o main.o

all-c:

	$(Q)gcc  -c initsrv/initsrv.c  $(CFLAGS) -I $(LIBC) -o initsrv.o

	@echo "Importing library ..."

# suspended	$(Q)cp $(LIBCOBJ)/crt0.o  .
	$(Q)cp $(LIBCOBJ)/rtl.o  .

	$(Q)cp $(LIBCOBJ)/ctype.o  .
	$(Q)cp $(LIBCOBJ)/stdio.o  .
	$(Q)cp $(LIBCOBJ)/fcntl.o  .
	$(Q)cp $(LIBCOBJ)/stdlib.o  .
	$(Q)cp $(LIBCOBJ)/string.o  .
	$(Q)cp $(LIBCOBJ)/unistd.o  .

	$(Q)cp $(LIBCOBJ)/termios.o  .
	$(Q)cp $(LIBCOBJ)/ioctl.o  .
	$(Q)cp $(LIBCOBJ)/socket.o  .

	$(Q)cp $(LIBCOBJ)/signal.o  .

link_app:
	@echo "Linking INIT.BIN ..."
	$(Q)ld -m elf_x86_64 -T ../api/link.ld -o INIT.BIN $(myObjects)    

finalize:
	$(Q)cp INIT.BIN  bin/

clean:
	-rm *.o
	-rm INIT.BIN 

clean-all:
	-rm *.o
	-rm bin/*.BIN
	-rm INIT.BIN 


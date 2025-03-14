# Build INIT.BIN
# This is the first program that runs in ring 3.
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = 

Q = @ 


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
LIBC    = ../libs/rtl/include/
LIBCOBJ = ../libs/rtl/obj

# Objects
myObjects = crt0.o main.o inittask.o rtl.o \
ctype.o stdio.o fcntl.o stdlib.o string.o unistd.o termios.o ioctl.o \
socket.o    

.PHONY: all all-c link_app finalize clean

all: main.o all-c link_app finalize clean
	@echo "Ok?"
main.o:
	gcc  -c main.c  $(CFLAGS) -I $(LIBC) -o main.o
all-c:
	gcc  -c inittask/inittask.c  $(CFLAGS) -I $(LIBC) -o inittask.o

	$(Q)cp $(LIBCOBJ)/crt0.o  .
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

link_app:
	ld -m elf_x86_64 -T ../link.ld -o INIT.BIN $(myObjects)    
finalize:
	cp INIT.BIN  ../bin/
clean:
	-rm *.o
	-rm INIT.BIN 
	

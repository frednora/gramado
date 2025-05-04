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
myObjects = initcrt0.o main.o inittask.o rtl.o \
ctype.o stdio.o fcntl.o stdlib.o string.o unistd.o termios.o ioctl.o \
socket.o    

.PHONY: all all-c link_app finalize clean

all: main.o all-c link_app finalize clean
	@echo "Ok?"
main.o:
	gcc  -c initcrt0.c  $(CFLAGS) -I $(LIBC) -o initcrt0.o
	gcc  -c main.c      $(CFLAGS) -I $(LIBC) -o main.o

all-c:

	gcc  -c inittask/inittask.c  $(CFLAGS) -I $(LIBC) -o inittask.o

	@echo "Importing library ..."

# suspended	$(Q)cp $(LIBCOBJ)/crt0.o  .
	@cp $(LIBCOBJ)/rtl.o  .

	@cp $(LIBCOBJ)/ctype.o  .
	@cp $(LIBCOBJ)/stdio.o  .
	@cp $(LIBCOBJ)/fcntl.o  .
	@cp $(LIBCOBJ)/stdlib.o  .
	@cp $(LIBCOBJ)/string.o  .
	@cp $(LIBCOBJ)/unistd.o  .

	@cp $(LIBCOBJ)/termios.o  .
	@cp $(LIBCOBJ)/ioctl.o  .
	@cp $(LIBCOBJ)/socket.o  .

link_app:
	@echo "Linking INIT.BIN ..."
	@ld -m elf_x86_64 -T ../link.ld -o INIT.BIN $(myObjects)    
finalize:
	@cp INIT.BIN  ../bin/
clean:
	-rm *.o
	-rm INIT.BIN 
	

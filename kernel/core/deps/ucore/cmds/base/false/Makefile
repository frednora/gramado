# false - Testing FALSE return value.
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = false

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
	
LIBC    = ../../alpha/libs/rtl/include/
LIBCOBJ = ../../alpha/libs/rtl/obj

# Objects
myObjects = crt0.o main.o \
ctype.o stdio.o fcntl.o stdlib.o string.o time.o \
unistd.o termios.o ioctl.o rtl.o    

.PHONY: all all-c false-link finalize clean

all: main.o all-c false-link finalize clean
	@echo "Ok?"
main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -o main.o
all-c:

	cp $(LIBCOBJ)/crt0.o  .
	cp $(LIBCOBJ)/rtl.o  .
	
	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/fcntl.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .

	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .

false-link:
	ld -m elf_x86_64 -T ../link.ld -o FALSE.BIN $(myObjects)    
finalize:
	cp FALSE.BIN  ../bin
clean:
	-rm *.o
	-rm FALSE.BIN 
	

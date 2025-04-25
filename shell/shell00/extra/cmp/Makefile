# cat for gde
#history:
#2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = cat

#todo:
#We need to simplify these flags

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
ctype.o stdio.o stdlib.o string.o time.o \
unistd.o termios.o ioctl.o fcntl.o rtl.o    


.PHONY: all all-c cat-link finalize clean

all: main.o all-c cat-link finalize clean
	@echo "Ok?"

main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -o main.o
all-c:

	cp $(LIBCOBJ)/crt0.o  .
	cp $(LIBCOBJ)/rtl.o  .
	
	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .

	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .
	cp $(LIBCOBJ)/fcntl.o  .

cat-link:
	ld -m elf_x86_64 -T ../link.ld -o CMP.BIN $(myObjects)    
finalize:
	cp CMP.BIN  ../bin
clean:
	-rm *.o
	-rm CMP.BIN 
	

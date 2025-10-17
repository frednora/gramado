
# init process.

VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION =
#NAME = Init


# Make variables (CC, etc...)
AS	= as
LD	= ld
CC	= gcc
AR	= ar
MAKE	= make
NASM	= nasm
OBJCOPY	= objcopy
OBJDUMP	= objdump
LEX	= flex
YACC	= bison
PERL	= perl
PYTHON	= python
PYTHON2	= python2
PYTHON3	= python3
RUBY	= ruby



#CFLAGS = -m32 \
#	--std=gnu89 \
#	-nodefaultlibs \
#	-nostdinc \
#	-nostdlib \
#	-static \
#	-fgnu89-inline \
#	-ffreestanding \
#	-fno-builtin \
#	-fno-pie \
#	-no-pie \
#	-fno-stack-protector \
#	-s


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


LIBC    =  ../lib/rtl/include/
LIBCOBJ =  ../lib/rtl/obj


#
# Objects
#

# myObjects = crt0.o main.o \
# ctype.o stdio.o stdlib.o string.o conio.o unistd.o \
# termios.o ioctl.o fcntl.o rtl.o    

myObjects = marginal.o main.o \
ctype.o stdio.o stdlib.o string.o conio.o unistd.o \
termios.o ioctl.o fcntl.o rtl.o    

myObjects2 = marginal2.o main2.o \
ctype.o stdio.o stdlib.o string.o conio.o unistd.o \
termios.o ioctl.o fcntl.o rtl.o    


.PHONY: all all-c shell-link finalize clean

all: crt0.o main.o all-c shell-link finalize clean
	@echo "Init Ok?"

head.o:
	# Nothing.

crt0.o:
#	gcc  -c  crt0.c -I $(LIBC) -I $(API) $(CFLAGS) -o crt0.o
	nasm marginal.asm  -f elf64 -o marginal.o 
	nasm marginal2.asm -f elf64 -o marginal2.o 
main.o:
	$(CC)  -c  ohboy.c  -I $(LIBC) $(CFLAGS) -o main.o
	$(CC)  -c  ohboy2.c -I $(LIBC) $(CFLAGS) -o main2.o
all-c:

# We need some api and libc objects.

	cp $(LIBCOBJ)/crt0.o .
	cp $(LIBCOBJ)/rtl.o .

	cp $(LIBCOBJ)/ctype.o .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/conio.o .
	cp $(LIBCOBJ)/unistd.o .

	cp $(LIBCOBJ)/termios.o .
	cp $(LIBCOBJ)/ioctl.o .
	cp $(LIBCOBJ)/fcntl.o .

	#cp $(APIOBJ)/api.o .

shell-link:
#	$(LD) -s -x -T applink.ld -o SM.BIN   $(myObjects)    
	$(LD) -s -x -T applink.ld -o SM2.BIN  $(myObjects2)  
finalize:
#	cp SM.BIN ../../bin/
clean:
	-rm *.o
#	-rm SM.BIN

 



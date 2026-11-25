
#history:
#2019 - Created by Fred Nora.

#testando a libc3
#tentando colocar os caracteres em stdout ao invés d epintá-los na tela.


VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = tascii

#todo:
#We need to simplify these flags

CFLAGS = -m32 \
	--std=gnu89 \
	-nodefaultlibs \
	-nostdinc \
	-nostdlib \
	-static \
	-fgnu89-inline \
	-ffreestanding \
	-fno-builtin \
	-fno-pie \
	-no-pie \
	-fno-stack-protector \
	-s

#	-fleading-underscore \
#	-fno-stack-protector \
#	-s


LIBC    = ../../lib/libc03/include/
LIBCOBJ = ../../lib/libc03/obj

API01   = ../../lib/api01/include/
APIOBJ  = ../../lib/api01/obj


	##
	## Objects
	##

myObjects = crt0.o main.o \
ctype.o \
stdio.o \
stdlib.o \
string.o \
time.o \
api.o \
unistd.o \
stubs.o    




.PHONY: all all-c tascii-link finalize clean

all: main.o all-c tascii-link finalize clean
	@echo "Ok?"

main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -I $(API01) -o main.o

all-c:
	cp $(APIOBJ)/api.o      .
	
	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .
	cp $(LIBCOBJ)/stubs.o  .
	cp $(LIBCOBJ)/crt0.o  .	

tascii-link:
	ld -m elf_i386 -T link.ld -o HELLO2.BIN $(myObjects) -Map app_map.s

finalize:
	cp HELLO2.BIN ../../bin

clean:
	-rm *.o
	-rm HELLO2.BIN 
	
	

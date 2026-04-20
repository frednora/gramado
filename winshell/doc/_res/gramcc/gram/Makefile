
#history:
#2019 - Created by Fred Nora.

#todo:
#Compilar o gram



VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = gramc

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



LIBC    = ../../../lib/libc03/include/
LIBCOBJ = ../../../lib/libc03/obj
API     = ../../../lib/api02/include/
APIOBJ  = ../../../lib/api02/obj


	##
	## Objects
	##

myObjects = crt0.o main.o \
ctype.o \
stdlib.o \
stdio.o \
string.o \
unistd.o \
api.o \
lexer.o \
parser.o \
tree.o \
stubs.o \
compiler.o    




.PHONY: all all-c gramcc-link finalize clean

all: main.o all-c gramcc-link finalize clean
	@echo "Ok?"

main.o:

	gcc  -c  main.c      -I $(LIBC) -I $(API) $(CFLAGS) -o main.o
	gcc  -c  lexer.c     -I $(LIBC) -I $(API) $(CFLAGS) -o lexer.o
	gcc  -c  parser.c    -I $(LIBC) -I $(API) $(CFLAGS) -o parser.o
	gcc  -c  tree.c      -I $(LIBC) -I $(API) $(CFLAGS) -o tree.o
	gcc  -c  compiler.c  -I $(LIBC) -I $(API) $(CFLAGS) -o compiler.o


all-c:

	cp $(LIBCOBJ)/crt0.o   .

	cp $(LIBCOBJ)/stubs.o   .
		
	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .
	
	cp $(APIOBJ)/api.o      .

gramcc-link:
	ld -m elf_i386 -T link.ld -o GRAM.BIN $(myObjects)   
	

finalize:
#	cp GRAM.BIN  ../../../bin

clean:
	-rm *.o
	-rm GRAM.BIN 
	
	

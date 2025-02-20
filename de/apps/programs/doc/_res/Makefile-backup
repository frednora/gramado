# gramcnf:
# It's a project based on gramc.
# The goal here is to create a small parser
# for a new language for configuration files.
# history:
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = gramcnf

CFLAGS = -m32 \
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
	

LIBC    = ../../../landlib/rtl/include/
LIBCOBJ = ../../../landlib/rtl/obj
API     = ../libcore/include/
APIOBJ  = ../libcore/obj


# Objects

myObjects = crt0.o main.o rtl.o \
ctype.o stdlib.o stdio.o string.o unistd.o termios.o ioctl.o fcntl.o \
api.o \
lexer.o parser.o tree.o compiler.o    




.PHONY: all all-c gramcc-link finalize clean

all: main.o all-c gramcc-link finalize clean
	@echo "Ok?"

main.o:

	gcc  -c  main.c  -I $(LIBC)  -I $(API)  $(CFLAGS)  -o  main.o


all-c:

	# rtl
	cp $(LIBCOBJ)/crt0.o    .
	cp $(LIBCOBJ)/rtl.o    .

	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o   .
	cp $(LIBCOBJ)/stdlib.o  .
	cp $(LIBCOBJ)/string.o  .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .
	cp $(LIBCOBJ)/termios.o .
	cp $(LIBCOBJ)/ioctl.o   .
	cp $(LIBCOBJ)/fcntl.o   .

	# from libgui
	cp $(APIOBJ)/api.o      .


	# from gramc
	gcc  -c  lexer.c     -I $(LIBC) -I $(API) $(CFLAGS) -o lexer.o
	gcc  -c  parser.c    -I $(LIBC) -I $(API) $(CFLAGS) -o parser.o
	gcc  -c  tree.c      -I $(LIBC) -I $(API) $(CFLAGS) -o tree.o
	gcc  -c  compiler.c  -I $(LIBC) -I $(API) $(CFLAGS) -o compiler.o


gramcc-link:
	ld -m elf_i386 -T ../link.ld -o GRAMCNF.BIN $(myObjects)  
	


finalize:
	cp GRAMCNF.BIN  ../bin

clean:
	-rm *.o
	-rm GRAMCNF.BIN 


	

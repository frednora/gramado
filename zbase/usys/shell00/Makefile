# shell2
# 2019 - Created by Fred Nora and Luciano Gonçalez.
# todo: Covert this code to 64bit.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = 

CFLAGS := \
	-Wall \
	-Wundef \
	-Wmissing-prototypes \
	-Wno-uninitialized \
	-Wno-format-security \
	-Wno-trigraphs \
	-Werror=strict-prototypes \
	-Werror=implicit-function-declaration \
	-Werror=implicit-int \
	-Werror=return-type \
	-std=gnu89 \
	-m64 \
	-s \
	-static \
	-fcommon \
	-fgnu89-inline \
	-finline-functions \
	-fshort-wchar \
	-ffreestanding \
	-fleading-underscore \
	-fstrict-overflow \
	-nostdlib \
	-nodefaultlibs \
	-nostdinc \
	-fno-builtin \
	-fno-stack-protector \
	-fno-strict-aliasing \
	-fno-PIE \
	-fno-pie \
	-fno-omit-frame-pointer \
	-fno-optimize-sibling-calls \
	-mno-red-zone \
	#-mno-mmx -mno-sse -mno-sse2   


# Libraries
LIBC     = ../../core00/libs/rtl/include/
LIBCOBJ  = ../../core00/libs/rtl/obj
LIBIO    = ../../core00/libs/libio01/include/
LIBIOOBJ = ../../core00/libs/libio01/obj

# Objects
myObjects = crt0.o \
main.o \
rtl.o \
shellui.o \
ctype.o stdio.o stdlib.o string.o conio.o unistd.o \
builtins.o \
desktop.o \
ioctl.o \
fcntl.o \
termios.o    

.PHONY: all all-c shell-link finalize clean

all: main.o all-c shell-link finalize clean
	@echo "Ok?"

#head.o:
#	nasm  head.s  -felf -o head.o

#nao compilar, pegar compilado. tem que ajustar os argumentos recebidos por main.
#crt0.o:
#	gcc  -c  crt0.c  $(CFLAGS) -I $(LIBC) -I $(API) -o crt0.o

main.o:
	gcc  -c  main.c -I $(LIBC) $(CFLAGS) -o main.o
all-c:
	gcc  -c  shellui.c   -I $(LIBC) $(CFLAGS) -o shellui.o
	gcc  -c  builtins.c  -I $(LIBC) $(CFLAGS) -o builtins.o
	gcc  -c  desktop.c   -I $(LIBC) $(CFLAGS) -o desktop.o
	
#	gcc  -c  parser.c    -I ../../../lib/libc01/include/ -I ../../../lib/api01/include/ $(CFLAGS) -o parser.o
#	gcc  -c  lexer.c     -I ../../../lib/libc01/include/ -I ../../../lib/api01/include/ $(CFLAGS) -o lexer.o
#	gcc  -c  net.c       -I ../../../lib/libc01/include/ -I ../../../lib/api01/include/ $(CFLAGS) -o net.o
#	gcc  -c  terminal.c  -I ../../../lib/libc01/include/ -I ../../../lib/api01/include/ $(CFLAGS) -o terminal.o

	cp $(LIBCOBJ)/crt0.o   .
	cp $(LIBCOBJ)/rtl.o  .

	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/conio.o .
	cp $(LIBCOBJ)/wait.o .
	cp $(LIBCOBJ)/unistd.o .
#	cp $(LIBCOBJ)/stubs.o .

	cp $(LIBCOBJ)/ioctl.o  .
	cp $(LIBCOBJ)/fcntl.o  .
	cp $(LIBCOBJ)/termios.o  .


shell-link:
	ld -m elf_x86_64 -T ../../link.ld -o SHELL00.BIN $(myObjects)    
finalize:
	cp SHELL00.BIN ../../bin/
clean:
	-rm *.o
	-rm SHELL00.BIN 



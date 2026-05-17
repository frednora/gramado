
#history:
#2019 - Created by Fred Nora.

#todo:
#Compilar o gramas



VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = gramas

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
	-fleading-underscore \
	-fno-stack-protector \
	-s


	##
	## Objects
	##

myObjects = crt0.o \
main.o \
ctype.o \
stdlib.o \
stdio.o \
string.o \
unistd.o \
api.o \
lexer.o \
parser.o \
tree.o  



.PHONY: all all-c gramcc-link finalize clean

all: main.o all-c gramcc-link finalize clean
	@echo "Ok?"

main.o:
	nasm crt0.asm -f elf -o crt0.o
	gcc  -c  main.c    -I ../../lib/libc01/include/ -I ../../lib/api01/include/ $(CFLAGS) -o main.o
	gcc  -c  lexer.c   -I ../../lib/libc01/include/ -I ../../lib/api01/include/ $(CFLAGS) -o lexer.o
	gcc  -c  parser.c  -I ../../lib/libc01/include/ -I ../../lib/api01/include/ $(CFLAGS) -o parser.o
	gcc  -c  tree.c    -I ../../lib/libc01/include/ -I ../../lib/api01/include/ $(CFLAGS) -o tree.o
#	gcc  -c  compiler.c -I ../../../lib/libc01/include/ -I ../../../lib/api01/include/ $(CFLAGS) -o compiler.o


all-c:
	cp ../../lib/libc01/obj/ctype.o   .
	cp ../../lib/libc01/obj/stdio.o   .
	cp ../../lib/libc01/obj/stdlib.o  .	
	cp ../../lib/libc01/obj/string.o  .
	cp ../../lib/libc01/obj/time.o    .
	cp ../../lib/libc01/obj/unistd.o  .
	cp ../../lib/api01/obj/api.o      .
	


gramcc-link:
	ld -m i386pe -T link.ld -o GRAMAS.BIN $(myObjects) -Map app_map.s

finalize:
	cp GRAMAS.BIN  ../../../bin

clean:
	-rm *.o
	-rm GRAMAS.BIN 
	
	


#history:
#2019 - Created by Fred Nora.

#todo:
#Compilar o gramcc
#Será um wrapper para os outros aplicativos, como gramas, gramc
#



VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = gramcc

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

myObjects = main.o \
ctype.o \
stdio.o \
stdlib.o \
string.o \
time.o \
api.o \
unistd.o  



.PHONY: all all-c gramcc-link finalize clean

all: main.o all-c gramcc-link finalize clean
	@echo "Ok?"

main.o:
	gcc  -c  main.c -I ../../lib/libc01/include/ $(CFLAGS) -o main.o

all-c:
	cp ../../lib/libc01/obj/ctype.o   .
	cp ../../lib/libc01/obj/stdio.o   .
	cp ../../lib/libc01/obj/stdlib.o  .	
	cp ../../lib/libc01/obj/string.o  .
	cp ../../lib/libc01/obj/time.o    .
	
	#todo
	cp ../../lib/api01/obj/api.o      .
	
	cp ../../lib/libc01/obj/unistd.o  .
	


gramcc-link:
	ld -m i386pe -T link.ld -o GRAMCC.BIN $(myObjects) -Map app_map.s

finalize:
	cp GRAMCC.BIN  ../../../bin

clean:
	-rm *.o
	-rm GRAMCC.BIN 
	
	

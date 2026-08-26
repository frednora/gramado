# NETD.BIN
# NET Control (Server-side)

linkTarget = NETD.BIN

# See: https://linux.die.net/man/1/ld
# -s --strip-all:   Omit all symbol information from the output file.
# -x --discard-all: Delete all local symbols.
# -static: Do not link against shared libraries. 
# -M --print-map:   Print a link map to the standard output.
# LDFLAGS := -m elf_x86_64 -static
# LDFLAGS := -m elf_x86_64 -s -x -static  
linkFlags  = -m elf_x86_64 \
-s -x -static -T ../link.ld


VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION = 
#NAME = 

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
LIBC    = ../../libs/rtl/include/
LIBCOBJ = ../../libs/rtl/obj

CINCLUDES  = -I $(LIBC)  
CINCLUDES += -I .    

#RENDERER = renderer/include/  
#CMDLINE = cmdline/include/   
#TERMINAL  = terminal/include/   

# Objects
myObjects = crt0.o main.o rtl.o globals.o gramado.o \
ctype.o stdio.o stdlib.o string.o conio.o socket.o signal.o unistd.o \
connect.o decode.o client.o \
termios.o ioctl.o fcntl.o    

.PHONY: all all-c gws-link finalize clean

all: main.o all-c gws-link finalize clean
	@echo "Ok?"
main.o:
	gcc  -c  main.c  $(CFLAGS) $(CINCLUDES) -o main.o
all-c:
	gcc -c globals.c $(CFLAGS) $(CINCLUDES) -o globals.o
	gcc -c connect.c $(CFLAGS) $(CINCLUDES) -o connect.o
	gcc -c decode.c  $(CFLAGS) $(CINCLUDES) -o decode.o
	gcc -c client.c  $(CFLAGS) $(CINCLUDES) -o client.o

	gcc -c osdep/gramado/gramado.c $(CFLAGS) $(CINCLUDES) -o gramado.o  

# Grab the objects

	cp $(LIBCOBJ)/crt0.o .
	cp $(LIBCOBJ)/rtl.o .

	cp $(LIBCOBJ)/ctype.o .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/conio.o .
	cp $(LIBCOBJ)/unistd.o .
	cp $(LIBCOBJ)/socket.o .
	cp $(LIBCOBJ)/signal.o .
	cp $(LIBCOBJ)/termios.o .
	cp $(LIBCOBJ)/ioctl.o .
	cp $(LIBCOBJ)/fcntl.o .

gws-link:
	ld $(linkFlags) -o $(linkTarget) $(myObjects)    
finalize:
	cp $(linkTarget) ../../bin
clean:
	-rm *.o
	-rm $(linkTarget)	
	

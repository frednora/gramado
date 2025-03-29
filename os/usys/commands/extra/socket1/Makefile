
# socket1

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = 

# PROGRAM = S1

# usermode cross compiler flags.
CFLAGS = -Wall
#CFLAGS += -O3
CFLAGS += -Wundef
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wno-uninitialized
CFLAGS += -Wno-format-security
CFLAGS += -Wno-trigraphs
CFLAGS += -Werror=strict-prototypes
CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -Werror=return-type
CFLAGS += -std=gnu89
CFLAGS += -m64
CFLAGS += -s
CFLAGS += -static
CFLAGS += -fcommon
CFLAGS += -fgnu89-inline
CFLAGS += -finline-functions
CFLAGS += -fshort-wchar
CFLAGS += -ffreestanding
CFLAGS += -fleading-underscore
CFLAGS += -fstrict-overflow
CFLAGS += -nostdlib
CFLAGS += -nodefaultlibs
CFLAGS += -nostdinc
CFLAGS += -fno-builtin
CFLAGS += -fno-stack-protector
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-PIE
CFLAGS += -fno-pie
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -fno-optimize-sibling-calls    


LIBC    = ../../alpha/libs/rtl/include/
LIBCOBJ = ../../alpha/libs/rtl/obj

# Objects
myObjects = crt0.o rtl.o main.o \
ctype.o \
string.o \
stdio.o \
stdlib.o \
unistd.o \
fcntl.o \
termios.o \
ioctl.o \
socket.o    

.PHONY: all all-c hello-link finalize clean

all: main.o all-c hello-link finalize clean
	@echo "Done?"
main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -o main.o
all-c:

	cp $(LIBCOBJ)/crt0.o   .
	cp $(LIBCOBJ)/rtl.o  .

	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/unistd.o  .

	cp $(LIBCOBJ)/fcntl.o .
	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .
	cp $(LIBCOBJ)/socket.o  .

hello-link:
	ld -m elf_x86_64 -T ../link.ld -o S1.BIN $(myObjects)    
finalize:
	cp S1.BIN  ../bin
clean:
	-rm *.o
	-rm S1.BIN 
	

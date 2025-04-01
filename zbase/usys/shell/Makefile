# shell
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = shell

CFLAGS_OLD = -m64 \
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

CFLAGS_OLD2 := \
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
	-fno-optimize-sibling-calls    


CFLAGSzzz := -Wall -m64 -std=gnu89 -ffreestanding -mno-red-zone -mno-mmx -mno-sse \
-mno-sse2 -nostdlib   

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
	-mno-mmx -mno-sse -mno-sse2   
	

# Libraries
LIBC     = ../libs/rtl/include/
LIBCOBJ  = ../libs/rtl/obj
LIBIO    = ../libs/libio01/include/
LIBIOOBJ = ../libs/libio01/obj


# Objects
myObjects = crt0.o main.o \
ctype.o stdio.o stdlib.o string.o time.o \
unistd.o termios.o ioctl.o fcntl.o rtl.o libio.o    

.PHONY: all all-c cat-link finalize clean

all: main.o all-c cat-link finalize clean
	@echo "Ok?"
main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -I $(LIBIO) -o main.o
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

	cp $(LIBIOOBJ)/libio.o  .
	
cat-link:
	ld -m elf_x86_64 -T ../link.ld -o SHELL.BIN $(myObjects)    
finalize:
	cp SHELL.BIN ../bin/
clean:
	-rm *.o
	-rm SHELL.BIN 
	

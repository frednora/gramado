# winu
# Ring 3 library to communicate with wink in kernel-side.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = 0
NAME = winu

# Target operating system.

TARGETOS ?= gramado
#TARGETOS ?= linux

#STATIC_LIB = obj/lib/libc03.a
#SHARED_LIB = obj/lib/libc03.so

# Gramado!
ifeq ($(TARGETOS),gramado)

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

endif

LIB_RTL = -I.  -I ../rtl/include/

myObjects = winu.o    
#myObjects = connect.o rw.o \
#events.o gr.o gws.o    


.PHONY: all libc-header libc-content libc-footer link install clean
all: libc-header libc-content libc-footer \
link install clean    

	@echo "Ok?"

libc-header:

# crt0 - initialization
#	gcc -c  os/crt0/crt0.c  $(CFLAGS) -I. -I include/ -o crt0.o

libc-content:

# libc - Standard
#	gcc -c libc/assert.c  $(CFLAGS) -I. -I include/ -o assert.o


libc-footer:
# The higher part of the library.
# + rtl.c - The core toolkit for this library.
# todo: Actually we need to remove the syscalls(lowlevel) 
# from this file and put it in os/xxxsyscalls.c.
#	gcc -c  osrtl/rtl.c  $(CFLAGS) -I. -I include/ -o rtl.o

	gcc -c  winu.c  $(CFLAGS) $(LIB_RTL) -I. -I include/ -o winu.o

# create 'ar'
# #todo: 
# We need a new ld file.
link:
#	ar rc LIBC03.A $(OBJECTS)
#	ld -m elf_i386 socrt0.o somain.o -nostdlib -T elf_ia32_efi.lds -shared -znocombreloc -Bsymbolic -L . -l:LIBC03.A -o LIBC03.SO -Map map.s
	#$(LD) -r -o ar_winu.o 
install:
	-mkdir        obj/
	-cp *.o       obj/
#-cp LIBC03.A  obj/
#-cp LIBC03.SO obj/

clean:
	-rm *.o

clean-all:
	-rm *.o
	-rm *.A
	-rm *.SO
	-rm obj/*.o



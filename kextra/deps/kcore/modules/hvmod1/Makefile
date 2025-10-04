# ring0 application.
# #bugbug:
# We don't want syscalls from ring0.
# #todo:
# Check if crt0 initialization is making some ring0 syscall.

VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION = 
#NAME = 

linkTarget = HVMOD1.BIN  


# Make variables (CC, etc...)
AS	= as
LD	= ld
CC	= gcc
AR	= ar
MAKE	= make
NASM	= nasm
OBJCOPY	= objcopy
OBJDUMP	= objdump
LEX	= flex
YACC	= bison
PERL	= perl
PYTHON	= python
PYTHON2	= python2
PYTHON3	= python3
RUBY	= ruby


# usermode cross compiler flags.
user_ccflags = \
	-Wall \
	-Wundef \
	-Wmissing-prototypes \
	-Wno-uninitialized \
	-Wno-format-security \
	-Wno-trigraphs \
	-Werror=strict-prototypes \
	-Werror=implicit-function-declaration \
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
	

# usermode linker flags.
user_ldflags = -m elf_x86_64 -T x86_64/link_r0.ld

#todo
# This is a invalid pathname.
# We're gonna point this to a valid mod0.bin rtl.

#LIBC              = ../../../comtier/lib/rtl/include/
#LIBCOBJ           = ../../../comtier/lib/rtl/obj
#WS_INCLUDES       = include/ 
#WS_INCLUDES_LOCAL = .

INCLUDES = -I include 

# Objects
myObjects = head.o mmain.o globals.o \
kmemory.o kstring.o kstdio.o \
display.o ports64.o hal.o kal.o   


.PHONY: all all-c gws-link finalize clean

all: head.o ports64.o mmain.o all-c gws-link finalize clean
	@echo "Ok?"

head.o:
	$(CC)  -c x86_64/head.c $(user_ccflags) $(INCLUDES) -o head.o
ports64.o:
	$(CC)  -c ke/ports64.c  $(user_ccflags) $(INCLUDES) -o ports64.o
mmain.o:
	$(CC)  -c mmain.c       $(user_ccflags) $(INCLUDES) -o mmain.o

all-c:

	$(CC) -c crt/kmemory.c $(user_ccflags) $(INCLUDES) -o kmemory.o
	$(CC) -c crt/kstring.c $(user_ccflags) $(INCLUDES) -o kstring.o
	$(CC) -c crt/kstdio.c  $(user_ccflags) $(INCLUDES) -o kstdio.o

	$(CC)  -c  dev/display.c  $(user_ccflags) $(INCLUDES) -o display.o

	$(CC)  -c  ke/globals.c  $(user_ccflags) $(INCLUDES) -o globals.o
	$(CC)  -c  ke/hal.c      $(user_ccflags) $(INCLUDES) -o hal.o
	$(CC)  -c  ke/kal.c      $(user_ccflags) $(INCLUDES) -o kal.o

# == imported objects =============================

# rtl/

gws-link:
	$(LD) $(user_ldflags) -o $(linkTarget) $(myObjects)
#	$(LD) -m elf_i386 -T ../link.ld -o HVMOD0.BIN $(myObjects)  
finalize:
	-cp $(linkTarget) ../bin/
clean:
	-rm *.o
clean-all:
	-rm *.o
	-rm $(linkTarget)


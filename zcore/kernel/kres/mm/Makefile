# mm/
# Compile all the files and create ar_mm.o
# Makefile for the kernel memory manager.
# Created by Fred Nora.

INCLUDE = ../../include

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


# test: gramado 64bit
# See: https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html
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
	-fno-optimize-sibling-calls    

# Internal files
# Used to build the kernel.
KERNEL_INCLUDE := -I $(INCLUDE)

# See: https://linux.die.net/man/1/ld
# -s --strip-all:   Omit all symbol information from the output file.
# -x --discard-all: Delete all local symbols.
# -M --print-map:   Print a link map to the standard output.
# LDFLAGS := -m elf_x86_64 -static
LDFLAGS := -m elf_x86_64 -s -x -static  

OBJS = mmsize.o \
mmusage.o \
mm.o \
mminfo.o \
mmpool.o \
pages.o \
slab.o \
mmrt.o \
mmft.o    


all:
	$(CC) -c mmsize.c   $(KERNEL_INCLUDE) $(CFLAGS) -o mmsize.o
	$(CC) -c mmusage.c  $(KERNEL_INCLUDE) $(CFLAGS) -o mmusage.o
	$(CC) -c mm.c       $(KERNEL_INCLUDE) $(CFLAGS) -o mm.o
	$(CC) -c mminfo.c   $(KERNEL_INCLUDE) $(CFLAGS) -o mminfo.o
	$(CC) -c mmpool.c   $(KERNEL_INCLUDE) $(CFLAGS) -o mmpool.o
	$(CC) -c pages.c    $(KERNEL_INCLUDE) $(CFLAGS) -o pages.o
	$(CC) -c slab.c     $(KERNEL_INCLUDE) $(CFLAGS) -o slab.o
	$(CC) -c mmrt.c     $(KERNEL_INCLUDE) $(CFLAGS) -o mmrt.o
	$(CC) -c mmft.c     $(KERNEL_INCLUDE) $(CFLAGS) -o mmft.o

# link
	$(LD) -r -o j4_mm.o $(OBJS)
	cp j4_mm.o ../../
	rm *.o

clean:
	rm *.o    
	
	

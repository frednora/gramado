
# clibs/
# Compile all the files and create ar_crt.o
# Makefile for the libc support.

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

# Everything is a file.
OBJS = kctype.o kstdio.o ksignal.o kstdlib.o kstring.o kunistd.o    

all:
	$(CC) -c kctype.c  $(KERNEL_INCLUDE) $(CFLAGS) -o kctype.o
	$(CC) -c kstdio.c  $(KERNEL_INCLUDE) $(CFLAGS) -o kstdio.o
	$(CC) -c ksignal.c $(KERNEL_INCLUDE) $(CFLAGS) -o ksignal.o
	$(CC) -c kstdlib.c $(KERNEL_INCLUDE) $(CFLAGS) -o kstdlib.o
	$(CC) -c kstring.c $(KERNEL_INCLUDE) $(CFLAGS) -o kstring.o
	$(CC) -c kunistd.c $(KERNEL_INCLUDE) $(CFLAGS) -o kunistd.o

# link
	$(LD) -r -o i1_clibs.o $(OBJS)
	cp i1_clibs.o ../../
	rm *.o

clean:
	rm *.o    
	
	
	

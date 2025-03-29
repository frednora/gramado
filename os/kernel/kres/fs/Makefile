
# fs/
# Compile all the files and create ar_fs.o

# Makefile for the kernel filesystems.

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
OBJS = fslib.o fsbp.o fsload.o fs.o \
elf.o pipe.o search.o \
fat16.o    

all:

	$(CC) -c fslib.c   $(KERNEL_INCLUDE) $(CFLAGS) -o fslib.o
	$(CC) -c fsbp.c    $(KERNEL_INCLUDE) $(CFLAGS) -o fsbp.o
	$(CC) -c fsload.c  $(KERNEL_INCLUDE) $(CFLAGS) -o fsload.o
	$(CC) -c fs.c      $(KERNEL_INCLUDE) $(CFLAGS) -o fs.o

	$(CC) -c elf.c     $(KERNEL_INCLUDE) $(CFLAGS) -o elf.o
	$(CC) -c pipe.c    $(KERNEL_INCLUDE) $(CFLAGS) -o pipe.o
	$(CC) -c search.c  $(KERNEL_INCLUDE) $(CFLAGS) -o search.o

# fat/
	$(CC) -c fat/fat16.c  $(KERNEL_INCLUDE) $(CFLAGS) -o fat16.o

# link
	$(LD) -r -o j2_fs.o $(OBJS)
	cp j2_fs.o ../../
	rm *.o

clean:
	rm *.o    
	
	

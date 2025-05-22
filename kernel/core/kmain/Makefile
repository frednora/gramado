# Compiling kmain/ folder.

INCLUDE = ../../include

# Internal files
# Used to build the kernel.
KERNEL_INCLUDE := -I $(INCLUDE)   

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

OBJS = globals.o bootblk.o \
debug.o \
info.o \
panic.o \
reboot.o \
request.o \
system.o \
hv.o \
wrappers.o \
sys.o \
sci.o \
mod.o \
public.o \
kmain.o    


all:

# globals
	$(CC) -c globals.c $(KERNEL_INCLUDE) $(CFLAGS) -o globals.o
	$(CC) -c bootblk/bootblk.c $(KERNEL_INCLUDE) $(CFLAGS) -o bootblk.o

# Overall routines.
	$(CC) -c debug.c    $(KERNEL_INCLUDE) $(CFLAGS) -o debug.o
	$(CC) -c info.c     $(KERNEL_INCLUDE) $(CFLAGS) -o info.o
	$(CC) -c panic.c    $(KERNEL_INCLUDE) $(CFLAGS) -o panic.o  
	$(CC) -c reboot.c   $(KERNEL_INCLUDE) $(CFLAGS) -o reboot.o
	$(CC) -c request.c  $(KERNEL_INCLUDE) $(CFLAGS) -o request.o
	$(CC) -c system.c   $(KERNEL_INCLUDE) $(CFLAGS) -o system.o

# Wrapper functions.
	$(CC) -c wrappers.c  $(KERNEL_INCLUDE) $(CFLAGS) -o wrappers.o

# Module manager.
	$(CC) -c mod/mod.c     $(KERNEL_INCLUDE) $(CFLAGS) -o mod.o
	$(CC) -c mod/public.c  $(KERNEL_INCLUDE) $(CFLAGS) -o public.o

# System calls support
	$(CC) -c sci/sys.c  $(KERNEL_INCLUDE) $(CFLAGS) -o sys.o
	$(CC) -c sci/sci.c  $(KERNEL_INCLUDE) $(CFLAGS) -o sci.o    

# Virtualization routines.
	$(CC) -c virt/hv.c  $(KERNEL_INCLUDE) $(CFLAGS) -o hv.o

# kmain
	$(CC) -c kmain.c   $(KERNEL_INCLUDE) $(CFLAGS) -o kmain.o

# ----------------------------------------------
# link
	$(LD) -r -o p1_kmain.o $(OBJS)
	cp p1_kmain.o ../../

# ----------------------------------------------
	rm *.o
	
clean:
	rm *.o     
	
	

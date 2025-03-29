
# virt/
# Compile all the files and create ar_zcall.o

# Makefile for the kernel syscall support
# and the toplevel functions.
# These are the wrappers that call the workers.

INCLUDE = ../../include

# Internal files
# Used to build the kernel.
KERNEL_INCLUDE := -I $(INCLUDE)   
STARTUP_INCLUDE := -I x86_64/startup/  

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

OBJS = globals.o \
debug.o \
info.o \
panic.o \
reboot.o \
request.o \
system.o \
hv.o \
cali.o \
zero.o \
sys.o \
sci.o \
mod.o \
public.o \
kmain.o    


all:

# Module manager.
	$(CC) -c mod/mod.c     $(KERNEL_INCLUDE) $(CFLAGS) -o mod.o
	$(CC) -c mod/public.c  $(KERNEL_INCLUDE) $(CFLAGS) -o public.o

# Overall routines.
	$(CC) -c overall/debug.c    $(KERNEL_INCLUDE) $(CFLAGS) -o debug.o
	$(CC) -c overall/info.c     $(KERNEL_INCLUDE) $(CFLAGS) -o info.o
	$(CC) -c overall/panic.c    $(KERNEL_INCLUDE) $(CFLAGS) -o panic.o  
	$(CC) -c overall/reboot.c   $(KERNEL_INCLUDE) $(CFLAGS) -o reboot.o
	$(CC) -c overall/request.c  $(KERNEL_INCLUDE) $(CFLAGS) -o request.o
	$(CC) -c overall/system.c   $(KERNEL_INCLUDE) $(CFLAGS) -o system.o
	$(CC) -c overall/zero.c     $(KERNEL_INCLUDE) $(CFLAGS) -o zero.o
	$(CC) -c overall/cali.c     $(KERNEL_INCLUDE) $(CFLAGS) -o cali.o

# System calls support
	$(CC) -c sci/sys.c  $(KERNEL_INCLUDE) $(CFLAGS) -o sys.o
	$(CC) -c sci/sci.c  $(KERNEL_INCLUDE) $(CFLAGS) -o sci.o    

# Virtualization routines.
	$(CC) -c virt/hv.c  $(KERNEL_INCLUDE) $(CFLAGS) -o hv.o

# kmain
	$(CC) -c kmain.c   $(KERNEL_INCLUDE) $(CFLAGS) -o kmain.o
	$(CC) -c globals.c $(KERNEL_INCLUDE) $(CFLAGS) -o globals.o

# ----------------------------------------------
# link
	$(LD) -r -o i3_kmain.o $(OBJS)
	cp i3_kmain.o ../../

# ----------------------------------------------
# startup
#	$(NASM) x86_64/startup/head_64.asm \
#	$(KERNEL_INCLUDE) \
#	$(STARTUP_INCLUDE) \
#	-f elf64 -o head_64.o    

#	cp head_64.o ../../

# ----------------------------------------------
	rm *.o
	
clean:
	rm *.o     
	
	

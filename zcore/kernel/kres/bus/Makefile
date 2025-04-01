
# bus/
# Compile all the files and create j0_bus.o

# Makefile for the kernel device drivers.

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
OBJS = pci.o pciinfo.o pciscan.o   

all:

# pci
	$(CC) -c pci/pci.c      $(KERNEL_INCLUDE) $(CFLAGS) -o pci.o
	$(CC) -c pci/pciinfo.c  $(KERNEL_INCLUDE) $(CFLAGS) -o pciinfo.o
	$(CC) -c pci/pciscan.c  $(KERNEL_INCLUDE) $(CFLAGS) -o pciscan.o

# usb
#	$(CC) -c usb/usb.c        $(KERNEL_INCLUDE) $(CFLAGS) -o usb.o
#	$(CC) -c usb/ehci/ehci.c  $(KERNEL_INCLUDE) $(CFLAGS) -o ehci.o
#	$(CC) -c usb/uhci/ehci.c  $(KERNEL_INCLUDE) $(CFLAGS) -o uhci.o
#	$(CC) -c usb/xhci/ehci.c  $(KERNEL_INCLUDE) $(CFLAGS) -o xhci.o

# ...

# link
	$(LD) -r -o j0_bus.o $(OBJS)
	cp j0_bus.o ../../
	rm *.o

clean:
	rm *.o    


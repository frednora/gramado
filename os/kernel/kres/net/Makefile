
# net/
# Compile all the files and create ar_net.o
# Makefile for the kernel networking.

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

OBJS = network.o domain.o inet.o \
socket.o sockint.o \
ethernet.o arp.o ipv4.o tcp.o udp.o dhcp.o    

all:

# network
	$(CC) -c network.c  $(KERNEL_INCLUDE) $(CFLAGS) -o network.o
	$(CC) -c domain.c   $(KERNEL_INCLUDE) $(CFLAGS) -o domain.o
	$(CC) -c inet.c     $(KERNEL_INCLUDE) $(CFLAGS) -o inet.o

# socket
	$(CC) -c socket.c   $(KERNEL_INCLUDE) $(CFLAGS) -o socket.o
	$(CC) -c sockint.c  $(KERNEL_INCLUDE) $(CFLAGS) -o sockint.o

# core protocols
	$(CC) -c prot/core/ethernet.c  \
$(KERNEL_INCLUDE) $(CFLAGS) -o ethernet.o
	$(CC) -c prot/core/arp.c  \
$(KERNEL_INCLUDE) $(CFLAGS) -o arp.o
	$(CC) -c prot/core/ipv4.c  \
$(KERNEL_INCLUDE) $(CFLAGS) -o ipv4.o

# protocols
	$(CC) -c prot/tcp.c   $(KERNEL_INCLUDE) $(CFLAGS) -o tcp.o
	$(CC) -c prot/udp.c   $(KERNEL_INCLUDE) $(CFLAGS) -o udp.o
	$(CC) -c prot/dhcp.c  $(KERNEL_INCLUDE) $(CFLAGS) -o dhcp.o

# link
	$(LD) -r -o j5_net.o $(OBJS)
	cp j5_net.o ../../
	rm *.o

clean:
	rm *.o    
	
	


# net/
# Compile all the files and create ex_net.o
# Makefile for the kernel networking.


#
# Config
#

include config.mk

ifeq ($(CONFIG_USE_VERBOSE),1)
# Not silent. It prints the commands.
	Q =
else
# silent
	Q = @
endif


INCLUDE = ../../include

AS	= as
LD	= ld
CC	= gcc
AR	= ar
MAKE	= make
NASM	= nasm
OBJCOPY	= objcopy
OBJDUMP	= objdump
#LEX	= flex
#YACC	= bison
#PERL	= perl
PYTHON	= python
PYTHON2	= python2
PYTHON3	= python3
#RUBY	= ruby


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

OBJS = domain.o inet.o \
socket.o sockint.o \
netif.o \
ethernet.o firewall.o arp.o ipv4.o tcp.o udp.o dhcp.o \
net.o    


all:

	$(Q)$(CC) -c net.c  $(KERNEL_INCLUDE) $(CFLAGS) -o net.o

# network
	$(Q)$(CC) -c domain.c   $(KERNEL_INCLUDE) $(CFLAGS) -o domain.o
	$(Q)$(CC) -c inet.c     $(KERNEL_INCLUDE) $(CFLAGS) -o inet.o

# socket
	$(Q)$(CC) -c socket.c   $(KERNEL_INCLUDE) $(CFLAGS) -o socket.o
	$(Q)$(CC) -c sockint.c  $(KERNEL_INCLUDE) $(CFLAGS) -o sockint.o

# Network inteface support
	$(Q)$(CC) -c ifconfig/netif.c  $(KERNEL_INCLUDE) $(CFLAGS) -o netif.o

# ethernet
	$(Q)$(CC) -c ethernet/ethernet.c  $(KERNEL_INCLUDE) $(CFLAGS) -o ethernet.o

# firewall
	$(Q)$(CC) -c firewall/firewall.c  $(KERNEL_INCLUDE) $(CFLAGS) -o firewall.o

# ipv4
	$(Q)$(CC) -c ipv4/arp.c   $(KERNEL_INCLUDE) $(CFLAGS) -o arp.o
	$(Q)$(CC) -c ipv4/ipv4.c  $(KERNEL_INCLUDE) $(CFLAGS) -o ipv4.o
	$(Q)$(CC) -c ipv4/tcp.c   $(KERNEL_INCLUDE) $(CFLAGS) -o tcp.o
	$(Q)$(CC) -c ipv4/udp.c   $(KERNEL_INCLUDE) $(CFLAGS) -o udp.o
	$(Q)$(CC) -c ipv4/dhcp.c  $(KERNEL_INCLUDE) $(CFLAGS) -o dhcp.o

# link
	$(Q)$(LD) -r -o ex_net.o $(OBJS)

# Copy
	$(Q)cp ex_net.o ../../

# Clean
	$(Q)rm *.o

clean:
	$(Q)rm *.o    
	

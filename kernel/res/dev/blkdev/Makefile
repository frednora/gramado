# dev/
# Compile all the files and create ar_dev.o
# Makefile for the kernel device drivers.


INCLUDE = ../../../include

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
OBJS = \
ata.o atadma.o atahdd.o atainfo.o atairq.o atapci.o \
disk_r.o disk_w.o storage.o    


all:

# ATA
	$(CC) -c ata/ata.c     $(KERNEL_INCLUDE) $(CFLAGS) -o ata.o 
	$(CC) -c ata/atadma.c  $(KERNEL_INCLUDE) $(CFLAGS) -o atadma.o 
	$(CC) -c ata/atahdd.c  $(KERNEL_INCLUDE) $(CFLAGS) -o atahdd.o 
	$(CC) -c ata/atainfo.c $(KERNEL_INCLUDE) $(CFLAGS) -o atainfo.o 
	$(CC) -c ata/atairq.c  $(KERNEL_INCLUDE) $(CFLAGS) -o atairq.o 
	$(CC) -c ata/atapci.c  $(KERNEL_INCLUDE) $(CFLAGS) -o atapci.o 
	$(CC) -c ata/disk_r.c  $(KERNEL_INCLUDE) $(CFLAGS) -o disk_r.o
	$(CC) -c ata/disk_w.c  $(KERNEL_INCLUDE) $(CFLAGS) -o disk_w.o

# AHCI
#	$(CC) -c ahci/ahci.c  $(KERNEL_INCLUDE) $(CFLAGS) -o ahci.o 

# storage
	$(CC) -c storage.c $(KERNEL_INCLUDE) $(CFLAGS) -o storage.o 

# link
	$(LD) -r -o blkdev.o $(OBJS)
	cp blkdev.o  ../../../
	rm *.o

clean:
	rm *.o    


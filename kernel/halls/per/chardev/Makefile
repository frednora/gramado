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
bldisp.o display.o \
tty.o console.o pty.o \
i8042.o kbdmap.o ps2kbd.o ps2mouse.o keyboard.o mouse.o \
serial.o    

all:


# bl display device
	$(CC) -c display/bldisp/bldisp.c  $(KERNEL_INCLUDE) $(CFLAGS) -o bldisp.o
#$(CC) -c display/qemudisp/qemudisp.c  $(KERNEL_INCLUDE) $(CFLAGS) -o qemudisp.o
	$(CC) -c display/display.c  $(KERNEL_INCLUDE) $(CFLAGS) -o display.o

# Keyboard map.
	$(CC) -c kbdmap.c   $(KERNEL_INCLUDE) $(CFLAGS) -o kbdmap.o

# i8042 (PS/2)
	$(CC) -c i8042/i8042.c    $(KERNEL_INCLUDE) $(CFLAGS) -o i8042.o
	$(CC) -c i8042/keyboard.c $(KERNEL_INCLUDE) $(CFLAGS) -o keyboard.o
	$(CC) -c i8042/mouse.c    $(KERNEL_INCLUDE) $(CFLAGS) -o mouse.o
	$(CC) -c i8042/ps2kbd.c   $(KERNEL_INCLUDE) $(CFLAGS) -o ps2kbd.o
	$(CC) -c i8042/ps2mouse.c $(KERNEL_INCLUDE) $(CFLAGS) -o ps2mouse.o

# serial
	$(CC) -c serial/serial.c  $(KERNEL_INCLUDE) $(CFLAGS) -o serial.o
# tty
	$(CC) -c tty/tty.c      $(KERNEL_INCLUDE) $(CFLAGS) -o tty.o
	$(CC) -c tty/pty.c      $(KERNEL_INCLUDE) $(CFLAGS) -o pty.o

	$(CC) -c console/console.c  $(KERNEL_INCLUDE) $(CFLAGS) -o console.o 


# link
	$(LD) -r -o chardev.o $(OBJS)
	cp chardev.o  ../../../
	rm *.o

clean:
	rm *.o    



# gramk/
# Compile all the files and create ar_gramk.o

# Makefile for the user interaction support.
# Keyboard, mouse and display.

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
OBJS = pixel.o line.o rect.o bitblt.o bg.o font.o char.o text.o gre.o \
input.o dispsrv.o callback.o user.o \
gramk.o    

all:

# gre - Graphics Engine
	$(CC) -c gre/pixel.c   $(KERNEL_INCLUDE) $(CFLAGS) -o pixel.o
	$(CC) -c gre/line.c    $(KERNEL_INCLUDE) $(CFLAGS) -o line.o
	$(CC) -c gre/rect.c    $(KERNEL_INCLUDE) $(CFLAGS) -o rect.o
	$(CC) -c gre/bitblt.c  $(KERNEL_INCLUDE) $(CFLAGS) -o bitblt.o
	$(CC) -c gre/bg.c      $(KERNEL_INCLUDE) $(CFLAGS) -o bg.o
	$(CC) -c gre/font.c    $(KERNEL_INCLUDE) $(CFLAGS) -o font.o
	$(CC) -c gre/char.c    $(KERNEL_INCLUDE) $(CFLAGS) -o char.o
	$(CC) -c gre/text.c    $(KERNEL_INCLUDE) $(CFLAGS) -o text.o
	$(CC) -c gre/gre.c     $(KERNEL_INCLUDE) $(CFLAGS) -o gre.o

#
	$(CC) -c user/input.c     $(KERNEL_INCLUDE) $(CFLAGS) -o input.o
	$(CC) -c user/dispsrv.c   $(KERNEL_INCLUDE) $(CFLAGS) -o dispsrv.o
	$(CC) -c user/callback.c  $(KERNEL_INCLUDE) $(CFLAGS) -o callback.o
	$(CC) -c user/user.c      $(KERNEL_INCLUDE) $(CFLAGS) -o user.o

	$(CC) -c gramk.c     $(KERNEL_INCLUDE) $(CFLAGS) -o gramk.o

# link
	$(LD) -r -o j3_gramk.o $(OBJS)
	cp j3_gramk.o ../../
	rm *.o

clean:
	rm *.o


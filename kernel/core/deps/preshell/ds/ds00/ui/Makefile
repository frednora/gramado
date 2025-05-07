# ui/
# 2020 - Created by Fred Nora.

#linkTarget = GRAMLAND.BIN

# See: https://linux.die.net/man/1/ld
# -s --strip-all:   Omit all symbol information from the output file.
# -x --discard-all: Delete all local symbols.
# -static: Do not link against shared libraries. 
# -M --print-map:   Print a link map to the standard output.
# LDFLAGS := -m elf_x86_64 -static
# LDFLAGS := -m elf_x86_64 -s -x -static  

#linkFlags  = -m elf_x86_64 \
#-s -x -static -T ../../link.ld

#linkFlags  = -m elf_x86_64 \
#-s -x -static -T link.ld


VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION = 
#NAME = GWS

# Make variables (CC, etc...)
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


# usermode cross compiler flags.
CFLAGS = -Wall
#CFLAGS += -O3
CFLAGS += -Wundef
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wno-uninitialized
CFLAGS += -Wno-format-security
CFLAGS += -Wno-trigraphs
CFLAGS += -Werror=strict-prototypes
CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -Werror=return-type
CFLAGS += -std=gnu89
CFLAGS += -m64
CFLAGS += -s
CFLAGS += -static
CFLAGS += -fcommon
CFLAGS += -fgnu89-inline
CFLAGS += -finline-functions
CFLAGS += -fshort-wchar
CFLAGS += -ffreestanding
CFLAGS += -fleading-underscore
CFLAGS += -fstrict-overflow
CFLAGS += -nostdlib
CFLAGS += -nodefaultlibs
CFLAGS += -nostdinc
CFLAGS += -fno-builtin
CFLAGS += -fno-stack-protector
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-PIE
CFLAGS += -fno-pie
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -fno-optimize-sibling-calls    

#todo
#path longo. vamos mudar  as libs de lugar para gramado/libs.
LIBC        = ../../libs/rtl/include/
LIBCOBJ     = ../../libs/rtl/obj

LIBDISP     = ../../libs/libdisp/include/
LIBDISPOBJ  = ../../libs/libdisp/obj
LIBGR       = ../../libs/libgr/include/
LIBGROBJ    = ../../libs/libgr/obj

CINCLUDES  = -I $(LIBC)  
CINCLUDES += -I $(LIBDISP)
CINCLUDES += -I $(LIBGR)
CINCLUDES += -I .    
CINCLUDES += -I ../

# Objects
myObjects = wm.o \
wminput.o \
bmp.o \
menu.o \
fnt8x16.o \
font.o \
painter.o \
window.o \
bar.o wa.o swamp.o sprite.o demos.o gws.o   


.PHONY: all all-c gws-link finalize clean

all: main.o all-c gws-link finalize clean
	@echo "Ok?"
main.o:
#	$(CC)  -c grinput.c  $(CFLAGS) $(CINCLUDES) -o grinput.o
	$(CC)  -c wm.c    $(CFLAGS) $(CINCLUDES) -o wm.o
all-c:

	$(CC)  -c wminput.c    $(CFLAGS) $(CINCLUDES) -o wminput.o

	$(CC) -c bmp.c       $(CFLAGS) $(CINCLUDES) -o bmp.o

	$(CC)  -c menu.c     $(CFLAGS) $(CINCLUDES) -o menu.o

	$(CC)  -c fnt8x16.c  $(CFLAGS) $(CINCLUDES) -o fnt8x16.o
	$(CC)  -c font.c     $(CFLAGS) $(CINCLUDES) -o font.o

	$(CC)  -c painter.c  $(CFLAGS) $(CINCLUDES) -o painter.o
	$(CC)  -c window.c   $(CFLAGS) $(CINCLUDES) -o window.o

	$(CC)  -c bar.c    $(CFLAGS) $(CINCLUDES) -o bar.o
	$(CC)  -c wa.c     $(CFLAGS) $(CINCLUDES) -o wa.o
	$(CC)  -c swamp.c  $(CFLAGS) $(CINCLUDES) -o swamp.o

	$(CC) -c sprite.c    $(CFLAGS) $(CINCLUDES) -o sprite.o
	$(CC) -c demos.c     $(CFLAGS) $(CINCLUDES) -o demos.o
	$(CC) -c gws.c       $(CFLAGS) $(CINCLUDES) -o gws.o

gws-link:
#	$(LD) $(linkFlags) -o $(linkTarget) $(myObjects)
# link
	$(LD) -r -o ui.o $(myObjects)
	cp ui.o ../
	rm *.o

finalize:
#	cp $(linkTarget) ../bin/
#	cp $(linkTarget) bin/
clean:
	-rm *.o
#	-rm $(linkTarget)
	

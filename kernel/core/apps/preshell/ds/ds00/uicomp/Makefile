# comp/
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
myObjects = comp.o    


.PHONY: all all-c gws-link finalize clean

all: main.o all-c gws-link finalize clean
	@echo "Ok?"
main.o:
#	$(CC)  -c grinput.c  $(CFLAGS) $(CINCLUDES) -o grinput.o
all-c:
#	$(CC)  -c grinput.c  $(CFLAGS) $(CINCLUDES) -o grinput.o
	$(CC)  -c comp.c  $(CFLAGS) $(CINCLUDES) -o comp.o   

gws-link:
#	$(LD) $(linkFlags) -o $(linkTarget) $(myObjects)
# link
	$(LD) -r -o uicomp.o $(myObjects)
	cp uicomp.o ../
	rm *.o

finalize:
#	cp $(linkTarget) ../bin/
#	cp $(linkTarget) bin/
clean:
	-rm *.o
#	-rm $(linkTarget)
	

# demo01 - 3D game for Gramado OS.
# This is also a server.
# It's called by the init process.
# 2022 - Created by Fred Nora.

linkTarget = DEMO01.BIN

# See: https://linux.die.net/man/1/ld
# -s --strip-all:   Omit all symbol information from the output file.
# -x --discard-all: Delete all local symbols.
# -static: Do not link against shared libraries. 
# -M --print-map:   Print a link map to the standard output.
# Usermode
linkFlags = -m elf_x86_64 \
-s -x -static -T ../link.ld


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
user_ccflags = -Wall
#user_ccflags += -O3
user_ccflags += -Wundef
user_ccflags += -Wmissing-prototypes
user_ccflags += -Wno-uninitialized
user_ccflags += -Wno-format-security
user_ccflags += -Wno-trigraphs
user_ccflags += -Werror=strict-prototypes
user_ccflags += -Werror=implicit-function-declaration
user_ccflags += -Werror=return-type
user_ccflags += -std=gnu89
user_ccflags += -m64
user_ccflags += -s
user_ccflags += -static
user_ccflags += -fcommon
user_ccflags += -fgnu89-inline
user_ccflags += -finline-functions
user_ccflags += -fshort-wchar
user_ccflags += -ffreestanding
user_ccflags += -fleading-underscore
user_ccflags += -fstrict-overflow
user_ccflags += -nostdlib
user_ccflags += -nodefaultlibs
user_ccflags += -nostdinc
user_ccflags += -fno-builtin
user_ccflags += -fno-stack-protector
user_ccflags += -fno-strict-aliasing
user_ccflags += -fno-PIE
user_ccflags += -fno-pie
user_ccflags += -fno-omit-frame-pointer
user_ccflags += -fno-optimize-sibling-calls    


#bugbug: Long path
LIBC       = ../libs/rtl/include/
LIBCOBJ    = ../libs/rtl/obj

LIBDISP    = ../libs/libdisp/include/
LIBDISPOBJ = ../libs/libdisp/obj
LIBGR      = ../libs/libgr/include/
LIBGROBJ   = ../libs/libgr/obj
LIBGR3D    = ../libs/libgr3d/include/
LIBGR3DOBJ = ../libs/libgr3d/obj

CINCLUDES  = -I $(LIBC)  
CINCLUDES += -I $(LIBDISP)
CINCLUDES += -I $(LIBGR)
CINCLUDES += -I $(LIBGR3D)
CINCLUDES += -I .    

WS_INCLUDES       = include/ 
WS_INCLUDES_LOCAL = .

# Objects
myObjects = crt0.o main.o rtl.o \
ctype.o stdio.o stdlib.o string.o conio.o socket.o signal.o unistd.o \
termios.o ioctl.o fcntl.o    

myObjects += globals.o 
myObjects += libdisp.o
myObjects += libgr.o
myObjects += libgr3d.o

myObjects += archive_basics1.o
myObjects += archive_libui.o
myObjects += archive_fx.o    
myObjects += archive_extra.o    

#test
myObjects += scan00.o   

myObjects += math.o   

myObjects += comp.o   
myObjects += wm.o   
myObjects += screen.o   
myObjects += gws.o   
myObjects += connect.o   


.PHONY: all all-c gws-link finalize clean

all: main.o all-c gws-link finalize clean
	@echo "Ok?"

main.o:
	$(CC)  -c  main.c     $(user_ccflags) $(CINCLUDES) -o main.o
	$(CC)  -c  globals.c  $(user_ccflags) $(CINCLUDES) -o globals.o

all-c:

# basics1
	$(CC)  -c  lib00/char.c  $(user_ccflags) $(CINCLUDES) -o char.o
	$(CC)  -c  lib00/dtext.c $(user_ccflags) $(CINCLUDES) -o dtext.o
	$(LD) -r -o archive_basics1.o char.o dtext.o 

# libui
	$(CC)  -c  libui/line.c    $(user_ccflags) $(CINCLUDES) -o line.o
	$(CC)  -c  libui/rect.c    $(user_ccflags) $(CINCLUDES) -o rect.o
	$(CC)  -c  libui/window.c  $(user_ccflags) $(CINCLUDES) -o window.o
	$(CC)  -c  libui/button.c  $(user_ccflags) $(CINCLUDES) -o button.o
	$(CC)  -c  libui/bmp.c     $(user_ccflags) $(CINCLUDES) -o bmp.o
	$(LD) -r -o archive_libui.o \
line.o rect.o window.o button.o bmp.o    


#fx
	$(CC)  -c lib00/grprim.c  $(user_ccflags) $(CINCLUDES) -o grprim.o
	$(CC)  -c lib00/camera.c  $(user_ccflags) $(CINCLUDES) -o camera.o
	$(CC)  -c lib00/world.c   $(user_ccflags) $(CINCLUDES) -o world.o
	$(CC)  -c lib00/proj.c    $(user_ccflags) $(CINCLUDES) -o proj.o
	$(CC)  -c lib00/sprite.c  $(user_ccflags) $(CINCLUDES) -o sprite.o
	$(CC)  -c lib00/surface.c $(user_ccflags) $(CINCLUDES) -o surface.o

# Common for all the demos.
	$(CC)  -c demos/demos.c   $(user_ccflags) $(CINCLUDES) -o demos.o
	$(CC)  -c demos/models.c  $(user_ccflags) $(CINCLUDES) -o models.o
# Demos
	$(CC)  -c demos/flycube.c  $(user_ccflags) $(CINCLUDES) -o flycube.o
	$(CC)  -c demos/cat00.c    $(user_ccflags) $(CINCLUDES) -o cat00.o
	$(CC)  -c demos/polygon.c  $(user_ccflags) $(CINCLUDES) -o polygon.o
	$(CC)  -c demos/tri00.c    $(user_ccflags) $(CINCLUDES) -o tri00.o
	$(CC)  -c demos/lin00.c    $(user_ccflags) $(CINCLUDES) -o lin00.o
	$(CC)  -c demos/curve00.c  $(user_ccflags) $(CINCLUDES) -o curve00.o


	$(LD) -r -o archive_fx.o \
grprim.o camera.o world.o proj.o sprite.o surface.o \
demos.o models.o flycube.o cat00.o polygon.o tri00.o lin00.o curve00.o    

#extra
	$(CC)  -c  xxxinput.c  $(user_ccflags) $(CINCLUDES) -o xxxinput.o
	$(CC)  -c  font.c      $(user_ccflags) $(CINCLUDES) -o font.o
	$(LD) -r -o archive_extra.o  xxxinput.o font.o 

	$(CC)  -c demos/scan00.c   $(user_ccflags) $(CINCLUDES) -I include/  -o scan00.o

# ui/
	$(CC)  -c  ui/wm.c    $(user_ccflags) $(CINCLUDES) -I include/  -o wm.o
	$(CC)  -c  ui/comp.c  $(user_ccflags) $(CINCLUDES) -o comp.o

	$(CC)  -c  screen.c    $(user_ccflags) $(CINCLUDES) -o screen.o
	$(CC)  -c  gws.c       $(user_ccflags) $(CINCLUDES) -o gws.o
	$(CC)  -c  connect.c   $(user_ccflags) $(CINCLUDES) -o connect.o


#
# == imported objects =============================
#

	@echo "Importing library ..."

# rtl/
	@cp $(LIBCOBJ)/crt0.o .
	@cp $(LIBCOBJ)/rtl.o .

	@cp $(LIBCOBJ)/ctype.o .
	@cp $(LIBCOBJ)/stdio.o .
	@cp $(LIBCOBJ)/stdlib.o .
	@cp $(LIBCOBJ)/string.o .
	@cp $(LIBCOBJ)/conio.o .
	@cp $(LIBCOBJ)/unistd.o .
	@cp $(LIBCOBJ)/socket.o .
	@cp $(LIBCOBJ)/signal.o .
	@cp $(LIBCOBJ)/termios.o .
	@cp $(LIBCOBJ)/ioctl.o .
	@cp $(LIBCOBJ)/fcntl.o .
	@cp $(LIBCOBJ)/math.o .

#test
	@cp $(LIBDISPOBJ)/libdisp.o .
	@cp $(LIBGROBJ)/libgr.o .
	@cp $(LIBGR3DOBJ)/libgr3d.o .

gws-link:
	$(LD) $(linkFlags) -o $(linkTarget) $(myObjects)
	#$(LD) -r -o gramado.o $(myObjects)
finalize:
	cp $(linkTarget)  ../bin/
clean:
	-rm *.o
	-rm $(linkTarget) 
clean2:
	-rm *.o


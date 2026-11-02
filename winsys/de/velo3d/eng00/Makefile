# demo00 - 3D game for Gramado OS.
# This is also a server.
# It's called by the init process.
# 2022 - Created by Fred Nora.

#linkTarget = DEMO00.BIN
linkTarget = ENG00.BIN

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
#LEX	= flex
#YACC	= bison
#PERL	= perl
PYTHON	= python
PYTHON2	= python2
PYTHON3	= python3
#RUBY	= ruby


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
LIBN3D    = ../libs/libn3d/include/
LIBN3DOBJ = ../libs/libn3d/obj


CINCLUDES  = -I $(LIBC)  
CINCLUDES += -I $(LIBDISP)
CINCLUDES += -I $(LIBGR)
CINCLUDES += -I $(LIBN3D)
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
myObjects += libn3d.o 

myObjects += archive_basics1.o 
myObjects += archive_libui.o 
myObjects += archive_fx.o 
myObjects += archive_extra.o 

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
	$(Q)$(CC)  -c  main.c     $(user_ccflags) $(CINCLUDES) -o main.o
	$(Q)$(CC)  -c  globals.c  $(user_ccflags) $(CINCLUDES) -o globals.o

all-c:

# basics1
	$(Q)$(CC)  -c  lib00/char.c  $(user_ccflags) $(CINCLUDES) -o char.o
	$(Q)$(CC)  -c  lib00/dtext.c $(user_ccflags) $(CINCLUDES) -o dtext.o
	$(Q)$(LD) -r -o archive_basics1.o char.o dtext.o 

# libui
	$(Q)$(CC)  -c  libui/line.c    $(user_ccflags) $(CINCLUDES) -o line.o
	$(Q)$(CC)  -c  libui/rect.c    $(user_ccflags) $(CINCLUDES) -o rect.o
	$(Q)$(CC)  -c  libui/window.c  $(user_ccflags) $(CINCLUDES) -o window.o
	$(Q)$(CC)  -c  libui/button.c  $(user_ccflags) $(CINCLUDES) -o button.o
	$(Q)$(CC)  -c  libui/bmp.c     $(user_ccflags) $(CINCLUDES) -o bmp.o
	$(Q)$(LD) -r -o archive_libui.o \
line.o rect.o window.o button.o bmp.o    


#fx
	$(Q)$(CC)  -c lib00/grprim.c  $(user_ccflags) $(CINCLUDES) -o grprim.o
	$(Q)$(CC)  -c lib00/camera.c  $(user_ccflags) $(CINCLUDES) -o camera.o
	$(Q)$(CC)  -c lib00/world.c   $(user_ccflags) $(CINCLUDES) -o world.o
	$(Q)$(CC)  -c lib00/proj.c    $(user_ccflags) $(CINCLUDES) -o proj.o
	$(Q)$(CC)  -c lib00/sprite.c  $(user_ccflags) $(CINCLUDES) -o sprite.o
	$(Q)$(CC)  -c lib00/surface.c $(user_ccflags) $(CINCLUDES) -o surface.o

# Common for all the levels
	$(Q)$(CC)  -c levels/demos.c   $(user_ccflags) $(CINCLUDES) -o demos.o
	$(Q)$(CC)  -c levels/models.c  $(user_ccflags) $(CINCLUDES) -o models.o
# Levels
	$(Q)$(CC)  -c levels/intro00.c   $(user_ccflags) $(CINCLUDES) -o intro00.o
	$(Q)$(CC)  -c levels/hub.c       $(user_ccflags) $(CINCLUDES) -o hub.o
	$(Q)$(CC)  -c levels/cat00.c     $(user_ccflags) $(CINCLUDES) -o cat00.o
	$(Q)$(CC)  -c levels/polygon.c   $(user_ccflags) $(CINCLUDES) -o polygon.o
	$(Q)$(CC)  -c levels/tri00.c     $(user_ccflags) $(CINCLUDES) -o tri00.o
	$(Q)$(CC)  -c levels/lin00.c     $(user_ccflags) $(CINCLUDES) -o lin00.o
	$(Q)$(CC)  -c levels/scan00.c    $(user_ccflags) $(CINCLUDES) -o scan00.o

	$(Q)$(LD) -r -o archive_fx.o \
grprim.o camera.o world.o proj.o sprite.o surface.o \
demos.o models.o intro00.o hub.o cat00.o polygon.o tri00.o lin00.o scan00.o    

#extra
	$(Q)$(CC)  -c  xxxinput.c  $(user_ccflags) $(CINCLUDES) -o xxxinput.o
	$(Q)$(CC)  -c  font.c      $(user_ccflags) $(CINCLUDES) -o font.o
	$(Q)$(LD) -r -o archive_extra.o  xxxinput.o font.o 

# ui/
	$(Q)$(CC)  -c  ui/wm.c    $(user_ccflags) $(CINCLUDES) -I include/  -o wm.o
	$(Q)$(CC)  -c  ui/comp.c  $(user_ccflags) $(CINCLUDES) -o comp.o

	$(Q)$(CC)  -c  screen.c    $(user_ccflags) $(CINCLUDES) -o screen.o
	$(Q)$(CC)  -c  gws.c       $(user_ccflags) $(CINCLUDES) -o gws.o
	$(Q)$(CC)  -c  connect.c   $(user_ccflags) $(CINCLUDES) -o connect.o

#
# == imported objects =============================
#

	@echo "Importing library ..."

# rtl/
	$(Q)cp $(LIBCOBJ)/crt0.o .
	$(Q)cp $(LIBCOBJ)/rtl.o .

	$(Q)cp $(LIBCOBJ)/ctype.o .
	$(Q)cp $(LIBCOBJ)/stdio.o .
	$(Q)cp $(LIBCOBJ)/stdlib.o .
	$(Q)cp $(LIBCOBJ)/string.o .
	$(Q)cp $(LIBCOBJ)/conio.o .
	$(Q)cp $(LIBCOBJ)/unistd.o .
	$(Q)cp $(LIBCOBJ)/socket.o .
	$(Q)cp $(LIBCOBJ)/signal.o .
	$(Q)cp $(LIBCOBJ)/termios.o .
	$(Q)cp $(LIBCOBJ)/ioctl.o .
	$(Q)cp $(LIBCOBJ)/fcntl.o .
	$(Q)cp $(LIBCOBJ)/math.o .

#test
	$(Q)cp $(LIBDISPOBJ)/libdisp.o .
	$(Q)cp $(LIBGROBJ)/libgr.o .
	$(Q)cp $(LIBN3DOBJ)/libn3d.o .

gws-link:
	$(Q)$(LD) $(linkFlags) -o $(linkTarget) $(myObjects)

finalize:
	$(Q)cp $(linkTarget)  ../bin/

clean:
	-rm *.o
	-rm $(linkTarget) 

clean2:
	-rm *.o

clean-all:
	-rm *.o
	-rm $(linkTarget) 

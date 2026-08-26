# rtl
# + Ring3 syscalls to request services from cali kernel.
# + Ring3 standard libc functions.
# + Ring3 runtime helper functions.
# + Ring3 toolkits running over the standard libc. 
# The functions in this library are called by all the other
# libraries and toolkits in the folder libs/.
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = 0
NAME = rtl

# Target operating system.

TARGETOS ?= gramado
#TARGETOS ?= linux

#STATIC_LIB = obj/lib/libc03.a
#SHARED_LIB = obj/lib/libc03.so

# Gramado!
ifeq ($(TARGETOS),gramado)

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
	-fno-optimize-sibling-calls \
	-mno-red-zone \
	#-mno-mmx -mno-sse -mno-sse2   

endif

.PHONY: all libc-header libc-content libc-footer link install clean

all: libc-header libc-content libc-footer \
link install clean    

	@echo "Ok?"

libc-header:

	gcc -c  os/crt/crt0.c  $(CFLAGS) -I. -I include/ -o crt0.o

libc-content:

# libc - Standard
	gcc -c libc/assert.c  $(CFLAGS) -I. -I include/ -o assert.o
	gcc -c libc/ctype.c   $(CFLAGS) -I. -I include/ -o ctype.o
	gcc -c libc/fcntl.c   $(CFLAGS) -I. -I include/ -o fcntl.o
	gcc -c libc/fscanf.c  $(CFLAGS) -I. -I include/ -o fscanf.o
	gcc -c libc/inet.c    $(CFLAGS) -I. -I include/ -o inet.o
	gcc -c libc/ioctl.c   $(CFLAGS) -I. -I include/ -o ioctl.o
	gcc -c libc/locale.c  $(CFLAGS) -I. -I include/ -o locale.o
	gcc -c libc/math.c    $(CFLAGS) -I. -I include/ -o math.o
	gcc -c libc/signal.c  $(CFLAGS) -I. -I include/ -o signal.o
	gcc -c libc/socket.c  $(CFLAGS) -I. -I include/ -o socket.o
	gcc -c libc/stdio.c   $(CFLAGS) -I. -I include/ -o stdio.o
	gcc -c libc/stdlib.c  $(CFLAGS) -I. -I include/ -o stdlib.o
	gcc -c libc/string.c  $(CFLAGS) -I. -I include/ -o string.o
	gcc -c libc/strtol.c  $(CFLAGS) -I. -I include/ -o strtol.o
	gcc -c libc/strtoul.c  $(CFLAGS) -I. -I include/ -o strtoul.o
	gcc -c libc/termios.c  $(CFLAGS) -I. -I include/ -o termios.o
	gcc -c libc/time.c     $(CFLAGS) -I. -I include/ -o time.o
	gcc -c libc/unistd.c   $(CFLAGS) -I. -I include/ -o unistd.o
	gcc -c libc/wait.c     $(CFLAGS) -I. -I include/ -o wait.o

# os - OS dependent
	gcc  -c  os/conio.c     $(CFLAGS) -I. -I include/ -o conio.o
	gcc  -c  os/err.c       $(CFLAGS) -I. -I include/ -o err.o
	gcc  -c  os/linkaddr.c  $(CFLAGS) -I. -I include/ -o linkaddr.o
	gcc  -c  os/mmap.c      $(CFLAGS) -I. -I include/ -o mmap.o
	gcc  -c  os/pty.c       $(CFLAGS) -I. -I include/ -o pty.o
	gcc  -c  os/sched.c     $(CFLAGS) -I. -I include/ -o sched.o
	gcc  -c  os/_split.c    $(CFLAGS) -I. -I include/ -o _split.o
	gcc  -c  os/tty.c       $(CFLAGS) -I. -I include/ -o tty.o
# ...

libc-footer:
# The higher part of the library.
# + rtl.c - The core toolkit for this library.
# todo: Actually we need to remove the syscalls(lowlevel) 
# from this file and put it in os/xxxsyscalls.c.
	gcc -c  osrtl/rtl.c  $(CFLAGS) -I. -I include/ -o rtl.o

# create 'ar'
# #todo: 
# We need a new ld file.
link:
#	ar rc LIBC03.A $(OBJECTS)
#	ld -m elf_i386 socrt0.o somain.o -nostdlib -T elf_ia32_efi.lds -shared -znocombreloc -Bsymbolic -L . -l:LIBC03.A -o LIBC03.SO -Map map.s
	#$(LD) -r -o archive_rtl.o 
install:
	-mkdir        obj/
	-cp *.o       obj/
	#-cp LIBC03.A  obj/
	#-cp LIBC03.SO obj/
clean:
	-rm *.o
	-rm *.A
	-rm *.SO
clean2:
	-rm obj/*.o



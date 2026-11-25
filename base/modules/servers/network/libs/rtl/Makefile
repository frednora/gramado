# rtl
# libc for Gramado 64bit ring3 part.
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = 0
NAME = rtl

# Target operating system.

TARGETOS ?= gramado
#TARGETOS ?= linux


CC = gcc 

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

LIBS = -I . -I include/  


.PHONY: all libc-header libc-content libc-footer link install clean

all: libc-header libc-content libc-footer \
link install clean    

	@echo "Ok?"

libc-header:

ifeq ($(TARGETOS),gramado)

# student
	$(CC) -c entrance/student/crt0.c  $(CFLAGS) $(LIBS) -o crt0.o
# visitor
	$(CC) -c entrance/visitor/rtl.c  $(CFLAGS) $(LIBS) -o rtl.o

endif  


libc-content:

	$(CC) -c  _split.c    $(CFLAGS)  $(LIBS) -o _split.o
	$(CC) -c  assert.c    $(CFLAGS)  $(LIBS) -o assert.o
	$(CC) -c  conio.c    $(CFLAGS)  $(LIBS) -o conio.o
	$(CC) -c  ctype.c    $(CFLAGS)  $(LIBS) -o ctype.o
	$(CC) -c  err.c       $(CFLAGS)  $(LIBS) -o err.o
	$(CC) -c  fcntl.c    $(CFLAGS)  $(LIBS) -o fcntl.o
	$(CC) -c  fscanf.c   $(CFLAGS)  $(LIBS) -o fscanf.o
	$(CC) -c  inet.c  $(CFLAGS)  $(LIBS) -o inet.o
	$(CC) -c  ioctl.c    $(CFLAGS)  $(LIBS) -o ioctl.o
	$(CC) -c  linkaddr.c  $(CFLAGS)  $(LIBS) -o linkaddr.o
	$(CC) -c  locale.c    $(CFLAGS)  $(LIBS) -o locale.o
	$(CC) -c  math/math.c      $(CFLAGS)  $(LIBS) -o math.o
	$(CC) -c  mmap.c     $(CFLAGS)  $(LIBS) -o mmap.o
	$(CC) -c  pty.c       $(CFLAGS)  $(LIBS) -o pty.o
	$(CC) -c  sched.c  $(CFLAGS)  $(LIBS) -o sched.o
	$(CC) -c  signal.c   $(CFLAGS)  $(LIBS) -o signal.o
	$(CC) -c  socket.c   $(CFLAGS)  $(LIBS) -o socket.o
	$(CC) -c  stdio.c    $(CFLAGS)  $(LIBS) -o stdio.o
	$(CC) -c  stdlib.c   $(CFLAGS)  $(LIBS) -o stdlib.o
	$(CC) -c  string.c   $(CFLAGS)  $(LIBS) -o string.o
	$(CC) -c  strtol.c    $(CFLAGS)  $(LIBS) -o strtol.o
	$(CC) -c  strtoul.c   $(CFLAGS)  $(LIBS) -o strtoul.o
	$(CC) -c  termios.c  $(CFLAGS)  $(LIBS) -o termios.o
	$(CC) -c  time.c     $(CFLAGS)  $(LIBS) -o time.o
	$(CC) -c  tty.c       $(CFLAGS)  $(LIBS) -o tty.o
	$(CC) -c  unistd.c   $(CFLAGS)  $(LIBS) -o unistd.o
	$(CC) -c  wait.c     $(CFLAGS)  $(LIBS) -o wait.o

# ...

#tests 
#	$(CC) -c  tests/test1.c   $(CFLAGS) -I. -I include/ -o test1.o
 
# test
# entry point para a biblioteca compartilhada. .so

	#$(CC) -c  socrt0.c -fno-stack-protector -fpic -mno-red-zone -DEFI_FUNCTION_WRAPPER  $(CFLAGS)  $(LIBS) -o socrt0.o	
	#$(CC) -c  somain.c -fno-stack-protector -fpic -mno-red-zone -DEFI_FUNCTION_WRAPPER  $(CFLAGS)  $(LIBS) -o somain.o	

#ifeq ($(TARGETOS),gramado)
#	$(CC) -c  sysdeps/gramado/gramado.c  $(CFLAGS)  $(LIBS) -o gramado.o
#endif

#ifeq ($(TARGETOS),linux)
#	$(CC) -c  sysdeps/gramado/linux.c  $(CFLAGS)  $(LIBS) -o linux.o
#endif

libc-footer:
# Nothing for now!

# create 'ar'
# #todo: 
# We need a new ld file.
link:
#	ar rc LIBC03.A $(OBJECTS)
#	ld -m elf_i386 socrt0.o somain.o -nostdlib -T elf_ia32_efi.lds -shared -znocombreloc -Bsymbolic -L . -l:LIBC03.A -o LIBC03.SO -Map map.s
	#$(LD) -r -o ar_rtl.o 
install:
	-mkdir         obj/
	-cp *.o        obj/
	#-cp LIBC03.A  obj/
	#-cp LIBC03.SO obj/
clean:
	-rm *.o
	-rm *.A
	-rm *.SO
clean2:
	-rm obj/*.o



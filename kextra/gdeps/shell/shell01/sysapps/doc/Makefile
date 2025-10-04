# docviewer (browser)
# gramcnf:
# It's a project based on gramc.
# The goal here is to create a small parser
# for a new language for configuration files.
# history:
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = gramcnf

linkFlags  = -m elf_x86_64 \
-s -x -static -T ../../link.ld

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

# API
LIBC      = ../../api/libs/rtl/include/
LIBCOBJ   = ../../api/libs/rtl/obj
LIBGWS    = ../../api/libs/libgws/include/
LIBGWSOBJ = ../../api/libs/libgws/obj

# Objects
myObjects = crt0.o main.o rtl.o \
ctype.o stdlib.o stdio.o string.o unistd.o termios.o ioctl.o fcntl.o \
globals.o lexer.o parser.o tree.o compiler.o \
socket.o \
libgws.o    

.PHONY: all all-c gramcc-link finalize clean

all: main.o all-c gramcc-link finalize clean
	@echo "Ok?"
main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -I $(LIBGWS) -I include/ -o main.o
all-c:

	# rtl
	cp $(LIBCOBJ)/crt0.o    .
	cp $(LIBCOBJ)/rtl.o    .

	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o   .
	cp $(LIBCOBJ)/stdlib.o  .
	cp $(LIBCOBJ)/string.o  .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .
	cp $(LIBCOBJ)/fcntl.o   .

	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .
	cp $(LIBCOBJ)/socket.o  .

# libgws
	cp $(LIBGWSOBJ)/libgws.o .

# ----------------------
	gcc  -c  globals.c   -I $(LIBC) $(CFLAGS) -o globals.o
	gcc  -c  lexer.c     -I $(LIBC) $(CFLAGS) -o lexer.o
	gcc  -c  parser.c    -I $(LIBC) $(CFLAGS) -o parser.o
	gcc  -c  tree.c      -I $(LIBC) $(CFLAGS) -o tree.o
	gcc  -c  compiler.c  -I $(LIBC) $(CFLAGS) -o compiler.o

gramcc-link:
	ld $(linkFlags) -o DOC.BIN $(myObjects)    
finalize:
	cp DOC.BIN  ../../bin
clean:
	-rm *.o
	-rm DOC.BIN 

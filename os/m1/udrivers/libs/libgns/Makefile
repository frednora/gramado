
# libgns
# Client-side library for gns - gramado network server.
# 2020 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION = 
#NAME = libgns

CFLAGS = -m64 \
	-std=gnu89 \
	-nodefaultlibs \
	-nostdinc \
	-nostdlib \
	-static \
	-fgnu89-inline \
	-ffreestanding \
	-fno-builtin \
	-fno-pie \
	-fleading-underscore \
	-fno-stack-protector \
	-s \
	-Werror=strict-prototypes    

# rtl
LIBS = -I. -I ../rtl/include/    

.PHONY: all all-c finalize clean

all: all-c finalize clean
	@echo "Ok?"
all-c:
	gcc -c gns.c  $(CFLAGS) $(LIBS) -o gns.o
finalize:
	-mkdir obj/
	-cp *.o obj/
clean:
	-rm *.o
	


# danger: ring3 i/o library. hahaha
# History:
# 2019 - Created by Fred Nora.

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = libio


CFLAGS = -m64 \
	--std=gnu89 \
	-nodefaultlibs \
	-nostdinc \
	-nostdlib \
	-static \
	-fgnu89-inline \
	-ffreestanding \
	-fno-builtin \
	-fno-pie \
	-no-pie \
	-fleading-underscore \
	-fno-stack-protector \
	-s

.PHONY: all all-c finalize clean

all: all-c finalize clean
	@echo "Ok?"
all-c:
	gcc  -c  libio.c $(CFLAGS) -I include/ -o libio.o 
finalize:
	-mkdir obj
	-cp libio.o obj/
clean:
	-rm *.o



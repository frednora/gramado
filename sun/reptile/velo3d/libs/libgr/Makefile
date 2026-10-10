# Graphics support in ring3. 2D?
# 2020 - Created by Fred Nora.
# todo: Link all togheter in a single object file .o.


VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = libgws - Client-side library for gws window server.

#todo:
#We need to simplify these flags

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

LIBS = -I.  -I ../rtl/include/

.PHONY: all all-c finalize clean

all: all-c finalize clean
	@echo "Ok?"
all-c:
	gcc -c libgr.c  $(CFLAGS) $(LIBS) -o libgr.o
finalize:
	-mkdir obj/
	-cp *.o obj/
clean:
	-rm *.o
	
	


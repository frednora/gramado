
# libgui - Client-side library
# 2020 - Created by Fred Nora.
# todo: Link all togheter in a single object file .o.


VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = libgui - Client-side library for gui routines.

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

LIBS = -I.  -I ../../garden/rtl/include/

OBJS = font.o libgui00.o bmp.o    


.PHONY: all all-c finalize clean

all: all-c finalize clean
	@echo "Ok?"

all-c:
	gcc -c font.c    $(CFLAGS) $(LIBS) -o font.o
	gcc -c libgui.c  $(CFLAGS) $(LIBS) -o libgui00.o
	gcc -c bmp.c     $(CFLAGS) $(LIBS) -o bmp.o

# Link the binaries to form the library
	$(LD) -r -o libgui.o $(OBJS)

finalize:
	-mkdir obj/
	-cp *.o obj/
clean:
	-rm *.o
	
	


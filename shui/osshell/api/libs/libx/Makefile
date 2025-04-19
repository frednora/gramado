
# Client-side library for gws window server.
# 2020 - Created by Fred Nora.
# todo: Link all togheter in a single object file .o.

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 0
# Compiling on gcc 12.2.0
# Linking on ld 2.39

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

myObjects = connect.o rw.o \
events.o gr.o gws.o    

.PHONY: all all-c finalize clean
all: all-c finalize clean
	@echo "Ok?"
all-c:
	gcc -c connect.c  $(CFLAGS) $(LIBS) -o connect.o
	gcc -c rw.c       $(CFLAGS) $(LIBS) -o rw.o
	gcc -c events.c   $(CFLAGS) $(LIBS) -o events.o
	gcc -c gr.c       $(CFLAGS) $(LIBS) -o gr.o
	gcc -c gws.c      $(CFLAGS) $(LIBS) -o gws.o

finalize:
	$(LD) -r -o libx.o $(myObjects)
	-mkdir obj/
	-cp *.o obj/
clean:
	-rm *.o
clean-all:
	-rm *.o
	-rm obj/*.o



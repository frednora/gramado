

# test: gramado 64bit
# See: https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html
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
	-fno-optimize-sibling-calls    


# Garden
all:

# Entries: 
#  + Boot time entry
#  + AP initialization entry
#  + Interrupt time entry

	make -C bsp/

# kmain for AP processor
	$(CC) -c ap/apmain.c   -I ../include  $(CFLAGS) -o apmain.o

# kmain for BSP processor
	$(CC) -c bsp/kmain.c   -I ../include  $(CFLAGS) -o kmain.o

# copy
	mv apmain.o  ../
	mv kmain.o   ../

clean:
	-rm *.o



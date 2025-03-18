# Build all in udrivers/
# Drivers in ring 3.
# Created by Fred Nora.

all:

# The libs for ring3
	make -C libs/
#	make -C toolkits/

#	make -C vgad/client/
	make -C vgad/server/
# ...

clean:
	-rm -f *.o
	-rm -f bin/*.BIN

clean-all:
	-rm -f *.o
	-rm -f bin/*.BIN

	-rm -f libs/libgns/obj/*.o
	-rm -f libs/rtl/obj/*.o
#	-rm -f toolkits/*.o

# ...


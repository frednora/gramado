# Build all in uservers/
# Servers in ring 3.
# Created by Fred Nora.

all:

# The libs for ring3
	make -C libs/
#	make -C toolkits/

#	make -C ftpd/client/
#	make -C ftpd/server/

	make -C netd/client/
	make -C netd/server/

#	make -C telnet/client/
#	make -C telnet/server/

#	make -C webd/client/
#	make -C webd/server/

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


# System commands running in ring 3.
# important: We can find the init process in init/ folder.
# Created by Fred Nora.

all:

# Ring 3 drivers
	make -C drivers/
# The init process
	make -C init/
# Ring 3 servers
	make -C servers/

clean:
	-rm -f *.o
	-rm -f bin/*.BIN

clean-all:
	-rm -f *.o
	-rm -f bin/*.BIN

	-rm -f libs/libgns/obj/*.o
	-rm -f libs/rtl/obj/*.o

	-rm -f init/src/*.o

# Unix-like commands
#	#-rm cmds/base/bin/*.BIN

	# ...

# ...



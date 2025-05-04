# Build all in usys/
# System commands running in ring 3.
# important: We can find the init process in init/ folder.
# Created by Fred Nora.

all:

# The libs for ring3
	make -C libs/
#	make -C toolkits/

# The init process
	make -C init/

# Extra
	make -C drivers/
	make -C servers/


clean:
	-rm -f *.o
	-rm -f bin/*.BIN

clean-all:
	-rm -f *.o
	-rm -f bin/*.BIN

	-rm -f libs/libgns/obj/*.o
	-rm -f libs/rtl/obj/*.o

	-rm -f init/*.o

	# ...

# ...



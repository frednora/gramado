# Gramado compositor/display server.
# COMP00.BIN is the main display server for Gramado OS.
# Created by Fred Nora.

all:

# garden - rtl
	make -C garden/

# gthouse - libs
# Libraries
# Excluive for the display server.
	make -C gthouse/

# Display server/ Compositor
	make -C src/

clean:
#	-rm -f src/*.o
	-rm -f src/bin/*.BIN

clean-all:
	-rm -f src/*.o
	-rm -f src/bin/*.BIN

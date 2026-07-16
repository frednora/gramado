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

# Display servers
	make -C comp00/

# ps: Not time for more display servers.
#	make -C ds01/

clean:
#	-rm -f comp00/*.o
#	-rm -f ds0~1/*.o
	-rm -f comp00/bin/*.BIN
#	-rm -f ds01/bin/*.BIN

clean-all:
	-rm -f comp00/*.o
	-rm -f comp00/bin/*.BIN

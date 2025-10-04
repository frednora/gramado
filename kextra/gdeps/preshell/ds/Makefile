# ds00/
# Gramado display server.
# DS00.BIN is the main display server for Gramado OS.
# Created by Fred Nora.

all:

# Libraries
# Excluive for the display server.
	make -C libs/

# Display servers
	make -C ds00/

# ps: Not time for more display servers.
#	make -C ds01/

clean:
#	-rm -f ds00/*.o
#	-rm -f ds0~1/*.o
	-rm -f ds00/bin/*.BIN
#	-rm -f ds01/bin/*.BIN

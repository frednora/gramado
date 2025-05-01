# Gramado display server.
# DS00.BIN is the main display server for Gramado OS.


all:

# Libraries
	make -C libs/

# Display servers
	make -C ds00/
# ps: Not time for more display servers.
#	make -C ds01/

clean:
	-rm -f ds00/bin/*.BIN
#	-rm -f ds01/bin/*.BIN

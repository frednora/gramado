
# Garden
all:

# Entries: 
#  + Boot time entry
#  + AP initialization entry
#  + Interrupt time entry

	make -C ap/
	make -C bsp/

	make -C gramnet/
	make -C net/

clean:
	-rm ap/*.o
	-rm bsp/*.o



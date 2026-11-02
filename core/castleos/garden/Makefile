# Garden
all:

# Entries: 
#  + Boot time entry
#  + AP initialization entry
#  + Interrupt time entry
	make -C entry/

	make -C gramnet/
	make -C net/

clean:
	-rm entry/*.o
clean-all:
	-rm entry/*.o
	-rm gramnet/*.o
	-rm net/*.o


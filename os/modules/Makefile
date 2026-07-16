# Compile all the loadable kernel modules.
# For now these are not dynlinked modules.
# Created by Fred Nora.

all:
	make -C dungeon/
	make -C netu/
	make -C udrivers/

clean:
	-rm bin/*.BIN

clean-all:

	-rm dungeon/*.o
	-rm netu/*.o
#	-rm udrivers/*.o

	-rm bin/*.BIN
	-rm dungeon/*.BIN 
	-rm netu/bin/*.BIN
	-rm udrivers/bin/*.BIN



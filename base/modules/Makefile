# Compile all the loadable kernel modules.
# For now these are not dynlinked modules.
# Created by Fred Nora.

all:
	make -C dungeon/
	make -C servers/


clean:
	-rm bin/*.BIN

clean-all:

	-rm dungeon/*.o
	-rm servers/*.o


	-rm bin/*.BIN
	-rm dungeon/*.BIN 
	-rm servers/bin/*.BIN



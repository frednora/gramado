# Compile all the loadable kernel modules.
# For now these are not dynlinked modules.
# Created by Fred Nora.

all:
	make -C hvmod0/
	make -C hvmod1/

clean:
	-rm bin/*.BIN

clean-all:

	-rm bin/*.BIN

	-rm hvmod0/*.o
	-rm hvmod0/*.BIN 

	-rm hvmod1/*.o
	-rm hvmod1/*.BIN 


# Create MIAMI.BIN.
# Created by Fred Nora

# No shell.
# This is faster version of the boot manager.
# It initializes the pre-selected graphics mode
# and jumps into the BL.BIN.

all: asm-stuff cp-stuff clean
	@echo "MIAMI.BIN OK ?"

asm-stuff:
	nasm main.asm -o MIAMI.BIN

cp-stuff:
	-cp MIAMI.BIN ../../bin/

clean:
#	find . -type f -name '*.o' -delete
	-rm MIAMI.BIN

clean2:
	-rm MIAMI.BIN
	


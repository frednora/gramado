# Create BMGRAM.BIN.
# Created by Fred Nora

# No shell.
# This is faster version of the boot manager.
# It initializes the pre-selected graphics mode
# and jumps into the BL.BIN.

all: asm-stuff cp-stuff clean
	@echo "BMGRAM.BIN OK ?"

asm-stuff:
	nasm main.asm -o BMGRAM.BIN

cp-stuff:
	-cp BMGRAM.BIN ../../bin/

clean:
#	find . -type f -name '*.o' -delete
	-rm BMGRAM.BIN

clean2:
	-rm BMGRAM.BIN
	


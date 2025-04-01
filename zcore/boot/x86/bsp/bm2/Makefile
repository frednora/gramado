
# Create BM2.BIN.
# No shell.
# This is faster version of the boot manager.
# It initializes the pre-selected graphics mode
# and jumps into the BL.BIN.

all: asm-stuff cp-stuff clean
	@echo "BM2.BIN OK ?"
asm-stuff:
	nasm main.asm -o BM2.BIN
cp-stuff:
	-cp BM2.BIN ../bin/
clean:
#	find . -type f -name '*.o' -delete
	-rm BM2.BIN
clean2:
	-rm BM2.BIN
	


# Create BM.BIN.
# It initializes the bm-shell in 32bit assembly.
# In this shell we can select the graphics mode
# switching back to realmode.
# This was the legacy version of this program.
#

all: asm-stuff cp-stuff clean
	@echo "BM.BIN OK ?"
asm-stuff:
	nasm main.asm -o BM.BIN
cp-stuff:
	-cp BM.BIN ../bin/
clean:
#	find . -type f -name '*.o' -delete
	-rm BM.BIN
clean2:
	-rm BM.BIN
	

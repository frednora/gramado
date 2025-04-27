# Build some stuff in boot/
# Created by Fred Nora.

NASM = nasm 

all:

# Create the virtual disk 0.
	@echo "Create VHD using nasm"
	@$(NASM) x86/bsp/vd/fat/main.asm \
	-I x86/bsp/vd/fat/ \
	-o GRAMHV.VHD 

# MBR (backup version)
# Create backup for MBR 0.
	@echo "Building MBR0.BIN"
	@$(NASM) x86/bsp/vd/fat/stage1.asm \
	-I x86/bsp/vd/fat/ \
	-o MBR0.BIN
	@echo "Copying MBR0.BIN"
	@-cp MBR0.BIN  x86/bin/

# BM.BIN
# ::Build BM.BIN. (legacy, no dialog)
	@echo "Building BM.BIN"
	@$(MAKE) -C x86/bsp/bm/ 

# BM2.BIN
# ::Build BM2.BIN. (Interface with dialog)
	@echo "Building BM2.BIN"
	@$(MAKE) -C x86/bsp/bm2/ 

# BLGRAM.BIN
# ::Build BLGRAM.BIN
	@echo "Compiling BL.BIN"
	@$(MAKE) -C x86/bsp/blgram/ 

clean:
#todo
	-rm *.o 

clean-all:
	-rm *.o 
	-rm *.BIN 
#	-rm -rf arm/bsp/bin/*.BIN
	-rm -rf x86/bsp/bin/*.BIN
	-rm -rf MBR0.BIN 
	-rm -rf GRAMHV.VHD 
 





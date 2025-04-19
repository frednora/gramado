# Build some stuff in boot/
# Created by Fred Nora.

NASM = nasm 

#
# Config
#

# verbose
# Quiet compilation or not.
ifndef CONFIG_USE_VERBOSE
	CONFIG_USE_VERBOSE = 1
endif

ifeq ($(CONFIG_USE_VERBOSE),1)
# Not silent. It prints the commands.
	Q =
else
# silent
	Q = @
endif



all:

# Create the virtual disk 0.
	$(Q)$(NASM) x86/bsp/vd/fat/main.asm \
	-I x86/bsp/vd/fat/ \
	-o GRAMHV.VHD 

# MBR (backup version)
# Create backup for MBR 0.
	$(Q)$(NASM) x86/bsp/vd/fat/stage1.asm \
	-I x86/bsp/vd/fat/ \
	-o MBR0.BIN

# BM.BIN
# ::Build BM.BIN. (legacy, no dialog)
	$(Q)$(MAKE) -C x86/bsp/bm/ 

# BM2.BIN
# ::Build BM2.BIN. (Interface with dialog)
	$(Q)$(MAKE) -C x86/bsp/bm2/ 

# BLGRAM.BIN
# ::Build BLGRAM.BIN
	$(Q)$(MAKE) -C x86/bsp/blgram/ 

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
 





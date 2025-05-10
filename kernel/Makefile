# Makefile for the Gramado kernel.
# Created by Fred Nora.

# Tools
include core/arch/x86_64/makeinc

linkTarget = KERNEL.BIN

# See: https://linux.die.net/man/1/ld
# -s --strip-all:   Omit all symbol information from the output file.
# -x --discard-all: Delete all local symbols.
# -static: Do not link against shared libraries. 
# -T scriptfile

linkFlags := -m elf_x86_64 \
-s \
-x \
-static \
-T core/arch/x86_64/link.ld   

#
# Config
#

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

# test: gramado 64bit
# See: https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html
CFLAGS := \
	-Wall \
	-Wundef \
	-Wmissing-prototypes \
	-Wno-uninitialized \
	-Wno-format-security \
	-Wno-trigraphs \
	-Werror=strict-prototypes \
	-Werror=implicit-function-declaration \
	-Werror=implicit-int \
	-Werror=return-type \
	-std=gnu89 \
	-m64 \
	-s \
	-static \
	-fcommon \
	-fgnu89-inline \
	-finline-functions \
	-fshort-wchar \
	-ffreestanding \
	-fleading-underscore \
	-fstrict-overflow \
	-nostdlib \
	-nodefaultlibs \
	-nostdinc \
	-fno-builtin \
	-fno-stack-protector \
	-fno-strict-aliasing \
	-fno-PIE \
	-fno-pie \
	-fno-omit-frame-pointer \
	-fno-optimize-sibling-calls    


# ========================
# Config

#test
#CONFIG_MAIN=y
# ...

# ========================
# Gramado objects:
# Heat control.

# ==[Part 01]======================
# core/ (HOT)
objects-y := head_64.o 
objects-y += p1_ke.o  
objects-y += p1_kmain.o  

# ==[Part 02]======================
# gramk/ - Main wrapper for the kernel io system.
objects-y += p2_gramk.o  

# ==[Part 03]======================
objects-y += p3_libk.o  

# ==[Part 04]======================
# res/ - Kernel resources (COLD)
# We can create interfaces that belongs to a given cgroup.

objects-y += p4_mm.o  

objects-y += p4_bus.o  

objects-y += blkdev.o  
objects-y += chardev.o  
objects-y += netdev.o  
objects-y += p4_dev.o  

objects-y += p4_net.o    

objects-y += p4_fs.o  


# ========================
PHONY := all
all: RedZone PinkZone WhiteZone \
link-kernel \
clean    

	@echo "Done?"

# ----------------
# Core kernel components
RedZone:
# Part 01
	@echo "Compiling: Compiling RedZone"
	$(Q)$(MAKE) -C core/

# ----------------
# Graphical Device Interface and User Interaction Interface.
PinkZone:
# Part 02
	@echo "Compiling: Compiling PinkZone"
	$(Q)$(MAKE) -C gramk/

# ----------------
WhiteZone:
# Part 03, Part 04.
	@echo "Compiling: Compiling WhiteZone"
	$(Q)$(MAKE) -C libk/
	$(Q)$(MAKE) -C res/
#   ...

# ----------------
link-kernel:
	@echo "Linking: Linking the kernel image ..."
	$(LD) $(linkFlags) -o $(linkTarget) $(objects-y)    
clean:
	rm -rf *.o   
	@echo "~clean"
clean-all:

	rm -rf *.o   
	rm -rf *.BIN   

	rm -rf core/*.o   
	rm -rf res/*.o   

	rm -rf core/arch/*.o   
	rm -rf core/clibs/*.o   
	rm -rf core/ke/*.o   
	rm -rf core/kmain/*.o   
	rm -rf core/gramk/*.o   

	rm -rf res/bus/*.o   
	rm -rf res/dev/*.o   
	rm -rf res/fs/*.o   
	rm -rf res/mm/*.o   
	rm -rf res/net/*.o   

	@echo "~clean-all"


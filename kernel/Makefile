# Makefile for the Gramado kernel.
# Created by Fred Nora.

# -----------------------------------------
# Order of compilation:
# -----------------------------------------
# Zone         | Subfolders to compile
# -----------------------------------------
# Red   (Hot)  | core/
# Pink  (Warm) | wink/ and res/
# White (Cold) | libk/ 

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
-Map=kernel.map \
-T core/arch/x86_64/link.ld   

#
# Config
#

ifndef CONFIG_USE_VERBOSE
	CONFIG_USE_VERBOSE=1
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

# test
MYTEST_CONFIG=n
# ...

# ========================
# Gramado objects:
# Heat control.

# I will not change the order for now.
# I want, but i will not.

# ==[Part 01]======================
# core/ (HOT)
objects-y := head_64.o 
objects-y += p1_ke.o  
objects-y += p1_kmain.o  

# ==[Part 02]======================
# wink/ - Main wrapper for the kernel io system.
objects-y += p2_wink.o  

# ==[Part 03]======================
# res/ - Kernel resources (COLD)
# We can create interfaces that belongs to a given cgroup.

objects-y += p3_mm.o  
objects-y += p3_bus.o  
objects-y += blkdev.o  
objects-y += chardev.o  
objects-y += netdev.o  
objects-y += p3_dev.o  
objects-y += p3_net.o    
objects-y += p3_fs.o  

# ==[Part 04]======================
# libk/ (Runtime Library)
objects-y += p4_libk.o  

# test
# objects-$(MYTEST_CONFIG) += fake_mytest.o

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
# Resources
PinkZone:
# Part 02 and Part 3
	@echo "Compiling: Compiling PinkZone"
	$(Q)$(MAKE) -C wink/
	$(Q)$(MAKE) -C res/

# ----------------
# libk/ (Runtime Library)
WhiteZone:
# Part 04
	@echo "Compiling: Compiling WhiteZone"
	$(Q)$(MAKE) -C libk/
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
	rm -rf core/wink/*.o   

	rm -rf res/bus/*.o   
	rm -rf res/dev/*.o   
	rm -rf res/fs/*.o   
	rm -rf res/mm/*.o   
	rm -rf res/net/*.o   

	@echo "~clean-all"


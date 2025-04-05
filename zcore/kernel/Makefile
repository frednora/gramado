# Makefile for the Gramado kernel.
# Created by Fred Nora.

# Tools
include arch/x86_64/makeinc

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
-T arch/x86_64/link.ld   

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
# Objects

# ========================
# :: arch - entrypoint and assembly files.
objects-y := head_64.o 

# ========================
# :: core - Resource controllers.
objects-y += i1_clibs.o  
objects-y += i2_ke.o  
objects-y += i3_kmain.o  

# ========================
# Main wrapper for the kernel.
objects-y += x_gramk.o  

# ========================
# :: kres - Kernel resources
# We can create interfaces that belongs to a given cgroup.
objects-y += j0_bus.o  
objects-y += j1_dev.o  
objects-y += j2_fs.o  
objects-y += j3_mm.o  
objects-y += j4_net.o    

# ========================
PHONY := all
all: k1 k2 \
link-kernel \
clean    

	@echo "Done?"

#----------------
# Core kernel components
k1:
	@echo "Compiling: Compiling k1"

# Kernel arch - Startup and entry code.
	$(Q)$(MAKE) -C arch/x86_64/

# kernel libraries
	$(Q)$(MAKE) -C core/clibs/
# main component. (cpu)
	$(Q)$(MAKE) -C core/ke/
# Kernel core - Top level routines.
	$(Q)$(MAKE) -C core/kmain/

# Top level routines.
	$(Q)$(MAKE) -C gramk/

#----------------
# Kernel resources
k2:
	@echo "Compiling: Compiling k2"
	$(Q)$(MAKE) -C kres/bus/
	$(Q)$(MAKE) -C kres/dev/
	$(Q)$(MAKE) -C kres/fs/
	$(Q)$(MAKE) -C kres/mm/
	$(Q)$(MAKE) -C kres/net/

#----------------
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
	rm -rf kres/*.o   

	rm -rf arch/*.o   
	rm -rf core/clibs/*.o   
	rm -rf core/ke/*.o   
	rm -rf core/kmain/*.o   
	rm -rf core/gramk/*.o   

	rm -rf kres/bus/*.o   
	rm -rf kres/dev/*.o   
	rm -rf kres/fs/*.o   
	rm -rf kres/mm/*.o   
	rm -rf kres/net/*.o   

	@echo "~clean-all"


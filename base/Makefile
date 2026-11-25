# It builds the whole operating system. 

# License: MIT License
# Compiling on gcc 11.4.0 (Ubuntu on wsl)
# Linking on ld 2.38

# Building full distributions into this directory
ALL_BUILDS = x1

# Target directory for the binaries.
# The binaries compiled here will go to this directory.
BUILD = $(ALL_BUILDS)/build1

# Desktop Environment
DE_DIR = ../gramado

# POSIX-like commands
CMDS_DIR = ../cmds

# Kernel image
KERNEL_DIR = kernel

# #test
# Putting the dependencies inside the kernel source tree.
# The OS has two major components:
# The 'kernel image' and the 'dependencies'
# The dependencies are: modules, and apps.
# All the dependencies are in ?/ folder,
# It's because of the close interaction userland
# with the other subfolders in ?/.

# The init process
# The first ring 3 process
INIT_SYSTEM_DIR = sysd


# =================================
# L1: cmds - POSIX-like commands and experiments

L1_CMDS  = $(CMDS_DIR)/cmds00
L1_CPP00 = $(CMDS_DIR)/cpp00


# =================================
# L2: GUI

# comp00: Display server / compositor
L2_COMP     = $(DE_DIR)/winsys/comp00

# assets: Assets for shell
L2_SHELL_ASSETS = $(DE_DIR)/shell/assets

# Client-side GUI applications (Nomad Pastors)
L2_WINAPPS  = $(DE_DIR)/shell/winapps
L2_WINSHELL = $(DE_DIR)/shell/winshell

# velo3d: 3D demo
L2_VELO3D   = $(DE_DIR)/velo3d



## =================================
# Make variables (CC, etc...)
AS      = as
LD      = ld
CC      = gcc
AR      = ar
MAKE    = make
NASM    = nasm
PYTHON  = python
PYTHON2 = python2
PYTHON3 = python3


# todo
# MYMAKE_FLAGS = -j4

# --------------------------------------
# The script starts here.
#
# The six steps:
# + We build the core components and install them into the ALL_BUILDS folder.
# + We also install the pre-built desktop environment componets.
# + We mount the virtual disk.
# + We copy the ALL_BUILDS folder into the virtual disk.
# + We unmount the virtual disk.
#

PHONY := all
all:  \
build-base \
build-install-cmds \
build-install-de \
$(ALL_BUILDS)/gramvd \
vhd-mount \
vhd-copy-files \
vhd-unmount \
clean    

# Giving permitions to run.
	chmod 755 ./run
	chmod 755 ./run2
	chmod 755 ./runt1
	chmod 755 ./runt2

	@echo "Done? (base + cmds + DE)"


# ===================================
# Alternative path: core only (no Desktop Environment)
# Produces a VHD that contains boot, kernel, init and modules
# but none of the gramado DE binaries/assets.
PHONY := all-base
all-base: \
build-base \
build-install-cmds \
$(ALL_BUILDS)/gramvd \
vhd-mount \
vhd-copy-files \
vhd-unmount \
clean

	chmod 755 ./run
	chmod 755 ./run2
	chmod 755 ./runt1
	chmod 755 ./runt2

	@echo "Done? (base + cmds, no DE)"

# ===================================
# Step 1) Build and install core components
PHONY := build-base  
build-base:     
	@echo ":: [] Building VHD, bootloaders and kernel image."
# options: 
# main.asm and main2.asm
# O mbr só consegue ler o root dir para pegar o BM.BIN
# See: stage1.asm
# O BM.BIN só consegue ler o root dir pra pegar o BL.BIN
# See: main.asm
# the kernel image
# O BL.BIN procura o kernel no diretorio GRAMADO/
# See: fs/loader.c

# (1) boot/arch/ 

# ::Build the bootloader.
	@echo "Compiling boot/"
	@$(MAKE) -C boot/arch/

	@echo "Installing boot/"

# Copy the virtual disk into the rootdir.
	@cp boot/arch/GRAMHV.VHD  .

# Copy the bootloader into the rootdir
	@cp boot/arch/x86/bin/MIAMI.BIN   $(BUILD)/
	@cp boot/arch/x86/bin/BMGRAM.BIN  $(BUILD)/
	@cp boot/arch/x86/bin/BLGRAM.BIN  $(BUILD)/
	@cp boot/arch/x86/bin/MBR0.BIN    $(BUILD)/
	@cp boot/arch/x86/bin/APX86.BIN   $(BUILD)/

# Copy the bootloader into the GRAMADO/ directory
	@cp boot/arch/x86/bin/MIAMI.BIN   $(BUILD)/
	@cp boot/arch/x86/bin/BMGRAM.BIN  $(BUILD)/GRAMADO
	@cp boot/arch/x86/bin/BLGRAM.BIN  $(BUILD)/GRAMADO
	@cp boot/arch/x86/bin/MBR0.BIN    $(BUILD)/GRAMADO

# Copy the bootloader into the DE/ directory.
#	@cp boot/arch/x86/bin/APX86.BIN   $(BUILD)/DE   

# (2) $(KERNEL_DIR)/

# ::Build kernel image
	@echo "Compiling $(KERNEL_DIR)/"
	@$(MAKE) -C $(KERNEL_DIR)/

	@echo "Installing kernel/"

# Copy the kernel to the standard system folder.
	@cp $(KERNEL_DIR)/KERNEL.BIN  $(BUILD)/GRAMADO
# Create a backup; The bootloder expects this.
	@cp $(KERNEL_DIR)/KERNEL.BIN  $(BUILD)/DE

# The init process
	@$(MAKE) -C $(INIT_SYSTEM_DIR)/
	@cp $(INIT_SYSTEM_DIR)/src/bin/INIT.BIN  $(BUILD)/


# (3) modules/

# ::Build the ring0 module image
# ::Build user mode drivers
# ::Build user mode servers

	@echo "Compiling modules/"
	@$(Q)$(MAKE) -C modules/

	@echo "Installing modules/"

# The main kernel module
	@cp modules/bin/DUNGEON.BIN  $(BUILD)/
	@cp modules/bin/DUNGEON.BIN  $(BUILD)/GRAMADO

# servers
	@-cp modules/servers/network/bin/NET.BIN   $(BUILD)/GRAMADO/
	@-cp modules/servers/network/bin/NETD.BIN  $(BUILD)/GRAMADO/
#	@-cp modules/servers/vgad/bin/VGAD.BIN  $(BUILD)/GRAMADO


	@echo "~build-base end?"


# ===================================
# Step 2.0) Importing the some core cmds if available.
# ps: It was previously built by another project.
PHONY := build-install-cmds
build-install-cmds:
	@echo "build-install-cmds"
# ----------------------------------
# L1: Install binaries
	@echo "Installing Core commands"

# ----------------------------------
# L1: Install binaries
	@echo "Installing L1"

	@-cp $(L1_CMDS)/bin/CAT.BIN       $(BUILD)/
	@-cp $(L1_CMDS)/bin/REBOOT.BIN    $(BUILD)/
	@-cp $(L1_CMDS)/bin/SHUTDOWN.BIN  $(BUILD)/
	@-cp $(L1_CMDS)/bin/UNAME.BIN     $(BUILD)/

	@-cp $(L1_CMDS)/bin/SYSINF.BIN    $(BUILD)/

#	@-cp $(L1_CMDS)/bin/CAT00.BIN    $(BUILD)/
#	@-cp $(L1_CMDS)/bin/CMP.BIN      $(BUILD)/
#	@-cp $(L1_CMDS)/bin/ED00.BIN     $(BUILD)/
	@-cp $(L1_CMDS)/bin/HTTP.BIN     $(BUILD)/
	@-cp $(L1_CMDS)/bin/HTTPSRV.BIN  $(BUILD)/
#	@-cp $(L1_CMDS)/bin/HEXDUMP.BIN  $(BUILD)/
	@-cp $(L1_CMDS)/bin/GRAMCNF.BIN  $(BUILD)/
	@-cp $(L1_CMDS)/bin/JS.BIN       $(BUILD)/
#	@-cp $(L1_CMDS)/bin/TASCII.BIN   $(BUILD)/
#	@-cp $(L1_CMDS)/bin/TPRINTF.BIN  $(BUILD)/

	@-cp $(L1_CMDS)/bin/PUBSH.BIN     $(BUILD)/DE/
	@-cp $(L1_CMDS)/bin/SHELL.BIN     $(BUILD)/DE/
	@-cp $(L1_CMDS)/bin/SHELL2.BIN    $(BUILD)/DE/
	@-cp $(L1_CMDS)/bin/SHELL3.BIN    $(BUILD)/DE/



#	@-cp $(L1_CMDS)/bin/FALSE.BIN     $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/TRUE.BIN      $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/CMP.BIN       $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/SHOWFUN.BIN   $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/SUM.BIN       $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/N9.BIN        $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/N10.BIN       $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/N11.BIN       $(BUILD)/GRAMADO/
#	@-cp $(L1_CMDS)/bin/UDPTEST.BIN   $(BUILD)/GRAMADO/
	@-cp $(L1_CMDS)/bin/PUBSH.BIN     $(BUILD)/GRAMADO/
	@-cp $(L1_CMDS)/bin/REBOOT.BIN    $(BUILD)/GRAMADO/
	@-cp $(L1_CMDS)/bin/SH7.BIN       $(BUILD)/GRAMADO/
	@-cp $(L1_CMDS)/bin/SHELL.BIN     $(BUILD)/GRAMADO/
	@-cp $(L1_CMDS)/bin/SHUTDOWN.BIN  $(BUILD)/GRAMADO/

	@-cp $(L1_CPP00)/bin/CPP00.BIN    $(BUILD)/DE

	@echo "~build-install-cmds"

# ===================================
# Step 2.1) Importing the Desktop Environment.
# ps: It was previously built by another project.
PHONY := build-install-de
build-install-de:
	@echo "build-install-de"

# ----------------------------------
# L2: Install binaries, theme and 3d demos
	@echo "Installing L2"


# Compositor
	@-cp $(L2_COMP)/src/bin/COMP00.BIN  $(BUILD)/DE

# winshell
	@-cp $(L2_WINSHELL)/bin/BROWSER.BIN   $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/DESKTOP.BIN   $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/TASKBAR.BIN   $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/TERMINAL.BIN  $(BUILD)/DE/

# winapps
	@-cp $(L2_WINAPPS)/bin/CALC.BIN     $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/EDITOR.BIN   $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/LAUNCH.BIN   $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/MEMORY.BIN   $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/PAINT.BIN    $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/PAIRS.BIN    $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/POWER.BIN    $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/REVERSI.BIN  $(BUILD)/DE/
	@-cp $(L2_WINAPPS)/bin/SYSINFO.BIN  $(BUILD)/DE/


# dependencies
	@cp $(L2_SHELL_ASSETS)/themes/theme01/*.BMP  $(BUILD)/DE

# Velociraptor 3D demo
	@-cp $(L2_VELO3D)/bin/ENG00.BIN   $(BUILD)/DE/
# dependencies
	@-cp $(L2_VELO3D)/assets/OBJ06.TXT  $(BUILD)/
	@-cp $(L2_VELO3D)/assets/OBJ07.TXT  $(BUILD)/
	@-cp $(L2_VELO3D)/assets/OBJ08.TXT  $(BUILD)/


	@echo "~build-install-de"


# ===================================
# Step 3: Creating the directory where we're gonna mount the VHD
$(ALL_BUILDS)/gramvd:
	@echo "Build: Creating the directory to mount the VHD ..."
	@sudo mkdir $(ALL_BUILDS)/gramvd

# ===================================
# Step 4: Mounting the VHD
# ps: It depends on the directory we already created
vhd-mount: $(ALL_BUILDS)/gramvd
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	@-sudo umount $(ALL_BUILDS)/gramvd
	@sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD  $(ALL_BUILDS)/gramvd/

# ===================================
# Step 5: Copying files into the mounted VHD
# Copying the $(BUILD)/ folder into the mounted VHD.
# The files been copied depends on the disk being mounted.
vhd-copy-files: vhd-mount
	@echo "========================="
	@echo "Build: Copying files into the mounted VHD ..."
	# Copy $(BUILD)/
	# sends everything from disk/ to root.
	@sudo cp -r $(BUILD)/*  $(ALL_BUILDS)/gramvd

# --------------------------------------
# Step 6: Unmounting the VHD
vhd-unmount: vhd-copy-files
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	@sudo umount $(ALL_BUILDS)/gramvd

# --------------------------------------
# Run on qemu using kvm.
PHONY := run
run: do_run
do_run:
	sh ./run

# --------------------------------------
# Run on qemu with no kvm.
PHONY := runnokvm
runnokvm: do_runnokvm
do_runnokvm:
	sh ./runnokvm


test-vhd-mount: $(ALL_BUILDS)/gramvd
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	@-sudo umount $(ALL_BUILDS)/gramvd
	@sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD  $(ALL_BUILDS)/gramvd/

test-vhd-unmount:
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	@sudo umount $(ALL_BUILDS)/gramvd

# --------------------------------------
# build: Developer comand 1.
# install
# Build the images and put them all into $(BUILD)/ folder.
PHONY := install
install: do_install
do_install: \
build-base  

# --------------------------------------
# build: Developer comand 2.
# image
# Copy all the files from $(BUILD)/ to the VHD.
PHONY := image
image: do_image
do_image: \
$(ALL_BUILDS)/gramvd    \
vhd-mount          \
vhd-copy-files     \
vhd-unmount        \

# --------------------------------------
# Basic clean
clean:
	-rm *.o
	-rm *.BIN
	-rm $(KERNEL_DIR)/*.o
	-rm $(KERNEL_DIR)/*.BIN
	@echo "~clean"

# #todo: Delete some files in the ALL_BUILDS/ folder.
# dist-clean:
#	@echo "~dist-clean"

# --------------------------------------
# Clean up all the mess.
clean-all: clean

	-rm *.o
	-rm *.BIN
	-rm *.VHD
	-rm *.ISO

	-rm boot/arch/*.VHD 

# ==================
# (1) boot/arch/
# Clear boot images
#	-rm -rf boot/arch/arm/bin/*.BIN
	-rm -rf boot/arch/x86/bin/*.BIN

# ==================
# (2) $(KERNEL_DIR)/
# Clear kernel image
	-rm $(KERNEL_DIR)/*.o
	-rm $(KERNEL_DIR)/*.BIN
	-rm -rf $(KERNEL_DIR)/KERNEL.BIN
	-rm -rf $(KERNEL_DIR)/kernel.map 

# ==================
# Clear INIT.BIN
	-rm $(INIT_SYSTEM_DIR)/src/*.o
	-rm $(INIT_SYSTEM_DIR)/src/*.BIN 
	-rm $(INIT_SYSTEM_DIR)/src/bin/*.BIN 

# ==================
# modules/
# Clear the ring0 module images
	-rm -rf modules/*.o
	-rm -rf modules/*.BIN
	-rm -rf modules/bin/*.BIN

# ==================
# Clear the disk cache
	-rm -rf $(BUILD)/*.BIN 
	-rm -rf $(BUILD)/*.BMP
	-rm -rf $(BUILD)/EFI/BOOT/*.EFI 
	-rm -rf $(BUILD)/GRAMADO/*.BIN 
	-rm -rf $(BUILD)/DE/*.BIN 
	-rm -rf $(BUILD)/DE/*.BMP

	@echo "~clean-all"

# --------------------------------------
# Usage instructions.
usage:
	@echo "Building everything:"
	@echo "make all"
	@echo "Clear the mess to restart:"
	@echo "make clean-all"
	@echo "Testing on qemu:"
	@echo "./run"
	@echo "./runnokvm"

# --------------------------------------
# Danger zone!
# This is gonna copy th image into the real HD.
# My host is running on sdb and i copy the image into sda.
# It is because the sda is in primary master IDE.
# Gramado has been tested on sda
# and the Fred's Linux host machine is on sdb.
danger-install-sda:
	sudo dd if=./GRAMHV.VHD of=/dev/sda
danger-install-sdb:
	sudo dd if=./GRAMHV.VHD of=/dev/sdb

# Remore Zone.Identifier files created by MS Windows.
danger-remove-zone-id:
	find . -name "*Zone.Identifier" -type f -delete

qemu-instance:
	-cp ./GRAMHV.VHD ./QEMU.VHD 
#xxx-instance:
#	-cp ./GRAMHV.VHD ./XXX.VHD 

# End

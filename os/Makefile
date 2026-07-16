# It builds the whole operating system. 

# License: MIT License
# Compiling on gcc 11.4.0 (Ubuntu on wsl)
# Linking on ld 2.38

# Building full distributions into this directory.
BASE = base

# Target directory for the binaries.
# The binaries compiled here will go to this directory.
BUILD = $(BASE)/build1

# #test
# Putting the dependencies inside the kernel source tree.
# The OS has two major components:
# The 'kernel image' and the 'dependencies'
# The dependencies are: modules, and apps.
# All the dependencies are in userland/ folder,
# It's because of the close interaction userland
# with the other subfolders in core/.


# =================================
# Native UI

__DEP_L2 = native
# Compositor / Display servers
L2_COMP     = $(__DEP_L2)/hunter
# Client-side GUI applications
L2_WINSHELL = $(__DEP_L2)/ifarmer/winshell


# =================================
# Nomad Pastors - POSIX-like commands and experiments

__DEP_LZ = np
L4_CPP00 = $(__DEP_LZ)/cpp00
LZ_CMDS  = $(__DEP_LZ)/cmds

__DEP_NORA3D = nora3d
#L3_XX = $(__DEP_LZ)/xx

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
# == Start ====
# build: User command.
PHONY := all
all:  \
build-gramado-os \
build-extras \
$(BASE)/gramvd \
vhd-mount \
vhd-copy-files \
vhd-unmount \
clean    

# Giving permitions to run.
	chmod 755 ./run
	chmod 755 ./run2
	chmod 755 ./runt1
	chmod 755 ./runt2

	@echo "Done?"

# ===================================
#::0 Build Gramado OS
PHONY := build-gramado-os  
build-gramado-os:     
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

# (2) kernel/

# ::Build kernel image.
	@echo "Compiling kernel/"
	@$(MAKE) -C kernel/

	@echo "Installing kernel/"

# Copy the kernel to the standard system folder.
	@cp kernel/KERNEL.BIN  $(BUILD)/GRAMADO
# Create a backup; The bootloder expects this.
	@cp kernel/KERNEL.BIN  $(BUILD)/DE

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

# netu
	@-cp modules/netu/core/bin/NET.BIN   $(BUILD)/GRAMADO/
	@-cp modules/netu/core/bin/NETD.BIN  $(BUILD)/GRAMADO/

# Ring 3 VGA device driver
	@cp modules/udrivers/bin/VGAD.BIN  $(BUILD)/GRAMADO


# The init process
	@$(MAKE) -C moon/

# Copy the init process.
	@cp moon/src/bin/INIT.BIN  $(BUILD)/

	@echo "~build-gramado-os end?"

# ===================================
#::1
# Build extras
PHONY := build-extras
build-extras:

	@echo "build-extras"

# __DEP_L2:: Display servers
	@echo "Compiling __DEP_L2"
	@make -C $(__DEP_L2)/

	@echo "Installing __DEP_L2"
# Winu Core - Compositor / Display server
	@-cp $(L2_COMP)/comp00/bin/COMP00.BIN    $(BUILD)/DE

#===================================
# Install BMPs from cali assets.
# Copy the $(L2_WINSHELL)/assets/
# We can't survive without this one.
	@cp $(L2_WINSHELL)/assets/themes/theme01/*.BMP  $(BUILD)/DE


# ?::
	@echo "Compiling np/"
	@make -C $(__DEP_LZ)/

	@echo "Compiling nora3d/"
	@make -C $(__DEP_NORA3D)/


# --------
# cpp00
# cpp application example
	@-cp $(L4_CPP00)/bin/CPP00.BIN  $(BUILD)/DE


# --------
# sdk

	@-cp $(LZ_CMDS)/bin/CAT.BIN       $(BUILD)/
	@-cp $(LZ_CMDS)/bin/CAT00.BIN     $(BUILD)/

# tests
	@-cp $(LZ_CMDS)/bin/LUA1.BIN       $(BUILD)/
#	@-cp $(LZ_CMDS)/bin/CMP.BIN        $(BUILD)/
#	@-cp $(LZ_CMDS)/bin/TASCII.BIN     $(BUILD)/
#	@-cp $(LZ_CMDS)/bin/TPRINTF.BIN    $(BUILD)/
	@-cp $(LZ_CMDS)/bin/ED00.BIN       $(BUILD)/

#	@-cp $(LZ_CMDS)/bin/HEXDUMP.BIN    $(BUILD)/


# Experimental commands
#	@-cp $(LZ_CMDS)/bin/FALSE.BIN      $(BUILD)/GRAMADO/
#	@-cp $(LZ_CMDS)/bin/TRUE.BIN       $(BUILD)/GRAMADO/
#	@-cp $(LZ_CMDS)/bin/CMP.BIN       $(BUILD)/GRAMADO/
#	@-cp $(LZ_CMDS)/bin/SHOWFUN.BIN   $(BUILD)/GRAMADO/
#	@-cp $(LZ_CMDS)/bin/SUM.BIN       $(BUILD)/GRAMADO/
	@-cp $(LZ_CMDS)/bin/GRAMCNF.BIN     $(BUILD)/
#@-cp $(LZ_CMDS)/bin/N9.BIN         $(BUILD)/GRAMADO/
#@-cp $(LZ_CMDS)/bin/N10.BIN        $(BUILD)/GRAMADO/
#@-cp $(LZ_CMDS)/bin/N11.BIN        $(BUILD)/GRAMADO/
#@-cp $(LZ_CMDS)/bin/UDPTEST.BIN  $(BUILD)/GRAMADO/

	@-cp $(LZ_CMDS)/bin/REBOOT.BIN    $(BUILD)/
	@-cp $(LZ_CMDS)/bin/SHUTDOWN.BIN  $(BUILD)/
	@-cp $(LZ_CMDS)/bin/UNAME.BIN     $(BUILD)/

	@-cp $(LZ_CMDS)/bin/PUBSH.BIN     $(BUILD)/GRAMADO/
	@-cp $(LZ_CMDS)/bin/REBOOT.BIN    $(BUILD)/GRAMADO/
	@-cp $(LZ_CMDS)/bin/SH7.BIN       $(BUILD)/GRAMADO/
	@-cp $(LZ_CMDS)/bin/SHELL.BIN     $(BUILD)/GRAMADO/
	@-cp $(LZ_CMDS)/bin/SHUTDOWN.BIN  $(BUILD)/GRAMADO/

	@-cp $(LZ_CMDS)/bin/PUBSH.BIN     $(BUILD)/DE/
	@-cp $(LZ_CMDS)/bin/SHELL.BIN     $(BUILD)/DE/
	@-cp $(LZ_CMDS)/bin/SHELL2.BIN    $(BUILD)/DE/


# --------
# winshell
# These need the '#' prefix.

# DE core applications
	@-cp $(L2_WINSHELL)/bin/TASKBAR.BIN   $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/TERMINAL.BIN  $(BUILD)/DE/

# DE Utilities
	@-cp $(L2_WINSHELL)/bin/DOC.BIN      $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/EDITOR.BIN   $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/MEMORY.BIN   $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/POWER.BIN    $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/SYSINFO.BIN  $(BUILD)/DE/

# Experimental applications
	@-cp $(L2_WINSHELL)/bin/LAUNCH.BIN   $(BUILD)/DE/
	@-cp $(L2_WINSHELL)/bin/MENUAPP.BIN  $(BUILD)/DE/

# Other applications
	@-cp $(L2_WINSHELL)/bin/GWS.BIN      $(BUILD)/DE/

# --------
# heavy (nora3d/)
# 3D game demos

	@-cp $(__DEP_NORA3D)/bin/ENG00.BIN   $(BUILD)/DE/


	@echo "~ build-extras"

# ===================================
#::2
# Step 2: $(BASE)/gramvd  - Creating the directory to mount the VHD.
$(BASE)/gramvd:
	@echo "========================="
	@echo "Build: Creating the directory to mount the VHD ..."
	@sudo mkdir $(BASE)/gramvd

# --------------------------------------
#::3
# ~ Step 3: vhd-mount - Mounting the VHD.
# mounts the disk, depends on the directory existing
vhd-mount: $(BASE)/gramvd
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	@-sudo umount $(BASE)/gramvd
	@sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD  $(BASE)/gramvd/

# --------------------------------------
#::4
# ~ Step 4 vhd-copy-files - Copying files into the mounted VHD.
# Copying the $(BUILD)/ folder into the mounted VHD.
# copies files, depends on the disk being mounted
vhd-copy-files: vhd-mount
	@echo "========================="
	@echo "Build: Copying files into the mounted VHD ..."
	# Copy $(BUILD)/
	# sends everything from disk/ to root.
	@sudo cp -r $(BUILD)/*  $(BASE)/gramvd

# --------------------------------------
#:::5
# ~ Step 5 vhd-unmount  - Unmounting the VHD.
 # unmounts, depends on files being copied
vhd-unmount: vhd-copy-files
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	@sudo umount $(BASE)/gramvd

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


test-vhd-mount: $(BASE)/gramvd
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	@-sudo umount $(BASE)/gramvd
	@sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD  $(BASE)/gramvd/

test-vhd-unmount:
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	@sudo umount $(BASE)/gramvd

# --------------------------------------
# build: Developer comand 1.
# install
# Build the images and put them all into $(BUILD)/ folder.
PHONY := install
install: do_install
do_install: \
build-gramado-os  

# --------------------------------------
# build: Developer comand 2.
# image
# Copy all the files from $(BUILD)/ to the VHD.
PHONY := image
image: do_image
do_image: \
$(BASE)/gramvd    \
vhd-mount          \
vhd-copy-files     \
vhd-unmount        \

# --------------------------------------
# Basic clean.
clean:
	-rm *.o
	-rm *.BIN
	-rm kernel/*.o
	-rm kernel/*.BIN
	@echo "~clean"

# #todo: Delete some files in the BASE/ folder.
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
# (2) kernel/
# Clear kernel image
	-rm kernel/*.o
	-rm kernel/*.BIN
	-rm -rf kernel/KERNEL.BIN
	-rm -rf kernel/kernel.map 

# ==================
# (3) modules/
# Clear the ring0 module images
	-rm -rf modules/*.o
	-rm -rf modules/*.BIN
	-rm -rf modules/bin/*.BIN

# ==================
# Clear INIT.BIN
	-rm moon/src/*.o
	-rm moon/src/*.BIN 
	-rm moon/src/bin/*.BIN 


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

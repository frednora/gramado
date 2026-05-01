# It builds the whole operating system. 

# License: MIT License
# Compiling on gcc 11.4.0 (Ubuntu on wsl)
# Linking on ld 2.38

# Building full distributions into this directory.
DISTROS = distros

# Target directory for the binaries.
# The binaries compiled here will go to this directory.
BASE = $(DISTROS)/base00

# #test
# Putting the dependencies inside the kernel source tree.
# The OS has two major components:
# The 'kernel image' and the 'dependencies'
# The dependencies are: modules, and apps.
# All the dependencies are in userland/ folder,
# It's because of the close interaction userland
# with the other subfolders in core/.


# =================================
# Windowing system
__DEP_L2 = native
# Compositor / Display servers
L2_COMP     = $(__DEP_L2)/hunter/comp
# Client-side GUI applications
L2_WINSHELL = $(__DEP_L2)/farmer/winshell


# =================================
# Nomad Pastors
__DEP_L3 = np
L3_HEAVY    = $(__DEP_L3)/heavy
L3_NETU     = $(__DEP_L3)/netu
L3_SDK      = $(__DEP_L3)/sdk
L3_SYSUTIL  = $(__DEP_L3)/sysutils
L3_SYSUTIL2 = $(__DEP_L3)/sysutils2


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
$(DISTROS)/gramvd \
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

# Copy the bootloader into the rootdir.
	@cp boot/arch/x86/bin/BM.BIN      $(BASE)/
	@cp boot/arch/x86/bin/BM2.BIN     $(BASE)/
	@cp boot/arch/x86/bin/BLGRAM.BIN  $(BASE)/
	@cp boot/arch/x86/bin/MBR0.BIN    $(BASE)/
	@cp boot/arch/x86/bin/APX86.BIN   $(BASE)/
# Copy the bootloader into the GRAMADO/ directory.
	@cp boot/arch/x86/bin/BM.BIN      $(BASE)/GRAMADO
	@cp boot/arch/x86/bin/BM2.BIN     $(BASE)/GRAMADO
	@cp boot/arch/x86/bin/BLGRAM.BIN  $(BASE)/GRAMADO
	@cp boot/arch/x86/bin/MBR0.BIN    $(BASE)/GRAMADO

# Copy the bootloader into the DE/ directory.
#	@cp boot/arch/x86/bin/APX86.BIN   $(BASE)/DE   

# (2) kernel/

# ::Build kernel image.
	@echo "Compiling kernel/"
	@$(MAKE) -C kernel/

	@echo "Installing kernel/"

# Copy the kernel to the standard system folder.
	@cp kernel/KERNEL.BIN  $(BASE)/GRAMADO
# Create a backup; The bootloder expects this.
	@cp kernel/KERNEL.BIN  $(BASE)/DE

# (3) modules/

	@echo "Compiling modules/"
# ::Build the ring0 module image.
	@$(Q)$(MAKE) -C modules/

	@echo "Installing modules/"

# Copy the ring0 module image.
# It is loadable, but it's not a dynlinked format.
	@cp modules/bin/DUNGEON.BIN  $(BASE)/
	@cp modules/bin/DUNGEON.BIN  $(BASE)/GRAMADO


	@$(MAKE) -C init/

# Copy the init process.
	@cp init/src/bin/INIT.BIN  $(BASE)/

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
	@-cp $(L2_COMP)/comp00/bin/COMP00.BIN    $(BASE)/DE

#===================================
# Install BMPs from cali assets.
# Copy the $(L2_WINSHELL)/assets/
# We can't survive without this one.
	@cp $(L2_WINSHELL)/assets/themes/theme01/*.BMP  $(BASE)/DE


# __DEP_L3::
	@echo "Compiling __DEP_L3"
	@make -C $(__DEP_L3)/


# --------
# heavy
# 3D game demos
	@-cp $(L3_HEAVY)/games3d/bin/DEMO00.BIN   $(BASE)/DE/
#	@-cp $(L3_HEAVY)/games3d/bin/DEMO01.BIN   $(BASE)/DE/
# ...

# --------
# netu
	@-cp $(L3_NETU)/core/bin/NET.BIN   $(BASE)/GRAMADO/
	@-cp $(L3_NETU)/core/bin/NETD.BIN  $(BASE)/GRAMADO/

# --------
# sdk

	@-cp $(L3_SDK)/bin/CAT.BIN       $(BASE)/
	@-cp $(L3_SDK)/bin/CAT00.BIN     $(BASE)/

# Experimental commands.
#	@-cp $(L3_SDK)/bin/FALSE.BIN      $(BASE)/GRAMADO/
#	@-cp $(L3_SDK)/bin/TRUE.BIN       $(BASE)/GRAMADO/
#	@-cp $(L3_SDK)/bin/CMP.BIN       $(BASE)/GRAMADO/
#	@-cp $(L3_SDK)/bin/SHOWFUN.BIN   $(BASE)/GRAMADO/
#	@-cp $(L3_SDK)/bin/SUM.BIN       $(BASE)/GRAMADO/
	@-cp $(L3_SDK)/bin/GRAMCNF.BIN     $(BASE)/
#@-cp $(L3_SDK)/bin/N9.BIN         $(BASE)/GRAMADO/
#@-cp $(L3_SDK)/bin/N10.BIN        $(BASE)/GRAMADO/
#@-cp $(L3_SDK)/bin/N11.BIN        $(BASE)/GRAMADO/
#@-cp $(L3_SDK)/bin/UDPTEST.BIN  $(BASE)/GRAMADO/

# --------
# sysutils/

	@-cp $(L3_SYSUTIL)/bin/REBOOT.BIN    $(BASE)/
	@-cp $(L3_SYSUTIL)/bin/SHUTDOWN.BIN  $(BASE)/
	@-cp $(L3_SYSUTIL)/bin/UNAME.BIN     $(BASE)/

	@-cp $(L3_SYSUTIL)/bin/PUBSH.BIN     $(BASE)/GRAMADO/
	@-cp $(L3_SYSUTIL)/bin/REBOOT.BIN    $(BASE)/GRAMADO/
	@-cp $(L3_SYSUTIL)/bin/SH7.BIN       $(BASE)/GRAMADO/
	@-cp $(L3_SYSUTIL)/bin/SHELL.BIN     $(BASE)/GRAMADO/
	@-cp $(L3_SYSUTIL)/bin/SHUTDOWN.BIN  $(BASE)/GRAMADO/

	@-cp $(L3_SYSUTIL)/bin/PUBSH.BIN     $(BASE)/DE/
	@-cp $(L3_SYSUTIL)/bin/SHELL.BIN     $(BASE)/DE/
	@-cp $(L3_SYSUTIL)/bin/SHELL2.BIN    $(BASE)/DE/


# --------
# sysutil2
# cpp application example
#	@-cp $(L3_CPP00)/bin/CPP00.BIN  $(BASE)/DE



# --------
# winshell
# These need the '#' prefix.

# DE core applications
	@-cp $(L2_WINSHELL)/bin/TASKBAR.BIN   $(BASE)/DE/
	@-cp $(L2_WINSHELL)/bin/TERMINAL.BIN  $(BASE)/DE/

# DE Utilities
	@-cp $(L2_WINSHELL)/bin/DOC.BIN      $(BASE)/DE/
	@-cp $(L2_WINSHELL)/bin/EDITOR.BIN   $(BASE)/DE/
	@-cp $(L2_WINSHELL)/bin/MEMORY.BIN   $(BASE)/DE/
	@-cp $(L2_WINSHELL)/bin/POWER.BIN    $(BASE)/DE/
	@-cp $(L2_WINSHELL)/bin/SYSINFO.BIN  $(BASE)/DE/

# Experimental applications
	@-cp $(L2_WINSHELL)/bin/DRAW.BIN     $(BASE)/DE/
	@-cp $(L2_WINSHELL)/bin/LAUNCH.BIN   $(BASE)/DE/
	@-cp $(L2_WINSHELL)/bin/MENUAPP.BIN  $(BASE)/DE/

# Other applications
	@-cp $(L2_WINSHELL)/bin/GWS.BIN      $(BASE)/DE/


	@echo "~ build-extras"

# ===================================
#::2
# Step 2: $(DISTROS)/gramvd  - Creating the directory to mount the VHD.
$(DISTROS)/gramvd:
	@echo "========================="
	@echo "Build: Creating the directory to mount the VHD ..."
	@sudo mkdir $(DISTROS)/gramvd

# --------------------------------------
#::3
# ~ Step 3: vhd-mount - Mounting the VHD.
# mounts the disk, depends on the directory existing
vhd-mount: $(DISTROS)/gramvd
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	@-sudo umount $(DISTROS)/gramvd
	@sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD  $(DISTROS)/gramvd/

# --------------------------------------
#::4
# ~ Step 4 vhd-copy-files - Copying files into the mounted VHD.
# Copying the $(BASE)/ folder into the mounted VHD.
# copies files, depends on the disk being mounted
vhd-copy-files: vhd-mount
	@echo "========================="
	@echo "Build: Copying files into the mounted VHD ..."
	# Copy $(BASE)/
	# sends everything from disk/ to root.
	@sudo cp -r $(BASE)/*  $(DISTROS)/gramvd

# --------------------------------------
#:::5
# ~ Step 5 vhd-unmount  - Unmounting the VHD.
 # unmounts, depends on files being copied
vhd-unmount: vhd-copy-files
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	@sudo umount $(DISTROS)/gramvd

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


test-vhd-mount: $(DISTROS)/gramvd
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	@-sudo umount $(DISTROS)/gramvd
	@sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD  $(DISTROS)/gramvd/

test-vhd-unmount:
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	@sudo umount $(DISTROS)/gramvd

# --------------------------------------
# build: Developer comand 1.
# install
# Build the images and put them all into $(BASE)/ folder.
PHONY := install
install: do_install
do_install: \
build-gramado-os  

# --------------------------------------
# build: Developer comand 2.
# image
# Copy all the files from $(BASE)/ to the VHD.
PHONY := image
image: do_image
do_image: \
$(DISTROS)/gramvd    \
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

# #todo: Delete some files in the distros/ folder.
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
	-rm init/src/*.o
	-rm init/src/*.BIN 
	-rm init/src/bin/*.BIN 


# ==================
# Clear the disk cache
	-rm -rf $(BASE)/*.BIN 
	-rm -rf $(BASE)/*.BMP
	-rm -rf $(BASE)/EFI/BOOT/*.EFI 
	-rm -rf $(BASE)/GRAMADO/*.BIN 
	-rm -rf $(BASE)/DE/*.BIN 
	-rm -rf $(BASE)/DE/*.BMP

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

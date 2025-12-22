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
# Kernel Services: Init process, ring 3 drivers and ring 3 servers.
__DEP_L1 = netu

# =================================
# Shell Pre-UI: The display server.
# The infrastruture for the windows.
__DEP_L2 = winu
# Display servers
L2_DS = $(__DEP_L2)/core/ds
# Display server with embedded 3D demos
L2_DS3D = $(__DEP_L2)/core/ds3d

# =================================
# Client-side GUI applications.
__DEP_L3 = apps
L3_APPS = apps

# =================================
# Posix commands
__DEP_L4 = cmds
L4_CMDS = cmds

## =================================
# (userland extras)
__DEP_L5 = woods
# Client-side GUI applications with X library
L5_XAPPS = $(__DEP_L5)/xapps
# Creating one cpp application just for fun
L5_CPP00 = $(__DEP_L5)/cpp00

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

# (1) iceboot/arch/ 

# ::Build the bootloader.
	@echo "Compiling iceboot/"
	@$(MAKE) -C iceboot/arch/

	@echo "Installing iceboot/"

# Copy the virtual disk into the rootdir.
	@cp iceboot/arch/GRAMHV.VHD  .

# Copy the bootloader into the rootdir.
	@cp iceboot/arch/x86/bin/BM.BIN      $(BASE)/
	@cp iceboot/arch/x86/bin/BM2.BIN     $(BASE)/
	@cp iceboot/arch/x86/bin/BLGRAM.BIN  $(BASE)/
	@cp iceboot/arch/x86/bin/MBR0.BIN    $(BASE)/
	@cp iceboot/arch/x86/bin/APX86.BIN   $(BASE)/
# Copy the bootloader into the GRAMADO/ directory.
	@cp iceboot/arch/x86/bin/BM.BIN      $(BASE)/GRAMADO
	@cp iceboot/arch/x86/bin/BM2.BIN     $(BASE)/GRAMADO
	@cp iceboot/arch/x86/bin/BLGRAM.BIN  $(BASE)/GRAMADO
	@cp iceboot/arch/x86/bin/MBR0.BIN    $(BASE)/GRAMADO

# Copy the bootloader into the DE/ directory.
#	@cp iceboot/arch/x86/bin/APX86.BIN   $(BASE)/DE   

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
	@cp modules/bin/HVMOD0.BIN  $(BASE)/
#	@cp modules/bin/HVMOD0.BIN  $(BASE)/GRAMADO
	@cp modules/bin/HVMOD0.BIN  $(BASE)/DE

# Copy the ring0 module image.
# It is loadable, but it's not a dynlinked format.
#	@cp modules/bin/HVMOD1.BIN  $(BASE)/
#	@cp modules/bin/HVMOD1.BIN  $(BASE)/GRAMADO

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

# __DEP_L1::

	@echo "Compiling __DEP_L1"
	@$(MAKE) -C $(__DEP_L1)

	@echo "Installing __DEP_L1"
	@-cp $(__DEP_L1)/core/bin/NET.BIN   $(BASE)/GRAMADO/
	@-cp $(__DEP_L1)/core/bin/NETD.BIN  $(BASE)/GRAMADO/

# __DEP_L2:: Display servers
	@echo "Compiling __DEP_L2"
	@make -C $(__DEP_L2)/

	@echo "Installing __DEP_L2"

	@-cp $(L2_DS)/ds00/bin/DS00.BIN    $(BASE)/DE
#@-cp $(L2_DS)/ds01/bin/DS01.BIN    $(BASE)/DE
# Display servers with 3D demos.
	@-cp $(L2_DS3D)/bin/DEMO00.BIN   $(BASE)/DE/
	@-cp $(L2_DS3D)/bin/DEMO01.BIN   $(BASE)/DE/

# __DEP_L3::
# Compiling client-side GUI applications
	@echo "Compiling __DEP_L3"
	@make -C $(__DEP_L3)/

# __DEP_L4::
# Compiling Unix-like commands
	@echo "Compiling __DEP_L4 (cmds/)"
	@make -C $(__DEP_L4)/

#===================================
# Install BMPs from cali assets.
# Copy the $(__DEP_L3)/assets/
# We can't survive without this one.
	@cp apps/assets/themes/theme01/*.BMP  $(BASE)/DE

# Well consolidated programs.
	@-cp $(L4_CMDS)/bin/PUBSH.BIN    $(BASE)/GRAMADO/
	@-cp $(L4_CMDS)/bin/PUBSH.BIN    $(BASE)/DE/
	@-cp $(L4_CMDS)/bin/SHELL.BIN    $(BASE)/GRAMADO/
	@-cp $(L4_CMDS)/bin/SHELL.BIN    $(BASE)/DE/
	@-cp $(L4_CMDS)/bin/SHELL2.BIN   $(BASE)/DE/
#	@-cp $(L4_CMDS)/bin/SHELLZZ.BIN  $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/SHELLZZ.BIN  $(BASE)/DE/

# Experimental programs.
	@-cp $(L4_CMDS)/bin/SH7.BIN        $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/SHELLXXX.BIN   $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/TASCII.BIN     $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/TPRINTF.BIN    $(BASE)/GRAMADO/

# Copy well consolidated commands.
	@-cp $(L4_CMDS)/bin/CAT.BIN       $(BASE)/
	@-cp $(L4_CMDS)/bin/CAT00.BIN     $(BASE)/
	@-cp $(L4_CMDS)/bin/REBOOT.BIN    $(BASE)/
	@-cp $(L4_CMDS)/bin/REBOOT.BIN    $(BASE)/GRAMADO/
	@-cp $(L4_CMDS)/bin/SHUTDOWN.BIN  $(BASE)/
	@-cp $(L4_CMDS)/bin/SHUTDOWN.BIN  $(BASE)/GRAMADO/
	@-cp $(L4_CMDS)/bin/UNAME.BIN     $(BASE)/

# Experimental commands.
#	@-cp $(L4_CMDS)/bin/FALSE.BIN      $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/TRUE.BIN       $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/CMP.BIN       $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/SHOWFUN.BIN   $(BASE)/GRAMADO/
#	@-cp $(L4_CMDS)/bin/SUM.BIN       $(BASE)/GRAMADO/
	@-cp $(L4_CMDS)/bin/GRAMCNF.BIN     $(BASE)/
#@-cp $(L4_CMDS)/bin/N9.BIN         $(BASE)/GRAMADO/
#@-cp $(L4_CMDS)/bin/N10.BIN        $(BASE)/GRAMADO/
#@-cp $(L4_CMDS)/bin/N11.BIN        $(BASE)/GRAMADO/
#@-cp $(L4_CMDS)/bin/UDPTEST.BIN  $(BASE)/GRAMADO/

	@-cp $(L3_APPS)/bin/TASKBAR.BIN    $(BASE)/DE

# by Copilot
	@-cp $(L3_APPS)/bin/POWER.BIN   $(BASE)/DE
	@-cp $(L3_APPS)/bin/MEMORY.BIN  $(BASE)/DE
	@-cp $(L3_APPS)/bin/GWTEX.BIN   $(BASE)/DE

	@-cp $(L3_APPS)/bin/MENUAPP.BIN    $(BASE)/DE
	@-cp $(L3_APPS)/bin/LAUNCH.BIN     $(BASE)/DE
	@-cp $(L3_APPS)/bin/TERM00.BIN     $(BASE)/DE
#@-cp $(L3_APPS)/bin/GWS.BIN       $(BASE)/DE
    # Experimental applications
    # These need the '#' prefix.
	@-cp $(L3_APPS)/bin/DOC.BIN      $(BASE)/DE/
	@-cp $(L3_APPS)/bin/EDITOR.BIN   $(BASE)/DE/

	@-cp $(L3_APPS)/bin/GDM.BIN      $(BASE)/DE/
	@-cp $(L3_APPS)/bin/SETUP.BIN    $(BASE)/DE/

# (browser/) browser.
# Teabox web browser
    # Experimental applications
    # These need the '@' prefix.
	@-cp $(L3_APPS)/bin/TEABOX.BIN  $(BASE)/DE/

# Compiling ulextras stuff
	@echo "Compiling __DEP_L5"
	@make -C $(__DEP_L5)/

# X-like applications
	@-cp $(L5_XAPPS)/bin/XTB.BIN  $(BASE)/DE

# cpp application example
	@-cp $(L5_CPP00)/bin/CPP00.BIN  $(BASE)/DE

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

	-rm iceboot/arch/*.VHD 

# ==================
# (1) iceboot/arch/
# Clear boot images
#	-rm -rf iceboot/arch/arm/bin/*.BIN
	-rm -rf iceboot/arch/x86/bin/*.BIN

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
# $(__DEP_L1)/

	-rm $(__DEP_L1)/core/netd/client/*.o
	-rm $(__DEP_L1)/core/netd/client/*.BIN
	-rm $(__DEP_L1)/core/netd/server/*.o
	-rm $(__DEP_L1)/core/netd/server/*.BIN 

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

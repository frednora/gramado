# It builds the whole opearating system.

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


## =================================
# Kernel Services: Init process, ring 3 drivers and ring 3 servers.
USERLAND_L1 = userland
# Unix-like commands
COMMANDS = $(USERLAND_L1)/cmds

## =================================
# Shell Pre-UI: The display server.
# The infrastruture for the windows.
DEP_L2_LAST = winu/core
# Display servers
DISPLAY_SERVERS = $(DEP_L2_LAST)/ds
# Display server with embedded 3D demos
GAMES = $(DEP_L2_LAST)/ds3d

## =================================
# Shell UI: Client-side GUI applications.
DEP_L3 = apps

# Client-side GUI applications
APPLICATIONS = apps

## =================================
# userland extras
USERLAND_EXTRAS = ulextras
# Client-side GUI applications with X library
ULEXTRAS_X = $(USERLAND_EXTRAS)/xapps
# Creating one cpp application just for fun
ULEXTRAS_CPP00 = $(USERLAND_EXTRAS)/cpp00

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

# --------------------------------------
#::0
# ~ Step 0: gramado files.
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

#===================================
# (1) gramboot/boot/ 

# ::Build the bootloader.
	@echo "Compiling boot/"
	@$(MAKE) -C gramboot/boot/

	@echo "Installing boot/"

# Copy the virtual disk into the rootdir.
	@cp gramboot/boot/GRAMHV.VHD  .

# Copy the bootloader into the rootdir.
	@cp gramboot/boot/x86/bin/BM.BIN      $(BASE)/
	@cp gramboot/boot/x86/bin/BM2.BIN     $(BASE)/
	@cp gramboot/boot/x86/bin/BLGRAM.BIN  $(BASE)/
	@cp gramboot/boot/x86/bin/MBR0.BIN    $(BASE)/
	@cp gramboot/boot/x86/bin/APX86.BIN   $(BASE)/
# Copy the bootloader into the GRAMADO/ directory.
	@cp gramboot/boot/x86/bin/BM.BIN      $(BASE)/GRAMADO
	@cp gramboot/boot/x86/bin/BM2.BIN     $(BASE)/GRAMADO
	@cp gramboot/boot/x86/bin/BLGRAM.BIN  $(BASE)/GRAMADO
	@cp gramboot/boot/x86/bin/MBR0.BIN    $(BASE)/GRAMADO

# Copy the bootloader into the DE/ directory.
#	@cp gramboot/boot/x86/bin/APX86.BIN   $(BASE)/DE   

#===================================
# (2) kernel/

# ::Build kernel image.
	@echo "Compiling kernel/"
	@$(MAKE) -C kernel/

	@echo "Installing kernel/"

# Copy the kernel to the standard system folder.
	@cp kernel/KERNEL.BIN  $(BASE)/GRAMADO
# Create a backup; The bootloder expects this.
	@cp kernel/KERNEL.BIN  $(BASE)/DE

#===================================
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

# ...

#===================================
# LEVEL : ucore/
	@echo "Compiling USERLAND_L1"
	@$(MAKE) -C $(USERLAND_L1)

	@echo "Installing USERLAND_L1"

# Copy the init process.
	@cp $(USERLAND_L1)/init/src/bin/INIT.BIN  $(BASE)/
#	@cp $(USERLAND_L1)/init/src/bin/INIT.BIN  $(BASE)/GRAMADO/

#===================================
# $(USERLAND_L1)/drivers/ in kernel project

	@-cp $(USERLAND_L1)/drivers/bin/VGAD.BIN  $(BASE)/GRAMADO/

#===================================
# $(USERLAND_L1)/servers/ in kernel project

	@-cp $(USERLAND_L1)/servers/bin/NET.BIN   $(BASE)/GRAMADO/
	@-cp $(USERLAND_L1)/servers/bin/NETD.BIN  $(BASE)/GRAMADO/

	@echo "~build-gramado-os end?"

# --------------------------------------
# Let's add a bit of shame in the project.
PHONY := build-extras
build-extras:

	@echo "build-extras"

# ==================================
# LEVEL : Display servers
	@echo "Compiling DEP_L2_LAST (winu/core/)"
	@make -C $(DEP_L2_LAST)/

	@echo "Installing DEP_L2_LAST (winu/core/)"

	@-cp $(DISPLAY_SERVERS)/ds00/bin/DS00.BIN    $(BASE)/DE
#@-cp $(DISPLAY_SERVERS)/ds01/bin/DS01.BIN    $(BASE)/DE
# Display servers with 3D demos.
	@-cp $(GAMES)/bin/DEMO00.BIN   $(BASE)/DE/
	@-cp $(GAMES)/bin/DEMO01.BIN   $(BASE)/DE/


# ==================================
# LEVEL : (os/) Client-side GUI applications
	@echo "Compiling DEP_L3 (shell/)"
	@make -C $(DEP_L3)/

	@echo "Installing DEP_L3 (shell/)"

#===================================
# Install BMPs from cali assets.
# Copy the $(DEP_L3)/assets/
# We can't survive without this one.
	@cp apps/assets/themes/theme01/*.BMP  $(BASE)/DE

# Well consolidated programs.
	@-cp $(COMMANDS)/base/bin/PUBSH.BIN    $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/base/bin/PUBSH.BIN    $(BASE)/DE/
	@-cp $(COMMANDS)/base/bin/SHELL.BIN    $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/base/bin/SHELL.BIN    $(BASE)/DE/
	@-cp $(COMMANDS)/base/bin/SHELLZZ.BIN  $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/base/bin/SHELLZZ.BIN  $(BASE)/DE/

# Experimental programs.
	@-cp $(COMMANDS)/base/bin/SH7.BIN        $(BASE)/GRAMADO/
#	@-cp $(COMMANDS)/base/bin/SHELLXXX.BIN   $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/extra/bin/TASCII.BIN     $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/extra/bin/TPRINTF.BIN    $(BASE)/GRAMADO/

#===================================

# Copy well consolidated commands.
	@-cp $(COMMANDS)/base/bin/CAT.BIN       $(BASE)/
	@-cp $(COMMANDS)/base/bin/CAT00.BIN     $(BASE)/
	@-cp $(COMMANDS)/base/bin/REBOOT.BIN    $(BASE)/
	@-cp $(COMMANDS)/base/bin/REBOOT.BIN    $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/base/bin/SHUTDOWN.BIN  $(BASE)/
	@-cp $(COMMANDS)/base/bin/SHUTDOWN.BIN  $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/base/bin/UNAME.BIN     $(BASE)/

# Experimental commands.
	@-cp $(COMMANDS)/base/bin/FALSE.BIN      $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/base/bin/TRUE.BIN       $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/extra/bin/CMP.BIN       $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/extra/bin/SHOWFUN.BIN   $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/extra/bin/SUM.BIN       $(BASE)/GRAMADO/
	@-cp $(COMMANDS)/sdk/bin/GRAMCNF.BIN     $(BASE)/
#@-cp $(COMMANDS)/sdk/bin/N9.BIN         $(BASE)/GRAMADO/
#@-cp $(COMMANDS)/sdk/bin/N10.BIN        $(BASE)/GRAMADO/
#@-cp $(COMMANDS)/sdk/bin/N11.BIN        $(BASE)/GRAMADO/
#@-cp $(COMMANDS)/extra/bin/UDPTEST.BIN  $(BASE)/GRAMADO/

	@-cp $(APPLICATIONS)/bin/TASKBAR.BIN    $(BASE)/DE
	@-cp $(APPLICATIONS)/bin/MENUAPP.BIN    $(BASE)/DE
	@-cp $(APPLICATIONS)/bin/LAUNCH.BIN     $(BASE)/DE
	@-cp $(APPLICATIONS)/bin/TERMINAL.BIN   $(BASE)/DE
	@-cp $(APPLICATIONS)/bin/TERM00.BIN     $(BASE)/DE
#@-cp $(APPLICATIONS)/bin/GWS.BIN       $(BASE)/DE
    # Experimental applications
    # These need the '#' prefix.
	@-cp $(APPLICATIONS)/bin/PUBTERM.BIN  $(BASE)/DE/
	@-cp $(APPLICATIONS)/bin/DOC.BIN      $(BASE)/DE/
	@-cp $(APPLICATIONS)/bin/EDITOR.BIN   $(BASE)/DE/

	@-cp $(APPLICATIONS)/bin/GDM.BIN      $(BASE)/DE/
	@-cp $(APPLICATIONS)/bin/SETUP.BIN    $(BASE)/DE/

# (browser/) browser.
# Teabox web browser
    # Experimental applications
    # These need the '@' prefix.
	@-cp $(APPLICATIONS)/bin/TEABOX.BIN  $(BASE)/DE/


# Compiling ulextras stuff
	@echo "Compiling DEP_L3 (ulextras/)"
	@make -C $(USERLAND_EXTRAS)/

# X-like applications
	@-cp $(ULEXTRAS_X)/bin/XTB.BIN  $(BASE)/DE

# cpp application example
	@-cp $(ULEXTRAS_CPP00)/bin/CPP00.BIN  $(BASE)/DE

	@echo "~ build-extras"

# --------------------------------------
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

	-rm gramboot/boot/*.VHD 

# ==================
# (1) gramboot/boot/
# Clear boot images
#	-rm -rf gramboot/boot/arm/bin/*.BIN
	-rm -rf gramboot/boot/x86/bin/*.BIN

# ==================
# (2) kernel/
# Clear kernel image
	-rm kernel/*.o
	-rm kernel/*.BIN
	-rm -rf kernel/KERNEL.BIN

# ==================
# (3) modules/
# Clear the ring0 module images
	-rm -rf modules/*.o
	-rm -rf modules/*.BIN
	-rm -rf modules/bin/*.BIN

# ==================
# $(USERLAND_L1)/

# Clear INIT.BIN
	-rm $(USERLAND_L1)/init/src/*.o
	-rm $(USERLAND_L1)/init/src/*.BIN 
	-rm $(USERLAND_L1)/init/src/bin/*.BIN 

	-rm $(USERLAND_L1)/servers/netd/client/*.o
	-rm $(USERLAND_L1)/servers/netd/client/*.BIN
	-rm $(USERLAND_L1)/servers/netd/server/*.o
	-rm $(USERLAND_L1)/servers/netd/server/*.BIN 

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

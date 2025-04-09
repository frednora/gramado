# It builds the whole opearating system.

# License: MIT License
# Compiling on gcc 11.4.0 (Ubuntu on wsl)
# Linking on ld 2.38

# Building full distributions into this directory.
DISTROS = distros

# Target directory for the binaries.
# The binaries compiled here will go to this directory.
BASE = $(DISTROS)/base00

# Client-side GUI applications.
DEP_L3 = os
# The display server.
DEP_L2 = windows
# Init process, ring 3 drivers and ring 3 servers.
DEP_L1 = zbase
# Boot, kernel and ring 0 modules.
DEP_L0 = zcore

# --------------------------
# Display servers
DISPLAY_SERVERS = $(DEP_L2)/ds
# windowing system with 3D demos
GAMES           = $(DEP_L2)/gramadox

# --------------------------
# Client-side GUI applications
APPLICATIONS = $(DEP_L3)/apps


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

# --------------------------------------
# == Start ====
# build: User command.
PHONY := all
all:  \
build-gramado-os \
copy-extras \
$(DEP_L3)/gramvd \
vhd-mount \
vhd-copy-files \
vhd-unmount \
clean    

# Giving permitions to run.
	chmod 755 ./run
	chmod 755 ./runnokvm

# tests
	chmod 755 ./runt1
	chmod 755 ./runt2
	@echo "Done?"

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
$(DEP_L3)/gramvd    \
vhd-mount          \
vhd-copy-files     \
vhd-unmount        \

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
# (1) $(DEP_L0)/boot/ 

# ::Build stuuf in $(DEP_L0)/boot/
	$(Q)$(MAKE) -C $(DEP_L0)/boot/

# Copy the virtual disk into the rootdir.
	cp $(DEP_L0)/boot/GRAMHV.VHD  .
# Copy $(DEP_L0)/bootloader stuff into rootdir.
	cp $(DEP_L0)/boot/x86/bsp/bin/BM.BIN      $(BASE)/
	cp $(DEP_L0)/boot/x86/bsp/bin/BM2.BIN     $(BASE)/
	cp $(DEP_L0)/boot/x86/bsp/bin/BLGRAM.BIN  $(BASE)/
	cp $(DEP_L0)/boot/MBR0.BIN                $(BASE)/
# Copy bootloader stuff into GRAMADO/ folder.
	cp $(DEP_L0)/boot/x86/bsp/bin/BM.BIN      $(BASE)/GRAMADO
	cp $(DEP_L0)/boot/x86/bsp/bin/BM2.BIN     $(BASE)/GRAMADO
	cp $(DEP_L0)/boot/x86/bsp/bin/BLGRAM.BIN  $(BASE)/GRAMADO
	cp $(DEP_L0)/boot/MBR0.BIN                $(BASE)/GRAMADO

#===================================
# (2) $(DEP_L0)/kernel/

# ::Build kernel image.
	$(Q)$(MAKE) -C $(DEP_L0)/kernel/

# Copy the kernel to the standard system folder.
	cp $(DEP_L0)/kernel/KERNEL.BIN  $(BASE)/GRAMADO
# Create a backup; The bootloder expects this.
	cp $(DEP_L0)/kernel/KERNEL.BIN  $(BASE)/DE

#===================================
# (3) $(DEP_L0)/modules/

# ::Build the ring0 module image.
	$(Q)$(MAKE) -C $(DEP_L0)/modules/

# Copy the ring0 module image.
# It is loadable, but it's not a dynlinked format.
	cp $(DEP_L0)/modules/bin/HVMOD0.BIN  $(BASE)/
	cp $(DEP_L0)/modules/bin/HVMOD0.BIN  $(BASE)/GRAMADO

# Copy the ring0 module image.
# It is loadable, but it's not a dynlinked format.
#	cp $(DEP_L0)/modules/bin/HVMOD1.BIN  $(BASE)/
#	cp $(DEP_L0)/modules/bin/HVMOD1.BIN  $(BASE)/GRAMADO

# ...

#===================================
# $(DEP_L1)/usys/
# Build and copy init process and some commands.

	$(Q)$(MAKE) -C $(DEP_L1)/usys/

# Copy the init process.
	cp $(DEP_L1)/usys/bin/INIT.BIN  $(BASE)/

# Well consolidated applications
	-cp $(DEP_L1)/usys/bin/PUBSH.BIN      $(BASE)/DE/
	-cp $(DEP_L1)/usys/bin/SH7.BIN        $(BASE)/DE/
	-cp $(DEP_L1)/usys/bin/SHELL.BIN      $(BASE)/DE/
#	-cp $(DEP_L1)/usys/bin/SHELL00.BIN    $(BASE)/DE/
	-cp $(DEP_L1)/usys/bin/TASCII.BIN     $(BASE)/DE/
	-cp $(DEP_L1)/usys/bin/TPRINTF.BIN    $(BASE)/DE/

#===================================
# $(DEP_L1)/usys/commands/

	$(Q)$(MAKE) -C $(DEP_L1)/usys/commands/

# Copy some basic commands.
	-cp $(DEP_L1)/usys/commands/base/bin/CAT.BIN       $(BASE)/
	-cp $(DEP_L1)/usys/commands/base/bin/UNAME.BIN     $(BASE)/
	-cp $(DEP_L1)/usys/commands/base/bin/REBOOT.BIN    $(BASE)/
	-cp $(DEP_L1)/usys/commands/base/bin/SHUTDOWN.BIN  $(BASE)/
	-cp $(DEP_L1)/usys/commands/sdk/bin/GRAMCNF.BIN    $(BASE)/

# Experimental applications
	-cp $(DEP_L1)/usys/commands/base/bin/FALSE.BIN      $(BASE)/DE/
	-cp $(DEP_L1)/usys/commands/base/bin/TRUE.BIN       $(BASE)/DE/
	-cp $(DEP_L1)/usys/commands/extra/bin/CMP.BIN       $(BASE)/DE/
	-cp $(DEP_L1)/usys/commands/extra/bin/SHOWFUN.BIN   $(BASE)/DE/
	-cp $(DEP_L1)/usys/commands/extra/bin/SUM.BIN       $(BASE)/DE/
#-cp $(DEP_L1)/usys/commands/sdk/bin/N9.BIN         $(BASE)/DE/
#-cp $(DEP_L1)/usys/commands/sdk/bin/N10.BIN        $(BASE)/DE/
#-cp $(DEP_L1)/usys/commands/sdk/bin/N11.BIN        $(BASE)/DE/
#-cp $(DEP_L1)/usys/commands/extra/bin/UDPTEST.BIN  $(BASE)/DE/

#===================================
# $(DEP_L1)/udrivers/ in kernel project

	$(Q)$(MAKE) -C $(DEP_L1)/udrivers/
	-cp $(DEP_L1)/udrivers/bin/VGAD.BIN  $(BASE)/GRAMADO/

#===================================
# $(DEP_L1)/uservers/ in kernel project
# Build the network server and the first client.

	$(Q)$(MAKE) -C $(DEP_L1)/uservers/
	-cp $(DEP_L1)/uservers/bin/NET.BIN   $(BASE)/GRAMADO/
	-cp $(DEP_L1)/uservers/bin/NETD.BIN  $(BASE)/GRAMADO/

#===================================
# Install BMPs from cali assets.
# Copy the $(DEP_L3)/assets/
# We can't survive without this one.
#	cp $(DEP_L3)/assets/themes/theme01/*.BMP  $(BASE)/
	cp $(DEP_L3)/assets/themes/theme01/*.BMP  $(BASE)/DE

	@echo "~build-gramado-os end?"

# --------------------------------------
# Let's add a bit of shame in the project.
PHONY := copy-extras
copy-extras:

	@echo "copy-extras"

# ------------------------
# LEVEL : Display servers
	make -C windows/
	-cp $(DISPLAY_SERVERS)/ds00/bin/DS00.BIN    $(BASE)/DE
#-cp $(DISPLAY_SERVERS)/ds01/bin/DS01.BIN    $(BASE)/DE
# 3D demos.
	-cp $(GAMES)/bin/DEMO00.BIN   $(BASE)/DE/
	-cp $(GAMES)/bin/DEMO01.BIN   $(BASE)/DE/

# ------------------------
# LEVEL : (de/apps) Client-side GUI applications
	make -C $(DEP_L3)/
	-cp $(APPLICATIONS)/bin/TASKBAR.BIN    $(BASE)/DE
	-cp $(APPLICATIONS)/bin/XTB.BIN        $(BASE)/DE
	-cp $(APPLICATIONS)/bin/TERMINAL.BIN   $(BASE)/DE
#-cp $(APPLICATIONS)/bin/GWS.BIN       $(BASE)/DE
    # Experimental applications
    # These need the '#' prefix.
	-cp $(APPLICATIONS)/bin/PUBTERM.BIN  $(BASE)/DE/
	-cp $(APPLICATIONS)/bin/DOC.BIN      $(BASE)/DE/
	-cp $(APPLICATIONS)/bin/GDM.BIN      $(BASE)/DE/
	-cp $(APPLICATIONS)/bin/EDITOR.BIN   $(BASE)/DE/

# (browser/) browser.
# Teabox web browser
    # Experimental applications
    # These need the '@' prefix.
	-cp $(APPLICATIONS)/browser/teabox/bin/TEABOX.BIN  $(BASE)/DE/

	@echo "~ copy-extras"

# --------------------------------------
#::2
# Step 2: $(DEP_L3)/gramvd  - Creating the directory to mount the VHD.
$(DEP_L3)/gramvd:
	@echo "========================="
	@echo "Build: Creating the directory to mount the VHD ..."
	sudo mkdir $(DEP_L3)/gramvd

# --------------------------------------
#::3
# ~ Step 3: vhd-mount - Mounting the VHD.
vhd-mount:
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	-sudo umount $(DEP_L3)/gramvd
	sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD  $(DEP_L3)/gramvd/

# --------------------------------------
#::4
# ~ Step 4 vhd-copy-files - Copying files into the mounted VHD.
# Copying the $(BASE)/ folder into the mounted VHD.
vhd-copy-files:
	@echo "========================="
	@echo "Build: Copying files into the mounted VHD ..."
	# Copy $(BASE)/
	# sends everything from disk/ to root.
	sudo cp -r $(BASE)/*  $(DEP_L3)/gramvd

# --------------------------------------
#:::5
# ~ Step 5 vhd-unmount  - Unmounting the VHD.
vhd-unmount:
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	sudo umount $(DEP_L3)/gramvd

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


# --------------------------------------
# Basic clean.
clean:
	-rm *.o
	-rm *.BIN
	-rm $(DEP_L0)/kernel/*.o
	-rm $(DEP_L0)/kernel/*.BIN
	@echo "~clean"

# --------------------------------------
# Clean up all the mess.
clean-all: clean

	-rm *.o
	-rm *.BIN
	-rm *.VHD
	-rm *.ISO

	-rm $(DEP_L0)/boot/*.VHD 

# ==================
# (1) $(DEP_L0)/boot/
# Clear boot images
#	-rm -rf $(DEP_L0)/boot/arm/bin/*.BIN
	-rm -rf $(DEP_L0)/boot/x86/bin/*.BIN

# ==================
# (2) $(DEP_L0)/kernel/
# Clear kernel image
	-rm $(DEP_L0)/kernel/*.o
	-rm $(DEP_L0)/kernel/*.BIN
	-rm -rf $(DEP_L0)/kernel/KERNEL.BIN

# ==================
# (3) $(DEP_L0)/modules/
# Clear the ring0 module images
	-rm -rf $(DEP_L0)/modules/*.o
	-rm -rf $(DEP_L0)/modules/*.BIN
	-rm -rf $(DEP_L0)/modules/bin/*.BIN

# ==================
# $(DEP_L1)/usys/
# Clear INIT.BIN
	-rm $(DEP_L1)/usys/bin/*.BIN
	-rm $(DEP_L1)/usys/init/*.o
	-rm $(DEP_L1)/usys/init/*.BIN 

	-rm $(DEP_L1)/uservers/netd/client/*.o
	-rm $(DEP_L1)/uservers/netd/client/*.BIN
	-rm $(DEP_L1)/uservers/netd/server/*.o
	-rm $(DEP_L1)/uservers/netd/server/*.BIN 

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

qemu-instance:
	-cp ./GRAMHV.VHD ./QEMU.VHD 
#xxx-instance:
#	-cp ./GRAMHV.VHD ./XXX.VHD 


# End


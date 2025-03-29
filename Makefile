# Polar D8 Kernel
# Codename: Lamb

# License: MIT License
# Compiling on gcc 11.4.0 (Ubuntu on wsl)
# Linking on ld 2.38

# --------------------------
# Display server
DEP_L1  = windows/ds

# --------------------------
# zde: Client-side GUI applications
DEP_L2  = zde/apps
BASE    = zde/base
GAMES   = zde/aurora

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
zde/GRAMVD \
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
zde/GRAMVD    \
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
# (1) os/boot/ 

# ::Build stuuf in os/boot/
	$(Q)$(MAKE) -C os/boot/

# Copy stuff created in os/boot/

# Copy the virtual disk into the rootdir.
	cp os/boot/GRAMHV.VHD  .
# Copy os/bootloader stuff into rootdir.
	cp os/boot/x86/bsp/bin/BM.BIN      $(BASE)/
	cp os/boot/x86/bsp/bin/BM2.BIN     $(BASE)/
	cp os/boot/x86/bsp/bin/BLGRAM.BIN  $(BASE)/
# Copy bootloader stuff into GRAMADO/ folder.
	cp os/boot/x86/bsp/bin/BM.BIN      $(BASE)/GRAMADO
	cp os/boot/x86/bsp/bin/BM2.BIN     $(BASE)/GRAMADO
	cp os/boot/x86/bsp/bin/BLGRAM.BIN  $(BASE)/GRAMADO
	cp os/boot/MBR0.BIN                $(BASE)/GRAMADO

#===================================
# (2) os/kernel/

# ::Build kernel image.
	$(Q)$(MAKE) -C os/kernel/

# Copy to the target folder.
# We need a backup
# The bootloader expect this.
# todo: We need to rethink this backup.
	cp os/kernel/KERNEL.BIN  $(BASE)/GRAMADO
	cp os/kernel/KERNEL.BIN  $(BASE)/DE

#===================================
# (3) os/mods/

# ::Build the ring0 module image.
	$(Q)$(MAKE) -C os/mods/

# Copy the ring0 module image.
# Not dynlinked modules.
	cp os/mods/bin/HVMOD0.BIN  $(BASE)/
#	cp os/mods/bin/HVMOD1.BIN  $(BASE)/
	cp os/mods/bin/HVMOD0.BIN  $(BASE)/GRAMADO
#	cp os/mods/bin/HVMOD1.BIN  $(BASE)/GRAMADO

# ...

#===================================
# os/usys/ in kernel project
# Build the init process.
# Copy the init process.
# We can't survive without this one. (Only this one).
	$(Q)$(MAKE) -C os/usys/
	$(Q)$(MAKE) -C os/usys/commands/

# Copy

	cp os/usys/bin/INIT.BIN      $(BASE)/
	-cp os/usys/commands/base/bin/REBOOT.BIN    $(BASE)/
	-cp os/usys/commands/base/bin/SHUTDOWN.BIN  $(BASE)/
	-cp os/usys/commands/sdk/bin/GRAMCNF.BIN    $(BASE)/

    # Well consolidated applications
	-cp os/usys/bin/PUBSH.BIN                $(BASE)/DE/
	-cp os/usys/bin/SH7.BIN                  $(BASE)/DE/
	-cp os/usys/bin/SHELL.BIN                $(BASE)/DE/
#	-cp os/usys/bin/SHELL00.BIN              $(BASE)/DE/
	-cp os/usys/bin/TASCII.BIN               $(BASE)/DE/
	-cp os/usys/bin/TPRINTF.BIN              $(BASE)/DE/
	-cp os/usys/commands/base/bin/CAT.BIN    $(BASE)/DE/
	-cp os/usys/commands/base/bin/UNAME.BIN  $(BASE)/DE/

    # Experimental applications
	-cp os/usys/commands/base/bin/FALSE.BIN      $(BASE)/DE/
	-cp os/usys/commands/base/bin/TRUE.BIN       $(BASE)/DE/
	-cp os/usys/commands/extra/bin/CMP.BIN       $(BASE)/DE/
	-cp os/usys/commands/extra/bin/SHOWFUN.BIN   $(BASE)/DE/
	-cp os/usys/commands/extra/bin/SUM.BIN       $(BASE)/DE/
#-cp os/usys/commands/sdk/bin/N9.BIN         $(BASE)/DE/
#-cp os/usys/commands/sdk/bin/N10.BIN        $(BASE)/DE/
#-cp os/usys/commands/sdk/bin/N11.BIN        $(BASE)/DE/
#-cp os/usys/commands/extra/bin/UDPTEST.BIN  $(BASE)/DE/

#===================================
# os/udrivers/ in kernel project

	$(Q)$(MAKE) -C os/udrivers/
	-cp os/udrivers/bin/VGAD.BIN  $(BASE)/GRAMADO/

#===================================
# os/uservers/ in kernel project
# Build the network server and the first client.

	$(Q)$(MAKE) -C os/uservers/
	-cp os/uservers/bin/NET.BIN   $(BASE)/GRAMADO/
	-cp os/uservers/bin/NETD.BIN  $(BASE)/GRAMADO/

# Install BMPs from cali assets.
# Copy the zde/assets/
# We can't survive without this one.
#	cp zde/assets/themes/theme01/*.BMP  $(BASE)/
	cp zde/assets/themes/theme01/*.BMP  $(BASE)/DE

	@echo "~build-gramado-os end?"

# --------------------------------------
# Let's add a bit of shame in the project.
PHONY := copy-extras
copy-extras:

	@echo "copy-extras"

# ------------------------
# LEVEL 1: (de/ds) Display servers

	-cp $(DEP_L1)/ds00/bin/DS00.BIN    $(BASE)/DE
#-cp $(DEP_L1)/ds01/bin/DS01.BIN    $(BASE)/DE

# ------------------------
# LEVEL 2: (de/apps) Client-side GUI applications

	-cp $(DEP_L2)/bin/TASKBAR.BIN    $(BASE)/DE
	-cp $(DEP_L2)/bin/XTB.BIN        $(BASE)/DE
	-cp $(DEP_L2)/bin/TERMINAL.BIN   $(BASE)/DE
#-cp $(DEP_L2)/bin/GWS.BIN       $(BASE)/DE
    # Experimental applications
    # These need the '#' prefix.
	-cp $(DEP_L2)/bin/PUBTERM.BIN  $(BASE)/DE/
	-cp $(DEP_L2)/bin/DOC.BIN      $(BASE)/DE/
	-cp $(DEP_L2)/bin/GDM.BIN      $(BASE)/DE/
	-cp $(DEP_L2)/bin/EDITOR.BIN   $(BASE)/DE/

# (browser/) browser.
# Teabox web browser
    # Experimental applications
    # These need the '@' prefix.
	-cp $(DEP_L2)/browser/teabox/bin/TEABOX.BIN  $(BASE)/DE/

# 3D demos.
	-cp $(GAMES)/bin/DEMO00.BIN   $(BASE)/DE/
	-cp $(GAMES)/bin/DEMO01.BIN   $(BASE)/DE/

	@echo "~ copy-extras"

# --------------------------------------
#::2
# Step 2: zde/GRAMVD  - Creating the directory to mount the VHD.
zde/GRAMVD:
	@echo "========================="
	@echo "Build: Creating the directory to mount the VHD ..."
	sudo mkdir zde/GRAMVD

# --------------------------------------
#::3
# ~ Step 3: vhd-mount - Mounting the VHD.
vhd-mount:
	@echo "=========================="
	@echo "Build: Mounting the VHD ..."
	-sudo umount zde/GRAMVD
	sudo mount -t vfat -o loop,offset=32256 GRAMHV.VHD zde/GRAMVD/

# --------------------------------------
#::4
# ~ Step 4 vhd-copy-files - Copying files into the mounted VHD.
# Copying the $(BASE)/ folder into the mounted VHD.
vhd-copy-files:
	@echo "========================="
	@echo "Build: Copying files into the mounted VHD ..."
	# Copy $(BASE)/
	# sends everything from disk/ to root.
	sudo cp -r $(BASE)/*  zde/GRAMVD

# --------------------------------------
#:::5
# ~ Step 5 vhd-unmount  - Unmounting the VHD.
vhd-unmount:
	@echo "======================"
	@echo "Build: Unmounting the VHD ..."
	sudo umount zde/GRAMVD

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
	-rm os/kernel/*.o
	-rm os/kernel/*.BIN
	@echo "~clean"

# --------------------------------------
# Clean up all the mess.
clean-all: clean

	-rm *.o
	-rm *.BIN
	-rm *.VHD
	-rm *.ISO

	-rm os/boot/*.VHD 

# ==================
# (1) os/boot/
# Clear boot images
#	-rm -rf os/boot/arm/bin/*.BIN
	-rm -rf os/boot/x86/bin/*.BIN

# ==================
# (2) os/kernel/
# Clear kernel image
	-rm os/kernel/*.o
	-rm os/kernel/*.BIN
	-rm -rf os/kernel/KERNEL.BIN

# ==================
# (3) os/mods/
# Clear the ring0 module images
	-rm -rf os/mods/*.o
	-rm -rf os/mods/*.BIN
	-rm -rf os/mods/bin/*.BIN

# ==================
# os/usys/
# Clear INIT.BIN
	-rm os/usys/bin/*.BIN
	-rm os/usys/init/*.o
	-rm os/usys/init/*.BIN 

	-rm os/uservers/netd/client/*.o
	-rm os/uservers/netd/client/*.BIN
	-rm os/uservers/netd/server/*.o
	-rm os/uservers/netd/server/*.BIN 

	-rm os/usys/commands/bin/*.BIN
	-rm os/usys/commands/init/*.o

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


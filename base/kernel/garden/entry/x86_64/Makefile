# Build the kernel header, for 64bit kernel.
# The final object is head_64.o  
# Created by Fred Nora.

#
# Config
#

include config.mk

ifeq ($(CONFIG_USE_VERBOSE),1)
# Not silent. It prints the commands.
	Q =
else
# silent
	Q = @
endif



objTARGET := head_64.o    

# Includes
KERNEL_INCLUDE   := -I ../../include 
STARTUP_INCLUDE  := -I startup/ 
ENTRY_INCLUDE    := -I entry/ 
INCLUDES := $(KERNEL_INCLUDE) $(STARTUP_INCLUDE) $(ENTRY_INCLUDE) 


DESTINATION_FOLDER := ../../../ 

MAKE = make
NASM = nasm
CC	= gcc
AS	= as
LD	= ld
AR	= ar

all:
# Startup code and entry code
	$(Q)$(NASM) head_64.asm $(INCLUDES) -f elf64 -o $(objTARGET) 
# Copy to destination folder
	$(Q)cp $(objTARGET) $(DESTINATION_FOLDER) 
# Clean unused objects
	$(Q)rm *.o 


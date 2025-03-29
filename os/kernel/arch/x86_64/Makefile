
# Build the kernel header, for 64bit kernel.
# The final object is head_64.o  

objTARGET := head_64.o    

KERNEL_INCLUDE   := -I ../../include
STARTUP_INCLUDE  := -I startup/    
ENTRY_INCLUDE    := -I entry/    
INCLUDES := $(KERNEL_INCLUDE) $(STARTUP_INCLUDE) $(ENTRY_INCLUDE)    

DESTINATION_FOLDER := ../../

AS	= as
LD	= ld
CC	= gcc
AR	= ar
MAKE	= make
NASM	= nasm
OBJCOPY	= objcopy
OBJDUMP	= objdump
LEX	= flex
YACC	= bison
PERL	= perl
PYTHON	= python
PYTHON2	= python2
PYTHON3	= python3
RUBY	= ruby

all:

# Startup code and entry code.
	$(NASM) head_64.asm $(INCLUDES) \
	-f elf64 -o $(objTARGET)    

# Copy to destination folder.
	cp $(objTARGET) $(DESTINATION_FOLDER)    

# Clean unused objects,
	rm *.o    


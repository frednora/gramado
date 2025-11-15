
# init process.

VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION =
#NAME = Init


# Make variables (CC, etc...)
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



#
# Objects
#

Objects = marginal_r0.o    



.PHONY: all all-c shell-link finalize clean

all: crt0.o main.o all-c shell-link finalize clean
	@echo "Init Ok?"
head.o:
	# Nothing.
crt0.o:
	nasm marginal_r0.asm -f elf64 -o marginal_r0.o 
main.o:
	# Nothing
all-c:
	# Nothing
shell-link:
	$(LD) -s -x -T link_r0.ld -o SM3.BIN $(Objects)    
	#$(LD) -s -x -T link_r0.ld -o SM3.BIN marginal_r0.o   
finalize:
#	cp SM.BIN ../../bin/

clean:
	-rm *.o
#	-rm SM.BIN

 



# Makefile for building all commands

all:

# Compile libraries and toolkits for the commands in cmds/.
	make -C alpha/

# Compile base commands
	make -C cat/
	make -C cat00/
#	make -C echo/
#	make -C false/
#	make -C true/

# Compile extra commands
#	make -C cmp/
#	make -C env/
#	make -C rect/
#	make -C replace/
#	make -C showfun/
#	make -C socket1/
#	make -C sum/
#	make -C tascii/
#	make -C tcrypt1/
#	make -C tcrypt2/
#	make -C template/
#	make -C tpass/
#	make -C tprintf/
#	make -C tty/
#	make -C udptest/
#	make -C unknown/

# Compile SDK commands.
#	make -C c4/
#	make -C gramc4/
	make -C gramcnf/
#	make -C miniyacc/
#	make -C n9/
#	make -C n10/
#	make -C n11/


clean:
	-rm -f *.o 
clean-all:
	-rm -f *.o 
#	-rm -f alpha/libs/
#	-rm -f alpha/toolkits/
	-rm -f bin/*.BIN



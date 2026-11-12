

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


all:

	@echo "\n"
	@echo "==== Compiling Gramado OS ===="

	@echo "\n"
	@echo "==== Compiling gramde ===="
	make -C gramde/

	@echo "\n"
	@echo "==== Compiling core ===="
	make -C core/

	@echo "\n"
	@echo "Done."


gramde:
	make -C gramde/
core:
	make -C core/

tools:
	echo "Build tool"
run:
	cd core/ && ./run  


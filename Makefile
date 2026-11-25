
DE_DIR = gramado
CMDS_DIR = cmds
BASE_DIR = base


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


build: all

all:

	@echo "\n"
	@echo "==== Compiling Gramado OS ===="

	@echo "\n"
	@echo "==== Compiling gramde ===="
	make -C $(DE_DIR)/

	@echo "\n"
	@echo "==== Compiling cmds ===="
	make -C $(CMDS_DIR)/

	@echo "\n"
	@echo "==== Compiling core ===="
	make -C $(BASE_DIR)/


	@echo "\n"
	@echo "Done."

all-base:
	cd $(BASE_DIR)/  && make all-base

de:
	make -C $(DE_DIR)/

cmds:
	make -C $(CMDS_DIR)/

base:
	make -C $(BASE_DIR)/

clean:
	cd $(DE_DIR)/    && make clean  
	cd $(CMDS_DIR)/  && make clean-all
	cd $(BASE_DIR)/  && make clean-all

clean-all:
	cd $(DE_DIR)/    && make clean  
	cd $(CMDS_DIR)/  && make clean-all
	cd $(BASE_DIR)/  && make clean-all

run:
	cd $(BASE_DIR)/ && ./run  


# shell01
# Client-side GUI applications.
# Desktop environment.

all:

# 1 - Libraries
	make -C api/

# 2 - DE core applications.
	make -C  taskbar/
	make -C  gdm/
	make -C  term00/

# 2 - DE Utilities
	make -C  doc/
	make -C  editor/
	make -C  memory/
	make -C  power/
	make -C  setup/
	make -C  sysinfo/

# 3 - Experiments
	make -C  bmenu/
	make -C  calc00/
	make -C  client/
	make -C  cmdline/
	make -C  dbox/
	make -C  draw/
	make -C  gwtest/
	make -C  launch/
	make -C  mbox/
	make -C  menuapp/

clean:
	-rm bin/*.BIN

clean-all:
	#-rm -f api/libs/libcpp/obj/*.o
	-rm -f api/libs/libdisp/obj/*.o
	-rm -f api/libs/libgr/obj/*.o
	-rm -f api/libs/libgr3d/obj/*.o
	-rm -f api/libs/libgws/obj/*.o
	-rm -f api/libs/libio01/obj/*.o
	-rm -f api/libs/rtl/obj/*.o

	-rm bin/*.BIN


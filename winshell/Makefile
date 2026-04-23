# shell01
# Client-side GUI applications.
# Desktop environment.

all:

# 1 - Libraries
	make -C api/

# 2 - DE core applications.
	make -C  taskbar/
	make -C  terminal/

# 2 - DE Utilities
	make -C  doc/
	make -C  editor/
	make -C  memory/
	make -C  power/
	make -C  sysinfo/

# 3 - Experiments
	make -C  draw/
	make -C  launch/
	make -C  menuapp/

# gws client example
	make -C gws/


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


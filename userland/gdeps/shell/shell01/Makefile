# shell01
# Client-side GUI applications.

all:
# Order of importance

# 1
# API: 
# Libraries and toolkits.
	make -C api/

# 2
# Taskbar
# "The explorer": desktop, taskbar, launcher, filemanager ...
# For now it is only a taskbar.
	make -C taskbar/

# 3
# Output entity for subsystems
	make -C ss/

# 4
# Normal programs.
	make -C sysapps/

# 5
# System programs.
	make -C sysshell/

# 6
# xapps
#	make -C xapps/

# test
	make -C menuapp/

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


# Client-side GUI applications.

all:
# Order of importance

# 1
# API: 
# Libraries and toolkits.
	make -C api/

# 2
# The explorer
# desktop, taskbar, launcher, filemanager ...
	make -C overview/

# 3
# Output entity for subsystems
	make -C  ss/pubterm/
	make -C  ss/terminal/
	make -C  ss/term00/
	make -C  ss/teabox/

# 4
# Normal programs.
	make -C sysapps/

# 5
# System programs.
	make -C sysshell/

# 6
# xapps
#	make -C xapps/

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


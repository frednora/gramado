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
	make -C  term00/

# 4
# Normal programs.
	make -C client/
	make -C cmdline/
	make -C doc/
	make -C editor/

# 5


# 6

# Apps created by Copilot
	make -C gwtest/
	make -C launch/
	make -C menuapp/
	make -C power/
	make -C memory/
	make -C dbox/
	make -C mbox/
	make -C sysinfo/
	make -C bmenu/
	make -C calc00/

# Gramado Desktop Manager
	make -C gdm/

# Setup program to install the o.s.
	make -C setup/

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


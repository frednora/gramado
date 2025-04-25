# Client-side GUI applications.

all:

# API: 
# Libraries and toolkits.
	make -C api/

# Apps
	make -C apps/

# Main system apps.

	# Gramado Desktop Manager
	make -C gdm/

	# Taskbar (The launcher)
	make -C taskbar/

	# Setup program to install the o.s.
	#make -C setup/ 

#
# Output entity for subsystems
#

	make -C  ss/pubterm/
	make -C  ss/terminal/
	make -C  ss/term00/
	make -C  ss/teabox/

#
# xapps
#

	make -C xapps/xtb/

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


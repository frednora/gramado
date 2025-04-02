# Client-side GUI applications.

all:

# API: 
# Libraries and toolkits.

	make -C api/

#
# External shell: The desktop. 
#

# Main system apps.

	# Gramado Desktop Manager
	make -C gdm/

	# Taskbar (The launcher)
	make -C taskbar/

#
# Internal shell: The applications.
#

# Apps for 'pro users'

	# Virtual terminal connected with the child.
	# It launches the child shell.
	make -C pubterm/

	# Virtual terminal 
	# with an embedded command line interpreter.
	make -C terminal/

	# First client (GWS.BIN)
	make -C client/
	make -C cmdline/

	# Setup program to install the o.s.
	#make -C setup/ 

# Apps for 'regular users'

	make -C doc/
	make -C editor/

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


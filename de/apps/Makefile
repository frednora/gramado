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
	make -C desktop/gdm/

	# Taskbar (The launcher)
	make -C desktop/taskbar/

#
# Internal shell: The applications.
#

# Apps for 'pro users'

	# Virtual terminal connected with the child.
	# It launches the child shell.
	make -C pro/pubterm/

	# Virtual terminal 
	# with an embedded command line interpreter.
	make -C pro/terminal/

	# First client (GWS.BIN)
	make -C pro/client/
	make -C pro/cmdline/

	# Setup programs to install the o.s.
	#make -C pro/setup/ 

# Apps for 'regular users'

	make -C programs/doc/
	make -C programs/editor/

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


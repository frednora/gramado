
all:


# Graphics support (Server-side?)
	make -C libgr/
# 3D Graphics support (Server-side?)
	make -C libgr3d/

# Client-side library for primitive drawing routines.
# This is an old version that draws using the dc but it is also 
# capable of drawing directly into the full screen.
# Not in use. Just backup.
#	make -C libgui00/

# Client-side library for primitive drawing routines.
# It uses the dc/canvas, not the display.
	make -C libgui/

# Gramado Window System
# Client-side library for the Display server.
# Using Gramado Protocol
	make -C libgws/

# toolkit created by Copilot
	make -C libgwt/

# I/O ports in ring3.
	make -C libio01/

clean:

	-rm -f libdisp/*.o
	-rm -f libdisp/obj/*.o

	-rm -f libgr/*.o
	-rm -f libgr/obj/*.o

	-rm -f libgr3d/*.o
	-rm -f libgr3d/obj/*.o

	-rm -f libgws/*.o
	-rm -f libgws/obj/*.o

	-rm -f libio01/*.o
	-rm -f libio01/obj/*.o






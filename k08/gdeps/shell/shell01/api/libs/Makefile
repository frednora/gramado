
all:

#	make -C libcpp/

	make -C libdisp/

# Graphics support (Server-side?)
	make -C libgr/
# 3D Graphics support (Server-side?)
	make -C libgr3d/

# Gramado Window System
# Client-side library for the Display server.
# Using Gramado Protocol
	make -C libgws/

# I/O ports in ring3.
	make -C libio01/

	make -C rtl/

clean:

	-rm -f libdisp/*.o
	-rm -f libdisp/obj/*.o

	-rm -f libgr/*.o
	-rm -f libgr/obj/*.o

	-rm -f libgr3d/*.o
	-rm -f libgr3d/obj/*.o

	-rm -f libgws/*.o
	-rm -f libgws/obj/*.o

	-rm -f libx/*.o
	-rm -f libx/obj/*.o

	-rm -f libio01/*.o
	-rm -f libio01/obj/*.o

	-rm -f rtl/*.o
	-rm -f rtl/obj/*.o






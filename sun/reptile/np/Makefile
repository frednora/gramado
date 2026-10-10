
all:

	make -C api/

	make -C winapps/
	make -C winshell/

clean:

	-rm -f api/garden/rtl/obj/*.o

	#-rm -f api/gthouse/libcpp/obj/*.o
	-rm -f api/gthouse/libdisp/obj/*.o
	-rm -f api/gthouse/libgr/obj/*.o
	-rm -f api/gthouse/libgr3d/obj/*.o
	-rm -f api/gthouse/libgws/obj/*.o
	-rm -f api/gthouse/libio01/obj/*.o


	-rm winapps/*.o
	-rm winapps/bin/*.BIN

	-rm winshell/*.o
	-rm winshell/bin/*.BIN

clean-all:

	-rm -f api/garden/rtl/obj/*.o

	#-rm -f api/gthouse/libcpp/obj/*.o
	-rm -f api/gthouse/libdisp/obj/*.o
	-rm -f api/gthouse/libgr/obj/*.o
	-rm -f api/gthouse/libgr3d/obj/*.o
	-rm -f api/gthouse/libgws/obj/*.o
	-rm -f api/gthouse/libio01/obj/*.o

	-rm winapps/*.o
	-rm winapps/bin/*.BIN

	-rm winshell/*.o
	-rm winshell/bin/*.BIN


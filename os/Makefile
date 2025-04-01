# Build the all in os/.
# This is the Desktop Environment.
# Client-side gui applications and wallpapers.
# Created by Fred Nora.

all:
# Client-side GUI applications
	make -C apps/
	make -C apps/browser/
# 3D games
	make -C gramadox/

clean:
# Client-side GUI applications
	-rm apps/bin/*.BIN
	-rm apps/browser/bin/*.BIN

clean-all:
# Client-side GUI applications
	-rm apps/bin/*.BIN
	-rm apps/browser/bin/*.BIN
# 3D games
	-rm gramadox/bin/*.BIN

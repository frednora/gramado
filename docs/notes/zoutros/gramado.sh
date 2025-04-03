
	echo "~~~~>"

	cd ..

	qemu-system-x86_64 -hda GRAMADO.VHD \
	-m 256 \
	-serial stdio \
	-netdev type=user,id=net1 -device e1000,netdev=net1


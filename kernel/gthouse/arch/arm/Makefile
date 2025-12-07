# Press Ctrl + A, then X to quit QEMU.

all:
	aarch64-linux-gnu-as boot.s -o boot.o
	aarch64-linux-gnu-ld boot.o -o boot.elf
	aarch64-linux-gnu-objcopy -O binary boot.elf boot.bin

run:
	qemu-system-aarch64 -machine virt -cpu cortex-a72 \
-nographic -kernel boot.bin


clean:
	rm *.o 
clean-all:
	rm *.o 
	rm *.elf 
	rm *.bin 


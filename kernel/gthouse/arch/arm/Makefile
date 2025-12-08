# Press Ctrl + A, then X to quit QEMU.
# (That's the QEMU monitor escape sequence when running in -nographic mode.)

# -------------------------------------------------------------------
# Build rules
# -------------------------------------------------------------------
all:
# Assemble your boot.s into an object file
	aarch64-linux-gnu-as boot.s -o boot.o
# Compile your C file into an object file
	aarch64-linux-gnu-gcc -ffreestanding -nostdlib -c blmain.c -o blmain.o
# Link the object file into an ELF executable
	aarch64-linux-gnu-ld -T linker.ld -o boot.elf boot.o blmain.o 
# Convert the ELF into a raw binary (no headers, just instructions/data)
	aarch64-linux-gnu-objcopy -O binary boot.elf boot.bin

# -------------------------------------------------------------------
# Run rule
# -------------------------------------------------------------------
run:
# Launch QEMU with:
# - AArch64 system emulation
# - "virt" machine (generic ARM virtual board with PL011 UART, GIC, etc.)
# - Cortex-A72 CPU model
# - No graphics (console only)
# - Load your raw binary as the kernel
	qemu-system-aarch64 -machine virt -cpu cortex-a72 \
-nographic -kernel boot.bin

# -------------------------------------------------------------------
# Cleanup rules
# -------------------------------------------------------------------
clean:
	-rm *.o 
clean-all:
	-rm *.o 
	-rm *.elf 
	-rm *.bin 


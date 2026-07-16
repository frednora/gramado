# boot - Gramado Boot Loader

```
Gramado Boot Loader. 
This is an external boot loader. It's not part of the kernel image.
It's able to load a kernel image, normally called KERNEL.BIN.
This is our own GRUB. 
```

## The boot loader

Main parts of the boot loader. (MBR | Boot manager | Boot loader)

## Where is the MBR code?

The code for the x86 16bit MBR is found in vd/fat/main.asm.

## Where is the boot manager?

The boot manager is a 16/32 bit Assembly program the run right after the MBR. It is found in x86/bsp/bm/ and x86/bsp/bm2/.

## Where is the boot loader?

The boot laoder program is 32bit C program the load the kernel. It is found in the x86/bsp/blgram/ folder.

## Memory map for the Gramado OS initialization.

--------------------------------------------------
Memory map for the stage 1:
This is the moment when the MBR 
loads the file BM.BIN at 0x0000:0x8000.

```
    +--------+
    |        |
    |--------|
    | FAT    |
    |--------| 0x17C0:0x0200
    |        |
    |--------|
    |        |
    | BM.BIN |
    |        |
    |--------| 0x0000:0x8000
    |        |
    |--------|
    | ROOT   |
    | DIR    |
    |--------| 0x07C0:0x0200
    |BOOT SEC| 
    |ORIGIN  | 
    |--------| 0x07C0:0x0000 <<<
    |        |
    |--------| 0x0000:0x6000
    |INITIAL | Initial stack address.
    |STACK   | It goes down.
    |--------| 
    |        |
    +--------+
```

--------------------------------------------------
Memory map for the BM:
This is the moment when the BM.BIN 
loads the file BL.BIN at 0x2000:0x0000.

```
    +----------+
    |          | 
    |----------| 
    | BL.BIN   | 
    |          | 
    |----------| 0x2000:0x0000
    |          | 
    |----------| 
    |          | 
    | BM.BIN   | 
    |          | The entry point.
    |----------| 0x0000:0x8000 <<<
    |          |
    |----------| 0x0000:0x6000
    | INITIAL  | Initial stack address.
    | STACK    | It goes down.
    |----------| 
    |          |
    |----------| 
    | FAT/ROOT |
    |----------| 0x0000:0x1000
    |          |
    +----------+
```

--------------------------------------------------
Memory map for the BL:
This is the moment when the BL.BIN 
loads the file KERNEL.BIN at 0x100000.

```
    +-------------+
    |             |
    | KERNEL.BIN  |
    | Entry point | 0x00101000
    |-------------| 0x00100000
    |             |
    |             |
    |-------------|
    | BL.BIN      |
    | Entry point | 0x00021000
    |-------------| 0x00020000 <<<
    |             |
    +-------------+
```













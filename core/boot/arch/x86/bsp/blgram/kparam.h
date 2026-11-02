// kparam.h
// The parameters passed from this bootloader to the kernel image.
// Created by Fred Nora.

#ifndef __KPARAM_H
#define __KPARAM_H    1

//
// Bootloader Parameters for 64-bit Kernel
// ----------------------------------------
//
// The 32-bit bootloader prepares a boot block to pass key parameters
// to the 64-bit kernel. The boot block is located at a fixed memory location:
//     Base Address = 0x00090000
//
// Registers:
//     RBX = Base address for the boot block (0x00090000)
//     RDX = Magic signature (used to verify the boot block)
//
// Boot Block Layout:
// Each field occupies 64 bits (8 bytes). The following fields are defined 
// relative to the base address. (Field offsets use zero-based indexing;
// i.e., the starting byte of Field i is: Offset = (i * 8).)
//
// Field 0: LFB_VA (Linear Frame Buffer Virtual Address)
//   - Located at bytes:    0–7
//   - Address: Base + 0
//
// Field 1: WIDTH (Screen Width)
//   - Located at bytes:    8–15
//   - Address: Base + 8
//
// Field 2: HEIGHT (Screen Height)
//   - Located at bytes:    16–23
//   - Address: Base + 16
//
// Field 3: BPP (Bits-Per-Pixel)
//   - Located at bytes:    24–31
//   - Address: Base + 24
//
// Field 4: LAST_VALID (Last Valid Physical Address)
//   - Lower 32 bits: Contains the valid 32-bit physical address.
//   - Upper 32 bits: Contains its complement (typically 0x00000000).
//   - Located at bytes:    32–39
//     * Note: The 32-bit value is at Base + 32, and its complement is at 
//             Base + 36 (i.e. 4 bytes offset inside the 8-byte field).
//
// Field 5: Gramado Mode
//   - Represents the operational mode (e.g., jail, p1, home, etc.).
//   - Located at bytes:    40–47
//   - Address: Base + 40
//
// Field 6: IDE Port Number (Deprecated/Test Field)
//   - Previously used for the IDE port number (now deprecated).
//   - Located at bytes:    48–55
//   - Address: Base + 48
//
// Field 7: Disk Signature
//   - Contains the boot disk signature passed by the bootloader (from blgram/bm2).
//   - Located at bytes:    56–63
//   - Address: Base + 56
//
// #todo:
// These boot parameters must be passed along to the kernel – combined with additional
// initialization values set up during early assembly (see head.s) – to ensure the 64-bit
// kernel receives all required information.
// 
// (Even though the bootloader is 32-bit, it constructs this table so that the kernel, which
// expects 64-bit values, will correctly interpret each field.)


// Base address for the bootblock
#define KPARAM_BOOTBLOCK_ADDRESS    0x00090000


#endif   



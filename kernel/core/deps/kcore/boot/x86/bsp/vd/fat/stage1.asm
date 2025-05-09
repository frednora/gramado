; state1.asm
;     Gramado MBR
;     2005 - Created by Fred Nora.
;     This file marks the beginning of the Gramado Project.

; What is this?
; This is the MBR for the Gramado Boot Loader.
; During the build time, a VHD disk is created using nasm,
; and at the beginning of the assembly code we have have
; this MBR code.
; The disk size is 32MB or 64MB.
; The fist partition is formated with FAT16 file system.
; It has 512 bytes per sector and 1 sector per cluster.

; Purpose:
; The main goal of this code is loading BM.BIN or BM2.BIN,
; from the first partition to the main memory.
; It passes some parameters an jump to the loaded file.

; #important
; Do not change 'Segments' and 'Offsets' used here,
; it's kinda canonical.

;; Partition table:
;; See:
;;     https://thestarman.pcministry.com/asm/mbr/PartTables.htm
;; org = 0
;; See: main.asm 

;       +--------+
;       |   ...  |
;       |--------|
;       |   FAT  |
;       |--------| 0x17C0:0x0200
;       |        |
;       |--------|
;       |        |
;       | BM.BIN |
;       |        |
;       |--------| 0x0000:0x8000
;       |        |
;       |--------|
;       |  ROOT  |
;       |  DIR   |
;       |--------| 0x07C0:0x0200
;       |BOOT SEC| 
;       |ORIGIN  | 
;  >>>  |--------| 0x07C0:0x0000 :)
;       |        |
;       |--------| 0x0000:0x6000
;       |INITIAL | Initial stack address.
;       |STACK   | It goes down.
;       |--------| 
;       |        |
;       +--------+

; Do NOT change this thing.
[ORG 0x0000]

; 16bit. 
; This is the MBR. 
; Used with legacy BIOS firmware.
[BITS 16]

; MBR's entry point.
; Jump to the real start routine.
stage1_main:
    jmp GRAMADOINIT

    %include "s1data.inc"
    %include "s1lib.inc"
    %include "s1load.inc"

; =============================================
; GRAMADOINIT:  (Real start)
; Start here 0x07C0:0.
; Stack here 0:6000h.
; Root dir in 0x07C0:0x0200.
; Load the FAT in es:bx 0x17C0:0x0200.
; Load image in 0:8000h.
; #todo
; BootSegment   equ 0x07C0
; BootOffset    equ 0
; StackSegment  equ 0
; StackOffset   equ 0x6000
; RootSegment   equ 0x07C0
; RootOffset    equ 0x0200
; FATSegment    equ 0x17C0
; FATOffset     equ 0x0200
; ImageSegment  equ 0
; ImageOffset   equ 0x8000

GRAMADOINIT:

; Ajust stage1 segment to 0x07C0 and stack to 0:0x6000.
; Step1: 
    cli
    mov ax, MBR_SEGMENT
    mov ds, ax
    mov es, ax
    mov ax, STACK_SEGMENT
    mov ss, ax
    mov sp, STACK_OFFSET
    sti

; Get disk number.
Step2:
    mov byte [DriveNumber], byte dl 

; Clear the Screen.
Step3:
    mov ax, 02h
    int 010h

; Load the rootdir into the memory.
Step4:
    call s1load_load_rootdir

; Search file in rootdir, load fat and go.
; Load file and pass the command to the BM2.BIN.
Step5:
    jmp s1load_load_file_and_go

; ======================================================
;  PARTITION TABLE 
; ======================================================
; http://cars.car.coocan.jp/misc/chs2lba.html
; https://en.wikipedia.org/wiki/Partition_type
; bios = limits: h=4, c=3FF, s=A
; vhd = CHS = 963/4/17

; Partition table support.
; Colocando a partition table no lugar certo. 
; (0x1BE).
    TIMES 446-($-$$) DB 0 

; 446  16  Partition table entry 1.
; 462  16  Partition table entry 2.
; 478  16  Partition table entry 3.
; 494  16  Partition table entry 4.
; 510  2   0xAA55. Indicates this is a valid MBR.

; Partition 0. 
P0:
.flag:      db  0x80
.startH:    db  0x01
.startC:    db  0x01
.startS:    db  0
.osType:    db  0xEF             ; EFI
.endH:      db  0
.endC:      db  0
.endS:      db  0
.startLBA:       dd  0x3F        ; 63
; #todo
; We gotta send this value to the kernel.
; The file system needs to respect this limit.
.partitionSize:  dd  0x0000FFA7  ; in sectors. almost 32MB

; Partition 1, 2 and 3.
P1: dd 0,0,0,0 
P2: dd 0,0,0,0 
P3: dd 0,0,0,0 

; ----------------------------------------
; Signature.
FREE_SPACE_SIZE EQU  510-($-$$)
MBR_SIG:
    TIMES FREE_SPACE_SIZE DB 0
    DW 0xAA55

;
; End
;


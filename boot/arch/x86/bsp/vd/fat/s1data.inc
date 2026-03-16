; s1data.inc
; Data for stage1.asm



END_OF_CLUSTER EQU  0xFFFF
;;END_OF_CLUSTER2 EQU 0xFFF8
ROOTDIRSTART EQU (BUFFER_NAME)
ROOTDIRSIZE  EQU (BUFFER_NAME+4)
;datasector      EQU (BUFFER_NAME)
;cluster         EQU (BUFFER_NAME+2)
;SAVE3           EQU (BUFFER_NAME+4)
;CylinderNumbers EQU (BUFFER_VOLUME_ID)

SECTOR_SIZE EQU  0x0200    ; Sector size in bytes


;-----------------

MBR_SEGMENT     EQU  0x07C0
MBR_OFFSET      EQU  0x0000  ;ORG 

; Same segment as MBR.
; When the loader of segments reaches the mark of 0x0000,
; it switches to the next segment.
; 512*32 = 16384 bytes. 
; 16384/512 = 32 setores.
; Operations with rootdir use 'ds' segment to access rootdir, not 'es'.
;ROOTDIR_SEGMENT EQU  0x07C0  ;(Not in use. It is the same of MBR)
ROOTDIR_OFFSET  EQU  0x0200

; When the loader of segments reaches the mark of 0x0000,
; it switches to the next segment.
; So the image can't be bigger than this.
; the loader cant reach the mark of 0x0000.
IMAGE_SEGMENT EQU  0x0000
IMAGE_OFFSET  EQU  0x8000

; Stack: (0x6000 and 0x7000).
; Each segment has 64KB, or 128 sectors.
; So, we need 2 segments for 246 sectors of fat
; Different segment from MBR.
; Fat has 2 segments. 
; When the loader of segments reaches the mark of 0x0000,
; it switches to the next segment.
; We access fat using 'gs' segment, not 'ds'.
FAT_SEGMENT EQU  0x6000
FAT_OFFSET  EQU  0x0200

STACK_SEGMENT EQU  0x8000
STACK_OFFSET  EQU  0xFFF0 

; ===================
; BPB:
; BPB Personalizado para o VHD de 32MB usado pelo Gramado.
; Na inicialização, reaproveitamos esse endereço como buffer.

BUFFER_NAME:
    DB "MSDOS"    ; OS name
    DB "5.0"      ; OS version

; The BPB itself.
BPB:
BytesPerSector        dw SECTOR_SIZE  ; 0x0200=512
SectorsPerCluster     db 1            ; Sectors per cluster
ReservedSectors       dw 62           ; Reserved sectors 
TotalFATs             db 0x02         ; Number of FATs
MaxRootEntries        dw 0x0200       ; Number of root entries.
; #wrong: (CHS: (963*4*17)-5)(Number of sectors - number of hidden sectors)
; #right: (4*17*963)-5
TotalSectorsSmall     dw 0x0000  
MediaDescriptor       db 0xF8
SectorsPerFAT         dw 246    
SectorsPerTrack       dw 17    ; 19 SPT.  
NumHeads              dw 4     ; 4 HEADS. Number of surfaces
; Hidden sectors.
; 1+1+3 ( mbr + vbr + reserved sectors depois do vbr)
HiddenSectors         dd 5
; Number of sectors, if 'TotalSectorsSmall = 0'.
; 32 bit version of number of sectors.
TotalSectorsLarge     dd 0       
DriveNumber           db 0x80
; Flags: is this the old 'current head' field?
Flags                 db 0x00
; Signature: 0x29 | 41.
Signature             db 0x29

; #bugbug
; The MBR represents the disk, not the volume.
; Volume ID and Volume label.
; VolumeID              dd 0x980E63F5     ; 32bit
; VolumeLabel           db "GRAMADO    "  ; 11 bytes

; Disk ID and Disk label.
DiskID     dd  0x980E63F5     ; 32bit
DiskLabel  db  "GRAMADODISK"  ; 11 bytes


; File system identification string. 
SystemID              db "FAT16   "
; =========================================
; End of BPB.


;=================================
; Start of a small data area.
DAPSizeOfPacket db 10h
DAPReserved     db 00h
DAPTransfer     dw 0001h
DAPBuffer       dd 00000000h
DAPStart        dq 0000000000000000h

datasector  dw  0x0000    ;Início da área de dados.
cluster     dw  0x0000

;absoluteSector db 0x00
;absoluteHead   db 0x00
;absoluteTrack  db 0x00
;CylinderNumbers:  dd 0  ;;dword

; ---------------------------
; Option 1:
; This is the legacy boot manager.
; It loads the legacy version of the boot manager,
; with the 32bit-assembly shell.
;
;ImageName db  "BM      BIN", 0x0D, 0x0A, 0x00

; ---------------------------
; Option 2:
; It loads a faster version of the boot manager,
; with NO 32bit-assembly shell.
; This boot manager with UI dialog window.
; In one option we the system is initialized and 
; in the second we go to the shell. Probably it is
; the old 32bit shell.
;
ImageName db  "BM2     BIN", 0x0D, 0x0A, 0x00


; ---------------------------
; Message:
; 'R' = Root fail.
msgFailure db  "R", 0x00 
;...
;=================================
; End of the data area.


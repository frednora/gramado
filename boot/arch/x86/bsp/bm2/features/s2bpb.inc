;
; File: s2bpb.inc
;
;     FAT12 and FAT16 BPB used in the Stage 2.
;
; Version: 1.0, 2015.
;


;===============================================
; Stage 2 BPB for fat16 filesystem.
;
; Partição 0 - (31MB).
;
;vbr  - ?     ;134 (Depende do tanto de setores reservados.).
;fat1 - ?
;fat2 - ?
;root - ?
;data - ?
;
s2_OEM_ID: db "FAT16BPB"
s2_bpb:
	.BytesPerSector:     dw 512
	.SectorsPerCluster:  db 1
	.ReservedSectors:    dw 2
	.TotalFATs:          db 2
	.MaxRootEntries:     dw 512         ;512 entradas de 32bytes = 32 setores.
	.TotalSectorsSmall:  dw 0
	.MediaDescriptor:    db 0xF0
	.SectorsPerFAT:      dw 64          ;64.      
	.SectorsPerTrack:    dw 0           ;Pegar via BIOS.     
	.NumHeads:           dw 0           ;Pegar via BIOS.       
	.HiddenSectors:      dd 0x00000000
	.TotalSectorsLarge:  dd 0x00000000
	 ;for extended.
	.DriveNumber:        db 0x80     
	.Flags:              db 0x00
	.Signature:          db 0           ;Extended Boot Signature.
	.VolumeID:           dd 0x00000001
	.VolumeLabel:        db "FREDNORA8MB"
	.SystemID:           db "FAT16   "


;=============================================
; Stage 2 BPB for fat12 filesystem.
;
; OBS:. fat12 é usado em hds pequenos.
;
; fat 12, BPB.
;	
OEM_ID:  db "FAT12BPB"
bpb:
	BytesPerSector     dw 0x0200
	SectorsPerCluster  db 0x01
	ReservedSectors    dw 0x0001
	TotalFATs          db 0x02
	MaxRootEntries     dw 0x00E0
	TotalSectorsSmall  dw 0x0B40
	MediaDescriptor    db 0xF0
	SectorsPerFAT      dw 0x0009
	SectorsPerTrack    dw 0x0012
	NumHeads           dw 0x0002
	HiddenSectors      dd 0x00000000
	TotalSectorsLarge  dd 0x00000000
	;for extended.
	DriveNumber        db 0x80
	Flags              db 0x00
	Signature          db 0x29
	VolumeID           dd 0xFFFFFFFF
	VolumeLabel        db "FREDNORAF12"
	SystemID           db "FAT12   "	
	
;Continua ...

;
;fim.
;

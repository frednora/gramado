;
; File: system.inc
; Descrição:
;    Configurações de inicialização do sistema. 
;    Obs: Não usado ainda.
; 2015 - Created by Fred Nora.
;

; System file pointer.
system_file:    ; ??


;
; System BPB support.
;

;system name. 
system_name: db "FREDNORA-OS"  ;+0

;bios parameter block.
system_bpb:  
	.BytesPerSector:        dw 0x0200         ;+11 
	.SectorsPerCluster:     db 0x01           ;+13
	.ReservedSectors:       dw 0x0001         ;+14
	.TotalFATs:             db 0x02           ;+16
	.MaxRootEntries:        dw 0x0200         ;+17
	.TotalSectorsSmall:     dw 0x0000         ;+19 
	.MediaDescriptor:       db 0xF0           ;+21
	.SectorsPerFAT:         dw 127            ;+22
	.SectorsPerTrack:       dw 0x0000         ;+24
	.NumHeads:              dw 0x0000         ;+26
	.HiddenSectors:         dd 0x00000000     ;+28
	.TotalSectorsLarge:     dd 0x00000000     ;+32 
	 ;for extended.
	.DriveNumber:           db 080h           ;+36
	.Flags:                 db 0x00           ;+37
	.Signature:             db 0              ;+38
	.VolumeID:              dd 0xFFFFFFFF     ;+39
	.VolumeLabel:           db "FREDNORA123"  ;+42
	.SystemID:              db "FAT16   "  ;+53

; ...

;
; End
;


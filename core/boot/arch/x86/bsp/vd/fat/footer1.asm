;;=============================================================
;; vhd_footer:
;;     VHD FOOTER.
;;
;VHD_FEATURES  EQU  0  ;features
;VHD_FILE_FORMAT_VERSION  EQU 0x00001000 ;file format version 
;VHD_CREATOR_APP EQU 0x61766D6A  ;;0x6A,0x6D,0x76,0x61 ;creator app
;VHD_PLATFORM_VERSION EQU 0x0A000400
;VHD_PLATFORM_CODE_WI2R  EQU  0x57693272  ;WI2R
;VHD_PLATFORM_CODE_WI2K  EQU  0x5769326B  ;WI2K
;VHD_PLATFORM_CODE_W2RU  EQU  0x57327275  ;W2RU
;VHD_PLATFORM_CODE_W2KU  EQU  0x57326B75  ;W2KU
;VHD_PLATFORM_CODE_NONE  EQU  0           ;NONE
;VHD_PLATFORM_CODE_MAC   EQU  0x4D163220  ;MAC
;VHD_PLATFORM_CODE_MACX  EQU  0x4D163258  ;MACX
;VHD_PLATFORM_TEST  EQU  0x6B326957       ;já funcionou.
;VHD_PLATFORM  EQU  VHD_PLATFORM_TEST 
;VHD_ORIGINAL_SIZE_PART1 EQU 0
;VHD_ORIGINAL_SIZE_PART2 EQU 0x00000001 ;16MB
;VHD_ORIGINAL_SIZE_PART2 EQU 0x00000002 ;32MB
;VHD_CURRENT_SIZE_PART1 EQU 0
;VHD_CURRENT_SIZE_PART2 EQU 0x00000001 ;16MB   
;VHD_CURRENT_SIZE_PART2 EQU 0x00000002 ;32MB   
;VHD_FOOTER_DISK_GEOMETRY_CYLINDER    EQU  0xC303  ;*49923
;VHD_FOOTER_DISK_GEOMETRY_HEADS       EQU  4       ;*4
;VHD_FOOTER_DISK_GEOMETRY_SECTORS     EQU  0x11    ;*17
;VHD_FOOTER_DISK_TYPE  EQU  2  ;FIXED   0x02000000
;VHD_FOOTER_DISK_TYPE  EQU  3  ;DYNAMIC
;VHD_FOOTER_DISK_TYPE  EQU  4  ;DIFFERENCING 
;VHD_FOOTER_CHECKSUM EQU 0xDBE6FFFF ;vhdSize=32MB partitionSize=8
;VHD_FOOTER_CHECKSUM EQU 0xDBE6FFFF ;vhdSize=32MB partitionSize=16
;VHD_FOOTER_CHECKSUM EQU 0xD0E8FFFF ;vhdSize=32MB partitionSize=16
;VHD_FOOTER_CHECKSUM EQU 0xC2E7FFFF ;funcionou com vhdsize=32MB patitionSize=32MB.
;;...
;; History:
;;     2016 - Created by Fred Nora.

vhd_footer:
    ;;
.cookie:
    db 'conectix'
.features:
    dd 0
.version:
    dd 0x00001000
.dataOffset:
    dd 0xFFFFFFFF, 0xFFFFFFFF
.timeStamp:
    dd 0x5868091E
.creatorApp:
    db 0x6A, 0x6D, 0x76, 0x61
.creatorVersion:
    dd 0x0A000400
;;Windows 0x5769326B (Wi2k)
.creatorOS:
    dd 0x6B326957
;; Original Size
;; This field stores the size of the hard disk in bytes, 
;; from the perspective of the virtual machine, at creation time. 
;; This field is for informational purposes. 
;bytes.: 8,589,934,592.0 Bytes (B)	=	8,192.0 Megabytes (MB)
.originalSyze:
    dd 0, 0x00000002  
;; Current Size
;; This field stores the current size of the hard disk, in bytes, 
;; from the perspective of the virtual machine.
;; This value is same as the original size when the hard disk is created. 
;; This value can change depending on whether the hard disk is expanded. 
;bytes. 8,589,934,592.0 Bytes (B)	=	8,192.0 Megabytes (MB)
.currentSyze:
    dd 0, 0x00000002  
.diskGeometry:
    dw 0xC303           ;# Cylinders.
    db 4                ;# Heads.
    db 0x11             ;# Sectors.
.diskType:
    dd 0x02000000       ;fixed.
.chechsum:
    dd 0xC2E7FFFF
.uniqueID:
    db 0x11, 0x80, 0xF2, 0x62
    db 0xD4, 0x95, 0x65, 0x44
    db 0xDD, 0xDD, 0xDD, 0xDD
    db 0xDD, 0xDD, 0xDD, 0xDD
.savedState:
    db 0
.reserved:
    ; Padded to 512 bytes?
    times 427 db 0
    
    

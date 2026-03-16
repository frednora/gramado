; shell32/shmsg.inc 
; + Strings for message
; + Variables
; 2015 - Created by Fred Nora.


; Variables
; ==========

; =============================================
; Struct.
; Structure for some random string used by the shell.

SHELL:
.prompt_string:
    db 'bm-shell', 0
.prompt_string_separator1:
    db ':', 0
.prompt_string_separator2:
    db '>', 0
.separador1:
    db '>', 0 
;Em que volume est� o Shell.
.volume:
    db 0
;Em que diret�rio est� o Shell.
.directory:
    db 0
; =============================================

; Prompt:   
; Endere�o da string no novo prompt.   
new_prompt: dd 0 

;Flag:
;Se vai mudar o nome de um arquivo.
;O que fazer com o arquivo? ... Salvar, Abrir.
file_flag: db 0 

;
; -- Command strings --------
;

; See: shell.inc

; Default gramado mode.
; Same as 'boothome'.
boot_string  db  'boot', 0 
; Gramado modes.
bootjail_string        db  'bootjail', 0
bootp1_string          db  'bootp1', 0
boothome_string        db  'boothome', 0
bootp2_string          db  'bootp2', 0
bootcastle_string      db  'bootcastle', 0
bootcalifornia_string  db  'bootcali', 0
; More
reboot_string    db  'reboot', 0  ; Reboot the system.
shutdown_string  db  'shutdown', 0  ; Poweroff via APM using BIOS.
cls_string   db  'cls', 0
help_string  db  'help', 0
dir_string   db  'dir', 0    ; List of files in root dir.
ver_string   db  'ver', 0
exit_string  db  'exit', 0
disk_string  db  'disk', 0  ; Disk info. 
part_string  db  'part', 0  ; Partition info.    
; ...
;----------------------

; ===============================================================
; Messages
version_msg:
    db 'Gramado Boot Manager ',
    db 'version ', BOOTMANAGER_VER, 13, 10,
    db 13, 10, 0  


;Para declarar o nome do arquivo.
prompt_change_file:  db 'filename=', 0


;File support.
appextension:  db '.app', 0
dirextension:  db '.dir', 0


; File operation.
msg_file_op0 db "Default operation on file", 13, 10 ,0
msg_file_op1 db "Open file",   13, 10 ,0
msg_file_op2 db "Create file", 13, 10 ,0
msg_file_op3 db "Delete file", 13, 10 ,0
msg_file_op4 db "Open dir",    13, 10 ,0
msg_file_op5 db "Create dir",  13, 10 ,0
msg_file_op6 db "Delete dir",  13, 10 ,0
msg_file_op7 db "Open app",    13, 10 ,0
; ...

not_found_msg:      
    db 'Command or file not found', 13, 10, 0

hd_msg:             
    db 'Reading sector', 13, 10, 0

dir_title1_string:  
    db "Current Directory:", 13, 10, 0

dir_title2_string:  
    db "Name                     Cluster", 13, \
       "================================", 13, 0

; Help text.
; Message for the 'help' command.
help_text:
    db 'boot', 13, 10,
    db 'bootjail, bootp1, boothome, bootp2, bootcastle', 13, 10,
    db 'cls, dir, disk, help, part, ver', 13, 10, 0

;
; End
;


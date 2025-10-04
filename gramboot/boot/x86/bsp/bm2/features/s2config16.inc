;
; File: s2config16.inc
;
;     This is a static structure used for boot configuration.
;
; Versin: 1.0, 2015.
;


;
; BOOT PARAMETERS.	
;

BOOT16:
    .mode:  dd 0
    .res2:  dd 0    
    .res3:  dd 0    
    .res4:  dd 0
  ;  .res5:  dd 0
   ; .res6:  dd 0
   ; .res7:  dd 0
   ; .res8:  dd 0
;...



DISKINFO16:

    DISKINFO16_disk_number: db 0 
	  DISKINFO16_heads:       dw 0
	  DISKINFO16_spt:         dw 0
	  DISKINFO16_cylinders:   dw 0
    ;; ...
	


;
; End.
;




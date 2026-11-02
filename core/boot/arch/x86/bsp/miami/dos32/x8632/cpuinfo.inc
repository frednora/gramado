; cpuinfo.inc
; Variables and routines for CPU support.
; 2015


;
; Variables
;

global _CPUInfo
_CPUInfo:
.eax:    dd 0
.ebx:    dd 0
.ecx:    dd 0
.edx:    dd 0
;Continua...	


;----------------------------------------
; _finding_cpu_info:
;     Encontra informa��es sobre o processador 
;     usando a instru��o cpuid.   
;
; IN:  eax = info_type.
;	
global _finding_cpu_info
_finding_cpu_info:

    PUSHA
	;salva o comando
	mov dword [.InfoType], eax  
	xor ecx, ecx  
	cpuid  

	;envia    
	mov dword [_CPUInfo.eax], eax
    mov dword [_CPUInfo.ebx], ebx  
    mov dword [_CPUInfo.ecx], ecx  
    mov dword [_CPUInfo.edx], edx 
	
	;Continua... ??
	
	POPA	
	RET	
.InfoType: 
    dd 0	
	
;
;fim.
;

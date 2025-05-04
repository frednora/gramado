;
; Arquivo: cpuinfo.inc
;
; Descrição:
;     Variáveis e rotinas sobre a CPU.
;
; Versão: 1.0, 2015, 2016.
;



;
; Variáveis.
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
;     Encontra informações sobre o processador 
;     usando a instrução cpuid.   
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

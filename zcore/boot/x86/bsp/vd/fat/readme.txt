#todo:
     
    +As informações aqui precisam ser revistas.
    +Criar arquivo de configuração da GUI. GUI.TXT.
    //----------
    Sobre a pasta /vhd:
    Essa pasta comtém o arquivo principal em assembly
que constroi o VHD, algumas pastas e arquivos de configuração.
    /images - Imagens usadas pelo sistema. Serão transferidas para dentro do VHD.
    /stage1 - MBR.
    /tests  - Arquivos de teste.Serão transferidas para dentro do VHD.

    O objetivo maior desse sistema 4Nora é criar um disco virtual chamado GRAMADO.VHD
e incluir dentro do disco virtual todas as outras partes do sistema operacional que já
foram compiladas anteriormente em outros projetos.

Desejamos valores fixos para facilitar a instalação.
Map for main LBA addresses in the system disk.
see: vd/fat/main.asm

--------------------------------------
    0 = mbr
--------------------------------------
Partition 0:
   63 = p0_vbr
   67 = p0_fat1
  313 = p0_fat2
  559 = p0_root
  591 = p0_dataarea
--------------------------------------
Partition 1:
  ...
--------------------------------------
Partition 2:
  ...
--------------------------------------
Partition 3:
  ...

--------------------------------------
  
    Fred Nora, Oct 2018.

;
; End
;
    

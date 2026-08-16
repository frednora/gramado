
bash clone:
    Isso é apenas um shell.
	Ele deve rodar nos limites da tela do terminal virtual.
	Isso não deve criar janelas.


  shell log:

   #todo: Incluir o suporte a lexer e parser, semelhante ao que acontece com os compiladores.
          Isso é necessário para interpretar os scripts;

na versão 2.1, o sistema de enviar mensagens para o procedimento da janela com o foco de entrada foi implementado. e funcionou corretamente. Ou seja.
o procedimento de janela de um processo em user mode é ativado toda vez que
pressionamos uma tecla do teclado.
O driver de teclado envia uma mensagem para o kernel, que coloca a mensagem
na estrutura da janela com o foco de entrada. O processo cliente, faz uma chamada
ao kernel solicitando uma mensagem. o processo quer saber se tem uma mensagem
na estrutura de determinda janela. O kernel envia a mensagem que está na estrutura enviada pelo processo. O processo recebe essa mensagem e envia para o procedimento de janela dentro do programa.
:)


nem todo caractere digitado está aparecendo na tela.
mas todo caractere que aparece na tela, também foi para o buffer de digitações.
é possível que seja algo com escalonamento do processo shell, estamos digitando
no momento em que a cpu está sendo usada por outro processo.

O escalonador do kernel tem que selecionar a thread à qual a janela com o foco de entrada esta associada no momento em que recebe uma digitação de teclado.


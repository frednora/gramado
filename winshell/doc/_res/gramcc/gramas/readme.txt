
//gramas
//vamos aproveitar o código do compilador para criar uma assembler ...
//vamos usar o mesmo esquema de lexer e parser. 

------------------------
old stuff
 /gramcc/c 

 Esse é o compilador de C da coleção gramcc.
 Ele poderá ser chamado pelo wrapper gramcc.bin eu diretamente através do comando gramc;


//gramado C Compiler. 

Esse é um aplicativo para rodar no sistema operacional Gramado 0.4.
Roda apenas no ambiente gramado core, no processo init.

É um editor de testes para testar funcionalidades do sistema.
No momento usamos ele para testar o envio de uma string do shell
para o editor através do kernel via memória compartilhada.
O envio da mensagem funcionou.

crt0.asm contém o entry point, chamando main em seguida.


entry point em crt0.asm
__crt0Main:
  


//função principal em main.c
mainGetMessage()



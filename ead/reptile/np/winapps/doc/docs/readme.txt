
 /gramcc/gramc

 Esse é o compilador de C da coleção gramcc.
 Ele poderá ser chamado pelo wrapper gramcc.bin ou diretamente 
 através do comando gramc;
 
 
 gramcc usage:



$gramcc.bin [flag] [filename]


flags:
    -s
    -c
    ...

filename:
    test.c
    ... 

ex:
    gramcc -c file.c
    gramc -c file.

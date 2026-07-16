// contador.c

#include <stdio.h>

int main(int argc, char *argv[])
{
	int letra, contador;
	contador = 0;
	
	printf ("contador: Initializing ...\n");
	
	while ((letra = getchar()) != EOF){
		if (letra == "\n"){
			++contador;
		}
	};
	
	printf ("Numero de lineas leidas: %d", contador);
	printf ("contador: done\n");
	return 0;
}

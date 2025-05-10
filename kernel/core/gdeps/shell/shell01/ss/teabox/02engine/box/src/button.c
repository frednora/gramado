/*
 * File: button.c
 *     Create button.
 */

#include "gram3d.h"

/*
 * gws_draw_button:
 *     Desenha um botão dado um tipo.
 * @todo: Medir o tamanho da string da tag.
 * @todo: O botão precisa de uma etiqueta, que pode ser uma string ou uma imagem.
 * Talvez uma estrutura de botão seja necessário, para armazenas coisas como 
 * deslocamento dentro da janela e endereço da imagem carregada como etiqueta.
 * #importante: 
 * Retorna o ponteiro pra estrutura do botão.
 */

// #todo
// Criar o argumento de ponteiro de estrutura de janela
// para usar em posicionamento relativo.

void *gws_draw_button ( 
    unsigned char *string,
    int style,
    int state,
    int type, 
    unsigned long x, 
    unsigned long y, 
    unsigned long width, 
    unsigned long height, 
    unsigned long color )
{
    // Now we are using the funcion 'create window' to create a button.
    gwssrv_debug_print ("gws_draw_button: [Deprecated] \n");
    return NULL;
}



// Draw a rectangle.
// Credits:
// See: https://gist.github.com/Silva97/0d7cfd0ef8ae19d8d74b654367d35997

// rtl
#include <stdio.h>

enum {
    RECT_T  = '-',  // Aresta do topo
    RECT_TR = '+', //  Vértice do canto superior direito
    RECT_R  = '|',  // Aresta da direita
    RECT_BR = '+', //  Vértice do canto inferior direito
    RECT_B  = '-',  // Aresta da parte inferior
    RECT_BL = '+', //  Vértice do canto inferior esquerdo
    RECT_L  = '|',  // Aresta da esquerda
    RECT_TL = '+', //  Vértice do canto superior esquerdo
    RECT_C  = ' '   // Preenchimento do retângulo
};

void putmchar(int c, int times);
void printrect(int width, int height);


int main(void)
{
    printf("\n");
    printf("\n");
    printrect(10,10);
    return 0;
}

void printrect(int width, int height)
{
    int i=0;

    for(i=0; i < height; i++){
        if(!i){
            putchar(RECT_TL);
            putmchar(RECT_T, width);
            putchar(RECT_TR);
            putchar('\n');
        } else if(i == height-1){
            putchar(RECT_BL);
            putmchar(RECT_B, width);
            putchar(RECT_BR);
            fflush(stdout);
        } else {
            putchar(RECT_L);
            putmchar(RECT_C, width);
            putchar(RECT_R);
            putchar('\n');
        }
    }
}

void putmchar(int c, int times)
{
    int i=0;
    for(i=0; i < times; i++)
        putchar(c);
}


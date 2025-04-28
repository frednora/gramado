
// 2023 - Created by Fred Nora.

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

// Gray
#define xCOLOR_BLACK  0x000000  //preto
#define xCOLOR_GRAY1  0x202020  //cinza mais escuro
#define xCOLOR_GRAY2  0x404040  //cinza
#define xCOLOR_GRAY3  0x606060  //cinza
#define xCOLOR_GRAY4  0x808080  //cinza
#define xCOLOR_GRAY5  0xa0a0a0  //cinza
#define xCOLOR_GRAY6  0xc0c0c0  //cinza
#define xCOLOR_GRAY7  0xe0e0e0  //cinza mais clarinho
#define xCOLOR_WHITE  0xffffff  //branco  

#define COLOR_INVALID    0xEEEEEEEE


struct object_d
{
    int id;
    unsigned int color0;
    unsigned int color1;
    unsigned int color2;
    unsigned int color3;
    unsigned int color4;
    unsigned int color5;
    unsigned int color6;
    unsigned int color7;
};
struct object_d  Objects[8];

int token=0;
#define TOKEN_SECTION    1 // #
#define TOKEN_SKIP       2
#define TOKEN_COLOR        20
#define TOKEN_START_COLOR  21  //[c
#define TOKEN_END_COLOR    22  //]c
int data=0;

int x1=0;
int x2=0;

static int current_section=0;

int n9_get_next(void)
{
    token=0;

    x1=getc(stdin);

    if (x1==' ' || x1=='\t' || x1=='\n')
    {
        token = TOKEN_SKIP;
        return token;
    }

    if (x1=='#')
    {
        x2=getc(stdin);
        if ( x2 >= '0' && x2 <= '9'){
            data = x2;   // sectio id
        }else{
            return -1;
        }
        token = TOKEN_SECTION;
        return token;
    }

    if (x1=='[')
    {
        x2=getc(stdin);
        if (x2=='c'){
            token=TOKEN_START_COLOR;
            return token; 
        }else{
            return -1;
        };
    }
        
    if (x1==']')
    {
        x2=getc(stdin);
        if (x2=='c'){
            token=TOKEN_END_COLOR;
            return token;
        }else{
            return -1;
        };
    }

    // color
    if ( x1 >= '0' && x1 <= '9')
    {
        token = TOKEN_COLOR;
        data = x1;
        return token;
    }

fail:
// Invalid token
    return -1;
}

unsigned int to_color(int color_index)
{
    unsigned int color_table[9];

    color_table[0] = xCOLOR_BLACK;
    color_table[1] = xCOLOR_GRAY1;
    color_table[2] = xCOLOR_GRAY2;
    color_table[3] = xCOLOR_GRAY3;
    color_table[4] = xCOLOR_GRAY4;
    color_table[5] = xCOLOR_GRAY5;
    color_table[6] = xCOLOR_GRAY6;
    color_table[7] = xCOLOR_GRAY7;
    color_table[8] = xCOLOR_WHITE;

    if ( color_index >= 0 || 
         color_index <= 8 )
    {
        return (unsigned int) color_table[color_index];
    } 

    return (unsigned int) COLOR_INVALID;
}


int main(int argc, char *argv[])
{
    FILE *fp;
    int Token=0;

    printf("n9.bin:\n");
    printf("\n");

    fp = fopen((char *) argv[2], "rb");
    if ( (void*) fp == NULL ){
        printf("fp\n");
        return 1;
    }
    stdin=fp;

    while(1)
    {
        Token = n9_get_next();

        if(Token == -1)
            break;
        if(Token == 0)
            break;

        if (Token==TOKEN_SECTION)
        {
            int sid = (int) ( data - 48 );
            printf("SECTION id{%d}\n",sid);
            if (sid < 0 || sid >= 8)
            {
                printf("Invalid section id\n");
                return 1;
            }
            current_section = sid;
            Objects[current_section].id = current_section; 
        }

        if (Token==TOKEN_SKIP){
            printf("SKIP\n");
        }

        if (Token==TOKEN_START_COLOR){
            printf("START COLOR\n");
        }

        if (Token==TOKEN_END_COLOR){
            printf("END COLOR\n");
        }

        if (Token==TOKEN_COLOR)
        {
            // 0=48
            int index = (int) ( data - 48 );
            unsigned int u32_c = to_color(index);
            
            printf("COLOR: data={%d} index={%d} color{%x}\n", 
                data, index, u32_c);
        
            switch (index)
            {
                case 0:
                    Objects[current_section].color0 = u32_c;
                    break;
                case 1:
                    Objects[current_section].color1 = u32_c;
                    break;
                case 2:
                    Objects[current_section].color2 = u32_c;
                    break;
                case 3:
                    Objects[current_section].color3 = u32_c;
                    break;
            };    
        }

        //...
    };

    printf("exit: \n");

    int i=0;
    for (i=0; i<8; i++)
    {
        printf ("\n");
        printf ("object id %d:\n",Objects[i].id);
        
        printf ("color 0: %x\n",Objects[i].color0);
        printf ("color 1: %x\n",Objects[i].color1);
        printf ("color 2: %x\n",Objects[i].color2);
        printf ("color 3: %x\n",Objects[i].color3);
    };

    return 0;
}




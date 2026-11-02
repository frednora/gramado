
// 2023 - Created by Fred Nora.

#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtl/gramado.h>


FILE *in_file;

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

#define TOKEN_DATA  10

#define TOKEN_START_COLOR  21  //[c
#define TOKEN_END_COLOR    22  //]c

#define TOKEN_START_PARAGRAPH  31   // [p
#define TOKEN_END_PARAGRAPH  32     // ]p

#define TOKEN_START_COMMENT  41   // [-
#define TOKEN_END_COMMENT  42     // ]-

#define TOKEN_BREAK_LINE  51      // [b

#define TOKEN_SEPARATOR  61     // [s


int data=0;
int data_type=0;   // 1=0~9: literal | 2=a~z | 3=A~Z 

int x1=0;
int x2=0;

static int current_section=0;
static int comment_is_open=FALSE;
static int paragraph_is_open=FALSE;
static int color_is_open=FALSE;
static int space_when_paragraph_is_open=FALSE;

static char paragraph_buffer[2048];
static int p_offset=0;

int n9_get_next(void)
{
    token=0;

    x1=getc(in_file);

    if (x1==' ' || x1=='\t' || x1=='\n')
    {
        token = TOKEN_SKIP;
        data=x1;
        return token;
    }

    if (x1=='#')
    {
        x2=getc(in_file);
        if ( x2 >= '0' && x2 <= '9'){
            data = x2;   // sectio id
            token = TOKEN_SECTION;
            return token;
        }else{
            return -1;
        }
        token = TOKEN_SECTION;
        return token;
    }

    if (x1=='[')
    {
        x2=getc(in_file);
        if (x2=='c'){  //color
            token=TOKEN_START_COLOR;
            return token; 
        }else if (x2=='p'){  //paragraph
            token=TOKEN_START_PARAGRAPH;
            return token;
        }else if (x2=='b'){   //break line
            token=TOKEN_BREAK_LINE;
            return token;
        }else if (x2=='s'){   //separator
            token=TOKEN_SEPARATOR;
            return token;
        }else if (x2=='-'){   //comment
            token=TOKEN_START_COMMENT;
            return token;
        }else{
            return -1;
        };
    }
        
    if (x1==']')
    {
        x2=getc(in_file);
        if (x2=='c'){
            token=TOKEN_END_COLOR;
            return token;
        }else if(x2=='p'){
            token=TOKEN_END_PARAGRAPH;
            return token;
        }else if(x2 == '-'){
            token=TOKEN_END_COMMENT;
            return token;
        }else{
            return -1;
        };
    }

    // data
    if ( x1 >= '0' && x1 <= '9' )
    {
        token = TOKEN_DATA;
        data = x1;
        data_type=1;
        return token;
    }

    // data
    if ( x1 >= 'a' && x1 <= 'z' )
    {
        token = TOKEN_DATA;
        data = x1;
        data_type=2;
        return token;
    }

    // data
    if ( x1 >= 'A' && x1 <= 'Z' )
    {
        token = TOKEN_DATA;
        data = x1;
        data_type=3;
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
    int index=0;
    unsigned int u32_c=0;
    int DumpInfo=TRUE; //FALSE;
    register int i=0;
    
    comment_is_open=0;
    color_is_open=0;
    paragraph_is_open = 0;
    p_offset = 0;
    space_when_paragraph_is_open = FALSE;

    //printf("n9.bin:\n");
    printf("\n");

    if (argc != 3){
        printf("argc fail\n");
        exit(1);
    }


    fp = fopen((char *) argv[2], "rb");
    if ( (void*) fp == NULL ){
        printf("fp\n");
        return 1;
    }
    in_file=fp;

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
            //printf("SECTION id{%d}\n",sid);
            if (sid < 0 || sid >= 8)
            {
                printf("Invalid section id\n");
                return 1;
            }
            current_section = sid;
            Objects[current_section].id = current_section; 
        }

        if (Token==TOKEN_SKIP)
        {
            //printf("SKIP\n");
            
            // Vários espaços viram apenas 1.
            if (paragraph_is_open == TRUE)
            {
                if (data == ' '){
                    space_when_paragraph_is_open = TRUE;
                }
            }
        }

        // color

        if (Token==TOKEN_START_COLOR){
            //printf("START COLOR\n");
            color_is_open=TRUE;
        }

        if (Token==TOKEN_END_COLOR){
            //printf("END COLOR\n");
            color_is_open = FALSE;
        }


        // COMMENT

        if (Token==TOKEN_START_COMMENT)
        {
            //printf("START COMMENT\n");
            comment_is_open=TRUE;
        }

        if (Token==TOKEN_END_COMMENT)
        {
            //printf("END COMMENT\n");
            comment_is_open = FALSE;
        }

        // p

        if (Token==TOKEN_START_PARAGRAPH)
        {
            //printf("START PARAGRAPH\n");
            paragraph_is_open=TRUE;
        }

        if (Token==TOKEN_END_PARAGRAPH)
        {
            //printf("END PARAGRAPH\n");
            paragraph_buffer[p_offset] = (char) 0;  //finaliza.
            //flush accumulated chars.
            //fflush(stdout);
            //printf("\n");
            //printf("bytes{%d} string{%s}\n",
            //    p_offset,paragraph_buffer);
            printf("%s\n",paragraph_buffer);
            
            //#test
            rtl_clone_and_execute(paragraph_buffer);
            
            
            for (i=0; i<2048; i++)
                paragraph_buffer[i]=(char)0;
            p_offset=0;
            data_type=0;
            paragraph_is_open = FALSE;
        }

        // data
        if (Token == TOKEN_DATA)
        {

            if (comment_is_open == TRUE)
            {
                //printf("%c\n",data);
            }

            if (paragraph_is_open == TRUE)
            {
                if (data_type == 1 || data_type == 2 || data_type == 3 )
                {
                    if (space_when_paragraph_is_open==TRUE){
                        paragraph_buffer[p_offset] = (char) ' ';
                        p_offset++;
                        space_when_paragraph_is_open=FALSE;
                    }
                    //printf("%c",data);
                    paragraph_buffer[p_offset] = (char) (data & 0xFF);
                    p_offset++;
                    if( p_offset >= 2048){
                         printf("buffer overflow\n");
                         exit(1);
                    }
                }
            }

            if ( color_is_open == TRUE )
            {
                if (data_type==1)
                {
                // 0=48
                index = (int) ( data - 48 );
                u32_c = to_color(index);
                //printf("COLOR: data={%d} index={%d} color{%x}\n", 
                //   data, index, u32_c);
                switch (index){
                case 0: Objects[current_section].color0 = u32_c;  break;
                case 1: Objects[current_section].color1 = u32_c;  break;
                case 2: Objects[current_section].color2 = u32_c;  break;
                case 3: Objects[current_section].color3 = u32_c;  break;
                };    
                }
            }
            
            // ...
        }

        // b
        if (Token==TOKEN_BREAK_LINE){
            //printf("BREAK LINE\n");
            printf("\n");
        }

        // b
        if (Token==TOKEN_SEPARATOR)
        {
            //printf("SEPARATOR\n");
            for(i=0; i<80; i++){    //max: create options
                printf("%c", '-');  //separator: create options
            };
            fflush(stdout);
        }

        //...
    };


    printf("\n");
    printf("------------------\n");
    printf("exit: \n");

    if(DumpInfo){
    for (i=0; i<8; i++)
    {
        printf ("\n");
        printf ("object id %d:\n",Objects[i].id);
        
        printf ("color 0: %x\n",Objects[i].color0);
        printf ("color 1: %x\n",Objects[i].color1);
        printf ("color 2: %x\n",Objects[i].color2);
        printf ("color 3: %x\n",Objects[i].color3);
    };
    }

    return 0;
}




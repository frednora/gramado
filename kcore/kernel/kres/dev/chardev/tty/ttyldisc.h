//tty line discipline header.



#ifndef ____TTYLDISC_H
#define ____TTYLDISC_H  1



//
// Enumerando os tipos de disciplina de linha.
//

#define N_TTY 0
//...
#define NR_LDISCS 20


//#define TTY_LDISC_MAGIC	0x5403
#define TTY_LDISC_MAGIC 1234


// (Virtual functions).
struct ttyldisc_d 
{

    int index;
    
    int used;
    int magic;
    
    // linux-like
    short type;       // type of tty ldisc. 
    short subtype;    // subtype of tty ldisc. 
    int flags;        // tty ldisc flags.    
    
    // contador de referências. ?
    int count;

    // Qual operação está acontecendo?
    // Não pode ser interrompida.
    //int state;

    // Transferência de dados.
    struct tty_d *from;
    struct tty_d *to;

    //driver envolvido na transferência.
    struct ttydrv_d *driver;
};
struct ttyldisc_d *CurrentTTYLDISC;

#endif   



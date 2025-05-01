// libata.h


#ifndef __DD_HDD_H
#define __DD_HDD_H    1


/*
// #test: Not in use.
struct hdd_d
{
    int dummy;
};
*/


// --------------------------

void 
ata_read_hd_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx ); 

void 
ata_write_sector ( 
    unsigned long ax, 
    unsigned long bx, 
    unsigned long cx, 
    unsigned long dx ); 



#endif   


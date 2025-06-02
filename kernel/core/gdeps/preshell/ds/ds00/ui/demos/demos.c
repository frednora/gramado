// demos.c
// A place for demos.
// #remember:
// This is the display server.
// This is not the right place for this thing.
// But maybe it will be useful for effects.
// Created by Fred Nora.

#include "../../gwsint.h"

int gUseDemos = TRUE;


// Rotina usada para rodar rotinas demo na inicializaçao.
// Seleciona a rotina demo a ser executada.
void demos_startup_animation(int i)
{
    switch (i){
    case 1: 
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    case 7: demoCurve();  break;
    case 8:
        break;
    case 9: demoCat();  break;
    case 10: demoTriangle();  break;
    case 11:
        break;
    case 12: demoLine1();  break;
    case 13:  
        break;
    case 14: demoPolygon();  break; 
    case 15: demoPolygon2();  break;
    default:
        break;
    };
}


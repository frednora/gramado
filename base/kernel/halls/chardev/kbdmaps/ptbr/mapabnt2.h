// mapabnt2.h
// Ring0 keymap support for abnt2 keyboards.
// Created by Fred Nora.

#ifndef __PTBR_MAPABNT2_H
#define __PTBR_MAPABNT2_H    1

#define ABNT2_CHARMAP_NAME  "abnt2"

// Char map size.
#define ABNT2_CHARMAP_SIZE  136 

// see: mapabnt2.c
extern unsigned char      map_abnt2[ABNT2_CHARMAP_SIZE];
extern unsigned char    shift_abnt2[ABNT2_CHARMAP_SIZE];
extern unsigned char      ctl_abnt2[ABNT2_CHARMAP_SIZE];
extern unsigned char extended_abnt2[ABNT2_CHARMAP_SIZE];
// ...

#endif   


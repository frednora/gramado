// compglob.c
// Globals for the compositor component in the project.
// Created by Fred Nora

#include "../ds.h"

int use_vsync=FALSE;

// List of pointer fo the available canvases.
// Our list will have only two of them for now ... 
// the backbuffer and the frontbuffer ,,, 
// the rest will be linked to the window structure.
unsigned long canvasList[CANVAS_COUNT_MAX];

struct canvas_information_d  *canvas_head;
struct canvas_information_d  *canvas_frontbuffer;
struct canvas_information_d  *canvas_backbuffer;


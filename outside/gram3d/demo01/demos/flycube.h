// demos.h
// Flying cube
// Created by Fred Nora.

#ifndef __DEMOS_FLYCUBE_H
#define __DEMOS_FLYCUBE_H    1

// Input support
void FlyingCubeMove(int number, int direction, float value);

// Draw a single frame.
// This is called by the gameloop.
void demoFlyingCube(int draw_terrain,unsigned long sec);

//
// #
// INITIALIZATION
//

// Setup the demos
void demoFlyingCubeSetup(void);

#endif    


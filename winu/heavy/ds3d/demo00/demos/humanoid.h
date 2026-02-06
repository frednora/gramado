// humanoid.h
// Flying cube
// Created by Fred Nora.

#ifndef __DEMOS_HUMANOID_H
#define __DEMOS_HUMANOID_H    1

// Input support
void demoHumanoidMoveCharacter(int number, int direction, float value);

// Draw a single frame.
// This is called by the gameloop.
void demoHumanoidDrawScene(unsigned long sec);

//
// #
// INITIALIZATION
//

// Setup the demos
void demoHumanoidSetup(void);

#endif    


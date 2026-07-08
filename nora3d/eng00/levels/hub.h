// hub.h
// Created by Fred Nora.

#ifndef __LEVELS_HUB_H
#define __LEVELS_HUB_H    1

//
// Levels
//

#define LEVEL_HUB  0
#define LEVEL_1  1  //#todo
#define LEVEL_2  2  //#todo
#define LEVEL_3  3  //#todo
// ...
extern int current_level;
extern int exit_level;

// Input support
void demoHumanoidMoveCharacter(int number, int direction, float value);
void demoHumanoidRotateWorld(int direction, float value);
void demoCameraOrbit(int direction, float value);
void demoCameraSpinWorld(int direction);

// Draw a single frame.
// This is called by the gameloop.
void demoHumanoidDrawScene(unsigned long sec);


void demoHumanoidUpdate(void);

//
// #
// INITIALIZATION
//

// Setup the demos
void demoHumanoidSetup(void);

#endif    


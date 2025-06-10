// cat00.h

#ifndef __DEMOS_CAT00_H
#define __DEMOS_CAT00_H    1


struct cat_demo_info_d
{
    int initialized;  // Demo initialized

    int z_counter;
    int z_max;
};
extern struct cat_demo_info_d  CatDemoInfo;


// Draw
void demoCat(void);

// Setup
// Called once
void cat00SetupDemo(void);

#endif   


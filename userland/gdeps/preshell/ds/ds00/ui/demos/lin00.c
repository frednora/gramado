// lin00.c
// Demo Lines
// Created by Fred Nora.

#include "../../ds.h"


void demoLines(void)
{
// Draw lines.

    register int i=0;

    for (i=0; i<320; i+=5)
    {
        plotLine3d ( 
                0, i, 0,
            320-i, 0, 0, 
            COLOR_WHITE); 

        plotLine3d ( 
                   0, i, 0,
            -(320-i), 0, 0, 
            COLOR_WHITE); 

        plotLine3d ( 
            -i,        0, 0,
             0, -(320-i), 0, 
            COLOR_WHITE); 

        plotLine3d ( 
           i,        0, 0,
           0, -(320-i), 0, 
            COLOR_WHITE);
    };

    refresh_screen();
}

void demoLine1(void)
{
// Draw lines.

    //int width = getWidth();
    //int height = getHeight();
    int width  = 200/2;  //320/2;
    int height = 200/2;
    int x1 = 0, y1 = 0,
        x2 = 0, y2 = height;

// Valid os mode
    if (os_mode != GRAMADO_JAIL){
        return;
    }

    while (y1 < height) {
        //g.drawLine(x1, y1, x2, y2);
        plotLine3d ( x1,y1,0, x2,y2,0, COLOR_WHITE); 
            y1+=8;                 //You should modify this if
            x2+=8;                 //it's not an equal square (like 250x250)
    };
    demoFlushSurface();  
}



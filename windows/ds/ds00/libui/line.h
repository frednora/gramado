
// line.h

#ifndef GWS_LINE_H
#define GWS_LINE_H    1


void plotLineRect4 ( int x1, int y1, int x2, int y2, int color );
void plotLineRectZ ( int x1, int y1, int z1, int x2, int y2, int z2, int color );

void testLines ( int x1, int y1, int x2, int y2, int color );
void testLinesZ ( int x1, int y1, int z1, int x2, int y2, int z2, int color );

// See: https://en.wikipedia.org/wiki/Line_drawing_algorithm
void
A_naive_line_drawing_algorithm(
    unsigned long x1,
    unsigned long x2, 
    unsigned long y1, 
    unsigned long y2, 
    unsigned long color );
    
void test_draw_line(void);
void test_draw_line2(void);


/* 
 * lineBackbufferDrawHorizontalLine:
 *     Draw a horizontal line on backbuffer. 
 */

void 
grBackbufferDrawHorizontalLine ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2,  
    unsigned int color );
    
void 
backbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags );

void 
frontbuffer_draw_horizontal_line ( 
    unsigned long x1,
    unsigned long y, 
    unsigned long x2, 
    unsigned int color,
    unsigned long rop_flags );


#endif    


//
// End.
//




// font.h
// the font support.

#ifndef GWS_FONT_H
#define GWS_FONT_H    1

//
// Font
//

extern int gfontSize;
//As fontes usadas pelo servidor gws
extern unsigned long gws_currentfont_address;  // fonte atual.
extern unsigned long g8x8fontAddress;          // 8×8, 80×25,CGA, EGA
extern unsigned long g8x14fontAddress;         // 8x14,80×25,EGA
extern unsigned long g8x16fontAddress;         // ??
extern unsigned long g9x14fontAddress;         // 9x14,80×25,MDA, Hercules
extern unsigned long g9x16fontAddress;         // 9x16,80×25,VGA
//extern unsigned long gws_eye_sprite_address;


//
// == prototypes =============================
//

int gwssrv_init_font(void);

#endif   









// bar.h
// Far Far Away area.
// Draw and update the notification bar. (yellow bar) (status bar).
// #todo
// Change the name of this file.

#ifndef __AREAS_FFA_H
#define __AREAS_FFA_H    1


struct statusbar_info_d
{
    int initialized;
    int style;
    // ...
};
extern struct statusbar_info_d  StatusBarInfo;

// ==========================================

void yellowstatus0(const char *string,int refresh);
void yellow_status(const char *string);


#endif   



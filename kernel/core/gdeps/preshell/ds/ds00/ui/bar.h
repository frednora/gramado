
// bar.h
// Far Far Away area.
// Draw and update the notification bar. (yellow bar) (status bar).
// #todo
// Change the name of this file.

#ifndef __UI_BAR_H
#define __UI_BAR_H    1

struct yellow_dialog_info_d
{
    int initialized;
    int useYellowDialog;
    int display_dialog;
    // ...    
};
extern struct yellow_dialog_info_d  YellowDialogInfo;


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

int 
yellow_status_dialog (
    int msg,
    unsigned long long1,
    unsigned long long2,
    unsigned long long3 );

int yellow_dialog_initialize(void);

#endif   



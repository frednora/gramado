// cat.h

#ifndef __DEMOS_CAT_H
#define __DEMOS_CAT_H    1

//
// models
//

struct cat_model_d
{
    int eyesVisible;
    int whiskersVisible;
    int mouthVisible;
    // ...
};

// See: demos.c
extern struct cat_model_d CatModel;


// =============================

// cat
void setupCatModel(int eyes, int whiskers, int mouth);
void demoCat(void);

#endif


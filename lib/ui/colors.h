#ifndef UI_COLORS_H
#define UI_COLORS_H

#include "backend.h"

#define rgb(r, g, b) (UIColor){r / 255.0, g / 255.0, b / 255.0, 1.0}

extern UIColor COLOR_WARM_GREY[10];
extern UIColor COLOR_NEUTRAL_GREY[10];
extern UIColor COLOR_GREEN[10];
extern UIColor COLOR_INDIGO[10];
extern UIColor COLOR_YELLOW[10];
extern UIColor COLOR_ORANGE[10];
extern UIColor COLOR_RED[10];
extern UIColor COLOR_ROSE[10];

extern UIColor* COLOR_PRIMARY;
extern UIColor* COLOR_DARK;
extern UIColor* COLOR_TEXT;
extern UIColor* COLOR_TEXT_LIGHT;

#endif

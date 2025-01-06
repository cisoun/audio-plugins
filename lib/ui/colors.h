#ifndef UI_COLORS_H
#define UI_COLORS_H

#include "backend.h"

#define rgb(r, g, b) {r / 255.0, g / 255.0, b / 255.0, 1.0}

#define LEVELS 10

static const int COLOR_LEVELS = 10;

UIColor COLOR_ROSADE[LEVELS];
UIColor COLOR_RED[LEVELS];
UIColor COLOR_ORANGE[LEVELS];
UIColor COLOR_YELLOW[LEVELS];

UIColor* COLOR_PRIMARY;
UIColor* COLOR_DARK;

#endif

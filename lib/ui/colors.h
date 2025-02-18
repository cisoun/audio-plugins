#ifndef UI_COLORS_H
#define UI_COLORS_H

#include "backend.h"

#define rgb(r, g, b) {r / 255.0, g / 255.0, b / 255.0, 1.0}

static const int _UI_COLOR_LEVELS = 10;

UIColor COLOR_WARM_GREY    [_UI_COLOR_LEVELS];
UIColor COLOR_NEUTRAL_GREY [_UI_COLOR_LEVELS];

UIColor COLOR_GREEN        [_UI_COLOR_LEVELS];
UIColor COLOR_INDIGO       [_UI_COLOR_LEVELS];
UIColor COLOR_ORANGE       [_UI_COLOR_LEVELS];
UIColor COLOR_RED          [_UI_COLOR_LEVELS];
UIColor COLOR_ROSE         [_UI_COLOR_LEVELS];
UIColor COLOR_YELLOW       [_UI_COLOR_LEVELS];

UIColor* COLOR_PRIMARY;
UIColor* COLOR_DARK;
UIColor* COLOR_TEXT;
UIColor* COLOR_TEXT_LIGHT;

#endif

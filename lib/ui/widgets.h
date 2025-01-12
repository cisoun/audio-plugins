#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include "backend.h"
#include "colors.h"

static const float _UI_KNOB_ANGLE_MIN   = deg2rad(140);
static const float _UI_KNOB_ANGLE_MAX   = deg2rad(400);
static const float _UI_KNOB_ANGLE_RANGE = _UI_KNOB_ANGLE_MAX - _UI_KNOB_ANGLE_MIN;

typedef struct {
	WIDGET
	UIColor* color;
	char*    text;
} UIButton;

typedef struct {
	WIDGET
	UIColor* color;
	float    value;
} UIKnob;

typedef struct {
	WIDGET
	char** items;
	int    items_count;
	float  offset_y;
	int    selected_index;
} UIList;

typedef struct {
	WIDGET
	bool     bold;
	UIColor  color;
	float    font_size;
	bool     italic;
	char*    text;
	UIOrigin origin;
} UIText;

UIWidget* ui_button            (UIButton*);
void      ui_button_draw       (UIWidget*, UIContext*);
void      ui_button_mouse_up   (UIWidget*, UIPosition, UIMouseButtons);

UIWidget* ui_knob              (UIKnob*);
void      ui_knob_destroy      (UIWidget*);
void      ui_knob_draw         (UIWidget*, UIContext*);
void      ui_knob_mouse_move   (UIWidget*, UIPosition);
void      ui_knob_scroll       (UIWidget*, UIDirections, float, float);
void      ui_knob_set_value    (UIWidget*, float value);

UIWidget* ui_list              (UIList*);
void      ui_list_draw         (UIWidget*, UIContext*);
char*     ui_list_get_selected (UIWidget*);
void      ui_list_mouse_down   (UIWidget*, UIPosition, UIMouseButtons);
void      ui_list_mouse_move   (UIWidget*, UIPosition);
void      ui_list_scroll       (UIWidget*, UIDirections, float, float);

UIWidget* ui_text              (UIText*);
void      ui_text_draw         (UIWidget*, UIContext*);
UISize    ui_text_get_size     (UIContext*, UITextProperties*);


#endif

#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include "backend.h"
#include "colors.h"

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

UIWidget* ui_button          (UIButton*);
void      ui_button_draw     (UIWidget*, UIContext*);
void      ui_button_mouse_up (UIWidget*, UIMouseButtons);

UIWidget* ui_knob            (UIKnob*);
void      ui_knob_destroy    (UIWidget*);
void      ui_knob_draw       (UIWidget*, UIContext*);
void      ui_knob_mouse_move (UIWidget*, UIPosition, UIPosition);
void      ui_knob_scroll     (UIWidget*, UIDirections, float, float);
void      ui_knob_set_value  (UIWidget*, float value);

UIWidget* ui_list            (UIList*);

UIWidget* ui_text            (UIText*);
void      ui_text_draw       (UIWidget*, UIContext*);
UISize    ui_text_get_size   (UIContext*, UITextProperties*);


#endif

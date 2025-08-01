#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include "backend.h"
#include "../kit.h"

#define UIKnob(...) ui_knob(&(UIKnob)__VA_ARGS__)
#define UIText(...) ui_knob(&(UIKnob)__VA_ARGS__)


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

#define UILIST \
	WIDGET \
	KitArray*    items; \
	unsigned int item_height; \
	float        offset_y; \
	unsigned int selection_index; \
	void         (*selection_change) (UIWidget*, unsigned int);

typedef struct {
	UILIST
} UIList;

typedef struct {
	UILIST
	char** filters;
} UIFileList;

typedef struct {
	WIDGET
	bool     bold;
	UIColor* color;
	float    font_size;
	bool     italic;
	UIOrigin origin;
	char*    text;
} UIText;

UIWidget*    ui_button                   (UIButton*);
void         ui_button_destroy           (UIButton*);
void         ui_button_draw              (UIWidget*, UIContext*);
void         ui_button_mouse_up          (UIWidget*, UIPosition, UIMouseButtons);
UIButton*    ui_button_new               (void);
void         ui_button_state_change      (UIWidget*, UIWidgetStates);

UIWidget*    ui_file_list                (UIFileList*);
void         ui_file_list_clear          (UIWidget*);
void         ui_file_list_destroy        (UIFileList*);
void         ui_file_list_draw           (UIWidget*, UIContext*);
KitFileInfo* ui_file_list_get_selection  (UIWidget*);
bool         ui_file_list_is_valid       (UIWidget*, KitFileInfo*);
void         ui_file_list_mouse_down     (UIWidget*, UIPosition, UIMouseButtons);

UIWidget*    ui_knob                     (UIKnob*);
void         ui_knob_destroy             (UIKnob*);
void         ui_knob_draw                (UIWidget*, UIContext*);
void         ui_knob_mouse_move          (UIWidget*, UIPosition);
void         ui_knob_scroll              (UIWidget*, UIDirections, float, float);
void         ui_knob_set_value           (UIWidget*, float);

UIWidget*    ui_list                     (UIList*);
void         ui_list_clear               (UIList*);
void         ui_list_draw                (UIWidget*, UIContext*);
void*        ui_list_get_selection       (UIWidget*);
unsigned int ui_list_get_selection_index (UIWidget*);
void         ui_list_mouse_down          (UIWidget*, UIPosition, UIMouseButtons);
void         ui_list_mouse_move          (UIWidget*, UIPosition);
void         ui_list_scroll              (UIWidget*, UIDirections, float, float);
void         ui_list_select              (UIWidget*, unsigned int);
void         ui_list_selection_change    (UIWidget*, unsigned int);

UIWidget*    ui_text                     (UIText*);
UIWidget*    ui_text_from                (UIText*);
void         ui_text_destroy             (UIText*);
void         ui_text_draw                (UIWidget*, UIContext*);
UISize       ui_text_get_size            (UIContext*, UITextProperties*);
void         ui_text_set_text            (UIWidget*, char*);

#endif

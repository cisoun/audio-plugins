#include "widgets.h"
#include "backend.h"

UIWidget* ui_button(UIButton* b) {
	set_default(b->color, COLOR_PRIMARY);
	set_default(b->size.width, 80);
	set_default(b->size.height, 20);
	b->draw     = ui_button_draw;
	b->mouse_up = ui_button_mouse_up;
	b->type     = WIDGET_BUTTON;
	return (UIWidget*)b;
}

void ui_button_draw(UIWidget* w, UIContext* c) {
	UIButton* b = (UIButton*)w;
	UITextProperties tp = {
		.text  = b->text,
		.color = b->color[9],
		.position = {
			.x = b->position.x + b->size.width / 2,
			.y = b->position.y + b->size.height / 2
		},
		.size   = 12,
		.bold   = false,
		.origin = ORIGIN_M
	};
	bool hovered = w->state & WIDGET_STATE_HOVERED;
	ui_draw_rounded_rectangle(c, &(UIRoundedRectangleProperties){
		.color    = hovered ? b->color[6] : b->color[5],
		.position = b->position,
		.radius   = 3.0,
		.size     = b->size,
		.stroke   = {
			.color = hovered ? b->color[7] : b->color[6],
			.width = 1
		}
	});
	ui_draw_text(c, &tp);
}

void ui_button_mouse_up(UIWidget* w, UIMouseButtons b) {
	if (b == MOUSE_BUTTON_LEFT && has_flag(w->state, WINDOW_STATE_HOVERED)) {
		if (w->click) {
			w->click(w);
		}
	}
}

UIWidget* ui_knob(UIKnob* k) {
	set_default(k->color, COLOR_PRIMARY);
	k->draw       = ui_knob_draw;
	k->mouse_move = ui_knob_mouse_move;
	k->scroll     = ui_knob_scroll;
	k->type       = WIDGET_KNOB;
	return (UIWidget*)k;
}

inline void ui_knob_destroy(UIWidget* w) {
	UIKnob* k = (UIKnob*)w;
	destroy(k);
}

void ui_knob_draw(UIWidget* w, UIContext* c) {
	UIKnob* k = (UIKnob*)w;
	float midW       = k->size.width / 2,
	      midH       = k->size.height / 2,
	      midX       = k->position.x + midW,
	      midY       = k->position.y + midH,
	      radius     = midW > midH ? midH : midW,
	      stroke     = radius * 0.4,
		  knobRadius = radius * 0.6,
		  arcRadius  = radius - stroke / 2,
		  angle      = _UI_KNOB_ANGLE_RANGE * k->value + _UI_KNOB_ANGLE_MIN;

	// Knob.
	ui_draw_circle(c, &(UICircleProperties){
		.radius = radius * 0.6,
		.position = {midX, midY}
	});

	// Knob indicator.
	ui_draw_lines(c, &(UILinesProperties){
		.positions = (UIPosition*[]){
			&(UIPosition){ midX, midY },
			&(UIPosition){
				midX + knobRadius * cos(angle),
				midY + knobRadius * sin(angle)
			}
		},
		.positions_count = 2,
		.stroke = {
			.color = COLOR_DARK[7],
			.width = 2
		}
	});

	// Outer arc.
	ui_draw_arc(c, &(UIArcProperties){
		.angle = {_UI_KNOB_ANGLE_MIN, _UI_KNOB_ANGLE_MAX},
		.position =  {midX, midY},
		.radius = arcRadius,
		.stroke = {
			.color = COLOR_DARK[2],
			.width = stroke
		}
	});

	// Inner arc.
	ui_draw_arc(c, &(UIArcProperties){
		.angle = {_UI_KNOB_ANGLE_MIN, _UI_KNOB_ANGLE_MAX},
		.position =  {midX, midY},
		.radius = arcRadius,
		.stroke = {
			.color = COLOR_DARK[0],
			.width = stroke * 0.5
		}
	});

	// Inner arc (value).
	ui_draw_arc(c, &(UIArcProperties){
		.angle = {_UI_KNOB_ANGLE_MIN, angle},
		.position =  {midX, midY},
		.radius = arcRadius,
		.stroke = {
			.color = k->color[6],
			.width = stroke * 0.5
		}
	});
}

static int knob_last_y = 0;

void ui_knob_mouse_move(UIWidget* w, UIPosition screen, UIPosition client) {
	UIKnob* k = (UIKnob*)w;
	if (has_flag(w->state, WIDGET_STATE_CLICKED)) {
		ui_knob_set_value(w, k->value - (screen.y - knob_last_y) / 100.0);
	}
	knob_last_y = screen.y;
}

void ui_knob_scroll(UIWidget* w, UIDirections direction, float dx, float dy) {
	UIKnob* k = (UIKnob*)w;
	dy = min(max(dy, -10), 10); // Limit speed, especially for mouse wheels.
	ui_knob_set_value(w, k->value + dy / 200);
}

void ui_knob_set_value(UIWidget* w, float value) {
	UIKnob* k = (UIKnob*)w;
	k->value = min(max(value, 0.0), 1.0);
}

UIWidget* ui_text(UIText* t) {
	t->draw = ui_text_draw;
	t->type = WIDGET_TEXT;
	return (UIWidget*)t;
}

void ui_text_draw(UIWidget* w, UIContext* c) {
	UIText* t = (UIText*)w;
	UITextProperties tp = {
		.bold     = t->bold,
		.color    = t->color,
		.origin   = t->origin,
		.position = t->position,
		.size     = t->font_size,
		.text     = t->text
	};
	ui_draw_text(c, &tp);
}

UISize ui_text_get_size(UIContext* c, UITextProperties* p) {
	cairo_text_extents_t te;
	cairo_set_font_size(c, p->size);
	cairo_text_extents(c, p->text, &te);
	return (UISize){
		.width = te.width,
		.height = te.height
	};
}

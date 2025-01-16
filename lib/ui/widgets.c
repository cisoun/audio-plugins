#include "backend.h"
#include "colors.h"
#include "widgets.h"

const int UI_LIST_ITEM_HEIGHT = 18;

static int knob_last_y = 0;

UIWidget* ui_button(UIButton* b) {
	set_default(b->color,       COLOR_PRIMARY);
	set_default(b->size.height, 20);
	set_default(b->size.width,  80);
	set_default(b->draw,        ui_button_draw);
	set_default(b->mouse_up,    ui_button_mouse_up);
	b->type     = WIDGET_BUTTON;
	return (UIWidget*)b;
}

UIButton* ui_button_new() {
	UIButton* b    = new(UIButton);
	b->color       = COLOR_PRIMARY;
	b->size        = (UISize){80, 20};
	b->draw        = ui_button_draw;
	b->mouse_up    = ui_button_mouse_up;
	b->type        = WIDGET_BUTTON;
	return b;
}

inline void ui_button_destroy(UIButton* w) {
	UIButton* b = (UIButton*)w;
	destroy(b);
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

void ui_button_mouse_up(UIWidget* w, UIPosition client, UIMouseButtons b) {
	if (b == MOUSE_BUTTON_LEFT && has_flag(w->state, WINDOW_STATE_HOVERED)) {
		if (w->click) {
			w->click(w);
		}
	}
}

UIWidget* ui_file_list(UIFileList* l) {
	ui_list((UIList*)l);
	l->draw = ui_file_list_draw;
	return (UIWidget*)l;
}

void ui_file_list_clear(UIWidget* w) {
	UIFileList* l = (UIFileList*)w;
	kit_file_info_array_clear(l->items);
}

void ui_file_list_destroy(UIFileList* f) {
	destroy(f);
}

void ui_file_list_draw(UIWidget* w, UIContext* c) {
	UIFileList* l    = (UIFileList*)w;
	int x            = w->position.x,
	    y            = w->position.y,
	    width        = w->size.width,
	    height       = w->size.height;
	const int HEIGHT = l->item_height;

	cairo_save(c);

	cairo_rectangle(c, x, y, width, height);
	cairo_set_source_rgba(c, ui_color_to_cairo(COLOR_DARK[1]));
	cairo_fill_preserve(c);
	cairo_set_source_rgba(c, ui_color_to_cairo(COLOR_DARK[3]));
	cairo_set_line_width(c, 1);
	cairo_stroke(c);

	cairo_rectangle(c, x, y, width, height);
	cairo_clip(c);

	// Draw odd/even lines.
	for (int i = -1; i < height / HEIGHT / 2 + 1; i++) {
		cairo_rectangle(c, x, y + ((int)l->offset_y % (HEIGHT * 2)) + (i * 2 + 1) * HEIGHT, width, HEIGHT);
		cairo_set_source_rgba(c, ui_color_to_cairo(COLOR_DARK[2]));
		cairo_fill(c);
	}

	// Draw selection.
	if (l->selected_index > -1) {
		cairo_rectangle(c, x, y + l->offset_y + HEIGHT * l->selected_index, width, HEIGHT);
		cairo_set_source_rgba(c, ui_color_to_cairo(COLOR_DARK[4]));
		cairo_fill(c);
	}

	// Draw text.
	KitArray* items = l->items;
	for (int i = 0; items && i < items->count; i++) {
		KitFileInfo* fi = items->items[i];
		if (fi == NULL) {
			continue;
		}
		ui_draw_text(c, &(UITextProperties){
			.color    =  ui_file_list_is_valid(w, fi) ? COLOR_DARK[8] : COLOR_DARK[5],
			.position = (UIPosition){
				w->position.x + 30,
				w->position.y + l->offset_y + i * HEIGHT + HEIGHT / 2
			},
			.origin   = ORIGIN_W,
			.text     = fi->name,
			.bold     = (fi->type == KIT_FILE_TYPE_FOLDER)
		});
		if (fi->type == KIT_FILE_TYPE_FOLDER) {
			ui_draw_rectangle(c, &(UIRectangleProperties){
				.color    = COLOR_DARK[5],
				.position = {
					w->position.x + 10,
					w->position.y + l->offset_y + i * HEIGHT + 4
				},
				.size = {10, 10}
			});
		}
	}
	cairo_restore(c);
}

bool ui_file_list_is_valid(UIWidget* w, KitFileInfo* fi) {
	UIFileList* l = (UIFileList*)w;
	if (l->filters == NULL) {
		return true;
	}
	char** filter = l->filters;
	while (*filter != NULL) {
		if (kit_string_ends_with(fi->name, *filter) == 1) {
			return true;
		}
		++filter;
	}
	return false;
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
		.radius   = radius * 0.6,
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
		.angle    = {_UI_KNOB_ANGLE_MIN, _UI_KNOB_ANGLE_MAX},
		.position =  {midX, midY},
		.radius   = arcRadius,
		.stroke   = {
			.color = COLOR_DARK[2],
			.width = stroke
		}
	});

	// Inner arc.
	ui_draw_arc(c, &(UIArcProperties){
		.angle    = {_UI_KNOB_ANGLE_MIN, _UI_KNOB_ANGLE_MAX},
		.position =  {midX, midY},
		.radius   = arcRadius,
		.stroke   = {
			.color = COLOR_DARK[0],
			.width = stroke * 0.5
		}
	});

	// Inner arc (value).
	ui_draw_arc(c, &(UIArcProperties){
		.angle    = {_UI_KNOB_ANGLE_MIN, angle},
		.position =  {midX, midY},
		.radius   = arcRadius,
		.stroke   = {
			.color = k->color[6],
			.width = stroke * 0.5
		}
	});
}

void ui_knob_mouse_move(UIWidget* w, UIPosition client) {
	UIKnob* k = (UIKnob*)w;
	if (has_flag(w->state, WIDGET_STATE_CLICKED)) {
		ui_knob_set_value(w, k->value - (client.y - knob_last_y) / 100.0);
	}
	knob_last_y = client.y;
}

void ui_knob_scroll(UIWidget* w, UIDirections direction, float dx, float dy) {
	UIKnob* k = (UIKnob*)w;
	dy = clamp(dy, -10, 10); // Limit speed, especially for mouse wheels.
	ui_knob_set_value(w, k->value + dy / 200);
}

void ui_knob_set_value(UIWidget* w, float value) {
	UIKnob* k = (UIKnob*)w;
	k->value  = clamp(value, 0.0, 1.0);
}

UIWidget* ui_list (UIList* l) {
	l->item_height = UI_LIST_ITEM_HEIGHT;
	l->draw = ui_list_draw;
	l->mouse_down = ui_list_mouse_down;
	l->mouse_move = ui_list_mouse_move;
	l->scroll = ui_list_scroll;
	l->type = WIDGET_LIST;
	return (UIWidget*)l;
}

void ui_list_clear(UIList* l) {
	if (l->items) {
		kit_array_clear(l->items);
	}
}

void ui_list_draw(UIWidget* w, UIContext* c) {
	// Nothing to do now.
}

void* ui_list_get_selected(UIWidget* w) {
	UIList* l       = (UIList*)w;
	KitArray* items = l->items;
	const int index = l->selected_index;
	if (index > -1 && items && items->items[index]) {
		return items->items[index];
	}
	return NULL;
}

void ui_list_mouse_down(UIWidget* w, UIPosition p, UIMouseButtons b){
	UIList* l         = (UIList*)w;
	const int count   = l->items->count;
	const int height  = count * UI_LIST_ITEM_HEIGHT;
	const int y       = ((p.y - l->offset_y) / height) * count;
	l->selected_index = clamp(y, 0, count - 1);
}

void ui_list_mouse_move(UIWidget* w, UIPosition client){
	if (has_flag(w->state, WIDGET_STATE_CLICKED)) {
		ui_list_mouse_down(w, client, 0);
	}
}

void ui_list_scroll(UIWidget* w, UIDirections direction, const float dx, const float dy) {
	UIList* l     = (UIList*)w;
	const int min = -(l->items->count * UI_LIST_ITEM_HEIGHT - w->size.height);
	l->offset_y   = clamp(l->offset_y + dy, min, 0);
}

UIWidget* ui_text(UIText* t) {
	t->color     = COLOR_DARK[7];
	t->draw      = ui_text_draw;
	t->font_size = UI_DEFAULT_FONT_SIZE;
	t->type      = WIDGET_TEXT;
	return (UIWidget*)t;
}

UIWidget* ui_text_new() {
	UIText* t    = new(UIText);
	t->color     = COLOR_DARK[7];
	t->draw      = ui_text_draw;
	t->font_size = UI_DEFAULT_FONT_SIZE;
	t->type      = WIDGET_TEXT;
	return (UIWidget*)t;
}

void ui_text_destroy(UIText* t) {
	destroy(t);
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
		.width  = te.width,
		.height = te.height
	};
}

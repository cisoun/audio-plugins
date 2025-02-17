#include "backend.h"
#include "colors.h"
#include "file-dialog.h"
#include "widgets.h"
#include <limits.h>
#include <stdio.h>

static double mouse_click_time;

static void handle_expose(UIWindow* w, const PuglEvent* e) {
	if (!w->is_dirty) {
		return;
	}
	UIContext*             context = (UIContext*)puglGetContext(w->view);
	const PuglExposeEvent* event   = &e->expose;
	const float            scale   = 1 / w->scale;
	ui_window_draw_area((UIWidget*)w, context, &(UIArea){
		scale * (event->x),
		scale * (event->y),
		scale * (event->x + event->width),
		scale * (event->y + event->height)
	});
}

static void handle_key_down(UIWindow* w, const PuglEvent* e) {
	w->on_key_down(w, e->key.keycode);
}

static void handle_mouse_down(UIWindow* w, const PuglEvent* e) {
	int x = e->motion.x / w->scale,
	    y = e->motion.y / w->scale;
	UIMouseButtons b = e->button.button;
	ui_window_mouse_down(w, (UIPosition){x, y}, b);
}

static void handle_mouse_enter(UIWindow* w) {
	flag_on(w->state, WIDGET_STATE_HOVERED);
}

static void handle_mouse_leave(UIWindow* w) {
	flag_off(w->state, WIDGET_STATE_HOVERED);
	for (int i = 0; i < w->children_count; i++) {
		UIWidget* widget = w->children[i];
		flag_off(widget->state, WIDGET_STATE_HOVERED);
	}
}

static void handle_mouse_move(UIWindow* w, const PuglEvent* e) {
	int x = e->motion.x / w->scale,
	    y = e->motion.y / w->scale;
	ui_window_mouse_move(w, (UIPosition){x, y});
}

static void handle_mouse_scroll(UIWindow* w, const PuglEvent* e) {
	if (w->hovered_widget && w->hovered_widget->scroll) {
		w->hovered_widget->scroll(
			w->hovered_widget,
			(UIDirections)e->scroll.direction,
			e->scroll.dx,
			e->scroll.dy
		);
	}
}

static void handle_mouse_up(UIWindow* w, const PuglEvent* e) {
	UIPosition p = {
		.x = e->motion.x / w->scale,
	    .y = e->motion.y / w->scale
	};
	UIMouseButtons b = e->button.button;
	ui_window_mouse_up(w, p, b, e->button.time);
}

static PuglStatus handle_event(PuglView* view, const PuglEvent* event)
{
	UIWindow* window = (UIWindow*)puglGetHandle(view);
	switch (event->type) {
		case PUGL_MOTION:         handle_mouse_move(window, event); break;
		case PUGL_POINTER_IN:     handle_mouse_enter(window); break;
		case PUGL_POINTER_OUT:    handle_mouse_leave(window); break;
		case PUGL_SCROLL:         handle_mouse_scroll(window, event); break;
 		case PUGL_BUTTON_PRESS:   handle_mouse_down(window, event); break;
        case PUGL_BUTTON_RELEASE: handle_mouse_up(window, event); break;
		case PUGL_KEY_PRESS:      handle_key_down(window, event); break;
		//case PUGL_REALIZE:        break;
		//case PUGL_UPDATE:         handle_update(window, event); break;
		//case PUGL_UPDATE:         break;
		case PUGL_EXPOSE:         handle_expose(window, event); break;
		case PUGL_CLOSE:
			window->on_close(window);
			break;
		default:
			break;
	}
	return PUGL_SUCCESS;
}

UIApp* ui_app(UIApp* a) {
	PuglWorld* world = puglNewWorld(PUGL_PROGRAM, 0);
	puglSetWorldString(world, PUGL_CLASS_NAME, a->name);
	a->world = world;
	return a;
}

void ui_app_close(UIApp* a) {
	a->quit = true;
}

void ui_app_destroy(UIApp* a) {
	puglFreeWorld(a->world);
}

void ui_app_run(UIApp* a) {
	while (!a->quit) {
		// Here we wait for Pugl events to update.
		// Otherwise, replace -1 by 1.0 / FPS.
		puglUpdate(a->world, -1);
	}
}

inline UIArea ui_area_add(UIArea* a, UIArea* b) {
	return (UIArea){
		min(a->x1, b->x1),
		min(a->y1, b->y1),
		max(a->x2, b->x2),
		max(a->y2, b->y2)
	};
}

void ui_draw_arc(UIContext* c, UIArcProperties* p) {
	cairo_set_line_width(c, p->stroke.width);
	cairo_set_source_rgba(c, ui_color_to_cairo(p->stroke.color));
	cairo_arc(c, p->position.x, p->position.y, p->radius, p->angle.start, p->angle.end);
	cairo_stroke(c);
}

void ui_draw_circle(UIContext* c, UICircleProperties* p) {
	cairo_arc(c, p->position.x, p->position.y, p->radius, 0, 2 * M_PI);
	cairo_set_source_rgba(c, ui_color_to_cairo(p->color));
	cairo_fill_preserve(c);
	cairo_set_source_rgba(c, ui_color_to_cairo(p->stroke.color));
	cairo_set_line_width(c, p->stroke.width);
	cairo_stroke(c);
}

void ui_draw_lines(UIContext* c, UILinesProperties* l) {
	assert(l->positions_count > 0);
	UIPosition* position = l->positions[0];
	cairo_new_path(c);
	cairo_move_to(c, position->x, position->y);
	for (int i = 1; i < l->positions_count; i++) {
		position = l->positions[i];
		cairo_line_to(c, position->x, position->y);
	}
	cairo_close_path(c);
	cairo_set_line_width(c, l->stroke.width);
	cairo_set_source_rgba(c, ui_color_to_cairo(l->stroke.color));
	cairo_stroke(c);
}

void ui_draw_rectangle(UIContext* c, UIRectangleProperties* p) {
	cairo_rectangle(c, p->position.x, p->position.y, p->size.width, p->size.height);
	cairo_set_source_rgba(c, ui_color_to_cairo(p->color));
	cairo_fill_preserve(c);
	cairo_set_source_rgba(c, ui_color_to_cairo(p->stroke.color));
	cairo_set_line_width (c, p->stroke.width);
	cairo_stroke(c);
}

void ui_draw_rounded_rectangle(UIContext* c, UIRoundedRectangleProperties* p) {
	double x             = p->position.x,
	       y             = p->position.y,
	       width         = p->size.width,
	       height        = p->size.height,
	       aspect        = 1.0,
	       corner_radius = p->radius;

	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cairo_move_to(c, p->position.x, p->position.y);

	cairo_new_sub_path(c);
	cairo_arc(c, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc(c, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc(c, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(c, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path(c);

	cairo_set_source_rgba(c, ui_color_to_cairo(p->color));
	cairo_fill_preserve(c);
	cairo_set_source_rgba(c, ui_color_to_cairo(p->stroke.color));
	cairo_set_line_width(c, p->stroke.width);
	cairo_stroke(c);
}

void ui_draw_text(UIContext* c, UITextProperties* p) {
	set_default_float(p->size, UI_DEFAULT_FONT_SIZE);

	cairo_font_slant_t slant = p->italic ?
		CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL;
	cairo_font_weight_t weigth = p->bold ?
		CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;

	cairo_set_source_rgba(c, ui_color_to_cairo(p->color));
	cairo_select_font_face(c, "sans-serif", slant, weigth);
	cairo_set_font_size(c, p->size);

	cairo_text_extents_t te;
	cairo_text_extents(c, p->text, &te);

	// Trying stupid approximation to improve vertical alignment.
	int height = p->size * 0.8; //-te.y_bearing;
	int x = p->position.x;
	int y = p->position.y;
	switch (p->origin) {
		case ORIGIN_NW: y += height; break;
		case ORIGIN_N:  x -= te.width / 2; y += height; break;
		case ORIGIN_NE: x -= te.width;     y += height; break;
		case ORIGIN_W:  y += height / 2; break;
		case ORIGIN_M:  x -= te.width / 2; y += height / 2; break;
		case ORIGIN_E:  x -= te.width;     y += height / 2; break;
		case ORIGIN_SW: break;
		case ORIGIN_S:  x -= te.width / 2; break;
		case ORIGIN_SE: x -= te.width; break;
		default: break;
	}

	cairo_move_to(c, x, y);
	cairo_show_text(c, p->text);
}

inline UISurface* ui_surface(UISize* size) {
	return cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size->width, size->height);
}

inline void ui_surface_destroy(UISurface* s) {
	cairo_surface_destroy(s);
}

inline UIContext* ui_surface_draw_begin(UISurface* s) {
	return cairo_create(s);
}

inline void ui_surface_draw_end(UISurface* s, UIContext* c) {
	cairo_surface_flush(s);
	cairo_destroy(c);
}

inline void ui_widget_disable(UIWidget* w) {
	ui_widget_set_state(w, WIDGET_STATE_DISABLED);
}

void ui_widget_double_click(UIWidget* w) {
	if (w->double_click) {
		w->double_click(w);
	}
}

void ui_widget_draw(UIWidget* w, UIContext* c) {
	assert(w->draw);
	if (has_flag(w->state, WIDGET_STATE_HIDDEN)) {
		return;
	}
	w->draw(w, c);
	for (int i = 0; i < w->children_count; i++) {
		UIWidget* widget = w->children[i];
		ui_widget_draw(widget, c);
	}
}

void ui_widget_draw_area(UIWidget* w, UIContext* c, UIArea* r) {
	cairo_rectangle(c, r->x1, r->y1, r->x2 - r->x1, r->y2 - r->y1);
	cairo_clip(c);
	ui_widget_draw(w, c);
	cairo_reset_clip(c);
}

inline void ui_widget_enable(UIWidget* w) {
	ui_widget_unset_state(w, WIDGET_STATE_DISABLED);
}

inline UIArea ui_widget_get_area(UIWidget* w) {
	return (UIArea){
		w->position.x,
		w->position.y,
		w->position.x + w->size.width,
		w->position.y + w->size.height
	};
}

UIWindow* ui_widget_get_window(UIWidget* w) {
	UIWidget* widget = w;
	while (widget && widget->type != WIDGET_WINDOW) {
		widget = widget->parent;
	}
	return (UIWindow*)widget;
}

static bool ui_widget_intersect_position(UIWidget* w, UIPosition* p) {
	return p->x >= w->position.x &&
	       p->x < w->position.x + w->size.width &&
	       p->y >= w->position.y &&
	       p->y < w->position.y + w->size.height;
}

static bool ui_widget_intersect_area(UIWidget* w, UIArea* a) {
	if (w->position.x > a->x2 || a->x1 > w->position.x + w->size.width) {
		return false;
	}
	if (w->position.y > a->y2 || a->y1 > w->position.y + w->size.height) {
		return false;
	}
	return true;
}

void ui_widget_mouse_down(UIWidget* w, UIPosition p, UIMouseButtons b) {
	ui_widget_set_state(w, WIDGET_STATE_CLICKED);
	if (w->mouse_down) {
		w->mouse_down(w, (UIPosition){p.x - w->position.x, p.y - w->position.y}, b);
	}
}

void ui_widget_mouse_enter(UIWidget* w) {
	ui_widget_set_state(w, WIDGET_STATE_HOVERED);
}

void ui_widget_mouse_leave(UIWidget* w) {
	ui_widget_unset_state(w, WIDGET_STATE_HOVERED);
}

void ui_widget_mouse_move(UIWidget* w, UIPosition p) {
	if (w->mouse_move) {
		w->mouse_move(w, (UIPosition){p.x - w->position.x, p.y - w->position.y});
	}
}

void ui_widget_mouse_up(UIWidget* w, UIPosition p, UIMouseButtons b) {
	ui_widget_unset_state(w, WIDGET_STATE_CLICKED);
	if (w->mouse_up) {
		w->mouse_up(w, (UIPosition){p.x - w->position.x, p.y - w->position.y}, b);
	}
}

void ui_widget_must_redraw(UIWidget* w) {
	UIWindow* window = ui_widget_get_window(w);
	if (window == NULL) {
		return;
	}
	UIArea a = ui_widget_get_area(w);
	ui_window_must_redraw(window, &a);
}

void ui_widget_scroll(UIWidget* w, UIDirections d, float dx, float dy) {
	if (w->scroll) {
		w->scroll(w, d, dx, dx);
	}
}

void ui_widget_set_state(UIWidget* w, UIWidgetStates s) {
	flag_on(w->state, s);
	if (w->state_change) {
		w->state_change(w, s);
	}
}

void ui_widget_set_surface(UIWidget* w, UISurface* s) {
	if (w->surface != NULL) {
		ui_surface_destroy(w->surface);
	}
	w->surface = s;
}


void ui_widget_unset_state(UIWidget* w, UIWidgetStates s) {
	flag_off(w->state, s);
	if (w->state_change) {
		w->state_change(w, s);
	}
}

UIWindow* ui_window(UIWindow* w, UIApp* a) {
	UIWindow* window = new(UIWindow);
	PuglView* view   = puglNewView(a->world);
	memcpy(window, w, sizeof(UIWindow));
	puglSetViewString(view, PUGL_WINDOW_TITLE, w->title);
	puglSetSizeHint  (view, PUGL_DEFAULT_SIZE, w->size.width * w->scale, w->size.height * w->scale);
	//puglSetSizeHint (view, PUGL_MIN_SIZE, w->size.width, w->size.height);
	//puglSetSizeHint (view, PUGL_MIN_ASPECT, 1, 1);
	//puglSetSizeHint (view, PUGL_MAX_ASPECT, 16, 9);
	puglSetViewHint  (view, PUGL_RESIZABLE, w->resizable);
	puglSetViewHint  (view, PUGL_IGNORE_KEY_REPEAT, PUGL_TRUE);
	puglSetBackend   (view, puglCairoBackend());
	puglSetViewHint  (view, PUGL_IGNORE_KEY_REPEAT, true);
	puglSetEventFunc (view, handle_event);
	puglSetHandle    (view, window);
	set_default      (window->draw,       ui_window_draw);
	set_default      (window->draw_area,  ui_widget_draw_area);
	set_default      (window->draw_begin, ui_window_draw_begin);
	set_default      (window->draw_end,   ui_window_draw_end);
	ui_window_attach (window, w->children);
	window->app           = a;
	window->is_dirty      = false;
	window->type          = WIDGET_WINDOW;
	window->view          = view;
	window->dirty_area.x1 = 0;
	window->dirty_area.y1 = 0;
	window->dirty_area.x2 = w->size.width;
	window->dirty_area.y2 = w->size.height;
	return window;
}

void ui_window_attach(UIWindow* w, UIWidget** widgets) {
	w->children = widgets;
	for (int i = 0; i < w->children_count; i++) {
		w->children[i]->parent = (UIWidget*)w;
	}
}

void ui_window_close(UIWindow* w) {
	puglFreeView(w->view);
}

void ui_window_draw(UIWidget* w, UIContext* c) {
	UIWindow* window = (UIWindow*)w;
	ui_window_draw_begin(window, c);
	if (window->draw) {
		window->draw((UIWidget*)window, c);
	}
	for (int i = 0; i < w->children_count; i++) {
		UIWidget* widget = w->children[i];
		ui_widget_draw(widget, c);
	}
	ui_window_draw_end(window, c);
}

void ui_window_draw_area(UIWidget* w, UIContext* c, UIArea* a) {
	UIWindow* window = (UIWindow*)w;
	ui_window_draw_begin(window, c);

	cairo_rectangle(c, a->x1, a->y1, a->x2 - a->x1, a->y2 - a->y1);
	cairo_clip_preserve(c);

	if (window->draw) {
		window->draw((UIWidget*)window, c);
	}
	for (int i = 0; i < w->children_count; i++) {
		UIWidget* widget = w->children[i];
		if (ui_widget_intersect_area(widget, a)) {
			ui_widget_draw_area(widget, c, a);
		}
	}

	ui_window_draw_end(window, c);
	window->is_dirty = false;
}

void ui_window_draw_begin(UIWindow* w, UIContext* c) {
	cairo_scale(c, w->scale, w->scale);
}

void ui_window_draw_end(UIWindow* w, UIContext* c) {
	// DEBUG
	return;
	if (w->hovered_widget) {
		UIWidget* widget = w->hovered_widget;
		ui_draw_rectangle(c, &(UIRectangleProperties){
			.position = widget->position,
			.size     = widget->size,
			.stroke   = {.color = {1, 0, 0, 1}, .width = 2}
		});
	}
	if (w->is_dirty) {
		ui_draw_rectangle(c, &(UIRectangleProperties){
			.position = {w->dirty_area.x1, w->dirty_area.y1},
			.size     = {
				w->dirty_area.x2 - w->dirty_area.x1,
				w->dirty_area.y2 - w->dirty_area.y1
			},
			.stroke   = {.color = {0, 1, 0, 1}, .width = 1}
		});
	}
}

void ui_window_mouse_down(UIWindow* w, UIPosition client, UIMouseButtons b) {
	if (w->hovered_widget) {
		flag_on(w->state, WIDGET_STATE_CLICKED);
		ui_widget_mouse_down(w->hovered_widget, client, b);
	}
}

static void ui_window_find_hovered_widget(UIWidget** focused, UIWidget* widget, UIPosition* p) {
	if (
		has_flag(widget->state, WIDGET_STATE_HIDDEN) ||
		has_flag(widget->state, WIDGET_STATE_DISABLED)
	) {
		return;
	}
	if (ui_widget_intersect_position(widget, p)) {
		*focused = widget;
	}
	if (widget->children_count > 0) {
		for (int i = 0; i < widget->children_count; i++) {
			UIWidget* w = widget->children[i];
			ui_window_find_hovered_widget(focused, w, p);
		}
	}
}

void ui_window_mouse_move(UIWindow* w, UIPosition client) {
	const int client_x = client.x;
	const int client_y = client.y;
	UIPosition position = {client_x, client_y};

	if (is_flag(w->state, WIDGET_STATE_HOVERED)) {
		UIWidget* focused = NULL;
		for (int i = 0; i < w->children_count; i++) {
			UIWidget* widget = w->children[i];
			ui_window_find_hovered_widget(&focused, widget, &position);
		}
		if (focused && focused != w->hovered_widget) {
			if (w->hovered_widget) {
				ui_widget_mouse_leave(w->hovered_widget);
			}
			ui_widget_mouse_enter(focused);
			w->hovered_widget = focused;
		} else if (focused == NULL && w->hovered_widget) {
			ui_widget_mouse_leave(w->hovered_widget);
			w->hovered_widget = NULL;
		}
	}

	if (w->hovered_widget) {
		ui_widget_mouse_move(w->hovered_widget, client);
	}
}

void ui_window_mouse_up(UIWindow* w, UIPosition p, UIMouseButtons b, double t) {
	flag_off(w->state, WIDGET_STATE_CLICKED);
	if (w->hovered_widget) {
		UIWidget* widget = w->hovered_widget;
		if (t - mouse_click_time <= UI_DOUBLE_CLICK_TIME) {
			ui_widget_mouse_up(widget, p, b);
			ui_widget_double_click(widget);
			mouse_click_time = 0;
		} else {
			ui_widget_mouse_up(widget, p, b);
			mouse_click_time = t;
		}
	}
}

void ui_window_must_redraw(UIWindow* w, UIArea* a) {
	w->is_dirty = true;
	int scale   = w->scale;
	puglObscureRegion(
		w->view,
		scale * (a->x1),
		scale * (a->y1),
		scale * (a->x2 - a->x1),
		scale * (a->y2 - a->y1)
	);
}

void ui_window_on_close(UIWindow* w) {
	ui_window_close(w);
}

void ui_window_show(UIWindow* w) {
	puglShow(w->view, PUGL_SHOW_RAISE);
	ui_window_must_redraw(w, &(UIArea){ 0, 0, w->size.width, w->size.height});
}

#include "backend.h"
#include "colors.h"
#include "file-dialog.h"
#include "widgets.h"

static void handle_key_down(UIWindow* w, const PuglEvent* e) {
	w->on_key_down(w, e->key.keycode);
}

static void handle_mouse_down(UIWindow* w, const PuglEvent* e) {
	ui_window_mouse_down(w, (UIMouseButtons)e->button.button);
}

static void handle_mouse_enter(UIWindow* w) {
	flag_on(w->state, WINDOW_STATE_HOVERED);
}

static void handle_mouse_leave(UIWindow* w) {
	flag_off(w->state, WINDOW_STATE_HOVERED);
	for (int i = 0; i < w->widgets_count; i++) {
		UIWidget* widget = w->widgets[i];
		flag_off(widget->state, WIDGET_STATE_HOVERED);
	}
}

static void handle_mouse_move(UIWindow* w, const PuglEvent* e) {
	ui_window_mouse_move(
		w,
		(UIPosition){e->motion.xRoot, e->motion.yRoot},
		(UIPosition){e->motion.x, e->motion.y}
	);
}

static void handle_mouse_scroll(UIWindow* w, const PuglEvent* e) {
	if (w->hovered_widget && w->hovered_widget->scroll) {
		//printf("DX: %f DY: %f\n", e->scroll.dx, e->scroll.dy);
		w->hovered_widget->scroll(
			w->hovered_widget,
			(UIDirections)e->scroll.direction,
			e->scroll.dx,
			e->scroll.dy
		);
	}
}

static void handle_mouse_up(UIWindow* w, const PuglEvent* e) {
	ui_window_mouse_up(w, (UIMouseButtons)e->button.button);
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
		case PUGL_REALIZE:        break;
		case PUGL_UPDATE:
			puglPostRedisplay(view);
			break;
		case PUGL_EXPOSE: {
			UIContext* context = (UIContext*)puglGetContext(view);
			window->draw(window, context);
			break;
		}
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
		puglUpdate(a->world, 1 / 60.0);
	}
}

void ui_draw_arc(UIContext* c, UIArcProperties* p) {
	cairo_set_line_width(c, p->stroke.width);
	cairo_set_source_rgba(c, ui_color_to_cairo(p->stroke.color));
	cairo_arc(c, p->position.x, p->position.y, p->radius, p->angle.start, p->angle.end);
	cairo_stroke(c);
}

void ui_draw_circle(UIContext* c, UICircleProperties* p) {
	cairo_arc(c, p->position.x, p->position.y, p->radius, 0, 2 * M_PI);
	cairo_set_source_rgba(c, ui_color_to_cairo(COLOR_DARK[5]));
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
	cairo_font_slant_t slant = p->italic ?
		CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL;
	cairo_font_weight_t weigth = p->bold ?
		CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;

	cairo_select_font_face(c, "sans-serif", slant, weigth);
	cairo_set_font_size(c, p->size);

	cairo_text_extents_t te;
	cairo_text_extents(c, p->text, &te);

	//UISize size = ui_text_get_size(c, p);
	UISize size = {te.width, te.height};
	int x = p->position.x;
	int y = p->position.y;
	switch (p->origin) {
		case ORIGIN_NW: y += size.height; break;
		case ORIGIN_N:  x -= size.width / 2; y += size.height; break;
		case ORIGIN_NE: x -= size.width;     y += size.height; break;
		case ORIGIN_W:  y += size.height / 2; break;
		case ORIGIN_M:  x -= size.width / 2; y += size.height / 2; break;
		case ORIGIN_E:  x -= size.width;     y += size.height / 2; break;
		case ORIGIN_SW: break;
		case ORIGIN_S:  x -= size.width / 2; break;
		case ORIGIN_SE: x -= size.width; break;
		default: break;
	}

	// ui_draw_rectangle(c, &(UIRectangleProperties){
	// 	.color    = {1, 0, 0, 1},
	// 	.position = {x, y - te.height},
	// 	.size     = size,
	// });

	cairo_set_source_rgba(c, ui_color_to_cairo(p->color));
	cairo_move_to(c, x, y);
	cairo_show_text(c, p->text);
}

void ui_widget_draw(UIWidget* w, UIContext* c) {
	assert(w->draw);
	if (has_flag(w->state, WIDGET_STATE_HIDDEN)) {
		return;
	}
	w->draw(w, c);
	for (int i = 0; i < w->children_count; i++) {
		UIWidget* widget = w->children[i];
		widget->draw(widget, c);
	}
}

static bool ui_widget_intersect(UIWidget* w, int x, int y) {
	return x >= w->position.x &&
	       x < w->position.x + w->size.width &&
	       y >= w->position.y &&
	       y < w->position.y + w->size.height;
}

void ui_widget_mouse_down(UIWidget* w, UIMouseButtons b) {
	flag_on(w->state, WIDGET_STATE_CLICKED);
	if (w->mouse_down) {
		w->mouse_down(w, b);
	}
}

void ui_widget_mouse_enter(UIWidget* w) {
	flag_on(w->state, WIDGET_STATE_HOVERED);
}

void ui_widget_mouse_leave(UIWidget* w) {
	flag_off(w->state, WIDGET_STATE_HOVERED);
}

void ui_widget_mouse_move(UIWidget* w, UIPosition screen, UIPosition client) {
	if (w->mouse_move) {
		w->mouse_move(w, screen, client);
	}
}

void ui_widget_mouse_up(UIWidget* w, UIMouseButtons b) {
	flag_off(w->state, WIDGET_STATE_CLICKED);
	if (w->mouse_up) {
		w->mouse_up(w, b);
	}
}

void ui_widget_on_scroll(UIWidget* w, UIDirections d, float dx, float dy) {
	w->scroll(w, d, dx, dx);
}

UIWindow* ui_window(UIWindow* w, UIApp* a) {
	w->app   = a;
	set_default(w->scale, 2);
	PuglView* view = puglNewView(a->world);
	puglSetViewString(view, PUGL_WINDOW_TITLE, w->title);
	puglSetSizeHint  (view, PUGL_DEFAULT_SIZE, w->size.width * w->scale, w->size.height * w->scale);
	//puglSetSizeHint  (view, PUGL_MIN_SIZE, w->size.width, w->size.height);
	//puglSetSizeHint(view, PUGL_MIN_ASPECT, 1, 1);
	//puglSetSizeHint(view, PUGL_MAX_ASPECT, 16, 9);
	puglSetViewHint  (view, PUGL_RESIZABLE, w->resizable);
	puglSetViewHint  (view, PUGL_IGNORE_KEY_REPEAT, PUGL_TRUE);
	puglSetHandle    (view, w);
	puglSetBackend   (view, puglCairoBackend());
	puglSetViewHint  (view, PUGL_IGNORE_KEY_REPEAT, true);
	puglSetEventFunc (view, handle_event);
	w->view  = view;
	set_default(w->draw, ui_window_draw);
	set_default(w->draw_begin, ui_window_draw_begin);
	set_default(w->draw_end, ui_window_draw_end);
	ui_window_attach(w, w->widgets);
	return w;
}

void ui_window_attach(UIWindow* w, UIWidget** widgets) {
	w->widgets = widgets;
	int count  = 0;
	while (widgets[count]) {
		count++;
	}
	w->widgets_count = count - 1;
}

void ui_window_close(UIWindow* w) {
	puglFreeView(w->view);
}

void ui_window_draw(UIWindow* w, UIContext* c) {
	w->draw_begin(w, c);
	ui_window_draw_widgets(w, c);
	w->draw_end(w, c);
}

void ui_window_draw_begin(UIWindow* w, UIContext* c) {
	cairo_scale(c, w->scale, w->scale);
}

void ui_window_draw_end(UIWindow* w, UIContext* c) {
	return;
	// if (w->focused_widget) {
	// 	UIWidget* widget = w->focused_widget;
	// 	ui_draw_rectangle(c, &(UIRectangleProperties){
	// 		.position = widget->position,
	// 		.size     = widget->size,
	// 		.stroke   = {.color = {1, 0, 0, 1}, .width = 2}
	// 	});
	// }
}

void ui_window_draw_widgets(UIWindow* w, UIContext* c) {
	for (int i = 0; i < w->widgets_count; i++) {
		UIWidget* widget = w->widgets[i];
		ui_widget_draw(widget, c);
	}
}

void ui_window_mouse_down(UIWindow* w, UIMouseButtons b) {
	if (w->hovered_widget) {
		flag_on(w->state, WINDOW_STATE_GRABBED);
		ui_widget_mouse_down(w->hovered_widget, b);
	}
}

static void ui_window_find_hovered_widget(UIWidget** focused, UIWidget* widget, UIPosition* p) {
	if (has_flag(widget->state, WIDGET_STATE_HIDDEN)) {
		return;
	}
	if (ui_widget_intersect(widget, p->x, p->y)) {
		*focused = widget;
	}
	if (widget->children_count > 0) {
		for (int i = 0; i < widget->children_count; i++) {
			UIWidget* w = widget->children[i];
			ui_window_find_hovered_widget(focused, w, p);
		}
	}
}

void ui_window_mouse_move(UIWindow* w, UIPosition screen, UIPosition client) {
	const int client_x = client.x / w->scale;
	const int client_y = client.y / w->scale;
	UIPosition position = {client_x, client_y};

	if (is_flag(w->state, WINDOW_STATE_HOVERED)) {
		UIWidget* focused = NULL;
		for (int i = 0; i < w->widgets_count; i++) {
			UIWidget* widget = w->widgets[i];
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
		ui_widget_mouse_move(w->hovered_widget, screen, client);
	}
}

void ui_window_mouse_up(UIWindow* w, UIMouseButtons b) {
	if (w->hovered_widget) {
		flag_off(w->state, WINDOW_STATE_GRABBED);
		ui_widget_mouse_up(w->hovered_widget, b);
	}
}

void ui_window_on_close(UIWindow* w) {
	ui_window_close(w);
}

void ui_window_show(UIWindow* w) {
	puglShow(w->view, PUGL_SHOW_RAISE);
}

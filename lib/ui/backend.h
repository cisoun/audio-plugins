#ifndef UI_BACKEND_H
#define UI_BACKEND_H

#include "../kit.h"
#include "../kit-audio.h"
#include "cairo/cairo.h"
#include "pugl/cairo.h"
#include "pugl/pugl.h"
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60

#define UIContext cairo_t
#define UISurface cairo_surface_t

#define UIApp(...) ui_app(&(UIApp)__VA_ARGS__)

#define clamp(a, b, c) min(max(a, b), c)
#define deg2rad(d) (d) * (M_PI / 180)
#define flag_off(s, f) s &= ~f
#define flag_on(s, f) s |= f
#define has_flag(s, f) (s & f)
#define is_flag(s, f) (s == f)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define set_default(a, b) if ((a) == 0) (a) = (b)
#define set_default_float(a, b) if ((a) <= 0) (a) = (b)
#define ui_color_to_cairo(c) (c).r, (c).g, (c).b, (c).a

static const float  UI_DEFAULT_FONT_SIZE = 12;
static const double UI_DOUBLE_CLICK_TIME = 0.5; // Seconds.

typedef enum {
	DIRECTION_DOWN  = PUGL_SCROLL_DOWN,
	DIRECTION_LEFT  = PUGL_SCROLL_LEFT,
	DIRECTION_RIGHT = PUGL_SCROLL_RIGHT,
	DIRECTION_UP    = PUGL_SCROLL_UP,
	DIRECTION_ALL   = PUGL_SCROLL_SMOOTH
} UIDirections;

typedef enum {
	MOUSE_BUTTON_LEFT   = 0,
	MOUSE_BUTTON_RIGHT  = 1,
	MOUSE_BUTTON_MIDDLE = 2
} UIMouseButtons;

typedef enum {
	ORIGIN_NW, ORIGIN_N, ORIGIN_NE,
	ORIGIN_W,  ORIGIN_M, ORIGIN_E,
	ORIGIN_SW, ORIGIN_S, ORIGIN_SE
} UIOrigin;

typedef enum {
	WIDGET_STATE_IDLE    = 0,
	WIDGET_STATE_HOVERED  = 1 << 0,
	WIDGET_STATE_CLICKED  = 1 << 1,
	WIDGET_STATE_DISABLED = 1 << 2,
	WIDGET_STATE_HIDDEN   = 1 << 3
} UIWidgetStates;

typedef enum {
	WIDGET_BUTTON,
	WIDGET_FILE_DIALOG,
	WIDGET_FILE_LIST,
	WIDGET_LIST,
	WIDGET_KNOB,
	WIDGET_TEXT,
	WIDGET_WAVEFORM,
	WIDGET_WINDOW
} UIWidgetTypes;

typedef struct UIWindow UIWindow;
typedef struct UIWidget UIWidget;

typedef struct {
	char*      name;
	int        quit;
	PuglWorld* world;
} UIApp;

typedef struct {
	float start;
	float end;
} UIAngle;

typedef struct {
	unsigned short x1;
	unsigned short y1;
	unsigned short x2;
	unsigned short y2;
} UIArea;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} UIColor;

typedef struct {
	int x;
	int y;
} UIPosition;

typedef struct {
	unsigned short width;
	unsigned short height;
} UISize;

typedef struct {
	UIColor color;
	float   width;
} UIStroke;

typedef struct {
	UIAngle    angle;
	UIPosition position;
	float      radius;
	UIStroke   stroke;
} UIArcProperties;

typedef struct {
	UIColor    color;
	UIPosition position;
	float      radius;
	UIStroke   stroke;
} UICircleProperties;

typedef struct {
	int          positions_count;
	UIPosition** positions;
	UIStroke     stroke;
} UILinesProperties;

typedef struct {
	char*      text;
	UIColor    color;
	UIPosition position;
	float      size;
	bool       bold;
	bool       italic;
	UIOrigin   origin;
} UITextProperties;

typedef struct {
	UIColor    color;
	UIPosition position;
	UISize     size;
	UIStroke   stroke;
} UIRectangleProperties;

typedef struct {
	UIColor    color;
	UIPosition position;
	float      radius;
	UISize     size;
	UIStroke   stroke;
} UIRoundedRectangleProperties;

typedef struct {
	int        count;
	UIWidget** items;
} UIWidgetsList;

#define WIDGET \
	UIWidget**     children; \
	int            children_count; \
	UIWidget*      parent; \
	UIPosition     position; \
	UISize         size; \
	UIWidgetStates state; \
	UISurface*     surface; \
	UIWidgetTypes  type; \
	void           (*draw)         (UIWidget*, UIContext*); \
	void           (*draw_area)    (UIWidget*, UIContext*, UIArea*); \
	void           (*click)        (UIWidget*); \
	void           (*double_click) (UIWidget*); \
	void           (*mouse_down)   (UIWidget*, UIPosition, UIMouseButtons); \
	void           (*mouse_move)   (UIWidget*, UIPosition); \
	void           (*mouse_up)     (UIWidget*, UIPosition, UIMouseButtons); \
	void           (*scroll)       (UIWidget*, UIDirections, float, float); \
	void           (*state_change) (UIWidget*, UIWidgetStates);

struct UIWidget {
	WIDGET
};

struct UIWindow {
	WIDGET
	UIApp*         app;
	UIWidget*      hovered_widget;
	bool           resizable;
	float          scale;
	char*          title;
	PuglView*      view;
	UIArea         dirty_area;
	bool           is_dirty;
	void           (*draw_begin)     (UIWindow*, UIContext*);
	void           (*draw_end)       (UIWindow*, UIContext*);
	void           (*on_close)       (UIWindow*);
	void           (*on_key_down)    (UIWindow*, int);
	void           (*on_mouse_enter) (UIWindow*);
	void           (*on_mouse_leave) (UIWindow*);
	void           (*on_mouse_move)  (UIWindow*, UIPosition);
};

UIApp*     ui_app                    (UIApp*);
void       ui_app_destroy            (UIApp*);
void       ui_app_close              (UIApp*);
void       ui_app_run                (UIApp*);

UIArea     ui_area_add               (UIArea*, UIArea*);

void       ui_draw_arc               (UIContext*, UIArcProperties*);
void       ui_draw_circle            (UIContext*, UICircleProperties*);
void       ui_draw_lines             (UIContext*, UILinesProperties*);
void       ui_draw_rectangle         (UIContext*, UIRectangleProperties*);
void       ui_draw_rounded_rectangle (UIContext*, UIRoundedRectangleProperties*);
void       ui_draw_text              (UIContext*, UITextProperties*);

UISurface* ui_surface                (UISize*);
UIContext* ui_surface_draw_begin     (UISurface*);
void       ui_surface_draw_end       (UISurface*, UIContext*);
void       ui_surface_destroy        (UISurface*);

void       ui_widget_disable         (UIWidget*);
void       ui_widget_double_click    (UIWidget*);
void       ui_widget_draw            (UIWidget*, UIContext*);
void       ui_widget_draw_area       (UIWidget*, UIContext*, UIArea*);
void       ui_widget_enable          (UIWidget*);
UIArea     ui_widget_get_area        (UIWidget*);
UIWindow*  ui_widget_get_window      (UIWidget*);
void       ui_widget_mouse_down      (UIWidget*, UIPosition, UIMouseButtons);
void       ui_widget_mouse_enter     (UIWidget*);
void       ui_widget_mouse_leave     (UIWidget*);
void       ui_widget_mouse_move      (UIWidget*, UIPosition);
void       ui_widget_mouse_up        (UIWidget*, UIPosition, UIMouseButtons);
void       ui_widget_must_redraw     (UIWidget*);
void       ui_widget_scroll          (UIWidget*, UIDirections, float, float);
void       ui_widget_set_state       (UIWidget*, UIWidgetStates);
void       ui_widget_set_surface     (UIWidget*, UISurface*);
void       ui_widget_unset_state     (UIWidget*, UIWidgetStates);

UIWindow*  ui_window                 (UIWindow*, UIApp*);
void       ui_window_attach          (UIWindow*, UIWidget**);
void       ui_window_close           (UIWindow*);
void       ui_window_draw            (UIWidget*, UIContext*);
void       ui_window_draw_area       (UIWidget*, UIContext*, UIArea*);
void       ui_window_draw_begin      (UIWindow*, UIContext*);
void       ui_window_draw_end        (UIWindow*, UIContext*);
void       ui_window_mouse_down      (UIWindow*, UIPosition, UIMouseButtons);
void       ui_window_mouse_move      (UIWindow*, UIPosition);
void       ui_window_mouse_up        (UIWindow*, UIPosition, UIMouseButtons, double);
void       ui_window_must_redraw     (UIWindow*, UIArea*);
void       ui_window_on_close        (UIWindow*);
void       ui_window_on_key_down     (UIWindow*);
void       ui_window_on_mouse_enter  (UIWindow*);
void       ui_window_on_mouse_leave  (UIWindow*);
void       ui_window_show            (UIWindow*);

#endif

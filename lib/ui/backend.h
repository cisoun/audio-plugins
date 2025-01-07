#ifndef UI_BACKEND_H
#define UI_BACKEND_H

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kit.h"
#include "cairo/cairo.h"
#include "pugl/pugl.h"
#include "pugl/cairo.h"

#define UIContext cairo_t
#define UIApp(...) ui_app(&(UIApp)__VA_ARGS__)
#define UIKnob(...) ui_knob(&(UIKnob)__VA_ARGS__)

#define deg2rad(d) (d) * (M_PI / 180)
#define flag_off(s, f) s &= ~f
#define flag_on(s, f) s |= f
#define has_flag(s, f) (s & f)
#define is_flag(s, f) (s == f)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define set_default(a, b) (a = a ? a : b)
#define ui_color_to_cairo(c) (c).r, (c).g, (c).b, (c).a

static const float _UI_KNOB_ANGLE_MIN   = deg2rad(140);
static const float _UI_KNOB_ANGLE_MAX   = deg2rad(400);
static const float _UI_KNOB_ANGLE_RANGE = _UI_KNOB_ANGLE_MAX - _UI_KNOB_ANGLE_MIN;

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
	WIDGET_STATE_HOVERED = 1 << 0,
	WIDGET_STATE_CLICKED = 1 << 1,
	WIDGET_STATE_HIDDEN  = 1 << 2,
} UIWidgetStates;

typedef enum {
	WIDGET_BUTTON,
	WIDGET_FILE_DIALOG,
	WIDGET_LIST,
	WIDGET_KNOB,
	WIDGET_TEXT
} UIWidgetTypes;

typedef enum {
	WINDOW_STATE_IDLE    = 0,
	WINDOW_STATE_HOVERED = 1 << 0,
	WINDOW_STATE_GRABBED = 1 << 1
} UIWindowStates;

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
	float width;
	float height;
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
	double     size;
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
	UIWidgetTypes  type; \
	void           (*draw)       (UIWidget*, UIContext*); \
	void           (*click)      (UIWidget*); \
	void           (*mouse_down) (UIWidget*, UIMouseButtons); \
	void           (*mouse_move) (UIWidget*, UIPosition, UIPosition); \
	void           (*mouse_up)   (UIWidget*, UIMouseButtons); \
	void           (*scroll)     (UIWidget*, UIDirections, float, float);

struct UIWidget {
	WIDGET
};

struct UIWindow {
	UIApp*         app;
	UIWidget*      hovered_widget;
	bool           resizable;
	float          scale;
	UISize         size;
	UIWindowStates state;
	char*          title;
	PuglView*      view;
	UIWidget**     widgets;
	int            widgets_count;
	void           (*draw)           (UIWindow*, UIContext*);
	void           (*draw_begin)     (UIWindow*, UIContext*); \
	void           (*draw_end)       (UIWindow*, UIContext*); \
	void           (*on_close)       (UIWindow*);
	void           (*on_key_down)    (UIWindow*, int);
	void           (*on_mouse_enter) (UIWindow*);
	void           (*on_mouse_leave) (UIWindow*);
	void           (*on_mouse_move)  (UIWindow*, UIPosition, UIPosition);
};

UIApp*    ui_app                    (UIApp*);
void      ui_app_destroy            (UIApp*);
void      ui_app_close              (UIApp*);
void      ui_app_run                (UIApp*);
void      ui_draw_arc               (UIContext*, UIArcProperties*);
void      ui_draw_circle            (UIContext*, UICircleProperties*);
void      ui_draw_lines             (UIContext*, UILinesProperties*);
void      ui_draw_rectangle         (UIContext*, UIRectangleProperties*);
void      ui_draw_rounded_rectangle (UIContext*, UIRoundedRectangleProperties*);
void      ui_draw_text              (UIContext*, UITextProperties*);
void      ui_widget_draw            (UIWidget*, UIContext*);
void      ui_widget_mouse_enter     (UIWidget*);
void      ui_widget_mouse_leave     (UIWidget*);
void      ui_widget_mouse_down      (UIWidget*, UIMouseButtons);
void      ui_widget_mouse_move      (UIWidget*, UIPosition, UIPosition);
void      ui_widget_mouse_up        (UIWidget*, UIMouseButtons);
void      ui_widget_on_scroll       (UIWidget*, UIDirections, float, float);
UIWindow* ui_window                 (UIWindow*, UIApp*);
void      ui_window_attach          (UIWindow*, UIWidget**);
void      ui_window_close           (UIWindow*);
void      ui_window_draw_begin      (UIWindow*, UIContext*);
void      ui_window_draw            (UIWindow*, UIContext*);
void      ui_window_draw_end        (UIWindow*, UIContext*);
void      ui_window_draw_widgets    (UIWindow*, UIContext*);
void      ui_window_on_close        (UIWindow*);
void      ui_window_on_key_down     (UIWindow*);
void      ui_window_mouse_down      (UIWindow*, UIMouseButtons);
void      ui_window_on_mouse_enter  (UIWindow*);
void      ui_window_on_mouse_leave  (UIWindow*);
void      ui_window_mouse_move      (UIWindow*, UIPosition, UIPosition);
void      ui_window_mouse_up        (UIWindow*, UIMouseButtons);
void      ui_window_show            (UIWindow*);

#endif

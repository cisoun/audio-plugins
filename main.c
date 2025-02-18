#include <stdio.h>
#include <math.h>
#include "lib/kit-audio.h"
#include "lib/kit.h"
#include "lib/ui/backend.h"
#include "lib/ui/colors.h"
#include "lib/ui/file-dialog.h"
#include "lib/ui/waveform.h"
#include "lib/ui/widgets.h"
#include "ui/ui.h"
#include <fftw3.h>

UIColor* COLOR_DARK       = COLOR_NEUTRAL_GREY;
UIColor* COLOR_PRIMARY    = COLOR_RED;
UIColor* COLOR_TEXT       = &COLOR_NEUTRAL_GREY[7];
UIColor* COLOR_TEXT_LIGHT = &COLOR_NEUTRAL_GREY[4];

KitFileInfo* file;
UIApp*       app;
UIWidget*    knob_input;
UIWidget*    knob_dry_wet;
UIWidget*    knob_output;
UIWidget*    text_input;
UIWidget*    text_dry_wet;
UIWidget*    text_file;
UIWidget*    text_output;
UIWidget*    text_title;
UIWidget*    button;
UIWidget*    waveform;
UIWidget*    dialog;
UIWindow*    window;
KitAudio*    audio;

void (*knob_mouse_move) (UIWidget*, UIPosition);

static void on_button_click(UIWidget* w) {
	ui_file_dialog_show(dialog);
}

static void on_close(UIWindow* w) {
	ui_app_close(w->app);
}

static void on_dialog_close(UIWidget* w, KitFileInfo* kfi) {
	if (kfi != NULL) {
		if (file != NULL) {
			kit_file_info_destroy(file);
		}
		if (audio != NULL) {
			kit_audio_destroy(audio);
		}
		file           = kit_file_info(*kfi);
		UIText* tf     = (UIText*)text_file;
		tf->text       = file->name;
		char* path     = kit_string_join3(kfi->path, PATH_SEPARATOR, kfi->name);
		audio          = kit_audio_from(path);
		ui_waveform_set_audio(waveform, audio);
		destroy(path);
	}
}

static void on_key_down(UIWindow* w, int code) {
	// Exit on 'q' or escape.
	if (code == 12 || code == 53) {
		ui_app_close(w->app);
	}
}

static void on_mouse_move(UIWindow* w, UIPosition client) {
	if (w->state & WIDGET_STATE_HOVERED) {
		printf("CURSOR: %d : %d\n", client.x, client.y);
	}
}

static void on_window_draw(UIWidget* widget, UIContext* c) {
	UIWindow* w = (UIWindow*)widget;
	ui_draw_rectangle(c, &(UIRectangleProperties){
		.color    = COLOR_DARK[1],
		.size     = w->size,
	});
	ui_draw_rounded_rectangle(c, &(UIRoundedRectangleProperties){
		.color    = COLOR_DARK[2],
		.position = {10, 40},
		.radius   = 6,
		.size     = {w->size.width - 20, 45},
	});
	ui_draw_rounded_rectangle(c, &(UIRoundedRectangleProperties){
		.color    = COLOR_DARK[0],
		.position = {10, 40 + 45 + 10},
		.radius   = 6,
		.size     = {w->size.width - 20, 95},
		// .stroke   = {
		// 	.width = 1,
		// 	.color = COLOR_DARK[3]
		// }
	});
	ui_draw_rounded_rectangle(c, &(UIRoundedRectangleProperties){
		.color    = COLOR_DARK[2],
		.position = {10, w->size.height - 10 - 90},
		.radius   = 6,
		.size     = {w->size.width - 20, 90},
	});
}

int main(int argc, char** argv) {
	app = ui_app(&(UIApp){
		.name = "Test"
	});

	knob_input = ui_knob(&(UIKnob){
		.color    = &COLOR_ROSE[5],
		.position = {20, 210},
		.size     = {60, 60},
		.value    = 0.5
	});

	knob_dry_wet = ui_knob(&(UIKnob){
		.color    = &COLOR_RED[5],
		.position = {20 + 60 + 10, 210},
		.size     = {60, 60},
		.value    = 0.5
	});

	knob_output = ui_knob(&(UIKnob){
		.color    = &COLOR_ORANGE[5],
		.position = {20 + 60 + 10 + 60 + 10, 210},
		.size     = {60, 60},
		.value    = 0.5
	});

	text_input = ui_text(&(UIText){
		.color     = &COLOR_DARK[5],
		.font_size = 12,
		.origin    = ORIGIN_M,
		.position  = {20 + 30, 270},
		.text      = "input"
	});

	text_dry_wet = ui_text(&(UIText){
		.color     = &COLOR_DARK[5],
		.font_size = 12,
		.origin    = ORIGIN_M,
		.position  = {20 + 30 + 10 + 60, 270},
		.text      = "dry / wet"
	});

	text_output = ui_text(&(UIText){
		.color     = &COLOR_DARK[5],
		.font_size = 12,
		.origin    = ORIGIN_M,
		.position  = {20 + 60 + 10 + 60 + 10 + 30, 270},
		.text      = "output"
	});

	button = ui_button(&(UIButton){
		.click    = on_button_click,
		.position = {20, 50},
		.size     = {90, 25},
		.text     = "Open IR"
	});

	text_file = ui_text(&(UIText){
		.color     = &COLOR_DARK[5],
		.origin    = ORIGIN_W,
		.position  = {20 + 110, 63},
		.text      = "No file selected"
	});

	text_title = ui_text(&(UIText){
		.bold      = true,
		.color     = &COLOR_DARK[5],
		.font_size = 16,
		.position  = {10, 15},
		.text      = "Impulse me daddy LV2 test"
	});

	waveform = ui_waveform(&(UIWaveform){
		.position = {11, 96},
		.size     = {478, 93}
	});

	dialog = ui_file_dialog((UIFileDialog){
		.close   = on_dialog_close,
		.filters = (char*[5]){".wav", ".flac", ".ogg"},
		.size    = {500, 300}
	});


	window = ui_window(&(UIWindow){
		.draw          = on_window_draw,
		.on_close      = on_close,
		.on_key_down   = on_key_down,
		.on_mouse_move = on_mouse_move,
		.scale         = 2,
		.size          = {500, 300},
		.title         = "Test LV2",
		.children      = (UIWidget*[]){
			button,
			knob_dry_wet,
			knob_input,
			knob_output,
			text_dry_wet,
			text_file,
			text_input,
			text_output,
			text_title,
			waveform,
			dialog
		},
		.children_count = 11
	}, app);

	ui_window_show         (window);

	ui_app_run             (app);

	ui_window_close        (window);

	destroy(window);

	if (file != NULL) {
		kit_file_info_destroy(file);
	}

	if (audio != NULL) {
		kit_audio_destroy(audio);
	}

	ui_file_dialog_destroy (dialog);
	ui_waveform_destroy    ((UIWaveform*)waveform);
	ui_app_destroy         (app);

	return 0;
}

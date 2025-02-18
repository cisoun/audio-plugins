#include "backend.h"
#include "colors.h"
#include "kit-thread.h"
#include "waveform.h"
#include <math.h>

static void* ui_waveform_generate_waveform(void*);

UIWidget* ui_waveform(UIWaveform* wf) {
	UIWaveform* waveform = new(UIWaveform);
	memcpy(waveform, wf, sizeof(UIWaveform));
	waveform->draw = ui_waveform_draw;
	waveform->type = WIDGET_WAVEFORM;
	return (UIWidget*)waveform;
}

void ui_waveform_destroy(UIWaveform* wf) {
	ui_surface_destroy(wf->surface);
	if (wf != NULL) {
		destroy(wf);
	}
}

void ui_waveform_draw(UIWidget* w, UIContext* c) {
	if (w->surface != NULL) {
		UIWindow* window = ui_widget_get_window(w);
		float     scale  = window->scale;
		cairo_save(c);
		cairo_scale(c, 1 / scale, 1 / scale);
		cairo_set_source_surface(c, w->surface, w->position.x * scale, w->position.y * scale);
		cairo_paint(c);
		cairo_restore(c);
	}
}

static void* ui_waveform_generate_waveform(void* args) {
	UIWaveform* wf = (UIWaveform*)args;

	if (wf->audio == NULL) {
		return NULL;
	}

	UIWidget*  widget         = (UIWidget*)wf;
	KitAudio*  audio          = wf->audio;
	int        channels       = wf->audio->channels;
	float      channel_height = wf->size.height / channels;

	UIWindow*  window         = ui_widget_get_window(widget);
	float      scale          = window->scale;

	UISurface* surface = ui_surface(&(UISize){
		wf->size.width * scale,
		wf->size.height * scale
	});
	UIContext* context = ui_surface_draw_begin(surface);

	cairo_scale(context, scale, scale);

	for (int i = 0; i < channels; i++) {
		int x = 0;
		int y = i * channel_height + channel_height / 2;
		cairo_new_path(context);
		cairo_move_to(context, x, y);
		for (long int j = 0; j < audio->samples; j++) {
			float value = kit_audio_get_sample(audio, i, j);
			cairo_line_to(context,
				x + (float)wf->size.width / audio->samples * j,
				y - channel_height / 2 * value
			);
		}
		cairo_set_line_width(context, 1);
		cairo_set_source_rgba(context, ui_color_to_cairo(COLOR_DARK[7]));
		cairo_stroke(context);
	}

	ui_surface_draw_end(surface, context);

	ui_widget_set_surface(widget, surface);
	ui_widget_must_redraw(widget);

	return NULL;
}

void ui_waveform_set_audio(UIWidget* w, KitAudio* a) {
	UIWaveform* wf = (UIWaveform*)w;
	wf->audio      = a;
	KitThread t = kit_thread(ui_waveform_generate_waveform, (void*)wf);
	kit_thread_detach(t);
	ui_widget_must_redraw(w);
}

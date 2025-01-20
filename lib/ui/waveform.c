#include "colors.h"
#include "waveform.h"
#include <math.h>

UIWidget* ui_waveform(UIWaveform* wf) {
	wf->draw = ui_waveform_draw;
	wf->type = WIDGET_WAVEFORM;
	return (UIWidget*)wf;
}

void ui_waveform_draw(UIWidget* w, UIContext* c) {
	UIWaveform* wf     = (UIWaveform*)w;
	if (wf->audio == NULL) {
		return;
	}
	int delta            = wf->audio->samples / w->size.width;
	int channels         = wf->audio->channels;
	float channel_height = w->size.height / channels;
	for (int i = 0; i < channels; i++) {
		int x = w->position.x;
		int y = w->position.y + i * channel_height + channel_height / 2;
		cairo_new_path(c);
		cairo_move_to(c, x, y);
		for (long int j = 0; j < w->size.width; j++) {
			float value = kit_audio_get_sample(wf->audio, i, j * delta);
			cairo_line_to(c,
				x + j,
				y - channel_height / 2 * value
			);
		}
		cairo_set_line_width(c, 1);
		cairo_set_source_rgba(c, ui_color_to_cairo(COLOR_DARK[9]));
		cairo_stroke(c);
	}
}

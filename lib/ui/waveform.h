#ifndef UI_WAVEFORM_H
#define UI_WAVEFORM_H

#include "backend.h"
#include "colors.h"
#include "kit-audio.h"
#include "kit-thread.h"
#include <math.h>

typedef struct{
	WIDGET
	KitAudio* audio;
} UIWaveform;

UIWidget* ui_waveform           (UIWaveform*);
void      ui_waveform_destroy   (UIWaveform*);
void      ui_waveform_draw      (UIWidget*, UIContext*);
void      ui_waveform_set_audio (UIWidget*, KitAudio*);


#endif

#ifndef UI_WAVEFORM_H
#define UI_WAVEFORM_H

#include "backend.h"

typedef struct{
	WIDGET
	KitAudio* audio;
} UIWaveform;

UIWidget* ui_waveform      (UIWaveform*);
void      ui_waveform_draw (UIWidget*, UIContext* c);

#endif

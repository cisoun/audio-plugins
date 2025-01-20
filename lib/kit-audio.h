#ifndef KIT_AUDIO_H
#define KIT_AUDIO_H

#include "kit.h"
#include <sndfile.h>
#include <stdio.h>

typedef enum {
	KIT_AUDIO_FORMAT_FLAC,
	KIT_AUDIO_FORMAT_OGG,
	KIT_AUDIO_FORMAT_WAVE,
	KIT_AUDIO_FORMAT_AIFF,
	KIT_AUDIO_FORMAT_MP3
} KitAudioFormats;

typedef struct {
	double**          data;
	unsigned int      channels;
	unsigned long int sample_rate;
	unsigned long int samples;
} KitAudio;

KitAudio* kit_audio_from       (const char* path);
void      kit_audio_destroy    (KitAudio* a);
double    kit_audio_get_sample (const KitAudio* a, const int, const long int);


#endif

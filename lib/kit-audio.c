#include "kit-audio.h"
#include "kit.h"
#include <assert.h>

KitAudio* kit_audio_from(const char* path) {
	SF_INFO info;
	SNDFILE* file  = sf_open(path, SFM_READ, &info);
	KitAudio* a    = new(KitAudio);
	a->channels    = info.channels;
	a->sample_rate = info.samplerate;
	a->samples     = info.frames;
	a->data = alloc(double*, a->channels);
	for (int i = 0; i < a->channels; i++) {
		a->data[i] = alloc(double, a->samples);
	}
	double* data = alloc(double, a->channels * a->samples);
	sf_readf_double(file, data, a->samples);
	for (int i = 0; i < a->samples; i++) {
		for (int c = 0; c < a->channels; c++) {
			 a->data[c][i] = data[i * a->channels + c];
		}
	}
	destroy(data);
	printf("INFO: channels:   %d\n",  info.channels);
	printf("INFO: samplerate: %d\n",  info.samplerate);
	printf("INFO: frames:     %ld\n", info.frames);
	sf_close(file);
	return a;
}

inline double kit_audio_get_sample(const KitAudio* a, const int channel, const long int x) {
	assert(x < a->samples);
	return a->data[channel][x];
}

void kit_audio_destroy(KitAudio* a) {
	for (int i = 0; i < a->channels; i++) {
		destroy(a->data[i]);
	}
	destroy(a->data);
	destroy(a);
}

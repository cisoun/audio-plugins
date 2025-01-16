#include "kit.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int allocs = 0;

KitArray* kit_array(void) {
	KitArray* a = new(KitArray);
	assert(a != NULL);
	a->items = NULL;
	a->count = 0;
	return a;
}

void kit_array_add(KitArray* a, void* i) {
	assert(a != NULL);
	a->items = realloc(a->items, sizeof(void*) * (a->count + 1));
	a->items[a->count] = i;
	a->count++;
}

void kit_array_clear(KitArray* a) {
	for (int i = 0; i < a->count; i++) {
		destroy(a->items[i]);
	}
	a->count = 0;
}

void kit_array_destroy(KitArray* a) {
	destroy(a);
}

void kit_array_remove_index(KitArray* a, int index) {
	for (int i = index; i < a->count - 1; i++) {
		a[i] = a[i + 1];
	}
	a->items = realloc(a->items, sizeof(void*) * (a->count - 1));
}

KitFileInfo* kit_file_info(KitFileInfo i) {
	KitFileInfo* kfi = new(KitFileInfo);
	kfi->name = kit_string_clone(i.name);
	kfi->path = kit_string_clone(i.path);
	kfi->type = i.type;
	return kfi;
}

void kit_file_info_destroy(KitFileInfo* i) {
	destroy(i->name);
	destroy(i->path);
	destroy(i);
}

static int kit_file_info_array_compare(const void* a, const void* b) {
	const KitFileInfo *c = *(KitFileInfo**)a;
	const KitFileInfo *d = *(KitFileInfo**)b;
	// Folders first.
	if (c->type != d->type) {
		return d->type - c->type;
	}
	// Order by name.
	return strcmp(c->name, d->name);
}

void kit_file_info_array_clear(KitArray* a) {
	for (int i = 0; i < a->count; i++) {
		KitFileInfo* k = a->items[i];
		kit_file_info_destroy(k);
	}
	destroy(a->items);
	a->count = 0;
}

void kit_file_info_array_destroy(KitArray* a) {
	if (a->items) {
		kit_file_info_array_clear(a);
	}
	destroy(a);
}

char* kit_path_parent(char* path) {
	return strrchr(path, PATH_SEPARATOR[0]);
}

KitArray* kit_path_scan(char* path) {
	struct dirent* entry = NULL;
	DIR* reader          = opendir(path);
	if (reader) {
		KitArray* a = kit_array();
		(void)readdir(reader); // Pass first entry: ".".
		while ((entry = readdir(reader)) != NULL) {
			KitFileInfo* kfi = kit_file_info((KitFileInfo){
				.name = entry->d_name,
				.path = path,
				.type = entry->d_type == DT_DIR ? KIT_FILE_TYPE_FOLDER : KIT_FILE_TYPE_FILE,
			});
			kit_array_add(a, kfi);
		}
		closedir(reader);
		qsort(a->items, a->count, sizeof(void*), kit_file_info_array_compare);
		return a;
	}
	return NULL;
}

int kit_string_ends_with(const char* text, const char* suffix) {
	if (!text || !suffix) {
		return 0;
	}
	size_t text_length   = strlen(text);
	size_t suffix_length = strlen(suffix);
	if (suffix_length > text_length) {
		return 0;
	}
	return strncmp(text + text_length - suffix_length, suffix, suffix_length) == 0;
}

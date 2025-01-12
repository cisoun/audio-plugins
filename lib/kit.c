#include "kit.h"
#include <stdio.h>
#include <string.h>

void kit_folder_scan(const char* path, KitFilesList* entries) {
	struct dirent* entry = NULL;
	DIR* reader          = NULL;
	reader = opendir(path);
	if (reader) {
		entries->items = (char**)malloc(0);
		entries->count = 0;
		while ((entry = readdir(reader)) != NULL) {
			entries->items = (char**)realloc(
				entries->items,
				sizeof(char*) * (entries->count + 1)
			);
			char* item = (char*)malloc(sizeof(char)*strlen(entry->d_name) + 1);
			memmove(item, entry->d_name, strlen(entry->d_name));
			entries->items[entries->count] = item;
			entries->count++;
		}
	}
	closedir(reader);
}

int kit_text_ends_with(const char* text, const char* suffix) {
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

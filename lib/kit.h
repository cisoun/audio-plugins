#ifndef INTERNAL_H
#define INTERNAL_H

#include <dirent.h>
#include <stdlib.h>

#define destroy(o) free(o); o = NULL
#define new(t) (t*)malloc(sizeof(t))

#define kit_list_t(t) struct { int count; t** items; }

#ifdef _WIN32
	#define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

typedef enum {
	KIT_FILE_TYPE_FILE,
	KIT_FILE_TYPE_FOLDER
} KitFileTypes;

typedef struct {
	char*        name;
	char*        path;
	KitFileTypes type;
} KitFileInfo;

typedef kit_list_t(char) KitFilesList;

#define kit_list(t) (t*)malloc(sizeof(t*))
#define kit_list_realloc(t, l, c) realloc(l, sizeof(t*) * l->*)

void kit_folder_scan    (const char* path, KitFilesList* items);
int  kit_text_ends_with (const char* text, const char* suffix);

#endif

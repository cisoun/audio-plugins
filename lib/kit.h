/*
 * KIT toolkit.
 *
 * Notes:
 *  - Use `calloc` rather than `malloc` to initialize the memory to 0.
 *    This improves safety.
 *  - LOGALLOC/LOGFREE to be removed. Useful to trace memory allocations.
 *    Upon termination, you should see "FREE: 0" in the output. Otherwise, that
 *    means a memory leak is occuring somewhere.
 */

#ifndef INTERNAL_H
#define INTERNAL_H

#include <dirent.h>
#include <stdlib.h>

int allocs;
#define LOGALLOC printf("MALLOC: %d %s:%d\n", ++allocs, __FILE__, __LINE__)
#define LOGFREE printf("FREE: %d %s:%d\n", --allocs, __FILE__, __LINE__)

#define new(t) calloc((LOGALLOC ? 0 : 0) + 1, sizeof(t))
#define alloc(t, n) calloc((LOGALLOC ? 0 : 0) + n, sizeof(t))
#define destroy(o) LOGFREE; if (o != NULL) free(o); o = NULL

#define kit_string_clone(a) strdup(a); LOGALLOC

#ifdef _WIN32
	#define PATH_SEPARATOR "\"
#else
    #define PATH_SEPARATOR "/"
#endif

typedef enum {
	KIT_FILE_TYPE_FILE,
	KIT_FILE_TYPE_FOLDER
} KitFileTypes;

typedef struct {
	int    count;
	void** items;
} KitArray;

typedef struct {
	char*        name;
	char*        path;
	KitFileTypes type;
} KitFileInfo;

#define kit_array_for_each(a, i) for (void** i = a->items; *i; ++i)

//void         destroy               (void*);

KitArray*    kit_array                   (void);
void         kit_array_add               (KitArray*, void*);
void         kit_array_clear             (KitArray*);
void         kit_array_destroy           (KitArray*);
void         kit_array_remove_index      (KitArray*, int);

KitArray*    kit_path_scan               (char*);
int          kit_string_ends_with        (const char*, const char*);

KitFileInfo* kit_file_info               (KitFileInfo i);
void         kit_file_info_destroy       (KitFileInfo* i);

void         kit_file_info_array_clear   (KitArray*);
void         kit_file_info_array_destroy (KitArray*);


#endif

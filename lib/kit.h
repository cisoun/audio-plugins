#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdlib.h>

#define destroy(o) free(o); o = NULL
#define new(t) (t*)malloc(sizeof(t))

#define kit_list(t) struct { int count; t items[]; }

#endif

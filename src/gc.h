#ifndef HYDRO_GC_H
#define HYDRO_GC_H

#include "value.h"

typedef struct GC {
  Obj *objects; // Intrusive linked list of objects allocated on the heap.
} GC;

void initGC(GC *gc);
void freeGC(GC *gc);

void gcAddObject(GC *gc, Obj *object);

#endif

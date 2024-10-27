#include "gc.h"
#include "memory.h"
#include "object.h"

static void freeObject(Obj *object) {
  switch (object->type) {
  case OBJ_STRING: {
    ObjString *string = (ObjString *)object;
    FREE_ARRAY(char, string->chars, string->length);
    FREE(ObjString, object);
    break;
  }
  }
}

// Traverses the intrusive linked list of the VM's objects and frees them.
static void freeObjects(Obj *objects) {
  Obj *o = objects;
  while (o != NULL) {
    Obj *next = o->next;
    freeObject(o);
    o = next;
  }
}

void initGC(GC *gc) { gc->objects = NULL; }

void freeGC(GC *gc) {
  freeObjects(gc->objects);
  initGC(gc);
}

// Inserts at the head of the GC's intrusive linked list of objects.
void gcAddObject(GC *gc, Obj *object) {
  object->next = gc->objects;
  gc->objects = object;
}

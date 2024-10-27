#ifndef HYDRO_TABLE_H
#define HYDRO_TABLE_H

#include <stdint.h>

#include "value.h"

typedef struct Entry {
  ObjString *key;
  Value value;
} Entry;

typedef struct Table {
  int count; // Number of entries plus tombstones.
  int capacity;
  Entry *entries; // The array of entries to the hash table.
} Table;

void initTable(Table *table);
void freeTable(Table *table);

/*
 * If an entry exists in the table for the key, returns true and sets the value
 * output parameter pointing to the resulting value. Otherwise, returns false.
 */
bool tableGet(Table *table, ObjString *key, Value *value);

/*
 * Adds the given key/value pair, overwriting any values for existing entries.
 * Returns true if a NEW entry was added, otherwise false.
 */
bool tableSet(Table *table, ObjString *key, Value value);

/*
 * Replaces the table entry for the key with a "tombstone" entry, represented
 * with a NULL key and true value. Here the function returns true. Otherwise
 * if the key does not exist in the table, returns false.
 */
bool tableDelete(Table *table, ObjString *key);

/* Copies all table entries from src to dest. */
void tableAddAll(Table *src, Table *dest);

ObjString *tableFindString(Table *table, const char *chars, int length,
                           uint32_t hash);

#endif

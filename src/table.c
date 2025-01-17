#include <stdint.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table *table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void freeTable(Table *table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
  uint32_t index = key->hash % capacity;
  Entry *tombstone = NULL;

  while (true) {
    Entry *entry = &entries[index];

    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        // Returning tombstone bucket allows for reuse.
        return tombstone != NULL ? tombstone : entry;
      }

      if (tombstone == NULL) {
        tombstone = entry; // Found tombstone.
      }
    } else if (entry->key == key) {
      return entry; // Found key.
    }

    index = (index + 1) % capacity; // Linear probing.
  }
}

static void adjustCapacity(Table *table, int capacity) {
  // Initialize every element to be empty bucket
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  // Rebuild the table by reinserting each existing entry. This has to be done
  // as insert locations are dependent on array size, so it can change buckets.
  table->count = 0;
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry == NULL)
      continue;

    Entry *dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = entries;
  table->capacity = capacity;
}

bool tableGet(Table *table, ObjString *key, Value *value) {
  if (table->count == 0)
    return false;

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;

  *value = entry->value;
  return true;
}

bool tableSet(Table *table, ObjString *key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry *entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;

  if (isNewKey && IS_NIL(entry->value))
    table->count++;

  entry->key = key;
  entry->value = value;

  return isNewKey;
}

bool tableDelete(Table *table, ObjString *key) {
  if (table->count == 0)
    return false;

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;

  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}

void tableAddAll(Table *src, Table *dest) {
  for (int i = 0; i < src->capacity; i++) {
    Entry *entry = &src->entries[i];
    if (entry->key != NULL) {
      tableSet(dest, entry->key, entry->value);
    }
  }
}

ObjString *tableFindString(Table *table, const char *chars, int length,
                           uint32_t hash) {
  if (table->count == 0)
    return NULL;

  uint32_t index = hash % table->capacity;

  while (true) {
    Entry *entry = &table->entries[index];

    if (entry->key == NULL) {
      if (IS_NIL(entry->value))
        return NULL; // Stop if we find an empty non-tombstone entry.
    } else if (entry->key->length == length && entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
      return entry->key; // Found.
    }

    index = (index + 1) % table->capacity; // Linear probing.
  }
}

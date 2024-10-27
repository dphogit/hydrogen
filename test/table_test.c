#include "gc.h"
#include "object.h"
#include "table.h"
#include "utest.h"
#include "value.h"

UTEST(Table, crudOperations) {
  GC gc;
  Table strings;
  initGC(&gc);
  initTable(&strings);

  ObjString *foo = copyString(&gc, &strings, "foo", 3);
  Value fooVal;

  Table table;
  initTable(&table);

  // Table initialized correctly.
  ASSERT_EQ(table.capacity, 0);
  ASSERT_EQ(table.count, 0);
  ASSERT_EQ(table.entries, NULL);

  // Retrieving non-existient key.
  EXPECT_FALSE(tableGet(&table, foo, &fooVal));

  // Delete non-existent key fails gracefully.
  EXPECT_FALSE(tableDelete(&table, foo));

  // Insert foo.
  EXPECT_TRUE(tableSet(&table, foo, NUMBER_VAL(6.9)));
  EXPECT_TRUE(tableGet(&table, foo, &fooVal));
  EXPECT_TRUE(IS_NUMBER(fooVal));
  EXPECT_EQ(AS_NUMBER(fooVal), 6.9);

  // Update foo.
  EXPECT_FALSE(tableSet(&table, foo, NUMBER_VAL(4.2))); // Key not new => false.
  EXPECT_TRUE(tableGet(&table, foo, &fooVal));
  EXPECT_TRUE(IS_NUMBER(fooVal));
  EXPECT_EQ(AS_NUMBER(fooVal), 4.2);

  // Delete foo.
  EXPECT_TRUE(tableDelete(&table, foo));
  EXPECT_FALSE(tableGet(&table, foo, &fooVal));

  freeTable(&table);
  freeTable(&strings);
  freeGC(&gc);
}

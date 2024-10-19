CC = gcc
CFLAGS = -Wall -Wextra -Og

TARGET_EXEC := hydro
TEST_EXEC := hydro_tests

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := test

SRCS := $(shell find $(SRC_DIR) -name '*.c')
TESTS := $(shell find $(TESTS_DIR) -name '*_test.c')

OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
TEST_SRCS := $(filter-out $(SRC_DIR)/main.c, $(SRCS)) # Don't compile src main

# Build program executable
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@

# Build step for C source
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build and run tests
.PHONY: test
test: $(BUILD_DIR)/$(TEST_EXEC)
	$(BUILD_DIR)/$(TEST_EXEC) --enable-mixed-units

# Build the tests (shorthand for below rule). Useful to pass in options:
# https://github.com/sheredom/utest.h#command-line-options
.PHONY: test_build
test_build: $(BUILD_DIR)/$(TEST_EXEC)

# Build step for tests.
$(BUILD_DIR)/$(TEST_EXEC): $(TESTS) $(TEST_SRCS)
	mkdir -p $(dir @)
	$(CC) -I$(SRC_DIR) $(CFLAGS) $(TESTS) $(TEST_SRCS) $(TEST_DIR)/main.c -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)


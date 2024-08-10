CC=gcc
CFLAGS=-g -I$(SRC_DIR) -I$(TEST_DIR)

SRC_DIR=src
BUILD_DIR=build
TEST_DIR=test
TEST_BUILD_DIR=$(TEST_DIR)/build
TARGET=server
LIBRARY=libpandio.a

SRCS=$(shell find $(SRC_DIR) -name '*.c')
TEST_SRCS=$(shell find $(TEST_DIR) -name '*.c')

OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TEST_OBJECTS=$(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%.o,$(TEST_SRCS))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

lib: $(OBJECTS)
	ar rcs $(BUILD_DIR)/$(LIBRARY) $(OBJECTS)

test: $(TARGET) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $(TEST_BUILD_DIR)/test -lcriterion
	$(TEST_BUILD_DIR)/test

$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TEST_BUILD_DIR)

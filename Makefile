CC=gcc
CFLAGS=-g

SRC_DIR=src
SRCS=$(wildcard $(SRC_DIR)/*.c)
BUILD_DIR=build
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TEST_DIR=test
TEST_SRCS=$(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS=$(patsubst $(TEST_DIR)/%.c,$(TEST_DIR)/build/%.o,$(TEST_SRCS))

TARGET=server

all: $(TARGET)

$(TARGET): ${OBJECTS}
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $(TEST_DIR)/build/test -lcriterion

$(TEST_DIR)/build/%.o:$(TEST_DIR)/%.c
	@mkdir -p $(TEST_DIR)/build
	$(CC) $(CFLAGS) -c $< -o $@-lcriterion

clean:
	@rm -rf $(BUILD_DIR)

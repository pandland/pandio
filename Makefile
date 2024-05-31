CC=gcc
SRC_DIR=src
SRCS=$(wildcard $(SRC_DIR)/*.c)
BUILD_DIR=build
OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET=server

all: $(TARGET)

$(TARGET): ${OBJECTS}
	$(CC) $(OBJECTS) -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $< -o $@

clean:
	@rm -rf ./build

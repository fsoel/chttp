CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -Iinclude
LIBS = -lws2_32

TARGET_LIB = libchttp.a
EXAMPLE_BIN = test
BUILD_DIR = build
SRC_DIR = src

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

EXAMPLE_SRCS = test.c
EXAMPLE_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(EXAMPLE_SRCS))

all: $(TARGET_LIB) $(EXAMPLE_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ./%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_LIB): $(OBJS)
	ar rcs $@ $^

$(EXAMPLE_BIN): $(TARGET_LIB) $(EXAMPLE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(EXAMPLE_OBJS) -L. -lchttp $(LIBS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET_LIB) $(EXAMPLE_BIN)

.PHONY: all clean

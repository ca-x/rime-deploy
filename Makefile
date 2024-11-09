CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = rime-deploy
TEST_TARGET = run-tests

.PHONY: all clean test install

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

install: $(TARGET)
	install -d $(DESTDIR)/usr/local/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/
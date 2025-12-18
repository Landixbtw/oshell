TARGET_EXEC := oshell
BUILD_DIR := ./build
SRC_DIR := ./src
CFLAGS = -Wall -Wextra -Wpedantic -g -O3 -std=c11

SRCS := $(shell find $(SRC_DIR) -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS := $(INC_FLAGS) -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

	---

.PHONY: showFormat
showFormat:
	scripts/run-clang-format.py -j8 -r --clang-format-executable clang-format --color always .

.PHONY: format
format:
	scripts/run-clang-format.py -j8 -r --clang-format-executable clang-format --color always -i .

-include $(DEPS)

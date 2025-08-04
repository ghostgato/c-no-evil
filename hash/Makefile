.PHONY: debug clean all asan tidy release

CFLAGS += -Wall -Wextra -Wpedantic -Wvla -Winline
STANDARD := -std=c99

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
BIN := driver

TIDY_CHECKS := -*,\
        readability-*,\
        performance-*,\
        bugprone-*,\
        clang-analyzer-*,\
        cert-*,\
        misc-*,\
        modernize-*,\
        -modernize-use-trailing-return-type,\
        -readability-braces-around-statements,\
        -readability-magic-numbers,\
        -cert-err34-c

TIDY_FLAGS := --checks='$(TIDY_CHECKS)' \
        --warnings-as-errors='*' \
        -- $(CFLAGS) -I$(INC_DIR) $(STANDARD)

all: $(BIN)

debug: CFLAGS += -g3 -DDEBUG
debug: $(BIN)

asan: CFLAGS += -g3 -fsanitize=address,undefined -O1 -fno-omit-frame-pointer
asan: LDFLAGS += -fsanitize=address,undefined
asan: $(BIN)
	
clean:
	@rm -rf $(OBJ_DIR) $(BIN)

tidy:
	-clang-tidy $(SRCS) $(TIDY_FLAGS)

release: CFLAGS += -O3 -DNDEBUG
release: $(BIN)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INC_DIR) -MM -MT $(@:.d=.o) $< > $@
.PHONY: debug clean all tidy

BIN := deque

OBJ_DIR := obj

SRC_DIR := src
INC_DIR := include

STANDARD := -std=c99
FEATURES := -D_POSIX_C_SOURCE=200809L
COMPILE_FLAGS := $(STANDARD) $(FEATURES) -I$(INC_DIR)

CFLAGS += -Wall -Wextra -Wpedantic -Werror -Wvla -Wconversion \
-Wfloat-conversion -Wsign-conversion -Wfloat-equal -Waggregate-return

TFLAGS := "cert*, bugprone*, misc*, readability*"

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(BIN)

debug: CFLAGS += -g3 -DNDEBUG
debug: $(BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN)

tidy:
	-clang-tidy -checks=$(TFLAGS) --extra-arg='$(CC)' $(SRCS) -- $(COMPILE_FLAGS)

$(OBJ_DIR):
	@mkdir -p $@

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(COMPILE_FLAGS) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $^ -o $@
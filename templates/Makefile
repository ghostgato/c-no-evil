.PHONY: debug clean all tidy tsan asan test test-run check profile

#-----------------------------------------------------#
# CONFIG VARIABLES
#-----------------------------------------------------#

CC ?= gcc

# Directories
OBJ_DIR := obj
SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
TEST_DIR := test
UNITY_DIR := test/unity

# Binaries and generated files
BIN := maze
TEST_BIN := $(BIN)-test
CLEANUP := gmon.out

# Compiler flags
STANDARD 	  := -std=c99
FEATURES 	  := -D_POSIX_C_SOURCE=200809L
COMPILE_FLAGS := $(STANDARD) $(FEATURES) -I$(INC_DIR) -I$(TEST_DIR) -I$(LIB_DIR)
CFLAGS 		  += -Wall -Wextra -Werror -Wvla -Waggregate-return \
				-Wwrite-strings -Wstack-usage=512

# Linker flags
LDFLAGS := -L$(LIB_DIR)

# clang-tidy flags
TIDY_CHECKS := "cert*, bugprone*, misc*, readability*"

#-----------------------------------------------------#
# CONFIG VARIABLES
#-----------------------------------------------------#
SRCS 	 := $(wildcard $(SRC_DIR)/*.c)
OBJS	 := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
MAIN_OBJ := $(OBJ_DIR)/main.o
NON_MAIN_OBJS := $(filter-out $(MAIN_OBJ), $(OBJS))

TEST_SRCS := $(wildcard $(TEST_DIR)/test_*.c) # files starting with the prefix
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SRCS)) \
						$(OBJ_DIR)/unity.o

#-----------------------------------------------------#
# TARGETS
#-----------------------------------------------------#

# default build
all: $(BIN)

debug: CFLAGS += -g3 -DNDEBUG
debug: $(BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN) $(TEST_BIN) $(CLEANUP)

update: clean all


# UTILITY TARGETS ---------------------#

tidy:
	-clang-tidy -checks=$(TIDY_CHECKS) $(SRCS) -- $(COMPILE_FLAGS)

tsan: CFLAGS += -g3 -fsanitize=thread -fno-omit-frame-pointer
tsan: $(BIN)

asan: CFLAGS += -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
asan: $(BIN)

profile: CFLAGS  += -pg
profile: LDFLAGS += -pg
profile: $(BIN)

# TEST TARGETS-------------------------#

# build test binary
test: $(TEST_BIN)

# build and run test
check: test
	./$(TEST_BIN)

# -------------------------------------------------------------------
# RULES
# -------------------------------------------------------------------

# Ensure object directory exists
$(OBJ_DIR):
	@mkdir -p $@

# Compile src files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(COMPILE_FLAGS) $(CFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(COMPILE_FLAGS) $(CFLAGS) -c $< -o $@

# Compile Unity test files
$(OBJ_DIR)/unity.o: $(UNITY_DIR)/unity.c | $(OBJ_DIR)
	$(CC) $(COMPILE_FLAGS) $(CFLAGS) -c $< -o $@

# Link main binary
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Link test binary
$(TEST_BIN): $(TEST_OBJS) $(NON_MAIN_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
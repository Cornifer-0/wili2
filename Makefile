
#compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2 -Iinclude

#project structure
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := bin
TEST_DIR := tests

#Output binary
TARGET := $(BIN_DIR)/wili2

#Find all .cc files
SRCS := $(wildcard $(SRC_DIR)/*.cc)
OBJS := $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRCS))

#Create necessary directories
$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

#Default rule
all: $(TARGET)

# Link the final binary
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the engine
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Rebuild everything
rebuild: clean all

# Test target (optional)
test:
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/test_board $(TEST_DIR)/test_board.cc $(SRC_DIR)/board.cc $(SRC_DIR)/bitboard.cc
	./$(BIN_DIR)/test_board

.PHONY: all run clean rebuild test




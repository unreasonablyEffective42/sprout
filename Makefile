CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic

SRC_DIR := src
OUT_DIR := out
TARGET := $(OUT_DIR)/main

SRCS := $(SRC_DIR)/cell.cpp $(SRC_DIR)/rational.cpp $(SRC_DIR)/value.cpp $(SRC_DIR)/token.cpp $(SRC_DIR)/complex.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/main.cpp 
OBJS := $(OUT_DIR)/cell.o $(OUT_DIR)/rational.o $(OUT_DIR)/value.o $(OUT_DIR)/token.o $(OUT_DIR)/complex.o $(OUT_DIR)/lexer.o $(OUT_DIR)/main.o

.PHONY: all clean

all: $(TARGET)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(TARGET): $(OUT_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) 

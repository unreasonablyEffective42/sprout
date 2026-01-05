CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic

SRC_DIR := src
OUT_DIR := out
TARGET := $(OUT_DIR)/cell

SRCS := $(SRC_DIR)/cell.cpp $(SRC_DIR)/rational.cpp $(SRC_DIR)/value.cpp
OBJS := $(OUT_DIR)/cell.o $(OUT_DIR)/rational.o $(OUT_DIR)/value.o

.PHONY: all clean

all: $(TARGET)

$(OUT_DIR):
\tmkdir -p $(OUT_DIR)

$(TARGET): $(OUT_DIR) $(OBJS)
\t$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OUT_DIR)
\t$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
\trm -f $(OBJS) $(TARGET)

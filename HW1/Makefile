# Compiler
CXX = g++

# Optimization flags for MICROBENCHMARKING (safe)
CXXFLAGS = -O2 -Wall -fno-tree-vectorize -march=native

# Debug flags (no optimization, for sanity checks)
DEBUGFLAGS = -O0 -g -Wall

# Target
TARGET = problem1
SRC = problem1.cpp

# Default build
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Debug build
debug: $(SRC)
	$(CXX) $(DEBUGFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all debug clean

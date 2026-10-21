CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
LIB      := src/avl.cpp src/redblack.cpp

.PHONY: all test bench demo clean
all: build/amazon_trees build/tests build/compare build/demo

build:
	mkdir -p build

build/amazon_trees: src/main.cpp $(LIB) src/tree.hpp | build
	$(CXX) $(CXXFLAGS) -o $@ src/main.cpp $(LIB)

build/tests: tests/tests.cpp $(LIB) src/tree.hpp | build
	$(CXX) $(CXXFLAGS) -o $@ tests/tests.cpp $(LIB)

build/compare: bench/compare.cpp $(LIB) src/tree.hpp | build
	$(CXX) $(CXXFLAGS) -o $@ bench/compare.cpp $(LIB)

build/demo: bench/demo.cpp $(LIB) src/tree.hpp | build
	$(CXX) $(CXXFLAGS) -o $@ bench/demo.cpp $(LIB)

test: build/tests
	./build/tests

bench: build/compare
	./build/compare

demo: build/demo
	./build/demo

clean:
	rm -rf build

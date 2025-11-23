CXX      := clang++
CXXOPT   := -O3
CXXFLAGS := -std=c++23 -Wall -Wextra -Iinclude 

all:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(CXXOPT) -o bin/main main.cpp

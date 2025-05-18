CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -O2

all: aras

aras: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f aras

.PHONY: all clean

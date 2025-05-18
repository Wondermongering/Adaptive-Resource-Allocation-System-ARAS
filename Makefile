CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -O2

SRCS = $(wildcard *.cpp)

all: aras

aras: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o aras

clean:
	rm -f aras

.PHONY: all clean

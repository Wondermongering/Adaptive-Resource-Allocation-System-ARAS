CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -O2

SRCS = main.cpp Blueprint.cpp Material.cpp Project.cpp ProjectManager.cpp Tool.cpp
OBJS = $(SRCS:.cpp=.o)

all: aras

aras: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) aras

.PHONY: all clean

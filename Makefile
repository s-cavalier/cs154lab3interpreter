CXX = g++
CXXFLAGS = -Werror -Wall -Wextra -Wno-unused-parameter -g

build: main.o Utils.o Hardware.o
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f build *.o

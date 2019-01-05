CXX=g++
CXXFLAGS = -Wall -O3 -std=c++17

server.app:  server.o
		$(CXX) -o $@ server.o $(LINK)

server.o: server.cpp
		$(CXX) -c $< $(CXXFLAGS)

clean: 
		rm -f *.o *.app
################ template makefile ##############
# We don't know what compiler to use to build fltk on this machine - but fltk-config does...
CXX = $(shell fltk-config --cxx)

# Set the flags for compiler: fltk-config knows the basic settings, then we can add our own...
CXXFLAGS = $(shell fltk-config --cxxflags) -Wall -O3 -std=c++17

# We don't know what libraries to link with: fltk-config does...
LINK = $(shell fltk-config --ldflags) -pthread
LINKFLTK = $(shell fltk-config --ldstaticflags)
LINKFLTK_GL = $(shell fltk-config --use-gl --ldstaticflags)
LINKFLTK_IMG = $(shell fltk-config --use-images --ldstaticflags)

# Define what your target application is called
all: main.app

# Define how to build the various object files...

main.o: main.cpp # a C++ file
		$(CXX) -c $< $(CXXFLAGS)

# Now define how to link the final app - let's assume it needs image and OpenGL support
main.app:  main.o
		$(CXX) -o $@ main.o $(LINK)

clean: 
		rm -f *.o *.app
############### end #################

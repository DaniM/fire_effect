CXXFLAGS=-Wall -g -Iinc

main: LDLIBS=-lGLEW -lglfw3 -lX11 -lpthread -lXrandr -lXi -ldl -lGL -lXinerama -lXxf86vm -lXcursor


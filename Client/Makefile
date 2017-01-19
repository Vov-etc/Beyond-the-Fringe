CXX = g++ -Wall -Wextra -Wshadow -Wno-unknown-pragmas -std=gnu++14 -c
LD = g++
DLL_WIN = -lglew32 -lopengl32 -lfreeglut -lws2_32
DLL_UNIX = -lGL -lGLEW -lglut
EXE = client

COMPILE = $(CXX) $< -o $@

all: app
	$(LD) build/*.o $(DLL_UNIX) -o $(EXE)
	
win: app
	$(LD) build/*.o $(DLL_WIN) -o $(EXE).exe

app: build build/main.o build/shader.o build/file.o build/net.o

build/main.o: src/main.cpp src/include/shader.h
	$(COMPILE)
	
build/net.o: src/net.cpp src/include/net.h src/include/net_includes.h
	$(COMPILE)
	
build/file.o: src/file.cpp src/include/file.h
	$(COMPILE)
	
build/shader.o: src/shader.cpp src/include/shader.h
	$(COMPILE)

build: 
	mkdir -p build
clear:
	rm -rf build
	rm -rf $(EXE)

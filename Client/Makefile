CXX = g++ -Wall -Wextra -Wshadow -Wno-unknown-pragmas -std=gnu++14 -c
LD = g++
DLL = -lglew32 -lopengl32 -lfreeglut -lws2_32
EXE = client.exe

COMPILE = $(CXX) $< -o $@

all: app

app: build build/main.o build/shader.o build/file.o build/net.o
	$(LD) build/*.o $(DLL) -o $(EXE)

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

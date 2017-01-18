CXX = g++ -Wall -Wextra -Wshadow -Wno-unknown-pragmas -std=gnu++14 -c
LD = g++
DLL = -lws2_32
EXE = server.exe

COMPILE = $(CXX) $< -o $@

all: app

app: build build/Server.o
	$(LD) build/*.o $(DLL) -o $(EXE)

build/Server.o: src/Server.cpp
	$(COMPILE)

build: 
	mkdir -p build
clear:
	rm -rf build
	rm -rf $(EXE)

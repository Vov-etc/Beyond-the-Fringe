CXX = g++ -Wall -Wextra -Wshadow -Wno-unknown-pragmas -std=gnu++14 -c
LD = g++
DLL_WIN = -lws2_32
DLL_UNIX = -lpthread -lm
EXE = server

COMPILE = $(CXX) $< -o $@

all: app
	$(LD) build/*.o $(DLL_UNIX) -o $(EXE)

win: app
	$(LD) build/*.o $(DLL_WIN) -o $(EXE).exe


app: build build/net.o build/main.o build/buffer.o

build/net.o: src/net.cpp src/include/net.h src/include/net_includes.h src/include/buffer.h
	$(COMPILE)

build/main.o: src/main.cpp src/include/net.h
	$(COMPILE)
	
build/buffer.o: src/buffer.cpp src/include/buffer.h
	$(COMPILE)

build: 
	mkdir -p build
clean:
	rm -rf build
	rm -rf $(EXE).exe

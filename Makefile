CXX=g++
CFLAGS=-std=c++14 -Wall -Wextra

SRC_SERVER=$(shell find ./src/server -name "*.cpp")
SRC_CLIENT=$(shell find ./src/client -name "*.cpp")
OBJ_SERVER=$(subst .cpp,.o, $(SRC_SERVER))
OBJ_CLIENT=$(subst .cpp,.o, $(SRC_CLIENT))

all: server client

server: $(OBJ_SERVER)
	$(CXX) $(CFLAGS) -o server $(OBJ_SERVER)

client: $(OBJ_CLIENT)
	$(CXX) $(CFLAGS) -o client $(OBJ_CLIENT)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

clean: clean-client clean-server
	rm -rf $(shell find . -name "*.o")

clean-client:
	rm -rf $(shell find src/client -name "*.o")
	rm -f ./client

clean-server:
	rm -rf $(shell find src/server -name "*.o")
	rm -f ./server

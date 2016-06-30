CC = g++
FLAGS = -Wall -Wextra -pedantic -std=c++11
LIBS_FLAGS = -lpthread
OUTPUT = server.out

.PHONY: clean

server: server.cpp classes/*.cpp
	$(CC) $(FLAGS) -o $(OUTPUT) server.cpp $(LIBS_FLAGS)

clean:
	rm -f $(OUTPUT)

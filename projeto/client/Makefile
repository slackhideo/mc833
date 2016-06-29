CC = g++
FLAGS = -Wall -Wextra -pedantic
LIBS_FLAGS = -lncurses -lpthread

.PHONY: clean

client-cli: client-cli.cpp
	$(CC) $(FLAGS) -o client-cli client-cli.cpp $(LIBS_FLAGS)

clean:
	rm -f client-cli

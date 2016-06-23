CC = g++
FLAGS = -Wall -Wextra -pedantic
NCURSES_FLAGS = -lncurses

.PHONY: clean

client-cli: client-cli.cpp
	$(CC) $(FLAGS) -o client-cli client-cli.cpp $(NCURSES_FLAGS)

clean:
	rm -f client-cli

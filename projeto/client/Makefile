CC = g++
FLAGS = -Wall -Wextra -pedantic
LIBS_FLAGS = -lncurses -lpthread
OUTPUT = client-cli.out

.PHONY: clean

client-cli: client-cli.cpp
	$(CC) $(FLAGS) -o $(OUTPUT) client-cli.cpp $(LIBS_FLAGS)

clean:
	rm -f $(OUTPUT)

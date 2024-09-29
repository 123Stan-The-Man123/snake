CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRC = src/main.c 

bin/main: $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ -lm -lncurses
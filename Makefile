CFLAGS = -g -Wall
CC = gcc

objects = read_token.o read_token_test.o

read_token: $(objects)
	$(CC) $(CFLAGS) -o read_token $(objects)

read_token_test.o: read_token_test.c
	$(CC) $(CFLAGS) -c read_token_test.c

read_token.o: read_token.c
	$(CC) $(CFLAGS) -c read_token.c

.PHONY: clean
clean:
	rm read_token $(objects)
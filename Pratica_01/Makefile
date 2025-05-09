CC		?= gcc
CFLAGS	?= -std=c99 -Wall -Wextra -Os -march=native

all: client

client: client.c
	$(CC) $(CFLAGS) -o client client.c

server: server.c
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -f client server

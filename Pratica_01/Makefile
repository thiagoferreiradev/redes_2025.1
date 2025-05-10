CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Os -march=native

OBJDIR  := obj

SRCS_CLIENT := client.c utils.c
SRCS_SERVER := server.c utils.c

OBJS_CLIENT := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS_CLIENT))
OBJS_SERVER := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS_SERVER))

all: client server

run-client: client
	@./client

run-server: server
	@./server

client: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o client server

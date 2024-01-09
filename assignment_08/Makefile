# Makefile for compiling threadpool.c and test1.c

CC = gcc
CFLAGS = -Wall -Wextra -g -lpthread
LDFLAGS = -lpthread

# Source files and object files
SRCS = threadpool.c test.c
OBJS = $(SRCS:.c=.o)
EXEC = test

all: $(EXEC) outputs

outputs:
	mkdir -p outputs

run:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./test

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXEC) $(OBJS)
	rm -rf outputs

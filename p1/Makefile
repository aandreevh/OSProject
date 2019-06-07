ifndef CC
	CC=gcc
endif
CFLAGS=-std=c99 -w
SRCS=./src/*.c
HEADERS=./src/headers
OBJS=$(subst .c,.o,$(SRCS))
RM=rm -f

all: main
	$(CC) $(SRCS) $(CFLAGS) -o main -I $(HEADERS)

clean:
	$(RM) $(OBJS) main


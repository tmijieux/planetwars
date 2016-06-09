CFLAGS=-std=gnu99 -Wall -ggdb
LDFLAGS=-lm -lrt -pthread

SRCS=$(wildcard *.c) $(wildcard ../util/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

TARGETS=server

all: $(TARGETS)

server: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(RM) *.o *~ $(OBJS)

mrproper: clean
	$(RM) $(TARGETS)

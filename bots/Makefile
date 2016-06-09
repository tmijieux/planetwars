INSTALL_DIR=../bin

TARGETS=bot
BOTS=$(wildcard bots/*.c)
SRC=$(wildcard *.c) $(BOTS)
OBJS=$(SRC:.c=.o)

CFLAGS=-Wall -std=gnu99 -ggdb
LIBS=-lm

all: $(TARGETS)

.PHONY: javabots all
.PRECIOUS: $(OBJS)

javabots:
	make -C example_bots

bot: $(OBJS)
	$(CC) $(CFLAGS) -I. -o $@ $^ $(LIBS) 

clean:
	$(RM) $(OBJS) *~ *.o
#	make -C example_bots clean

mrproper: clean
	$(RM) $(TARGETS)
	make -C example_bots mrproper

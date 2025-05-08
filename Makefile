CC       = gcc
CFLAGS   = -Wall -Wextra -std=c11 -g
SRCS     = main.c combat.c techniques.c cartes.c
OBJS     = $(SRCS:.c=.o)
DEPS     = structures.h combat.h techniques.h cartes.h

.PHONY: all clean

all: projet

projet: $(OBJS)
	$(CC) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f projet $(OBJS)

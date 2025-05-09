CC       = gcc
CFLAGS   = -Wall -Wextra -std=c11 -g

PROG     = projet
SRCS     = main.c combat.c techniques.c cartes.c jeu.c
OBJS     = $(SRCS:.c=.o)
DEPS     = structures.h combat.h techniques.h cartes.h jeu.h

.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJS)

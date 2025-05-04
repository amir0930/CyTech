# Variables
CC       = gcc
CFLAGS   = -Wall -Wextra -std=c11 -g
LDFLAGS  =

PROG     = projet
SRCS     = main.c combat.c techniques.c cartes.c
OBJS     = $(SRCS:.c=.o)
DEPS     = structures.h combat.h techniques.h cartes.h

.PHONY: all run clean

# Cible par défaut : compiler et lier
all: $(PROG)

# Lien des objets pour produire l'exécutable
$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Règle générique : .c → .o
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Lance le programme
run: all
	./$(PROG)

# Supprime exécutable et fichiers objets
clean:
	rm -f $(PROG) $(OBJS)

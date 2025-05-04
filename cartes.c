// cartes.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cartes.h"

#define MAX_CARTES 100

Deck *init_deck(const char *filename, int *out_size) {
    Carte buffer[MAX_CARTES];
    int n = 0;

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        *out_size = 0;
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), f) && n < MAX_CARTES) {
        Carte c;
        if (sscanf(line, "%49[^;];%99[^;];%d;%d;%19s",
                   c.nom, c.description,
                   &c.effet_valeur, &c.duree, c.type) == 5) {
            buffer[n++] = c;
        }
    }
    fclose(f);

    Deck *d = malloc(sizeof *d);
    d->cards = malloc(sizeof(Carte) * n);
    memcpy(d->cards, buffer, sizeof(Carte) * n);
    d->size = n;
    d->top = 0;

    shuffle_deck(d);
    *out_size = n;
    return d;
}

void shuffle_deck(Deck *d) {
    srand((unsigned)time(NULL));
    for (int i = d->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carte tmp       = d->cards[i];
        d->cards[i]     = d->cards[j];
        d->cards[j]     = tmp;
    }
    d->top = 0;
}

Hand *init_hand(int initial_capacity) {
    Hand *h = malloc(sizeof *h);
    h->cards    = malloc(sizeof(Carte) * initial_capacity);
    h->size     = 0;
    h->capacity = initial_capacity;
    return h;
}

void draw_card(Deck *deck, Hand *hand) {
    if (!deck) return;
    if (deck->top >= deck->size) {
        shuffle_deck(deck);
    }
    Carte c = deck->cards[deck->top++];
    if (hand->size == hand->capacity) {
        hand->capacity *= 2;
        hand->cards = realloc(hand->cards, sizeof(Carte) * hand->capacity);
    }
    hand->cards[hand->size++] = c;
}

void play_card(Hand *hand, int idx, Combattant *cible) {
    if (idx < 0 || idx >= hand->size) return;
    Carte c = hand->cards[idx];

    // Applique l'effet de la carte
    utiliserCarte(&c, cible);

    // Retire la carte de la main
    for (int i = idx + 1; i < hand->size; i++) {
        hand->cards[i-1] = hand->cards[i];
    }
    hand->size--;
}

void afficher_main(const Hand *hand) {
    if (!hand || hand->size == 0) {
        printf("Main vide.\n");
        return;
    }
    printf("Votre main :\n");
    for (int i = 0; i < hand->size; i++) {
        printf("%2d) %-20s [%s] Effet:%+d Durée:%d\n",
               i,
               hand->cards[i].nom,
               hand->cards[i].type,
               hand->cards[i].effet_valeur,
               hand->cards[i].duree);
    }
}

void free_deck(Deck *d) {
    if (!d) return;
    free(d->cards);
    free(d);
}

void free_hand(Hand *h) {
    if (!h) return;
    free(h->cards);
    free(h);
}

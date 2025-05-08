#ifndef CARTES_H
#define CARTES_H

#include "structures.h"

#define MAX_CARTES 100

typedef struct {
    Carte *cartes;
    int size;
} Deck;

typedef struct {
    Carte *cartes;
    int size;
    int capacity;
} Hand;

Deck*    init_deck(const char *filename, int *out_count);
void     free_deck(Deck *d);
Hand*    init_hand(int capacity);
void     free_hand(Hand *h);
void     draw_card(Deck *d, Hand *h);
void     afficher_main(const Hand *h);
void     play_card(Hand *h, int idx, Combattant *cible);

void     utiliserCarte(const Carte *carte, Combattant *cible);
void     chargerCartes(const char *nomFichier, Carte liste[], int *taille);
void     afficherCartesDisponibles(const Carte cartes[], int tailleCartes);

#endif // CARTES_H


#ifndef CARTES_H
#define CARTES_H

#include "structures.h"

// Carte de jeu
typedef struct {
    char nom[50];
    char description[100];
    int effet_valeur;
    int duree;
    char type[20];     // "Offensive", "Buff", "Heal"
} Carte;

// Deck (pioche)
typedef struct {
    Carte *cards;      // tableau dynamique
    int size;          // nombre total de cartes
    int top;           // index de la prochaine carte à piocher
} Deck;

// Main du joueur
typedef struct {
    Carte *cards;      // tableau dynamique
    int size;          // nb de cartes en main
    int capacity;      // capacité du tableau
} Hand;

// Initialisation
Deck *init_deck(const char *filename, int *out_size);
void shuffle_deck(Deck *deck);

// Gestion de la main
Hand *init_hand(int initial_capacity);
void draw_card(Deck *deck, Hand *hand);
void play_card(Hand *hand, int index, Combattant *cible);

// Nettoyage
void free_deck(Deck *deck);
void free_hand(Hand *hand);

// Affichage
void afficher_main(const Hand *hand);

#endif // CARTES_H

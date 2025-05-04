#ifndef CARTES_H
#define CARTES_H

#include "structures.h"

// Représente une carte
typedef struct {
    char nom[50];
    char description[100];
    int effet_valeur;  // dégâts, soin ou buff
    int duree;         // en tours
    char type[20];     // "Offensive", "Defensive", "Buff"
} Carte;

// Deck dynamique de cartes
typedef struct {
    Carte *cartes;
    int size;   // nombre de cartes dans le deck
    int pos;    // index de la prochaine carte à piocher
} Deck;

// Main du joueur / de l'IA
typedef struct {
    Carte *cartes;
    int size;      // nb de cartes en main
    int capacity;  // capacité max
} Hand;

// Prototype pour l'utilisation d'une carte
void utiliserCarte(const Carte *carte, Combattant *cible);

// Chargement depuis fichier
void chargerCartes(const char *filename, Carte liste[], int *taille);

// Gestion du deck
Deck* init_deck(const char *filename, int *nCartes);
void free_deck(Deck *deck);
void shuffle_deck(Deck *deck);

// Gestion de la main
Hand* init_hand(int capacity);
void free_hand(Hand *hand);
void draw_card(Deck *deck, Hand *hand);
void afficher_main(const Hand *hand);
void play_card(Hand *hand, int idx, Combattant *cible);

#endif // CARTES_H

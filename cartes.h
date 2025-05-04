// cartes.h
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
    Carte *cards;      // tableau dynamique de cartes
    int size;          // nombre total de cartes
    int top;           // index de la prochaine carte à piocher
} Deck;

// Main du joueur
typedef struct {
    Carte *cards;      // tableau dynamique de cartes en main
    int size;          // nombre de cartes en main
    int capacity;      // capacité actuelle du tableau
} Hand;

/**
 * Initialise le deck en chargeant toutes les cartes depuis un fichier CSV.
 * @param filename   chemin du fichier cartes.txt
 * @param out_size   retourne le nombre de cartes chargées
 * @return           pointeur vers un Deck alloué dynamiquement
 */
Deck *init_deck(const char *filename, int *out_size);

/**
 * Mélange le deck (Fisher–Yates) et remet top à 0.
 */
void shuffle_deck(Deck *deck);

/**
 * Initialise une main vide avec une capacité initiale donnée.
 */
Hand *init_hand(int initial_capacity);

/**
 * Pioche la carte suivante du deck et l'ajoute à la main.
 */
void draw_card(Deck *deck, Hand *hand);

/**
 * Joue la carte d'indice idx dans la main, en appliquant son effet sur cible.
 */
void play_card(Hand *hand, int idx, Combattant *cible);

/**
 * Affiche les cartes actuellement en main.
 */
void afficher_main(const Hand *hand);

/**
 * Libère la mémoire allouée au deck.
 */
void free_deck(Deck *d);

/**
 * Libère la mémoire allouée à la main.
 */
void free_hand(Hand *h);

#endif // CARTES_H

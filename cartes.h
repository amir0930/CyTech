// cartes.h
#ifndef CARTES_H
#define CARTES_H

#include "structures.h"  // Pour Combattant et Effet

// Carte de jeu
typedef struct {
    char nom[50];
    char description[100];
    int effet_valeur;    // dégâts, soin ou buff
    int duree;           // durée en tours (0 pour instantané)
    char type[20];       // "Offensive", "Buff", "Heal"
} Carte;

// Deck (pioche)
typedef struct {
    Carte *cards;        // tableau dynamique de cartes
    int size;            // nombre total de cartes dans le deck
    int top;             // index de la prochaine carte à piocher
} Deck;

// Main du joueur
typedef struct {
    Carte *cards;        // tableau dynamique de cartes en main
    int size;            // nombre de cartes actuellement en main
    int capacity;        // capacité du tableau (pour realloc)
} Hand;

/**
 * Joue une carte sur une cible en appliquant son effet.
 */
void utiliserCarte(const Carte *carte, Combattant *cible);

/**
 * Initialise le deck en chargeant toutes les cartes depuis un fichier CSV
 * (format : nom;description;effet_valeur;duree;type).
 * @param filename Chemin du fichier à lire
 * @param out_size Retourne le nombre de cartes chargées
 * @return         Pointeur vers un Deck alloué dynamiquement
 */
Deck *init_deck(const char *filename, int *out_size);

/**
 * Mélange le deck (algorithme Fisher–Yates) et remet top à 0.
 */
void shuffle_deck(Deck *deck);

/**
 * Initialise une main vide avec une capacité initiale donnée.
 * @param initial_capacity Taille de départ de la main
 * @return                 Pointeur vers une Hand allouée dynamiquement
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
 * Affiche les cartes actuellement dans la main.
 */
void afficher_main(const Hand *hand);

/**
 * Libère la mémoire allouée au deck.
 */
void free_deck(Deck *deck);

/**
 * Libère la mémoire allouée à la main.
 */
void free_hand(Hand *hand);

#endif // CARTES_H

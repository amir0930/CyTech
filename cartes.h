#ifndef CARTES_H
#define CARTES_H

#include "structures.h"

// Représente une carte de jeu
typedef struct {
    char nom[50];
    char description[100];
    int effet_valeur;  // dégâts, soin ou valeur de buff
    int duree;         // en tours
    char type[20];     // "Offensive", "Defensive", "Buff"
} Carte;

// Deck dynamique de cartes
typedef struct {
    Carte *cartes;
    int size;   // nombre total de cartes
    int pos;    // index de la prochaine carte à piocher
} Deck;

// Main du joueur / de l'IA
typedef struct {
    Carte *cartes;
    int size;      // nombre de cartes en main
    int capacity;  // capacité max
} Hand;

// Charge toutes les cartes d'un fichier (même format que précédemment)
void chargerCartes(const char *filename, Carte liste[], int *taille);

// Initialise et mélange un deck à partir du fichier, renvoie le deck et son nombre de cartes
Deck* init_deck(const char *filename, int *nCartes);
void free_deck(Deck *deck);

// Crée une main vide de capacité donnée
Hand* init_hand(int capacity);
void free_hand(Hand *hand);

// Mélange le deck (Fisher–Yates)
void shuffle_deck(Deck *deck);

// Pioche une carte du deck dans la main
void draw_card(Deck *deck, Hand *hand);

// Affiche la main à l'écran
void afficher_main(const Hand *hand);

// Joue la carte d'indice idx contre le combattant cible,
// puis retire cette carte de la main
void play_card(Hand *hand, int idx, Combattant *cible);

#endif // CARTES_H

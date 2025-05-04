#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cartes.h"

#define MAX_CARTES  100  // taille max pour chargement temporaire

// ------------------------------------------------------------------
// Chargement brut des cartes depuis un fichier texte
// Format d'une ligne :
//   nom;description;effet_valeur;duree;type
// ------------------------------------------------------------------
void chargerCartes(const char *filename, Carte liste[], int *taille) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        *taille = 0;
        return;
    }
    *taille = 0;
    char line[256];
    while (fgets(line, sizeof(line), f) && *taille < MAX_CARTES) {
        Carte *c = &liste[(*taille)];
        // retire le '\n'
        line[strcspn(line, "\r\n")] = '\0';
        sscanf(line, "%49[^;];%99[^;];%d;%d;%19s",
               c->nom,
               c->description,
               &c->effet_valeur,
               &c->duree,
               c->type);
        (*taille)++;
    }
    fclose(f);
}

// ------------------------------------------------------------------
// Mélange Fisher–Yates
// ------------------------------------------------------------------
void shuffle_deck(Deck *deck) {
    for (int i = deck->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carte tmp = deck->cartes[i];
        deck->cartes[i] = deck->cartes[j];
        deck->cartes[j] = tmp;
    }
}

// ------------------------------------------------------------------
// Initialise un deck et le mélange
// ------------------------------------------------------------------
Deck* init_deck(const char *filename, int *nCartes) {
    Carde listeTemp[MAX_CARTES];
    int nTemp;
    chargerCartes(filename, listeTemp, &nTemp);

    Deck *d = malloc(sizeof(Deck));
    d->size = nTemp;
    d->pos  = 0;
    d->cartes = malloc(sizeof(Carte) * nTemp);
    memcpy(d->cartes, listeTemp, sizeof(Carte) * nTemp);

    srand((unsigned)time(NULL));
    shuffle_deck(d);

    *nCartes = nTemp;
    return d;
}

void free_deck(Deck *deck) {
    free(deck->cartes);
    free(deck);
}

// ------------------------------------------------------------------
// Main initialement vide
// ------------------------------------------------------------------
Hand* init_hand(int capacity) {
    Hand *h = malloc(sizeof(Hand));
    h->size     = 0;
    h->capacity = capacity;
    h->cartes   = malloc(sizeof(Carte) * capacity);
    return h;
}

void free_hand(Hand *hand) {
    free(hand->cartes);
    free(hand);
}

// ------------------------------------------------------------------
// Pioche une carte si possible
// ------------------------------------------------------------------
void draw_card(Deck *deck, Hand *hand) {
    if (deck->pos >= deck->size) return;          // plus de cartes
    if (hand->size >= hand->capacity) return;     // main pleine
    hand->cartes[hand->size++] = deck->cartes[deck->pos++];
}

// ------------------------------------------------------------------
// Affiche la main
// ------------------------------------------------------------------
void afficher_main(const Hand *hand) {
    printf("\nVotre main (%d cartes) :\n", hand->size);
    for (int i = 0; i < hand->size; i++) {
        printf("  %2d) %-15s : %s\n",
               i,
               hand->cartes[i].nom,
               hand->cartes[i].description);
    }
}

// ------------------------------------------------------------------
// Joue une carte et retire de la main
// ------------------------------------------------------------------
void play_card(Hand *hand, int idx, Combattant *cible) {
    if (idx < 0 || idx >= hand->size) return;
    Carte c = hand->cartes[idx];
    utiliserCarte(&c, cible);

    // retrait de la main
    for (int j = idx; j < hand->size - 1; j++) {
        hand->cartes[j] = hand->cartes[j+1];
    }
    hand->size--;
}

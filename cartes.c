#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cartes.h"

#define MAX_CARTES 100

// ------------------------------------------------------------------
// Applique l'effet d'une carte sur un combattant cible
// ------------------------------------------------------------------
void utiliserCarte(const Carte *carte, Combattant *cible) {
    if (strcmp(carte->type, "Offensive") == 0) {
        cible->pv_courants -= carte->effet_valeur;
        if (cible->pv_courants < 0) cible->pv_courants = 0;
        printf("%s subit %d degats (PV : %d/%d)\n",
               cible->nom, carte->effet_valeur,
               cible->pv_courants, cible->pv_max);

    } else if (strcmp(carte->type, "Defensive") == 0) {
        cible->pv_courants += carte->effet_valeur;
        if (cible->pv_courants > cible->pv_max)
            cible->pv_courants = cible->pv_max;
        printf("%s recupere %d PV (PV : %d/%d)\n",
               cible->nom, carte->effet_valeur,
               cible->pv_courants, cible->pv_max);

    } else if (strcmp(carte->type, "Buff") == 0) {
        cible->attaque += carte->effet_valeur;
        printf("%s gagne +%d d'attaque pour %d tours\n",
               cible->nom, carte->effet_valeur, carte->duree);

    } else {
        printf("Type de carte inconnu : %s\n", carte->type);
    }
}

// ------------------------------------------------------------------
// Chargement des cartes depuis fichier
// Format : nom;description;valeur;duree;type
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
        // enlève le '\n'
        line[strcspn(line, "\r\n")] = '\0';
        sscanf(line, "%49[^;];%99[^;];%d;%d;%19s",
               liste[*taille].nom,
               liste[*taille].description,
               &liste[*taille].effet_valeur,
               &liste[*taille].duree,
               liste[*taille].type);
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
// Initialise un deck à partir d'un fichier, puis le mélange
// ------------------------------------------------------------------
Deck* init_deck(const char *filename, int *nCartes) {
    Carte listeTemp[MAX_CARTES];
    int nTemp;
    chargerCartes(filename, listeTemp, &nTemp);

    Deck *d = malloc(sizeof(Deck));
    d->size   = nTemp;
    d->pos    = 0;
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
// Initialise une main vide de capacité donnée
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
// Pioche une carte du deck dans la main
// ------------------------------------------------------------------
void draw_card(Deck *deck, Hand *hand) {
    if (deck->pos < deck->size && hand->size < hand->capacity) {
        hand->cartes[hand->size++] = deck->cartes[deck->pos++];
    }
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
// Joue une carte et la retire de la main
// ------------------------------------------------------------------
void play_card(Hand *hand, int idx, Combattant *cible) {
    if (idx < 0 || idx >= hand->size) return;
    Carte c = hand->cartes[idx];
    utiliserCarte(&c, cible);

    // suppression de la carte jouée
    for (int j = idx; j < hand->size - 1; j++) {
        hand->cartes[j] = hand->cartes[j+1];
    }
    hand->size--;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cartes.h"

Deck* init_deck(const char *filename, int *out_count) {
    Carte tmp[MAX_CARTES];
    int n = 0;
    chargerCartes(filename, tmp, &n);
    Deck *d = malloc(sizeof(Deck));
    d->size = n;
    d->cartes = malloc(sizeof(Carte) * n);
    for (int i = 0; i < n; i++) d->cartes[i] = tmp[i];
    srand((unsigned)time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carte c = d->cartes[i];
        d->cartes[i] = d->cartes[j];
        d->cartes[j] = c;
    }
    if (out_count) *out_count = n;
    return d;
}

void free_deck(Deck *d) {
    if (!d) return;
    free(d->cartes);
    free(d);
}

Hand* init_hand(int capacity) {
    Hand *h = malloc(sizeof(Hand));
    h->size = 0;
    h->capacity = capacity;
    h->cartes = malloc(sizeof(Carte) * capacity);
    return h;
}

void free_hand(Hand *h) {
    if (!h) return;
    free(h->cartes);
    free(h);
}

void draw_card(Deck *d, Hand *h) {
    if (!d || !h) return;
    if (d->size <= 0 || h->size >= h->capacity) return;
    h->cartes[h->size++] = d->cartes[--d->size];
}

void afficher_main(const Hand *h) {
    printf("Votre main (%d cartes) :\n", h->size);
    for (int i = 0; i < h->size; i++) {
        const Carte *c = &h->cartes[i];
        printf(" %2d) %-15s : %s (Effet %+d, Duree %d)\n",
               i, c->nom, c->description,
               c->effet_valeur, c->duree);
    }
}

void play_card(Hand *h, int idx, Combattant *cible) {
    if (!h || idx < 0 || idx >= h->size) return;
    utiliserCarte(&h->cartes[idx], cible);
    for (int j = idx; j < h->size - 1; j++)
        h->cartes[j] = h->cartes[j + 1];
    h->size--;
}

void utiliserCarte(const Carte *carte, Combattant *cible) {
    if (cible->pv_courants <= 0) {
        printf("La carte %s ne peut pas etre utilisee car %s est deja KO !\n",
               carte->nom, cible->nom);
        return;
    }
    printf("Activation de la carte %s !\n", carte->nom);
    printf("Effet : %s\n", carte->description);
    if (strcmp(carte->type, "Offensive") == 0) {
        cible->pv_courants -= carte->effet_valeur;
        if (cible->pv_courants < 0) cible->pv_courants = 0;
        printf("%s subit %d degats (PV : %d/%d)\n",
               cible->nom, carte->effet_valeur,
               cible->pv_courants, cible->pv_max);
    } else {
        cible->pv_courants += carte->effet_valeur;
        if (cible->pv_courants > cible->pv_max)
            cible->pv_courants = cible->pv_max;
        printf("%s recupere %d PV (PV : %d/%d)\n",
               cible->nom, carte->effet_valeur,
               cible->pv_courants, cible->pv_max);
    }
}

void chargerCartes(const char *nomFichier, Carte liste[], int *taille) {
    FILE *file = fopen(nomFichier, "r");
    if (!file) {
        printf("Erreur de lecture %s\n", nomFichier);
        *taille = 0;
        return;
    }
    *taille = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) && *taille < MAX_CARTES) {
        if (sscanf(line, "%49[^;];%99[^;];%d;%d;%19s",
                   liste[*taille].nom,
                   liste[*taille].description,
                   &liste[*taille].effet_valeur,
                   &liste[*taille].duree,
                   liste[*taille].type) == 5) {
            (*taille)++;
        }
    }
    fclose(file);
}

void afficherCartesDisponibles(const Carte cartes[], int tailleCartes) {
    if (tailleCartes == 0) {
        printf("Aucune carte disponible.\n");
        return;
    }
    printf("Cartes disponibles :\n");
    for (int i = 0; i < tailleCartes; i++) {
        printf(" %2d) %-15s (Effet %+d, Duree %d)\n",
               i,
               cartes[i].nom,
               cartes[i].effet_valeur,
               cartes[i].duree);
    }
}


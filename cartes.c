// cartes.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cartes.h"    // Déclare utiliserCarte, init_deck, draw_card, play_card, etc.

// ------------------------------------------------------------------
// Joue une carte et applique son effet sur la cible
// ------------------------------------------------------------------
void utiliserCarte(const Carte *carte, Combattant *cible) {
    if (cible->pv_courants <= 0) {
        printf("La carte %s ne peut pas être utilisée car %s est KO !\n",
               carte->nom, cible->nom);
        return;
    }

    printf("Jouer la carte %s : %s\n", carte->nom, carte->description);

    if (strcmp(carte->type, "Offensive") == 0) {
        cible->pv_courants -= carte->effet_valeur;
        if (cible->pv_courants < 0) cible->pv_courants = 0;
        printf("%s subit %d dégâts (PV : %d/%d).\n",
               cible->nom, carte->effet_valeur,
               cible->pv_courants, cible->pv_max);

    } else if (strcmp(carte->type, "Buff") == 0) {
        for (int i = 0; i < MAX_EFFETS; i++) {
            if (cible->effets[i].tours_restants == 0) {
                strncpy(cible->effets[i].nom, carte->nom, sizeof(cible->effets[i].nom)-1);
                cible->effets[i].valeur = carte->effet_valeur;
                cible->effets[i].tours_restants = carte->duree;
                cible->attaque += carte->effet_valeur;
                printf("%s gagne +%d d'attaque pour %d tours.\n",
                       cible->nom, carte->effet_valeur, carte->duree);
                break;
            }
        }

    } else if (strcmp(carte->type, "Heal") == 0) {
        cible->pv_courants += carte->effet_valeur;
        if (cible->pv_courants > cible->pv_max)
            cible->pv_courants = cible->pv_max;
        printf("%s récupère %d PV (PV : %d/%d).\n",
               cible->nom, carte->effet_valeur,
               cible->pv_courants, cible->pv_max);

    } else {
        printf("Type de carte inconnu : %s\n", carte->type);
    }
}

// ------------------------------------------------------------------
// Pioche une carte depuis le deck dans la main
// ------------------------------------------------------------------
void draw_card(Deck *deck, Hand *hand) {
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

// ------------------------------------------------------------------
// Joue la carte d’indice idx dans la main, en l’appliquant sur cible
// ------------------------------------------------------------------
void play_card(Hand *hand, int idx, Combattant *cible) {
    if (idx < 0 || idx >= hand->size) return;
    Carte c = hand->cards[idx];

    // Appliquer l’effet via la fonction déclarée ci-dessus
    utiliserCarte(&c, cible);

    // Retirer la carte de la main (décalage)
    for (int i = idx + 1; i < hand->size; i++) {
        hand->cards[i-1] = hand->cards[i];
    }
    hand->size--;
}

// ------------------------------------------------------------------
// Affiche la main du joueur
// ------------------------------------------------------------------
void afficher_main(const Hand *hand) {
    if (hand->size == 0) {
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

// ------------------------------------------------------------------
// Nettoyage
// ------------------------------------------------------------------
void free_deck(Deck *d) {
    free(d->cards);
    free(d);
}

void free_hand(Hand *h) {
    free(h->cards);
    free(h);
}

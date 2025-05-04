// cartes.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cartes.h"

#define C_RESET    "\033[0m"
#define C_MAGENTA  "\033[1;35m"
#define C_GREEN    "\033[1;32m"
#define C_RED      "\033[1;31m"
#define C_CYAN     "\033[1;36m"

void utiliserCarte(const Carte *carte, Combattant *cible) {
    printf("\n%sJouer la carte %s : %s%s\n",
           C_MAGENTA, carte->nom, carte->description, C_RESET);

    if (strcmp(carte->type, "Offensive") == 0) {
        cible->pv_courants -= carte->effet_valeur;
        if (cible->pv_courants < 0) cible->pv_courants = 0;
        printf("%s%s%s subit %s%d%s degats (PV : %d/%d)\n",
               C_CYAN, cible->nom, C_RESET,
               C_RED, carte->effet_valeur, C_RESET,
               cible->pv_courants, cible->pv_max);

    } else if (strcmp(carte->type, "Buff") == 0) {
        // Exemple de buff d'attaque
        cible->attaque += carte->effet_valeur;
        printf("%s%s%s gagne %s+%d%s d'attaque pour %d tours\n",
               C_CYAN, cible->nom, C_RESET,
               C_GREEN, carte->effet_valeur, C_RESET,
               carte->duree);

    } else if (strcmp(carte->type, "Defensive") == 0) {
        cible->pv_courants += carte->effet_valeur;
        if (cible->pv_courants > cible->pv_max)
            cible->pv_courants = cible->pv_max;
        printf("%s%s%s recupere %s%d%s PV (PV : %d/%d)\n",
               C_CYAN, cible->nom, C_RESET,
               C_GREEN, carte->effet_valeur, C_RESET,
               cible->pv_courants, cible->pv_max);

    } else {
        printf("%sType inconnu : %s%s\n",
               C_RED, carte->type, C_RESET);
    }
}

// ... le reste (init_deck, draw_card, play_card, etc.) reste inchangé

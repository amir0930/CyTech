// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "combat.h"
#include "techniques.h"
#include "cartes.h"

#define MAX_DISPO    10
#define TEAM_SIZE    5
#define CARTES_FILE  "cartes.txt"
#define COMBATS_FILE "combattants.txt"

#define C_RESET    "\033[0m"
#define C_BLUE     "\033[1;34m"
#define C_CYAN     "\033[1;36m"
#define C_GREEN    "\033[1;32m"
#define C_YELLOW   "\033[1;33m"

// Chargement, création d'équipe et affichage identiques (avec un peu plus de sauts de ligne)

void menuPrincipal(Equipe *joueur, Equipe *ia) {
    int choix;
    do {
        printf("\n%s--- MENU PRINCIPAL --- %s\n", C_BLUE, C_RESET);
        printf("  %s1)%s Lancer combat\n", C_GREEN, C_RESET);
        printf("  %s2)%s Voir etat des equipes\n", C_GREEN, C_RESET);
        printf("  %s3)%s Quitter\n", C_GREEN, C_RESET);
        printf("\nVotre choix : ");
        scanf("%d", &choix);
        switch (choix) {
            case 1:
                boucleCombat(joueur, ia);
                break;
            case 2:
                afficherEtatEquipes(joueur, ia);
                break;
            case 3:
                printf("\n%sAu revoir !%s\n", C_YELLOW, C_RESET);
                break;
            default:
                printf("\n%sChoix invalide.%s\n", C_RED, C_RESET);
        }
    } while (choix != 3);
}

int main(void) {
    srand((unsigned)time(NULL));

    Combattant dispo[MAX_DISPO];
    int nbDispo;
    chargerCombattants(COMBATS_FILE, dispo, &nbDispo);

    Equipe joueur = { .nom = "Joueur" };
    Equipe ia     = { .nom = "IA"     };
    creerEquipe(&joueur, dispo, &nbDispo);
    creerEquipe(&ia,     dispo, &nbDispo);

    menuPrincipal(&joueur, &ia);

    free(joueur.combattants);
    free(ia.combattants);
    return 0;
}

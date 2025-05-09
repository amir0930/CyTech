#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "structures.h"
#include "combat.h"
#include "techniques.h"
#include "cartes.h"
#include "jeu.h"

int main(void) {
    srand((unsigned)time(NULL));

    Combattant dispo[MAX_DISPO];
    int nbDispo;
    chargerCombattants(COMBATS_FILE, dispo, &nbDispo);

    Equipe equipe1 = { .nom = "Equipe 1", .combattants = NULL, .taille = 0 };
    Equipe equipe2 = { .nom = "Equipe 2", .combattants = NULL, .taille = 0 };

    // Création des deux équipes
    creerEquipe(&equipe1, dispo, &nbDispo);
    printf("\n");
    creerEquipe(&equipe2, dispo, &nbDispo);

    // Lancement du menu (IA vs Local vs état)
    menuPrincipal(&equipe1, &equipe2);

    free(equipe1.combattants);
    free(equipe2.combattants);
    return 0;
}


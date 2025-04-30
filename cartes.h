#ifndef CARTES_H
#define CARTES_H
#include "structures.h"

void utiliserCarte(Carte *carte, Combattant *cible) {
    printf("Activation de la carte : %s\n", carte->nom);
    printf("Effet : %s\n", carte->description);

//changer de cartes
void chargerCartes(char *nomFichier, Carte liste[], int *taille) {
    FILE *file = fopen(nomFichier, "r");
    if (file == NULL) {
        printf("Erreur de lecture du fichier %s\n", nomFichier);
        return;
    }

    *taille = 0;
    while (fscanf(file, "%[^;];%[^;];%d;%d;%s\n", liste[*taille].nom, liste[*taille].description,
                  &liste[*taille].effet_valeur, &liste[*taille].duree, liste[*taille].type) == 5) {
        (*taille)++;
    }

    fclose(file);
}

//utiliser les cartes
    printf("3 - Jouer une carte\n");

case 3: {
    printf("Choisir une carte (0 à %d) : ", tailleCartes - 1);
    int choixCarte;
    scanf("%d", &choixCarte);

    if (choixCarte >= 0 && choixCarte < tailleCartes) {
        utiliserCarte(&cartes[choixCarte], &ordreCombat[i]);
    } else {
        printf("Choix de carte invalide.\n");
    }
}
break;

void afficherCartesDisponibles(Carte cartes[], int tailleCartes) {
    printf("Cartes disponibles :\n");
    for (int i = 0; i < tailleCartes; i++) {
        printf("%d - %s : %s (Effet : %d, Durée : %d tours)\n", 
               i, cartes[i].nom, cartes[i].description, cartes[i].effet_valeur, cartes[i].duree);
    }
}

#endif

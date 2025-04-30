#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "cartes.h"

void utiliserCarte(Carte carte, Combattant *cible) {
    if (cible->pv_courants <= 0) {
        printf("La carte %s ne peut pas être utilisée car %s est déjà KO !\n", carte.nom, cible->nom);
        return;
    }

    printf("Activation de la carte %s !\n", carte.nom);
    printf("Effet : %s\n", carte.description);

    // Application de l'effet
    cible->attaque += carte.effet_valeur;
    printf("%s reçoit %d points d'attaque grâce à %s (Durée : %d tours).\n",
           cible->nom, carte.effet_valeur, carte.nom, carte.duree);
}

void chargerCartes(char *nomFichier, Carte liste[], int *taille) {
    FILE *file = fopen(nomFichier, "r");
    if (file == NULL) {
        printf("Erreur : Impossible de lire le fichier %s\n", nomFichier);
        return;
    }

    *taille = 0;
    while (fscanf(file, "%[^;];%[^;];%d;%d;%s\n",
                  liste[*taille].nom,
                  liste[*taille].description,
                  &liste[*taille].effet_valeur,
                  &liste[*taille].duree,
                  liste[*taille].type) == 5) {
        (*taille)++;
        if (*taille >= MAX_CARTES) {
            printf("Attention : Limite maximale de cartes atteinte !\n");
            break;
        }
    }
    fclose(file);
}

void afficherCartesDisponibles(Carte cartes[], int tailleCartes) {
    if (tailleCartes == 0) {
        printf("Aucune carte disponible.\n");
        return;
    }

    printf("Cartes disponibles :\n");
    for (int i = 0; i < tailleCartes; i++) {
        printf("%d : %s (Effet : %d, Durée : %d tours)\n", 
               i, cartes[i].nom, cartes[i].effet_valeur, cartes[i].duree);
    }
}

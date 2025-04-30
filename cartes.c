#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "cartes.h"

void utiliserCarte(Carte carte, Combattant *cible) {
    printf("Activation de la carte %s !\n", carte.nom);
    printf("Effet : %s\n", carte.description);

    // Application de l'effet
    cible->attaque += carte.effet_valeur;  // Exemple : boost d'attaque
    printf("%s reçoit %d points d'attaque grâce à %s (Durée : %d tours).\n", 
           cible->nom, carte.effet_valeur, carte.nom, carte.duree);
}


//changer de cartes
void chargerCartes(char *nomFichier, Carte liste[], int *taille) {
    FILE *file = fopen(nomFichier, "r");
    if (file == NULL) {
        printf("Erreur de lecture du fichier %s\n", nomFichier);
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
    }
    fclose(file);
}

void afficherCartesDisponibles(Carte cartes[], int tailleCartes) {
    printf("Cartes disponibles :\n");
    for (int i = 0; i < tailleCartes; i++) {
        printf("%d : %s (Effet : %d, Durée : %d tours)\n", 
               i, cartes[i].nom, cartes[i].effet_valeur, cartes[i].duree);
    }
}


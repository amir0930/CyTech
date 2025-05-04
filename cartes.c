// cartes.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cartes.h"

/**
 * Joue une carte sur une cible : inflige dégâts, buff ou soin selon le type.
 */
void utiliserCarte(const Carte *carte, Combattant *cible) {
    if (cible->pv_courants <= 0) {
        printf("La carte %s ne peut pas être utilisée car %s est KO !\n",
               carte->nom, cible->nom);
        return;
    }

    printf("Jouer la carte %s : %s\n", carte->nom, carte->description);

    if (strcmp(carte->type, "Offensive") == 0) {
        // Dégâts directs
        cible->pv_courants -= carte->effet_valeur;
        if (cible->pv_courants < 0) cible->pv_courants = 0;
        printf("%s subit %d dégâts (PV : %d/%d).\n",
               cible->nom, carte->effet_valeur,
               cible->pv_courants, cible->pv_max);

    } else if (strcmp(carte->type, "Buff") == 0) {
        // Bonus d'attaque temporaire
        for (int i = 0; i < MAX_EFFETS; i++) {
            if (cible->effets[i].tours_restants == 0) {
                strncpy(cible->effets[i].nom, carte->nom, 50);
                cible->effets[i].valeur = carte->effet_valeur;
                cible->effets[i].tours_restants = carte->duree;
                cible->attaque += carte->effet_valeur;
                printf("%s gagne +%d d'attaque pour %d tours.\n",
                       cible->nom, carte->effet_valeur, carte->duree);
                break;
            }
        }

    } else if (strcmp(carte->type, "Heal") == 0) {
        // Soins
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

/**
 * Charge un fichier CSV de cartes, format :
 * nom;description;effet_valeur;duree;type
 */
void chargerCartes(const char *nomFichier, Carte liste[], int *taille) {
    FILE *f = fopen(nomFichier, "r");
    if (!f) {
        perror(nomFichier);
        *taille = 0;
        return;
    }

    char line[256];
    *taille = 0;
    while (fgets(line, sizeof(line), f)) {
        Carte c;
        if (sscanf(line, "%49[^;];%99[^;];%d;%d;%19s",
                   c.nom, c.description,
                   &c.effet_valeur, &c.duree,
                   c.type) == 5)
        {
            liste[(*taille)++] = c;
            if (*taille >= MAX_CARTES) {
                printf("Limite de %d cartes atteinte, arrêt du chargement.\n", MAX_CARTES);
                break;
            }
        }
    }
    fclose(f);
}

/**
 * Affiche la liste des cartes disponibles avec leurs caractéristiques.
 */
void afficherCartesDisponibles(const Carte cartes[], int taille) {
    if (taille == 0) {
        printf("Aucune carte disponible.\n");
        return;
    }
    printf("Cartes disponibles :\n");
    for (int i = 0; i < taille; i++) {
        printf("%2d: %-20s (Type: %-9s Effet: %+d Durée: %d tours)\n",
               i,
               cartes[i].nom,
               cartes[i].type,
               cartes[i].effet_valeur,
               cartes[i].duree);
    }
}

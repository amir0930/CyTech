// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "structures.h"
#include "combat.h"
#include "techniques.h"
#include "cartes.h"

#define MAX_DISPO   10
#define TEAM_SIZE   5
#define CARTES_FILE "cartes.txt"
#define COMBATS_FILE "combattants.txt"

// ------------------------------------------------------------------
// Chargement des combattants
// ------------------------------------------------------------------
void chargerCombattants(const char *filename, Combattant liste[], int *taille) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror(filename); *taille = 0; return; }

    *taille = 0;
    while (*taille < MAX_DISPO) {
        Combattant c;
        if (fscanf(f, "%49s %d %d %d %d %d",
                   c.nom,
                   &c.pv_max,
                   &c.attaque,
                   &c.defense,
                   &c.agilite,
                   &c.vitesse) != 6)
            break;
        c.pv_courants   = c.pv_max;
        c.nb_techniques = 0;
        c.techniques    = NULL;
        for (int i = 0; i < MAX_EFFETS; i++)
            c.effets[i].tours_restants = 0;
        liste[(*taille)++] = c;
    }
    fclose(f);
}

// ------------------------------------------------------------------
// Création d’une équipe par sélection
// ------------------------------------------------------------------
void creerEquipe(Equipe *equipe, Combattant dispo[], int nbDispo) {
    printf("=== Création de l'équipe %s ===\n", equipe->nom);
    equipe->taille     = 0;
    equipe->combattants = malloc(sizeof(Combattant) * TEAM_SIZE);

    while (equipe->taille < TEAM_SIZE) {
        printf("\nCombattants dispos :\n");
        for (int i = 0; i < nbDispo; i++) {
            printf("%2d) %-10s PV:%3d ATK:%2d DEF:%2d VIT:%2d\n",
                   i,
                   dispo[i].nom,
                   dispo[i].pv_max,
                   dispo[i].attaque,
                   dispo[i].defense,
                   dispo[i].vitesse);
        }
        printf("Votre choix (0-%d) : ", nbDispo-1);
        int c; if (scanf("%d", &c)!=1) { getchar(); continue; }
        if (c>=0 && c<nbDispo) {
            equipe->combattants[equipe->taille++] = dispo[c];
            printf("  -> %s ajouté\n", dispo[c].nom);
        } else {
            printf("  Choix invalide\n");
        }
    }
}

// ------------------------------------------------------------------
// Affichage de l'état de deux équipes
// ------------------------------------------------------------------
void afficherEtatEquipes(Equipe *e1, Equipe *e2) {
    printf("\n--- ÉTAT DES ÉQUIPES ---\n");
    printf("%s :\n", e1->

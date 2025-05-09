#ifndef JEU_H
#define JEU_H

#include "structures.h"

#define MAX_DISPO    10
#define TEAM_SIZE    5
#define CARTES_FILE  "cartes.txt"
#define COMBATS_FILE "combattants.txt"

/* Charge les combattants depuis COMBATS_FILE */
void chargerCombattants(const char *filename, Combattant liste[], int *taille);

/* Crée une équipe à partir d’un tableau dispo[] sans doublons */
void creerEquipe(Equipe *equipe, Combattant dispo[], int *nbDispo);

/* Affiche les barres de vie + effets de deux équipes */
void afficherEtatEquipes(Equipe *e1, Equipe *e2);

/* Boucle combat contre l’IA (mode solo) */
void boucleCombat(Equipe *joueur, Equipe *ia);

/* Boucle combat local (2 joueurs en alternance) */
void boucleCombatLocal(Equipe *p1, Equipe *p2);

/* Menu principal pour choisir mode de jeu ou afficher l’état */
void menuPrincipal(Equipe *joueur, Equipe *ia);

#endif // JEU_H


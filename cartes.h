#ifndef CARTES_H
#define CARTES_H

#include "structures.h"  // Pour accéder aux structs Combattant et Carte

void utiliserCarte(Carte carte, Combattant *cible);
void chargerCartes(char *nomFichier, Carte liste[], int *taille);
void afficherCartesDisponibles(Carte cartes[], int tailleCartes);

#endif
